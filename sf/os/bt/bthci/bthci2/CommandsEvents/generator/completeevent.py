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
# Writes a 'complete event', i.e. command completion event.
# Template substitution values are deduced from completion event name and completion event parameters.
# Template substitutions are:
# $CLASSNAME: TNameOfCommandCompleteEvent. Name of complete event class.
# $FILENAME: nameofcommandcompleteevent. Lower case version for use in filenames.
# $HEADERGUARD: NAMEOFCOMMANDCOMPLETEEVENT. Upper case version for use in header guard defines.
# $OPCODE: KNameOfCommandOpcode. Opcode of command that has been completed. This value MUST exist in hcibase.h.
# $PARAMETERS: Always: THCIErrorCode aErrorCode, const TDesC8& aEventData.
# $FAKERPARAMETERS: fake event parameters TDesC8& aEventData
# $FAKERWRITE_IMPLEMENTATION: the writing of the fake event parameters into the event data
# $CONSTRUCTOR_PARAMETERS: Always: aErrorCode, aEventData.
# $ACCESSOR_DEFINITION: const ParameterType GetParameterName() const;
# - for every parameter
# $ACCESSOR_IMPLEMENTATION: const ParameterType TNameOfCommandCompleteEvent::GetParameterName() const { retunr iParameterName; }
# - for every parameter
# $GENERATE_TIME_STAMP: timestamp showing when the generator produced the file
# 
#

from event import makeAccessorDef, makeAccessorImpl, makeFakingParameters, makeEventSetErrorCode, makeFakingWriteImplementation, makeParameterTotalLengthValue
from time import strftime
from utils import doTimeStampCompareAndWrite

def completeEventPreHeaderBytes():
    return 5;

def writeCompleteEvent(aCmd, aParams, aHeaderTemplate, aSourceTemplate, aHeaderPath, aSourcePath):
    c = {'COMMAND' : aCmd,
         'CLASSNAME' : 'T' + aCmd + 'CompleteEvent',
         'FILENAME' : aCmd.lower() + 'completeevent',
         'HEADERGUARD' : aCmd.upper() + 'COMPLETEEVENT',
         'OPCODE' : 'K' + aCmd + 'Opcode',
         'PARAMETERS' : 'THCIErrorCode aErrorCode, const TDesC8& aEventData',
         'CONSTRUCTOR_PARAMETERS' : 'aErrorCode, aEventData',
         'ACCESSOR_DEFINITION' : makeAccessorDef(aParams),
         'ACCESSOR_IMPLEMENTATION' : makeAccessorImpl(aCmd + 'CompleteEvent', aParams),
         'GENERATE_TIME_STAMP': strftime("%a, %d %b %Y %H:%M:%S") + ' (time stamp)'}
    c['FAKERPARAMETERS'] = makeFakingParameters(aParams, True)
    c['FAKER_PARAMETER_TOTAL_LENGTH'] = makeParameterTotalLengthValue(aParams, True)
    c['FAKERSETERRORCODEBYPARAM'] = makeEventSetErrorCode(aParams, True, completeEventPreHeaderBytes())
    c['EVENTSETERRORCODEBYEXTRACT'] = makeEventSetErrorCode(aParams, False, completeEventPreHeaderBytes())
    c['FAKERWRITE_IMPLEMENTATION'] = makeFakingWriteImplementation(aParams, True)
    
    #file(aHeaderPath + '\\' + c['FILENAME'] + '.h', 'w+').write(aHeaderTemplate.substitute(c))
    #file(aSourcePath + '\\' + c['FILENAME'] + '.cpp', 'w+').write(aSourceTemplate.substitute(c))

    doTimeStampCompareAndWrite(aHeaderPath + '\\' + c['FILENAME'] + '.h', aHeaderTemplate.substitute(c))
    doTimeStampCompareAndWrite(aSourcePath + '\\' + c['FILENAME'] + '.cpp', aSourceTemplate.substitute(c))
