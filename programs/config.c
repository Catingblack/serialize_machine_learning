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
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

#include <tco.h>
#include "ini.h"

#define MATCH(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)

int config_line_handler(void *user, const char *section, const char *name, const char *value)
{
	tco_conf_t *pconfig = (tco_conf_t *)user;
	if (MATCH("main", "bpp")) {
		pconfig->bpp = atof(value);
		return 1;
	} else if (MATCH("main", "bitstream_nbytes")) {
		pconfig->bitstream_nbytes = atoi(value);
		return 1;
	} else if (MATCH("main", "profile")) {
		if (!strcmp(value, "PROFILE_0")) {
			pconfig->profile = TCO_PROFILE_0;
			return 1;
		} else if (!strcmp(value, "PROFILE_JPEGXS_MAIN")) {
			pconfig->profile = TCO_PROFILE_JPEGXS_MAIN;
			return 1;
		} else if (!strcmp(value, "PROFILE_JPEGXS_HIGH")) {
			pconfig->profile = TCO_PROFILE_JPEGXS_HIGH;
			return 1;
		} else if (!strcmp(value, "PROFILE_JPEGXS_LIGHT_SUBLINE")) {
			pconfig->profile = TCO_PROFILE_JPEGXS_LIGHT_SUBLINE;
			return 1;
		} else if (!strcmp(value, "PROFILE_JPEGXS_LIGHT")) {
			pconfig->profile = TCO_PROFILE_JPEGXS_LIGHT;
			return 1;
		} else {
			fprintf(stderr, "config: unknown profile %s\n", value);
			return 0;
		}
	} else if (MATCH("main", "group_size")) {
		pconfig->group_size = atoi(value);
		return 1;
	} else if (MATCH("main", "ndecomp_h")) {
		pconfig->ndecomp_h = atoi(value);
		return 1;
	} else if (MATCH("main", "hor_kernel")) {
		pconfig->kernel_h = atoi(value);
		return 1;
	} else if (MATCH("main", "ndecomp_vh")) {
		pconfig->ndecomp_vh = atoi(value);
		return 1;
	} else if (MATCH("main", "ndecomp_v")) {
		pconfig->ndecomp_v = atoi(value);
		return 1;
	} else if (MATCH("main", "ver_kernel")) {
		pconfig->kernel_v = atoi(value);
		return 1;
	} else if (MATCH("main", "v_slice_mirroring")) {
		pconfig->v_slice_mirroring = atoi(value);
		return 1;
	} else if (MATCH("main", "rct")) {
		pconfig->rct = atoi(value);
		return 1;
	} else if (MATCH("main", "rct_type")) {
		pconfig->rct_type = atoi(value);
		return 1;
	} else if (MATCH("main", "cbr")) {
		pconfig->cbr = atoi(value);
		return 1;
	} else if (MATCH("main", "haar_use_shift")) {
		pconfig->haar_use_shift = atoi(value);
		return 1;
	} else if (MATCH("main", "col_sz")) {
		pconfig->col_sz = atoi(value);
		return 1;
	} else if (MATCH("main", "col_max_width")) {
		pconfig->col_max_width = atoi(value);
		return 1;
	} else if (MATCH("main", "col_cnt")) {
		pconfig->col_cnt = atoi(value);
		return 1;
	} else if (MATCH("main", "col_granularity")) {
		pconfig->col_granularity = atoi(value);
		return 1;
	} else if (MATCH("main", "cols_ra_opt")) {
		pconfig->cols_ra_opt = atoi(value);
		return 1;
	} else if (MATCH("gcli", "raw")) {
		pconfig->gc_raw = atoi(value);
		return 1;
	} else if (MATCH("gcli", "bounded")) {
		pconfig->gc_bounded = atoi(value);
		return 1;
	} else if (MATCH("gcli", "pred_per_sb")) {
		pconfig->gc_pred_per_sb = atoi(value);
		return 1;
	} else if (MATCH("gcli", "run_per_sb")) {
		pconfig->gc_run_per_sb = atoi(value);
		return 1;
	} else if (MATCH("gcli", "ver")) {
		pconfig->gc_ver = atoi(value);
		return 1;
	} else if (MATCH("gcli", "nopred")) {
		pconfig->gc_nopred = atoi(value);
		return 1;
	} else if (MATCH("gcli", "run_none")) {
		pconfig->gc_run_none = atoi(value);
		return 1;
	} else if (MATCH("gcli", "run_sigflags_zrf")) {
		pconfig->gc_run_sigflags_zrf = atoi(value);
		return 1;
	} else if (MATCH("gcli", "run_sigflags_zrcsf")) {
		pconfig->gc_run_sigflags_zrcsf = atoi(value);
		return 1;
	} else if (MATCH("gcli", "sigflags_group_width")) {
		pconfig->sigflags_group_width = atoi(value);
		return 1;
	} else if (MATCH("gcli", "codesig_with_0")) {
		pconfig->sigflags_group_width = atoi(value);
		return 1;
	} else if (MATCH("gcli", "bounded_alphabet")) {
		pconfig->bounded_alphabet = atoi(value);
		return 1;
	} else if (MATCH("gcli", "nonsig_group_size")) {
		pconfig->gc_nonsig_group_size = atoi(value);
		return 1;
	} else if (MATCH("gcli", "top_pred_mode")) {
		pconfig->gc_top_pred_mode = atoi(value);
		return 1;
	} else if (MATCH("gcli", "trunc")) {
		pconfig->gc_trunc = atoi(value);
		return 1;
	} else if (MATCH("gcli", "gc_trunc_use_priority")) {
		pconfig->gc_trunc_use_priority = atoi(value);
		return 1;
	} else if (MATCH("main", "gc_trunc_use_priority")) {
		pconfig->gc_trunc_use_priority = atoi(value);
		return 1;
	} else if (MATCH("main", "slice_height")) {
		pconfig->slice_height = atoi(value);
		return 1;
	} else if (MATCH("main", "use_slice_footer")) {
		pconfig->use_slice_footer = atoi(value);
		return 1;
	} else if (MATCH("main", "order")) {
		if (!strcmp(value, "GCVH") || !strcmp(value, "ORDER_GCVH")) {
			pconfig->order = ORDER_GCVH;
			return 1;
		} else if (!strcmp(value, "VGCH") || !strcmp(value, "ORDER_VGCH")) {
			pconfig->order = ORDER_VGCH;
			return 1;
		} else if (!strcmp(value, "CVHG") || !strcmp(value, "ORDER_CVHG")) {
			pconfig->order = ORDER_CVHG;
			return 1;
		} else if (!strcmp(value, "VGRC") || !strcmp(value, "ORDER_VGRC")) {
			pconfig->order = ORDER_VGRC;
			return 1;
		} else if (!strcmp(value, "RPBC") || !strcmp(value, "ORDER_RPBC")) {
			pconfig->order = ORDER_RPBC;
			return 1;
		} else {
			fprintf(stderr, "config: invalid order: %s (should be GCVH, VGCH, CVHG, VGRC or RPBC)\n", value);
			return 0;
		}
	} else if (MATCH("main", "budget_report_lines")) {
		pconfig->budget_report_lines = atof(value);
		return 1;
	} else if (MATCH("main", "budget_report_nbytes")) {
		pconfig->budget_report_nbytes = atoi(value);
		return 1;
	} else if (MATCH("main", "dq_type")) {
		pconfig->dq_type = atoi(value);
		return 1;
	} else if (MATCH("main", "data_coding")) {
		pconfig->data_coding = atoi(value);
		return 1;
	} else if (MATCH("main", "sp_lenhdr")) {
		pconfig->sp_lenhdr = atoi(value);
		return 1;
	} else if (MATCH("main", "sign_lenhdr")) {
		pconfig->sign_lenhdr = atoi(value);
		return 1;
	} else if (MATCH("main", "prec_lenhdr")) {
		pconfig->prec_lenhdr = atoi(value);
		return 1;
	} else if (MATCH("main", "in_depth")) {
		pconfig->in_depth = atoi(value);
		return 1;
	} else if (MATCH("main", "quant")) {
		pconfig->quant = atoi(value);
		return 1;
	} else if (MATCH("main", "sign_opt")) {
		pconfig->sign_opt = atoi(value);
		return 1;
	} else if (MATCH("main", "skip_wavelet_on_comp")) {
		pconfig->skip_wavelet_on_comp = atoi(value);
		return 1;
	} else if (MATCH("gcli", "run_mode")) {
		 pconfig->run_mode = atoi(value);
		 return 1;
	} else if (MATCH("levels", "gains_choice")) {
		if (!strcmp(value, "gains_choice_psnr")) {
			pconfig->gains_choice = GAINS_CHOICE_PSNR;
			return 1;
		} else if (!strcmp(value, "gains_choice_visual")) {
			pconfig->gains_choice = GAINS_CHOICE_VISUAL;
			return 1;
		} else if (!strcmp(value, "gains_choice_manual")) {
			pconfig->gains_choice = GAINS_CHOICE_MANUAL;
			return 1;
		} else {
			fprintf(stderr, "config: unknown gains_choice value %s\n", value);
			return 0;
		}
	} else {
		if (MATCH("levels", "lvl_gains") || (MATCH("levels", "lvl_priorities")))
		{
			uint8_t *u8ptr = NULL;
			const char* ptr = value;
			int read=0;
			int tmp;
			int idx;
			int ret;

			if (MATCH("levels", "lvl_gains"))
				u8ptr = pconfig->lvl_gains;
			else if (MATCH("levels", "lvl_priorities"))
				u8ptr = pconfig->lvl_priorities;
			else
				assert(0);

			for (idx = 0; idx < MAX_FILTER_TYPES*MAX_NCOMPS; idx++)
			{
				if (idx==0)
					ret = sscanf(ptr, "{%i,%n", &tmp, &read);
				else
					ret = sscanf(ptr, "%i,%n", &tmp, &read);
				ptr+=read;
				if (ret != 1)
					break;
				*u8ptr++ = tmp;
			}
			return 1;
		}
	}
	return 0;
}


int config_parse(const char *cfgfn, tco_conf_t *config)
{
	int status;
	if ((status = ini_parse(cfgfn, config_line_handler, config)) < 0) {
		fprintf(stderr, "config: %s not found\n", cfgfn);
		return -1;
	} else if (status) {
		fprintf(stderr, "config: syntax error line %d\n", status);
		return -1;
	}
	return 0;
}
