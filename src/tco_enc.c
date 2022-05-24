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
#include <stdint.h>
#include <assert.h>

#include "common.h"
#include "xs_markers.h"
#include "tco_conf.h"
#include "precinct.h"
#include "predbuffer.h"
#include "image_transforms.h"
#include "dwt53.h"
#include "buf_mgmt.h"
#include "bitpacking.h"
#include "gcli.h"
#include "gcli_budget.h"
#include "data_budget.h"
#include "sig_mag.h"
#include "budget.h"
#include "ra.h"
#include "packing.h"
#include "quant.h"
#include "pred.h"
#include "slice.h"
#include "lvls_map.h"
#include "sigbuffer.h"
#include "rate_control.h"
#include "budget_dump.h"

#ifdef _MSC_VER
#include <intrin.h>
#endif

#if defined(_WIN32) && !defined(__MINGW32__)
static unsigned int __inline BSR(unsigned long x)
{
		unsigned long r = 0;
		_BitScanReverse(&r, x);
		return r;
}
#else
#define BSR(x) (31 - __builtin_clz(x))
#endif
#define FLOOR_LOG2(x)  BSR(x)


#define MODULO_NEG(value, modulo) ((value + modulo) % modulo)

struct tco_enc_context_t
{
	int ncomps;
	int w;
	int h;
	int is_422;
	int depth;
	tco_conf_t conf;
	lvls_map_t map;
	precinct_t  *precinct[MAX_PREC_COLS];
	precinct_t  *precinct_tmp[2];
	packing_context_t* packer;

	dwt_data_t* dwt53_tmp_data;

	bit_packer_t* bitstream;
	int bitstream_len;
	image_t* image_tmp[2];
	image_t* image_transformed;

	rate_control_t* rc[MAX_PREC_COLS];
};





tco_enc_context_t* tco_enc_init(tco_conf_t* conf, image_t* image, const char* budget_dump_filename)
{
	int i, column;
	tco_enc_context_t* ctx;

	if (tco_conf_validate(conf, image, conf))
		return NULL;


	ctx = (tco_enc_context_t*) malloc(sizeof(tco_enc_context_t));

	if (ctx)
	{
		memset(ctx, 0, sizeof(tco_enc_context_t));



		if (1 << conf->ndecomp_h > image->w[0]) {
			int maxdecomp = FLOOR_LOG2(image->w[0]);
			fprintf(stderr, "Warning: reducing ndecomp_h from %d down to %d\n", conf->ndecomp_h, maxdecomp);
			conf->ndecomp_h = maxdecomp;
		}


		ctx->ncomps = image->ncomps;
		ctx->w = image->w[0];
		ctx->h = image->h[0];
		ctx->is_422 = image->is_422;
		ctx->depth = image->depth;

		fill_lvls_map(conf, image, &ctx->map);

		for (column = 0; column < num_cols(&ctx->map); column++)
			ctx->rc[column] = rate_control_open(conf, &(ctx->map), image->h[0], budget_dump_filename, column);


		ctx->conf = *conf;

		for (i=0; i<2; i++)
		{
			ctx->image_tmp[i] =	tco_clone_image(image, 0);
		}
		ctx->image_transformed = NULL;

		ctx->dwt53_tmp_data = (dwt_data_t*) malloc( MAX((ctx->w+1)/2, (ctx->h+1)/2) * sizeof(dwt_data_t));

		for (column = 0; column < num_cols(&ctx->map); column++)
			ctx->precinct[column] = precinct_open_column(&(ctx->map), conf->group_size, column);

		ctx->packer = packer_open(conf, ctx->precinct[0]);
		ctx->bitstream = bitpacker_open2();
		ctx->bitstream_len = 0;
		ctx->precinct_tmp[0] = ctx->precinct_tmp[1] = NULL;
	}
	return ctx;
}

int tco_enc_close(tco_enc_context_t* ctx)
{
	int i = 0;
	if (ctx)
	{
		if (ctx->packer)
			packer_close(ctx->packer);

		for (i=0; i<2; i++)
			if (ctx->image_tmp[i])
			{
				tco_free_image(ctx->image_tmp[i]);
				free(ctx->image_tmp[i]);
			}
		for (i = 0; i < MAX_PREC_COLS; i++)
			if (ctx->precinct[i] != NULL)
				precinct_close(ctx->precinct[i]);


		if (ctx->dwt53_tmp_data)
			free(ctx->dwt53_tmp_data);
		bitpacker_close(ctx->bitstream);

		for (i = 0; i < MAX_PREC_COLS; i++)
			if (ctx->rc[i] != NULL)
				rate_control_close(ctx->rc[i]);
		for (i=0;i<2;i++)
			if (ctx->precinct_tmp[i])
				precinct_close(ctx->precinct_tmp[i]);
		free(ctx);
	}
	return 0;
}

