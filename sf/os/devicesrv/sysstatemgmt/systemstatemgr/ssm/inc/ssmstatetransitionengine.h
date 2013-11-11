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

#ifndef __SSMSTATETRANSITIONENGINE_H__
#define __SSMSTATETRANSITIONENGINE_H__

#include <e32base.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmcmd.hrh>

class CSsmStatePolicyFrame;
class CSsmCommandList;	
class MCleSessionProxy;
class MSsmStatePolicyResolverProxy;
class CSsmStateTransitionRequest;

/** 
The CSsmStateTransitionEngine class is responsible for drivingin 
System State Changes by requesting answers and actions from System State 
Policy implememtations and the Command List Execution Server.

@internalComponent
@released
*/		
class CSsmStateTransitionEngine : public CActive
	{
public:
	static CSsmStateTransitionEngine* NewL(MSsmStatePolicyResolverProxy* aResolver, MCleSessionProxy* aCleSession);
	static CSsmStateTransitionEngine* NewLC(MSsmStatePolicyResolverProxy* aResolver, MCleSessionProxy* aCleSession);
	~CSsmStateTransitionEngine();
	
	TSsmStateTransition const* CurrentTransition() const;
	TSsmStateTransition const* QueuedTransition() const;
	
	MSsmStatePolicy::TResponse TransitionAllowed(const TSsmStateTransition& aRequest, const RMessagePtr2& aMessage);
	void SubmitL(const TSsmStateTransition& aRequest);
	void SubmitL(const TSsmStateTransition& aRequest, const RMessage2& aMessage);
	void Cancel(CSession2*);
	void PerformCommandListValidation(TBool aSetting);
#ifdef _DEBUG
	void CleanupTransitionEngine();
#endif
	
protected:
	//from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	CSsmStateTransitionEngine(MSsmStatePolicyResolverProxy* aResolver, MCleSessionProxy* aCleSession);
	void DoSubmit(CSsmStateTransitionRequest* aRequest);
	void Start();
	void ShiftQueueIfNeeded();
	CSsmStatePolicyFrame* Policy();
	void DoConnectCleSessionL();
	void DoStartTransitionL();
	void DoInitialize();
	void DoPrepareCommandList();
	void DoExecuteCommandList();
	void DoValidation();
	TBool FurtherTransition();
	TBool InTransition() const;
	
private:
	enum TInternalActions
		{
		EUnConnected,
		EIdle,
		EStartTransition,
		EInitialize,
		EPrepareCommandList,
		EExecuteCommandList,
		EReadNextTransition
		};

	MSsmStatePolicyResolverProxy& iResolver;
	MCleSessionProxy& iCleSession;
	TBool iPerformCommandListValidation;

	CSsmStateTransitionRequest* iQueuedTransition;
	CSsmStateTransitionRequest* iCurrentTransition;
 	
 	TSsmState iTargetSystemState;		 	
	TInternalActions iNextAction;
	/** The severity of the current command*/
	TCmdErrorSeverity iSeverity;
	CSsmCommandList* iCommandList;
	};

#endif
