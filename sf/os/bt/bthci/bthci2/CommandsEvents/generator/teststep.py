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
import string
import re

from datainput import getFolded, getEntries, getTypes
from time import strftime
from utils import doTimeStampCompareAndWrite

#
# Class wraps up external information needed for test steps
# such as script file data, ini file data etc.
#

class TestStep:
    def __init__(self, aName, aType):
        self.iName = aName
        self.iType = aType

    def headerFile(self):
        if self.iType == 'command':
            return self.iName.lower() + 'commandstep.h'
        elif self.iType == 'event':
            return self.iName.lower() + 'eventstep.h'
        elif self.iType == 'completeevent':
            return self.iName.lower() + 'completeeventstep.h'

    def teststepName(self):
        if self.iType == 'command':
            return self.iName + 'CommandStep'
        elif self.iType == 'event':                                  
            return self.iName + 'EventStep'
        elif self.iType == 'completeevent':
            return self.iName + 'CompleteEventStep'

    def isPanic(self):
        if self.iType == 'command':
            return False
        elif self.iType == 'event' or self.iType == 'completeevent':
            return True

    def panicCode(self):
        return '0'

    def panicMessage(self):
        return 'Event cast error'


#returns a maximum possible value of the parameter to be used as an initialiser
#this function is needed because parameters representing array dimension cannot be 0xff

def getParamValueMaxMaker( aParam, aParams):
   for p in aParams:
      if aParam == p.iArray:
           return '0x01'
           break
   for p in aParams:
      if aParam == p.iName:
           return p.iType.toMaxMaker()   
           break                 

           
#returns a string containing maximum possible value of the parameter
#this function is needed because parameters representing array dimension cannot be 0xff

def getParamValueMaxStr( aParam, aParams):
   for p in aParams:
      if aParam == p.iArray:
           return '\\x01'                                                                        
   for p in aParams:
      if aParam == p.iName:
           return p.iType.maxValueStr()   
           
           
#
# Returns a string with the maximum values of parameters that can be used
# when creating commands. Maximum is largest possible value of the data and not the delimiter noted in
# in the bluetooth spec. The maximum value is retrieved from the type file.
#

def makeMaxParameters(aParams):
    max_str = ''

    for p in aParams:
        if p.iArray != '':
            max_str += p.iName + '_max, '
        else:
            max_str += getParamValueMaxMaker( p.iName, aParams) + ', '

    return max_str[:-2]


#
# Returns a string with the minimum values of parameters that can be used
# when creating commands. Minimum value is retrieved from the type file.
#

def makeMinParameters(aParams):
    min_str = ''

    for p in aParams:                
        if p.iArray != '':
            min_str += p.iName + '_min, '
        else:
            min_str += p.iType.toMinMaker() + ', '        

    return min_str[:-2]


#
# Returns a string testing command 'getter' values against the maximum value.
# For arrays this will test each and every value in the array.
#

def makeCommandMaxAccessors(aParams, aPtr):
    acc_str = ''

    for p in aParams:
        if p.iArray != '':                        
            acc_str += '\tfor(TInt i = 0; i < cmd->' + p.iArray + '(); ++i)\n\t\t{\n'
            acc_str += '\t\tif(' + aPtr + '->' + p.iName + '(i) != ' + p.iType.toMaxMaker() + ')\n\t\t\t{\n\t\t\treturn TestStepResult();\n\t\t\t}\n'
            acc_str += '\t\t}\n\n'
        else:
            acc_str += '\tif(' + aPtr + '->' + p.iName + '() != ' + getParamValueMaxMaker( p.iName, aParams) + ')\n\t\t{\n\t\treturn TestStepResult();\n\t\t}\n\n'

    return acc_str


#
# Returns a string testing event 'getter' values against the maximum value for events.
# For arrays this will test each and every value in the array.
# Event are specific because "Status"  value is checked by ErrorCode method, so we need to skip getter for Status
#

