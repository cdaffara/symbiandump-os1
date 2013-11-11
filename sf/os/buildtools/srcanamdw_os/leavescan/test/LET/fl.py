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
#!/usr/bin/python
import string
import sys
import os
from os import listdir
from os.path import isdir


dir_path=['.']

os_type = 'unix'
if sys.platform.lower().startswith('win'):
    os_type = 'windows'

TRUE = 1
FALSE = 0

def correct_slashes(value):
    """
    This module corrects slashes in pathnames supplied to it.
    """

    while value.find('\\\\') != -1:
        value = value.replace('\\\\', '\\')
        continue

    while value.find('//') != -1:
        value = value.replace('//', '/')
        continue
    if os_type == 'windows' and '/' in value:
        while value.find('/') != -1:
            value = value.replace('/', '\\')
            continue
    return value

def filter(file_name):
	if file_name.endswith('.cpp'):
		return TRUE
	else:
		return FALSE

def print_files(dir_path):
	if not os.path.exists(dir_path):
		return
	file_list = os.listdir(dir_path)
	for cur_file in file_list:
		if cur_file.endswith('\\') or cur_file.endswith('/'):
			cur_file = dir_path+cur_file
		else:
			cur_file = dir_path+'\\'+cur_file	
		if not os.path.isdir(cur_file):
			if os.path.exists(cur_file) and filter(cur_file)== TRUE:
				print correct_slashes(cur_file)
		else:
			print_files(cur_file)

if __name__ == '__main__':
	argv = sys.argv[1:]
	if len(argv) >0:
		dir_path = argv
	for cur_path in dir_path:
		print_files(os.path.abspath(cur_path))
