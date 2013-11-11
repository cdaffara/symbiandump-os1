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
#include <e32property.h>
#include <ssm/ssmstatemanager.h>
#include <ssmcmd_createswp1.rsg>

#include "cmdcreateswp.h"
#include "tcmd_step_createswp.h"
#include "tcmd_cmdlist.h"
#include "ssmcommandparameters.h"
#include "ssmcommandfactory.h"

_LIT(KRscFileBuffer, "z:\\resource\\ssmatest\\bic\\ssmcmd_createswp1.rsc");
_LIT(KPolicyFilename, "ssm.swp.policy.test.cmdreqswp");

static const TUint KCreateSwpKey = {0xEEEE};
static const TInt KSwpValue = 876;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
#endif

CCmdTestCreateSwp::CCmdTestCreateSwp()
	: CCmdTestBase(KTCCmdTestCreateSwp)
	{
	}

CCmdTestCreateSwp::~CCmdTestCreateSwp()
	{
	}

TVerdict CCmdTestCreateSwp::doTestStepL()
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
Old Test CaseID 		AFSS-CMDCSP-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestNewLWithGoodDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;

	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmSwp info(KCreateSwpKey, KSwpValue);
	TPtrC filename = KPolicyFilename();

	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, CallInternalizeDataTestL(severity, info, filename));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo, const TDesC& aFilename)"));
	TRAP(err, ConstructAndDeleteL(severity, info, filename););
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP1);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo, const TDesC& aFilename, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Construct a command with valid priority "));
	TRAP(err, ConstructAndDeleteL(severity, info, filename, ECmdCreateSwpVersionWithPriority, KTestPriority););
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("Read valid priority from list"));
	RResourceReader& reader1 = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP6);
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("Set to default priority as version is initial"));
	RResourceReader& reader2 = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP7);
	TSsmCommandParameters params2(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("Negative Priority will be very high positive value"));
	RResourceReader& reader3 = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP8);
	TSsmCommandParameters params3(reader3, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();	
#endif
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestNewLWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrArgument;
	_LIT(KMissingPolicyFilename, "");
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmSwp info(KCreateSwpKey, KSwpValue);
	TPtrC filename = KMissingPolicyFilename();

	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - Command construction with Invalid data"));

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, CallInternalizeDataTestL(severity, info, filename));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo, const TDesC& aFilename)"));
	TRAP(err, ConstructAndDeleteL(severity, info, filename););
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP4);	// missing file name
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/


