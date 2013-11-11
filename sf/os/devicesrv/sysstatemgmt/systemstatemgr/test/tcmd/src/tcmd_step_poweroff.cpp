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

#include <ssmcmd_poweroff1.rsg>

#include "cmdpoweroff.h"
#include "tcmd_step_poweroff.h"
#include "ssmcommandparameters.h"
#include "ssmcommandfactory.h"
#include "tcmd_cmdlist.h"

_LIT(KRscFileBuffer, "z:\\resource\\ssmatest\\bic\\ssmcmd_poweroff1.rsc");

CCmdTestPowerOff::CCmdTestPowerOff()
	: CCmdTestBase(KTCCmdTestPowerOff)
	{
	}


CCmdTestPowerOff::~CCmdTestPowerOff()
	{
	}


TVerdict CCmdTestPowerOff::doTestStepL()
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
Old Test CaseID 		AFSS-CMDPOF-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestNewLWithGoodDataL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	TPowerState powerEvent = EPwActive;
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, powerEvent));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, powerEvent);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(severity, powerEvent));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, powerEvent);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	powerEvent = EPwStandby;
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_POWER_OFF1);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, powerEvent);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestNewLWithBadDataL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt err = KErrNone;
	TInt expectedErr = KErrArgument;
	//test with negative value of power event
	TPowerState powerEvent = static_cast<TPowerState>(-1);
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with invalid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, powerEvent));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(severity, powerEvent));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_POWER_OFF4);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	//test with negative out of bound value of power event
	powerEvent = static_cast<TPowerState>(99);
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, powerEvent));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:05 Test: NewL(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(severity, powerEvent));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_POWER_OFF5);
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	expectedErr = KErrNotSupported;
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_POWER_OFF2);	// invalid type
	TSsmCommandParameters params2(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:08 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_POWER_OFF3);	// unsupported version
	TSsmCommandParameters params3(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/


void CCmdTestPowerOff::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with invalid data"));
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestResourceReaderWithMissingDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNotSupported;
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - Command construction with missing resource reader data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_POWER_OFF4);
	TSsmCommandParameters params(reader, NULL, 0);
	// read some of the data so there will not be enough to read
	reader.ReadInt32L();
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - construct from resource with invalid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:01 & 02 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestCommandTypeL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TPowerState powerEvent = EPwActive;
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Test command type"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAPD(err, iTestCmd = CreateCmdFromStreamL(severity, powerEvent));
	CompareCommandsDataL(iTestCmd, severity, powerEvent);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdPowerOff);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(severity, powerEvent));
	CompareCommandsDataL(iTestCmd, severity, powerEvent);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdPowerOff);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	powerEvent = EPwStandby;
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_POWER_OFF1);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdPowerOff::NewL(params));
	CompareCommandsDataL(iTestCmd, severity, powerEvent);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdPowerOff);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestCmdFailureIgnoreSeverityL()
	{
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestCmdFailureHighSeverityL()
	{
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestCancelCmdIgnoreSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TPowerState powerEvent = EPwActive;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdIgnoreFailure Severity and cancel"));

	INFO_PRINTF1(_L("doTestCancelCmdIgnoreSeverityL:01 Test: Run a command and cancel"));
	CreateCmdAndExecuteL(severity, powerEvent, expectedErr, ETrue);
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/

void CCmdTestPowerOff::doTestCancelCmdHighSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TPowerState powerEvent = EPwActive;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdHighSeverity Severity and cancel"));

	INFO_PRINTF1(_L("doTestCancelCmdIgnoreSeverityL:01 Test: Run a command and cancel"));
	CreateCmdAndExecuteL(severity, powerEvent, expectedErr, ETrue);
	}

/**
Old Test CaseID 		AFSS-CMDPOF-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0012
*/


void CCmdTestPowerOff::doTestNormalExecutionL()
	{
	}

/**
Helper function to create instance of CCmdPowerOff trough readstream.
*/
CCmdPowerOff* CCmdTestPowerOff::CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteInt16L(aPowerEvent);
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdPowerOff* newCmd = CCmdPowerOff::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	
	return newCmd;
	}

/**
Helper function to compare the data of the command.
*/
void CCmdTestPowerOff::CompareCommandsDataL(CCmdPowerOff* aTestCmd, TCmdErrorSeverity aSeverity, TPowerState aPowerEvent)
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
	TPowerState state = static_cast<TPowerState>(readStream.ReadInt16L());
	TEST(aPowerEvent == state);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(outputBuffer);
	}

/**
Helper function to run a command and test the expected results
*/
void CCmdTestPowerOff::CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent, TInt aExpectedErr, TBool aCancel)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddPowerOffCommandL(aSeverity, aPowerEvent);
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

