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
# Writes a 'event'.
# Template substitution values are deduced from event name and event parameters.
# Template substitutions are:
# $CLASSNAME: TNameOfCommandEvent. Name of event class.
# $FILENAME: nameofcommandevent. Lower case version for use in filenames.
# $HEADERGUARD: NAMEOFCOMMANDEVENT. Upper case version for use in header guard defines.
# $EVENTCODE: ENameOfEventEvent. Event code for event, must be in hciopcodes.h.
# $PARAMETERS: If Status parameter: THCIErrorCode aErrorCode, const TDesC8& aEventData. Else: const TDesC8& aEventData
# $FAKERPARAMETERS: fake event parameters TDesC8& aEventData
# $FAKERWRITE_IMPLEMENTATION: the writing of the fake event parameters into the event data
# $CONSTRUCTOR_PARAMETERS: If Status parameter: aErrorCode, aEventData. Else: EOK, aEventData
# $ACCESSOR_DEFINITION: const ParameterType GetParameterName() const;
# - for every parameter
# $ACCESSOR_IMPLEMENTATION: const ParameterType TNameOfCommandCompleteEvent::GetParameterName() const { retunr iParameterName; }
# - for every parameter
# $GENERATE_TIME_STAMP: timestamp showing when the generator produced the file
# 
#

from time import strftime
from utils import doTimeStampCompareAndWrite

# If event doesn't have Status parameter we need to provide a default value of EOK to constructor.
def hasStatus(aParams):
    for p in aParams:
        if p.iName == 'Status':
            return True

    return False

# Make accessor definition for all parameters except Status (provided by THCIEventBase)
def makeAccessorDef(aParams):
    def_str = ''
    
    for p in aParams:
        if p.getName() != 'Status':
            def_str += p.accessorDef('Event') + '\n\t'

    return def_str

# Make accessor implementation for all parameters in except Status (provided by THCIEventBase)
def makeAccessorImpl(aEvent, aParams):
    impl_str = ''

    for p in aParams:
        if p.getName() != 'Status':
            impl_str += p.eventAccessorImpl('T' + aEvent)

    return impl_str

# Make parameters for faking event
# trying to make something like...
#   THCIErrorCode aErrorCode, TBTDevAddr aBDADDR, TBTLinkKey aLinkKey, TUint8 aKeyType, TDesC8& aFakeEventData
# or 
#   THCIErrorCode aErrorCode, TUint8 NumResponses, BTDevAddr aBDADDR[]
# for use in creating the parameter string to fake up an event
def makeFakingParameters(aParams, aCompleteEvent):

    if hasStatus(aParams):
        fakingparams_str = 'THCIErrorCode aStatus, '
    else:
        fakingparams_str = ''
    
    if aCompleteEvent:
        fakingparams_str += 'TUint8 aNumHCICommandPackets, '	

    for p in aParams:
        if p.getName() != 'Status':
            fakingparams_str += p.toEventParameter() + ', '

    fakingparams_str += 'TDes8& aEventData'

    return fakingparams_str

# this adds
#	iErrorCode = aErrorCode; // set the base class error value	
# dependant on whether the event has status 
def makeEventSetErrorCode(aParams, aMemberInitialise, aPreHeaderBytes):
   if (hasStatus(aParams)):
        if aMemberInitialise:
            setErrorCode_str = 'iErrorCode = aStatus;'
        else:
            bytesBefore = 0;
            for p in aParams:
                if p.getName() == 'Status':
                    break
                bytesBefore += p.iType.iSize;
            offset = aPreHeaderBytes + bytesBefore;
            setErrorCode_str = 'iErrorCode = THCIErrorCode(iEventData[' + str(offset) + ']); // Status field is in octet ' + str(offset+1)
   else:
        setErrorCode_str = ''
   return setErrorCode_str

# Make the string for the the Parameter_Total_Length field value
# produces something like
#       3 + 3
def makeParameterTotalLengthValue(aParams, aCompleteEvent):
    totalParamSize = 0
    totalArrayParamSize = 0;
    toPacketEnd = False
    arrayMultipier = '';
    for p in aParams:
        if p.iType.iSize == 'n':
            toPacketEnd = True
            break
        if p.iArray:
            arrayMultipier = 'a' + p.iArray
            totalArrayParamSize += p.iType.iSize
            continue
        totalParamSize += int(p.iType.iSize)
    val = ''
    if toPacketEnd:
        val = 'aEventData.Length()'
    else:
        if arrayMultipier != '':
            val = arrayMultipier + ' * ' + str(totalArrayParamSize) + ' + '
        val += str(totalParamSize)
    if aCompleteEvent:
        val += ' + KCommandCompleteCommonFieldsLength'
    return val

