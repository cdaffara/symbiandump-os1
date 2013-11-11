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

#ifndef __TCMD_STEP_SIMSECURITYCHECK_H__
#define __TCMD_STEP_SIMSECURITYCHECK_H__

#include "cmdsimsecuritycheck.h"
#include <test/testexecutestepbase.h>

_LIT(KTCCustomCmdTestSimSecurityCheck, "CCustomCmdTestSimSecurityCheck");

class CCustomCmdTestSimSecurityCheck: public CTestStep
	{
public:
	CCustomCmdTestSimSecurityCheck();
	~CCustomCmdTestSimSecurityCheck();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

public:
	void CallBack1RunL();
	void CallBack2RunL();
	void CallBack3RunL();

private:
	void TestGeneralL();
	
	void TestHandleSIMPresent1L();
	void TestHandleSIMPresent2L();
	void TestHandleSIMPresent3L();

    void TestHandleSIMInvalid1L();
    void TestHandleSIMInvalid2L();
    void TestHandleSIMInvalid3L();

	
	void TestHandleSIMRejected1L();
	void TestHandleSIMRejected2L();
	void TestHandleSIMRejected3L();
	
	void TestHandleSIMBlocked1L();
	void TestHandleSIMBlocked2L();
	void TestHandleSIMBlocked3L();
	void TestHandleSIMBlocked4L();
	
	void TestHandlePINRequired1L();
	void TestHandlePINRequired2L();
	void TestHandlePINRequired3L();
	void TestHandlePINRequired4L();

	void TestHandleSIMLock1L();
	void TestHandleSIMLock2L();
	void TestHandleSIMLock3L();
	void TestHandleSIMLock4L();
	void TestHandleSIMLock5L();

	void TestHandlePINAsked1L();
	void TestHandlePINAsked2L();
	void TestHandlePINAsked3L();
	void TestHandlePINAsked4L();

	void TestHandlePUKAskedL();

	void TestSimFunctionsL();
	void TestSimNotSupported();
	void TestSimRemoved();
	void TestSimReadable();
	void TestSimUsable();

	void TestHandleSIMPresentL();
	void TestHandleSIMInvalidL();
	void TestHandleSIMRejectedL();
	void TestHandleSIMBlockedL();
	void TestHandlePINRequiredL();
	void TestHandleSIMLessOfflineSupportedL();
	void TestHandleSIMLockL();
	void TestHandlePINAskedL();

	void TestSecurityNoteObserverL();
	void TestSecurityNoteObserver1L();
	void TestSecurityNoteObserver2L();

	void SimulatePasswordEntry();
	void RegisterSimSecurityStatus();
	
private:
	CActiveScheduler* iActiveScheduler;
	CActiveSchedulerWait* iActiveSchedulerWait;
	CAsyncCallBack* iAsyncStopScheduler;
	CCustomCmdSimSecurityCheck* iCustomCmdCheck;
	TRequestStatus iRequest;
	RFs iFs;
	};

#endif	// __TCMD_STEP_SIMSECURITYCHECK_H__
