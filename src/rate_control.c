/***************************************************************************
** intoPIX SA & Fraunhofer IIS (hereinafter the "Software Copyright       **
** Holder") hold or have the right to license copyright with respect to   **
** the accompanying software (hereinafter the "Software").                **
**                                                                        **
** Copyright License for Evaluation and Testing                           **
** --------------------------------------------                           **
**                                                                        **
** The Software Copyright Holder hereby grants, to any implementer of     **
** this ISO Standard, an irrevocable, non-exclusive, worldwide,           **
** royalty-free, sub-licensable copyright licence to prepare derivative   **
** works of (including translations, adaptations, alterations), the       **
** Software and reproduce, display, distribute and execute the Software   **
** and derivative works thereof, for the following limited purposes: (i)  **
** to evaluate the Software and any derivative works thereof for          **
** inclusion in its implementation of this ISO Standard, and (ii)         **
** to determine whether its implementation conforms with this ISO         **
** Standard.                                                              **
**                                                                        **
** The Software Copyright Holder represents and warrants that, to the     **
** best of its knowledge, it has the necessary copyright rights to        **
** license the Software pursuant to the terms and conditions set forth in **
** this option.                                                           **
**                                                                        **
** No patent licence is granted, nor is a patent licensing commitment     **
** made, by implication, estoppel or otherwise.                           **
**                                                                        **
** Disclaimer: Other than as expressly provided herein, (1) the Software  **
** is provided “AS IS” WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING  **
** BUT NOT LIMITED TO, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A   **
** PARTICULAR PURPOSE AND NON-INFRINGMENT OF INTELLECTUAL PROPERTY RIGHTS **
** and (2) neither the Software Copyright Holder (or its affiliates) nor  **
** the ISO shall be held liable in any event for any damages whatsoever   **
** (including, without limitation, damages for loss of profits, business  **
** interruption, loss of information, or any other pecuniary loss)        **
** arising out of or related to the use of or inability to use the        **
** Software.”                                                             **
**                                                                        **
** RAND Copyright Licensing Commitment                                    **
** -----------------------------------                                    **
**                                                                        **
** IN THE EVENT YOU WISH TO INCLUDE THE SOFTWARE IN A CONFORMING          **
** IMPLEMENTATION OF THIS ISO STANDARD, PLEASE BE FURTHER ADVISED THAT:   **
**                                                                        **
** The Software Copyright Holder agrees to grant a copyright              **
** license on reasonable and non- discriminatory terms and conditions for **
** the purpose of including the Software in a conforming implementation   **
** of the ISO Standard. Negotiations with regard to the license are       **
** left to the parties concerned and are performed outside the ISO.       **
**                                                                        **
** No patent licence is granted, nor is a patent licensing commitment     **
** made, by implication, estoppel or otherwise.                           **
***************************************************************************/


#include "rate_control.h"
#include "precinct.h"
#include "slice.h"
#include "gcli_budget.h"
#include "data_budget.h"
#include "buf_mgmt.h"
#include "gcli_methods.h"
#include "budget.h"
#include <assert.h>
#include "sb_weighting.h"
#include "budget_dump.h"
#include "precinct_budget_table.h"
#include "xs_markers.h"

struct rate_control_t
{
	tco_conf_t conf;
	int image_height;
	precinct_t* precinct;
	precinct_budget_table_t* pbt;
	predbuffer_t* pred_residuals;
	precinct_t* precinct_top;
	uint64_t gc_enabled_modes;
	 
	int nibbles_image;
	int nibbles_report;
	int nibbles_consumed;
	int lines_consumed;
	 
	ra_params_t ra_params;
	int *gtli_table_data;
	int *gtli_table_gcli;
	int *gtli_table_gcli_prec;
	int gcli_methods_table;
	int *gcli_sb_methods;

	 
	precinct_budget_info_t* pbinfo;

	int scenario2gtli_table[MAX_GCLI + 1][MAX_LVLS];
	struct budget_dump_struct_t *budget_dump_handle;
};


