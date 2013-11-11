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
#include "ssmtestapps.h"
#include "cmdstartapp.h"

#include "tcmd_step_startapp.h"
#include "tcmd_cmdlist.h"

#include <ssmcmd_startapp.rsg>
_LIT(KTestRscFileName, "z:\\resource\\ssmatest\\bic\\ssmcmd_startapp.rsc");
_LIT(KBadFileName, "c:\\badfile.exe");	//this file does not exist
_LIT(KEmptyFileName, "");
_LIT(KEmptyArgs, "");
_LIT(KAppInRsc, "c:\\test.exe");
_LIT(KArgsInRsc, "arguments");

const TInt KNumOfRetries = 5;
const TInt KTestTimeOut = 100;
const TInt KTimeoutMultipleWait = 80000;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
const TUint16 KTestPriority = 1000;
const TUint16 KNegPriority = 65535;
#endif

/**
Destructor
*/
CCmdTestStartApp::~CCmdTestStartApp()
	{
	delete iAppInfo;
	delete iTestCmd;
	}

/**
Constructor
*/
CCmdTestStartApp::CCmdTestStartApp()
	: CCmdTestBase(KTCCmdTestStartApp)
	{
	}

/**
Run the tests
*/
TVerdict CCmdTestStartApp::doTestStepL()
	{
	iAppInfo = CSsmStartupProperties::NewL();
	TInt err = iSem.CreateGlobal(KStartAppSignalSemaphore, 0);
	INFO_PRINTF2(_L("Created semaphore with err %d"),err);
	TEST(err == KErrNone);
		
	__UHEAP_MARK;
	TRAP(err, doTestDwfsStateAppWithMonitorL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	
	__UHEAP_MARK;
	TRAP(err, doTestNewLWithGoodDataL());
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
	
	__UHEAP_MARK;
	TRAP(err, doTestNegativeRetryL());
	Reset();	// reset all the data which has been set in SetAppInfoDataL()
	TEST(err == KErrArgument);
	__UHEAP_MARKEND;
		
	delete iAppInfo;
	iAppInfo = NULL;
	iSem.Close();

	return TestStepResult();
	}
/**
Old Test CaseID 		AFSS-CMDSAP-0001
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestNewLWithGoodDataL()
	{
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TInt err = KErrNone;
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("Test:doTestNewLWithGoodDataL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:01 Test: NewL(RReadStream& aReadStream)"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs);
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd);		//validate the values
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:02 Test: NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo)"));
	severity = ECmdCriticalSeverity;
	SetAppInfoDataL(KBadFileName, KEmptyArgs);
	TRAP(err, iTestCmd = CCmdStartApp::NewL(severity, iAppInfo));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd);		//validate the values
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartApp::NewL(params));
	// set iAppInfo same as rsc file to compare
	SetAppInfoDataL(KAppInRsc, KArgsInRsc, ESsmFireAndForget, 10, 20);
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd);
	Reset();
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo, const TUint16 aPriority)"));
	INFO_PRINTF1(_L("Test NewL with priority"));
	severity = ECmdCriticalSeverity;
	SetAppInfoDataL(KBadFileName, KEmptyArgs);
	TRAP(err, iTestCmd = CCmdStartApp::NewL(severity, iAppInfo, KTestPriority));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd, KTestPriority);		//validate the values
	Reset();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters and priority too)"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader on contents
	RResourceReader& reader1 = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_PRIORITY);
	TSsmCommandParameters params1(reader1, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartApp::NewL(params1));
	// set iAppInfo same as rsc file to compare
	SetAppInfoDataL(KAppInRsc, KArgsInRsc, ESsmFireAndForget, 10, 20);
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd, KTestPriority);
	Reset();
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters and priority too)"));
	INFO_PRINTF1(_L("initial version = default priority"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader on contents
	RResourceReader& reader2 = LoadResourcesL(KTestRscFileName, R_COMMAND_DEFAULT_PRIORITY);
	TSsmCommandParameters params2(reader2, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartApp::NewL(params2));
	// set iAppInfo same as rsc file to compare
	SetAppInfoDataL(KAppInRsc, KArgsInRsc, ESsmFireAndForget, 10, 20);
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd, KDefaultCommandPriority);
	Reset();
	FreeResources();
	
	INFO_PRINTF1(_L("doTestNewLWithGoodDataL:07 Test: NewL(TSsmCommandParameters& aCommandParameters and priority too)"));
	INFO_PRINTF1(_L("negative priority will be interpreted as a very high positive value"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader on contents
	RResourceReader& reader3 = LoadResourcesL(KTestRscFileName, R_COMMAND_NEGATIVE_PRIORITY);
	TSsmCommandParameters params3(reader3, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartApp::NewL(params3));
	// set iAppInfo same as rsc file to compare
	SetAppInfoDataL(KAppInRsc, KArgsInRsc, ESsmFireAndForget, 10, 20);
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	CompareCommandsDataL(severity, iTestCmd, KNegPriority);
	Reset();
	FreeResources();

	
#endif
	}
/**
Old Test CaseID 		AFSS-CMDSAP-0002
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestNewLWithBadDataL()
	{
	TInt expectedErr = KErrArgument;
	TInt err = KErrNone;
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TSsmExecutionBehaviour badExecutionBehaviour = (TSsmExecutionBehaviour)-1;
	INFO_PRINTF1(_L("Test:doTestNewLWithBadDataL - Command construction with in-valid data"));

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:01 Test: NewL(RReadStream& aReadStream)- Create command with NULL file name"));
	SetAppInfoDataL(KEmptyFileName, KEmptyArgs);
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:02 Test: NewL(RReadStream& aReadStream)- Invalid execution behaviour"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs, badExecutionBehaviour);
	TRAP(err, iTestCmd = CreateCmdFromStreamL(severity));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:03 Test: NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo)- Create command with NULL file name"));
	severity = ECmdCriticalSeverity;
	SetAppInfoDataL(KEmptyFileName, KEmptyArgs);
	TRAP(err, iTestCmd = CCmdStartApp::NewL(severity, iAppInfo));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();
	INFO_PRINTF1(_L("doTestNewLWithBadDataL:04 Test: NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo)- Invalid execution behaviour"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs, badExecutionBehaviour);
	TRAP(err, iTestCmd = CCmdStartApp::NewL(severity, iAppInfo));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:05 Test: NewL(TSsmCommandParameters& aCommandParameters)- Create command with NULL file name"));
	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_INVALID_DATA);
	TSsmCommandParameters firstParam(reader, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartApp::NewL(firstParam));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();
	FreeResources();

	INFO_PRINTF1(_L("doTestNewLWithBadDataL:06 Test: NewL(TSsmCommandParameters& aCommandParameters)- Invalid execution behaviour"));
	// open reader on contents
	reader = LoadResourcesL(KTestRscFileName, R_COMMAND_INVALID_DATA1);
	TSsmCommandParameters secondParam(reader, ResourceFile(), 0);
	TRAP(err, iTestCmd = CCmdStartApp::NewL(secondParam));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();
	FreeResources();
	
    INFO_PRINTF1(_L("doTestNewLWithBadDataL:07 Test: CSsmStartupProperties :: Invalid Command type"));
    TRAP(err, iAppInfo->SetCommandTypeL(ESsmCmdWaitForApparcInit));
    INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
    TEST(err == expectedErr);
    Reset();
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0003
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */
void CCmdTestStartApp::doTestInternalizeLWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithGoodDataL -  InternalizeL with valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:01 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0004
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestInternalizeLWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestInternalizeLWithBadDataL - InternalizeL with in valid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:01 and doTestNewLWithBadDataL:02 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0005
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestResourceReaderWithGoodDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithGoodDataL - construct from resource with valid reader"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithGoodDataL:03 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0006
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestResourceReaderWithMissingDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithMissingDataL - construct from resource with invalid reader"));
	TInt expectedErr = KErrNotSupported;
	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_INVALID_DATA);
	//read some data so reader will become invalid
	reader.ReadUint16L();
	TSsmCommandParameters params(reader, ResourceFile(), 0);
	TRAPD(err, iTestCmd = CCmdStartApp::NewL(params));
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	Reset();
	FreeResources();
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0007
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestResourceReaderWithBadDataL()
	{
	INFO_PRINTF1(_L("Test:doTestResourceReaderWithBadDataL - Construct from resource with valid reader and invalid data"));
	INFO_PRINTF1(_L("**** Tested in doTestNewLWithBadDataL:05 & doTestNewLWithBadDataL:06 **** "));
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0008
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestCommandTypeL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	INFO_PRINTF1(_L("Test:doTestCommandTypeL - Command construction with valid data"));

	INFO_PRINTF1(_L("doTestCommandTypeL:01 Test: Construct from NewL(RReadStream& aReadStream) and check type"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs);
	iTestCmd = CreateCmdFromStreamL(severity);
	CompareCommandsDataL(severity, iTestCmd);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdStartApp);
	Reset();

	INFO_PRINTF1(_L("doTestCommandTypeL:02 Test: Construct from NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo) and check type"));
	severity = ECmdCriticalSeverity;
	SetAppInfoDataL(KBadFileName, KEmptyArgs);
	iTestCmd = CCmdStartApp::NewL(severity, iAppInfo);
	CompareCommandsDataL(severity, iTestCmd);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdStartApp);
	Reset();
	
	INFO_PRINTF1(_L("doTestCommandTypeL:03 Test: NewL(TSsmCommandParameters& aCommandParameters)  and check type"));
	severity = ECmdMediumSeverity;	//this severity is same as defined in r_command_valid_data
	// open reader on contents
	RResourceReader& reader = LoadResourcesL(KTestRscFileName, R_COMMAND_VALID_DATA);
	TSsmCommandParameters params(reader, ResourceFile(), 0);
	iTestCmd = CCmdStartApp::NewL(params);
	// set iAppInfo same as rsc file to compare
	SetAppInfoDataL(KAppInRsc, KArgsInRsc, ESsmFireAndForget, 10, 20);
	CompareCommandsDataL(severity, iTestCmd);
	TEST(iTestCmd->Type() == ESsmCmdStartApp);
	Reset();
	FreeResources();
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	INFO_PRINTF1(_L("doTestCommandTypeL:04 Test: Construct from NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo, const TUint16 aPriority) and check type"));
	severity = ECmdCriticalSeverity;
	SetAppInfoDataL(KBadFileName, KEmptyArgs);
	iTestCmd = CCmdStartApp::NewL(severity, iAppInfo, KTestPriority);
	CompareCommandsDataL(severity, iTestCmd, KTestPriority);		//validate the values
	TEST(iTestCmd->Type() == ESsmCmdStartApp);
	Reset();
