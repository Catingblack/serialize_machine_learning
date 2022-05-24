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

#include "sig_mag.h"
#include "common.h"

static INLINE void to_sig_mag(sig_mag_data_in_t val, sig_mag_data_t* val_out)
{
	*val_out = (val >= 0) ? val : (-val) | SIGN_BIT_MASK;
}

static INLINE void from_sig_mag(sig_mag_data_t val, sig_mag_data_in_t* val_out)
{
	*val_out = (val & SIGN_BIT_MASK) ? -(val^SIGN_BIT_MASK) : val;
}

int to_sig_mag_buf(const sig_mag_data_in_t* data_in, int in_len, sig_mag_data_t* data_out, int max_out_len, int* out_len)
{
	int i=0;
	if (max_out_len < in_len)
		return -1;
	*out_len = in_len;
	for (i=0;i<in_len;i++)
		to_sig_mag(*data_in++,data_out++);
	return 0;
}

int from_sig_mag_buf(const sig_mag_data_t* data_in, int in_len, sig_mag_data_in_t* data_out, int max_out_len, int* out_len)
{
	int i=0;
	if (max_out_len < in_len)
		return -1;
	*out_len = in_len;
	for (i=0;i<in_len;i++)
		from_sig_mag(*data_in++,data_out++);
	return 0;
}


#ifdef TEST_SIG_MAG

int main()
{
	int out_len;
	int i;
	int pass;
	
	#define data_len sizeof(data_in)/sizeof(sig_mag_data_in_t)
	sig_mag_data_in_t data_in[] = {
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
	
	sig_mag_data_t data_sig_mag[data_len];
	sig_mag_data_in_t data_out[data_len];

	to_sig_mag_buf(data_in, data_len, data_sig_mag, data_len, &out_len);
	from_sig_mag_buf(data_sig_mag, data_len, data_out, data_len, &out_len);

	pass = 1;
	for (i=0; i<data_len;i++)
		if (data_in[i]!=data_out[i])
		{
			fprintf(stderr, "%d %d - %d %d FAIL!\n", data_in[i],data_out[i]);
			pass=0;
		}
	if (pass)
		fprintf(stderr, "PASS! (%04X %04X)\n");
}

#endif
