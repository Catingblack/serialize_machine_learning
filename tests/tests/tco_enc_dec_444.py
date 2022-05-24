#!/usr/bin/python
import sys, time, os, re
sys.path.append('ipxvalidlib')
import tco_content
from intopix_executor import local_executor
from intopix_test import intopix_test
from parsers import gm_psnr

class tco_enc_dec_444(intopix_test):

	_description = "This test will run tico and check the psnrs on the default tico test content"

	_local_config = {
		"tco_path": "../build/20140129_tico_delivery_1.0.0.0_NO_LIMIT",
	}

	_degrees = {
		'profile' : ['3'],
		'chrom':  ['444'],
		'bpp':    [8, 48],
	}

	_degrees_priority = ['profile', 'bpp' , 'chrom']

	_report_header = (
		("test_name",       "tco_enc_dec_psnr"),
		("filename",        ""),
		("profile",         ""),
		("chrom",           ""),
		("bpp",             ""),
		("psnr_R",          ""),
		("psnr_G",          ""),
		("psnr_B",          ""),
		("psnr_Glob",       ""),
		("size_codestream", ""),
		("size_expected",   ""),
		("test_passed",     False),
	)

	def list_freedom_degrees(self):
		return self._degrees

	def degrees_combi_is_valid(self,deg):
		return True

	def get_content_name(self, chrom):
		content_name = None
		if (chrom == '444'):
			content_name = 'jpegxs_content_444'
		return content_name

	def get_content_list(self):
		contents = []
		contents.append(self.get_content_name('444'))
		f_out = open("tco_enc_dec_444_psnr.txt", 'w')
		f_out.close()
		return contents

	def get_report_header(self):
		return self._report_header

	#params in same order as freedom degree
	def execute(self, params):
		report_lines = []
		content = self._content[self.get_content_name(params["chrom"])]
		ext = ".exe" if os.name == "nt" else ""
		enc_dec_fname = "jxs_enc_dec" + ext

		#file_in = "%s_test_reports/" % (self._config['tco_path'])
		f_out = open("tco_enc_dec_444_psnr.txt", 'a')
		f_out.write("Run with chrom = %06s, bpp = %03d\n" % (params["chrom"], params["bpp"]))

		cv = content.get_all()
		cl = len(cv)

		for idx,f in enumerate(cv):
			filename, filepath = f
			params["filename"] = filename
			sys.stdout.write("\r" + " "*70 + "\r[%3d%%] Processing %s..." % (100*idx/cl, filename))
			sys.stdout.flush()
			report_line = self.init_reportline(params)
			rep_log = "profile%s_%s_%dbpp" % (str(params["profile"]), params["chrom"], params["bpp"])
			logpath_stderr = self.create_logpath('%s_stderr.log' % (params["filename"]), idx, rep_log, report_line = report_line)
			logpath_stdout = self.create_logpath('%s_stdout.log' % (params["filename"]), idx, rep_log, report_line = report_line)
			logpath_image = self.create_logpath('%s.out.ppm' % (params["filename"]), idx, rep_log, report_line = report_line)

			cmd = [os.path.join(self._config['tco_path'], enc_dec_fname),
			       "-b", str(params["bpp"]), '-p', params['profile']] + [
			       filepath, logpath_image]
			tco_runner = local_executor(cmd, logpath_stdout,logpath_stderr)
			if tco_runner.start() < 0:
				raise BaseException( str(self.__class__.__name__) + (": Unable to start tico decoder "))
			else:
				tco_runner.wait_for_end(60)
				if (not tco_runner.is_finished()):
					tco_runner.kill()

			if not os.path.exists(logpath_image):
				report_line['test_passed'] = False
			else:
				#Get PSNR
				logpath_gm_stdout = self.create_logpath('%s.psnr.log' % (params["filename"]), idx, rep_log, report_line = report_line)
				logpath_gm_stderr = self.create_logpath('%s.psnr_err.log' % (params["filename"]), idx, rep_log, report_line = report_line)
				cmd_gm = ["gm", "compare", "-metric", "psnr", filepath, logpath_image]
				gm_runner = local_executor(cmd_gm, logpath_gm_stdout,logpath_gm_stderr)
				gm_runner.start()
				gm_runner.wait_for_end()
				psnrs=gm_psnr.get_psnrs_list(logpath_gm_stdout)
				for k,psnr in zip(["psnr_R","psnr_G","psnr_B","psnr_Glob"],psnrs):
					report_line[k] = psnr

				f_out.write("%050s; psnr_R: %06s; psnr_G: %06s; psnr_B: %06s; psnr_Glob: %06s \n" % (filename,psnrs[0],psnrs[1],psnrs[2],psnrs[3]))

				psnr_ok = (len(psnrs) == 4) and (params["bpp"] < 48 or psnrs[3] == 'inf')

				#Check compression ratio
				report_line["size_expected"] = 0
				report_line["size_codestream"] = -1
				id_err = self.create_logpath('%s.id.err.log' % (params["filename"]), idx, rep_log, report_line = report_line)
				id_out = self.create_logpath('%s.id.log' % (params["filename"]), idx, rep_log, report_line = report_line)
				cmd_gm = ["gm", "identify", filepath]
				gm_runner = local_executor(cmd_gm, id_out,id_err)
				gm_runner.start()
				gm_runner.wait_for_end()

				cols = open(id_out).readlines()[0].split()
				p = re.match(r'([0-9]+)x([0-9]+)',cols[2])
				if p:
					w,h = map(int,[p.group(1),p.group(2)])
					report_line["size_expected"] = int((params["bpp"] * w * h + 7) / 8)
                                for l in open(logpath_stderr).readlines():
					p = re.match(r"Encoder produced codestream of ([0-9]+) bytes", l)
					if p:
						file_size = int(p.group(1))
						report_line["size_codestream"] = file_size
						break
				
				delta = report_line["size_expected"] - report_line["size_codestream"] 
				size_ok = delta == 0
				os.remove(logpath_image)
				os.remove(logpath_gm_stderr)
				os.remove(id_err)

				report_line['test_passed'] = size_ok and psnr_ok
			report_lines.append(report_line)
		sys.stdout.write("\r" + " "*70 + "\r")
		sys.stdout.flush()
		f_out.close()
		return report_lines

intopix_test.register_test(tco_enc_dec_444)


if (__name__ == '__main__'):
	tco_enc_dec_444().run_all()
