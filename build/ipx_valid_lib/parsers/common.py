#!/usr/bin/python
import os;
import re;



def get_lines_if_exists(logpath):
	if (os.path.exists(logpath)):
		return open(logpath,'r').readlines();
	else:
		return [];


def get_number_of_lines_with(string_to_find, logpath):
	n = 0;
	for line in get_lines_if_exists(logpath):
		if string_to_find in line:
			n+=1;
	return n;

