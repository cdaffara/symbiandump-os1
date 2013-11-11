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
#ifndef __TCMD_STEP_STARTPROCESS_H__
#define __TCMD_STEP_STARTPROCESS_H__

#include "tcmd_step_base.h"
#include "ssmcommandfactory.h"

_LIT(KTCCmdTestStartProcess, "CCmdTestStartProcess");

class CSsmStartupProperties;
class CCmdStartProcess;

class CCmdTestStartProcess:  public CCmdTestBase
	{
public:
	CCmdTestStartProcess();
	~CCmdTestStartProcess();

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
	void doTestDwfsStateWithMonitorL();
	void doTestNegativeRetryL();
	
private:
	CCmdStartProcess* CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, const TUint16 aPriority = KDefaultPriority);
	void CompareCommandsDataL(TCmdErrorSeverity aSeverity, CCmdStartProcess* aTestCmd, const TUint16 aPriority = KDefaultPriority);
	void SetProcessInfoDataL(const TDesC& aFileName, const TDesC& aArgs, TSsmExecutionBehaviour aExecutionBehaviour = ESsmWaitForSignal, TInt aNumOfRetries = 0, TInt32 aTimeout = 0);
	void Reset();
	void CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TInt aExpectedErr, TBool aCancel = EFalse, TInt32 aTimeout = 0);

private:
	CCmdStartProcess* iTestCmd;
	CSsmStartupProperties* iProcessInfo;
	RSemaphore iSem;
	};

#endif	// __TCMD_STEP_STARTPROCESS_H__
