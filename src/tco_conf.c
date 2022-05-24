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


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include "tco_conf.h"
#include "common.h"
#include "gcli.h"  
#include "default_weights.h"
#include "lvls_map.h"

 

int tco_conf_validate(tco_conf_t* conf_in, image_t* image, tco_conf_t* conf)
{
	int budget_report_nbytes;
	int precinct_bgt_nbytes;
	int coded_depth;
	int budget_report_precincts;
	tco_conf_t dummy;

	if (!conf)
		conf = &dummy;

	 
	*conf = *conf_in;

	if (conf->bitstream_nbytes == -1) 
	{
		fprintf(stderr, "Error: encoding rate is unknown!\n");
		return -1;
	}

	conf->bpp = (double) (conf->bitstream_nbytes * 8) / (image->w[0]*image->h[0]);

	precinct_bgt_nbytes = 2 * conf->bitstream_nbytes / image->h[0];
	
	 
	budget_report_precincts = conf->budget_report_lines / 2;
	 
	budget_report_nbytes = budget_report_precincts * precinct_bgt_nbytes;

	if (conf->budget_report_nbytes == -1)
		conf->budget_report_nbytes = budget_report_nbytes;

	 
	if (conf->in_depth == 0)
	{
		 
		conf->in_depth = image->depth;
	}

	coded_depth = conf->in_depth - conf->quant;
	if (coded_depth > TCO_MAX_DEPTH)
	{
		if (conf->quant == 0)
		{
			conf->quant = coded_depth - TCO_MAX_DEPTH;
			fprintf(stderr, "Note: quantification applied (%d)\n", conf->quant);
		}
		else
		{
			fprintf(stderr, "Error: image depth too large, or not compatible with quantification\n");
			return -1;
		}
	}

	/*if ((image->ncomps != 3) && (conf->profile != TCO_PROFILE_0))
	{
		fprintf(stderr, "Error: Monochrome image not supported with this tco profile! Use profile0\n");
		return -1;
	}*/
	 
	if (conf->pkt_hdr_size_auto)
	{
		if (conf->order == ORDER_RPBC)
			conf->pkt_hdr_size_short = (((image->w[0] * image->ncomps) < 32768) && (conf->ndecomp_v < 3));
		else
			conf->pkt_hdr_size_short = 0;
	}

	 
	if ((conf->gains_choice == GAINS_CHOICE_PSNR) ||
		(conf->gains_choice == GAINS_CHOICE_VISUAL))
	{
		int lvl;
		int nlvls = map_get_NL(image->ncomps, conf->ndecomp_h, conf->ndecomp_v);
		int nlvls_per_comp = map_get_N_filtertypes(conf->ndecomp_h, conf->ndecomp_v);
		int visual = conf->gains_choice == GAINS_CHOICE_VISUAL;
		int rct = ((image->is_422 || conf->rct==0)) ? GAINS_NOCT : GAINS_RCT;
		struct default_weights_t* def = get_default_weights(conf->kernel_v, conf->kernel_h, conf->ndecomp_v, conf->ndecomp_h, visual, rct);
		if (def)
		{
			for (lvl = 0; lvl < nlvls; lvl++)
			{
				int comp = lvl / nlvls_per_comp;
				int filtertype = lvl % nlvls_per_comp;
				conf->lvl_gains[lvl] = def->gains[comp][filtertype];
				conf->lvl_priorities[lvl] = def->priorities[comp][filtertype];
			}
		}
		else
		{
			fprintf(stderr, "Error: No default weigths defined for this configuration\n");
			return -1;
		}
	}
	
	if ((conf->col_max_width > 0) || (conf->col_cnt != 1) || ((conf->col_sz!=0) && (conf->col_sz != image->w[0])))
	{
		if (!conf->profile == TCO_PROFILE_0)
		{
			 
			int col_granularity = 8 * (1 << conf->ndecomp_h);
			if (image->is_422)
				col_granularity *= 2;
			if (conf->col_granularity != col_granularity)
			{
				fprintf(stderr, "Note: forcing column granularity to %d\n", col_granularity);
				conf->col_granularity = col_granularity;
			}
		}
	}
	if (conf->col_max_width > 0)
	{
		 
		conf->col_sz = MIN
			(
				conf->col_max_width,
				((image->w[0] + (conf->col_cnt * conf->col_granularity - 1))
				/
				(conf->col_cnt * conf->col_granularity)) * conf->col_granularity
			);
		fprintf(stderr, "Info: col_sz set via col_max_width to %d\n", conf->col_sz);
	}
	if ((conf->col_sz > image->w[0]) || (conf->col_sz == 0))
	{
		conf->col_sz = image->w[0];
		fprintf(stderr, "Info: col_sz set to width of luma channel in pixels %d\n", conf->col_sz);
	}

	if (conf->col_sz < 0)
	{
		fprintf(stderr, "Error: col_sz (%d) must be >= 0\n", conf->col_sz);
		return -1;
	}
	return 0;
}


