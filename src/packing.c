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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include "../std/bsr.h"

#include "budget.h"
#include "common.h"
#include "packing.h"
#include "pred.h"
#include "ra.h"
#include "buf_mgmt.h"
#include "lvls_map.h"
#include "sb_weighting.h"
#include "../inc/tco.h"
#include "gcli_methods.h"
#include "sig_flags.h"
#include "xs_markers.h"

#define ALIGN(value, n) ((value + n - 1) & (~(n - 1)))
#define GENERATE_FRAGMENT_CSV

struct packing_context_t
{
	tco_conf_t conf;
	int use_sign_subpkt;
	int enabled_methods;
	int gcli_method_nbits;
	sigbuffer_t* gcli_significance;
	sigbuffer_t* gcli_nonsig_flags;
};


struct unpacking_context_t
{
	tco_conf_t conf;
	int position_count;
	int level_count;
	int* gtli_table_data;
	int* gtli_table_gcli;
	gcli_pred_t* gclis_pred;
	sigbuffer_t *gclis_significance;
	int use_sign_subpkt;
	int* inclusion_mask;
	int gcli_method_nbits;
	uint64_t enabled_methods;
#ifdef GENERATE_FRAGMENT_CSV
	int fragment_cnt;
	FILE* cbr_csv_f;
	FILE* vbr_csv_f;
	char* csv_buffer_model_cbr;
	char* csv_buffer_model_vbr;
#endif
};

packing_context_t* packer_open(const tco_conf_t* conf, const precinct_t *prec)
{
	packing_context_t* pack_ctx = (packing_context_t*) malloc(sizeof(packing_context_t));
	if (!pack_ctx)
		return NULL;
	pack_ctx->conf = *conf;
	pack_ctx->use_sign_subpkt = (conf->sign_opt == SIGN_PACKING);
	pack_ctx->enabled_methods = gcli_method_get_enabled(conf);
	pack_ctx->gcli_significance = sigbuffer_open(prec);
	pack_ctx->gcli_nonsig_flags = sigbuffer_open(prec);
	pack_ctx->gcli_method_nbits = gcli_method_get_signaling_nbits(pack_ctx->enabled_methods);

	return pack_ctx;
}

void packer_close(packing_context_t* ctx)
{
	if (ctx->gcli_significance)
		sigbuffer_close(ctx->gcli_significance);
	if (ctx->gcli_nonsig_flags)
		sigbuffer_close(ctx->gcli_nonsig_flags);
}

unpacking_context_t* unpacker_open(const tco_conf_t* conf, lvls_map_t* map, const precinct_t *prec)
{
	unpacking_context_t* unpack_ctx = (unpacking_context_t*) malloc(sizeof(unpacking_context_t));
	if (unpack_ctx)
	{

		unpack_ctx->conf			 = *conf;
		unpack_ctx->position_count   = map->position_count;
		unpack_ctx->level_count      = map->level_count;
		unpack_ctx->gtli_table_data  = (int*) malloc(map->position_count * sizeof(int));
		unpack_ctx->gtli_table_gcli  = (int*) malloc(map->position_count * sizeof(int));
		unpack_ctx->gclis_pred       = (gcli_pred_t*) malloc((map->lvls_size_max+conf->group_size-1)/conf->group_size * sizeof(gcli_pred_t) );
		unpack_ctx->use_sign_subpkt = (conf->sign_opt == SIGN_PACKING);
		unpack_ctx->inclusion_mask   = (int*) malloc((map->lvls_size_max+conf->group_size-1)/conf->group_size * sizeof(int));
		unpack_ctx->enabled_methods  = gcli_method_get_enabled(conf);
		unpack_ctx->gcli_method_nbits = gcli_method_get_signaling_nbits(unpack_ctx->enabled_methods);


		unpack_ctx->gclis_significance = sigbuffer_open(prec);
#ifdef GENERATE_FRAGMENT_CSV
		unpack_ctx->fragment_cnt = 0;
		unpack_ctx->cbr_csv_f = NULL;
		unpack_ctx->vbr_csv_f = NULL;
#endif
	}
	return unpack_ctx;
}

int unpacker_set_csv_buffer_model_cbr(unpacking_context_t* unpack_ctx, char* csv_buffer_model_cbr)
{
#ifdef GENERATE_FRAGMENT_CSV
	unpack_ctx->csv_buffer_model_cbr = csv_buffer_model_cbr;
#endif
	return 0;
}

int unpacker_set_csv_buffer_model_vbr(unpacking_context_t* unpack_ctx, char* csv_buffer_model_vbr)
{
#ifdef GENERATE_FRAGMENT_CSV
	unpack_ctx->csv_buffer_model_vbr = csv_buffer_model_vbr;
#endif
	return 0;
}

void unpacker_close(unpacking_context_t* unpack_ctx)
{
#ifdef GENERATE_FRAGMENT_CSV
	if (unpack_ctx->cbr_csv_f)
		fclose(unpack_ctx->cbr_csv_f);
	if (unpack_ctx->vbr_csv_f)
		fclose(unpack_ctx->vbr_csv_f);
#endif
	if (unpack_ctx)
	{
		sigbuffer_close(unpack_ctx->gclis_significance);

		if (unpack_ctx->gtli_table_data)
			free(unpack_ctx->gtli_table_data);
		if (unpack_ctx->gtli_table_gcli)
			free(unpack_ctx->gtli_table_gcli);
		if (unpack_ctx->gclis_pred)
			free(unpack_ctx->gclis_pred);
		if (unpack_ctx->inclusion_mask)
			free(unpack_ctx->inclusion_mask);
		free(unpack_ctx);
	}
}

