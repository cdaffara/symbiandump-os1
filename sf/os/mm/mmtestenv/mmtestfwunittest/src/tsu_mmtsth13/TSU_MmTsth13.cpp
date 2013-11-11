// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This file contains the test steps for Unit Test Suite 13 : TestUtils.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TSU_MmTsthStep13.h"
#include "TSU_MmTsthSuite13.h"

// Specific includes for these test steps
#include "TSU_MmTsth13.h"

// --------------------------------------------

// Unit Test Suite 13 : TestUtils.cpp
// Depends on : none

// Tests :-
//  1. NewL / Construct - create with a CLog;
// RunUtils / RunUtilsL - script-line format testing :-
// 	2 Copy File
// 	3 Make Dir.
// 	4 Change Dir.
// 	5 Make Read/Write
// 	6 Delete File
// 	11 Syntax error (demonstrate no panic)

// NB! Test Utils in themselves are not test steps, and do NOT run in their own thread

// ------------------------
// RTestMmTsthU1301

RTestMmTsthU1301* RTestMmTsthU1301::NewL()
	{
	RTestMmTsthU1301* self = new(ELeave) RTestMmTsthU1301;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1301::RTestMmTsthU1301()
	{
	iTestStepName = _L("MM-TSTH-U-1301");
	}

// preamble
TVerdict RTestMmTsthU1301::OpenL()
	{
	// stub - purpose is that for this test we do not run the parent preamble
	// which initialises iTestUtils
	return iTestStepResult = EPass;
	}

// postamble
void RTestMmTsthU1301::Close()
	{
	}

// do the test step
TVerdict RTestMmTsthU1301::DoTestStepL()
	{
	// NB CTestUtils does not report errors, but it DOES log them.

	// NB a single run of TestFrameworkMain can only have ONE log file open at the server.
	// Hence we must use that one.

	TVerdict currentVerdict = EPass;

	INFO_PRINTF1(_L("Unit test for TestUtils : Construct"));

	CLog* theLog = iSuite->LogSystem();	// use the current log
	CTestUtils* theTestUtils = NULL;
	TRAPD(err, theTestUtils = CTestUtils::NewL(theLog));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CTestUtils::NewL() failed with error code %d"), err);
		return iTestStepResult = EFail;
		}

	delete theTestUtils;

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------
// RTestMmTsthU1302

RTestMmTsthU1302* RTestMmTsthU1302::NewL()
	{
	RTestMmTsthU1302* self = new(ELeave) RTestMmTsthU1302;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1302::RTestMmTsthU1302()
	{
	iTestStepName = _L("MM-TSTH-U-1302");
	}


// preamble
TVerdict RTestMmTsthU1302::OpenL()
	{
	// do the standard preamble
	TVerdict currentVerdict = RTSUMmTsthStep13::OpenL();
	if(currentVerdict != EPass)
		return currentVerdict;

	// do extra, to set up the files/directories we need, which are :-
	// C:\\TFData1\\testfile1.txt must exist
	// C:\\TFData2 must exist

	RFs theFs;
	theFs.Connect();
	RFile theFile;

	TFileName theDirName  = _L("c:\\TFData1\\");
	TFileName theFileName = _L("c:\\TFData1\\testfile1.txt");
	TInt rc = theFs.MkDir(theDirName);
	if (rc != KErrNone && rc != KErrAlreadyExists)
		{
		ERR_PRINTF2(_L("Preamble failed RFs::MkDir() error code %d"), rc);
		theFs.Close();
		return iTestStepResult = EFail;
		}
	rc = theFile.Replace(theFs, theFileName, EFileWrite | EFileStreamText); 

	// check if open fails 
	if (rc == KErrNone)
		{
		theFile.Write(_L8("This is a test file for MM_TSTH_U_1302\n"));
		theFile.Close();
		}
	else
		{
		ERR_PRINTF2(_L("Preamble failed RFile::Write() error code %d"), rc);
		theFs.Close();
		return iTestStepResult = EFail;
		}

	theDirName  = _L("c:\\TFData2\\");
	rc = theFs.MkDir(theDirName);
	if (rc != KErrNone && rc != KErrAlreadyExists)
		{
		ERR_PRINTF2(_L("Preamble failed RFs::MkDir() error code %d"), rc);
		theFs.Close();
		return iTestStepResult = EFail;
		}

	theFs.Close();
	return iTestStepResult = currentVerdict;
	}

// postamble
void RTestMmTsthU1302::Close()
	{
	// clean up the extra files / directories we created
	CleanupFileSystem();

	// do the standard postamble
	RTSUMmTsthStep13::Close();
	}

// do the test step
TVerdict RTestMmTsthU1302::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestUtils : RunUtils - CopyFile"));

	TVerdict currentVerdict = EPass;

	iTestUtils->RunUtils(_L("run_utils copyfile c:\\TFData1\\testfile1.txt c:\\TFData2\\testfile2.txt"));

	// check that testfile2.txt exists
	TUint dummy;
	RFs theFs;
	theFs.Connect();
	TInt rc = theFs.Att(_L("c:\\TFData2\\testfile2.txt"), dummy);
	if(rc != KErrNone)
		{
		ERR_PRINTF2(_L("CTestUtils : run_utils copyfile failed, error %d"), rc);
		theFs.Close();
		return iTestStepResult = EFail;
		}

	// cleanup
	theFs.Close();
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// --------------------
// RTestMmTsthU1303

RTestMmTsthU1303* RTestMmTsthU1303::NewL()
	{
	RTestMmTsthU1303* self = new(ELeave) RTestMmTsthU1303;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1303::RTestMmTsthU1303()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-TSTH-U-1303");
	}

// preamble
TVerdict RTestMmTsthU1303::OpenL()
	{
	// do the standard preamble
	TVerdict currentVerdict = RTSUMmTsthStep13::OpenL();
	if(currentVerdict != EPass)
		return currentVerdict;

	// do extra, to set up the files/directories we need, which are :-
	// C:\\TFData2 must not exist

	RFs theFs;
	CFileMan* theFm = NULL;
	theFs.Connect();
	TRAPD(err, theFm = CFileMan::NewL(theFs));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("Cannot create CFileMan, error code %d"), err);
		theFs.Close();
		return iTestStepResult = EFail;
		}
	
	TInt rc = theFm->RmDir(_L("c:\\TFData2\\"));
	if (rc != KErrNone && rc != KErrPathNotFound)
		{
		ERR_PRINTF2(_L("Preamble failed CFileMan::RmDir() error code %d"), rc);
		currentVerdict = EFail;
		}

	theFs.Close();
	delete theFm;
	return iTestStepResult = currentVerdict;
	}

