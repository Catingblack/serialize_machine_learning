#include <iostream>
#include <fstream>
#include <string>

//Test git


struct rate_control_t
{
	tco_conf_t conf;
	int image_height;
	precinct_t* precinct;
	precinct_budget_table_t* pbt;
	predbuffer_t* pred_residuals;
	precinct_t* precinct_top;
	uint64_t gc_enabled_modes;

	int nibbles_image;
	int nibbles_report;
	int nibbles_consumed;
	int lines_consumed;

	ra_params_t ra_params;
	int* gtli_table_data;
	int* gtli_table_gcli;
	int* gtli_table_gcli_prec;
	int gcli_methods_table;
	int* gcli_sb_methods;


	precinct_budget_info_t* pbinfo;

	int scenario2gtli_table[MAX_GCLI + 1][MAX_LVLS];
	struct budget_dump_struct_t* budget_dump_handle;
};




struct directional_prediction_struct {

	multi_buf_t* gclis_mb[MAX_GCLI + 1];
	multi_buf_t* predictors_mb[MAX_GCLI + 1];


	int idx_from_level[MAX_PRECINCT_HEIGHT][MAX_LVLS];
};

struct predbuffer_struct {

	directional_prediction_t direction[PRED_COUNT];
};

struct budget_dump_struct_t {
	FILE* output_file;
	int n_lvls;
	int max_column_size;

	int scenario2gtli_table[MAX_GCLI + 1][MAX_LVLS];
	const uint8_t* sb_priority;


};

//serialize for ptr

void ser_int(std::ostream& stream, int* ptr) {

	stream << '\n' << "*POINTER" << '\n';
	stream << '/n';
	if (ptr == nullptr) {
		stream << "null" << ' ';
	}
	else {
		stream << *ptr << ' ';
	}
	stream << '/n';
}


void ser_uint8_t(std::ostream& stream, uint8_t* ptr) {

	stream << '\n' << "*POINTER" << '/n';
	stream << '/n';
	if (ptr == nullptr) {
		stream << "null" << ' ';
	}
	else {
		stream << *ptr << ' ';
	}
	stream << '/n';
}

void ser_uint8_t_const(std::ostream& stream, const uint8_t* ptr) {

	stream << '\n' << "*POINTER" << '/n';
	stream << '/n';
	if (ptr == nullptr) {
		stream << "null" << ' ';
	}
	else {
		stream << *ptr << ' ';
	}
	stream << '/n';
}



//



// Third class functions
//

void ser_multi_buf_t(std::ostream& stream, multi_buf_t* gclis_mb) {

	stream << '\n' << "^^^MULTI_BUF_T" << '\n';

	//magic
	stream << gclis_mb->magic << ' ';
	//n_buffers
	stream << gclis_mb->n_buffers << ' ';
	//int* sizes
	ser_int(stream, gclis_mb->sizes);
	//int* sizes_byte
	ser_int(stream, gclis_mb->sizes_byte);
	//element_size
	stream << gclis_mb->element_size << ' ';

	//union bufs todo
	
	//int8_t** s8
	//uint8_t** u8
	//int16_t** s16
	//uint16_t**
	//int32_t** s32
	//uint32_t** u32

	stream << '\n' << "BUFS" << '\n';
	stream << **(gclis_mb->bufs.u32) << '\n';
	
	//FILE** fout todo nullptr

	//dump_mode
	stream << gclis_mb->dump_mode << ' ';

	//void* storage todo

	stream << '\n';

}



