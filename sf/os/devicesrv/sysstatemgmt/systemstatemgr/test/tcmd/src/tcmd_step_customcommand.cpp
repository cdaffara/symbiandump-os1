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

#include <ssm/ssmcustomcommandinfo.h>
#include <ssm/ssmcustomcommand.h>

#include <ssmcmd_customcmd1.rsg>

#include "cmdcustomcommand.h"
#include "tcmd_step_customcommand.h"
#include "tcmd_cmdlist.h"
#include "ssmcommandparameters.h"

_LIT(KDllName, "tcustom_cmd.dll");
_LIT(KRscFileBuffer, "z:\\resource\\ssmatest\\bic\\ssmcmd_customcmd1.rsc");
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
const TUint16 KNegPriority = 65535;
#endif


/**
 Constructor
*/
CCmdTestCustomCommand::CCmdTestCustomCommand()
	: CCmdTestBase(KTCCmdTestCustomCommand)
	{
	}

/**
 Destructor
*/
CCmdTestCustomCommand::~CCmdTestCustomCommand()
	{
	}

/**
 Run the tests
*/
TVerdict CCmdTestCustomCommand::doTestStepL()
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

	__UHEAP_MARK;
	TRAP(err, doTestRetriesL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestInitializeResourceHandleLeakL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestNewLWithGoodDataL()
	{
	TPtrC filename = KDllName();
	TInt32 ordinal = 1;
	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
	TInt retries = 3;

	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->SetL(filename, ordinal, unloading, retries, params);

	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(ECmdIgnoreFailure, ESsmFireAndForget, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdIgnoreFailure, ESsmFireAndForget, *info);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo)"));
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(ECmdIgnoreFailure, ESsmFireAndForget, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdIgnoreFailure, ESsmFireAndForget, *info);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD1);
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdCriticalSeverity, ESsmWaitForSignal, *info);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(ECmdIgnoreFailure, ESsmFireAndForget, *info, KTestPriority));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdIgnoreFailure, ESsmFireAndForget, *info, KTestPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters - and priority also!)"));
	// open reader
	RResourceReader& reader2 = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD31);
	TSsmCommandParameters params2(reader2, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdCriticalSeverity, ESsmWaitForSignal, *info, KTestPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters - and priority also!)"));
	INFO_PRINTF1(_L("As version is one it should be default priority"));
	// open reader
	RResourceReader& reader3 = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD32);
	TSsmCommandParameters params3(reader3, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdCriticalSeverity, ESsmWaitForSignal, *info, KDefaultCommandPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters - and priority also!)"));
	INFO_PRINTF1(_L("negative priority will be interpreted as a very high positive value"));
	// open reader
	RResourceReader& reader4 = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD33);
	TSsmCommandParameters params4(reader4, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params4));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdCriticalSeverity, ESsmWaitForSignal, *info, KNegPriority);		//validate the values
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
		
