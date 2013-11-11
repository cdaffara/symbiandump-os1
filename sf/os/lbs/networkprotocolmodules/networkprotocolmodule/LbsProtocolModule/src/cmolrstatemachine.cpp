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
// the MO-LR protocol state machine.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "cmolrstatemachine.h"
#include "cmolrstatehandler.h"


/** KMoLrReqIssuedTimerEvent
Identity of timer for when MO-LR request issued to network
*/
const TInt KMoLrReqIssuedTimerEvent = 0x100;

/** KMoLrReqIssuedTimerDurationInSec
Timer duration for when MO-LR request issued to network
*/
const TInt KMoLrReqIssuedTimerDurationInSec = 20;

/** KLocRespTimerEvent
Identity of timer for when location response expected by network
*/
const TInt KLocRespTimerEvent = 0x200;

/** KLocRespTimerDurationInSec
Timer duration for when location response expected by network
*/
const TInt KLocRespTimerDurationInSec = 64;

/** KFacResultTimerEvent
Identity of timer for when facility result expected from network
*/
const TInt KFacResultTimerEvent = 0x300;

/** KFacResultTimerDurationInSec
Timer duration for when facility result expected from network
*/
const TInt KFacResultTimerDurationInSec = 10;

/** KMaxQueueEntry
Maximum entries in this state machine request queue.
*/
const TInt KMaxQueueEntry = 5;


