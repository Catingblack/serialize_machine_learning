import sys;
import os;
import time;
from collections import defaultdict;

class logger:
	
	#logger always create a log folder with test name and time
	#inside log folder, all log files of test will be stored
	def __init__(self, basepath, testname):
		current_time = time.strftime('%Y%m%d_%Hh%Mm%Ss');
		self._logpath = os.path.join(basepath, testname , current_time);
		self._logcount = defaultdict(int);#if a user requests twice the same logname, they will be numerated with logname_1, _2, _3, ++
		os.makedirs(self._logpath);
		
	#get the path for logfile with name "name" or subfolder\name
	def get_logpath(self, name, subfolder = None):
		if subfolder:
			subfolder_path = os.path.join(self._logpath, subfolder);
			if (not os.path.exists(subfolder_path)):
				os.makedirs(subfolder_path);
			name = os.path.join(subfolder, name);
		if (self._logcount[name] > 0):
			#let's try to keep the same extension
			base, ext = os.path.splitext(name)
			logname = base + "_%d" % self._logcount[name] + ext;
		else:
			logname = name;
		self._logcount[name] = self._logcount[name] + 1;
		return os.path.join(self._logpath,logname);

