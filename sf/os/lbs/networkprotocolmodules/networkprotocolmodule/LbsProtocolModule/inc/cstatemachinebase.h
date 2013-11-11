// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the base class for protocol state machines.
// It also defined a timer class and a request queue class used
// by the state machines.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CSTATEMACHINEBASE_H__
#define __CSTATEMACHINEBASE_H__

#include <e32base.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbspositioninfo.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsloccommon.h>
#include "lbstimer.h"
#include "lbsprotocolmoduleerrors.h"
#include "cprotocolmanager.h"


//-----------------------------------------------------------------------------
// State Machine Base Class
//-----------------------------------------------------------------------------

/** KAssistDataTimerEvent
Identity of timer for when assistance data is expected from network
*/
const TInt KAssistDataTimerEvent = 0x01;

/** KAssistDataTimerDurationInSec
Timer duration for when assistance data is expected from network
*/
const TInt KAssistDataTimerDurationInSec = 20;


class CStateQueue;
class CStateHandlerBase;

/** Base class definition for protocol state machines.
This class provides support for common features of all state machines.

A significant aspect incorporated in this base class is the active object
mechanism for performing state transitions. This feature is used to provide
state machines with the ability to perform autonomous and asynchronous actions
e.g. to receive a single network measurement control indication and to perform
one or more interactions with LBS.

State transitions are broken down into stages: to leave one state and then
enter a new state. This staged approach ensures that lengthy protocol module
activity is broken down into shorter steps, thereby releasing the active
scheduler to process other threads. A self-completion approach is employed
to achieve this. @see CStateMachineBase::DoStateTransitionStage()

The base class includes various attributes that describe protocol activity,
together with access methods used to retrieve the value of these attributes.
*/
NONSHARABLE_CLASS(CStateMachineBase) : public CActive
	{
public:

	/** State machine protocol state.
	This defines the general protocol state for state machines.
	*/
	enum TProtocolState
		{
		/** Not valid */
		EStateNull,
		/** Ready to use, but not currently active. */
		EStateReady,
		/** Actively performing a protocol procedure. */
		EStateActive,
		/** In the process of cancelling. */
		EStateCancelling
		};

	/** State machine cancel source.
	This defines the source of a cancellation.
	*/
	enum TCancelSource
		{
		/** Not cancelling */
		ECancelNone,
		/** The LBS client cancelled the procedure */
		ECancelClientCancel,
		/** The LBS client cancelled the procedure and there should be no
		cancellation messages sent to LBS
		*/
		ECancelClientCancelSilent,
		/** The network cancelled the procedure */
		ECancelNetworkCancel,
		/** A network error occurred */
		ECancelNetworkError,
		/** A client LBS timeout occurred */
		ECancelClientTimeout,
		/** A network timeout occurred */
		ECancelNetworkTimeout,
		/** State machine is closing down (destruction)*/
		ECancelClosing
		};

protected:

	/** State transition stage.
	This defines the individual stages used when a state machine makes a
	transition from one state to another. This approach allows the state
	machine to perform a number of asynchronous actions without making 
	any call to RunL too lengthy.
	*/
	enum TTransitionStage
		{
		/** Not in a transition */
		ETransitionNull,
		/** Exit from current state */
		ETransitionExit,
		/** Enter new state */
		ETransitionEnter
		};

public:
	
	virtual ~CStateMachineBase();

	void PerformStateTransition();
	void EnterNewState();
	void ExitCurrentState();
	void SetMachineAsQueued();
	void CheckForQueuedRequest();
	void CancelMachine(const TCancelSource& aCancelSource);
	void CancelMachine(const TCancelSource& aCancelSource, TInt aReason);

	void StartAssistDataTimer();

	void SessionId(const TLbsNetSessionId& aSessionId);
	void QueueSessionId(const TLbsNetSessionId& aSessionId);
	void ResetSessionId();
	const TLbsNetSessionId& SessionId() const;
	const TLbsNetSessionId& QueueSessionId() const;
	MStateMachineObserver& Observer();
	TProtocolState State() const;

	TBool IsMachineQueued() const;
	
	virtual TInt ClientCloseReason() const;
	TInt NetworkCloseReason() const;
	TBool IsNetworkConnectionError() const;
	TBool IsSilentClientCancel() const;

	TInt LocRespReason() const;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService LocReqType() const;

	const TPositionInfoBase& LocRespPosition() const;
	const TPositionInfoBase& ReferenceLoc() const;
	const TLbsNetPosRequestQuality& LocReqQuality() const;
	const TLbsNetPosRequestMethod& PosMethod() const;

	TBool IsLocReqReceived() const;
	TBool IsLocRespSent() const;
	TBool IsMeasureControlHandled() const;
	TBool IsAssistanceDataActionReq() const;

	void HandleMeasureControlError(const TBool& aCancelRequired, TInt aReason);
	
	// CActive derived methods
	
	void RunL();
	void DoCancel();
	
	// Methods that can be overriden in derived classes
	
	virtual void MeasurementControlInd(const TPositionInfoBase& aPosInfo,
										const TLbsNetPosRequestQuality& aQuality,
										const TLbsNetPosRequestMethod& aPosMethod);
	virtual void AssistanceDataReq();
	virtual void AdditionalAssistDataInd();
	virtual void MeasurementControlErrorInd(TInt aReason);
	virtual void NetworkErrorInd();


	// Pure virtual methods to be implemented in derived classes
	
	/** Start the previously queued state machine.
	Derived classes implement a method to start the state machine
	which has been previously queued by the Protocol Manager.
	*/
	virtual void StartQueuedMachine() = 0;
	
	/** Timeout occured waiting for measurement control from network.
	Derived classes implement a method to handle this timeout, with due
	consideration of the prevailing state machine activity.
	*/
	virtual void MeasurementControlTimeout() = 0;
	
	/** A location response has been received from LBS.
	Derived classes implement a method to handle this response, with due
	consideration of the prevailing state machine activity.
	*/
	virtual void LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo) = 0;

	/** Prepare state transition.
	Derived classes implement a method to perform actions prior to a state transition.
	*/
	virtual void PreStateTransition() = 0;
	
	/** Change state.
	Derived classes implement a method to perform actions during a state transition.
	*/
	virtual void StateTransition() = 0;
	
	/** Complete state transition.
	Derived classes implement a method to perform actions after a state transition.
	*/
	virtual void PostStateTransition() = 0;
	
	/** Cancel the active procedure.
	Derived classes implement a method to cancel the active procedure.
	*/
	virtual void CancelProcedure() = 0;
	
	/** Do queued request.
	Derived classes implement a method to perform any requests that were queued
	due to busy activity.
	*/
	virtual void DoQueuedRequest(TInt aRequest) = 0;