/** Static constructor.
@param aObserver Reference to state machine observer.
@return A new instance of the CMoLrStateMachine class
*/  
CMoLrStateMachine* CMoLrStateMachine::NewL(MStateMachineObserver& aObserver)
	{
	CMoLrStateMachine* self = new (ELeave) CMoLrStateMachine(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
@param aObserver Reference to state machine observer.
*/  
CMoLrStateMachine::CMoLrStateMachine(MStateMachineObserver& aObserver)
: CStateMachineBase(aObserver)
	{
	iLocReqType = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	}


/** Standard destructor.
*/  
CMoLrStateMachine::~CMoLrStateMachine()
	{
	delete iFacResultTimer;
	delete iLocRespTimer;
	delete iMoLrReqIssuedTimer;
	}


/** Private second-stage constructor.
*/  
void CMoLrStateMachine::ConstructL()
	{
	CActiveScheduler::Add(this);

	// Create state handler
	iStateHandler = CMoLrStateHandler::NewL(*this);

	// Create timers used during MO-LR protocol procedure
	iMoLrReqIssuedTimer = CLbsCallbackTimer::NewL(*this);
	iLocRespTimer = CLbsCallbackTimer::NewL(*this);
	iFacResultTimer = CLbsCallbackTimer::NewL(*this);
	iAssistDataTimer = CLbsCallbackTimer::NewL(*this);

	// Create request queue
	iQueue = CStateQueue::NewL(KMaxQueueEntry);
	}


/** Retrieve current MO-Lr state
@return TMoLrState Current MO-LR state
*/
CMoLrStateMachine::TMoLrState CMoLrStateMachine::CurrentState()
	{
	return iCurrentState;
	}

	
/** Start MO-LR request timer
The state handler calls this when it has issued an MO-LR request
to the network and it requires a response before the timer expires.
*/
void CMoLrStateMachine::StartMoLrReqTimer()
	{
	if(iMoLrReqIssuedTimer->IsActive())
		{
		iMoLrReqIssuedTimer->Cancel();
		}
	iMoLrReqIssuedTimer->EventAfter(TTimeIntervalSeconds(KMoLrReqIssuedTimerDurationInSec), KMoLrReqIssuedTimerEvent);
	}

	
/** Start location response timer
The state handler calls this when it has sent a location request
to the client and requires a response before the timer expires.
*/
void CMoLrStateMachine::StartLocRespTimer()
	{
	if(iLocRespTimer->IsActive())
		{
		iLocRespTimer->Cancel();
		}
	iLocRespTimer->EventAfter(TTimeIntervalSeconds(KLocRespTimerDurationInSec), KLocRespTimerEvent);
	}

	
/** Start facility result timer
The state handler calls this when it has sent a location report
to the network and requires a result before the timer expires.
*/
void CMoLrStateMachine::StartFacResultTimer()
	{
	if(iFacResultTimer->IsActive())
		{
		iFacResultTimer->Cancel();
		}
	iFacResultTimer->EventAfter(TTimeIntervalSeconds(KFacResultTimerDurationInSec), KFacResultTimerEvent);
	}


/** Indicates if location request has been sent to LBS.
@return TBool ETrue if location request has been sent to LBS.
*/  
TBool CMoLrStateMachine::IsLocReqSentToLbs() const
	{
	return iIsLocReqSentToLbs;
	}


/** Network Result Location.
This is called by state handlers to retrieve the most
recent reported network result location.
@return TPositionInfoBase Currently held network result position
*/
const TPositionInfoBase& CMoLrStateMachine::NetResultLoc() const
	{
	return iNetResultPosInfo;
	}


/** Store network resulting location
@param aPosInfo Network result location information
*/
void CMoLrStateMachine::StoreNetResultLoc(const TPositionInfoBase& aPosInfo)
	{
	iNetResultPosInfo = reinterpret_cast <const TPositionInfo&> (aPosInfo);
	}


/** Initialise internal state attributes.
This is used when new MO-LR procedure commences.
*/
void CMoLrStateMachine::InitialiseProcedure()
	{
	// Initialise state machine
	InitialiseMachineBase();

	iIsLocReqSentToLbs = EFalse;
	iIsNetResultLocAvailable = EFalse;

	ASSERT(iStateHandler != NULL);
	iStateHandler->Initialise();
	}
	

/** Prepare state transition.
*/  
void CMoLrStateMachine::PreStateTransition()
	{
	}
	

/** State transition.
This method determines the next state to be adopted by the state machine.
*/  
void CMoLrStateMachine::StateTransition()
	{
	if (CancelPending())
		{
		SetMachineAsCancelling();
		iCurrentState = EStateCancelling;
		}
	else
		{
		// Set new state		
		switch (iCurrentState)
			{
			
		case EStateNull:
			iCurrentState = EStateClientReqRecvd;
			break;
			
		case EStateClientReqRecvd:
			iCurrentState = EStateNetSessStarted;
			break;
			
		case EStateNetSessStarted:
			iCurrentState = EStateMeasureDataRecvd;
			break;
			
		case EStateMeasureDataRecvd:
			iCurrentState = EStateNetBasedLocSent;
			break;
			
		case EStateNetBasedLocSent:
			iCurrentState = EStateLocReqByNet;
			iIsMeasureControlHandled = ETrue;
			iIsLocReqSentToLbs = ETrue;
			break;
			
		case EStateLocReqByNet:
			iCurrentState = EStateLocRespRecvd;
			break;
			
		case EStateLocRespRecvd:
			iCurrentState = EStateLocSentToNet;
			break;
			
		case EStateLocSentToNet:
			iCurrentState = EStateNetSessToClose;
			break;
			
		case EStateNetSessToClose:
			// If a network result is provided then it is sent to LBS
			// otherwise the LBS client session closes
			iCurrentState = iIsNetResultLocAvailable ? EStateNetResultSent : EStateClientSessToClose;
			break;
			
		case EStateNetResultSent:
			iCurrentState = EStateClientSessToClose;
			break;

		case EStateClientSessToClose:
			iCurrentState = EStateSessionsClosed;
			break;
			
		case EStateSessionsClosed:
			// Procedure has completed
			CompleteProcedure();
			break;
			
		case EStateCancelling:
			iCurrentState = EStateNetSessToClose;
			break;

		default:
			User::Panic(KProtocolModulePanic, EProtocolModuleMoLrState);
			break;
			};
		}

	}
	

/** Complete the procedure.
*/  
void CMoLrStateMachine::CompleteProcedure()
	{
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
void CMoLrStateMachine::PostStateTransition()
	{
	// Some states are transitory i.e. they require
	// an automatic transition to the next state
	if ( CancelPending() ||
		 (EStateClientReqRecvd		== iCurrentState) ||
	 	 (EStateMeasureDataRecvd	== iCurrentState) ||
	 	 (EStateNetBasedLocSent		== iCurrentState) ||
	 	 (EStateLocRespRecvd		== iCurrentState) ||
	 	 (EStateNetSessToClose		== iCurrentState) ||
	 	 (EStateNetResultSent		== iCurrentState) ||
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
void CMoLrStateMachine::DoQueuedRequest(TInt aRequest)
	{
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
void CMoLrStateMachine::CancelProcedure()
	{
	// Kill all timers
	iMoLrReqIssuedTimer->Cancel();
	iLocRespTimer->Cancel();
	iFacResultTimer->Cancel();
	iAssistDataTimer->Cancel();
	}


/** Timer expired callback.
This is called by a CStateTimer object when the timer
has expired - the event is identified by aEvent parameter.
@param aTimerId The timer event identifier.
*/
void CMoLrStateMachine::OnTimerEventL(TInt aTimerId)
	{
	// Perform relevant action for the expired timer
	switch (aTimerId)
		{

	// MO-LR request timer
	case KMoLrReqIssuedTimerEvent:
		// Inform protocol manager
		Observer().MeasurementControlTimeout();
		break;

	// Additional assistance data timer
	case KAssistDataTimerEvent:
		iLocRespReason = KErrTimedOut;
		// Inform protocol manager
		Observer().MeasurementControlTimeout();
		break;

	// Facility result timer
	case KFacResultTimerEvent:
		CancelMachine(ECancelNetworkTimeout);
		break;

	// Location response timer
	case KLocRespTimerEvent:
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
@see CMoLrStateMachine::OnTimerEventL()
@param aTimerId The timer event identifier.
@param aError Error value.
*/
TInt CMoLrStateMachine::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	return aError;
	}


/** Handle Measurement Control timeout
*/
void CMoLrStateMachine::MeasurementControlTimeout()
	{
	// Ensure assistance data action is performed upon cancellation
	iAssistanceDataActionRequired = ETrue;

	// Cancel procedure
	CancelMachine(ECancelNetworkTimeout);
	}


/** Handle LBS request for MO-LR
@param aSessionId The session ID supplied by LBS.
*/
void CMoLrStateMachine::MoLrReq(const TLbsNetSessionId& aSessionId)
	{
	// Store the supplied ID information
	SessionId(aSessionId);

	// Initialise the new procedure
	InitialiseProcedure();

	// This state machine requires assistance data actions
	// to be performed immediately immediate
	iObserver.DoAssistanceDataActions();

	// Perform a state transition
	PerformStateTransition();
	}


/** Queue an LBS request for MO-LR
@param aSessionId The session ID supplied by LBS.
*/
void CMoLrStateMachine::QueueMoLrReq(const TLbsNetSessionId& aSessionId)
	{
	// Store the session ID
	QueueSessionId(aSessionId);

	CStateMachineBase::SetMachineAsQueued();
	}
	

/** Start previously queued state machine.
*/  
void CMoLrStateMachine::StartQueuedMachine()
	{
	MoLrReq(iQueueSessionId);
	}


/** Handle LBS Location response
@param aReason Location response error reason.
@param aPosInfo The location information response from LBS.
*/
void CMoLrStateMachine::LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo)
	{
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
void CMoLrStateMachine::MeasurementControlInd(const TPositionInfoBase& aPosInfo,
	const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod)
	{
	// Kill timer related to the MO-LR request
	iMoLrReqIssuedTimer->Cancel();

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
This is called by the protocol manager durinq an active MO-LR when
a measurement control error is received from the network.
*/
void CMoLrStateMachine::MeasurementControlErrorInd(TInt aReason)
	{
	TBool cancelRequired = EFalse;
	TBool assistActionRequired = ETrue;

	// If there is no outstanding assistance data request then 
	// inhibit assistance data error responses.
	if (!iMoLrReqIssuedTimer->IsActive() && 
		!iAssistDataTimer->IsActive())
		{
		assistActionRequired = EFalse;
		}

	// Is this the first measurement control received?
	if (!IsLocReqReceived() || (KErrCancel == aReason))
		{
		// Kill timer related to the MO-LR request
		iMoLrReqIssuedTimer->Cancel();
		cancelRequired = ETrue;
		}

	// Perform base class actions
	CStateMachineBase::HandleMeasureControlError(cancelRequired, aReason);

	// Ensure assistance data action only happens if necessary
	iAssistanceDataActionRequired = assistActionRequired;
	}


/** Handle Network Session Result
@param aResult The error result from network session closure.
@param aPosInfo A pointer to final result location information from the network.
	This can be null if there is not final position provided.
*/
void CMoLrStateMachine::SessionResult(TInt aResult, const TPositionInfoBase* aPosInfo)
	{
	// Kill timer related to the facility result
	iFacResultTimer->Cancel();

	iNetSessionResult = aResult;

	// Is a position provided?
	if (aPosInfo)
		{
		// Store network resulting location
		StoreNetResultLoc(*aPosInfo);
		iIsNetResultLocAvailable = ETrue;
		}
	else
		{
		iIsNetResultLocAvailable = EFalse;
		}

	// Is state machine inactive?
	if (!IsActive())
		{
		// Perform a state transition
		PerformStateTransition();
		}
	else
		{
		// Queue request
		iQueue->AddRequest(EQueueNetworkResult);
		}
	}


/** Handle a network error indication.
*/  
void CMoLrStateMachine::NetworkErrorInd()
	{
	// Do we need to report assistance data errors?
	if (!IsLocReqReceived())
		{
		iAssistanceDataActionRequired = ETrue;
		}

	// Perform base class actions
	CStateMachineBase::NetworkErrorInd();
	}

