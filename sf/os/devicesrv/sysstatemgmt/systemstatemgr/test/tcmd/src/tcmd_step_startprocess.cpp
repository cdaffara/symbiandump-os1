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
#include <barsread2.h>
#include <s32mem.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmstartupproperties.h>
#include "ssmcommandparameters.h"
#include "ssmdebug.h"
#include "cmdstartprocess.h"
#include "tcmd_step_startprocess.h"
#include "tcmd_cmdlist.h"
#include <ssmcmd_startprocess1.rsg>
#include "ssmtestprocgoodsession.h"

_LIT(KTestRscFileName, "z:\\resource\\ssmatest\\bic\\ssmcmd_startprocess1.rsc");
_LIT(KBadFileName, "c:\\badfile.exe");	//this file does not exist
_LIT(KEmptyFileName, "");
_LIT(KGoodProc, "ssmtestprocgood.exe");
_LIT(KEmptyArgs, "");
_LIT(KAppInRsc, "c:\\test.exe");
_LIT(KArgsInRsc, "arguments");

const TInt KNumOfRetries = 5;
const TInt KTestTimeOut = 10000;
const TInt KTimeoutMultipleWait = 3000;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
const TUint16 KNegPriority = 65535;
#endif
//
// Destructor
//
CCmdTestStartProcess::~CCmdTestStartProcess()
	{
	delete iProcessInfo;
	delete iTestCmd;
	}

//
// Constructor
//
CCmdTestStartProcess::CCmdTestStartProcess()
	: CCmdTestBase(KTCCmdTestStartProcess)
	{
	}

//
// Run the tests
//
TVerdict CCmdTestStartProcess::doTestStepL()
	{
	iProcessInfo = CSsmStartupProperties::NewL();
	
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
	TRAP(err, doTestSuccessfulExecution1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestSuccessfulExecution2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	err = iSem.CreateGlobal(KStartProcSignalSemaphore, 0);
	INFO_PRINTF2(_L("Created semaphore with err %d"),err);
	TEST(err == KErrNone);

	__UHEAP_MARK;
	TRAP(err, doTestDwfsStateWithMonitorL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	
	__UHEAP_MARK;
	TRAP(err, doTestNegativeRetryL());
	Reset();	// reset all the data which has been set in SetProcessInfoDataL()
	TEST(err == KErrArgument);
	__UHEAP_MARKEND;
	
	delete iProcessInfo;
	iProcessInfo = NULL;
	iSem.Close();
	return TestStepResult();
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */

void CCmdTestStartProcess::doTestNewLWithGoodDataL()
	{
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs);
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd);		//validate the values
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo)"));
	severity = ECmdCriticalSeverity;
	SetProcessInfoDataL(KBadFileName, KEmptyArgs);
	TRAP(err, iTestCmd = CCmdStartProcess::NewL(severity, iProcessInfo));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd);		//validate the values
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartProcess::NewL(params));
	// set iProcessInfo same as rsc file to compare
	SetProcessInfoDataL(KAppInRsc, KArgsInRsc, ESsmFireAndForget, 10, 20);
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd);
	Reset();
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	severity = ECmdCriticalSeverity;
	SetProcessInfoDataL(KBadFileName, KEmptyArgs);
	TRAP(err, iTestCmd = CCmdStartProcess::NewL(severity, iProcessInfo, KTestPriority));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd, KTestPriority);		//validate the values
	Reset();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters) and priority too"));
	INFO_PRINTF1(_L("read from rss file"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader on contents
	RResourceReader& reader1 = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_PRIORITY);
	TSsmCommandParameters params1(reader1, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartProcess::NewL(params1));
	// set iProcessInfo same as rsc file to compare
	SetProcessInfoDataL(KAppInRsc, KArgsInRsc, ESsmFireAndForget, 10, 20);
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd, KTestPriority);
	Reset();
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters) and priority too"));
	INFO_PRINTF1(_L("initial version = default priority"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader on contents
	RResourceReader& reader2 = LoadResourcesL(KTestRscFileName, R_COMMAND_DEFAULT_PRIORITY);
	TSsmCommandParameters params2(reader2, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartProcess::NewL(params2));
	// set iProcessInfo same as rsc file to compare
	SetProcessInfoDataL(KAppInRsc, KArgsInRsc, ESsmFireAndForget, 10, 20);
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd, KDefaultCommandPriority);
	Reset();
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters) and priority too"));
	INFO_PRINTF1(_L("negative priority will be interpreted as a very high positive value"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader on contents
	RResourceReader& reader3 = LoadResourcesL(KTestRscFileName, R_COMMAND_NEGATIVE_PRIORITY);
	TSsmCommandParameters params3(reader3, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartProcess::NewL(params3));
	// set iProcessInfo same as rsc file to compare
	SetProcessInfoDataL(KAppInRsc, KArgsInRsc, ESsmFireAndForget, 10, 20);
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd, KNegPriority);
	Reset();
	FreeResources();
#endif
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */
void CCmdTestStartProcess::doTestNewLWithBadDataL()
	{
	TInt expectedErr = KErrArgument;
	TInt err = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmExecutionBehaviour badExecutionBehaviour = (TSsmExecutionBehaviour)-1;
	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - Command construction with in-valid data"));

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(RReadStream& aReadStream)- Create command with NULL file name"));
	SetProcessInfoDataL(KEmptyFileName, KEmptyArgs);
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(RReadStream& aReadStream)- Invalid execution behaviour"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, badExecutionBehaviour);
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:03 Test: NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo)- Create command with NULL file name"));
	severity = ECmdCriticalSeverity;
	SetProcessInfoDataL(KEmptyFileName, KEmptyArgs);
	TRAP(err, iTestCmd = CCmdStartProcess::NewL(severity, iProcessInfo));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo)- Invalid execution behaviour"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, badExecutionBehaviour);
	TRAP(err, iTestCmd = CCmdStartProcess::NewL(severity, iProcessInfo));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters)- Create command with NULL file name"));
	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_INVALID_DATA);
	TSsmCommandParameters firstParam(reader, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartProcess::NewL(firstParam));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();
	FreeResources();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters)- Invalid execution behaviour"));
	// open reader on contents
	reader = LoadResourcesL(KTestRscFileName, R_COMMAND_INVALID_DATA1);
	TSsmCommandParameters secondParam(reader, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartProcess::NewL(secondParam));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:07 Test: CSsmStartupProperties :: Invalid Command type"));
	TRAP(err, iProcessInfo->SetCommandTypeL(ESsmCmdWaitForApparcInit));
    INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */

