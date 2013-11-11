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
// This file provides the implementation of the class for
// the MT-LR protocol state machine.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#include "suplmtlrfsmsession.h"
#include "suplmtlrstatehandler.h"
#include "suplrrlpstatemachine.h"
#include "suplend.h"
#include "suplinit.h"
#include "supldevloggermacros.h"
#include "suplgatewayinterface.h"


/** KMaxQueueEntry
Maximum number of messages waiting to be processed
in the state machine event store.
*/
const TInt KMaxQueueEntry = 5;


/** Static constructor.
@param aObserver Reference to state machine observer.
@return A new instance of the CSuplMtlrFsmSession class
*/
CSuplMtlrFsmSession* CSuplMtlrFsmSession::NewL(MSuplFsmSessionObserver& aObserver)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::NewL() Begin\n");
	CSuplMtlrFsmSession* self = new (ELeave) CSuplMtlrFsmSession(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::NewL() End\n");
	return self;
	}

/** Standard constructor.
@param aObserver Reference to state machine observer.
*/  
CSuplMtlrFsmSession::CSuplMtlrFsmSession(MSuplFsmSessionObserver& aObserver)
:CSuplFsmSessionBase(aObserver)
	{ 
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::CSuplMtlrFsmSession() Begin\n");
	iLocReqType = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::CSuplMtlrFsmSession() End\n");
	}


/** Standard destructor.
*/ 
CSuplMtlrFsmSession::~CSuplMtlrFsmSession()
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::~CSuplMtlrFsmSession() Begin\n");
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::~CSuplMtlrFsmSession() End\n");
	}

/** Private second-stage constructor.
*/  
void CSuplMtlrFsmSession::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::ConstructL() Begin\n");
	CActiveScheduler::Add(this);

	iEventStore = CSuplFsmEventStore::NewL(KMaxQueueEntry);

	// Create state handler
	iStateHandler = CSuplMtLrStateHandler::NewL(*this);

	// Open up assistance data builder
	iAssistanceDataBuilderSet.OpenL();

	// Create a Positioning Protocol State Machine
	iPositioningProtocol = CSuplRrlpFsm::NewL(*this,iAssistanceDataBuilderSet);
	
	// Create timers used during MTLR protocol procedure
	iSuplProtTimer = CLbsCallbackTimer::NewL(*this);
	
	iSlpSettingsStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::ConstructL() End\n");
	}

/** GetHostId()
This method returns the ID of an entry in the HostSettings store
containing the address of the host (SLP) to connect to.
For MTLR, and in accordance with the SUPL specifications, only
a host that has been provisioned by the home network will be used.
This method retrieves the network-provisioned host that has last
been modified.
@param aHostId the ID of the host selected for the MTLR
@return KErrNone if a valid host ID has been found
*/
TInt CSuplMtlrFsmSession::GetHostId(TLbsHostSettingsId& aHostId)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::GetHostId() Begin\n");
	TInt err = KErrNotFound;
	TBool hostFound = EFalse;
	if (iSlpSettingsStore)
		{
		TLbsHostSettingsSupl slpSettings;
		TLbsHostSettingsId 	settingsId;

		// Get the network-provisioned SLP that has been 
		// modified most recently
		TTime mostRecent = 0;
		err = iSlpSettingsStore->RewindHostSettings();
		if (KErrNone == err)
			{
			while (KErrNone == err)
				{
				err = iSlpSettingsStore->GetNextHostSettingsByCreator(KLbsHostSettingsDevProvCreatorId, slpSettings, settingsId);		
				if (KErrNone == err)
					{
					if (mostRecent < slpSettings.LastModified())
						{
						mostRecent = slpSettings.LastModified();
						aHostId = settingsId;
						hostFound = ETrue;
						}
					}
				}
			}			
		}	

	if(!hostFound)
		{
		SUPLLOG(ELogP1, "CSuplMtlrFsmSession::GetHostId(), GenerateHostId() End\n");
		return GenerateHostId(aHostId);
		}

	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::GetHostId() End\n");
	// If we did not return earlier, we must have found a valid ID
	return KErrNone;
	}


/** Initialise internal state attributes.
This is used when new MTLR procedure commences.
*/
void CSuplMtlrFsmSession::InitialiseProcedure()
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::InitialiseProcedure() Begin\n");
	// Initialise state machine
	InitialiseMachineBase();	
	iCurrentState = EStateProcedureNull;
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::InitialiseProcedure() End\n");
	}
	

