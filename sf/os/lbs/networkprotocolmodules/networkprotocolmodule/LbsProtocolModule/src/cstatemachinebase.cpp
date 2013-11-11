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
// This file provides the implementation of the base class for
// protocol state machines used in the Test Protocol Module.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "cstatemachinebase.h"
#include "cstatehandlerbase.h"
#include "lbsdevloggermacros.h"

//-----------------------------------------------------------------------------
// State Machine Base Class
//-----------------------------------------------------------------------------

/** Standard constructor.
@param aObserver A reference to the state machine observer. 
*/  
CStateMachineBase::CStateMachineBase(MStateMachineObserver& aObserver)
: CActive(EPriorityLow), iObserver(aObserver), iProtocolState(EStateReady)
	{
	ResetSessionId();
	}


/** Standard destructor.
*/  
CStateMachineBase::~CStateMachineBase()
	{
	iCancelSource = ECancelClosing;
	Cancel();
	delete iAssistDataTimer;
	delete iQueue;
	delete iStateHandler;
	}


/** Retrieve reference to state machine observer.
@return MStateMachineObserver A reference to the observer. 
*/  
MStateMachineObserver& CStateMachineBase::Observer()
	{
	return iObserver;
	}

/** Set session ID
@param aSessionId Identifier to be used by the state machine whenever
	the session ID is to be used.
*/
void CStateMachineBase::SessionId(const TLbsNetSessionId& aSessionId)
	{
	iSessionId = aSessionId;
	}

/** Reset session ID
*/
void CStateMachineBase::ResetSessionId()
	{
	iSessionId.SetSessionOwner(TUid::Uid(0));
	iSessionId.SetSessionNum(0);
	}

/** Get session ID
@return const TLbsNetSessionId& The session ID currently used by the state machine.
*/
const TLbsNetSessionId& CStateMachineBase::SessionId() const
	{
	return iSessionId;
	}

/** Set ID for queued session
@param aSessionId Identifier to be used for a queued session.
*/
void CStateMachineBase::QueueSessionId(const TLbsNetSessionId& aSessionId)
	{
	iQueueSessionId = aSessionId;
	}

/** Get ID of queued session
@return const TLbsNetSessionId& The session ID of the queued session
*/
const TLbsNetSessionId& CStateMachineBase::QueueSessionId() const
	{
	return iQueueSessionId;
	}


/** Retrieve current general protocol state.
@return CStateMachineBase::TProtocolState The current general 
	protocol state for this state machine.

@see CStateMachineBase::TProtocolState
*/  
CStateMachineBase::TProtocolState CStateMachineBase::State() const
	{
	return iProtocolState;
	}

	
/** Start additional assistance data timer.
The state machine calls this when it has issued a request for additional
assistance data from the network.
*/
void CStateMachineBase::StartAssistDataTimer()
	{
	if (iAssistDataTimer->IsActive())
		{
		iAssistDataTimer->Cancel();
		}
	iAssistDataTimer->EventAfter(TTimeIntervalSeconds(KAssistDataTimerDurationInSec), KAssistDataTimerEvent);
	}


/** Indicates if location request has been received.
@return TBool ETrue if location request has been received.
*/  
TBool CStateMachineBase::IsLocReqReceived() const
	{
	return iIsLocReqReceived;
	}
	
/** Indicates if location response has been sent to network.
@return TBool ETrue if location response has been sent.
*/ 
TBool CStateMachineBase::IsLocRespSent() const
	{
	return iIsLocRespSent;
	}

/** Indicates if Measurement Control has been handled.
@return TBool ETrue if Measurement Control has been handled.
*/  
TBool CStateMachineBase::IsMeasureControlHandled() const
	{
	return iIsMeasureControlHandled;
	}


/** Indicates if assistance data action is required.
@return TBool ETrue if assistance data action is required.
*/  
TBool CStateMachineBase::IsAssistanceDataActionReq() const
	{
	return iAssistanceDataActionRequired;
	}


