// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ssmcmd_publishswp1.rsg>
#include "cmdpublishswp.h"
#include "tcmd_step_publishswp.h"
#include "tcmd_cmdlist.h"
#include "ssmcommandparameters.h"
#include "ssmcommandfactory.h"
#include "e32property.h"

static const TUint KPublishSwpKey = {777};
static const TUint KSwpNullUid = {0};

_LIT(KRscFileBuffer, "z:\\resource\\ssmatest\\bic\\ssmcmd_publishswp1.rsc");
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
const TUint16 KNegPriority = 65535;
#endif
	
CCmdTestPublishSwp::CCmdTestPublishSwp()
	: CCmdTestBase(KTCCmdTestPublishSwp)
	{
	}

CCmdTestPublishSwp::~CCmdTestPublishSwp()
	{
	}

TVerdict CCmdTestPublishSwp::doTestStepL()
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
Old Test CaseID 		AFSS-CMDPSW-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/

void CCmdTestPublishSwp::doTestNewLWithGoodDataL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmSwp info(KSwpNullUid, 0);
	TSsmExecutionBehaviour executionBehaviour = ESsmFireAndForget;	//valid execution behaviour
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, TSsmSwp& aSwp)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdCriticalSeverity;	//this severity is same as defined in r_command_valid_data
	executionBehaviour = ESsmWaitForSignal;

	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SWP1);
	TSsmCommandParameters params(reader, NULL, 0, info);
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, TSsmSwp& aSwp, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(severity, executionBehaviour, info, KTestPriority));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info, KTestPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters and priority too)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	severity = ECmdCriticalSeverity;	//this severity is same as defined in r_command_valid_data
	executionBehaviour = ESsmWaitForSignal;
	// open reader
	RResourceReader& reader2 = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SWP6);
	TSsmCommandParameters params2(reader2, NULL, 0, info);
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info, KTestPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters and priority too)"));
	INFO_PRINTF1(_L("Initial version gives default priority"));
	severity = ECmdCriticalSeverity;	//this severity is same as defined in r_command_valid_data
	executionBehaviour = ESsmWaitForSignal;
	// open reader
	RResourceReader& reader3 = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SWP7);
	TSsmCommandParameters params3(reader3, NULL, 0, info);
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info, KDefaultCommandPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters and priority too)"));
	INFO_PRINTF1(_L("negative priority will be interpreted as a very high positive value"));
	severity = ECmdCriticalSeverity;	//this severity is same as defined in r_command_valid_data
	executionBehaviour = ESsmWaitForSignal;
	// open reader
	RResourceReader& reader4 = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SWP8);
	TSsmCommandParameters params4(reader4, NULL, 0, info);
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(params4));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info, KNegPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
#endif
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/

void CCmdTestPublishSwp::doTestNewLWithBadDataL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmSwp info(KSwpNullUid, 0);
	TSsmExecutionBehaviour executionBehaviour =  static_cast<TSsmExecutionBehaviour>(-1);	//valid execution behaviour
	TInt err = KErrNone;
	TInt expectedErr = KErrArgument;
	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - Command construction with invalid data"));

	// test invalid execution behaviour (-ve value)
	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - test invalid execution behaviour (-ve value)"));
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, TSsmSwp& aSwp)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SWP4);
	TSsmCommandParameters params(reader, NULL, 0, info);
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	// test invalid execution behaviour (out of bound value )
	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - test invalid execution behaviour (out of bound value )"));
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:04 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:05 Test: NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, TSsmSwp& aSwp)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SWP5);
	TSsmCommandParameters params1(reader, NULL, 0, info);
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	// other bad data
	expectedErr = KErrNotSupported;
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SWP2);
	TSsmCommandParameters params2(reader, NULL, 0, info);
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:08 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SWP3);
	TSsmCommandParameters params3(reader, NULL, 0, info);
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/


void CCmdTestPublishSwp::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/

void CCmdTestPublishSwp::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL - InternalizeL with in valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL: **** "));
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/


void CCmdTestPublishSwp::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/


void CCmdTestPublishSwp::doTestResourceReaderWithMissingDataL()
	{
	TInt expectedErr = KErrNotSupported;
	TSsmSwp info(KSwpNullUid, 0);

	INFO_PRINTF1(_L("doTestResourceReaderWithMissingDataL:01 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SWP1);
	TSsmCommandParameters params(reader, NULL, 0, info);
	reader.ReadUint16L();	//read some data so reader will become invalid
	TRAPD(err, iTestCmd = CCmdPublishSwp::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/

void CCmdTestPublishSwp::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL **** "));
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/

void CCmdTestPublishSwp::doTestCommandTypeL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmSwp info(KSwpNullUid, 0);
	TSsmExecutionBehaviour executionBehaviour = ESsmFireAndForget;	//valid execution behaviour
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestCommandTypeL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, executionBehaviour, info));
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdPublishSwp);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, TSsmSwp& aSwp)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(severity, executionBehaviour, info));
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdPublishSwp);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdCriticalSeverity;	//this severity is same as defined in r_command_valid_data
	executionBehaviour = ESsmWaitForSignal;
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SWP1);
	TSsmCommandParameters params(reader, NULL, 0, info);
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(params));
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdPublishSwp);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestCommandTypeL:03 Test: NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, TSsmSwp& aSwp, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdPublishSwp::NewL(severity, executionBehaviour, info, KTestPriority));
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, info, KTestPriority);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdPublishSwp);
	delete iTestCmd;
	iTestCmd = NULL;
#endif
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/

