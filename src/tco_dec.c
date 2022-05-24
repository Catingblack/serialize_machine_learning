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

#include "xs_markers.h"
#include "common.h"
#include "precinct.h"
#include "tco_conf.h"
#include "buf_mgmt.h"
#include "dwt53.h"
#include "image_transforms.h"
#include "bitpacking.h"
#include "gcli.h"
#include "sig_mag.h"
#include "budget.h"
#include "ra.h"
#include "packing.h"
#include "quant.h"
#include "slice.h"
#include "lvls_map.h"


#ifdef DUMP_INTERMEDIATE_DATA
#include "../programs/image_open.h"
#endif


struct tco_dec_context_t
{




	tco_conf_t conf;
	lvls_map_t map;

	precinct_t* precincts[2][MAX_PREC_COLS];
	precinct_t* precinct_top[MAX_PREC_COLS];
	int	gtlis_table_top[MAX_PREC_COLS][MAX_LVLS];
	precinct_t* precinct[MAX_PREC_COLS];

	bit_unpacker_t* bitstream;
	unpacking_context_t* unpack_ctx;
	image_t* image_tmp[2];
	int w[MAX_NCOMPS];
	int h;
};

int tco_dec_probe(uint8_t* bitstream_buf, int len, tco_conf_t* conf, image_t* image)
{
	bit_unpacker_t* bitstream = bitunpacker_open(bitstream_buf, len);
	int nbits = xs_parse_head(bitstream, image, conf);
	bitunpacker_close(bitstream);
	return (!(nbits>0));
}





tco_dec_context_t* tco_dec_init(tco_conf_t* conf, image_t* image)
{
	int i;
	tco_dec_context_t* ctx;

	if (tco_conf_validate(conf, image, conf))
		return NULL;

	ctx = (tco_dec_context_t*) malloc(sizeof(tco_dec_context_t));

	if (ctx)
	{

		memset(ctx, 0, sizeof(tco_dec_context_t));
		memcpy(&ctx->conf, conf, sizeof(tco_conf_t));

		fill_lvls_map(conf, image, &ctx->map);

		ctx->h		  = image->h[0];
		for (i=0; i<MAX_NCOMPS; i++)
			ctx->w[i] = image->w[i];

		for (i=0; i<2; i++)
		{
			ctx->image_tmp[i] = tco_clone_image(image, 0);
			for (int column = 0; column < num_cols(&ctx->map); column++)
			{
				ctx->precincts[i][column] = precinct_open_column(&ctx->map, conf->group_size, column);
			}
		}
		for (int column = 0; column < num_cols(&ctx->map); column++)
		{
			ctx->precinct[column] = ctx->precincts[0][column];
			ctx->precinct_top[column] = ctx->precincts[1][column];
		}
 		ctx->unpack_ctx = unpacker_open(conf, &ctx->map, ctx->precinct[0]);
		ctx->bitstream = bitunpacker_open2();
	}
	return ctx;
}

int tco_dec_set_fragment_csv_cbr_path(tco_dec_context_t* ctx, char* csv_buffer_model_cbr)
{
	unpacker_set_csv_buffer_model_cbr(ctx->unpack_ctx,csv_buffer_model_cbr);
	return 0;
}

int tco_dec_set_fragment_csv_vbr_path(tco_dec_context_t* ctx, char* csv_buffer_model_vbr)
{
	unpacker_set_csv_buffer_model_vbr(ctx->unpack_ctx,csv_buffer_model_vbr);
	return 0;
}

int tco_dec_close(tco_dec_context_t* ctx)
{
	int i;
	for (i=0; i<2; i++)
		if (ctx->image_tmp[i])
		{
			tco_free_image(ctx->image_tmp[i]);
			free(ctx->image_tmp[i]);
			for (int column = 0; column < num_cols(&ctx->map); column++)
				precinct_close(ctx->precincts[i][column]);
		}
	unpacker_close(ctx->unpack_ctx);
	bitunpacker_close(ctx->bitstream);
	free(ctx);
	return 0;
}

