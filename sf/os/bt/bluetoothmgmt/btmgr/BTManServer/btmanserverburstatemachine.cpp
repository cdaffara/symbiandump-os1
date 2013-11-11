// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the state machine which drives backup and restore support for the 
// Bluetooth Manager server.
//

#include <e32std.h>
#include <e32base.h>
#include "btmanserverburmgr.h"

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_MANAGER_SERVER);
#endif

/**
CBTManServerBURMgrStateFactory - Manages the state machine that drives CBTManServerBURMgr
**/

CBTManServerBURMgrStateFactory* CBTManServerBURMgrStateFactory::NewL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_STATIC_FUNC

	CBTManServerBURMgrStateFactory* result = new (ELeave) CBTManServerBURMgrStateFactory(aBURMgr);
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(result);
	return result;
	}

CBTManServerBURMgrStateFactory::CBTManServerBURMgrStateFactory(CBTManServerBURMgr& aBURMgr) 
	: iBURMgr(aBURMgr), 
	iState(EBTBURStateNormal)
	{
	LOG_FUNC
	}

CBTManServerBURMgrStateFactory::~CBTManServerBURMgrStateFactory()
	{
	LOG_FUNC

	// Destroy state machine
	iStateActions.ResetAndDestroy();
	}

void CBTManServerBURMgrStateFactory::ConstructL()
	{
	LOG_FUNC

	ConstructStateMachineL();
	}

void CBTManServerBURMgrStateFactory::ConstructStateMachineL()
	{
	LOG_FUNC

	// Construct the state instances - each state knows how to carry out its action and its possible transitions
	// Note that the ordering here is critical - it must reflect the ordering of the enums in TBTBURState
	for (TInt i = EBTBURStateNormal; i < EBTBURStateUnknown; i++)
		{
		CBTManServerBURMgrStateAction* nextState = ConstructStateLC(static_cast<TBTBURState>(i));
		iStateActions.AppendL(nextState);
		CleanupStack::Pop(nextState);
		}
	}

CBTManServerBURMgrStateAction* CBTManServerBURMgrStateFactory::ConstructStateLC(TBTBURState aState)
	{
	LOG_FUNC

	CBTManServerBURMgrStateAction* result = NULL;

	switch (aState)
		{
	case EBTBURStateNormal:
		result = CBTManServerBURMgrStateNormal::NewLC();
		break;
	case EBTBURStateBackupRequest:
		result = CBTManServerBURMgrStateBackupRequest::NewLC();
		break;
	case EBTBURStateBackupOngoing:
		result = CBTManServerBURMgrStateBackupOngoing::NewLC();
		break;
	case EBTBURStateBackupIgnore:
		result = CBTManServerBURMgrStateBackupIgnore::NewLC();
		break;
	case EBTBURStateRestoreRequest:
		result = CBTManServerBURMgrStateRestoreRequest::NewLC();
		break;
	case EBTBURStateRestoreOngoing:
		result = CBTManServerBURMgrStateRestoreOngoing::NewLC();
		break;
	case EBTBURStateRestoreIgnore:
		result = CBTManServerBURMgrStateRestoreIgnore::NewLC();
		break;
	case EBTBURStateProcessRestoreFile:
		result = CBTManServerBURMgrStateProcessRestoreFile::NewLC();
		break;
	case EBTBURStateRestoreFileProcessingComplete:
		result = CBTManServerBURMgrStateRestoreFileProcessingComplete::NewLC();
		break;
	default:
		// EBTBURStateUnknown
		__ASSERT_DEBUG(EFalse, PANIC(KBTBackupPanicCat, EBTBURMgrInvalidStateIndex));
		User::Leave(KErrNotFound);
		}

	return result;
	}

/**
Attempts to transition to a new state in the state machine, in response to a given event.
This only succeeds if the event yields a valid transition from the current state.
The transition event EBTBUREventAbortStateAction yields a valid transition for any state, 
and this is tested for here.
Upon successful transition, the action associated with the new state is executed.
An invalid state transition in debug mode will result in a panic. In release mode, the current
state is maintained.
@param aTransitionEvent The event driving the state transition.
@return	KErrNone if state transition is successful.
	KErrNotFound if aTransitionEvent does not yield a valid transition for the current state.
**/
TInt CBTManServerBURMgrStateFactory::TransitionState(TBTBUREvent aTransitionEvent)
	{
	LOG_FUNC

	TBTBURState nextState;

	// We allow EBTBUREventAbortStateAction to transition any state back to normal (used for error handling)
	if (aTransitionEvent == EBTBUREventAbortStateAction)
		{
		nextState = EBTBURStateNormal;
		}
	else
		{
		nextState = iStateActions[iState]->GetNextState(aTransitionEvent);
		}
	
	// A request to process a restore file may come in at any time, and we may not be in a correct state to deal with it.
	// This scenario is not an error, instead the request is simply ignored (as state transition is disallowed) and will be picked up
	// when BTManServer next starts.
	__ASSERT_DEBUG(nextState != EBTBURStateUnknown || aTransitionEvent == EBTBUREventProcessRestoreFile, InvalidStateTransitionPanic(iState, aTransitionEvent));

	if (nextState == EBTBURStateUnknown)
		{
		// Invalid transition
		return KErrNotFound;
		}
	else
		{
		// Make the transition and execute new state action.
		iState = nextState;
		ExecuteAction();

		return KErrNone;
		}
	}

