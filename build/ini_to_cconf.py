#!/usr/bin/env python
import sys
import re
import os


default_conf = """{
	$bitstream_nbytes,
	$bpp,
	$profile,
	$group_size,
	$kernel_h,
	$ndecomp_h,
	$kernel_v,
	$ndecomp_v,
	$ndecomp_vh,
	$skip_wavelet_on_comp,
	$rct,
	$rct_type,
	$cbr,
	$haar_use_shift,
	$col_sz,
	$col_max_width,
	$col_cnt,
	$col_granularity,
	$cols_ra_opt,
	$gc_raw,
	$gc_bounded,
	$gc_pred_per_sb,
	$gc_run_per_sb,
	$gc_ver,
	$gc_nopred,
	$gc_run_none,
	$gc_run_sigflags_zrf,
	$gc_run_sigflags_zrcsf,
	$gc_trunc,
	$gc_trunc_use_priority,
	$slice_height,
	$order,
	$budget_report_nbytes,
	$budget_report_lines,
	$dq_type,
	$data_coding,
	$sp_lenhdr,
	$sign_lenhdr,
	$prec_lenhdr,
	$encoder_id,
	$use_slice_footer,
	$in_depth,
	$quant,
	$v_slice_mirroring,
	$sign_opt,
	$gc_nonsig_group_size,
	$run_mode,
	$gc_top_pred_mode,
	$sigflags_group_width,
	$bounded_alphabet,
	$verbose,
	$pkt_hdr_size_auto,
	$pkt_hdr_size_short,
	$codesig_with_0,
	$gains_choice,
	{0},
	{0},
};
"""


config_ini_aliasses = {
	"gc_raw" : "raw",
	"gc_ver" : "ver",
	"gc_nopred" : "nopred",
	"gc_bounded" : "bounded",
	"gc_top_pred_mode" : "top_pred_mode",
	"kernel_v" : "ver_kernel",
	"kernel_h" : "hor_kernel",
	"gc_trunc" : "trunc",}


input_fn = sys.argv[1]
output_fn = sys.argv[2]
conf_name = os.path.splitext( os.path.basename(sys.argv[2]) )[0] + '_conf'

my_dict = {}

for line in open(input_fn).readlines():
	if '=' in line:
		s = line.split('=')
		k = s[0].strip()
		v = s[1].replace('{','').replace('}',',').strip()
		my_dict[k] = v

of = open(output_fn,"wb")

def_val = os.path.basename(output_fn).upper().replace('.','_')
of.write("#ifndef %s\n" % def_val)
of.write("#define %s\n\n" % def_val)
of.write("static tco_conf_t %s = " % conf_name)

for line in default_conf.splitlines():
	p=re.match(r'.*\$([a-zA-Z0-9_]*)', line)
	if p:
		param_name_h = p.group(1)
		if config_ini_aliasses.has_key(param_name_h):
			param_name = config_ini_aliasses[param_name_h]
		else:
			param_name = param_name_h

		param_value = 'TCO_CONF_DEFAULT_' + param_name_h.upper()
		if my_dict.has_key(param_name):
			param_value = my_dict[param_name]
		else:
			print "Using default for key", param_name
		if param_name == 'order' and "ORDER" not in param_value:
			param_value = "ORDER_" + param_value
		if param_name == 'profile' and not 'DEFAULT' in param_value:
			param_value = "TCO_" + param_value.upper()
		line = re.sub(r'\$[a-zA-Z0-9_]*','%s\t\t/* %s */' % (param_value, param_name), line)
	of.write(line)
	of.write("\n")

of.write("\n\n#endif /* %s */\n" % def_val)
