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

#include <ssmcmd_loadsup1.rsg>

#include "cmdloadsup.h"
#include "tcmd_step_loadsup.h"
#include "tcmd_cmdlist.h"
#include "ssmcommandparameters.h"

_LIT(KFileName, "tcmdgoodsup.dll");
_LIT(KNotExistFileName, "tcmdgoodsup_doesnotexist.dll");
_LIT(KRscFileBuffer, "z:\\resource\\ssmatest\\bic\\ssmcmd_loadsup1.rsc");
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
#endif
		
/**
 Destructor
*/
CCmdTestLoadSup::~CCmdTestLoadSup()
	{
	}

/**
 Constructor
*/
CCmdTestLoadSup::CCmdTestLoadSup()
	: CCmdTestBase(KTCCmdTestLoadSup)
	{
	}

/**
 Run the tests
*/
TVerdict CCmdTestLoadSup::doTestStepL()
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
Old Test CaseID 		AFSS-CMDSUP-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/

void CCmdTestLoadSup::doTestNewLWithGoodDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;

	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TUid uid;
	uid.iUid = 9;
	TInt32 ordinal = 1;
	TInt16 retries = 5;

	TSsmSupInfo info;
	info.SetL(KFileName, ordinal, uid);

	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, retries, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo)"));
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, retries, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo)"));
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, retries, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP1);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:05 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, retries, info, ECmdLoadSupVersionWithPriority, KTestPriority));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	RResourceReader& reader2 = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP15);
	TSsmCommandParameters params2(reader2, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("Invalid version returns default priority"));
	RResourceReader& reader3 = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP16);
	TSsmCommandParameters params3(reader3, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	
#endif
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/

void CCmdTestLoadSup::doTestNewLWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrArgument;

	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TInt16 retries = 5;
	TInt32 ordinal = 1;
		
	TSsmSupInfo info;
	_LIT(KEmptyFileName, "");
	_LIT(KFileName, "tcmdgoodsup.dll");
	TUid uid;
	uid.iUid = 9;

	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL:01 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo)"));

	executionBehaviour = static_cast<TSsmExecutionBehaviour>(ESsmFireAndForget-1);
	INFO_PRINTF1(_L("doTestNewLWithBadDataL: Test: fail with executionBehaviour"));
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, retries, info));	// this should fail with executionBehaviour
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	executionBehaviour = static_cast<TSsmExecutionBehaviour>(100);
	info.SetL(KFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestNewLWithBadDataL: Test: fail with executionBehaviour"));
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, retries, info));	// this should fail with executionBehaviour
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	severity = ECmdCriticalSeverity;
	info.SetL(KEmptyFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestNewLWithBadDataL: Test: fail with empty filename"));	
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, retries, info));	// this should fail with empty filename
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	ordinal = 0;	// invalid
	info.SetL(KFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestNewLWithBadDataL: Test: fail with ordinal"));	
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, retries, info));	// this should fail with ordinal
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	ordinal = -5;	// invalid
	info.SetL(KFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestNewLWithBadDataL: Test: fail with ordinal"));	
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, retries, info));	// this should fail with ordinal
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	retries = -5;	// invalid
	ordinal = 1;
	info.SetL(KFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestNewLWithBadDataL: Test: fail with negative retries"));	
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, retries, info));	// this should fail with negative retries
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/


void CCmdTestLoadSup::doTestInternalizeLWithGoodDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;

	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TInt16 retries = 5;
	_LIT(KFileName, "tcmdgoodsup.dll");
	TUid uid;
	uid.iUid = 9;
	TInt32 ordinal = 1;
	
	TSsmSupInfo info;
	info.SetL(KFileName, ordinal, uid);

	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL:01 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo)"));

	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, retries, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);	

	severity = ECmdIgnoreFailure;
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, retries, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);	
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/