//#define TCO_VERBOSE


int tco_dec_transforms(tco_dec_context_t* ctx, image_t* image_out)
{
	tco_conf_t* conf = &(ctx->conf);
	int im_idx = 0;
	int comp;
    int decomp_kernels_h[MAX_NCOMPS];
    int decomp_kernels_v[MAX_NCOMPS];

	#ifdef TCO_VERBOSE
		fprintf(stderr, "transformed images dumped into file dec%%d.raw files!\n");
		fprintf(stderr, "Use : \"gm convert -size %dx%d -depth 8 rgb:dec%%d.raw out.png\" to convert to png\n", ctx->image_tmp[im_idx]->w[0],ctx->image_tmp[im_idx]->h[0]);
		tco_image_dump(ctx->image_tmp[im_idx], "dec0.raw");
	#endif


	tco_image_shift_all(ctx->image_tmp[im_idx], ctx->image_tmp[im_idx],ctx->conf.quant, 0);

#ifdef TCO_VERBOSE
	tco_image_dump(ctx->image_tmp[im_idx], "dec1.raw");
#endif

	for (comp = 0; comp < image_out->ncomps; comp++)
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
		int vlvl = conf->ndecomp_v;
		assert(ctx->map.n_lvls_h[0] - 1 >= vlvl);

		if (ctx->map.n_lvls_h[0] - 1 > vlvl)
		{
			tco_image_apply_partial_idwt_hor(ctx->image_tmp[im_idx], ctx->image_tmp[!im_idx],
							 ctx->map.n_lvls_h[0] - 1 - vlvl, decomp_kernels_h,
							 vlvl);
			im_idx = !im_idx;
		}

		while(vlvl > 0)
		{
			vlvl--;
			tco_image_apply_partial_idwt_hor(ctx->image_tmp[im_idx], ctx->image_tmp[!im_idx],
							 1, decomp_kernels_h,
							 vlvl);
			im_idx = !im_idx;
			tco_image_apply_partial_idwt_ver(ctx->image_tmp[im_idx], ctx->image_tmp[!im_idx], decomp_kernels_v,vlvl);
			im_idx = !im_idx;
		}
	}

	assert(conf->haar_use_shift == 0);

	#ifdef TCO_VERBOSE
		tco_image_dump(ctx->image_tmp[im_idx], "dec3.raw");
	#endif

    if (((image_out->ncomps == 3) || (image_out->ncomps == 4)) && (!image_out->is_422) && (conf->rct))
	{
		tco_image_apply_irct(ctx->image_tmp[im_idx], ctx->image_tmp[im_idx], conf->rct_type);
	}

	#ifdef TCO_VERBOSE
		tco_image_dump(ctx->image_tmp[im_idx], "dec4.raw");
	#endif


	tco_image_apply_offset(ctx->image_tmp[im_idx], ctx->image_tmp[im_idx], (1<<(conf->in_depth-1)));

	#ifdef TCO_VERBOSE
		tco_image_dump(ctx->image_tmp[im_idx], "dec5.raw");
	#endif


	tco_image_shift_all(ctx->image_tmp[im_idx], ctx->image_tmp[im_idx], (image_out->depth - conf->in_depth), 1);


	tco_image_clamp(ctx->image_tmp[im_idx], ctx->image_tmp[im_idx], 0, (1<<image_out->depth) - 1);



	tco_image_copy_data(ctx->image_tmp[im_idx], image_out);

	#ifdef TCO_VERBOSE
		tco_image_dump(ctx->image_tmp[im_idx], "dec6.raw");
	#endif
	return 0;
}




void swap_precincts_ptr(precinct_t** prec1 , precinct_t** prec2)
{
	precinct_t* tmp;
	tmp = *prec1;
	*prec1 = *prec2;
	*prec2 = tmp;
}


