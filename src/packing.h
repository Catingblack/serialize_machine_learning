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


#ifndef PACKING_H
#define PACKING_H

#include "buf_mgmt.h"
#include "sig_mag.h"
#include "gcli.h"
#include "gcli_budget.h"
#include "pred.h"
#include "bitpacking.h"
#include "precinct.h"
#include "sigbuffer.h"
#include "gcli_methods.h"
#include "rate_control.h"

#define PREC_HDR_PREC_SIZE 24
#define PREC_HDR_SCENARIO_SIZE 8
#define PREC_HDR_REFINEMENT_SIZE 8

#define PREC_HDR_ALIGNMENT 8


#define PKT_HDR_DATA_SIZE_SHORT 15
#define PKT_HDR_DATA_SIZE_LONG 20
#define PKT_HDR_GCLI_SIZE_SHORT 13
#define PKT_HDR_GCLI_SIZE_LONG 20
#define PKT_HDR_SIGN_SIZE_SHORT 11
#define PKT_HDR_SIGN_SIZE_LONG 15
#define PKT_HDR_ALIGNMENT 8

#define SUBPKT_ALIGNMENT 8

typedef struct packing_context_t packing_context_t;

typedef struct unpacking_context_t unpacking_context_t;

typedef struct unpacked_info_t
{
	int gtli_table_data[MAX_LVLS];
	int gtli_table_gcli[MAX_LVLS];
	int haar_shift_index;

	int prec_len;
	int padding_len;

	int gcli_len[MAX_SUBPKTS];
	int sign_len[MAX_SUBPKTS];
	int data_len[MAX_SUBPKTS];
}unpacked_info_t;


unpacking_context_t* unpacker_open(const tco_conf_t* conf, lvls_map_t* map,const precinct_t *prec);
void unpacker_close(unpacking_context_t* ctx);


packing_context_t* packer_open(const tco_conf_t* conf, const precinct_t *prec);
void packer_close(packing_context_t* ctx);

int unpacker_set_csv_buffer_model_cbr(unpacking_context_t* unpack_ctx, char* csv_buffer_model_cbr);
int unpacker_set_csv_buffer_model_vbr(unpacking_context_t* unpack_ctx, char* csv_buffer_model_vbr);


int pack_precinct(packing_context_t* ctx, bit_packer_t* bitstream, precinct_t* precinct, rc_results_t* ra_result);


int unpack_precinct(unpacking_context_t* ctx, bit_unpacker_t* bitstream,
					precinct_t* precinct, precinct_t* precinct_top, int* gtli_table_top,
					unpacked_info_t* info);

int pack_gclis(packing_context_t* ctx, bit_packer_t* bitstream, precinct_t* precinct, rc_results_t* ra_result, int position);
int pack_data(bit_packer_t* bitstream, sig_mag_data_t* buf, int buf_len, gcli_data_t* gclis, int group_size, int gtli, int sign_packing);
int pack_sign(bit_packer_t* bitstream, sig_mag_data_t* buf, int buf_len, gcli_data_t* gclis, int group_size, int gtli);



int unary_encode(bit_packer_t* bitstream, gcli_pred_t* gcli_pred_buf, int len, int no_sign, unary_alphabet_t alph);
int unary_decode(bit_unpacker_t* bitstream, gcli_pred_t* gcli_pred_buf, int len, int no_sign, unary_alphabet_t alph);

int unpack_data(bit_unpacker_t* bitstream, sig_mag_data_t* buf, int buf_len, gcli_data_t* gclis, int group_size, int gtli, int sign_packing);
int unpack_sign(bit_unpacker_t* bitstream, sig_mag_data_t* buf, int buf_len, int group_size);


int pack_raw_gclis(bit_packer_t* bitstream, gcli_data_t* gclis, int len);
int unpack_raw_gclis(bit_unpacker_t* bitstream, gcli_data_t* gclis, int len);


#endif
