#!/usr/bin/python
import os;
import sys;
import re;
from subprocess import *;
import time;
import datetime;
import signal;
import tempfile;
import atexit;
from sys import exit;
import signal

try:
	import paramiko;
	has_paramiko = True;
except:
	has_paramiko = False;


#this class is a wrapper on top of paramiko AND Popen
#giving a common interface for way to
class executor(object):

	def __init__(self, cmd , log_stdout, log_stderr):
		self._cmd = cmd;
		self._logs = {};
		self._logs['stdout'] = log_stdout;
		self._logs['stderr'] = log_stderr;

	def run_blocking(self):
		self.run_non_blocking();
		self.wait_for_end();
		return self.get_exit_code();

	def wait_for_end(self, timeout = 0):
		start = datetime.datetime.now()
		while (True):
			ret = self.is_finished();
			if (ret == 0):
				time.sleep(0.1);
				now = datetime.datetime.now()
				if (timeout > 0) and ((now - start).seconds > timeout):
					break;
			else:
				break;
		if (ret < 0):
			return -1;
		else:
			return 0;

	def get_exit_code(self):
		print "Warning No implementation for get_exit_code in ", self.__name__;
		return 0;

	def kill(self):
		print "Warning No implementation for kill in ", self.__name__;
		return 0;

	def write_stdin(self, text):
		print "Warning No implementation for write_stdin in ", self.__name__;
		return 0;

	def is_finished(self):
		print "Warning No implementation for is_finished in ", self.__name__;
		return 0;




#The basic executor..., local
class local_executor(executor):
	
	__list_started = []

	@classmethod
	def cleanup(*args, **kwargs):
		cl = args[0]
		to_kill = tuple(cl.__list_started)
		for executor in to_kill:
			print "Killing", executor._cmd
			executor.kill()

	def __init__(self, cmd , log_stdout, log_stderr):
		self._child_info=[0,0];
		self._child = None;
		self._logs_hdl = {'stderr' : None, 'stdout' : None};
		super(self.__class__,self).__init__(cmd , log_stdout, log_stderr);

	def __close_fds(self):
		if (self._child):
			#Without doing this, the PIPE fd is never closed. Strange ?
			self._child.stdin.close();
		
	def __prepare_log_handles(self):
		for key in ('stderr', 'stdout'):
			try:
				self._logs_hdl[key] = open(self._logs[key],'w+b');
			except:
				print sys.exc_info()[0]
				print 'Error Cannot open log file for writing', self._logs[key];
				self.__close_log_handles();
				return -1;
		return 0;

	def __close_log_handles(self):
		for key in ( 'stderr', 'stdout' ):
			if (not self._logs_hdl[key] is None):
				self._logs_hdl[key].close();
				self._logs_hdl[key] = None;
		if self in local_executor.__list_started:
			local_executor.__list_started.remove(self)

	def start(self):
		if (self.__prepare_log_handles() < 0):
			return -1;
		try:
			self._child = Popen(self._cmd, stdout=self._logs_hdl['stdout'], stdin=PIPE, stderr=self._logs_hdl['stderr']);
			local_executor.__list_started.append(self)
		except:
			print "Unable to run Popen with following arguments", self._cmd
			print "Check \"%s\" is properly installed on your system" % self._cmd[0]
			return -1;
		return 0;

	def write_stdin(self, text):
		if (self._child != None):
			if (self._child.poll() == None):
				file.write(self._child.stdin, text);
			return 0;
		else:
			return -1;

	def is_finished(self):
		if (self._child != None):
			if (self._child.poll() == None):
				return 0;
			else:
				self.__close_log_handles();#automatically close files when user get exit code
				self.__close_fds();
				return 1;
		else:
			print 'Error: process not started';
			return -1;

	def kill(self):
		if (self._child is not None):
			self._child.kill();
			self._child.wait();
			self.__close_log_handles();#automatically close files when user get exit code
			self.__close_fds();

	def get_exit_code(self):
		if (self._child is None):
			return [None,2];#never started
		retcode=self._child.poll();
		if (retcode is None):
			return [None,None];
		self.__close_log_handles();#automatically close files when user get exit code
		if (retcode < 0):
			return [-self._child.poll(), 1];#was killed
		else:
			return [(self._child.poll()), 0];#exit normally




