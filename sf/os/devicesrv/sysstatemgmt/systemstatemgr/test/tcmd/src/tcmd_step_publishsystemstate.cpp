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

#include <ssm/ssmcommandlistresourcereader.h>
#include <domainmanager.h>

#include <ssmcmd_publishsystemstate1.rsg>

#include "cmdpublishsystemstate.h"
#include "tcmd_step_publishsystemstate.h"
#include "tcmd_cmdlist.h"
#include "ssmcommandparameters.h"

_LIT(KRscFileBuffer, "z:\\resource\\ssmatest\\bic\\ssmcmd_publishsystemstate1.rsc");
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
#endif

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
typedef TDmTraverseDirection TTestSsmDirection;
TTestSsmDirection testTraverseParentsFirst = ETraverseParentsFirst;
TTestSsmDirection testTraverseChildrenFirst = ETraverseChildrenFirst;
TTestSsmDirection testTraverseDefault = ETraverseDefault;
TTestSsmDirection testTraverseMax = ETraverseMax;
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
typedef TSsmDmTraverseDirection TTestSsmDirection;
TTestSsmDirection testTraverseParentsFirst = ESsmTraverseParentsFirst;
TTestSsmDirection testTraverseChildrenFirst = ESsmTraverseChildrenFirst;
TTestSsmDirection testTraverseDefault = ESsmTraverseDefault;
TTestSsmDirection testTraverseMax = ESsmTraverseMax;
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
	
/**
Destructor
*/
CCmdTestPublishSystemState::~CCmdTestPublishSystemState()
	{
	}


/**
Constructor
*/
CCmdTestPublishSystemState::CCmdTestPublishSystemState()
	: CCmdTestBase(KTCCmdTestPublishSystemState), iMainstate(ESsmNormal)
	{
	}