int tco_dec_bitstream(tco_dec_context_t* ctx, void* bitstream_buf, int bitstream_buf_size, image_t* image_out)
{
	tco_conf_t* conf = &(ctx->conf);
	int line_idx;
	int precinct_h = 1 << (ctx->map.n_lvls_v - 1);
	tco_conf_t conf_hdr;
	image_t image_hdr;
	int  header_len=0;
	int slice_idx = 0, column = 0;

	bitunpacker_set_buffer(ctx->bitstream, bitstream_buf, bitstream_buf_size);


	header_len = xs_parse_head(ctx->bitstream, &image_hdr, &conf_hdr);
	#ifdef TCO_VERBOSE
	fprintf(stderr, "HEADER_LEN (read=%d returned=%d)\n", header_len, (int) bitunpacker_consumed_bits(ctx->bitstream));
	#endif
	assert(header_len == bitunpacker_consumed_bits(ctx->bitstream));




	for (line_idx=0; line_idx < ctx->h; line_idx += precinct_h)
	{
		unpacked_info_t unpack_out;
		int prec_idx = (line_idx / precinct_h);
		int first_of_slice;
		int last_of_slice;
		for (column = 0; column < num_cols(&ctx->map); column++)
		{
			precinct_set_idx_of(ctx->precinct[column], prec_idx);
			first_of_slice = precinct_is_first_of_slice(ctx->precinct[column], conf->slice_height);
			last_of_slice = precinct_is_last_of_slice(ctx->precinct[column], conf->slice_height, ctx->h);

			if (first_of_slice && column == 0)
			{

				int slice_idx_check;
				xs_parse_slice_header(ctx->bitstream, &slice_idx_check);
				assert(slice_idx_check == (slice_idx++));
				#ifdef TCO_VERBOSE
				fprintf(stderr, "Read Slice Header (slice_idx=%d)\n", slice_idx_check);
				#endif
			}

			if (unpack_precinct(ctx->unpack_ctx, ctx->bitstream, ctx->precinct[column],
							   (!first_of_slice) ? ctx->precinct_top[column] : NULL, ctx->gtlis_table_top[column],
							   &unpack_out) < 0)
			{
				fprintf(stderr, "Corrupted codestream! line number %d\n", line_idx);
				return -1;
			}

			dequantize_precinct(ctx->precinct[column],unpack_out.gtli_table_data,
					    ctx->conf.dq_type);


			if (ctx->conf.use_slice_footer && last_of_slice)
			{
				bitunpacker_skip(ctx->bitstream, SLICE_FOOTER_LEN);
#ifdef TCO_VERBOSE
				fprintf(stderr, "%d: slice footer (%d)\n", line_idx, (int) (bitunpacker_consumed_bits(ctx->bitstream)-header_len)>>2);
#endif
			}


			update_image(ctx->precinct[column], ctx->image_tmp[0], line_idx >> (ctx->map.n_lvls_v - 1));

			swap_precincts_ptr(&ctx->precinct_top[column], &ctx->precinct[column]);
			memcpy(ctx->gtlis_table_top[column], unpack_out.gtli_table_gcli, MAX_LVLS * sizeof(int));
		}
	}

#ifdef DUMP_INTERMEDIATE_DATA
	if (image_save_auto("InvWT.rgb16", ctx->image_tmp[0]) < 0) {
		fprintf(stderr, "Could not write debug output image");
	}
#endif

	if (tco_dec_transforms(ctx, image_out) < 0)
		return -1;

	xs_parse_tail(ctx->bitstream);
	if (conf->bitstream_nbytes)
		assert(bitunpacker_consumed_bits(ctx->bitstream)/8 == conf->bitstream_nbytes);
	return 0;
}


#ifdef TEST_TCO_DEC

