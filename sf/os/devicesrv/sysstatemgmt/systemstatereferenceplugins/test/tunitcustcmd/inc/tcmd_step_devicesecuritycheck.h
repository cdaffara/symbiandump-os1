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

#ifndef __TCMD_STEP_DEVICESECURITYCHECK_H__
#define __TCMD_STEP_DEVICESECURITYCHECK_H__

#include "cmddevicesecuritycheck.h"
#include <test/testexecutestepbase.h>

_LIT(KTCCustomCmdTestDeviceSecurityCheck, "CCustomCmdTestDeviceSecurityCheck");

class CCustomCmdTestDeviceSecurityCheck: public CTestStep
	{
public:
	CCustomCmdTestDeviceSecurityCheck();
	~CCustomCmdTestDeviceSecurityCheck();
	void CallBackRunL();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	void SimulatePasswordEntry();
	
	void TestLockPhoneDeviceRespReceivedHelperL();
	void TestLockPhoneDeviceRespReceivedL();
	void TestLockPhoneDeviceRespReceived1L();
	void TestLockPhoneDeviceRespReceived2L();
	void TestLockPhoneDeviceRespReceived3L();
	void TestLockPhoneDeviceRespReceived4L();
	void TestLockPhoneDeviceRespReceived5L();
	
	void TestLockPhoneToIccRespReceivedHelperL();
	void TestLockPhoneToIccRespReceivedL();
	void TestLockPhoneToIccRespReceived1L();
	void TestLockPhoneToIccRespReceived2L();
	void TestLockPhoneToIccRespReceived3L();

	void TestSecCodeQueryRespReceivedL();
	void TestSecCodeQueryRespReceived1L();
	void TestSecCodeQueryRespReceived2L();

private:
	CActiveScheduler* iActiveScheduler;
	CActiveSchedulerWait* iActiveSchedulerWait;
	CAsyncCallBack* iAsyncStopScheduler;
	TRequestStatus iRequest;
	CCustomCmdDeviceSecurityCheck* iCustomCmdDevSecurityCheck;
	TBool iWrongPwd;
	};

#endif	// __TCMD_STEP_DEVICESECURITYCHECK_H__
