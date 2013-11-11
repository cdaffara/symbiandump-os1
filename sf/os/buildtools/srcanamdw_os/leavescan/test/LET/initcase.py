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

import time
import sys
import os
from os.path import isdir
def getCurTime():
	curTime = time.localtime()
	return str(curTime[0])+"-"+str(curTime[1])+"-"+str(curTime[2])+" "+str(curTime[3])+":"+str(curTime[4])+":"+str(curTime[5])

def getForamtIndex(index):
	if index<0:
		return '000'
	elif index<10:
		return '00'+str(index)
	elif index<100:
		return '0'+str(index)
	else:
		return str(index)
if __name__ == '__main__':
	#init
	cur_time = getCurTime()
	type = 'CT'
	USAGE = 'initcase outputdir name copies author'
	copies = 10
	args = sys.argv[1:]
	args_len = len(args)
	ok  = True
	if args_len >= 3:
		output_dir = args[0]
		name = args[1]
		copies = int(args[2])
		if args_len>3:
			author = args[3]
		else:
			author = 'Bolow'
	else:
		print USAGE

	
	#dir
	if not os.path.exists(output_dir):
		#mkdir
		os.makedirs(output_dir)

	elif not os.path.isdir(output_dir):
		print output_dir, 'is not a dir'
		ok = False
	if ok:
		index = 0
		while copies>0:
			copies=copies-1
			index = index + 1
			outfile_name = output_dir+'\\'+name+'-'+getForamtIndex(index)+'.cpp'
			print outfile_name
			outfile = open(outfile_name,"w")
			outfile.write ( '//desc:\n' )
			outfile.write ( '//option:\n' )
			outfile.write ('//date:'+cur_time+'\n')
			outfile.write ('//author:'+author+'\n')
			outfile.write ('//type: CT\n')
			outfile.close()
			
	