rate_control_t* rate_control_open( tco_conf_t* conf, lvls_map_t* map, int image_height, const char* budget_dump_filename, int column)
{
	rate_control_t* rc = (rate_control_t*) malloc(sizeof(rate_control_t));
	if (rc)
	{
		rc->conf = *conf;

		rc->precinct = precinct_open_column(map, conf->group_size, column);
		rc->pbt = pbt_open(map->position_count, GCLI_METHODS_NB);
		rc->pred_residuals = predbuffer_open(rc->precinct);

		assert(rc->precinct);
		assert(rc->pbt);

		rc->precinct_top = precinct_open_column(map, conf->group_size, column);
		assert(rc->precinct_top);
		
		rc->gtli_table_data = (int*) malloc(map->level_count * sizeof(int));
		rc->gtli_table_gcli_prec = (int*) malloc(map->level_count * sizeof(int));
		rc->gtli_table_gcli = (int*) malloc(map->level_count * sizeof(int));
		rc->gcli_sb_methods = (int*) malloc(MAX_LVLS * sizeof(int));
		rc->pbinfo = (precinct_budget_info_t*) malloc(sizeof(precinct_budget_info_t));
		
		rc->ra_params.gc_trunc_use_priority = conf->gc_trunc_use_priority; 
		rc->ra_params.use_sign_packing = (conf->sign_opt == SIGN_PACKING);
		rc->ra_params.all_enabled_methods = gcli_method_get_enabled(conf);
		rc->ra_params.pkt_hdr_size_short = conf->pkt_hdr_size_short;
		rc->ra_params.lvl_gains = conf->lvl_gains;
		rc->ra_params.lvl_priorities = conf->lvl_priorities;

		rc->image_height = image_height;
		rc->gc_enabled_modes = 0;
		rc->gc_enabled_modes = gcli_method_get_enabled(conf);
	
		 
		compute_gtli_tables_all_scenarios(
			map->level_count,
			conf->lvl_gains,
			conf->gc_trunc,
			rc->scenario2gtli_table);

		 
		if (budget_dump_filename != NULL)
		{
			rc->budget_dump_handle = budget_dump_init(
				rc->precinct,
				budget_dump_filename, 
				map->level_count, 
				rc->scenario2gtli_table,
				conf->lvl_priorities);

			if (!rc->budget_dump_handle) {
				fprintf(stderr, "Could not create handle to dump budget\n");
			}
		}
		else
		{
			rc->budget_dump_handle = NULL;
		}
	}
	return rc;
}

void rate_control_init(rate_control_t* rc, int image_rate_bytes, int report_nbytes)
{
	if (rc)
	{
		rc->nibbles_consumed = 0;
		rc->lines_consumed = 0;
		rc->nibbles_image = image_rate_bytes * 2;
		rc->nibbles_report = report_nbytes * 2;
	}
}


void rate_control_close( rate_control_t* rc )
{
	budget_dump_free(rc->budget_dump_handle);
	free(rc->gcli_sb_methods);
	free(rc->gtli_table_gcli);
	free(rc->gtli_table_gcli_prec);
	free(rc->gtli_table_data);
	free(rc->pbinfo);
	precinct_close(rc->precinct_top);
	pbt_close(rc->pbt);
	predbuffer_close(rc->pred_residuals);
	precinct_close(rc->precinct);

	free(rc);
}

