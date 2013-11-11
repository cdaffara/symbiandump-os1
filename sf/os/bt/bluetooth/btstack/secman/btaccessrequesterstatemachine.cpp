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

#include <bluetooth/logger.h>

#include "secman.h"
#include "secevent.h"
#include "btaccessrequesterstatemachine.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SECMAN);
#endif


void Panic(TBTFlyStateMachinePanic aPanic)
	{
	User::Panic(KBTFlyStateStateMachine, aPanic);
	}


// This is the separation between the action and the constructors of the states.

CBTAccessRequesterStateFactory* CBTAccessRequesterStateFactory::NewL()
	{
	LOG_STATIC_FUNC
	CBTAccessRequesterStateFactory* self = CBTAccessRequesterStateFactory::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CBTAccessRequesterStateFactory* CBTAccessRequesterStateFactory::NewLC()
	{
	LOG_STATIC_FUNC
	CBTAccessRequesterStateFactory* self = new(ELeave) CBTAccessRequesterStateFactory();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CBTAccessRequesterStateFactory::~CBTAccessRequesterStateFactory()
	{
	LOG_FUNC
	iStateActions.ResetAndDestroy();
	}

CBTAccessRequesterStateFactory::CBTAccessRequesterStateFactory()
	{
	LOG_FUNC
	}

void CBTAccessRequesterStateFactory::ConstructL()
	{
	LOG_FUNC

	for(TInt i=EBTUninitialised;i<EBTMaximumStateNumber;i++)
		{
		CBTBaseStateAction* ptr = GetStateL(static_cast<TBTAccessRequesterState>(i));
		CleanupStack::PushL(ptr);
		iStateActions.AppendL(ptr);
		CleanupStack::Pop();
		}
	}



TInt CBTAccessRequesterStateFactory::ProcessRequesterState(TBTAccessRequesterState aState, CBTAccessRequester& aRequester, TBTSecEvent& aEvent)
	{
	LOG_FUNC

	TInt ret = KErrNotFound;

	TBTAccessRequesterState nextState = iStateActions[aState]->GetNextState(aEvent.Event());
	if(nextState != EBTUnknownState)
		{
		LOG(_L8("\tState transition FOUND!"));
		// We should ensure that at each iteration through the state machine the requirements
		// are up-to-date. This is needed as the physical link may have changed
		// some internal state e.g. as a result of the Registry having been modified.
		ret = EBTSecManAccessDenied;
		if(!aRequester.RequirementsDenied())
			{
			ret = KErrNone;
			if(nextState != EBTNopState)
				{
				aRequester.SetCurrentState(nextState);
				iStateActions[nextState]->ExecuteAction(aRequester, aEvent);
				}
			}
		}

	return ret;
	}

CBTBaseStateAction* CBTAccessRequesterStateFactory::GetStateL(TBTAccessRequesterState aState)
	{
	LOG_FUNC

	LOG1(_L8("\taState = %d"),aState);
	CBTBaseStateAction* ptr = NULL;
	switch(aState)
		{
	case EBTUninitialised:
		ptr = CBTUninitialisedState::NewL(aState);
		break;

	case EBTReady:
		ptr = CBTReadyState::NewL(aState);
		break;

	case EBTWaiting:
		ptr = CBTWaitingState::NewL(aState);
		break;

	case EBTPhysicalLinkUp:
		ptr = CBTPhysicalLinkUpState::NewL(aState);
		break;

	case EBTAuthenticationRequested:
		ptr = CBTAuthenticationRequestedState::NewL(aState);
		break;

	case EBTRequestAuthentication:
		ptr = CBTRequestAuthenticationState::NewL(aState);
		break;

	case EBTAuthenticationComplete:
		ptr = CBTAuthenticationCompleteState::NewL(aState);
		break;

	case EBTEncryptionChangePending:
		ptr = CBTEncryptionChangePendingState::NewL(aState);
		break;

	case EBTEncryptionChangeComplete:
		ptr = CBTEncryptionChangeCompleteState::NewL(aState);
		break;

	case EBTAuthorisationRequested:
		ptr = CBTAuthorisationRequestedState::NewL(aState);
		break;

	case EBTRequestAuthorisation:
		ptr = CBTRequestAuthorisationState::NewL(aState);
		break;

	case EBTAuthorisationComplete:
		ptr = CBTAuthorisationCompleteState::NewL(aState);
		break;

	// Simple Pairing actions
	case EBTIOCapsResponse:
		ptr = CBTIOCapsResponseState::NewL(aState);
		break;

	case EBTIOCapsRequested:
		ptr = CBTIOCapsRequestedState::NewL(aState);
		break;

	case EBTUserConfirmation:
		ptr = CBTUserConfirmationState::NewL(aState);
		break;

	case EBTPasskeyEntry:
		ptr = CBTPasskeyEntryState::NewL(aState);
		break;

	case EBTRemoteOOBDataRequest:
		ptr = CBTRemoteOOBDataRequestState::NewL(aState);
		break;

	case EBTSimplePairingPending:
		ptr = CBTSimplePairingPendingState::NewL(aState);
		break;

	case EBTPrefetchZombie:
		ptr = CBTPrefetchZombieState::NewL(aState);
		break;

	default:
		__ASSERT_ALWAYS(0, User::Panic(KBTFlyStateStateMachine,EBTFlyUnknowState));
		break;
		}//end switch

	return ptr;
	}


/*********************** SECOND GENERATION CODE ************/

/**
class CBTBaseStateAction
*/
CBTBaseStateAction* CBTBaseStateAction::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTBaseStateAction* self = CBTBaseStateAction::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTBaseStateAction* CBTBaseStateAction::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTBaseStateAction* self = new(ELeave) CBTBaseStateAction(aState);
	CleanupStack::PushL(self);
	return self;
	}
CBTBaseStateAction::CBTBaseStateAction(TBTAccessRequesterState aState)
: iState(aState)
	{
	LOG_FUNC
	}


CBTBaseStateAction::~CBTBaseStateAction()
	{
	LOG_FUNC
	}

/* virtual */void CBTBaseStateAction::ExecuteAction(CBTAccessRequester& /*aRequester*/,TBTSecEvent& /*aEvent*/)
	{
	LOG_FUNC
	}

/* virtual */TBTAccessRequesterState CBTBaseStateAction::GetNextState(TInt /*aEventId*/)
	{
	LOG_FUNC
	return EBTUnknownState;
	}


// CBTUninitialisedState
CBTUninitialisedState* CBTUninitialisedState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTUninitialisedState* self = CBTUninitialisedState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTUninitialisedState* CBTUninitialisedState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTUninitialisedState* self = new(ELeave) CBTUninitialisedState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTUninitialisedState::~CBTUninitialisedState()
	{
	LOG_FUNC
	}

void CBTUninitialisedState::ExecuteAction(CBTAccessRequester& /*aRequester*/,TBTSecEvent& /*aEvent*/)
	{
	LOG_FUNC
	//this should never be called
	__ASSERT_ALWAYS(0, User::Panic(KBTFlyStateStateMachine,EBTFlyMismatchExecuteActionState));
	}

TBTAccessRequesterState CBTUninitialisedState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;
	switch(aEventId)
		{
	case TBTSecEvent::EStart:
		state = EBTWaiting;
		break;
	case TBTSecEvent::EPhysicalLinkUp:
		state = EBTPhysicalLinkUp;
		break;
	case TBTSecEvent::EPhysicalLinkDown:
		state = EBTPrefetchZombie;
		break;
		}
	return state;
	}


