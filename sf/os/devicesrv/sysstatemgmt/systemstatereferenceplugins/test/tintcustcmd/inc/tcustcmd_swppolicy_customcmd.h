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


#ifndef __TSSM_SWPPOLICY_CUSTOMCMD_H__
#define __TSSM_SWPPOLICY_CUSTOMCMD_H__

#include "e32base.h" 
#include <ssm/ssmcommandfactory.h>
#include <ssm/ssmswppolicy.h>

/*
 SecurityCustomCommand Test Case ID

 @internalComponent
 @released
*/
enum TSwpValForCustCmd
	{
	ESwpValForActivateEmergencyCall = 11,
	ESwpValForDeActivateEmergencyCall = 22,
	ESwpValForSecCustCmdWaitForSignal = 33,
	ESwpValForSecCustCmdDeferredWaitForSignal = 44,
	ESwpValForSecCustCmdFireAndForget = 55,
	ESwpValForStopSchedulerCmdWaitForSignal = 66,
	ESwpValForStopSchedulerCmdDeferredWaitForSignal = 77,
	ESwpValForSecCustCmdCancelRequest = 99
	};	

NONSHARABLE_CLASS(CTestSsmSwpCustomCmd) : public CBase, public MSsmSwpPolicy
	{
public:
	IMPORT_C static MSsmSwpPolicy* NewL(); //lint !e1511
	
	//from MSsmSwpPolicy
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	TResponse TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage);
	void PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	void HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus);
	void HandleCleReturnValueCancel();
	
private:
	//from CBase
	CTestSsmSwpCustomCmd();
	~CTestSsmSwpCustomCmd();

private:
	void doPrepareCommandListL(TSsmSwp aSwp);

private:
	TSsmExecutionBehaviour iExecutionBehaviour;
	CSsmCommandList* iCommandList;
	};

#endif	// __TSSM_SWPPOLICY_CUSTOMCMD_H__