void ser_lvls_map_t(std::ostream& stream, const lvls_map_t* geometry) {

	stream << '\n' << "^^^LVLS_MAP_T" << '\n';

	//n_comps
	stream << geometry->n_comps << ' ';
	//n_lvls_v
	stream << geometry->n_lvls_v << ' ';
	
	//n_lvls_h[MAX_NDECOMP_V + 1]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_NDECOMP_V + 1; i++) {
		stream << geometry->n_lvls_h[i] << ' ';
	}
	stream << '/n';

	//n_lvls_per_comp
	stream << geometry->n_lvls_per_comp << ' ';
	//n_positions_per_comp
	stream << geometry->n_positions_per_comp << ' ';
	//level_count
	stream << geometry->level_count << ' ';
	//position_count
	stream << geometry->position_count << ' ';

	//lvls_order_map[MAX_BANDLINES]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_BANDLINES; i++) {
		stream << geometry->lvls_order_map[i] << ' ';
	}
	stream << '/n';

	//ypos_order_map[MAX_BANDLINES]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_BANDLINES; i++) {
		stream << geometry->ypos_order_map[i] << ' ';
	}
	stream << '/n';


	//position_from_lvl_ypos[MAX_LVLS][MAX_NDECOMP_V]
	stream << '\n' << "ArrArr:" << ' ';
	for (int i = 0; i < MAX_LVLS; i++) {
		for (int j = 0; j < MAX_NDECOMP_V; j++) {
			stream << geometry->position_from_lvl_ypos[i][j] << ' ';
		}
	}
	stream << '/n';

	//positions_subpkt[MAX_BANDLINES]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_BANDLINES; i++) {
		stream << geometry->positions_subpkt[i] << ' ';
	}
	stream << '/n';

	//n_subpkt
	stream << geometry->n_subpkt << ' ';

	//v_from_lvl[MAX_LVLS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_LVLS; i++) {
		stream << geometry->v_from_lvl[i] << ' ';
	}
	stream << '/n';

	//h_from_lvl[MAX_LVLS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_LVLS; i++) {
		stream << geometry->h_from_lvl[i] << ' ';
	}
	stream << '/n';

	//band_widths[MAX_NCOMPS][MAX_LVLS]
	stream << '\n' << "ArrArr:" << ' ';
	for (int i = 0; i < MAX_NCOMPS; i++) {
		for (int j = 0; j < MAX_LVLS; j++) {
			stream << geometry->band_widths[i][j] << ' ';
		}
	}
	stream << '/n';

	//band_heights[MAX_NCOMPS][MAX_LVLS]
	stream << '\n' << "ArrArr:" << ' ';
	for (int i = 0; i < MAX_NCOMPS; i++) {
		for (int j = 0; j < MAX_LVLS; j++) {
			stream << geometry->band_heights[i][j] << ' ';
		}
	}
	stream << '/n';

	//lvls_size_max
	stream << geometry->lvls_size_max << ' ';
	//col_sz
	stream << geometry->col_sz << ' ';
	//col_frac
	stream << geometry->col_frac << ' ';
	//num_cols
	stream << geometry->num_cols << ' ';

	stream << '\n';

}



//
//



// Second class functions
//


