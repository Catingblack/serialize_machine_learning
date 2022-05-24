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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <tco.h>
#include "default_weights.h"

 

#define WEIGHTS_INVALID_TOKEN {WEIGHTS_INVALID,{0}}



struct default_weights_t default_weights[]=
{
 
{VER_KERNEL_53, HOR_KERNEL_53,  0,  1, GAINS_OPT_PSNR, GAINS_NOCT,
{{0,0},{0,0},{0,0}},
{{2,4},{1,3},{0,5}}},
{VER_KERNEL_53, HOR_KERNEL_53,  0,  1, GAINS_OPT_PSNR, GAINS_RCT,
{{1,1},{0,0},{0,0}},
{{0,3},{2,4},{1,5}}},
{VER_KERNEL_53, HOR_KERNEL_53,  0,  2, GAINS_OPT_PSNR, GAINS_NOCT,
{{0,0,0},{0,0,0},{0,0,0}},
{{2,5,7},{0,4,8},{1,3,6}}},
{VER_KERNEL_53, HOR_KERNEL_53,  0,  2, GAINS_OPT_PSNR, GAINS_RCT,
{{2,1,1},{0,0,0},{0,0,0}},
{{6,2,5},{0,4,8},{1,3,7}}},
{VER_KERNEL_53, HOR_KERNEL_53,  0,  3, GAINS_OPT_PSNR, GAINS_NOCT,
{{1,0,0,0},{1,0,0,0},{1,0,0,0}},
{{5,1,6,10},{4,2,8,9},{3,0,7,11}}},
{VER_KERNEL_53, HOR_KERNEL_53,  0,  3, GAINS_OPT_PSNR, GAINS_RCT,
{{2,1,1,1},{1,0,0,0},{1,0,0,0}},
{{3,0,6,9},{5,2,8,10},{4,1,7,11}}},
{VER_KERNEL_53, HOR_KERNEL_53,  0,  4, GAINS_OPT_PSNR, GAINS_NOCT,
{{1,1,0,0,0},{1,1,0,0,0},{1,1,0,0,0}},
{{2,9,5,7,13},{1,11,3,6,12},{0,10,4,8,14}}},
{VER_KERNEL_53, HOR_KERNEL_53,  0,  4, GAINS_OPT_PSNR, GAINS_RCT,
{{3,2,1,1,1},{1,1,0,0,0},{1,1,0,0,0}},
{{12,8,2,5,9},{1,11,3,6,13},{0,10,4,7,14}}},
{VER_KERNEL_53, HOR_KERNEL_53,  0,  5, GAINS_OPT_PSNR, GAINS_NOCT,
{{2,1,1,0,0,0},{2,1,1,0,0,0},{2,1,1,0,0,0}},
{{8,5,13,1,9,16},{7,3,12,0,11,15},{6,4,14,2,10,17}}},
{VER_KERNEL_53, HOR_KERNEL_53,  0,  5, GAINS_OPT_PSNR, GAINS_RCT,
{{3,2,2,1,1,1},{2,1,1,0,0,0},{2,1,1,0,0,0}},
{{6,1,12,0,9,13},{8,4,14,2,11,16},{7,5,15,3,10,17}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  1, GAINS_OPT_PSNR, GAINS_NOCT,
{{1,0,0,0},{1,0,0,0},{1,0,0,0}},
{{8,1,2,10},{7,5,4,9},{6,0,3,11}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  1, GAINS_OPT_PSNR, GAINS_RCT,
{{2,1,1,1},{1,0,0,0},{1,0,0,0}},
{{6,0,1,7},{9,5,4,10},{8,2,3,11}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  2, GAINS_OPT_PSNR, GAINS_NOCT,
{{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0}},
{{11,0,8,5,13},{10,2,4,3,12},{9,1,7,6,14}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  2, GAINS_OPT_PSNR, GAINS_RCT,
{{2,1,1,1,1},{1,0,0,0,0},{1,0,0,0,0}},
{{5,0,4,3,12},{11,2,7,6,13},{10,1,9,8,14}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  3, GAINS_OPT_PSNR, GAINS_NOCT,
{{1,1,0,0,0,0},{1,1,0,0,0,0},{1,1,0,0,0,0}},
{{2,14,4,7,8,16},{1,12,3,6,11,15},{0,13,5,9,10,17}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  3, GAINS_OPT_PSNR, GAINS_RCT,
{{3,2,1,1,1,1},{1,1,0,0,0,0},{1,1,0,0,0,0}},
{{15,11,2,5,6,14},{1,12,3,7,10,16},{0,13,4,8,9,17}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  4, GAINS_OPT_PSNR, GAINS_NOCT,
{{2,1,1,0,0,0,0},{2,1,1,0,0,0,0},{2,1,1,0,0,0,0}},
{{14,4,17,0,7,11,20},{12,3,15,1,10,9,18},{13,5,16,2,8,6,19}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  4, GAINS_OPT_PSNR, GAINS_RCT,
{{3,2,2,1,1,1,1},{2,1,1,0,0,0,0},{2,1,1,0,0,0,0}},
{{8,3,15,0,4,5,18},{13,6,16,1,12,11,19},{14,7,17,2,10,9,20}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  5, GAINS_OPT_PSNR, GAINS_NOCT,
{{2,2,1,1,0,0,0,0},{2,2,1,1,0,0,0,0},{2,2,1,1,0,0,0,0}},
{{0,18,6,15,3,10,9,21},{1,19,7,16,4,12,11,22},{2,20,8,17,5,14,13,23}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  5, GAINS_OPT_PSNR, GAINS_RCT,
{{4,3,2,2,1,1,1,1},{2,2,1,1,0,0,0,0},{2,2,1,1,0,0,0,0}},
{{21,15,5,14,2,7,6,20},{1,19,9,17,4,13,12,23},{0,18,8,16,3,11,10,22}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  5, GAINS_OPT_VISUAL, GAINS_NOCT,
{{6,5,4,4,3,1,1,0},{5,4,3,3,3,1,1,0},{5,4,3,3,3,1,1,0}},
{{15,11,0,14,20,6,5,23},{16,12,1,9,21,3,7,18},{17,13,2,10,22,4,8,19}}},
{VER_KERNEL_53, HOR_KERNEL_53,  1,  5, GAINS_OPT_VISUAL, GAINS_RCT,
{{8,7,7,6,5,3,3,2},{6,5,4,4,3,1,1,0},{6,5,4,4,3,1,1,0}},
{{12,8,21,9,17,3,2,20},{15,10,1,14,18,4,6,22},{16,11,0,13,19,5,7,23}}},
{VER_KERNEL_53, HOR_KERNEL_53,  2,  2, GAINS_OPT_PSNR, GAINS_NOCT,
{{1,1,1,0,0,0,0},{1,1,1,0,0,0,0},{1,1,1,0,0,0,0}},
{{2,13,17,9,4,8,20},{0,12,15,10,7,6,18},{1,14,16,11,5,3,19}}},
{VER_KERNEL_53, HOR_KERNEL_53,  2,  2, GAINS_OPT_PSNR, GAINS_RCT,
{{2,2,2,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,0,0,0,0}},
{{0,12,13,9,3,4,18},{1,14,16,10,8,7,19},{2,15,17,11,6,5,20}}},
{VER_KERNEL_53, HOR_KERNEL_53,  2,  3, GAINS_OPT_PSNR, GAINS_NOCT,
{{2,1,1,1,0,0,0,0},{2,1,1,1,0,0,0,0},{2,1,1,1,0,0,0,0}},
{{11,1,16,19,12,7,6,21},{9,0,17,20,13,3,5,22},{10,2,15,18,14,4,8,23}}},
{VER_KERNEL_53, HOR_KERNEL_53,  2,  3, GAINS_OPT_PSNR, GAINS_RCT,
{{3,2,2,2,1,1,1,1},{2,1,1,1,0,0,0,0},{2,1,1,1,0,0,0,0}},
{{9,0,15,16,10,2,1,21},{11,3,18,20,13,5,7,22},{12,4,17,19,14,6,8,23}}},
{VER_KERNEL_53, HOR_KERNEL_53,  2,  4, GAINS_OPT_PSNR, GAINS_NOCT,
{{2,2,1,1,1,0,0,0,0},{2,2,1,1,1,0,0,0,0},{2,2,1,1,1,0,0,0,0}},
{{2,22,4,15,18,14,11,7,24},{0,23,3,16,19,12,9,10,26},{1,21,5,17,20,13,6,8,25}}},
{VER_KERNEL_53, HOR_KERNEL_53,  2,  4, GAINS_OPT_PSNR, GAINS_RCT,
{{3,3,2,2,2,1,1,1,1},{2,2,1,1,1,0,0,0,0},{2,2,1,1,1,0,0,0,0}},
{{0,21,3,15,16,12,5,4,22},{1,24,6,17,19,13,10,11,26},{2,23,7,18,20,14,8,9,25}}},
{VER_KERNEL_53, HOR_KERNEL_53,  2,  5, GAINS_OPT_PSNR, GAINS_NOCT,
{{3,2,2,1,1,1,0,0,0,0},{3,2,2,1,1,1,0,0,0,0},{3,2,2,1,1,1,0,0,0,0}},
{{14,9,25,0,19,23,17,4,3,28},{13,11,24,1,20,22,15,8,6,27},{12,10,26,2,18,21,16,5,7,29}}},
{VER_KERNEL_53, HOR_KERNEL_53,  2,  5, GAINS_OPT_PSNR, GAINS_RCT,
{{4,3,3,2,2,2,1,1,1,1},{3,2,2,1,1,1,0,0,0,0},{3,2,2,1,1,1,0,0,0,0}},
{{12,3,24,0,18,19,13,2,1,25},{15,11,26,4,21,23,16,9,7,28},{14,10,27,5,20,22,17,6,8,29}}},
{VER_KERNEL_53, HOR_KERNEL_53,  2,  5, GAINS_OPT_VISUAL, GAINS_NOCT,
{{6,5,5,4,3,3,1,1,1,0},{6,5,5,4,3,3,1,1,1,0},{6,5,5,4,3,3,1,1,1,0}},
{{11,3,25,6,19,23,2,13,12,28},{10,5,24,7,20,22,0,17,15,27},{9,4,26,8,18,21,1,14,16,29}}},
{VER_KERNEL_53, HOR_KERNEL_53,  2,  5, GAINS_OPT_VISUAL, GAINS_RCT,
{{8,7,7,6,5,5,4,3,3,2},{6,5,5,4,3,3,1,1,1,0},{6,5,5,4,3,3,1,1,1,0}},
{{6,0,23,3,17,18,27,12,11,26},{10,5,24,7,20,22,1,16,14,28},{9,4,25,8,19,21,2,13,15,29}}},
 
 
 
{VER_KERNEL_22, HOR_KERNEL_53,  1,  5, GAINS_OPT_PSNR, GAINS_NOCT,
{{3,2,2,1,1,0,0,0},{3,2,2,1,1,0,0,0},{3,2,2,1,1,0,0,0}},
{{14,7,19,4,15,2,10,21},{12,6,20,5,16,0,9,22},{13,8,18,3,17,1,11,23}}},
{VER_KERNEL_22, HOR_KERNEL_53,  1,  5, GAINS_OPT_PSNR, GAINS_RCT,
{{4,3,3,2,2,2,1,1},{3,2,2,1,1,0,0,0},{3,2,2,1,1,0,0,0}},
{{11,3,17,2,14,19,4,18},{12,7,21,6,15,0,9,22},{13,8,20,5,16,1,10,23}}},
{VER_KERNEL_22, HOR_KERNEL_53,  2,  5, GAINS_OPT_PSNR, GAINS_NOCT,
{{3,3,2,2,1,1,0,0,0,0},{3,3,2,2,1,1,0,0,0,0},{3,3,2,2,1,1,0,0,0,0}},
{{5,24,13,21,7,20,11,0,15,28},{4,26,12,22,10,19,8,2,16,27},{3,25,14,23,6,18,9,1,17,29}}},
{VER_KERNEL_22, HOR_KERNEL_53,  2,  5, GAINS_OPT_PSNR, GAINS_RCT,
{{5,4,3,3,2,2,1,2,1,1},{3,3,2,2,1,1,0,0,0,0},{3,3,2,2,1,1,0,0,0,0}},
{{27,20,10,19,4,16,5,24,11,23},{3,26,12,21,7,18,8,1,14,28},{2,25,13,22,6,17,9,0,15,29}}},
 
{VER_KERNEL_22, HOR_KERNEL_22,  1,  2, GAINS_OPT_PSNR, GAINS_NOCT,
{{2,1,1,1,0,0},{2,1,1,1,0,0},{2,1,1,1,0,0}},
{{4,5,14,12,13,0},{2,3,11, 9,10,0},{0,1,8,6,7,0}}},
{VER_KERNEL_22, HOR_KERNEL_22,  1,  2, GAINS_OPT_PSNR, GAINS_RCT,
{{3,2,2,0,0,0},{2,1,0,2,1,0},{2,1,0,0,0,0}},
{{4,5,12,10,11,0},{8,9,3,2,14,0},{6,7,1,0,13,0}}},
{VER_KERNEL_22, HOR_KERNEL_22,  1,  2, GAINS_OPT_VISUAL, GAINS_NOCT,
{{5,3,2,2,0,0},{4,2,2,1,0,0},{4,2,2,1,0,0}},	
{{0,3,14,11,17},{1,4,6,12,15},{2,5,7,13,16}}},
{VER_KERNEL_22, HOR_KERNEL_22,  1,  2, GAINS_OPT_VISUAL, GAINS_RCT,
{{7,5,4,4,2},{5,3,2,2,0},{5,3,2,2,0}},
{{0,1,7,11,16,0},{6,15,0,3,5,14},{2,4,13,12,17,0}}},
{VER_KERNEL_22, HOR_KERNEL_22,  1,  5, GAINS_OPT_PSNR, GAINS_NOCT,
{{4,3,2,2,1,1,1,0},{4,3,2,2,1,1,1,0},{4,3,2,2,1,1,1,0}},
{{20,21,4,22,5,23,18,19},{14,15,2,16,3,17,12,13},{8,9,0,10,1,11,6,7}}},
{VER_KERNEL_22, HOR_KERNEL_22,  1,  5, GAINS_OPT_PSNR, GAINS_RCT,
{{5,4,3,3,2,2,2,1},{3,2,2,1,1,0,0,0},{3,2,2,1,1,0,0,0}},
{{18,19,10,20,11,21,16,17},{6,7,14,8,15,9,5,23},{1,2,12,3,13,4,0,22}}},
{VER_KERNEL_22, HOR_KERNEL_22,  1,  5, GAINS_OPT_VISUAL, GAINS_NOCT,
{{7,6,5,5,3,2,2},{6,5,4,4,2,2,1},{6,5,4,4,2,2,1}},	
{{6,9,0,14,3,20,17,23},{7,10,1,15,4,12,18,21},{8,11,2,16,5,13,19,22}}},
{VER_KERNEL_22, HOR_KERNEL_22,  1,  5, GAINS_OPT_VISUAL, GAINS_RCT,
{{9,8,7,7,5,4,4,2},{7,6,5,5,3,2,2,0},{7,6,5,5,3,2,2,0}},
{{6,7,0,12,1,16,15,21},{8,10,2,13,4,19,17,22},{9,11,3,14,5,20,18,23}}},
 
 
 
 
{VER_KERNEL_137, HOR_KERNEL_137,  1,  5, GAINS_OPT_PSNR, GAINS_RCT,
{{4,4,3,3,2,2,2,2},{3,3,2,2,1,1,1,0},{3,3,2,2,1,1,1,0}},
{{0,18,8,17,5,10,9,23},{4,22,12,20,7,16,15,2},{3,21,11,19,6,14,13,1}}},
};


struct default_weights_t* get_default_weights(int kernel_v, int kernel_h, int ndecomp_v, int ndecomp_h, int opt, int rct)
{
	int line = 0;

	for (line=0; line<sizeof(default_weights)/sizeof(struct default_weights_t); line++)
	{
		if 	((default_weights[line].kernel_v==kernel_v) &&
			(default_weights[line].kernel_h==kernel_h) &&
			(default_weights[line].ndecomp_v==ndecomp_v) &&
			(default_weights[line].ndecomp_h==ndecomp_h) &&
			(default_weights[line].opt==opt) &&
			(default_weights[line].rct==rct))
		{
			return &default_weights[line];
		}
	}
	return NULL;
}