def makeEventMaxAccessors(aParams, aPtr):
    acc_str = ''

    for p in aParams:
        if p.iName != 'Status':
            if p.iArray != '':
                acc_str += '\tfor(TInt i = 0; i < event->' + p.iArray + '(); ++i)\n\t\t{\n'
                acc_str += '\t\tif(' + aPtr + '->' + p.iName + '(i) != ' + p.iType.toMaxMaker() + ')\n\t\t\t{\n\t\t\treturn TestStepResult();\n\t\t\t}\n'
                acc_str += '\t\t}\n\n'
            else:
                acc_str += '\tif(' + aPtr + '->' + p.iName + '() != ' + getParamValueMaxMaker( p.iName, aParams) + ')\n\t\t{\n\t\treturn TestStepResult();\n\t\t}\n\n'

    return acc_str


#
# Returns a string testing command 'getter' values against the minimum value.
# For arrays this will test each and every value in the array.
#

def makeCommandMinAccessors(aParams, aPtr):
    acc_str = ''

    for p in aParams:
        if p.iArray != '':
            acc_str += '\tfor(TInt i = 0; i < cmd->' + p.iArray + '(); ++i)\n\t\t{\n'
            acc_str += '\t\tif(' + aPtr + '->' + p.iName + '(i) != ' + p.iType.toMinMaker() + ')\n\t\t\t{\n\t\t\treturn TestStepResult();\n\t\t\t}\n'
            acc_str += '\t\t}\n\n'
        else:
            acc_str += '\tif(' + aPtr + '->' + p.iName + '() != ' + p.iType.toMinMaker() + ')\n\t\t{\n\t\treturn TestStepResult();\n\t\t}\n\n'

    return acc_str

    
#
# Returns a string testing event 'getter' values against the minimum value for events.
# For arrays this will test each and every value in the array.
# Event are specific because "Status"  value is checked by ErrorCode method, so we need to skip getter for Status
#

def makeEventMinAccessors(aParams, aPtr):
    acc_str = ''

    for p in aParams:
        if p.iName != 'Status':
            if p.iArray != '':
                acc_str += '\tfor(TInt i = 0; i < event->' + p.iArray + '(); ++i)\n\t\t{\n'
                acc_str += '\t\tif(' + aPtr + '->' + p.iName + '(i) != ' + p.iType.toMinMaker() + ')\n\t\t\t{\n\t\t\treturn TestStepResult();\n\t\t\t}'
                acc_str += '\t\t}\n\n'
            else:
                acc_str += '\tif(' + aPtr + '->' + p.iName + '() != ' + p.iType.toMinMaker() + ')\n\t\t{\n\t\treturn TestStepResult();\n\t\t}\n\n'

    return acc_str    


#
# Returns a C-string, "...", useful for creating event data with maximum values of every parameter.
#

def makeMaxData(aParams):
    frame_str = ''

    for p in aParams:
        if not p.iNotInFrame:
            if p.iArray != '':
                num = int(getParamValueMaxMaker( p.iArray,aParams), 16)
                while num > 0:
                    num -= 1
                    frame_str += p.iType.maxValueStr()
            else:
                frame_str += getParamValueMaxStr(p.iName, aParams)

    return frame_str


#
# Returns a C-string, "...", useful for creating event data with minimum values of every parameter.
#

def makeMinData(aParams):
    frame_str = ''

    for p in aParams:
        if not p.iNotInFrame:
            if p.iArray != '':
                num = int(filter(lambda (e): e.iName == p.iArray, aParams)[0].iType.minValue(), 16)
                while num < 0:
                    num -= num
                    frame_str +=  p.iType.minValueStr()
            else:
                frame_str +=  p.iType.minValueStr()

    return frame_str


#
# Returns a string for creating a single argument string to be used in a constructr for commands.
#                                               

def makeCommandInitMinParams( aParams):
    min_str = ''

    for p in aParams:                
        if p.iType.CStringAvoider() != '':
            min_str += '\t_LIT8(' + p.iType.CStringAvoider() + ',"' + p.iType.minValueStr() + '");\n'
        if p.iType.PckgBufType() != '':
            min_str += '\t' + p.iType.PckgBufType() + ' ' + p.iType.PckgBufType() + '_instance_min = ' + p.iType.minValue() + ';\n'
        if p.iArray != '':
            min_str += '\tRArray<' + p.iType.iTypename + '> ' + p.iName + '_min;\n';
    return min_str


