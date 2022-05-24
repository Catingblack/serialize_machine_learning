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
#include <string.h>

#include "../inc/tco.h"

#define TCO_IM_MAGIC 0x74636f69
#define MIN(a, b) (((a)>(b))?(b):(a))

int tco_allocate_image(image_t* ptr)
{
	int comp = 0;
	if (ptr->w[0] % 2 && ptr->is_422)
	{
		fprintf(stderr,"This lib do not support 422 with odd image width\n");
		return -1;
	}
	 
	for (comp = 0; comp < ptr->ncomps; comp++)
		ptr->comps_array[comp] = NULL;
	ptr->magic = TCO_IM_MAGIC;
	
	 
	for (comp = 1; comp < ptr->ncomps; comp++)
		ptr->h[comp] = ptr->h[0];

	 
	for (comp = 0; comp < ptr->ncomps; comp++)
	{
		ptr->comps_array[comp] = (tco_data_in_t*) malloc(ptr->w[comp]*ptr->h[comp]*sizeof(tco_data_in_t));
		if (!ptr->comps_array[comp])
			return -1;
	}
	return 0;
}

int tco_free_image(image_t* ptr)
{
	int comp = 0;
	if (ptr->magic == TCO_IM_MAGIC)
	{
		for (comp = 0; comp < ptr->ncomps; comp++)
		{
			if (ptr->comps_array[comp])
				free(ptr->comps_array[comp]);
			ptr->comps_array[comp] = NULL;
		}
	}
	return 0;
}

int tco_print_image_info(image_t* ptr)
{
	if (ptr)
	{
		fprintf(stderr, "Image Geometry\n");
		fprintf(stderr, "ncomp=%d width=%d heigth=%d depth=%d is_422=%d\n", ptr->ncomps,ptr->w[0],ptr->h[0], ptr->depth, ptr->is_422);
	}
	return 0;
}

int tco_image_copy_data(image_t* im_in, image_t* im_out)
{
	int comp, line;
	for (comp=0;comp<im_in->ncomps;comp++)
	{
		tco_data_in_t* ptr_i = im_in->comps_array[comp];
		tco_data_in_t* ptr_o = im_out->comps_array[comp];
		for (line = 0; (line < im_in->h[comp]) && (line < im_out->h[comp]); line++)
		{
			int w = MIN(im_in->w[comp], im_out->w[comp]);
			memcpy(ptr_o, ptr_i, w * sizeof(tco_data_in_t));
			ptr_i+=im_in->w[comp];
			ptr_o+=im_out->w[comp];
		}
	}
	return 0;
}

image_t* tco_clone_image(image_t* im_in, int copy_data)
{
	int i;
	image_t* im_out = (image_t*) malloc(sizeof(image_t));
	if (im_out)
	{
		*im_out = *im_in;
		for (i=0; i<MAX_NCOMPS; i++)
			im_out->comps_array[i] = NULL;
		if (tco_allocate_image(im_out) < 0)
		{
			free(im_out);
			return NULL;
		}
		if (copy_data)
			tco_image_copy_data(im_in, im_out);
	}
	return im_out;
}


int tco_image_dump(image_t* im, char* filename)
{
	int i = 0, comp = 0;
	{
		int min_val, max_val;
		FILE* f_out = fopen(filename, "wb");
		if (f_out == NULL)
			return -1;

		 
		min_val = ~(1 << 31);
		max_val = (1<<31);
		for (comp=0;comp<im->ncomps;comp++)
		{
			for (i = 0; i < im->w[comp] * im->h[comp]; i++)
			{
				if (im->comps_array[comp][i] < min_val)
					min_val = im->comps_array[comp][i];
				if (im->comps_array[comp][i] > max_val)
					max_val = im->comps_array[comp][i];
			}
		}
		if (max_val == min_val)
			max_val = min_val + 1;

		#define SCALE_TO_8BITS(v) (((v-min_val) * 256)/(max_val - min_val))

		 
		if (!im->is_422)
		{
			for (i = 0; i < im->w[0] * im->h[0]; i++)
			{
				uint8_t buf[3];
				buf[0] = SCALE_TO_8BITS(im->comps_array[0][i]);
				buf[1] = SCALE_TO_8BITS(im->comps_array[1][i]);
				buf[2] = SCALE_TO_8BITS(im->comps_array[2][i]);
				fwrite(buf, sizeof(uint8_t), 3, f_out);
			}
		}
		else
		{
			int c;
			for(c = 0;c < im->ncomps;c++)
			{
				for(i = 0;i < im->w[c] * im->h[c]; i++)
				{
					putc(SCALE_TO_8BITS(im->comps_array[c][i]),f_out);
				}
			}
		}
		fclose(f_out);
		return 0;
	}
	return -1;  
}


 
int tco_image_transpose(image_t* in, image_t* out, int only_header)
{
	int comp,line,col;
	tco_data_in_t* ptr_in,*ptr_out;
	
	 
	if ((in==out) && (!only_header))
		return -1;

	 
	for (comp = 0; comp < in->ncomps; comp++)
		if (out->h[comp] * out->w[comp] < in->h[comp] * in->w[comp])
			return -1;

	for (comp = 0; comp < in->ncomps; comp++)
	{
		int h = in->h[comp];
		int w = in->w[comp];

		if (!only_header)
		{
			ptr_in = in->comps_array[comp];
			ptr_out = out->comps_array[comp];

			for (line = 0; line < h; line++)
			{
				for (col = 0; col < w; col++)
				{
					ptr_out[col * h] = ptr_in[col];
				}
				ptr_in+=w;
				ptr_out++;
			}
		}
		 
		out->h[comp] = w;
		out->w[comp] = h;
	}
	return 0;
}

 
int tco_image_horizontal_split(image_t* in, image_t* up, image_t* down)
{
	int comp;

	*up = *in;
	*down = *in;

	for (comp = 0; comp < in->ncomps; comp++)
	{
		up->h[comp] = (in->h[comp] + 1) /2;
		down->h[comp] = in->h[comp] / 2;
		down->comps_array[comp] = in->comps_array[comp] + in->w[comp] * up->h[comp];
	}
	return 0;
}