/**
Executes the action associated with the current state.
The implementation of the action is given by CBTManServerBURMgrStateAction::ExecuteActionL().
Error handling is provided by CBTManServerBURMgrStateAction::HandleActionError().
**/
void CBTManServerBURMgrStateFactory::ExecuteAction()
	{
	LOG_FUNC

	TRAPD(err, iStateActions[iState]->ExecuteActionL(iBURMgr));
	if (err != KErrNone)
		{
		iStateActions[iState]->HandleActionError(iBURMgr, err);
		}
	}

/**
CBTManServerBURMgrStateAction - Base class for action/transition handling for states of CBTManServerBURMgr
**/

CBTManServerBURMgrStateAction::CBTManServerBURMgrStateAction(TBTBURState aState) 
	: iState(aState)
	{
	LOG_FUNC
	}

CBTManServerBURMgrStateAction::~CBTManServerBURMgrStateAction()
	{
	LOG_FUNC
	}

/**
Handles a leave generated by ExecuteActionL().
The default implementation is to do nothing. This accomodates states whose implementation of ExecuteActionL()
is guaranteed not to leave.
If a leave does occur, the state must either recover from the error in some way (allowing the state machine to 
remain in its current state) or initiate a transition back to the normal state using the EBTBUREventAbortStateAction
transition event (after any necessary cleanup has been performed).
@param aBURMgr The CBTManServerBURMgr instance
@param aError The error which generated the leave in ExecuteActionL()
**/
void CBTManServerBURMgrStateAction::HandleActionError(CBTManServerBURMgr& /*aBURMgr*/, TInt /*aError*/)
	{
	LOG_FUNC

	// Do nothing by default (state may not have a leavable action)
	}

/**
CBTManServerBURMgrStateNormal - Action/transition handling for EBTBURStateNormal state of CBTManServerBURMgr
**/

CBTManServerBURMgrStateNormal* CBTManServerBURMgrStateNormal::NewLC()
	{
	LOG_STATIC_FUNC

	CBTManServerBURMgrStateNormal* result = new(ELeave) CBTManServerBURMgrStateNormal();
	CleanupStack::PushL(result);
	return result;
	}

CBTManServerBURMgrStateNormal::CBTManServerBURMgrStateNormal() 
	: CBTManServerBURMgrStateAction(EBTBURStateNormal)
	{
	LOG_FUNC
	}		

CBTManServerBURMgrStateNormal::~CBTManServerBURMgrStateNormal()
	{
	LOG_FUNC
	}

TBTBURState CBTManServerBURMgrStateNormal::GetNextState(TBTBUREvent aTransitionEvent)
	{
	LOG_FUNC

	// Valid transition events: EBTBUREventBackup, EBTBUREventRestore, EBTBUREventProcessRestoreFile, EBTBUREventUnset, EBTBUREventNormal
	switch (aTransitionEvent)
		{
	case EBTBUREventBackup:
		return EBTBURStateBackupRequest;
	case EBTBUREventRestore:
		return EBTBURStateRestoreRequest;
	case EBTBUREventProcessRestoreFile:
		return EBTBURStateProcessRestoreFile;
	case EBTBUREventUnset:
		// Fall-through
	case EBTBUREventNormal:
		return EBTBURStateNormal;
	default:
		// Unknown state (no valid transition)
		return EBTBURStateUnknown;
		}
	}

void CBTManServerBURMgrStateNormal::ExecuteActionL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_FUNC

	aBURMgr.HandleStateNormal();
	}

/**
CBTManServerBURMgrStateBackupRequest - Action/transition handling for EBTBURStateBackupRequest state of CBTManServerBURMgr
**/