#endif

	CleanupStack::PopAndDestroy(info);
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestNewLWithBadDataL()
	{
	TInt err = KErrNone;
	TInt expectedErr = KErrArgument;
	TSsmExecutionBehaviour executionBehaviour = ESsmFireAndForget;	//valid execution behaviour
	TPtrC filename = KDllName();
	TInt32 ordinal = 1;
	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
	TInt retries = 3;
	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->SetL(filename, ordinal, unloading, retries, params);
	
	executionBehaviour = static_cast<TSsmExecutionBehaviour>(0);
	//invalid execution behaviour
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(ECmdIgnoreFailure, executionBehaviour, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo)"));
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(ECmdIgnoreFailure, executionBehaviour, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD4);
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	// empty filename
	_LIT(KEmptyFileName, "");
	info->SetL(KEmptyFileName, ordinal, unloading, retries, params);
	executionBehaviour = ESsmFireAndForget;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:04 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(ECmdIgnoreFailure, executionBehaviour, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:05 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo)"));
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(ECmdIgnoreFailure, executionBehaviour, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	// invalid ordinal
	info->SetL(filename, 0, unloading, retries, params);
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:06 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(ECmdIgnoreFailure, executionBehaviour, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:07 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo)"));
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(ECmdIgnoreFailure, executionBehaviour, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:08 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD6);
	TSsmCommandParameters params2(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	// unloading
	unloading = static_cast<TCmdCustomCommandLibUnloading>(10);
	info->SetL(filename, ordinal, unloading, retries, params);

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:09 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(ECmdIgnoreFailure, executionBehaviour, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:10 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo)"));
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(ECmdIgnoreFailure, executionBehaviour, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	// negative retries
	unloading = EUnloadOnCommandCompletion;
	retries = -5;
	info->SetL(filename, ordinal, unloading, retries, params);
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:11 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(ECmdIgnoreFailure, executionBehaviour, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:12 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo)"));
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(ECmdIgnoreFailure, executionBehaviour, *info));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;

	CleanupStack::PopAndDestroy(info);
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestInternalizeLWithGoodDataL()
	{
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestInternalizeLWithBadDataL()
	{
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestResourceReaderWithGoodDataL()
	{
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestResourceReaderWithMissingDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - construct from resource with invalid reader"));

	INFO_PRINTF1(_L("doTestResourceReaderWithMissingDataL:01 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD1);
	TSsmCommandParameters params(reader, NULL, 0);
	// read some of the data so there will not be enough to read
	reader.ReadInt32L();
	TRAPD(err, iTestCmd = CCmdCustomCommand::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, KErrNotSupported);
	TEST(err == KErrNotSupported);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - "));
	TInt expectedErr = KErrNotSupported;
	TInt err = KErrNone;

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:01 Invalid type"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD2);
	TSsmCommandParameters params(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:02 Invalid Version"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD3);
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params1));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	expectedErr = KErrArgument;
	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:03 Invalid executionBehaviour 1"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD4);
	TSsmCommandParameters params2(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params2));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();


	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:04 Invalid executionBehaviour 2"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD5);
	TSsmCommandParameters params3(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params3));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:05 Missing ordinal"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD6);
	TSsmCommandParameters params4(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params4));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:06 Negative ordinal"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD22);
	TSsmCommandParameters params5(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params5));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:07 Missing filename"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD7);
	TSsmCommandParameters params6(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params6));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:08 Bad UnloadOnFinish 1"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD9);
	TSsmCommandParameters params8(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params8));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:09 Bad UnloadOnFinish 2"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD10);
	TSsmCommandParameters params9(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params9));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();

	INFO_PRINTF1(_L("doTestResourceReaderWithBadDataL:10 Negative retries"));
	reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD11);
	TSsmCommandParameters params10(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params10));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/


void CCmdTestCustomCommand::doTestCommandTypeL()
	{
	TPtrC filename = KDllName();
	TInt32 ordinal = 1;
	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
	TInt retries = 3;

	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->SetL(filename, ordinal, unloading, retries, params);

	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	TRAP(err, iTestCmd = CreateCmdFromStreamL(ECmdIgnoreFailure, ESsmFireAndForget, *info));
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdIgnoreFailure, ESsmFireAndForget, *info);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdCustomCommand);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo)"));
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(ECmdIgnoreFailure, ESsmFireAndForget, *info));
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdIgnoreFailure, ESsmFireAndForget, *info);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdCustomCommand);
	delete iTestCmd;
	iTestCmd = NULL;

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	// open reader
	RResourceReader& reader = LoadResourcesL(KRscFileBuffer, R_COMMAND_CUSTOM_CMD1);
	TSsmCommandParameters params1(reader, NULL, 0);
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(params1));
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdCriticalSeverity, ESsmWaitForSignal, *info);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdCustomCommand);
	delete iTestCmd;
	iTestCmd = NULL;
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	TRAP(err, iTestCmd = CCmdCustomCommand::NewL(ECmdIgnoreFailure, ESsmFireAndForget, *info, KTestPriority));
	TEST(err == expectedErr);
	CompareCommandsDataL(iTestCmd, ECmdIgnoreFailure, ESsmFireAndForget, *info, KTestPriority);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdCustomCommand);
	delete iTestCmd;
	iTestCmd = NULL;
