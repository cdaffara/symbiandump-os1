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
// This file contains the test steps for Unit Test Suite 22 : LogFile.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TSU_MmTsthSuite22.h"

// Specific includes for these test steps
#include "TSU_MmTsth22.h"
#include "TestFrameworkServer/LogFile.h"

// --------------------------------------------

// Unit Test Suite 22 : LogFile.cpp
// Depends on : None

// Tests :-

// Create a file log
// Open an existing file log
// Write to an open file log
// Close a file log
// Open and close a file server session


RTestMmTsthU2201* RTestMmTsthU2201::NewL()
	{
	RTestMmTsthU2201* self = new(ELeave) RTestMmTsthU2201;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU2201::RTestMmTsthU2201()
	{
	iTestStepName = _L("MM-TSTH-U-2201");
	}

// preamble
TVerdict RTestMmTsthU2201::OpenL()
	{
	// ensure the directory for our test log exists

	// parse the filenames
	_LIT(KDefault,"C:\\"); 
	_LIT(KLogPath, "C:\\Logs\\TestResults\\TestFrameworkTemp\\");
	TParse fullFileName;
	TInt returnCode = fullFileName.Set(KLogPath, &KDefault, NULL);
	if (returnCode != KErrNone)
		{
		ERR_PRINTF3(_L("Preamble : failed to set file name %S, error %d"), 
			&fullFileName.FullName(), returnCode);
		return iTestStepResult = EFail;
		}

	RFs fileSystem;
	returnCode=fileSystem.Connect();
	if (returnCode != KErrNone)
		{
		ERR_PRINTF2(_L("Preamble : failed to connect to file server, error %d"), 
			returnCode);
		fileSystem.Close();
		return iTestStepResult = EFail;
		}

	returnCode = fileSystem.MkDir(fullFileName.DriveAndPath()); 
	if (returnCode != KErrNone && returnCode != KErrAlreadyExists)
		{
		ERR_PRINTF3(_L("Preamble : failed to make directory %S, error %d"), 
			&fullFileName.FullName(), returnCode);
		fileSystem.Close();
		return iTestStepResult = EFail;
		}

	fileSystem.Close();
	return iTestStepResult = EPass;
	}

// postamble
void RTestMmTsthU2201::Close()
	{
	// clean up the test directory we created in the preamble

	// parse the filenames
	_LIT(KDefault,"C:\\"); 
	_LIT(KLogPath, "C:\\Logs\\TestResults\\TestFrameworkTemp\\");
	TParse fullFileName;
	TInt returnCode = fullFileName.Set(KLogPath, &KDefault, NULL);
	if (returnCode != KErrNone)
		{
		ERR_PRINTF3(_L("RTestMmTsthU2201::Close() failed to set file name %S, error %d"), 
			&fullFileName.FullName(), returnCode);
		return;
		}

	RFs fileSystem;
	returnCode=fileSystem.Connect();
	if (returnCode != KErrNone)
		{
		ERR_PRINTF2(_L("RTestMmTsthU2201::Close() failed to connect to file server, error %d"), 
			returnCode);
		fileSystem.Close();
		return;
		}

	returnCode = fileSystem.RmDir(fullFileName.DriveAndPath()); 
	if (returnCode != KErrNone)
		{
		ERR_PRINTF3(_L("RTestMmTsthU2201::Close() failed to remove directory %S, error %d"), 
			&fullFileName.FullName(), returnCode);
		}

	fileSystem.Close();
	}

// do the test step
TVerdict RTestMmTsthU2201::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for CFileLogger"));

	TVerdict currentVerdict = EPass;

	// create a CFileLogger
	CFileLogger* theFileLogger = new CFileLogger();
	if(!theFileLogger)
	{
		ERR_PRINTF1(_L("Failed to create a CFileLogger"));
		return iTestStepResult = EFail;
	}

	// directory path as created in the Preamble
	_LIT(KLogName, "CFileLoggerTest");
	_LIT(KLogPath, "C:\\Logs\\TestResults\\TestFrameworkTemp");
	_LIT(KDefault, "C:\\.htm"); 

	// ensure the path exists
	TParse parseLogName;
	parseLogName.Set(KLogName, NULL, NULL); 

	TFileName logFilePath;
	logFilePath = KLogPath;

	if(parseLogName.PathPresent())
		logFilePath.Append(parseLogName.Path());
	else
		logFilePath.Append(_L("\\"));

	// overwrite extension if supplied with .htm
	TParse logFileFullName;
	TInt returnCode = logFileFullName.Set(KDefault, &logFilePath, &KLogName());
	if (returnCode == KErrNone)
		{
		TInt ret = theFileLogger->Connect();
		if (ret == KErrNone)
			{
			theFileLogger->CreateLog(logFilePath, logFileFullName.NameAndExt());
			}
		else
			{
			ERR_PRINTF1(_L("CFileLogger could not connect to file server"));
			delete theFileLogger;
			return iTestStepResult = EFail;
			}
		}

	_LIT(KFileLogTestString, "CFileLoggerTest Verification");
	_LIT8(KFileLogTestString8, "CFileLoggerTest Verification");

	// write to it
	theFileLogger->WriteLog(KFileLogTestString);

	// now read it back
	RFs fileSystem;
	returnCode=fileSystem.Connect();
	if (returnCode != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to connect to file server, error %d"), 
			returnCode);
		theFileLogger->Close();
		delete theFileLogger;
		return iTestStepResult = EFail;
		}

	TFileName logFile;
	logFile.Format(_L("%S\\%S.htm"), &KLogPath(), &KLogName());
	RFile theFile;
	returnCode = theFile.Open(fileSystem, logFile, EFileRead | EFileStreamText | EFileShareAny);

	// NB :- We're reading in 8-bit, so need to compare against an 8-bit string literal
	TInt fileSize;
	returnCode = theFile.Size(fileSize);
	TPtr8 theString(REINTERPRET_CAST(TUint8*,User::AllocLC(fileSize)), 0, fileSize);
	theFile.Read(theString, fileSize);

	returnCode = theString.Find(KFileLogTestString8());

	// see if our string is present. if it is, we've passed.
	if(returnCode == KErrNotFound)
		{
		ERR_PRINTF1(_L("Test string not found in test log file!"));
		currentVerdict = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Read string %S found in test file"), &KFileLogTestString());
		}

	// close file logger session
	theFileLogger->Close();
	theFile.Close();
	// clean up our file
	fileSystem.Delete(logFile);
	fileSystem.Close();
	delete(theFileLogger);
	CleanupStack::PopAndDestroy();	// theString

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