/** State transition.
This method determines the next state to be adopted by the state machine.
@return TBool ETrue if the state has changed.
@param aForceRedo, indicates that the states entry action must be re-performed when there has been no change in state
*/
TBool CSuplMtlrFsmSession::SelectNextState(TBool& aForceRedo)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Begin\n");
	TBool stateChanged = ETrue;
	aForceRedo = EFalse;
	// Regardless of current state, check first if an event has occurred
	// that implies cancelling the machine (terminate session)
	if (IsCancelPending())
		{
		SetMachineAsCancelling();
		iCurrentState = (IsSessionConnected() && iSessionInProgress)? EStateSuplSessionEnded : EStatePosSessionEnded;
		}
	else
		{
		// Set new state	
		switch (iCurrentState)
			{
		case EStateProcedureNull:
			SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Entry state: EStateProcedureNull\n");
			iCurrentState = EStateSuplInitReceived;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStateSuplInitReceived\n");
			SetSessionInProgress(ETrue);
			break;
			
		case EStateSuplInitReceived:
			SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Entry state: EStateSuplInitReceived\n");
			stateChanged = DetermineStateFromInitReceived();
			break;

		case EStateNetConnectionStarted:
			SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Entry state: EStateNetConnectionStarted\n");
			stateChanged = DetermineStateFromConnectionStarted(aForceRedo);
			break;
	
		case EStatePosInitSent:
			SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Entry state: EStatePosInitSent\n");
			stateChanged = DetermineStateFromPosInitSent();
			break;

		case EStatePositioningInProgress:
			SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Entry state: EStatePositioningInProgress\n");
			stateChanged = DetermineStateFromPositioningInProgress(aForceRedo);
			break;

		case EStatePositionReceived:
			SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Entry state: EStatePositionReceived\n");
			iCurrentState = EStatePosSessionEnded;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStatePosSessionEnded\n");
			SetMachineAsNotCancellable();
			break;
			
		case EStateSuplSessionEnded:
			SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Entry state: EStateSuplSessionEnded\n");
			iCurrentState = EStatePosSessionEnded;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStateSuplSessionEnded\n");
			break;

		case EStatePosSessionEnded:
			SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Entry state: EStatePosSessionEnded\n");
			iCurrentState = EStateLbsSessionEnded;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStateLbsSessionEnded\n");
			break;
			
		case EStateLbsSessionEnded:
			SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Entry state: EStateLbsSessionEnded\n");
			iCurrentState = EStateNetConnectionClosed;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStateNetConnectionClosed\n");
			break;
			
		case EStateNetConnectionClosed:
			SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() Entry state: EStateNetConnectionClosed\n");
			// Procedure has completed
			iCurrentState = EStateProcedureNull;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStateProcedureNull\n");
			break;
			
		default:
			SUPLLOG(ELogP3, "CSuplMtlrFsmSession::SelectNextState() unknown entry state\n");
			ASSERT(EFalse);
			break;
			};
		}
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::SelectNextState() End\n");
	return stateChanged;
	}
	

/** Complete the procedure.
*/  
void CSuplMtlrFsmSession::CompleteProcedure()
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::CompleteProcedure() Begin\n");
	// Complete state machine
	CompleteMachineBase();
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::CompleteProcedure() End\n");
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
void CSuplMtlrFsmSession::PostTransition()
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::PostTransition() Begin\n");
	// Some states are transitory i.e. they require
	// an automatic transition to the next state
	if (IsCancelPending() ||
		EStatePositionReceived		== iCurrentState ||
	 	EStateSuplSessionEnded		== iCurrentState ||
	 	EStatePosSessionEnded		== iCurrentState ||
	 	EStateLbsSessionEnded		== iCurrentState ||
	 	EStateNetConnectionClosed	== iCurrentState)
		{
		// Perform a state transition
		PerformTransition();
		}
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::PostTransition() End\n");
	}


/** Cancel the active procedure
*/  
void CSuplMtlrFsmSession::CancelProcedure()
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::CancelProcedure() Begin\n");
	// Kill all timers
	iSuplProtTimer->Cancel();
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::CancelProcedure() End\n");
	}