void CCmdTestCreateSwp::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL - InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));	
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL - InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:01 **** "));	
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - Test Resource Reader with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestResourceReaderWithMissingDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - Test Resource Reader with Invalid data"));
	TInt expectedErr = KErrNotSupported;
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP1);   
	
	// read some of the data so there will not be enough to read
	reader.ReadInt32L();

	TSsmCommandParameters params(reader, NULL, 0);
	TRAPD(err, ConstructAndDeleteViaReaderL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);	
	TEST(expectedErr == err);
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - Test Resource Reader with Bad data"));
	TInt expectedErr = KErrNotSupported;

	INFO_PRINTF1(_L("Test Invalid Command Type"));
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP2);   // invalid type
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAPD(err, ConstructAndDeleteViaReaderL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(expectedErr == err);
	FreeResources();

	INFO_PRINTF1(_L("Test Unsupported version"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP3);   // unsupported version
	TSsmCommandParameters params2(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);	
	TEST(expectedErr == err);
	FreeResources();

	INFO_PRINTF1(_L("Test Missing File Name"));
	expectedErr = KErrArgument;
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP4);   // missing filename
	TSsmCommandParameters params3(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);	
	TEST(expectedErr == err);
	FreeResources();

	INFO_PRINTF1(_L("Test valid filename with path"));
	expectedErr = KErrNone;
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP5);   // filename with path
	TSsmCommandParameters params4(reader, NULL, 0);
	TRAP(err, ConstructAndDeleteViaReaderL(params4));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(expectedErr == err);
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestCommandTypeL()
	{
	TInt expectedType = ESsmCmdCreateSwp;
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Check command type"));
	TCmdErrorSeverity severity = ECmdCriticalSeverity;
	TSsmSwp swpInfo(KCreateSwpKey, KSwpValue);
	TPtrC filename = KPolicyFilename();

	CCmdCreateSwp* swp = CCmdCreateSwp::NewL(severity, swpInfo, filename);
	CleanupStack::PushL(swp);	
	TEST(swp->Type() == expectedType);
	INFO_PRINTF3(_L("Test completed with Swp type : %d. expected Swp Type : %d"), swp->Type(), expectedType);
	CleanupStack::PopAndDestroy(swp);
	
	// From Rss
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CREATE_SWP5);
	TSsmCommandParameters params(reader, NULL, 0);
	swp = CCmdCreateSwp::NewL(params);
	CleanupStack::PushL(swp);
	TEST(swp->Type() == expectedType);
	INFO_PRINTF3(_L("Test completed with Swp type : %d. expected Swp Type : %d"), swp->Type(), expectedType);
	FreeResources();
	CleanupStack::PopAndDestroy(swp);
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestCmdFailureIgnoreSeverityL()
	{
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmSwp info(KCreateSwpKey, KSwpValue);
	TPtrC filename = KPolicyFilename();

	CreateSwpAndExecuteL(severity, info, filename, expectedErr, EFalse);
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestCmdFailureHighSeverityL()
	{
	TInt expectedErr = KErrAlreadyExists;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TSsmSwp info(KCreateSwpKey, KSwpValue);
	TPtrC filename = KPolicyFilename();

	CreateSwpAndExecuteL(severity, info, filename, expectedErr, EFalse);
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestCancelCmdIgnoreSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmSwp info(KCreateSwpKey, KSwpValue);
 	TPtrC filename = KPolicyFilename();
 
	CreateSwpAndExecuteL(severity, info, filename, expectedErr, ETrue);
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestCancelCmdHighSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TSsmSwp info(KCreateSwpKey, KSwpValue);
	TPtrC filename = KPolicyFilename();

	CreateSwpAndExecuteL(severity, info, filename, expectedErr, ETrue);
	}

/**
Old Test CaseID 		AFSS-CMDCSP-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0004
*/

void CCmdTestCreateSwp::doTestNormalExecutionL()
	{
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdHighSeverity;	
	TSsmSwp info(0x1234, 123);
	TPtrC filename = KPolicyFilename();

	// delete the key just in case
	TInt err = RProperty::Delete(RProcess().SecureId(), info.Key());
	INFO_PRINTF3(_L("Swp deleted with error: %d for Key : %x. "), err, info.Key());
	TEST(err == KErrNone || err == KErrNotFound);

	INFO_PRINTF2(_L("Creating a new Swp as a command for Key : %x. "), info.Key());
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddCreateSwpCommandL(severity, info, filename);
	StartScheduler();
	err = iTestList->Errorcode();
	INFO_PRINTF4(_L("Command execution finished with error : %d, expectedErr : %d, for Key : %x. "), err, expectedErr, info.Key());
	TEST(err == expectedErr);
	delete iTestList;
	iTestList = NULL;
	// delete the key as we have created it
	err = RProperty::Delete(RProcess().SecureId(), info.Key());
	INFO_PRINTF3(_L("Swp deleted with error: %d for Key : %x. "), err, info.Key());
	TEST(err == KErrNone);
	}

/**
 Helper function - Construct and delete a command
*/
void CCmdTestCreateSwp::ConstructAndDeleteL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, TDesC& aFilename, const TInt aVersion, const TUint16 aPriority)
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdCreateSwp* swp = (aVersion == ECmdCreateSwpInitialVersion) ? CCmdCreateSwp::NewL(aSeverity, aSwpInfo, aFilename) : CCmdCreateSwp::NewL(aSeverity, aSwpInfo, aFilename, aPriority);
#else
	TEST (aPriority == KDefaultPriority);
	TEST (aVersion == ECmdCreateSwpInitialVersion);
	CCmdCreateSwp* swp = CCmdCreateSwp::NewL(aSeverity, aSwpInfo, aFilename);
	
#endif
	CleanupStack::PushL(swp);
		

	TEST(swp->Severity() == aSeverity);
	TEST(swp->Type() == ESsmCmdCreateSwp);

	CleanupStack::PopAndDestroy(swp);
	}

/**
 Helper function - Construct and delete a command from resource
*/ 
void CCmdTestCreateSwp::ConstructAndDeleteViaReaderL(TSsmCommandParameters& aParams)
	{
	CCmdCreateSwp* swp = CCmdCreateSwp::NewL(aParams);
	CleanupStack::PushL(swp);

	TEST(swp->Type()==ESsmCmdCreateSwp);

	CleanupStack::PopAndDestroy(swp);
	}

/**
 Helper function - Compare input and output
 */