#endif

	CleanupStack::PopAndDestroy(info);
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestCmdFailureIgnoreSeverityL()
	{
	TInt expectedErr = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	_LIT(KNoSuchDll, "nosuchdll.dll");
	TPtrC filename = KNoSuchDll();
	TInt32 ordinal = 1;
	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
	TInt retries = 5;
	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->SetL(filename, ordinal, unloading, retries, params);

	INFO_PRINTF1(_L("Test command failure with ECmdIgnoreFailure Severity"));

	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL:01 - executionBehaviour = ESsmWaitForSignal"));
	TRAPD(err, CreateCustCmdAndExecuteL(severity, ESsmWaitForSignal, *info, expectedErr, EFalse));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
	TEST(err == expectedErr);

	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL:02 - executionBehaviour = ESsmDeferredWaitForSignal"));
	TRAP(err, CreateCustCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, *info, expectedErr, EFalse, 1000));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedErr : %d. "), err, expectedErr);
	TEST(err == expectedErr);
		
	CleanupStack::PopAndDestroy(info);
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/


void CCmdTestCustomCommand::doTestCmdFailureHighSeverityL()
	{
	TInt expectedErr = KErrNotFound;
	TInt expectedResult = KErrNone;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	_LIT(KNoSuchDll, "nosuchdll.dll");
	TPtrC filename = KNoSuchDll();
	TInt32 ordinal = 1;
	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
	TInt retries = 5;
	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->SetL(filename, ordinal, unloading, retries, params);

	INFO_PRINTF1(_L("Test command failure with ECmdHighSeverity Severity"));

	INFO_PRINTF1(_L("Test:doTestCmdFailureHighSeverityL:01 - executionBehaviour = ESsmWaitForSignal"));
	TRAPD(err, CreateCustCmdAndExecuteL(severity, ESsmWaitForSignal, *info, expectedErr, EFalse));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	INFO_PRINTF1(_L("Test:doTestCmdFailureHighSeverityL:02 - executionBehaviour = ESsmDeferredWaitForSignal"));
	TRAP(err, CreateCustCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, *info, expectedErr, EFalse, 1000));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	CleanupStack::PopAndDestroy(info);
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestCancelCmdIgnoreSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TInt expectedResult = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TPtrC filename = KDllName();
	TInt32 ordinal = 1;
	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
	TInt retries = 5;
	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->SetL(filename, ordinal, unloading, retries, params);

	INFO_PRINTF1(_L("Test cancel command with ECmdIgnoreFailure Severity"));

	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL:01 - executionBehaviour = ESsmWaitForSignal"));
	TRAPD(err, CreateCustCmdAndExecuteL(severity, ESsmWaitForSignal, *info, expectedErr, ETrue));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL:02 - executionBehaviour = ESsmDeferredWaitForSignal"));
	TRAP(err, CreateCustCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, *info, expectedErr, ETrue, 1000));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	CleanupStack::PopAndDestroy(info);
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/