#
# Returns a string for creating a single argument string to be used in a constructr for commands.
#                                               

def makeCommandInitMaxParams(aParams):
    max_str = ''
    for p in aParams:
           if p.iType.PckgBufType() != '':
               max_str += '\t' + p.iType.PckgBufType() + ' ' + p.iType.PckgBufType() + '_instance_max = ' + p.iType.maxValue() + ';\n'
           if p.iArray != '':
               max_str += '\tRArray<' + p.iType.iTypename + '> ' + p.iName + '_max;\n';
               max_str += '\tfor(TInt i = 0; i < ' + getParamValueMaxMaker( p.iArray, aParams) + '; ++i)\n\t\t{\n'
               max_str += '\t\t' + p.iName + '_max.Append(' + p.iType.toMaxMaker() + ');\n'
               max_str += '\t\t}\n\n'    
    return max_str
                  

#                                                                                    
# Returns a string for creating a single argument string to be used in a constructr for events .
#

def makeEventInitMinParams(aEventCode, aParams):

    par_str = '\t_LIT8(formatter,"'
    #1- event code,1- length of the event data;   
    par_str +=  '%c\\x0'
    par_str +=  makeMinData(aParams) + '");\n'          
    par_str +=  '\tTBuf8<255> init_value;'
    par_str +=  '\n\tinit_value.Format(formatter,' 
    par_str +=  aEventCode + ' & 0xff);\n '
    for p in aParams:
        if p.iType.CStringAvoider() != '':
            par_str += '\t_LIT8(' + p.iType.CStringAvoider() + ',"' + p.iType.minValueStr() + '");\n'
        if p.iType.PckgBufType() != '':
            par_str += p.iType.PckgBufType() + ' ' + p.iType.PckgBufType() + '_instance_min = ' + p.iType.minValue() + ';'
    return par_str


#                                                                                    
# Returns a string for creating a single argument string to be used in a constructr for completeevents.
#

def makeCompleteEventInitMinParams(aEventCode, aOpcode, aParams):

    par_str = '\t_LIT8(formatter,"'
    #1- length of the event data, 1- command credits, 2- opcode;   
    par_str +=  '%c\\x0\\x0%c%c'
    par_str +=  makeMinData(aParams) + '");\n'
    par_str +=  '\tTBuf8<255> init_value;\n'
    par_str +=  '\tinit_value.Format(formatter,' 
    par_str +=  aEventCode + ' & 0xff, ' 
    par_str +=  aOpcode + ' & 0xff, ' 
    par_str +=  aOpcode + '>>8 & 0xff);\n '
    for p in aParams:               
        if p.iType.CStringAvoider() != '':
            par_str += '\t_LIT8(' + p.iType.CStringAvoider() + ',"' + p.iType.minValueStr() + '");\n'
        if p.iType.PckgBufType() != '':
           par_str += p.iType.PckgBufType() + ' ' + p.iType.PckgBufType() + '_instance_max = ' + p.iType.maxValue() + ';\n'
    return par_str

def makeMatchCompleteEevnt(aOpcode):
    match_str = '\t_LIT8(complete_formatter,"'
    #1- event code, #1- length of the event data, 1- command credits, 2- opcode;   
    match_str +=  '%c\\x0\\x0%c%c");\n'
    match_str +=  '\tTBuf8<255> event_data;\n'
    match_str +=  '\tevent_data.Format(complete_formatter,' 
    match_str +=  'ECommandCompleteEvent & 0xff, ' 
    match_str +=  aOpcode + ' & 0xff, ' 
    match_str +=  aOpcode + '>>8 & 0xff);\n'
    match_str += '\tTVendorDebugCompleteEvent* complete_event = new(ELeave) TVendorDebugCompleteEvent(event_data);\n'
    match_str += '\tTBool event_matches=EFalse; TBool event_concludes=EFalse; TBool event_continue_matching=EFalse;\n'
    match_str += '\tcmd->Match (*complete_event, event_matches, event_concludes, event_continue_matching);\n'
    match_str += '\tif ((!event_matches)||(!event_concludes))\n' # We don't consider continuations as "in general" a complete event
                                                                 # may or may not continue (e.g. Inquiry_Cancel_Complete_Event continues)
    match_str += '\t\t{\n\t\treturn TestStepResult();\n\t\t}\n'
    return match_str
    