void ser_tco_conf_t(std::ostream& stream, tco_conf_t* conf) {

	stream << '\n' << "^^TCO_CONF_T" << '\n';
	
	stream << conf->bitstream_nbytes << ' ';
	stream << conf->bpp << ' ';
	stream << conf->profile << ' ';
	stream << conf->group_size << ' ';
	stream << conf->kernel_h << ' ';
	stream << conf->ndecomp_h << ' ';
	stream << conf->ndecomp_vh << ' ';
	stream << conf->skip_wavelet_on_comp << ' ';
	stream << conf->rct << ' ';
	stream << conf->rct_type << ' ';
	stream << conf->cbr << ' ';
	stream << conf->haar_use_shift << ' ';
	stream << conf->col_sz << ' ';
	stream << conf->col_max_width << ' ';
	stream << conf->col_cnt << ' ';
	stream << conf->col_granularity << ' ';
	stream << conf->cols_ra_opt << ' ';
	stream << conf->gc_raw << ' ';
	stream << conf->gc_bounded << ' ';
	stream << conf->gc_pred_per_sb << ' ';
	stream << conf->gc_run_per_sb << ' ';
	stream << conf->gc_ver << ' ';
	stream << conf->gc_nopred << ' ';
	stream << conf->gc_run_none << ' ';
	stream << conf->gc_run_sigflags_zrf << ' ';
	stream << conf->gc_run_sigflags_zrcsf << ' ';
	stream << conf->gc_trunc << ' ';
	stream << conf->gc_trunc_use_priority << ' ';
	stream << conf->slice_height << ' ';
	stream << conf->order << ' ';
	stream << conf->budget_report_nbytes << ' ';
	stream << conf->budget_report_lines << ' ';
	stream << conf->dq_type << ' ';
	stream << conf->data_coding << ' ';
	stream << conf->sp_lenhdr << ' ';
	stream << conf->sign_lenhdr << ' ';
	stream << conf->prec_lenhdr << ' ';
	stream << conf->encoder_id << ' ';
	stream << conf->use_slice_footer << ' ';
	stream << conf->in_depth << ' ';
	stream << conf->quant << ' ';
	stream << conf->v_slice_mirroring << ' ';
	stream << conf->sign_opt << ' ';
	stream << conf->gc_nonsig_group_size << ' ';
	stream << conf->run_mode << ' ';
	stream << conf->gc_top_pred_mode << ' ';
	stream << conf->sigflags_group_width << ' ';
	stream << conf->bounded_alphabet << ' ';
	stream << conf->verbose << ' ';
	stream << conf->pkt_hdr_size_auto << ' ';
	stream << conf->pkt_hdr_size_short << ' ';
	stream << conf->codesig_with_0 << ' ';
	stream << conf->gains_choice << ' ';

	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_NCOMPS * MAX_FILTER_TYPES; i++) {
		stream << conf->lvl_gains[i] << ' ';
	}
	stream << '\n';

	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_NCOMPS * MAX_FILTER_TYPES; i++) {
		stream << conf->lvl_priorities[i] << ' ';
	}

	stream << '\n';

}


void ser_precinct_t(std::ostream& stream, precinct_t* precinct) {

	stream << '\n'  << "^^PRECINCT_T" << '\n';


	ser_multi_buf_t(stream, precinct->gclis_mb);
	ser_multi_buf_t(stream, precinct->sig_mag_data_mb);

	ser_lvls_map_t(stream, precinct->geometry);

	//group_size
	stream << precinct->group_size << ' ';

	//idx_from_level[MAX_PRECINCT_HEIGHT][MAX_LVLS]
	stream << '\n' << "ArrArr:" << ' ';
	for (int i = 0; i < MAX_PRECINCT_HEIGHT; i++) {
		for (int j = 0; j < MAX_LVLS; j++) {
			stream << precinct->idx_from_level[i][j] << ' ';
		}
	}
	stream << '\n';

	//idx
	stream << precinct->idx << ' ';
	//column
	stream << precinct->column << ' ';

	stream << '\n';


}


void ser_precinct_budget_table_t(std::ostream& stream, precinct_budget_table_t* pbt) {

	stream << '\n' << "^^PRECINCT_BTT" << '\n';

	ser_multi_buf_t(stream, pbt->sigf_budget_table);
	ser_multi_buf_t(stream, pbt->gcli_budget_table);
	ser_multi_buf_t(stream, pbt->data_budget_table);
	ser_multi_buf_t(stream, pbt->sign_budget_table);

	//position_count
	stream << pbt->position_count << ' ';
	//method_count
	stream << pbt->method_count << ' ';

	stream << '\n';
	

}


