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
# Functions for generating CHCICommandBase derived classes.
# Attempts to fill in provided file templates with values deduced from the 
# command name and command parameters.
# Template substitutions are:
# $CLASSNAME: CNameOfCommand, where NameOfCommand is given as parameter.
# $FILENAME: namofcommand, lower case version of NameOfCommand. Used when substituting filename.
# $HEADERGUARD: NAMEOFCOMMAND, upper case version of NameOfCommand. Used as header guard define.
# $PARAMETERS: Type1 aParam1, Type2 aParam2, etc. List of parameters. Used in NewL and constructor definitions and implementations.
# $MEMBERS: Type1 iParam1; Type2 iParam2; etc. Class member declarations.
# $NEWL_DEFINITIONS: static $CLASSNAME NewL(); and if parameter count > 0, static $CLASSNAME NewL($PARAMETERS);. NewL definitions.
# $NEWL_IMPLEMENTATIONS: Implementations of the NewLs.
# $CONSTRUCTOR_DEFINITION: Constructor definition. A class always has a default constructor.
# $CONSTRUCTOR_IMPLEMENTATION: Constructor implementation.
# $DESTRUCTOR_IMPLEMENTATION: Destructor implementation.
# $WRITE_FRAME: Code to write command data to a CHctlCommandFrame.
# $COMMANDFRAME: Comments out CHctlCommandFrame parameter of the WriteFrame method of commands that have no data. This to avoid compiler warnings.
# $MEMBER_ASSIGNMENT: iParam1 = aParam1; For Reset implementation.
# $ACCESSOR_DEFINITIONS: Type1 Param1(); Parameter getter method definition.
# $ACCESSOR_IMPLEMENTATION: Type1 $CLASSNAME::Param1() { return iParam1; }. Parameter getter method implementation.
# $EVENT_MATCH_DEFINITIONS: Definition of the event matching methods
# $EVENT_MATCH_IMPLEMENTATIONS: Implementations of the event matching methods
# $EVENT_MATCH_INCLUDES: System includes required by event matching methods
# $GENERATE_TIME_STAMP: timestamp showing when the generator produced the file
# $PRAGMA_HW_VOICE_SETTING_ADVICE: Write pragma lines for commands setupsynchronousconnection and writevoicesetting
# 
#

import re
import string
from parameter import makeParameters, makeMembers, makeNonOwnedParameters, makeOwnedParameters
from time import strftime
from utils import doTimeStampCompareAndWrite

# makes the aParam part of constructor
def makeConstructorParameters(aParams):
    ctor_str = ""

    for p in aParams:
        if not p.Owned():
            ctor_str += 'a' + p.getName() + ', '

    return ctor_str[:-2]
    
def makeConstructLParameters(aParams):
    conl_str = ""

    for p in aParams:
        if p.Owned():
            conl_str += 'a' + p.getName() + ', '

    return conl_str[:-2]

# determines if a custom ConstructL needs to be created for this class
# currently this is only needed for handling owned parameters.
def customConstructL(aParams):
    return makeConstructLParameters(aParams) != ''

# makes member initialization part of constructor
def makeMemberInitialization(aParams):
    init_str = ", "

    for p in aParams:
        if not p.Owned():
            init_str += 'i' + p.getName() + '(a' + p.getName() + ')\n\t, '

    return init_str[:-4]

# makes class NewL definition
def makeNewLDefinitions(aParams, aClass):
    def_str = ''

    if len(aParams) > 0:
        def_str += 'IMPORT_C static C' + aClass + '* NewL(' + makeParameters(aParams) +');\n\t'

    return def_str + 'IMPORT_C static C' + aClass +'* NewL();' 
    
def makeNewLImp(aClass, aParamString, aConsParamString, aConstructLString):
    imp_str = ''
    imp_str += 'EXPORT_C C' + aClass + '* C' + aClass + '::NewL(' + aParamString + ')\n\t{\n\t'
    imp_str += 'C' + aClass + '* self = new (ELeave) C' + aClass + '(' + aConsParamString + ');\n\t'
    imp_str += 'CleanupStack::PushL(self);\n\t'
    imp_str += 'self->' + aConstructLString + ';\n\t'
    imp_str += 'CleanupStack::Pop(self);\n\t'
    imp_str += 'return self;\n\t}'
    return imp_str

# makes class NewL implementation
def makeNewLImplementations(aParams, aClass):
    imp_str = ''

    if len(aParams) > 0:
        imp_str += makeNewLImp(aClass, makeParameters(aParams), makeConstructorParameters(aParams), makeConstructLCall(aParams))
        imp_str += '\n\n'
        
    imp_str += makeNewLImp(aClass, '', '', 'CHCICommandBase::BaseConstructL()')
    return imp_str
    