/**
class CBTWaitingState
*/
CBTWaitingState* CBTWaitingState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTWaitingState* self = CBTWaitingState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTWaitingState* CBTWaitingState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTWaitingState* self = new(ELeave) CBTWaitingState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTWaitingState::~CBTWaitingState()
	{
	LOG_FUNC
	}

void CBTWaitingState::ExecuteAction(CBTAccessRequester& /*aRequester*/, TBTSecEvent& /*aEvent*/)
	{
	LOG_FUNC
	// Nothing to do except wait for the next even.
	}

TBTAccessRequesterState CBTWaitingState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;
	switch(aEventId)
		{
	case TBTSecEvent::EPhysicalLinkUp:
		state = EBTReady;
		break;
	case TBTSecEvent::EPhysicalLinkDown:
		state = EBTPrefetchZombie;
		break;
	//Handle the Encryption change event
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;
		}
	return state;
	}


/**
class CBTPhysicalLinkUpState
*/
CBTPhysicalLinkUpState* CBTPhysicalLinkUpState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTPhysicalLinkUpState* self = CBTPhysicalLinkUpState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTPhysicalLinkUpState* CBTPhysicalLinkUpState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTPhysicalLinkUpState* self = new(ELeave) CBTPhysicalLinkUpState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTPhysicalLinkUpState::~CBTPhysicalLinkUpState()
	{
	LOG_FUNC
	}

