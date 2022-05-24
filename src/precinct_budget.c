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

#include "gcli_methods.h"
#include "packing.h"
#include "precinct_budget.h"
#include "precinct_budget_table.h"
#include <assert.h>

#define ALIGN_TO_BITS(value, nbits) ((value + nbits - 1) & (~(nbits - 1)))

int precinct_get_best_gcli_method(precinct_t *precinct, 
									precinct_budget_table_t* pbt,
									int* gtli_table_gcli,
									int* res_gcli_sb_methods)
{
	int lvl, gcli_method, y_pos, position, gtli;
	uint32_t bgt_min;
	uint32_t bgt_method, bgt_min_method;
	uint32_t line_bgt = 0;

	 
	for (lvl = 0; lvl < bands_count_of(precinct); lvl++)
	{
		bgt_min = RA_BUDGET_INVALID;
		bgt_min_method = -1;

		for (gcli_method = 0; gcli_method < GCLI_METHODS_NB; gcli_method++)
		{
			gtli = gtli_table_gcli[lvl];
			 
			if (gcli_method == method_get_idx(ALPHABET_RAW_4BITS,PRED_NONE,RUN_NONE))
				continue;
			 
			if (pbt_get_gcli_bgt_of(pbt, gcli_method, 0)[gtli] == RA_BUDGET_INVALID)
				continue;

			bgt_method = 0;
			for (y_pos = 0; y_pos < precinct_height_of(precinct, lvl); y_pos++)
			{
				position = precinct_position_of(precinct, lvl, y_pos);
				 
				line_bgt = pbt_get_sigf_bgt_of(pbt, gcli_method, position)[gtli];
				line_bgt+= pbt_get_gcli_bgt_of(pbt, gcli_method, position)[gtli];
				bgt_method += line_bgt;
			}
			if ((bgt_min == RA_BUDGET_INVALID) || (bgt_method < bgt_min))
			{
				bgt_min = bgt_method;
				bgt_min_method = gcli_method;
			}
		}
		if (bgt_min == RA_BUDGET_INVALID)
		{
			 
			bgt_min_method = method_get_idx(ALPHABET_RAW_4BITS,PRED_NONE,RUN_NONE);
		}
		res_gcli_sb_methods[lvl] = bgt_min_method;
	}
	return 0;
}

 
int detect_gcli_raw_fallback(precinct_t* precinct,
							 precinct_budget_info_t* pbi)
{
	int band, position, ypos, subpkt = 0;
	for (band = 0; band < bands_count_of(precinct); band++)
	{
		int size_noraw_sum = 0;
		int size_raw_sum = 0;
		for (ypos = 0; ypos < precinct_height_of(precinct, band); ypos++)
		{
			position = precinct_position_of(precinct, band, ypos);
			subpkt = precinct_subpkt_of(precinct, position);
			if (pbi->subpkt_uses_raw_fallback[subpkt] != -1)
				break;  
			size_noraw_sum += pbi->subpkt_size_gcli[subpkt];
			size_noraw_sum += pbi->subpkt_size_sigf[subpkt];
			size_raw_sum   += pbi->subpkt_size_gcli_raw[subpkt];
		}
		
		if (pbi->subpkt_uses_raw_fallback[subpkt] == -1)
		{
			 
			 
			if (size_raw_sum < size_noraw_sum)
			{
				 
				for (ypos = 0; ypos < precinct_height_of(precinct, band); ypos++)
				{
					position = precinct_position_of(precinct, band, ypos);
					subpkt = precinct_subpkt_of(precinct, position);
					pbi->subpkt_size_sigf[subpkt] = 0;
					pbi->subpkt_size_gcli[subpkt] = pbi->subpkt_size_gcli_raw[subpkt];
					pbi->subpkt_uses_raw_fallback[subpkt] = 1;					
				}
			}
			else
			{
				 
				for (ypos = 0; ypos < precinct_height_of(precinct, band); ypos++)
				{
					position = precinct_position_of(precinct, band, ypos);
					subpkt = precinct_subpkt_of(precinct, position);
					pbi->subpkt_uses_raw_fallback[subpkt] = 0;
				}
			}
		}
	}
	return 0;
}