/** Client close reason.
This is called by state handlers to determine what reason to
provide to the client for the client session closing.
@return TInt Reason value.
*/
TInt CStateMachineBase::ClientCloseReason()  const
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

		case ECancelNone:
			{
			// The reson returned from here is used for teht Session Complete Message from the 
			// Network Gateway to the NRH, so use the value which was received with the location
			// response
			reason = LocRespReason();
			if(reason == KErrCancel || reason == KErrPermissionDenied)
				{
				reason = KErrNone;
				}
			}
			break;
 		
		case ECancelClientCancel:
			reason = iClientCancelReason;
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


/** Network close reason.
This is called by state handlers to determine what reason to
provide to the network for the network session closing.
@return TInt Reason value.
*/
TInt CStateMachineBase::NetworkCloseReason() const
	{
	TInt reason = KErrNone;

	if (KErrPositionHighPriorityReceive == iNetworkCancelReason)
		{
		reason = iNetworkCancelReason;
		}
	else if (KErrNone == iClientCancelReason)
		{
		switch (iCancelSource)
			{
		case ECancelClientCancel:
		case ECancelClientCancelSilent:
			reason = KErrCancel;
			break;

		case ECancelClientTimeout:
		case ECancelNetworkTimeout:
			reason = KErrTimedOut;
			break;

		case ECancelNone:
		case ECancelNetworkCancel:
		case ECancelNetworkError:
		case ECancelClosing:
			break;

		default:
			User::Panic(KProtocolModulePanic, EProtocolModuleCancelSource);
			break;
			};
		}
	else
		{
		reason = iClientCancelReason;
		}

	return reason;
	}


/** Silent cancellation of LBS client is taking place.
This is called by state handlers to determine if there is a silent cancellation
of the LBS client session taking place, which means that LBS does not receive
any cancel-related messages for this session.
@return TBool A value of ETrue indicates silent client cancellation is taking place.
*/
TBool CStateMachineBase::IsSilentClientCancel() const
	{
	return (ECancelClientCancelSilent == iCancelSource);
	}


/** Network connection error indication.
This is called by state handlers to determine if a network connection error
has been experienced. In this situation the state handlers should not attempt
to send responses to the network.
@return TBool A value of ETrue indicates a network connection error exists.
*/
TBool CStateMachineBase::IsNetworkConnectionError() const
	{
	return ((EStateCancelling == iProtocolState) && (ECancelNetworkError == iCancelSource));
	}


/** Location request type.
This is called by state handlers to determine what type of location
request is to be specified to LBS. This is usually fixed for a given
state machine, but there is scope to support more than type of request
within a single state machine.
@return MLbsNetworkProtocolObserver::TLbsNetProtocolService request type.
*/
MLbsNetworkProtocolObserver::TLbsNetProtocolService CStateMachineBase::LocReqType() const
	{
	return iLocReqType;
	}


/** Location response reason.
This is called by state handlers to determine what reason to
provide to the network for the location response failure.
@return TInt Reason value.
*/
TInt CStateMachineBase::LocRespReason() const
	{
	TInt reason = KErrNone;
	// Was a Location response error provided?
	if (KErrNone != iLocRespReason)
		{
		reason = iLocRespReason;
		}
	// Are we cancelling?
	else if (Cancelling())
		{
		switch (iCancelSource)
			{
		case ECancelClientCancel:
		case ECancelClientCancelSilent:
			if (KErrPositionHighPriorityReceive == iClientCancelReason)
				{
				reason = iClientCancelReason;
				}
			else
				{
				reason = KErrCancel;
				}
			break;

		case ECancelClientTimeout:
			reason = KErrGeneral;
			break;

		case ECancelNone:
		case ECancelNetworkError:
		case ECancelNetworkTimeout:
		case ECancelNetworkCancel:
		case ECancelClosing:
			break;

		default:
			User::Panic(KProtocolModulePanic, EProtocolModuleCancelSource);
			break;

			};
		}
	return reason;
	}