void CBTPhysicalLinkUpState::ExecuteAction(CBTAccessRequester& /*aRequester*/, TBTSecEvent& /*aEvent*/)
	{
	LOG_FUNC
	// Nothing to do
	}

TBTAccessRequesterState CBTPhysicalLinkUpState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	// This state the access requester is waiting to start - any other events
	// are ignored for the moment.
	TBTAccessRequesterState state = EBTPhysicalLinkUp;
	switch(aEventId)
		{
	case TBTSecEvent::EStart:
		state = EBTReady;
		break;
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;
	case TBTSecEvent::EPhysicalLinkDown:
		state = EBTPrefetchZombie;
		break;
		}
	return state;
	}


/**
class CBTReadyState
*/
CBTReadyState* CBTReadyState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTReadyState* self = CBTReadyState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTReadyState* CBTReadyState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTReadyState* self = new(ELeave) CBTReadyState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTReadyState::~CBTReadyState()
	{
	LOG_FUNC
	}

void CBTReadyState::ExecuteAction(CBTAccessRequester& aRequester, TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.Ready(aEvent);
	}

TBTAccessRequesterState CBTReadyState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;
	switch(aEventId)
		{
	case TBTSecEvent::ERequestAuthentication:
		state = EBTRequestAuthentication;
		break;

	case TBTSecEvent::EAuthenticationComplete:
		state = EBTAuthenticationComplete;
		break;

	case TBTSecEvent::EAuthenticationRequested:
		state = EBTAuthenticationRequested;
		break;
		
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;

	case TBTSecEvent::EPhysicalLinkDown:
		state = EBTPrefetchZombie;
		break;
		}
	return state;
	}



/**
class CBTAuthenticationRequestedState
*/
CBTAuthenticationRequestedState* CBTAuthenticationRequestedState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTAuthenticationRequestedState* self = CBTAuthenticationRequestedState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTAuthenticationRequestedState* CBTAuthenticationRequestedState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTAuthenticationRequestedState* self = new(ELeave) CBTAuthenticationRequestedState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTAuthenticationRequestedState::~CBTAuthenticationRequestedState()
	{
	LOG_FUNC
	}

/* virtual */void CBTAuthenticationRequestedState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.AuthenticationRequested(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTAuthenticationRequestedState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EAuthenticationComplete:
		state = EBTAuthenticationComplete;
		break;

	case TBTSecEvent::EIOCapsResponse:
		state = EBTIOCapsResponse;
		break;

	case TBTSecEvent::EIOCapsRequested:
		state = EBTIOCapsRequested;
		break;
		
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;

	case TBTSecEvent::EPhysicalLinkDown:
		state = EBTPrefetchZombie;
		break;
		}
	return state;
	}


/**
class CBTRequestAuthenticationState
*/
CBTRequestAuthenticationState* CBTRequestAuthenticationState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTRequestAuthenticationState* self = CBTRequestAuthenticationState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTRequestAuthenticationState* CBTRequestAuthenticationState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTRequestAuthenticationState* self = new(ELeave) CBTRequestAuthenticationState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTRequestAuthenticationState::~CBTRequestAuthenticationState()
	{
	LOG_FUNC
	}

/* virtual */void CBTRequestAuthenticationState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.RequestAuthentication(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTRequestAuthenticationState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EAuthenticationComplete:
		state = EBTAuthenticationComplete;
		break;

	case TBTSecEvent::EIOCapsResponse:
		state = EBTIOCapsResponse;
		break;

	case TBTSecEvent::EIOCapsRequested:
		state = EBTIOCapsRequested;
		break;
		
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;

	case TBTSecEvent::EPhysicalLinkDown:
		state = EBTPrefetchZombie;
		break;
		}
	return state;
	}