CBTManServerBURMgrStateBackupRequest* CBTManServerBURMgrStateBackupRequest::NewLC()
	{
	LOG_STATIC_FUNC

	CBTManServerBURMgrStateBackupRequest* result = new(ELeave) CBTManServerBURMgrStateBackupRequest();
	CleanupStack::PushL(result);
	return result;
	}

CBTManServerBURMgrStateBackupRequest::CBTManServerBURMgrStateBackupRequest() 
	: CBTManServerBURMgrStateAction(EBTBURStateBackupRequest)
	{
	LOG_FUNC
	}		

CBTManServerBURMgrStateBackupRequest::~CBTManServerBURMgrStateBackupRequest()
	{
	LOG_FUNC
	}

TBTBURState CBTManServerBURMgrStateBackupRequest::GetNextState(TBTBUREvent aTransitionEvent)
	{
	LOG_FUNC
	
	// Valid transition events: EBTBUREventBackupProceed, EBTBUREventBackupReject
	switch (aTransitionEvent)
		{
	case EBTBUREventBackupProceed:
		return EBTBURStateBackupOngoing;
	case EBTBUREventBackupReject:
		return EBTBURStateBackupIgnore;
	default:
		// Unknown state (no valid transition)
		return EBTBURStateUnknown;
		}
	}

void CBTManServerBURMgrStateBackupRequest::ExecuteActionL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_FUNC

	aBURMgr.HandleStateBackupRequestL();
	}

void CBTManServerBURMgrStateBackupRequest::HandleActionError(CBTManServerBURMgr& aBURMgr, TInt /*aError*/)
	{
	LOG_FUNC

	aBURMgr.HandleStateBackupRequestError();
	}

/**
CBTManServerBURMgrStateBackupOngoing - Action/transition handling for EBTBURStateBackupOngoing state of CBTManServerBURMgr
**/

CBTManServerBURMgrStateBackupOngoing* CBTManServerBURMgrStateBackupOngoing::NewLC()
	{
	LOG_STATIC_FUNC

	CBTManServerBURMgrStateBackupOngoing* result = new(ELeave) CBTManServerBURMgrStateBackupOngoing();
	CleanupStack::PushL(result);
	return result;
	}

CBTManServerBURMgrStateBackupOngoing::CBTManServerBURMgrStateBackupOngoing() 
	: CBTManServerBURMgrStateAction(EBTBURStateBackupOngoing)
	{
	LOG_FUNC
	}		

CBTManServerBURMgrStateBackupOngoing::~CBTManServerBURMgrStateBackupOngoing()
	{
	LOG_FUNC
	}

TBTBURState CBTManServerBURMgrStateBackupOngoing::GetNextState(TBTBUREvent aTransitionEvent)
	{
	LOG_FUNC
	
	// Valid transition events: EBTBUREventNormal
	if (aTransitionEvent == EBTBUREventNormal)
		{
		return EBTBURStateNormal;
		}
	else
		{
		// Unknown state (no valid transition)
		return EBTBURStateUnknown;
		}
	}

void CBTManServerBURMgrStateBackupOngoing::ExecuteActionL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_FUNC

	aBURMgr.HandleStateBackupOngoingL();
	}

void CBTManServerBURMgrStateBackupOngoing::HandleActionError(CBTManServerBURMgr& aBURMgr, TInt /*aError*/)
	{
	LOG_FUNC

	aBURMgr.HandleStateBackupOngoingError();
	}	

/**
CBTManServerBURMgrStateBackupIgnore - Action/transition handling for EBTBURStateBackupIgnore state of CBTManServerBURMgr
**/

CBTManServerBURMgrStateBackupIgnore* CBTManServerBURMgrStateBackupIgnore::NewLC()
	{
	LOG_STATIC_FUNC

	CBTManServerBURMgrStateBackupIgnore* result = new(ELeave) CBTManServerBURMgrStateBackupIgnore();
	CleanupStack::PushL(result);
	return result;
	}

CBTManServerBURMgrStateBackupIgnore::CBTManServerBURMgrStateBackupIgnore() 
	: CBTManServerBURMgrStateAction(EBTBURStateBackupIgnore)
	{
	LOG_FUNC
	}		

CBTManServerBURMgrStateBackupIgnore::~CBTManServerBURMgrStateBackupIgnore()
	{
	LOG_FUNC
	}

TBTBURState CBTManServerBURMgrStateBackupIgnore::GetNextState(TBTBUREvent aTransitionEvent)
	{
	LOG_FUNC
	
	// Valid transition events: EBTBUREventNormal
	if (aTransitionEvent == EBTBUREventNormal)
		{
		return EBTBURStateNormal;
		}
	else
		{
		// Unknown state (no valid transition)
		return EBTBURStateUnknown;
		}
	}