protected:

	CStateMachineBase(MStateMachineObserver& aObserver);

	void InitialiseMachineBase();
	void CompleteMachineBase();

	void StoreLocationResp(TInt aReason, const TPositionInfoBase& aPosInfo);
	void StoreNetLocCancelReason(TInt aReason);
	void StoreLocReqQuality(const TLbsNetPosRequestQuality& aQuality);
	void StoreRefLoc(const TPositionInfoBase& aPosInfo);
	void StorePosMethod(const TLbsNetPosRequestMethod& aPosMethod);

	TBool Cancelling() const;
	TBool CancelPending() const;
	void SetMachineAsCancelling();

private:
	
	void DoStateTransitionStage();
	
protected:

	/** Reference to State machine observer
	*/
	MStateMachineObserver& iObserver;
	
	/** Local queue for buffering requests.
	*/
	CStateQueue* iQueue;
	
	/** Pointer to state handler.
	This is owned by the derived classes that create state handlers.
	*/
	CStateHandlerBase* iStateHandler;
	
	/** Current protocol state.
	*/
	TProtocolState iProtocolState;
	
	/** Indicates if state machine is in a queued state.
	*/
	TBool iIsMachineQueued;
	
	/** Session ID.
	This is owned by the derived classes that create state handlers.
	*/
	TLbsNetSessionId iSessionId;
	
	/** ID for a queued session.
	This is owned by the derived classes that create state handlers.
	*/
	TLbsNetSessionId iQueueSessionId;
	
	/** State transition stage enumeration.
	*/
	TTransitionStage iTransitionState;
	
	/** Timer for additional assistance data expected from network
	*/
	CLbsCallbackTimer* iAssistDataTimer;
	
	/** Network Session result.
	*/
	TInt iNetSessionResult;
	
	/** Flag to indicate if state machine is to be cancelled.
	*/
	TBool iIsCancelPending;
	
	/** Indication of the source for cancellation.
	*/
	TCancelSource iCancelSource;
	
	/** A specific reason for client cancelling.
	*/
	TInt iClientCancelReason;
	
	/** A specific reason for network cancelling.
	*/
	TInt iNetworkCancelReason;
	
	/** Flag to show assistance data action required.
	This indicates when assistance data has been requested or received
	and is used to decide when to perform corresponding actions.
	*/
	TBool iAssistanceDataActionRequired;
	
	/** Flag to show location request has been received.
	*/
	TBool iIsLocReqReceived;
	
	/** Flag to show that a response to the location request has
	been sent to the network
	*/
	TBool iIsLocRespSent;
		
	/** Flag to show measurement control data has been handled.
	*/
	TBool iIsMeasureControlHandled;
	
	/** Flag to show additional assistance data is expected.
	*/
	TBool iIsAdditionalDataExpected;

	/** Location request type.
	*/
	MLbsNetworkProtocolObserver::TLbsNetProtocolService iLocReqType;

	/** Location error response reason.
	*/
	TInt iLocRespReason;
	
	/** Location response position info.
	*/
	TPositionInfo iLocRespPosInfo;
	
	/** Reference Location position info.
	*/
	TPositionInfo iReferenceLoc;
	
	/** Location request quality.
	*/
	TLbsNetPosRequestQuality iLocReqQuality;

	/** Network location request cancel reason.
	*/
	TInt iNetLocCancelReason;

	/** Location request positioning method.
	*/
	TLbsNetPosRequestMethod iPosMethod;

	};


//-----------------------------------------------------------------------------
// State Machine Request Queue Class
//-----------------------------------------------------------------------------

/** State machine queue.
This is used to hold requests that require asynchronous actions but have
arrived when the state machine is already in an active state.

Requests are added to the queue and are popped off the queue by a state machine
when it is next in a position to be able to service the request.
*/
NONSHARABLE_CLASS(CStateQueue) : public CBase
{

public:
	static CStateQueue* NewL(TInt aMaxEntries);
	~CStateQueue();

	TBool IsRequestPending();
	TInt AddRequest(TInt aNewRequest);
	TInt PopRequest();
	
protected:
	CStateQueue(TInt aMaxEntries);
	void ConstructL(TInt aMaxEntries);
	
protected:
	/** The internal queue, represented by an array of request numbers.
	*/
	RArray<TInt> iQueue;
	/** The maximum number of request entries permitted in the queue.
	*/
	TInt iMaxEntries;
};


#endif // __CSTATEMACHINEBASE_H__
