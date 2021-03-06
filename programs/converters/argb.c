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
** is provided ???AS IS??? WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING  **
** BUT NOT LIMITED TO, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A   **
** PARTICULAR PURPOSE AND NON-INFRINGMENT OF INTELLECTUAL PROPERTY RIGHTS **
** and (2) neither the Software Copyright Holder (or its affiliates) nor  **
** the ISO shall be held liable in any event for any damages whatsoever   **
** (including, without limitation, damages for loss of profits, business  **
** interruption, loss of information, or any other pecuniary loss)        **
** arising out of or related to the use of or inability to use the        **
** Software.???                                                             **
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
#include <stdlib.h>
#include <string.h>

#include "argb.h"
#include "helpers.h"

int argb_decode(char* filename, image_t* im)
{
	int pix;
	FILE* f_in;
	uint8_t buf[4];
	tco_data_in_t* ptr_R;
	tco_data_in_t* ptr_G;
	tco_data_in_t* ptr_B;
	 

	if ((im->w[0] <= 0) || (im->h[0] <= 0))
	{
		fprintf(stderr, "argb_decode error: unknown image width/height\n");
		return -1;
	}
	
	f_in = fopen(filename, "rb");
	if (f_in == NULL)
		return -1;

	im->is_422 = 0;
	im->ncomps = 3;
	im->w[1] = im->w[2] = im->w[0];
	if (im->depth == -1)
		im->depth = 8;
	if (tco_allocate_image(im) < 0)
		return -1;
	ptr_R = im->comps_array[0];
	ptr_G = im->comps_array[1];
	ptr_B = im->comps_array[2];

	for (pix=0; pix < im->w[0] * im->h[0]; pix++)
	{
		if (fread(buf, sizeof(uint8_t), 4, f_in) == 0)
			break;
		 
		*ptr_R++ = buf[2];
		*ptr_G++ = buf[1];
		*ptr_B++ = buf[0];
	}
	fclose(f_in);
	return 0;
}


 
int argb_encode(char* filename, image_t* im)
{
	int pix;
	FILE* f_out;
	uint8_t buf[4];
	tco_data_in_t* ptr_R = im->comps_array[0];
	tco_data_in_t* ptr_G = im->comps_array[1];
	tco_data_in_t* ptr_B = im->comps_array[2];
	
	f_out = fopen(filename, "wb");
	if (f_out == NULL)
		return -1;

	for (pix=0; pix < im->w[0] * im->h[0]; pix++)
	{
		buf[3] = 0xff;
		buf[2] = (*ptr_R++) >> (im->depth - 8);
		buf[1] = (*ptr_G++) >> (im->depth - 8);
		buf[0] = (*ptr_B++) >> (im->depth - 8);
		fwrite(buf, sizeof(uint8_t), 4, f_out);
	}
	fclose(f_out);
	return 0;
}


#ifdef TEST_ARGB

int main()
{
	char filename[] = "test_argb.tmp";
	int little_endian = 0, pix;
	image_t im;
	image_t im_out;
	im.ncomps = im_out.ncomps = 3;
	im.w[0] = im_out.w[0] = 1920;
	im.w[1] = 1920;
	im.w[2] = 1920;
	im.h[0] = im_out.h[0] = 1080;
	im.is_422 = im_out.is_422 = 0;
	im.depth = 8;
	fprintf(stderr, "Allocating Images\n");
	tco_allocate_image(&im);
	fprintf(stderr, "Filling with dummy data\n");
	for (pix=0; pix < im.w[0]*im.h[0]; pix++)
	{
		im.comps_array[0][pix] = (tco_data_in_t) pix & 0xff;
		im.comps_array[1][pix] = (tco_data_in_t) (pix+1) & 0xff;
		im.comps_array[2][pix] = (tco_data_in_t) (pix+2) & 0xff;
	}
	
	argb_encode(filename, &im);
	fprintf(stderr, "Decoding image\n");
	argb_decode(filename, &im_out);

	fprintf(stderr, "Comparing values\n");
	for (pix=0; pix < im.w[0]*im.h[0]; pix++)
	{
		if ((im_out.comps_array[0][pix] != im.comps_array[0][pix]) || 
			(im_out.comps_array[1][pix] != im.comps_array[1][pix]) || 
			(im_out.comps_array[2][pix] != im.comps_array[2][pix]))
		{
			fprintf(stderr, "TEST FAILED! pixel:%d\n", pix);
			return -1;
		}
	}
	fprintf(stderr, "TEST PASSED!\n");
	return 0;
}

#endif