def makeConstructLCall(aParams):
    imp_str = ''
    if customConstructL(aParams):
        imp_str += 'ConstructL(' + makeConstructLParameters(aParams) + ')'
    else:
        imp_str += 'CHCICommandBase::BaseConstructL()'
    return imp_str

# makes class constructor definition (also ConstructL if appropriate).
def makeConstructorDefinitions(aParams, aClass):
    def_str = ''

    if len(aParams) > 0:
        def_str += 'C' + aClass + '(' + makeNonOwnedParameters(aParams) + ');\n\t'
        
    def_str += 'C' + aClass + '();'
    
    if customConstructL(aParams):
        def_str += '\n\tvoid ConstructL(' + makeOwnedParameters(aParams) + ');'

    return def_str

# makes class constructor implementation
def makeConstructorImplementations(aParams, aClass, aMatchParams):
    imp_str = ''
    
    CreditsConsumed = aMatchParams[0]
    ExpCmdStatus = aMatchParams[1]
    ExpCmdComplete = aMatchParams[2]

    if len(aParams) > 0:
        imp_str += 'C' + aClass + 'Command::C' + aClass + 'Command(' + makeNonOwnedParameters(aParams) + ')\n\t: CHCICommandBase(K' + aClass + 'Opcode)\n\t' + makeMemberInitialization(aParams) + '\n\t{\n\t'
        if int(CreditsConsumed) != 1:
            imp_str += 'SetCreditsConsumed(' + str(CreditsConsumed) + ');\n\t'
        if ExpCmdStatus == 'False':
            imp_str += 'SetExpectsCommandStatusEvent(EFalse);\n\t'
        if ExpCmdComplete == 'False':
            imp_str += 'SetExpectsCommandCompleteEvent(EFalse);\n\t'
        imp_str += '}\n\n'

    imp_str += 'C' + aClass + 'Command::C' + aClass + 'Command()\n\t: CHCICommandBase(K' + aClass + 'Opcode)\n\t{\n\t'
    if int(CreditsConsumed) != 1:
        imp_str += 'SetCreditsConsumed(' + str(CreditsConsumed) + ');\n\t'
    if ExpCmdStatus == 'False':
        imp_str += 'SetExpectsCommandStatusEvent(EFalse);\n\t'
    if ExpCmdComplete == 'False':
        imp_str += 'SetExpectsCommandCompleteEvent(EFalse);\n\t'

    imp_str += '}'
    
    if customConstructL(aParams):
        imp_str += '\n\nvoid C' + aClass + 'Command::ConstructL(' + makeOwnedParameters(aParams) + ')\n\t{\n\tCHCICommandBase::BaseConstructL();\n\t'
        imp_str += makeOwnedMemberAssignment(aParams)
        imp_str += '\n\t}'

    return imp_str
    
def makeDestructorImplementation(aParams):
    imp_str = ''

    # We only need to close arrays
    for p in aParams:
        if p.iArray != '':
            imp_str += 'i' + p.getName() + '.Close();\n\t'

    return imp_str[0:-2]

# makes code that write command data to CHctlCommandFrame
def makeWriteFrame(aParams):

    ## (the parameter length is now output by the base class)
    frame_str = ''
    
    ## Write the functions to set the data for the parameters
    ## If there are multiple array parameters then they are ordered like an
    ## array of structures, see BT Spec 2.0 Vol 2 Part E 5.2. The logic below allows
    ## for multiple such arrays of structures based on sets of consecutive parameters
    ## sharing the same array bound - however this is not really needed since no current
    ## event has more than one array.
    firstOne = True
    currentArray = None
    for p in aParams:
        if not p.iNotInFrame:
           if not firstOne:
               frame_str += '\n\t'
           else:
               firstOne = False
           if p.iArray:
               if currentArray != p.iArray:
                   if currentArray != None:
                       # end the previous array loop (actually, this is never needed)
                       frame_str += '\t}\n\t'
                       currentArray = None
                   # start the new array loop
                   currentArray = p.iArray	# remember we're doing this array
                   frame_str += 'for(int i=0;i<i' + p.iArray + ';++i)\n\t\t{\n\t'
                   
               # continue generating the array
               frame_str += '\taCommandFrame.' + p.iType.iSetter + '(' + p.setterParams('i') + ');'
           else:
               if currentArray != None:
                   # finish off the array we were doing (won't actually happen, arrays always come last really)
                   frame_str += '\t}\n\t'
               frame_str += 'aCommandFrame.' + p.iType.iSetter + '(' + p.setterParams() + ');'

    if currentArray != None:
        frame_str += '\n\t\t}'

    return frame_str


