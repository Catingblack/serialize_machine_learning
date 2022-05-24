#!/usr/bin/python
import re;
import time;
import datetime;
import sys;
import os
import socket
import hashlib
import sys
import urllib
from os.path import join, getsize

def sleep_with_progress_bar(seconds, size = 25):
	current_progress = 0;
	for i in range(0,size):
		sys.stderr.write('.');
	sys.stderr.write("\n");
	for i in range(0,seconds):
		time.sleep(1);
		if (i * size / seconds != current_progress):
			current_progress = i * size / seconds;
			sys.stderr.write('.');
	sys.stderr.write(".\n");

def raw_input_yes_no(sentence):
	rep = raw_input("%s [y/n]\n" % (sentence));
	if (re.match('y',rep,flags=re.IGNORECASE)):
		return True;
	return False;

def raw_input_int(sentence, start, stop):
	rep = raw_input("%s [%d-%d]\n" % (sentence,start,stop-1));
	if (re.match('^\d+$',rep)):
		if (int(rep) in range(start,stop)):
			return int(rep)
	return -1;

def raw_input_int_list(sentence, start, stop):
	list = [];
	while (True):
		rep = raw_input("%s [%d-%d]\n" % (sentence,start,stop-1));
		for i in rep.split(','):
			if (re.match('^\d+$',i)):
				if (int(i) in range(start,stop)):
					list.append(int(i));
			else:
				break;
		else:
			break;
	return list;

	
def find_all_files_in_dir(path,extension):
	#this is a quick trick
	list_files = [];
	for root, dirs, files in os.walk(path):
		list_files.extend([join(root, name) for name in files if os.path.splitext(name)[1] == '.' + extension]);
	return list_files;

	
def dict_product(dicts, keys_priority = None):
	from itertools import izip, product;
	#return (dict(izip(dicts, x)) for x in product(*dicts.itervalues()));
	if (keys_priority is None):
		keys_priority = dicts.keys(); #let's use default
	elif (len(keys_priority) != len(dicts.keys())):
		print "Error: some keys not present in degree priority list"
		print "Priority:",keys_priority
		print "All:",dicts.keys()
		keys_priority = dicts.keys();
	itervalues=[dicts[key] for key in keys_priority];
	return (dict(izip(keys_priority, x)) for x in product(*itervalues));
	
#This functions gives the IP of the network interface
#talking on network of remote machine
def find_my_ip_on_remote_machine_network(remote_ip):
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM);
	s.connect((remote_ip, 22));
	name = s.getsockname();
	ip = name[0];
	s.close();
	return ip;

def md5sum(fobj):
    '''Returns an md5 hash for an object with read() method.'''
    m = hashlib.md5()
    while True:
        d = fobj.read(8096)
        if not d:
            break
        m.update(d)
    return m.hexdigest()

def get_duration(start_time, stop_time):
	seconds = (stop_time - start_time).seconds;
	hours, remainder = divmod(seconds, 3600)
	minutes, seconds = divmod(remainder, 60)
	duration = '%s:%s:%s' % (hours, minutes, seconds);
	return duration;
	
if __name__ == "__main__":
	print find_all_files_in_dir('/mnt/Data_Win','mxf');
