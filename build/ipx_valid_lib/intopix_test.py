import sys;
sys.path.append('../validlib');
sys.path.append('./validlib');
import pickle;
import time;
import content;
import logger;
import os;
import itertools;
import utils;
#from content import content_copier;
from logger import logger;
from intopix_test_report import intopix_test_report, intopix_test_report_parser;
#this class is the parent class of all intopix tests that are meant to be included in
#a main test launcher
from intopix_test_setups import intopix_test_setups;
from datetime import datetime;
try:
	from concurrent.futures import ThreadPoolExecutor
	from multiprocessing import cpu_count
	multithread = True
except Exception as e:
	print "Please install concurrent futures for python if you want to use multithread"
	multithread = False


if sys.version_info < (2, 6):
	print "python 2.6 or greater required for test framework!";
	exit(0);

def run_mt(args):
	test_instance = args[0]
	params = args[1]
	return test_instance.execute(params)


class intopix_test(object):

	#--Test results constants--
	PASSED = "Passed"
	WARNING = "Warning"
	ERROR = "Error"
	CRITICAL = "Critical"

	line_colours = {
		PASSED : "lightgreen",
		WARNING : "Orange",
		ERROR : "DarkRed",
		CRITICAL : "Red",
	}
	#--

	_list_tests=[];
	_content_packages = [];
	_test_reports = {};
	_description="";

	@classmethod
	def register_test(cl,classname):
		cl._list_tests.append(classname);

	@classmethod
	def register_content_package(cl, package):
		cl._content_packages.append(package);

	@classmethod
	def by_name(cl, testname):
		for testclass in cl._list_tests:
			if (testclass.__name__ == testname):
				return testclass;
		else:
			return None;

#defaults
	_local_config = {}; #redefined by upperclasses
	_setup_resources = [];

#intopix_test config (will be merged with local_config so that test._config contains all the config)
	_config = {	'logpath' : './logs',
			'xmldir'  : '/usr/local/share/intopix/Pristine_P4_SDI_Streaming/xml',
			'xfgmrun_path' : None, #no custom xfgmrun path by default
			'summary_path': None,#default none
			"warn_unknown_param" : True,
			'dump_log_files' : False, #cat logfile one command line at the end
		};