void CCmdTestCustomCommand::doTestCancelCmdHighSeverityL()
	{
	TInt expectedErr = KErrCancel;
	TInt expectedResult = KErrNone;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TPtrC filename = KDllName();
	TInt32 ordinal = 1;
	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
	TInt retries = 5;
	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->SetL(filename, ordinal, unloading, retries, params);

	INFO_PRINTF1(_L("Test cancel command with ECmdHighSeverity Severity"));

	INFO_PRINTF1(_L("Test:doTestCancelCmdHighSeverityL:01 - executionBehaviour = ESsmWaitForSignal"));
	TRAPD(err, CreateCustCmdAndExecuteL(severity, ESsmWaitForSignal, *info, expectedErr, ETrue));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	INFO_PRINTF1(_L("Test:doTestCancelCmdHighSeverityL:02 - executionBehaviour = ESsmDeferredWaitForSignal"));
	TRAP(err, CreateCustCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, *info, expectedErr, ETrue, 1000));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	CleanupStack::PopAndDestroy(info);
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestNormalExecutionL()
	{
	TInt expectedResult = KErrNone;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TPtrC filename = KDllName();
	TInt32 ordinal = 1;
	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
	TInt retries = 5;
	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->SetL(filename, ordinal, unloading, retries, params);

	INFO_PRINTF1(_L("Test normal execution"));

	INFO_PRINTF1(_L("Test:doTestNormalExecutionL:01 - executionBehaviour = ESsmWaitForSignal"));
	TRAPD(err, CreateCustCmdAndExecuteL(severity, ESsmWaitForSignal, *info, expectedResult, EFalse));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	INFO_PRINTF1(_L("Test:doTestNormalExecutionL:02 - executionBehaviour = ESsmDeferredWaitForSignal"));
	TRAP(err, CreateCustCmdAndExecuteL(severity, ESsmDeferredWaitForSignal, *info, expectedResult, EFalse));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, expectedResult);
	TEST(err == expectedResult);

	CleanupStack::PopAndDestroy(info);
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestSupportingClassesL()
	{
	INFO_PRINTF1(_L("doTestSupportingClassesL"));

	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	TInt8 ordinal = 5;
	TCmdCustomCommandLibUnloading unloading = ENeverUnload;
	TInt retries = 3;
	_LIT8(KDllParams, "12345");
	
	// test default values
	TEST(info->FileName() == KNullDesC);
	TEST(info->Ordinal() == 1);
	TEST(info->Unloading() == EUnloadOnCommandCompletion);
	TEST(info->Retries() == 0);
	TEST(info->Params() == KNullDesC8);
	
	// test set values
	info->SetL(KDllName, ordinal, unloading, retries, KDllParams);
	TEST(info->FileName() == KDllName);
	TEST(info->Ordinal() == ordinal);
	TEST(info->Unloading() == unloading);
	TEST(info->Retries() == retries);
	TEST(info->Params() == KDllParams);
	
	// test clear values
	info->Clear();
	TEST(info->FileName() == KNullDesC);
	TEST(info->Ordinal() == 1);
	TEST(info->Unloading() == EUnloadOnCommandCompletion);
	TEST(info->Retries() == 0);
	TEST(info->Params() == KNullDesC8);

	CleanupStack::PopAndDestroy(info);
	
	RFs rfs1;	
	RFs rfs2;
	rfs1.Connect();
	rfs2.Connect();
	CSsmCustomCommandEnv* cmdEnv1 = CSsmCustomCommandEnv::NewL(rfs1);
	CleanupStack::PushL(cmdEnv1);
	CSsmCustomCommandEnv* cmdEnv2 = CSsmCustomCommandEnv::NewL(rfs2);
	CleanupStack::PushL(cmdEnv2);

	// RFs handle 1 should match the reference in the environment
	const RFs& rfsref1 = cmdEnv1->Rfs();
	TEST(rfsref1.Handle() == rfs1.Handle());
	
	CleanupStack::PopAndDestroy(cmdEnv2);
	CleanupStack::PopAndDestroy(cmdEnv1);
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0015
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestRetriesL()
	{
	}

/**
Old Test CaseID 		AFSS-CMDCUS-0016
New Test CaseID 		DEVSRVS-SSMA-CMD-0006
*/

void CCmdTestCustomCommand::doTestInitializeResourceHandleLeakL()
	{
	INFO_PRINTF1(_L("Test:doTestInitializeResourceHandleLeakL - executionBehaviour = ESsmWaitForSignal"));
	_LIT(KTestDllName, "tcustom_cmd_rleak.dll");
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TPtrC filename = KTestDllName();
	TInt32 ordinal = 1;
	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
	TInt retries = 2;
	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->SetL(filename, ordinal, unloading, retries, params);
	
	// Get the count of processes and threads before creating and executing the custom command.
	TInt processHandles_BeforeCreateCmdAndExe = 0;
	TInt threadHandles_BeforeCreateCmdAndExe = 0;
	RThread().HandleCount(processHandles_BeforeCreateCmdAndExe, threadHandles_BeforeCreateCmdAndExe);
	INFO_PRINTF3(_L("Before calling CreateCustCmdAndExecuteL- Process Handles : %d, Thread Handles : %d. "), processHandles_BeforeCreateCmdAndExe, threadHandles_BeforeCreateCmdAndExe);
	
	// Creates and executes the custom command with ESsmWaitForSignal.
	TRAPD(err, CreateCustCmdAndExecuteL(severity, ESsmWaitForSignal, *info, KErrNone, EFalse));
	INFO_PRINTF3(_L("Command execution finished with error : %d, expectedResult : %d. "), err, KErrNone);
	TEST(err == KErrNone);
	
	// Get the count of processes and threads after executing the custom command.
	TInt processHandles_AfterCreateCmdAndExe = 0;
	TInt threadHandles_AfterCreateCmdAndExe = 0;
	RThread().HandleCount(processHandles_AfterCreateCmdAndExe, threadHandles_AfterCreateCmdAndExe);
	INFO_PRINTF3(_L("After getting called CreateCustCmdAndExecuteL- Process Handles : %d, Thread Handles : %d. "), processHandles_AfterCreateCmdAndExe, threadHandles_AfterCreateCmdAndExe);
	
	// Verify if the count of processes and threads before and after executing the commands are equal.
	TEST(processHandles_BeforeCreateCmdAndExe == processHandles_AfterCreateCmdAndExe);
	TEST(threadHandles_BeforeCreateCmdAndExe == threadHandles_AfterCreateCmdAndExe);
	
	CleanupStack::PopAndDestroy(info);
	}

/**
Helper function to create instance of CCmdAmaStarter trough readstream.
*/
CCmdCustomCommand* CCmdTestCustomCommand::CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo, const TUint16 aPriority)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteUint8L(aExecutionBehaviour);
	aInfo.ExternalizeL(writeStream);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	writeStream.CommitL();

	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdCustomCommand* newCmd = CCmdCustomCommand::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	
	return newCmd;
	}
	