/** Timer expired callback.
This is called by a CStateTimer object when the timer
has expired - the event is identified by aEvent parameter.
@param aTimerId The timer event identifier.
*/
void CSuplMtlrFsmSession::OnTimerEventL(TInt aTimerId)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::OnTimerEventL() Begin\n");
	// Perform relevant action for the expired timer
	switch (aTimerId)
		{
		
	// Connection result timer
	case KSessionConnectedTimerEvent:
		CancelMachine(ECancelNetwork,EReasonTimerExpiry);
		break;

	// SUPL POS timer
	case KSuplPosTimerEvent:
		CancelMachine(ECancelSuplProt,EReasonTimerExpiry);
		break;

	case KSuplEndTimerEvent:
		CancelMachine(ECancelSuplProt,EReasonTimerExpiry);
		break;

	// Ignore unknown timer events
	default:
		break;
		};

	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::OnTimerEventL() End\n");
	}


/** This method stores the parameters received in a SUPL INIT message.
If any incorrect or unsupported parameter is detected the method returns leaves. 
*/
void CSuplMtlrFsmSession::ProcessExternalLocationRequestL(const CSuplInit* aSuplInit)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::StoreExternalLocationRequestL() Begin\n");
	TLbsHostSettingsId hostId;
	TLbsHostSettingsSupl slpSettings;
	TLbsHostNameAddress hostNameAddress;
	
	// aSuplInit is constant (read-only) but
	// access to non-constant methods is needed
	CSuplInit* suplInitPtr = const_cast<CSuplInit*> (aSuplInit);
	
	// Store the Session ID
	CSuplSessionId* sessionId = CSuplSessionId::NewL();
	

	CleanupStack::PushL(sessionId);
	User::LeaveIfError(suplInitPtr->GetSessionId(*sessionId));
	
	// SLP session ID must be present
	// SET session ID must not be present
	if (sessionId->iSetSessionIdPresent ||
		! sessionId->iSlpSessionIdPresent)
		{
		User::Leave(KErrArgument);
		}
	

	SetMessageSessionId(sessionId);
	CleanupStack::Pop(sessionId);

	// Only proxy mode is supported
	// (SUPL INIT with non-proxy mode is ignored)
	CSuplInit::TSuplSlpMode slpMode = suplInitPtr->SlpMode();
	if (CSuplInit::ESuplSlpModeProxy != slpMode)
		{
		User::Leave(KErrArgument);
		}

	TLbsNetPosRequestPrivacy privacy;
	if (suplInitPtr->NotificationPresent())
		{
		User::LeaveIfError(suplInitPtr->GetNotificationType(privacy));
		
		// Check for Stealth / Privacy Override. This is not supported by LBS.
		if (privacy.RequestAdvice() == TLbsNetPosRequestPrivacy::ERequestAdviceStealth)
			{
			User::Leave(KErrArgument);
			}
		}
	else
		{
		// If notification is not present that means
		// "no notification and no verification" as per
		// SUPL specification (but it is up to LBS to
		// accept such conditions depending on its configuration).	
		privacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent);
		}

	//External requestor info
	TLbsExternalRequestInfo requestInfo;
	if (suplInitPtr->ExternalRequestInfoPresent())
		{
		User::LeaveIfError(suplInitPtr->GetExternalRequestInfo(requestInfo));
		}

	// QOP
	TLbsNetPosRequestQuality quality;
	if (suplInitPtr->QopPresent())
		{
		User::LeaveIfError(suplInitPtr->GetQop(quality));
		}
	else
		{
		// Set accuracy to NaN to prompt LBS to
		// use accuracies from its pre-configured
		// profiles if available.
		TRealX nan;
		nan.SetNaN();
		quality.SetMinHorizontalAccuracy(nan);
		quality.SetMinVerticalAccuracy(nan);
		}

	// Positioning method
	TLbsNetPosRequestMethod method;
	User::LeaveIfError(suplInitPtr->GetPosMethod(method));

	// VER
	User::LeaveIfError(GetHostId(hostId));
	User::LeaveIfError(iSlpSettingsStore->GetHostSettings(hostId, slpSettings));
	slpSettings.GetHostNameAddress(hostNameAddress);
	TPtrC8 ver = suplInitPtr->GetVerL(hostNameAddress);

	iEventStore->StoreExternalLocationRequest(privacy, requestInfo, quality, method, ver);
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::StoreExternalLocationRequestL() End\n");
	}



//---------------------------------------------------------------------------------------------------------------------
// --------------------------------         EVENT HANDLING METHODS          -------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//------------------------------------ EVENTS FROM PROTOCOL MANAGER  ---------------------------------------						

