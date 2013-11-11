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

#ifndef _BTACCESSREQUESTERSTATEMACHINE_H
#define _BTACCESSREQUESTERSTATEMACHINE_H

#include "secman.h"

class CBTAccessRequester;
class CBTAccessRequesterTransition;
class TBTSecEvent;
class CBTBaseStateAction;


enum TBTAccessRequesterState
	{
	EBTUninitialised,
	EBTReady,
	EBTWaiting,
	EBTPhysicalLinkUp,
	EBTAuthenticationRequested,
	EBTRequestAuthentication,
	EBTAuthenticationComplete,
	EBTEncryptionChangePending,
	EBTEncryptionChangeComplete,
	EBTAuthorisationRequested,
	EBTRequestAuthorisation,
	EBTAuthorisationComplete,
	EBTIOCapsResponse,
	EBTIOCapsRequested,
	EBTUserConfirmation,
	EBTPasskeyEntry,
	EBTRemoteOOBDataRequest,
	EBTSimplePairingPending,
	EBTPrefetchZombie,

	// --- End of states ---
	EBTMaximumStateNumber,
	EBTUnknownState, //< Special state value
	EBTNopState, //< Special state value
	};


NONSHARABLE_CLASS(CBTAccessRequesterStateFactory)
	: public CBase
	{
public:
	static CBTAccessRequesterStateFactory* NewL();
	static CBTAccessRequesterStateFactory* NewLC();
	~CBTAccessRequesterStateFactory();

	void ExecuteAction(TBTAccessRequesterState aState, CBTAccessRequester& aRequester, TBTSecEvent& aEvent);

	TInt ProcessRequesterState(TBTAccessRequesterState aState, CBTAccessRequester& aRequester, TBTSecEvent& aEvent);

private:
	CBTAccessRequesterStateFactory();
	void ConstructL();
	void ConstructStateMachineL();
	void AddTransitionL(CBTAccessRequesterTransition* aTransition);
	CBTBaseStateAction* GetStateL(TBTAccessRequesterState aState);


private:
	RPointerArray<CBTBaseStateAction> iStateActions;
	};



_LIT(KBTFlyStateStateMachine, "BT FlyStateMachine");
enum TBTFlyStateMachinePanic
	{
	EBTFlyUnknowState,
	EBTFlyMismatchState,
	EBTFlyMismatchExecuteActionState,
	EBTFlyUnknowNextState,
	};


/*********************** SECOND GENERATION CODE ************/

