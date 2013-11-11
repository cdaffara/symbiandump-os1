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
// the X3P protocol state machine.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "cx3pstatemachine.h"
#include "cx3pstatehandler.h"


/** KX3pReqIssuedTimerEvent
Identity of timer for when X3P request issued to network
*/
const TInt KX3pReqIssuedTimerEvent = 0x1000;

/** KX3pReqIssuedTimerDurationInSec
Timer duration for when X3P request issued to network
*/
const TInt KX3pReqIssuedTimerDurationInSec = 20;

/** KLocRespTimerEvent
Identity of timer for when location response expected by network
*/
const TInt KLocRespTimerEvent = 0x2000;

/** KLocRespTimerDurationInSec
Timer duration for when location response expected by network
*/
const TInt KLocRespTimerDurationInSec = 64;

/** KX3pFacResultTimerEvent
Identity of timer for when facility result expected from network
*/
const TInt KX3pFacResultTimerEvent = 0x300;

/** KX3pFacResultTimerDurationInSec
Timer duration for when facility result expected from network
*/
const TInt KX3pFacResultTimerDurationInSec = 10;

/** KMaxQueueEntry
Maximum entries in this state machine request queue.
*/
const TInt KMaxQueueEntry = 5;


/** Static constructor.
@param aObserver Reference to state machine observer.
@return A new instance of the CX3pStateMachine class
*/  
CX3pStateMachine* CX3pStateMachine::NewL(MStateMachineObserver& aObserver)
	{
	CX3pStateMachine* self = new (ELeave) CX3pStateMachine(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
@param aObserver Reference to state machine observer.
*/  
CX3pStateMachine::CX3pStateMachine(MStateMachineObserver& aObserver)
: CStateMachineBase(aObserver)
	{
	iLocReqType = MLbsNetworkProtocolObserver::EServiceTransmitThirdParty;
	}


/** Standard destructor.
*/  
CX3pStateMachine::~CX3pStateMachine()
	{
	delete iLocRespTimer;
	delete iX3pReqIssuedTimer;
	delete iFacResultTimer;
	}


/** Private second-stage constructor.
*/  
void CX3pStateMachine::ConstructL()
	{
	CActiveScheduler::Add(this);

	// Create state handler
	iStateHandler = CX3pStateHandler::NewL(*this);

	// Create timers used during X3P protocol procedure
	iX3pReqIssuedTimer = CLbsCallbackTimer::NewL(*this);
	iLocRespTimer = CLbsCallbackTimer::NewL(*this);
	iFacResultTimer = CLbsCallbackTimer::NewL(*this);
	iAssistDataTimer = CLbsCallbackTimer::NewL(*this);


	// Create request queue
	iQueue = CStateQueue::NewL(KMaxQueueEntry);

	// We are not cancelling
	iStillCancelling = EFalse;
	}


/** Retrieve current X3P state
@return TX3pState Current X3P state
*/
CX3pStateMachine::TX3pState CX3pStateMachine::CurrentState()
	{
	return iCurrentState;
	}

	
/** Start X3P request timer
The state handler calls this when it has issued an X3P request
to the network and it requires a response before the timer expires.
*/
void CX3pStateMachine::StartX3pReqTimer()
	{
	if(iX3pReqIssuedTimer->IsActive())
		{
		iX3pReqIssuedTimer->Cancel();
		}
	iX3pReqIssuedTimer->EventAfter(TTimeIntervalSeconds(KX3pReqIssuedTimerDurationInSec), KX3pReqIssuedTimerEvent);
	}

	
/** Start location response timer
The state handler calls this when it has sent a location request
to the client and requires a response before the timer expires.
*/
void CX3pStateMachine::StartLocRespTimer()
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
void CX3pStateMachine::StartFacResultTimer()
	{
	if(iFacResultTimer->IsActive())
		{
		iFacResultTimer->Cancel();
		}
	iFacResultTimer->EventAfter(TTimeIntervalSeconds(KX3pFacResultTimerDurationInSec), KX3pFacResultTimerEvent);
	}
	
/** Response sent
Remember that a Measurement Response Location was sent to the network
*/
void CX3pStateMachine::ResponseSent()
	{
	iIsLocRespSent = ETrue;
	}

/** Network Result Location.
This is called by state handlers to retrieve the most
recent reported network result location.
@return TPositionInfoBase Currently held network result position
*/
const TPositionInfoBase& CX3pStateMachine::NetResultLoc() const
	{
	return iNetResultPosInfo;
	}


/** Store network resulting location
@param aPosInfo Network result location information
*/
void CX3pStateMachine::StoreNetResultLoc(const TPositionInfoBase& aPosInfo)
	{
	iNetResultPosInfo = reinterpret_cast <const TPositionInfo&> (aPosInfo);
	}

/** Retrieve the transmit destination
@return const TDesC& A reference to the transmit destination.
*/
const TDesC& CX3pStateMachine::Destination() const
	{
	return iDestination;
	}


/** Store the requested transmit destination
@param aDest The destination for this transmit request
*/
void CX3pStateMachine::StoreDestination(const TDesC& aDest)
	{
	// Check size of destination string
	if (KMaxSizeX3pDestination < aDest.Length())
		{
		User::Panic(KProtocolModulePanic, EProtocolModuleX3PDestination);
		}
	// Store destination
	iDestination = aDest;
	}


/** Store the queued transmit destination
@param aDest The destination for a queued transmit request
*/
void CX3pStateMachine::QueueDestination(const TDesC& aDest)
	{
	// Check size of destination string
	if (KMaxSizeX3pDestination < aDest.Length())
		{
		User::Panic(KProtocolModulePanic, EProtocolModuleX3PDestination);
		}
	// Store destination
	iQueueDestination = aDest;
	}


/** Initialise internal state attributes.
This is used when new X3P procedure commences.
*/
void CX3pStateMachine::InitialiseProcedure()
	{
	// Initialise state machine
	InitialiseMachineBase();

	ASSERT(iStateHandler != NULL);
	iStateHandler->Initialise();
	}
	

/** Prepare state transition.
*/  
void CX3pStateMachine::PreStateTransition()
	{
	}
	

/** State transition.
This method determines the next state to be adopted by the state machine.
*/  
void CX3pStateMachine::StateTransition()
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
			iIsMeasureControlHandled = ETrue;
			iCurrentState = EStateLocReqByNet;
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
			User::Panic(KProtocolModulePanic, EProtocolModuleX3pState);
			break;
			};
		}

	}
	

