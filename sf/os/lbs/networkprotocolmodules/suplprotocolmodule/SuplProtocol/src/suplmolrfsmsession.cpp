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
// the MO-LR protocol state machine.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#include "suplmolrfsmsession.h"
#include "suplmolrstatehandler.h"
#include "suplrrlpstatemachine.h"
#include "suplgatewayinterface.h"
#include "suplend.h"
#include "supldevloggermacros.h"

/** KSuplResponseTimerEvent
Identity of timer for when SUPL RESPONSE is expected from the network
*/
const TInt KSuplResponseTimerEvent = 0x100;

/** KSuplResponseTimerDurationInSec
Timer duration for when SUPL RESPONSE is expected from the network
*/
const TInt KSuplResponseTimerDurationInSec = 10;


/** KMaxQueueEntry
Maximum number of messages waiting to be processed
in the state machine event store.
*/
const TInt KMaxQueueEntry = 5;


/** Static constructor.
@param aObserver Reference to state machine observer.
@return A new instance of the CSuplMolrFsmSession class
*/
CSuplMolrFsmSession* CSuplMolrFsmSession::NewL(MSuplFsmSessionObserver& aObserver, TSuplMolrMachineType aMachineType)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::NewL() Begin\n");
	CSuplMolrFsmSession* self = new (ELeave) CSuplMolrFsmSession(aObserver, aMachineType);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::NewL() End\n");
	return self;
	}

/** Standard constructor.
@param aObserver Reference to state machine observer.
*/
CSuplMolrFsmSession::CSuplMolrFsmSession(MSuplFsmSessionObserver& aObserver, TSuplMolrMachineType aMachineType)
:CSuplFsmSessionBase(aObserver)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::CSuplMolrFsmSession() Begin\n");
	iLocReqType = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	iMachineType = aMachineType;
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::CSuplMolrFsmSession() End\n");
	}


/** Standard destructor.
*/ 
CSuplMolrFsmSession::~CSuplMolrFsmSession()
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::~CSuplMolrFsmSession() Begin\n");
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::~CSuplMolrFsmSession() End\n");
	}

/** Private second-stage constructor.
*/  
void CSuplMolrFsmSession::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::ConstructL() Begin\n");
	CActiveScheduler::Add(this);

	iEventStore = CSuplFsmEventStore::NewL(KMaxQueueEntry);

	// Create state handler
	iStateHandler = CSuplMoLrStateHandler::NewL(*this);

	// Open up assistance data builder
	iAssistanceDataBuilderSet.OpenL();

	// Create a Positioning Protocol State Machine
	iPositioningProtocol = CSuplRrlpFsm::NewL(*this,iAssistanceDataBuilderSet);
	
	// Create timers used during MO-LR protocol procedure
	iSuplProtTimer = CLbsCallbackTimer::NewL(*this);
	iSlpSettingsStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::ConstructL() End\n");
	}

	
/** Retrieve machine type
@return TSuplMolrMachineType type of MOLR state machine
*/
CSuplMolrFsmSession::TSuplMolrMachineType CSuplMolrFsmSession::MachineType()
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::MachineType() Begin\n");
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::MachineType() End\n");
	return iMachineType;
	}
	

