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
# first argument is path to file to modify
filePath=sys.argv[1]
outFile=filePath
# second argument is string to find
findstring=sys.argv[2]
# third argument is string to replace first find with
replacestring=sys.argv[3]

inF = open(filePath,'rb')
s=unicode(inF.read(),'utf-16')
inF.close()

outtext=s.replace(findstring,replacestring)

outF = open(outFile,'wb')
outF.write(outtext.encode('utf-16'))
outF.close()
