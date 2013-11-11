# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Tools to parse mbc files
# NOTE: There is unit etc test code for this in testMbcUtils.py
# Any changes to this file should be checked with "python testMbcUtils.py"
# Also note that the test code coverage is not 100%
#

import re
import types
import os

class InvalidInput(Exception):
    """Raised for invalid data in files"""

    def __init__(self, badInput):
        self.__badInput = badInput

    def __str__(self):
        return "InvalidInput(%s)" % self.__badInput

class Unimplemented(Exception):
    """Raised for features we don't cover"""

    def __init__(self, msg):
        self.__msg = msg

    def __str__(self):
        return "Unimplemented(%s)" % self.__msg

class InvalidArg(Exception):
    """Raised for invalid data in argument"""

    def __init__(self, badArg):
        self.__badArg = badArg

    def __str__(self):
        return "InvalidArg(%s)" % self.__badArg

class MissingFile(Exception):
    "Raised when we expect a file and it does not exist"

    def __init__(self, badFname):
        self.__badFname = badFname

    def __str__(self):
        return "MissingFile(%s)" % self.__badFname

class _MbcParser(object):
    """Parse an mbc file, given by name. Return as tuple of the given categories, coupled with the filename."""

    # __slots__ = ["__fname", "__result", "__state"]

    __nullState = -1
    __dirsSectionState = 1 # assumed to be same as tuple index
    __optionDirsSectionState = 2 # assumed to be same as tuple index
    __commandSectionState = 3 # assumed to be same as tuple index

    def __init__(self, fname):
        self.__fname = fname
        self.__dirname = os.path.dirname(fname)
        self.__result = None
        self.__state = _MbcParser.__nullState
        self.__executed = False

    def execute(self):
        "Call to generate the result"
        self.__reset(self.__fname) # initialize __result etc
        self.__openAndParse()
        self.__executed = True

    def __call__(self):
        if not self.__executed:
            self.execute()
        return self.__result

    def __reset(self, fname=None):
        self.__result = (fname,[],[],[])

    def __openAndParse(self):
        "Open file and then parse line-by-line"
        # note deliberately use old version to avoid 2.4 issue. Going foreard, "with" seems to be the correct approach
        # could perhaps write this better using a parser module, or more extensive use of re, but this is not seen as module specific
        inputFile = open(self.__fname)
        try:
            for line in inputFile:
                self.__parseLine(line)
        finally:
            inputFile.close()

    def __parseLine(self,line):
        # initially we want the bit of the line up to the first // or \n
        # TODO should be able to do in one re?
        lookforeoln = re.match(r"(.*)$", line) # lose any \n
        if lookforeoln:
            line = lookforeoln.group(1)
        lookforcomment = re.match(r"(.*)//.*", line)
        if lookforcomment:
            line = lookforcomment.group(1)
        line.strip() # remove spaces at start and end
        if not line:
            # skip blank line
            return;
        if line == "SECTION_DIRS":
            self.__state = _MbcParser.__dirsSectionState
        elif line == "SECTION_OPTIONALDIRS":
            self.__state = _MbcParser.__optionDirsSectionState
        elif line == "SECTION_COMMANDS":
            self.__state = _MbcParser.__commandSectionState
        else:
            # for dirs or optionDirs section we are after a single string to treat as directory. don't check here
            if ((self.__state == _MbcParser.__dirsSectionState) or
                (self.__state == _MbcParser.__optionDirsSectionState)):
                matchre = re.match(r'[_\-a-z0-9\\/\.]+$', line, re.IGNORECASE)
                if matchre:
                    # we have a match - add to the tuple
                    matchresult = line # no need to decode. whole thing is the line
                    if matchresult[0] != "\\":
                        # relative path - need to add folder name
                        matchresult = os.path.abspath(os.path.join(self.__dirname, matchresult))
                    self.__result[self.__state].append(matchresult)
                else:
                    raise InvalidInput (line)
            elif self.__state == _MbcParser.__commandSectionState:
                matchre = re.match(r'oneoff\s+([_\-a-z0-9\\/\.]+)\s+(.+)$', line, re.IGNORECASE)
                if matchre:
                    # append tuple of (directory, command). Comes as (group(1),group(2)) but have to
                    # convert relative directory we get to absolute if required
                    matchDir = matchre.group(1)
                    if matchDir[0] != "\\":
                        # relative path- need to add folder name
                        matchDir = os.path.abspath(os.path.join(self.__dirname, matchDir))
                    self.__result[self.__state].append((matchDir,matchre.group(2)))
                else:
                    raise InvalidInput (line)

class _MbcListHandle(object):
    """Handle a list or tuple of filenames, recursively list of tuples as produced by _MbcParser"""

    def __init__(self, fnameList):
        """Assume fnameList is a container. Always generate a list"""
        self.__fnameList = fnameList
        self.__result = []
        self.__executed = False

    def execute(self):
        for fname in self.__fnameList:
            parser = MbcParser(fname)
            parser.execute()
            self.__result.append(parser())
        self.__exectuted = True

    def __call__(self):
        if not self.__exectuted:
            self.execute()
        return self.__result