/**
NONSHARABLE_CLASS(CBTBaseStateAction
*/
NONSHARABLE_CLASS(CBTBaseStateAction) : public CBase
	{

public:
	static CBTBaseStateAction* NewL(TBTAccessRequesterState aState);
	static CBTBaseStateAction* NewLC(TBTAccessRequesterState aState);
	~CBTBaseStateAction();

	virtual void ExecuteAction(CBTAccessRequester& aRequester, TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTBaseStateAction(TBTAccessRequesterState aState);


protected:
	TBTAccessRequesterState iState;
	};



NONSHARABLE_CLASS(CBTUninitialisedState) : public CBTBaseStateAction
	{

public:
	static CBTUninitialisedState* NewL(TBTAccessRequesterState aState);
	static CBTUninitialisedState* NewLC(TBTAccessRequesterState aState);
	~CBTUninitialisedState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTUninitialisedState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};

NONSHARABLE_CLASS(CBTWaitingState) : public CBTBaseStateAction
	{

public:
	static CBTWaitingState* NewL(TBTAccessRequesterState aState);
	static CBTWaitingState* NewLC(TBTAccessRequesterState aState);
	~CBTWaitingState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTWaitingState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTReadyState
*/
NONSHARABLE_CLASS(CBTReadyState) : public CBTBaseStateAction
	{

public:
	static CBTReadyState* NewL(TBTAccessRequesterState aState);
	static CBTReadyState* NewLC(TBTAccessRequesterState aState);
	~CBTReadyState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTReadyState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTPhysicalLinkUpState
*/
NONSHARABLE_CLASS(CBTPhysicalLinkUpState) : public CBTBaseStateAction
	{

public:
	static CBTPhysicalLinkUpState* NewL(TBTAccessRequesterState aState);
	static CBTPhysicalLinkUpState* NewLC(TBTAccessRequesterState aState);
	~CBTPhysicalLinkUpState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTPhysicalLinkUpState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTAuthenticationRequestedState
*/
NONSHARABLE_CLASS(CBTAuthenticationRequestedState) : public CBTBaseStateAction
	{

public:
	static CBTAuthenticationRequestedState* NewL(TBTAccessRequesterState aState);
	static CBTAuthenticationRequestedState* NewLC(TBTAccessRequesterState aState);
	~CBTAuthenticationRequestedState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTAuthenticationRequestedState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTRequestAuthenticationState
*/
NONSHARABLE_CLASS(CBTRequestAuthenticationState) : public CBTBaseStateAction
	{

public:
	static CBTRequestAuthenticationState* NewL(TBTAccessRequesterState aState);
	static CBTRequestAuthenticationState* NewLC(TBTAccessRequesterState aState);
	~CBTRequestAuthenticationState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTRequestAuthenticationState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTAuthenticationCompleteState
*/
NONSHARABLE_CLASS(CBTAuthenticationCompleteState) : public CBTBaseStateAction
	{

public:
	static CBTAuthenticationCompleteState* NewL(TBTAccessRequesterState aState);
	static CBTAuthenticationCompleteState* NewLC(TBTAccessRequesterState aState);
	~CBTAuthenticationCompleteState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTAuthenticationCompleteState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};



/**
NONSHARABLE_CLASS(CBTEncryptionChangePendingState
*/
NONSHARABLE_CLASS(CBTEncryptionChangePendingState) : public CBTBaseStateAction
	{

public:
	static CBTEncryptionChangePendingState* NewL(TBTAccessRequesterState aState);
	static CBTEncryptionChangePendingState* NewLC(TBTAccessRequesterState aState);
	~CBTEncryptionChangePendingState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTEncryptionChangePendingState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTEncryptionChangeCompleteState
*/
NONSHARABLE_CLASS(CBTEncryptionChangeCompleteState) : public CBTBaseStateAction
	{

public:
	static CBTEncryptionChangeCompleteState* NewL(TBTAccessRequesterState aState);
	static CBTEncryptionChangeCompleteState* NewLC(TBTAccessRequesterState aState);
	~CBTEncryptionChangeCompleteState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTEncryptionChangeCompleteState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTAuthorisationRequestedState
*/
NONSHARABLE_CLASS(CBTAuthorisationRequestedState) : public CBTBaseStateAction
	{

public:
	static CBTAuthorisationRequestedState* NewL(TBTAccessRequesterState aState);
	static CBTAuthorisationRequestedState* NewLC(TBTAccessRequesterState aState);
	~CBTAuthorisationRequestedState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTAuthorisationRequestedState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTRequestAuthorisationState
*/
NONSHARABLE_CLASS(CBTRequestAuthorisationState) : public CBTBaseStateAction
	{

public:
	static CBTRequestAuthorisationState* NewL(TBTAccessRequesterState aState);
	static CBTRequestAuthorisationState* NewLC(TBTAccessRequesterState aState);
	~CBTRequestAuthorisationState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTRequestAuthorisationState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTAuthorisationCompleteState
*/
NONSHARABLE_CLASS(CBTAuthorisationCompleteState) : public CBTBaseStateAction
	{

public:
	static CBTAuthorisationCompleteState* NewL(TBTAccessRequesterState aState);
	static CBTAuthorisationCompleteState* NewLC(TBTAccessRequesterState aState);
	~CBTAuthorisationCompleteState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTAuthorisationCompleteState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTIOCapsResponseState
*/
NONSHARABLE_CLASS(CBTIOCapsResponseState) : public CBTBaseStateAction
	{

public:
	static CBTIOCapsResponseState* NewL(TBTAccessRequesterState aState);
	static CBTIOCapsResponseState* NewLC(TBTAccessRequesterState aState);
	~CBTIOCapsResponseState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTIOCapsResponseState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTIOCapsRequestedState
*/
NONSHARABLE_CLASS(CBTIOCapsRequestedState) : public CBTBaseStateAction
	{

public:
	static CBTIOCapsRequestedState* NewL(TBTAccessRequesterState aState);
	static CBTIOCapsRequestedState* NewLC(TBTAccessRequesterState aState);
	~CBTIOCapsRequestedState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTIOCapsRequestedState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTUserConfirmationState
*/
NONSHARABLE_CLASS(CBTUserConfirmationState) : public CBTBaseStateAction
	{

public:
	static CBTUserConfirmationState* NewL(TBTAccessRequesterState aState);
	static CBTUserConfirmationState* NewLC(TBTAccessRequesterState aState);
	~CBTUserConfirmationState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTUserConfirmationState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTPasskeyEntryState
*/
NONSHARABLE_CLASS(CBTPasskeyEntryState) : public CBTBaseStateAction
	{

public:
	static CBTPasskeyEntryState* NewL(TBTAccessRequesterState aState);
	static CBTPasskeyEntryState* NewLC(TBTAccessRequesterState aState);
	~CBTPasskeyEntryState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTPasskeyEntryState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTRemoteOOBDataRequestState
*/
NONSHARABLE_CLASS(CBTRemoteOOBDataRequestState) : public CBTBaseStateAction
	{

public:
	static CBTRemoteOOBDataRequestState* NewL(TBTAccessRequesterState aState);
	static CBTRemoteOOBDataRequestState* NewLC(TBTAccessRequesterState aState);
	~CBTRemoteOOBDataRequestState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTRemoteOOBDataRequestState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


/**
NONSHARABLE_CLASS(CBTSimplePairingPendingState
*/
NONSHARABLE_CLASS(CBTSimplePairingPendingState) : public CBTBaseStateAction
	{

public:
	static CBTSimplePairingPendingState* NewL(TBTAccessRequesterState aState);
	static CBTSimplePairingPendingState* NewLC(TBTAccessRequesterState aState);
	~CBTSimplePairingPendingState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTSimplePairingPendingState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};


NONSHARABLE_CLASS(CBTPrefetchZombieState) : public CBTBaseStateAction
	{

public:
	static CBTPrefetchZombieState* NewL(TBTAccessRequesterState aState);
	static CBTPrefetchZombieState* NewLC(TBTAccessRequesterState aState);
	~CBTPrefetchZombieState();

	virtual void ExecuteAction(CBTAccessRequester& aRequester,TBTSecEvent& aEvent);
	virtual TBTAccessRequesterState GetNextState(TInt aEventId);

protected:
	CBTPrefetchZombieState(TBTAccessRequesterState aState)
	: CBTBaseStateAction(aState)
	{}

	};



#endif //_BTACCESSREQUESTERSTATEMACHINE_H
