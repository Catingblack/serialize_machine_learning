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
#include <assert.h>

#include "image_transforms.h"
#include "rct.h"
#include "ict.h"
#include "int_funcs.h"
#include "dwt53.h"
#include "dwt137.h"
#include "dwt22.h"




int tco_image_shift_all(image_t* image_in, image_t* image_out, int shift, int rounding)
{
	int comp = 0;
	for (comp = 0; comp < image_in->ncomps; comp++)
		data_change_depth_s32_s32(image_in->comps_array[comp], image_in->w[comp] * image_in->h[comp], image_out->comps_array[comp], shift, rounding);
	return 0;
}


int tco_image_apply_offset(image_t* image_in, image_t* image_out, int offset)
{
	int comp = 0;
	for (comp = 0; comp < image_in->ncomps; comp++)
		data_apply_offset(image_in->comps_array[comp], image_in->w[comp] * image_in->h[comp], image_out->comps_array[comp], offset);
	return 0;
}

int tco_image_clamp(image_t* image_in, image_t* image_out, int low, int high)
{
	int comp = 0;
	int out_len = 0;
	for (comp = 0; comp < image_in->ncomps; comp++)
	{
		int len = image_in->w[comp] * image_in->h[comp];
		data_clamp_s32(image_in->comps_array[comp], len, image_out->comps_array[comp],len,&out_len,low,high);
	}
	return 0;
}


int tco_image_apply_rct(image_t* image_in, image_t* image_out, int rct_type)
{
	int out_len = 0;
	rct_buf(rct_type,
			image_in->comps_array[0],
			image_in->comps_array[1],
			image_in->comps_array[2],
			image_in->w[0] * image_in->h[0],
			image_out->comps_array[0],
			image_out->comps_array[1],
			image_out->comps_array[2],
			image_in->w[0] * image_in->h[0], &out_len);
	return 0;
}

int tco_image_apply_irct(image_t* image_in, image_t* image_out, int rct_type)
{
	int out_len = 0;
	irct_buf(rct_type,
			image_in->comps_array[0],
			image_in->comps_array[1],
			image_in->comps_array[2],
			image_in->w[0] * image_in->h[0],
			image_out->comps_array[0],
			image_out->comps_array[1],
			image_out->comps_array[2],
			image_in->w[0] * image_in->h[0], &out_len);
	return 0;
}


int tco_image_apply_ict(image_t* image_in, image_t* image_out)
{
	int out_len = 0;
	ict_buf(image_in->comps_array[0],
			image_in->comps_array[1],
			image_in->comps_array[2],
			image_in->w[0] * image_in->h[0],
			image_out->comps_array[0],
			image_out->comps_array[1],
			image_out->comps_array[2],
			image_in->w[0] * image_in->h[0], &out_len);
	return 0;
}

int tco_image_apply_iict(image_t* image_in, image_t* image_out)
{
	int out_len = 0;
	iict_buf(image_in->comps_array[0],
			image_in->comps_array[1],
			image_in->comps_array[2],
			image_in->w[0] * image_in->h[0],
			image_out->comps_array[0],
			image_out->comps_array[1],
			image_out->comps_array[2],
			image_in->w[0] * image_in->h[0], &out_len);
	return 0;
}