void CCmdTestStartProcess::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */
void CCmdTestStartProcess::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL - InternalizeL with in valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:01 and doTestNewLWithBadDataL:02 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */
 

void CCmdTestStartProcess::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */
 

void CCmdTestStartProcess::doTestResourceReaderWithMissingDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - construct from resource with invalid reader"));
	TInt expectedErr = KErrNotSupported;
	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_INVALID_DATA);
	//read some data so reader will become invalid
	reader.ReadUint16L();
	TSsmCommandParameters params(reader, ResourceFile(), 0);
	TRAPD(err, iTestCmd = CCmdStartProcess::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */

void CCmdTestStartProcess::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - Construct from resource with valid reader and invalid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:05 & doTestNewLWithBadDataL:06 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */
void CCmdTestStartProcess::doTestCommandTypeL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestCommandTypeL:01 Test: Construct from NewL(RReadStream& aReadStream) and check type"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs);
	iTestCmd = CreateCmdFromStreamL(severity);
	CompareCommandsDataL(severity, iTestCmd);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdStartProcess);
	Reset();

	INFO_PRINTF1(_L("doTestCommandTypeL:02 Test: Construct from NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo) and check type"));
	severity = ECmdCriticalSeverity;
	SetProcessInfoDataL(KBadFileName, KEmptyArgs);
	iTestCmd = CCmdStartProcess::NewL(severity, iProcessInfo);
	CompareCommandsDataL(severity, iTestCmd);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdStartProcess);
	Reset();
	
	INFO_PRINTF1(_L("doTestCommandTypeL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)  and check type"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, ResourceFile(), 0);
	iTestCmd = CCmdStartProcess::NewL(params);
	// set iProcessInfo same as rsc file to compare
	SetProcessInfoDataL(KAppInRsc, KArgsInRsc, ESsmFireAndForget, 10, 20);
	CompareCommandsDataL(severity, iTestCmd);
	TEST(iTestCmd->Type() == ESsmCmdStartProcess);
	Reset();
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */


void CCmdTestStartProcess::doTestCmdFailureIgnoreSeverityL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL - command failure with ECmdIgnoreFailure Severity"));

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:01 Start a non existing process with execution behaviour as ESsmFireAndForget"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmFireAndForget);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:02 Start a non existing process with execution behaviour as ESsmFireAndForget and retries %d"), KNumOfRetries);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmFireAndForget, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:03 Start a non existing process with execution behaviour as ESsmFireAndForget and timeout %d "), 10);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmFireAndForget, 0, 10000);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:04 Start a non existing process with execution behaviour as ESsmWaitForSignal"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:05 Start a non existing process with execution behaviour as ESsmWaitForSignal and retries %d"), KNumOfRetries);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:06 Start a non existing process with execution behaviour as ESsmWaitForSignal and timeout %d "), KTestTimeOut);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:07 Start a non existing process with execution behaviour as ESsmDeferredWaitForSignal"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:08 Start a non existing process with execution behaviour as ESsmDeferredWaitForSignal and retries %d"), KNumOfRetries);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:09 Start a non existing process with execution behaviour as ESsmDeferredWaitForSignal and timeout %d "), KTestTimeOut);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */


void CCmdTestStartProcess::doTestCmdFailureHighSeverityL()
	{
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TInt expectedErr = KErrNotFound;
	INFO_PRINTF1(_L("Test:doTestCmdFailureHighSeverityL - command failure with ECmdHighSeverity Severity"));

	INFO_PRINTF1(_L("doTestCmdFailureHighSeverityL:01 Start a non existing process with execution behaviour as ESsmWaitForSignal"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF2(_L("doTestCmdFailureHighSeverityL:02 Start a non existing process with execution behaviour as ESsmWaitForSignal and retries %d"), KNumOfRetries);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF2(_L("doTestCmdFailureHighSeverityL:03 Start a non existing process with execution behaviour as ESsmWaitForSignal and timeout %d "), KTestTimeOut);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestCmdFailureHighSeverityL:04 Start a non existing process with execution behaviour as ESsmDeferredWaitForSignal"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCmdFailureHighSeverityL:05 Start a non existing process with execution behaviour as ESsmDeferredWaitForSignal and timeout %d "), KTestTimeOut);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCmdFailureHighSeverityL:06 Start a non existing process with execution behaviour as ESsmDeferredWaitForSignal and retries %d"), KNumOfRetries);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */
void CCmdTestStartProcess::doTestCancelCmdIgnoreSeverityL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt expectedErr = KErrCancel;
	INFO_PRINTF1(_L("Test:doTestCancelCmdIgnoreSeverityL - command failure with ECmdIgnoreFailure Severity"));

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:01 Start and cancel a non existing process with execution behaviour as ESsmWaitForSignal"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:02 Start  and cancel a non existing process with execution behaviour as ESsmWaitForSignal and retries %d"), KNumOfRetries);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:03 Start and cancel a non existing process with execution behaviour as ESsmWaitForSignal and timeout %d "), KTestTimeOut);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:04 Start and cancel a non existing process with execution behaviour as ESsmDeferredWaitForSignal"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:05 Start and cancel a non existing process with execution behaviour as ESsmDeferredWaitForSignal and retries %d"), KNumOfRetries);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:06 Start and cancel a non existing process with execution behaviour as ESsmDeferredWaitForSignal and timeout %d "), KTestTimeOut);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */
void CCmdTestStartProcess::doTestCancelCmdHighSeverityL()
	{
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TInt expectedErr = KErrCancel;
	INFO_PRINTF1(_L("Test:doTestCancelCmdHighSeverityL - command failure with ECmdHighSeverity Severity"));

	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:01 Start and cancel a non existing process with execution behaviour as ESsmWaitForSignal"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF2(_L("doTestCancelCmdHighSeverityL:02 Start  and cancel a non existing process with execution behaviour as ESsmWaitForSignal and retries %d"), KNumOfRetries);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF2(_L("doTestCancelCmdHighSeverityL:03 Start and cancel a non existing process with execution behaviour as ESsmWaitForSignal and timeout %d "), KTestTimeOut);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:04 Start and cancel a non existing process with execution behaviour as ESsmDeferredWaitForSignal"));
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCancelCmdHighSeverityL:05 Start and cancel a non existing process with execution behaviour as ESsmDeferredWaitForSignal and retries %d"), KNumOfRetries);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCancelCmdHighSeverityL:06 Start and cancel a non existing process with execution behaviour as ESsmDeferredWaitForSignal and timeout %d "), KTestTimeOut);
	SetProcessInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);
	}