/** GetHostId()
This method returns the ID of an entry in the HostSettings store
containing the address of the host (SLP) to connect to.
For MOLR, if a host in the store has been declared as "default", such
host will be used for the MOLR session. Otherwise, the last modified
network-provisioned host will be used.
If a host is not found in the store using the logic above, then a
host is created using the IMSI stored in the SIM (see "Auto
configuration of the H-SLP Address" in OMA-AD-SUPL-V1_0-20070615-A, 
section 7.2.2).
@param aHostId the ID of the host selected for the MOLR
@return KErrNone if a valid host ID has been found
*/
TInt CSuplMolrFsmSession::GetHostId(TLbsHostSettingsId& aHostId)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::GetHostId() Begin\n");
	TInt err = KErrNotFound;
	TBool hostFound = EFalse;
	if (iSlpSettingsStore)
		{
		TLbsHostSettingsSupl slpSettings;
		TLbsHostSettingsId 	settingsId;
		// Get the default SLP if one exists
		// jcm
		err = iSlpSettingsStore->GetDefaultHostSettings(slpSettings, settingsId);
		if (KErrNone == err)
			{
		    TLbsHostNameAddress hostNameAddress;
		    slpSettings.GetHostNameAddress(hostNameAddress);
		    //TBuf<128> name;
		    //name.Copy(hostNameAddress);
		    SUPLLOG2_L8(ELogP1, "Host name address = %S\n", &hostNameAddress);
	                   
		    TLbsHostName hostName;
		    slpSettings.GetName(hostName);
	        //name.Copy(hostName);
		    SUPLLOG2_L8(ELogP1, "Host name  = %S\n", &hostName);
		    
		    SUPLLOG2(ELogP1, "SettingsId=%d\n",settingsId.iUid );

	        
			aHostId = settingsId;
			hostFound = ETrue;
			}
		else
			{
			// If no default SLP is present, get the
			// network-provisioned SLP that has been 
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
	}	
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::GetHostId() End\n");
	if(!hostFound)
		{
		return GenerateHostId(aHostId);
		}
	
	return KErrNone;
	}

/** Start SUPL RESPONSE timer
The state handler calls this when it has issued a SUPL START
to the network and it requires a response before the timer expires.
*/
void CSuplMolrFsmSession::StartSuplResponseTimer()
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::StartSuplResponseTimer() Begin\n");
	iSuplProtTimer->EventAfter(TTimeIntervalSeconds(KSuplResponseTimerDurationInSec), KSuplResponseTimerEvent);
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::StartSuplResponseTimer() End\n");
	}

/** Initialise internal state attributes.
This is used when new MO-LR procedure commences.
*/
void CSuplMolrFsmSession::InitialiseProcedure()
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::InitialiseProcedure() Begin\n");
	// Initialise state machine
	InitialiseMachineBase();
	iCurrentState = EStateProcedureNull;
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::InitialiseProcedure() End\n");
	}
	

/** State transition.
This method determines the next state to be adopted by the state machine.
@return TBool ETrue if the state has changed.
@param aForceRedo, indicates that the states entry action must be re-performed when there has been no change in state
*/
TBool CSuplMolrFsmSession::SelectNextState(TBool& aForceRedo)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Begin\n");
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
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStateProcedureNull\n");
			iCurrentState = EStateNetConnectionStarted;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStateNetConnectionStarted\n");
			break;
			
		case EStateNetConnectionStarted:
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStateNetConnectionStarted\n");
			iCurrentState = EStateStartSent;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStateStartSent\n");
			break;
			
		case EStateStartSent:
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStateStartSent\n");
			stateChanged = DetermineStateFromStartSent();
			break;
			
		case EStateResponseReceived:
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStateResponseReceived\n");
		    iCurrentState = EStatePosInitSent;
		    SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStatePosInitSent\n");
			break;
			
		case EStatePosInitSent:
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStatePosInitSent\n");
			stateChanged = DetermineStateFromPosInitSent();
			break;

		case EStatePositioningInProgress:
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStatePositioningInProgress\n");
			stateChanged = DetermineStateFromPositioningInProgress(aForceRedo);
			break;

		case EStatePositionReceived:
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStatePositionReceived\n");
			iCurrentState = EStatePosSessionEnded;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStatePosSessionEnded\n");
			SetMachineAsNotCancellable();
			break;
			
		case EStateSuplSessionEnded:
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStateSuplSessionEnded\n");
			iCurrentState = EStatePosSessionEnded;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStatePosSessionEnded\n");
			break;

		case EStatePosSessionEnded:
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStatePosSessionEnded\n");
			iCurrentState = EStateLbsSessionEnded;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStateLbsSessionEnded\n");
			break;
			
		case EStateLbsSessionEnded:
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStateLbsSessionEnded\n");
			iCurrentState = EStateNetConnectionClosed;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStateNetConnectionClosed\n");
			break;
			
		case EStateNetConnectionClosed:
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() entry state EStateNetConnectionClosed\n");
			// Procedure has completed
			iCurrentState = EStateProcedureNull;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() Nex state: EStateProcedureNull\n");
			break;
			
		default:
			SUPLLOG(ELogP3, "CSuplMolrFsmSession::SelectNextState() unknown entry state\n");
			ASSERT(EFalse);
			break;
			};
		}
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::SelectNextState() End\n");
	return stateChanged;
	}
	