/** Location response position.
This is called by state handlers to retrieve the most
recent reported position.
@return TPositionInfoBase Currently held location response position
*/
const TPositionInfoBase& CStateMachineBase::LocRespPosition() const
	{
	return iLocRespPosInfo;
	}


/** Reference Location.
This is called by state handlers to retrieve the most
recent reported reference location.
@return TPositionInfoBase Currently held reference position
*/
const TPositionInfoBase& CStateMachineBase::ReferenceLoc() const
	{
	return iReferenceLoc;
	}


/** Location request quality.
This is called by state handlers to retrieve the most
recent location request quality.
@return TLbsNetPosRequestQuality Currently held location request quality
*/ 
const TLbsNetPosRequestQuality& CStateMachineBase::LocReqQuality() const
	{
	return iLocReqQuality;
	}


/** Location request positioning method.
This is called by state handlers to retrieve the most
recent location request positioning method.
@return TLbsNetPosRequestMethod Currently held location request positioning method
*/ 
const TLbsNetPosRequestMethod& CStateMachineBase::PosMethod() const
	{
	return iPosMethod;
	}


/** Initialise internal state machine attributes.
This is used when a new protocol procedure commences.
*/
void CStateMachineBase::InitialiseMachineBase()
	{
	iTransitionState = ETransitionNull;
	iIsCancelPending = EFalse;
	iCancelSource = ECancelNone;
	iClientCancelReason = KErrNone;
	iNetworkCancelReason = KErrNone;
	iIsLocReqReceived = EFalse;
	iIsLocRespSent = EFalse;
	iIsMeasureControlHandled = EFalse;
	iIsAdditionalDataExpected = EFalse;
	iAssistanceDataActionRequired = EFalse;
	iLocRespReason = KErrNone;

	LBSLOG(ELogP2, "CStateMachineBase::InitialiseMachineBase() protocol state ---> EStateActive \n");
	iProtocolState = EStateActive;
	iIsMachineQueued = EFalse;
	}


/** Complete state machine attributes.
This is used when a protocol procedure completes.
*/
void CStateMachineBase::CompleteMachineBase()
	{
	LBSLOG(ELogP2, "CStateMachineBase::CompleteMachineBase() protocol state ---> EStateReady\n");
	iProtocolState = EStateReady;
	// Inform Protocol Manager that procedure is complete
	Observer().ProcedureCompleteInd();
	}


/** Store location response.
@param aPosInfo Location information response
*/
void CStateMachineBase::StoreLocationResp(TInt aReason, const TPositionInfoBase& aPosInfo)
	{
	iLocRespReason = aReason;
	iLocRespPosInfo = reinterpret_cast <const TPositionInfo&> (aPosInfo);
	}


/** Store network location cancel reason.
@param aReason The reason for cancelling network location
*/
void CStateMachineBase::StoreNetLocCancelReason(TInt aReason)
	{
	iNetLocCancelReason = aReason;
	}


/** Store location request quality
@param aQuality Location request quality
*/
void CStateMachineBase::StoreLocReqQuality(const TLbsNetPosRequestQuality& aQuality)
	{
	iLocReqQuality = aQuality;
	}


/** Store location request positioning method
@param aPosMethod Location request positioning method
*/
void CStateMachineBase::StorePosMethod(const TLbsNetPosRequestMethod& aPosMethod)
	{
	iPosMethod = aPosMethod;
	}


/** Store reference location
@param aPosInfo Network reference location information
*/
void CStateMachineBase::StoreRefLoc(const TPositionInfoBase& aPosInfo)
	{
	iReferenceLoc = reinterpret_cast <const TPositionInfo&> (aPosInfo);
	}