def makeMatchStatusEevnt(aOpcode):
    match_str = '\t_LIT8(status_formatter,"'
    #1- event code, #1- length of the event data, 1- command credits, 2- opcode;   
    match_str +=  '%c\\x0\\x0\\x0%c%c");\n'
    match_str +=  '\tevent_data.Format(status_formatter,' 
    match_str +=  'ECommandStatusEvent & 0xff, '
    match_str +=  aOpcode + ' & 0xff, ' 
    match_str +=  aOpcode + '>>8 & 0xff);\n'
    match_str += '\tTVendorDebugEvent* status_event = new(ELeave) TVendorDebugEvent(event_data);\n'
    match_str += '\tevent_matches=EFalse; event_concludes=EFalse; event_continue_matching=EFalse;\n'
    match_str += '\tcmd->Match (*status_event, event_matches, event_concludes, event_continue_matching);\n' 
    match_str += '\tif ((!event_matches)||(event_concludes)||(event_continue_matching))\n'
    match_str += '\t\t{\n\t\treturn TestStepResult();\n\t\t}\n'   
    return match_str

def makeMatchProprietaryEevnt(aOpcode, aEvent, ind):    
    match_str = '\t_LIT8(proprietary_formatter'+ ind + ',"'
    if aEvent == 'EModeChangeEvent':
        match_str +=  '%c\\x0\\x0\\x0\\x0\\x0\\x0\\x0");\n'
        match_str +=  '\tevent_data.Format(proprietary_formatter'+ ind + ',' 
        match_str +=  aEvent + ' & 0xff);\n'
    else:
        if aEvent == 'EConnectionCompleteEvent' or aEvent == 'ESynchronousConnectionCompleteEvent':
            #for ConnectionCompleteEvent we imitate BT Device Address as it will be needed to match the event
            match_str +=  '%c\\x0\\x0%c%c\\x0\\x0\\x0\\x0\\x0\\x0\\x0\\x0");\n'
        else:
        #1- event code, 1- length of the event data, 1- command credits, 2- opcode;   	
            match_str +=  '%c\\x0\\x0%c%c");\n'
        match_str +=  '\tevent_data.Format(proprietary_formatter'+ ind + ',' 
        match_str +=  aEvent + ' & 0xff, '
        match_str +=  aOpcode + ' & 0xff, '
        match_str +=  aOpcode + '>>8 & 0xff);\n'    	
    match_str += '\tTVendorDebugEvent* proprietary_event'+ ind + '= new(ELeave) TVendorDebugEvent(event_data);\n'
    match_str += '\tevent_matches=EFalse; event_concludes=EFalse; event_continue_matching=EFalse;\n'
    match_str += '\tcmd->Match (*proprietary_event'+ ind + ', event_matches, event_concludes, event_continue_matching);\n'
    match_str += '\tif (!event_matches)\n'
    match_str += '\t\t{\n\t\treturn TestStepResult();\n\t\t}\n'   
 
    return match_str
             
def makeMatchProprietaryEevnts(aOpcode, aParams):
    m =0 
    ind = 1
    match_str = ''
    while ('Match' in aParams[m:]):
        m = m + aParams[m:].index('Match') + 1
        match_str += makeMatchProprietaryEevnt(aOpcode, 'E' + aParams[m], str(ind)) + '\n'       
        ind += 1 
    return match_str         
    
