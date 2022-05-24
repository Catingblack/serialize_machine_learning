#!/usr/bin/python

#This File contains the different hardware/software setups that can be used by the tests
#Each setup provide a preparation function for the test to be put in place and a description
import pickle;
import itertools;
import copy;
import utils;
import content;

def bylength(list1, list2):
	return len(list1) - len(list2);

class intopix_test_setups():

	_list_resources = { 'operator' : { 'instruction' : "An opertor must be present during the test"},
						'content' : { 'instruction' : "The content server needs to be ready"},
						'no_operator' : {}, #trick to put the line 'operator', 'no_operator' in incompatibles
						'linux_server' : {},
						'windows_server' : {},
						'pristine_dec_0' : { 'instruction' : "A PRISTINE4 Decoder should be present with ID 0"},
						'pristine_dec_1' : { 'instruction' : "A PRISTINE4 Decoder should be present with ID 1"},
						'pristine_enc_0' : { 'instruction' : "A PRISTINE4 Encoder should be present with ID 0"},
						'pristine_enc_1' : { 'instruction' : "A PRISTINE4 Encoder should be present with ID 1"},
						'phabrix' : { 'instruction' : "A Phabrix connected to network is required" , 'params' : [ ('phabrix_ip','What is Phabrix IP address?')] },
						'panel0_TX0_on_PhabIn' : { 'instruction' : "Panel0 TX0 must be connected to Phabrix SDI IN port"},
						'panel0_TX1_on_PhabIn' : { 'instruction' : "Panel0 TX1 must be connected to Phabrix SDI IN port"},
						'panel0_TX2_on_PhabIn' : { 'instruction' : "Panel0 TX2 must be connected to Phabrix SDI IN port"},
						'panel0_TX3_on_PhabIn' : { 'instruction' : "Panel0 TX3 must be connected to Phabrix SDI IN port"},
						'PhabOut_on_panel1_RX0': { 'instruction' : "Phabrix SDI OUT port must be connected to Panel1 RX0" },
						'panel0_TX0_on_panel1_RX0' : { 'instruction' : "Panel0 TX0 must be connected to Panel1 RX0" },
						'panel0_TX1_on_panel1_RX1' : { 'instruction' : "Panel0 TX1 must be connected to Panel1 RX1" },
						'panel0_TX2_on_panel1_RX2' : { 'instruction' : "Panel0 TX2 must be connected to Panel1 RX2" },
						'panel0_TX3_on_panel1_RX3' : { 'instruction' : "Panel0 TX3 must be connected to Panel1 RX3" },
						'PhabOut_on_Repeater_on_panel1_RX0' : { 'instruction' : "Phabrix SDI OUT port must be connected to an AJA repeater\nOne AJA Output must be connected on Panel1 RX0" },
						'PhabOut_on_Repeater_on_panel1_RX1' : { 'instruction' : "Phabrix SDI OUT port must be connected to an AJA repeater\nOne AJA Output must be connected on Panel1 RX1" },
						'PhabOut_on_Repeater_on_panel1_RX2' : { 'instruction' : "Phabrix SDI OUT port must be connected to an AJA repeater\nOne AJA Output must be connected on Panel1 RX2" },
						'PhabOut_on_Repeater_on_panel1_RX3' : { 'instruction' : "Phabrix SDI OUT port must be connected to an AJA repeater\nOne AJA Output must be connected on Panel1 RX3" },
						'PhabOut_on_Repeater_on_panel0_RX0' : { 'instruction' : "Phabrix SDI OUT port must be connected to an AJA repeater\nOne AJA Output must be connected on Panel0 RX0" },
						'PhabOut_on_Repeater_on_panel0_RX1' : { 'instruction' : "Phabrix SDI OUT port must be connected to an AJA repeater\nOne AJA Output must be connected on Panel0 RX1" },
						'PhabOut_on_Repeater_on_panel0_RX2' : { 'instruction' : "Phabrix SDI OUT port must be connected to an AJA repeater\nOne AJA Output must be connected on Panel0 RX2" },
						'PhabOut_on_Repeater_on_panel0_RX3' : { 'instruction' : "Phabrix SDI OUT port must be connected to an AJA repeater\nOne AJA Output must be connected on Panel0 RX3" },
						'remote_machine' : { 'instruction' : "A remote machine must be accessible by SSH", 'params' : [ ('remote_ip','What is Remote Machine IP address?') ]},
						'remote_machine_with_windows' : { 'instruction' : "A remote machine with windows running must be accessible by SSH", 'params' : [ ('remote_ip','What is Remote Machine IP address?') ]},
						'remote_dec0' : { 'instruction' : "A decoder board must be present on the remote machine"},
						'remote_enc0' : { 'instruction' : "An encoder board with device id 0 must be present on the remote machine"},
						'remote_enc1' : { 'instruction' : "An encoder board with device id 1 must be present on the remote machine"},
						'panel0_TX0_on_remotepanel0_RX0' : { 'instruction' : 'Panel0 TX0 must be connected on RX0 of Panel0 of remote machine'},
						'panel0_TX0_on_remotepanel1_RX0' : { 'instruction' : 'Panel0 TX1 must be connected on RX0 of Panel1 of remote machine'},
						'panel1_TX0_on_remotepanel0_RX1' : { 'instruction' : 'Panel0 TX0 must be connected on Panel0 of remote machine RX1'},
						'panel0_TX1_on_remotepanel0_RX2' : { 'instruction' : 'Panel0 TX1 must be connected on Panel0 of remote machine RX2'},
						'panel1_TX1_on_remotepanel0_RX3' : { 'instruction' : 'Panel0 TX1 must be connected on Panel0 of remote machine RX3'},
						'panel0_TX0_on_panel1_RX0_and_remotepanel0_RX0' : { 'instruction' : 'Panel0 TX0 must be connected to a "T".\n First output of "T" should go to RX0 of remote machine Panel0. \nSecond must go to panel1 RX0.'},
						'panel1_TX0_on_panel0_RX0' : { 'instruction' : 'Panel1 TX0 must be connected to panel0 RX0'},
                        '6_speakers' : {'instruction' : '6 speakers must be connected to your machine'},
						'geffen' : {'instruction' : 'A geffen with a timecoded output in 1080i format'},
						'GeffenOut_on_panel1_RX' : {'instruction' : 'Connect one geffen output to RX0 of panel1'},
					};

	_list_incompatibles = [ ('pristine_dec_1',	'pristine_enc_1'),
							 ('windows_server',	'linux_server'),
							 ('panel0_TX0_on_PhabIn','panel0_TX1_on_PhabIn','panel0_TX2_on_PhabIn','panel0_TX3_on_PhabIn'),
							 ('panel0_TX0_on_panel1_RX0' , 'panel0_TX0_on_PhabIn', 'panel0_TX0_on_remotepanel0_RX0', 'panel0_TX0_on_panel1_RX0_and_remotepanel0_RX0'),
							 ('panel0_TX1_on_panel1_RX1' , 'panel0_TX1_on_PhabIn', 'panel0_TX1_on_remotepanel0_RX2'),
							 ('panel0_TX2_on_panel1_RX2' , 'panel0_TX2_on_PhabIn', 'panel1_TX0_on_remotepanel0_RX1'),
							 ('panel0_TX3_on_panel1_RX3' , 'panel0_TX3_on_PhabIn', 'panel1_TX1_on_remotepanel0_RX3'),
							 ('PhabOut_on_panel1_RX0', 'panel0_TX0_on_panel1_RX0', 'PhabOut_on_Repeater_on_panel1_RX0', 'panel0_TX0_on_panel1_RX0_and_remotepanel0_RX0'),
							 ('PhabOut_on_panel1_RX0', 'PhabOut_on_Repeater_on_panel1_RX0', 'PhabOut_on_Repeater_on_panel1_RX1', 'PhabOut_on_Repeater_on_panel1_RX2',
								'PhabOut_on_Repeater_on_panel1_RX3', 'PhabOut_on_Repeater_on_panel0_RX0','PhabOut_on_Repeater_on_panel0_RX1','PhabOut_on_Repeater_on_panel0_RX2',
								'PhabOut_on_Repeater_on_panel0_RX3'),
							('remote_dec0', 'remote_enc0'),
							('no_operator', 'operator'),
							];

	_list_automatic = {	'phabrix' : ('panel0_TX0_on_PhabIn','panel0_TX1_on_PhabIn','panel0_TX2_on_PhabIn','panel0_TX3_on_PhabIn','PhabOut_on_panel1_RX0',
									'PhabOut_on_Repeater_on_panel1_RX0','PhabOut_on_Repeater_on_panel1_RX1','PhabOut_on_Repeater_on_panel1_RX2',
									'PhabOut_on_Repeater_on_panel1_RX3','PhabOut_on_Repeater_on_panel0_RX0','PhabOut_on_Repeater_on_panel0_RX1',
									'PhabOut_on_Repeater_on_panel0_RX2','PhabOut_on_Repeater_on_panel0_RX3')
						};

	_list_typical_setups = {
							'DecEncLoopSDI' : ['no_operator','content', 'linux_server', 'pristine_dec_0','pristine_enc_1','panel0_TX0_on_panel1_RX0',
													'panel0_TX1_on_panel1_RX1','panel0_TX2_on_panel1_RX2','panel0_TX3_on_panel1_RX3'],
							};



	_list_current_resources = [];
	_list_params = {};

	_resource_variables = {
		'phabrix' : None,
		'remote_ip' : None,
	};


	_setup_config_save_filename = "test_setup.save";
	#_list_std_setups = { 'dec0_enc1_with_panels' : [ 'pristine_dec_0', 'pristine_dec_1' , ]
	#					'pristine_enc_1',],
	#				];