int pack_data(bit_packer_t* bitstream, sig_mag_data_t* buf, int buf_len, gcli_data_t* gclis, int group_size, int gtli, int sign_packing)
{
	int i=0,idx=0,group=0,bp=0,bit_len=0;
	int start_bp = 0;

	for (group = 0; group<(buf_len+group_size-1)/group_size; group++)
	{
		int valid_group_size = (buf_len-idx) > group_size ? group_size : (buf_len-idx);

		if (gclis[group] > gtli)
		{
			//Transmit sign bitplane
			if (!sign_packing)
			{
				for (i = 0; i < group_size; i++) {
					if (i < valid_group_size) {
						bit_len += bitpacker_write(bitstream, (buf[MIN(idx+i, buf_len-1)]>>SIGN_BIT_POSITION),1);
					} else {
						bit_len += bitpacker_write(bitstream, 0 ,1);
					}
				}
			}
			//Transmit bitplane per bitplane
			start_bp = (group_size > 1) ? gclis[group]-1 : gclis[group]-2;
			for (bp=start_bp; bp>=gtli; bp--) {
				for (i=0; i<group_size; i++) {
					if (i < valid_group_size) {
						bit_len += bitpacker_write(bitstream,(buf[MIN(idx+i, buf_len-1)]>>bp),1);
					} else {
						bit_len += bitpacker_write(bitstream,0,1);
					}
				}
			}
		}
		idx+=group_size;
	}
	return bit_len;
}

int pack_sign(bit_packer_t* bitstream, sig_mag_data_t* buf, int buf_len, gcli_data_t* gclis, int group_size, int gtli)
{
	int idx=0,group=0,bit_len=0;
	for (idx = 0; idx < buf_len; idx++)
	{
		group = idx / group_size;
		if ((gclis[group] > gtli) && ((buf[idx] & ~SIGN_BIT_MASK) >> gtli))
			bit_len += bitpacker_write(bitstream, (buf[idx]>>SIGN_BIT_POSITION),1);
	}
	return bit_len;
}


int unpack_data(bit_unpacker_t* bitstream, sig_mag_data_t* buf, int buf_len, gcli_data_t* gclis, int group_size, int gtli, int sign_packing)
{
	uint64_t val;
	int i=0,idx=0,group=0,bp=0;

	for (group = 0; group<(buf_len+group_size-1)/group_size; group++)
	{
		for (i=0; i<group_size; i++)
			buf[idx+i] = 0;

		if (gclis[group] > gtli)
		{
			if(!sign_packing)
			{
				for (i=0; i<group_size; i++)
				{
					bitunpacker_read(bitstream, &val, 1);
					buf[idx+i] |= (sig_mag_data_t) val<<SIGN_BIT_POSITION;
				}
			}
			for (bp=gclis[group]-1; bp>=gtli; bp--)
				for (i=0; i<group_size; i++)
				{
					if ((bp == gclis[group]-1) && group_size == 1)
						val = 1;
					else
						bitunpacker_read(bitstream, &val, 1);
					buf[idx+i] |= (sig_mag_data_t) ((val&0x01) << bp);
				}
		}
		idx+=group_size;
	}
	return 0;
}

int unpack_sign(bit_unpacker_t* bitstream, sig_mag_data_t* buf, int buf_len, int group_size)
{
	uint64_t val;
	int i=0,idx=0,group=0;

	for (group = 0; group<(buf_len+group_size-1)/group_size; group++)
	{
		for (i=0; i<group_size; i++)
		{
			if(buf[idx+i] != 0)
			{
				bitunpacker_read(bitstream, &val, 1);
				if (idx+i < buf_len) {
					buf[idx+i] |= (sig_mag_data_t) ((val&0x01) << SIGN_BIT_POSITION);
				}
			}
		}
		idx+=group_size;
	}
	return 0;
}

int unary_encode(bit_packer_t* bitstream, gcli_pred_t* gcli_pred_buf, int len, int no_sign, unary_alphabet_t alph)
{
	int n=1,bit_len = 0;
	int i;
	for (i=0; (i < len) && (n>0); i++)
	{
		if (!no_sign)
			n = bitpacker_write_unary_signed(bitstream, gcli_pred_buf[i], alph);
		else
			n = bitpacker_write_unary_unsigned(bitstream, gcli_pred_buf[i]);
		bit_len += n;
	}
	if (n <= 0)
		return -1;
	return bit_len;
}

int bounded_encode(bit_packer_t* bitstream, gcli_pred_t* gcli_pred_buf, gcli_data_t* predictors, int len, int gtli, int alphabet, int band_index)
{
	int n=1,bit_len = 0;
	int i;
	for (i=0; (i < len) && (n>0); i++)
	{
		int min_value;
		int max_value;
		if (predictors)
		{
			bounded_code_get_min_max(predictors[i], gtli, alphabet, &min_value, &max_value, band_index);
		}
		else
		{

			min_value =  -20;
			max_value = 20;
		}
		n = bitpacker_write_bounded_code(bitstream, gcli_pred_buf[i], min_value,max_value, alphabet);
		bit_len += n;
	}
	if (n <= 0)
		return -1;
	return bit_len;
}

int unary_decode(bit_unpacker_t* bitstream, gcli_pred_t* gcli_pred_buf, int len, int no_sign, unary_alphabet_t alph)
{
	int i;
	for (i=0; i < len; i++)
		if (!no_sign)
			bitunpacker_read_unary_signed(bitstream, &(gcli_pred_buf[i]), alph);
		else
			bitunpacker_read_unary_unsigned(bitstream, &(gcli_pred_buf[i]));
	return 0;
}

int bounded_decode(bit_unpacker_t* bitstream, gcli_pred_t* gcli_pred_buf, gcli_data_t* gcli_top_buf, int gtli, int gtli_top, int len, int alphabet, int band_index, int top_trunc_method)
{
	int i;
	for (i=0; (i < len); i++)
	{
		int min_value;
		int max_value;
		if (gcli_top_buf)
		{
			int predictor;
			predictor = tco_pred_ver_compute_predictor(gcli_top_buf[i], gtli_top, gtli, top_trunc_method);
			bounded_code_get_min_max(predictor, gtli, alphabet, &min_value, &max_value, band_index);
		}
		else
		{

			min_value = -20;
			max_value = 20;
		}
		bitunpacker_read_bounded_code(bitstream, min_value, max_value, &(gcli_pred_buf[i]), alphabet);
	}
	return 0;
}

