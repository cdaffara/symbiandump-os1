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

from datainput import getEntries, getFolded, getTypes
from teststep import TestStep
from time import strftime
from utils import doTimeStampCompareAndWrite

#
# Creates test step from .ini file as used when creating commands and events.
#

def makeTestSteps(aCfg, aSections):
    if aSections == []:
        return []
    else:
        return makeTestStep(aCfg.get(aSections[0], 'Type'),
                            getFolded(getEntries(aCfg.get(aSections[0], 'Datafile')), getTypes(aCfg.get(aSections[0], 'Typefile')), 0)) + makeTestSteps(aCfg, aSections[1:])

#
# Creates a test step from an entry as provided by the getFolded method.
#

def makeTestStep(aType, aEntries):
    if len(aEntries) == 0:
        return []
    else:
        key, value = aEntries.popitem()
        return [TestStep(key, aType)] + makeTestStep(aType, aEntries)

#
# Makes a string of the form
#
# if(aStepName == KTestStepName1)
#   {
#   return new CTestStepName;
#   }
# else if(aStepName == KTestStepName2)
#   {
#    ....
#
# for every test name in the list provided.
#

def makeCreateTestStepImpl(aTestSteps):
    impl_str = ''

    for t in aTestSteps:
        impl_str += 'if(aStepName == K' + t.teststepName() + ')\n\t\t{\n\t\treturn new C' + t.teststepName() + ';\n\t\t}\n\telse '

    impl_str += '\n\t\t{\n\t\treturn NULL;\n\t\t}'
    return impl_str

#
# Makes a string of the form '#include "teststepheaderfile.h"\n' for every test step in the list provided.
#

def makeTestStepHeaders(aTestSteps):
    hdr_str = ''

    for t in aTestSteps:
        hdr_str += '#include "' + t.headerFile() + '"\n'

    return hdr_str

#
# Returns a string for test step run instructions.
#

def makeTests(aName, aTestSteps):
    scr_str = ''

    for t in aTestSteps:
        if t.isPanic(): # Test step will panic
            scr_str += 'RUN_TEST_STEP !PanicCode=' + t.panicCode() + ' 100 ' + aName + ' ' + t.teststepName() + '\n' 
        else:
            scr_str += 'RUN_TEST_STEP 100 ' + aName + ' ' + t.teststepName() + '\n'

    return scr_str

#
# Writes a test server with test script. Template substitutions are:
#
# For the source template:
# $CREATE_TEST_STEP_IMPL: implementation of CreateTestStep method
# $TEST_STEP_HEADERS: #include "teststep.h", include directive for all test step implementations
# $SERVERNAME: test step server name, used for SetServerName
# $CLASSNAME: test step server class name, of the form CTestStepServerName
# $FILENAME: filename for test step server
# $HEADERGUARD: the name of the server in capital letters, suitable for include guards
# $GENERATE_TIME_STAMP: timestamp showing when the generator produced the file
#
# For the header template:
# $CLASSNAME: test step server class name, of the form CTestStepServerName
# $FILENAME: filename for test step server
# $HEADERGUARD: the name of the server in capital letters, suitable for include guards
# $GENERATE_TIME_STAMP: timestamp showing when the generator produced the file

def writeTestServer(aName, aHeaderTemplate, aSourceTemplate, aScriptTemplate, aTestSteps, aPath):
    cfg = ConfigParser.ConfigParser()
    cfg.readfp(file(aTestSteps))

    steps = makeTestSteps(cfg, cfg.sections())
    
    src = {'CREATE_TEST_STEP_IMPL': makeCreateTestStepImpl(steps),
           'TEST_STEP_HEADERS': makeTestStepHeaders(steps),
           'SERVERNAME': aName,
           'CLASSNAME': 'C' + aName,
           'FILENAME': aName.lower(),
           'HEADERGUARD': aName.upper(),
           'GENERATE_TIME_STAMP': strftime("%a, %d %b %Y %H:%M:%S") + ' (time stamp)'}
    
    hdr = {'CLASSNAME': 'C' + aName,
           'FILENAME': aName.lower(),
           'HEADERGUARD': aName.upper(),
           'GENERATE_TIME_STAMP': strftime("%a, %d %b %Y %H:%M:%S") + ' (time stamp)'}

    #file(aPath + '\\' + hdr['FILENAME'] + '.h', 'w+').write(aHeaderTemplate.substitute(hdr))
    #file(aPath + '\\' + src['FILENAME'] + '.cpp', 'w+').write(aSourceTemplate.substitute(src))
    doTimeStampCompareAndWrite(aPath + '\\' + hdr['FILENAME'] + '.h', aHeaderTemplate.substitute(hdr))
    doTimeStampCompareAndWrite(aPath + '\\' + src['FILENAME'] + '.cpp', aSourceTemplate.substitute(src))

    writeTestScripts(aName, aScriptTemplate, steps, aPath)

#
# Writes test scripts. Template substitutions:
# $FILENAME: name of script file to write
# $TESTSERVERNAME: name of test server to load
# $TESTS: calls to test steps, of the form RUN...
# $GENERATE_TIME_STAMP: timestamp showing when the generator produced the file
#

def writeTestScripts(aName, aScriptTemplate, aTestSteps, aPath):
    script = {'FILENAME': aName.lower(),
              'TESTSERVERNAME': aName,
              'TESTS': makeTests(aName, aTestSteps),
              'GENERATE_TIME_STAMP': strftime("%a, %d %b %Y %H:%M:%S") + ' (time stamp)'}
    
    #file(aPath + '\\' + script['FILENAME'] + '.script', 'w+').write(aScriptTemplate.substitute(script))
    doTimeStampCompareAndWrite(aPath + '\\' + script['FILENAME'] + '.script', aScriptTemplate.substitute(script))