def makeMatchEmptyEevnt(aOpcode): 
    match_str = '\t_LIT8(empty_formatter,"'
    #1- event code, #1- length of the event data, 1- command credits, 2- opcode;   
    match_str +=  '\\x0\\x0\\x0\\x0\\x0");\n'
    match_str += '\tTVendorDebugEvent* empty_event = new(ELeave) TVendorDebugEvent(empty_formatter);\n'
    match_str += '\tevent_matches=EFalse; event_concludes=EFalse; event_continue_matching=EFalse;\n'
    match_str += '\tcmd->Match (*empty_event, event_matches, event_concludes, event_continue_matching);\n' 
    match_str += '\tif ((event_matches)||(event_concludes)||(event_continue_matching))\n'
    match_str += '\t\t{\n\t\treturn TestStepResult();\n\t\t}\n'

    return match_str   
    
    
#                                                                                    
# Returns a string to be passed as a single argument to the constructr for events.
#

def makeEventParams(aParams):
     return 'init_value'      


#
# Writes command test step. Template substitutions are:
#
# $FILENAME: name of file, of the form nameofcommandcommandstep
# $CLASSNAME: name of test step class, of the form CNameOfCommandCommandStep
# $HEADERGUARD: for header guard defines, of the form NAMEOFCOMMANDCOMMANDSTEP 
# $CMDNAME: name of command to test, of the form CNameOfCommandCommand
# $CMDFILENAME: name of files containing command to test, of the for nameofcommandcommand
# $MAXPARAMETERS: command maximum construction parameters
# $CMDOPCODE: opcode of the command to test, of the form KNameOfCommandOpcode
# $CHECK_MAX_ACCESSORS: code checking that the command parameters have been set to their maximum values
# $FRAMEDATA: a descriptor containing max parameters values, used to very that CHctlCommandFrame is written correctly
# $MINPARAMETERS: command minimum construction parameters
# $CHECK_MAX_ACCESSORS: code checking that the command parameters have been set to their minimum values
# $TESTSTEPNAME: name for this test step                      
# $CMDINITMAXPARAMS: initialistation of params for the constructor of command with max values of all fields
# $CMDINITMIXPARAMS: initialistation of params for the constructor of command with min values of all fields
# $GENERATE_TIME_STAMP: timestamp showing when the generator produced the file
#
            
def writeCommandTestStep(aCmd, aMatchData, aParams, aHeaderTemplate, aSourceTemplate, aPath):
   matchParams = re.split(r'[^a-zA-Z_0-9\*]+', aMatchData);
   
   cmd = {'FILENAME': aCmd.lower() + 'commandstep',
           'CLASSNAME': 'C' + aCmd + 'CommandStep',
           'HEADERGUARD': aCmd.upper() + 'COMMANDSTEP',
           'CMDNAME': 'C' + aCmd + 'Command',         
           'CMDFILENAME': aCmd.lower() + 'command',
           'MAXPARAMETERS': makeMaxParameters(aParams),
           'CMDOPCODE': 'K' + aCmd + 'Opcode',
           'CHECK_MAX_ACCESSORS': makeCommandMaxAccessors(aParams, 'cmd'),
           'FRAMEDATA': makeMaxData(aParams),
           'MINPARAMETERS': makeMinParameters(aParams),
           'CHECK_MIN_ACCESSORS': makeCommandMinAccessors(aParams, 'cmd'),
           'TESTSTEPNAME': aCmd + 'CommandStep',
           'CMDINITMAXPARAMS': makeCommandInitMaxParams(aParams),   
           'CMDINITMINPARAMS': makeCommandInitMinParams(aParams),
           'MATCH_COMPLETE_EVENT': makeMatchCompleteEevnt('K' + aCmd + 'Opcode'),
           'MATCH_STATUS_EVENT': makeMatchStatusEevnt('K' + aCmd + 'Opcode'),
           'MATCH_PROPRIETARY_EVENT': makeMatchProprietaryEevnts('K' + aCmd + 'Opcode',matchParams),
           'MATCH_EMPTY_EVENT': makeMatchEmptyEevnt('K' + aCmd + 'Opcode'),
           'GENERATE_TIME_STAMP': strftime("%a, %d %b %Y %H:%M:%S") + ' (time stamp)'}
            
   #file(aPath + '\\' + cmd['FILENAME'] + '.h', 'w+').write(aHeaderTemplate.substitute(cmd))
   #file(aPath + '\\' + cmd['FILENAME'] + '.cpp', 'w+').write(aSourceTemplate.substitute(cmd))
   doTimeStampCompareAndWrite(aPath + '\\' + cmd['FILENAME'] + '.h', aHeaderTemplate.substitute(cmd))
   doTimeStampCompareAndWrite(aPath + '\\' + cmd['FILENAME'] + '.cpp', aSourceTemplate.substitute(cmd))


