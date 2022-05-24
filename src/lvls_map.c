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



#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "lvls_map.h"
#include "dwt53.h"
#include "common.h"


 
 
int map_get_N_filtertypes(int ndecomp_h, int ndecomp_v)
{
	
	return 2*MIN(ndecomp_v, ndecomp_h) + MAX(ndecomp_h, ndecomp_v) + 1;
}
 
int map_get_NL(int ncomps, int ndecomp_h, int ndecomp_v)
{
	return ncomps * map_get_N_filtertypes(ndecomp_h, ndecomp_v);
}


 
int band_to_level(const lvls_map_t *map,int comp,int res_level,band_t band_type)
{
	int one_band_levels =  map->n_lvls_h[0] - map->n_lvls_v;
	 
	if (res_level <= one_band_levels)
	{
		 
		assert(!(band_type == LL_Band && res_level));
		assert(!(res_level == 0 && band_type != LL_Band));

		return comp * map->n_lvls_per_comp + res_level;
	}
	else
	{
		 
		assert(band_type > LL_Band);
		assert(band_type <= HH_Band);

		return comp * map->n_lvls_per_comp + one_band_levels + 1 +
			(res_level - one_band_levels - 1) * 3 + band_type - HL_Band;
	}
}

 
int height_of_band(const lvls_map_t *map,int reslvl)
{
	int one_band_levels =  map->n_lvls_h[0] - map->n_lvls_v;

	if (reslvl <= one_band_levels)
	{
		return 1;
	}
	else
	{
		return 1 << (reslvl - one_band_levels - 1);
	}
}

 
band_t first_band_of_map(const lvls_map_t *map,int reslvl)
{
	if (reslvl == 0)
		return LL_Band;
	else
		return HL_Band;
}

 
band_t last_band_of_map(const lvls_map_t *map,int reslvl)
{
	int one_band_levels =  map->n_lvls_h[0] - map->n_lvls_v;

	if (reslvl == 0)
	{
		return LL_Band;
	}
	else if (reslvl <= one_band_levels)
	{
		return HL_Band;
	}
	else
		return HH_Band;
}

 
int resolutions_of_map(const lvls_map_t *map)
{
	return map->n_lvls_h[0];
}

 
static int to_hlvl(const lvls_map_t* map,int res,int bandtype)
{
	switch(bandtype)
	{
	case LL_Band:
	case LH_Band:
		return 0;
	case HL_Band:
	case HH_Band:
		return res;
	}
	assert(!"invalid band type");
	return 0;
}

int num_cols(const lvls_map_t *map)
{
	return map->num_cols;
}

int divide_into_columns(const lvls_map_t *map, int size, int column_index)
{
	int col_width = (int)(((double)size + 0.5) * map->col_frac);
	if (column_index == (map->num_cols - 1))
	{
		col_width = size - (col_width * column_index);
	}

	return col_width;
}

int width_of_level_by_column(const lvls_map_t *map, int lvl, int column)
{
	int comp = lvl / map->n_lvls_per_comp;
	int hlvl = map->h_from_lvl[lvl];
	int width = 0;

	if (hlvl == 0)
	{
		int lvl_width = width_of_level(map, lvl);
		width = divide_into_columns(map, lvl_width, column);
	}
	else
	{
		width = divide_into_columns(map, map->band_widths[comp][hlvl], column);
	}

	return width;
}

 
int width_of_level(const lvls_map_t *map,int lvl)
{
	int comp  = lvl / map->n_lvls_per_comp;
	int hlvl  = map->h_from_lvl[lvl];
	int width = 0;

	if (hlvl == 0)
	{
		 
		int _lvl;
		for (_lvl = 0; _lvl < map->h_from_lvl[lvl+1]; _lvl++)
			width += map->band_widths[comp][_lvl];
	}
	else
	{
		width = map->band_widths[comp][hlvl];
	}
	return width;
}

static int to_vlvl(const lvls_map_t *map,int res,int bandtype)
{
	int one_band_levels =  map->n_lvls_h[0] - map->n_lvls_v;

	switch(bandtype)
	{
	case LL_Band:
	case HL_Band:
		return 0;
	case LH_Band:
	case HH_Band:
		return res - one_band_levels;
	}
	assert(!"invalid band type");
	return 0;
}

static int to_lvl(const lvls_map_t* map, int comp, int v_lvl, int h_lvl)
{
	return comp * map->n_lvls_per_comp + v_lvl * map->n_lvls_h[0] + h_lvl;
}

