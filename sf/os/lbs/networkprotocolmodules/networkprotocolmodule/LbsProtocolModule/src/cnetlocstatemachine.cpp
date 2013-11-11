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
// This file provides the implementation of the class for
// the NETLOC protocol state machine.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "cnetlocstatemachine.h"
#include "cnetlocstatehandler.h"
#include "lbsdevloggermacros.h"

/** KNetLocReqIssuedTimerEvent
Identity of timer for when Network Based Location request issued to network
*/
const TInt KNetLocReqIssuedTimerEvent = 0x800;

/** KNetLocReqIssuedTimerDurationInSec
Timer duration for when Network Based Location request issued to network
*/
const TInt KNetLocReqIssuedTimerDurationInSec = 20;

/** KLocRespTimerEvent
Identity of timer for when location response expected by network
*/
const TInt KLocRespTimerEvent = 0x900;

/** KLocRespTimerDurationInSec
Timer duration for when location response expected by network
*/
const TInt KLocRespTimerDurationInSec = 64;

/** KMaxQueueEntry
Maximum entries in this state machine request queue.
*/
const TInt KMaxQueueEntry = 5;


/** Static constructor.
@param aObserver Reference to state machine observer.
@return A new instance of the CNetLocStateMachine class
*/  
CNetLocStateMachine* CNetLocStateMachine::NewL(MStateMachineObserver& aObserver)
	{
	CNetLocStateMachine* self = new (ELeave) CNetLocStateMachine(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
@param aObserver Reference to state machine observer.
*/  
CNetLocStateMachine::CNetLocStateMachine(MStateMachineObserver& aObserver)
: CStateMachineBase(aObserver)
	{
	iLocReqType = MLbsNetworkProtocolObserver::EServiceNetworkLocation;
	}


/** Standard destructor.
*/  
CNetLocStateMachine::~CNetLocStateMachine()
	{
	delete iLocRespTimer;
	delete iNetLocReqIssuedTimer;
	}


/** Private second-stage constructor.
*/  
void CNetLocStateMachine::ConstructL()
	{
	CActiveScheduler::Add(this);

	// Create state handler
	iStateHandler = CNetLocStateHandler::NewL(*this);

	// Create timers used during Network Based Location protocol procedure
	iNetLocReqIssuedTimer = CLbsCallbackTimer::NewL(*this);
	iLocRespTimer = CLbsCallbackTimer::NewL(*this);
	iAssistDataTimer = CLbsCallbackTimer::NewL(*this);

	// Create request queue
	iQueue = CStateQueue::NewL(KMaxQueueEntry);
	}


/** Retrieve current Network Based Location state
@return TNetLocState Current Network Based Location state
*/
CNetLocStateMachine::TNetLocState CNetLocStateMachine::CurrentState()
	{
	return iCurrentState;
	}

	
/** Start Network Based Location request timer
The state handler calls this when it has issued an Network Based Location request
to the network and it requires a response before the timer expires.
*/
void CNetLocStateMachine::StartNetLocReqTimer()
	{
	if(iNetLocReqIssuedTimer->IsActive())
		{
		iNetLocReqIssuedTimer->Cancel();
		}
	iNetLocReqIssuedTimer->EventAfter(TTimeIntervalSeconds(KNetLocReqIssuedTimerDurationInSec), KNetLocReqIssuedTimerEvent);
	}

	
/** Start location response timer
The state handler calls this when it has sent a location request
to the client and requires a response before the timer expires.
*/
void CNetLocStateMachine::StartLocRespTimer()
	{
	if(iLocRespTimer->IsActive())
		{
		iLocRespTimer->Cancel();
		}
	iLocRespTimer->EventAfter(TTimeIntervalSeconds(KLocRespTimerDurationInSec), KLocRespTimerEvent);
	}


/** Initialise internal state attributes.
This is used when new Network Based Location procedure commences.
*/
void CNetLocStateMachine::InitialiseProcedure()
	{
	// Initialise state machine
	InitialiseMachineBase();

	ASSERT(iStateHandler != NULL);
	iStateHandler->Initialise();
	}
	

/** Prepare state transition.
*/  
void CNetLocStateMachine::PreStateTransition()
	{
	}
	

/** State transition.
This method determines the next state to be adopted by the state machine.
*/  
void CNetLocStateMachine::StateTransition()
	{
	if (CancelPending())
		{
		LBSLOG(ELogP2, "CNetLocStateMachine::StateTransition() ---> EStateCancelling \n");
		SetMachineAsCancelling();
		iCurrentState = EStateCancelling;
		}
	else
		{
		// Set new state		
		switch (iCurrentState)
			{
		case EStateNull:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateNull ---> EStateClientReqRecvd\n");
			iCurrentState = EStateClientReqRecvd;
			break;
			
		case EStateClientReqRecvd:
			LBSLOG(ELogP2, "CNetLocStateMachine: ---> EStateNetSessStarted\n");
			iCurrentState = EStateNetSessStarted;
			break;
			
		case EStateNetSessStarted:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateNetSessStarted ---> EStateMeasureDataRecvd\n");
			iCurrentState = EStateMeasureDataRecvd;
			break;
			
		case EStateMeasureDataRecvd:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateMeasureDataRecvd ---> EStateNetBasedLocSent\n");
			iCurrentState = EStateNetBasedLocSent;
			break;
			
		case EStateNetBasedLocSent:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateNetBasedLocSent ---> EStateLocReqByNet\n");
			iIsMeasureControlHandled = ETrue;
			iCurrentState = EStateLocReqByNet;
			break;
			
		case EStateLocReqByNet:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateLocReqByNet ---> EStateLocRespRecvd\n");
			iCurrentState = EStateLocRespRecvd;
			break;
			
		case EStateLocRespRecvd:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateLocRespRecvd ---> EStateLocSentToNet\n");
			iCurrentState = EStateLocSentToNet;
			break;
			
		case EStateLocSentToNet:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateLocSentToNet ---> EStateNetSessToClose\n");
			iCurrentState = EStateNetSessToClose;
			break;
			
		case EStateNetSessToClose:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateNetSessToClose ---> EStateClientSessToClose\n");
			iCurrentState = EStateClientSessToClose;
			break;
			
		case EStateClientSessToClose:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateClientSessToClose ---> EStateSessionsClosed\n");
			iCurrentState = EStateSessionsClosed;
			break;
			
		case EStateSessionsClosed:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateSessionsClosed --> Procedure has completed\n");
			// Procedure has completed
			CompleteProcedure();
			break;
			
		case EStateCancelling:
			LBSLOG(ELogP2, "CNetLocStateMachine: EStateCancelling ---> EStateNetSessToClose\n");
			iCurrentState = EStateNetSessToClose;
			break;

		default:
			User::Panic(KProtocolModulePanic, EProtocolModuleNetLocState);
			break;
			};
		}

	}
	

/** Complete the procedure.
*/  
void CNetLocStateMachine::CompleteProcedure()
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::CompleteProcedure\n");
	iCurrentState = EStateNull;
	// Complete state machine
	CompleteMachineBase();
	}


/** Complete a state transition.
This is called by the base class when a state transition has
concluded and it provides an opportunity for the state machine 
to perform actions required immediately after this transition.

The method can also initiate a further change of state. This is
relevant when the state machine is required to perform an autonomous
transition from one state to another e.g. this occurs when several
interactions are required arising from a single external trigger.
*/  
void CNetLocStateMachine::PostStateTransition()
	{
	// Some states are transitory i.e. they require
	// an automatic transition to the next state
	if ( CancelPending() ||
		 (EStateClientReqRecvd		== iCurrentState) ||
	 	 (EStateMeasureDataRecvd	== iCurrentState) ||
	 	 (EStateNetBasedLocSent		== iCurrentState) ||
	 	 (EStateLocRespRecvd		== iCurrentState) ||
	 	 (EStateLocSentToNet		== iCurrentState) ||
	 	 (EStateNetSessToClose		== iCurrentState) ||
	 	 (EStateClientSessToClose	== iCurrentState) ||
	 	 (EStateSessionsClosed		== iCurrentState) ||
	 	 (EStateCancelling			== iCurrentState)
	   )
		{
		// Perform a state transition
		PerformStateTransition();
		}
	}


/** Do actions required for a queued request.
This currently only initiates another state change but it 
has the scope for further actions to be carried out according
to the nature of a queued request.
*/
void CNetLocStateMachine::DoQueuedRequest(TInt aRequest)
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::DoQueuedRequest\n");
	switch (aRequest)
		{
	case EQueueLocResponse:
	case EQueueMeasurementControl:
	case EQueueNetworkResult:
		// Perform a state transition
		PerformStateTransition();
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleQueueRequestId);
		break;
		};

	}