void ser_predbuffer_t(std::ostream& stream, predbuffer_t* pred_residuals) {

	stream << '\n' << "^^PREDBUFFER_T" << '\n';

	//directional_prediction_struct* ptr = pred_residuals->direction;

	/*struct predbuffer_struct {

		directional_prediction_t direction[PRED_COUNT];
	};*/

	//multi_buf_t* gclis_mb[MAX_GCLI + 1] 
	stream << '\n' << "ArrArr:" << ' ';
	for (int i = 0; i < PRED_COUNT; i++) {
		for (int j = 0; j < MAX_GCLI + 1; j++) {
			ser_multi_buf_t(stream, (pred_residuals->direction[i]).gclis_mb[j]);
		}
	}
	stream << '\n';

	//multi_buf_t* predictors_mb[MAX_GCLI + 1] 
	stream << '\n' << "ArrArr:" << ' ';
	for (int i = 0; i < PRED_COUNT; i++) {
		for (int j = 0; j < MAX_GCLI + 1; j++) {
			ser_multi_buf_t(stream, (pred_residuals->direction[i]).predictors_mb[j]);
		}
	}
	stream << '\n';


	//idx_from_level[MAX_PRECINCT_HEIGHT][MAX_LVLS]
	
	for (int i = 0; i < PRED_COUNT; i++) {
		stream << '\n' << "ArrArr:" << ' ';
		for (int j = 0; j < MAX_PRECINCT_HEIGHT; j++) {
			for (int k = 0; k < MAX_LVLS; k++) {
				stream << (pred_residuals->direction[i]).idx_from_level[j][k] << ' ';
			}
		}
		stream << '\n';
	}

	stream << '\n';

}


void ser_params_t(std::ostream& stream, ra_params_t* ra_params) {

	stream << '\n' <<  "^^PARAMS_T" << '\n';

	//use_sign_packing
	stream << ra_params->use_sign_packing << ' ';
	//precinct_line
	stream << ra_params->precinct_line << ' ';
	//budget
	stream << ra_params->budget << ' ';
	//gc_trunc
	stream << ra_params->gc_trunc << ' ';
	//gc_trunc_use_priority
	stream << ra_params->gc_trunc_use_priority << ' ';
	//use_priorities
	stream << ra_params->use_priorities << ' ';
	//pkt_hdr_size_short
	stream << ra_params->pkt_hdr_size_short << ' ';
	//all_enabled_methods
	stream << ra_params->all_enabled_methods << ' ';
	//uint8_t* lvl_gains
	ser_uint8_t(stream, ra_params->lvl_gains);
	//uint8_t* lvl_priorities
	ser_uint8_t(stream, ra_params->lvl_priorities);

	stream << '\n';

}


void ser_precinct_budget_info_t(std::ostream& stream, precinct_budget_info_t* pbinfo) {

	stream << '\n' <<  "^^PRECINCT_BIT" << '\n';


	//precinct_bits
	stream << pbinfo->precinct_bits << ' ';
	
	//subpkt_uses_raw_fallback[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_uses_raw_fallback[i] << ' ';
	}
	stream << '\n';

	//prec_header_size
	stream << pbinfo->prec_header_size << ' ';

	//pkt_header_size[MAX_BANDLINES]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_BANDLINES; i++) {
		stream << pbinfo->pkt_header_size[i] << ' ';
	}
	stream << '\n';

	//subpkt_size_sigf[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_size_sigf[i] << ' ';
	}
	stream << '\n';

	//subpkt_size_gcli[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_size_gcli[i] << ' ';
	}
	stream << '\n';

	//subpkt_size_gcli_raw[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_size_gcli_raw[i] << ' ';
	}
	stream << '\n';

	//subpkt_size_data[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_size_data[i] << ' ';
	}
	stream << '\n';

	//subpkt_size_sign[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_size_sign[i] << ' ';
	}
	stream << '\n';

	//subpkt_alignbits_sigf[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_alignbits_sigf[i] << ' ';
	}
	stream << '\n';

	//subpkt_alignbits_gcli[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_alignbits_gcli[i] << ' ';
	}
	stream << '\n';

	//subpkt_alignbits_gcli_raw[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_alignbits_gcli_raw[i] << ' ';
	}
	stream << '\n';

	//subpkt_alignbits_data[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_alignbits_data[i] << ' ';
	}
	stream << '\n';

	//subpkt_alignbits_sign[MAX_SUBPKTS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_SUBPKTS; i++) {
		stream << pbinfo->subpkt_alignbits_sign[i] << ' ';
	}
	stream << '\n';

	stream << '\n';
	
}


