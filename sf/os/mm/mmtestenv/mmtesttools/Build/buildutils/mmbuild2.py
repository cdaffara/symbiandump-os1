
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
# mmbuild2 - python script intended to replace mmbuild.pl
#

from optparse import (make_option,OptionParser)
import sys
import os
import subprocess
import MbcUtils

# some literals. TODO - use explicit type?
_KAll = "all"
_KMmf = "mmf"
_KMisc = "misc"
_KIcl = "icl"

_KMainTempConfig = ".config.xml"
_KTestTempConfig = ".testconfig.xml"
_KTestBuildTempConfig = ".testbuildconfig.xml"

class Unimplemented(Exception):
    """Raised for invalid commands"""

    def __init__(self, msg):
        self.__msg = msg

    def __str__(self):
        return self.__msg

class Script(object):


    # TODO this option_list should be replaced by add_option calls to parameter
    # option_list parameter to OptionParser is down as deprecated
    option_table = [
        make_option ("-t", "--test",
                     action="store_true", dest="buildTest", default=False,
                     help="""build test code rather than main code (without -b, that built by "abld build" or equiv)"""),
        make_option ("-b", "--testBuild",
                     action="store_true", dest="buildTestBuild", default=False,
                     help="""when given with -t, build that code that requires "abld test build" """),
         make_option ("-x", "--additional", "--opt", "--extra",
                     action="store_true", dest="extra", default=False,
                     help="build additional modules (from Xopt.mbc files)"),
        make_option ("-i", "--icl",
                     action="store_const", const=_KIcl, dest="toBuild", default=_KAll,
                     help="build icl modules only"),
        make_option ("-m", "--mmf",
                     action="store_const", const=_KMmf, dest="toBuild", default=_KAll,
                     help="build mmf modules only"),
        make_option ("-c", "--misc",
                     action="store_const", const=_KMisc, dest="toBuild", default=_KAll,
                     help="build misc modules only"),
        make_option ("-f", "--full", "--all",
                     action="store_const", const=_KAll, dest="toBuild", default=_KAll,
                     help="build all modules (which depends on if -x is given too)"),
        make_option ("-k", "--keepgoing",
                     action="store_true", dest="keepgoing", default=False,
                     help="keep going if errors are found"),
        make_option ("-g", "--gccxml",
                     action="store_true", dest="gccxml", default=False,
                     help="build for gccxml"),
        make_option ("-s", "--skipmake",
                     action="store_true", dest="skipmake", default=False,
                     help="skip building makefile (ignored)"),
        make_option ("--iclTestdata",
                     action="store_true", dest="icl_tests", default=False,
                     help="build ICL Tests"),
        ]

    description = \