int unary_decode2(bit_unpacker_t* bitstream, gcli_pred_t* gcli_pred_buf, int* inclusion_mask, int len, int no_sign, unary_alphabet_t alph)
{
	int i;
	int nbits = 0;
	for (i=0; i < len; i++)
	{
		if (inclusion_mask[i])
		{
			if (!no_sign)
				nbits+=bitunpacker_read_unary_signed(bitstream, &(gcli_pred_buf[i]), alph);
			else
				nbits+=bitunpacker_read_unary_unsigned(bitstream, &(gcli_pred_buf[i]));
		}
		else
		{
			gcli_pred_buf[i] = 0;
		}
	}
	return nbits;
}

int bounded_decode2(bit_unpacker_t* bitstream, gcli_pred_t* gcli_pred_buf, int* inclusion_mask, gcli_data_t* gcli_top_buf, int gtli, int gtli_top, int len, int alphabet, int band_index, int top_trunc_method)
{
	int i;
	int nbits = 0;
	for (i=0; (i < len); i++)
	{
		if (inclusion_mask[i])
		{
			int min_value;
			int max_value;
			if (gcli_top_buf)
			{
				int predictor;
				predictor = tco_pred_ver_compute_predictor(gcli_top_buf[i], gtli_top, gtli, top_trunc_method);
				bounded_code_get_min_max(predictor, gtli, alphabet, &min_value, &max_value, band_index);
			}
			else
			{

				min_value = -20;
				max_value = 20;
			}
			nbits+=bitunpacker_read_bounded_code(bitstream, min_value, max_value, &(gcli_pred_buf[i]), alphabet);
		}
		else
		{
			gcli_pred_buf[i] = 0;
		}
	}
	return nbits;
}

int pack_raw_gclis(bit_packer_t* bitstream, gcli_data_t* gclis, int len)
{
	int bit_len = 0,n = 1;
	int i;
	for (i=0; (i<len) && (n>0); i++)
	{
		n = bitpacker_write(bitstream,gclis[i],4);
		bit_len += n;
	}
	if (n<=0)
		return -1;
	return bit_len;
}

int unpack_raw_gclis(bit_unpacker_t* bitstream, gcli_data_t* gclis, int len)
{
	int i;
	uint64_t val;
	for (i=0; i<len; i++)
	{
		bitunpacker_read(bitstream,&val,4);
		gclis[i] = (gcli_data_t) val;
	}
	return 0;
}


int pack_gclis_significance(packing_context_t* ctx, bit_packer_t* bitstream, precinct_t* precinct, rc_results_t* ra_result, int idx_start, int idx_stop)
{
	int nbits = 0;
	int idx;
	sig_flags_t* sig_flags = NULL;

	for (idx=idx_start; idx<=idx_stop; idx++)
	{
		int lvl = precinct_lvl_of(precinct, idx);
		int ypos = precinct_ypos_of(precinct, idx);
		int gtli	   = ra_result->gtli_table_gcli[lvl];
		int sb_gcli_method  = ra_result->gcli_sb_methods[lvl];
		if (method_uses_sig_flags(sb_gcli_method))
		{
			int gcli_len;
			directional_prediction_t* pred_res;
			directional_prediction_t* pred_sig_flags;
			int group_width = ctx->conf.sigflags_group_width;
			if (!sig_flags)
				sig_flags = sig_flags_malloc(precinct_max_gcli_width_of(precinct), group_width);


			pred_res = directional_data_of(ra_result->pred_residuals, method_get_pred(sb_gcli_method));
			pred_sig_flags = pred_res;
			if (method_get_run(sb_gcli_method) == RUN_SIGFLAGS_ZRCSF)
				pred_sig_flags = directional_data_of(ra_result->pred_residuals, PRED_NONE);

			gcli_len = prediction_width_of(pred_sig_flags,lvl);

			sig_flags_init(sig_flags, prediction_values_of(pred_sig_flags,lvl,ypos,gtli), gcli_len, group_width);


			sig_flags_filter_values(sig_flags, prediction_values_of(pred_res,lvl,ypos,gtli), sig_values_of(ctx->gcli_significance, lvl, ypos), sig_width_of(ctx->gcli_significance, lvl, ypos));
			sig_flags_filter_values(sig_flags, prediction_predictors_of(pred_res, lvl, ypos, gtli), sig_predictors_of(ctx->gcli_significance, lvl, ypos), sig_width_of(ctx->gcli_significance, lvl, ypos));

			nbits += sig_flags_write(bitstream, sig_flags, ctx->conf.codesig_with_0);
		}

#ifdef PACKING_VERBOSE
		fprintf(stderr, "SIGF (cursor=%d) (lvl=%d ypos=%d) (w=%d,wg=%d))\n", (int)bitpacker_get_len(bitstream), lvl, ypos,(int)precinct_width_of(precinct,lvl),(int)precinct_gcli_width_of(precinct,lvl));
#endif
	}
	if (sig_flags)
		sig_flags_free(sig_flags);
	return nbits;
}