int tco_enc_test_input_image(tco_enc_context_t* ctx, image_t* image_in)
{
	if ((image_in->is_422 != ctx->is_422) ||
		(image_in->w[0] != ctx->w) ||
		(image_in->ncomps != ctx->ncomps) ||
		(image_in->depth != ctx->depth))
	{
		fprintf(stderr, "Error: input image geometry does not match encoding context\n");
		return -1;
	}
	return 0;
}


//#define TCO_VERBOSE

int tco_enc_transforms( tco_enc_context_t* ctx, image_t* image_in )
{
    tco_conf_t* conf = &(ctx->conf);
    int comp;
    int apply_rct = (((image_in->ncomps == 3) || (image_in->ncomps == 4)) && conf->rct && (! image_in->is_422));
    int im_idx = 0;
    int decomp_kernels_h[MAX_NCOMPS];
    int decomp_kernels_v[MAX_NCOMPS];

	#ifdef TCO_VERBOSE
		fprintf(stderr, "transformed images dumped into enc%%d.raw files!\n");
		fprintf(stderr, "Use : \"gm convert -size %dx%d -depth 8 rgb:enc%%d.raw out.png\" to convert to png\n", image_in->w[0],image_in->h[0]);
		tco_image_dump(image_in, "enc0.raw");
	#endif


	tco_image_shift_all(image_in, ctx->image_tmp[im_idx], (conf->in_depth - ctx->depth), 0);
	#ifdef TCO_VERBOSE
		tco_image_dump(ctx->image_tmp[im_idx], "enc1.raw");
	#endif


	tco_image_apply_offset(ctx->image_tmp[im_idx], ctx->image_tmp[im_idx], -(1<<(conf->in_depth-1)));
	#ifdef TCO_VERBOSE
		tco_image_dump(ctx->image_tmp[im_idx], "enc2.raw");
	#endif


	if (apply_rct)
	{
		tco_image_apply_rct(ctx->image_tmp[im_idx], ctx->image_tmp[im_idx], conf->rct_type);
	}
	else
		conf->rct = 0;

	#ifdef TCO_VERBOSE
		tco_image_dump(ctx->image_tmp[im_idx], "enc3.raw");
	#endif

	#ifdef DUMP_VECTORS
		image_subbands_dump(ctx->image_tmp[im_idx], 0, 0, "dwt_in");
	#endif


	for (comp = 0; comp < image_in->ncomps; comp++)
	{
		if (comp == conf->skip_wavelet_on_comp)
		{
			decomp_kernels_h[comp] = HOR_KERNEL_SKIP;
			decomp_kernels_v[comp] = VER_KERNEL_SKIP;
		}
		else
		{
			decomp_kernels_h[comp] = conf->kernel_h;
			decomp_kernels_v[comp] = conf->kernel_v;
		}
	}

	{
		int vlvl;

		for(vlvl = 0;vlvl < conf->ndecomp_v;vlvl++)
		{


			tco_image_apply_partial_dwt_ver(ctx->image_tmp[im_idx], ctx->image_tmp[!im_idx], decomp_kernels_v,vlvl);
			im_idx = !im_idx;
#ifdef DUMP_VECTORS
			char filename[128];
			sprintf(filename,"dwt_v%d",vlvl);
			image_subbands_dump(ctx->image_tmp[im_idx], vlvl, vlvl+1, filename);
#endif

			tco_image_apply_partial_dwt_hor(ctx->image_tmp[im_idx], ctx->image_tmp[!im_idx],
							1, decomp_kernels_h,
							ctx->dwt53_tmp_data, vlvl);
			im_idx = !im_idx;

#ifdef TCO_VERBOSE
			if (vlvl == 0)
				tco_image_dump(ctx->image_tmp[im_idx], "enc40.raw");
			else
				tco_image_dump(ctx->image_tmp[im_idx], "enc41.raw");
#endif

#ifdef DUMP_VECTORS
			sprintf(filename,"dwt_vh%d",vlvl);
			image_subbands_dump(ctx->image_tmp[im_idx], vlvl+1, vlvl+1, filename);
#endif

		}
		assert(ctx->map.n_lvls_h[0] - 1 >= vlvl);



		if (ctx->map.n_lvls_h[0] - 1 > vlvl)
		{
			tco_image_apply_partial_dwt_hor(ctx->image_tmp[im_idx], ctx->image_tmp[!im_idx],
							ctx->map.n_lvls_h[0] - 1 - vlvl, decomp_kernels_h,
							ctx->dwt53_tmp_data, vlvl);
			im_idx = !im_idx;
		}
#ifdef DUMP_VECTORS
		image_subbands_dump(ctx->image_tmp[im_idx], ctx->map.n_lvls_h[0], vlvl, "dwt_out");
#endif
	}

#ifdef TCO_VERBOSE
	//image_subbands_dump(ctx->image_tmp[im_idx], 5, 2, "dwt_full");
	tco_image_dump(ctx->image_tmp[im_idx], "enc5.raw");
#endif


	tco_image_shift_all(ctx->image_tmp[im_idx], ctx->image_tmp[im_idx],-ctx->conf.quant, 1);

	#ifdef TCO_VERBOSE
		tco_image_dump(ctx->image_tmp[im_idx], "enc6.raw");
	#endif

	ctx->image_transformed = ctx->image_tmp[im_idx];

	return 0;
}


