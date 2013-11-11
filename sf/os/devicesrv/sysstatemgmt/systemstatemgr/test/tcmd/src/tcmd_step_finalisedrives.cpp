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
#include <f32file.h>
#include "cmdfinalisedrives.h"
#include "tcmd_step_finalisedrives.h"
#include "tcmd_cmdlist.h"
#include "ssmcommandparameters.h"
#include <ssmcmd_finalisedrives.rsg>

_LIT(KRscFileBuffer, "z:\\resource\\ssmatest\\bic\\ssmcmd_finalisedrives.rsc");

CCmdTestFinaliseDrives::CCmdTestFinaliseDrives()
	: CCmdTestBase(KTCCmdTestFinaliseDrives)
	{
	}

CCmdTestFinaliseDrives::~CCmdTestFinaliseDrives()
	{
	}


TVerdict CCmdTestFinaliseDrives::doTestStepL()
	{
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

	__UHEAP_MARK;
	TRAP(err, doTestNormalExecutionL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestNewLWithGoodDataL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdFinaliseDrives::NewL(severity));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_FINALISE_DRIVES1);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdFinaliseDrives::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestNewLWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNotSupported;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_FINALISE_DRIVES2);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdFinaliseDrives::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_FINALISE_DRIVES3);
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdFinaliseDrives::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with invalid data"));
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/


void CCmdTestFinaliseDrives::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestResourceReaderWithMissingDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNotSupported;

	INFO_PRINTF1(_L("doTestResourceReaderWithMissingDataL:01 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_FINALISE_DRIVES2);
	TSsmCommandParameters params(reader, NULL, 0);
	// read some of the data so there will not be enough to read
	reader.ReadInt32L();
	TRAP(err, iTestCmd = CCmdFinaliseDrives::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - construct from resource with invalid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:01 & 02 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestCommandTypeL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestCommandTypeL:01 Construct from NewL(RReadStream& aReadStream) and check type"));
	iTestCmd = CreateCmdFromStreamL(severity);
	CompareCommandsDataL(iTestCmd, severity);
	TEST(iTestCmd->Type() == ESsmCmdFinaliseDrives);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:02 Construct from NewL(TCmdErrorSeverity aSeverity) and check type"));
	severity = ECmdCriticalSeverity;
	iTestCmd = CCmdFinaliseDrives::NewL(severity);
	CompareCommandsDataL(iTestCmd, severity);
	TEST(iTestCmd->Type() == ESsmCmdFinaliseDrives);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:03 Construct from NewL(TSsmCommandParameters& aCommandParameters) and check type"));
	severity = ECmdMediumSeverity;
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_FINALISE_DRIVES1);
	TSsmCommandParameters params(reader, NULL, 0);
	iTestCmd = CCmdFinaliseDrives::NewL(params);
	CompareCommandsDataL(iTestCmd, severity);		//validate the value
	TEST(iTestCmd->Type() == ESsmCmdFinaliseDrives);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestCmdFailureIgnoreSeverityL()
	{
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestCmdFailureHighSeverityL()
	{
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestCancelCmdIgnoreSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdIgnoreFailure Severity and cancel"));

	INFO_PRINTF1(_L("doTestCancelCmdIgnoreSeverityL:01 Test: Run a command and cancel"));
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestCancelCmdHighSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdHighSeverity Severity and cancel"));

	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:01 Test: Run a command and cancel"));
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);
	}

/**
Old Test CaseID 		AFSS-CMDFDR-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0007
*/

void CCmdTestFinaliseDrives::doTestNormalExecutionL()
	{
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdHighSeverity Severity and cancel"));

	INFO_PRINTF1(_L("doTestNormalExecutionL:01 Test: Run command normally"));
	CreateCmdAndExecuteL(severity, expectedErr, EFalse);
	}

/**
Helper function to create instance of CCmdFinaliseDrives trough readstream.
*/
CCmdFinaliseDrives* CCmdTestFinaliseDrives::CreateCmdFromStreamL(TCmdErrorSeverity aSeverity)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdFinaliseDrives* newCmd = CCmdFinaliseDrives::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	
	return newCmd;
	}

/**
Helper function to compare the data of the command.
*/
void CCmdTestFinaliseDrives::CompareCommandsDataL(CCmdFinaliseDrives* aTestCmd, TCmdErrorSeverity aSeverity)
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

	TCmdErrorSeverity severity = static_cast<TCmdErrorSeverity>(readStream.ReadInt16L());
	TEST(aSeverity == severity);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(outputBuffer);
	}

/**
Helper function to run a command and test the expected results
*/
void CCmdTestFinaliseDrives::CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TInt aExpectedErr, TBool aCancel)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddFinaliseDrivesCommandL(aSeverity);
	if (aCancel)
		{
		CreateCancelOnCommands(iTestList);
		}
	StartScheduler();
	TInt err = iTestList->Errorcode();
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, aExpectedErr);
	TEST(err == aExpectedErr);
	delete iTestList;
	iTestList = NULL;
	}