# Make the implementation for faking the event
# produces something like
#       for (i = 0;i<numiitems;i++)
#          WriteDevAddr(aBDADDR[i],0,6,i);
# or 
#	WriteLinkKey(aLinkKey,6);
#	WriteTUint8(aKeyType,22);
def makeFakingWriteImplementation(aParams, aCompleteEvent):
    ## (the parameter length is now output by the base class)
    impl_str = ''
    
    ## Write the functions to set the data for the parameters
    ## If there are multiple array parameters then they are ordered like an
    ## array of structures, see BT Spec 2.0 Vol 2 Part E 5.2. The logic below allows
    ## for multiple such arrays of structures based on sets of consecutive parameters
    ## sharing the same array bound - however this is not really needed since no current
    ## event has more than one array.
    firstOne = True
    currentArray = None
    for p in aParams:
        if True: #p.getName() != 'Status': ## why don't we do the status?
           if not firstOne:
               impl_str += '\n\t'
           else:
               firstOne = False
           if p.iArray:
               if currentArray != p.iArray:
                   if currentArray != None:
                       # end the previous array loop (actually, this is never needed)
                       impl_str += '\t}\n\t'
                       currentArray = None
                   # start the new array loop
                   currentArray = p.iArray	# remember we're doing this array
                   impl_str += 'for(int i=0;i<a' + p.iArray + ';++i)\n\t\t{\n\t'
                   
               # continue generating the array
               impl_str += '\t'+ p.iType.iEventSetter + '(a' + p.getName() + '[i]' 
               if p.iType.iTypename == 'TUint32' or p.iType.iEventSetter == 'PutPaddedString':
                   impl_str += ', ' + str(p.iType.iSize)
               impl_str += ', aEventData);'
           else: 
               if currentArray != None:
                   # finish off the array we were doing (won't actually happen, arrays always come last really)
                   impl_str += '\t}\n\t'
               impl_str += p.iType.iEventSetter + '(a' + p.getName() 
               if p.iType.iTypename == 'TUint32' or p.iType.iEventSetter == 'PutPaddedString':
                  impl_str += ', ' + str(p.iType.iSize)
               impl_str += ', aEventData);'

    if currentArray != None:
        impl_str += '\n\t\t}'
        
    return impl_str
    
def eventPreHeaderBytes():
   return 2

def writeEvent(aEvent, aParams, aHeaderTemplate, aSourceTemplate, aHeaderPath, aSourcePath):
    c = {'EVENT' : aEvent,
         'CLASSNAME' : 'T' + aEvent + 'Event',
         'FILENAME' : aEvent.lower() + 'event',
         'HEADERGUARD' : aEvent.upper() + 'EVENT',
         'EVENTCODE' : 'E' + aEvent + 'Event',
         'ACCESSOR_DEFINITION' : makeAccessorDef(aParams),
         'ACCESSOR_IMPLEMENTATION' : makeAccessorImpl(aEvent + 'Event', aParams),
         'GENERATE_TIME_STAMP': strftime("%a, %d %b %Y %H:%M:%S") + ' (time stamp)'}
    if hasStatus(aParams):
        c['PARAMETERS'] = 'THCIErrorCode aErrorCode, const TDesC8& aEventData'
        c['CONSTRUCTOR_PARAMETERS'] = 'aErrorCode, aEventData'
    else:
        c['PARAMETERS'] = 'const TDesC8& aEventData'
        c['CONSTRUCTOR_PARAMETERS'] = 'EOK, aEventData'
    c['FAKERPARAMETERS'] = makeFakingParameters(aParams, False)
    c['FAKER_PARAMETER_TOTAL_LENGTH'] = makeParameterTotalLengthValue(aParams, False)
    c['FAKERSETERRORCODEBYPARAM'] = makeEventSetErrorCode(aParams, True, eventPreHeaderBytes())
    c['EVENTSETERRORCODEBYEXTRACT'] = makeEventSetErrorCode(aParams, False, eventPreHeaderBytes())
    c['FAKERWRITE_IMPLEMENTATION'] = makeFakingWriteImplementation(aParams, False)

    #file(aHeaderPath + '\\' + c['FILENAME'] + '.h', 'w+').write(aHeaderTemplate.substitute(c))
    #file(aSourcePath + '\\' + c['FILENAME'] + '.cpp', 'w+').write(aSourceTemplate.substitute(c))

    doTimeStampCompareAndWrite(aHeaderPath + '\\' + c['FILENAME'] + '.h', aHeaderTemplate.substitute(c))
    doTimeStampCompareAndWrite(aSourcePath + '\\' + c['FILENAME'] + '.cpp', aSourceTemplate.substitute(c))