void CCmdTestLoadSup::doTestInternalizeLWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrArgument;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TInt16 retries = 5;
	TInt32 ordinal = 1;
		
	TSsmSupInfo info;
	_LIT(KEmptyFileName, "");
	_LIT(KFileName, "tcmdgoodsup.dll");
	TUid uid;
	uid.iUid = 9;

	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL:01 Test: InternalizeL"));

	executionBehaviour = static_cast<TSsmExecutionBehaviour>(ESsmFireAndForget-1);
	info.SetL(KFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestInternalizeLWithBadDataL: Test: fail with executionBehaviour"));
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, retries, info));	// this should fail with executionBehaviour
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	executionBehaviour = static_cast<TSsmExecutionBehaviour>(100);
	info.SetL(KFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestInternalizeLWithBadDataL: Test: fail with executionBehaviour"));
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, retries, info));	// this should fail with executionBehaviour
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	severity = ECmdCriticalSeverity;
	info.SetL(KEmptyFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestInternalizeLWithBadDataL: Test: fail with empty filename"));	
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, retries, info));	// this should fail with empty filename
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	
	ordinal = 0;	// invalid
	info.SetL(KFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestInternalizeLWithBadDataL: Test: fail with ordinal"));	
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, retries, info));	// this should fail with ordinal
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	ordinal = -5;	// invalid
	info.SetL(KFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestInternalizeLWithBadDataL: Test: fail with ordinal"));	
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, retries, info));	// this should fail with ordinal
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	retries = -5;
	ordinal = 1;
	info.SetL(KFileName, ordinal, uid);
	INFO_PRINTF1(_L("doTestInternalizeLWithBadDataL: Test: fail with negative retries"));	
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, retries, info));	// this should fail with negative retries
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/



void CCmdTestLoadSup::doTestResourceReaderWithGoodDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	// these values must match what is in the resource file
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;

	TUid uid;
	uid.iUid = 0;
	TInt32 ordinal = 1;
	TInt16 retries = 5;
		
	TSsmSupInfo info;
	info.SetL(KFileName, ordinal, uid);

	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL:01 Test: ExternalizeL"));

	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP1);  
	
	// construct command
	TSsmCommandParameters params(reader, NULL, 0);
	CCmdLoadSup* loadSupCmd = CCmdLoadSup::NewL(params);
	CleanupStack::PushL(loadSupCmd);

	TEST(loadSupCmd->Type() == ESsmCmdLoadSup);
		
	// create output buffer
 	CBufFlat* writebuf = CBufFlat::NewL(1024);
	CleanupStack::PushL(writebuf);	
	
	// create output stream on buffer
	RBufWriteStream writeStream(*writebuf);
	CleanupClosePushL(writeStream);	

	// externalise the data
	TRAP(err, loadSupCmd->ExternalizeL(writeStream));
	INFO_PRINTF3(_L("ExternalizeL completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	// check the data is correct
	CompareInputAndOutputL(severity, executionBehaviour, retries, info, writebuf);
	
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(writebuf);	
	CleanupStack::PopAndDestroy(loadSupCmd);
	FreeResources();	
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/


void CCmdTestLoadSup::doTestResourceReaderWithMissingDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNotSupported;

	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL:01"));
	
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP1);   
	
	// read some of the data so there will not be enough to read
	reader.ReadInt32L();

	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);	// could fail the version test due to the incorrect read of bytes

	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/

void CCmdTestLoadSup::doTestResourceReaderWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNotSupported;
	
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL:01"));

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:01: Invalid Type"));
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP2);  
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:02: Invalid Version"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP3);  
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:03: Invalid executionBehaviour 1"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP4);  
	TSsmCommandParameters params2(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params2));
	expectedErr = KErrArgument;
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:04: Invalid executionBehaviour 2"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP5);  
	TSsmCommandParameters params3(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);	
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:05: Invalid ordinal"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP6);  
	TSsmCommandParameters params4(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params4));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:06: Negative ordinal"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP14);
	TSsmCommandParameters params5(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params5));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err==expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:07: Missing filename"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP7);
	TSsmCommandParameters params6(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params6));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:08: Negative retries"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_LOAD_SUP12);
	TSsmCommandParameters params7(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params7));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/


