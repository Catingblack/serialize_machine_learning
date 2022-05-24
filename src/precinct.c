/***************************************************************************
** intoPIX SA, Fraunhofer IIS and Canon Inc. (hereinafter the "Software   **
** Copyright Holder") hold or have the right to license copyright with    **
** respect to the accompanying software (hereinafter the "Software").     **
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

#include "precinct.h"
#include "sig_mag.h"
#include "buf_mgmt.h"
#include "slice.h"
#include "quant.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

	int column;
 
precinct_t *precinct_open(const lvls_map_t *geometry,int group_size)
{
	assert(num_cols(geometry) == 1);
	return precinct_open_column(geometry, group_size, 0);
}

precinct_t *precinct_open_column(const lvls_map_t *geometry, int group_size, int column)
{
	precinct_t *prec = malloc(sizeof(precinct_t));
	if (prec)
	{
		prec->gclis_mb        = NULL;
		prec->sig_mag_data_mb = NULL;
		prec->geometry        = geometry;
		prec->group_size      = group_size;
		prec->sig_mag_data_mb = multi_buf_create(geometry->position_count, sizeof(sig_mag_data_t));
		prec->gclis_mb        = multi_buf_create(geometry->position_count, sizeof(gcli_data_t));
		prec->idx = 0;
		prec->column		  = column;
		if (prec->sig_mag_data_mb && prec->gclis_mb)
		{
			int idx;
			for (idx = 0; idx < geometry->position_count; idx++)
			{
				int size;
				int size_gcli;
				int lvl   = geometry->lvls_order_map[idx];
				int ypos  = geometry->ypos_order_map[idx];

				prec->idx_from_level[ypos][lvl] = idx;

				size = width_of_level_by_column(geometry, lvl, column);
				size_gcli = (size+group_size-1)/group_size;
				
				if (multi_buf_set_size(prec->gclis_mb,idx,size_gcli) < 0)
					goto error;
				if (multi_buf_set_size(prec->sig_mag_data_mb,idx,size) < 0)
					goto error;
			}

			if (multi_buf_allocate(prec->gclis_mb,1) < 0)
				goto error;
			if (multi_buf_allocate(prec->sig_mag_data_mb,group_size) < 0)
				goto error;
			return prec;
		}
	error:		
		precinct_close(prec);
	}
	assert(0);
	return NULL;
}


 
void precinct_close(precinct_t *prec)
{
	if (prec)
	{
		if (prec->gclis_mb)
			multi_buf_destroy(prec->gclis_mb);
		if (prec->sig_mag_data_mb)
			multi_buf_destroy(prec->sig_mag_data_mb);
		free(prec);
	}
}

 
void update_image(const precinct_t *prec,image_t *target,int prec_idx)
{
	int lvl,ypos;
	tco_data_in_t* ptr;

	if (prec_idx < 0)
		prec_idx = precinct_idx_of(prec);

	for (lvl = 0; lvl < prec->geometry->level_count; lvl++)
	{
		int height = precinct_height_of(prec,lvl);
		
		for(ypos = 0;ypos < height;ypos++)
		{
			ptr = lvlmap_get_ptr_by_column(prec->geometry, target, lvl, ypos, prec_idx, height, prec->column);
			if (ptr)
			{
				const sig_mag_data_t *src = precinct_line_of(prec,lvl,ypos);
				int reclen,len            = (int)precinct_width_of(prec,lvl);
				from_sig_mag_buf(src,len,ptr,len,&reclen);
			}
		}
	}
}


tco_data_in_t* precinct_lvlmap_get_ptr_by_column(const precinct_t *prec, const image_t* image, int lvl, int ypos, int prec_idx, int column)
{
	return lvlmap_get_ptr_by_column(prec->geometry, image, lvl, ypos, prec_idx, precinct_height_of(prec, lvl), column);
}

tco_data_in_t* precinct_lvlmap_get_ptr(const precinct_t *prec, const image_t* image, int lvl, int ypos, int prec_idx)
{
	return lvlmap_get_ptr(prec->geometry, image, lvl, ypos, prec_idx, precinct_height_of(prec,lvl));
}

int precinct_is_line_present(precinct_t *prec,int lvl, int ypos)
{
	return lvlmap_is_line_present(prec->geometry,lvl,ypos,prec->idx,precinct_height_of(prec,lvl));
}

void fill_precinct(precinct_t *prec,const image_t *image,int prec_idx)
{
	int lvl,ypos;
	const tco_data_in_t* ptr;
	 
	prec->idx = prec_idx;
	
	for (lvl = 0; lvl < prec->geometry->level_count; lvl++)
	{
		int height = precinct_height_of(prec,lvl);
		
		for(ypos = 0;ypos < height;ypos++)
		{
			ptr = lvlmap_get_ptr_by_column(prec->geometry, image, lvl, ypos, prec_idx, height, prec->column);
			if (ptr)
			{
				sig_mag_data_t *dst = precinct_line_of(prec,lvl,ypos);
				int reclen,len      = (int)precinct_width_of(prec,lvl);
				to_sig_mag_buf(ptr,len,dst,len,&reclen);
			}
			else
			{
				sig_mag_data_t *dst = precinct_line_of(prec,lvl,ypos);
				int len             = (int)precinct_width_of(prec,lvl);
				memset(dst, 0, len * sizeof(sig_mag_data_t));
			}
		}
	}
}

int precinct_idx_of(const precinct_t *prec)
{
	return prec->idx;
}

int precinct_set_idx_of(precinct_t *prec, int idx)
{
	prec->idx = idx;
	return idx;
}

 
int component_count_of(const precinct_t *prec)
{
	return prec->geometry->n_comps;
}

 
band_t first_band_of(const precinct_t *prec,int res_lvl)
{
	return first_band_of_map(prec->geometry,res_lvl);
}

 
band_t last_band_of(const precinct_t *prec,int res_lvl)
{
	return last_band_of_map(prec->geometry,res_lvl);
}

 
int band_index_of(const precinct_t *prec,int component,int res_level,band_t band_type)
{
	return band_to_level(prec->geometry,component,res_level,band_type);
}


 
int bands_count_of(const precinct_t *prec)
{
	return prec->geometry->level_count;
}

 
int line_count_of(const precinct_t *prec)
{
	return prec->geometry->position_count;
}

 
sig_mag_data_t *precinct_line_of(const precinct_t *prec,int band_index,int ypos)
{
	int idx = prec->idx_from_level[ypos][band_index];
	
	return prec->sig_mag_data_mb->bufs.u32[idx];
}

 
size_t precinct_width_of(const precinct_t *prec,int band_index)
{
	int idx = prec->idx_from_level[0][band_index];
	
	return prec->sig_mag_data_mb->sizes[idx];
}

 
gcli_data_t *precinct_gcli_of(const precinct_t *prec,int band_index,int ypos)
{
	int idx = prec->idx_from_level[ypos][band_index];

	return prec->gclis_mb->bufs.s8[idx];
}

gcli_data_t *precinct_gcli_top_of(const precinct_t *prec, const precinct_t *prec_top, int band_index,int ypos)
{
	const precinct_t *prec_above = (ypos == 0) ? (prec_top) : (prec);
	int ylast = (ypos == 0) ? (precinct_height_of(prec, band_index) - 1) : (ypos - 1);
	if (prec_above)
		return precinct_gcli_of(prec_above, band_index, ylast);
	else
		return NULL;
}

int precinct_get_gcli_group_size(const precinct_t *prec)
{
	return prec->group_size;
}

int precinct_max_gcli_of(const precinct_t * prec, int band_index, int ypos)
{
	int idx = prec->idx_from_level[ypos][band_index];
	int i;
	static int max[36] = {0};

	for ( i = 0; i< precinct_gcli_width_of(prec,band_index); i++)
	{
		if (prec->gclis_mb->bufs.s16[idx][i] > max[band_index])
		{
			max[band_index] = prec->gclis_mb->bufs.s16[idx][i];
			fprintf(stderr, "lvl%d max=%d\n", band_index, max[band_index]);
		}
	}
	return max[band_index];
}


 
size_t precinct_gcli_width_of(const precinct_t *prec,int band_index)
{
	assert(band_index >= 0);
	assert(band_index < prec->geometry->level_count);

	int idx = prec->idx_from_level[0][band_index];

	assert(idx >= 0);
	assert(idx < prec->gclis_mb->n_buffers);	

	return prec->gclis_mb->sizes[idx];
}

size_t precinct_max_gcli_width_of(const precinct_t *prec)
{
	return prec->geometry->lvls_size_max;
}

 
void update_gclis(precinct_t *prec)
{
	int band,ypos;

	for(band = 0;band < bands_count_of(prec);band++)
	{
		for(ypos = 0;ypos < precinct_height_of(prec,band);ypos++)
		{
			const sig_mag_data_t *in = precinct_line_of(prec,band,ypos);
			gcli_data_t *dst         = precinct_gcli_of(prec,band,ypos);
			int reclen,width         = (int)precinct_width_of(prec,band);
			int gcli_count           = (int)precinct_gcli_width_of(prec,band);
			compute_gcli_buf(in,width,dst,gcli_count,&reclen,prec->group_size);
		}
	}
}

 
void copy_gclis(precinct_t *dest,const precinct_t *src)
{
	int band,ypos;

	assert(bands_count_of(dest) == bands_count_of(src));
	
	for(band = 0;band < bands_count_of(dest);band++)
	{
		assert(precinct_height_of(dest,band) == precinct_height_of(src,band));
		
		for(ypos = 0;ypos < precinct_height_of(dest,band);ypos++)
		{
			int gcli_count         = (int)precinct_gcli_width_of(dest,band);
			const gcli_data_t *in  = precinct_gcli_of(src,band,ypos);
			gcli_data_t *out       = precinct_gcli_of(dest,band,ypos);
			
			assert(gcli_count == precinct_gcli_width_of(src,band));
			memcpy(out,in,gcli_count * sizeof(gcli_data_t));
		}
	}
}

void dump_precinct_gclis(precinct_t* precinct, precinct_t* precinct_top, int* gtli, int* gtli_top)
{
	int band,ypos,i;
	static FILE* f = NULL;
	
	if (!f)
	{
		f = fopen("gclis.log","wb");
		fprintf(f, "subband, gcli, gcli_top, gtli, gtli_top\n");
	}

	for(band = 0;band < bands_count_of(precinct);band++)
	{	
		for(ypos = 0;ypos < precinct_height_of(precinct,band);ypos++)
		{
			int gcli_count         = (int)precinct_gcli_width_of(precinct,band);
			gcli_data_t *gclis     = precinct_gcli_of(precinct,band,ypos);
			gcli_data_t *gclis_top = precinct_gcli_of(precinct_top,band,ypos);
			for ( i = 0 ; i < gcli_count; i++)
			{
				fprintf(f, "%d, %d, %d, %d, %d\n",band,*gclis,*gclis_top,gtli[band],gtli_top[band]);
				gclis++;
				gclis_top++;
			}
		}
	}
	return;
}


 
void quantize_precinct(precinct_t *prec,const int *gtli,int dq_type)
{
	int band,ypos;

	for(band = 0;band < bands_count_of(prec);band++)
	{
		for(ypos = 0;ypos < precinct_height_of(prec,band);ypos++)
		{
			sig_mag_data_t *data = precinct_line_of(prec,band,ypos);
			size_t width         = precinct_width_of(prec,band);
			gcli_data_t *gclis   = precinct_gcli_of(prec,band,ypos);

			quant(data,(int)width,gclis,prec->group_size,gtli[band],dq_type);
		}
	}
}

 
void dequantize_precinct(precinct_t *prec,const int *gtli,int dq_type)
{
	int band,ypos;

	for(band = 0;band < bands_count_of(prec);band++)
	{
		for(ypos = 0;ypos < precinct_height_of(prec,band);ypos++)
		{
			sig_mag_data_t *data = precinct_line_of(prec,band,ypos);
			size_t width         = precinct_width_of(prec,band);
			gcli_data_t *gclis   = precinct_gcli_of(prec,band,ypos);

			dequant(data,(int)width,gclis,prec->group_size,gtli[band],dq_type);
		}
	}
}


 
void copy_data(precinct_t *dest,const precinct_t *src)
{
	int band,ypos;

	assert(bands_count_of(dest) == bands_count_of(src));
	
	for(band = 0;band < bands_count_of(dest);band++)
	{
		assert(precinct_height_of(dest,band) == precinct_height_of(src,band));
		
		for(ypos = 0;ypos < precinct_height_of(dest,band);ypos++)
		{
			int count            = (int)precinct_width_of(dest,band);
			const sig_mag_data_t *in  = precinct_line_of(src,band,ypos);
			sig_mag_data_t *out       = precinct_line_of(dest,band,ypos);
			
			assert(count == precinct_width_of(src,band));
			memcpy(out,in,count * sizeof(sig_mag_data_t));
		}
	}
}

int precinct_copy(precinct_t *dest,const precinct_t *src)
{
	dest->idx = src->idx;
	copy_gclis(dest,src);
	copy_data(dest,src);
	return 0;
}


 
int precinct_is_first_of_slice(const precinct_t *prec, int slice_height)
{
	int precheight = 1 << (prec->geometry->n_lvls_v - 1);
	if (slice_height > 0)
	{
		return (((prec->idx*precheight) % slice_height) == 0);
	}
	else
	{
		return (prec->idx == 0);
	}
}

 
int precinct_is_last_of_slice(const precinct_t *prec, int slice_height, int im_height)
{
	int ret;
	int precheight = 1 << (prec->geometry->n_lvls_v - 1);
	ret = is_last_of_slice(prec->idx, slice_height, im_height, precheight);
	 
	return ret;
}

 
int precinct_is_last_of_image(const precinct_t *prec, int im_height)
{
	int ret;
	int precheight = 1 << (prec->geometry->n_lvls_v - 1);
	ret = ((im_height + precheight - 1) >> (prec->geometry->n_lvls_v - 1)) == (prec->idx+1);
	 
	return ret;
}

 
int precinct_spacial_lines_of(const precinct_t *prec, int im_height)
{
	int ret = 0;
	int precheight = 1 << (prec->geometry->n_lvls_v - 1);
	int leftover = (im_height % precheight);

	if ((!precinct_is_last_of_image(prec, im_height)) || (leftover == 0))
		ret = precheight;
	else
		ret = leftover;
	 
	return ret;
}

uint16_t Fletcher16( uint8_t *data, int count )
{
   uint16_t sum1 = 0;
   uint16_t sum2 = 0;
   int index;
   for( index = 0; index < count; ++index )
   {
      sum1 = (sum1 + data[index]) % 255;
      sum2 = (sum2 + sum1) % 255;
   }

   return (sum2 << 8) | sum1;
}

int precinct_data_checksum_of(precinct_t* prec)
{
	int lvl,ypos;
	int checksum = 0;
	for (lvl = 0; lvl < prec->geometry->level_count; lvl++)
	{
		int height = precinct_height_of(prec,lvl);		
		for(ypos = 0;ypos < height;ypos++)
		{
			const sig_mag_data_t *src = precinct_line_of(prec,lvl,ypos);
			int len                   = (int)precinct_width_of(prec,lvl);

			 
			 
			 
			 
			 
			 
			 
			 
			checksum+= Fletcher16((uint8_t*)src,len * sizeof(sig_mag_data_t));
			 
		}
	}
	return checksum;
}

int precinct_compare(precinct_t* prec1, precinct_t* prec2)
{
	int lvl,ypos;
	for (lvl = 0; lvl < prec1->geometry->level_count; lvl++)
	{
		int height = precinct_height_of(prec1,lvl);		
		for(ypos = 0;ypos < height;ypos++)
		{
			const sig_mag_data_t *src1 = precinct_line_of(prec1,lvl,ypos);
			const sig_mag_data_t *src2 = precinct_line_of(prec2,lvl,ypos);
			int len                   = (int)precinct_width_of(prec1,lvl);
			fprintf(stderr, "%d %d %d\n", lvl, ypos, memcmp((char*)src1, (char*)src2, len*sizeof(sig_mag_data_t)));
		}
	}
	return 0;
}
