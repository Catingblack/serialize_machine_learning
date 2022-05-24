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

#include "quant.h"
#include "common.h"
#include "../inc/tco.h"

int uniform_dq(int sig_mag_value, int gcli, int gtli)
{
	uint32_t dq_out = 0;
	if (gcli > gtli)
	{
		int zeta = gcli - gtli + 1;
		int d    = sig_mag_value & ~SIGN_BIT_MASK;
		dq_out   = ((d << zeta) - d + (1 << gcli)) >> (gcli + 1);
		if (dq_out)
			dq_out |= (sig_mag_value & SIGN_BIT_MASK);
	}
	return dq_out;
}

int uniform_dq_inverse(int dq_in, int gcli, int gtli)
{
	int sign = (dq_in & SIGN_BIT_MASK);
	int phi  =  dq_in & ~SIGN_BIT_MASK;
	int zeta =  gcli - gtli + 1;
	int rho  =  0;
	for(rho = 0;phi > 0;phi >>= zeta)
		rho += phi;
	return sign | rho;
}

int deadzone_dq(int sig_mag_value, int gtli)
{
	int dq_out = (sig_mag_value & ~SIGN_BIT_MASK) >> gtli;
	if (dq_out)
		dq_out |= (sig_mag_value & SIGN_BIT_MASK);
	return dq_out;
}

int deadzone_dq_inverse(int dq_in, int gcli, int gtli)
{
	sig_mag_data_t sigmag_data;
	sigmag_data = dq_in;
	if (gtli>0 && (dq_in & ~SIGN_BIT_MASK))
		sigmag_data |= (1<<(gtli-1));
	return sigmag_data;
}

int apply_dq(int dq_type, int sig_mag_value, int gcli, int gtli)
{
	switch(dq_type) {
	case 1:
		return uniform_dq(sig_mag_value, gcli, gtli);
	case 0:
		return deadzone_dq(sig_mag_value, gtli);
	default:
		assert(!"invalid quantizer type");
	}
}

sig_mag_data_t apply_dq_inverse(int dq_type, int dq_in, int gcli, int gtli)
{
	switch(dq_type) {
	case 1:
		return uniform_dq_inverse(dq_in, gcli, gtli);
	case 0:
		return deadzone_dq_inverse(dq_in, gcli, gtli);
	default:
		assert(!"invalid quantizer type");
	}
}

int quant(sig_mag_data_t* buf, int buf_len, gcli_data_t* gclis, int group_size, int gtli, int dq_type)
{
	int i=0,group=0,idx=0,gcli=0;
	for (group = 0; group<(buf_len+group_size-1)/group_size; group++)
	{
		int valid_group_size = (buf_len-idx) > group_size ? group_size : (buf_len-idx);
		gcli = gclis[group];
		if (gcli <= gtli)
		{
			 
			memset(&buf[idx], 0, sizeof(sig_mag_data_t) * valid_group_size);
		}
		else
		{
			if (gtli > 0)
			{
				for (i=0; i < valid_group_size; i++)
				{
					int sign = (SIGN_BIT_MASK & buf[idx+i]);
					buf[idx+i]  = (~SIGN_BIT_MASK) & apply_dq(dq_type,buf[idx+i], gcli, gtli);
					buf[idx+i]  = buf[idx+i] << gtli;
					buf[idx+i] |= sign;
				}
			}
		}
		idx+=group_size;
	}
	return 0;
}

int dequant(sig_mag_data_t* buf, int buf_len, gcli_data_t* gclis, int group_size, int gtli, int dq_type)
{
	int i=0,group=0,idx=0,gcli=0;
	for (group = 0; group<(buf_len+group_size-1)/group_size; group++)
	{
		gcli = gclis[group];
		if (gcli > gtli)
		{
			if (gtli > 0)
			{
				int valid_group_size = (buf_len-idx) > group_size ? group_size : (buf_len-idx);
				for (i=0; i < valid_group_size; i++)
					buf[idx+i] = apply_dq_inverse(dq_type, buf[idx+i], gcli, gtli);
			}
		}
		idx+=group_size;
	}
	return 0;
}


#ifdef TEST_QUANT
 
 


#include <stdlib.h>
#include <math.h>

#define TEST_SIZE 10001  
#define DYNAMIC 0xffff
int	signed_data[TEST_SIZE];
int signed_data2[TEST_SIZE];
sig_mag_data_t sigmag_data[TEST_SIZE];
sig_mag_data_t sigmag_data2[TEST_SIZE];
gcli_data_t gcli_data[TEST_SIZE];


float get_psnr(int* ref, int* out, int len)
{
	int i;
	uint64_t sqe = 0;
	int delta = 0;
	float psnr = 0;
	
	for (i=0; i<len; i++)
	{
		delta = (ref[i] - out[i]);
		sqe += pow(delta,2);
	}
	psnr = 10 * log10(((float) DYNAMIC*DYNAMIC*len)/(float) (sqe));
	return psnr;
}


int report_psnr(int* signed_data, int* signed_data2, int group_size, int sign_reuse, int dq_type, int gtli)
{
	fprintf(stderr, "group=%d reuse=%d dq=%d gtli=%d: psnr=%.2f\n", group_size, sign_reuse, dq_type, gtli, get_psnr(signed_data, signed_data2, TEST_SIZE));
	return 0;
}

int simulate_packing(sig_mag_data_t* data, int len, int gtli, int use_rounding)
{
	int i;
	if (gtli > 0)
	{
		for (i=0;i<len;i++)
		{
			data[i] &= ~((1<<gtli)-1);
			if (use_rounding) 
				if (data[i] & (~SIGN_BIT_MASK))				
					data[i] |= (1<<(gtli-1));
		}
	}
	return 0;
}

int main()
{
	int i, dummy, dq_type, gtli, group_size, sign_reuse, use_rounding=0;
	srand(123456);

	 
	for (i=0; i<TEST_SIZE; i++)
		signed_data[i] = (rand() % DYNAMIC) - (DYNAMIC/2);

	for (gtli=0; gtli < 15; gtli++)
	{
		int test_idx = rand() % TEST_SIZE;
		fprintf(stderr, "=============================\n");
		for (group_size = 4; group_size<5; group_size++)				
		{
			for (sign_reuse = 0; sign_reuse < 2; sign_reuse++)
			{
				for (dq_type = 0; dq_type<3; dq_type++)
				{
					 
					assert(to_sig_mag_buf(signed_data, TEST_SIZE, sigmag_data, TEST_SIZE, &dummy) == 0);
					 
					assert(compute_gcli_buf(sigmag_data, TEST_SIZE, gcli_data, (TEST_SIZE+group_size-1)/group_size, &dummy, group_size) == 0);
					assert(quant(sigmag_data, TEST_SIZE, gcli_data, group_size, gtli, dq_type, sign_reuse) == 0);
					 

					 
					simulate_packing(sigmag_data, TEST_SIZE, gtli, use_rounding);
					 
	
					assert(dequant(sigmag_data, TEST_SIZE, gcli_data, group_size, gtli, dq_type, sign_reuse) == 0);
					 
					assert(from_sig_mag_buf(sigmag_data, TEST_SIZE, signed_data2, TEST_SIZE, &dummy) == 0);
					 
					report_psnr(signed_data, signed_data2, group_size, sign_reuse, dq_type, gtli);
				}
			}
		}
	}
	return 0;
}

#endif

