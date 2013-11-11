// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32def.h>
#include <s32mem.h>
#include <barsc2.h>

#include "tcmd_step_base.h"
#include "tcmd_cmdlist.h"

//
// Callback to stop the scheduler
//
TInt StopSchedulerCallBack(TAny* aCCmdTestBase)
	{
	RDebug::Printf("Test Stopping scheduler");
	CCmdTestBase* test = reinterpret_cast<CCmdTestBase*>(aCCmdTestBase);
	test->StopScheduler();
	return KErrNone;
	}


//
// Callback to cancel pending commands
//
TInt CancelCommandCallBack(TAny* aCmdList)
	{
	RDebug::Printf("Test Cancelling command");
	CCmdTestList* test = reinterpret_cast<CCmdTestList*>(aCmdList);
	test->CancelAllCommands();
	return KErrNone;
	}


//
// Constructor
//
CCmdTestBase::CCmdTestBase(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}


//
// Destructor
//
CCmdTestBase::~CCmdTestBase()
	{
	delete iAsyncCancelCommands;
	delete iAsyncStopScheduler;
	delete iActiveScheduler;
	delete iTestList;
	iReader.Close();
	iSsmProcess.Close();
	iSupProcess.Close();
	}


//
// Add Stop Scheduler active object to the scheduler to run when everything else has
// completed, then start the Scheduler
//
void CCmdTestBase::StartScheduler()
	{
	if (iActivateCancel)
		{
		INFO_PRINTF1(_L("Adding cancel command callback"));
		iAsyncCancelCommands->CallBack();
		}

	INFO_PRINTF1(_L("Starting scheduler..."));
	iActiveScheduler->Start();
	}

//
// Stop the scheduler
//
void CCmdTestBase::StopScheduler()
	{
	iActiveScheduler->Halt(KErrNone);
	iActivateCancel = EFalse;
	}

//
// Cancel all commands
// Activate AO to cancel the commands
//
void CCmdTestBase::CreateCancelOnCommands(CCmdTestList* aCmdList)
	{
	INFO_PRINTF1(_L("CCmdTestBase::CreateCancelOnCommands"));
	TCallBack cancel(CancelCommandCallBack, aCmdList);
	iAsyncCancelCommands->Set(cancel);
	iActivateCancel = ETrue;
	}

//
// Enable the stop scheduler callback
//
void CCmdTestBase::EnableStopSchedulerCallback()
	{
	INFO_PRINTF1(_L("Adding stop scheduler callback"));
	iAsyncStopScheduler->CallBack();
	}

//
// Prepare test environment
//
TVerdict CCmdTestBase::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CCmdTestBase::doTestStepPreambleL"));
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	// create AO to stop the scheduler
	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopSchedulerCallBack, this);
	iAsyncStopScheduler->Set(stop);

	// create AO to cancel the commands
	iAsyncCancelCommands = new(ELeave) CAsyncCallBack(CActive::EPriorityStandard + 1);

	return CTestStep::doTestStepPreambleL();
	}


//
// Clean up
//
TVerdict CCmdTestBase::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

//
// helper functions for reading resource files
//
RResourceReader& CCmdTestBase::LoadResourcesL(const TDesC& aRscFileBuffer, TInt aStructId)
	{
	User::LeaveIfError(iFs.Connect());

	// load file
	TEntry entry;
	SSMLOGLEAVEIFERROR(iFs.Entry(aRscFileBuffer, entry));
	TUint fileOffset = 0;
	TInt fileLength = entry.iSize - fileOffset;
	iFile = CResourceFile::NewL(iFs, aRscFileBuffer, fileOffset, fileLength);

	// open iReader on contents
	iReader.OpenL(iFile, aStructId);
	// skip over conditional information
	iReader.ReadInt32L();

	return iReader;
	}

void CCmdTestBase::FreeResources()
	{
	// clean up loaded resources
	iReader.Close();
	delete iFile;
	iFile = NULL;
	iFs.Close();
	}

CResourceFile* CCmdTestBase::ResourceFile()
	{
	return iFile;
	}


//
// Helper functions for StartApp and StartProcess commands
//
/**
Helper function to find and kill the specified process
*/
TInt CCmdTestBase::FindAndKill(const TDesC& aProcessName)
	{
	TFullName searchTerm(aProcessName);
	StripExtension(searchTerm);
	searchTerm += _L("*");
	TFindProcess find(searchTerm);
	TFullName name;
	TInt instancesFound = 0;
	while(find.Next(name) == KErrNone)
		{
		RProcess process;
		const TInt err = process.Open(find);

		if (KErrNone == err)
			{
			if (process.ExitType() == EExitPending)
				{
				instancesFound++;
				process.Kill(KErrCancel);
				INFO_PRINTF3(_L("Process %S found and killed with Exit Reason = %d"), &aProcessName, process.ExitReason());
				}
			process.Close();
			}
		}
	INFO_PRINTF2(_L("No of instances of process killed = %d"), instancesFound);
	return instancesFound;
	}

/**
Helper function to strip extension
*/
void CCmdTestBase::StripExtension(TDes& aFilename)
	{
	TInt dot = aFilename.Find(_L("."));

	if(KErrNotFound != dot)
		{
		aFilename.SetLength(dot);
		}
	}