void CCmdTestPublishSwp::doTestCmdFailureIgnoreSeverityL()
	{
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmSwp info(KSwpNullUid, 0);
	INFO_PRINTF1(_L("Test cancel command execution with ECmdCriticalSeverity Severity and cancel"));

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:01 Test: ESsmWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmWaitForSignal, info, expectedErr, EFalse);

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:02 Test: ESsmDeferredWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, info, expectedErr, EFalse, 20000);
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/

void CCmdTestPublishSwp::doTestCmdFailureHighSeverityL()
	{
	TInt expectedErr = KErrNotFound;	//trying to publish a key which does not exist
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmSwp info(KSwpNullUid, 0);
	INFO_PRINTF1(_L("Test cancel command execution with ECmdCriticalSeverity Severity and cancel"));

	INFO_PRINTF1(_L("doTestCmdFailureHighSeverityL:01 Test: ESsmWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmWaitForSignal, info, expectedErr);

	INFO_PRINTF1(_L("doTestCmdFailureHighSeverityL:02 Test: ESsmDeferredWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, info, expectedErr, EFalse, 20000);
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/


void CCmdTestPublishSwp::doTestCancelCmdIgnoreSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmSwp info(KSwpNullUid, 0);
	INFO_PRINTF1(_L("Test cancel command execution with ECmdIgnoreFailure Severity and cancel"));

	INFO_PRINTF1(_L("doTestCancelCmdIgnoreSeverityL:01 Test: ESsmWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmWaitForSignal, info, expectedErr, ETrue);

	INFO_PRINTF1(_L("doTestCancelCmdIgnoreSeverityL:02 Test: ESsmDeferredWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, info, expectedErr, ETrue, 20000);
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/

void CCmdTestPublishSwp::doTestCancelCmdHighSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmSwp info(KSwpNullUid, 0);
	INFO_PRINTF1(_L("Test cancel command execution with ECmdCriticalSeverity Severity and cancel"));

	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:01 Test: ESsmWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmWaitForSignal, info, expectedErr, ETrue);

	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:02 Test: ESsmDeferredWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, info, expectedErr, ETrue, 20000);
	}

/**
Old Test CaseID 		AFSS-CMDPSW-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0013
*/

void CCmdTestPublishSwp::doTestNormalExecutionL()
	{
	
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TInt expectedErr = KErrNone;
	TSsmSwp info(KPublishSwpKey, 123);
	TInt actualValue;
	TInt err = CreateTestSwpKeyL();
	TEST(err == KErrNone);

	INFO_PRINTF1(_L("doTestNormalExecutionL:01 Test: ESsmWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmWaitForSignal, info, expectedErr, EFalse);
	err = RProperty::Get(RProcess().SecureId(), info.Key(), actualValue);
	TEST(err == KErrNone);
	TEST(actualValue == info.Value());

	INFO_PRINTF1(_L("doTestNormalExecutionL:02 Test: ESsmDeferredWaitForSignal"));
	TSsmSwp info1(KPublishSwpKey, 456);
	CreateCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, info1, expectedErr, EFalse, 10000);
	err = RProperty::Get(RProcess().SecureId(), info1.Key(), actualValue);
	TEST(err == KErrNone);
	TEST(actualValue == info1.Value());
	
	//delete test key
	err = RProperty::Delete(RProcess().SecureId(), info.Key());
	TEST(err == KErrNone);	
	}


/**
Helper function to compare the data of the command.
*/
void CCmdTestPublishSwp::CompareCommandsDataL(CCmdPublishSwp* aTestCmd, TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwp, const TUint16 aPriority)
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
	TSsmExecutionBehaviour executionBehaviour = static_cast<TSsmExecutionBehaviour>(readStream.ReadUint8L());
	TEST(aExecutionBehaviour == executionBehaviour);
	const TUint key = readStream.ReadUint32L();
	TEST(key == aSwp.Key());
	const TInt32 value = readStream.ReadInt32L();	
	TEST(value == aSwp.Value());
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	const TUint16 priority = readStream.ReadUint16L();
	TEST(aPriority == priority);
#else
	TEST(aPriority == KDefaultPriority);
#endif

	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(outputBuffer);
	}

/**
Helper function to create instance of CCmdPublishSwp trough readstream.
*/
CCmdPublishSwp* CCmdTestPublishSwp::CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwp, const TUint16 aPriority)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteUint8L(aExecutionBehaviour);
	writeStream.WriteInt32L(aSwp.Key());
	writeStream.WriteInt32L(aSwp.Value());
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdPublishSwp* newCmd = CCmdPublishSwp::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	
	return newCmd;
	}

/**
Helper function to run a command and test the expected results
*/
void CCmdTestPublishSwp::CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, 
											 TSsmSwp& aSwp, TInt aExpectedErr, TBool aCancel, TInt32 aTimeout)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddPublishSwpCommandL(aSeverity, aExecutionBehaviour, aSwp);
	if (ESsmDeferredWaitForSignal == aExecutionBehaviour)
		{
		iTestList->AddMultipleWaitCommandL(aTimeout);
		}
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

/**
helper function to create test swp
*/
TInt CCmdTestPublishSwp::CreateTestSwpKeyL()
	{
	_LIT(KPolicyFilename, "ssm.swp.policy.test.cmdreqswp");
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TSsmSwp info(KPublishSwpKey, 876);
	TPtrC filename1 = KPolicyFilename();

	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddCreateSwpCommandL(severity, info, filename1);
	
	StartScheduler();
	TInt err = iTestList->Errorcode();
	delete iTestList;
	iTestList = NULL;
	return err;
	}