#assume no priority by default
	_degrees_priority=None;

	def degrees_combi_is_valid(self,deg):
		return True;

	def __init__(self, **kwargs):
		self._list_xfgmcontexts=[];
		self._list_logfiles={};
		self._report=[];
		self._content={};
		#Update config based on args. Warn on unkown key
		self._config=dict(self._local_config.items() + self._config.items());
		unknowns = [];
		for confname in kwargs.keys():
			if confname in self._config.keys():
				self._config[confname] = kwargs[confname];
			else:
				unknowns.append(confname);
		if (self._config["warn_unknown_param"] and len(unknowns) > 0):
			print "Warning: unkown config parameters found in intopix_test init", unknowns;
		self._log = None;
		self._test_setup = None;
		self._content_factory = content.intopix_content_factory();
		self._report_path = None;
		self._report_lines = [];
		self._start_time = None;
		self._stop_time = None;

	def get_description(self):
		return self._description;

	#download content
	def prepare_content(self):
		for pack in self._content_packages:
			self._content_factory.register_package(pack);
		#Get Content
		print "Getting Content"
		for content_name in self.get_content_list():
			content = self._content_factory.create_content(content_name);
			if (content):
				self._content[content_name] = content;
			else:
				print "Error: unable to find the content with name", content_name;
				return -1;
		return 0;

	#create logger
	def prepare_logger(self):
	#Create a logger object, to store all logfile properly
		if (not os.path.exists(self._config['logpath'])):
			print "Create log directory %s." % self._config['logpath'];
			try:
				os.mkdir(self._config['logpath']);
			except:
				print "unable to create log directory!";
				return -1;
		scriptname = self.__class__.__name__;
		self._log = logger(self._config['logpath'],scriptname);
		return 0;

	def fd_filename(self):
		return str(self.__class__.__name__) + '.fdeg';

	def status_filename(self):
		return str(self.__class__.__name__) + '.status';

	def save_status(self, status):
		f = open(self.status_filename(), 'w');
 		pickle.dump(status, f);
 		f.close();

	def can_run_with_setup(self, setup_name):
		return intopix_test_setups().are_all_resources_in_setup(setup_name, self._setup_resources);

	def run_previous(self):
		try:
			f_fd = open(self.fd_filename(), 'r');
			freedom_degs = pickle.load(f_fd);
			print "Using freedom degrees from file %s" % self.fd_filename();
			f_fd.close();
		except:
			print "No fdeg file -> Using all freedom degrees";
			freedom_degs = self.list_freedom_degrees();
   		try:
			f = open(self.status_filename(), 'r');
			status = pickle.load(f);
			if (status is not None):
				print "Last run interrupted at " , status;
				rep = raw_input("Restart test from this values (y/n)");
				if (rep != 'y'):
					status = None;
					os.remove(self.status_filename());
				else:
					print "Using freedom degrees from file %s" % self.fd_filename();
			f.close();
		except:
			status = None;
		return self.run(freedom_degs, status);

	def run(self, freedom_degs, start_at = None,  break_on_first_fail = False):
		f_fd = open(self.fd_filename(), 'w');
 		pickle.dump(freedom_degs, f_fd);
 		f_fd.close();
		#ret = self.prepare_execute();
		if (self.prepare_content() < 0):
			print "Error preparing content"
			return -1;
		if (self.prepare_logger() < 0):
			print "Error preparing logger"
			return -1;
		self._test_setup = intopix_test_setups();
		ret = self._test_setup.prepare_resources(self._setup_resources);
		if (ret is not None and ret < 0):
			print "Unable to prepare execution";
			return -1;
		#Prepare report
		report = intopix_test_report(self.get_report_path(), self.__class__.__name__);
		column_titles = [val[0] for val in self.get_report_header()] + [ 'logpath' ];
		report.open_table(column_titles);

		#Execute installing instructions
		whole_test_passed = True;
		current_status = None;
		started = False;
		try:
			self._report_lines = [];
			self._start_time = datetime.now();

			param_list = [p for p in utils.dict_product(freedom_degs, self._degrees_priority) if self.degrees_combi_is_valid(p)]
			
			if not multithread:
				results = []
				for p in param_list:
					results.append(run_mt([self,p]))
			else:
				p = ThreadPoolExecutor(max_workers = cpu_count())
				results = p.map(run_mt, zip([self]*len(param_list), list(param_list)))

			for result in results:
				self.cleanup_xfgmcontexts();
				#Report Line could be multiple
				if (type(result).__name__ != 'list'):
					report_lines = [result];
				else:
					report_lines = result
				self._report_lines.extend(report_lines);
				for idx, report_line in enumerate(report_lines):
					if (report_line is not None):
						#Test should have a test_passed report_line
						if not report_line.has_key('test_result') and not report_line.has_key('test_passed'):
							report_line["test_result"] = intopix_test.CRITICAL
						elif report_line.has_key('test_passed'): #Backward compatible
							report_line["test_result"] = intopix_test.PASSED if report_line["test_passed"] else intopix_test.CRITICAL
							report_line["test_passed"] = report_line["test_result"] #set a String to be written in report

						test_ok = report_line['test_result'] == intopix_test.PASSED
						whole_test_passed = whole_test_passed and test_ok

						if report_line.has_key('test_name'):
							print report_line['test_name'] + " [ %s ] " % report_line['test_result'];
						
						if not report_line.has_key('logpath_list'):
							if (self._list_logfiles.has_key(idx)):
								report_line['logpath'] = ",".join([report.generate_url_link(name,path) for (name,path) in self._list_logfiles[idx]]);
								#Let's archive the log path so we can get an access to them in the summary generation
								report_line["_logpath"] = self._list_logfiles[idx]
							else:
								report_line['logpath'] = "No Log";
								report_line["_logpath"] = []
						else:
							report_line['logpath'] = ",".join([report.generate_url_link(name,path) for (name,path) in report_line['logpath_list']]);

						#Test color
						if report_line.has_key('test_report_color'):
							color = report_line['test_report_color']
						else:
							color = intopix_test.line_colours[report_line['test_result']]
						report.add_table_row( [report_line[key] for key in column_titles], color);

						if break_on_first_fail and not test_ok:
							break;
				#current_status = params;
				if self._config["dump_log_files"]:
					self.dump_logfiles();
				self.cleanup_logfiles();

			if len(self._report_lines) > 0:
				intopix_test._test_reports.update({self._report_lines[0]['test_name']:self._report_lines});


		except KeyboardInterrupt:
			print "Run interrupted";
			print "Saving status";
			#self.save_status(current_status);
		except:
			print "Run ERROR";
			report.close_table();
			report.close();
			self.cleanup_xfgmcontexts();
			raise;
		self.cleanup_xfgmcontexts();
		report.close_table();
		report.close();
		self.post_execute();
		self._stop_time = datetime.now();
		self.generate_summary();

		if (whole_test_passed):
			return 0;
		else:
			return -1;

	def run_all(self, break_on_first_fail = False):
		all_degs = self.list_freedom_degrees();
		return self.run(all_degs, None, break_on_first_fail);

	#here we will use all degrees except the one overidden by freedom_degs
	# run_only({'chrom' : 422}) will run with crhom 422 and all the other degrees
	def run_only(self, freedom_degs, break_on_first_fail = False):
		all_degs = self.list_freedom_degrees();
		for key,value in freedom_degs.iteritems():
			all_degs[key] = value;
		return self.run(all_degs, None, break_on_first_fail);

	#This function is called once before running the test with all the freedom degrees
	def pre_execute(self):
		pass;

	#This function is called once after having ran the test with all the freedom degrees
	def post_execute(self):
		pass;

	#This is the default implementation (could be overidden)
	def get_global_report(self, summary):
		pass;

	def generate_summary(self):
		result_codes = sorted(intopix_test.line_colours.keys())
		n_lines_per_code = {}
		for result_code in result_codes:
			n_lines_per_code[result_code] = len([line for line in self._report_lines if line['test_result']==result_code])

		summary_path = self._config["summary_path"] or self._log.get_logpath('summary.html');
		#Open summary for appending
		critical_error = (n_lines_per_code[intopix_test.CRITICAL] > 0)
		summary = intopix_test_report(summary_path, os.path.split(summary_path)[1], 2, True, critical_error);
		summary.add_title(self.__class__.__name__);
		summary.open_table();
		summary.add_table_row(['Test Description:', self.get_description()]);
		summary.add_table_row(['Test Start:', str(datetime.now())]);
		summary.add_table_row(['Test Duration:', utils.get_duration(self._start_time,self._stop_time)]);
		for result_code in result_codes:
			n_lines = n_lines_per_code[result_code]
			col = intopix_test.line_colours[result_code]
			summary.add_table_row(['Number of %s:' % result_code, n_lines], [col,'white'][n_lines == 0]);
		summary.add_table_row(['Report:', summary.generate_url_link('report', self.get_report_path())]);
		summary.close_table();
		self.get_global_report(summary);
		summary.close();

	def get_report_path(self):
		if (not self._report_path):
			self._report_path = self._log.get_logpath('report.html');
		return self._report_path;

	def init_reportline(self, preset = {}):
		reportline = {};
		for col, default in self.get_report_header():
			if preset.has_key(col):
				reportline[col] = preset[col]
			else:
				reportline[col] = default
		return reportline;

	def get_setup_variable(self, setupvar_name):
		return self._test_setup.get_setup_variable(setupvar_name);

