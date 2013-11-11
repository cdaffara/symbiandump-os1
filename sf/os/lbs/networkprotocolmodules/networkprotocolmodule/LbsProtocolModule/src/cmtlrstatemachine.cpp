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
// the MT-LR protocol state machine.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "cmtlrstatemachine.h"
#include "cmtlrstatehandler.h"
#include <e32property.h>
#include <lbs/test/lbsnetsimstatus.h>

/** KMtLrReqIssuedTimerEvent
Identity of timer for when MT-LR request issued by network
*/
const TInt KMtLrReqIssuedTimerEvent = 0x10;

/** KMtLrReqIssuedTimerDurationInSec
Timer duration for when MT-LR request issued by network
*/
const TInt KMtLrReqIssuedTimerDurationInSec = 30;

/** KMeasureControlTimerEvent
Identity of timer for when measurement control expected by LBS
*/
const TInt KMeasureControlTimerEvent = 0x20;

/** KMeasureControlTimerDurationInSec
Timer duration for when measurement control expected by LBS
*/
const TInt KMeasureControlTimerDurationInSec = 20;

/** KLocRespTimerEvent
Identity of timer for when location response expected by network
*/
const TInt KLocRespTimerEvent = 0x30;

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
@return A new instance of the CMtLrStateMachine class
*/  
CMtLrStateMachine* CMtLrStateMachine::NewL(MStateMachineObserver& aObserver)
	{
	CMtLrStateMachine* self = new (ELeave) CMtLrStateMachine(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
@param aObserver Reference to state machine observer.
*/  
CMtLrStateMachine::CMtLrStateMachine(MStateMachineObserver& aObserver)
: CStateMachineBase(aObserver)
	{
	}


/** Standard destructor.
*/  
CMtLrStateMachine::~CMtLrStateMachine()
	{
	delete iLocRespTimer;
	delete iMeasureControlTimer;
	delete iMtLrReqIssuedTimer;
	}


/** Private second-stage constructor.
*/  
void CMtLrStateMachine::ConstructL()
	{
	CActiveScheduler::Add(this);

	// Create state handler
	iStateHandler = CMtLrStateHandler::NewL(*this);

	// Create timers used during MT-LR protocol procedure
	iMtLrReqIssuedTimer = CLbsCallbackTimer::NewL(*this);
	iMeasureControlTimer = CLbsCallbackTimer::NewL(*this);
	iLocRespTimer = CLbsCallbackTimer::NewL(*this);
	iAssistDataTimer = CLbsCallbackTimer::NewL(*this);

	// Create request queue
	iQueue = CStateQueue::NewL(KMaxQueueEntry);
	}


/** Retrieve current MT-LR State
@return TMtLrState Current MT-LR state
*/
CMtLrStateMachine::TMtLrState CMtLrStateMachine::CurrentState()
	{
	return iCurrentState;
	}

	
/** Start MT-LR request timer
This is called when an MT-LR request is received from the network
and it requires a response before the timer expires.
*/
void CMtLrStateMachine::StartMtLrReqTimer()
	{
	if(iMtLrReqIssuedTimer->IsActive())
		{
		iMtLrReqIssuedTimer->Cancel();
		}
	iMtLrReqIssuedTimer->EventAfter(TTimeIntervalSeconds(KMtLrReqIssuedTimerDurationInSec), KMtLrReqIssuedTimerEvent);
	}

	
/** Start Measure Control timer
This is called when Measurement Control is expected by LBS.
*/
void CMtLrStateMachine::StartMeasureControlTimer()
	{
	if(iMeasureControlTimer->IsActive())
		{
		iMeasureControlTimer->Cancel();
		}
	iMeasureControlTimer->EventAfter(TTimeIntervalSeconds(KMeasureControlTimerDurationInSec), KMeasureControlTimerEvent);
	}

	
/** Start location response timer
The state handler calls this when it has sent a location request
to the client and requires a response before the timer expires.
*/
void CMtLrStateMachine::StartLocRespTimer()
	{
	if(iLocRespTimer->IsActive())
		{
		iLocRespTimer->Cancel();
		}
	iLocRespTimer->EventAfter(TTimeIntervalSeconds(KLocRespTimerDurationInSec), KLocRespTimerEvent);
	}


/** Privacy requestor information
This is called by state handlers to retrieve the most
recent privacy request info.
@return TLbsExternalRequestInfo Currently held privacy requestor information
*/
const TLbsExternalRequestInfo& CMtLrStateMachine::PrivacyReqInfo() const
	{
	return iPrivacyReqInfo;
	}


/** Privacy request
This is called by state handlers to retrieve the most
recent privacy request.
@return const TLbsNetPosRequestPrivacy& Currently held privacy request
*/
const TLbsNetPosRequestPrivacy& CMtLrStateMachine::PrivacyReq() const
	{
	return iPrivacyRequest;
	}


/** Privacy response value
This is called by state handlers to retrieve the most
recent privacy response.
@return TLbsPrivacyResponse Currently held privacy response value
*/
const CLbsNetworkProtocolBase::TLbsPrivacyResponse& CMtLrStateMachine::PrivacyResponseValue() const
	{
	return iPrivacyResp;
	}


/** Has privacy response been handled.
This is called to determine if privacy response handled.
@return TBool ETrue if privacy response handled
*/
TBool CMtLrStateMachine::IsPrivacyResponseHandled() const
	{
	return iPrivacyResponseHandled;
	}


/** Has privacy response been handled.
This is called to determine if privacy response handled.
@return TBool ETrue if privacy response handled
*/
TBool CMtLrStateMachine::IsRejectedWaitingMeasureControl() const
	{
	return (EStateClientSessToCloseWaitMeasure == iCurrentState);
	}


/** Is this an emergency situation.
This is called to determine if there is an emergency situation for which
an MT-LR is being requested.
@return TBool ETrue if emergency situation exists
*/
TBool CMtLrStateMachine::IsEmergency() const
	{
#pragma message("Emergency timers not implemented.")
	// Retrieve current value of emergency property published by NetSim
	TBool stateValue = EFalse;
	RProperty::Get(TUid::Uid(KLbsNetSimStatus), KLbsEmergencyStatus, stateValue);
	return stateValue;
	}


/** Initialise internal state attributes.
This is used when new MT-LR procedure commences.
*/
void CMtLrStateMachine::InitialiseProcedure()
	{
	// Initialise state machine
	InitialiseMachineBase();

	iPrivacyResponseRecvd = EFalse;
	iPrivacyResponseHandled = EFalse;

	ASSERT(iStateHandler != NULL);
	iStateHandler->Initialise();
	}
	

/** Procedure is done.
*/  
void CMtLrStateMachine::ProcedureDone()
	{
	// If a location response error has been reported then forward it to network
	if (KErrNone != LocRespReason())
		{
		CNetworkInterface::TNetworkError netError = 
						Observer().Network()->LocationResp(LocRespReason());
		// Handle network-related error
		if (CNetworkInterface::ENone != netError)
			{
			Observer().NetworkErrorReported();
			}
		}
	// Procedure is complete
	CompleteProcedure();
	}
	

/** Prepare state transition.
*/  
void CMtLrStateMachine::PreStateTransition()
	{
	}
	

/** State transition.
This method determines the next state to be adopted by the state machine.
*/
void CMtLrStateMachine::StateTransition()
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
			iCurrentState = EStateNetReqRecvd;
			break;
			
		case EStateNetReqRecvd:
			iCurrentState = EStatePrivacySessStarted;
			iIsPrivacyRespBeforeMeasureControl = EFalse;
			break;
			
		case EStatePrivacySessStarted:
			// Location request received?
			if (iIsLocReqReceived)
				{
				iCurrentState = EStateMeasureDataRecvd;
				}
			// Privacy response received?
			else if (iPrivacyResponseRecvd)
				{
				iIsPrivacyRespBeforeMeasureControl = ETrue;
				iPrivacyResponseHandled = ETrue;
				iCurrentState = EStatePrivacyRespRecvd;

				// Start timer for when the measurement control is expected
				StartMeasureControlTimer();
				}
			break;
			
		case EStatePrivacyRespRecvd:
			// Privacy not accepted?
			if (CLbsNetworkProtocolBase::EPrivacyResponseAccepted != iPrivacyResp)
				{
				iCurrentState = EStatePrivacyReject;
				}
			else
				{
				iCurrentState = EStateMeasureDataRecvd;
				}
			break;
			
		case EStatePrivacyRespRecvdAfterMeasure:
			// Privacy not accepted?
			if (CLbsNetworkProtocolBase::EPrivacyResponseAccepted != iPrivacyResp)
				{
				iCurrentState = EStatePrivacyRejectAfterMeasure;
				}
			else
				{
				iCurrentState = EStateLocRespRecvd;
				}
			break;
			
		case EStatePrivacyReject:
			iCurrentState = EStateClientSessToCloseWaitMeasure;
			break;
			
		case EStatePrivacyRejectAfterMeasure:
			iCurrentState = EStateLocRespRecvd;
			break;
			
		case EStateMeasureDataRecvd:
			iCurrentState = EStateNetBasedLocSent;
			break;
			
		case EStateNetBasedLocSent:
			iIsMeasureControlHandled = ETrue;
			// Was privacy already rejected
			if (iIsPrivacyRespBeforeMeasureControl &&
				CLbsNetworkProtocolBase::EPrivacyResponseAccepted != iPrivacyResp)
				{
				ProcedureDone();
				}
			else
				{
				iCurrentState = EStateLocReqByNet;
				}
			break;
			
		case EStateLocReqByNet:
			iCurrentState = iIsPrivacyRespBeforeMeasureControl ? 
							EStateLocRespRecvd : EStatePrivacyRespRecvdAfterMeasure;
			break;
			
		case EStateLocRespRecvd:
			iCurrentState = EStateLocSentToNet;
			break;
			
		case EStateLocSentToNet:
			iCurrentState = EStateClientSessToCloseAfterMeasure;
			break;
			
		case EStateClientSessToCloseWaitMeasure:
			iCurrentState = EStateMeasureDataRecvd;
			break;
			
		case EStateClientSessToCloseWaitLocResp:
			// We are waiting for location response from LBS after the
			// preceding session closes. Check to see if we have timeout
			// otherwise we must have received the response.
			if (Cancelling() && (KErrTimedOut == iLocRespReason))
				{
				iCurrentState = EStateCancelling;
				}
			else
				{
				ProcedureDone();
				}
			break;
			
		case EStateClientSessToCloseAfterMeasure:
			ProcedureDone();
			break;
			
		case EStateSessionsClosed:
			// Procedure has completed
			CompleteProcedure();
			break;

		case EStateCancelling:
			switch (iCancelSource)
				{
			case ECancelNone:
			case ECancelClosing:
			case ECancelClientCancel:
			case ECancelClientCancelSilent:
				CompleteProcedure();
				break;

			case ECancelNetworkError:
				iCurrentState = EStateClientSessToCloseAfterMeasure;
				break;

			case ECancelNetworkTimeout:
				// We were waiting for measurement control.
				// Was privacy rejected?
				if (CLbsNetworkProtocolBase::EPrivacyResponseAccepted != iPrivacyResp)
					{
					ProcedureDone();
					}
				else
					{
					iCurrentState = EStateClientSessToCloseAfterMeasure;
					}
				break;

			case ECancelClientTimeout:
				// We were either waiting for a location response or a privacy response
				// Privacy response will timeout first, but we may need to still wait for
				// a location response.
				if (!iPrivacyResponseRecvd && iIsMeasureControlHandled && 
					(KErrTimedOut != iLocRespReason))
					{
					iCurrentState = EStateClientSessToCloseWaitLocResp;
					}
				else
					{
					iCurrentState = EStateClientSessToCloseAfterMeasure;
					}
				break;

			case ECancelNetworkCancel:
				iCurrentState = EStateClientSessToCloseAfterMeasure;
				break;

			default:
				User::Panic(KProtocolModulePanic, EProtocolModuleCancelSource);
				break;
				};
			break;

		default:
			User::Panic(KProtocolModulePanic, EProtocolModuleMtLrState);
			break;
			};
		}

	}


