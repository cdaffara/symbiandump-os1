// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ssm/ssmcommandlist.h>

#include "ssmdebug.h"
#include "ssmserverpanic.h"
#include "ssmstatetransitionengine.h"
#include "ssmstatepolicyresolverproxy.h"
#include "clesessionproxy.h"
#include "ssmstatetransitionrequest.h"
#include "ssmstatepolicyframe.h"
#include "ssmcommandlistutils.h"


CSsmStateTransitionEngine* CSsmStateTransitionEngine::NewL(MSsmStatePolicyResolverProxy* aResolver, MCleSessionProxy* aCleSession)
	{
	CSsmStateTransitionEngine* self = CSsmStateTransitionEngine::NewLC(aResolver, aCleSession);
	CleanupStack::Pop(self);
	return self;
	}

CSsmStateTransitionEngine* CSsmStateTransitionEngine::NewLC(MSsmStatePolicyResolverProxy* aResolver, MCleSessionProxy* aCleSession)
	{
	__ASSERT_DEBUG(aResolver,	PanicNow(KPanicSysStateMgr,ESsmStateEngineError5));
	__ASSERT_DEBUG(aCleSession,	PanicNow(KPanicSysStateMgr,ESsmStateEngineError2));
	CSsmStateTransitionEngine* self = new (ELeave) CSsmStateTransitionEngine(aResolver, aCleSession);
	CleanupStack::PushL(self);
	return self;
	}

CSsmStateTransitionEngine::CSsmStateTransitionEngine(MSsmStatePolicyResolverProxy* aResolver, MCleSessionProxy* aCleSession)
	: CActive(CActive::EPriorityStandard),
	iResolver(*aResolver),
	iCleSession(*aCleSession),
	iPerformCommandListValidation(ETrue),
	iNextAction(EUnConnected)
	{
	CActiveScheduler::Add(this);
	}

CSsmStateTransitionEngine::~CSsmStateTransitionEngine()
	{
	CActive::Cancel();

	iCleSession.ReleaseCle();
	iResolver.ReleasePolicyResolver();

	delete iQueuedTransition;
	delete iCurrentTransition;
    delete iCommandList;
	}

TSsmStateTransition const* CSsmStateTransitionEngine::CurrentTransition() const
	{
	return iCurrentTransition ? &(iCurrentTransition->StateTransition()) : NULL;
	}

TSsmStateTransition const* CSsmStateTransitionEngine::QueuedTransition() const
	{
	return iQueuedTransition ? &(iQueuedTransition->StateTransition()) : NULL;
	}

MSsmStatePolicy::TResponse CSsmStateTransitionEngine::TransitionAllowed(const TSsmStateTransition& aRequest, const RMessagePtr2& aMessage)
	{
	return Policy()->CallTransitionAllowed(aRequest, CurrentTransition(), QueuedTransition(), aMessage);
	}

void CSsmStateTransitionEngine::SubmitL(const TSsmStateTransition& aRequest)
	{
	CSsmStateTransitionRequest* request = new (ELeave) CSsmStateTransitionRequest(aRequest);
	DoSubmit(request);
	}

void CSsmStateTransitionEngine::SubmitL(const TSsmStateTransition& aRequest, const RMessage2& aMessage)
	{
	CSsmStateTransitionRequest* request = new (ELeave) CSsmStateTransitionRequest(aRequest, aMessage);
	DoSubmit(request);
	}
/**
If the current slot is free, aRequest is assigned to the current-slot (the queue-slot should be free).
If the current slot is occupied, aRequest is assigned to the queue-slot. Any already existing
queued request is lost.

(You should adhere to the TResponse given by the policy's TransitionAllowed before calling this method)
*/
void CSsmStateTransitionEngine::DoSubmit(CSsmStateTransitionRequest* aRequest)
	{
	if(iCurrentTransition)
		{
		//ECurrentRemainReplaceQueued
		if(iQueuedTransition)
			{
			iQueuedTransition->Complete(KErrCancel);
			delete iQueuedTransition;
			iQueuedTransition = NULL;
			}
		iQueuedTransition = aRequest;
		}
	else
		{
		//EReplaceCurrentClearQueue
		__ASSERT_DEBUG(NULL == iQueuedTransition, PanicNow(KPanicSysStateMgr,ESsmStateEngineError10)); // should be already be deleted by DoCancel
		if(iQueuedTransition)
			{
			iQueuedTransition->Complete(KErrCancel);
			delete iQueuedTransition;
			iQueuedTransition = NULL;
			}
		iCurrentTransition = aRequest;
		}

	if(!IsActive())
		{
		if (EUnConnected == iNextAction)
			{
			Start();
			}
		else if (EIdle == iNextAction)	
			{
			iNextAction = iCleSession.IsConnected() ? EStartTransition : EUnConnected;
			Start();
			}
		}
	}

