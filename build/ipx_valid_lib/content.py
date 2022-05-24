import types, sys, os, time, re, shutil, getpass, subprocess, pickle, base64, copy, itertools
from subprocess import *

from shutil import copyfile, copytree, rmtree
from utils import find_all_files_in_dir, dict_product


#One intopix content creator package define helpers for creating
#content types that share the same remotepath/localpath (most of our contents do)
class intopix_content_creators_package:

	#default parameters for content creation
	ipx_content_src = os.getenv("IPX_CONTENT_SRC")
	if not ipx_content_src:
		if sys.platform == "win32":
			ipx_content_src = "K:\\content_path"
		elif sys.platform == "cygwin":
			ipx_content_src = "/cygdrive/c/content_path"
		else:
			ipx_content_src = "/mnt/content_path"
	ipx_content_cache = os.getenv("IPX_CONTENT_CACHE")
	if not ipx_content_cache:
		if sys.platform == "win32":
			ipx_content_cache = ".\\ImagesCache"
		elif sys.platform == "cygwin":
			ipx_content_cache = "./ImagesCache"
		else:
			ipx_content_cache = "./ImagesCache"
	_config = { "remotepath": ipx_content_src, "localpath": ipx_content_cache }
	if not os.path.isdir(ipx_content_cache):
		os.mkdir(ipx_content_cache)

	def __init__(self, **kwargs):
		self._config = dict(self._config.items());
		self._autogen_keys = {}
		self._content_creator_list = {};
		for confname in kwargs.keys():
			if confname in self._config.keys():
				self._config[confname] = kwargs[confname];
			else:
				print "Warning: unkown config parameters found in intopix_content_creators_package init", confname;
		if (not os.path.exists(self._config['localpath'])):
			print "Test images retrieval failed"
			print "Please adapt the IPX_CONTENT_CACHE variable to an existing folder where you want TestImages/RegressionTest to be cached on your system"
			print "Or simply create ImagesCache inside current directory"
			raise BaseException("Unable to find local content storage path : " + self._config['localpath']);
		if (not os.path.exists(self._config['remotepath'])):
			print "Test images retrieval failed"
			print "Please adapt the IPX_CONTENT_SRC variable to a path like where TestImages/RegressionTest is on your system"
			print "Or make a simlink to this path from /mnt/content_path"
			raise BaseException("Unable to find remote content storage path : " + self._config['remotepath']);

	def autogen_pack(self, agid, params):
		params_list = [params[k] for k in self._autogen_keys[agid]]
		params_list = [base64.b64encode(params_list[0], "+-")] + list(params_list[1:])
		return ("_ag_%s_" % agid) + "_".join([str(si) for si in params_list])

	def autogen_unpack(self, agid, params):
		args = params.split("_")
		return dict(zip(self._autogen_keys[agid], [base64.b64decode(args[0], "+-")] + args[1:]))

	def autogen_pack_product(self, agid, params):
		return [self.autogen_pack(agid, i) for i in dict_product(params)]

	def register_content_creator(self, content_creator_name, content_creator):
		if (self._content_creator_list.has_key(content_creator_name)):
			print 'Content with name', content_creator_name, "already exists!";
			return -1;
		content_creator.set_package(self)
		content_creator.set_paths(self._config['remotepath'], self._config['localpath']);
		self._content_creator_list[content_creator_name] = content_creator;
		return 0;

	def get_content_list(self):
		return self._content_creator_list

	#A sequence content is a content made of %0Xd numbers
	# (content_name, content_path, image_first, image_last, repeat) (repeat is not mandatory 1 means no repeat, 2 means repeated once => played twice))
	def register_seq(self, name, path, first_index, last_index, repeat = 1):
		local_filename = 'img_%06d' + os.path.splitext(path)[1];
		content_creator = intopix_seq_content_creator(name=name, path=path, local_filename = local_filename, first_index=first_index, last_index=last_index, repeat=repeat);
		return self.register_content_creator(name, content_creator);

	def register_local_seq(self, name, path, first_index, last_index):
		content_creator = intopix_local_seq_content_creator(name=name, path=path, first_index=first_index, last_index=last_index);
		return self.register_content_creator(name, content_creator);

	def register_fold(self, name, path):
		content_creator = intopix_fold_content_creator(name=name, path=path);
		return self.register_content_creator(name, content_creator);

	def register_autogen(self, name, local_dir_name, file_basename, gen_function, args):
		content_creator = intopix_autogen_content_creator(name=name, local_dir_name=local_dir_name,file_basename=file_basename,gen_function=gen_function, args=args);
		return self.register_content_creator(name, content_creator);

	def register_autogenseq(self, ag_id, base_package, gen_function, autogen_keys, args):
		name = '^_ag_(%s)_(.*)$' % ag_id
		content_creator = intopix_autogenseq_content_creator(name = name, base_package = base_package, gen_function = gen_function, args = args)
		self._autogen_keys[ag_id] = autogen_keys
		return self.register_content_creator(name, content_creator)

	def register_multiseq(self, name, seq_list):
		local_filename = 'img_%06d' + os.path.splitext(seq_list[0][0])[1];
		content_creator = intopix_multiseq_content_creator(name=name, local_filename = local_filename, seq_list=seq_list);
		return self.register_content_creator(name, content_creator);

	def register_local_seq_list(self, lines):
		self.register_list(self.register_local_seq,lines)

	def register_seq_list(self, lines):
		self.register_list(self.register_seq,lines);

	def register_fold_list(self, lines):
		print "FIXME NOT SUPPORTED!!!!"
		self.register_list(self.register_fold,lines);

	def register_autogen_list(self, lines):
		self.register_list(self.register_autogen,lines);

	def register_autogenseq_list(self, lines):
		self.register_list(self.register_autogenseq,lines);

	def register_multiseq_list(self, lines):
		self.register_list(self.register_multiseq,lines);

	def register_list(self,func,lines):
		ret = 0;
		for line in lines:
			try:
				if (func(*line)<0):
					raise BaseException();
			except:
				ret = -1;#do not break, just warn
				print "Unable to handle content creator line:", line
		return ret;

	def find_content_creator(self, name):
		#either find the creator with the name of the content
		#or with a name matching name of content (reg exp style)
		if (self._content_creator_list.has_key(name)):
			return self._content_creator_list[name];
		else:
			for ref_content_name in self._content_creator_list.keys():
				p = re.match(ref_content_name + "$", name); #use "+ $" so that edyuv_1920x1080 does not match edyuv
				if (p):
					return self._content_creator_list[ref_content_name];
		return None;