/**
class CBTAuthenticationCompleteState
*/
CBTAuthenticationCompleteState* CBTAuthenticationCompleteState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTAuthenticationCompleteState* self = CBTAuthenticationCompleteState::NewLC(aState);
	CleanupStack::Pop(self);
	return self;
	}

CBTAuthenticationCompleteState* CBTAuthenticationCompleteState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTAuthenticationCompleteState* self = new(ELeave) CBTAuthenticationCompleteState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTAuthenticationCompleteState::~CBTAuthenticationCompleteState()
	{
	LOG_FUNC
	}

/* virtual */void CBTAuthenticationCompleteState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.AuthenticationComplete(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTAuthenticationCompleteState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::ERequestEncryptionChange: //fall through
	case TBTSecEvent::EEncryptionChangeRequested:
		state = EBTEncryptionChangePending;
		break;
	
	case TBTSecEvent::ERequestAuthentication: // for re-issue
		state = EBTRequestAuthentication;
		break;
	case TBTSecEvent::EAuthenticationRequested: // maybe
		state = EBTAuthenticationRequested;
		break;

	case TBTSecEvent::EPhysicalLinkDown:
		state = EBTPrefetchZombie;
		break;
		}
	return state;
	}


/**
class CBTEncryptionChangePendingState
*/
CBTEncryptionChangePendingState* CBTEncryptionChangePendingState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTEncryptionChangePendingState* self = CBTEncryptionChangePendingState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTEncryptionChangePendingState* CBTEncryptionChangePendingState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTEncryptionChangePendingState* self = new(ELeave) CBTEncryptionChangePendingState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTEncryptionChangePendingState::~CBTEncryptionChangePendingState()
	{
	LOG_FUNC
	}

/* virtual */void CBTEncryptionChangePendingState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.EncryptionChangePending(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTEncryptionChangePendingState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTEncryptionChangeComplete;
		break;
		}
	return state;
	}


/**
class CBTEncryptionChangeCompleteState
*/
CBTEncryptionChangeCompleteState* CBTEncryptionChangeCompleteState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTEncryptionChangeCompleteState* self = CBTEncryptionChangeCompleteState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTEncryptionChangeCompleteState* CBTEncryptionChangeCompleteState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTEncryptionChangeCompleteState* self = new(ELeave) CBTEncryptionChangeCompleteState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTEncryptionChangeCompleteState::~CBTEncryptionChangeCompleteState()
	{
	LOG_FUNC
	}

/* virtual */void CBTEncryptionChangeCompleteState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.EncryptionChangeComplete(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTEncryptionChangeCompleteState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::ERequestEncryptionChange:
		state = EBTEncryptionChangePending;
		break;

	case TBTSecEvent::ERequestAuthorisation:
		state = EBTRequestAuthorisation;
		break;
		}
	return state;
	}


/**
class CBTAuthorisationRequestedState
*/
CBTAuthorisationRequestedState* CBTAuthorisationRequestedState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTAuthorisationRequestedState* self = CBTAuthorisationRequestedState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTAuthorisationRequestedState* CBTAuthorisationRequestedState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTAuthorisationRequestedState* self = new(ELeave) CBTAuthorisationRequestedState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTAuthorisationRequestedState::~CBTAuthorisationRequestedState()
	{
	LOG_FUNC
	}

/* virtual */void CBTAuthorisationRequestedState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.AuthorisationRequested(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTAuthorisationRequestedState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EAuthorisationComplete:
		state = EBTAuthorisationComplete;
		break;
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;
		}
	return state;
	}


/**
class CBTRequestAuthorisationState
*/
CBTRequestAuthorisationState* CBTRequestAuthorisationState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTRequestAuthorisationState* self = CBTRequestAuthorisationState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTRequestAuthorisationState* CBTRequestAuthorisationState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTRequestAuthorisationState* self = new(ELeave) CBTRequestAuthorisationState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTRequestAuthorisationState::~CBTRequestAuthorisationState()
	{
	LOG_FUNC
	}