void CSsmStateTransitionEngine::Start()
	{
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CSsmStateTransitionEngine::Cancel(CSession2* aSession)
	{
	if(aSession)
		{
		//Client side request to cancel
		if(iQueuedTransition && iQueuedTransition->OriginatesFrom(aSession))
			{
			iQueuedTransition->Complete(KErrCancel);
			delete iQueuedTransition;
			iQueuedTransition = NULL;
			}

		if(iCurrentTransition)
			{
			if(iCurrentTransition->OriginatesFrom(aSession) && !InTransition())
				{
				CActive::Cancel();
				ShiftQueueIfNeeded();
				if(iCurrentTransition)
					{
					iNextAction = EStartTransition;
					Start();
					}
				}
			}
		}
	else
		{
		//Server-side request, raised when the policy allows a new request with EReplaceCurrentClearQueue
		if(iQueuedTransition)
			{
			iQueuedTransition->Complete(KErrCancel);
			delete iQueuedTransition;
			iQueuedTransition = NULL;
			}

		if(iCurrentTransition)
			{
			CActive::Cancel();
			ShiftQueueIfNeeded();
			}
		}
	}

void CSsmStateTransitionEngine::DoCancel()
	{
	//Cancel any pending request this engine has submitted to external providers
	CSsmStatePolicyFrame* policy = iResolver.Policy();
	if(policy &&(iNextAction == EPrepareCommandList))
		{
		policy->CallInitializeCancel();
		}
	else if(policy &&(iNextAction == EExecuteCommandList))
		{
		policy->CallPrepareCommandListCancel();
		}
	else if(iNextAction == EReadNextTransition)
		{
		iCleSession.ExecuteCommandListCancel();
		//iCommandList needs to be deleted when cancel has been called before completion of command 
		if (iCommandList)
            {
            delete iCommandList;
            iCommandList = NULL;
            }
		}

	//Then Cancel the pending operations this engine has been asked to do
	if(iCurrentTransition)
		{
		iCurrentTransition->Complete(KErrCancel);
		delete iCurrentTransition;
		iCurrentTransition = NULL;
		}

	//Reset internal state
	if(iNextAction > EUnConnected)
		{
		iNextAction = EIdle;
		}
	}

void CSsmStateTransitionEngine::RunL()
	{
	if(iNextAction != EReadNextTransition)
		{
		//Propagate all problems to RunError except the return value from CleSrv
		SSMLOGLEAVEIFERROR(iStatus.Int()); //will leave in release builds as well
		}

 	switch(iNextAction)
	 	{
		case EUnConnected:
			DoConnectCleSessionL();
			iNextAction = EStartTransition;
			break;

	 	case EStartTransition:
	 		DoStartTransitionL();
	 		iNextAction = EInitialize;
	 		break;

	 	case EInitialize:
	 		DoInitialize();
	 		iNextAction = EPrepareCommandList;
	 		break;

	 	case EPrepareCommandList:
	 		DoPrepareCommandList();
	 		iNextAction = EExecuteCommandList;
	 		break;

	 	case EExecuteCommandList:
	 		DoExecuteCommandList();
			iNextAction = EReadNextTransition;
	 		break;

		case EReadNextTransition:
			{
			if (iCommandList)
				{
				delete iCommandList;
				iCommandList = NULL;
				}
			TBool furtherTransition = FurtherTransition();
			iNextAction = iCleSession.IsConnected() ? EStartTransition : EUnConnected;
			if(!furtherTransition && !iCurrentTransition)
				{
				iNextAction = EIdle;	// If no further transition and nothing was in queue --> set to idle
				}
			}
			break;

		case  EIdle:
		default:
			__ASSERT_DEBUG(EFalse, PanicNow(KPanicSysStateMgr,ESsmStateEngineError11));
			break;
		}
	}

/**
 Most errors cause a Panic followed by Kernel fault. The rationale behind this is that all 
 the state policies placed on the rom by the device manufacturer must be error free when
 the device is shipped. There is no case for graceful handling of missing commandlists,
 bad resource files or an unknown state.

 The only error which does not cause a panic is the return value from CleSrv. When a command
 execution results in an error, CleSrv reports the error-code back to CSsmStateTransitionEngine
 which forwards the error-code to the State Policy DLL in the call MSsmStatePolicy::GetNextState
 */
TInt CSsmStateTransitionEngine::RunError(TInt aError)
	{
	DEBUGPRINT3(_L("CSsmStateTransitionEngine RunError: %d occured in operation: %d"), aError, iNextAction);

	TSsmStateName name = iTargetSystemState.Name();
 	switch(iNextAction)
	 	{
	 	case EUnConnected:
			{
			iCurrentTransition->Complete(aError);
			DEBUGPRINT3(_L("State transition to %S failed, could not connect to CleSrv, reason: %d"), &name, aError);
			PanicNow(KPanicSysStateMgr,ESsmStateEngineError3);
			break;
			}
	 	case EStartTransition:
			{
			DEBUGPRINT3(_L("State transition to %S failed, possibly due to policy file err: %d"), &name, aError);
			PanicNow(KPanicSysStateMgr,ESsmStateEngineError7);
	 		break;
			}
	 	case EInitialize:
	 	case EPrepareCommandList:
	 		{
	 		DEBUGPRINT3(_L("State Policy DLL %S failed to initialize, leavecode: %d"), &name, aError);
	 		PanicNow(KPanicSysStateMgr,ESsmStateEngineError6);
	 		break;
	 		}
	 	case EExecuteCommandList:
	 		{
	 		DEBUGPRINT3(_L("Command list execution failed, target state %S, error: %d"), &name, aError);
	 		PanicNow(KPanicSysStateMgr,ESsmStateEngineError8);
	 		break;
	 		}
		case EReadNextTransition:
			{
			//This error will not be raised from the usual User::LeaveIfError at the top of RunL, but from a bad
			//virtual implementation of GetNextState (which leaves despite this should be a non-leaving function).
			DEBUGPRINT3(_L("State Policy error: %d occured during a call to GetNextState %S"), aError, &name);
	 		PanicNow(KPanicSysStateMgr,ESsmStateEngineError4);
	 		break;
			}
		default:
		case EIdle:
			__ASSERT_DEBUG(EFalse,	PanicNow(KPanicSysStateMgr,ESsmStateEngineError15));
			break;
		}

 	return KErrNone;
	} //lint !e529 not subsequently referenced - dependent on debug

/**
 Before this transition engine is used for the first time, we need to connect
 to (and probably start) the CleSrv.
 The session remains connected because we do not want to risk not being able to
 connect to CleSrv (due to low memory in kernel) if we get a request to transition
 to state ESsmFail.
 */
void CSsmStateTransitionEngine::DoConnectCleSessionL()
	{
	iCleSession.ConnectL();
	Start();
	}

void CSsmStateTransitionEngine::DoStartTransitionL()
	{
	//Let the client know this transition has started, from now on this transition can't be cancelled
	//  note: in case this is called as part of a FurtherTransition(), complete will do nothing
	iCurrentTransition->Complete(KErrNone);

#ifdef _DEBUG
	TSsmStateName name = iCurrentTransition->State().Name();
	DEBUGPRINT2(_L("Initiating transition to state %S."), &name);
#endif

	//Start off by loading the correct state policy DLL
	iTargetSystemState = iCurrentTransition->State();
	iResolver.GetStatePolicyL(iTargetSystemState);
	Start();
	}

void CSsmStateTransitionEngine::DoInitialize()
	{
	if(!Policy()->Initialized())
		{
		//Let the state policy do whatever it needs to do for example initialize its CSsmCommandListResourceReader
		Policy()->CallInitialize(iStatus);
		SetActive();
		}
	else
		{
		Start();
		}
	}

void CSsmStateTransitionEngine::DoPrepareCommandList()
	{
	__ASSERT_DEBUG(iCurrentTransition, PanicNow(KPanicSysStateMgr,ESsmStateEngineError13));
	Policy()->CallPrepareCommandList(iTargetSystemState, iCurrentTransition->Reason(), iStatus);
	SetActive();
	}

void CSsmStateTransitionEngine::DoExecuteCommandList()
	{
	iCommandList = Policy()->CallCommandList();
	if(iPerformCommandListValidation)
		{
		DoValidation();
		}

	if(iCommandList)
		{
		iCleSession.ExecuteCommandList(*iCommandList, iStatus, iSeverity);
		SetActive();
		}
	else
		{
		Start();
		}
	}

void CSsmStateTransitionEngine::DoValidation()
	{
	TBool valid = EFalse;
	if(!iCommandList)
		{
		DEBUGPRINT1(_L("State Policy DLL returned NULL from CommandList()"));
		}
	else
		{
		valid = CSsmCommandListUtils::IsValidStateList(*iCommandList);
		if(!valid)
			{
			DEBUGPRINT1(_L("State Policy DLL returned an invalid CommandList()"));
			}
		}
	__ASSERT_ALWAYS( iCommandList && valid, PanicNow(KPanicSysStateMgr,ESsmStateEngineError9));
	}

TBool CSsmStateTransitionEngine::FurtherTransition()
	{
	__ASSERT_DEBUG(iCurrentTransition, PanicNow(KPanicSysStateMgr,ESsmStateEngineError14));
	const TInt error = iStatus.Int();

	// In case the server is no longer present
	if(error == KErrServerTerminated)
		{
		// Closes the cle handle and marks it as disconnected
		iCleSession.Close();
		}
	// Severity must be obtained from CLE somehow. This needs proper implementation.
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	if(KErrNone != error)
		{
		DEBUGPRINT3(_L("Commandlist execution reported error: %d, with severity %d."), error, severity);
		severity = ECmdCriticalSeverity;
		}

	TSsmState nextSystemState;
	const TBool moreTransitions = Policy()->CallGetNextState(iTargetSystemState,
															 iCurrentTransition->Reason(),
															 error,
															 severity,
															 nextSystemState);
	if(!moreTransitions)
		{
		// Request completed
#ifdef _DEBUG
		TSsmStateName name = iCurrentTransition->State().Name();
		DEBUGPRINT2(_L("Transition to state %S completed."), &name);
#endif
		delete iCurrentTransition;
		iCurrentTransition = NULL;
		// See if there is another request queued
		ShiftQueueIfNeeded();
		}
	else
		{
		// FurtherTransition requested
#ifdef _DEBUG
		TSsmStateName name = nextSystemState.Name();
		DEBUGPRINT2(_L("FurtherTransition requested to state %S."), &name);
#endif
		// Update the target system state (Note: We only update the SystemState, same reason is being re-used)
		const TSsmStateTransition request(nextSystemState, iCurrentTransition->Reason());
		iCurrentTransition->SetStateTransition(request);
		}

	if(moreTransitions || iCurrentTransition)
		{
		Start();
		}

	return moreTransitions;
	}

TBool CSsmStateTransitionEngine::InTransition() const
	{
	return (iNextAction > EStartTransition);
	}

void CSsmStateTransitionEngine::ShiftQueueIfNeeded()
	{
	if(iQueuedTransition && !iCurrentTransition)
		{
		iCurrentTransition = iQueuedTransition;
		iQueuedTransition = NULL;
		}
	}

CSsmStatePolicyFrame* CSsmStateTransitionEngine::Policy()
	{
	CSsmStatePolicyFrame* policy = iResolver.Policy();
	__ASSERT_DEBUG( policy, PanicNow(KPanicSysStateMgr,ESsmStateEngineError1));
	return policy;
	}

void CSsmStateTransitionEngine::PerformCommandListValidation(TBool aSetting)
	{
	iPerformCommandListValidation = aSetting;
#ifdef _DEBUG
	if(iPerformCommandListValidation)
		{
		DEBUGPRINT1(_L("Turning on State command list validation"));
		}
	else
		{
		DEBUGPRINT1(_L("Turning off State command list validation"));
		}
#endif
	}

/**
 * Only for test purposes
 * Cleanup the transition engine
 */
 #ifdef _DEBUG
void CSsmStateTransitionEngine::CleanupTransitionEngine()
	{
	iResolver.ReleasePolicyResolver();
	delete iQueuedTransition;
	iQueuedTransition = NULL;
	delete iCurrentTransition;
	iCurrentTransition = NULL;	
	iCleSession.ReleaseCle();
	}
#endif
