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


#ifndef __TCLAYER_STEP_SSMEVENTOBSERVER_H__
#define __TCLAYER_STEP_SSMEVENTOBSERVER_H__

#include <etelmm.h> 
#include <test/testexecutestepbase.h>

_LIT(KTCLayerTestSsmEventObserver, "CLayerTestSsmEventObserver");

class CLayerTestSsmEventObserver : public CTestStep
	{
public:
	CLayerTestSsmEventObserver();
	~CLayerTestSsmEventObserver();

	//from CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void CallBackRunL();
	void SimulatePasswordEntry();
private:
	void TestStartSecurityEventObserverSchedulerL();
	void TestSecurityEventObserverL();
	void TestSecurityEventObserver1L();
	void TestSecurityEventObserver2L();
	void TestSecurityEventObserver3L();
	void TestSecurityEventObserver4L();
	void TestSecurityEventObserver5L();
	
	void TestSecNoteRequestQueueL();
	void TestSecPhaseObserverL();
	void TestSecurityNoteControllerL();
	void TestSecurityEventObserverHelperL(RMobilePhone::TMobilePhoneSecurityEvent aSecurityEvent, RMobilePhone::TMobilePhoneSecurityEvent aSecurityVerificationEvent);
	
private:
	CActiveScheduler* iActiveScheduler;
	CActiveSchedulerWait* iActiveSchedulerWait;
	CAsyncCallBack* iAsyncStopScheduler;
	TRequestStatus* iRequest;
	};

#endif	// __TCLAYER_STEP_SSMEVENTOBSERVER_H__