int main()
{
	tco_data_in_t red[]={
		182, 182, 182, 181, 181, 181, 181, 181, 181, 182, 182, 183, 184, 185, 185, 186,
		186, 188, 188, 188, 188, 188, 188, 188, 189, 189, 189, 191, 191, 191, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 191, 191, 191, 191, 190, 190, 189, 189, 189,
		189, 189, 189, 189, 188, 188, 187, 187, 186, 186, 186, 186, 185, 185, 184, 184,
		183, 183, 183, 181, 181, 182, 182, 182, 182, 181, 180, 180, 180, 180, 180, 179,
		179, 178, 177, 177, 176, 176, 176, 175, 174, 174, 174, 173, 173, 173, 173, 172,
		172, 171, 171, 172, 172, 170, 169, 169, 169, 168, 167, 166, 166, 164, 164, 163,
		162, 161, 162, 162, 161, 160, 159, 159, 157, 156, 156, 154, 154, 153, 152, 151,
		150, 149, 148, 147, 147, 145, 145, 145, 145, 146, 147, 147, 148, 149, 149, 150,
		149, 149, 149, 149, 149, 149, 148, 147, 145, 143, 143, 141, 139, 138, 137, 138,
		136, 136, 135, 133, 133, 132, 132, 131, 130, 130, 129, 128, 128, 127, 128, 127,
		127, 126, 126, 126, 126, 125, 125, 125, 125, 125, 125, 123, 123, 123, 122, 120,
		120, 118, 117, 115, 114, 111, 109, 107, 182, 182, 182, 181, 181, 181, 180, 181,
		181, 181, 181, 183, 184, 185, 186, 187, 188, 189, 189, 190, 190, 190, 190, 190,
		191, 191, 191, 192, 193, 192, 194, 195, 196, 196, 196, 196, 195, 195, 195, 195,
		195, 194, 194, 194, 193, 193, 192, 192, 192, 192, 193, 193, 193, 192, 193, 192,
		191, 191, 191, 190, 190, 189, 189, 188, 188, 188, 188, 187, 187, 186, 186, 186,
		185, 185, 184, 184, 184, 184, 183, 182, 182, 182, 182, 181, 182, 182, 182, 181,
		180, 180, 179, 179, 179, 179, 178, 178, 178, 177, 177, 178, 177, 176, 175, 175,
		174, 174, 174, 172, 172, 171, 171, 169, 168, 168, 166, 166, 165, 165, 164, 163,
		163, 163, 161, 160, 159, 158, 157, 156, 155, 154, 152, 153, 152, 152, 151, 151,
		152, 152, 152, 153, 153, 154, 155, 154, 156, 156, 156, 156, 156, 155, 156, 153,
		150, 147, 147, 146, 145, 142, 141, 140, 139, 138, 139, 137, 136, 136, 135, 136,
		134, 134, 134, 132, 132, 131, 131, 131, 130, 129, 129, 130, 130, 130, 130, 130,
		129, 129, 129, 128, 128, 128, 128, 127, 126, 125, 124, 121, 119, 118, 116, 114,
		198, 198, 198, 198, 198, 198, 198, 199, 199, 199, 200, 202, 203, 204, 205, 206,
		206, 207, 208, 208, 208, 209, 209, 209, 210, 210, 211, 211, 211, 213, 213, 213,
		214, 213, 214, 214, 214, 213, 213, 212, 213, 213, 212, 212, 212, 211, 211, 211,
		211, 211, 211, 211, 210, 211, 211, 210, 210, 210, 209, 209, 208, 207, 207, 206,
		206, 205, 205, 205, 204, 205, 204, 204, 204, 204, 204, 203, 203, 202, 202, 202,
		202, 201, 201, 201, 199, 199, 199, 198, 198, 197, 197, 197, 197, 197, 197, 197,
		197, 196, 196, 195, 195, 194, 194, 194, 193, 193, 192, 191, 191, 190, 189, 188,
		187, 186, 186, 186, 185, 185, 184, 184, 183, 182, 182, 181, 180, 179, 177, 176,
		176, 174, 173, 173, 172, 172, 172, 172, 172, 172, 173, 175, 175, 176, 177, 178,
		178, 179, 178, 178, 179, 178, 177, 177, 175, 172, 172, 171, 169, 167, 166, 165,
		165, 164, 163, 162, 162, 160, 159, 159, 158, 157, 156, 156, 155, 155, 155, 153,
		153, 153, 153, 154, 154, 153, 153, 152, 152, 152, 152, 151, 151, 151, 149, 148,
		148, 145, 144, 142, 140, 139, 136, 134, 195, 195, 195, 195, 195, 195, 195, 196,
		197, 198, 199, 200, 203, 204, 205, 206, 207, 209, 209, 210, 210, 210, 211, 212,
		212, 212, 213, 213, 214, 214, 215, 216, 217, 217, 217, 217, 217, 217, 217, 217,
		216, 216, 215, 215, 215, 215, 215, 215, 215, 215, 215, 214, 214, 214, 214, 214,
		213, 213, 212, 212, 212, 212, 211, 211, 211, 210, 210, 210, 209, 208, 208, 207,
		207, 207, 206, 206, 205, 205, 205, 204, 204, 204, 204, 204, 205, 205, 205, 203,
		203, 203, 203, 203, 202, 202, 202, 201, 201, 201, 201, 201, 200, 199, 199, 199,
		198, 197, 196, 195, 194, 194, 193, 192, 191, 191, 191, 190, 190, 190, 189, 189,
		188, 187, 186, 185, 185, 183, 182, 182, 180, 179, 179, 179, 178, 178, 177, 177,
		177, 178, 179, 179, 180, 182, 182, 182, 184, 185, 185, 185, 185, 185, 185, 182,
		180, 177, 176, 175, 173, 171, 170, 170, 168, 167, 167, 166, 166, 164, 163, 163,
		162, 162, 161, 160, 160, 160, 159, 159, 159, 158, 158, 157, 157, 157, 157, 157,
		157, 157, 157, 157, 156, 155, 155, 154, 153, 151, 151, 148, 147, 145, 143, 141,
	};
	tco_data_in_t green[]={
		198, 198, 198, 198, 198, 198, 198, 199, 199, 199, 200, 202, 203, 204, 205, 206,
		206, 207, 208, 208, 208, 209, 209, 209, 210, 210, 211, 211, 211, 213, 213, 213,
		214, 213, 214, 214, 214, 213, 213, 212, 213, 213, 212, 212, 212, 211, 211, 211,
		211, 211, 211, 211, 210, 211, 211, 210, 210, 210, 209, 209, 208, 207, 207, 206,
		206, 205, 205, 205, 204, 205, 204, 204, 204, 204, 204, 203, 203, 202, 202, 202,
		202, 201, 201, 201, 199, 199, 199, 198, 198, 197, 197, 197, 197, 197, 197, 197,
		197, 196, 196, 195, 195, 194, 194, 194, 193, 193, 192, 191, 191, 190, 189, 188,
		187, 186, 186, 186, 185, 185, 184, 184, 183, 182, 182, 181, 180, 179, 177, 176,
		176, 174, 173, 173, 172, 172, 172, 172, 172, 172, 173, 175, 175, 176, 177, 178,
		178, 179, 178, 178, 179, 178, 177, 177, 175, 172, 172, 171, 169, 167, 166, 165,
		165, 164, 163, 162, 162, 160, 159, 159, 158, 157, 156, 156, 155, 155, 155, 153,
		153, 153, 153, 154, 154, 153, 153, 152, 152, 152, 152, 151, 151, 151, 149, 148,
		148, 145, 144, 142, 140, 139, 136, 134, 195, 195, 195, 195, 195, 195, 195, 196,
		197, 198, 199, 200, 203, 204, 205, 206, 207, 209, 209, 210, 210, 210, 211, 212,
		212, 212, 213, 213, 214, 214, 215, 216, 217, 217, 217, 217, 217, 217, 217, 217,
		216, 216, 215, 215, 215, 215, 215, 215, 215, 215, 215, 214, 214, 214, 214, 214,
		213, 213, 212, 212, 212, 212, 211, 211, 211, 210, 210, 210, 209, 208, 208, 207,
		207, 207, 206, 206, 205, 205, 205, 204, 204, 204, 204, 204, 205, 205, 205, 203,
		203, 203, 203, 203, 202, 202, 202, 201, 201, 201, 201, 201, 200, 199, 199, 199,
		198, 197, 196, 195, 194, 194, 193, 192, 191, 191, 191, 190, 190, 190, 189, 189,
		188, 187, 186, 185, 185, 183, 182, 182, 180, 179, 179, 179, 178, 178, 177, 177,
		177, 178, 179, 179, 180, 182, 182, 182, 184, 185, 185, 185, 185, 185, 185, 182,
		180, 177, 176, 175, 173, 171, 170, 170, 168, 167, 167, 166, 166, 164, 163, 163,
		162, 162, 161, 160, 160, 160, 159, 159, 159, 158, 158, 157, 157, 157, 157, 157,
		157, 157, 157, 157, 156, 155, 155, 154, 153, 151, 151, 148, 147, 145, 143, 141,
		214, 214, 214, 214, 214, 214, 215, 216, 217, 219, 220, 221, 222, 223, 224, 225,
		227, 228, 228, 229, 229, 229, 230, 230, 231, 232, 232, 233, 233, 234, 234, 235,
		236, 236, 236, 236, 236, 236, 235, 235, 235, 235, 235, 235, 234, 234, 234, 233,
		234, 234, 234, 234, 235, 235, 233, 233, 232, 232, 232, 231, 231, 230, 229, 229,
		229, 228, 227, 227, 227, 227, 227, 227, 227, 227, 227, 226, 226, 226, 226, 226,
		225, 225, 225, 225, 223, 223, 222, 222, 221, 221, 221, 221, 220, 220, 220, 221,
		221, 220, 220, 220, 220, 220, 219, 218, 217, 216, 216, 215, 214, 213, 213, 213,
		212, 212, 213, 212, 211, 211, 211, 210, 210, 208, 208, 207, 206, 205, 204, 204,
		202, 201, 200, 200, 199, 198, 199, 198, 199, 200, 200, 201, 203, 205, 206, 206,
		206, 207, 208, 208, 208, 207, 207, 206, 204, 203, 202, 200, 198, 196, 195, 193,
		193, 192, 191, 190, 190, 189, 188, 186, 186, 185, 184, 184, 183, 183, 182, 181,
		181, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 178, 177, 177, 176, 175,
		174, 173, 171, 170, 168, 167, 164, 162, 210, 210, 210, 211, 211, 211, 211, 213,
		214, 215, 217, 219, 221, 222, 223, 225, 227, 228, 229, 229, 230, 230, 231, 232,
		232, 233, 233, 234, 236, 235, 236, 237, 237, 237, 237, 237, 237, 237, 237, 237,
		237, 237, 237, 237, 236, 236, 236, 235, 235, 236, 236, 236, 236, 236, 236, 236,
		236, 235, 235, 235, 235, 233, 233, 233, 233, 232, 232, 232, 230, 230, 229, 229,
		229, 229, 228, 228, 228, 228, 228, 227, 227, 227, 227, 227, 228, 228, 228, 227,
		226, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 224, 224, 224, 223, 223,
		222, 220, 220, 219, 218, 217, 217, 217, 216, 216, 215, 214, 214, 214, 214, 214,
		213, 212, 212, 211, 211, 210, 209, 207, 207, 205, 204, 204, 204, 204, 203, 204,
		204, 204, 205, 206, 206, 208, 209, 211, 212, 213, 213, 213, 213, 214, 213, 210,
		208, 207, 206, 203, 203, 201, 200, 199, 198, 197, 195, 195, 194, 193, 193, 192,
		191, 190, 190, 188, 188, 187, 187, 187, 186, 186, 186, 186, 186, 186, 185, 185,
		185, 185, 185, 184, 184, 183, 183, 182, 181, 180, 179, 177, 175, 174, 172, 169,
	};
	tco_data_in_t blue[]={
		214, 214, 214, 214, 214, 214, 215, 216, 217, 219, 220, 221, 222, 223, 224, 225,
		227, 228, 228, 229, 229, 229, 230, 230, 231, 232, 232, 233, 233, 234, 234, 235,
		236, 236, 236, 236, 236, 236, 235, 235, 235, 235, 235, 235, 234, 234, 234, 233,
		234, 234, 234, 234, 235, 235, 233, 233, 232, 232, 232, 231, 231, 230, 229, 229,
		229, 228, 227, 227, 227, 227, 227, 227, 227, 227, 227, 226, 226, 226, 226, 226,
		225, 225, 225, 225, 223, 223, 222, 222, 221, 221, 221, 221, 220, 220, 220, 221,
		221, 220, 220, 220, 220, 220, 219, 218, 217, 216, 216, 215, 214, 213, 213, 213,
		212, 212, 213, 212, 211, 211, 211, 210, 210, 208, 208, 207, 206, 205, 204, 204,
		202, 201, 200, 200, 199, 198, 199, 198, 199, 200, 200, 201, 203, 205, 206, 206,
		206, 207, 208, 208, 208, 207, 207, 206, 204, 203, 202, 200, 198, 196, 195, 193,
		193, 192, 191, 190, 190, 189, 188, 186, 186, 185, 184, 184, 183, 183, 182, 181,
		181, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 178, 177, 177, 176, 175,
		174, 173, 171, 170, 168, 167, 164, 162, 210, 210, 210, 211, 211, 211, 211, 213,
		214, 215, 217, 219, 221, 222, 223, 225, 227, 228, 229, 229, 230, 230, 231, 232,
		232, 233, 233, 234, 236, 235, 236, 237, 237, 237, 237, 237, 237, 237, 237, 237,
		237, 237, 237, 237, 236, 236, 236, 235, 235, 236, 236, 236, 236, 236, 236, 236,
		236, 235, 235, 235, 235, 233, 233, 233, 233, 232, 232, 232, 230, 230, 229, 229,
		229, 229, 228, 228, 228, 228, 228, 227, 227, 227, 227, 227, 228, 228, 228, 227,
		226, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 224, 224, 224, 223, 223,
		222, 220, 220, 219, 218, 217, 217, 217, 216, 216, 215, 214, 214, 214, 214, 214,
		213, 212, 212, 211, 211, 210, 209, 207, 207, 205, 204, 204, 204, 204, 203, 204,
		204, 204, 205, 206, 206, 208, 209, 211, 212, 213, 213, 213, 213, 214, 213, 210,
		208, 207, 206, 203, 203, 201, 200, 199, 198, 197, 195, 195, 194, 193, 193, 192,
		191, 190, 190, 188, 188, 187, 187, 187, 186, 186, 186, 186, 186, 186, 185, 185,
		185, 185, 185, 184, 184, 183, 183, 182, 181, 180, 179, 177, 175, 174, 172, 169,
		10, 10, 10, 214, 214, 214, 215, 216, 217, 219, 220, 221, 222, 223, 224, 225,
		227, 228, 228, 229, 229, 229, 230, 230, 231, 232, 232, 233, 233, 234, 234, 235,
		236, 236, 236, 236, 236, 236, 235, 235, 235, 235, 235, 235, 234, 234, 234, 233,
		234, 234, 234, 234, 235, 235, 233, 233, 232, 232, 232, 231, 231, 230, 229, 229,
		229, 228, 227, 227, 227, 227, 227, 227, 227, 227, 227, 226, 226, 226, 226, 226,
		225, 225, 225, 225, 223, 223, 222, 222, 221, 221, 221, 221, 220, 220, 220, 221,
		221, 220, 220, 220, 220, 220, 219, 218, 217, 216, 216, 215, 214, 213, 213, 213,
		212, 212, 213, 212, 211, 211, 211, 210, 210, 208, 208, 207, 206, 205, 204, 204,
		202, 201, 200, 200, 199, 198, 199, 198, 199, 200, 200, 201, 203, 205, 206, 206,
		206, 207, 208, 208, 208, 207, 207, 206, 204, 203, 202, 200, 198, 196, 195, 193,
		193, 192, 191, 190, 190, 189, 188, 186, 186, 185, 184, 184, 183, 183, 182, 181,
		181, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 178, 177, 177, 176, 175,
		174, 173, 171, 170, 168, 167, 164, 162, 210, 210, 210, 211, 211, 211, 211, 213,
		214, 215, 217, 219, 221, 222, 223, 225, 227, 228, 229, 229, 230, 230, 231, 232,
		232, 233, 233, 234, 236, 235, 236, 237, 237, 237, 237, 237, 237, 237, 237, 237,
		237, 237, 237, 237, 236, 236, 236, 235, 235, 236, 236, 236, 236, 236, 236, 236,
		236, 235, 235, 235, 235, 233, 233, 233, 233, 232, 232, 232, 230, 230, 229, 229,
		229, 229, 228, 228, 228, 228, 228, 227, 227, 227, 227, 227, 228, 228, 228, 227,
		226, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 224, 224, 224, 223, 223,
		222, 220, 220, 219, 218, 217, 217, 217, 216, 216, 215, 214, 214, 214, 214, 214,
		213, 212, 212, 211, 211, 210, 209, 207, 207, 205, 204, 204, 204, 204, 203, 204,
		204, 204, 205, 206, 206, 208, 209, 211, 212, 213, 213, 213, 213, 214, 213, 210,
		208, 207, 206, 203, 203, 201, 200, 199, 198, 197, 195, 195, 194, 193, 193, 192,
		191, 190, 190, 188, 188, 187, 187, 187, 186, 186, 186, 186, 186, 186, 185, 185,
		185, 185, 185, 184, 184, 183, 183, 182, 181, 180, 179, 177, 175, 174, 172, 169,
	};
	tco_data_in_t red_out[200*4], blue_out[200*4], green_out[200*4];
	image_t image;
	int len;
	void * bitstream_storage;
	tco_conf_t conf = {
		48,
		4,
		5,
		1,
		1,
		1,
		8,
		4,
		1,
		{4,4,3,3,2,2,3,3,2,2,1,1,  3,3,2,2,1,1,2,2,1,1,0,0,  3,3,2,2,1,1,2,2,1,1,0,0},
		{0,5,2,4,1,3,0,5,2,4,1,3,  0,5,2,4,1,3,0,5,2,4,1,3,  0,5,2,4,1,3,0,5,2,4,1,3},
	};
	int bistream_maxsize_byte;
	image.w = 200;
	image.h[0] = 4;
	image.ncomps = 3;
	image.is_422 = 0;
	image.depth = 8;
	image.comps_array[0] = red;
	image.comps_array[1] = green;
	image.comps_array[2] =  blue;
	bistream_maxsize_byte = ((image.w*image.h[0]*(int)conf.bpp+7)/8)*8;
	bitstream_storage = (void*) malloc(bistream_maxsize_byte);
	fprintf(stderr, "Starting Enc\n");
	tco_enc_image( &image, &conf, bitstream_storage, bistream_maxsize_byte, &len, NULL);

	image.comps_array[0] = red_out;
	image.comps_array[1] = green_out;
	image.comps_array[2] =  blue_out;
	memset(image.comps_array[0], 0, sizeof(red_out));
	memset(image.comps_array[1], 0, sizeof(red_out));
	memset(image.comps_array[2], 0, sizeof(red_out));
	tco_dec_bitstream(bitstream_storage, len, &image, &conf);
}




#endif