void CBTManServerBURMgrStateBackupIgnore::ExecuteActionL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_FUNC

	aBURMgr.HandleStateBackupIgnore();
	}

/**
CBTManServerBURMgrStateRestoreRequest - Action/transition handling for EBTBURStateRestoreRequest state of CBTManServerBURMgr
**/

CBTManServerBURMgrStateRestoreRequest* CBTManServerBURMgrStateRestoreRequest::NewLC()
	{
	LOG_STATIC_FUNC

	CBTManServerBURMgrStateRestoreRequest* result = new(ELeave) CBTManServerBURMgrStateRestoreRequest();
	CleanupStack::PushL(result);
	return result;
	}

CBTManServerBURMgrStateRestoreRequest::CBTManServerBURMgrStateRestoreRequest() 
	: CBTManServerBURMgrStateAction(EBTBURStateRestoreRequest)
	{
	LOG_FUNC
	}		

CBTManServerBURMgrStateRestoreRequest::~CBTManServerBURMgrStateRestoreRequest()
	{
	LOG_FUNC
	}

TBTBURState CBTManServerBURMgrStateRestoreRequest::GetNextState(TBTBUREvent aTransitionEvent)
	{
	LOG_FUNC
	
	// Valid transition events: EBTBUREventRestoreProceed, EBTBUREventRestoreReject
	switch (aTransitionEvent)
		{
	case EBTBUREventRestoreProceed:
		return EBTBURStateRestoreOngoing;
	case EBTBUREventRestoreReject:
		return EBTBURStateRestoreIgnore;
	default:
		// Unknown state (no valid transition)
		return EBTBURStateUnknown;
		}
	}

void CBTManServerBURMgrStateRestoreRequest::ExecuteActionL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_FUNC

	aBURMgr.HandleStateRestoreRequestL();
	}

void CBTManServerBURMgrStateRestoreRequest::HandleActionError(CBTManServerBURMgr& aBURMgr, TInt /*aError*/)
	{
	LOG_FUNC

	aBURMgr.HandleStateRestoreRequestError();
	}

/**
CBTManServerBURMgrStateRestoreOngoing - Action/transition handling for EBTBURStateRestoreOngoing state of CBTManServerBURMgr
**/

CBTManServerBURMgrStateRestoreOngoing* CBTManServerBURMgrStateRestoreOngoing::NewLC()
	{
	LOG_STATIC_FUNC

	CBTManServerBURMgrStateRestoreOngoing* result = new(ELeave) CBTManServerBURMgrStateRestoreOngoing();
	CleanupStack::PushL(result);
	return result;
	}

CBTManServerBURMgrStateRestoreOngoing::CBTManServerBURMgrStateRestoreOngoing() 
	: CBTManServerBURMgrStateAction(EBTBURStateRestoreOngoing)
	{
	LOG_FUNC
	}		

CBTManServerBURMgrStateRestoreOngoing::~CBTManServerBURMgrStateRestoreOngoing()
	{
	LOG_FUNC
	}

TBTBURState CBTManServerBURMgrStateRestoreOngoing::GetNextState(TBTBUREvent aTransitionEvent)
	{
	LOG_FUNC

	// Valid transition events: EBTBUREventNormal
	if (aTransitionEvent == EBTBUREventNormal)
		{
		return EBTBURStateNormal;
		}
	else
		{
		// Unknown state (no valid transition)
		return EBTBURStateUnknown;
		}
	}

void CBTManServerBURMgrStateRestoreOngoing::ExecuteActionL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_FUNC

	aBURMgr.HandleStateRestoreOngoingL();
	}

void CBTManServerBURMgrStateRestoreOngoing::HandleActionError(CBTManServerBURMgr& aBURMgr, TInt /*aError*/)
	{
	LOG_FUNC

	aBURMgr.HandleStateRestoreOngoingError();
	}

/**
CBTManServerBURMgrStateRestoreIgnore - Action/transition handling for EBTBURStateRestoreIgnore state of CBTManServerBURMgr
**/

CBTManServerBURMgrStateRestoreIgnore* CBTManServerBURMgrStateRestoreIgnore::NewLC()
	{
	LOG_STATIC_FUNC

	CBTManServerBURMgrStateRestoreIgnore* result = new(ELeave) CBTManServerBURMgrStateRestoreIgnore();
	CleanupStack::PushL(result);
	return result;
	}

