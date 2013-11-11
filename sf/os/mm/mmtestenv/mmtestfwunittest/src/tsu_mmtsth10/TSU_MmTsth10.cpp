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
// This file contains the test steps for Unit Test Suite 10 : Log.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TSU_MmTsthSuite10.h"

// Specific includes for these test steps
#include "TSU_MmTsth10.h"

// --------------------------------------------

// Unit Test Suite 10 : Log.cpp
// Depends on : none

// Tests :-
// 1 RTestFrameworkClientSession : check connect
// 11 Log : create
// 12 Log : open log file (existing)
// 13 Log : write format - no test specified as this is called every time the INFO_PRINTF or ERR_PRINTF
//			macro is called by the framework or suite(s)
// 14 Log : status		(incorporated in test 12)

// ---------------------
// RTestMmTsthU1001

RTestMmTsthU1001* RTestMmTsthU1001::NewL()
	{
	RTestMmTsthU1001* self = new(ELeave) RTestMmTsthU1001;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1001::RTestMmTsthU1001()
	{
	iTestStepName = _L("MM-TSTH-U-1001");
	}

// Do the test step.
TVerdict RTestMmTsthU1001::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for RTestFrameworkServerSession"));

	// Check we can connect to the existing server.
	// NB - open, write, close, status will be called by Log tests

	TVerdict currentVerdict = EPass;

	TInt ret;
	RTestFrameworkClientSession testSession;
	ret = testSession.Connect();
	if(ret != KErrNone)
	{
		ERR_PRINTF2(_L("Could not connect to server, error %d"), ret);
		return iTestStepResult = EFail;
	}

	INFO_PRINTF1(_L("Successfully connected to server with a new session"));

	testSession.Close(); // cleanup session
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ---------------------
// RTestMmTsthU1011

RTestMmTsthU1011* RTestMmTsthU1011::NewL()
	{
	RTestMmTsthU1011* self = new(ELeave) RTestMmTsthU1011;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1011::RTestMmTsthU1011()
	{
	iTestStepName = _L("MM-TSTH-U-1011");
	}

// Do the test step.
TVerdict RTestMmTsthU1011::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for Log : create"));

	TVerdict currentVerdict = EPass;

	CLog* theLog = NULL;
	TRAPD(err, theLog = CLog::NewL());
	if(err != KErrNone)
	{
		ERR_PRINTF2(_L("CLog::NewL() left, error code %d"), err);
		return iTestStepResult = EFail;
	}

	INFO_PRINTF1(_L("CLog created successfully"));
	delete theLog;
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ---------------------
// RTestMmTsthU1012

RTestMmTsthU1012* RTestMmTsthU1012::NewL()
	{
	RTestMmTsthU1012* self = new(ELeave) RTestMmTsthU1012;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1012::RTestMmTsthU1012()
	{
	iTestStepName = _L("MM-TSTH-U-1012");
	}

// Do the test step.
TVerdict RTestMmTsthU1012::DoTestStepL()
{
	INFO_PRINTF1(_L("Unit test for Log : open"));

	TVerdict currentVerdict = EPass;

	CLog* theLog = NULL;
	TRAPD(err, theLog = CLog::NewL());
	if(err != KErrNone)
	{
		ERR_PRINTF2(_L("CLog::NewL() left, error code %d"), err);
		return iTestStepResult = EInconclusive;
	}

	// Open the current logfile (it's already open at the server)
	TRAP(err, theLog->OpenLogFileL());
	if(err != KErrNone)
	{
		ERR_PRINTF2(_L("CLog::OpenLogFileL() left, error code %d"), err);
		delete theLog;
		return iTestStepResult = EFail;
	}

	// Attempt to open a new log file. This will complete without opening the log file and
	// without disturbing the server; we can
	// ascertain this by checking the log status, which will be unchanged.
	TInt theStatus = theLog->LogStatus();
	delete theLog;
	theLog = NULL;
	
	_LIT(KDummyLogName, "DummyLog");
	TRAP(err, theLog = CLog::NewL());
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CLog::NewL() left, error code %d"), err);
		return iTestStepResult = EInconclusive;
		}
	
	TRAP(err, theLog->OpenLogFileL(KDummyLogName, theStatus - 1));
	if(err != KErrNone)
	{
		ERR_PRINTF2(_L("CLog::OpenLogFileL(KDummyLogName) left, error code %d"), err);
		delete theLog;
		return iTestStepResult = EFail;
	}
	
	TInt theNewStatus = theLog->LogStatus();
	if(theNewStatus != theStatus)
	{
		ERR_PRINTF1(_L("CLog::OpenLogFileL(KDummyLogName) changed log status!"));
		delete theLog;
		return iTestStepResult = EFail;
	}

	// NB no test for CloseLogFileL - as this acts at the server and is something we only ever
	// want to do at the end of the TestFramework run

	INFO_PRINTF1(_L("CLog opened existing log file successfully"));
	delete theLog;
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ---------------------
// RTestMmTsthU1013

RTestMmTsthU1013* RTestMmTsthU1013::NewL()
	{
	RTestMmTsthU1013* self = new(ELeave) RTestMmTsthU1013;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU1013::RTestMmTsthU1013()
	{
	iTestStepName = _L("MM-TSTH-U-1013");
	}

// Do the test step.
TVerdict RTestMmTsthU1013::DoTestStepL()
{
	INFO_PRINTF1(_L("Unit test for Log : write format"));

	TVerdict currentVerdict = EPass;

	// any call to LogExtra will test this. if it doesn't crash, consider it passed

	INFO_PRINTF1(_L("Testing WriteFormat..."));

	return iTestStepResult = currentVerdict;
	}