int fill_lvls_map(tco_conf_t* conf, image_t* im, lvls_map_t* map)
{
	int subpkt = 0;
	int idx = 0, lvl = 0, res;
	int comp = 0;
	int v_lvl, h_lvl;
	 

	map->n_comps = im->ncomps;
	map->n_lvls_v = conf->ndecomp_v + 1;
	map->n_lvls_per_comp = 0;
	map->n_positions_per_comp = 0;
	for (v_lvl=0; v_lvl<map->n_lvls_v;v_lvl++)
	{
		map->n_lvls_h[v_lvl] = ((v_lvl==0) || (conf->ndecomp_vh == 0)) ? conf->ndecomp_h + 1 : conf->ndecomp_vh + 1;
		map->n_lvls_per_comp += map->n_lvls_h[v_lvl];
		if (v_lvl==0)
			map->n_positions_per_comp = conf->ndecomp_h + 1;
		else
			map->n_positions_per_comp = (map->n_positions_per_comp - 1) + 3 * (1<<(v_lvl-1));
	}
	map->level_count = map->n_lvls_per_comp * map->n_comps;
	map->position_count = map->n_positions_per_comp * map->n_comps;


	 
	 
	map->col_sz = conf->col_sz;
	map->col_frac = (double)map->col_sz / (double)im->w[0];
	map->num_cols = (im->w[0] + map->col_sz - 1) / map->col_sz;
	assert(map->num_cols <= MAX_PREC_COLS);

	for(res = 0; res < map->n_lvls_h[0]; res++)
	{
		int first = first_band_of_map(map,res);
		int last  = last_band_of_map(map,res);
		int band;
		for(band = first; band <= last; band++)
		{
			for(comp = 0; comp < map->n_comps; comp++)
			{
				int lvl = band_to_level(map,comp,res,band);
				map->h_from_lvl[lvl]     = to_hlvl(map,res,band);
				map->v_from_lvl[lvl]     = to_vlvl(map,res,band);
			}
		}
	}

	 

	 
	switch (conf->order) {
	case ORDER_GCVH:
		assert(conf->ndecomp_v <= 1);  
		for (comp=0; comp < map->n_comps; comp++) {
			for (v_lvl=0; v_lvl<map->n_lvls_v; v_lvl++) {
				for (h_lvl = 0; h_lvl < map->n_lvls_h[v_lvl]; h_lvl++)
				{
					lvl = to_lvl(map,	comp, v_lvl, h_lvl);
					map->lvls_order_map[idx] = lvl;
					map->ypos_order_map[idx] = 0;
					map->positions_subpkt[lvl]    = 0;
					idx++;
				}
			}
		}
		subpkt=1;
		break;
	case ORDER_VGCH:
		assert(conf->ndecomp_v <= 1);  
		for (v_lvl=0; v_lvl<map->n_lvls_v; v_lvl++) {
			for (comp=0; comp < map->n_comps; comp++) {
				for (h_lvl = 0; h_lvl < map->n_lvls_h[v_lvl]; h_lvl++)
				{
					lvl = to_lvl(map,	comp, v_lvl, h_lvl);
					map->lvls_order_map[idx] = lvl;
					map->ypos_order_map[idx] = 0;
					map->positions_subpkt[lvl]    = subpkt;
					idx++;
				}
			}
			subpkt++;
		}
		break;
	case ORDER_CVHG:
		assert(conf->ndecomp_v <= 1);  
		for (comp=0; comp < map->n_comps; comp++) {
			for (v_lvl=0; v_lvl<map->n_lvls_v; v_lvl++) {
				for (h_lvl = 0; h_lvl < map->n_lvls_h[v_lvl]; h_lvl++)
				{
					lvl = to_lvl(map,	comp, v_lvl, h_lvl);
					map->lvls_order_map[idx] = lvl;
					map->ypos_order_map[idx] = 0;
					map->positions_subpkt[lvl]    = subpkt++;
					idx++;
				}
			}
		}
		break;
	case ORDER_VGRC:
		 
	    {
			int lastres = resolutions_of_map(map);
			int res;
			for(res = 0; res < lastres; res++)
			{
				 
				int ypos;
				for(ypos = 0; ypos < height_of_band(map,res); ypos++)
				{
					int first = first_band_of_map(map,res);
					int last  = last_band_of_map(map,res);
					int band;
					for(band = first; band <= last; band++)
					{
						for(comp = 0; comp < map->n_comps; comp++)
						{
							int lvl = band_to_level(map,comp,res,band);
							map->lvls_order_map[idx] = lvl;
							map->ypos_order_map[idx] = ypos;
							map->positions_subpkt[idx] = subpkt;
							idx++;
						}
					}
				}
			}
			subpkt=1;
			break;
		}
	 
	case ORDER_RPBC:
	    {			
			int res, ypos;
			int lvl_height;
			int res_lvl0;
			int subpkt0_lvls;

			assert(conf->ndecomp_h >= conf->ndecomp_v);
			
			 
			idx = 0;
			res = 0;
			subpkt = 0;
			subpkt0_lvls = conf->ndecomp_h - conf->ndecomp_v + 1;
			for (lvl=0; lvl < subpkt0_lvls; lvl++)
			{
				for (comp = 0; comp < map->n_comps; comp++)
				{
					 
					map->lvls_order_map[idx] = (comp * map->n_lvls_per_comp) + lvl;
					map->ypos_order_map[idx] = 0;
					map->positions_subpkt[idx] = subpkt;
					idx++;
				}
				res++;
			}
		
			 
			subpkt = 1;
			lvl_height = 1;
			for (res_lvl0 = subpkt0_lvls; res_lvl0 < map->n_lvls_per_comp; res_lvl0 += 3)  
			{
				for (ypos=0; ypos<lvl_height; ypos++)  
				{
					for (lvl = res_lvl0; lvl < res_lvl0+3; lvl++)  
					{
						 
						for (comp = 0; comp < map->n_comps; comp++)  
						{
							map->lvls_order_map[idx] = (comp * map->n_lvls_per_comp) + lvl;
							map->ypos_order_map[idx] = ypos;
							map->positions_subpkt[idx] = subpkt;
							idx++;
						}
						 
						subpkt++;
					}
				}
				 
				res++;
				lvl_height = lvl_height * 2;
			}
		}
		break;
	}

	 
	memset(map->band_heights,0,sizeof(map->band_heights));
	memset(map->band_widths ,0,sizeof(map->band_widths));

	for(comp = 0;comp <  map->n_comps; comp++)
	{
		dwt53_get_lvl_sizes(im->h[comp],map->n_lvls_v - 1   ,map->band_heights[comp]);
		dwt53_get_lvl_sizes(im->w[comp],map->n_lvls_h[0] - 1,map->band_widths[comp]);
	}

	 
	for (lvl = 0; lvl <map->n_lvls_h[0]; lvl++)
		if (map->band_widths[0][lvl] > map->lvls_size_max)
			map->lvls_size_max = map->band_widths[0][lvl];

	map->n_subpkt = subpkt;

	 
	memset(map->position_from_lvl_ypos, 0xFF, sizeof(map->position_from_lvl_ypos));
	for (idx = 0; idx < map->position_count; idx++)
		map->position_from_lvl_ypos[map->lvls_order_map[idx]][map->ypos_order_map[idx]] = idx;
	return 0;
}