/** Cancel the active procedure
*/  
void CNetLocStateMachine::CancelProcedure()
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::CancelProcedure()\n");
	// Kill all timers
	iNetLocReqIssuedTimer->Cancel();
	iLocRespTimer->Cancel();
	iAssistDataTimer->Cancel();
	}


/** Timer expired callback.
This is called by a CStateTimer object when the timer
has expired - the event is identified by aEvent parameter.
@param aTimerId The timer event identifier.
*/
void CNetLocStateMachine::OnTimerEventL(TInt aTimerId)
	{
	
	// Perform relevant action for the expired timer
	switch (aTimerId)
		{

	// Network Based Location request timer
	case KNetLocReqIssuedTimerEvent:
		LBSLOG(ELogP2, "CNetLocStateMachine::OnTimerEventL(KNetLocReqIssuedTimerEvent)\n");
		// Inform protocol manager
		Observer().MeasurementControlTimeout();
		break;

	// Additional assistance data timer
	case KAssistDataTimerEvent:
		LBSLOG(ELogP2, "CNetLocStateMachine::OnTimerEventL(KAssistDataTimerEvent)\n");
		// Ensure assistance data action is performed upon cancellation
		iAssistanceDataActionRequired = ETrue;
		iLocRespReason = KErrTimedOut;
		// Inform protocol manager
		Observer().MeasurementControlTimeout();
		break;

	// Location response timer
	case KLocRespTimerEvent:
		LBSLOG(ELogP2, "CNetLocStateMachine::OnTimerEventL(KLocRespTimerEvent)\n");
		iLocRespReason = KErrTimedOut;
		CancelMachine(ECancelClientTimeout);
		break;

	// Ignore unknown timer events
	default:
		break;

		};

	}