#The factory owns packages of "content creators".
#It finds, based on content name, the right creator
#and create the content related
class intopix_content_factory():

	_list_content_packages = [];

	def register_package(self, package):
		#FIXME: maybe warn if two packages export the same name?
		self._list_content_packages.append(package);

	def create_content(self, content_name):
		content = None;
		if (1):#try:
			for package in self._list_content_packages:
				content_creator = package.find_content_creator(content_name);
				if (content_creator):
					content = content_creator.create_content(content_name);
					break;
		#except Exception as e:
			#print "Error creating content: ", content_name
			#print e
		return content;



class intopix_content_creator(object):

	def __init__(self,**kwargs):
		self._config={};
		self._config['local'] = False
		for confname in kwargs.keys():
			self._config[confname] = kwargs[confname];

	def set_paths(self, remotepath, localpath):
		self._config["remotepath"] = remotepath;
		self._config["localpath"] = localpath;

	def get_local_path(self, *subdirs):
		return os.path.join(self._config['localpath'] , *subdirs);

	def get_remote_path(self, *subdirs):
		return os.path.join(self._config['remotepath'] , *subdirs);

	def copy_files_list(self, src_dst_list):
		#basic check of file existance
		for src,dst in src_dst_list:
			shutil.copyfile(src, dst);
			print "cp",src,dst

	def set_package(self, package):
		self._package = package

	def printout(self):
		for a,b in self._config.iteritems():
			print a,b

class intopix_local_seq_content_creator(intopix_content_creator):
	#name
	#path
	#first_index
	#last_index
	def __init__(self,**kwargs):
		intopix_content_creator.__init__(self,**kwargs)
		self._config['local'] = True

	def create_content(self, content_name):
		content = intopix_seq_content(content_name,self._config["path"],self._config["last_index"]-self._config["first_index"]+1,self._config["first_index"],True)
		#Just do a sanity check on content
		first_frame_path = content.get_path(file_index=content.get_first_index())
		last_frame_path = content.get_path(file_index=(content.get_first_index() + content.get_length() - 1))

		if not os.path.exists(first_frame_path) or not os.path.exists(last_frame_path):
			print "ERROR WITH LOCAL CONTENT"
			print "Please check path/start index and stop index of Local Content:", content_name
			return None
		return content


