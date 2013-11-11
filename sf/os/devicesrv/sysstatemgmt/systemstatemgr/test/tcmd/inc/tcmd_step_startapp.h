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
#ifndef __TCMD_STEP_STARTAPP_H__
#define __TCMD_STEP_STARTAPP_H__

#include "tcmd_step_base.h"
#include "ssmcommandfactory.h"

_LIT(KTCCmdTestStartApp, "CCmdTestStartApp");

class CSsmStartupProperties;
class CCmdStartApp;

class CCmdTestStartApp:  public CCmdTestBase
	{
public:
	CCmdTestStartApp();
	~CCmdTestStartApp();

	//from CTestStep
	TVerdict doTestStepL();

private:
	//from CCmdTestBase
	void doTestNewLWithGoodDataL();
	void doTestNewLWithBadDataL();
	void doTestInternalizeLWithGoodDataL();
	void doTestInternalizeLWithBadDataL();
	void doTestResourceReaderWithGoodDataL();
	void doTestResourceReaderWithMissingDataL();
	void doTestResourceReaderWithBadDataL();
	void doTestCommandTypeL();
	void doTestCmdFailureIgnoreSeverityL();
	void doTestCmdFailureHighSeverityL();
	void doTestCancelCmdIgnoreSeverityL();
	void doTestCancelCmdHighSeverityL();
	void doTestSuccessfulExecution1L();
	void doTestSuccessfulExecution2L();
	void doTestDwfsStateAppWithMonitorL();
	void doTestNegativeRetryL();
	
private:
	CCmdStartApp* CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, const TUint16 aPriority = KDefaultPriority);
	void CompareCommandsDataL(TCmdErrorSeverity aSeverity, CCmdStartApp* aTestCmd, const TUint16 aPriority = KDefaultPriority);
	void SetAppInfoDataL(const TDesC& aFileName, const TDesC& aArgs, TSsmExecutionBehaviour aExecutionBehaviour = ESsmWaitForSignal, TInt aNumOfRetries = 0, TInt32 aTimeout = 0);
	void Reset();
	void CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TInt aExpectedErr, TBool aCancel = EFalse, TInt32 aTimeout = 0);

private:
	CCmdStartApp* iTestCmd;
	CSsmStartupProperties* iAppInfo;
	RSemaphore iSem;
	};

#endif	// __TCMD_STEP_STARTAPP_H__