int unpack_gclis_significance(bit_unpacker_t* bitstream, unpacking_context_t* ctx, precinct_t* prec, int* sb_gcli_methods, int idx_start, int idx_stop)
{
	int bits = 0;
	int idx;
	sig_flags_t* sig_flags = NULL;
	for (idx=idx_start; idx<=idx_stop; idx++)
	{
		int lvl = precinct_lvl_of(prec, idx);
		int ypos = precinct_ypos_of(prec, idx);
		int sb_gcli_method = sb_gcli_methods[lvl];
		if (method_uses_sig_flags(sb_gcli_method))
		{
			if (!sig_flags)
				sig_flags = sig_flags_malloc((int)precinct_max_gcli_width_of(prec), ctx->conf.sigflags_group_width);

			bits = sig_flags_read(bitstream, sig_flags, precinct_gcli_width_of(prec,lvl), ctx->conf.sigflags_group_width, ctx->conf.codesig_with_0);
			sig_flags_inclusion_mask(sig_flags, (uint8_t*) sig_values_of(ctx->gclis_significance,lvl,ypos));
		}
#ifdef PACKING_VERBOSE
		fprintf(stderr, "SIGF (cursor=%d) (lvl=%d ypos=%d) (w=%d,wg=%d))\n", (int)bitunpacker_consumed_bits(bitstream), lvl, ypos,(int)precinct_width_of(prec,lvl),(int)precinct_gcli_width_of(prec,lvl));
#endif
	}
	if (sig_flags)
		sig_flags_free(sig_flags);
	return bits;
}


int pack_gclis(packing_context_t* ctx, bit_packer_t* bitstream, precinct_t* precinct, rc_results_t* ra_result, int position)
{
	int err  = 0;
	int lvl  = precinct_lvl_of(precinct, position);
	int ypos = precinct_ypos_of(precinct, position);
	int gtli = ra_result->gtli_table_gcli[lvl];
	int subpkt = precinct_subpkt_of(precinct, position);
	int sb_method = ra_result->pbinfo.subpkt_uses_raw_fallback[subpkt] ? method_get_idx(ALPHABET_RAW_4BITS,0,0) : ra_result->gcli_sb_methods[lvl];
	bool_t method_use_gcli_opt = method_uses_sig_flags(sb_method);
	unary_alphabet_t alph = FIRST_ALPHABET;
	int values_len;
	gcli_pred_t *values_to_code;
	gcli_data_t *predictors;
	int unsigned_unary_alph;

#ifdef PACKING_VERBOSE
	fprintf(stderr, "GCLI (cursor=%d) (method=", bitpacker_get_len(bitstream));
	print_method(stderr, sb_method);
	fprintf(stderr, " lvl=%d ypos=%d gtli=%d)\n", lvl, ypos, gtli);
#endif

	if (method_is_raw(sb_method))
	{
		err = (err) || (pack_raw_gclis(bitstream,
						   precinct_gcli_of(precinct,lvl,ypos),
						   (int)precinct_gcli_width_of(precinct,lvl)) < 0);
	}
	else
	{
		int nbits = 0;
		if (method_use_gcli_opt)
		{
			values_to_code = sig_values_of(ctx->gcli_significance, lvl, ypos);
			values_len = *sig_width_of(ctx->gcli_significance, lvl, ypos);
			predictors = sig_predictors_of(ctx->gcli_significance, lvl, ypos);
		}
		else
		{
			int pred_type;
			directional_prediction_t* pred;
			pred_type = method_prediction_type(sb_method);
			assert(pred_type >= 0);
			pred = directional_data_of(ra_result->pred_residuals,pred_type);
			values_to_code = prediction_values_of(pred,lvl,ypos,gtli);
			values_len = prediction_width_of(pred,lvl);
			predictors = prediction_predictors_of(pred,lvl,ypos,gtli);
		}
		unsigned_unary_alph = method_uses_no_pred(sb_method);


		if  ((method_get_alphabet(sb_method) != ALPHABET_UNARY_UNSIGNED_BOUNDED)
			||(method_get_pred(sb_method) == PRED_NONE))
		{
			err = (err) || ((nbits=unary_encode(bitstream, values_to_code, values_len, unsigned_unary_alph, alph)) < 0);
		}
		else
		{
			err = (err) || ((nbits=bounded_encode(bitstream, values_to_code, predictors, values_len, gtli, ctx->conf.bounded_alphabet, lvl)) < 0);
		}
		assert(nbits >= 0);
	}
	return err;
}


int unpack_gclis(bit_unpacker_t* bitstream,
		 precinct_t* precinct,
		 precinct_t* precinct_top,
		 int* gtli_table_top,
		 unpacking_context_t* ctx,
		 int position,
		 int sb_gcli_method,
		 int scenario)
{
	int lvl  = precinct_lvl_of(precinct, position);
	int ypos = precinct_ypos_of(precinct, position);
	int gtli = ctx->gtli_table_gcli[lvl];
	int gtli_top = (ypos == 0)?(gtli_table_top[lvl]):(gtli);
	unary_alphabet_t alph = FIRST_ALPHABET;

#ifdef PACKING_VERBOSE
	fprintf(stderr, "GCLI (cursor=%d) (method=", (int) bitunpacker_consumed_bits(bitstream));
	print_method(stderr, sb_gcli_method);
	fprintf(stderr, " lvl=%d ypos=%d gtli=%d)\n", lvl, ypos, gtli);
#endif

	if (method_is_raw(sb_gcli_method))
	{
		unpack_raw_gclis(bitstream, precinct_gcli_of(precinct,lvl,ypos),
						 (int)precinct_gcli_width_of(precinct,lvl));
	}
	else
	{
		int gclis_len = (int)precinct_gcli_width_of(precinct,lvl);
		int i, nbits;
		int no_prediction;
		int sig_flags_are_zrcsf = (method_get_run(sb_gcli_method) == RUN_SIGFLAGS_ZRCSF);
		gcli_data_t* gclis_top = precinct_gcli_top_of(precinct, precinct_top, lvl, ypos);


		if (method_uses_sig_flags(sb_gcli_method))
			for (i = 0; i < precinct_gcli_width_of(precinct, lvl); i++)
				ctx->inclusion_mask[i] = sig_values_of(ctx->gclis_significance, lvl, ypos)[i];
		else
			for (i = 0; i < precinct_gcli_width_of(precinct, lvl); i++)
				ctx->inclusion_mask[i] = 1;

		no_prediction = method_uses_no_pred(sb_gcli_method) || ((method_uses_ver_pred(sb_gcli_method) && method_get_alphabet(sb_gcli_method) == ALPHABET_UNARY_UNSIGNED_BOUNDED) && (gclis_top==NULL));

		if  ((method_get_alphabet(sb_gcli_method) != ALPHABET_UNARY_UNSIGNED_BOUNDED) || no_prediction)
			nbits = unary_decode2(bitstream, ctx->gclis_pred, ctx->inclusion_mask, gclis_len, no_prediction, alph);
		else
			nbits = bounded_decode2(bitstream, ctx->gclis_pred, ctx->inclusion_mask, gclis_top, gtli, gtli_top, gclis_len, ctx->conf.bounded_alphabet, lvl, ctx->conf.gc_top_pred_mode);
		assert(nbits >= 0);

		if ((method_uses_ver_pred(sb_gcli_method) && gclis_top))
		{
			tco_pred_ver_inverse(ctx->gclis_pred, gclis_top, gclis_len, precinct_gcli_of(precinct,lvl,ypos),gtli, gtli_top, ctx->conf.gc_top_pred_mode);
			if (sig_flags_are_zrcsf)
				for (i = 0; i < precinct_gcli_width_of(precinct, lvl); i++)
					if (!ctx->inclusion_mask[i])
						precinct_gcli_of(precinct,lvl,ypos)[i] = 0;
		}
		else if (method_uses_no_pred(sb_gcli_method) || (method_uses_ver_pred(sb_gcli_method) && gclis_top==NULL))
		{
			tco_pred_none_inverse(ctx->gclis_pred, gclis_len, precinct_gcli_of(precinct,lvl,ypos), gtli);
		}
		else
		{
			assert(0);
		}
	}
	return 0;
}