#endif
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0009
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestCmdFailureIgnoreSeverityL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt expectedErr = KErrNone;
	INFO_PRINTF1(_L("Test:doTestCmdFailureIgnoreSeverityL - command failure with ECmdIgnoreFailure Severity"));

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:01 Start a non existing application with execution behaviour as ESsmWaitForSignal"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:02 Start a non existing application with execution behaviour as ESsmWaitForSignal and retries %d"), KNumOfRetries);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:03 Start a non existing application with execution behaviour as ESsmWaitForSignal and timeout %d "), KTestTimeOut);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:04 Start a non existing application with execution behaviour as ESsmDeferredWaitForSignal"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:05 Start a non existing application with execution behaviour as ESsmDeferredWaitForSignal and retries %d"), KNumOfRetries);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCmdFailureIgnoreSeverityL:06 Start a non existing application with execution behaviour as ESsmDeferredWaitForSignal and timeout %d "), KTestTimeOut);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0010
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestCmdFailureHighSeverityL()
	{
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TInt expectedErr = KErrNotFound;
	INFO_PRINTF1(_L("Test:doTestCmdFailureHighSeverityL - command failure with ECmdHighSeverity Severity"));

	INFO_PRINTF1(_L("doTestCmdFailureHighSeverityL:01 Start a non existing application with execution behaviour as ESsmWaitForSignal"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF2(_L("doTestCmdFailureHighSeverityL:02 Start a non existing application with execution behaviour as ESsmWaitForSignal and retries %d"), KNumOfRetries);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF2(_L("doTestCmdFailureHighSeverityL:03 Start a non existing application with execution behaviour as ESsmWaitForSignal and timeout %d "), KTestTimeOut);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestCmdFailureHighSeverityL:04 Start a non existing application with execution behaviour as ESsmDeferredWaitForSignal"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCmdFailureHighSeverityL:05 Start a non existing application with execution behaviour as ESsmDeferredWaitForSignal and retries %d"), KNumOfRetries);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCmdFailureHighSeverityL:06 Start a non existing application with execution behaviour as ESsmDeferredWaitForSignal and timeout %d "), KTestTimeOut);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0011
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestCancelCmdIgnoreSeverityL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt expectedErr = KErrCancel;
	INFO_PRINTF1(_L("Test:doTestCancelCmdIgnoreSeverityL - command failure with ECmdIgnoreFailure Severity"));

	INFO_PRINTF1(_L("doTestCmdFailureIgnoreSeverityL:01 Start and cancel a non existing application with execution behaviour as ESsmWaitForSignal"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF2(_L("doTestCancelCmdIgnoreSeverityL:02 Start  and cancel a non existing application with execution behaviour as ESsmWaitForSignal and retries %d"), KNumOfRetries);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF2(_L("doTestCancelCmdIgnoreSeverityL:03 Start and cancel a non existing application with execution behaviour as ESsmWaitForSignal and timeout %d "), KTestTimeOut);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF1(_L("doTestCancelCmdIgnoreSeverityL:04 Start and cancel a non existing application with execution behaviour as ESsmDeferredWaitForSignal"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCancelCmdIgnoreSeverityL:05 Start and cancel a non existing application with execution behaviour as ESsmDeferredWaitForSignal and retries %d"), KNumOfRetries);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCancelCmdIgnoreSeverityL:06 Start and cancel a non existing application with execution behaviour as ESsmDeferredWaitForSignal and timeout %d "), KTestTimeOut);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0012
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestCancelCmdHighSeverityL()
	{
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TInt expectedErr = KErrCancel;
	INFO_PRINTF1(_L("Test:doTestCancelCmdHighSeverityL - command failure with ECmdHighSeverity Severity"));

	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:01 Start and cancel a non existing application with execution behaviour as ESsmWaitForSignal"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF2(_L("doTestCancelCmdHighSeverityL:02 Start  and cancel a non existing application with execution behaviour as ESsmWaitForSignal and retries %d"), KNumOfRetries);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF2(_L("doTestCancelCmdHighSeverityL:03 Start and cancel a non existing application with execution behaviour as ESsmWaitForSignal and timeout %d "), KTestTimeOut);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue);

	INFO_PRINTF1(_L("doTestCancelCmdHighSeverityL:04 Start and cancel a non existing application with execution behaviour as ESsmDeferredWaitForSignal"));
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCancelCmdHighSeverityL:05 Start and cancel a non existing application with execution behaviour as ESsmDeferredWaitForSignal and retries %d"), KNumOfRetries);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmWaitForSignal, KNumOfRetries);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);

	INFO_PRINTF2(_L("doTestCancelCmdHighSeverityL:06 Start and cancel a non existing application with execution behaviour as ESsmDeferredWaitForSignal and timeout %d "), KTestTimeOut);
	SetAppInfoDataL(KBadFileName, KEmptyArgs, ESsmDeferredWaitForSignal, 0, KTestTimeOut);
	CreateCmdAndExecuteL(severity, expectedErr, ETrue, KTimeoutMultipleWait);
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0013
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestSuccessfulExecution1L()
	{
	INFO_PRINTF1(_L("Test successful command execution with  ECmdIgnoreFailure Severity"));
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt expectedErr = KErrNone;

	INFO_PRINTF1(_L("doTestSuccessfulExecution1L:01 Start an application with execution behaviour as ESsmFireAndForget"));
	SetAppInfoDataL(KTestAppGood, KEmptyArgs, ESsmFireAndForget);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestSuccessfulExecution1L:02 Start an application with execution behaviour as ESsmDeferredWaitForSignal"));
	SetAppInfoDataL(KTestAppGood, KEmptyArgs, ESsmDeferredWaitForSignal, 0, 20000);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);

	INFO_PRINTF1(_L("doTestSuccessfulExecution1L:03 Start an application with execution behaviour as ESsmWaitForSignal"));
	SetAppInfoDataL(KTestAppGood, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr);

	// we are expecting three instances of KTestAppGood
	TEST(3 == FindAndKill(KTestAppGood));
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestSuccessfulExecution2L()
	{
	INFO_PRINTF1(_L("Test successful command execution with  ECmdHighSeverity Severity"));
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt expectedErr = KErrNone;

	INFO_PRINTF1(_L("doTestSuccessfulExecution2L:01 Start an application with execution behaviour as ESsmFireAndForget"));
	SetAppInfoDataL(KTestAppGood, KEmptyArgs, ESsmFireAndForget);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestSuccessfulExecution2L:01 Start an application with execution behaviour as ESsmWaitForSignal"));
	SetAppInfoDataL(KTestAppGood, KEmptyArgs, ESsmWaitForSignal);
	CreateCmdAndExecuteL(severity, expectedErr);

	INFO_PRINTF1(_L("doTestSuccessfulExecution2L:06 Start an application with execution behaviour as ESsmDeferredWaitForSignal"));
	SetAppInfoDataL(KTestAppGood, KEmptyArgs, ESsmDeferredWaitForSignal, 0, 20000);
	CreateCmdAndExecuteL(severity, expectedErr, EFalse, KTimeoutMultipleWait);
	// we are expecting three instances of KTestAppGood
	TEST(3 == FindAndKill(KTestAppGood));
	}

/**
Old Test CaseID 		AFSS-CMDSAP-0015
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestDwfsStateAppWithMonitorL()
	{
	// The KIntervalForReLaunchRateOfFailure time interval is 30mins by default, its patched to 25secs on emulator in epoc.ini file
	// and for hardware in iby file.
	INFO_PRINTF1(_L("For test to pass in Winscw - (patchdata_sysmon_exe_KIntervalForReLaunchRateOfFailure 25) should be added to epoc32\\data\\epoc.ini file"));

	INFO_PRINTF1(_L("Test:doTestDwfsStateAppWithMonitorL - command ECmdIgnoreFailure Severity"));
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	TInt expectedErr = KErrNone;
	SetAppInfoDataL(KTestAppGood, KEmptyArgs, ESsmDeferredWaitForSignal, 1, 0);
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iRetries = 1;
	monitorInfo.iTimeout = 0;	
	iAppInfo->SetMonitorInfoL(monitorInfo);
	INFO_PRINTF1(_L("doTestDwfsStateAppWithMonitorL:Start a application with execution behaviour as ESsmDeferredWaitForSignal"));
	CreateCmdAndExecuteL(severity, expectedErr);
	
	//Waiting to decrement the count of the semaphore which is being signalled as the application is started first time 
	iSem.Wait();
		
	INFO_PRINTF1(_L("Call Find and Kill now - instances should be one"));
	//find and kill the application
	TEST(1 == FindAndKill(KTestAppGood));
	
	INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Wait for one second and call Find and Kill again - instances should be zero"));
	User::After(1000000);
	// system monitor waits for 15secs(throttle time) before restarting the application
	TEST(0 == FindAndKill(KTestAppGood)); 
	
	INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Wait for Throttle time and call find and kill again - instances should be one"));
	//Waiting for the application to restart using semaphore
	iSem.Wait();
	// application is restarted by system monitor
	TEST(1 == FindAndKill(KTestAppGood));
	
	INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Wait for one second and call Find and Kill again - instances should be zero"));
	User::After(1000000);
	// application is restarted by system monitor
	TEST(0 == FindAndKill(KTestAppGood));
	
	INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Wait for Throttle time and call find and kill again - instances should be one"));
	//Waiting for the application to restart using semaphore
	iSem.Wait();
	// application is restarted by system monitor
	TEST(1 == FindAndKill(KTestAppGood));
	
	INFO_PRINTF1(_L("-----------------------------------------------------------------------------"));
	INFO_PRINTF1(_L("Wait for more than throttle time and call find and kill again - instances should be one"));
	INFO_PRINTF1(_L("Done to ensure that the app is closed and not hanging around"));
	//Waiting for the application to restart using semaphore
	iSem.Wait();
	TEST(1 == FindAndKill(KTestAppGood));
	}
	
/**
Old Test CaseID 		AFSS-CMDSAP-0016
New Test CaseID 		DEVSRVS-SSMA-CMD-0018
 */

void CCmdTestStartApp::doTestNegativeRetryL()
	{
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	SetAppInfoDataL(KTestAppGood, KArgsInRsc, ESsmFireAndForget, -1);
	CCmdStartApp* cmd = CCmdStartApp::NewL(severity, iAppInfo);
	}


/**
Helper functiosn to compare the commands data.
*/
void CCmdTestStartApp::CompareCommandsDataL(TCmdErrorSeverity aSeverity, CCmdStartApp* aTestCmd, const TUint16 aPriority)
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
	TEST(iAppInfo->CommandType() == commandType);
	HBufC* fileName = HBufC::NewL(readStream, KMaxFileName);
	CleanupStack::PushL(fileName);
	HBufC* args = HBufC::NewL(readStream, KMaxFileName);
	CleanupStack::PushL(args);
	TSsmExecutionBehaviour executionBehaviour = static_cast<TSsmExecutionBehaviour>(readStream.ReadUint8L());
	TInt32 timeout = readStream.ReadInt32L();
	TInt numOfRetries = readStream.ReadInt16L();
	if (commandType == ESsmCmdStartApp)
		{
		TBool viewless = readStream.ReadUint8L();
		TBool startInBackground = readStream.ReadUint8L();
		}
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
CCmdStartApp* CCmdTestStartApp::CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	{
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt16L(aSeverity);
	writeStream.WriteUint16L(iAppInfo->CommandType());
	writeStream << iAppInfo->FileName();
	writeStream << iAppInfo->Args();;
	writeStream.WriteUint8L(iAppInfo->ExecutionBehaviour());
	writeStream.WriteInt32L(iAppInfo->Timeout());
	writeStream.WriteInt16L(iAppInfo->Retries());
	writeStream.WriteUint8L(iAppInfo->Viewless());
	writeStream.WriteUint8L(iAppInfo->StartInBackground());

	writeStream.WriteUint8L(iAppInfo->IsMonitoringRequired());
	if (iAppInfo->IsMonitoringRequired())
		{
		writeStream.WriteUint8L(iAppInfo->MonitorInfo().iRestartPolicy);
		writeStream.WriteUint8L(iAppInfo->MonitorInfo().iRestartMode);
		writeStream.WriteInt32L(iAppInfo->MonitorInfo().iTimeout);
		writeStream.WriteInt16L(iAppInfo->MonitorInfo().iRetries);
		}
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(aPriority);
#else
	TEST(aPriority == KDefaultPriority);
#endif
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	CCmdStartApp* newCmd = CCmdStartApp::NewL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	
	return newCmd;
	}

/**
Helper functiosn to set specified values in iAppInfo.
*/
void CCmdTestStartApp::SetAppInfoDataL(const TDesC& aFileName, const TDesC& aArgs, TSsmExecutionBehaviour aExecutionBehaviour, TInt aNumOfRetries, TInt32 aTimeout)
	{
	iAppInfo->Reset();
	iAppInfo->SetFileParamsL(aFileName, aArgs);
	iAppInfo->SetCommandTypeL(ESsmCmdStartApp);
	iAppInfo->SetExecutionBehaviour(aExecutionBehaviour);
	iAppInfo->SetRetries(aNumOfRetries);
	iAppInfo->SetTimeout(aTimeout);
	}

/**
Resets iAppInfo
*/
void CCmdTestStartApp::Reset()
	{
	iAppInfo->Reset();
	delete iTestCmd;
	iTestCmd = NULL;
	}

/**
Helper function to run a command and test the expected results
*/
void CCmdTestStartApp::CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TInt aExpectedErr, TBool aCancel, TInt32 aTimeout)
	{
	iTestList = CCmdTestList::NewL(StopSchedulerCallBack, this);
	iTestList->AddStartApplicationCommandL(aSeverity, iAppInfo);
	if (ESsmDeferredWaitForSignal == iAppInfo->ExecutionBehaviour())
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
	iTestList->CancelAllCommands();
	delete iTestList;
	iTestList = NULL;
	Reset();
	}

