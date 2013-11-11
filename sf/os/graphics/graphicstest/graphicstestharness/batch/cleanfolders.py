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
# Script to filter out components that are not contributed to the foundation for any reason.
#

import re
import glob
import sys
import os
import shutil
import stat

policyexpression = re.compile(r"^\s*3\s*$")

def cleanfolder(folder):
	os.chdir(folder)
	here = os.getcwd()
	if os.path.exists('distribution.policy.s60'):
		f = open('distribution.policy.s60', 'r')
		policy = f.read()
		f.close()
		if not policyexpression.match(policy):
			print 'Deleting ' + os.getcwd() + ' due to policy ' + policy
			for root, dirs, files in os.walk(os.getcwd()):
				for name in files:
					os.chmod(os.path.join(root,name), stat.S_IWRITE)
			os.chdir('\\')
			shutil.rmtree(here)
			return
	for file in glob.glob('*'):
		os.chdir(here)
		if os.path.isdir(file):
			cleanfolder(file)

print 'Removing subfolder trees from ' + sys.argv[1] + ' with a distribution.policy.s60 entry other than 3'

cleanfolder(sys.argv[1])