#If you have multiple report lines for a single degree of your test,
#you may want to use idx to specify to which report_line your logpath refer
#If you want to group log paths with a subfolder, please specify it
	def create_logpath(self,logname, idx=0, subfolder=None, report_line=None):
		logpath = self._log.get_logpath(logname, subfolder);
		self.register_logfile(logname, logpath, idx);
		if report_line:
			if not report_line.has_key('logpath_list'):
				report_line['logpath_list'] = []
			report_line['logpath_list'].append( (logname, logpath) )
		return logpath;

	def register_logfile(self, name, path, idx=0):
		if (not self._list_logfiles.has_key(idx)):
			self._list_logfiles[idx] = [];
		#self._list_logfiles[idx].append((name,os.path.abspath(path)));
		self._list_logfiles[idx].append((name,path));

	def dump_logfiles(self):
		for k,v in self._list_logfiles.iteritems():
			for lname,fname in v:
				try:
					f=open(fname,'rb');
					for line in f.readlines():
						sys.stdout.write(line)
					f.close()
				except:
					pass;

	def cleanup_logfiles(self):
		self._list_logfiles = {};

	def create_xfgmcontext(self, **kwargs):
		args =  dict([item for item in self._config.items() if item[0] in ('xmldir','xfgmrun_path')]);
		args.update(kwargs);
		context = xfgmcontext(**args);
		self.register_xfgmcontext(context);
		return context;

	def register_xfgmcontext(self, context):
		self._list_xfgmcontexts.append(context);

	def cleanup_xfgmcontexts(self):
		for context in self._list_xfgmcontexts:
			if (context is not None and not context.is_finished()):
				context.stop(1);
		self._list_xfgmcontexts = [];

	#Debug function to load a report and test
	#the global report that could be made from it
	def read_report(self, report_path):
		self.prepare_logger();
		self._start_time = datetime.now();
		self._report_lines = intopix_test_report_parser(report_path).get_parsed_lines();
		self._stop_time = datetime.now();