#the more complex (but same interface) executor - remote.
class remote_executor(executor):
	@classmethod
	def check_paramiko_installed(self):
		if (not has_paramiko):
			raise BaseException("You must install python paramiko module to use remote executors.");

	def __init__(self, cmd , log_stdout, log_stderr, address, username, password):
		self.check_paramiko_installed();
		self._address = address;
		self._username = username;
		self._password = password;
		self._remote_log_stdout= '';
		self._remote_log_stderr= '';
 		self._logs_on_host=False;
		self._chan = None;
		self._ssh = None;
		self._was_killed = False;
		super(self.__class__,self).__init__(cmd , log_stdout, log_stderr);

 	def __del__(self):
		if (self._ssh):
			self._ssh.close();

	def __prepare_remote_logs(self):
		(stdin,stdout,stderr) = self._ssh.exec_command("python -c 'import tempfile;import os;print os.path.join(tempfile.mkdtemp(),\"remote.log\");'");
		self._remote_log_stdout = stdout.read().rstrip("\n\r");
		self._remote_log_stderr = self._remote_log_stdout.replace(".log","2.log");

	def __get_remote_logs(self):
		if (not self._logs_on_host):
			self.get_file(self._remote_log_stdout,self._logs['stdout'],self._ssh);
			self.get_file(self._remote_log_stderr,self._logs['stderr'],self._ssh);
			self._logs_on_host = True;

	def start(self):
		try:
			self._ssh = paramiko.SSHClient();
			self._ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy());
			self._ssh.connect(self._address, username=self._username, password=self._password, timeout=10.0,allow_agent=False, look_for_keys=False);
			self.__prepare_remote_logs();
			self._ssh.exec_command("killall "+self._cmd[0]);
			cmd = self._cmd + ["> " + self._remote_log_stdout, "2> " + self._remote_log_stderr];
			self._chan = self._ssh.get_transport().open_session();
			self._chan.exec_command(' '.join(cmd));
		except:
			print "Unable to open remote command on %s" % self._address;
			self._ssh.close();
			raise;
		return 0;

	def write_stdin(self, text):
		if (self._chan != None):
			self._chan.send(text);
			return 0;
		else:
			return -1;

	def kill(self):
		if (self._chan is not None):
			self._chan.close();
			self.__get_remote_logs();#automatically close files when user get exit code
			self._was_killed = True;
			self._ssh.exec_command("killall "+self._cmd[0]);
			self._ssh.close();

	def is_finished(self):
		finished = 1;
		if (self._chan):
			finished = self._chan.exit_status_ready();
			if (finished):
				self.__get_remote_logs();
				self._ssh.close();
		return finished;

	def get_exit_code(self):
		try:
			if (self._chan is None):
				return [None,2];#never started
			if (self._was_killed):
				return [-1, 1];#was killed
			if (not self._chan.exit_status_ready()):
				return [None,None];
			else:
				return [(self._chan.recv_exit_status()), 0];#exit normally
		except:
			if (self._ssh):
				self._ssh.close();
			raise;
		return -1;

	#Some user may want to make small operations on remote machine
	#and store some files before executing cmds
	#dst must be a dir
 	@classmethod
	def copy_file_to_dir(self, src, dst, address, username, password):
		self.check_paramiko_installed();
		ssh = paramiko.SSHClient();
		try:
			ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy());
			ssh.connect(address, username=username, password=password, timeout=10.0,allow_agent=False, look_for_keys=False);
			(stdin,stdout,stderr) = ssh.exec_command("python -c 'import os; print os.path.join(\"%s\", \"%s\");'" % (dst,os.path.basename(src)));
			dst_file = stdout.read().rstrip("\n\r");
			self.put_file(src, dst_file, ssh);
		except:
			ssh.close();
			raise;
		ssh.close();
		return dst_file;

	@classmethod
	def put_file(self, src, dst, ssh):
		if (not os.path.exists(src)):
			print "File "+ src + "does not exist";
		else:
			self.check_paramiko_installed();
			try:
				sftp = ssh.open_sftp();
				sftp.put(src, dst);
				sftp.close();
			except:
				ssh.close();
				raise;

	@classmethod
	def get_file(self, src, dst, ssh):
		self.check_paramiko_installed();
		try:
			sftp = ssh.open_sftp();
			sftp.get(src, dst);
			sftp.close();
		except:
			ssh.close();
			raise;

	#Some user may want to make small operations on remote machine
	#and store some files before executing cmds
	@classmethod
	def get_remote_tmp_dir(self, address, username, password):
 		self.check_paramiko_installed();
		ssh = paramiko.SSHClient();
		try:
			ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy());
			ssh.connect(address, username=username, password=password, timeout=10.0,allow_agent=False, look_for_keys=False);
			(stdin,stdout,stderr) = ssh.exec_command("python -c 'import tempfile; print tempfile.mkdtemp();'");
			tmp_dir = stdout.read().rstrip("\n\r");
		except:
			ssh.close();
			raise;
		ssh.close();
		return tmp_dir;



if __name__ == '__main__':
	my_remote_executor = remote_executor(['xfgmrun', '/usr/local/share/intopix/Pristine_P4_SDI_Streaming/xml/SDI_Ingest_1ch.xml', '--mywriter.dontwrite 1'] , 'remote_output.log', 'remote_error.log', '10.128.0.136', 'intopix', 'intopix');
	#my_remote_executor = remote_executor(['tail','-f', '/dev/null'] , 'remote_output.log', 'remote_error.log', '10.128.0.136', 'intopix', 'intopix');
	my_remote_executor.start();
	time.sleep(5);
	my_remote_executor.kill();
	my_remote_executor.__del__();

#Automatically kill local executors when SIGTERM received
atexit.register(local_executor.cleanup, local_executor)
for sig in (signal.SIGTERM,):
	signal.signal(sig, lambda signum, stack_frame: exit(1))
