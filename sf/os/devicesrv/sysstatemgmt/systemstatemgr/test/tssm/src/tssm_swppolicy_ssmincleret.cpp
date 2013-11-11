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


#include <ssm/ssmcommandfactory.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmsubstates.hrh>

#include <e32debug.h>
#include <e32property.h>

#include "tssm_ssmclient.h"
#include "tssm_statepolicy_fail.h"
#include "tssm_statepolicy_generic.h"
#include "tssm_swppolicy_ssmincleret.h"
#include "tssm_statepolicy_generic.hrh"
/********************* utility class ******************/

CRequestRunInThread::CRequestRunInThread(TRequestStatus& aClientStatus) : CActive(EPriorityNormal)
	{
	CActiveScheduler::Add(this);

	aClientStatus = KRequestPending;
	iClientStatus = &aClientStatus;
	}
	
CRequestRunInThread::CRequestRunInThread() : CActive(EPriorityNormal)
	{
	CActiveScheduler::Add(this);
	iClientStatus = NULL;
	}

CRequestRunInThread::~CRequestRunInThread()
	{
	Cancel();
	iClient.Close();
	}
	
void CRequestRunInThread::RunL()
	{
	TInt err = iStatus.Int();
	if (iClientStatus)
		User::RequestComplete(iClientStatus, err);
	iClient.Close();
	RDebug::Printf("--- CSsmSwpPolicySsmInCleRet --> CRequestRunInThread request completed in RunL() with code=%d.", err);
	}

void CRequestRunInThread::StartL(const TSsmStateTransition& aState)
	{
	// Connect to the server...
	RDebug::Printf ("--- CSsmSwpPolicySsmInCleRet --> connecting to the server...");
	User::LeaveIfError(iClient.Connect());

	// Request state transitions in the session
	iClient.RequestStateTransition(aState, iStatus);
	SetActive();
	}

void CRequestRunInThread::DoCancel()
	{
	iClient.RequestStateTransitionCancel();
	}

/******************** ssmincleret class ***************/

EXPORT_C MSsmSwpPolicy* CSsmSwpPolicySsmInCleRet::NewL()
	{
	CSsmSwpPolicySsmInCleRet* self = new (ELeave) CSsmSwpPolicySsmInCleRet;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmSwpPolicySsmInCleRet::CSsmSwpPolicySsmInCleRet()
	{
	}

CSsmSwpPolicySsmInCleRet::~CSsmSwpPolicySsmInCleRet()
	{
	delete iCommandList;
	}

void CSsmSwpPolicySsmInCleRet::ConstructL()
	{
	}

void CSsmSwpPolicySsmInCleRet::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicySsmInCleRet::InitializeCancel()
	{
	}

void CSsmSwpPolicySsmInCleRet::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CSsmSwpPolicySsmInCleRet::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aMessage;
	
	// This swp is the one allowed to test for state transition request in the HandleCleReturnValue method
	if (aSwp.Key() == KTestSwpHandleCleRetStateReq)
		{
		return EAllowed;
		}
	
	return ENotAllowed; 
	}	

void CSsmSwpPolicySsmInCleRet::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	RDebug::Printf ("--- CSsmSwpPolicySsmInCleRet::PrepareCommandList for swp with key %d", aSwp.Key());
	TRAPD(err, doPrepareCommandListL(aSwp));
	RDebug::Printf ("--- CSsmSwpPolicySsmInCleRet::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	}

void CSsmSwpPolicySsmInCleRet::doPrepareCommandListL(TSsmSwp aSwp)
	{
	// reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();

	// Use the value to create a simple valid swp cmd list with just a publish swp command. 
	MSsmCommand* cmd = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, aSwp);
	iCommandList->AppendL(cmd);
	CleanupStack::Pop(cmd);
	}

void CSsmSwpPolicySsmInCleRet::PrepareCommandListCancel()
	{
	delete iCommandList;
	iCommandList = NULL;
	}

CSsmCommandList* CSsmSwpPolicySsmInCleRet::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

void CSsmSwpPolicySsmInCleRet::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSeverity;

	RDebug::Printf ("--- CSsmSwpPolicySsmInCleRet::HandleCleReturnValue gets aReturnValue '%d'", aError);

	// We've executed a valid command list --> we're expecting a KErrNone
	__ASSERT_ALWAYS(aError == KErrNone, User::Panic(KSwpSsmInCleRetTestPanicCategory, ESwpSsmInCleRetTestFailed2));

	TInt err = KErrNone;
	if (aSwp.Value() == ERequestStateTranSync)
		{
		TRAP(err,
			{
			CRequestRunInThread* request = new (ELeave) CRequestRunInThread(aStatus);
			TSsmStateTransition state(KSsmTestGenericState, EStateDoNothing, 0);	// Substate & reason not important
			// We are waiting for the state transition to complete
			request->StartL(state);
			});
		}
	else if(aSwp.Value() == ERequestStateTranAsync)
		{
		TRAP(err,
			{
			CRequestRunInThread* request = new (ELeave) CRequestRunInThread();
			TSsmStateTransition state(ESsmTestFailState,123,456);	// Substate & reason not important
			// We are not waiting for the state transition to complete
			request->StartL(state);
			TRequestStatus* rs = &aStatus;
			User::RequestComplete(rs, err);
			});
		}

	// We've executed a valid command list --> we're expecting a KErrNone
	__ASSERT_ALWAYS(err == KErrNone, User::Panic(KSwpSsmInCleRetTestPanicCategory, ESwpSsmInCleRetTestFailed1));

	RDebug::Printf ("--- CSsmSwpPolicySsmInCleRet::HandleCleReturnValue completed!");

	return;
	}

void CSsmSwpPolicySsmInCleRet::HandleCleReturnValueCancel()
	{
	}
