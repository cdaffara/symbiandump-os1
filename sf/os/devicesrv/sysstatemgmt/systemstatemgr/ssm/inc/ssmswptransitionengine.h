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

#ifndef __SSMSWPTRANSITIONENGINE_H__
#define __SSMSWPTRANSITIONENGINE_H__

#include <e32base.h>
#include <ssm/ssmswp.h>
#include <ssm/ssmcmd.hrh>
#include "ssmswppolicyproxy.h"

class CSsmSwpPolicyFrame;
class CSsmCommandList;
class MCleSessionProxy;
class CSsmSwpPolicyFrame;

/** 
The CSsmSwpTransitionEngine class is responsible for:-
- retrieving the command list from the policy
- executing the command list
- informing the client of the result when the commands have finished executing

@internalComponent
@released

*/
class CSsmSwpTransitionEngine : public CActive
	{
public:
	// construction
	static CSsmSwpTransitionEngine* NewL(const TSsmSwp& SwpValue);
	~CSsmSwpTransitionEngine();

	// wiring
	void SetClientMessage(const RMessage2* aMessage);
	void SetCleSessionProxy(MCleSessionProxy* aCleSession);
	void SetSsmSwpPolicySession(CSsmSwpPolicyCliSession* aSsmSwpPolicySession);
	void SetSwpPolicyFrame(CSsmSwpPolicyFrame* aPolicyFrame);

	// functions
#ifdef TEST_SSM_SERVER
	void Submit();
#endif
	void Submit(TRequestStatus& aClientStatus);
	void CompleteClientMessage(TInt aError);
	CSession2* OriginatingSesssion() const;
	void PerformCommandListValidation(TBool aSetting);
	TBool InTransition() const;
	const TSsmSwp& TransitionValue() const;
	MCleSessionProxy* CleSessionProxy();

protected:
	//from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	CSsmSwpTransitionEngine(const TSsmSwp& SwpValue);
	void ConstructL();
	void DoSubmit();
	void Start();
	void CancelCommandExecution();
	void DoConnectCleSessionL();
	void DoInitialize();
	TBool DoTransitionAllowed();
	void DoPrepareCommandList();
	void DoExecuteCommandList();
	void DoValidation();
	void DoHandleCleReturnValue();
	void DoRunError(TInt aError);

private:
	enum TInternalActions
		{
		EIdle,
		EUnConnected,
		ESetSwpPolicyDllHandleAndInitialize,
		ETransitionAllowed,
		EPrepareCommandList,
		EExecuteCommandList,
		EWaitForCommandListExecute,
		EWaitForHandleCleReturnValue,
		ETransitionComplete
		};
		
	/** the policy frame used to access the policy DLL functions */
	CSsmSwpPolicyFrame* iPolicyFrame; // owned by me
	/** the interface used to communicate with the command executor */
	MCleSessionProxy* iCleSession; // not owned by me
	/** The client (caller) status */
	TRequestStatus* iClientStatus; // not owned by me
	/** The current change in progress */
	TSsmSwp iCurrentSwpValue; // copied to me
	/** to keep track of the internal state for extra checking */
	TInternalActions iNextAction; // internal to me
	/** to inform the client of the result of the engines run */
	RMessage2 iClientMessage;
	TBool iPerformCommandListValidation;
	/** The severity of the current command*/
	TCmdErrorSeverity iSeverity;
	CSsmSwpPolicyCliSession* iSsmSwpPolicySession; // not owned by me
	CSsmCommandList* iCommandList;
	};

#endif
