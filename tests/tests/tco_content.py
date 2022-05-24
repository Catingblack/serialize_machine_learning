#!/usr/bin/python
import os;
import sys;
sys.path.append('ipxvalidlib');
import content;
from intopix_test import intopix_test;


tco_content_package = content.intopix_content_creators_package();
tco_content_package.register_fold("jpegxs_content_444", "jpegxs_content_444")
tco_content_package.register_fold("jpegxs_content_422", "jpegxs_content_422")

#Register it to make it available to tests that use import tco_content
intopix_test.register_content_package(tco_content_package);


if __name__ == '__main__':
	cf = content.intopix_content_factory();
	cf.register_package(tco_content_package);
	for cname in ['test_content_444']:
		c1 = cf.create_content(cname);
		print c1.get_path(idx=0);
		print c1.get_path(idx=1);
		print c1.get_path(idx=2);
