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
#include "rct.h"


static INLINE void rct_pixel(rct_t r,rct_t g,rct_t b,rct_t* y, rct_t* cb, rct_t* cr)
{
	*y = (rct_t) (r+2*g+b)>>2;
	*cb = (rct_t) b-g;  
	*cr = (rct_t) r-g;  
}

static INLINE void irct_pixel(rct_t y, rct_t cb, rct_t cr,rct_t* r,rct_t* g,rct_t* b)
{
	*g = (rct_t) (y-((cb+cr)>>2));
	*r = (rct_t) (*g + cr);
	*b = (rct_t) (*g + cb);
}

static INLINE void ycocg_pixel(rct_t r,rct_t g,rct_t b,rct_t* y, rct_t* co, rct_t* cg)
{
	rct_t t;

	*co = r - b;
	t = b + ((*co)>>1);
	*cg = g - t;
	*y = t + ((*cg)>>1);
}

static INLINE void iycocg_pixel(rct_t y, rct_t co, rct_t cg,rct_t* r,rct_t* g,rct_t* b)
{
	rct_t t;

	t = y-(cg>>1);
	*g = t + cg;
	*b = t - (co>>1);
	*r = *b + co;
}


int rct_buf(int rct_type, rct_t* r,rct_t* g,rct_t* b, int in_len, rct_t* y, rct_t* cb, rct_t* cr, int max_out_len, int* out_len)
{
	 
	int i=0;
	if (max_out_len < in_len)
		return -1;
	*out_len = in_len;

	for (i=0;i<in_len;i++)
	{
		if (rct_type == RCT_TYPE_YCBCR)
			rct_pixel(*r,*g,*b,y,cb,cr);
		else
			ycocg_pixel(*r,*g,*b,y,cb,cr);
		r++;g++;b++;
		y++;cb++;cr++;
	}
	return 0;
}

int irct_buf(int rct_type, rct_t* y, rct_t* cb, rct_t* cr,int in_len, rct_t* r,rct_t* g,rct_t* b, int max_out_len, int* out_len)
{
	 
	int i=0;
	if (max_out_len < in_len)
		return -1;
	*out_len = in_len;

	for (i=0;i<in_len;i++)
	{
		if (rct_type == RCT_TYPE_YCBCR)
			irct_pixel(*y,*cb,*cr,r,g,b);
		else
			iycocg_pixel(*y,*cb,*cr,r,g,b);
		y++;cb++;cr++;
		r++;g++;b++;
	}
	return 0;
}


#ifdef TEST_RCT

rct_t my_data_r[256*256*256];
rct_t my_data_g[256*256*256];
rct_t my_data_b[256*256*256];
rct_t my_data_y[256*256*256];
rct_t my_data_cb[256*256*256];
rct_t my_data_cr[256*256*256];
rct_t my_data_r_out[256*256*256];
rct_t my_data_g_out[256*256*256];
rct_t my_data_b_out[256*256*256];

int main()
{
	int i,j,k,comp;
	int out_len;
	int data_len = 256*256*256;
	
	rct_t* rgb_orig[]={my_data_r, my_data_g, my_data_b};
	rct_t* ycbcr[]={my_data_y,my_data_cb,my_data_cr};
	rct_t* rgb[]={my_data_r_out,my_data_g_out,my_data_b_out};

	fprintf(stderr, "Filling struct\n");
	for (i=0; i<256; i++)
		for (j=0; j<256; j++)
			for (k=0; k<256; k++)
			{
				my_data_r[i*256*256+j*256+k] = i-128;
				my_data_g[i*256*256+j*256+k] = j-128;
				my_data_b[i*256*256+j*256+k] = k-128;
			}

	fprintf(stderr, "RCT\n");
	rct_buf(RCT_TYPE_YCBCR, my_data_r, my_data_g, my_data_b, data_len, my_data_y,my_data_cb,my_data_cr, data_len, &out_len);
	fprintf(stderr, "iRCT\n");
	irct_buf(RCT_TYPE_YCBCR, my_data_y,my_data_cb,my_data_cr, data_len, my_data_r_out,my_data_g_out,my_data_b_out, data_len, &out_len);

	for (i=0; i<data_len; i++)
	{
		for (comp=0; comp<3; comp++)
		{
			if (rgb_orig[comp][i]!=rgb[comp][i])
			{
				fprintf(stderr, "FAIL\n");
				fprintf(stderr, "%d %d %d -> %d %d %d -> %d %d %d\n", rgb_orig[0][i],rgb_orig[1][i],rgb_orig[2][i],ycbcr[0][i],ycbcr[1][i],ycbcr[2][i],rgb[0][i],rgb[1][i],rgb[2][i]);
				return -1;
			}
		}
	}
	fprintf(stderr, "PASS\n");
	return -1;
}

#endif



