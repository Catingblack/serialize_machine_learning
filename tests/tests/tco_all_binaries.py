#!/usr/bin/python
import sys, time, os, re
import hashlib

try:
	from tco_all_binaries_md5s import tco_all_binaries_md5_list
	from tco_all_binaries_md5s import tco_all_binaries_psnrs_list
except:
	tco_all_binaries_md5_list = {}
	tco_all_binaries_psnrs_list = {}

sys.path.append('ipxvalidlib')
import tco_content
from intopix_executor import local_executor
from intopix_test import intopix_test
from parsers import gm_psnr

#fixme move this to validlib?
def md5file(filepath):
	f = open(filepath)
	m = hashlib.md5()
	m.update(f.read())
	f.close()
	return m.hexdigest()

def md5tico(filepath, skip_hdr):
	f = open(filepath)
	tco_content = f.read()
	if(skip_hdr) and len(tco_content) > 4:
		offset=ord(tco_content[4]) #5th byte of file is length of header
	else:
		offset = 0

	m = hashlib.md5()
	m.update(tco_content[offset:])
	f.close()
	return m.hexdigest()

class test_all_binaries(intopix_test):

	_description = "Checking that encoder, decoder, enc_dec_chain are all functionnal"

	_local_config = {
		"tco_path": "../../build/bin/linux64",
	}

	_profile_names = {
		"3" : "3(XS_main)",
		"4" : "4(XS_high)",
		"5" : "5(XS_light_subline)",
		"6" : "6(XS_light)",
	}

	_degrees  = { 	'profile': ['3', '4','5','6'],
					'image' : [	'FemaleStripedHorseFly_1920x1080_8b.ppm',
								'HintergrundMusik_1920x1080_8b.ppm',
								'LeavesIso200_3008x2000_8b.ppm',
								'Peacock_1920x1080_8b.ppm',
								'Tools_1524x1200_8b.ppm',
								'Zoneplate-rbf_1920x1080_10b.ppm',
								'Zoneplate-rgf_1920x1080_10b.ppm'],
					'options' : ['',
								'[main]\ndq_type=0\n',
								'[main]\ndq_type=2\n',
								'[main]\nsign_opt=0\n',
								'[main]\nver_kernel=1\nhor_kernel=1\n',
								'[main]\nver_kernel=2\nhor_kernel=2\n',
								'[main]\nver_kernel=1\n',
								'[main]\nsign_opt=1\nsign_lenhdr=1\n',
								'[main]\nra_nlines=2\nbudget_report_lines=0\n',
								"[gcli]\nrun_sigflags_zrf = 0\n",
								"[gcli]\nrun_sigflags_zrcsf = 1\nrun_per_sb = 1\n",
								"[gcli]\nrun_sigflags_zrf = 0\nasymmetric=1\n",
								"[gcli]\nrun_minus1=1\n",
								"[gcli]\nrun_minus1=1\nasymmetric=1\n",
								"[gcli]\nnopred=1\nbounded=1\npred_per_sb=1\n",
								"[gcli]\nnopred=1\nbounded=1\npred_per_sb=1\nrun_per_sb=1\n",
								"[gcli]nopred=1\npred_per_sb=1\n",
								"[gcli]\nasymmetric=1\npred_per_sb=1\n",
								"[gcli]\nnopred=1\nsigflags_group_width=4\n",
								"[gcli]\nnopred=1\nbounded=1\nsigflags_group_width=4\n",
								"[gcli]\nnopred=1\nbounded=1\nsigflags_group_width=8\n",
								"[gcli]\nnopred=1\nbounded=1\nsigflags_group_width=4\npred_per_sb=1\n",
								"[gcli]\nnopred=1\nbounded=1\nsigflags_group_width=4\npred_per_sb=1\nrun_per_sb=1\n",
								'[main]\nndecomp_v=2\norder=VGRC\n',
								'[gcli]\nbounded=1\nbounded_alphabet=0\n',
								'[gcli]\nbounded=1\nbounded_alphabet=1\n',
								'[gcli]\nbounded=1\nbounded_alphabet=2\n',
								'[gcli]\nbounded=1\nbounded_alphabet=3\n',
								'[gcli]\nbounded=1\nbounded_alphabet=4\n',
								'[gcli]\nbounded=1\nbounded_alphabet=5\n',
								'[gcli]\nhor=0\nfirst_of_slice_use_nopred=1\n',
								'[gcli]\nhor=1\nsigned=0\nbounded=1\nbounded_hor_active=1\n',
								 ],
				}

	_degrees_priority = ['profile' , 'image', 'options']

	_report_header = (
		("test_name",      "test binaries"),
		("profile_name",   ""),
		("image",          ""),
		("options",        ""),
		("enc_ok",         ""),
		("enc_md5_ok",     ""),
		("dec_ok",         ""),
		("dec_md5_ok",     ""),
		("chain_ok",       ""),
		("output_same",    ""),
		("psnr", 		   ""),
		("psnr_ref", 	   ""),
		("tco_size_enc",   0),
		("tco_size_chain", 0),
		("header_len",     0),
		("size_ok",        ""),
		("test_passed",    True),
	)

	_skip_hdr = True,

	_list_computed_md5s = {}
	_list_computed_psnrs = {}

	def list_freedom_degrees(self):
		return self._degrees

	def get_content_list(self):
		return ['jpegxs_content_444']

	def get_report_header(self):
		return self._report_header

	def degrees_combi_is_valid(self, deg):
		if deg['profile'] != '0' and deg['options'] != '':
			return False
		if deg['image'] == 'languette.ppm' and 'CVHG' in deg['options']:
			return False
		return True

	#params in same order as freedom degree
	def execute(self, params):
		#print "tco_all_binaries: execute(%s)" % params
		report_line = self.init_reportline(params)
		report_line["profile_name"] = self._profile_names[params["profile"]]
		content = self._content['jpegxs_content_444']
		filename= params['image']
		filepath = content.get_path(filename=filename)
		ext = ".exe" if os.name == "nt" else ""
		enc_fname = "jxs_encoder" + ext
		dec_fname = "jxs_decoder" + ext
		enc_dec_fname = "jxs_enc_dec" + ext
		prefix = "profile%s" % params['profile'] + '_' + re.sub(r'\r|\n|\[|\]|\=|\;|\-|\.','_',(filename+params['options']))

		#First encode and produce a jxs
		logpath_cmds = self.create_logpath('cmds.log', subfolder=prefix, report_line=report_line)
		logpath_stderr = self.create_logpath('enc_err.log', subfolder=prefix, report_line=report_line)
		logpath_stdout = self.create_logpath("enc_out.log", subfolder=prefix, report_line=report_line)
		logpath_tcofile = self.create_logpath("enc.jxs",subfolder=prefix, report_line=report_line)
		fcmds = open(logpath_cmds,'wb')

		enc_options = ['-p', params['profile']]
		if params['profile'] == '0' and params['options']:
			config_path = self.create_logpath('config.ini', subfolder=prefix, report_line=report_line)
			f=open(config_path,'wb')
			f.write(params["options"])
			f.close()
			enc_options.extend(['-C', config_path])
		if not 'bitstream_nbytes' in params['options']:
			enc_options.extend(['-b', '5'])

		cmd = [os.path.join(self._config['tco_path'], enc_fname)] + enc_options + [filepath, logpath_tcofile]
		fcmds.write(' '.join(cmd).replace('\\','/') + '\n')
		tco_runner = local_executor(cmd, logpath_stdout,logpath_stderr)
		tco_runner.start()
		tco_runner.wait_for_end()

		exit_code = tco_runner.get_exit_code()
		exit_code_ok = exit_code[0] == 0 and exit_code[1] == 0

		if not os.path.exists(logpath_tcofile) or not exit_code_ok:
			report_line["enc_ok"] = False
		else:
			report_line["enc_ok"] = True
			report_line["tco_size_enc"] = os.stat(logpath_tcofile).st_size

			#Compute md5 of output
			md5 = md5tico(logpath_tcofile, self._skip_hdr)
			k = tuple(['enc'] + [params[d] for d in self._degrees_priority])
			test_all_binaries._list_computed_md5s[k] = md5
			if tco_all_binaries_md5_list.get(k,None):
				if tco_all_binaries_md5_list[k] == md5:
					report_line['enc_md5_ok'] = True
				else:
					report_line['enc_md5_ok'] = False
			else:
				report_line['enc_md5_ok'] = False

			#Decode produce a PPM
			logpath_stderr = self.create_logpath('dec_derr.log', subfolder=prefix, report_line=report_line)
			logpath_stdout = self.create_logpath("dec_dout.log", subfolder=prefix, report_line=report_line)
			logpath_decfile = self.create_logpath("dec.ppm", subfolder=prefix, report_line=report_line)

			cmd = [os.path.join(self._config['tco_path'], dec_fname),
			       logpath_tcofile, logpath_decfile]
			fcmds.write(' '.join(cmd).replace('\\','/') + '\n')
			tco_runner = local_executor(cmd, logpath_stdout,logpath_stderr)
			tco_runner.start()
			tco_runner.wait_for_end()

			exit_code = tco_runner.get_exit_code()
			exit_code_ok = exit_code[0] == 0 and exit_code[1] == 0

			if not os.path.exists(logpath_decfile) or not exit_code_ok:
				report_line['dec_ok'] = False
			else:
				#Compute md5 of output
				md5 = md5file(logpath_decfile)
				k = tuple(['dec'] + [params[d] for d in self._degrees_priority])
				test_all_binaries._list_computed_md5s[k] = md5

				if tco_all_binaries_md5_list.get(k,None):
					if tco_all_binaries_md5_list[k] == md5:
						report_line['dec_md5_ok'] = True
					else:
						report_line['dec_md5_ok'] = False
				else:
					report_line['dec_md5_ok'] = False

				report_line['dec_ok'] = True

				#Get PSNR for information
				logpath_gm_stdout = self.create_logpath('psnr.log', subfolder=prefix, report_line=report_line)
				logpath_gm_stderr = self.create_logpath('psnr_err.log', subfolder=prefix, report_line=report_line)
				cmd_gm = ["gm", "compare", "-metric", "psnr", filepath, logpath_decfile]
				gm_runner = local_executor(cmd_gm, logpath_gm_stdout,logpath_gm_stderr)
				gm_runner.start()
				gm_runner.wait_for_end()
				psnrs=gm_psnr.get_psnrs_list(logpath_gm_stdout)
				report_line['psnr'] = psnrs[-1]
				report_line['psnr_ref'] = tco_all_binaries_psnrs_list.get(k,None)
				test_all_binaries._list_computed_psnrs[k] = report_line['psnr']

		#do a one call chain encode decode
		logpath_stderr = self.create_logpath('chain_err.log',subfolder=prefix, report_line=report_line)
		logpath_stdout = self.create_logpath("chain_out.log",subfolder=prefix, report_line=report_line)
		logpath_encdecfile = self.create_logpath("chain.ppm",subfolder=prefix, report_line=report_line)

		cmd = [os.path.join(self._config['tco_path'], enc_dec_fname)] + enc_options + [filepath, logpath_encdecfile]
		fcmds.write(' '.join(cmd).replace('\\','/') + '\n')
		tco_runner = local_executor(cmd, logpath_stdout,logpath_stderr)
		tco_runner.start()
		ret = tco_runner.wait_for_end()

		exit_code = tco_runner.get_exit_code()
		exit_code_ok = exit_code[0] == 0 and exit_code[1] == 0

		if not os.path.exists(logpath_encdecfile) or not exit_code_ok:
			report_line['chain_ok'] = False
		else:
			report_line['chain_ok'] = True
			p = None
			for l in open(logpath_stderr).readlines():
				p = re.match(r"Encoder produced codestream of ([0-9]+) bytes", l)
				if p:
					break
			if p:
				report_line["tco_size_chain"] = int(p.group(1))

			delta = report_line["tco_size_enc"] - report_line["tco_size_chain"]
			report_line["header_len"] = delta
			report_line['size_ok'] = delta == 0

			#Compare PSNR
			if all([report_line.get(k,False) for k in ["enc_ok","dec_ok","chain_ok"]]):
				logpath_gm_stdout = self.create_logpath('out.psnr.log' , subfolder=prefix, report_line=report_line)
				logpath_gm_stderr = self.create_logpath('out.psnr_err.log' , subfolder=prefix, report_line=report_line)
				cmd_gm = ["gm", "compare", "-metric", "psnr", logpath_decfile, logpath_encdecfile]
				gm_runner = local_executor(cmd_gm, logpath_gm_stdout,logpath_gm_stderr)
				gm_runner.start()
				gm_runner.wait_for_end()
				psnrs=gm_psnr.get_psnrs_list(logpath_gm_stdout)
				for k,psnr in zip(["psnr_R","psnr_G","psnr_B","psnr_Glob"],psnrs):
					report_line[k] = psnr
				report_line['output_same'] = (len(psnrs) == 4) and (psnrs[3] == 'inf')

		report_line['test_passed'] = all([report_line.get(k,False) for k in ["enc_ok","enc_md5_ok","dec_ok",'dec_md5_ok',"chain_ok","output_same","size_ok"]])
		fcmds.close()
		return report_line

#This code saves all the computed md5 in a file
#to ease updating the md5s
def save_computed_md5s():
	print "tco_all_binaries_md5s.py.new file generated with test md5s."
	print "You may want to use this new file as reference"
	f=open("tco_all_binaries_md5s.py.new", "w")
	f.write("tco_all_binaries_md5_list = {\n")
	for k in sorted(test_all_binaries._list_computed_md5s.keys()):
		f.write("\t\t%s : '%s',\n" % (str(k), str(test_all_binaries._list_computed_md5s[k])))
	f.write("}\n")

	f.write("tco_all_binaries_psnrs_list = {\n")
	for k in sorted(test_all_binaries._list_computed_psnrs.keys()):
		f.write("\t\t%s : '%s',\n" % (str(k), str(test_all_binaries._list_computed_psnrs[k])))
	f.write("}\n")
	f.close()

import atexit
atexit.register(save_computed_md5s)


intopix_test.register_test(test_all_binaries)


if (__name__ == '__main__'):
	test_all_binaries().run_all()