#
# Write event test step. Template substitutions are:
#
# $FILENAME: name of file, of the form nameofeventeventstep
# $CLASSNAME: name of test step class, of the the form CNameOfEventEventStep
# $HEADERGUARD: for header guard defines, of the form NAMEOFEVENTEVENTSTEP
# $EVENTNAME: name of event to test, of the form TNameOfEventEvent
# $EVENTFILENAME: name of files containing command to test, of the form nameofeventevent
# $EVENTCODE: event code of event to test, of the form ENameOfEventEvent
# $EVENTPARAMS: parameters used to construct the event, is a descriptor
# $CHECK_EVENT_ACCESSORS: code checking that the event parameters have been set properly
# $TESTSTEPNAME: name for this test step                      
# $EVENTINITPARAMS: initialistation of params for the constructor of event with min values of all fields
# $GENERATE_TIME_STAMP: timestamp showing when the generator produced the file
#

def writeEventTestStep(aEvent, aParams, aHeaderTemplate, aSourceTemplate, aPath):
    eve = {'FILENAME': aEvent.lower() + 'eventstep',
           'CLASSNAME': 'C' + aEvent + 'EventStep',
           'HEADERGUARD': aEvent.upper() + 'EVENTSTEP',
           'EVENTNAME': 'T' + aEvent + 'Event',
           'EVENTFILENAME': aEvent.lower() + 'event',
           'EVENTCODE': 'E' + aEvent + 'Event',      
           'EVENTPARAMS': makeEventParams(aParams),
           'CHECK_EVENT_ACCESSORS': makeEventMinAccessors(aParams, 'event'),
           'TESTSTEPNAME': aEvent + 'EventStep',
           'EVENTINITPARAMS': makeEventInitMinParams('E' + aEvent + 'Event', aParams),
           'GENERATE_TIME_STAMP': strftime("%a, %d %b %Y %H:%M:%S") + ' (time stamp)'}

    #file(aPath + '\\' + eve['FILENAME'] + '.h', 'w+').write(aHeaderTemplate.substitute(eve))
    #file(aPath + '\\' + eve['FILENAME'] + '.cpp', 'w+').write(aSourceTemplate.substitute(eve))
    doTimeStampCompareAndWrite(aPath + '\\' + eve['FILENAME'] + '.h', aHeaderTemplate.substitute(eve))
    doTimeStampCompareAndWrite(aPath + '\\' + eve['FILENAME'] + '.cpp', aSourceTemplate.substitute(eve))


#
# Write complete event test step. Template substitutions are:
#
# $FILENAME: name of file, of the form nameofcompleteeventcompleteeventstep
# $CLASSNAME: name of test step class, of the the form CNameOfCompleteEventCompleteEventStep
# $HEADERGUARD: for header guard defines, of the form NAMEOFCOMPLETEEVENTCOMPLETEEVENTSTEP
# $EVENTNAME: name of event to test, of the form TNameOfCompleteEventCompleteEvent
# $EVENTFILENAME: name of files containing command to test, of the form nameofcompleteeventcompleteevent
# $EVENTCODE: event code of event to test, always ECommandCompleteEvent
# $EVENTPARAMS: parameters used to construct the event, is a descriptor
# $CHECK_EVENT_ACCESSORS: code checking that the event parameters have been set properly
# $TESTSTEPNAME: name for this test step                      
# $EVENTINITPARAMS: initialistation of params for the constructor of completeevent with min values of all fields
# $GENERATE_TIME_STAMP: timestamp showing when the generator produced the file
#