// postamble
void RTestMmTsthU1303::Close()
	{
	// clean up the extra files / directories we created
	CleanupFileSystem();

	// do the standard postamble
	RTSUMmTsthStep13::Close();
	}

// do the test step.
TVerdict RTestMmTsthU1303::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestUtils : RunUtils - MkDir"));

	TVerdict currentVerdict = EPass;

	iTestUtils->RunUtils(_L("run_utils mkdir c:\\TFData2\\"));

	// check that TFData2 exists
	TUint dummy;
	RFs theFs;
	theFs.Connect();
	TInt rc = theFs.Att(_L("c:\\TFData2\\"), dummy);
	if(rc != KErrNone)
		{
		ERR_PRINTF2(_L("CTestUtils : run_utils mkdir failed, error %d"), rc);
		theFs.Close();
		return iTestStepResult = EFail;
		}

	theFs.Close();
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// --------------------
// RTestMmTsthU1304
RTestMmTsthU1304* RTestMmTsthU1304::NewL()
	{
	RTestMmTsthU1304* self = new(ELeave) RTestMmTsthU1304;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1304::RTestMmTsthU1304()
	{
	iTestStepName = _L("MM-TSTH-U-1304");
	}

// preamble
TVerdict RTestMmTsthU1304::OpenL()
	{
	// do the standard preamble
	TVerdict currentVerdict = RTSUMmTsthStep13::OpenL();
	if(currentVerdict != EPass)
		return currentVerdict;

	// do extra, to set up the files/directories we need, which are :-
	// C:\\TFData1\\testfile1.txt must exist and be read-only
	
	RFs theFs;
	theFs.Connect();
	RFile theFile;

	TFileName theDirName  = _L("c:\\TFData1\\");
	TFileName theFileName = _L("c:\\TFData1\\testfile1.txt");
	TInt rc = theFs.MkDir(theDirName);
	if (rc != KErrNone && rc != KErrAlreadyExists)
		{
		ERR_PRINTF2(_L("Preamble failed RFs::MkDir() error code %d"), rc);
		theFs.Close();
		return iTestStepResult = EFail;
		}
	rc = theFile.Replace(theFs, theFileName, EFileWrite | EFileStreamText); 

	// check if open fails 
	if (rc == KErrNone)
		{
		theFile.Write(_L8("This is a test file for MM_TSTH_U_1304\n"));
		theFile.Close();
		}
	else
		{
		ERR_PRINTF2(_L("Preamble failed RFile::Replace() error code %d"), rc);
		theFs.Close();
		return iTestStepResult = EFail;
		}

	// set read-only
	rc = theFs.SetAtt(theFileName, KEntryAttReadOnly, 0);

	theFs.Close();
	return iTestStepResult = currentVerdict;
	}

// postamble
void RTestMmTsthU1304::Close()
	{
	// set the file read-write for deletion
	RFs theFs;
	theFs.Connect();
	TFileName theFileName = _L("c:\\TFData1\\testfile1.txt");
	theFs.SetAtt(theFileName, 0, KEntryAttReadOnly);
	theFs.Close();

	// clean up the extra files / directories we created
	CleanupFileSystem();

	// do the standard postamble
	RTSUMmTsthStep13::Close();
	}

// do the test step
TVerdict RTestMmTsthU1304::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestUtils : RunUtils - MakeReadWrite"));

	TVerdict currentVerdict = EPass;

	iTestUtils->RunUtils(_L("run_utils makereadwrite c:\\TFData1\\testfile1.txt"));

	// check that file exists and is read-only
	TUint theAtts;
	RFs theFs;
	theFs.Connect();
	theFs.Att(_L("c:\\TFData1\\testfile1.txt"), theAtts);
	if(theAtts & KEntryAttReadOnly)
		{
		ERR_PRINTF1(_L("CTestUtils : run_utils makereadwrite failed, file is still read-only"));
		theFs.Close();
		return iTestStepResult = EFail;
		}

	theFs.Close();
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// --------------------
// RTestMmTsthU1305
RTestMmTsthU1305* RTestMmTsthU1305::NewL()
	{
	RTestMmTsthU1305* self = new(ELeave) RTestMmTsthU1305;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1305::RTestMmTsthU1305()
	{
	iTestStepName = _L("MM-TSTH-U-1305");
	}

// preamble
TVerdict RTestMmTsthU1305::OpenL()
	{
	// do the standard preamble
	TVerdict currentVerdict = RTSUMmTsthStep13::OpenL();
	if(currentVerdict != EPass)
		return currentVerdict;

	// do extra, to set up the files/directories we need, which are :-
	// C:\\TFData1\\testfile1.txt must exist and be read-write
	
	RFs theFs;
	theFs.Connect();
	RFile theFile;

	TFileName theDirName  = _L("c:\\TFData1\\");
	TFileName theFileName = _L("c:\\TFData1\\testfile1.txt");
	TInt rc = theFs.MkDir(theDirName);
	if (rc != KErrNone && rc != KErrAlreadyExists)
		{
		ERR_PRINTF2(_L("Preamble failed RFs::MkDir() error code %d"), rc);
		theFs.Close();
		return iTestStepResult = EFail;
		}
	rc = theFile.Replace(theFs, theFileName, EFileWrite | EFileStreamText); 

	// check if open fails 
	if (rc == KErrNone)
		{
		theFile.Write(_L8("This is a test file for MM_TSTH_U_1305\n"));
		theFile.Close();
		}
	else
		{
		ERR_PRINTF2(_L("Preamble failed RFile::Replace() error code %d"), rc);
		theFs.Close();
		return iTestStepResult = EFail;
		}

	// set read-write
	rc = theFs.SetAtt(theFileName, 0, KEntryAttReadOnly);

	theFs.Close();
	return iTestStepResult = currentVerdict;
	}

// postamble
void RTestMmTsthU1305::Close()
	{
	// clean up the extra files / directories we created
	CleanupFileSystem();

	// do the standard postamble
	RTSUMmTsthStep13::Close();
	}

// do the test step
TVerdict RTestMmTsthU1305::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestUtils : RunUtils - Delete"));

	TVerdict currentVerdict = EPass;

	iTestUtils->RunUtils(_L("run_utils delete c:\\TFData1\\testfile1.txt"));

	// check that file does not exist
	TUint dummy;
	RFs theFs;
	theFs.Connect();
	TInt rc = theFs.Att(_L("c:\\TFData1\\testfile1.txt"), dummy);
	if(rc != KErrNotFound)
		{
		ERR_PRINTF1(_L("CTestUtils : run_utils delete failed, file still exists"));
		theFs.Close();
		return iTestStepResult = EFail;
		}

	theFs.Close();
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ----------------------
// RTestMmTsthU1306

RTestMmTsthU1306* RTestMmTsthU1306::NewL()
	{
	RTestMmTsthU1306* self = new(ELeave) RTestMmTsthU1306;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1306::RTestMmTsthU1306()
	{
	iTestStepName = _L("MM-TSTH-U-1306");
	}

// preamble
TVerdict RTestMmTsthU1306::OpenL()
	{
	// do the standard preamble
	TVerdict currentVerdict = RTSUMmTsthStep13::OpenL();
	if(currentVerdict != EPass)
		return currentVerdict;

	// do extra, to set up the files/directories we need, which are :-
	// C:\\TFData2 must exist

	RFs theFs;
	theFs.Connect();

	TFileName theDirName  = _L("c:\\TFData2\\");
	TInt rc = theFs.MkDir(theDirName);
	if (rc != KErrNone && rc != KErrAlreadyExists)
		{
		ERR_PRINTF2(_L("Preamble failed RFs::MkDir() error code %d"), rc);
		theFs.Close();
		return iTestStepResult = EFail;
		}

	theFs.Close();
	return iTestStepResult = currentVerdict;
	}

// postamble
void RTestMmTsthU1306::Close()
	{
	// clean up the extra files / directories we created
	CleanupFileSystem();

	// do the standard postamble
	RTSUMmTsthStep13::Close();
	}

// do the test step.
TVerdict RTestMmTsthU1306::DoTestStepL()
	{

	INFO_PRINTF1(_L("This test step is not available on EKA2 - Passing test!"));
	return EPass;
	
	}

// --------------------
// RTestMmTsthU1311

RTestMmTsthU1311* RTestMmTsthU1311::NewL()
	{
	RTestMmTsthU1311* self = new(ELeave) RTestMmTsthU1311;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1311::RTestMmTsthU1311()
	{
	iTestStepName = _L("MM-TSTH-U-1311");
	}

// do the test step
TVerdict RTestMmTsthU1311::DoTestStepL()
	{
	// to demonstrate that bad syntax does not panic
	INFO_PRINTF1(_L("Unit test for TestUtils : RunUtils - bad syntax"));

	TVerdict currentVerdict = EPass;

	iTestUtils->RunUtils(_L("run_utils gobbledygook"));
	INFO_PRINTF1(_L("RunUtils did not panic"));

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// --------------------
