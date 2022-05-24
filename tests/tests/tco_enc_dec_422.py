#!/usr/bin/python
import sys, time, os, re
sys.path.append('ipxvalidlib')
import tco_content
from intopix_executor import local_executor
from intopix_test import intopix_test
from parsers import gm_psnr
import filecmp
from image_compare import get_pae_image
from image_read import read_yuv16p

class tco_enc_dec_422(intopix_test):

	_description = "This test will run tico and check the psnrs on the default tico test content"

	_local_config = {
		"tco_path": "../bin/linux64",
	}

	_degrees = {
		'profile' : ['3'],
		'chrom':  ['422'],
		'bpp':    [48],
	}

	_degrees_priority = ['profile', 'bpp' , 'chrom']

	_report_header = (
		("test_name",       "tco_enc_dec_422"),
		("filename",        ""),
		("profile",         ""),
		("chrom",           ""),
		("bpp",             ""),
		("PAE",        		False),
		("lossless",        False),
		("PAE < 10",        False),
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
		if (chrom == '422'):
			content_name = 'jpegxs_content_422'
		return content_name

	def get_content_list(self):
		contents = []
		contents.append(self.get_content_name('422'))
		return contents

	def get_report_header(self):
		return self._report_header

	#params in same order as freedom degree
	def execute(self, params):
		report_lines = []
		content = self._content[self.get_content_name(params["chrom"])]
		ext = ".exe" if os.name == "nt" else ""
		enc_dec_fname = "jxs_enc_dec" + ext

		cv = content.get_all()
		cl = len(cv)

		for idx,f in enumerate(cv):
			filename, filepath = f
			params["filename"] = filename
			sys.stdout.write("\r" + " "*70 + "\r[%3d%%] Processing %s..." % (100*idx/cl, filename))
			sys.stdout.flush()
			report_line = self.init_reportline(params)
			rep_log = "profile%s_%s_%dbpp" % (str(params["profile"]), params["chrom"], params["bpp"])
			logpath_stderr = self.create_logpath('%s_stderr.log' % (params["filename"]), idx, rep_log, report_line=report_line)
			logpath_stdout = self.create_logpath('%s_stdout.log' % (params["filename"]), idx, rep_log, report_line=report_line)

			cext = filename.split('.')[-1]
			#if cext in ['v210','yuv10','yuv10le', 'yuv10be']:
			#	w,h = {
			#		5529600 : (1920,1080),
			#	}[os.stat(filepath).st_size]
			#elif cext in ['yuv16','yuv16le','yuv16be']:
			#	w,h = {
			#		8294400 : (1920,1080),
			#	}[os.stat(filepath).st_size]
                        # thor: image dimensions were wrong. Use pairs() instead of lists {}!
			w,h = (3840, 2160)

			logpath_image = self.create_logpath('%s.out.%s' % (params["filename"], cext), idx, rep_log, report_line=report_line)

			cmd = [os.path.join(self._config['tco_path'], enc_dec_fname),
			       "-b", str(params["bpp"])] + [
			       "-w", str(w), "-h", str(h), '-p', params['profile'], filepath, logpath_image]
			#if cext in ['yuv16']:
			cmd.extend(["-d", "10"])
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
				report_line["size_expected"] = int((params["bpp"] * w * h + 7) / 8)
				p = None
				for l in open(logpath_stderr).readlines():
					p = re.match(r"Encoder produced codestream of ([0-9]+) bytes", l)
					if p:
						file_size = int(p.group(1))
						report_line["size_codestream"] = file_size
						break
				delta = report_line["size_expected"] - report_line["size_codestream"] 
				size_ok = delta == 0
				report_line["lossless"] = filecmp.cmp(filepath,logpath_image)
				img_ref = read_yuv16p(filepath, w, h)
				img_out = read_yuv16p(logpath_image, w, h)
				paes = get_pae_image(img_ref, img_out)
				report_line["PAE"] = str(paes)
				report_line["PAE < 10"] = all([pae < 4 for pae in paes])
				os.remove(logpath_image)
				report_line['test_passed'] = size_ok and report_line["PAE < 10"]

			report_lines.append(report_line)
		sys.stdout.write("\r" + " "*70 + "\r")
		sys.stdout.flush()
		return report_lines

intopix_test.register_test(tco_enc_dec_422)


if (__name__ == '__main__'):
	tco_enc_dec_422().run_all()
