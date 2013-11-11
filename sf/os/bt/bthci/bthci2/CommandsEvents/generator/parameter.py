# Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

# Creates a comma separated string of parameters suitable for method signatures.
def makeParameters(aParams):
    param_str = ""
    
    for p in aParams:
        param_str += p.toParameter() + ', '

    return param_str[:-2]
    
def makeOwnedParameters(aParams):
    param_str = ""
    
    for p in aParams:
        if p.Owned():
            param_str += p.toParameter() + ', '

    return param_str[:-2]
    
def makeNonOwnedParameters(aParams):
    param_str = ""
    
    for p in aParams:
        if not p.Owned():
            param_str += p.toParameter() + ', '

    return param_str[:-2]

# Creates a ;\n\t separated string of parameters suitable for class member declarations.
def makeMembers(aParams):
    member_str = ""

    for p in aParams:
        member_str += p.toMember() + ';\n\t'

    return member_str

# Encapsulates a command parameter, has a name, a type, a offset and if is an 'array' of values has a array size specifier,
# i.e. the name of the parameter that determines the number of 'array entries' of this parameter.
class Parameter:
    # aType, type of parameter
    # aName, name of parameter
    # aArray, name of array size specifier parameter. Optional.
    def __init__(self, aType, aName, aOffset, aArray = ''):
        self.iOffset = aOffset
        if aName[0] == '@':
            self.iName = aName[1:]
            self.iNotInFrame = True
        else:
            self.iName = aName
            self.iNotInFrame = False
        self.iType = aType
        self.iArray = aArray
        

    def getName(self):
        # Defective code strikes again - these are badly coded up values in the txt file so
        # they have to be handled as a special case.
        if self.iName == 'RxPacket_Length' or self.iName == 'TxPacket_Length':
            return self.iName
        ix = self.iName.find('_')
        if ix >= 0:
            return self.iName[0:ix]
        else:
            return self.iName

    # Returns a string of ''MemberType' iName' or 'RArray<'MemberType'> iName' depending on parameter is of type array or not.
    def toMember(self):
        if self.iArray == '':
            return self.iType.toMember() + ' i' + self.getName()
        else:
            return 'RArray< ' + self.iType.toMember() + ' > i' + self.getName()

    # Returns a string of ''ParamType' aName' or 'const RArray<'ParamType'>& aName' depending on parameter is of type array or not.
    def toParameter(self):
        if self.iArray == '':
            return self.iType.toParameter() + ' a' + self.getName()
        else:
            return 'const RArray< ' + self.iType.toArrayParameter() + ' >& a' + self.getName()
            
    def toEventParameter(self):
        if self.iArray == '':
            return self.iType.toParameter() + ' a' + self.getName()
        else:
            return 'const TArray< ' + self.iType.toArrayParameter() + ' >& a' + self.getName()

    # Returns a string used when writing data to a CHctlCommandFrame
    def setterParams(self, indexer = ''):
        # Needs size specifier, for TUint32
        if (self.iType.iSize == 3 or self.iType.iSize == 4 and self.iType.iSetter == 'PutBytes32') or self.iType.iSetter == 'PutPaddedString':
            if indexer != '':
                return 'i' + self.getName() + '[' + indexer + '], ' + str(self.iType.iSize);
            else:
                return 'i' + self.getName() + ', ' + str(self.iType.iSize)
        else:
            if indexer != '':
                return 'i' + self.getName() + '[' + indexer + ']'
            else:
                return 'i' + self.getName()

    # Returns a string used when accessing values in the event data descriptor
    def getterParams(self):
        getter_str = str(self.iOffset)

        if self.iArray != '':
            getter_str += ', ' + str(self.iBlockSize) + ', aIndex'
        
        if self.iType.iSize == 3 or self.iType.iSize == 4 and self.iType.iGetter == 'AsTUint32':
            getter_str += ', ' + str(self.iType.iSize)

        return getter_str

    # Returns a string used to return the value from the event data descriptor
    def getterFunc(self):
        getter_str = self.iType.iGetter + '(' + self.getterParams() + ')'
        if self.iType.iCast != '':
            getter_str = 'static_cast<' + self.iType.iCast + '>(' + getter_str + ')'
        return getter_str

    # Returns an accessor definition in the form of 'IMPORT_C 'AccessorType' iName() const;' or
    # 'IMPORT_C 'AccessorType' iName(TInt aIndex) const;' if parameter is of array type.
    def accessorDef(self, aEventOrCommand):
        if self.iArray == '':
            return 'IMPORT_C ' + self.iType.toAccessor(aEventOrCommand) + ' ' + self.getName() + '() const;'
        else:
            return 'IMPORT_C ' + self.iType.toAccessor(aEventOrCommand) + ' ' + self.getName() + '(TInt aIndex) const;'

    # Returns an accessor implementation in the form of 'EXPORT_C 'AccessorType' aClass::iName() const { ... }' or
    # 'EXPORT_C 'AccessorType' aClass::iName(TInt aIndex) const { ... }' if parameter is of array type
    # Used for events and complete events.
    def eventAccessorImpl(self, aClass):
        if self.iArray == '':
            return 'EXPORT_C ' + self.iType.toAccessor('Event') + ' ' + aClass + '::' + self.getName() + '() const\n\t{\n\treturn ' + self.getterFunc() + ';\n\t}\n\n'
        else:
            return 'EXPORT_C ' + self.iType.toAccessor('Event') + ' ' + aClass + '::' + self.getName() + '(TInt aIndex) const\n\t{\n\treturn ' + self.getterFunc() + ';\n\t}\n\n'

    # Returns an accessor implementation in the form of 'EXPORT_C 'AccessorType' aClass::iName() const { ... }' or
    # 'EXPORT_C 'AccessorType' aClass::iName(TInt aIndex) const { ... }' if parameter is of array type
    # Used for command events.
    def commandAccessorImpl(self, aClass):
        memberName = 'i' + self.getName()
        if self.iType.iRetTypename != '':
            memberName = self.iType.iRetTypename + '(' + memberName + ')'
        if self.iArray == '':
            return 'EXPORT_C ' + self.iType.toAccessor('Command') + ' ' + aClass + '::' + self.getName() + '() const\n\t{\n\treturn ' + memberName + ';\n\t}\n\n'
        else:
            return 'EXPORT_C ' + self.iType.toAccessor('Command') + ' ' + aClass + '::' + self.getName() + '(TInt aIndex) const\n\t{\n\treturn ' + memberName + '[aIndex];\n\t}\n\n'

    # Returns a string of the format 'iName = aName;'
    def memberAssignment(self):
        return 'i' + self.getName() + ' = a' + self.getName() + ';'
        
    def Owned(self):
        return self.iArray != ''
    
    