/** Complete the procedure.
*/  
void CX3pStateMachine::CompleteProcedure()
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
void CX3pStateMachine::PostStateTransition()
	{
	// Some states are transitory i.e. they require
	// an automatic transition to the next state
	if (CancelPending() ||
		(EStateClientReqRecvd == iCurrentState) ||
		(EStateMeasureDataRecvd == iCurrentState) ||
		(EStateNetBasedLocSent == iCurrentState) ||
		(EStateLocRespRecvd == iCurrentState) ||
		((EStateLocSentToNet == iCurrentState) && (LocRespReason() < KErrNone)) ||
		(EStateNetSessToClose == iCurrentState) ||
		(EStateNetResultSent == iCurrentState) ||
		(EStateClientSessToClose == iCurrentState) ||
		(EStateSessionsClosed == iCurrentState) ||
		(EStateCancelling == iCurrentState)
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
void CX3pStateMachine::DoQueuedRequest(TInt aRequest)
	{
	switch (aRequest)
		{
	//case EQueueLocResponse:
		// Perform a state transition
		//PerformStateTransition();
		//break;
	case EQueueLocResponse:
	case EQueueMeasurementControl:	
	case EQueueNetworkResult:
		// Only do state transition if not just cancelling.
		if (!iStillCancelling)
			{
			PerformStateTransition();
			iStillCancelling = EFalse;
			}
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleQueueRequestId);
		break;
		};

	}


/** Cancel the active procedure
*/  
void CX3pStateMachine::CancelProcedure()
	{
	// Kill all timers
	iX3pReqIssuedTimer->Cancel();
	iLocRespTimer->Cancel();
	iAssistDataTimer->Cancel();
	iFacResultTimer->Cancel();
	}

/** Set the cancelling flag which stops certain queued transitions occurring
*/
void CX3pStateMachine::SetStillCancelling(TBool aValue)
	{
	iStillCancelling = aValue;
	}

/** Timer expired callback.
This is called by a CStateTimer object when the timer
has expired - the event is identified by aEvent parameter.
@param aTimerId The timer event identifier.
*/
void CX3pStateMachine::OnTimerEventL(TInt aTimerId)
	{
	// Perform relevant action for the expired timer
	switch (aTimerId)
		{

	// X3P request timer
	case KX3pReqIssuedTimerEvent:
		// Inform protocol manager
		Observer().MeasurementControlTimeout();
		break;

	// Additional assistance data timer
	case KAssistDataTimerEvent:
		// Ensure assistance data action is performed upon cancellation
		iAssistanceDataActionRequired = ETrue;
		iLocRespReason = KErrTimedOut;
		// Inform protocol manager
		Observer().MeasurementControlTimeout();
		break;

	// Location response timer
	case KLocRespTimerEvent:
		iLocRespReason = KErrTimedOut;
		CancelMachine(ECancelClientTimeout);
		break;
		
	// Facility result timer
	case KX3pFacResultTimerEvent:
		CancelMachine(ECancelNetworkTimeout);
		break;
		
	// Ignore unknown timer events
	default:
		break;

		};

	}


/** Timer callback error handler.
This is called if the timer expiry callback leaves.
@see CX3pStateMachine::OnTimerEventL()
@param aTimerId The timer event identifier.
@param aError Error value.
*/
TInt CX3pStateMachine::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	return aError;
	}