#---------------------------------------------------------------------------------------------
#Named with __prepare_"name of resource". Automatically called.
#List of function to prepare something in a custom way (not just print instruction on screen)
#
	def __prepare_phabrix(self):
		self._resource_variables['phabrix'] = phabrix(self._list_params['phabrix_ip']);

	def __prepare_remote_machine(self):
		self._resource_variables['remote_ip'] = self._list_params['remote_ip'];

	def __prepare_content(self):
		#content_copier.prepare_remote_server();
		pass

#---------------------------------------------------------------------------------------------
	def __get_resource_by_name(self, resource_name):
		if (self._list_resources.has_key(resource_name)):
			resource = self._list_resources[resource_name];
			resource["name"] = resource_name;
			return resource;
		else:
			return None;

	def __prepare_resource(self, resource):
		if (resource.has_key('instruction')):
			print resource['instruction'];
		else:
			print resource['name'];
		if (resource.has_key('params')):
			for param_name, ask_sentence in resource['params']:
				self._list_params[param_name] = raw_input(ask_sentence+"\n");

		custom_prepare_function_name = '_intopix_test_setups__prepare_'  + resource["name"];
		if (hasattr(self,custom_prepare_function_name)):
			 getattr(self, custom_prepare_function_name)();

	def __reload_resource(self, resource):
		custom_prepare_function_name = '_intopix_test_setups__prepare_'  + resource["name"];
		if (hasattr(self,custom_prepare_function_name)):
			 getattr(self, custom_prepare_function_name)();

	def __save_setup(self):
		dump_object = (self._list_current_resources, self._list_params);
		f = open(self._setup_config_save_filename, 'w');
 		pickle.dump(dump_object, f);
 		f.close();

	def __load_old_setup_if_match(self, setup_resources_names):
		try:
			f_fd = open(self._setup_config_save_filename, 'r');
			dump_object = pickle.load(f_fd);
			(self._list_current_resources, self._list_params) = dump_object;
			f_fd.close();
		except:
			pass;
		for resource_name in setup_resources_names:
			if (not resource_name in self._list_current_resources):
				break;
		else:
			for resource_name in setup_resources_names:
				self.__reload_resource(self.__get_resource_by_name(resource_name));
			return 0;
		return -1;

	def prepare_resources(self, setup_resources_names, prevent_reload = False):
		if ((not prevent_reload) and (self.__load_old_setup_if_match(setup_resources_names) == 0)):
				pass
				#print "Assuming setup did not change...";
		else:
			print "Here is what your setup requires:"
			setup_resources = [];
			for resource_name in setup_resources_names:
				resource = self.__get_resource_by_name(resource_name);
				if (resource):
					setup_resources.append(resource);
				else:
					print "Unkown Resource!", resource_name;
					return -1;

			for idx,resource in enumerate(setup_resources):
				print idx, ")\t",;
				self.__prepare_resource(resource);
				print "";
			raw_input("Press ENTER When Setup is properly in place");
			self._list_current_resources = setup_resources_names;
			self.__save_setup();

	def prepare_setup(self, setup_name, prevent_reload = False):
		return self.prepare_resources(self._list_typical_setups[setup_name], prevent_reload);

	def get_setup_variable(self, setupvar_name):
		return self._resource_variables[setupvar_name];

	def get_remote_machine_ip(self):
		return self._remote_ip;

	def __check_valid(self, setup):
		for contraint in self._list_incompatibles:
			for bi_element in itertools.combinations(contraint,2):
				if (bi_element[0] in setup and bi_element[1] in setup):
					return False;
		return True;

	def get_next_setups( self, current_resources , remaining_resources , depth=0):
		setups = [];
		remaining_resources2 = copy.copy(remaining_resources);
		for added_resource in remaining_resources:
			new_current_resources = current_resources + [added_resource];
			remaining_resources2.remove(added_resource);
			new_remaining_resources = copy.copy(remaining_resources2);
			for incompatible_resource in self._list_incompatible_with_resource[added_resource]:
				if (incompatible_resource in new_remaining_resources):
					#print incompatible_resource,new_remaining_resources
					new_remaining_resources.remove(incompatible_resource);

			setups.extend( self.get_next_setups( new_current_resources , new_remaining_resources, depth+1 ) );
		if (len(remaining_resources)==0):
			setups.append(current_resources);
		return setups;

	def get_valid_setups(self):
		filename = 'test_setups.list';
		list = None;
		try:
			f_fd = open(filename, 'r');
			dump_object = pickle.load(f_fd);
			list = dump_object;
			f_fd.close();
		except:
			self._list_incompatible_with_resource = {};
			for resource in self._list_resources:
				self._list_incompatible_with_resource[resource] = [];
				for incomp in self._list_incompatibles:
					if (resource in incomp):
						for incomp_res in incomp:
							self._list_incompatible_with_resource[resource].append(incomp_res);
			list = self.get_next_setups(  [], self._list_resources.keys() );
			f = open(filename, 'w');
			pickle.dump(list, f);
			f.close();
		return list;

	#a subset is a set of resources among list_resources
	def find_appropriate_setups(self, list_of_subsets):
		print "searching..."
		setups = self.get_valid_setups();
		#sorted_setups = setups.sort(cmp=bylength);

		counter_setups = [];
		for index,setup in enumerate(setups):
			counter_setups.append(0);
			for subset in list_of_subsets:
				for resource in subset:
					if resource not in setup:
						break;
				else:
					counter_setups[index]+=1;
					if (counter_setups[index] > 8):
						print setup

	def are_all_resources_in_setup(self, setup_name, resources_set):
		setup_resources = self._list_typical_setups[setup_name];
		for resource in resources_set:
			if (resource not in setup_resources):
				break;
		else:
			return True;
		return False;


intopix_test_setups();


if (__name__ == '__main__'):
	#setup = intopix_test_setups.get_setup_by_name('server_with_pristine_dec0');
	#if (setup):
	#	print setup.get_description();
	#	print setup.prepare();
	#intopix_test_setups().prepare_resources(['phabrix' , 'pristine_dec_1']);

	import sys
	sys.path.append('./');
	from tests import *;
	from intopix_test import intopix_test;
	list_subsets = [];
	for test in intopix_test._list_tests:
		list_subsets.append(test._setup_resources);

	intopix_test_setups().find_appropriate_setups(list_subsets);