/** Enter new state.
This asks the state handler to perform the actions required when
entering a new state.
*/  
void CStateMachineBase::EnterNewState()
	{
	ASSERT(iStateHandler != NULL);

	// Perform entry actions for the new state
	iStateHandler->EntryActions();
	}


/** Exit current state.
This asks the state handler to perform the actions required when
exiting from a state.
*/  
void CStateMachineBase::ExitCurrentState()
	{
	ASSERT(iStateHandler != NULL);

	// Perform exit actions for the current state
	iStateHandler->ExitActions();
	}


/** Perform a state transition.
This initiates a state transition and performs the first stage of
this by exiting from the current state.
*/  
void CStateMachineBase::PerformStateTransition()
	{
	if (!IsActive())
		{
		iTransitionState = ETransitionExit;
		DoStateTransitionStage();
		}
	}


/** Do one stage of the state transition.
This employs a self-completion mechanism to perform one stage of
a state transition. The state machine object is set active and then its
own status is completed, which results in its RunL being called.
*/  
void CStateMachineBase::DoStateTransitionStage()
	{
	TRequestStatus* localStatus = &iStatus;
	iStatus = KRequestPending;
	SetActive();
	User::RequestComplete(localStatus, KErrNone);
	}


/** Indicates if state machine is cancelling
@return TBool ETrue if machine is cancelling
*/  
TBool CStateMachineBase::Cancelling() const
	{
	return (EStateCancelling == iProtocolState) ? ETrue : EFalse;
	}


/** Indicates if state machine is to be cancelled
@return TBool ETrue if machine is to be cancelled
*/  
TBool CStateMachineBase::CancelPending() const
	{
	return iIsCancelPending;
	}


/** Cancel state machine
@param aCancelSource The source that decided to cancel.
*/  
void CStateMachineBase::CancelMachine(const TCancelSource& aCancelSource)
	{
	iIsCancelPending = ETrue;
	iCancelSource = aCancelSource;

	// Cancel the active procedure
	CancelProcedure();

	// Perform state transition
	if (!IsActive() && (ETransitionNull == iTransitionState))
		{
		PerformStateTransition();
		}
	}


/** Sets state machine attributes to represent cancelling
*/  
void CStateMachineBase::SetMachineAsCancelling()
	{
	LBSLOG(ELogP2, "CStateMachineBase::SetMachineAsCancelling() protocol state ---> EStateCancelling \n");
	iIsCancelPending = EFalse;
	iProtocolState = CStateMachineBase::EStateCancelling;
	}


/** Cancel state machine
@param aCancelSource The source that decided to cancel.
@param aReason The reason for the cancellation.
*/  
void CStateMachineBase::CancelMachine(const TCancelSource& aCancelSource,
										TInt aReason)
	{
	// Store the specific cancel reason
	switch (aCancelSource)
		{
	case ECancelNetworkCancel:
	case ECancelNetworkError:
	case ECancelNetworkTimeout:
		iNetworkCancelReason = aReason;
		break;

	case ECancelClientTimeout:
	case ECancelClientCancel:
	case ECancelClientCancelSilent:
		iClientCancelReason = aReason;
		break;

	case ECancelNone:
	case ECancelClosing:
		break;

	default:
		User::Panic(KProtocolModulePanic, EProtocolModuleCancelSource);
		break;
		};

	// Cancel the machine
	CancelMachine(aCancelSource);
	}


/** Set state machine as queued.
This is called by individual derived state machines when they need
to enter into a queued state.
*/  
void CStateMachineBase::SetMachineAsQueued()
	{
	iIsMachineQueued = ETrue;
	}


/** Set state machine as queued.
This is called by individual derived state machines when they need
to enter into a queued state.
*/  
TBool CStateMachineBase::IsMachineQueued() const
	{
	return iIsMachineQueued;
	}


/** Check for a queued request.
Determine if there is a queued request and, if so, handle it.
*/
void CStateMachineBase::CheckForQueuedRequest()
	{
	// Check for a pending request
	if (!IsActive() && iQueue->IsRequestPending())
		{
		// Perform the required action
		DoQueuedRequest(iQueue->PopRequest());
		}
	}

	
