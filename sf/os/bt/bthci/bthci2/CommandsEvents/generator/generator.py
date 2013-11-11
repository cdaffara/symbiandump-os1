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

import ConfigParser
import getopt
import string
import sys
import os
import os.path

from command import writeCommand
from mmp import writeMMPFile
from event import writeEvent
from completeevent import writeCompleteEvent
from datainput import getEntries, getTypes, getFolded
from testserver import writeTestServer
from teststep import writeTestSteps

def printHelp():
    print('Automatically generates source files for Bluetooth commands')
    print('Usage: python ' + sys.argv[0] + ' -i ini-file -t type [mmp|impl|test] [template] [destination]')
    print('Example: \"python ' + sys.argv[0] + ' -i dllmmp.ini -t mmp dllmmp.tmpl hcilib.mmp\"')
    print('Options:\n-i\t File specifing data associated with this command. It\'s layout is type specific.')
    print('-t\t Type of data to generate, either mmp for mmp file, impl for implementation or test for test code.')

# Retrieves command line options
def getOpts():
    scanned = {}
    opts, args = getopt.getopt(sys.argv[1:], 'i:t:e:')

    for o, a in opts:
        if o == '-i':
            scanned['INI'] = a
        elif o == '-t':
            scanned['COMMAND'] = a.lower()
        else:
            printHelp()

    return scanned, args

opts, args = getOpts()

if len(opts) < 2:
    printHelp()
else:
    if opts['COMMAND'] == 'mmp': # Generate mmp file
        writeMMPFile(opts['INI'], string.Template(file(args[0]).read()), args[1])
    elif opts['COMMAND'] == 'impl': # Generate implementation
        cfg = ConfigParser.ConfigParser()
        cfg.readfp(file(opts['INI']))
                   
        for s in cfg.sections():
            entries = getEntries(cfg.get(s, 'Datafile'))
            types = getTypes(cfg.get(s, 'Typefile'))
            headerTemplate = string.Template(file(cfg.get(s, 'HeaderTemplate')).read())
            sourceTemplate = string.Template(file(cfg.get(s, 'SourceTemplate')).read())

            typ = cfg.get(s, 'Type').lower()

            if os.path.exists(cfg.get(s, 'SourcePath')) == False:
                os.mkdir(cfg.get(s, 'SourcePath'))

            if os.path.exists(cfg.get(s, 'HeaderPath')) == False:
                os.mkdir(cfg.get(s, 'HeaderPath'))

            if typ == 'command':
                folded = getFolded(entries, types, 3)
                print('Writing %i commands\n\theaders at %s\n\tsource at %s...'
                      % (len(folded), cfg.get(s, 'HeaderPath'), cfg.get(s, 'SourcePath')))

                for f in folded:
                    writeCommand(f, folded[f][0], folded[f][1], headerTemplate, sourceTemplate,
                                 cfg.get(s, 'HeaderPath'), cfg.get(s, 'SourcePath'))
            elif typ == 'event':
                folded = getFolded(entries, types, 2)
                print('Writing %i events\n\theaders at %s\n\tsource at %s...'
                      % (len(folded), cfg.get(s, 'HeaderPath'), cfg.get(s, 'SourcePath')))

                for f in folded:
                    writeEvent(f, folded[f][1], headerTemplate, sourceTemplate,
                               cfg.get(s, 'HeaderPath'), cfg.get(s, 'SourcePath'))
            elif typ == 'completeevent':
                folded = getFolded(entries, types, 5)
                print('Writing %i command complete events\n\theaders at %s\n\tsource at %s...'
                      % (len(folded), cfg.get(s, 'HeaderPath'), cfg.get(s, 'SourcePath')))

                for f in folded:
                    writeCompleteEvent(f, folded[f][1], headerTemplate, sourceTemplate,
                                       cfg.get(s, 'HeaderPath'), cfg.get(s, 'SourcePath'))
    elif opts['COMMAND'] == 'test': # Generate test code
        cfg = ConfigParser.ConfigParser()
        cfg.readfp(file(opts['INI']))

        for s in cfg.sections():
            if os.path.exists(cfg.get(s, 'Path')) == False:
                os.mkdir(cfg.get(s, 'Path'))
            
            writeTestServer(cfg.get(s, 'Name'),
                            string.Template(file(cfg.get(s, 'HeaderTemplate')).read()),
                            string.Template(file(cfg.get(s, 'SourceTemplate')).read()),
                            string.Template(file(cfg.get(s, 'ScriptTemplate')).read()),
                            cfg.get(s, 'TestSteps'),
                            cfg.get(s, 'Path'))

            writeTestSteps(cfg.get(s, 'TestSteps'), cfg.get(s, 'Path'))

            
    
