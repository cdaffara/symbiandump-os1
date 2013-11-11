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
# Test code for MbcUtils
#

from MbcUtils import *
import testDataMbcUtils as testdata
import unittest
import tempfile
import StringIO

class TestParent(unittest.TestCase):
    """Potential parent test"""

    def TempFile(self, str):
        """Create a temp file. Write given string and return filename.
        Note caller is responsible for deleting the file"""
        tfile = tempfile.mkstemp()
        file = os.fdopen(tfile[0],"w")
        print >> file, str
        file.close() # close the file - we just want the filename then
        return tfile[1] # return filename. Note client expected to delete

    def fixupDirsInExpectedList(self, expectedToFixUp):
        """Convert relative filenames to absolute.
        The returned list returns absolute filenames. Need to convert relelative
        names to absolute so can easily compare."""

        for tupEle in expectedToFixUp:
            mbcFile = tupEle[0]
            baseDir = os.path.dirname(mbcFile)
            for i in range(1,3):
                # tupEle[1] is list of required folders
                # tupEle[2] is list of optional folders
                for j in range(0, len(tupEle[i])):
                    absDir = os.path.abspath(os.path.join(baseDir, tupEle[i][j]))
                    tupEle[i][j] = absDir
            # tupEle[3] is list of tuples of dir and command
            for j in range(0, len(tupEle[3])):
                # note need to tuple to list and back to do this
                lst = list(tupEle[3][j])
                absDir = os.path.abspath(os.path.join(baseDir, lst[0]))
                lst[0] = absDir
                tupEle[3][j] = tuple(lst)
        return expectedToFixUp

class SimpleTest(TestParent):
    """Open and return basic case"""

    def __init__(self, sample, expected):
        super(SimpleTest,self).__init__()
        self.sample = sample
        self.expected = expected
        self.tfileName = None

    def fixExpected(self, fname):
        """change the filename slot in self.expected"""
        # expected is a tuple. Need to convert to list and back.
        # then turn into a single element list so can run through
        # fixupDirsInExpectedList()
        tempList = list(self.expected)
        tempList[0] = fname
        newTuple = tuple(tempList)
        fixedList = self.fixupDirsInExpectedList([newTuple])
        self.expected = fixedList[0]

    def runTest(self):
        """SimpleTest.runTest

        Create temp file with known content. Run parser and compare result
        """
        self.tfileName = self.TempFile(self.sample)
        parser = MbcParser(self.tfileName)
        parser.execute()
        self.fixExpected(self.tfileName)
        result = parser()
        self.assertEquals(result, self.expected,
                        "Result(%s) not same as Expected(%s)"%(result,self.expected))

    def tearDown(self):
        if self.tfileName:
            os.unlink(self.tfileName)

class BadDataTest(SimpleTest):
    """Bad data so should get an exception"""

    def __init__(self, sample, testName):
        super(BadDataTest,self).__init__(sample, [])
        self.__testName = testName

    def runTest(self):
        """BadDataTest.runTest

        Use SimpleTest.runTest. Should throw an exception"""
        self.assertRaises(InvalidInput, SimpleTest.runTest, self)

    def shortDescription(self):
		"One line description - additionally giving test name"
		parentStr = super(BadDataTest, self).shortDescription()
		return  "%s-%s" % (parentStr, self.__testName)