int tco_image_apply_dwt_hor(image_t* image_in, image_t* image_out, int ndecomp, int kernel, dwt_data_t* tmp_buffer, int tile_width)
{
	int comp = 0;
	int total_len,len;
	int i = 0;
	int line = 0;
	int lvl_sizes[MAX_NDECOMP_H + 1];
	int tile_lvl_sizes[MAX_NDECOMP_H + 1];
	int tile , n_tiles;
	dwt_data_t* data_in;
	dwt_data_t* data_out[MAX_NDECOMP_H + 1];


	assert(image_in != image_out);


	int (*dwt_get_lvl_sizes)(int, int, int*);
	int (*dwt_multi)(dwt_data_t*, int, int, dwt_data_t**, dwt_data_t*);
	switch(kernel)
	{
	case HOR_KERNEL_53:
		dwt_get_lvl_sizes = &dwt53_get_lvl_sizes;
		dwt_multi = &dwt53_multi;
		break;
	case HOR_KERNEL_137:
		dwt_get_lvl_sizes = &dwt137_get_lvl_sizes;
		dwt_multi = &dwt137_multi;
		break;
	default:
		dwt_get_lvl_sizes = &dwt22_get_lvl_sizes;
		dwt_multi = &dwt22_multi;
		break;
	}

	for (comp = 0 ; comp < image_in->ncomps; comp++)
	{
		total_len = image_in->w[comp];
		n_tiles = (tile_width == 0) ? 1 : (total_len + tile_width - 1)/tile_width;

		data_in = image_in->comps_array[comp];
		memset(lvl_sizes , 0, MAX_NDECOMP_H * sizeof(int));

		(*dwt_get_lvl_sizes)(total_len, ndecomp, lvl_sizes);
		data_out[0] = image_out->comps_array[comp];
		for (i=1; i<=MAX_NDECOMP_H; i++)
			data_out[i] = data_out[i-1] + lvl_sizes[i-1];

		for (line = 0; line < image_in->h[comp]; line++)
		{
			for (tile = 0; tile	< n_tiles; tile++)
			{
				if (tile_width == 0)
					len = total_len;
				else if ((total_len % tile_width == 0) || (tile < (n_tiles-1)))
					len = tile_width;
				else
					len = (total_len % tile_width);


				(*dwt_get_lvl_sizes)(len, ndecomp, tile_lvl_sizes);

				(*dwt_multi)(data_in, len, ndecomp, data_out, tmp_buffer);

				data_in += len;
				for (i=0; i<=ndecomp; i++)
					data_out[i] += tile_lvl_sizes[i];
			}

			for (i=0; i<=ndecomp; i++)
				data_out[i] += (total_len - lvl_sizes[i]);
		}
	}
	return 0;
}






int tco_image_apply_partial_dwt_hor(image_t* image_in, image_t* image_out, int ndecomp, int* kernel, dwt_data_t* tmp_buffer,
				    int level)
{

	assert(image_in != image_out);
	int (*dwt_get_lvl_sizes)(int, int, int*);
	for (int comp = 0 ; comp < image_in->ncomps; comp++)
	{
		int (*dwt_multi)(dwt_data_t*, int, int, dwt_data_t**, dwt_data_t*);
		int lvl_sizes[MAX_NDECOMP_H + 1];
		const int len       = image_in->w[comp];
		const int width     = (image_in->w[comp] + (1 << level) - 1) >> level;
		const int height    = (image_in->h[comp] + (1 << level) - 1) >> level;
		dwt_data_t* data_in = image_in->comps_array[comp];
		dwt_data_t* data_out[MAX_NDECOMP_H + 1];

		switch(kernel[comp])
		{
		case HOR_KERNEL_53:
			dwt_get_lvl_sizes = &dwt53_get_lvl_sizes;
			dwt_multi = &dwt53_multi;
			break;
		case HOR_KERNEL_137:
			dwt_get_lvl_sizes = &dwt137_get_lvl_sizes;
			dwt_multi = &dwt137_multi;
			break;
		case HOR_KERNEL_SKIP:
			dwt_get_lvl_sizes = &dwt53_get_lvl_sizes;
			dwt_multi = &dwt_skip_multi;
			break;
		default:
			dwt_get_lvl_sizes = &dwt22_get_lvl_sizes;
			dwt_multi = &dwt22_multi;
			break;
		}

		memset(lvl_sizes , 0, MAX_NDECOMP_H * sizeof(int));

		(*dwt_get_lvl_sizes)(width, ndecomp, lvl_sizes);

		data_out[0] = image_out->comps_array[comp];
		for (int i=1; i<=MAX_NDECOMP_H; i++)
			data_out[i] = data_out[i-1] + lvl_sizes[i-1];

		for (int line = 0; line < height; line++)
		{
			(*dwt_multi)(data_in, width, ndecomp, data_out, tmp_buffer);

			if (width < len)
				memcpy(data_out[0] + width,data_in + width,(len - width) * sizeof(dwt_data_t));

			data_in += len;
			for (int i=0; i<=ndecomp; i++)
				data_out[i] += len;
		}

		if (height < image_in->h[comp])
			memcpy(data_out[0],data_in,(image_in->h[comp] - height) * len * sizeof(dwt_data_t));
	}
	return 0;
}





