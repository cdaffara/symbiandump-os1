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


#ifndef __TCUSTCMD_STEP_CUSTOMCMDSECURITYPINCHECK_H__
#define __TCUSTCMD_STEP_CUSTOMCMDSECURITYPINCHECK_H__

#include "tcustcmd_const.h"
#include <ssm/ssmstartupproperties.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmswp.h>

_LIT(KTCCustomCmdTestSecurityPinCheck, "CCustomCmdTestPinCheck");

class CStopSimulatekey;
class CCustomCmdTestSecurityPinCheck: public CTestStep
	{
public:
	CCustomCmdTestSecurityPinCheck();
	~CCustomCmdTestSecurityPinCheck();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

	void CallBackForEnterPin();
	void StopScheduler();
	void CancelSwpRequest();
	TSsmExecutionBehaviour GetExecutionBehaviour();
private:	
	void TestHandleSecCustCmdWithExeBehaviourWaitForSignalL();
	void TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalL();
	void TestHandleSecCustCmdWithExeBehaviourFireAndForgetL();
	
	void TestHandleSecCustCmdWithExeBehaviourWaitForSignalCancelL();
	void TestHandleSecCustCmdWithExeBehaviourDeferredWaitForSignalCancelL();

	void CreateAndExecuteCustCmd();
	void CreateAndExecuteCancelCustCmd();
	
	void TestOOMDevSecCustCmdwithExeBehaviourDWFL();
	
	void StartSimulateKey();
	void RegisterSimSecurityStatus();
	void WritePinCheckTestCaseNumToFileL(TInt aTestNum);

private:
	CActiveScheduler* iActiveScheduler;
	CActiveSchedulerWait* iActiveSchedulerWait;
	CAsyncCallBack* iAsyncStopScheduler;
	TRequestStatus iStatus;
	TRequestStatus iCancelReqStatus;
	TBool iValidPin;
	RFs iFs;
	CStopSimulatekey* iStopSimulatekey;
	TSsmExecutionBehaviour iExecutionBehaviour;
	RFile iFile;
	TSsmSwp iSwp;
	RSsmStateManager iClient;
	RProperty iProperty;
	TBool iCancelRequest;
	};

class CStopSimulatekey	: public CActive
	{
	public:
	static CStopSimulatekey* NewL(CCustomCmdTestSecurityPinCheck* aCustomCmdTestSecurityPinCheck); 
	~CStopSimulatekey();
	void Start();
	void Construct(CCustomCmdTestSecurityPinCheck* aCustomCmdTestSecurityPinCheck);
	//from CActive
	void DoCancel();
	void RunL();
	
	private:
	CStopSimulatekey();
	
	private :
	CCustomCmdTestSecurityPinCheck *iCustomCmdTestSecurityPinCheck;
	RProperty iProperty;
	};

#endif	// __TCUSTCMD_STEP_CUSTOMCMDSECURITYPINCHECK_H__
