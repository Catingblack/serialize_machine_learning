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
#include <math.h>
#include <stdint.h>

#include "gcli.h"
#include "../std/bsr.h"


#define FLOOR_LOG2(x)  BSR(x)
#define GCLI(x) (((x) == 0) ? 0 : (BSR((x))+1))  

int compute_gcli_buf(const sig_mag_data_t* in, int len, gcli_data_t* out, int max_out_len, int* out_len, int group_size)
{
	int i,j;
	sig_mag_data_t or_all;
	*out_len = (len+group_size-1)/group_size;
	if (*out_len > max_out_len)
		return -1;

	for (i=0; i < (len/group_size)*group_size; i+=group_size)
	{
		for (or_all = 0, j=0; j<group_size; j++)
			or_all |= *in++;
		*out++ = GCLI(or_all & (~SIGN_BIT_MASK));
	}
	 
	if (len % group_size)
	{
		for (or_all = 0, j=0; j<len % group_size; j++)
			or_all |= *in++;
		*out++ = GCLI(or_all & (~SIGN_BIT_MASK));
	}
	return 0;
}


#ifdef TEST_GCLI

int main()
{
	int out_len;
	int i;
	int pass;
	
	#define data_len sizeof(data_in)/sizeof(sig_mag_data_t)
	sig_mag_data_t data_in[] = {
		18,  37,  44, -30, -17, -47,  36, -31,   7,  33,   0, -37,  -9,
		0,  20,  25,  41,  -4,  14,  46,   1, -23,  37, -32,  41,  -7,
		-10, -42,   5,  41,   3,  23,  34,  19,  15, -42,  12, -26,   3,
		-22,  -9, -39,   7, -28,   5,  42, -35,  16,  -8, -10,   9, -25,
		-30,  36, -45, -20,  33, -16, -32,  39,  31,  39, -24,  25,  11,
		-24, -31, -42, -47,  30, -32, -12, -22, -22,  -1,   6,  16, -32,
		-28, -28, -24, -35,  34, -18,  19,  24,  21,  19, -27, -45, -12,
		-34,  17,  -1,  30, -25,  22,  17,  31,  -5,  34, -42, -19,  -3,
		-28, -31,  12,  26,  23,   9, -49,  32,   4, -22, -23, -23,  35,
		2,  43,  -2,  38, -38, -23,  33,  39};
	
	gcli_data_t gcli_out[data_len];

	 
	to_sig_mag_buf(data_in, data_len, data_in, data_len, &out_len);
	for (i=0; i<out_len;i++)
		fprintf(stderr, "%08X ", data_in[i]);
	fprintf(stderr, "\n");

	compute_gcli_buf(data_in, data_len, gcli_out,data_len, &out_len, 4);

	for (i=0; i<out_len;i++)
		fprintf(stderr, "%d ", gcli_out[i]);
	fprintf(stderr, "\n");
	compute_gcli_buf(data_in, data_len, gcli_out,data_len, &out_len, 1);
	for (i=0; i<out_len;i++)
		fprintf(stderr, "%d ", gcli_out[i]);
	fprintf(stderr, "\n");
}

#endif

