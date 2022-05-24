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
#include "common.h"
#include "ict.h"

float ICT_Y_R  =  0.299;
float ICT_Y_G  =  0.587;
float ICT_Y_B  =  0.114;
float ICT_Cb_R =  -0.168736;
float ICT_Cb_G =  -0.331264;
float ICT_Cb_B =  0.5;
float ICT_Cr_R =  0.5;
float ICT_Cr_G =  -0.418688;
float ICT_Cr_B =  -0.081312;

float IICT_R_Y  =  1.000;
float IICT_R_Cb =  0.000;
float IICT_R_Cr =  1.402;
float IICT_G_Y  =  1;
float IICT_G_Cb =  -0.34414;
float IICT_G_Cr =  -0.71414;
float IICT_B_Y  =  1;
float IICT_B_Cb =  1.772;
float IICT_B_Cr =  0.000;

static INLINE void ict_pixel(ict_t r,ict_t g,ict_t b,ict_t* y, ict_t* cb, ict_t* cr)
{
	*y  = (ict_t) roundf( ((ICT_Y_R  * r) + (ICT_Y_G  * g) + (ICT_Y_B  * b)) );
	*cb = (ict_t) roundf( ((ICT_Cb_R * r) + (ICT_Cb_G * g) + (ICT_Cb_B * b)) );
	*cr = (ict_t) roundf( ((ICT_Cr_R * r) + (ICT_Cr_G * g) + (ICT_Cr_B * b)) );
}

static INLINE void iict_pixel(ict_t y, ict_t cb, ict_t cr,ict_t* r,ict_t* g,ict_t* b)
{
	*r = (ict_t) roundf( ((IICT_R_Y * y) + (IICT_R_Cb * (cb)) + (IICT_R_Cr * (cr))) );
	*g = (ict_t) roundf( ((IICT_G_Y * y) + (IICT_G_Cb * (cb)) + (IICT_G_Cr * (cr))) );
	*b = (ict_t) roundf( ((IICT_B_Y * y) + (IICT_B_Cb * (cb)) + (IICT_B_Cr * (cr))) );
}

int ict_buf(ict_t* r,ict_t* g,ict_t* b, int in_len, ict_t* y, ict_t* cb, ict_t* cr, int max_out_len, int* out_len)
{
	 
	int i=0;
	if (max_out_len < in_len)
		return -1;
	*out_len = in_len;

	for (i=0;i<in_len;i++)
	{
		ict_pixel(*r,*g,*b,y,cb,cr);
		r++;g++;b++;
		y++;cb++;cr++;
	}
	return 0;
}

int iict_buf(ict_t* y, ict_t* cb, ict_t* cr,int in_len, ict_t* r,ict_t* g,ict_t* b, int max_out_len, int* out_len)
{
	 
	int i=0;
	if (max_out_len < in_len)
		return -1;
	*out_len = in_len;

	for (i=0;i<in_len;i++)
	{
		iict_pixel(*y,*cb,*cr,r,g,b);
		y++;cb++;cr++;
		r++;g++;b++;
	}
	return 0;
}


 
#ifdef TEST_ICT

ict_t my_data_r[256*256*256];
ict_t my_data_g[256*256*256];
ict_t my_data_b[256*256*256];
ict_t my_data_r_high_pres[256*256*256];
ict_t my_data_g_high_pres[256*256*256];
ict_t my_data_b_high_pres[256*256*256];
ict_t my_data_y[256*256*256];
ict_t my_data_cb[256*256*256];
ict_t my_data_cr[256*256*256];
ict_t my_data_r_out_high_pres[256*256*256];
ict_t my_data_g_out_high_pres[256*256*256];
ict_t my_data_b_out_high_pres[256*256*256];
ict_t my_data_r_out[256*256*256];
ict_t my_data_g_out[256*256*256];
ict_t my_data_b_out[256*256*256];

int main()
{
	int i,j,k,comp;
	int out_len;
	int data_len = 256*256*256;

	ict_t* rgb_orig[]={my_data_r, my_data_g, my_data_b};
	ict_t* rgb_orig_pres[]={my_data_r_high_pres, my_data_g_high_pres, my_data_b_high_pres};
	ict_t* ycbcr[]={my_data_y,my_data_cb,my_data_cr};
	ict_t* rgb_pres[]={my_data_r_out_high_pres,my_data_g_out_high_pres,my_data_b_out_high_pres};
	ict_t* rgb[]={my_data_r_out,my_data_g_out,my_data_b_out};

	fprintf(stderr, "Filling struct\n");
	for (i=0; i<256; i++)
		for (j=0; j<256; j++)
			for (k=0; k<256; k++)
			{
				my_data_r[i*256*256+j*256+k] = (i-128);
				my_data_g[i*256*256+j*256+k] = (j-128);
				my_data_b[i*256*256+j*256+k] = (k-128);
			}

	data_change_depth_s32_s32(my_data_r, data_len, my_data_r_high_pres, 6, 0);
	data_change_depth_s32_s32(my_data_g, data_len, my_data_g_high_pres, 6, 0);
	data_change_depth_s32_s32(my_data_b, data_len, my_data_b_high_pres, 6, 0);

	fprintf(stderr, "ICT\n");
	ict_buf(my_data_r_high_pres, my_data_g_high_pres, my_data_b_high_pres, data_len, my_data_y,my_data_cb,my_data_cr, data_len, &out_len);
	fprintf(stderr, "iICT\n");
	iict_buf(my_data_y,my_data_cb,my_data_cr, data_len, my_data_r_out_high_pres,my_data_g_out_high_pres,my_data_b_out_high_pres, data_len, &out_len);

	data_change_depth_s32_s32(my_data_r_out_high_pres, data_len, my_data_r_out, -6, 1);
	data_change_depth_s32_s32(my_data_g_out_high_pres, data_len, my_data_g_out, -6, 1);
	data_change_depth_s32_s32(my_data_b_out_high_pres, data_len, my_data_b_out, -6, 1);


	for (i=0; i<data_len; i++)
	{
		for (comp=0; comp<3; comp++)
		{
			if (rgb_orig[comp][i]!=rgb[comp][i])
			{
				fprintf(stderr, "FAIL (idx%d)\n", i);
				fprintf(stderr, "RGB Original     : %08X %08X %08X\n", rgb_orig[0][i],rgb_orig[1][i],rgb_orig[2][i]);
				fprintf(stderr, "RGB Precision    : %08X %08X %08X\n", rgb_orig_pres[0][i],rgb_orig_pres[1][i],rgb_orig_pres[2][i]);
				fprintf(stderr, "YCbCr            : %08X %08X %08X\n", ycbcr[0][i],ycbcr[1][i],ycbcr[2][i]);
				fprintf(stderr, "OutRGB Precision : %08X %08X %08X\n", rgb_pres[0][i],rgb_pres[1][i],rgb_pres[2][i]);
				fprintf(stderr, "OutRGB           : %08X %08X %08X\n", rgb[0][i],rgb[1][i],rgb[2][i]);
				return -1;
			}
		}
	}
	fprintf(stderr, "PASS\n");
	return -1;
}

#endif



