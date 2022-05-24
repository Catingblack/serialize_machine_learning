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


#ifndef DWT53_H
#define DWT53_H

#include <stdint.h>
#include "dwt.h"


int dwt53_get_lvl_sizes(int w, int n_decomp, int* lvl_sizes);


int dwt53(dwt_data_t* data_in, int len, dwt_data_t* lf_out, int lf_max_len, int* lf_len, dwt_data_t* hf_out, int hf_max_len, int* hf_len, int inc);
int idwt53(dwt_data_t* lf_in, int lf_len, dwt_data_t* hf_in, int hf_len, dwt_data_t* data_out, int data_max_len, int* data_len, int inc);


/* small trick to be able to skip dwt while ordering the data the same way */
int dwt_skip(dwt_data_t* data_in, int len, dwt_data_t* lf_out, int lf_max_len, int* lf_len, dwt_data_t* hf_out, int hf_max_len, int* hf_len, int inc);
int dwt_skip_multi(dwt_data_t* data_in, int len, int ndecomp, dwt_data_t** lvls_buffers, dwt_data_t* tmp_buffer);
int idwt_skip(dwt_data_t* lf_in, int lf_len, dwt_data_t* hf_in, int hf_len, dwt_data_t* data_out, int data_max_len, int* data_len, int inc);
int idwt_skip_multi(dwt_data_t** lvls_buffers, int* lvls_sizes, int ndecomp, dwt_data_t* data_out, int out_data_max_len);


int dwt53_multi(dwt_data_t* data_in, int len, int ndecomp, dwt_data_t** lvls_buffers, dwt_data_t* tmp_buffer);
int idwt53_multi(dwt_data_t** lvls_buffers, int* lvls_sizes, int ndecomp, dwt_data_t* data_out, int out_data_max_len);

#endif