/** Active object completion handler
*/
void CStateMachineBase::RunL()
	{
	// Check we have completed ok
	if (KErrNone == iStatus.Int())
		{
		// Which stage of the transition is this?
		switch (iTransitionState)
			{

		// About to exit from a state
		case ETransitionExit:
			// Prepare state machine prior to the transition.
			PreStateTransition();

			// Exit current state.
			ExitCurrentState();

			// Do transition to next stage
			iTransitionState = ETransitionEnter;
			DoStateTransitionStage();
			break;

		// About to enter a state
		case ETransitionEnter:
			// Reset transition state value
			iTransitionState = ETransitionNull;

			// Perform state machine transition.
			StateTransition();

			// Enter new state.
			EnterNewState();

			// If an assistance data action is required then do it now.
			// Note: Deferred until first measurement & control is handled.
			if (iAssistanceDataActionRequired)
				{
				iAssistanceDataActionRequired = EFalse;
				iObserver.DoAssistanceDataActions();
				}

			// Perform state machine post-transition actions
			// Note: This may initiate another state transition
			PostStateTransition();

			// If the state machine is not active then we can see if there 
			// are any queued requests to be handled
			if (!IsActive())
				{
				CheckForQueuedRequest();
				}
			break;

		// Not changing state just performing asynchronous activities
		case ETransitionNull:
		default:
			// If an assistance data action is required then do it now.
			if (iAssistanceDataActionRequired)
				{
				iAssistanceDataActionRequired = EFalse;
				iObserver.DoAssistanceDataActions();
				}
			break;
			};

		}

	}


/** Active object cancellation handler
*/
void CStateMachineBase::DoCancel()
	{
	iStatus = KErrCancel;
	}


/** Handle Network Measurement Control indication
A measurement control indication has been received and the associated content
must be stored. Note: At this stage the assistance data has already been handled 
by the protocol manager and is thus not passed into this method.

@param aPosInfo The network reference location
@param aQuality The location request quality
@param aMethod Positioning method to adopt
*/
void CStateMachineBase::MeasurementControlInd(const TPositionInfoBase& aPosInfo,
							const TLbsNetPosRequestQuality& aQuality,
							const TLbsNetPosRequestMethod& aPosMethod)
	{
	// Ensure assistance data action is performed when appropriate
	iAssistanceDataActionRequired = ETrue;

	// Store ref location and location request criteria
	StoreRefLoc(aPosInfo);
	StoreLocReqQuality(aQuality);
	StorePosMethod(aPosMethod);

	iIsLocReqReceived = ETrue;
	iIsMeasureControlHandled = EFalse;
	}


/** Handle LBS request for additional Assistance Data
This is called by the protocol manager during an active procedure when
a request has been made by LBS for further assistance data.
*/
void CStateMachineBase::AssistanceDataReq()
	{
	// Ensure assistance data action is performed when appropriate
	iAssistanceDataActionRequired = ETrue;
	iIsAdditionalDataExpected = ETrue;

	// If not already active then we must get the state machine to
	// perform asynchronous assistance data actions
	if (!IsActive())
		{
		DoStateTransitionStage();
		}
	}


/** Handle Network Additional Assistance data indication
This is called by the protocol manager during an active procedure when
an additional assistance data response is received from the network.
*/
void CStateMachineBase::AdditionalAssistDataInd()
	{
	// Stop the related timer
	iAssistDataTimer->Cancel();

	// Ensure assistance data action is performed when appropriate
	iAssistanceDataActionRequired = ETrue;
	iIsAdditionalDataExpected = EFalse;

	// If not already active then we must get the state machine to
	// perform asynchronous assistance data actions
	if (!IsActive())
		{
		DoStateTransitionStage();
		}
	}


