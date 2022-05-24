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
#include <string.h>

#include "dwt137.h"


 
int dwt137_get_lvl_sizes(int w, int ndecomp, int* lvl_sizes)
{
	int h_lvl, hf_size, size;
	int lf_size = w;

	for (h_lvl = ndecomp; h_lvl >= 0; h_lvl--)
	{
		if (h_lvl > 0)
		{
			hf_size = (lf_size) / 2;
			lf_size = (lf_size - hf_size);
			size = hf_size;
		}
		else
		{
			size = lf_size;
		}
		lvl_sizes[h_lvl] = size;
	}
	return 0;
}


int dwt137(dwt_data_t* data_in, int len, dwt_data_t* lf_out, int lf_max_len,
	  int* lf_len, dwt_data_t* hf_out, int hf_max_len, int* hf_len, int inc)
{
	int i;
	*hf_len = len/2;
	*lf_len = (len+1)/2;

	if ((*hf_len > hf_max_len) || (*lf_len > lf_max_len))
		return -1;

	 
	for (i=1; i<len; i+=2)
	{
		int left       = (i - 1);
		int leftleft   = (i - 3);
		int right      = (i + 1);
		int rightright = (i + 3);
		if (left < 0)
			left     = -left;
		if (leftleft < 0)
			leftleft = -leftleft;
		if (right >= len)
			right      = ((len - 1) << 1) - right;
		if (rightright >= len)
			rightright = ((len - 1) << 1) - rightright;
		hf_out[(i >> 1)*inc] = data_in[i*inc] - ((-data_in[leftleft*inc] +
							  9*data_in[left * inc] +
							  9*data_in[right * inc] -
							  data_in[rightright * inc] + 8) >> 4);
	}

	 
	 
	for(i = 0;i < len;i += 2)
	{
		int left       = (i - 1);
		int leftleft   = (i - 3);
		int right      = (i + 1);
		int rightright = (i + 3);
		if (left < 0)
			left     = -left;
		if (leftleft < 0)
			leftleft = -leftleft;
		if (right >= len)
			right      = ((len - 1) << 1) - right;
		if (rightright >= len)
			rightright = ((len - 1) << 1) - rightright;

		lf_out[(i >> 1)*inc] = data_in[i * inc] + ((-hf_out[(leftleft >> 1)  * inc] +
							    9 * hf_out[(left >> 1)   * inc] +
							    9 * hf_out[(right >> 1)  * inc] -
							    hf_out[(rightright >> 1) * inc] + 16) >> 5);
	}
	
	return 0;
}

 
int dwt137_multi(dwt_data_t* data_in, int len, int ndecomp, dwt_data_t** lvls_buffers, dwt_data_t* tmp_buffer)
{
	int out_len;
	int decomp;
	int h_lvl;
	dwt_data_t* lf = data_in;
	int lf_size = len;
	for (decomp=0; decomp < ndecomp; decomp++)
	{
		h_lvl = ndecomp-decomp;
		dwt137(lf,lf_size,tmp_buffer,(lf_size+1)/2,&lf_size,lvls_buffers[h_lvl],lf_size/2, &out_len, 1);
		lf = tmp_buffer;
		if (h_lvl == 1)
			memcpy(lvls_buffers[0], tmp_buffer, lf_size*sizeof(dwt_data_t));
	}
	return 0;
}




int idwt137(dwt_data_t* lf_in, int lf_len, dwt_data_t* hf_in, int hf_len,
	   dwt_data_t* data_out, int data_max_len, int* data_len, int inc)
{
	int i;
	int len = lf_len + hf_len;
	
	if (len > data_max_len)
		return -1;

	 
	 
	for(i = 0;i < len;i += 2)
	{
		int left       = (i - 1);
		int leftleft   = (i - 3);
		int right      = (i + 1);
		int rightright = (i + 3);
		if (left < 0)
			left     = -left;
		if (leftleft < 0)
			leftleft = -leftleft;
		if (right >= len)
			right      = ((len - 1) << 1) - right;
		if (rightright >= len)
			rightright = ((len - 1) << 1) - rightright;

		data_out[i * inc] = lf_in[(i >> 1) * inc] - ((-hf_in[(leftleft >> 1) * inc] +
							      9 * hf_in[(left >> 1) * inc] +
							      9 * hf_in[(right >> 1) * inc] -
							      hf_in[(rightright >> 1) * inc] + 16) >> 5);
	}

	 
	 
	for(i = 1;i < len; i+= 2)
	{
		int left       = (i - 1);
		int leftleft   = (i - 3);
		int right      = (i + 1);
		int rightright = (i + 3);
		if (left < 0)
			left     = -left;
		if (leftleft < 0)
			leftleft = -leftleft;
		if (right >= len)
			right      = ((len - 1) << 1) - right;
		if (rightright >= len)
			rightright = ((len - 1) << 1) - rightright;

		data_out[i * inc] = hf_in[(i >> 1)*inc] + ((-data_out[leftleft*inc] +
							    9*data_out[left * inc] +
							    9*data_out[right * inc] -
							    data_out[rightright * inc] + 8) >> 4);
	}

	*data_len = len;
	
	return 0;
}


int idwt137_multi(dwt_data_t** lvls_buffers, int* lvls_sizes, int ndecomp, dwt_data_t* data_out,
		  int out_data_max_len)
{
	dwt_data_t* lf, *hf, *lf_out;
	int lvl, lf_len, hf_len; 
	int out_data_len = 0;
	dwt_data_t* outs[2];
	
	for (lvl=0; lvl<ndecomp+1; lvl++)
		out_data_len += lvls_sizes[lvl];
	
	if (out_data_len > out_data_max_len)
	{
		fprintf(stderr, "(idwt53_multi) programming error: data_out buffer too small (need %d have %d)\n", out_data_len, out_data_max_len);
		return -1;
	}

	outs[0] = data_out;
	outs[1] = data_out + (out_data_len/2);

	lf = lvls_buffers[0];
	lf_len = lvls_sizes[0];
	for (lvl=1; lvl < ndecomp + 1; lvl++)
	{
		hf = lvls_buffers[lvl];
		hf_len = lvls_sizes[lvl];
		 
		lf_out = outs[(ndecomp - lvl) % 2];
		idwt137(lf, lf_len, hf, hf_len, lf_out, out_data_len, &lf_len, 1);
		lf = lf_out;
		 
	}
	return 0;
}
