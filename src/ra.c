/***************************************************************************
** intoPIX SA, Fraunhofer IIS and Canon Inc. (hereinafter the "Software   **
** Copyright Holder") hold or have the right to license copyright with    **
** respect to the accompanying software (hereinafter the "Software").     **
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

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ra.h"
#include "packing.h"
#include "precinct.h"
#include "precinct_budget.h"
#include "common.h"
#include "sb_weighting.h"
#include "gcli_methods.h"

 
int ra_helper_get_total_budget(
					precinct_t* precinct,
					ra_params_t* ra_params, 
					precinct_budget_table_t* pbt,
					int* gtli_table_data, 
					int* gtli_table_gcli,
					int* gcli_sb_methods,
					precinct_budget_info_t* bgt_info)
{
	precinct_get_best_gcli_method(precinct, pbt, gtli_table_gcli, gcli_sb_methods);
	precinct_get_budget( precinct,
						 pbt,
						 gtli_table_gcli,
						 gtli_table_data,
						 ra_params->use_sign_packing,
						 gcli_sb_methods,
						 gcli_method_get_signaling_nbits(ra_params->all_enabled_methods),
						 ra_params->pkt_hdr_size_short,
						 bgt_info);

	return bgt_info->precinct_bits;
}

enum
{
	RA_FINDING_SCENARIO,
	RA_FINDING_REFINEMENT,
	RA_DONE,
	RA_ERROR,
};

int do_rate_allocation( precinct_t* precinct,
						ra_params_t* ra_params,
						precinct_budget_table_t* pbt,
						int* scenario_out,
						int* refinement_out,
						int* gtli_table_data,
						int* gtli_table_gcli,
						int* gcli_sb_methods,
						precinct_budget_info_t* bgt_info,
						struct budget_dump_struct_t* budget_dump_handle)
{
	int scenario, refinement;
	int state=RA_FINDING_SCENARIO;
	int total_budget = 0;
	uint8_t* lvl_gains = ra_params->lvl_gains;
	uint8_t* lvl_priorities = ra_params->lvl_priorities;
	int max_refinement = (ra_params->use_priorities) ? get_max_uint8_arr(lvl_priorities, bands_count_of(precinct)) : 0;
	int empty;
	
	if (ra_params->budget < 0)
	{
		fprintf(stderr, "Error: rate allocation budget must be > 0! (%d)\n", ra_params->budget);
		return -1;
	}

	 
	 
	scenario=0;
	refinement=0;
		
	while (state != RA_DONE)
	{	

		 
		compute_gtli_tables(scenario, 
							refinement, 
							bands_count_of(precinct), 
							lvl_gains, 
							lvl_priorities, 
							ra_params->gc_trunc,
							ra_params->gc_trunc_use_priority,
							gtli_table_data,
							gtli_table_gcli,
							&empty);


		total_budget = ra_helper_get_total_budget(  precinct,
													ra_params, 
													pbt,
													gtli_table_data, 
													gtli_table_gcli, 
													gcli_sb_methods,
													bgt_info);

		if (state == RA_FINDING_SCENARIO)
		{
			if (total_budget <= ra_params->budget)
			{
				if (((scenario) > 0) && (max_refinement!=0))
				{
					state = RA_FINDING_REFINEMENT;
					(refinement)++;
				}
				else
					state = RA_DONE;
			}
			else if (!empty)
			{
				(scenario)++;
			}
			else
			{
				 
				fprintf(stderr, "RATE Error: unable to reach precinct budget=%d (min_possible=%d)\n",ra_params->budget,total_budget);
				state = RA_ERROR;
				break;
			}
		}
		else  
		{
			if (total_budget > ra_params->budget)
			{
				(refinement)--;  
				 
				compute_gtli_tables(scenario, 
									refinement, 
									bands_count_of(precinct), 
									lvl_gains, 
									lvl_priorities, 
									ra_params->gc_trunc,
									ra_params->gc_trunc_use_priority,
									gtli_table_data,
									gtli_table_gcli,
									&empty);

				total_budget = ra_helper_get_total_budget(
					precinct,
					ra_params, 
					pbt,
					gtli_table_data, 
					gtli_table_gcli, 
					gcli_sb_methods,
					bgt_info);

				state = RA_DONE;
			}
			else if (refinement==max_refinement)
			{
				state = RA_DONE;
			}
			else
			{
				(refinement)++;
			}
		}
	}
	if (state==RA_ERROR)
		return -1;

	 
	#ifdef RA_VERBOSE
	{
		fprintf(stderr, "R.A %d,%d: budget_max=%d total=%d budgets=", scenario,refinement, ra_params->budget, total_budget);
		fprintf(stderr, "%d (method_lvl0=%d),", line_budget[0], gcli_sb_methods[0]);
		fprintf(stderr, "\n");	
	}
	#endif

	*scenario_out = scenario;
	*refinement_out = refinement;
	return total_budget;
}