/** Handle Measurement Control timeout
*/
void CX3pStateMachine::MeasurementControlTimeout()
	{
	// Cancel procedure
	CancelMachine(ECancelNetworkTimeout, KErrTimedOut);
	}


/** Handle LBS request for X3P
@param aDest The destination for this transmit request
*/
void CX3pStateMachine::X3pReq(const TLbsNetSessionId& aNewSessionId, const TDesC& aDest)
	{
	// Store the supplied ID information
	SessionId(aNewSessionId);

	// Initialise the new procedure
	InitialiseProcedure();

	// Store destination
	StoreDestination(aDest);

	// Perform a state transition
	PerformStateTransition();
	}


/** Queue a LBS request for X3P
@param aDest The destination for this transmit request
*/
void CX3pStateMachine::QueueX3pReq(const TLbsNetSessionId& aNewSessionId, const TDesC& aDest)
	{
	// Store the session ID
	QueueSessionId(aNewSessionId);

	// Store destination
	QueueDestination(aDest);

	CStateMachineBase::SetMachineAsQueued();
	}
	

/** Handle Network Session Result
@param aResult The error result from network session closure.
@param aPosInfo A pointer to final result location information from the network.
	This can be null if there is not final position provided.
*/
void CX3pStateMachine::SessionResult(TInt aResult, const TPositionInfoBase* aPosInfo)
	{
	// Kill timer related to the facility result
	iFacResultTimer->Cancel();

	iNetSessionResult = aResult; // does this result say anything about the validity of the position?

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

/** Start previously queued state machine.
*/  
void CX3pStateMachine::StartQueuedMachine()
	{
	X3pReq(iQueueSessionId, iQueueDestination);
	}


/** Handle LBS Location response
@param aReason Location response error reason.
@param aPosInfo The location response from LBS.
*/
void CX3pStateMachine::LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo)
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

@param aPosInfo Reference location provided in the measuerment control
@param aQuality Location request quality specified in the measuerment control
*/
void CX3pStateMachine::MeasurementControlInd(const TPositionInfoBase& aPosInfo,
	const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod)
	{
	// Kill timer related to the X3P request
	iX3pReqIssuedTimer->Cancel();

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
This is called by the protocol manager durinq an active X3P when
a measurement control error is received from the network.
*/
void CX3pStateMachine::MeasurementControlErrorInd(TInt aReason)
	{
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
		// Kill timer related to the X3P request
		iX3pReqIssuedTimer->Cancel();
		cancelRequired = ETrue;
		}

	// Perform base class actions
	CStateMachineBase::HandleMeasureControlError(cancelRequired, aReason);

	// Ensure assistance data action only happens if necessary
	iAssistanceDataActionRequired = assistActionRequired;
	}


/** Handle a network error indication.
*/  
void CX3pStateMachine::NetworkErrorInd()
	{
	// Do we need to report assistance data errors?
	if (!IsLocReqReceived())
		{
		iAssistanceDataActionRequired = ETrue;
		}

	// Perform base class actions
	CStateMachineBase::NetworkErrorInd();
	}

/** Client close reason.
This is called by state handlers to determine what reason to
provide to the client for the client session closing.
@return TInt Reason value.
*/
TInt CX3pStateMachine::ClientCloseReason()  const
	{
	TInt reason = KErrNone;

	if (KErrPositionHighPriorityReceive == iClientCancelReason)
		{
		reason = iClientCancelReason;
		}
	else if (KErrNone == iNetworkCancelReason)
		{
		switch (iCancelSource)
			{
		case ECancelNetworkCancel:
			reason = KErrCancel;
			break;

		case ECancelNetworkError:
			reason = KErrDisconnected;
			break;

		case ECancelClientTimeout:
		case ECancelNetworkTimeout:
			reason = KErrTimedOut;
			break;

		case ECancelClientCancel:
			{
			reason = iClientCancelReason;
			break;
			}
			
		case ECancelNone:
			reason = LocRespReason();
			break;

		case ECancelClientCancelSilent:
		case ECancelClosing:
			break;

		default:
			User::Panic(KProtocolModulePanic, EProtocolModuleCancelSource);
			break;
			};
		}
	else
		{
		reason = iNetworkCancelReason;
		}

	return reason;
	}

