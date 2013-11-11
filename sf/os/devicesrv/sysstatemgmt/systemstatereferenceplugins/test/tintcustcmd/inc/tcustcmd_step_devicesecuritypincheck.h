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


#ifndef __TCUSTCMD_STEP_CUSTOMCMDDEVICESECURITYPINCHECK_H__
#define __TCUSTCMD_STEP_CUSTOMCMDDEVICESECURITYPINCHECK_H__

#include "tcustcmd_const.h"
#include <ssm/ssmstartupproperties.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmswp.h>

_LIT(KTCCustomCmdTestDeviceSecurityPinCheck, "CCustomCmdTestDevicePinCheck");

class CStopSimulateDevicekey;
class CCustomCmdTestDeviceSecurityPinCheck: public CTestStep
	{
public:
	CCustomCmdTestDeviceSecurityPinCheck();
	~CCustomCmdTestDeviceSecurityPinCheck();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

	void CallBackForEnterPin();
	void StopScheduler();
private:	
	void TestHandleSecCustCmdWithExeBehaviourWaitForSignal();
	void TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignal();
	
	void TestHandleSecCustCmdWithExeBehaviourWaitForSignalCancel();
	void TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalCancel();
	
	void TestOOMDevSecCustCmdWithExeBehaviourWFL();
	void TestOOMDevSecCustCmdWithExeBehaviourDWFL();
	
	void CreateAndExecuteCustCmd();
	void CreateAndExecuteCancelCustCmd();
	
	void StartSimulateKey();
	void RegisterDeviceSecurityStatus();

private:
	CActiveScheduler* iActiveScheduler;
	CActiveSchedulerWait* iActiveSchedulerWait;
	CAsyncCallBack* iAsyncStopScheduler;
	TRequestStatus iStatus;
	TRequestStatus iCancelReqStatus;
	CStopSimulateDevicekey* iStopSimulatekey;
	TSsmExecutionBehaviour iExecutionBehaviour;
	TSsmSwp iSwp;
	RSsmStateManager iClient;
	RProperty iProperty;
	};

class CStopSimulateDevicekey	: public CActive
	{
	public:
	static CStopSimulateDevicekey* NewL(CCustomCmdTestDeviceSecurityPinCheck* aCustomCmdTestDeviceSecurityPinCheck); 
	~CStopSimulateDevicekey();
	void Start();
	void Construct(CCustomCmdTestDeviceSecurityPinCheck* aCustomCmdTestDeviceSecurityPinCheck);
	//from CActive
	void DoCancel();
	void RunL();
	
	private:
	CStopSimulateDevicekey();
	
	private :
	CCustomCmdTestDeviceSecurityPinCheck *iCustomCmdTestDeviceSecurityPinCheck;
	RProperty iProperty;
	};

#endif	// __TCUSTCMD_STEP_CUSTOMCMDDeviceSecurityPINCHECK_H__