class ListTest(TestParent):
    """Open and return basic case, but with list input"""

    def __init__(self, samples, expected, secondStageExpected=None, filenameToLookFor=None):
        super(ListTest,self).__init__()
        self.samples = samples
        self.expected = expected
        self.secondStageExpected = secondStageExpected
        self.filenameToLookFor = filenameToLookFor
        self.tfileNames = []

    def fixExpected(self, indx, fname):
        """change the filename slot in self.expected"""
        # expected is a tuple. Need to convert to list and back
        tempList = list(self.expected[indx])
        tempList[0] = fname
        self.expected[indx] = tuple(tempList)

    def runTest(self):
        """ListTest.runTest

        Create temp files with known content, one per list element. Run parser and compare result
        """
        for indx in range(0, len(self.samples)):
            tfileName = self.TempFile(self.samples[indx])
            self.tfileNames.append(tfileName)
            self.fixExpected(indx, tfileName)
        self.expected = self.fixupDirsInExpectedList(self.expected)
        parser = MbcParser(self.tfileNames)
        parser.execute()
        result = parser()
        self.assertEquals(result, self.expected,
                        "Result(%s) not same as Expected(%s)"%(result,self.expected))
        if self.secondStageExpected:
            getFolderList = GetFolderList(result)
            getFolderList.execute()
            folderList = getFolderList()
            self.assertEquals(folderList, self.secondStageExpected,
                       "Result2(%s) not same as Expected2(%s)"%(folderList,self.secondStageExpected))

    def tearDown(self):
        if self.tfileNames:
            for fname in self.tfileNames:
                os.unlink(fname)

class FolderListUnitTest(TestParent):
    """Test GetFolderList with on-the-fly generated data.

    Create a list on fly with folder of one temp file and a non-existant one. Treatement of second
    is optional."""

    def __init__(self, fussyOnNonExist):
        super(FolderListUnitTest,self).__init__()
        self.__fussyOnNonExist = fussyOnNonExist
        self.__tempFile = None

    def shortDescription(self):
		"""One line description - additionally giving option"""
		parentStr = super(FolderListUnitTest, self).shortDescription()
		return  "%s-%s" % (parentStr, str(self.__fussyOnNonExist))

    def __realTest(self):
        "real test run - separate so can be trapped if required"

        self.__tempFile = tempfile.NamedTemporaryFile() # used to create list and check

        (input,tempFileName,expected) = self.__createDataLists()
        getFolderList = GetFolderList(input, nameToCheck=tempFileName)
        getFolderList.execute()
        result = getFolderList()
        self.assertEquals(result, expected,
                "Result2(%s) not same as Expected2(%s)(filename=%s)"%(result,expected,tempFileName))

    def runTest(self):
        "FolderListUnitTest.runTest"
        if not self.__fussyOnNonExist:
            self.__realTest()
        else:
            # should fail
            self.assertRaises(MissingFile, self.__realTest)

    def tearDown(self):
        "Stop using temp file, so will be deleted"
        self.__tempFile = None

    def __createDataLists(self):
        """Create input and output for test run as lists.

        input will be one element with <tempdir> and second with "nonexistant",
        but which element is used depends on fussyOnNonExist"""
        input = []
        tempFileName = None
        expected = []

        (tempDir,tempFileName) = os.path.split(self.__tempFile.name) # the dir and name of the temporary file we are using
        input += [("f1", [tempDir], [], [])] # the existing file
        expected += [(True, tempDir, 'From f1')]
        if self.__fussyOnNonExist:
            input += [("f2", ["nonexistant"], [], [])]
        else:
            input += [("f2", [], ["nonexistant"], [])]
        expected += [(True, None, 'Skip "nonexistant" from f2')] # always return the non-fussy version, as with fussy we expect exception raised

        return (input,tempFileName,expected)