/** Handle an MTLR request due to a SUPL INIT received.
@param aSessionId The session ID supplied by LBS.
@param aSuplInit The SUPL INIT that triggered the MTLR
*/
void CSuplMtlrFsmSession::MtLrReq(const TLbsNetSessionId& aSessionId, const CSuplInit* aSuplInit)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::MtLrReq() Begin\n");
	// Store parameters received in the SUPL INIT message
	TRAPD(err, ProcessExternalLocationRequestL(aSuplInit))
	if (KErrNone == err)
		{
		// Initialise the new procedure
		InitialiseProcedure();
		// Store the supplied ID information
		SetSessionId(aSessionId);
		// Perform a state transition
		PerformTransition();
		}
	else
		{
		// A problem was found with the parameters
		// in the SUPL INIT. Ignore the SUPL INIT
		// and clear the event store.
		iEventStore->ClearStore();
		}

	delete aSuplInit;
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::MtLrReq() End\n");
	}


/** Handle a Response from LBS to an earlier Privacy Request
@param aResponse Privacy response value.
*/
void CSuplMtlrFsmSession::PrivacyResp(const TLbsNetSessionId& /*aSessionId */, 
									  const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::PrivacyResp() Begin\n");
	iEventStore->StoreEvent(aResponse);
	// Perform a state transition out of EStateSuplInitReceived
	// (only state expecting this event)
	if (EStateSuplInitReceived == iCurrentState)
		PerformTransition();
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::PrivacyResp() End\n");
	}

/** Handle LBS Location response
@param aReason Location response error reason.
@param aPosInfo The location information response from LBS.
*/	
void CSuplMtlrFsmSession::LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::LocationResp() Begin\n");
	switch (aReason)
		{
		case KErrCancel:
			// LBS is cancelling the MTLR
			CancelMachine(CSuplFsmSessionBase::ECancelClient, CSuplFsmSessionBase::EReasonNone);
			break;
					
		case KPositionCalculationFutile:
			// LBS unable to calculate a position using selected positioning method
			CancelMachine(CSuplFsmSessionBase::ECancelClient, CSuplFsmSessionBase::EReasonFutilePosCalc);
			break;
			
		case KErrNone:
			if(EStatePositioningInProgress == iCurrentState)
				{
				// While a SUPL POS session is in progress, the recipient of the location update is
				// the Positioning Protocol state machine.
				iPositioningProtocol->LocationResp(aReason, aPosInfo);
				}
			else if (EStateNetConnectionStarted == iCurrentState)
				{
				// The position provided by LBS can be sent to 
				// the SLP in a SUPL POS INIT. Store it:
				TInt err = KErrNone;
				TRAP(err, iEventStore->StoreEventL(aPosInfo));

				// Leave current state if a connection is already up.
				if (IsSessionConnected())
						PerformTransition();
				}
			break;

		default:
			// Not one of the reason codes for SUPL to handle. If positioning is ongoing
			// send the position and the error code to the positioning state machine for
			// it to handle according to the specifications of the positioning protocol.
			if(EStatePositioningInProgress == iCurrentState)
				{
				// While a SUPL POS session is in progress, the recipient of the location update is
				// the Positioning Protocol state machine.
				iPositioningProtocol->LocationResp(aReason, aPosInfo);
				}
			else
				{
				// Cancel the MTLR except when this location response arrives after a privacy
				// reject (KErrAccessDenied), in which case the cancel must happen after the connection has been 
				// created so a SUPL END can be sent
				CLbsNetworkProtocolBase::TLbsPrivacyResponse response;
				if (!((KErrAccessDenied == aReason) &&
					 (iEventStore->GetPrivacyResp(response)) &&
					 (CLbsNetworkProtocolBase::EPrivacyResponseRejected == response)))
					{
					CancelMachine(CSuplFsmSessionBase::ECancelClient, CSuplFsmSessionBase::EReasonNone);					
					}
				}
				
			break;
		}

	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::LocationResp() End\n");
	}