class intopix_seq_content_creator(intopix_content_creator):

	def __init__(self,**kwargs):
		intopix_content_creator.__init__(self,**kwargs);

	def create_content(self, content_name):
		local_dir_path = self.get_local_path(content_name);
		local_file_path = self.get_local_path(content_name, self._config['local_filename']);
		remote_file_path = self.get_remote_path(self._config['path']);

		if (not os.path.exists(local_dir_path)):
				os.mkdir(local_dir_path);

		image_indexes = range(self._config['first_index'], self._config['last_index']+1) * self._config['repeat'];
		n_files = len(image_indexes);
		if (self._config['first_index'] != -1):
			src_files = [(remote_file_path % i) for i in image_indexes];
		else:
			src_files = [remote_file_path] * len(image_indexes);
		dst_files = [(local_file_path % i) for i in range(0,len(image_indexes))];
		#check if exists (first image, last image, in between image)
		if (not os.path.exists(dst_files[0]) or not os.path.exists(dst_files[-1]) or not os.path.exists(dst_files[len(dst_files)/2])):
			print "Getting Content:", content_name
			self.copy_files_list(zip(src_files,dst_files));

		content = intopix_seq_content(content_name, local_file_path, n_files);
		return content;



class intopix_autogen_content_creator(intopix_content_creator):

	def __init__(self,**kwargs):
		intopix_content_creator.__init__(self,**kwargs);

	def create_content(self, content_name):
		#We need to autogenerate the content
		#in this case, name of content is not name of content_creator !
		content = None;
		p = re.match(self._config['name'], content_name);
		if (p):
			#name=name, local_dir_name=local_dir_name,file_basename=file_basename,gen_function=gen_function, args=args)
			#Generate the content and create the intopix_content object
			params = p.groups();
			local_dir_path = self.get_local_path(self._config['local_dir_name']);
			local_file_path = self.get_local_path(self._config['local_dir_name'], self._config['file_basename'] % params);
			if (not os.path.exists(local_dir_path)):
				os.mkdir(local_dir_path);
			ret = self._config['gen_function']( local_file_path, params ,self._config['args']);
			if (ret < 0):
				print "Error generating content (generation functions return < 0)", content_name;
			else:
				content = intopix_monofile_content(content_name, local_file_path);
		return content;

class intopix_autogenseq_content_creator(intopix_content_creator):

	def __init__(self,**kwargs):
		intopix_content_creator.__init__(self,**kwargs)

	def create_content(self, content_name):
		# We need to autogenerate the sequence
		content = None
		p = re.match(self._config['name'], content_name)
		if (p):
			# Unpack parameters
			agid, params = p.groups()
			params = self._package.autogen_unpack(agid, params)
			local_dir_path = self.get_local_path(content_name)

			if (not os.path.exists(local_dir_path)):
				os.mkdir(local_dir_path)

			if params.has_key('content'):
				cf = intopix_content_factory()
				cf.register_package(self._config['base_package'])
				base_sequence = cf.create_content(params['content'])
			else:
				base_sequence = None
			if params.has_key('extension'):
				prefix = "img_%06d." + params["extension"].lstrip(".")
			else:
				prefix = "img_%06d.j2k" #extension is currently not passed out by decoder ag content

			n_files = self._config['gen_function'](
				base_sequence, self.get_local_path(content_name, prefix),
				params, self._config['args'])
			if n_files < 0:
				print "Error generating content (generation functions return < 0)", content_name
			content = intopix_seq_content(content_name, self.get_local_path(content_name, prefix), n_files)
		return content