int tco_image_apply_dwt_ver(image_t* image_in, image_t* image_out, int kernel)
{
	int comp, col, lf_len, hf_len;
	tco_data_in_t *ptr_in, *ptr_lf, *ptr_hf;


	int (*dwt)(dwt_data_t*, int, dwt_data_t*, int, int*, dwt_data_t*, int, int*, int);
	switch(kernel)
	{
	case VER_KERNEL_53:
		dwt = &dwt53;
		break;
	case VER_KERNEL_137:
		dwt = &dwt137;
		break;
	default:
		dwt = &dwt22;
		break;
	}

	for (comp = 0; comp < image_in->ncomps; comp++)
	{
		for (col = 0; col < image_in->w[comp]; col++)
		{
			lf_len = (image_in->h[comp] + 1) / 2;
			hf_len = (image_in->h[comp] - lf_len);

			ptr_in = image_in->comps_array[comp]  + col;
			ptr_lf = image_out->comps_array[comp] + col;
			ptr_hf = image_out->comps_array[comp] + (lf_len * image_in->w[comp] + col);
			(*dwt)(ptr_in, image_in->h[comp], ptr_lf, lf_len, &lf_len, ptr_hf, hf_len, &hf_len, image_in->w[comp]);
		}
	}
	return 0;
}



int tco_image_apply_partial_dwt_ver(image_t* image_in, image_t* image_out, int* kernel, int lvl)
{
	tco_data_in_t *ptr_in, *ptr_lf, *ptr_hf;

	for (int comp = 0; comp < image_in->ncomps; comp++)
	{
		int (*dwt)(dwt_data_t*, int, dwt_data_t*, int, int*, dwt_data_t*, int, int*, int);
		const int width  = (image_in->w[comp] + (1 << lvl) - 1) >> lvl;
		const int height = (image_in->h[comp] + (1 << lvl) - 1) >> lvl;
		const int len    =  image_in->w[comp];

		switch(kernel[comp])
		{
		case VER_KERNEL_53:
			dwt = &dwt53;
			break;
		case VER_KERNEL_137:
			dwt = &dwt137;
			break;
		case VER_KERNEL_SKIP:
			dwt = &dwt_skip;
			break;
		default:
			dwt = &dwt22;
			break;
		}

		for (int col = 0; col < width; col++)
		{
			int lf_len = (height + 1) >> 1;
			int hf_len = (height - lf_len);

			ptr_in = image_in->comps_array[comp]  + col;
			ptr_lf = image_out->comps_array[comp] + col;
			ptr_hf = image_out->comps_array[comp] + (lf_len * len + col);
			(*dwt)(ptr_in, height, ptr_lf, lf_len, &lf_len, ptr_hf, hf_len, &hf_len, len);
		}

		if (width < len)
		{
			for(int row = 0;row < height;row++)
				memcpy(image_out->comps_array[comp] + len * row + width,
				       image_in ->comps_array[comp] + len * row + width,
				       (len - width) * sizeof(tco_data_in_t));

		}
		if (height < image_in->h[comp])
			memcpy(image_out->comps_array[comp] + len * height,
			       image_in->comps_array[comp]  + len * height,
			       (image_in->h[comp] - height) * len * sizeof(tco_data_in_t));
	}
	return 0;
}

