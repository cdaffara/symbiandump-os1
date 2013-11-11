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


#ifndef __TCUSTCMD_STEP_CUSTOMCMDACTIVATEEMERGENCYCALL_H__
#define __TCUSTCMD_STEP_CUSTOMCMDACTIVATEEMERGENCYCALL_H__

#include "tcustcmd_const.h" 
#include <test/testexecutestepbase.h>
#include <ssm/ssmstatemanager.h>

_LIT(KTCCustomCmdTestActivateEmergencyCall, "CCustomCmdTestActivateEmergencyCall");

class CCustomCmdTestActivateEmergencyCall: public CTestStep
	{
public:
	CCustomCmdTestActivateEmergencyCall();
	~CCustomCmdTestActivateEmergencyCall();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:	
	void TestHandleActivateEmergencyCallL();
	void TestHandleActivateEmergencyCallCancelL();
	
private:
	CActiveScheduler* iActiveScheduler;
	CActiveSchedulerWait* iActiveSchedulerWait;
	CAsyncCallBack* iAsyncStopScheduler;
	};

#endif	// __TCUSTCMD_STEP_CUSTOMCMDSECURITYPINCHECK_H__
