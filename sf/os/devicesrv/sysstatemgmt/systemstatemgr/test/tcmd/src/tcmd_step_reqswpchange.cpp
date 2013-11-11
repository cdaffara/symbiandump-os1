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

#include <ssmcmd_reqswpchange1.rsg>
#include <e32property.h>
#include <ssm/ssmsystemwideproperty.h>

#include "cmdreqswpchange.h"
#include "tcmd_step_reqswpchange.h"
#include "tcmd_cmdlist.h"
#include "ssmcommandparameters.h"


//We require two SwP keys.For normal execution we need to define the key and for rest of the cases SwP key
//does not need to be define.Running test with one key, more than one time will give Kern Exec error(in HighSeverity).
static const TUint KChangeSwpKeyDefined = 0xAABB;
static const TUint KChangeSwpKeyNotDefined = 0xAAAA;


static const TUint KSwpNullKey = 0;
static const TInt KSwpKeyValue = 876;
_LIT(KPolicyFilename, "ssm.swp.policy.test.cmdreqswp");

_LIT(KRscFileBuffer, "z:\\resource\\ssmatest\\bic\\ssmcmd_reqswpchange1.rsc");
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
#endif
	
/**
Constructor
*/
CCmdTestReqSwpChange::CCmdTestReqSwpChange()
	: CCmdTestBase(KTCCmdTestReqSwpChange)
	{
	}

/**
Destructor
*/
CCmdTestReqSwpChange::~CCmdTestReqSwpChange()
	{
	}

