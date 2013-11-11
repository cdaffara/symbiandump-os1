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
// This file contains the test steps for Unit Test Suite 20 : TestFrameworkServer.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TSU_MmTsthSuite20.h"

// Specific includes for these test steps
#include "TSU_MmTsth20.h"
#include "TestFrameworkServer/TestFrameworkServer.h"

// --------------------------------------------

// Unit Test Suite 20 : TestFrameworkServer.cpp
// Depends on : TestFrameworkClient (running)
// Needs : LogFile.cpp, ServerConsole.cpp

// Tests :-

//	1. verify that a CTestFrameworkServer is running
//	2. verify that a CTestFrameworkServerSession is running

// (CTestFrameworkServerShutdown is a private utility class)
// (TWindow is private and obsolete; will be removed)

// NB : to test within the TestFramework, we can't open a new server (it's already
// running) - nor can we access the private API's of Server and ServerSession
// The tests will validate that the server and session are already running.


// --------------------------------------------
// RTestMmTsthU2001

RTestMmTsthU2001* RTestMmTsthU2001::NewL()
	{
	RTestMmTsthU2001* self = new(ELeave) RTestMmTsthU2001;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU2001::RTestMmTsthU2001()
	{
	iTestStepName = _L("MM-TSTH-U-2001");
	}

// Do the test step.
TVerdict RTestMmTsthU2001::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestFrameworkServer - Server"));

	// we can't construct a server - it's already running. Attempts to construct
	// again will leave.
	// Test :- Check it's running, by trying to construct it again. Trap the leave.

	TVerdict currentVerdict = EPass;

	// create and install the active scheduler we need
	CActiveScheduler* theScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(theScheduler);
	CActiveScheduler::Install(theScheduler);
	//

	CMmfIpcServer* theServer = NULL;
	TRAPD(err, theServer = CTestFrameworkServer::NewL());
	if(err != KErrAlreadyExists)
		{
		TPtrC errortxt = CLog::EpocErrorToText(err);
		ERR_PRINTF2(_L("Server not already running, create returned %S"), &errortxt);
		delete theServer;
		currentVerdict = EFail;
		}
	else
		{
		TPtrC errortxt = CLog::EpocErrorToText(err);
		INFO_PRINTF2(_L("Server already running, create returned %S"), &errortxt);
		}

	// Cleanup the scheduler
	CleanupStack::PopAndDestroy(theScheduler);

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------
// RTestMmTsthU2002

RTestMmTsthU2002* RTestMmTsthU2002::NewL()
	{
	RTestMmTsthU2002* self = new(ELeave) RTestMmTsthU2002;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU2002::RTestMmTsthU2002()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-TSTH-U-2002");
	}

// Do the test step.
TVerdict RTestMmTsthU2002::DoTestStepL()
	{
	INFO_PRINTF1(_L("Unit test for TestFrameworkServer - Server Session"));

	TVerdict currentVerdict = EPass;

	// create and install the active scheduler we need
	CActiveScheduler* theScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(theScheduler);
	CActiveScheduler::Install(theScheduler);
	//

	CMmfIpcServer* theServer = NULL;
	TRAPD(err, theServer = CTestFrameworkServer::NewL());
	if(err != KErrAlreadyExists)
		{
		TPtrC errortxt = CLog::EpocErrorToText(err);
		ERR_PRINTF2(_L("Server not already running, create returned %S"), &errortxt);
		delete theServer;
		CleanupStack::PopAndDestroy(theScheduler);
		return iTestStepResult = EInconclusive;
		}

	// setup local logger - this will cause a server session to be created
	// (we can't get a handle to it!)
	CLog* logClient	= CLog::NewLC();
	logClient->OpenLogFileL();

	TInt status = logClient->LogStatus();
	// this will have retrieved log status from the server - the value is dependent on
	// params passed into TestFramework, but in all cases will be nonzero. this demonstrates
	// that a server session is running
	if(status == 0)
		{
		ERR_PRINTF1(_L("Log status is zero - server session may not be running"));
		currentVerdict = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Log status %d retrieved from server session"), status);
		currentVerdict = EPass;
		}

	// cleanup the logger and the scheduler
	CleanupStack::PopAndDestroy(2); // logClient, theScheduler

	return iTestStepResult = currentVerdict;
	}