# if we do not have command data comment out parameter to avoid compiler warnings
def makeCommandFrame(aParams):
    if len(aParams) > 0:
        return 'CHctlCommandFrame& aCommandFrame'
    else:
        return 'CHctlCommandFrame& /*aCommandFrame*/'

# generates code block of form iParam = aParam;
def makeMemberAssignment(aParams):
    ass_str = ''
    
    for p in aParams:
        ass_str += p.memberAssignment() + '\n\t'

    return ass_str[:-2]
    
def makeOwnedMemberAssignment(aParams):
    ass_str = ''
    
    for p in aParams:
        if p.Owned():
            ass_str += p.memberAssignment() + '\n\t'

    return ass_str[:-2]

# make getter definition of form Param()
def makeAccessorDefinitions(aParams):
    def_str = ''

    for p in aParams:
        def_str += p.accessorDef('Command') + '\n\t'

    return def_str[:-2]

# make getter implementation
def makeAccessorImplementations(aParams, aClass):
    imp_str = ''

    for p in aParams:
        imp_str += p.commandAccessorImpl(aClass)

    return imp_str


def zsafe_codeEventMatch(aMatchParams, aFirstTime):
    code_str = ''

    code_str += '\n'
    for p in aMatchParams[:]:
        code_str += p + '\n'

    return code_str

#make event match
def codeEventMatch(aMatchParams, aFirstTime):
    code_str = ''
    eventName = aMatchParams[0]
    concludesCmd = aMatchParams[1]
    continueMatching = aMatchParams[2]

    if not aFirstTime:
        code_str += 'else '

    code_str += 'if (aEvent.EventCode() == ' 'E' + eventName +')\n\t\t'
    code_str += '{\n\t\t'

    if len(aMatchParams) > 3:
        matchOperands = aMatchParams[3:]
        
        if eventName == 'CommandCompleteEvent':
            code_str += 'THCI' + eventName + '& event = THCI' + eventName + '::Cast(aEvent);\n\t\t'
        else:
            code_str += 'T' + eventName + '& event = T' + eventName + '::Cast(aEvent);\n\t\t'

        if len(matchOperands) > 1:
            code_str += 'if ('
        else:
            code_str += 'if '

        i = 0
        while i < len(matchOperands):
            operands = string.split(matchOperands[i],'_')
 
            if len(operands) == 1:
                code_str += '(event.' + operands[0] + '() == ' + operands[0] + '())'
            else:
                code_str += '(event.' + operands[0] + '() == ' + operands[1]
                if len(operands) > 2:
                	code_str += '()'	# kludge: trailing _ means we need a function call syntax
                code_str += ')'
            i += 1
            if i < len(matchOperands):
                code_str += ' && '

        if len(matchOperands) > 1:
            code_str += ')\n\t\t\t'
        else:
            code_str += '\n\t\t\t'

        code_str += '{\n\t\t\t'
        code_str += 'aMatchesCmd = ETrue;\n\t\t\t'
        code_str += 'aConcludesCmd = E' + concludesCmd + ';\n\t\t\t'
        code_str += 'aContinueMatching = E' + continueMatching + ';\n\t\t\t'
        code_str += '}\n\t\t'

    else:    
        code_str += 'aMatchesCmd = ETrue;\n\t\t'
        code_str += 'aConcludesCmd = E' + concludesCmd + ';\n\t\t'
        code_str += 'aContinueMatching = E' + continueMatching + ';\n\t\t'
        
    code_str += '}\n\t'

    return code_str

#make event matching method definitions
def makeEventMatchDefinitions(aParams):
    def_str = ''
    Creditsconsumed = aParams[0]
    ExpCmdStatus = aParams[1]
    ExpCmdComplete = aParams[2]

    if len(aParams) > 3 and 'Match' in aParams: # Always define a match function.
        def_str += 'virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;\n\t'

    return def_str

#make event matching method definitions
def makeEventMatchImplementations(aParams, aClass):
    imp_str = ''
    Creditsconsumed = aParams[0]
    ExpCmdStatus = aParams[1]
    ExpCmdComplete = aParams[2]

    if len(aParams) > 3 and 'Match' in aParams:
        imp_str += '/*virtual*/ void ' + 'C' + aClass + 'Command::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const\n\t'
        imp_str += '{\n\t'

        # Produce matching code specific to command
        m = aParams.index('Match') + 1
        firstTime = True
        while ('Match' in aParams[m:]):
            nm = m + aParams[m:].index('Match')
            imp_str += codeEventMatch(aParams[m:nm], firstTime)
            m = nm + 1
            firstTime = False
        else:
            imp_str += codeEventMatch(aParams[m:], firstTime)

        # Insert general matching code
        imp_str += '// Command Status Event and default Command Complete Event matching\n\t'
        imp_str += '// is implemented in the base class.  If we haven\'t matched already\n\t'
        imp_str += '// then we should try the default matching.\n\t'
        imp_str += 'if (!aMatchesCmd)\n\t\t'
        imp_str += '{\n\t\t'
        imp_str += 'CHCICommandBase::Match(aEvent, aMatchesCmd, aConcludesCmd, aContinueMatching);\n\t\t'
        imp_str += '}\n\t'
        imp_str += '}\n\t'

    return imp_str