/**
Run the tests
*/
TVerdict CCmdTestReqSwpChange::doTestStepL()
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
Old Test CaseID 		AFSS-CMDRPC-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestNewLWithGoodDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;

	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TSsmSwp info(KSwpNullKey, 0);

	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo)"));
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, info););
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	severity = ECmdIgnoreFailure;
	info.Set(KChangeSwpKeyNotDefined, KSwpKeyValue);
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo)"));
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_REQ_SWP1);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:05 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, info, KTestPriority););
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters with priority too)"));
	INFO_PRINTF1(_L("with  valid priority"));
	RResourceReader& reader1 = LoadResourcesL(KRscFileBuffer, R_COMMAND_REQ_SWP7);
	TSsmCommandParameters params1(reader1, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters with priority too)"));
	INFO_PRINTF1(_L("initial version gives default priority"));
	RResourceReader& reader2 = LoadResourcesL(KRscFileBuffer, R_COMMAND_REQ_SWP8);
	TSsmCommandParameters params2(reader2, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	
#endif
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestNewLWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrArgument;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = static_cast<TSsmExecutionBehaviour>(0);	
	TSsmSwp info(KSwpNullKey, 0);

	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - Command construction with Invalid data"));

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, CallInternalizeDataTestL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo)"));
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	executionBehaviour = static_cast<TSsmExecutionBehaviour>(10);
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:03 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo)"));
	TRAP(err, ConstructAndDeleteL(severity, executionBehaviour, info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:04 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_REQ_SWP4);	// invalid execution_behaviour 1
	TSsmCommandParameters params3(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_REQ_SWP5);	// invalid execution_behaviour 2
	TSsmCommandParameters params4(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params4));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	expectedErr = KErrNotSupported;
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_REQ_SWP2);	// invalid type
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_REQ_SWP3);	// unsupported version
	TSsmCommandParameters params2(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL - InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:02 and 03 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL - InternalizeL with Invalid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:01 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:04 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestResourceReaderWithMissingDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - InternalizeL with Invalid data"));
	TInt expectedErr = KErrNotSupported;
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_REQ_SWP1);   

	// read some of the data so there will not be enough to read
	reader.ReadInt32L();

	TSsmCommandParameters params(reader, NULL, 0);
	TRAPD(err, ConstructAndDeleteViaReaderL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	
	TEST(err == expectedErr);
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:	AFSS-CMDRPC-0002 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestCommandTypeL()
	{
	TInt expectedType = ESsmCmdReqSwProperty;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TSsmSwp swpInfo(KSwpNullKey, 0);

	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Check command type"));
	CCmdReqSwpChange* reqSwp = CCmdReqSwpChange::NewL(severity, executionBehaviour, swpInfo);
	CleanupStack::PushL(reqSwp);	
	TEST(reqSwp->Type() == expectedType);
	INFO_PRINTF3(_L("Test completed with Swp type : %d. expected Swp Type : %d"), reqSwp->Type(), expectedType);
	CleanupStack::PopAndDestroy(reqSwp);

	// From Rss
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_REQ_SWP1);
	TSsmCommandParameters params(reader, NULL, 0);
	reqSwp = CCmdReqSwpChange::NewL(params);
	CleanupStack::PushL(reqSwp);
	TEST(reqSwp->Type() == expectedType);
	INFO_PRINTF3(_L("Test completed with Swp type : %d. expected Swp Type : %d"), reqSwp->Type(), expectedType);
	FreeResources();
	CleanupStack::PopAndDestroy(reqSwp);
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestCmdFailureIgnoreSeverityL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TSsmSwp info(KChangeSwpKeyNotDefined, KSwpKeyValue);
	
	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL:01 - executionBehaviour = ESsmWaitForSignal"));
	TRAP(err, ReqSwpChangeAndExecuteL(severity, info, executionBehaviour, expectedErr, EFalse));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
	TEST(err == expectedErr);

	executionBehaviour = ESsmDeferredWaitForSignal;
	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL:02 - executionBehaviour = ESsmDeferredWaitForSignal"));
	TRAP(err, ReqSwpChangeAndExecuteL(severity, info, executionBehaviour, expectedErr, EFalse, 1000));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
	TEST(err == expectedErr);
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestCmdFailureHighSeverityL()
	{
	TInt expectedErr = KErrNotFound;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TSsmSwp info(KChangeSwpKeyNotDefined, KSwpKeyValue);

	INFO_PRINTF1(_L("Test:doTestCmdFailureHighSeverityL:01 - executionBehaviour = ESsmWaitForSignal"));
	ReqSwpChangeAndExecuteL(severity, info, executionBehaviour, expectedErr, EFalse);

	executionBehaviour = ESsmDeferredWaitForSignal;
	INFO_PRINTF1(_L("Test:doTestCmdFailureHighSeverityL:02 - executionBehaviour = ESsmDeferredWaitForSignal"));
	ReqSwpChangeAndExecuteL(severity, info, executionBehaviour, expectedErr, EFalse, 1000);
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestCancelCmdIgnoreSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TSsmSwp info(KChangeSwpKeyNotDefined, KSwpKeyValue);

	ReqSwpChangeAndExecuteL(severity, info, executionBehaviour, expectedErr, ETrue);
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestCancelCmdHighSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TSsmSwp info(KChangeSwpKeyNotDefined, KSwpKeyValue);

	ReqSwpChangeAndExecuteL(severity, info, executionBehaviour, expectedErr, ETrue);
	}

/**
Old Test CaseID 		AFSS-CMDRPC-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0015
*/

void CCmdTestReqSwpChange::doTestNormalExecutionL()
	{
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdCriticalSeverity;	// It should be critical to ensure this test works properly, ignorefailure will pass silently otherwise.
	TSsmExecutionBehaviour executionBehaviour = ESsmWaitForSignal;
	TSsmSwp info(KChangeSwpKeyDefined, KSwpKeyValue);
	TPtrC filename = KPolicyFilename();

	RSsmSystemWideProperty swpNotification;
	TInt err = swpNotification.Connect(KChangeSwpKeyDefined);
	CleanupClosePushL(swpNotification);
	TEST(err == KErrNone);

	// create a swp	here
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);

	iTestList->AddCreateSwpCommandL(severity, info, filename);
	iTestList->AddReqSwpChangeCommandL(severity, executionBehaviour, info);
	
	StartScheduler();
	err = iTestList->Errorcode();
	INFO_PRINTF4(_L("Command execution finished with error : %d, expectedErr : %d, for Key : %x. "), err, expectedErr, info.Key());
	TEST(err == expectedErr || err == KErrAlreadyExists);
	delete iTestList;
	iTestList = NULL;
	
	TInt value = 0;
	swpNotification.GetValue(value);
	//subscribe for notification if the value is not changed.
    if ( value != KSwpKeyValue )
        {
        TRequestStatus status;
        swpNotification.Subscribe(status);
        TEST(status == KRequestPending);
        User::WaitForRequest(status);
        INFO_PRINTF2(_L("status %d. "), status.Int());
        INFO_PRINTF2(_L("Subscribe for the swp change notification Completed with %d. "), status.Int());
        TEST(status.Int() == KErrNone);
              
        swpNotification.GetValue(value);
        INFO_PRINTF2(_L("swp Actual Value %d: Expected Value 876"), value);
        TEST(value == KSwpKeyValue);
        }
    else
        {
        INFO_PRINTF2(_L("Swp value set is %d"), value);
        }

	CleanupStack::PopAndDestroy(&swpNotification);
	}

