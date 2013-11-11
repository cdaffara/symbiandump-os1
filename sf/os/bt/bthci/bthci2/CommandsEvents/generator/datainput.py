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
import ConfigParser

from parameter import Parameter
from type import Type

# Opens and lexes the type spec file.
def getTypes(aTypefile):
    # Add default types, looked up by 'size'
    types = {'1' : Type('1', 'TUint8',  '', 'TUint8',  1, 'AsTUint8',  'PutByte',    'PutTUint8',  '', False, False, False, '0x0', '0xff', '', '', ''),
             '2' : Type('2', 'TUint16', '', 'TUint16', 2, 'AsTUint16', 'PutBytes16', 'PutTUint16', '', False, False, False, '0x0', '0xffff', '', '', ''),
             '3' : Type('3', 'TUint32', '', 'TUint32', 3, 'AsTUint32', 'PutBytes32', 'PutTUint32', '', False, False, False, '0x0', '0xffffff', '', '', ''),
             '4' : Type('4', 'TUint32', '', 'TUint32', 4, 'AsTUint32', 'PutBytes32', 'PutTUint32', '', False, False, False, '0x0', '0xffffffff', '', '', ''),
             '8' : Type('8', 'TUint64', '', 'TUint64', 8, 'AsTUint64', '',           'PutTUint64', '', False, False, False, '0x0', '0xffffffffffffffff', '', '', '')}
    
    cfg = ConfigParser.ConfigParser()
    cfg.read([aTypefile])

    for s in cfg.sections():
        t = Type(s, 
                 cfg.get(s, 'Type'),
                 cfg.get(s, 'RetType'),
                 cfg.get(s, 'EventRetType'),
                 cfg.getint(s, 'Size'),
                 cfg.get(s, 'Getter'),
                 cfg.get(s, 'Setter'),
                 cfg.get(s, 'EventSetter'),
                 cfg.get(s, 'Maker'),
                 cfg.getboolean(s, 'PassByRef'),
                 cfg.getboolean(s, 'ReturnByRef'),
                 cfg.getboolean(s, 'EventReturnByRef'),
                 cfg.get(s, 'MinValue'),
                 cfg.get(s, 'MaxValue'),
                 cfg.get(s, 'CStringAvoider'),
                 cfg.get(s, 'PckgBufType'),
                 cfg.get(s, 'Cast'))
        types[t.iName] = t

    return types

def checked_regexop(op, regex, string):
    if op is None or regex is None or string is None:
        print "Invalid argument passed to checked_regexop";
        sys.exit(0);

    result = op(regex, string);
    if result == None:
        print "Regex operation '" + op.__name__ + "' returned None object"
        print "with regex '" + regex + "'"
        print "and string '" + string + "'"
        import sys;
        sys.exit(0);

    return result;

# Retrieves one line from the command spec file.
# Uses a regexp to split the data.
def getLines(aLines):
    if aLines == []:
        return []
    else:
        m = checked_regexop(re.match, r'([0-9a-zA-Z_,\(\)]+):(,?)(.*)', aLines[0])

        name = checked_regexop(re.match, r'([a-zA-Z_]+)', m.group(1))
        # we would check_regexop for the extras, but if we don't have any arguments
        # to pass then we will get a None back.... so we don't want to exit if
        # the result is None.
        # ext = checked_regexop(re.search, r'(\([0-9a-zA-Z_,]+\))', m.group(1))
        ext = re.search(r'(\([0-9a-zA-Z_,]+\))', m.group(1))

        name = name.group(1)
        if ext == None:
            ext = ""
        else:
            ext = ext.group(1)[1:-1]

        m = checked_regexop(re.split, r'[^a-zA-Z_0-9@\*]+', m.group(3))

        m = filter(lambda (v): len(v) != 0,
                   map(lambda (v): v.strip(), m))
        return [(name, ext, m)] + getLines(aLines[1:])

# Retrieves all entries from the command spec file, ignoring lines starting with a #.
def getEntries(aFile):
    return getLines(filter(lambda(e): e[0] != '#', file(aFile).readlines()))

# Calculates the size of all parameters used by a command.
def calcArrayBlockSize(aParams):
    blockSize = 0

    for p in aParams:
        if p.iArray != '':
            blockSize += p.iType.iSize

    for p in aParams:
        p.iBlockSize = blockSize

# Retrieves a dictionary of the 'folded' entries from the command file.
# A folded entry is a list of parameters indexed in the dictionary by the command name.
def getFolded(aEntries, aTypes, aHeaderOffset):
    folded = {}

    for e in aEntries:
        folded[e[0]] = (e[1], getParameters(e[2], aTypes, aHeaderOffset))
        calcArrayBlockSize(folded[e[0]][1])

    return folded

# Builds a list of parameters for a command
def getParameters(aParams, aTypes, aOffset):
    if aParams == []:
        return []
                                                            
    if aParams[0] in aTypes:
        t = aTypes[aParams[0]]
    elif aParams[0][0] == '@' and aParams[0][1:] in aTypes:
        t = aTypes[aParams[0][1:]]
    else: # Type not known, assume TDesC8
        t = aTypes.get(aParams[1], Type(aParams[0], 'TDesC8', 'TPtrC8', 'TPtrC8', aParams[1], 'AsString', 'PutString', 'PutString', '', True, False, False, '_L8("\\x0")', '_L8("\\xff")', 'UnknownType_instance', '', ''))

    newOffset = aOffset

    if t.iSize != 'n': # Not of 'infinite' size
        newOffset += int(t.iSize)

    if len(aParams) > 2 and aParams[2] == '*': # Is 'array', delimited by other parameter.
        return [Parameter(t, aParams[0], aOffset, aParams[3])] + getParameters(aParams[4:], aTypes, newOffset)
    else:
        return [Parameter(t, aParams[0], aOffset)] + getParameters(aParams[2:], aTypes, newOffset)