int pack_precinct(packing_context_t* ctx, bit_packer_t* bitstream, precinct_t* precinct, rc_results_t* ra_result)
{
	int err = 0;
	int len_before = bitpacker_get_len(bitstream);
	int len_after = 0;
	int lvl, ypos, idx_start, idx_stop, idx, gtli;
	int len_before_subpkt=0;
	int subpkt;
	int position;
	int header_len;

#ifdef PACKING_VERBOSE
	fprintf(stderr, "(cursor=%d) New Precinct\n", (int) bitpacker_get_len(bitstream));
#endif

	header_len = PREC_HDR_PREC_SIZE + PREC_HDR_SCENARIO_SIZE + PREC_HDR_REFINEMENT_SIZE;
	header_len += (ctx->gcli_method_nbits * bands_count_of(precinct));
	header_len = (header_len + 7) & ~0x7;



	err = (err) || (bitpacker_write(bitstream, ((ra_result->precinct_total_bits-header_len)>>3), PREC_HDR_PREC_SIZE) < 0);


	assert(ra_result->scenario < 16);
	err = (err) || (bitpacker_write(bitstream, ra_result->scenario, PREC_HDR_SCENARIO_SIZE) < 0);
	err = (err) || (bitpacker_write(bitstream, ra_result->refinement, PREC_HDR_REFINEMENT_SIZE) < 0);



	for (position = 0; position < line_count_of(precinct); position++)
	{
		int method_signaling;
		lvl = precinct_lvl_of(precinct, position);
		ypos = precinct_ypos_of(precinct, position);
		if (ypos == 0)
		{
			method_signaling = gcli_method_get_signaling(ra_result->gcli_sb_methods[lvl], ctx->enabled_methods);
			err = (err) || (bitpacker_write(bitstream, method_signaling, ctx->gcli_method_nbits) < 0);
		}
	}
	bitpacker_align(bitstream, PREC_HDR_ALIGNMENT);

#ifdef PACKING_VERBOSE
	fprintf(stderr, "(cursor=%d) precinct header packed (%d %d)\n", bitpacker_get_len(bitstream), ra_result->scenario, ra_result->refinement);
#endif

	subpkt=0;
	for (idx_start=idx_stop=0; idx_stop < line_count_of(precinct); idx_stop++)
	{

		if ((idx_stop != (line_count_of(precinct) - 1)) &&
			 (precinct_subpkt_of(precinct, idx_stop) ==
			  precinct_subpkt_of(precinct, idx_stop+1)))
			continue;


		bitpacker_write(bitstream, (uint64_t) ra_result->pbinfo.subpkt_uses_raw_fallback[subpkt], 1);
		bitpacker_write(bitstream, (uint64_t) ra_result->pbinfo.subpkt_size_data[subpkt] >> 3,ctx->conf.pkt_hdr_size_short ? PKT_HDR_DATA_SIZE_SHORT : PKT_HDR_DATA_SIZE_LONG);
		bitpacker_write(bitstream, (uint64_t) ra_result->pbinfo.subpkt_size_gcli[subpkt] >> 3,ctx->conf.pkt_hdr_size_short ? PKT_HDR_GCLI_SIZE_SHORT : PKT_HDR_GCLI_SIZE_LONG);
		bitpacker_write(bitstream, (uint64_t) ra_result->pbinfo.subpkt_size_sign[subpkt] >> 3,ctx->conf.pkt_hdr_size_short ? PKT_HDR_SIGN_SIZE_SHORT : PKT_HDR_SIGN_SIZE_LONG);
		bitpacker_align(bitstream, PKT_HDR_ALIGNMENT);

#ifdef PACKING_VERBOSE
		fprintf(stderr, "(cursor=%d) New Subpacket DATALEN%d GCLILEN=%d SIGNLEN=%d (force_raw%d)\n", bitpacker_get_len(bitstream),
			ra_result->pbinfo.subpkt_size_data[subpkt] >> 3,
			ra_result->pbinfo.subpkt_size_gcli[subpkt] >> 3,
			ra_result->pbinfo.subpkt_size_sign[subpkt] >> 3,
			ra_result->pbinfo.subpkt_uses_raw_fallback[subpkt]);
#endif


		len_before_subpkt = bitpacker_get_len(bitstream);
		if (!ra_result->pbinfo.subpkt_uses_raw_fallback[subpkt])
		{
			pack_gclis_significance(ctx, bitstream, precinct, ra_result, idx_start, idx_stop);
		}
		bitpacker_align(bitstream, SUBPKT_ALIGNMENT);

		if ((bitpacker_get_len(bitstream) - len_before_subpkt) != ra_result->pbinfo.subpkt_size_sigf[subpkt])
		{
			fprintf(stderr, "Error: (SIG) length mismatch - rate_control=%d packing=%d\n", ra_result->pbinfo.subpkt_size_sigf[subpkt], bitpacker_get_len(bitstream) - len_before_subpkt);
			return -1;
		}

		len_before_subpkt = bitpacker_get_len(bitstream);
		for (idx=idx_start; idx<=idx_stop; idx++)
		{
			err = (err) || pack_gclis(ctx, bitstream, precinct, ra_result, idx);
			if (err)
				return -1;
		}
		bitpacker_align(bitstream, SUBPKT_ALIGNMENT);

		if ((bitpacker_get_len(bitstream) - len_before_subpkt) != ra_result->pbinfo.subpkt_size_gcli[subpkt])
		{
			fprintf(stderr, "Error: (GCLI) length mismatch - rate_control=%d packing=%d\n", ra_result->pbinfo.subpkt_size_gcli[subpkt], bitpacker_get_len(bitstream) - len_before_subpkt);
			return -1;
		}


		len_before_subpkt = bitpacker_get_len(bitstream);
		for (idx=idx_start; idx<=idx_stop; idx++)
		{
			lvl = precinct_lvl_of(precinct, idx);
			ypos = precinct_ypos_of(precinct, idx);
			gtli = ra_result->gtli_table_data[lvl];
#ifdef PACKING_VERBOSE
	fprintf(stderr, "DATA (cursor=%d) ", bitpacker_get_len(bitstream));
	fprintf(stderr, " (lvl=%d ypos=%d gtli=%d)\n", lvl, ypos, gtli);
#endif
			err  = (err) || (pack_data(bitstream,
						  precinct_line_of(precinct,lvl,ypos),
						  (int)precinct_width_of(precinct,lvl),
						  precinct_gcli_of(precinct,lvl,ypos),
						  ctx->conf.group_size, gtli,
						  ctx->use_sign_subpkt) < 0);
		}
		bitpacker_align(bitstream, SUBPKT_ALIGNMENT);

		if ((bitpacker_get_len(bitstream) - len_before_subpkt) != ra_result->pbinfo.subpkt_size_data[subpkt])
		{
			fprintf(stderr, "Error: (DATA) length mismatch - rate_control=%d packing=%d\n", ra_result->pbinfo.subpkt_size_data[subpkt], bitpacker_get_len(bitstream) - len_before_subpkt);
			return -1;
		}

		len_before_subpkt = bitpacker_get_len(bitstream);
		if (ctx->use_sign_subpkt)
		{
			for (idx=idx_start; idx<=idx_stop; idx++)
			{
				lvl = precinct_lvl_of(precinct, idx);
				ypos = precinct_ypos_of(precinct, idx);
				gtli = ra_result->gtli_table_data[lvl];
#ifdef PACKING_VERBOSE
	fprintf(stderr, "SIGN (cursor=%d) ", bitpacker_get_len(bitstream));
	fprintf(stderr, " (lvl=%d ypos=%d gtli=%d)\n", lvl, ypos, gtli);
#endif
				err  = (err) || (pack_sign(bitstream,
							  precinct_line_of(precinct,lvl,ypos),
							  (int)precinct_width_of(precinct,lvl),
							  precinct_gcli_of(precinct,lvl,ypos),
							  ctx->conf.group_size, gtli) < 0);
			}
			bitpacker_align(bitstream, SUBPKT_ALIGNMENT);
		}
		if ((bitpacker_get_len(bitstream) - len_before_subpkt) != ra_result->pbinfo.subpkt_size_sign[subpkt])
		{
			fprintf(stderr, "Error: (SIGN) length mismatch - rate_control=%d packing=%d\n", ra_result->pbinfo.subpkt_size_sign[subpkt], bitpacker_get_len(bitstream) - len_before_subpkt);
			return -1;
		}

		idx_start = idx_stop + 1;
		subpkt++;
	}

	if (err)
	{
		fprintf(stderr, "Error: packing could not write all data to output buffer\n");
		return -1;
	}
	len_after = bitpacker_get_len(bitstream);

	if ((ra_result->pbinfo.precinct_bits > 0) &&
		((len_after - len_before) != ra_result->pbinfo.precinct_bits))
	{
		fprintf(stderr, "Error: (packed_len != computed_len) packed=%d expected=%d\n",
			len_after - len_before, ra_result->pbinfo.precinct_bits);
		return -1;
	}
	if (len_after % 4)
	{
		fprintf(stderr, "Error: end of precincts are supposed to be aligned on 4 bits\n");
		return -1;
	}


	if (ra_result->pbinfo.precinct_bits < ra_result->precinct_total_bits)
	{
		bitpacker_add_padding(bitstream,ra_result->precinct_total_bits - ra_result->pbinfo.precinct_bits);
	}
	else if (ra_result->pbinfo.precinct_bits > ra_result->precinct_total_bits)
	{
		fprintf(stderr, "Error: too much data packed (%d) for specified precinct lenght marker (%d)\n",
			ra_result->pbinfo.precinct_bits,ra_result->precinct_total_bits);
		return -1;
	}
	return 0;
}