/** Timer callback error handler.
This is called if the timer expiry callback leaves.
@see CNetLocStateMachine::OnTimerEventL()
@param aTimerId The timer event identifier.
@param aError Error value.
*/
TInt CNetLocStateMachine::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::OnTimerError()\n");
	return aError;
	}


/** Handle Measurement Control timeout
*/
void CNetLocStateMachine::MeasurementControlTimeout()
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::MeasurementControlTimeout()\n");
	// Cancel procedure
	CancelMachine(ECancelNetworkTimeout, KErrTimedOut);
	}


/** Handle LBS request for Network Based Location
@param aSessionId Session ID provided by LBS 
*/
void CNetLocStateMachine::NetLocReq(const TLbsNetSessionId& aSessionId)
	{
	LBSLOG2(ELogP2, "CNetLocStateMachine::NetLocReq() SessionId %d\n", aSessionId.SessionNum());
	// Store the supplied ID information
	SessionId(aSessionId);

	// Initialise the new procedure
	InitialiseProcedure();

	// Perform a state transition
	PerformStateTransition();
	}


/** Queue an LBS request for Network Based Location
@param aSessionId Session ID provided by LBS 
*/
void CNetLocStateMachine::QueueNetLocReq(const TLbsNetSessionId& aSessionId)
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::QueueNetLocReq()\n");

	QueueSessionId(aSessionId);

	CStateMachineBase::SetMachineAsQueued();
	}


/** Start a previously queued state machine.
*/  
void CNetLocStateMachine::StartQueuedMachine()
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::StartQueuedMachine()\n");
	NetLocReq(iQueueSessionId);
	}


/** Handle LBS Location response
@param aReason Location response error reason.
@param aPosInfo The location information response from LBS.
*/
void CNetLocStateMachine::LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo)
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::LocationResp()\n");
	// Store location response data
	StoreLocationResp(aReason, aPosInfo);

	// Kill the timer related to location response expected by network
	iLocRespTimer->Cancel();

	// Is the state machine inactive?
	if (!IsActive())
		{
		// Perform a state transition
		PerformStateTransition();
		}
	else
		{
		// Queue the request
		iQueue->AddRequest(EQueueLocResponse);
		}
	}


/** Handle Network Measurement Control indication.
A measurement control indication has been received from the network.
Note: At this stage the assistance data has already been stored by the
protocol manager and is thus not passed into this method.

@param aPosInfo Reference location information provided in the measurement control
@param aQuality Location request quality specified in the measuerment control
*/
void CNetLocStateMachine::MeasurementControlInd(const TPositionInfoBase& aPosInfo,
	const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod)
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::MeasurementControlInd()\n");
	// Kill timer related to the Network Based Location request
	iNetLocReqIssuedTimer->Cancel();

	// Perform base class actions
	CStateMachineBase::MeasurementControlInd(aPosInfo, aQuality, aPosMethod);

	// Is the state machine inactive?
	if (!IsActive())
		{
		// Perform a state transition
		PerformStateTransition();
		}
	else
		{
		// Queue the request
		iQueue->AddRequest(EQueueMeasurementControl);
		}
	}


/** Handle Network Measurement Control Error indication
This is called by the protocol manager durinq an active NETLOC when
a measurement control error is received from the network.
*/
void CNetLocStateMachine::MeasurementControlErrorInd(TInt aReason)
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::MeasurementControlErrorInd()\n");	
	TBool cancelRequired = EFalse;
	TBool assistActionRequired = ETrue;

	// If there is no outstanding assistance data request then 
	// inhibit assistance data error responses.
	if (!iAssistDataTimer->IsActive())
		{
		assistActionRequired = EFalse;
		}

	// If this is the first measurement control received
	// Then we shall need to cancel the state machine
	if (!IsLocReqReceived() || (KErrCancel == aReason))
		{
		// Kill timer related to the Network Based Location request
		iNetLocReqIssuedTimer->Cancel();
		cancelRequired = ETrue;
		}

	// Perform base class actions
	CStateMachineBase::HandleMeasureControlError(cancelRequired, aReason);

	// Ensure assistance data action only happens if necessary
	iAssistanceDataActionRequired = assistActionRequired;
	}


/** Handle a network error indication.
*/  
void CNetLocStateMachine::NetworkErrorInd()
	{
	LBSLOG(ELogP2, "CNetLocStateMachine::NetworkErrorInd()\n");
	// Do we need to report assistance data errors?
	if (!IsLocReqReceived())
		{
		iAssistanceDataActionRequired = ETrue;
		}

	// Perform base class actions
	CStateMachineBase::NetworkErrorInd();
	}

