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



#ifndef TCO_H
#define TCO_H

#include <stdint.h>
#include <math.h>

#if defined(_MSC_VER)
#define EXPORTED __declspec(dllexport)
#else
#define EXPORTED
#endif






typedef int32_t tco_data_in_t;


typedef char bool_t;
#define FALSE 0
#define TRUE 1

#define MAX_NDECOMP_H 5
#define MAX_NDECOMP_V 2
#define MAX_FILTER_TYPES 10
#define MAX_NCOMPS 4
#define DO_PADDING

#define MAX_PREC_COLS (130)



#define ORDER_GCVH	0
#define ORDER_VGCH	1
#define ORDER_CVHG	2


#define ORDER_VGRC	3
#define ORDER_RPBC  4


#define MAX_GROUP_SIZE 4

enum {
	TCO_PROFILE_0,
	TCO_PROFILE_1,
	TCO_PROFILE_2,
	TCO_PROFILE_JPEGXS_MAIN,
	TCO_PROFILE_JPEGXS_HIGH,
	TCO_PROFILE_JPEGXS_LIGHT_SUBLINE,
	TCO_PROFILE_JPEGXS_LIGHT,
};

enum {
	VER_KERNEL_53 = 0,
	VER_KERNEL_22 = 1,
	VER_KERNEL_137 = 2,
	VER_KERNEL_SKIP = 3,
	VER_KERNEL_NB = 4,
};

enum {
	HOR_KERNEL_53 = 0,
	HOR_KERNEL_22 = 1,
	HOR_KERNEL_137 = 2,
	HOR_KERNEL_SKIP = 3,
	HOR_KERNEL_NB = 4,
};

enum {
	SIGN_KEEP,
	SIGN_PACKING,
	SIGN_REUSE,
};

enum {
	GAINS_CHOICE_PSNR,
	GAINS_CHOICE_VISUAL,
	GAINS_CHOICE_MANUAL,
};



#define BPP2BYTESIZE(bpp, im_w, im_h) ((int)floor((bpp)*(im_w)*(im_h)/8))




#define TCO_CONF_DEFAULT_BITSTREAM_NBYTES		-1
#define TCO_CONF_DEFAULT_BPP					-1
#define TCO_CONF_DEFAULT_PROFILE				TCO_PROFILE_JPEGXS_MAIN
#define TCO_CONF_DEFAULT_GROUP_SIZE				4
#define TCO_CONF_DEFAULT_KERNEL_H				HOR_KERNEL_53
#define TCO_CONF_DEFAULT_NDECOMP_H				5
#define TCO_CONF_DEFAULT_KERNEL_V				VER_KERNEL_53
#define TCO_CONF_DEFAULT_NDECOMP_V				1
#define TCO_CONF_DEFAULT_NDECOMP_VH				1
#define TCO_CONF_DEFAULT_RCT					1
#define TCO_CONF_DEFAULT_RCT_TYPE				0
#define TCO_CONF_DEFAULT_CBR					1
#define TCO_CONF_DEFAULT_SKIP_WAVELET_ON_COMP   -1
#define TCO_CONF_DEFAULT_HAAR_USE_SHIFT			0
#define TCO_CONF_DEFAULT_COL_SZ					0
#define TCO_CONF_DEFAULT_COL_MAX_WIDTH			0
#define TCO_CONF_DEFAULT_COL_CNT				2
#define TCO_CONF_DEFAULT_COL_GRANULARITY		128
#define TCO_CONF_DEFAULT_COLS_RA_OPT			0

#define TCO_CONF_DEFAULT_GC_RAW					1
#define TCO_CONF_DEFAULT_GC_BOUNDED				1

#define TCO_CONF_DEFAULT_GC_PRED_PER_SB			1
#define TCO_CONF_DEFAULT_GC_RUN_PER_SB			1

#define TCO_CONF_DEFAULT_GC_VER					1
#define TCO_CONF_DEFAULT_GC_NOPRED				1

#define TCO_CONF_DEFAULT_GC_RUN_NONE			1
#define TCO_CONF_DEFAULT_GC_RUN_SIGFLAGS_ZRF	1
#define TCO_CONF_DEFAULT_GC_RUN_SIGFLAGS_ZRCSF	0
#define TCO_CONF_DEFAULT_SIGFLAGS_GROUP_WIDTH	8