/* virtual */void CBTRequestAuthorisationState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.RequestAuthorisation(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTRequestAuthorisationState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EAuthorisationRequested:
		state = EBTAuthorisationRequested;
		break;
		
	case TBTSecEvent::EAuthorisationComplete:
		state = EBTAuthorisationComplete;
		break;
		
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;
		}
	return state;
	}


/**
class CBTAuthorisationCompleteState
*/
CBTAuthorisationCompleteState* CBTAuthorisationCompleteState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTAuthorisationCompleteState* self = CBTAuthorisationCompleteState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTAuthorisationCompleteState* CBTAuthorisationCompleteState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTAuthorisationCompleteState* self = new(ELeave) CBTAuthorisationCompleteState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTAuthorisationCompleteState::~CBTAuthorisationCompleteState()
	{
	LOG_FUNC
	}

/* virtual */void CBTAuthorisationCompleteState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.AuthorisationComplete(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTAuthorisationCompleteState::GetNextState(TInt /*aEventId*/)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(0, User::Panic(KBTFlyStateStateMachine,EBTFlyUnknowNextState));
	return EBTUnknownState;
	}


/**
class CBTIOCapsResponseState
*/
CBTIOCapsResponseState* CBTIOCapsResponseState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTIOCapsResponseState* self = CBTIOCapsResponseState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTIOCapsResponseState* CBTIOCapsResponseState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTIOCapsResponseState* self = new(ELeave) CBTIOCapsResponseState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTIOCapsResponseState::~CBTIOCapsResponseState()
	{
	LOG_FUNC
	}

/* virtual */void CBTIOCapsResponseState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.IOCapsResponse(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTIOCapsResponseState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EAuthenticationComplete:
		state = EBTAuthenticationComplete;
		break;

	case TBTSecEvent::EIOCapsRequested:
		state = EBTIOCapsRequested;
		break;

	case TBTSecEvent::EUserConfirmationRequested:
		state = EBTUserConfirmation;
		break;

	case TBTSecEvent::EPasskeyNotfication:
		state = EBTPasskeyEntry;
		break;

	case TBTSecEvent::ERemoteOOBDataRequested:
		state = EBTRemoteOOBDataRequest;
		break;
		
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;
		}
	return state;
	}


/**
class CBTIOCapsRequestedState
*/
CBTIOCapsRequestedState* CBTIOCapsRequestedState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTIOCapsRequestedState* self = CBTIOCapsRequestedState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTIOCapsRequestedState* CBTIOCapsRequestedState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTIOCapsRequestedState* self = new(ELeave) CBTIOCapsRequestedState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTIOCapsRequestedState::~CBTIOCapsRequestedState()
	{
	LOG_FUNC
	}

/* virtual */void CBTIOCapsRequestedState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.IOCapsRequested(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTIOCapsRequestedState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EAuthenticationComplete:
		state = EBTAuthenticationComplete;
		break;

	case TBTSecEvent::EIOCapsResponse:
		state = EBTIOCapsResponse;
		break;

	case TBTSecEvent::EUserConfirmationRequested:
		state = EBTUserConfirmation;
		break;

	case TBTSecEvent::EPasskeyNotfication:
		state = EBTPasskeyEntry;
		break;

	case TBTSecEvent::ERemoteOOBDataRequested:
		state = EBTRemoteOOBDataRequest;
		break;
		
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;
		}
	return state;
	}


/**
class CBTUserConfirmationState
*/
CBTUserConfirmationState* CBTUserConfirmationState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTUserConfirmationState* self = CBTUserConfirmationState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTUserConfirmationState* CBTUserConfirmationState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTUserConfirmationState* self = new(ELeave) CBTUserConfirmationState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTUserConfirmationState::~CBTUserConfirmationState()
	{
	LOG_FUNC
	}

/* virtual */void CBTUserConfirmationState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.UserConfirmation(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTUserConfirmationState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EUserConfirmationRequestComplete:
		state = EBTSimplePairingPending;
		break;

	case TBTSecEvent::EAuthenticationComplete:
		state = EBTAuthenticationComplete;
		break;
		
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;
		}
	return state;
	}