/**
Old Test CaseID 		AFSS-CMDSPR-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */
void CCmdTestStartProcess::doTestSuccessfulExecution1L()
	{
	INFO_PRINTF1(_L("Test successful command execution with ECmdIgnoreFailure Severity"));
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt expectedErr = KErrNone;

	INFO_PRINTF1(_L("doTestSuccessfulExecution1L:01 Start a process with execution behaviour as ESsmFireAndForget"));
	SetProcessInfoDataL(KGoodProc, KEmptyArgs, ESsmFireAndForget);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestSuccessfulExecution1L:01 Start a approcessith execution behaviour as ESsmWaitForSignal"));
	SetProcessInfoDataL(KGoodProc, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestSuccessfulExecution1L:06 Start an process with execution behaviour as ESsmDeferredWaitForSignal"));
	SetProcessInfoDataL(KGoodProc, KEmptyArgs, ESsmDeferredWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, 5000);
	// we are expecting three instances of KGoodProc
	TEST(3 == FindAndKill(KGoodProc));
	}

/**
Old Test CaseID 		AFSS-CMDSPR-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */

void CCmdTestStartProcess::doTestSuccessfulExecution2L()
	{
	INFO_PRINTF1(_L("Test successful command execution with ECmdHighSeverity Severity"));
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt expectedErr = KErrNone;

	INFO_PRINTF1(_L("doTestSuccessfulExecution2L:01 Start a process with execution behaviour as ESsmFireAndForget"));
	SetProcessInfoDataL(KGoodProc, KEmptyArgs, ESsmFireAndForget);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestSuccessfulExecution2L:01 Start an process with execution behaviour as ESsmWaitForSignal"));
	SetProcessInfoDataL(KGoodProc, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestSuccessfulExecution2L:06 Start a process with execution behaviour as ESsmDeferredWaitForSignal"));
	SetProcessInfoDataL(KGoodProc, KEmptyArgs, ESsmDeferredWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, 5000);
	// we are expecting three instances of KGoodProc
	TEST(3 == FindAndKill(KGoodProc));
	}


/**
Old Test CaseID 		AFSS-CMDSPR-0015
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */
void CCmdTestStartProcess::doTestDwfsStateWithMonitorL()
	{
	// The KIntervalForReLaunchRateOfFailure time interval is 30mins by default, its patched to 25secs on emulator in epoc.ini file
	// and for hardware in iby file.
	INFO_PRINTF1(_L("For test to pass in Winscw - (patchdata_sysmon_exe_KIntervalForReLaunchRateOfFailure 25) should be added to epoc32\\data\\epoc.ini file"));
	
	INFO_PRINTF1(_L("Test:doTestDwfsStateWithMonitor - command ECmdIgnoreFailure Severity"));
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt expectedErr = KErrNone;
	SetProcessInfoDataL(KGoodProc, KEmptyArgs, ESsmDeferredWaitForSignal, 1, 0);
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iRetries = 1;
	monitorInfo.iTimeout = 0;	
	iProcessInfo->SetMonitorInfoL(monitorInfo);
	INFO_PRINTF1(_L("doTestDwfsStateWithMonitor:Start a process with execution behaviour as ESsmDeferredWaitForSignal"));
	CreateCmdAndExecuteL(severity, expectedErr);
	
	//Waiting to decrement the count of the semaphore which is being signalled as the application is started first time 
	iSem.Wait();
	
	INFO_PRINTF1(_L("Call Find and Kill now - instances should be one"));
	//find and kill the process
	TEST(1 == FindAndKill(KGoodProc));
	
	INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Wait for one second and call Find and Kill again - instances should be zero"));
	User::After(1000000);
	// system monitor waits for 15secs(throttle time) before restarting the process
	TEST(0 == FindAndKill(KGoodProc)); 
	
	INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Wait for Throttle time and call find and kill again - instances should be one"));
	//Waiting for the process to restart using semaphore
	iSem.Wait();
	// Process is restarted by system monitor
	TEST(1 == FindAndKill(KGoodProc));
	
	INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Wait for one second and call Find and Kill again - instances should be zero"));
	User::After(1000000);
	// application is restarted by system monitor
	TEST(0 == FindAndKill(KGoodProc));
	
	INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Wait for Throttle time and call find and kill again - instances should be one"));
	//Waiting for the process to restart using semaphore
	iSem.Wait();
	// application is restarted by system monitor
	TEST(1 == FindAndKill(KGoodProc));
	
	INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Wait for more than throttle time and call find and kill again - instances should be one"));
	INFO_PRINTF1(_L("Done to ensure that the app is closed and not hanging around"));
	//Waiting for the process to restart using semaphore
	iSem.Wait();
	TEST(1 == FindAndKill(KGoodProc));
	}
	
/**
Old Test CaseID 		AFSS-CMDSPR-0016
New Test CaseID 		DEVSRVS-SSMA-CMD-0019
 */



void CCmdTestStartProcess::doTestNegativeRetryL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	SetProcessInfoDataL(KGoodProc, KEmptyArgs, ESsmFireAndForget, -1);
	CCmdStartProcess* cmd = CCmdStartProcess::NewL(severity, iProcessInfo);
	}



/**
Helper functiosn to compare the commands data.
*/
void CCmdTestStartProcess::CompareCommandsDataL(TCmdErrorSeverity aSeverity, CCmdStartProcess* aTestCmd, const TUint16 aPriority)
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
	TEST(aTestCmd->Severity() == aSeverity);
	TSsmCommandType commandType = static_cast<TSsmCommandType>(readStream.ReadUint16L());
	TEST(iProcessInfo->CommandType() == commandType);
	HBufC* fileName = HBufC::NewL(readStream, KMaxFileName);
	CleanupStack::PushL(fileName);
	HBufC* args = HBufC::NewL(readStream, KMaxFileName);
	CleanupStack::PushL(args);
	TSsmExecutionBehaviour executionBehaviour = static_cast<TSsmExecutionBehaviour>(readStream.ReadUint8L());
	TInt32 timeout = readStream.ReadInt32L();
	TInt numOfRetries = readStream.ReadInt16L();
	TInt viewless = readStream.ReadUint8L();
	TInt startInBackground = readStream.ReadUint8L();
	TBool monitorInfoRequired = readStream.ReadUint8L();

	if (monitorInfoRequired)
		{
		TSsmRecoveryMethod restartPolicy = static_cast<TSsmRecoveryMethod>(readStream.ReadUint8L());
		TInt restartMode = readStream.ReadUint8L();
		TInt timeout = readStream.ReadInt32L();
		TInt numOfRetries = readStream.ReadInt16L();
		}
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	const TUint16 priority = readStream.ReadUint16L();
	INFO_PRINTF2(_L("priority %d"),priority);
	TEST(aPriority == priority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	CleanupStack::PopAndDestroy(args);
	CleanupStack::PopAndDestroy(fileName);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(outputBuffer);
	}

/**
Helper functiosn to create a commands from a read stream.
*/
CCmdStartProcess* CCmdTestStartProcess::CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteUint16L(iProcessInfo->CommandType());
	writeStream << iProcessInfo->FileName();
	writeStream << iProcessInfo->Args();;
	writeStream.WriteUint8L(iProcessInfo->ExecutionBehaviour());
	writeStream.WriteInt32L(iProcessInfo->Timeout());
	writeStream.WriteInt16L(iProcessInfo->Retries());
	writeStream.WriteUint8L(iProcessInfo->Viewless());
	writeStream.WriteUint8L(iProcessInfo->StartInBackground());
	writeStream.WriteUint8L(iProcessInfo->IsMonitoringRequired());

	if (iProcessInfo->IsMonitoringRequired())
		{
		writeStream.WriteUint8L(iProcessInfo->MonitorInfo().iRestartPolicy);
		writeStream.WriteUint8L(iProcessInfo->MonitorInfo().iRestartMode);
		writeStream.WriteInt32L(iProcessInfo->MonitorInfo().iTimeout);
		writeStream.WriteInt16L(iProcessInfo->MonitorInfo().iRetries);
		}
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdStartProcess* newCmd = CCmdStartProcess::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	
	return newCmd;
	}

/**
Helper functiosn to set specified values in iProcessInfo.
*/
void CCmdTestStartProcess::SetProcessInfoDataL(const TDesC& aFileName, const TDesC& aArgs, TSsmExecutionBehaviour aExecutionBehaviour, TInt aNumOfRetries, TInt32 aTimeout)
	{
	iProcessInfo->Reset();
	iProcessInfo->SetFileParamsL(aFileName, aArgs);
	iProcessInfo->SetCommandTypeL(ESsmCmdStartProcess);
	iProcessInfo->SetExecutionBehaviour(aExecutionBehaviour);
	iProcessInfo->SetRetries(aNumOfRetries);
	iProcessInfo->SetTimeout(aTimeout);
	}

/**
Resets iProcessInfo
*/
void CCmdTestStartProcess::Reset()
	{
	iProcessInfo->Reset();
	delete iTestCmd;
	iTestCmd = NULL;
	}

/**
Helper function to run a command and test the expected results
*/
void CCmdTestStartProcess::CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TInt aExpectedErr, TBool aCancel, TInt32 aTimeout)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddStartProcessCommandL(aSeverity, iProcessInfo);
	if (ESsmDeferredWaitForSignal == iProcessInfo->ExecutionBehaviour())
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
	Reset();
	}

