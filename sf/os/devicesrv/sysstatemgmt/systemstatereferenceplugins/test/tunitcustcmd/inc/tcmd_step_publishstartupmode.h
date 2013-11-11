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


#ifndef __TCMD_STEP_PUBLISHSTARTUPMODE_H__
#define __TCMD_STEP_PUBLISHSTARTUPMODE_H__

#include <test/testexecutestepbase.h>

_LIT(KTestPublishStartupModeWithCap, "TestPublishStartupModeWithCap");
_LIT(KTestPublishStartupModeWithoutCap,"TestPublishStartupModeWithoutCap");

class CCustomCmdTestPublishStartupMode: public CTestStep
	{
public:
	CCustomCmdTestPublishStartupMode(const TDesC& aTestStepName);
	~CCustomCmdTestPublishStartupMode();

	//from CTestStep
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

	void CallBackRunL();
	
private:
	void doTestCreateExecuteAndDestroyL();
	void doTestFactoryCreateAndExecuteCancelL();

private:
    CActiveScheduler* iActiveScheduler;
    CAsyncCallBack* iAsyncStopScheduler;
    TRequestStatus iRequestStatus;
    const TDesC& iTestStepName;
	};

#endif	// __TCMD_STEP_PUBLISHSTARTUPMODE_H__
