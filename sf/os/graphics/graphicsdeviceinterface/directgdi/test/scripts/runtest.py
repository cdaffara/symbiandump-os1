# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

"""
Run tests for image comparison

"""
import sys
import os
from testcase import TestCase

# default to winscw
if len(sys.argv) < 2:
	rootDir = os.environ.get('EMULATOR_C_DIR',"\\epoc32\\winscw\\c")+"\\"
elif sys.argv[1].lower() == 'armv5':
	rootDir = os.environ.get('EPOCROOT')+"epoc32\\data\\c\\"
elif sys.argv[1].lower() == 'winscw':
	rootDir = os.environ.get('EMULATOR_C_DIR',"\\epoc32\\winscw\\c")+"\\"
else:
	print 'Select winscw or armv5. (Defaults to winscw if no argument supplied).'
	sys.exit()

# Root directory for test and reference images
# Test images are at imgDir + "test\\"
# Reference images are at imgDir + "ref\\"
imgDir = rootDir+"img\\"

# Directory for log result files
logDir = rootDir+"logs\\"

# Directory for ONB summary file
onbDir = rootDir;

# HTML file name for an error message page of the test results
KErrorMessagePageName = "error.html"

errorLog = file(logDir + KErrorMessagePageName, "wt")

# Testing the images
myTest = TestCase("myTest", imgDir, logDir, onbDir)
myTest.readRefImage(errorLog)
myTest.readTestImage(errorLog)
myTest.computeDifferences(errorLog)

errorLog.close()