int init_budget_info(precinct_budget_info_t *pbi, int n_subpkts)
{
	int subpkt;
	 
	for (subpkt = 0; subpkt < n_subpkts; subpkt++)
	{
		pbi->subpkt_size_sigf[subpkt] = 0;
		pbi->subpkt_size_gcli[subpkt] = 0;
		pbi->subpkt_size_gcli_raw[subpkt] = 0;
		pbi->subpkt_size_data[subpkt] = 0;
		pbi->subpkt_size_sign[subpkt] = 0;
		pbi->subpkt_uses_raw_fallback[subpkt] = -1;
	}
	return 0;
}


 
int precinct_get_budget(	precinct_t* precinct,
								precinct_budget_table_t* pbt,
								int* gtli_table_gcli,
								int* gtli_table_data,
								int use_sign_packing, 
								int* res_gcli_sb_methods,
								int gcli_signaling_nbits,
								int pkt_hdr_size_short,
								precinct_budget_info_t *out)
{
	int lvl, position, gtli_gcli, gtli_data;
	int gcli_method;
	int subpkt = 0;
	int raw_budget;
	int raw_method = method_get_idx(ALPHABET_RAW_4BITS,PRED_NONE,RUN_NONE);

	init_budget_info(out, precinct_nb_subpkts_of(precinct));

	 
	for (position = 0; position < line_count_of(precinct); position++)
	{
		lvl = precinct_lvl_of(precinct, position);
		gtli_gcli = gtli_table_gcli[lvl];
		gtli_data = gtli_table_data[lvl];
		gcli_method = res_gcli_sb_methods[lvl];
		subpkt = precinct_subpkt_of(precinct, position);

		out->subpkt_size_sigf[subpkt] += pbt_get_sigf_bgt_of(pbt, gcli_method, position)[gtli_gcli];
		out->subpkt_size_gcli[subpkt] += pbt_get_gcli_bgt_of(pbt, gcli_method, position)[gtli_gcli];
		out->subpkt_size_data[subpkt] += pbt_get_data_bgt_of(pbt, position)[gtli_data];
		out->subpkt_size_sign[subpkt] += pbt_get_sign_bgt_of(pbt, position)[gtli_data];

		raw_budget = pbt_get_gcli_bgt_of(pbt, raw_method, position)[gtli_gcli];		
		if (raw_budget == RA_BUDGET_INVALID)
			out->subpkt_size_gcli_raw[subpkt] = RA_BUDGET_INVALID;
		else if (out->subpkt_size_gcli_raw[subpkt] != RA_BUDGET_INVALID)
			out->subpkt_size_gcli_raw[subpkt]+=raw_budget;
	}

	 
	for (subpkt = 0; subpkt < precinct_nb_subpkts_of(precinct); subpkt++)
	{
		out->subpkt_alignbits_sigf[subpkt] = ALIGN_TO_BITS(out->subpkt_size_sigf[subpkt], 8) - out->subpkt_size_sigf[subpkt];
		out->subpkt_alignbits_gcli[subpkt] = ALIGN_TO_BITS(out->subpkt_size_gcli[subpkt], 8) - out->subpkt_size_gcli[subpkt];
		out->subpkt_alignbits_data[subpkt] = ALIGN_TO_BITS(out->subpkt_size_data[subpkt], 8) - out->subpkt_size_data[subpkt];
		out->subpkt_alignbits_sign[subpkt] = ALIGN_TO_BITS(out->subpkt_size_sign[subpkt], 8) - out->subpkt_size_sign[subpkt];
		out->subpkt_alignbits_gcli_raw[subpkt] = ALIGN_TO_BITS(out->subpkt_size_gcli_raw[subpkt], 8) - out->subpkt_size_gcli_raw[subpkt];

		out->subpkt_size_sigf[subpkt] += out->subpkt_alignbits_sigf[subpkt];
		out->subpkt_size_gcli[subpkt] += out->subpkt_alignbits_gcli[subpkt];
		out->subpkt_size_data[subpkt] += out->subpkt_alignbits_data[subpkt];
		out->subpkt_size_sign[subpkt] += out->subpkt_alignbits_sign[subpkt];
		out->subpkt_size_gcli_raw[subpkt] += out->subpkt_alignbits_gcli_raw[subpkt];
	}

	 
	detect_gcli_raw_fallback(precinct, out);

	 
	out->prec_header_size = ALIGN_TO_BITS(PREC_HDR_PREC_SIZE + PREC_HDR_SCENARIO_SIZE + PREC_HDR_REFINEMENT_SIZE + bands_count_of(precinct)*gcli_signaling_nbits, PREC_HDR_ALIGNMENT);
	out->precinct_bits = out->prec_header_size;
	for (subpkt = 0; subpkt < precinct_nb_subpkts_of(precinct); subpkt++)
	{
		 
		if (pkt_hdr_size_short)
		{
			out->pkt_header_size[subpkt] = PKT_HDR_DATA_SIZE_SHORT + PKT_HDR_GCLI_SIZE_SHORT;
			out->pkt_header_size[subpkt]+=PKT_HDR_SIGN_SIZE_SHORT;
		}
		else
		{
			out->pkt_header_size[subpkt] = PKT_HDR_DATA_SIZE_LONG + PKT_HDR_GCLI_SIZE_LONG;
			out->pkt_header_size[subpkt]+=PKT_HDR_SIGN_SIZE_LONG;
		}
		out->pkt_header_size[subpkt] = ALIGN_TO_BITS(out->pkt_header_size[subpkt], PKT_HDR_ALIGNMENT);
		out->precinct_bits += out->pkt_header_size[subpkt];

		 
		out->precinct_bits += out->subpkt_size_sigf[subpkt];
		out->precinct_bits += out->subpkt_size_gcli[subpkt];
		out->precinct_bits += out->subpkt_size_data[subpkt];
		out->precinct_bits += out->subpkt_size_sign[subpkt];
	}

	#ifdef DEBUG_PREC_BUDGET
		fprintf(stderr, "Precinct Budget: precinct_bits=%8d (prec_hdr=%d):", out->precinct_bits, out->prec_header_size);
		for (subpkt = 0; subpkt < precinct_nb_subpkts_of(precinct); subpkt++)
			fprintf(stderr, " pkt%d (hdr=%d gcli=%d sigf=%d data=%d sign=%d)", subpkt, out->pkt_header_size[subpkt], out->subpkt_size_gcli[subpkt], out->subpkt_size_sigf[subpkt], out->subpkt_size_data[subpkt], out->subpkt_size_sign[subpkt]);
		fprintf(stderr, "\n");
	#endif

	return out->precinct_bits;
}