#define TCO_CONF_DEFAULT_RUN_MODE				0
#define TCO_CONF_DEFAULT_GC_TRUNC				1
#define TCO_CONF_DEFAULT_GC_TRUNC_USE_PRIORITY	1
#define TCO_CONF_DEFAULT_SLICE_HEIGHT			64
#define TCO_CONF_DEFAULT_ORDER					ORDER_RPBC
#define TCO_CONF_DEFAULT_BUDGET_REPORT_LINES	0
#define TCO_CONF_DEFAULT_BUDGET_REPORT_NBYTES	-1
#define TCO_CONF_DEFAULT_DQ_TYPE				1
#define TCO_CONF_DEFAULT_DATA_CODING			0
#define TCO_CONF_DEFAULT_SP_LENHDR				1
#define TCO_CONF_DEFAULT_PREC_LENHDR			1
#define TCO_CONF_DEFAULT_SIGN_LENHDR			1
#define TCO_CONF_DEFAULT_GAINS_C1_H				4, 3, 2, 2, 1, 1,
#define TCO_CONF_DEFAULT_GAINS_C1_V				1, 1, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_GAINS_C1_V2			0, 0, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_GAINS_C2_H				2, 2, 1, 1, 0, 0,
#define TCO_CONF_DEFAULT_GAINS_C2_V				0, 0, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_GAINS_C2_V2			0, 0, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_GAINS_C3_H				2, 2, 1, 1, 0, 0,
#define TCO_CONF_DEFAULT_GAINS_C3_V				0, 0, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_GAINS_C3_V2			0, 0, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_PRIORITIES_C1_H		21, 15, 5, 14, 2, 7,
#define TCO_CONF_DEFAULT_PRIORITIES_C1_V		6, 20,  0, 0, 0, 0,
#define TCO_CONF_DEFAULT_PRIORITIES_C1_V2		0, 0, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_PRIORITIES_C2_H		1, 19, 9, 17, 4, 13,
#define TCO_CONF_DEFAULT_PRIORITIES_C2_V		12, 27, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_PRIORITIES_C2_V2		12, 27, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_PRIORITIES_C3_H		0, 18, 8, 16, 3, 11,
#define TCO_CONF_DEFAULT_PRIORITIES_C3_V		10, 22, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_PRIORITIES_C3_V2		0, 0, 0, 0, 0, 0,
#define TCO_CONF_DEFAULT_ENCODER_ID				0x1
#define TCO_CONF_DEFAULT_USE_SLICE_FOOTER       0
#define TCO_CONF_DEFAULT_IN_DEPTH				20
#define TCO_CONF_DEFAULT_QUANT					8
#define TCO_CONF_DEFAULT_V_SLICE_MIRRORING		0
#define TCO_CONF_DEFAULT_SIGN_OPT				SIGN_PACKING
#define TCO_CONF_DEFAULT_GC_NONSIG_GROUP_SIZE	8
#define TCO_CONF_DEFAULT_GC_TOP_PRED_MODE       0
#define TCO_CONF_DEFAULT_BOUNDED_ALPHABET		0
#define TCO_CONF_DEFAULT_VERBOSE				0
#define TCO_CONF_DEFAULT_PKT_HDR_SIZE_AUTO		1
#define TCO_CONF_DEFAULT_PKT_HDR_SIZE_SHORT		0
#define TCO_CONF_DEFAULT_CODESIG_WITH_0			1
#define TCO_CONF_DEFAULT_GAINS_CHOICE			GAINS_CHOICE_PSNR