void CCmdTestLoadSup::doTestCommandTypeL()
	{
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TInt16 retries = 5;
		
	TUid uid;
	uid.iUid = 9;
	TInt32 ordinal = 1;
	
	TSsmSupInfo info;
	info.SetL(KFileName, ordinal, uid);

	INFO_PRINTF1(_L("Test:doTestCommandTypeL:01"));

	CCmdLoadSup* loadSupCmd = CCmdLoadSup::NewL(severity, executionBehaviour, retries, info);
	CleanupStack::PushL(loadSupCmd);	

	TEST(loadSupCmd->Type() == ESsmCmdLoadSup);

	CleanupStack::PopAndDestroy(loadSupCmd);
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/

void CCmdTestLoadSup::doTestCmdFailureIgnoreSeverityL()
	{
	TInt err = KErrNone;	
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;

	TUid uid;
	uid.iUid = 0;
	TInt32 ordinal = 1;
	TInt16 retries = 5;
	TSsmSupInfo info;
	info.SetL(KFileName, ordinal, uid);
	
	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL:01: ESsmWaitForSignal"));
	TRAP(err, LoadSupAndExecuteL(severity, info, ESsmWaitForSignal, retries, expectedErr, EFalse, 0));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:02: ESsmDeferredWaitForSignal"));
	TRAP(err, LoadSupAndExecuteL(severity, info, ESsmDeferredWaitForSignal, retries, expectedErr, EFalse, 0));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
	TEST(err == expectedErr);

	}

/**
Old Test CaseID 		AFSS-CMDSUP-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/

void CCmdTestLoadSup::doTestCmdFailureHighSeverityL()
	{
	TInt err = KErrNone;	
	TInt expectedErr = KErrNotFound;
	TInt expectedResult = KErrNone;	
	TCmdErrorSeverity severity = ECmdHighSeverity;

	TUid uid;
	uid.iUid = 0;
	TInt32 ordinal = 1;
	TInt16 retries = 5;
	TSsmSupInfo info;
	info.SetL(KNotExistFileName, ordinal, uid);
	
	INFO_PRINTF1(_L("Test:doTestCmdFailureHighSeverityL:01: ESsmWaitForSignal"));
	TRAP(err, LoadSupAndExecuteL(severity, info, ESsmWaitForSignal, retries, expectedErr, EFalse, 0));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	INFO_PRINTF1(_L("doTestCmdFailureHighSeverityL:02: ESsmDeferredWaitForSignal"));
	TRAP(err, LoadSupAndExecuteL(severity, info, ESsmDeferredWaitForSignal, retries, expectedErr, EFalse, 0));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/

void CCmdTestLoadSup::doTestCancelCmdIgnoreSeverityL()
	{
	TInt err = KErrNone;	
	TInt expectedErr = KErrCancel;
	TInt expectedResult = KErrNone;	
	TCmdErrorSeverity severity = ECmdIgnoreFailure;

	TUid uid;
	uid.iUid = 0;
	TInt32 ordinal = 1;
	TInt16 retries = 5;
	TSsmSupInfo info;
	info.SetL(KFileName, ordinal, uid);
	
	INFO_PRINTF1(_L("Test:doTestCancelCmdIgnoreSeverityL:01: ESsmWaitForSignal"));
	TRAP(err, LoadSupAndExecuteL(severity, info, ESsmWaitForSignal, retries, expectedErr, ETrue, 0));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	INFO_PRINTF1(_L("doTestCancelCmdIgnoreSeverityL:02: ESsmDeferredWaitForSignal"));
	TRAP(err, LoadSupAndExecuteL(severity, info, ESsmDeferredWaitForSignal, retries, expectedErr, ETrue, 0));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/

void CCmdTestLoadSup::doTestCancelCmdHighSeverityL()
	{
	TInt err = KErrNone;	
	TInt expectedErr = KErrCancel;
	TInt expectedResult = KErrNone;	
	TCmdErrorSeverity severity = ECmdHighSeverity;

	TUid uid;
	uid.iUid = 0;
	TInt32 ordinal = 1;
	TInt16 retries = 5;
	TSsmSupInfo info;
	info.SetL(KFileName, ordinal, uid);
	
	INFO_PRINTF1(_L("Test:doTestCancelCmdHighSeverityL:01: ESsmWaitForSignal"));
	TRAP(err, LoadSupAndExecuteL(severity, info, ESsmWaitForSignal, retries, expectedErr, ETrue, 0));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:02: ESsmDeferredWaitForSignal"));
	TRAP(err, LoadSupAndExecuteL(severity, info, ESsmDeferredWaitForSignal, retries, expectedErr, ETrue, 0));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);
	}

/**
Old Test CaseID 		AFSS-CMDSUP-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0009
*/

void CCmdTestLoadSup::doTestNormalExecutionL()
	{
	TInt err = KErrNone;	
	TInt expectedErr = KErrNotFound;

	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TInt16 retries = 5;
	
	TUid uid;
	uid.iUid = 0;
	TInt32 ordinal = 1;

	TSsmSupInfo info;
	info.SetL(KNotExistFileName, ordinal, uid);

	INFO_PRINTF1(_L("Test:doTestNormalExecutionL:01: ESsmWaitForSignal"));

	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddLoadSupCommandL(severity, executionBehaviour, retries, info);
	StartScheduler();
	err = iTestList->Errorcode();
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
	TEST(err == expectedErr); // need SID of State Manager to load a SUP
	delete iTestList;
	iTestList = NULL;
	INFO_PRINTF1(_L("Call to RequestUnLoadSup() is required in many of these tests to load the sup again."));	
	}

/**
 Construct and delete a command
*/ 
void CCmdTestLoadSup::ConstructAndDeleteL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo, const TInt aVersion, const TUint16 aPriority)
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdLoadSup* loadSupCmd = (aVersion == ECmdLoadSupInitialVersion) ? CCmdLoadSup::NewL(aSeverity, aExecutionBehaviour, aRetries, aInfo) : CCmdLoadSup::NewL(aSeverity, aExecutionBehaviour, aRetries, aInfo, aPriority);
#else
	TEST (aPriority == KDefaultPriority);
	TEST (aVersion == ECmdLoadSupInitialVersion);
	CCmdLoadSup* loadSupCmd = CCmdLoadSup::NewL(aSeverity, aExecutionBehaviour, aRetries, aInfo);