int lvlmap_is_line_present(const lvls_map_t* map, int lvl, int ypos, int prec_idx, int prec_height)
{
	int comp, v_lvl;
	int i,last = 0;

	comp = lvl / map->n_lvls_per_comp;
	v_lvl = map->v_from_lvl[lvl];

	for (i = 0; i < v_lvl; i++)
		ypos += map->band_heights[comp][i];
	ypos += prec_idx * prec_height;

	for(i = 0,last = 0;i <= lvl;i++) {
		if (map->h_from_lvl[i] == 0) {
			last += map->band_heights[comp][map->v_from_lvl[i]];
		}
	}


	if (ypos >= last)
		return 0;

	return 1;
}
	
tco_data_in_t* lvlmap_get_ptr_by_column(const lvls_map_t* map, const image_t* image, int lvl, int ypos, int prec_idx, int prec_height, int column)
{
	tco_data_in_t* ptr = NULL;
	int comp, v_lvl, h_lvl;
	int xpos = 0;
	int offset = 0;
	int width;

	comp = lvl / map->n_lvls_per_comp;
	v_lvl = map->v_from_lvl[lvl];
	h_lvl = map->h_from_lvl[lvl];


	for (int i = 0; i < v_lvl; i++)
		ypos += map->band_heights[comp][i];
	ypos += prec_idx * prec_height;

	if (ypos >= image->h[comp])
		return NULL;


	ptr = image->comps_array[comp];
	ptr += ypos * image->w[comp];

	for (int i = 0; i < h_lvl; i++)
		xpos += map->band_widths[comp][i];

	 
	width = width_of_level(map, lvl);
	for (int col = 0; col < column; col++)
	{
		offset += divide_into_columns(map, width, col);
	}


	return ptr + xpos + offset;
}

tco_data_in_t* lvlmap_get_ptr(const lvls_map_t* map, const image_t* image, int lvl, int ypos, int prec_idx, int prec_height)
{
	tco_data_in_t* ptr = NULL;
	int comp, v_lvl, h_lvl, i;
	int xpos = 0;

	assert(num_cols(map) == 1);  

	 
	comp  = lvl / map->n_lvls_per_comp;
	v_lvl = map->v_from_lvl[lvl];
	h_lvl = map->h_from_lvl[lvl];

	 
	for(i = 0;i < v_lvl;i++)
		ypos += map->band_heights[comp][i];
	 
	ypos += prec_idx * prec_height;

	 
	if (ypos >= image->h[comp])
		return NULL;

	 
	ptr  = image->comps_array[comp];
	ptr += ypos * image->w[comp];

	for (i=0; i < h_lvl; i++)
		xpos += map->band_widths[comp][i];

	return ptr + xpos;
}

