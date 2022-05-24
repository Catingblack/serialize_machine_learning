#!/usr/bin/python
import os;
import re;
#from parsers import common
import common

def get_psnrs_list(logpath):
	psnr_list = [];
	re_pattern = r".*:\s+([0-9.]+)"
	for line in common.get_lines_if_exists(logpath):
		if ('Red' in line) or ('Blue' in line) or ('Green' in line) or ('Total' in line):
			if "1.#J" in line or 'inf' in line:
				psnr_list.append("inf")
			else:
				p = re.match(re_pattern,line);
				if p:
					psnr_list.append(p.group(1));
	return psnr_list;

if __name__ == '__main__':
	print get_psnrs_list('D:/625sweep.bmp.psnr.log')