#ifdef TEST_RAW_FALLBACK

 

int main(int argc, char*argv[])
{
	image_t im;
	tco_conf_t conf;
	lvls_map_t geometry;
	precinct_t *p;
	precinct_budget_info_t pbi;
	int n_subpkts;

	im.ncomps = 2;
	im.w[0] = im.w[1] = im.w[2] = 1920;
	im.h[0] = im.h[1] = im.h[2] = 1080;
	conf.ndecomp_v = 2;
	conf.ndecomp_h = 5;
	conf.ndecomp_vh = 1;
	conf.col_sz = 1920;
	conf.order = ORDER_RPBC;

	fill_lvls_map(&conf, &im, &geometry);
	p = precinct_open(&geometry,4);

	n_subpkts = precinct_nb_subpkts_of(p);

	init_budget_info(&pbi, n_subpkts);

	 

	 
	pbi.subpkt_size_sigf[0] = 50;
	pbi.subpkt_size_gcli[0] = 50;
	pbi.subpkt_size_gcli_raw[0] = 80;

	pbi.subpkt_size_sigf[1] = 0;
	pbi.subpkt_size_gcli[1] = 70;
	pbi.subpkt_size_gcli_raw[1] = 80;

	pbi.subpkt_size_sigf[2] = 0;
	pbi.subpkt_size_gcli[2] = 90;
	pbi.subpkt_size_gcli_raw[2] = 80;

	pbi.subpkt_size_sigf[3] = 90;
	pbi.subpkt_size_gcli[3] = 0;
	pbi.subpkt_size_gcli_raw[3] = 80;

	 
	
	 
	pbi.subpkt_size_sigf[4] = 50;
	pbi.subpkt_size_gcli[4] = 50;
	pbi.subpkt_size_gcli_raw[4] = 80;
	pbi.subpkt_size_sigf[7] = 50;
	pbi.subpkt_size_gcli[7] = 50;
	pbi.subpkt_size_gcli_raw[7] = 80;

	 
	pbi.subpkt_size_sigf[5] = 50;
	pbi.subpkt_size_gcli[5] = 50;
	pbi.subpkt_size_gcli_raw[5] = 80;
	pbi.subpkt_size_sigf[8] = 0;
	pbi.subpkt_size_gcli[8] = 50;
	pbi.subpkt_size_gcli_raw[8] = 80;

	 
	pbi.subpkt_size_sigf[6] = 50;
	pbi.subpkt_size_gcli[6] = 70;
	pbi.subpkt_size_gcli_raw[6] = 80;
	pbi.subpkt_size_sigf[9] = 0;
	pbi.subpkt_size_gcli[9] = 50;
	pbi.subpkt_size_gcli_raw[9] = 80;

	detect_gcli_raw_fallback(p, &pbi);

	assert(pbi.subpkt_uses_raw_fallback[0] == 1);
	assert(pbi.subpkt_uses_raw_fallback[1] == 0);
	assert(pbi.subpkt_uses_raw_fallback[2] == 1);
	assert(pbi.subpkt_uses_raw_fallback[3] == 1);
	
	assert(pbi.subpkt_uses_raw_fallback[4] == 1);
	assert(pbi.subpkt_uses_raw_fallback[5] == 0);
	assert(pbi.subpkt_uses_raw_fallback[6] == 1);
	assert(pbi.subpkt_uses_raw_fallback[7] == 1);
	assert(pbi.subpkt_uses_raw_fallback[8] == 0);
	assert(pbi.subpkt_uses_raw_fallback[9] == 1);
	
}


#endif