void CCmdTestCreateSwp::CompareInputAndOutputL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, const TDesC& aFilename, CBufFlat* aWriteBuf, const TUint16 aPriority)
	{
	RDesReadStream aReadStream(aWriteBuf->Ptr(0));
	CleanupClosePushL(aReadStream);

	TCmdErrorSeverity severity = static_cast<TCmdErrorSeverity>(aReadStream.ReadUint16L());
	TUint key = aReadStream.ReadUint32L();
	TInt32 value = aReadStream.ReadInt32L();
	HBufC* filename = HBufC::NewL(aReadStream, KMaxFileName);
	CleanupStack::PushL(filename);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	TUint16 priority = aReadStream.ReadUint16L();
#endif
	
	TPtrC ptr = filename->Des();
	INFO_PRINTF5(_L("Original: Severity %d Key %d Value %d Filename [%S]"), aSeverity, aSwpInfo.Key(), aSwpInfo.Value(), &aFilename);
	INFO_PRINTF5(_L("Got:      Severity %d Key %d Value %d Filename [%S]"), severity, key, value, &ptr);
	TEST(aSeverity==severity);
	TEST(aSwpInfo.Key()==key);
	TEST(aSwpInfo.Value()==value);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF2(_L("Original: Priority %d"), aPriority);
	INFO_PRINTF2(_L("Got     : Priority %d"), priority);
	TEST(aPriority == priority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	TInt result = aFilename.Compare(*filename);

	CleanupStack::PopAndDestroy(filename);
	CleanupStack::PopAndDestroy(&aReadStream);

	TEST(result == KErrNone);
	}

/**
 Helper function - Use read and write stream to create swp.
 */
void CCmdTestCreateSwp::CallInternalizeDataTestL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, const TDesC& aFilename, const TUint16 aPriority)
	{
	INFO_PRINTF2(_L("Executing test with file [%S]"), &aFilename);
	// create a set of data and put it into a buffer
 	CBufFlat* bufferin = CBufFlat::NewL(1024);
	CleanupStack::PushL(bufferin);

	// create write stream on buffer and put the data in
	RBufWriteStream writeStream(*bufferin);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteUint32L(aSwpInfo.Key());
	writeStream.WriteInt32L(aSwpInfo.Value());
	writeStream << aFilename;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	writeStream.CommitL();

	// create a readstream on the data and put the data into the command
	RDesReadStream aReadStream(bufferin->Ptr(0));
	CleanupClosePushL(aReadStream);
	CCmdCreateSwp* swp = CCmdCreateSwp::NewL(aReadStream);
	CleanupStack::PushL(swp);

	TEST(swp->Type() == ESsmCmdCreateSwp);

	// create output buffer and stream
 	CBufFlat* bufferout = CBufFlat::NewL(1024);
	CleanupStack::PushL(bufferout);
	RBufWriteStream writeStream2(*bufferout);
	CleanupClosePushL(writeStream2);

	// externalise the data
	swp->ExternalizeL(writeStream2);
	CleanupStack::PopAndDestroy(&writeStream2);

	// check the data is correct
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CompareInputAndOutputL(aSeverity, aSwpInfo, aFilename, bufferout, aPriority);
#else
	CompareInputAndOutputL(aSeverity, aSwpInfo, aFilename, bufferout);
#endif

	CleanupStack::PopAndDestroy(bufferout);
	CleanupStack::PopAndDestroy(swp);
	CleanupStack::PopAndDestroy(&aReadStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(bufferin);
	}

/**
 Helper function - Define an (RProperty) Swp, Create one using values supplied, delete the created one.
*/
void CCmdTestCreateSwp::CreateSwpAndExecuteL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, TDesC& aFilename, TInt aExpectedErr, TBool aCancel)
	{
	// create a swp	here
	TInt err = RProperty::Define(RProcess().SecureId(), aSwpInfo.Key(), RProperty::EInt);
	INFO_PRINTF3(_L("A new Swp created with error : %d for Key : %d. "), err, aSwpInfo.Key());
	TEST(err == KErrNone);

	// try to define the key again as a swp
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddCreateSwpCommandL(aSeverity, aSwpInfo, aFilename);
	if (aCancel)
		{
		CreateCancelOnCommands(iTestList);
		}

	StartScheduler();
	err = iTestList->Errorcode();
	INFO_PRINTF4(_L("Command execution finished with error : %d, expectedErr : %d, for Key : %x. "), err, aExpectedErr, aSwpInfo.Key());
	TEST(err == aExpectedErr);
	delete iTestList;
	iTestList = NULL;
	// delete the key
	err = RProperty::Delete(RProcess().SecureId(), aSwpInfo.Key());
	INFO_PRINTF3(_L("Swp deleted with error: %d for Key : %x. "), err, aSwpInfo.Key());
	TEST(err == KErrNone);
	}