#endif
	CleanupStack::PushL(loadSupCmd);

	TEST(loadSupCmd->Severity() == aSeverity);
	TEST(loadSupCmd->Type() == ESsmCmdLoadSup);

	CleanupStack::PopAndDestroy(loadSupCmd);
	}

/**
 Construct and delete a command from resource
*/ 
void CCmdTestLoadSup::ConstructAndDeleteViaReaderL(TSsmCommandParameters& aParams)
	{
	CCmdLoadSup* loadSupCmd = CCmdLoadSup::NewL(aParams);
	CleanupStack::PushL(loadSupCmd);	

	TEST(loadSupCmd->Type() == ESsmCmdLoadSup);

	CleanupStack::PopAndDestroy(loadSupCmd);
	}

/**
 Construct and delete a command using Internalise
*/ 
void CCmdTestLoadSup::CallInternalizeDataTestL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo, const TUint16 aPriority)
	{
	// create a set of data and put it into a buffer
 	CBufFlat* bufferin = CBufFlat::NewL(1024);
	CleanupStack::PushL(bufferin);	
	
	// create write stream on buffer and put the data in
	RBufWriteStream writeStream(*bufferin);
	CleanupClosePushL(writeStream);		
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteInt8L(aExecutionBehaviour);
	aInfo.ExternalizeL(writeStream);
	writeStream.WriteInt16L(aRetries);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	writeStream.CommitL();

	// create a readstream on the data and put the data into the command
	RDesReadStream aReadStream(bufferin->Ptr(0));
	CleanupClosePushL(aReadStream);
	CCmdLoadSup* loadSupCmd = CCmdLoadSup::NewL(aReadStream);
	CleanupStack::PushL(loadSupCmd);

	TEST(loadSupCmd->Type() == ESsmCmdLoadSup);
		
	// create output buffer and stream
 	CBufFlat* bufferout = CBufFlat::NewL(1024);
	CleanupStack::PushL(bufferout);	
	RBufWriteStream writeStream2(*bufferout);
	CleanupClosePushL(writeStream2);		

	// externalise the data
	loadSupCmd->ExternalizeL(writeStream2);
	
	// check the data is correct
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CompareInputAndOutputL(aSeverity, aExecutionBehaviour, aRetries, aInfo, bufferout, aPriority);
#else
	CompareInputAndOutputL(aSeverity, aExecutionBehaviour, aRetries, aInfo, bufferout);
#endif
	
	CleanupStack::PopAndDestroy(&writeStream2);
	CleanupStack::PopAndDestroy(bufferout);	
	CleanupStack::PopAndDestroy(loadSupCmd);
	CleanupStack::PopAndDestroy(&aReadStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(bufferin);	
	}

/**
 Compare input and output
*/ 
void CCmdTestLoadSup::CompareInputAndOutputL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo, CBufFlat* aWriteBuf, const TUint16 aPriority)
	{
	RDesReadStream aReadStream(aWriteBuf->Ptr(0));
	CleanupClosePushL(aReadStream);

	TCmdErrorSeverity severity = static_cast<TCmdErrorSeverity>(aReadStream.ReadUint16L());
	TSsmExecutionBehaviour executionBehaviour = static_cast<TSsmExecutionBehaviour>(aReadStream.ReadUint8L());
	TSsmSupInfo info;
	info.InternalizeL(aReadStream);
	TInt16 retries = aReadStream.ReadInt16L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	TUint16 priority = aReadStream.ReadUint16L();
#endif
	
	INFO_PRINTF4(_L("Original: Severity %d Start Method %d Retries %d"), aSeverity, aExecutionBehaviour, aRetries);
	INFO_PRINTF4(_L("Original: FileName %S NewLOrdinal %d Identity %d"), &aInfo.FileName(), aInfo.NewLOrdinal(), aInfo.Identity().iUid);
	INFO_PRINTF4(_L("Write:    Severity %d Start Method %d Retries %d"), severity, executionBehaviour, retries);
	INFO_PRINTF4(_L("Write:    FileName %S NewLOrdinal %d Identity %d"), &info.FileName(), info.NewLOrdinal(), info.Identity().iUid);
	
	TEST(aSeverity == severity);
	TEST(aExecutionBehaviour == executionBehaviour);
	TEST(aRetries == retries);
	TEST(aInfo.FileName() == info.FileName());
	TEST(aInfo.NewLOrdinal() == info.NewLOrdinal());
	TEST(aInfo.Identity().iUid == info.Identity().iUid);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF2(_L("Original: Priority %d "), aPriority);
	INFO_PRINTF2(_L("Write   : Priority %d "), priority);
	TEST(aPriority == priority);
#else
	TEST(aPriority == KDefaultPriority);
#endif

	CleanupStack::PopAndDestroy(&aReadStream);
	}