/** Handle LBS Assistance Data Request

The Protocol Manager calls this method following an assitance data request from
LBS. However, the PM only does that if a location request had been sent
to LBS (which signals the point when the state machines can handle a request
for assitance data). Consequently assistance data requests are only handled
from states that may have resulted in a Location Request sent to LBS from the
state machine. 

@param aFilter Assistance data types being requested
*/
void CSuplMtlrFsmSession::AssistanceDataReq(const TLbsAssistanceDataGroup& aFilter)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::AssistanceDataReq() Begin\n");
	if (EStatePositioningInProgress == iCurrentState)
		{
		// While a SUPL POS session is in progress, the recipient of the assistance data 
		// request is the Positioning Protocol state machine, but only if the assistance
		// data request is not empty.
		if (aFilter != 0)
			{
			iPositioningProtocol->AssistanceDataRequest(aFilter);
			}
		else
			{
			// ignore assistance data request
			// 	
			}
		}
	 else if (EStateNetConnectionStarted == iCurrentState)
		{
		StoreAssistanceDataRequest(aFilter);
		// If the connection had already been created, this state
		// was simply waiting for the assistance data. Transition out
		// of it.
		if (IsSessionConnected())
			PerformTransition();		
		}
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::AssistanceDataReq() End\n");
	}


//----------------------------- EVENTS FROM POSITIONING PROTOCOL (RRLP) ---------------------------

/** Handle a notification of availability of new assistance data from
	the Positioning Protocol (RRLP) state machine.
*/
void CSuplMtlrFsmSession::ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, TInt aReason)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::ProcessAssistanceData() Begin\n");
    ASSERT(iCurrentState == EStatePositioningInProgress);
	iObserver.ProcessAssistanceData(aGroupMask, iAssistanceDataBuilderSet, aReason,LocReqType());
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::ProcessAssistanceData() End\n");
	}

/** Handle a Location Request from the Positioning Protocol.
@param aQuality  quality parameters for the location request.
@param aPosMethod positioning method requested by the Positioning Protocol
*/
void CSuplMtlrFsmSession::ProcessPositioningRequest(const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::ProcessPositioningRequest() Begin\n");
	// Disregard this event if MOLR FSM is no longer in positioning state
	// (this could happen if the  SUPL session has changed state due to
	// a cancel or error but the Positioning Protocol
	// state machine has not been notified yet)
	if (iCurrentState == EStatePositioningInProgress)
		{
		// Verify that the positioning method requested by the positioning
		// state machine is supported by LBS
		TLbsNetPosMethod selectedMethod;//only one to send to LBS
		TLbsNetPosCapabilities capabilities;
		iObserver.Gateway().GetCapabilities(capabilities);
		if (PosMethodSupported(aPosMethod,selectedMethod,capabilities))
			{
			TLbsNetPosRequestMethod posMethod;
			posMethod.SetPosMethods(&selectedMethod, 1);
			// SUPL MoLr state machine just passes the request on to Protocol Manager.
			iObserver.LocationReq(LbsSessionId(), LocReqType(), aQuality, posMethod);
			}
		else
			{
			// The associated positioning protocol has sent a location request
			// with an unsupported positioning method. Terminate.
			CancelMachine(CSuplFsmSessionBase::ECancelSuplProt,CSuplFsmSessionBase::EReasonMethodMismatch);	
			}
		}
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::ProcessPositioningRequest() End\n");
	}

/** Handle a request from the Positioning Protocol state machine to send
a positioning protocol message to the network as the payload in a SUPL POS
@param aPositioningPayload Positioning Protocol (RRLP) message that will be payload of a SUPL POS message 
*/
void CSuplMtlrFsmSession::PositioningPayloadToNetwork(const CSuplPosPayload* aPositioningPayload)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::PositioningPayloadToNetwork() Begin\n");

	// Disregard this event if MTLR FSM is no longer in positioning state
	// (this could happen if the  SUPL session has changed state due to
	// a cancel or error but this information the Positioning Protocol
	// state machine has not been cancelled yet)
	if (iCurrentState == EStatePositioningInProgress)
		{
		// As this event involves sending a SUPL POS to the network,
		// run it through the MTLR state machine
		//
		StorePosPayload(aPositioningPayload);
		// Perform a state transition
		PerformTransition();
		}
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::PositioningPayloadToNetwork() End\n");
	}

/** Handle error notification from Positioning Protocol
This is an unrecoverable error. Cancel state machine.
@param aError Error code reported by Postioning Protocol State Machine.
*/
void CSuplMtlrFsmSession::PositioningProtocolError(const TInt& aError)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::PositioningProtocolError() Begin\n");
	(void)aError;
	ASSERT(iCurrentState == EStatePositioningInProgress);
	CancelMachine(ECancelPosProt, EReasonNone);
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::PositioningProtocolError() End\n");
	}

//---------------------------------------------------------------------------------------------------------------------
// --------------------------------         STATE CHANGE METHODS          ---------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