int unpack_precinct(unpacking_context_t* ctx, bit_unpacker_t* bitstream,
		    precinct_t* precinct, precinct_t* precinct_top, int* gtli_table_top,
		    unpacked_info_t* info)
{
	uint64_t val;
	int empty;
	int len_before = (int)bitunpacker_consumed_bits(bitstream);
	int len_before_subpkt = 0;
	int lvl, idx_start, idx_stop, idx, gtli, ypos;
	int scenario, refinement;
	int gcli_sb_methods[MAX_LVLS];
	int subpkt = 0;
	int subpkt_len;
	int uses_raw_fallback;
	int position;
	int header_len;
	int prec_len_marker;

#ifdef GENERATE_FRAGMENT_CSV
	if (ctx->csv_buffer_model_cbr && ! ctx->cbr_csv_f)
	{
		ctx->cbr_csv_f = fopen(ctx->csv_buffer_model_cbr,"wb");
	}
	if (ctx->csv_buffer_model_vbr && ! ctx->vbr_csv_f)
	{
		ctx->vbr_csv_f = fopen(ctx->csv_buffer_model_vbr,"wb");
	}
#endif

	header_len = PREC_HDR_PREC_SIZE + PREC_HDR_SCENARIO_SIZE + PREC_HDR_REFINEMENT_SIZE;
	header_len += (ctx->gcli_method_nbits * bands_count_of(precinct));
	header_len = (header_len + 7) & ~0x7;

#ifdef PACKING_VERBOSE
	fprintf(stderr, "(cursor=%d) New Precinct\n", (int) bitunpacker_consumed_bits(bitstream));
#endif


	bitunpacker_read(bitstream, &val, PREC_HDR_PREC_SIZE);
	prec_len_marker = val;
	info->prec_len = (int) (prec_len_marker<<3) + header_len;
	bitunpacker_read(bitstream, &val, PREC_HDR_SCENARIO_SIZE);
	scenario = (int) val;
	bitunpacker_read(bitstream, &val, PREC_HDR_REFINEMENT_SIZE);
	refinement = (int) val;


	for (position = 0; position < line_count_of(precinct); position++)
	{
		lvl = precinct_lvl_of(precinct, position);
		ypos = precinct_ypos_of(precinct, position);
		if (ypos == 0)
		{
			bitunpacker_read(bitstream, &val, ctx->gcli_method_nbits);
			gcli_sb_methods[lvl] = gcli_method_from_signaling(val, ctx->enabled_methods);
		}
	}
	bitunpacker_align(bitstream, PREC_HDR_ALIGNMENT);

#ifdef PACKING_VERBOSE
	fprintf(stderr, "(cursor=%d) precinct header read (prec_len=%d quant=(%d,%d)\n", (int) bitunpacker_consumed_bits(bitstream), prec_len_marker, scenario, refinement);
#endif


	compute_gtli_tables(scenario,
						refinement,
						ctx->level_count,
						ctx->conf.lvl_gains,
						ctx->conf.lvl_priorities,
						ctx->conf.gc_trunc,
						ctx->conf.gc_trunc_use_priority,
						ctx->gtli_table_data,
						ctx->gtli_table_gcli,
						&empty);


	for (idx_start=idx_stop=0; idx_stop<ctx->position_count; idx_stop++)
	{
#ifdef GENERATE_FRAGMENT_CSV
		int packet_start_cr = (int)bitunpacker_consumed_bits(bitstream);
#endif


		if ( (idx_stop != (ctx->position_count - 1)) &&
			 (precinct_subpkt_of(precinct, idx_stop) ==
			  precinct_subpkt_of(precinct, idx_stop+1) ) )
			continue;

		lvl  = precinct_lvl_of(precinct, idx_start);
		ypos = precinct_ypos_of(precinct, idx_start);
		if (!precinct_is_line_present(precinct,lvl,ypos))
		    continue;

		bitunpacker_read(bitstream, &val, 1);
		uses_raw_fallback = (int) val & 0x1;

		bitunpacker_read(bitstream, &val, ctx->conf.pkt_hdr_size_short ? PKT_HDR_DATA_SIZE_SHORT : PKT_HDR_DATA_SIZE_LONG);
		info->data_len[subpkt] = (int) val;

		bitunpacker_read(bitstream, &val, ctx->conf.pkt_hdr_size_short ? PKT_HDR_GCLI_SIZE_SHORT : PKT_HDR_GCLI_SIZE_LONG);
		info->gcli_len[subpkt] = (int) val;

		bitunpacker_read(bitstream, &val, ctx->conf.pkt_hdr_size_short ? PKT_HDR_SIGN_SIZE_SHORT : PKT_HDR_SIGN_SIZE_LONG);
		info->sign_len[subpkt] = (int) val;

		bitunpacker_align(bitstream, PKT_HDR_ALIGNMENT);

#ifdef PACKING_VERBOSE
		fprintf(stderr, "(cursor=%d) New Subpacket DATALEN%d GCLILEN=%d SIGNLEN=%d (force_raw%d)\n", (int) bitunpacker_consumed_bits(bitstream),
			info->data_len[subpkt],
			info->gcli_len[subpkt],
			info->sign_len[subpkt],
			uses_raw_fallback);
#endif



		if (!uses_raw_fallback)
			unpack_gclis_significance(bitstream, ctx, precinct, gcli_sb_methods, idx_start, idx_stop);
		bitunpacker_align(bitstream, SUBPKT_ALIGNMENT);


		len_before_subpkt = (int)bitunpacker_consumed_bits(bitstream);

		for (idx=idx_start; idx<=idx_stop; idx++)
		{
			int sb_gcli_method = uses_raw_fallback ? method_get_idx(ALPHABET_RAW_4BITS,0,0) : gcli_sb_methods[precinct_lvl_of(precinct, idx)];
			unpack_gclis(bitstream, precinct, precinct_top, gtli_table_top, ctx, idx, sb_gcli_method, scenario);
		}


		bitunpacker_align(bitstream, SUBPKT_ALIGNMENT);

		subpkt_len = (int)bitunpacker_consumed_bits(bitstream) - len_before_subpkt;
		if (subpkt_len != (info->gcli_len[subpkt]<<3))
		{
			fprintf(stderr, "Error: (GCLI) corruption detected - unpacked=%d , expected=%d\n", subpkt_len, info->gcli_len[subpkt]<<3);
#ifdef PACKING_REALIGN
			goto realign;
#endif
			return -1;
		}

		len_before_subpkt = (int)bitunpacker_consumed_bits(bitstream);


		for (idx=idx_start; idx<=idx_stop; idx++)
		{
			lvl  = precinct_lvl_of(precinct, idx);
			ypos = precinct_ypos_of(precinct, idx);
			gtli = ctx->gtli_table_data[lvl];
#ifdef PACKING_VERBOSE
			fprintf(stderr, "DATA (cursor=%d) ", (int) bitunpacker_consumed_bits(bitstream));
			fprintf(stderr, " (lvl=%d ypos=%d gtli=%d)\n", lvl, ypos, gtli);
#endif
			unpack_data(bitstream,
				    precinct_line_of(precinct,lvl,ypos),
				    (int)precinct_width_of(precinct,lvl),
				    precinct_gcli_of(precinct,lvl,ypos),
				    ctx->conf.group_size, gtli, ctx->use_sign_subpkt);
		}
		bitunpacker_align(bitstream, SUBPKT_ALIGNMENT);

		subpkt_len = (int)bitunpacker_consumed_bits(bitstream) - len_before_subpkt;
		if (subpkt_len != (info->data_len[subpkt]<<3))
		{
			fprintf(stderr, "Error: (DATA) corruption detected - unpacked=%d , expected=%d\n", subpkt_len, info->data_len[subpkt]<<3);
#ifdef PACKING_REALIGN
			goto realign;
#endif
			return -1;
		}

		if(ctx->use_sign_subpkt)
		{
			len_before_subpkt = (int)bitunpacker_consumed_bits(bitstream);
			for (idx=idx_start; idx<=idx_stop; idx++)
			{
				lvl  = precinct_lvl_of(precinct, idx);
				ypos = precinct_ypos_of(precinct, idx);
#ifdef PACKING_VERBOSE
				fprintf(stderr, "SIGN (cursor=%d) ", (int)bitunpacker_consumed_bits(bitstream));
				fprintf(stderr, " (lvl=%d ypos=%d gtli=%d)\n", lvl, ypos, ctx->gtli_table_data[lvl]);
#endif
				unpack_sign(bitstream,
					    precinct_line_of(precinct,lvl,ypos),
					    (int)precinct_width_of(precinct,lvl),
					    ctx->conf.group_size);
			}
			bitunpacker_align(bitstream, SUBPKT_ALIGNMENT);

			subpkt_len = (int)bitunpacker_consumed_bits(bitstream) - len_before_subpkt;
			if (subpkt_len != (info->sign_len[subpkt]<<3))
			{
				fprintf(stderr, "Error: (SIGN) corruption detected - unpacked=%d , expected=%d\n", subpkt_len, info->sign_len[subpkt]<<3);
#ifdef PACKING_REALIGN
			goto realign;
#endif
			return -1;
			}
		}

#ifdef GENERATE_FRAGMENT_CSV
		if (ctx->vbr_csv_f || ctx->cbr_csv_f)
		{
			int packet_end_cr = (int)bitunpacker_consumed_bits(bitstream);
			int fragment_size = packet_end_cr - packet_start_cr;
			int n_gclis = 0;
			int padding_len = info->prec_len - ((int)bitunpacker_consumed_bits(bitstream) - len_before);

			for (idx=idx_start; idx<=idx_stop; idx++)
			{
				lvl  = precinct_lvl_of(precinct, idx);
				n_gclis += (int)precinct_gcli_width_of(precinct,lvl);
			}
			if (idx_start == 0)
			{

				if ((precinct_idx_of(precinct) == 0) && (precinct->column == 0))
				{

					fragment_size = bitunpacker_consumed_bits(bitstream);
				}
				else
				{
					if (precinct_is_first_of_slice(precinct, ctx->conf.slice_height) && (precinct->column == 0))
					{

						fragment_size += XS_SLICE_HEADER_NBYTES*8;
					}
					fragment_size += header_len;
				}
			}
			if ((idx_stop == ctx->position_count - 1) &&
				((int) bitunpacker_consumed_bits(bitstream) + padding_len + XS_MARKER_NBITS == ctx->conf.bitstream_nbytes*8))
			{

				fragment_size += XS_MARKER_NBITS;
			}

			if (ctx->vbr_csv_f)
			{
				fprintf(ctx->vbr_csv_f, "%d;%d;%d\n", ctx->fragment_cnt, fragment_size, n_gclis);
			}
			if (idx_stop == ctx->position_count - 1)
			{

				fragment_size += padding_len;
			}
			if (ctx->cbr_csv_f)
			{
				fprintf(ctx->cbr_csv_f, "%d;%d;%d\n", ctx->fragment_cnt, fragment_size, n_gclis);
			}
			ctx->fragment_cnt++;
		}
#endif


		subpkt++;
		idx_start = idx_stop + 1;
	}




#ifdef PACKING_REALIGN
realign:
#endif
	info->padding_len = info->prec_len - ((int)bitunpacker_consumed_bits(bitstream) - len_before);
#ifdef PACKING_VERBOSE
	fprintf(stderr, "PADDING (cursor=%d bytes) until (cursor=%d bytes) bits", (int) bitunpacker_consumed_bits(bitstream)/8, (int) bitunpacker_consumed_bits(bitstream)/8 + info->padding_len/8);
#endif
	assert( info->padding_len >= 0);
	bitunpacker_skip(bitstream, info->padding_len);


	memcpy(info->gtli_table_data, ctx->gtli_table_data, ctx->position_count*sizeof(int));
	memcpy(info->gtli_table_gcli, ctx->gtli_table_gcli, ctx->position_count*sizeof(int));
	return 0;
}


#ifdef TEST_PACKING


int main()
{

}

#endif
