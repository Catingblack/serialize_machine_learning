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


#ifndef LVLS_MAP_H
#define LVLS_MAP_H

 

#include "../inc/tco.h"

 
 
#define MAX_LVLS_PER_COMP ((MAX_NDECOMP_V+1) * (MAX_NDECOMP_H+1))
#define MAX_LVLS (MAX_LVLS_PER_COMP * MAX_NCOMPS)



 
#define MAX_BANDLINES (MAX_LVLS + 3 * ((1 << (MAX_NDECOMP_V - 1)) - 1))


 
#define MAX_SUBPKTS MAX_BANDLINES


 
typedef enum {
	LL_Band = 0,  
	HL_Band = 1,  
	LH_Band = 2,
	HH_Band = 3
} band_t;

typedef struct lvls_map_t
{
	int n_comps;
	int n_lvls_v;  
	int n_lvls_h[MAX_NDECOMP_V+1];  
	int n_lvls_per_comp;
	int n_positions_per_comp;

	int level_count;     
	int position_count;  
	 
	int lvls_order_map[MAX_BANDLINES];  
	int ypos_order_map[MAX_BANDLINES];  

	 
	int position_from_lvl_ypos[MAX_LVLS][MAX_NDECOMP_V];

	 
	int positions_subpkt[MAX_BANDLINES]; 
	int n_subpkt;

	 
	int v_from_lvl[MAX_LVLS];
	int h_from_lvl[MAX_LVLS];
	 
	int band_widths[MAX_NCOMPS][MAX_LVLS];
	int band_heights[MAX_NCOMPS][MAX_LVLS];
	int lvls_size_max;  	

	 
	int col_sz;
	 
	double col_frac;
	 
	int num_cols;
} lvls_map_t;


 
int fill_lvls_map(tco_conf_t* conf, image_t* im, lvls_map_t* map);


int lvlmap_is_line_present(const lvls_map_t* map, int lvl, int ypos, int prec_idx, int prec_height);

tco_data_in_t* lvlmap_get_ptr(const lvls_map_t* map, const image_t* image, int lvl, int ypos, int prec_idx, int prec_height);
 
tco_data_in_t* lvlmap_get_ptr_by_column(const lvls_map_t* map, const image_t* image, int lvl, int ypos, int prec_idx, int prec_height, int column);

 
extern int band_to_level(const lvls_map_t *map,int component,int res_level,band_t band_type);

 
extern band_t first_band_of_map(const lvls_map_t *map,int reslvl);

 
extern band_t last_band_of_map(const lvls_map_t *map,int reslvl);

 
extern int num_cols(const lvls_map_t *map);

 
extern int width_of_level(const lvls_map_t *map,int lvl);
extern int width_of_level_by_column(const lvls_map_t *map, int lvl, int column);

 
int map_get_N_filtertypes(int ndecomp_h, int ndecomp_v);
 
int map_get_NL(int ncomps, int ndecomp_h, int ndecomp_v);



#endif
