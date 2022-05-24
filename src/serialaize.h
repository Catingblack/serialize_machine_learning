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




void serialize_conf(std::ostream& stream, rate_control_t* rc) {

    //C:\cpp\jpeg - xsm - ref_sw\serialize_machine_learning\build\vs_2015


	//tco_conf_t
	stream << rc->conf.bitstream_nbytes << ' ';
	stream << rc->conf.bpp << ' ';
	stream << rc->conf.profile << ' ';
	stream << rc->conf.group_size << ' ';
	stream << rc->conf.kernel_h << ' ';
	stream << rc->conf.ndecomp_h << ' ';
	stream << rc->conf.ndecomp_vh << ' ';
	stream << rc->conf.skip_wavelet_on_comp << ' ';
	stream << rc->conf.rct << ' ';
	stream << rc->conf.rct_type << ' ';
	stream << rc->conf.cbr << ' ';
	stream << rc->conf.haar_use_shift << ' ';
	stream << rc->conf.col_sz << ' ';
	stream << rc->conf.col_max_width << ' ';
	stream << rc->conf.col_cnt << ' ';
	stream << rc->conf.col_granularity << ' ';
	stream << rc->conf.cols_ra_opt << ' ';
	stream << rc->conf.gc_raw << ' ';
	stream << rc->conf.gc_bounded << ' ';
	stream << rc->conf.gc_pred_per_sb << ' ';
	stream << rc->conf.gc_run_per_sb << ' ';
	stream << rc->conf.gc_ver << ' ';
	stream << rc->conf.gc_nopred << ' ';
	stream << rc->conf.gc_run_none << ' ';
	stream << rc->conf.gc_run_sigflags_zrf << ' ';
	stream << rc->conf.gc_run_sigflags_zrcsf << ' ';
	stream << rc->conf.gc_trunc << ' ';
	stream << rc->conf.gc_trunc_use_priority << ' ';
	stream << rc->conf.slice_height << ' ';
	stream << rc->conf.order << ' ';
	stream << rc->conf.budget_report_nbytes << ' ';
	stream << rc->conf.budget_report_lines << ' ';
	stream << rc->conf.dq_type << ' ';
	stream << rc->conf.data_coding << ' ';
	stream << rc->conf.sp_lenhdr << ' ';
	stream << rc->conf.sign_lenhdr << ' ';
	stream << rc->conf.prec_lenhdr << ' ';
	stream << rc->conf.encoder_id << ' ';
	stream << rc->conf.use_slice_footer << ' ';
	stream << rc->conf.in_depth << ' ';
	stream << rc->conf.quant << ' ';
	stream << rc->conf.v_slice_mirroring << ' ';
	stream << rc->conf.sign_opt << ' ';
	stream << rc->conf.gc_nonsig_group_size << ' ';
	stream << rc->conf.run_mode << ' ';
	stream << rc->conf.gc_top_pred_mode << ' ';
	stream << rc->conf.sigflags_group_width << ' ';
	stream << rc->conf.bounded_alphabet << ' ';
	stream << rc->conf.verbose << ' ';
	stream << rc->conf.pkt_hdr_size_auto << ' ';
	stream << rc->conf.pkt_hdr_size_short << ' ';
	stream << rc->conf.codesig_with_0 << ' ';
	stream << rc->conf.gains_choice << ' ';


	for (int i = 0; i < MAX_NCOMPS * MAX_FILTER_TYPES; i++) {
		stream << rc->conf.lvl_gains[i] << ' ';
	}

	for (int i = 0; i < MAX_NCOMPS * MAX_FILTER_TYPES; i++) {
		stream << rc->conf.lvl_priorities[i] << ' ';
	}

}