int tco_enc_image( tco_enc_context_t* ctx, image_t* image_in, void* codestream_buf, int codestream_buf_byte_size, int* codestream_byte_size)
{
	tco_conf_t* conf = &(ctx->conf);
	int out_len;
	int line_idx;
	rc_results_t rc_results;
	int header_len = 0;
	int slice_idx = 0;
	int rate_control_nbytes;
	int markers_len = 0;
	int column;
	int nbytes_column;
	int report_column;
	int nbytes_last_column;
	int report_last_column;

	if (tco_enc_test_input_image(ctx,image_in) < 0)
		return -1;


	if ((codestream_buf_byte_size / 8) * 8 < conf->bitstream_nbytes)
	{
		fprintf(stderr, "Error: codestream_buf is too small. Please allocate at least %d bytes\n", ((conf->bitstream_nbytes + 7) / 8 * 8));
		return -1;
	}
	bitpacker_set_buffer(ctx->bitstream , codestream_buf,codestream_buf_byte_size);


	header_len = xs_write_head(ctx->bitstream, image_in, conf);
	if (conf->verbose)
	{
		fprintf(stderr, "HEADER_LEN %d\n", bitpacker_get_len(ctx->bitstream));
	}


	rate_control_nbytes = conf->bitstream_nbytes - (header_len/8) - XS_MARKER_NBYTES - (XS_SLICE_HEADER_NBYTES * ((image_in->h[0]+conf->slice_height-1)/conf->slice_height));
	rate_control_compute_column_rate(num_cols(&ctx->map),ctx->map.col_frac, rate_control_nbytes, conf->budget_report_nbytes, &nbytes_column, &report_column, &nbytes_last_column, &report_last_column);

	for (column = 0; column < num_cols(&ctx->map); column++)
	{
		if (column != num_cols(&ctx->map)-1)
			rate_control_init(ctx->rc[column], nbytes_column, report_column);
		else
			rate_control_init(ctx->rc[column], nbytes_last_column, report_last_column);
	}


	if (tco_enc_transforms(ctx, image_in) < 0)
		return -1;

	for (line_idx=0; line_idx < image_in->h[0]; line_idx+=(1 << (ctx->map.n_lvls_v-1)))
	{
		for (column = 0; column < num_cols(&ctx->map); column++)
		{
			fill_precinct(ctx->precinct[column],ctx->image_transformed, line_idx >> (ctx->map.n_lvls_v - 1));
			update_gclis(ctx->precinct[column]);

			if (rate_control_process_presinct(ctx->rc[column], ctx->precinct[column], &rc_results) < 0) {
				return -1;
			}

			quantize_precinct(ctx->precinct[column],rc_results.gtli_table_data,ctx->conf.dq_type);

			if (precinct_is_first_of_slice(ctx->precinct[column], conf->slice_height) && (column == 0))
			{
				if (conf->verbose)
					fprintf(stderr, "writing slice header %d\n", slice_idx);
				markers_len+=xs_write_slice_header(ctx->bitstream, slice_idx++);
			}


			if (conf->verbose)
			{
				int cbr = budget_getcbr(conf->bitstream_nbytes*2, precinct_idx_of(ctx->precinct[column]) + 1, image_in->h[0] / (1 << (ctx->map.n_lvls_v-1)) );
				int report = cbr - (rc_results.bits_consumed >> 2);
				int lvl;
				fprintf(stderr, "%d: sc=%d ref=%d prec=%d pad=%d (%d/%d) report=%d ",
								line_idx,
								rc_results.scenario,
								rc_results.refinement,
								(rc_results.pbinfo.precinct_bits >> 2),
								(rc_results.padding_bits >> 2),
								(rc_results.bits_consumed >> 2),
								conf->bitstream_nbytes*2,
								report);
				for (lvl = 0; lvl < ctx->map.level_count; lvl++)
				{
					fprintf(stderr, "lvl%d ", lvl);
					print_method(stderr,rc_results.gcli_sb_methods[lvl]);
				}
				fprintf(stderr, "\n");
			}

			if (pack_precinct(ctx->packer, ctx->bitstream, ctx->precinct[column], &rc_results) < 0)
				return -1;

			if (num_cols(&ctx->map) == 1)
			{
				assert(bitpacker_get_len(ctx->bitstream) - header_len - markers_len == (rc_results.bits_consumed));
			}

			if (rc_results.rc_error == 1)
				break;
		}
	}

	xs_write_tail(ctx->bitstream);
	assert(bitpacker_get_len(ctx->bitstream)/8 == conf->bitstream_nbytes);


	out_len = ((bitpacker_get_len(ctx->bitstream) + 7) / 8);
	bitpacker_flush(ctx->bitstream);
	*codestream_byte_size = out_len;
	return 0;
}