/**
 Helper function - Construct and delete a command.
*/
void CCmdTestReqSwpChange::ConstructAndDeleteL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwpInfo, const TInt aVersion, const TUint16 aPriority)
	{
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdReqSwpChange* reqSwp = (aVersion == ECmdReqSwPropertyInitialVersion) ? CCmdReqSwpChange::NewL(aSeverity, aExecutionBehaviour, aSwpInfo) : CCmdReqSwpChange::NewL(aSeverity, aExecutionBehaviour, aSwpInfo, aPriority);
#else
	TEST (aPriority == KDefaultPriority);
	TEST (aVersion == ECmdReqSwPropertyInitialVersion);
	CCmdReqSwpChange* reqSwp = CCmdReqSwpChange::NewL(aSeverity, aExecutionBehaviour, aSwpInfo);
#endif

	CleanupStack::PushL(reqSwp);	

	TEST(reqSwp->Severity()==aSeverity);
	TEST(reqSwp->Type()==ESsmCmdReqSwProperty);
	
	CleanupStack::PopAndDestroy(reqSwp);
	}

/**
 Helper function - Construct and delete a command from resource.
*/
void CCmdTestReqSwpChange::ConstructAndDeleteViaReaderL(TSsmCommandParameters& aParams)
	{
	CCmdReqSwpChange* reqSwp = CCmdReqSwpChange::NewL(aParams);
	CleanupStack::PushL(reqSwp);	

	TEST(reqSwp->Type() == ESsmCmdReqSwProperty);
	
	CleanupStack::PopAndDestroy(reqSwp);
	}

