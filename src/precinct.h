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

#ifndef SRC_PRECINCT_H
#define SRC_PRECINCT_H

#include "buf_mgmt.h"
#include "lvls_map.h"
#include "gcli.h"
#include "../inc/tco.h"

 
#define MAX_PRECINCT_HEIGHT 2

 

struct precinct_struct {
	 
	multi_buf_t *gclis_mb;
	 
	multi_buf_t *sig_mag_data_mb;

	 
	const lvls_map_t *geometry;
	 
	int group_size;
	 
	int idx_from_level[MAX_PRECINCT_HEIGHT][MAX_LVLS];
	 
	int idx;
	 
	int column;
};

typedef struct precinct_struct precinct_t;

 
extern precinct_t *precinct_open_column(const lvls_map_t *geometry, int group_size, int column);
extern precinct_t *precinct_open(const lvls_map_t *geometry,int group_size);

 
extern void precinct_close(precinct_t *prec);

 
extern void copy_gclis(precinct_t *dest,const precinct_t *src);

 
extern void fill_precinct(precinct_t *prec,const image_t *img,int prec_idx);


 
extern void update_image(const precinct_t *prec, image_t *target, int prec_idx);

extern tco_data_in_t* precinct_lvlmap_get_ptr_by_column(const precinct_t *prec, const image_t* image, int lvl, int ypos, int prec_idx, int column);
extern tco_data_in_t* precinct_lvlmap_get_ptr(const precinct_t *prec, const image_t* image, int lvl, int ypos, int prec_idx);


 
extern int component_count_of(const precinct_t *prec);

 
extern band_t first_band_of(const precinct_t *prec,int res_lvl);

 
extern band_t last_band_of(const precinct_t *prec,int res_lvl);

 
extern int band_index_of(const precinct_t *prec,int component,int res_level,band_t band_type);

 
extern int bands_count_of(const precinct_t *prec);

 
extern int line_count_of(const precinct_t *prec);

 
extern sig_mag_data_t *precinct_line_of(const precinct_t *prec,int band_index,int ypos);

 
extern size_t precinct_width_of(const precinct_t *prec,int band_index);

extern int precinct_max_gcli_of(const precinct_t * prec, int lvl, int ypos);

 
extern gcli_data_t *precinct_gcli_of(const precinct_t *prec,int band_index,int ypos);

extern gcli_data_t *precinct_gcli_top_of(const precinct_t *prec, const precinct_t *prec_top, int lvl, int ypos);


 
extern size_t precinct_gcli_width_of(const precinct_t *prec,int band_index);
extern size_t precinct_max_gcli_width_of(const precinct_t *prec);


extern int precinct_get_gcli_group_size(const precinct_t *prec);

 
extern void update_gclis(precinct_t *prec);

 
extern void quantize_precinct(precinct_t *prec,const int *gtli,int dq_type);

 
extern void dequantize_precinct(precinct_t *prec,const int *gtli,int dq_type);



 
int precinct_idx_of(const precinct_t *prec);

 
int precinct_set_idx_of(precinct_t *prec, int idx);

 
void copy_data(precinct_t *dest, const precinct_t *src);

 
int precinct_copy(precinct_t *dest, const precinct_t *src);


 
int precinct_is_first_of_slice(const precinct_t *prec, int slice_height);

 
int precinct_is_last_of_slice(const precinct_t *prec, int slice_height, int im_height);

 
int precinct_is_last_of_image(const precinct_t *prec, int im_height);


 
int precinct_spacial_lines_of(const precinct_t *prec, int im_height);

int precinct_data_checksum_of(precinct_t* prec);

int precinct_is_line_present(precinct_t *prec,int lvl, int ypos);

void dump_precinct_gclis(precinct_t* precinct, precinct_t* precinct_top, int* gtli, int* gtli_top);



 
static INLINE int precinct_lvl_of(const precinct_t* precinct, int position)
{
	return precinct->geometry->lvls_order_map[position];
}

static INLINE int precinct_ypos_of(const precinct_t* precinct, int position)
{
	return precinct->geometry->ypos_order_map[position];
}

static INLINE int precinct_subpkt_of(const precinct_t* precinct, int position)
{
	return precinct->geometry->positions_subpkt[position];
}

static INLINE int precinct_nb_subpkts_of(const precinct_t* precinct)
{
	return precinct->geometry->n_subpkt;
}

static INLINE int precinct_position_of(const precinct_t* precinct, int lvl, int ypos)
{
	 
	return precinct->geometry->position_from_lvl_ypos[lvl][ypos];
}

 
static INLINE int resolution_levels_of(const precinct_t *prec)
{
	return prec->geometry->n_lvls_h[0];
}

 
static INLINE int vertical_levels_of(const precinct_t *prec)
{
	return prec->geometry->n_lvls_v;
}

 
static INLINE int precinct_height_of(const precinct_t *prec,int band_index)
{
	int last_one_band = resolution_levels_of(prec) - vertical_levels_of(prec);
	int height;

	 
	band_index = band_index % prec->geometry->n_lvls_per_comp;
	
	if (band_index <= last_one_band) {
		height = 1;  
	} else {
		 
		height = 1 << ((band_index - last_one_band - 1) / 3);
	}

	return height;
}

#endif
