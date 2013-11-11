# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

import re

def doTimeStampCompareAndWrite(aFile, aNewFileContents):
    oldFileSub = ''
    try:
        oldFile = open(aFile, 'r')
        oldFileContents = oldFile.read()
        if len(oldFileContents) > 0:
            oldFileSub = re.sub(r'\n[\W\w]*\(time stamp\)', '', oldFileContents)
    except IOError:
        pass

    newFileSub = re.sub(r'\n[\W\w]*\(time stamp\)', '', aNewFileContents)
    
    #compare sub-ed files
    if oldFileSub == '' or re.search(re.escape(oldFileSub), newFileSub) == None:
        file(aFile, 'w+').write(aNewFileContents)
        return True
    return False