int tco_image_apply_idwt_ver(image_t* image_in, image_t* image_out, int kernel)
{
	int comp, col, lf_len, hf_len, len;
	tco_data_in_t *ptr_out, *ptr_lf, *ptr_hf;


	int (*idwt)(dwt_data_t*, int, dwt_data_t*, int, dwt_data_t*, int, int*, int);
	switch(kernel)
	{
	case VER_KERNEL_53:
		idwt = &idwt53;
		break;
	case VER_KERNEL_137:
		idwt = &idwt137;
		break;
	default:
		idwt = &idwt22;
		break;
	}

	for (comp = 0; comp < image_in->ncomps; comp++)
	{
		for (col = 0; col < image_in->w[comp]; col++)
		{
			lf_len = (image_in->h[comp] + 1) / 2;
			hf_len = (image_in->h[comp] - lf_len);

			ptr_lf = image_in->comps_array[comp] + col;
			ptr_hf = image_in->comps_array[comp] + (lf_len * image_in->w[comp] + col);
			ptr_out = image_out->comps_array[comp] + col;

			(*idwt)(ptr_lf, lf_len, ptr_hf, hf_len, ptr_out, image_in->h[comp], &len, image_in->w[comp]);
		}
	}
	return 0;
}


int tco_image_apply_partial_idwt_ver(image_t* image_in, image_t* image_out, int* kernel,int lvl)
{
	tco_data_in_t *ptr_out, *ptr_lf, *ptr_hf;
	for (int comp = 0; comp < image_in->ncomps; comp++)
	{
		int (*idwt)(dwt_data_t*, int, dwt_data_t*, int, dwt_data_t*, int, int*, int);
		const int width  = (image_in->w[comp] + (1 << lvl) - 1) >> lvl;
		const int height = (image_in->h[comp] + (1 << lvl) - 1) >> lvl;
		const int len    = image_in->w[comp];

		switch(kernel[comp])
		{
		case VER_KERNEL_53:
			idwt = &idwt53;
			break;
		case VER_KERNEL_137:
			idwt = &idwt137;
			break;
		case VER_KERNEL_SKIP:
			idwt = &idwt_skip;
			break;
		default:
			idwt = &idwt22;
			break;
		}

		for (int col = 0; col < width;col++)
		{
			const int lf_len = (height + 1) >> 1;
			const int hf_len = (height - lf_len);
			int llen;

			ptr_lf = image_in->comps_array[comp] + col;
			ptr_hf = image_in->comps_array[comp] + (lf_len * len + col);
			ptr_out = image_out->comps_array[comp] + col;

			(*idwt)(ptr_lf, lf_len, ptr_hf, hf_len, ptr_out, height, &llen, len);
		}

		if (width < len)
		{
			for(int row = 0;row < height;row++)
				memcpy(image_out->comps_array[comp] + len * row + width,
				       image_in ->comps_array[comp] + len * row + width,
				       (len - width) * sizeof(tco_data_in_t));

		}
		if (height < image_in->h[comp])
			memcpy(image_out->comps_array[comp] + len * height,
			       image_in->comps_array[comp]  + len * height,
			       (image_in->h[comp] - height) * len * sizeof(tco_data_in_t));
	}
	return 0;
}





