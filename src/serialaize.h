#include <iostream>
#include <fstream>
#include <string>

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


struct precinct_struct {

	multi_buf_t* gclis_mb;

	multi_buf_t* sig_mag_data_mb;


	const lvls_map_t* geometry;

	int group_size;

	int idx_from_level[MAX_PRECINCT_HEIGHT][MAX_LVLS];

	int idx;

	int column;
};


void ser_tco_conf_t(std::ostream& stream, tco_conf_t* conf) {
	
	//tco_conf_t
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


	for (int i = 0; i < MAX_NCOMPS * MAX_FILTER_TYPES; i++) {
		stream << conf->lvl_gains[i] << ' ';
	}

	for (int i = 0; i < MAX_NCOMPS * MAX_FILTER_TYPES; i++) {
		stream << conf->lvl_priorities[i] << ' ';
	}
}

void ser_multi_buf_t(std::ostream& stream, rate_control_t* rc) {

}

//

void ser_precinct_t(std::ostream& stream, precinct_t* precinct) {

}

void ser_precinct_budget_table_t(std::ostream& stream, precinct_budget_table_t* pbt) {

}

void ser_predbuffer_t(std::ostream& stream, predbuffer_t* pred_residuals) {

}

///

void serialize_first(std::ostream& stream, rate_control_t* rc) {

	ser_tco_conf_t(stream, rc->conf);

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

}

void serialize_second(std::ostream& stream, precinct_t* precinct) {

}

void serialize_third(std::ostream& stream, rc_results_t* rc_results) {

}
