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
#include "tcmd_step_amastarter.h"
#include "cmdamastarter.h"
#include "ssmcommandparameters.h"
#include <ssmcmd_amastarter.rsg>


#include "tcmd_cmdlist.h"

const TUid KTestDscId = {0x1234};
const TUid KTestBadDscId = KNullUid;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
const TUint16 KNegPriority = 65535;
#endif

_LIT(KTestRscFileName, "z:\\resource\\ssmatest\\bic\\ssmcmd_amastarter.rsc");

CCmdTestAMAStarter::~CCmdTestAMAStarter()
	{
	delete iTestCmd;
	}


CCmdTestAMAStarter::CCmdTestAMAStarter()
	: CCmdTestBase(KTCCmdTestAMAStarter)
	{
	}


TVerdict CCmdTestAMAStarter::doTestStepL()
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
Old testcaseID 			AFSS-CMDAMA-0001
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/
void CCmdTestAMAStarter::doTestNewLWithGoodDataL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmExecutionBehaviour executionBehaviour = ESsmFireAndForget;	//valid execution behaviour
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, executionBehaviour, KTestDscId));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, const TUid aDscId)"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdAmaStarter::NewL(severity, executionBehaviour, KTestDscId));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdAmaStarter::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, const TUid aDscId, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Tests the NewL with priority"));
	severity = ECmdCriticalSeverity;
	TRAP(err, iTestCmd = CCmdAmaStarter::NewL(severity, executionBehaviour, KTestDscId, KTestPriority));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId, KTestPriority);
	delete iTestCmd;
	iTestCmd = NULL;
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("Read valid priority from resorce file"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_test_priority
	// open reader
	RResourceReader& reader1 = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_TEST_PRIORITY);
	TSsmCommandParameters params1(reader1, NULL, 0);
	TRAP(err, iTestCmd = CCmdAmaStarter::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId, KTestPriority); //priority same as that in rss file
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	INFO_PRINTF1(_L("Read default priority for initial version of command "));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command__data
	// open reader
	RResourceReader& reader2 = LoadResourcesL(KTestRscFileName, R_COMMAND_FIRST_VERSION);
	TSsmCommandParameters params2(reader2, NULL, 0);
	TRAP(err, iTestCmd = CCmdAmaStarter::NewL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId, KDefaultCommandPriority); //priority should be default
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters) - negative priority"));
	INFO_PRINTF1(_L("negative priority will be interpreted as a very high positive value"));
	// open reader
	RResourceReader& reader3 = LoadResourcesL(KTestRscFileName, R_COMMAND_NEGATIVE_PRIORITY);
	TSsmCommandParameters params3(reader3, NULL, 0);
	TRAP(err, iTestCmd = CCmdAmaStarter::NewL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	// negative priority will not cause any error
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId, KNegPriority); //priority should be very high value
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

#endif
	}