def writeCompleteEventTestStep(aEvent, aParams, aHeaderTemplate, aSourceTemplate, aPath):
    eve = {'FILENAME': aEvent.lower() + 'completeeventstep',
           'CLASSNAME': 'C' + aEvent + 'CompleteEventStep',
           'HEADERGUARD': aEvent.upper() + 'COMPLETEEVENTSTEP',
           'EVENTNAME': 'T' + aEvent + 'CompleteEvent',
           'EVENTFILENAME': aEvent.lower() + 'completeevent',
           'EVENTCODE': 'ECommandCompleteEvent',
           'EVENTPARAMS': makeEventParams(aParams),
           'CHECK_EVENT_ACCESSORS': makeEventMinAccessors(aParams, 'event'),
           'TESTSTEPNAME': aEvent + 'CompleteEventStep',
           'EVENTINITPARAMS': makeCompleteEventInitMinParams('ECommandCompleteEvent', 'K' + aEvent + 'Opcode', aParams),
           'GENERATE_TIME_STAMP': strftime("%a, %d %b %Y %H:%M:%S") + ' (time stamp)'}

    #file(aPath + '\\' + eve['FILENAME'] + '.h', 'w+').write(aHeaderTemplate.substitute(eve))
    #file(aPath + '\\' + eve['FILENAME'] + '.cpp', 'w+').write(aSourceTemplate.substitute(eve))
    doTimeStampCompareAndWrite(aPath + '\\' + eve['FILENAME'] + '.h', aHeaderTemplate.substitute(eve))
    doTimeStampCompareAndWrite(aPath + '\\' + eve['FILENAME'] + '.cpp', aSourceTemplate.substitute(eve))

#
# Writes aEntries as CommandTestSteps
#

def writeCommandTestSteps(aEntries, aHeaderTemplate, aSourceTemplate, aPath):
    for e in aEntries:
        writeCommandTestStep(e, aEntries[e][0], aEntries[e][1], aHeaderTemplate, aSourceTemplate, aPath)


#
# Writes aEntries as EventTestSteps
#

def writeEventTestSteps(aEntries, aHeaderTemplate, aSourceTemplate, aPath):
    for e in aEntries:
        writeEventTestStep(e, aEntries[e][1], aHeaderTemplate, aSourceTemplate, aPath)

#
# Writes aEntries as CompleteEventTestSteps
#

def writeCompleteEventTestSteps(aEntries, aHeaderTemplate, aSourceTemplate, aPath):
    for e in aEntries:
        writeCompleteEventTestStep(e, aEntries[e][1], aHeaderTemplate, aSourceTemplate, aPath)

#
# Writes test steps to files. Uses .ini file aIni to get types, datafile and templates.
#
        
def writeTestSteps(aIni, aPath):
    cfg = ConfigParser.ConfigParser()
    cfg.readfp(file(aIni))

    for s in cfg.sections():
        typ = cfg.get(s, 'Type').lower()

        if typ == 'command':
            folded = getFolded(getEntries(cfg.get(s, 'Datafile')), getTypes(cfg.get(s, 'Typefile')), 3)
            writeCommandTestSteps(folded,
                                  string.Template(file(cfg.get(s, 'TestStepHeaderTemplate')).read()),
                                  string.Template(file(cfg.get(s, 'TestStepSourceTemplate')).read()),
                                  aPath)
        elif typ == 'event':
            writeEventTestSteps(getFolded(getEntries(cfg.get(s, 'Datafile')), getTypes(cfg.get(s, 'Typefile')), 0),
                                string.Template(file(cfg.get(s, 'TestStepHeaderTemplate')).read()),
                                string.Template(file(cfg.get(s, 'TestStepSourceTemplate')).read()),
                                aPath)
        elif typ == 'completeevent':
            writeCompleteEventTestSteps(getFolded(getEntries(cfg.get(s, 'Datafile')), getTypes(cfg.get(s, 'Typefile')), 0),
                                        string.Template(file(cfg.get(s, 'TestStepHeaderTemplate')).read()),
                                        string.Template(file(cfg.get(s, 'TestStepSourceTemplate')).read()),
                                        aPath)