/** Handle Network Measurement Control Error indication
This is called by the protocol manager during an active procedure when
a measurement control error is received from the network.
@param aCancelRequired Indicates if the derived state machine requires
	the state machine to be cancelled as a result of this error.
*/
void CStateMachineBase::HandleMeasureControlError(
			const TBool& aCancelRequired, TInt aReason)
	{
	// Stop the related timer
	iAssistDataTimer->Cancel();

	// Ensure assistance data action is performed
	iAssistanceDataActionRequired = ETrue;

	// Do we have to cancel the state machine?
	if (aCancelRequired)
		{
		iIsMeasureControlHandled = ETrue;
		CancelMachine(ECancelNetworkCancel, aReason);
	}
	else
		{
		// If not already active then we must get the state machine to
		// perform asynchronous assistance data actions
		if (!IsActive())
			{
			DoStateTransitionStage();
			}
		}
	}


/** Network Measurement Control Error indication
This implements the default actions to be taken if a derived class
does not override this method.
@param aReason The reason for network error
*/
void CStateMachineBase::MeasurementControlErrorInd(TInt aReason)
	{
	HandleMeasureControlError(EFalse, aReason);
	}


/** Handle a network error indication.
*/  
void CStateMachineBase::NetworkErrorInd()
	{
	// Only handle this the first time
	if (ECancelNetworkError != iCancelSource)
		{
		// Ensure assistance data actions are performed
		if (iIsAdditionalDataExpected)
			{
			iAssistanceDataActionRequired = ETrue;
			iIsAdditionalDataExpected = EFalse;
			}
		// Cancel state machine
		CancelMachine(ECancelNetworkError);
		}
	}


//-----------------------------------------------------------------------------
// State Machine Request Queue Class
//-----------------------------------------------------------------------------

/** Static constructor
@param aMaxEntries Maximum requests to be held in the queue.
@return A new instance of the CStateQueue class.
*/
CStateQueue* CStateQueue::NewL(TInt aMaxEntries)
	{
	CStateQueue* self = new (ELeave) CStateQueue(aMaxEntries);
	CleanupStack::PushL(self);
	self->ConstructL(aMaxEntries);
	CleanupStack::Pop(self);
	return self;
	}


/** Default private constructor
@param aMaxEntries Maximum requests to be held in the queue.
*/
CStateQueue::CStateQueue(TInt aMaxEntries)
 : iMaxEntries(aMaxEntries)
	{
	}


/** Destructor
*/
CStateQueue::~CStateQueue()
	{
	iQueue.Reset();
	}
	
	
/** Private second stage constructor
@param aMaxEntries Maximum requests to be held in the queue.
*/
void CStateQueue::ConstructL(TInt aMaxEntries) 
	{
	iQueue.ReserveL(aMaxEntries);
	}


/** Check if there is a pending request
@return TBool ETrue if there is a pending request in the queue
*/
TBool CStateQueue::IsRequestPending()
	{
	return (0 < iQueue.Count()) ? ETrue : EFalse;
	}


/** Add a request to the queue
The new request is inserted at the start of the internal array
because this makes it easier to pop the oldest request out of
the array, @see CStateQueue::PopRequest()
@param aNewRequest A number identifying the request to be queued.
@return TInt KErrNone if there is no error queueing the request.
*/
TInt CStateQueue::AddRequest(TInt aNewRequest)
	{
	TInt err = KErrOverflow;
	if (iMaxEntries > iQueue.Count())
		{
		TRAP(err, iQueue.InsertL(aNewRequest, 0));
		}

	return err;
	}


/** Pop the oldest request off the queue
The entry at the end of the array represents the oldest queued request.
@return TInt The number identifying the popped request
*/
TInt CStateQueue::PopRequest()
	{
	TInt request = 0;
	if (0 < iQueue.Count())
		{
		request = iQueue[iQueue.Count() - 1];
		iQueue.Remove(iQueue.Count() - 1);
		}

	return request;
	}