typedef struct tco_conf_t {
	int32_t		bitstream_nbytes;
	double		bpp;
	uint8_t		profile;
	uint8_t		group_size;
	uint8_t		kernel_h;
	uint8_t		ndecomp_h;
	uint8_t		kernel_v;
	uint8_t		ndecomp_v;
	uint8_t		ndecomp_vh;
	uint8_t     skip_wavelet_on_comp;

	uint8_t		rct;
	uint8_t		rct_type;
	uint8_t		cbr;
	uint8_t		haar_use_shift;


	uint16_t col_sz;





	uint16_t col_max_width;

	uint8_t  col_cnt;



	uint16_t col_granularity;


	uint8_t cols_ra_opt;
	uint8_t		gc_raw;
	uint8_t		gc_bounded;
	uint8_t		gc_pred_per_sb;
	uint8_t		gc_run_per_sb;
	uint8_t		gc_ver;
	uint8_t		gc_nopred;
	uint8_t		gc_run_none;
	uint8_t		gc_run_sigflags_zrf;
	uint8_t		gc_run_sigflags_zrcsf;
	uint8_t		gc_trunc;
	uint8_t		gc_trunc_use_priority;
	int16_t		slice_height;
	uint8_t		order;
	int32_t		budget_report_nbytes;
	float		budget_report_lines;
	uint8_t		dq_type;
	uint8_t		data_coding;
	uint8_t		sp_lenhdr;
	uint8_t		sign_lenhdr;
	uint8_t		prec_lenhdr;
	uint32_t	encoder_id;
	uint8_t		use_slice_footer;
	uint8_t		in_depth;
	uint8_t		quant;
	uint8_t		v_slice_mirroring;
	uint8_t		sign_opt;
	uint32_t	gc_nonsig_group_size;
	uint8_t 	run_mode;
	uint8_t 	gc_top_pred_mode;
	uint8_t 	sigflags_group_width;
	uint8_t 	bounded_alphabet;
	uint8_t		verbose;
	uint8_t 	pkt_hdr_size_auto;
	uint8_t		pkt_hdr_size_short;
	uint8_t     codesig_with_0;

	int 		gains_choice;
	uint8_t 	lvl_gains[MAX_NCOMPS*MAX_FILTER_TYPES];
	uint8_t 	lvl_priorities[MAX_NCOMPS*MAX_FILTER_TYPES];
} tco_conf_t;



typedef struct tco_valid_weights_t
{
	bool_t lvl_gains_valid;
	bool_t lvl_priorities_valid;
} tco_valid_weights;


typedef struct image_t
{
	int magic;
	int ncomps;
	int w[MAX_NCOMPS];
	int h[MAX_NCOMPS];
	int depth;
	int is_422;
	tco_data_in_t *comps_array[MAX_NCOMPS];
} image_t;





#ifdef __cplusplus
extern "C"
{
#endif


EXPORTED int tco_allocate_image(image_t* ptr);


EXPORTED int tco_free_image(image_t* ptr);



EXPORTED int tco_print_image_info(image_t* ptr);



EXPORTED image_t* tco_clone_image(image_t* im_in, int copy_data);


EXPORTED int tco_image_copy_data(image_t* im_in, image_t* im_out);



EXPORTED int tco_image_dump(image_t* im, char* filename);


EXPORTED int tco_image_transpose(image_t* in, image_t* out, int only_header);


EXPORTED int tco_image_horizontal_split(image_t* in, image_t* up, image_t* down);



EXPORTED int* tco_get_version();


EXPORTED char* tco_get_version_str();





typedef struct tco_enc_context_t tco_enc_context_t;


EXPORTED tco_enc_context_t* tco_enc_init(tco_conf_t* conf, image_t* image_in, const char* budget_dump_filename);


EXPORTED int tco_enc_close(tco_enc_context_t* ctx);


EXPORTED int tco_enc_image( tco_enc_context_t* ctx, image_t* image_in,
							void* codestream_buf, int codestream_buf_byte_size,
							int* codestream_byte_size);






typedef struct tco_dec_context_t tco_dec_context_t;



EXPORTED int tco_dec_probe(uint8_t* bitstream_buf, int codestream_size, tco_conf_t* conf, image_t* image);



EXPORTED tco_dec_context_t* tco_dec_init(tco_conf_t* conf, image_t* image);

EXPORTED int tco_dec_set_fragment_csv_cbr_path(tco_dec_context_t* ctx, char* csv_buffer_model_cbr);

EXPORTED int tco_dec_set_fragment_csv_vbr_path(tco_dec_context_t* ctx, char* csv_buffer_model_vbr);

EXPORTED int tco_dec_close(tco_dec_context_t* ctx);


EXPORTED int tco_dec_bitstream(tco_dec_context_t* ctx, void* bitstream_buf,
									int bitstream_buf_size, image_t* image_out);


EXPORTED int tco_get_default_conf(tco_conf_t* conf, int tco_profile);

EXPORTED int tco_conf_validate(tco_conf_t* conf_in, image_t* image, tco_conf_t* conf_out);

#ifdef __cplusplus
}
#endif

#endif