class FolderListIntTest(TestParent):
    """Integration test combining MbcParser and FolderList

    Create a temporary folder. Add the following contents:

    x/bld.inf
    y/bld.inf
    z/bld.inf
    mbc containing "./x, ./y, [optional] ./z"

    z/bld.inf and y.bld are optional
        option="x" no z or y. Should raise exception.
        option="xy" no z but y. Should get list of ../x and ../y
        option="xz" no y but z. Should raise exception.
        option="xyz" both y and z. Should get list of ../x, ../y and ../z

    """

    def __init__(self, option):
        super(FolderListIntTest,self).__init__()
        self.__option = option
        self.__testDir = None
        self.__testDirs = [] # list of dirs we create, so we can delete them
        self.__testFiles = [] # list of files we create, so we can delete them
        self.__fullMbcFileName = None
        self.__expectedResult = []

    def shortDescription(self):
        "One line description - additionally giving option"
        parentStr = super(FolderListIntTest, self).shortDescription()
        return  "%s-%s" % (parentStr, str(self.__option))

    def tearDown(self):
        for fle in self.__testFiles:
            try:
                os.unlink(fle)
            except:
                pass # ignore any error. assume means we didn't create the file in the end
        # delete folders in reverse, so child directories deleted after parent
        for dir in self.__testDirs[len(self.__testDirs)-1::-1]:
            try:
                os.rmdir(dir)
            except:
                pass # ignore any error. assume means we didn't create the file in the end

    def runTest(self):
        "FolderListIntTest.runTest"
        self.__setup()
        if "y" in self.__option:
            self.__realTest()
        else:
            # expected to raise exception as y/bld.inf does not exist
            self.assertRaises(MissingFile, self.__realTest)

    def __realTest(self):
        parser = MbcParser(self.__fullMbcFileName)
        parser.execute()
        folderList = GetFolderList(parser())
        folderList.execute()
        result = folderList()
        self.assertEquals(result, self.__expectedResult,
            "Result (%s) not that expected (%s)" % (str(result), str(self.__expectedResult)))

    def __setup(self):
        self.__testDir = tempfile.mkdtemp() # __testDir is name of temp folder
        self.__testDirs.append(self.__testDir)
        self.__fullMbcFileName = os.path.join(self.__testDir, "test.mbc")

        for name in ["x","y","z"]:
            fullpath = os.path.join(self.__testDir, name)
            if name in self.__option:
                os.mkdir(fullpath)
                self.__testDirs.append(fullpath)
                filepath = os.path.join(fullpath, "bld.inf")
                fileToWrite = file(filepath, "w")
                self.__testFiles.append(filepath)
                print >>fileToWrite, "//generated" # 9.5 syntax
                fileToWrite.close()
                expected = (True, fullpath, "From %s" % self.__fullMbcFileName)
                self.__expectedResult.append(expected)
            else:
                expected = (True, None, """Skip "%s" from %s""" % (fullpath, self.__fullMbcFileName))
                self.__expectedResult.append(expected)
        mbcFile = file(self.__fullMbcFileName, "w")
        self.__testFiles.append(self.__fullMbcFileName)
        print >>mbcFile, testdata.intTestMbcFile
        mbcFile.close()

class ConfigFileUnitTest(unittest.TestCase):
    """ConfigFile UnitTest"""

    def __init__(self, inputData, expected):
        super(ConfigFileUnitTest,self).__init__()
        self.__inputData = inputData
        self.__expected = expected

    def runTest(self):
        """ConfigFileUnitTest.runTest

        Take dummy folder list and generate XML file. Output goes to string. Compare with expected
        """

        outputStream = StringIO.StringIO()
        generator = ConfigFileGenerator(self.__inputData, outputStream)
        generator.write()
        outputString = outputStream.getvalue()
##        print ("output=%s" % str(outputString))
##        print ("expected=%s" % str(self.__expected))
        self.assertEquals(outputString, self.__expected,
                "Generated output (%s) not same as expected (%s)" % (outputString, self.__expected))

def suite():
    """TestSuite"""

    tests = [
        SimpleTest(testdata.import1, testdata.result1),
        ListTest(testdata.import2, testdata.result2),
        BadDataTest(testdata.badImport1, "bad1"),
        BadDataTest(testdata.badImport2, "bad2"),
        FolderListUnitTest(False),
        FolderListUnitTest(True),
        FolderListIntTest("x"),
        FolderListIntTest("xy"),
        FolderListIntTest("xz"),
        FolderListIntTest("xyz"),
        ConfigFileUnitTest(testdata.testFolderList1, testdata.testXmlFile1),
        ]

    return unittest.TestSuite(tests)


if __name__ == "__main__":
    unittest.TextTestRunner(verbosity=2).run(suite())
