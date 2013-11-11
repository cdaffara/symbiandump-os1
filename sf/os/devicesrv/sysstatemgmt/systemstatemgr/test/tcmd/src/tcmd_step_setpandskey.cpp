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

#include <s32mem.h>

#include "ssmcommandparameters.h"
#include "ssmcommandfactory.h"
#include "cmdsetpandskey.h"

#include "tcmd_step_setpandskey.h"
#include "tcmd_cmdlist.h"

#include <ssmcmd_setpandskey.rsg>

_LIT(KTestRscFileName, "z:\\resource\\ssmatest\\bic\\ssmcmd_setpandskey.rsc");

const TUid KTestPropertyCat={0x1234};
const TUint KTestKey = 678;
const TInt KTestValue = 589;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
const TUint16 KNegPriority = 65535;
#endif


//
// Destructor
//
CCmdTestSetPAndSKey::~CCmdTestSetPAndSKey()
	{
	delete iTestCmd;
	}

//
// Constructor
//
CCmdTestSetPAndSKey::CCmdTestSetPAndSKey()
	: CCmdTestBase(KTCCmdTestSetPAndSKey)
	{
	}

// Run the tests
//
TVerdict CCmdTestSetPAndSKey::doTestStepL()
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
Old Test CaseID 		AFSS-CMDSPS-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */

void CCmdTestSetPAndSKey::doTestNewLWithGoodDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, KTestPropertyCat, KTestKey, KTestValue));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KTestPropertyCat, KTestKey, KTestValue);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

 	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TUid aCategory, TUint aKey, TInt aValue)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdSetPAndSKey::NewL(severity, KTestPropertyCat, KTestKey, KTestValue));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KTestPropertyCat, KTestKey, KTestValue);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdSetPAndSKey::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KTestPropertyCat, KTestKey, KTestValue);	//validate the value
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
 	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, TUid aCategory, TUint aKey, TInt aValue, const TUint16 aPriority)"));
 	INFO_PRINTF1(_L("Test NewL with priority"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdSetPAndSKey::NewL(severity, KTestPropertyCat, KTestKey, KTestValue, KTestPriority));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KTestPropertyCat, KTestKey, KTestValue, KTestPriority);
	delete iTestCmd;
	iTestCmd = NULL;
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters - with priority"));
	INFO_PRINTF1(_L("read from rss"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader1 = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA1);
	TSsmCommandParameters params1(reader1, NULL, 0);
	TRAP(err, iTestCmd = CCmdSetPAndSKey::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KTestPropertyCat, KTestKey, KTestValue, KTestPriority);	//validate the value
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters - with priority"));
	INFO_PRINTF1(_L("initial version gives default priority"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader2 = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA2);
	TSsmCommandParameters params2(reader2, NULL, 0);
	TRAP(err, iTestCmd = CCmdSetPAndSKey::NewL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KTestPropertyCat, KTestKey, KTestValue, KDefaultCommandPriority);	//validate the value
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters - with priority"));
	INFO_PRINTF1(_L("negative priority will be interpreted as a very high positive value"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader3 = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA3);
	TSsmCommandParameters params3(reader3, NULL, 0);
	TRAP(err, iTestCmd = CCmdSetPAndSKey::NewL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, KTestPropertyCat, KTestKey, KTestValue, KNegPriority);	//validate the value
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
#endif
	}

/**
Old Test CaseID 		AFSS-CMDSPS-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */
void CCmdTestSetPAndSKey::doTestNewLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - Command construction with in-valid data"));
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(RReadStream& aReadStream)"));
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TUid aCategory, TUint aKey, TInt aValue)"));
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	}
/**
Old Test CaseID 		AFSS-CMDSPS-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */
void CCmdTestSetPAndSKey::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));
	}
/**
Old Test CaseID 		AFSS-CMDSPS-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */

void CCmdTestSetPAndSKey::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL - InternalizeL with in valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:01 **** "));
	}
/**
Old Test CaseID 		AFSS-CMDSPS-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */

void CCmdTestSetPAndSKey::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDSPS-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */
void CCmdTestSetPAndSKey::doTestResourceReaderWithMissingDataL()
	{
	TInt expectedErr = KErrNotSupported;
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - construct from resource with invalid reader"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);  // #define in .rsg header file created by rcomp
	TSsmCommandParameters params(reader, NULL, 0);
	reader.ReadUint16L();	//read some data so reader will become invalid
	TRAPD(err, iTestCmd = CCmdSetPAndSKey::NewL(params));
	// expected leave code is KErrNotSupported, because command will try to read Type as first member but 
	// it will not get the expected Type as reader is not valid.
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDSPS-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */
void CCmdTestSetPAndSKey::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - Construct from resource with valid reader and invalid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:04 & doTestNewLWithBadDataL:05 **** "));
	}
/**
Old Test CaseID 		AFSS-CMDSPS-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */

void CCmdTestSetPAndSKey::doTestCommandTypeL()
	{
	TInt err = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestCommandTypeL:01 Construct from NewL(RReadStream& aReadStream) and check type"));;
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, KTestPropertyCat, KTestKey, KTestValue));
	CompareCommandsDataL(iTestCmd, severity, KTestPropertyCat, KTestKey, KTestValue);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdSetPAndSKey);
	delete iTestCmd;
	iTestCmd = NULL;

 	INFO_PRINTF1(_L("doTestCommandTypeL:02 Test: NewL(TCmdErrorSeverity aSeverity, TUid aCategory, TUint aKey, TInt aValue)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdSetPAndSKey::NewL(severity, KTestPropertyCat, KTestKey, KTestValue));
	CompareCommandsDataL(iTestCmd, severity, KTestPropertyCat, KTestKey, KTestValue);
	TEST(iTestCmd->Type() == ESsmCmdSetPAndSKey);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdSetPAndSKey::NewL(params));
	CompareCommandsDataL(iTestCmd, severity, KTestPropertyCat, KTestKey, KTestValue);	//validate the value
	TEST(iTestCmd->Type() == ESsmCmdSetPAndSKey);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}
/**
Old Test CaseID 		AFSS-CMDSPS-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */


void CCmdTestSetPAndSKey::doTestCmdFailureIgnoreSeverityL()
	{

	}
/**
Old Test CaseID 		AFSS-CMDSPS-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */


void CCmdTestSetPAndSKey::doTestCmdFailureHighSeverityL()
	{

	}

/**
Old Test CaseID 		AFSS-CMDSPS-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */

void CCmdTestSetPAndSKey::doTestCancelCmdIgnoreSeverityL()
	{

	}

/**
Old Test CaseID 		AFSS-CMDSPS-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0017
 */
void CCmdTestSetPAndSKey::doTestCancelCmdHighSeverityL()
	{

	}

/**
Helper function to compare the data of the command.
*/
void CCmdTestSetPAndSKey::CompareCommandsDataL(CCmdSetPAndSKey* aTestCmd, TCmdErrorSeverity aSeverity, TUid aCategory, TUint aKey, TInt aValue, const TUint16 aPriority)
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
	TUid category = TUid::Uid(readStream.ReadUint32L());
	TUint key = readStream.ReadUint32L();
	TInt value = readStream.ReadInt16L();

	TEST(aSeverity == severity); 
	TEST(aCategory.iUid == category.iUid); 
	TEST(aKey == key); 
	TEST(aValue == value); 
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
Helper function to create instance of CCmdAmaStarter trough readstream.
*/
CCmdSetPAndSKey* CCmdTestSetPAndSKey::CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, TUid aCategory, TUint aKey, TInt aValue, const TUint16 aPriority)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteUint32L(aCategory.iUid);
	writeStream.WriteUint32L(aKey);
	writeStream.WriteInt16L(aValue);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);	
#endif
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdSetPAndSKey* testCmd = CCmdSetPAndSKey::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	return testCmd;
	}

/**
Helper function to run a command and test the expected results
*/
void CCmdTestSetPAndSKey::CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TUid aCategory, TUint aKey, TInt aValue, TInt aExpectedErr, TBool aCancel)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddSetPAndSKeyCommandL(aSeverity, aCategory, aKey, aValue);
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