/** Decide next state from EStateSuplInitReceived
@return TBool ETrue if the state has changed
*/
TBool CSuplMtlrFsmSession::DetermineStateFromInitReceived()
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::DetermineStateFromInitReceived() Begin\n");
	CLbsNetworkProtocolBase::TLbsPrivacyResponse response;
	
	// There must be a privacy response in the event store.
	// If the privacy response is ignore, then cancel session.
	// If the privacy response says accept or reject, then transition
	// to the state that connects to the server (EStateNetConnectionStarted).
	if (iEventStore->GetPrivacyResp(response))
		{
		if (CLbsNetworkProtocolBase::EPrivacyResponseIgnored == response)
			{
			CancelMachine(CSuplFsmSessionBase::ECancelClient, CSuplFsmSessionBase::EReasonPrivacyIgnored);
			}
		else
			{
			iCurrentState = EStateNetConnectionStarted;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::DetermineStateFromInitReceived() Next state: EStateNetConnectionStarted\n");
			}
		}
	else
		{
		// A transition out of EStateSuplInitReceived should not have started
		// This is only possible if there has been an internal error.
		CancelMachine(CSuplFsmSessionBase::ECancelSuplProt, CSuplFsmSessionBase::EReasonNone);
		}
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::DetermineStateFromInitReceived() End\n");
	return ETrue;
	}

/** Decide next state from EStateNetConnectionStarted
@return TBool ETrue if the state has changed
@param aForceRedo, indicates that the states entry action must be re-performed when there has been no change in state
*/
TBool CSuplMtlrFsmSession::DetermineStateFromConnectionStarted(TBool& aForceRedo)
	{
	SUPLLOG(ELogP1, "CSuplMtlrFsmSession::DetermineStateFromConnectionStarted() Begin\n");
	// The next state depends on the value of privacy response kept in the event store:
	// 1.- If PrivacyReqAccepted: 
	//     1.1- Transition to the state EStatePosInitSent if LBS has sent an assistance
	//     		data request or a position.
	//		or
	//
	//     1.2- Transition to EStateSuplSessionEnded if the original request in the SUPL INIT
	//          was a "no position" one. 
	// 2.- If PrivacyReqRejected: no new state selected, just Cancel the session after
	//		receiving a "Connected" indication so SUPL END is sent.
	
	
	// Get the preferred method received in a SUPL INIT
	// (kept in event store).
	TLbsNetPosRequestMethod locReqMethods;
	TLbsNetPosMethod prefMethod;
	RetrieveInitPosMethod(locReqMethods);
	locReqMethods.GetPosMethod(0, prefMethod);
	
	// always force Entry Actions for this state to be performed on new events,
	// even if there is not a change in state.
	aForceRedo =  ETrue;
	
	CLbsNetworkProtocolBase::TLbsPrivacyResponse privResponse;
	iEventStore->GetPrivacyResp(privResponse);
	if (CLbsNetworkProtocolBase::EPrivacyResponseAccepted == privResponse)
		{	
		// Only transition to EStatePosInitSent if LBS has sent an Assistance
		// Data Request or a Location Response. Otherwise the transition will take place when
		// the first of those events occurs after the connection has been created.
		if (iEventStore->IsAssistanceDataReqStored() || iEventStore->IsPositionStored())
			{
			iCurrentState = EStatePosInitSent;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::DetermineStateFromConnectionStarted() Next state: EStatePosInitSent End\n");
			return ETrue;
			}
		else if (KLbsPositioningMeansNone == prefMethod.PosMeans())	// "no position" in SUPL INIT
			{
			iCurrentState = EStateSuplSessionEnded;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::DetermineStateFromConnectionStarted() Next state: EStateSuplSessionEnded End\n");
			return ETrue;			
			}
		else
			{
			// Nothing to do but wait for new events. Intentionally empty.
			}
		}
	else if((CLbsNetworkProtocolBase::EPrivacyResponseRejected == privResponse) && IsSessionConnected())
		{
		CancelMachine(CSuplFsmSessionBase::ECancelClient, CSuplFsmSessionBase::EReasonPrivacyRejected);
		SUPLLOG(ELogP1, "CSuplMtlrFsmSession::DetermineStateFromConnectionStarted() CancelMachine End\n");
		return ETrue;
		}
		SUPLLOG(ELogP1, "CSuplMtlrFsmSession::DetermineStateFromConnectionStarted() End\n");
		return EFalse;
	}