class intopix_multiseq_content_creator(intopix_content_creator):

	def __init__(self,**kwargs):
		intopix_content_creator.__init__(self,**kwargs);

	#intopix_multiseq_content_creator(name=name, local_filename = local_filename, seq_list=seq_list);
	def create_content(self, content_name):
		local_dir_path = self.get_local_path(content_name);
		local_file_path = self.get_local_path(content_name, self._config['local_filename']);
		local_file_index = 0;
		n_files = 0;

		if (not os.path.exists(local_dir_path)):
			os.mkdir(local_dir_path);

		for seq in self._config['seq_list']:
			image_indexes = None
			#a seq must have following form :
			#2 arguments : path, index_list
			#3 arguments : path, start_index, stop_index
			#4 arguments : path, start_index, stop_index, repeat
			#Note: if files without index, use -1 as start and stop_index
			if len(seq) == 2:
				path, image_indexes = seq
				if (not getattr(image_indexes, '__iter__', False)) or (len(image_indexes) == 0):
					print "Content with name %s bad format for second argument! Must be iterable" % (content_name,)
					exit(-1);
			elif (len(seq) == 3):
				path, first_index, last_index = seq;
				repeat = 1;
			elif (len(seq) == 4):
				path, first_index, last_index, repeat = seq;
			else:
				print "Error in content", content_name;
				print "Error with seq parameter ", seq;
				return None;

			remote_file_path = self.get_remote_path(path);

			if not image_indexes:
				image_indexes = range(first_index, last_index+1)*repeat;

			n_files += len(image_indexes);
			if (image_indexes[0] != -1):
				src_files = [(remote_file_path % i) for i in image_indexes];
			else:
				src_files = [remote_file_path] * len(image_indexes);
			dst_files = [(local_file_path % i) for i in range(local_file_index,local_file_index+len(image_indexes))];
			#check if exists (first image, last image, in between image)
			if (not os.path.exists(dst_files[0]) or not os.path.exists(dst_files[-1]) or not os.path.exists(dst_files[len(dst_files)/2])):
				print "Getting Content:", content_name, path
				self.copy_files_list(zip(src_files,dst_files));
			local_file_index+=len(image_indexes);

		content = intopix_seq_content(content_name, local_file_path, n_files);
		return content;

class intopix_fold_content_creator(intopix_content_creator):
	# path : path to folder
	def __init__(self,**kwargs):
		intopix_content_creator.__init__(self,**kwargs);

	def create_content(self, content_name):
		local_dir_path = self.get_local_path(content_name);
		remote_file_path = self.get_remote_path(self._config['path']);

		if (not os.path.exists(local_dir_path)):
			os.mkdir(local_dir_path);

		src_filenames = [f for f in os.listdir(remote_file_path) if os.path.isfile(os.path.join(remote_file_path,f))]
		src_files = [os.path.join(remote_file_path,name) for name in src_filenames]
		dst_files = [os.path.join(local_dir_path,name) for name in src_filenames]

		if (not os.path.exists(dst_files[0]) or not os.path.exists(dst_files[-1]) or not os.path.exists(dst_files[len(dst_files)/2])):
			print "Getting Content:", content_name
			self.copy_files_list(zip(src_files,dst_files));

		content = intopix_fold_content(content_name, local_dir_path, src_filenames);
		return content;


#Master class of all contents
class intopix_content(object):

	def __init__(self, name):
		self._name = name;

	def get_name(self):
		return self._name

	#Supported:
	#	fileindex
	#	filename
	def get_path(self, **kwargs):
		#must be overidden!
		pass;

	#Compatibility function
	def get_pathname(self, filename=None):
		if filename:
			return self.get_path(filename=filename);
		else:
			return self.get_path();


class intopix_seq_content(intopix_content):

	def __init__(self,content_name, local_file_path, n_files, first_index = 0, is_local = False):
		self._local_file_path = local_file_path;
		self._first_index = first_index
		self._n_files = n_files;
		self._is_local = is_local;
		intopix_content.__init__(self,content_name);

	def get_path(self, **kwargs):
		file_index=kwargs.pop('file_index', None);
		if (file_index is None):
			return self._local_file_path;
		else:
			return self._local_file_path % file_index;

	def get_first_index(self):
		return self._first_index

	def get_length(self):
		return self._n_files;

	def get_max_size(self):
		max_size = 0;
		#print self.get_length()
		for i in range(self.get_length()):
			filepath = self.get_path(file_index=i+self._first_index);
			size = os.path.getsize(filepath);
			if (size > max_size):
				max_size = size;
		return max_size;

	def get_max_size_merged(self,n_comp):
		max_size = 0;
		#print self.get_length()
		for i in range((self.get_length())/n_comp):
			size = 0;
			for j in range(n_comp):
				filepath = self.get_path(file_index=i*n_comp+j);
				size = size+os.path.getsize(filepath);
			if (size > max_size):
				max_size = size;
		return max_size;

	def get_mean_size(self):
		total_size = 0.00;
		#print self.get_length()
		for i in range(self.get_length()):
			filepath = self.get_path(file_index=i);
			total_size = total_size + os.path.getsize(filepath);
		return total_size/self.get_length();

	def get_mean_size_merged(self,n_comp):
		total_size = 0.00;
		#print self.get_length()
		for i in range(self.get_length()):
			filepath = self.get_path(file_index=i);
			total_size = total_size + os.path.getsize(filepath);
		return n_comp*total_size/self.get_length();

	def get_max_pix_count(self,n_frames):
		max_pix_cnt={};
		max_pix_cnt['total'] = 0;
		#print self.get_length()
		for i in range(min(self.get_length(),n_frames)):
			filepath = self.get_path(file_index=i);
			j2k_options = kakadu_lib.get_j2k_properties_parsed(filepath);
			pix_cnt = j2k_options["resolution"][0]*j2k_options["resolution"][1]
			if pix_cnt > max_pix_cnt['total']:
				max_pix_cnt['total'] = pix_cnt;
				max_pix_cnt['size_x'] = j2k_options["resolution"][1];
				max_pix_cnt['size_y'] = j2k_options["resolution"][0];
		return max_pix_cnt;