CBTManServerBURMgrStateRestoreIgnore::CBTManServerBURMgrStateRestoreIgnore() 
	: CBTManServerBURMgrStateAction(EBTBURStateRestoreIgnore)
	{
	LOG_FUNC
	}		

CBTManServerBURMgrStateRestoreIgnore::~CBTManServerBURMgrStateRestoreIgnore()
	{
	LOG_FUNC
	}

TBTBURState CBTManServerBURMgrStateRestoreIgnore::GetNextState(TBTBUREvent aTransitionEvent)
	{
	LOG_FUNC
	
	// Valid transition events: EBTBUREventNormal
	if (aTransitionEvent==EBTBUREventNormal)
		{
		return EBTBURStateNormal;
		}
	else
		{
		// Unknown state (no valid transition)
		return EBTBURStateUnknown;
		}
	}

void CBTManServerBURMgrStateRestoreIgnore::ExecuteActionL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_FUNC

	aBURMgr.HandleStateRestoreIgnore();
	}

/**
CBTManServerBURMgrStateProcessRestoreFile - Action/transition handling for EBTBURStateProcessRestoreFile state of CBTManServerBURMgr
**/

CBTManServerBURMgrStateProcessRestoreFile* CBTManServerBURMgrStateProcessRestoreFile::NewLC()
	{
	LOG_STATIC_FUNC

	CBTManServerBURMgrStateProcessRestoreFile* result = new(ELeave) CBTManServerBURMgrStateProcessRestoreFile();
	CleanupStack::PushL(result);
	return result;
	}

CBTManServerBURMgrStateProcessRestoreFile::CBTManServerBURMgrStateProcessRestoreFile() 
	: CBTManServerBURMgrStateAction(EBTBURStateProcessRestoreFile)
	{
	LOG_FUNC
	}		

CBTManServerBURMgrStateProcessRestoreFile::~CBTManServerBURMgrStateProcessRestoreFile()
	{
	LOG_FUNC
	}

TBTBURState CBTManServerBURMgrStateProcessRestoreFile::GetNextState(TBTBUREvent aTransitionEvent)
	{
	LOG_FUNC
	
	// Valid transition event: EBTBUREventProcessRestoreFileComplete
	switch (aTransitionEvent)
		{
	case EBTBUREventProcessRestoreFileComplete:
		return EBTBURStateRestoreFileProcessingComplete;
	default:
		// Unknown state (no valid transition)
		return EBTBURStateUnknown;
		}
	}

void CBTManServerBURMgrStateProcessRestoreFile::ExecuteActionL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_FUNC

	aBURMgr.HandleStateProcessRestoreFileL();
	}

void CBTManServerBURMgrStateProcessRestoreFile::HandleActionError(CBTManServerBURMgr& aBURMgr, TInt aError)
	{
	LOG_FUNC

	aBURMgr.HandleStateProcessRestoreFileError(aError);
	}

/**
CBTManServerBURMgrStateRestoreFileProcessingComplete - Action/transition handling for EBTBURStateRestoreFileProcessingComplete state of CBTManServerBURMgr
**/

CBTManServerBURMgrStateRestoreFileProcessingComplete* CBTManServerBURMgrStateRestoreFileProcessingComplete::NewLC()
	{
	LOG_STATIC_FUNC

	CBTManServerBURMgrStateRestoreFileProcessingComplete* result = new(ELeave) CBTManServerBURMgrStateRestoreFileProcessingComplete();
	CleanupStack::PushL(result);
	return result;
	}

CBTManServerBURMgrStateRestoreFileProcessingComplete::CBTManServerBURMgrStateRestoreFileProcessingComplete() 
	: CBTManServerBURMgrStateAction(EBTBURStateRestoreFileProcessingComplete)
	{
	LOG_FUNC
	}		

CBTManServerBURMgrStateRestoreFileProcessingComplete::~CBTManServerBURMgrStateRestoreFileProcessingComplete()
	{
	LOG_FUNC
	}

TBTBURState CBTManServerBURMgrStateRestoreFileProcessingComplete::GetNextState(TBTBUREvent aTransitionEvent)
	{
	LOG_FUNC
	
	// Valid transition events: EBTBUREventRestoreFileTransitionNormal
	if (aTransitionEvent == EBTBUREventRestoreFileTransitionNormal)
		{
		return EBTBURStateNormal;
		}
	else
		{
		// Unknown state (no valid transition)
		return EBTBURStateUnknown;
		}
	}

void CBTManServerBURMgrStateRestoreFileProcessingComplete::ExecuteActionL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_FUNC
	aBURMgr.HandleStateRestoreFileProcessingComplete();
	}