/**
 Overloaded factory methods
*/ 
CSsmCommandBase* CCmdTestLoadSup::ConstructCommandFromResourceLC(TSsmCommandType /*aType*/, TSsmCommandParameters& aCommandParameters)
	{
	CSsmCommandBase* cmd = CCmdLoadSup::NewL(aCommandParameters);
	CleanupStack::PushL(cmd);
	return cmd;
	}

/**
 Overloaded factory methods
*/
CSsmCommandBase* CCmdTestLoadSup::ConstructCommandFromStreamLC(TSsmCommandType /*aType*/, RReadStream& aReadStream, TArray<MSsmCommand*>& /*aDeferredList*/)
	{
	CSsmCommandBase* cmd = CCmdLoadSup::NewL(aReadStream);
	CleanupStack::PushL(cmd);
	return cmd;
	}

/**
 Helper function - Load an Sup, delete the created one.
*/
void CCmdTestLoadSup::LoadSupAndExecuteL(TCmdErrorSeverity aSeverity, TSsmSupInfo& aSupInfo, TSsmExecutionBehaviour aExecutionBehaviour, 
													TInt16 aRetries, TInt aExpectedErr, TBool aCancel, TInt32 aTimeout)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddLoadSupCommandL(aSeverity, aExecutionBehaviour, aRetries, aSupInfo);

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
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, aExpectedErr);
	TEST(err == aExpectedErr);
	delete iTestList;
	iTestList = NULL;
	}