class intopix_monofile_content(intopix_content):

	def __init__(self,content_name, local_file_path):
		self._local_file_path = local_file_path;
		intopix_content.__init__(self,content_name);

	def get_path(self, **kwargs):
		return self._local_file_path;

	def get_length(self,):
		return self._n_files;


class intopix_fold_content(intopix_content):

	def __init__(self,content_name, local_dir_path, src_filenames):
		self._content_name = content_name
		self._local_dir_path = local_dir_path
		self._src_filenames = sorted(src_filenames)

	def get_name(self, **kwargs):
		filename=self._src_filenames[kwargs.pop('idx', 0)]
		return filename

	def get_path(self, **kwargs):
		filename = kwargs.get('filename',False)
		if not filename:
			filename=self._src_filenames[kwargs.pop('idx', 0)]
		return os.path.join(self._local_dir_path, filename)

	def get_all(self):
		return [ (self.get_name(idx=idx),self.get_path(idx=idx)) for idx in range(self.get_length())]

	def get_length(self):
		return len(self._src_filenames);


if __name__ == "__main__":
	def generate_noise_raw24_picture( filepath, params, args):
		print params
		print args
		f = open(filepath, 'w+b');
		f.write(os.urandom(int(params[0])*int(params[1])*3));
		f.close();
		return 0;

	def generate_j2k_noise( filepath, params, args):
		print params
		print args
		f = open(filepath, 'w+b');
		f.write(os.urandom(int(params[0])));
		f.close();
		return 0;


	cp1 = intopix_content_creators_package();
	cp1.register_seq('evertz_corrupted_frames','1080_60i_422/Evertz_corrupted/compressed/j2k/ko1_file_%06d.j2k',0,25);
	cp1.register_seq_list([('evertz_corrupted_frames1','1080_60i_422/Evertz_corrupted/compressed/j2k/ko1_file_%06d.j2k',0,21),
													('evertz_corrupted_frames2','1080_60i_422/Evertz_corrupted/compressed/j2k/ko1_file_%06d.j2k',0,32)]);

	cp1.register_autogen('noise_([0-9]+)x([0-9]+)_raw','noise_allim_sizes_raw','img_%s_%s.raw',generate_noise_raw24_picture, ("testarg1","testarg2"));

	cp1.register_multiseq('test_content',(
			["1920_1080/CW-ComFocus/Compressed/j2k/97/comfocus_1080p-0a_00000.j2k",-1,-1,3],
			["1920_1080/BigBuckBunny/Compressed/j2k/97/big_buck_bunny_%05d.j2k",[1,300,500]],
			["1920_1080/CW-ComFocus/Compressed/j2k/97/comfocus_1080p-0a_00000.j2k",-1,-1,3]
			));

	cf = intopix_content_factory();
	cf.register_package(cp1);
	content1 = cf.create_content('evertz_corrupted_frames');
	content2 = cf.create_content('evertz_corrupted_frames1');
	content3 = cf.create_content('evertz_corrupted_frames2');
	content4 = cf.create_content('noise_18x42_raw');
	content5 = cf.create_content('test_content');

	print content1.get_path();
	print content2.get_path();
	print content3.get_path();
	print content4.get_path();