/**
Old testcaseID 			AFSS-CMDAMA-0002
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/

void CCmdTestAMAStarter::doTestNewLWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrArgument;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmExecutionBehaviour executionBehaviour = ESsmFireAndForget;	//valid execution behaviour

	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - Command construction with in-valid data"));

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(RReadStream& aReadStream) - dscId == KNullUid"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, executionBehaviour, KTestBadDscId));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(RReadStream& aReadStream) - invalid execution_behaviour"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity, TSsmExecutionBehaviour(0), KTestDscId));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:03 Test: NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, TUid aDscId) - dscId == KNullUid"));
	TRAP(err, iTestCmd = CCmdAmaStarter::NewL(severity, executionBehaviour, KTestBadDscId));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, TUid aDscId) - invalid execution_behaviour"));
	TRAP(err, iTestCmd = CCmdAmaStarter::NewL(severity, TSsmExecutionBehaviour(0), KTestDscId));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters) - dscId == KNullUid"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_INVALID_DATA);  // #define in .rsg header file created by rcomp
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdAmaStarter::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters) - invalid execution_behaviour"));
	// open reader
	RResourceReader& reader1 = LoadResourcesL(KTestRscFileName, R_COMMAND_INVALID_EXECUTION_BEHAVIOUR);
	TSsmCommandParameters params1(reader1, NULL, 0);
	TRAP(err, iTestCmd = CCmdAmaStarter::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old testcaseID 			AFSS-CMDAMA-0003
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/
void CCmdTestAMAStarter::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));
	}
/**
Old testcaseID 			AFSS-CMDAMA-0004
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/
void CCmdTestAMAStarter::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL - InternalizeL with in valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:01 **** "));
	}

/**
Old testcaseID 			AFSS-CMDAMA-0005
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/
void CCmdTestAMAStarter::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}
/**
Old testcaseID 			AFSS-CMDAMA-0006
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/
void CCmdTestAMAStarter::doTestResourceReaderWithMissingDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - construct from resource with invalid reader"));
	TInt expectedErr = KErrNotSupported;
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, NULL, 0);
	reader.ReadUint16L();	//read some data so reader will become invalid
	TRAPD(err, iTestCmd = CCmdAmaStarter::NewL(params));
	// expected leave code is KErrNotSupported, because command will try to read Type as first member but 
	// it will not get the expected Type as reader is not valid.
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old testcaseID 			AFSS-CMDAMA-0007
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/

void CCmdTestAMAStarter::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - Construct from resource with valid reader and invalid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:04 & doTestNewLWithBadDataL:05 **** "));
	}

/**
Old testcaseID 			AFSS-CMDAMA-0008
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/

void CCmdTestAMAStarter::doTestCommandTypeL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmExecutionBehaviour executionBehaviour = ESsmFireAndForget;	//valid execution behaviour
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestCommandTypeL:01 Construct from NewL(RReadStream& aReadStream) and check type"));
	iTestCmd = CreateCmdFromStreamL(severity, executionBehaviour, KTestDscId);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId);
	TEST(iTestCmd->Type() == ESsmCmdAMAStarter);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:02 Construct from NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, TUid aDscId) and check type"));
	severity = ECmdCriticalSeverity;
	iTestCmd = CCmdAmaStarter::NewL(severity, executionBehaviour, KTestDscId);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId);
	TEST(iTestCmd->Type() == ESsmCmdAMAStarter);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestCommandTypeL:03 Construct from NewL(TSsmCommandParameters& aCommandParameters) and check type"));
	severity = ECmdMediumSeverity;
	// open reader
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, NULL, 0);
	iTestCmd = CCmdAmaStarter::NewL(params);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId);		//validate the value
	TEST(iTestCmd->Type() == ESsmCmdAMAStarter);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestCommandTypeL:02 Construct from NewL(TCmdErrorSeverity aSeverity, TSsmStartMethod aExecutionBehaviour, TUid aDscId, const TUint16 aPriority) and check type"));
	severity = ECmdCriticalSeverity;
	iTestCmd = CCmdAmaStarter::NewL(severity, executionBehaviour, KTestDscId, KTestPriority);
	CompareCommandsDataL(iTestCmd, severity, executionBehaviour, KTestDscId, KTestPriority);
	TEST(iTestCmd->Type() == ESsmCmdAMAStarter);
	delete iTestCmd;
	iTestCmd = NULL;
#endif
	}
/**
Old testcaseID 			AFSS-CMDAMA-0009
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/
void CCmdTestAMAStarter::doTestCmdFailureIgnoreSeverityL()
	{
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test command failure with ECmdIgnoreFailure Severity"));
	
	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:01 Test: Start ama starter which does not exist - ESsmWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmWaitForSignal, KTestDscId, expectedErr);

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:02 Test: Start ama starter which does not exist - ESsmDeferredWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, KTestDscId, expectedErr, EFalse, 20000);
	}

/**
Old testcaseID 			AFSS-CMDAMA-0010
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/
void CCmdTestAMAStarter::doTestCmdFailureHighSeverityL()
	{
	TInt expectedErr = KErrArgument;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	INFO_PRINTF1(_L("Test command failure with ECmdHighSeverity Severity"));

	INFO_PRINTF1(_L("doTestCmdFailureHighSeverityL:02 Test: Start ama starter which does not exist - ESsmWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmWaitForSignal, KTestDscId, expectedErr);

	INFO_PRINTF1(_L("doTestCmdFailureHighSeverityL:03 Test: Start ama starter which does not exist - ESsmDeferredWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, KTestDscId, expectedErr, EFalse, 20000);

	}
/**
Old testcaseID 			AFSS-CMDAMA-0011
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/

void CCmdTestAMAStarter::doTestCancelCmdIgnoreSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdIgnoreFailure Severity and cancel"));

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:01 Test: Start ama starter which does not exist - ESsmWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmWaitForSignal, KTestDscId, expectedErr, ETrue);

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:02 Test: Start ama starter which does not exist - ESsmDeferredWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, KTestDscId, expectedErr, ETrue, 20000);
	}

/**
Old testcaseID 			AFSS-CMDAMA-0012
New	testcaseID			DEVSRVS-SSMA-CMD-0002
*/

void CCmdTestAMAStarter::doTestCancelCmdHighSeverityL()
	{
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TInt expectedErr = KErrCancel;
	INFO_PRINTF1(_L("Test cancel command execution with ECmdHighSeverity Severity"));
/*
	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:02 Test: Start ama starter which does not exist - ESsmFireAndForget"));
	CreateCmdAndExecuteL(severity, ESsmFireAndForget, KTestDscId, expectedErr, ETrue);
*/
	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:01 Test: Start ama starter which does not exist - ESsmWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmWaitForSignal, KTestDscId, expectedErr, ETrue);

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:02 Test: Start ama starter which does not exist - ESsmDeferredWaitForSignal"));
	CreateCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, KTestDscId, expectedErr, ETrue);
	}


/**
Helper function to compare the data of the command.
*/
void CCmdTestAMAStarter::CompareCommandsDataL(CCmdAmaStarter* aTestCmd, TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TUid aDscId, const TUint16 aPriority)
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
	TSsmExecutionBehaviour executionBehaviour = static_cast<TSsmExecutionBehaviour>(readStream.ReadUint8L());
	TUid dscId = TUid::Uid(readStream.ReadUint32L());
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	TUint16 priority = readStream.ReadUint16L();
#endif


	TEST(aSeverity == severity);
	TEST(aExecutionBehaviour == executionBehaviour);
	TEST(aDscId.iUid == dscId.iUid);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
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
CCmdAmaStarter* CCmdTestAMAStarter::CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TUid aDscId, const TUint16 aPriority)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteUint8L(aExecutionBehaviour);
	writeStream.WriteUint32L(aDscId.iUid);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdAmaStarter* newCmd = CCmdAmaStarter::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	
	return newCmd;
	}
/**
Helper function to run a command and test the expected results
*/
void CCmdTestAMAStarter::CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid aDscId, TInt aExpectedErr, TBool aCancel, TInt32 aTimeout)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddAMAStarterCommandL(aSeverity, aExecutionBehaviour, aDscId);
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


CSsmCommandBase* CCmdTestAMAStarter::ConstructCommandFromStreamLC(TSsmCommandType /*aType*/, RReadStream& aReadStream, TArray<MSsmCommand*>& /*aDeferredList*/)
	{
	CSsmCommandBase* cmd = CCmdAmaStarter::NewL(aReadStream);
	CleanupStack::PushL(cmd);
	return cmd;
	}