/** Complete the procedure.
*/  
void CMtLrStateMachine::CompleteProcedure()
	{
	iCurrentState = EStateNull;
	// Complete state machine
	CompleteMachineBase();
	}


/** Complete state transition.
This is called by the base class when a state transition has
concluded and it provides an opportunity for the state machine 
to perform actions required immediately after this transition.

The method can also initiate a further change of state. This is
relevant when the state machine is required to perform an autonomous
transition from one state to another e.g. this occurs when several
interactions are required arising from a single external trigger.
*/  
void CMtLrStateMachine::PostStateTransition()
	{
	// Some states are transitory i.e. they require
	// an automatic transition to the next state
	if ( CancelPending() ||
		 (EStateNetReqRecvd == iCurrentState) ||
	 	 ((CLbsNetworkProtocolBase::EPrivacyResponseAccepted != PrivacyResponseValue()) &&
		  ((EStatePrivacyRespRecvd == iCurrentState) || (EStatePrivacyRespRecvdAfterMeasure == iCurrentState))) ||
	 	 (EStatePrivacyReject == iCurrentState) ||
	 	 (EStateMeasureDataRecvd == iCurrentState) ||
	 	 (EStateNetBasedLocSent == iCurrentState) ||
	 	 (EStateLocRespRecvd == iCurrentState) ||
	 	 (EStateLocSentToNet == iCurrentState) ||
	 	 (EStateClientSessToCloseAfterMeasure == iCurrentState) ||
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
void CMtLrStateMachine::DoQueuedRequest(TInt aRequest)
	{
	switch (aRequest)
		{
	case EQueuePrivacyResponse:
	case EQueueLocResponse:
	case EQueueMeasurementControl:
		// Perform a state transition
		PerformStateTransition();
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleQueueRequestId);
		break;
		};

	}


/** Cancel active procedure
*/  
void CMtLrStateMachine::CancelProcedure()
	{
	// Kill all timers
	iMtLrReqIssuedTimer->Cancel();
	iMeasureControlTimer->Cancel();
	iLocRespTimer->Cancel();
	iAssistDataTimer->Cancel();
	}


/** Timer expired callback.
This is called by a CStateTimer object when the timer
has expired - the event is identified by aEvent parameter.
@param aTimerId The timer event identifier.
*/
void CMtLrStateMachine::OnTimerEventL(TInt aTimerId)
	{
	// Perform relevant action for the expired timer
	switch (aTimerId)
		{

	// MT-LR request timer
	case KMtLrReqIssuedTimerEvent:
		iPrivacyResp = CLbsNetworkProtocolBase::EPrivacyResponseUnknown;
		CancelMachine(ECancelClientTimeout);
		break;

	// Measurement control timer
	case KMeasureControlTimerEvent:
		// Inform protocol manager
		Observer().MeasurementControlTimeout();
		break;

	// Additional assistance data timer
	case KAssistDataTimerEvent:
		// Ensure assistance data action is performed upon cancellation
		iAssistanceDataActionRequired = ETrue;
		// Inform protocol manager
		Observer().MeasurementControlTimeout();
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
@see CMtLrStateMachine::OnTimerEventL()
@param aTimerId The timer event identifier.
@param aError Error value.
*/
TInt CMtLrStateMachine::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	return aError;
	}


/** Handle Measurement Control timeout
*/
void CMtLrStateMachine::MeasurementControlTimeout()
	{
	// Privacy not rejected?
	if (CLbsNetworkProtocolBase::EPrivacyResponseRejected != iPrivacyResp)
		{
		iLocRespReason = KErrTimedOut;
		CancelMachine(ECancelNetworkTimeout);
		}
	else
		{
		CancelMachine(ECancelNone);
		}
	}


/** Handle LBS Location response
@param aReason Location response error reason.
@param aPosInfo The location information response from LBS.
*/
void CMtLrStateMachine::LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo)
	{
	// Store location response data
	StoreLocationResp(aReason, aPosInfo);

	// Kill the timer related to location response expected by network
	iLocRespTimer->Cancel();

	// Perform a state transition
	if (!IsActive())
		{
		PerformStateTransition();
		}
	else
		{
		iQueue->AddRequest(EQueueLocResponse);
		}
	}


/** Handle LBS Privacy response
@param aResponse Privacy response value
*/
void CMtLrStateMachine::PrivacyResp(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse)
	{
	// Kill the timer related to the privacy request
	iMtLrReqIssuedTimer->Cancel();

	// Store privacy response data
	StorePrivacyResp(aResponse);

	// Perform a state transition
	if (!IsActive())
		{
		PerformStateTransition();
		}
	else
		{
		iQueue->AddRequest(EQueuePrivacyResponse);
		}
	}


/** Handle Network MT-LR request
@param aReqInfo External requestor information
@param aPrivacy Privacy request information
*/
void CMtLrStateMachine::MtLrReq(const TLbsExternalRequestInfo& aReqInfo,
		const TLbsNetPosRequestPrivacy& aPrivacy)
	{
	// This requires a new session to be started
	SessionId(iObserver.NewSessionId());

	// Associate location requests with MT-LR
	iLocReqType = MLbsNetworkProtocolObserver::EServiceMobileTerminated;

	// Initialise the new procedure
	InitialiseProcedure();

	// Start corresponding timer
	StartMtLrReqTimer();

	// Store privacy request data
	StorePrivacyReq(aReqInfo, aPrivacy);

	// Perform a state transition
	PerformStateTransition();
	}


/** Queue a Network MT-LR request
@param aReqInfo External requestor information
@param aPrivacy Privacy request
*/
void CMtLrStateMachine::QueueMtLrReq(const TLbsExternalRequestInfo& aReqInfo,
		const TLbsNetPosRequestPrivacy& aPrivacy)
	{
	// Store privacy request data
	StorePrivacyReq(aReqInfo, aPrivacy);

	CStateMachineBase::SetMachineAsQueued();
	}
	

/** Start previously queued state machine.
*/  
void CMtLrStateMachine::StartQueuedMachine()
	{
	MtLrReq(iPrivacyReqInfo, iPrivacyRequest);
	}


/** Handle Network MT-LR cancel indication
@param aReason The reason for cancelling.
*/
void CMtLrStateMachine::MtLrCancelInd(TInt aReason)
	{
	// Store location cancel reason
	StoreNetLocCancelReason(aReason);

	// Cancel procedure
	CancelMachine(ECancelNetworkCancel);
	}


/** Handle Network Measurement Control indication
A measurement control indication has been received from the network.
Note: At this stage the assistance data has already been stored by the
protocol manager and is thus not passed into this method.

@param aPosInfo Reference location information provided in the measurement control
@param aQuality Location request quality specified in the measuerment control
*/
void CMtLrStateMachine::MeasurementControlInd(const TPositionInfoBase& aPosInfo,
			const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod)
	{
	// A network induced location request may arise when the MT-LR state machine
	// is not active. To handle this we fake a privacy response and proceed with 
	// the remainder of MT-LR state handling.
	if (CStateMachineBase::EStateReady == State())
		{
		// Associate location requests with NI-LR
		iLocReqType = MLbsNetworkProtocolObserver::EServiceNetworkInduced;

		// Initialise the protocol procedure
		InitialiseProcedure();

		// Fake a privacy response
		iPrivacyResponseRecvd = ETrue;
		iPrivacyResponseHandled = ETrue;
		iIsPrivacyRespBeforeMeasureControl = ETrue;
		iPrivacyResp = CLbsNetworkProtocolBase::EPrivacyResponseAccepted;
		iCurrentState = EStatePrivacyRespRecvd;
		}

	// Kill the timer for the receipt of Measurement Control
	iMeasureControlTimer->Cancel();

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
This is called by the protocol manager durinq an active MT-LR when
a measurement control error is received from the network.
*/
void CMtLrStateMachine::MeasurementControlErrorInd(TInt aReason)
	{
	TBool cancelRequired = EFalse;
	TBool assistActionRequired = ETrue;

	// If there is no outstanding assistance data request then 
	// inhibit assistance data error responses.
	if (!iAssistDataTimer->IsActive())
		{
		assistActionRequired = EFalse;
		}

	// Is cancel required?
	if (KErrCancel == aReason)
		{
		cancelRequired = ETrue;
		}

	// Kill timer related to the MT-LR request
	iMtLrReqIssuedTimer->Cancel();

	// Perform base class actions
	CStateMachineBase::HandleMeasureControlError(cancelRequired, aReason);

	// Ensure assistance data action only happens if necessary
	iAssistanceDataActionRequired = assistActionRequired;
	}


/** Store Privacy request
@param aReqInfo External requestor information
@param aPrivacy Privacy request
*/
void CMtLrStateMachine::StorePrivacyReq(const TLbsExternalRequestInfo& aReqInfo,
				const TLbsNetPosRequestPrivacy& aPrivacy)
	{
	iPrivacyReqInfo = aReqInfo;
	// For emergencies always override the privacy request
	if (IsEmergency())
		{
		iPrivacyRequest.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);
		iPrivacyRequest.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
		}
	else
		{
		iPrivacyRequest = aPrivacy;
		}
	}


/** Store Privacy response
@param aPrivacyResp Privacy response value
*/
void CMtLrStateMachine::StorePrivacyResp(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aPrivacyResp)
	{
	iPrivacyResponseRecvd = ETrue;
	iPrivacyResponseHandled = EFalse;
	iPrivacyResp = aPrivacyResp;
	if (CLbsNetworkProtocolBase::EPrivacyResponseAccepted != PrivacyResponseValue())
		{
		iLocRespReason = KErrPermissionDenied;
		}
	}


/** Handle a network error indication.
*/  
void CMtLrStateMachine::NetworkErrorInd()
	{
	// Perform base class actions
	CStateMachineBase::NetworkErrorInd();
	}