/**
 Helper function - Construct and delete a command using Internalise.
*/
void CCmdTestReqSwpChange::CallInternalizeDataTestL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwpInfo, const TUint16 aPriority)
	{
	// create a set of data and put it into a buffer
 	CBufFlat* bufferin = CBufFlat::NewL(1024);
	CleanupStack::PushL(bufferin);	
	
	// create write stream on buffer and put the data in
	RBufWriteStream writeStream(*bufferin);
	CleanupClosePushL(writeStream);		
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteInt8L(aExecutionBehaviour);
	writeStream.WriteUint32L(aSwpInfo.Key());
	writeStream.WriteInt32L(aSwpInfo.Value());
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	writeStream.CommitL();
	
	// create a readstream on the data and put the data into the command
	RDesReadStream aReadStream(bufferin->Ptr(0));
	CleanupClosePushL(aReadStream);

	CCmdReqSwpChange* reqSwp = CCmdReqSwpChange::NewL(aReadStream);
	CleanupStack::PushL(reqSwp);
	
	TEST(reqSwp->Type()==ESsmCmdReqSwProperty);
	
	// create output buffer and stream
 	CBufFlat* bufferout = CBufFlat::NewL(1024);
	CleanupStack::PushL(bufferout);	
	RBufWriteStream writeStream2(*bufferout);
	CleanupClosePushL(writeStream2);		

	// externalise the data
	reqSwp->ExternalizeL(writeStream2);
	
	// check the data is correct
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CompareInputAndOutputL(aSeverity, aExecutionBehaviour, aSwpInfo, bufferout, aPriority);
#else
	CompareInputAndOutputL(aSeverity, aExecutionBehaviour, aSwpInfo, bufferout);
#endif
	
	CleanupStack::PopAndDestroy(&writeStream2);
	CleanupStack::PopAndDestroy(bufferout);	
	CleanupStack::PopAndDestroy(reqSwp);
	CleanupStack::PopAndDestroy(&aReadStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(bufferin);	
	}

/**
 Helper function - Compare input and output.
*/
void CCmdTestReqSwpChange::CompareInputAndOutputL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwpInfo, CBufFlat* aWriteBuf, const TUint16 aPriority)
	{
	RDesReadStream aReadStream(aWriteBuf->Ptr(0));
	CleanupClosePushL(aReadStream);

	TCmdErrorSeverity severity = static_cast<TCmdErrorSeverity>(aReadStream.ReadUint16L());
	TSsmExecutionBehaviour executionBehaviour = static_cast<TSsmExecutionBehaviour>(aReadStream.ReadUint8L());
	const TUint32 key = aReadStream.ReadUint32L();
	TInt  value = aReadStream.ReadInt32L();

	INFO_PRINTF5(_L("Original: Severity %d ExecutionBehaviour %d Key %d Value %d"), aSeverity, aExecutionBehaviour, aSwpInfo.Key(), aSwpInfo.Value());
	INFO_PRINTF5(_L("Got:      Severity %d ExecutionBehaviour %d Key %d Value %d"), severity, executionBehaviour, key, value);

	TEST(aSeverity == severity);
	TEST(aExecutionBehaviour == executionBehaviour);
	TEST(aSwpInfo.Key() == key);
	TEST(aSwpInfo.Value() == value);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	const TUint16 priority = aReadStream.ReadUint16L();
	INFO_PRINTF2(_L("Original: priority %d"), aPriority);
	INFO_PRINTF2(_L("Got:      priority %d"), priority);
	TEST(aPriority == priority);
#else
	TEST(aPriority == KDefaultPriority);
#endif

	CleanupStack::PopAndDestroy(&aReadStream);
	}

/**
 Helper function - Overloaded factory methods.
*/
CSsmCommandBase* CCmdTestReqSwpChange::ConstructCommandFromResourceLC(TSsmCommandType /*aType*/, TSsmCommandParameters& aCommandParameters)
	{
	CSsmCommandBase* cmd = CCmdReqSwpChange::NewL(aCommandParameters);
	CleanupStack::PushL(cmd);
	return cmd;
	}

CSsmCommandBase* CCmdTestReqSwpChange::ConstructCommandFromStreamLC(TSsmCommandType /*aType*/, RReadStream& aReadStream, TArray<MSsmCommand*>& /*aDeferredList*/)
	{
	CSsmCommandBase* cmd = CCmdReqSwpChange::NewL(aReadStream);
	CleanupStack::PushL(cmd);
	return cmd;
	}

/**
 Helper function - Define an (RProperty) Swp, Create one using values supplied, delete the created one.
*/
void CCmdTestReqSwpChange::ReqSwpChangeAndExecuteL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, TSsmExecutionBehaviour aExecutionBehaviour, 
													TInt aExpectedErr, TBool aCancel, TInt32 aTimeout)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddReqSwpChangeCommandL(aSeverity, aExecutionBehaviour, aSwpInfo);

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
