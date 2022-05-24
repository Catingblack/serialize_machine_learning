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


#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "int_funcs.h"
#include "common.h"

int data_apply_offset(int32_t* data_in, int len, int32_t* data_out, int offset)
{
	int i;
	for (i=0; i<len; i++)
		*data_out++ = *data_in++ + offset;
	return 0;
}

int data_shift_u_s_u32_s32(uint32_t* data_in, int len, int32_t* data_out, int shift_value)
{
	int i;
	for (i=0; i<len; i++)
		*data_out++ = (int32_t) *data_in++ - shift_value;
	return 0;
}

int data_shift_s_u_s32_u32(int32_t* data_in, int len, uint32_t* data_out, int shift_value)
{
	int i;
	for (i=0; i<len; i++)
		*data_out++ = (uint32_t) *data_in++ + shift_value;
	return 0;
}

int data_shift_u_s(uint16_t* data_in, int len, int16_t* data_out, int shift_value)
{
	int i;
	for (i=0; i<len; i++)
		*data_out++ = (int16_t) *data_in++ - shift_value;
	return 0;
}

int data_shift_s_u(int16_t* data_in, int len, uint16_t* data_out,int shift_value)
{
	int i;
	for (i=0; i<len; i++)
		*data_out++ = (uint16_t) *data_in++ + shift_value;
	return 0;
}

#define DATA_CLAMP_CORE \
	int i; \
	for (i=0; i<len; i++) \
	{ \
		if (*data_in > max_value) \
			*data_out = max_value; \
		else if (*data_in < min_value) \
			*data_out = min_value; \
		else \
			*data_out = *data_in; \
		data_out++; \
		data_in++; \
	} \
	return 0;

int data_clamp(int16_t* data_in, int len, int16_t* data_out, int max_len_out, int* out_len, int16_t min_value, int16_t max_value)
{
	DATA_CLAMP_CORE;
}

int data_clamp_s32(int32_t* data_in, int len, int32_t* data_out, int max_len_out, int* out_len, int32_t min_value, int32_t max_value)
{
	DATA_CLAMP_CORE;
}


#define DATA_CHANGE_DEPTH_CORE \
int i; \
if (delta_depth > 0) \
{ \
	  \
	for (i=0; i<len; i++) \
	{ \
		data_out[i] = data_in[i] << delta_depth; \
		if (data_out[i] && rounding) \
			data_out[i] |= (1<<(delta_depth-1)); \
	} \
} \
else if (delta_depth < 0)\
{ \
	  \
	for (i=0; i<len; i++) \
	{ \
		if (rounding) \
			data_out[i] =  (data_in[i] + (1 << (-delta_depth-1))) >> (-delta_depth); \
		else \
			data_out[i] = data_in[i] >> (-delta_depth); \
	} \
} \
else   \
{ \
	for (i=0; i<len; i++) \
		data_out[i] = data_in[i]; \
} \
return 0;


int data_change_depth_u32_u32(uint32_t* data_in, int len, uint32_t* data_out, int delta_depth, int rounding)
{
	DATA_CHANGE_DEPTH_CORE
}

int data_change_depth_s32_s32( int32_t* data_in, int len,  int32_t* data_out, int delta_depth, int rounding)
{
	DATA_CHANGE_DEPTH_CORE
}

int data_change_depth(uint16_t* data_in , int len,  uint16_t* data_out, int delta_depth, int rounding)
{
	DATA_CHANGE_DEPTH_CORE
}


int data_sum_abs(int32_t* data_in, int len)
{
	int sum = 0;
	int i;
	for (i=0; i<len; i++)
		sum+=ABS(data_in[i]);
	return sum;
}

