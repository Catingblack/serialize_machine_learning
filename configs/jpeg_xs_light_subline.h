#ifndef JPEG_XS_LIGHT_SUBLINE_H
#define JPEG_XS_LIGHT_SUBLINE_H

static tco_conf_t jpeg_xs_light_subline_conf = {
	TCO_CONF_DEFAULT_BITSTREAM_NBYTES		/* bitstream_nbytes */,
	TCO_CONF_DEFAULT_BPP		/* bpp */,
	TCO_PROFILE_JPEGXS_LIGHT_SUBLINE		/* profile */,
	TCO_CONF_DEFAULT_GROUP_SIZE		/* group_size */,
	TCO_CONF_DEFAULT_KERNEL_H		/* hor_kernel */,
	5		/* ndecomp_h */,
	TCO_CONF_DEFAULT_KERNEL_V		/* ver_kernel */,
	0		/* ndecomp_v */,
	TCO_CONF_DEFAULT_NDECOMP_VH		/* ndecomp_vh */,
	TCO_CONF_DEFAULT_SKIP_WAVELET_ON_COMP		/* skip_wavelet_on_comp */,
	TCO_CONF_DEFAULT_RCT		/* rct */,
	TCO_CONF_DEFAULT_RCT_TYPE		/* rct_type */,
	TCO_CONF_DEFAULT_CBR		/* cbr */,
	TCO_CONF_DEFAULT_HAAR_USE_SHIFT		/* haar_use_shift */,
	TCO_CONF_DEFAULT_COL_SZ		/* col_sz */,
	2048		/* col_max_width */,
	2		/* col_cnt */,
	256		/* col_granularity */,
	TCO_CONF_DEFAULT_COLS_RA_OPT		/* cols_ra_opt */,
	1		/* raw */,
	1		/* bounded */,
	TCO_CONF_DEFAULT_GC_PRED_PER_SB		/* gc_pred_per_sb */,
	TCO_CONF_DEFAULT_GC_RUN_PER_SB		/* gc_run_per_sb */,
	1		/* ver */,
	1		/* nopred */,
	TCO_CONF_DEFAULT_GC_RUN_NONE		/* gc_run_none */,
	TCO_CONF_DEFAULT_GC_RUN_SIGFLAGS_ZRF		/* gc_run_sigflags_zrf */,
	TCO_CONF_DEFAULT_GC_RUN_SIGFLAGS_ZRCSF		/* gc_run_sigflags_zrcsf */,
	TCO_CONF_DEFAULT_GC_TRUNC		/* trunc */,
	TCO_CONF_DEFAULT_GC_TRUNC_USE_PRIORITY		/* gc_trunc_use_priority */,
	16		/* slice_height */,
	ORDER_RPBC		/* order */,
	TCO_CONF_DEFAULT_BUDGET_REPORT_NBYTES		/* budget_report_nbytes */,
	0		/* budget_report_lines */,
	1		/* dq_type */,
	TCO_CONF_DEFAULT_DATA_CODING		/* data_coding */,
	TCO_CONF_DEFAULT_SP_LENHDR		/* sp_lenhdr */,
	TCO_CONF_DEFAULT_SIGN_LENHDR		/* sign_lenhdr */,
	TCO_CONF_DEFAULT_PREC_LENHDR		/* prec_lenhdr */,
	TCO_CONF_DEFAULT_ENCODER_ID		/* encoder_id */,
	TCO_CONF_DEFAULT_USE_SLICE_FOOTER		/* use_slice_footer */,
	TCO_CONF_DEFAULT_IN_DEPTH		/* in_depth */,
	TCO_CONF_DEFAULT_QUANT		/* quant */,
	TCO_CONF_DEFAULT_V_SLICE_MIRRORING		/* v_slice_mirroring */,
	TCO_CONF_DEFAULT_SIGN_OPT		/* sign_opt */,
	TCO_CONF_DEFAULT_GC_NONSIG_GROUP_SIZE		/* gc_nonsig_group_size */,
	TCO_CONF_DEFAULT_RUN_MODE		/* run_mode */,
	TCO_CONF_DEFAULT_GC_TOP_PRED_MODE		/* top_pred_mode */,
	8		/* sigflags_group_width */,
	TCO_CONF_DEFAULT_BOUNDED_ALPHABET		/* bounded_alphabet */,
	TCO_CONF_DEFAULT_VERBOSE		/* verbose */,
	TCO_CONF_DEFAULT_PKT_HDR_SIZE_AUTO		/* pkt_hdr_size_auto */,
	TCO_CONF_DEFAULT_PKT_HDR_SIZE_SHORT		/* pkt_hdr_size_short */,
	TCO_CONF_DEFAULT_CODESIG_WITH_0		/* codesig_with_0 */,
	TCO_CONF_DEFAULT_GAINS_CHOICE		/* gains_choice */,
	{0},
	{0},
};


#endif /* JPEG_XS_LIGHT_SUBLINE_H */
