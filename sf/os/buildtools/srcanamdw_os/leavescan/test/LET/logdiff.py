#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

import sys
import os
from os import listdir
from os.path import isdir
import csv

def check_argv(name):
	if not os.path.exists(name):
		print name,' doesn\'t exist.'
		return False
	if not os.path.isdir(name):
		print name,' is not a dir.'
		return False
	file_list = os.listdir(name)
	properDir = False
	for current_file in file_list:
		if current_file.endswith('.csv'):
			properDir =True
			break
	if not properDir:
		print name,' is not a LET log dir'
		return False
	return True

def find_logfile(name):
	file_list = os.listdir(name)
	for current_file in file_list:
		if current_file.endswith('.csv'):
			return name+'\\'+current_file	
	return None


def get_result_map(csvfile):
	map = {}
	file = open(csvfile,"r")
	lines = csv.reader(file)
	skip_file_line = True
	for line in lines:
		if skip_file_line:
			skip_file_line=False
		else:
			case_name = line[0]
			test_result = line[3]
			if map.has_key(case_name):
				print 'Duplicate casename:',case_name,' ignored'
			else:
				map[case_name] = test_result
	return map

def diff_result(map_old, map_new):
	keys_new = map_new.keys()
	for key_new in keys_new:
		item_new = map_new[key_new]
		value_new = item_new
		if map_old.has_key(key_new):
			value_old = map_old[key_new]
			if not value_old == value_new:
				print 'CHANGE:',key_new,value_old,'-->',value_new
		else:
			print 'ADD:',key_new,value_new
	keys_old = map_old.keys()
	for key_old in map_old:
		item_old = map_old[key_old]
		value_old = item_old
		if not map_new.has_key(key_old):
			print 'REMOVE:',key_old,value_old

if __name__ == '__main__':
	argv = sys.argv[1:]
	if len(argv)<2:
		print 'usage: logdiff base_log_dir derived_log_dir'
	else :
		old_file = argv[0]
		new_file = argv[1]
		if check_argv(old_file) and check_argv(new_file):
			new_map = get_result_map(find_logfile(new_file))
			old_map = get_result_map(find_logfile(old_file))
			diff_result(old_map,new_map)