class MbcParser(object):
    """Given a list of or a filename, return equivalent structure with each filename replaced by tuple of content

    tuple elements are:
    0 - filename
    1 - main directories
    2 - optional directories
    3 - oneoff"""

    def __init__(self, fnameOrList):
        self.__fnameOrList = fnameOrList
        self.__result = None
        self.__executed = False

    def execute(self):
        fnameOrList = self.__fnameOrList
        if isinstance(fnameOrList, list) or isinstance(fnameOrList, tuple):
            parser = _MbcListHandle(fnameOrList)
            parser.execute()
            self.__result = parser()
        elif isinstance(fnameOrList, types.StringTypes):
            parser = _MbcParser(fnameOrList)
            parser.execute()
            self.__result = parser()
        else:
            raise InvalidArg(fnameOrList)

    def __call__(self):
        if not self.__executed:
            self.execute()
        return self.__result

class GetFolderList(object):
    """Given output of MbcParser(), produces list of tuples in the format:

    0 - Element is folder if True (only thing currently supported)
    1 - folder
    2 - comment to use in generated file including original filename

    If folder is optional, will only be added if exists - actually if nameToCheck exists in folder.
    If folder is not optional, and does not exist, this will raise an error.
    """

    def __init__(self, inputList, nameToCheck="bld.inf"):
        if isinstance(inputList, tuple):
            # single element "list" from MbcParser is not always a list!
            self.__list = [inputList]
        else:
            self.__list = inputList
        self.__nameToCheck = nameToCheck
        self.__result = []
        self.__exectuted = False

    def execute(self):
        self.__result = []
        self.__genResult()
        self.__executed = True

    def __genResult(self):
        "Process whole list"
        for listEle in self.__list:
            self.__processEle(listEle)

    def __processEle(self, listEle):
        "Process single element in input list"
        (fname, dirs, optDirs, commands) = listEle
        for dir in dirs:
            combinedFname = os.path.join(dir, self.__nameToCheck)
            exists = os.path.exists(combinedFname)
            if not exists:
                raise MissingFile(combinedFname)
            self.__result.append((True, dir, "From %s"%fname))
        for dir in optDirs:
            combinedFname = os.path.join(dir, self.__nameToCheck)
            exists = os.path.exists(combinedFname)
            if exists:
                self.__result.append((True, dir, "From %s"%fname))
            else:
                self.__result.append((True, None, """Skip "%s" from %s"""%(dir,fname)))
        if commands:
            raise Unimplemented("No support for oneoff - %s" % str(commands))

    def __call__(self):
        if not self.__exectuted:
            self.execute()
        return self.__result

## Minimal example configuration file as we have to produce
## See http://developer.symbian.com/wiki/x/rgD6Bg
##
##<SystemDefinition name="BLAH" schema="2.0.0">
##  <systemModel>
##    <layer name="NEW_CUSTOM_LAYER">
##      <collection name="Fake Collection">
##	<component name="examples">
##	  <unit bldFile="C:\Symbian\Carbide2\workspace\hello_whirled\group" />
##	  <unit bldFile="C:\Symbian\Carbide2\workspace\hello_abld\group" />
##	</component>
##      </collection>
##    </layer>
##  </systemModel>
##</SystemDefinition>


class ConfigFileGenerator(object):
    """Generate xml config file given output from GetFolderList

    Output corresponds to the example in source
    folderList is input from GetFolderList
    outputStream is either filename or assumed to be an open file or StringIO"""

    def __init__(self, folderList, outputStream):
        self.__folderList = folderList
        self.__streamParam = outputStream
        self.__streamIsLocal = False
        self.__stream = None

    def __initStream(self):
        "Work out stream to use. Open stream if required"

        if isinstance(self.__streamParam, basestring):
            # unicode or normal string
            self.__streamIsLocal = True
            self.__stream = open(self.__streamParam, "w")
        else:
            self.__streamIsLocal = False
            self.__stream = self.__streamParam

    def __closeStream(self):
        "If stream is local, close it"

        if self.__streamIsLocal:
            self.__stream.close()

        self.__stream = None # orphan if needs be
        self.__streamIsLocal = False

    def write(self):
        "Called will write output to stream"

        try:
            self.__initStream()
            self.__writeHeaderBit()
            self.__writeFolderList()
            self.__writeTailBit()
        except: # TODO not sure we need this - if we ommit is finally clause run?
            raise
        finally:
            self.__closeStream()

    def __writeHeaderBit(self):
        "Write bit of xml before the folder list"
        # these names all come from the original. Seems no need to vary
        # this code is 9.5 based
        print >> self.__stream, r"""<SystemDefinition name="BLAH" schema="2.0.0">"""
        print >> self.__stream, r"""  <systemModel>"""
        print >> self.__stream, r"""    <layer name="NEW_CUSTOM_LAYER">"""
        print >> self.__stream, r"""      <collection name="Fake Collection">"""
        print >> self.__stream, r"""        <component name="Fake Multimedia">"""

    def __writeTailBit(self):
        "write bit of xml after the folder list"
        print >> self.__stream, r"""        </component>"""
        print >> self.__stream, r"""      </collection>"""
        print >> self.__stream, r"""    </layer>"""
        print >> self.__stream, r"""  </systemModel>"""
        print >> self.__stream, r"""</SystemDefinition>"""

    def __writeFolderList(self):
        for ele in self.__folderList:
            str = self.__strForListEle(ele)
            if str:
                print >> self.__stream, "          %s" % str

    def __strForListEle(self, ele):
        (isFolder, folder, comment) = ele # break down tuple
        result = None
        if isFolder:
            if folder:
                result = r"""<unit bldFile="%s" /><!-- %s -->""" % (folder,comment)
            else:
                result = r"""<!-- %s -->""" % (comment)
        return result