/**
Helper function to compare the data of the command.
*/	
void CCmdTestCustomCommand::CompareCommandsDataL(CCmdCustomCommand* aTestCmd, TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo, const TUint16 aPriority)
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

	const TCmdErrorSeverity severity = static_cast<TCmdErrorSeverity>(readStream.ReadInt16L());
	const TSsmExecutionBehaviour executionBehaviour = static_cast<TSsmExecutionBehaviour>(readStream.ReadUint8L());
	
	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->InternalizeL(readStream);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	TUint16 priority = readStream.ReadUint16L();
#endif

	TEST(aSeverity == severity);
	TEST(aExecutionBehaviour == executionBehaviour);
	TEST(aInfo.FileName() == info->FileName());
	TEST(aInfo.Ordinal() == info->Ordinal());
	TEST(aInfo.Unloading() == info->Unloading());
	TEST(aInfo.Retries() == info->Retries());
	TEST(aInfo.Params().Length() == info->Params().Length());

	TInt len = aInfo.Params().Length();
	for (TInt i = 0; i < len; i++)
		{
		TEST(aInfo.Params()[i]==info->Params()[i]);
		}
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	TEST(aPriority == priority);
#else
	TEST(aPriority == KDefaultPriority);
#endif

	CleanupStack::PopAndDestroy(info);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(outputBuffer);
	}
	
/**
Helper function to run a command and test the expected results
*/
void CCmdTestCustomCommand::CreateCustCmdAndExecuteL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, CSsmCustomCommandInfo& aInfo, TInt aExpectedErr, TBool aCancel, TInt32 aTimeout)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddCustomCommandCommandL(aSeverity, aExecutionBehaviour, aInfo);
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