static int rate_control_refresh_line0(rate_control_t* rc, precinct_t* precinct)
{
	fill_gcli_budget_table(
		gcli_method_get_enabled_ver(rc->gc_enabled_modes),
		precinct,
		rc->precinct_top,
		rc->gtli_table_gcli_prec,
		rc->pbt,
		rc->conf.gc_trunc,
		rc->conf.gc_nonsig_group_size,
		rc->conf.gc_top_pred_mode,
		rc->pred_residuals,
		1,
		rc->scenario2gtli_table,
		rc->conf.run_mode,
		rc->conf.sigflags_group_width,
		rc->conf.bounded_alphabet);
	return 0;
}

 
int rate_control_process_presinct(rate_control_t* rc, precinct_t* precinct, rc_results_t* rc_results)
{
	int lvl,scenario, refinement;
	int padding_nibbles;
	int budget_cbr;
	int budget_minimum;
	int precinct_nibbles;
	int precinct_bits;
	uint64_t gcli_enabled_modes;
	precinct_t* precinct_top;

	copy_gclis(rc->precinct_top, rc->precinct);

	precinct_copy(rc->precinct, precinct);

	gcli_enabled_modes = rc->gc_enabled_modes;

	if (precinct_is_first_of_slice(precinct, rc->conf.slice_height))
		precinct_top = NULL;
	else
		precinct_top = rc->precinct_top;

	fill_gcli_budget_table(
			gcli_enabled_modes,
			precinct,
			precinct_top,
			NULL,
			rc->pbt,
			rc->conf.gc_trunc,
			rc->conf.gc_nonsig_group_size,
			rc->conf.gc_top_pred_mode,
			rc->pred_residuals,
			0,
			rc->scenario2gtli_table,
			rc->conf.run_mode,
			rc->conf.sigflags_group_width,
			rc->conf.bounded_alphabet);


	fill_data_budget_table(precinct,
						   rc->pbt,
						   rc->conf.group_size,
						   rc->conf.sign_opt == SIGN_PACKING,
						   rc->conf.dq_type);

	if (!precinct_is_first_of_slice(rc->precinct, rc->conf.slice_height))
	{
		rate_control_refresh_line0(rc, rc->precinct);
	}

	 
	precinct_nibbles = budget_getcbr(rc->nibbles_image, (rc->lines_consumed + precinct_spacial_lines_of(rc->precinct, rc->image_height)), rc->image_height);
	precinct_nibbles -= rc->nibbles_consumed;
	 
	budget_cbr = budget_getcbr(rc->nibbles_image, rc->lines_consumed + precinct_spacial_lines_of(rc->precinct, rc->image_height), rc->image_height);
	budget_minimum = (budget_cbr - rc->nibbles_report);

	precinct_nibbles = MAX(budget_minimum - rc->nibbles_consumed, precinct_nibbles);

	 
	 
	if (!precinct_is_first_of_slice(rc->precinct, rc->conf.slice_height))
	{
		rate_control_refresh_line0(rc, rc->precinct);
	}

	rc->ra_params.budget = (precinct_nibbles << 2);
	rc->ra_params.gc_trunc = rc->conf.gc_trunc;
	rc->ra_params.use_priorities = 1;
	rc->ra_params.precinct_line = precinct_idx_of(rc->precinct);

	precinct_bits = do_rate_allocation( rc->precinct,
										&rc->ra_params,
										rc->pbt,
										&scenario,
										&refinement,
										rc->gtli_table_data,
										rc->gtli_table_gcli_prec,
										rc->gcli_sb_methods,
										rc->pbinfo,
										rc->budget_dump_handle);
	if (precinct_bits < 0) {
		fprintf(stderr, "Unable to make the rate allocation on the precinct\n");
		rc_results->rc_error = 1;
		return -1;
	}

	precinct_nibbles = (precinct_bits >> 2);

	rc->lines_consumed += precinct_spacial_lines_of(rc->precinct, rc->image_height);
	rc->nibbles_consumed += precinct_nibbles;

	 
	padding_nibbles = 0;
	if (precinct_is_last_of_image(rc->precinct, rc->image_height))
		padding_nibbles = rc->nibbles_image - rc->nibbles_consumed;  
	else if (rc->nibbles_consumed < budget_minimum)
		padding_nibbles = (budget_minimum - rc->nibbles_consumed);
	assert(padding_nibbles >= 0);

	rc->nibbles_consumed += padding_nibbles;

	rc_results->scenario = scenario;
	rc_results->refinement = refinement;
	rc_results->gcli_method = rc->gcli_methods_table;
	for (lvl = 0; lvl < bands_count_of(rc->precinct); lvl++)
		rc_results->gcli_sb_methods[lvl] = rc->gcli_sb_methods[lvl];
	rc_results->pred_residuals = rc->pred_residuals;
	rc_results->bits_consumed = (rc->nibbles_consumed << 2);
	rc_results->gtli_table_data = rc->gtli_table_data;
	rc_results->gtli_table_gcli = rc->gtli_table_gcli_prec;
	
	rc_results->pbinfo = rc->pbinfo[0];
	rc_results->padding_bits = (padding_nibbles << 2);
	rc_results->precinct_total_bits = rc_results->pbinfo.precinct_bits + rc_results->padding_bits;

	rc_results->rc_error = 0;
	return 0;
}

void rate_control_compute_column_rate(int ncols,
									  float col_frac,
									  int image_rate_bytes, 
									  int image_report_bytes, 
									  int* nbytes_column,
									  int* report_column,
									  int* nbytes_last_column,
									  int* report_last_column)
{
	*nbytes_column = ((double) image_rate_bytes + 0.5) * col_frac;
	*report_column = ((double) image_report_bytes + 0.5) * col_frac;
	*nbytes_last_column = image_rate_bytes - (ncols-1) * *nbytes_column;
	*report_last_column = image_report_bytes - (ncols-1) * *report_column;
	return;
}