/**
class CBTPasskeyEntryState
*/
CBTPasskeyEntryState* CBTPasskeyEntryState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTPasskeyEntryState* self = CBTPasskeyEntryState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTPasskeyEntryState* CBTPasskeyEntryState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTPasskeyEntryState* self = new(ELeave) CBTPasskeyEntryState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTPasskeyEntryState::~CBTPasskeyEntryState()
	{
	LOG_FUNC
	}

/* virtual */void CBTPasskeyEntryState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.PasskeyEntry(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTPasskeyEntryState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EKeypressEntry:
		state = EBTPasskeyEntry;
		break;

	case TBTSecEvent::EAuthenticationComplete:
		state = EBTAuthenticationComplete;
		break;

	case TBTSecEvent::EKeypressComplete:
		state = EBTSimplePairingPending;
		break;
		
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;
		}
	return state;
	}


/**
class CBTRemoteOOBDataRequestState
*/
CBTRemoteOOBDataRequestState* CBTRemoteOOBDataRequestState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTRemoteOOBDataRequestState* self = CBTRemoteOOBDataRequestState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTRemoteOOBDataRequestState* CBTRemoteOOBDataRequestState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTRemoteOOBDataRequestState* self = new(ELeave) CBTRemoteOOBDataRequestState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTRemoteOOBDataRequestState::~CBTRemoteOOBDataRequestState()
	{
	LOG_FUNC
	}

/* virtual */void CBTRemoteOOBDataRequestState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.RemoteOOBDataRequest(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTRemoteOOBDataRequestState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EAuthenticationComplete:
		state = EBTAuthenticationComplete;
		break;

	case TBTSecEvent::ERemoteOOBDataRequestComplete:
		state = EBTSimplePairingPending;
		break;
		
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;
		}
	return state;
	}


/**
class CBTSimplePairingPendingState
*/
CBTSimplePairingPendingState* CBTSimplePairingPendingState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTSimplePairingPendingState* self = CBTSimplePairingPendingState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTSimplePairingPendingState* CBTSimplePairingPendingState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTSimplePairingPendingState* self = new(ELeave) CBTSimplePairingPendingState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTSimplePairingPendingState::~CBTSimplePairingPendingState()
	{
	LOG_FUNC
	}

/* virtual */void CBTSimplePairingPendingState::ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent)
	{
	LOG_FUNC
	aRequester.SimplePairingPending(aEvent);
	}

/* virtual */TBTAccessRequesterState CBTSimplePairingPendingState::GetNextState(TInt aEventId)
	{
	LOG_FUNC
	TBTAccessRequesterState state = EBTUnknownState;

	switch(aEventId)
		{
	case TBTSecEvent::EKeypressEntry:
		// Although display is finished - keypresses can be received still (as
		// authentication may still be in progress.)
		state = EBTPasskeyEntry;
		break;
	case TBTSecEvent::EAuthenticationComplete:
		state = EBTAuthenticationComplete;
		break;
	case TBTSecEvent::EEncryptionChangeComplete:
		state = EBTNopState;
		break;
		}
	return state;
	}


/**
class CBTPrefetchZombieState
*/
CBTPrefetchZombieState* CBTPrefetchZombieState::NewL(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTPrefetchZombieState* self = CBTPrefetchZombieState::NewLC(aState);
	CleanupStack::Pop();	//self
	return self;
	}

CBTPrefetchZombieState* CBTPrefetchZombieState::NewLC(TBTAccessRequesterState aState)
	{
	LOG_STATIC_FUNC
	CBTPrefetchZombieState* self = new(ELeave) CBTPrefetchZombieState(aState);
	CleanupStack::PushL(self);
	return self;
	}

CBTPrefetchZombieState::~CBTPrefetchZombieState()
	{
	LOG_FUNC
	}

void CBTPrefetchZombieState::ExecuteAction(CBTAccessRequester& /*aRequester*/, TBTSecEvent& /*aEvent*/)
	{
	LOG_FUNC
	// Do nothing this state swallows all events as the requester is a member of the living dead.
	}

TBTAccessRequesterState CBTPrefetchZombieState::GetNextState(TInt /*aEventId*/)
	{
	LOG_FUNC
	// Once dead, remain dead
	return EBTPrefetchZombie;
	}