/** Complete the procedure.
*/  
void CSuplMolrFsmSession::CompleteProcedure()
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::CompleteProcedure() Begin\n");
	// Complete state machine
	CompleteMachineBase();
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::CompleteProcedure() End\n");
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
void CSuplMolrFsmSession::PostTransition()
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::PostTransition() Begin\n");
	// Some states are transitory i.e. they require
	// an automatic transition to the next state
	if (IsCancelPending() ||
	    ((EStateResponseReceived == iCurrentState) && (ESuplMolrCellBased == MachineType())) || 
		EStatePositionReceived		== iCurrentState ||
	 	EStateSuplSessionEnded		== iCurrentState ||
	 	EStatePosSessionEnded		== iCurrentState ||
	 	EStateLbsSessionEnded		== iCurrentState ||
	 	EStateNetConnectionClosed	== iCurrentState)
		{
		// Perform a state transition
		PerformTransition();
		}
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::PostTransition() End\n");
	}


/** Cancel the active procedure
*/  
void CSuplMolrFsmSession::CancelProcedure()
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::CancelProcedure() Begin\n");
	// Kill all timers
	iSuplProtTimer->Cancel();

	SUPLLOG(ELogP1, "CSuplMolrFsmSession::CancelProcedure() End\n");
	}


/** Timer expired callback.
This is called by a CStateTimer object when the timer
has expired - the event is identified by aEvent parameter.
@param aTimerId The timer event identifier.
*/
void CSuplMolrFsmSession::OnTimerEventL(TInt aTimerId)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::OnTimerEventL() Begin\n");
	// Perform relevant action for the expired timer
	switch (aTimerId)
		{
	// Connection result timer
	case KSessionConnectedTimerEvent:
		CancelMachine(CSuplFsmSessionBase::ECancelNetwork,CSuplFsmSessionBase::EReasonTimerExpiry);
		break;

	// SUPL RESPONSE timer
	case KSuplResponseTimerEvent:
		CancelMachine(CSuplFsmSessionBase::ECancelSuplProt,CSuplFsmSessionBase::EReasonTimerExpiry);
		break;

	// SUPL POS timer
	case KSuplPosTimerEvent:
		CancelMachine(CSuplFsmSessionBase::ECancelSuplProt,CSuplFsmSessionBase::EReasonTimerExpiry);
		break;

	case KSuplEndTimerEvent:
		CancelMachine(CSuplFsmSessionBase::ECancelSuplProt,CSuplFsmSessionBase::EReasonTimerExpiry);
		break;
		
	// Ignore unknown timer events
	default:
		break;
		};

	SUPLLOG(ELogP1, "CSuplMolrFsmSession::OnTimerEventL() End\n");
	}


//---------------------------------------------------------------------------------------------------------------------
// --------------------------------         EVENT HANDLING METHODS          -------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//----------------------------- EVENTS FROM PROTOCOL MANAGER (LBS) --------------------------						

