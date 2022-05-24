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


#ifndef IMAGE_TRANSFORMS_H

#include <malloc.h>
#include <stdio.h>

#include "../inc/tco.h"




int tco_image_shift_all(image_t* image_in, image_t* image_out, int shift, int rounding);


int tco_image_apply_offset(image_t* image_in, image_t* image_out, int offset);


int tco_image_clamp(image_t* image_in, image_t* image_out, int low, int high);


int tco_image_apply_rct(image_t* image_in, image_t* image_out, int rct_type);


int tco_image_apply_ict(image_t* image_in, image_t* image_out);


int tco_image_apply_irct(image_t* image_in, image_t* image_out, int rct_type);


int tco_image_apply_iict(image_t* image_in, image_t* image_out);





int tco_image_apply_dwt_hor(image_t* image_in, image_t* image_out, int ndecomp, int kernel, int* tmp_buffer, int tile_width);


int tco_image_apply_partial_dwt_hor(image_t* image_in, image_t* image_out, int ndecomp, int* kernel, int* tmp_buffer,
				    int level);


int tco_image_apply_dwt_ver(image_t* image_in, image_t* image_out, int kernel);


int tco_image_apply_partial_dwt_ver(image_t* image_in, image_t* image_out, int* kernel, int lvl);


int tco_image_apply_idwt_ver(image_t* image_in, image_t* image_out, int kernel);


int tco_image_apply_partial_idwt_ver(image_t* image_in, image_t* image_out, int* kernel,int lvl);






int tco_image_apply_idwt_hor(image_t* image_in, image_t* image_out, int ndecomp, int kernel, int tile_width);


int tco_image_apply_partial_idwt_hor(image_t* image_in, image_t* image_out, int ndecomp, int* kernel, int level);

#endif
