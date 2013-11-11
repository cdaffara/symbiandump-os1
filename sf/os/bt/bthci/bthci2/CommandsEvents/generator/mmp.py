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
# Write a mmp file
# Template substitution values are partially deduced from 'datafiles'.
# Substitutions are:
# $FILENAME: the name of the file generated.
# $SOURCES: source files to add.
# $GENERATE_TIME_STAMP: timestamp showing when the generator produced the file
# 
#

import ConfigParser
from datainput import getLines
from time import strftime
from utils import doTimeStampCompareAndWrite

# Reads .ini file looking for two things, either a 'Datafile' for multiple source file or a 'Source' entry for a single source file.
# SOURCEPATH and SOURCE specifiers to the mmp file. Entries can have a 'Postfix' which will be appended to source file names.
def makeSources(aIni):
    cfg = ConfigParser.ConfigParser()
    cfg.readfp(file(aIni))
    src = ''
    
    for s in cfg.sections():
        print('Processing SOURCEPATH for %s...' % s)
        src += 'SOURCEPATH\t\t' + cfg.get(s, 'SourcePath') + '\n'

        print('Processing SOURCE for %s...' % s)
        
        if cfg.has_option(s, 'Datafile'):
            entries = getLines(filter(lambda(e): e[0] != '#', file(cfg.get(s, 'Datafile')).readlines()))
            for e in entries:
			#To remove checksource build warnings the following check is made to ensure mmp file is generated with the same case filenames as in Perforce. 
				if e[0] == "WriteLocalName" and cfg.get(s, 'Postfix') == "command":
					src += 'SOURCE\t\t\t' + e[0] + 'Command' + '.cpp\n'
				elif e[0] == "ReadLocalName":
					if cfg.get(s, 'Postfix') == "command":
						src += 'SOURCE\t\t\t' + e[0] + 'Command' + '.cpp\n'
					elif cfg.get(s, 'Postfix') == "completeevent":
						src += 'SOURCE\t\t\t' + e[0] + 'CompleteEvent' + '.cpp\n'
				else:
					src += 'SOURCE\t\t\t' + e[0].lower() + (cfg.get(s, 'Postfix')).lower() + '.cpp\n'
                
        elif cfg.has_option(s, 'Source'):
            src += 'SOURCE\t\t\t' + cfg.get(s, 'Source') + '\n'
                
        src += '\n'
                
        print('Done, %s source files.\n' % len(entries))
        
    return src

def writeMMPFile(aIni, aTemplate, aDst):
    t = {'FILENAME': aDst.lower(),
         'SOURCES': makeSources(aIni),
         'GENERATE_TIME_STAMP': strftime("%a, %d %b %Y %H:%M:%S") + ' (time stamp)'}

    #file(aDst.lower(), 'w+').write(aTemplate.substitute(t))
    doTimeStampCompareAndWrite(aDst.lower(), aTemplate.substitute(t))
    
    