#make event matching method definitions
def makeEventMatchIncludes(aParams):
    inc_str = ''

    remParams = aParams[:]
    while ('Match' in remParams):
        e = remParams.index('Match') + 1
        eventName = remParams[e]
        remParams = remParams[e:]

        if eventName != 'CommandCompleteEvent':
            inc_str += '#include <bluetooth/hci/' + string.lower(eventName) + '.h>\n'

    return inc_str

# if command is setupsynchronousconnection or writevoicesetting write pragma lines
def makePragmaHwVoiceSettingAdvice(aCmd):
    if aCmd == 'WriteVoiceSetting' or aCmd == 'SetupSynchronousConnection':
        strPragma = '#pragma message("Value of iVoiceSetting member is closely related to hardware used. ")\n'
        strPragma += '#pragma message("Bits: Input Coding, Input Data, Input Sample Size and Linear_PCM_Bit_pos, should be set...")\n'
        strPragma += '#pragma message("...according to hardware capabilities, and OR-ed to create appropriate iVoiceSetting value.")'
        return strPragma
    return ''

def writeCommand(aCmd, aMatchData, aParams, aHeaderTemplate, aSourceTemplate, aHeaderPath, aSourcePath):
    # separate by ,
    matchParams = re.split(r'[^a-zA-Z_0-9\*]+', aMatchData);
    
    # Make template values
    c = {'COMMAND': aCmd, # plain command name
         'CLASSNAME': 'C' + aCmd + 'Command', #CaCmd
         'FILENAME': aCmd.lower() + 'command', # acmd
         'HEADERGUARD': aCmd.upper() + 'COMMAND', # ACMD
         'PARAMETERS': makeParameters(aParams), # aParam1, aParam2
         'MEMBERS': makeMembers(aParams), # iParam1; iParam2
         'NEWL_DEFINITIONS': makeNewLDefinitions(aParams, aCmd + 'Command'), # NewL($PARAMETERS)
         'NEWL_IMPLEMENTATIONS': makeNewLImplementations(aParams, aCmd + 'Command'), 
         'CONSTRUCTOR_DEFINITIONS': makeConstructorDefinitions(aParams, aCmd + 'Command'),
         'CONSTRUCTOR_IMPLEMENTATIONS': makeConstructorImplementations(aParams, aCmd, matchParams), # Don't add 'Command', aCmd is used to build K...Opcode
         'DESTRUCTOR_IMPLEMENTATION': makeDestructorImplementation(aParams),
         'WRITE_FRAME':  makeWriteFrame(aParams),
         'COMMANDFRAME': makeCommandFrame(aParams),
         'MEMBER_ASSIGNMENT': makeMemberAssignment(aParams),
         'ACCESSOR_DEFINITIONS': makeAccessorDefinitions(aParams),
         'ACCESSOR_IMPLEMENTATIONS': makeAccessorImplementations(aParams, 'C' + aCmd + 'Command'),
         'EVENT_MATCH_DEFINITIONS': makeEventMatchDefinitions(matchParams),
         'EVENT_MATCH_IMPLEMENTATIONS': makeEventMatchImplementations(matchParams, aCmd),
         'EVENT_MATCH_INCLUDES': makeEventMatchIncludes(matchParams),
         'PRAGMA_HW_VOICE_SETTING_ADVICE': makePragmaHwVoiceSettingAdvice(aCmd),
         'GENERATE_TIME_STAMP': strftime("%a, %d %b %Y %H:%M:%S") + ' (time stamp)'}  # (time stamp) is needed for preventing unneccesary submissions.

    # Fill in template
    #file(aHeaderPath + '\\' + c['FILENAME'] + '.h', 'w+').write(aHeaderTemplate.substitute(c))
    #file(aSourcePath + '\\' + c['FILENAME'] + '.cpp', 'w+').write(aSourceTemplate.substitute(c))
    
    doTimeStampCompareAndWrite(aHeaderPath + '\\' + c['FILENAME'] + '.h', aHeaderTemplate.substitute(c))
    doTimeStampCompareAndWrite(aSourcePath + '\\' + c['FILENAME'] + '.cpp', aSourceTemplate.substitute(c))


