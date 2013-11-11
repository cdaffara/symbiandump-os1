// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __TSYSMON_STEPCRITICALNORETRIES_H__
#define __TSYSMON_STEPCRITICALNORETRIES_H__

#include "tsysmon_stepbase.h"
#include "sysmontesthelper.h"
#include <ssm/ssmstateawaresession.h>
#include <s32file.h>

//APPFWK-SYSMON-0004

_LIT(KCTestCaseCriticalNoRetries, "CriticalNoRetries");
_LIT(KCTestCaseRestartOS, "RestartOS");
_LIT(KCTestCaseCriticalNoRetriesStep, "CriticalNoRetriesStep");
_LIT(KCTestCaseRestartOSStep, "RestartOSStep");

class CStepOSRestart : public CTestStepBase, public MStateChangeNotificationSubscriber
	{
public:
	CStepOSRestart(const TDesC& aTestName);
	void CallStopSchedulerL();
	void StateChanged(TSsmState aSsmState);
	~CStepOSRestart();
	
	//from CTestStep
	TVerdict doTestStepL();
protected:
	RSsmStateAwareSession iSsmStateAwareSess;
	CAsyncCallBack* iAsyncCallbackForStartTest;
    CAsyncCallBack* iAsyncCallbackForStopScheduler;
    
private:
    void TestCriticalNoRetriesL();
    void TestRestartOSL();
    
private:
    const TDesC& iTestName;
	};

#endif