int tco_image_apply_idwt_hor(image_t* image_in, image_t* image_out, int ndecomp, int kernel, int tile_width)
{
	int comp = 0;
	int total_len,len;
	int i = 0;
	int line = 0;
	int lvl_sizes[MAX_NDECOMP_H + 1];
	int tile_lvl_sizes[MAX_NDECOMP_H + 1];
	int tile , n_tiles;
	dwt_data_t* data_in[MAX_NDECOMP_H + 1];
	dwt_data_t* data_out;


	int (*dwt_get_lvl_sizes)(int, int, int*);
	int (*idwt_multi)(dwt_data_t**, int*, int, dwt_data_t*, int);
	switch(kernel)
	{
	case HOR_KERNEL_53:
		dwt_get_lvl_sizes = &dwt53_get_lvl_sizes;
		idwt_multi = &idwt53_multi;
		break;
	case HOR_KERNEL_137:
		dwt_get_lvl_sizes = &dwt137_get_lvl_sizes;
		idwt_multi = &idwt137_multi;
		break;
	default:
		dwt_get_lvl_sizes = &dwt22_get_lvl_sizes;
		idwt_multi = &idwt22_multi;
		break;
	}


	assert(image_in != image_out);
	for (comp = 0 ; comp < image_in->ncomps; comp++)
	{
		total_len = image_in->w[comp];
		n_tiles = (tile_width == 0) ? 1 : (total_len + tile_width - 1)/tile_width;

		data_out = image_out->comps_array[comp];
		memset(lvl_sizes, 0, MAX_NDECOMP_H * sizeof(int));
		(*dwt_get_lvl_sizes)(total_len, ndecomp, lvl_sizes);
		data_in[0] = image_in->comps_array[comp];
		for (i=1; i<=MAX_NDECOMP_H; i++)
			data_in[i] = data_in[i-1] + lvl_sizes[i-1];

		for (line = 0; line < image_in->h[comp]; line++)
		{
			for (tile = 0; tile	< n_tiles; tile++)
			{
				if (tile_width == 0)
					len = total_len;
				else if ((total_len % tile_width == 0) || (tile < (n_tiles-1)))
					len = tile_width;
				else
					len = (total_len % tile_width);


				(*dwt_get_lvl_sizes)(len, ndecomp, tile_lvl_sizes);

				(*idwt_multi)(data_in, tile_lvl_sizes, ndecomp, data_out, len);

				for (i=0; i<=ndecomp; i++)
					data_in[i] += tile_lvl_sizes[i];
				data_out += len;
			}

			for (i=0; i<=ndecomp; i++)
				data_in[i] += (total_len - lvl_sizes[i]);
		}
	}
	return 0;
}





int tco_image_apply_partial_idwt_hor(image_t* image_in, image_t* image_out, int ndecomp, int* kernel, int lvl)
{
	assert(image_in != image_out);
	for (int comp = 0 ; comp < image_in->ncomps; comp++)
	{
		int (*dwt_get_lvl_sizes)(int, int, int*);
		int (*idwt_multi)(dwt_data_t**, int*, int, dwt_data_t*, int);
		int lvl_sizes[MAX_NDECOMP_H + 1];
		const int len        = image_in->w[comp];
		const int width      = (image_in->w[comp] + (1 << lvl) - 1) >> lvl;
		const int height     = (image_in->h[comp] + (1 << lvl) - 1) >> lvl;
		dwt_data_t* data_out = image_out->comps_array[comp];
		dwt_data_t* data_in[MAX_NDECOMP_H + 1];

		switch(kernel[comp])
		{
		case HOR_KERNEL_53:
			dwt_get_lvl_sizes = &dwt53_get_lvl_sizes;
			idwt_multi = &idwt53_multi;
			break;
		case HOR_KERNEL_137:
			dwt_get_lvl_sizes = &dwt137_get_lvl_sizes;
			idwt_multi = &idwt137_multi;
			break;
		case HOR_KERNEL_SKIP:
			dwt_get_lvl_sizes = &dwt137_get_lvl_sizes;
			idwt_multi = &idwt_skip_multi;
			break;
		default:
			dwt_get_lvl_sizes = &dwt22_get_lvl_sizes;
			idwt_multi = &idwt22_multi;
			break;
		}

		memset(lvl_sizes, 0, MAX_NDECOMP_H * sizeof(int));
		(*dwt_get_lvl_sizes)(width, ndecomp, lvl_sizes);
		data_in[0] = image_in->comps_array[comp];
		for (int i=1; i<=MAX_NDECOMP_H; i++)
			data_in[i] = data_in[i-1] + lvl_sizes[i-1];

		for (int line = 0; line < height; line++)
		{
			(*idwt_multi)(data_in, lvl_sizes, ndecomp, data_out, len);

			if (width < len)
				memcpy(data_out + width,data_in[0] + width,(len - width) * sizeof(dwt_data_t));

			for (int i=0; i<=ndecomp; i++)
				data_in[i] += len;
			data_out += len;
		}

		if (height < image_in->h[comp])
			memcpy(data_out,data_in[0],(image_in->h[comp] - height) * len * sizeof(dwt_data_t));
	}
	return 0;
}
