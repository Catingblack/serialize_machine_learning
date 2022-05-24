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
#include <stdlib.h>
#include <string.h>

#include "image_open.h"
#include "converters/v210.h"
#include "converters/rgb16.h"
#include "converters/yuv16.h"
#include "converters/yuv16p.h"
#include "converters/yuv8p.h"
#include "converters/uyvy8.h"
#include "converters/argb.h"
#include "converters/mono.h"
#include "converters/ppm.h"
#include "converters/pgx.h"

static int str_endswith(const char* haystack, const char* needle)
{
	size_t hlen;
	size_t nlen;

	hlen = strlen(haystack);
	nlen = strlen(needle);
	if(nlen > hlen) return 0;


	return (strcmp(&haystack[hlen-nlen], needle)) == 0;
}

EXPORTED int image_open_auto(char* filename, image_t* image, int width, int height, int depth)
{
	int ret = -1;
	image->w[0] = width;
	image->h[0] = height;
	image->depth = depth;
	if (str_endswith(filename, ".yuv16le") ||
		str_endswith(filename, ".yuv16"))
	{
		fprintf(stdout, "[image_open] yuv16 interleaved decoder little endian\n");
		ret = yuv16_decode(filename, image, 1);
	}
	else if (str_endswith(filename, ".yuv16be"))
	{
		fprintf(stdout, "[image_open] yuv16 interleaved decoder big endian\n");
		ret = yuv16_decode(filename, image, 0);
	}
	else if (str_endswith(filename, ".yuv"))
	{
		if (image->depth <= 8)
		{
			fprintf(stdout, "[image_open] yuv8p planar decoder\n");
			ret = yuv8p_decode(filename, image);
		}
		else
		{
			fprintf(stdout, "[image_open] yuv16p planar decoder little endian\n");
			ret = yuv16p_decode(filename, image, 1);
		}
	}
	else if (str_endswith(filename, ".yuv16p"))
	{
		fprintf(stdout, "[image_open] yuv16p planar decoder little endian\n");
		ret = yuv16p_decode(filename, image, 1);
	}
	else if (str_endswith(filename, ".v210le") ||
		 	 str_endswith(filename, ".v210") ||
		 	 str_endswith(filename, ".yuv10le"))
	{
		fprintf(stdout, "[image_open] yuv10 decoder little endian\n");
		ret = v210_decode(filename, image, 1  );
	}
	else if (str_endswith(filename, ".v210be") ||
		 	 str_endswith(filename, ".yuv10be") ||
		 	 str_endswith(filename, ".yuv10"))
	{
		fprintf(stdout, "[image_open] yuv10 decoder big endian\n");
		ret = v210_decode(filename, image, 0  );
	}
	else if (str_endswith(filename, ".uyvy") ||
			 str_endswith(filename, ".uyvy8"))
	{
		fprintf(stdout, "[image_open] uyvy8 decoder\n");
		ret = uyvy8_decode(filename, image);
	}
	else if (str_endswith(filename, ".rgb16")   ||
			 str_endswith(filename, ".rgb16be") ||
		 	 str_endswith(filename, ".rgb16le"))
	{
		fprintf(stdout, "[image_open] Using raw rgb16 decoder\n");
		ret = rgb16_decode(filename, image, (!str_endswith(filename, "be")));
	}
	else if (str_endswith(filename, ".argb"))
	{
		fprintf(stdout, "[image_open] Using raw argb decoder\n");
		ret = argb_decode(filename, image);
	}
	else if (str_endswith(filename, ".mono8"))
	{
		fprintf(stdout, "[image_open] Using mono8 decoder\n");
		ret = raw_mono_decode(filename, image);
	}
    else if (str_endswith(filename, ".pgx"))
    {
        fprintf(stdout, "[image_open] Using pgx lib to parse input file\n");
        ret = pgx_decode(filename, image);
    }
	else if (str_endswith(filename, ".ppm") || str_endswith(filename, ".pgm"))
	{
		fprintf(stdout, "[image_open] Using ppm lib to parse input file\n");
		ret = ppm_decode(filename, image);
	}
	else
	{
		fprintf(stderr,"Please use ppm or yuv formats\n");
	}
	if (ret == 0)
	{
        fprintf(stdout, "[image_open] Image: %dx%d %s  with bitdepth=%d\n", image->w[0],image->h[0], (image->is_422) ? ((image->ncomps==4) ? "4224" : "422") : (image->ncomps==1) ? "mono" : ((image->ncomps==4) ? "4444" : "4444"), image->depth);
	}
	return ret;
}