/** Handle LBS request for MO-LR
@param aSessionId The session ID supplied by LBS.
*/
void CSuplMolrFsmSession::MoLrReq(const TLbsNetSessionId& aSessionId, const TLbsNetPosRequestOptionsBase& aOptions)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::MoLrReq() Begin\n");

	// Initialise the new procedure
	InitialiseProcedure();
	
	// Store the supplied ID information
	SetSessionId(aSessionId);

	// Store MOLR Options needed for SUPL START
	//
	if (KErrNone != StoreLocationRequest(aOptions))
		CancelMachine(CSuplFsmSessionBase::ECancelClosing, CSuplFsmSessionBase::EReasonNone);

	// Perform a state transition
	PerformTransition();
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::MoLrReq() End\n");
	}


/** Handle LBS Location response
@param aReason Location response error reason.
@param aPosInfo The location information response from LBS.
*/
void CSuplMolrFsmSession::LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::LocationResp() Begin\n");
	switch (aReason)
		{
		case KErrNone:
			if(EStatePositioningInProgress == iCurrentState)
				{
				// While a SUPL POS session is in progress, the recipient of the location update is
				// the Positioning Protocol state machine.
				iPositioningProtocol->LocationResp(aReason, aPosInfo);
				}
			else
				{
				// SUPL state machine has no use for a location update. Ignore it.
				}
			break;
			
		case KPositionCalculationFutile:
			// LBS unable to calculate a position using selected positioning method
			CancelMachine(CSuplFsmSessionBase::ECancelClient, CSuplFsmSessionBase::EReasonFutilePosCalc);
			break;

		default:
			if(EStatePositioningInProgress == iCurrentState)
				{
				// Send the position and the error code to the positioning state machine for
				// it to handle according to the specifications of the Positioning Protocol.
				iPositioningProtocol->LocationResp(aReason, aPosInfo);
				}
			else
				{
				CancelMachine(ECancelSuplProt,EReasonNone);
				}
			break;
		}
			
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::LocationResp() End\n");
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
void CSuplMolrFsmSession::AssistanceDataReq(const TLbsAssistanceDataGroup& aFilter)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::AssistanceDataReq() Begin\n");
	if (iCurrentState == EStatePositioningInProgress)
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
	else if (iCurrentState == EStateResponseReceived)
		{
		// The assistance data request can be sent to the SLP in a SUPL POS INIT
		StoreAssistanceDataRequest(aFilter);
		PerformTransition();
		}
	 else
		{
		 // Assistance data request has arrived unsolicitedly from LBS and SUPL has no 
		 // use for it. Log it and ignore it.
		}	
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::AssistanceDataReq() End\n");
	}


/** Handle a request to switch LBS session Id

The Protocol Manager calls this method following a conflict whose resolution
requires that the MOLR procedure remains active but the session ID changes
when a message is sent to LBS (the session ID in SUPL messages must remain the
same)


@param aFilter Assistance data types being requested
*/
void CSuplMolrFsmSession::AdoptNewLbsSessionId(const TLbsNetSessionId& aSessionId)
{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::AdoptNewLbsSessionId() Begin\n");
	iSessionIdReplaced = ETrue;
	iReplacementSessionId = aSessionId;
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::AdoptNewLbsSessionId() End\n");	
}


//----------------------------- EVENTS FROM POSITIONING PROTOCOL (RRLP) ---------------------------

/** Handle a notification of availability of new assistance data from
	the Positioning Protocol (RRLP) state machine.
*/
void CSuplMolrFsmSession::ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, TInt aReason)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::ProcessAssistanceData() Begin\n");
    ASSERT(iCurrentState == EStatePositioningInProgress);
	iObserver.ProcessAssistanceData(aGroupMask, iAssistanceDataBuilderSet, aReason,LocReqType());
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::ProcessAssistanceData() End\n");
	}

/** Handle a Location Request from the Positioning Protocol.
@param aQuality  quality parameters for the location request.
@param aPosMethod positioning method requested by the Positioning Protocol
*/
void CSuplMolrFsmSession::ProcessPositioningRequest(const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::ProcessPositioningRequest() Begin\n");
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
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::ProcessPositioningRequest() End\n");
	}

