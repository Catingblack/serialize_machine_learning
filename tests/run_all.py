#!/usr/bin/python
import sys, os, time
#do not forget to put a validlib in your PYTHONPATH or uncomment here
#sys.path.append('./ipxvalidlib');#TODO: where will be stored the lib? Current dir?
from intopix_test import intopix_test
from tests import *

if (len(sys.argv) not in [2,3]):
	print "Usage : run_all.py path_to_tco_delivery"
	exit(0)

config = {
	"tco_path": sys.argv[1] + "/bin" + ("/win32" if os.name == "nt" else "/linux64"),
	"logpath":  sys.argv[1] + "/test_reports",
}
config["summary_path"] = os.path.join(config['logpath'],"summary_" + time.strftime('%Y%m%d_%Hh%Mm%Ss') + ".html")

#3. Tests
# MD5 test
intopix_test.by_name('test_all_binaries')(**config).run_all()

# PSNR test
intopix_test.by_name('tco_enc_dec_444')(**config).run_all()
intopix_test.by_name('tco_enc_dec_422')(**config).run_all()