EXPORTED int image_save_auto(char* filename, image_t* image)
{
	int ret = -1;

	if (str_endswith(filename, ".yuv16le") ||
		str_endswith(filename, ".yuv16"))
	{
		if (!image->is_422)
		{
			fprintf(stderr, "We do not support 444 saving with this extension\n");
			return -1;
		}
		ret = yuv16_encode(filename, image, 1);
	}
	else if (str_endswith(filename, ".yuv16be"))
	{
		if (!image->is_422)
		{
			fprintf(stderr, "We do not support 444 saving with this extension\n");
			return -1;
		}
		ret = yuv16_encode(filename, image, 0);
	}
	else if (str_endswith(filename, ".yuv"))
	{
		if (!image->is_422)
		{
			fprintf(stderr, "We do not support 444 saving with this extension\n");
			return -1;
		}
		if (image->depth <= 8)
		{
			ret = yuv8p_encode(filename, image);
		}
		else
		{
			ret = yuv16p_encode(filename, image, 1);
		}
	}
	else if (str_endswith(filename, ".yuv16p"))
	{
		if (!image->is_422)
		{
			fprintf(stderr, "We do not support 444 saving with this extension\n");
			return -1;
		}
		ret = yuv16p_encode(filename, image, 1);
	}
	else if (str_endswith(filename, ".v210le") ||
		 	 str_endswith(filename, ".v210") ||
		 	 str_endswith(filename, ".yuv10le"))
	{
		if (!image->is_422)
		{
			fprintf(stderr, "We do not support 444 saving with this extension\n");
			return -1;
		}
		ret = v210_encode(filename, image, 1  );
	}
	else if (str_endswith(filename, ".v210be") ||
		 	 str_endswith(filename, ".yuv10be") ||
		 	 str_endswith(filename, ".yuv10"))
	{
		if (!image->is_422)
		{
			fprintf(stderr, "We do not support 444 saving with this extension\n");
			return -1;
		}
		ret = v210_encode(filename, image, 0  );
	}
	else if (str_endswith(filename, ".uyvy") ||
			 str_endswith(filename, ".uyvy8"))
	{
		if (!image->is_422)
		{
			fprintf(stderr, "We do not support 444 saving with this extension\n");
			return -1;
		}
		ret = uyvy8_encode(filename, image);
	}
	else if (str_endswith(filename, ".rgb16") ||
			 str_endswith(filename, ".rgb16be") ||
		 	 str_endswith(filename, ".rgb16le"))
	{
		if (image->is_422)
		{
			fprintf(stderr, "We do not support 422 saving with this extension\n");
			return -1;
		}
		ret = rgb16_encode(filename, image, !str_endswith(filename, "be"));
	}
	else if (str_endswith(filename, ".argb"))
	{
		if (image->is_422)
		{
			fprintf(stderr, "We do not support 422 saving with this extension\n");
			return -1;
		}
		ret = argb_encode(filename, image);
	}
	else if (str_endswith(filename, ".mono8"))
	{
		ret = raw_mono_encode(filename, image);
	}
	else if (str_endswith(filename, ".ppm") || str_endswith(filename, ".pgm"))
	{
		if (image->is_422)
		{
			fprintf(stderr, "We do not support 422 saving with this extension\n");
			return -1;
		}
		ret = ppm_encode(filename, image,0);
	}
    else if (str_endswith(filename, ".pgx"))
    {
        ret = pgx_encode(filename, image);
    }
	else
	{
		image_t image2;
		if (image->is_422)
		{

			fprintf(stderr, "Warning: v210/yuv10/yuv16 file format are better for 4:2:2 images!\n");
			fprintf(stderr, "Warning: This 4:2:2 image is upsampled in a 4:4:4 image format.\n");
			fprintf(stderr, "Warning: no colour transform is applied. Output colors will be incorrect\n");
			if (image_upsample(image, &image2) < 0)
				return -1;
		}
		fprintf(stderr,"cannot save %s please use ppm format or yuv\n",filename);
		if (image->is_422)
			tco_free_image(&image2);
	}
	return ret;
}


int image_copy(image_t* image_in, image_t* image_out)
{
	int comp,x,y,off;

	*image_out = *image_in;

	tco_allocate_image(image_in);
	for (comp=0; comp<image_in->ncomps; comp++)
	{
		off = 0;
		for (y=0;y<image_in->h[0];y++)
		{
			for (x=0;x<image_in->w[comp]; x++)
			{
				image_out->comps_array[comp][off] = image_in->comps_array[comp][off];
				off++;
			}
		}
	}
	return 0;
}

int image_upsample(image_t* image_in, image_t* image_out)
{
	int comp,x,y,off;
	if (image_in->is_422 == 0)
	{
		fprintf(stderr, "Trying to upsample an already 4:4:4 image\n");
		return -1;
	}


	*image_out = *image_in;
	image_out->is_422 = 0;
	image_out->w[0] = image_in->w[0];
	image_out->w[1] = image_in->w[1]*2;
	image_out->w[2] = image_in->w[2]*2;

	tco_allocate_image(image_out);
	for (comp=0; comp<image_in->ncomps; comp++)
	{
		off = 0;
		for (y=0;y<image_in->h[0];y++)
		{
			for (x=0;x<image_in->w[comp]; x++)
			{
				int off2 = (comp==0) ? off : 2*off;
				image_out->comps_array[comp][off2++] = image_in->comps_array[comp][off];
				if (comp > 0)
					image_out->comps_array[comp][2*off+1] = image_in->comps_array[comp][off];
				off++;
			}
		}
	}
	return 0;
}