int tco_conf_print(tco_conf_t* conf)
{
	fprintf(stderr, "TicoOptions\n");
	fprintf(stderr, "bitstream_nbytes=%d\n", conf->bitstream_nbytes);
	fprintf(stderr, "bpp=%f\n", conf->bpp);
	fprintf(stderr, "profile=%d\n", conf->profile);
	fprintf(stderr, "group_size=%d\n", conf->group_size);
	fprintf(stderr, "kernel_h=%d\n", conf->kernel_h) ;
	fprintf(stderr, "ndecomp_h=%d\n", conf->ndecomp_h);
	fprintf(stderr, "kernel_v=%d\n", conf->kernel_v) ;
	fprintf(stderr, "ndecomp_v=%d\n", conf->ndecomp_v);
	fprintf(stderr, "ndecomp_vh=%d\n", conf->ndecomp_vh);
	fprintf(stderr, "rct=%d\n", conf->rct);
	fprintf(stderr, "rct_type=%d\n", conf->rct_type);
	fprintf(stderr, "cbr=%d\n", conf->cbr);
	fprintf(stderr, "haar_use_shift=%d\n", conf->haar_use_shift);
	fprintf(stderr, "col_sz=%d\n", conf->col_sz);
	fprintf(stderr, "cols_ra_opt=%d\n", conf->cols_ra_opt);
	fprintf(stderr, "gc_raw=%d\n", conf->gc_raw);
	fprintf(stderr, "gc_bounded=%d\n", conf->gc_bounded);
	fprintf(stderr, "gc_pred_per_sb=%d\n", conf->gc_pred_per_sb);
	fprintf(stderr, "gc_run_per_sb=%d\n", conf->gc_run_per_sb);
	fprintf(stderr, "gc_ver=%d\n", conf->gc_ver);
	fprintf(stderr, "gc_nopred=%d\n", conf->gc_nopred);
	fprintf(stderr, "gc_run_none=%d\n", conf->gc_run_none);
	fprintf(stderr, "gc_run_sigflags_zrf=%d\n", conf->gc_run_sigflags_zrf);
	fprintf(stderr, "gc_run_sigflags_zrcsf=%d\n", conf->gc_run_sigflags_zrcsf);
	fprintf(stderr, "gc_trunc=%d\n", conf->gc_trunc);
	fprintf(stderr, "gc_trunc_use_priority=%d\n", conf->gc_trunc_use_priority);
	fprintf(stderr, "slice_height=%d\n", conf->slice_height);
	fprintf(stderr, "order=%d\n", conf->order);
	fprintf(stderr, "budget_report_nbytes=%d\n", conf->budget_report_nbytes);
	fprintf(stderr, "budget_report_lines=%f\n", conf->budget_report_lines);
	fprintf(stderr, "dq_type=%d\n", conf->dq_type);
	fprintf(stderr, "data_coding=%d\n", conf->data_coding);
	fprintf(stderr, "sp_lenhdr=%d\n", conf->sp_lenhdr);
	fprintf(stderr, "sign_lenhdr=%d\n", conf->sign_lenhdr);
	fprintf(stderr, "prec_lenhdr=%d\n", conf->prec_lenhdr);
	fprintf(stderr, "encoder_id=%d\n", conf->encoder_id);
	fprintf(stderr, "use_slice_footer=%d\n", conf->use_slice_footer);
	fprintf(stderr, "in_depth=%d\n", conf->in_depth);
	fprintf(stderr, "quant=%d\n", conf->quant);
	fprintf(stderr, "v_slice_mirroring=%d\n", conf->v_slice_mirroring);
	fprintf(stderr, "sign_opt=%d\n", conf->sign_opt);
	fprintf(stderr, "gc_nonsig_group_size=%d\n", conf->gc_nonsig_group_size);
	fprintf(stderr, "run_mode=%d\n", conf->run_mode);
	fprintf(stderr, "gc_top_pred_mode=%d\n", conf->gc_top_pred_mode);
	fprintf(stderr, "sigflags_group_width=%d\n", conf->sigflags_group_width);
	fprintf(stderr, "bounded_alphabet=%d\n", conf->bounded_alphabet);
	fprintf(stderr, "verbose=%d\n", conf->verbose);
 
	return 0;
}
