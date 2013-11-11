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
#include "ssmcommandparameters.h"
#include "cmdpersisthalattributes.h"
#include "tcmd_step_persisthalattributes.h"
#include "tcmd_cmdlist.h"

#include <ssmcmd_persisthal.rsg>
_LIT(KRscFileBuffer, "z:\\resource\\ssmatest\\bic\\ssmcmd_persisthal.rsc");

CCmdTestPersistHalAttributes::CCmdTestPersistHalAttributes()
	: CCmdTestBase(KTCCmdTestPersistHalAttributes)
	{
	}

CCmdTestPersistHalAttributes::~CCmdTestPersistHalAttributes()
	{
	}

TVerdict CCmdTestPersistHalAttributes::doTestStepL()
	{
	TInt err = KErrNone;
	
	__UHEAP_MARK;
	TRAP(err, doTestNewLWithGoodDataL());
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
Old Test CaseID 		AFSS-CMDHAL-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestNewLWithGoodDataL()
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
	TRAP(err, iTestCmd = CCmdPersistHalAttributes::NewL(severity));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PERSISTHAL1);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdPersistHalAttributes::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestNewLWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNotSupported;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PERSISTHAL2);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdPersistHalAttributes::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PERSISTHAL3);
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdPersistHalAttributes::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with invalid data"));
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestResourceReaderWithMissingDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNotSupported;

	INFO_PRINTF1(_L("doTestResourceReaderWithMissingDataL:01 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PERSISTHAL1);
	TSsmCommandParameters params(reader, NULL, 0);
	// read some of the data so there will not be enough to read
	reader.ReadInt32L();
	TRAP(err, iTestCmd = CCmdPersistHalAttributes::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - construct from resource with invalid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:01 & 02 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestCommandTypeL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestCommandTypeL:01 Construct from NewL(RReadStream& aReadStream) and check type"));
	iTestCmd = CreateCmdFromStreamL(severity);
	CompareCommandsDataL(iTestCmd, severity);
	TEST(iTestCmd->Type() == ESsmCmdPersistHalAttributes);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:02 Construct from NewL(TCmdErrorSeverity aSeverity) and check type"));
	severity = ECmdCriticalSeverity;
	iTestCmd = CCmdPersistHalAttributes::NewL(severity);
	CompareCommandsDataL(iTestCmd, severity);
	TEST(iTestCmd->Type() == ESsmCmdPersistHalAttributes);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:03 Construct from NewL(TSsmCommandParameters& aCommandParameters) and check type"));
	severity = ECmdMediumSeverity;
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PERSISTHAL1);
	TSsmCommandParameters params(reader, NULL, 0);
	iTestCmd = CCmdPersistHalAttributes::NewL(params);
	CompareCommandsDataL(iTestCmd, severity);		//validate the value
	TEST(iTestCmd->Type() == ESsmCmdPersistHalAttributes);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestCmdFailureIgnoreSeverityL()
	{
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestCmdFailureHighSeverityL()
	{
	}


/**
Old Test CaseID 		AFSS-CMDHAL-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestCancelCmdIgnoreSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdIgnoreFailure Severity and cancel"));

	INFO_PRINTF1(_L("doTestCancelCmdIgnoreSeverityL:01 Test: Run a command and cancel"));
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestCancelCmdHighSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdHighSeverity Severity and cancel"));

	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:01 Test: Run a command and cancel"));
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);
	}

/**
Old Test CaseID 		AFSS-CMDHAL-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0011
*/

void CCmdTestPersistHalAttributes::doTestNormalExecutionL()
	{
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdHighSeverity Severity and cancel"));

	INFO_PRINTF1(_L("doTestNormalExecutionL:01 Test: Run command normally"));
	CreateCmdAndExecuteL(severity, expectedErr, EFalse);
	}

/**
Helper function to create instance of CCmdPersistHalAttributes trough readstream.
*/
CCmdPersistHalAttributes* CCmdTestPersistHalAttributes::CreateCmdFromStreamL(TCmdErrorSeverity aSeverity)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdPersistHalAttributes* newCmd = CCmdPersistHalAttributes::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	
	return newCmd;
	}

/**
Helper function to compare the data of the command.
*/
void CCmdTestPersistHalAttributes::CompareCommandsDataL(CCmdPersistHalAttributes* aTestCmd, TCmdErrorSeverity aSeverity)
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
void CCmdTestPersistHalAttributes::CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TInt aExpectedErr, TBool aCancel)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddPersistHalAttributesCommandL(aSeverity);
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