/** Handle a request from the Positioning Protocol state machine to send
a positioning protocol message to the network as the payload in a SUPL POS
@param aPositioningPayload Positioning Protocol (RRLP) message that will be payload of a SUPL POS message 
*/
void CSuplMolrFsmSession::PositioningPayloadToNetwork(const CSuplPosPayload* aPositioningPayload)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::PositioningPayloadToNetwork() Begin\n");

	// Disregard this event if MOLR FSM is no longer in positioning state
	// (this could happen if the  SUPL session has changed state due to
	// a cancel or error but this information the Positioning Protocol
	// state machine has not been cancelled yet)
	if (iCurrentState == EStatePositioningInProgress)
		{
		// As this event involves sending a SUPL POS to the network,
		// run it through the MOLR state machine
		//
		StorePosPayload(aPositioningPayload);
		// Perform a state transition
		PerformTransition();
		}
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::PositioningPayloadToNetwork() End\n");
	}

/** Handle error notification from Positioning Protocol
This is an unrecoverable error. Cancel state machine.
@param aError Error code reported by Postioning Protocol State Machine.
*/
void CSuplMolrFsmSession::PositioningProtocolError(const TInt& aError)
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::PositioningProtocolError() Begin\n");
	(void)aError;
	ASSERT(iCurrentState == EStatePositioningInProgress);
	CancelMachine(CSuplFsmSessionBase::ECancelPosProt, CSuplFsmSessionBase::EReasonNone);
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::PositioningProtocolError() End\n");
	}


//---------------------------------------------------------------------------------------------------------------------
// --------------------------------         STATE CHANGE METHODS          ---------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

/** Decide next state from EStateStartSent
@return TBool ETrue if the state has changed
*/
TBool CSuplMolrFsmSession::DetermineStateFromStartSent()
	{
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::DetermineStateFromStartSent() Begin\n");
	TBool stateChanged = ETrue;
	
	// Check what SUPL Message was received.
	//
	const CSuplMessageBase* msg = NULL;
	TBool messageAvailable = iEventStore->IsSuplMessageStored(msg);

	if (messageAvailable)
		{
		// Received message was stored as constant (read-only) but
		// access to non-constant methods is needed
		CSuplMessageBase* message = const_cast<CSuplMessageBase*> (msg);
		if (CSuplMessageBase::ESuplEnd == message->MessageType())
			{
			iSessionInProgress = EFalse;
			CSuplEnd* suplEnd = static_cast <CSuplEnd*> (message);

			if (suplEnd->PositionPresent())
				{
				iCurrentState = EStatePositionReceived;
				SUPLLOG(ELogP1, "CSuplMolrFsmSession::DetermineStateFromStartSent(). Next state : EStatePositionReceived\n");
				// The position is about to be sent to LBS.
				// The states following will terminate the session.
				// Handling a cancel would result in sending an 
				// unnecessary SUPL END to the SLP.
				SetMachineAsNotCancellable();
				}
			else
				{
				// Unexpected session termination.
				// Remember this fact before switching state.
				iEventStore->SetUnexpectedSuplEnd();
				iCurrentState = EStateLbsSessionEnded;
				SUPLLOG(ELogP1, "CSuplMolrFsmSession::DetermineStateFromStartSent(). Next state : EStateLbsSessionEnded\n");
				SetMachineAsNotCancellable();
				}
			}
		else if (CSuplMessageBase::ESuplResponse == message->MessageType())
			{
			iCurrentState = EStateResponseReceived;
			SUPLLOG(ELogP1, "CSuplMolrFsmSession::DetermineStateFromStartSent(). Next state : EStateResponseReceived\n");
			}
		else
			{
			// Unexpected SUPL Message was received
			//
			CancelMachine(CSuplFsmSessionBase::ECancelSuplProt,CSuplFsmSessionBase::EReasonUnexpectedMessage);
			}
		}
	else
		{
		stateChanged = EFalse;
		}
	SUPLLOG(ELogP1, "CSuplMolrFsmSession::DetermineStateFromStartSent() End\n");
	return stateChanged;
	}
