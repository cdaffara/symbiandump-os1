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
#include "tcmd_cmdlist.h"
#include "tcmd_step_waitforapparcinit.h"
#include "cmdwaitforapparcinit.h"
#include "ssmdebug.h"
#include "ssmcommandparameters.h"
#include <ssmcmd_waitforapparcinit.rsg>

_LIT(KTestRscFileName, "z:\\resource\\ssmatest\\bic\\ssmcmd_waitforapparcinit.rsc");
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
const TUint16 KNegPriority = 65535;
#endif

/**
Destructor
*/
CCmdTestWaitForApparcInit::~CCmdTestWaitForApparcInit()
	{
	delete iTestCmd;
	}

/**
Constructor
*/
CCmdTestWaitForApparcInit::CCmdTestWaitForApparcInit()
	: CCmdTestBase(KTCCmdTestWaitForApparcInit)
	{
	}

TVerdict CCmdTestWaitForApparcInit::doTestStepL()
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

	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */

void CCmdTestWaitForApparcInit::doTestNewLWithGoodDataL()
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
	TRAP(err, iTestCmd = CCmdWaitForApparcInit::NewL(severity));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdWaitForApparcInit::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, const TUint16 aPriority)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdWaitForApparcInit::NewL(severity, KTestPriority));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KTestPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("Read priority from rss"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader1 = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA1);
	TSsmCommandParameters params1(reader1, NULL, 0);
	TRAP(err, iTestCmd = CCmdWaitForApparcInit::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KTestPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("Default priority for initial version"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader2 = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA2);
	TSsmCommandParameters params2(reader2, NULL, 0);
	TRAP(err, iTestCmd = CCmdWaitForApparcInit::NewL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KDefaultCommandPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("negative priority will be interpreted as a very high positive value"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader3 = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA3);
	TSsmCommandParameters params3(reader3, NULL, 0);
	TRAP(err, iTestCmd = CCmdWaitForApparcInit::NewL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KNegPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
#endif
	
	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */

void CCmdTestWaitForApparcInit::doTestNewLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - Command construction with in-valid data"));
	//CCmdWaitForApparcInit takes only severity for command constrution and we can not validate the severity
	INFO_PRINTF1(_L("*****Can not implement- no way to pass invalid data*****"));
	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */

void CCmdTestWaitForApparcInit::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));
	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */


void CCmdTestWaitForApparcInit::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL - InternalizeL with in valid data"));
	//InternalizeL function is now private for CCmdWaitForApparcInit so this test is not required
	INFO_PRINTF1(_L("*****Can not implement- no way to pass invalid data*****"));
	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */

void CCmdTestWaitForApparcInit::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */


void CCmdTestWaitForApparcInit::doTestResourceReaderWithMissingDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - construct from resource with invalid reader"));
	TInt expectedErr = KErrNotSupported;
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, NULL, 0);
	reader.ReadUint16L();	//read some data so reader will become invalid
	TRAPD(err, iTestCmd = CCmdWaitForApparcInit::NewL(params));
	// expected leave code is KErrNotSupported, because command will try to read Type as first member but 
	// it will not get the expected Type as reader is not valid.
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */

void CCmdTestWaitForApparcInit::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - Construct from resource with valid reader and invalid data"));
	//CCmdWaitForApparcInit takes only severity for command constrution and we can not validate the severity 
	INFO_PRINTF1(_L("*****Can not implement- no way to pass invalid data*****"));
	}


/**
Old Test CaseID 		APPFWK-CMDWAI-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */

void CCmdTestWaitForApparcInit::doTestCommandTypeL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestCommandTypeL:01 Construct from NewL(RReadStream& aReadStream) and check type"));
	iTestCmd = CreateCmdFromStreamL(severity);
	CompareCommandsDataL(iTestCmd, severity);
	TEST(iTestCmd->Type() == ESsmCmdWaitForApparcInit);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:02 Construct from NewL(TCmdErrorSeverity aSeverity) and check type"));
	severity = ECmdCriticalSeverity;
	iTestCmd = CCmdWaitForApparcInit::NewL(severity);
	CompareCommandsDataL(iTestCmd, severity);
	TEST(iTestCmd->Type() == ESsmCmdWaitForApparcInit);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:03 Construct from NewL(TSsmCommandParameters& aCommandParameters) and check type"));
	severity = ECmdMediumSeverity;
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, NULL, 0);
	iTestCmd = CCmdWaitForApparcInit::NewL(params);
	CompareCommandsDataL(iTestCmd, severity);		//validate the value
	TEST(iTestCmd->Type() == ESsmCmdWaitForApparcInit);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */

void CCmdTestWaitForApparcInit::doTestCmdFailureIgnoreSeverityL()
	{
	
	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */

void CCmdTestWaitForApparcInit::doTestCmdFailureHighSeverityL()
	{

	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */

void CCmdTestWaitForApparcInit::doTestCancelCmdIgnoreSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdIgnoreFailure Severity and cancel"));
	
	INFO_PRINTF1(_L("doTestCancelCmdIgnoreSeverityL:01 "));
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);
	}

/**
Old Test CaseID 		APPFWK-CMDWAI-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0020
 */

void CCmdTestWaitForApparcInit::doTestCancelCmdHighSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdIgnoreFailure Severity and cancel"));
	
	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:01 "));
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);
	}

/**
Helper function to compare the data of the command.
*/
void CCmdTestWaitForApparcInit::CompareCommandsDataL(CCmdWaitForApparcInit* aTestCmd, TCmdErrorSeverity aSeverity, const TUint16 aPriority)
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
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	TUint16 priority = readStream.ReadUint16L();
	TEST(aPriority == priority);
#else
	TEST(aPriority == KDefaultPriority);
#endif

	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(outputBuffer);
	}

/**
Helper function to create instance of CCmdWaitForApparcInit trough readstream.
*/
CCmdWaitForApparcInit* CCmdTestWaitForApparcInit::CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdWaitForApparcInit* newCmd = CCmdWaitForApparcInit::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	
	return newCmd;
	}

/**
Helper function to run a command and test the expected results
*/
void CCmdTestWaitForApparcInit::CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TInt aExpectedErr, TBool aCancel)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddWaitForApparcInitCommandL(aSeverity);
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
