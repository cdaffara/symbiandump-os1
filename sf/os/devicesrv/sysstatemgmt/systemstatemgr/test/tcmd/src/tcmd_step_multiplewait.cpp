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

#include <ssm/ssmcommandlist.h>
#include <barsread2.h>
#include <s32mem.h>
#include <ssm/ssmstartupproperties.h>
#include "tcmd_step_multiplewait.h"
#include "cmdstartapp.h"
#include "cmdstartprocess.h"
#include "cmdmultiplewait.h"
#include "ssmcommandparameters.h"
#include "tcmd_cmdlist.h"
#include <ssmcmd_multiplewait.rsg>

_LIT(KTestRscFileName, "z:\\resource\\ssmatest\\bic\\ssmcmd_multiplewait.rsc");

_LIT(KFileName, "c:\\badfile.exe");
_LIT(KEmptyArgs, "");

const TInt KNumOfRetries = 5;
const TInt KTestTimeOut = 1000;

CCmdTestMultipleWait::~CCmdTestMultipleWait()
	{
	}

CCmdTestMultipleWait::CCmdTestMultipleWait()
	: CCmdTestBase(KTCCmdTestMultipleWait)
	{
	}

//
// Run the tests
//
TVerdict CCmdTestMultipleWait::doTestStepL()
	{
	CreateDummyDeferredListL();
	SsmCommandFactory::DelegateTo(this);
	// common BIC tests
	__UHEAP_MARK;
	TRAPD(err, doTestNewLWithGoodDataL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestNewLWithBadDataL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestInternalizeLWithGoodDataL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestInternalizeLWithBadDataL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestResourceReaderWithGoodDataL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestResourceReaderWithMissingDataL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestResourceReaderWithBadDataL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestCommandTypeL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	
	__UHEAP_MARK;
	TRAP(err, doTestCmdFailureIgnoreSeverityL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestCmdFailureHighSeverityL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestCancelCmdIgnoreSeverityL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestCancelCmdHighSeverityL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	return TestStepResult();
	}
/**
Old testcaseID 			AFSS-CMDMWT-0001
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/

void CCmdTestMultipleWait::doTestNewLWithGoodDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	TInt32 timeout = 10000;
	TArray<MSsmCommand*> testDeferredList = iDeferredList.Array();
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredCommands)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(testDeferredList, timeout));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, timeout);
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TInt32 aTimeout)"));

	TRAP(err, iTestCmd = CCmdMultipleWait::NewL(timeout));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, timeout);
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	timeout = 1000;
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdMultipleWait::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, timeout);
	Reset();
	FreeResources();
	}
/**
Old testcaseID 			AFSS-CMDMWT-0002
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/

void CCmdTestMultipleWait::doTestNewLWithBadDataL()
	{
	RPointerArray<MSsmCommand> emptyDeferredList;
	TArray<MSsmCommand*> testDeferredList = emptyDeferredList.Array();
	TInt err = KErrNone;
	// If empty deferred list is passed then command should return KErrNone
	TInt expectedErr = KErrNone;
	TInt32 timeout = 10000;
	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - Command construction with in-valid data"));

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredCommands)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(testDeferredList, timeout));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(TInt32 aTimeout)"));

	}

/**
Old testcaseID 			AFSS-CMDMWT-0003
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/

void CCmdTestMultipleWait::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));
	}

/**
Old testcaseID 			AFSS-CMDMWT-0004
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/
void CCmdTestMultipleWait::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL - InternalizeL with in valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:01 **** "));
	}
/**
Old testcaseID 			AFSS-CMDMWT-0005
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/

void CCmdTestMultipleWait::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}
/**
Old testcaseID 			AFSS-CMDMWT-0006
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/

void CCmdTestMultipleWait::doTestResourceReaderWithMissingDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - construct from resource with invalid reader"));
	TInt expectedErr = KErrNotSupported;
	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	//read some data so reader will become invalid
	reader.ReadUint16L();
	TSsmCommandParameters params(reader, ResourceFile(), 0);
	TRAPD(err, iTestCmd = CCmdMultipleWait::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();
	FreeResources();
	}
/**
Old testcaseID 			AFSS-CMDMWT-0007
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/
void CCmdTestMultipleWait::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - Construct from resource with valid reader and invalid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:05 & doTestNewLWithBadDataL:06 **** "));
	}
/**
Old testcaseID 			AFSS-CMDMWT-0008
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/
void CCmdTestMultipleWait::doTestCommandTypeL()
	{
	TInt32 timeout = 10000;
	TArray<MSsmCommand*> testDeferredList = iDeferredList.Array();
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestCommandTypeL:01 Test: Construct from NewL(RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredCommands) and check type"));
	iTestCmd = CreateCmdFromStreamL(testDeferredList, timeout);
	CompareCommandsDataL(iTestCmd, timeout);
	TEST(iTestCmd->Type() == ESsmCmdMultipleWait);
	Reset();

	INFO_PRINTF1(_L("doTestCommandTypeL:02 Test: Construct from NewL(TInt32 aTimeout) and check type"));
	iTestCmd = CCmdMultipleWait::NewL(timeout);
	CompareCommandsDataL(iTestCmd, timeout);
	TEST(iTestCmd->Type() == ESsmCmdMultipleWait);
	Reset();

	INFO_PRINTF1(_L("doTestCommandTypeL:03 Test: Construct from NewL(TSsmCommandParameters& aCommandParameters) and check type"));
	timeout = 1000;
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, NULL, 0);
	iTestCmd = CCmdMultipleWait::NewL(params);
	CompareCommandsDataL(iTestCmd, timeout);
	TEST(iTestCmd->Type() == ESsmCmdMultipleWait);
	Reset();
	FreeResources();
	}
/**
Old testcaseID 			AFSS-CMDMWT-0009
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/

void CCmdTestMultipleWait::doTestCmdFailureIgnoreSeverityL()
	{
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:01 Test: Add one deferred command with severity ECmdIgnoreFailure and run multiple wait"));
	// add a deferred command with ECmdIgnoreFailure Severity 
	// add multiple wait command
	// command should finish with KErrNone
	iDeferredCmds.Append(TDeferredCmdConfiguration(ESsmCmdStartApp, R_DEFERRED_COMMAND1));
	CreateCmdAndExecuteL(iDeferredCmds.Array(), expectedErr, EFalse, KTestTimeOut);
	}
/**
Old testcaseID 			AFSS-CMDMWT-0010
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/
void CCmdTestMultipleWait::doTestCmdFailureHighSeverityL()
	{
	TInt expectedErr = KErrNotFound;
	INFO_PRINTF1(_L("doTestCmdFailureHighSeverityL:01 Test: Add one deferred command with severity other then ECmdIgnoreFailure and run multiple wait"));
	// add a deferred command with Severity other then ECmdIgnoreFailure
	// add multiple wait command
	// command should finish with KErrNotFound as deferred command returns with KErrNotFound
	iDeferredCmds.Append(TDeferredCmdConfiguration(ESsmCmdStartApp, R_DEFERRED_COMMAND2));
	CreateCmdAndExecuteL(iDeferredCmds.Array(), expectedErr, EFalse, KTestTimeOut);
	}
/**
Old testcaseID 			AFSS-CMDMWT-0011
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/
void CCmdTestMultipleWait::doTestCancelCmdIgnoreSeverityL()
	{
	TInt expectedErr = KErrCancel;
	INFO_PRINTF1(_L("doTestCancelCmdIgnoreSeverityL:01 Test: Add one deferred command with severity with severity ECmdIgnoreFailure and run multiple wait and cancel"));
	// add a deferred command with ECmdIgnoreFailure Severity 
	// add multiple wait command
	// cancel the command 
	// command should finish with KErrCancel
	iDeferredCmds.Append(TDeferredCmdConfiguration(ESsmCmdStartApp, R_DEFERRED_COMMAND1));
	CreateCmdAndExecuteL(iDeferredCmds.Array(), expectedErr, ETrue, KTestTimeOut);
	}
/**
Old testcaseID 			AFSS-CMDMWT-0012
New testcaseID			DEVSRVS-SSMA-CMD-0010
*/

void CCmdTestMultipleWait::doTestCancelCmdHighSeverityL()
	{
	TInt expectedErr = KErrCancel;
	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:01 Test: Add one deferred command with severity other then ECmdIgnoreFailure and run multiple wait and cancel"));
	// add a deferred command with other then ECmdIgnoreFailure Severity 
	// add multiple wait command
	// cancel the command
	// command should finish with KErrCancel
	iDeferredCmds.Append(TDeferredCmdConfiguration(ESsmCmdStartApp, R_DEFERRED_COMMAND2));
	CreateCmdAndExecuteL(iDeferredCmds.Array(), expectedErr, ETrue, KTestTimeOut);
	}

/**
Helper functiosn to compare the commands data.
*/
void CCmdTestMultipleWait::CompareCommandsDataL(CCmdMultipleWait* aTestCmd, TInt32 aTimeout)
	{
	// create output buffer and stream
 	CBufFlat* outputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(outputBuffer);
	RBufWriteStream writeStream(*outputBuffer);
	CleanupClosePushL(writeStream);

	// externalise the data
	aTestCmd->ExternalizeL(writeStream);
	RDesReadStream readStream(outputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	TInt32 timeout = readStream.ReadInt32L();
	TEST(timeout == aTimeout);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(outputBuffer);
	}

/**
Helper functiosn to create a commands from a read stream.
*/
CCmdMultipleWait* CCmdTestMultipleWait::CreateCmdFromStreamL(TArray<MSsmCommand*> aDeferredList, TInt32 aTimeout)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteUint32L(aTimeout);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(KDefaultCommandPriority);
#endif //SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdMultipleWait* newCmd = CCmdMultipleWait::NewL(readStream, aDeferredList);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);

	return newCmd;
	}

/**
Resets data
*/
void CCmdTestMultipleWait::Reset()
	{
	delete iTestCmd;
	iTestCmd = NULL;
	delete iTestList;
	iTestList = NULL;
	iDeferredCmds.Reset();
	}

/**
Helper function to create dummy deferred list.
*/
void CCmdTestMultipleWait::CreateDummyDeferredListL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	CSsmStartupProperties* appInfo = CSsmStartupProperties::NewL();
	CleanupStack::PushL(appInfo);
	appInfo->Reset();
	appInfo->SetFileParamsL(KFileName, KEmptyArgs);
	appInfo->SetCommandTypeL(ESsmCmdStartApp);
	appInfo->SetExecutionBehaviour(ESsmDeferredWaitForSignal);
	appInfo->SetRetries(KNumOfRetries);
	appInfo->SetTimeout(KTestTimeOut);
	
	CSsmCommandBase* cmd1 = CCmdStartApp::NewL(severity, appInfo);
	CleanupStack::PushL(cmd1);
	appInfo->Reset();
	appInfo->SetFileParamsL(KFileName, KEmptyArgs);
	appInfo->SetCommandTypeL(ESsmCmdStartProcess);
	appInfo->SetExecutionBehaviour(ESsmDeferredWaitForSignal);
	appInfo->SetRetries(KNumOfRetries);
	appInfo->SetTimeout(KTestTimeOut);
	CSsmCommandBase* cmd2 = CCmdStartProcess::NewL(severity, appInfo);
	CleanupStack::PushL(cmd2);
	appInfo->Reset();	

	iDeferredList.Append(cmd1);
	iDeferredList.Append(cmd2);

	CleanupStack::Pop(cmd2);
	CleanupStack::Pop(cmd1);
	CleanupStack::PopAndDestroy(appInfo);
	}

CSsmCommandBase* CCmdTestMultipleWait::ConstructCommandFromResourceLC(TSsmCommandType aType, TSsmCommandParameters& aCommandParameters)
	{
	CSsmCommandBase* cmd = NULL;
	
	switch (aType)
		{
		case ESsmCmdStartApp:
			{
			cmd = CCmdStartApp::NewL(aCommandParameters);
			break;
			}
		case ESsmCmdStartProcess:
			{
			cmd = CCmdStartProcess::NewL(aCommandParameters);
			break;
			}
		}
	CleanupStack::PushL(cmd);
	return cmd;
	}


/**
Helper function to run a command and test the expected results
*/
void CCmdTestMultipleWait::CreateCmdAndExecuteL(TArray<TDeferredCmdConfiguration> aDeferredCmds, TInt aExpectedErr, TBool aCancel, TInt32 aMultipleWaitTimeout)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	// add all deferred commands
	TInt count = aDeferredCmds.Count();
	for (TInt i = 0; i < count; i++)
		{
		AddDeferredCommandsInListL(aDeferredCmds[i]);
		}
	iTestList->AddMultipleWaitCommandL(aMultipleWaitTimeout);

	if (aCancel)
		{
		CreateCancelOnCommands(iTestList);
		}
	StartScheduler();
	TInt err = iTestList->Errorcode();
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, aExpectedErr);
	TEST(err == aExpectedErr);
	Reset();
	}

void CCmdTestMultipleWait::AddDeferredCommandsInListL(TDeferredCmdConfiguration aDeferredCmd)
	{
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, aDeferredCmd.iCmdResourceId);
	TSsmCommandParameters params(reader, NULL, 0);
	iTestList->AddCommandFromResourceL(aDeferredCmd.iCmdType, params);
	FreeResources();
	}
