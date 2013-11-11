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

# Generates a descriptor from a hexidecimal specifier of the format 0xyy [* n] where the '* n' is optional.
# if '* n' is present as many 0xyy will be strung together to form the string.
def makeStrValue(aValue):
    res = re.split(r'[^a-fA-FxX0-9\*]+', aValue)

    if len(res) == 1:
        return res[0]
    elif len(res) == 3:
        res_str = '_L("'

        i = 0
        while i < int(res[2]):
            i += 1
            res_str += '\\' + res[0]

        return res_str + '")'
    else:
        return aValue

# Generates a 8-bit descriptor from a hexidecimal specifier of the format 0xnn [* n] where the '* n' is optional.
# if '* n' is present as many 0xnn will be strung together to form the string.
def makeShortStrValue(aValue):
    res = re.split(r'[^a-fA-FxX0-9\*]+', aValue)

    if len(res) == 1:
        return res[0]
    elif len(res) == 3:
        res_str = '_L8("'

        i = 0
        while i < int(res[2]):
            i += 1
            res_str += '\\' + res[0]

        return res_str + '")'
    else:
        return aValue        

# Encapsulates a parameter type.
# Has a name, as specified in the command file
# typename, Symbian type mapped to
# size
# getter, method to get type from event data descriptor
# setter, method to write type to CHctlCommandFrame
# maker, method (usually constructor) for creating values in test cases
# byref, if value is passed by reference or not
# min value, minimum value to use when testing
# max value, maximum value to use when testing

class Type:
    def __init__(self, aName, aTypename, aRetTypename, aEventRetTypename, aSize, aGetter, aCmdFrameSetter, aEventSetter, aMaker, aPassByRef, aReturnByRef, aEventReturnByRef, aMinValue, aMaxValue, aCStringAvoider, aPckgBufType, aCast):
        self.iName = aName
        self.iTypename = aTypename
        self.iRetTypename = aRetTypename
        self.iEventRetTypename = aEventRetTypename
        self.iSize = aSize
        self.iGetter = aGetter
        self.iSetter = aCmdFrameSetter
        self.iEventSetter = aEventSetter
        self.iMaker = aMaker
        self.iPassByRef = aPassByRef
        self.iReturnByRef = aReturnByRef
        self.iEventReturnByRef = aEventReturnByRef
        self.iMinValue = makeShortStrValue(aMinValue)
        self.iMaxValue = makeShortStrValue(aMaxValue)
        self.iCStringAvoider = aCStringAvoider
        self.iPckgBufType = aPckgBufType
        self.iCast = aCast

    # Returns a string suitable for member type declarations
    def toMember(self):
        if self.iTypename == 'TDesC8':
            return 'TBuf8<' + str(self.iSize) + '>'
        else:
            return self.iTypename

    # Returns a string suitable for parameter type declarations
    def toParameter(self):
        if self.iPassByRef:
            return 'const ' + self.iTypename + '&'
        else:
            return self.iTypename

    # Returns a string suitable for parameter array type declarations
    def toArrayParameter(self):
        return self.iTypename

    # Returns a string suitable for accessor type declarations
    def toAccessor(self, aEventOrCommand):
        typeName = ''
        if aEventOrCommand == 'Event':
            if self.iEventReturnByRef:
                return 'const ' + self.iEventRetTypename + '&'
            else:
                return self.iEventRetTypename
        elif aEventOrCommand == 'Command':
            typename = self.iTypename
            if self.iRetTypename != '':
                typename = self.iRetTypename
            if self.iReturnByRef:
                return 'const ' + typename + '&'
            else:
                return typename
        else:
            1/0

    # Returns a string constructing a 'max value' of this type
    def toMaxMaker(self):
        if self.iPckgBufType != '':
            return 'TPckgBuf<' + self.iPckgBufType + '>(' + self.iPckgBufType + '_instance_max)'
        elif self.iMaker != '':
            return self.iMaker + '(' + self.maxValue() + ')'
        else:
            return self.maxValue()

    # Returns a string constructing a 'min value' of this type
    def toMinMaker(self):
        if self.iPckgBufType != '':
            return 'TPckgBuf<' + self.iPckgBufType + '>(' + self.iPckgBufType + '_instance_min)'
        elif self.iMaker != '':
            if self.iCStringAvoider != '':
                return self.iMaker + '(' + self.iCStringAvoider + ')'
            else:    
                return self.iMaker + '(' + self.minValue() + ')'
        else:
            if self.iCStringAvoider != '':
                return self.iCStringAvoider
            else:    
                return self.minValue()    

    # Returns the minimum value of type
    def minValue(self):
        return self.iMinValue

    # Returns the maximun value of type
    def maxValue(self):
        return self.iMaxValue
    
    def CStringAvoider(self):
        return self.iCStringAvoider

    def PckgBufType(self):
        return self.iPckgBufType

    # Returns the minimum value of type suitable for constructing a string
    def minValueStr(self):
        #process TUintX types separetely
        if self.iTypename[0:8] == 'TUint16':
            return '\\x0\\x0' 
        elif self.iTypename[0:8] == 'TUint32':
            if self.iSize == 3:
               return '\\x0\\x0\\x0'
            else:   
               return '\\x0\\x0\\x0\\x0'  
        elif self.iTypename[0:8] == 'TUint64':
            return '\\x0\\x0\\x0\\x0\\x0\\x0\\x0\\x0'     
        elif self.iMinValue == '':
            return ''
        #RArray type    
        elif self.iMinValue[0:1] == '{':
            i = 0
            res = ''
            while i < self.iSize:
                i += 1
                res += '\\x0'
            return res 
        #8-bit string       
        elif self.iMinValue[0:4] == '_L8(':
            return self.iMinValue[5:-2]
        #16-bit string
        elif self.iMinValue[0] == '_' :
            return self.iMinValue[4:-2]
        elif self.iMinValue[0:2] == '0x':
            i = 2
            ret = ''
            while i < len(self.iMinValue):
                ret += '\\x' + self.iMinValue[i:i+2]
                i += 2
            return  ret
        else:
            return '\\' + self.iMinValue
            
    # Returns the maximum value of type suitable for constructing a string
    def maxValueStr(self):
        #process TUintX types separetely
        if self.iTypename[0:8] == 'TUint16':
            return '\\xff\\xff' 
        elif self.iTypename[0:8] == 'TUint32':
            if self.iSize == 3:
               return '\\xff\\xff\\xff' 
            else:   
               return '\\xff\\xff\\xff\\xff' 
        elif self.iTypename[0:8] == 'TUint64':
            return '\\xff\\xff\\xff\\xff\\xff\\xff\\xff\\xff'     
        elif self.iMaxValue == '':                                     
            return ''
        #RArray type    
        elif self.iMaxValue[0:1] == '{':
            res = '' 
            i = 0
            while i < self.iSize:                                         
                i += 1
                res += '\\xff'
            return res     
        #8-bit string                         
        elif self.iMaxValue[0:4] == '_L8(':
            return self.iMaxValue[5:-2]
        #16-bit string                   
        elif self.iMaxValue[0] == '_':
            return self.iMaxValue[4:-2]
        elif self.iMaxValue == 'void':
            return ''   
        elif self.iMaxValue[0:2] == '0x':
            i = 2
            ret = ''
            while i < len(self.iMaxValue):
                ret += '\\x' + self.iMaxValue[i:i+2]
                i += 2
            return  ret
        else:
            return '\\' + self.iMaxValue
    