"""Script for selection appropriate set of multimedia
components and build them together, so build order does not matter.
One, and only one of "setup", "build" or "clean" must be given.
"""

    def __init__(self):
        # need to implement commands as property and not class constant or
        # we get a forward declaration problem
        self.iCommands = {"setup": self.doSetup, "build": self.doBuild, "clean": self.doClean}
        self.iOptions = None
        self.iProdCodeList = []
        self.iTestCodeList = []
        self.iTestBuildCodeList = []
        self.iOptionParser = None
        self.iDebug = False

    def main(self):
        "main function of script"
        self.iOptionParser = OptionParser(
            option_list = Script.option_table,
            usage="%prog [-t [-b]] [-i|-m|-c|-f] (setup|build [winscw]|clean)",
            description = Script.description
            )
        (self.iOptions, args) = self.iOptionParser.parse_args()

        self.DebugPrint (str(self.iOptions))
        self.DebugPrint (str(args))

        if (len(args)==0 or not args[0] in self.iCommands or
                not self.CheckOptions()):
            self.iOptionParser.print_usage()
            return -1

        remainingArgs = args[1:len(args)]

        return self.iCommands[args[0]](remainingArgs) # effective switch to doBuild, doSetup or doClean

    def CheckOptions(self):
        "Check for any invalid option combinations. Warn about ignored ones etc"
        if self.iOptions.skipmake:
            print ("Warning ignoring -s - option is no longer valid in raptor version")
        if not self.iOptions.buildTest and self.iOptions.buildTestBuild:
            return False # can't do -b without -t
        return True # all other combinations OK

    def doBuild(self, remainingArgs):
        # if we have a remainingArg, only "winscw" is allowed
        if not (len(remainingArgs)==0 or len(remainingArgs)==1 and remainingArgs[0]=="winscw"):
            self.iOptionParser.print_usage()
            return -1

        # for normal build need to run "sbs -s .config.xml build"
        # for test build need to run "sbs -s .testconfig.xml build"
        # use --logfile=- to send output to stdout instead of log file
        configFile = _KMainTempConfig
        testBuild = False
        if self.iOptions.buildTest:
            if not self.iOptions.buildTestBuild:
                # build test config instead when -t etc given
                configFile = _KTestTempConfig
            else:
                # build test config instead when -t -b etc given
                configFile = _KTestBuildTempConfig
                testBuild = True
        sbs_command = self.sbsCommand()
        commands = [sbs_command, "-s", configFile]
        commands += ["--logfile=-"] # send output to stdout
        if self.iOptions.keepgoing:
            commands += ["--keepgoing"]
        if testBuild:
            commands += ["--config=winscw.test"]
        else:
            commands += ["--config=winscw"]
        if not(len(remainingArgs)>0 and remainingArgs[0]=="winscw"):
            # not the winscw scenario - we want both winscw and armv5
            if testBuild:
                commands += ["--config=armv5.test"]
            else:
                commands += ["--config=armv5"]
        commands += ["--filters=FilterSquashLog"] # reduce log size
        commands += ["--tries=2"] # retry on failure - e.g. for license fails
        commands += ["build"]
        self.DebugPrint("""command="%s" """ % str(commands))
        print "------------------ sbs start : %s" % str(commands)
        sys.stdout.flush() # flush any output here so appears correct in log
        subprocess.check_call(commands) # raises exception on error
        print "------------------ sbs end"
        return 0

    def sbsCommand(self):
        "sbs command - that can be used by subprocess"
        # For some reason, have to work out batch file to run by longhand
        # rather than just saying "sbs" and letting command work it out.
        # Could use sys.command() instead, but that is deprecated
        sbs_home = os.getenv("SBS_HOME")
        assert sbs_home, "SBS_HOME must be defined to use this script"
        sbs_command = os.path.join(sbs_home, "bin", "sbs.bat")
        return sbs_command

    def doSetup(self, remainingArgs):
        if len(remainingArgs)!=0:
            self.iOptionParser.print_usage()
            return -1

        self.buildMbcLists()

        self.DebugPrint ("prodCodeList=%s" % str(self.iProdCodeList))
        self.DebugPrint ("testCodeList=%s" % str(self.iTestCodeList))
        self.DebugPrint ("testBuildCodeList=%s" % str(self.iTestBuildCodeList))

        mbcParser = MbcUtils.MbcParser(self.iProdCodeList)
        folders = mbcParser()
        self.DebugPrint ("folders=%s" % str (folders))
        getFolderList = MbcUtils.GetFolderList(folders)
        groupFolders = getFolderList()
        self.DebugPrint ("prodCodeFolderList=%s" % str(groupFolders))

        generator = MbcUtils.ConfigFileGenerator(groupFolders, _KMainTempConfig)
        generator.write()

        mbcParser = MbcUtils.MbcParser(self.iTestCodeList)
        folders = mbcParser()
        self.DebugPrint ("testfolders=%s" % str (folders))
        getFolderList = MbcUtils.GetFolderList(folders)
        groupFolders = getFolderList()
        self.DebugPrint ("testCodeFolderList=%s" % str(groupFolders))

        generator = MbcUtils.ConfigFileGenerator(groupFolders, _KTestTempConfig)
        generator.write()

        mbcParser = MbcUtils.MbcParser(self.iTestBuildCodeList)
        folders = mbcParser()
        self.DebugPrint ("testBuildfolders=%s" % str (folders))
        getFolderList = MbcUtils.GetFolderList(folders)
        groupFolders = getFolderList()
        self.DebugPrint ("testBuildCodeFolderList=%s" % str(groupFolders))

        generator = MbcUtils.ConfigFileGenerator(groupFolders, _KTestBuildTempConfig)
        generator.write()
        return 0

    def buildMbcLists(self):
        # some boolean values
        want_mmf = self.iOptions.toBuild in [_KAll, _KMmf];
        want_icl = self.iOptions.toBuild in [_KAll, _KIcl];
        want_misc = self.iOptions.toBuild in [_KAll, _KMisc];
        want_extra = self.iOptions.extra
        want_icl_tests = self.iOptions.icl_tests

        # now build up the lists depending on which "component" we want
        # perhaps this should be reworked as a table, but not obvious as to how
        self.iTestCodeList += ["AllTests.mbc", "TestFrameworkTest.mbc",
                               "targettools.mbc"]
        if want_mmf:
            self.iProdCodeList += ["mmf.mbc"]
            self.iTestCodeList += ["mmfTest.mbc"]
            self.iTestBuildCodeList += ["mmfTestBuild.mbc"]
            # assume mmfPhys and mmfOptPhys can currently be ignored
            # they should generate another list and be built as special cases
            # self.iProdCodeList += ["mmfPhys.mbc"]
            if want_extra:
                self.iProdCodeList += ["mmfOpt.mbc"]
                self.iTestCodeList += ["mmfOptTest.mbc"]
                self.iTestBuildCodeList += ["mmfOptTestBuild.mbc"]
                # self.iProdCodeList += ["mmfOptPhys.mbc"]
            else:
                self.iTestCodeList += ["mmfNotOptTest.mbc"]
        if want_icl:
            self.iProdCodeList += ["icl.mbc"]
            if want_icl_tests:
            	self.iTestCodeList += ["iclTest.mbc"]
            	self.iTestBuildCodeList += ["iclTestBuild.mbc"]
            	if want_extra:
            		self.iTestCodeList += ["iclOptTest.mbc"]
            		self.iTestBuildCodeList += ["iclOptTestBuild.mbc"]
            if want_extra:
                self.iProdCodeList += ["iclOpt.mbc"]   
        if want_misc:
            self.iProdCodeList += ["misc.mbc"]
            self.iTestCodeList += ["miscTest.mbc"]
            self.iTestBuildCodeList += ["miscTestBuild.mbc"]
            if want_extra:
                self.iProdCodeList += ["miscOpt.mbc"]
                self.iTestCodeList += ["miscOptTest.mbc"]
                self.iTestBuildCodeList += ["miscOptTestBuild.mbc"]

    def doClean(self, remainingArgs):
        "clean is called. Note -t clean means clean test code too, not instead"
        # for normal clean need to run "sbs -s .config.xml clean"
        # for test clean need to run "sbs -s .testconfig.xml clean"
        # use --logfile=- to send output to stdout instead of log file
        whatToClean = [_KMainTempConfig]
        if self.iOptions.buildTest:
            whatToClean += [_KTestTempConfig]
        for configFile in whatToClean:
            sbs_command = self.sbsCommand()
            commands = [sbs_command, "-s", configFile]
            commands += ["--logfile=-"] # send output to stdout
            commands += ["reallyclean"]
            self.DebugPrint ("""command="%s" """ % str(commands))
            subprocess.check_call(commands) # raises exception on error
        return 0

    def DebugPrint(self, str):
        "print a string if self.iDebug is set - would be turned on manually"
        if self.iDebug:
            print(str)

if __name__ == "__main__":
    script = Script()
    sys.exit(script.main())