void ser_budget_dump_struct_t(std::ostream& stream, budget_dump_struct_t* budget_dump_handle) {
	//todo
	if (budget_dump_handle == nullptr) {
		return;
	};


	//FILE* output_file todo
	stream << "FILE:" << '\n';
	stream << budget_dump_handle->output_file << '\n';

	//n_lvls
	stream << budget_dump_handle->n_lvls << ' ';
	//max_column_size
	stream << budget_dump_handle->max_column_size << ' ';

	//scenario2gtli_table[MAX_GCLI + 1][MAX_LVLS]
	stream << '\n' << "ArrArr:" << ' ';
	for (int i = 0; i < MAX_GCLI + 1; i++) {
		for (int j = 0; j < MAX_LVLS; j++) {
			stream << budget_dump_handle->scenario2gtli_table[i][j] << ' ';
		}
	}
	stream << '\n';

	//const uint8_t* sb_priority
	ser_uint8_t_const(stream, budget_dump_handle->sb_priority);

}


//
//


// First class functions
//

void ser_rate_control_t(std::ostream& stream, rate_control_t* rc) {
	
	stream << "^RC" << '\n';

	ser_tco_conf_t(stream, &(rc->conf));

	//image_height 
	stream << rc->image_height << ' ';

	ser_precinct_t(stream, rc->precinct);
	ser_precinct_budget_table_t(stream, rc->pbt);
	ser_predbuffer_t(stream, rc->pred_residuals);
	ser_precinct_t(stream, rc->precinct_top);

	//gc_enabled_modes
	stream << rc->gc_enabled_modes << ' ';

	//nibbles_image
	stream << rc->nibbles_image << ' ';
	//nibbles_report
	stream << rc->nibbles_report << ' ';
	//nibbles_consumed
	stream << rc->nibbles_consumed << ' ';
	//lines_consumed
	stream << rc->lines_consumed << ' ';

	ser_params_t(stream, &(rc->ra_params));

	//int* gtli_table_data
	ser_int(stream, rc->gtli_table_data);
	//int* gtli_table_gcli
	ser_int(stream, rc->gtli_table_gcli);
	//int* gtli_table_gcli_prec
	ser_int(stream, rc->gtli_table_gcli_prec);

	//gcli_methods_table
	stream << rc->gcli_methods_table << ' ';

	//int* gcli_sb_methods
	ser_int(stream, rc->gcli_sb_methods);

	ser_precinct_budget_info_t(stream, rc->pbinfo);

	//scenario2gtli_table[MAX_GCLI + 1][MAX_LVLS]
	stream << '\n' << "ArrArr:" << ' ';
	for (int i = 0; i < MAX_GCLI + 1; i++) {
		for (int j = 0; j < MAX_LVLS; j++) {
			stream << rc->scenario2gtli_table[i][j] << ' ';
		}
	}
	stream << '\n';

	ser_budget_dump_struct_t(stream, rc->budget_dump_handle);

	stream << '\n';

}


void ser_rc_results_t(std::ostream& stream, rc_results_t* rc_results) {

	stream << "^RC_RESULTS" << '\n';

	//scenario
	stream << rc_results->scenario << ' ';
	//refinement
	stream << rc_results->refinement << ' ';
	//gcli_method
	stream << rc_results->gcli_method << ' ';

	//gcli_sb_methods[MAX_LVLS]
	stream << '\n' << "Arr:" << ' ';
	for (int i = 0; i < MAX_LVLS; i++) {
		stream << rc_results->gcli_sb_methods[i] << ' ';
	}
	stream << '\n';

	ser_precinct_budget_info_t(stream, &(rc_results->pbinfo));

	//precinct_total_bits
	stream << rc_results->precinct_total_bits << ' ';
	//padding_bits
	stream << rc_results->padding_bits << ' ';
	//bits_consumed
	stream << rc_results->bits_consumed << ' ';

	ser_predbuffer_t(stream, rc_results->pred_residuals);

	//int* gtli_table_data
	ser_int(stream, rc_results->gtli_table_data);
	//int* gtli_table_gcli
	ser_int(stream, rc_results->gtli_table_gcli);

	//rc_error
	stream << rc_results->rc_error << ' ';

	stream << '\n';

}

//
//