/**
Run the tests
*/
TVerdict CCmdTestPublishSystemState::doTestStepL()
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

	__UHEAP_MARK;
	TRAP(err, doTestSupportingClassesL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/


void CCmdTestPublishSystemState::doTestNewLWithGoodDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;

	TSsmPublishSystemStateInfo info;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmState state(iMainstate, 0);
	TTestSsmDirection direction = testTraverseDefault;
	TInt16 retries = 5;

	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	info.Set(state, direction, retries);
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo)"));
	TRAP(err, ConstructAndDeleteL(severity, ESsmWaitForSignal, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo)"));	
	TRAP(err, ConstructAndDeleteL(severity, ESsmWaitForSignal, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, CallInternalizeDataTestL(severity, ESsmWaitForSignal, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	TRAP(err, ConstructAndDeleteL(severity, ESsmWaitForSignal, info, KTestPriority));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("Read valid priority from list"));
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE11);
	TSsmCommandParameters params(reader, NULL, 0, state);
	TRAP(err, ConstructAndDeleteViaReaderL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("initial version gives default priortiy"));
	RResourceReader& reader2 = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE12);
	TSsmCommandParameters params2(reader2, NULL, 0, state);
	TRAP(err, ConstructAndDeleteViaReaderL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

#endif
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestNewLWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrArgument;
	TSsmPublishSystemStateInfo info;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TSsmState state(iMainstate, 0);
	TTestSsmDirection direction = testTraverseDefault;
	TInt16 retries = 5;

	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with Invalid data"));
		
	state.SetFromInt(123);
	direction = static_cast<TTestSsmDirection>(testTraverseParentsFirst - 1);
	info.Set(state, direction, retries);
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, info));	// this should fail with direction
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	direction = static_cast<TTestSsmDirection>(testTraverseMax + 1);
	info.Set(state, direction, retries);
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, info));	// this should fail with direction
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	
	direction = testTraverseDefault;
	info.Set(state, direction, retries);
	executionBehaviour = static_cast<TSsmExecutionBehaviour>(0);
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	
	executionBehaviour = static_cast<TSsmExecutionBehaviour>(10);
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	
	executionBehaviour = ESsmWaitForSignal;
	retries = -27;
	info.Set(state, direction, retries);
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == KErrArgument);
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestInternalizeLWithGoodDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	TSsmPublishSystemStateInfo info;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmState state(iMainstate, 0);
	TTestSsmDirection direction = testTraverseDefault;
	TInt16 retries = 5;

	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL - Command construction using InternalizeL with valid data"));
		
	info.Set(state, direction, retries);
	TRAP(err, CallInternalizeDataTestL(severity, ESsmWaitForSignal, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	severity = ECmdIgnoreFailure;
	TRAP(err, CallInternalizeDataTestL(severity, ESsmWaitForSignal, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestInternalizeLWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrArgument;
	TSsmPublishSystemStateInfo info;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TSsmState state(iMainstate, 0);
	TTestSsmDirection direction = testTraverseDefault;
	TInt16 retries = 5;

	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL - Command construction using InternalizeL with Invalid data"));

	state.SetFromInt(123);
	direction = static_cast<TTestSsmDirection>(testTraverseParentsFirst - 1);
	info.Set(state, direction, retries);
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, info));	// this should fail with direction
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);	
	TEST(err == expectedErr);
	
	direction = static_cast<TTestSsmDirection>(testTraverseMax + 1);
	info.Set(state, direction, retries);
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, info));	// this should fail with direction
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);	
	TEST(err == expectedErr);
	
	direction = testTraverseDefault;
	info.Set(state, direction, retries);
	executionBehaviour = static_cast<TSsmExecutionBehaviour>(0);
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);	
	TEST(err == expectedErr);
	
	executionBehaviour = static_cast<TSsmExecutionBehaviour>(10);
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);	
	TEST(err == expectedErr);
	
	executionBehaviour = ESsmWaitForSignal;
	retries = -27;
	info.Set(state, direction, retries);
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestResourceReaderWithGoodDataL()
	{
	TInt err = KErrNone;
	TInt expectedType = ESsmCmdPublishSystemState;
	TInt expectedErr = KErrNone;
	// these values must match what is in the resource file
	TSsmPublishSystemStateInfo info;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmState state(iMainstate, 0);
	TTestSsmDirection direction = testTraverseDefault;
	TInt16 retries = 5;
		
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - Test InternalizeL and ExternalizeL with valid data"));

	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE1);  

	info.Set(state, direction, retries);
	TSsmCommandParameters params(reader, NULL, 0, state);
	
	// create command
	CCmdPublishSystemState* pubCmd = CCmdPublishSystemState::NewL(params);
	CleanupStack::PushL(pubCmd);
	TEST(pubCmd->Type() == expectedType);

	// create output buffer
 	CBufFlat* writebuf = CBufFlat::NewL(1024);
	CleanupStack::PushL(writebuf);	
	
	// create output stream on buffer
	RBufWriteStream writeStream(*writebuf);
	CleanupClosePushL(writeStream);	

	// externalise the data
	pubCmd->ExternalizeL(writeStream);

	// check the data is correct
	TRAP(err, CompareInputAndOutputL(severity, ESsmWaitForSignal, info, writebuf));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(writebuf);	
	CleanupStack::PopAndDestroy(pubCmd);
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestResourceReaderWithMissingDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNotSupported;	

	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - Test with missing data"));

	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE1);  

	// read some of the data so there will not be enough to read
	reader.ReadInt32L();

	TSsmState state(iMainstate, 0);
	TSsmCommandParameters* params = new TSsmCommandParameters(reader, NULL, 0, state);
	TRAP(err, ConstructAndDeleteViaReaderL(*params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);	// due to reading ahead the version might not be correct any longer

	FreeResources();
	delete params;
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestResourceReaderWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNotSupported;	
	TSsmState state(iMainstate, 0);

	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - Test with Invalid data"));

	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL:01: - Invalid Type"));
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE2);	
	TSsmCommandParameters params(reader, NULL, 0, state);
	TRAP(err, ConstructAndDeleteViaReaderL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL:02: - Invalid Version"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE3);  
	TSsmCommandParameters params1(reader, NULL, 0, state);
	TRAP(err, ConstructAndDeleteViaReaderL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	expectedErr = KErrArgument;
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL:03: - Invalid Direction 1"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE4);  
	TSsmCommandParameters params2(reader, NULL, 0, state);
	TRAP(err, ConstructAndDeleteViaReaderL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL:04: - Invalid Direction 2"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE5);  
	TSsmCommandParameters params3(reader, NULL, 0, state);
	TRAP(err, ConstructAndDeleteViaReaderL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL:05: - Invalid executionBehaviour 1"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE8);  
	TSsmCommandParameters params4(reader, NULL, 0, state);
	TRAP(err, ConstructAndDeleteViaReaderL(params4));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL:06: - Invalid executionBehaviour 2"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE9);  
	TSsmCommandParameters params5(reader, NULL, 0, state);
	TRAP(err, ConstructAndDeleteViaReaderL(params5));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL:07: - Negative retries"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_PUBLISH_SYSSTATE10);  
	TSsmCommandParameters params6(reader, NULL, 0, state);
	TRAP(err, ConstructAndDeleteViaReaderL(params6));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);	
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestCommandTypeL()
	{
	TInt type = ESsmCmdPublishSystemState;
	TSsmPublishSystemStateInfo info;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmState state(iMainstate, 0);
	TTestSsmDirection direction = testTraverseDefault;
	TInt16 retries = 5;

	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Test for command type"));

	info.Set(state, direction, retries);

	CCmdPublishSystemState* pubCmd = CCmdPublishSystemState::NewL(severity, ESsmWaitForSignal, info);
	CleanupStack::PushL(pubCmd);
	INFO_PRINTF3(_L("Test completed with Type : %d. expected Type : %d"), pubCmd->Type(), type);
	TEST(pubCmd->Type() == type);

	CleanupStack::PopAndDestroy(pubCmd);
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/


void CCmdTestPublishSystemState::doTestCmdFailureIgnoreSeverityL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmPublishSystemStateInfo info;
	TSsmState state(iMainstate, 0);
	TTestSsmDirection direction = testTraverseDefault;
	TInt16 retries = 5;

	info.Set(state, direction, retries);

	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL:01: - executionBehaviour = ESsmWaitForSignal"));
	TRAP(err, PublishSystemStateL(severity, ESsmWaitForSignal, info, expectedErr, EFalse));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL:02: - executionBehaviour = ESsmDeferredWaitForSignal"));
	TRAP(err, PublishSystemStateL(severity, ESsmDeferredWaitForSignal, info, expectedErr, EFalse));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestCmdFailureHighSeverityL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TSsmPublishSystemStateInfo info;
	TSsmState state(ESsmEmergencyCallsOnly, 0);
	TTestSsmDirection direction = testTraverseDefault;
	TInt16 retries = 5;

	info.Set(state, direction, retries);

	INFO_PRINTF1(_L("Test:doTestCmdFailureHighSeverityL:01: - executionBehaviour = ESsmWaitForSignal"));
	TRAP(err, PublishSystemStateL(severity, ESsmWaitForSignal, info, expectedErr, EFalse));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("Test:doTestCmdFailureHighSeverityL:02: - executionBehaviour = ESsmDeferredWaitForSignal"));
	TRAP(err, PublishSystemStateL(severity, ESsmWaitForSignal, info, expectedErr, EFalse));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestCancelCmdIgnoreSeverityL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	TInt expectedCmdStatus = KErrCancel;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmPublishSystemStateInfo info;
	TSsmState state(iMainstate, 0);
	TTestSsmDirection direction = testTraverseDefault;
	TInt16 retries = 5;

	info.Set(state, direction, retries);

	INFO_PRINTF1(_L("Test:doTestCancelCmdIgnoreSeverityL:01: - executionBehaviour = ESsmWaitForSignal"));
	TRAP(err, PublishSystemStateL(severity, ESsmWaitForSignal, info, expectedCmdStatus, ETrue));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("Test:doTestCancelCmdIgnoreSeverityL:02: - executionBehaviour = ESsmDeferredWaitForSignal"));
	TRAP(err, PublishSystemStateL(severity, ESsmDeferredWaitForSignal, info, expectedCmdStatus, ETrue));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestCancelCmdHighSeverityL()
	{
	TInt err = KErrCancel;
	TInt expectedErr = KErrNone;
	TInt expectedCmdStatus = KErrCancel;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TSsmPublishSystemStateInfo info;
	TSsmState state(iMainstate, 0);
	TTestSsmDirection direction = testTraverseDefault;
	TInt16 retries = 5;

	info.Set(state, direction, retries);

	INFO_PRINTF1(_L("Test:doTestCancelCmdHighSeverityL:01: - executionBehaviour = ESsmWaitForSignal"));
	TRAP(err, PublishSystemStateL(severity, ESsmWaitForSignal, info, expectedCmdStatus, ETrue));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("Test:doTestCancelCmdHighSeverityL:02: - executionBehaviour = ESsmDeferredWaitForSignal"));
	TRAP(err, PublishSystemStateL(severity, ESsmDeferredWaitForSignal, info, expectedCmdStatus, ETrue));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestNormalExecutionL()
	{
	INFO_PRINTF1(_L("Test:doTestNormalExecutionL - InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in AFSS-CMDPSS-0009 and AFSS-CMDPSS-0010 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDPSS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0014
*/

void CCmdTestPublishSystemState::doTestSupportingClassesL()
	{
	INFO_PRINTF1(_L("Test:doTestSupportingClassesL"));	
	TSsmPublishSystemStateInfo info;
	
    TSsmState state(iMainstate, 4);
	TTestSsmDirection direction = testTraverseChildrenFirst;
	TInt16 retries = 5;
 	
	// set
	info.Set(state, direction, retries);
	TEST(info.State().MainState() == state.MainState());
	TEST(info.State().SubState() == state.SubState());
	TEST(info.Direction() == direction);
	TEST(info.Retries() == retries);

	// reset/clear	
 	info.Clear();
	TEST(info.State().Int() == 0);
	TEST(info.Direction() == testTraverseDefault);
	TEST(info.Retries() == 0);

	TSsmPublishSystemStateInfo info2;
	info2.Set(state, direction, retries);
 	
	// new assignment	
	info = info2;
	TEST(info.State().MainState() == state.MainState());
	TEST(info.State().SubState() == state.SubState());
	TEST(info.Direction() == direction);
	TEST(info.Retries() == retries);

	// self assignment - values unchanged
	info = info;
	TEST(info.State().MainState() == state.MainState());
	TEST(info.State().SubState() == state.SubState());
	TEST(info.Direction() == direction);
	TEST(info.Retries() == retries);
	
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
	// test to check that enum values in TDmTraverseDirection and 
	// TSsmDmTraverseDirection are consistent
	TDmTraverseDirection domainDirection = ETraverseMax;
	TSsmDmTraverseDirection ssmDomainDirection = ESsmTraverseMax;
	INFO_PRINTF2(_L("The max value of TDmTraverseDirection is		: %d"), domainDirection);
	INFO_PRINTF2(_L("The max value of TSsmDmTraverseDirection is	: %d"), ssmDomainDirection);
	TEST(domainDirection == ssmDomainDirection);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
	
	}

/** 
 Construct and delete a command
*/
void CCmdTestPublishSystemState::ConstructAndDeleteL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo, const TInt aVersion, const TUint16 aPriority)
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdPublishSystemState* pubCmd = (aVersion == ECmdPublishSystemStateInitialVersion) ? CCmdPublishSystemState::NewL(aSeverity, aExecutionBehaviour, aInfo) : CCmdPublishSystemState::NewL(aSeverity, aExecutionBehaviour, aInfo, aPriority);
#else
	TEST (aPriority == KDefaultPriority);
	TEST (aVersion == ECmdPublishSystemStateInitialVersion);
	CCmdPublishSystemState* pubCmd = CCmdPublishSystemState::NewL(aSeverity, aExecutionBehaviour, aInfo);
#endif

	CleanupStack::PushL(pubCmd);

	TEST(pubCmd->Severity()==aSeverity);
	TEST(pubCmd->Type()==ESsmCmdPublishSystemState);

	CleanupStack::PopAndDestroy(pubCmd);
	}

/**
 Construct and delete a command from resource
*/ 
void CCmdTestPublishSystemState::ConstructAndDeleteViaReaderL(TSsmCommandParameters& aParams)
	{
	CCmdPublishSystemState* pubCmd = CCmdPublishSystemState::NewL(aParams);
	CleanupStack::PushL(pubCmd);	

	TEST(pubCmd->Type() == ESsmCmdPublishSystemState);

	CleanupStack::PopAndDestroy(pubCmd);
	}

/**
 Construct and delete a command using Internalise
*/ 
void CCmdTestPublishSystemState::CallInternalizeDataTestL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo, const TUint16 aPriority)
	{
	// create a set of data and put it into a buffer
 	CBufFlat* bufferin = CBufFlat::NewL(1024);
	CleanupStack::PushL(bufferin);	
	
	// create write stream on buffer and put the data in
	RBufWriteStream writeStream(*bufferin);
	CleanupClosePushL(writeStream);		
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteInt8L(aExecutionBehaviour);
	writeStream.WriteInt32L(aInfo.State().Int());
	writeStream.WriteInt32L(aInfo.Direction());
	writeStream.WriteInt16L(aInfo.Retries());
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	writeStream.CommitL();

	// create a readstream on the data and put the data into the command
	RDesReadStream aReadStream(bufferin->Ptr(0));
	CleanupClosePushL(aReadStream);
	CCmdPublishSystemState* pubCmd = CCmdPublishSystemState::NewL(aReadStream);
	CleanupStack::PushL(pubCmd);

	TEST(pubCmd->Type() == ESsmCmdPublishSystemState);
		
	// create output buffer and stream
 	CBufFlat* bufferout = CBufFlat::NewL(1024);
	CleanupStack::PushL(bufferout);	
	RBufWriteStream writeStream2(*bufferout);
	CleanupClosePushL(writeStream2);		

	// externalise the data
	pubCmd->ExternalizeL(writeStream2);
	
	// check the data is correct
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CompareInputAndOutputL(aSeverity, aExecutionBehaviour, aInfo, bufferout, aPriority);
#else
	CompareInputAndOutputL(aSeverity, aExecutionBehaviour, aInfo, bufferout);
#endif
	
	CleanupStack::PopAndDestroy(&writeStream2);
	CleanupStack::PopAndDestroy(bufferout);	
	CleanupStack::PopAndDestroy(pubCmd);
	CleanupStack::PopAndDestroy(&aReadStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(bufferin);	
	}

/**
 Helper function - Compare input and output
 */
void CCmdTestPublishSystemState::CompareInputAndOutputL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo, CBufFlat* aWriteBuf, const TUint16 aPriority)
	{
	RDesReadStream aReadStream(aWriteBuf->Ptr(0));
	CleanupClosePushL(aReadStream);

	TCmdErrorSeverity severity = static_cast<TCmdErrorSeverity>(aReadStream.ReadUint16L());
	TSsmExecutionBehaviour executionBehaviour = static_cast<TSsmExecutionBehaviour>(aReadStream.ReadUint8L());
	TUint32 state = aReadStream.ReadUint32L();
	TTestSsmDirection direction = static_cast<TTestSsmDirection>(aReadStream.ReadUint32L());
	TInt16 retries = aReadStream.ReadInt16L();
	
	INFO_PRINTF5(_L("Original: Severity %d State %d Direction %d Retries %d"), aSeverity, aInfo.State().Int(), aInfo.Direction(), aInfo.Retries());
	INFO_PRINTF5(_L("Write:    Severity %d State %d Direction %d Retries %d"), severity, state, direction, retries);

	TEST(aSeverity == severity);
	TEST(aExecutionBehaviour == executionBehaviour);
	TEST(aInfo.State().Int() == state);	
	TEST(aInfo.Direction() == direction);
	TEST(aInfo.Retries() == retries);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	const TUint16 priority = aReadStream.ReadUint16L();
	INFO_PRINTF2(_L("Original: Priority %d"), aPriority);
	INFO_PRINTF2(_L("Got     : Priority %d"), priority);
	TEST(aPriority == priority);
#else
	TEST(aPriority == KDefaultPriority);
#endif


	CleanupStack::PopAndDestroy(&aReadStream);
	}

/**
 Helper function - 
*/ 
CSsmCommandBase* CCmdTestPublishSystemState::ConstructCommandFromStreamLC(TSsmCommandType /*aType*/, RReadStream& aReadStream, TArray<MSsmCommand*>& /*aDeferredList*/)
	{
	CSsmCommandBase* cmd = CCmdPublishSystemState::NewL(aReadStream);
	CleanupStack::PushL(cmd);
	return cmd;
	}

/**
 Helper function - Publish System State.
*/
void CCmdTestPublishSystemState::PublishSystemStateL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, 
													TSsmPublishSystemStateInfo aPubSysStateInfo, TInt aExpectedCmdStatus, TBool aCancel, TInt32 aTimeout)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddPublishSystemStateCommandL(aSeverity, aExecutionBehaviour, aPubSysStateInfo);

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
	INFO_PRINTF3(_L("Command execution finished with error : %d, aExpectedCmdStatus : %d. "), err, aExpectedCmdStatus);
	TEST(err == aExpectedCmdStatus);
	delete iTestList;
	iTestList = NULL;
	}

