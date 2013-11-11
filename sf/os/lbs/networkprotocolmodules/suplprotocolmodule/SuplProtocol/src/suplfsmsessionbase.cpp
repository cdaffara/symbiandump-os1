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
// This file provides the implementation of the base class for
// protocol state machines used in the SUPL Protocol Module.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#include <in_sock.h>
#include "suplfsmsessionbase.h"
#include "suplstatehandlerbase.h"
#include <lbs/lbsnetclasstypes.h>
#include "supldevloggermacros.h"
#include "suplend.h"
#include <lbs/lbsextendedsatellite.h>


const TLbsHostCreatorId KUidLbsHostSettingsCreator = {0x10285A9D};

//-----------------------------------------------------------------------------
// State Machine Base Class
//-----------------------------------------------------------------------------

/** Standard constructor.
@param aObserver A reference to the state machine observer. 
*/  
CSuplFsmSessionBase::CSuplFsmSessionBase(MSuplFsmSessionObserver& aObserver)
: CActive(EPriorityLow), iObserver(aObserver), iProtocolState(EStateReady)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::CSuplFsmSessionBase() Begin\n");
	ResetSessionId();
	iDefSettingsId.Null();
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::CSuplFsmSessionBase() End\n");
	}


/** Standard destructor.
*/  
CSuplFsmSessionBase::~CSuplFsmSessionBase()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::~CSuplFsmSessionBase() Begin\n");
	
	if(iDefSettingsId!=TUid::Uid(KNullUidValue) && iSlpSettingsStore)
		{
		iSlpSettingsStore->DeleteHostSettings(iDefSettingsId);
		}
	
	Cancel();
	delete iEventStore;
	delete iStateHandler;
	delete iPositioningProtocol;
	iAssistanceDataBuilderSet.Close();
	delete iSlpSettingsStore;
	delete iMessageSessionId;
	delete iTelNumber;
	delete iSuplProtTimer;
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::~CSuplFsmSessionBase() End\n");
	}


/** Retrieve reference to state machine observer.
@return MSuplFsmSessionObserver A reference to the observer. 
*/  
MSuplFsmSessionObserver& CSuplFsmSessionBase::Observer()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::Observer() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::Observer() End\n");
	return iObserver;
	}

/** Set session ID
@param aSessionId Identifier to be used by the state machine whenever
	the session ID is to be used.
*/
void CSuplFsmSessionBase::SetSessionId(const TLbsNetSessionId& aSessionId)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SessionId() Begin\n");
	iSessionId = aSessionId;
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SessionId() End\n");
	}

/** Reset session ID
*/
void CSuplFsmSessionBase::ResetSessionId()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::ResetSessionId() Begin\n");
	iSessionId.SetSessionOwner(TUid::Uid(0));
	iSessionId.SetSessionNum(0);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::ResetSessionId() End\n");
	}

/** Get session ID
@return const TLbsNetSessionId& The session ID currently used by the state machine.
*/
const TLbsNetSessionId& CSuplFsmSessionBase::SessionId() const
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SessionId() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SessionId() End\n");
	return iSessionId;
	}
	
/** Obtain the session Id to be set in a call back to LBS
*/
const TLbsNetSessionId& CSuplFsmSessionBase::LbsSessionId() const
{
	// This base class just returns iSessionId
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::LbsSessionId() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::LbsSessionId() End\n");
	return iSessionId;
}
	
CSuplPositioningProtocolFsm* CSuplFsmSessionBase::PositioningFsm()
{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PositioningFsm() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PositioningFsm() End\n");
	return iPositioningProtocol;
}

/** Retrieve current general protocol state.
@return CSuplFsmSessionBase::TMachineState The current general 
	protocol state for this state machine.

@see CSuplFsmSessionBase::TMachineState
*/
CSuplFsmSessionBase::TMachineState CSuplFsmSessionBase::State() const
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::State() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::State() End\n");
	return iProtocolState;
	}

/** Location request type.
This is called by state handlers to determine what type of location
request is to be specified to LBS. This is usually fixed for a given
state machine.
@return MLbsNetworkProtocolObserver::TLbsNetProtocolService request type.
*/
MLbsNetworkProtocolObserver::TLbsNetProtocolService CSuplFsmSessionBase::LocReqType() const
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::LocReqType() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::LocReqType() End\n");
	return iLocReqType;
	}


/** Initialise internal state machine attributes.
This is used when a new protocol procedure commences.
*/
void CSuplFsmSessionBase::InitialiseMachineBase()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::InitialiseMachineBase() Begin\n");
	iTransitionState = ETransitionNull;
	iProtocolState = EStateActive;
	iIsCancelPending = EFalse;
	iSessionConnected = EFalse;
	iCancelNoLongerAllowed = EFalse;
	
	ResetSessionId();
	
	// Clear store from any events that may remain
	// from previous session
	iEventStore->ClearStore();
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::InitialiseMachineBase() End\n");
	}

/** Complete state machine attributes.
This is used when a protocol procedure completes.
*/
void CSuplFsmSessionBase::CompleteMachineBase()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::CompleteMachineBase() Begin\n");
	iProtocolState = EStateReady;

	// close connection
	iObserver.ConnectionManager().Disconnect(iSessionId.SessionNum());
	iSessionConnected = EFalse;
	
	// clear event store
	iEventStore->ClearStore();

	iSessionIdReplaced = EFalse;
	
	// Inform Protocol Manager that procedure is complete
	iObserver.ProcedureCompleteInd(LocReqType());
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::CompleteMachineBase() End\n");
	}

/** Enter new state.
This asks the state handler to perform the actions required when
entering a new state.
*/  
TBool CSuplFsmSessionBase::NextStateEntryActionsL()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::NextStateEntryActionsL() Begin\n");
	ASSERT(iStateHandler);

	// Perform entry actions for the new state
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::NextStateEntryActionsL() End\n");
	return iStateHandler->EntryActionsL();
	}

/** Perform a transition.
This initiates a transition following either an external trigger
or a decision by the state machine that it must move on to the 
next state (self-transition).

If the transition has been due to an external event, the state machine
may yet decide to remain in the current state; @see SelectNextState()
*/  
void CSuplFsmSessionBase::PerformTransition()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PerformTransition() Begin\n");
	if (!IsActive())
		{
		iTransitionState = ETransitionEnter;
		DoTransitionStage();
		}
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PerformTransition() End\n");
	}

/** Do one stage of the state transition.
This employs a self-completion mechanism to perform one stage of
a state transition. The state machine object is set active and then its
own status is completed, which results in its RunL being called.
*/  
void CSuplFsmSessionBase::DoTransitionStage()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::DoTransitionStage() Begin\n");
	TRequestStatus* localStatus = &iStatus;
	iStatus = KRequestPending;
	SetActive();
	User::RequestComplete(localStatus, KErrNone);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::DoTransitionStage() End\n");
	}

/** Returns ETrue if at least one of the positioning methods in a SUPL message
is within LBS' capabilities.

@param aPosMethod Positioning methods received in a SUPL message. A maximum of two positioning methods are expected.
@param aLbsCapabilities Positioning methods supported by LBS.
@param aSelected method. If the method returns ETrue, this variable will contain the network preferred
		         positioning method if supported by LBS, otherwise, the network alternative positioning.
		         method. This variable is not modified when the return value is EFalse.
@return ETrue if at least one method in aPosMethods is supported by LBS
*/
TBool CSuplFsmSessionBase::PosMethodSupported(const TLbsNetPosRequestMethod& aPosMethods, TLbsNetPosMethod& aSelectedMethod, const TLbsNetPosCapabilities& aLbsCapabilities)
{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PosMethodSupported() Begin\n");
	TInt index;
	TBool methodSupported = EFalse;
	TLbsNetPosMethod capPosMethod;
	TInt alternativeMethod = KErrNone;
	TLbsNetPosMethod netPrefMethod;
	TLbsNetPosMethod netAltMethod;
	
	// There must be a method preferred by the SUPL server
	if (KErrNone != aPosMethods.GetPosMethod(0, netPrefMethod))
		return methodSupported;
	// There may be an alternative method from the SUPL server
	alternativeMethod = aPosMethods.GetPosMethod(1, netAltMethod);

	// Loop through all of LBS-supported positioning methods
	for (index = 0; index < aLbsCapabilities.NumPosMethods(); index++)
		{
		aLbsCapabilities.GetPosMethod(index,capPosMethod);
		if (netPrefMethod.PosMeans() == capPosMethod.PosMeans() &&
			netPrefMethod.PosMode() == capPosMethod.PosMode())
			{
			// If the preferred network method is supported
			// just return it.
			aSelectedMethod = netPrefMethod;
			methodSupported = ETrue;
			break;
			}
		else if ((KErrNone != alternativeMethod) &&
				 (netAltMethod.PosMeans() == capPosMethod.PosMeans()) &&
		 		 (netAltMethod.PosMode() == capPosMethod.PosMode()))
			{
			aSelectedMethod = netPrefMethod;
			methodSupported = ETrue;
			}
		}
	
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PosMethodSupported() End\n");
	return methodSupported;
}


/** Indicates if state machine is to be cancelled
@return TBool ETrue if machine is to be cancelled
*/
TBool CSuplFsmSessionBase::IsCancelPending() const
	{
	SUPLLOG2(ELogP1, "CSuplFsmSessionBase::IsCancelPending() = %d\n", iIsCancelPending);
	return iIsCancelPending;
	}

/** Set the status of the connection (connected or not)
@param aSessionConnected ETrue means the connection is up
*/
void  CSuplFsmSessionBase::SetSessionConnected(TBool aSessionConnected)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SetSessionConnected() Begin\n");
	iSessionConnected = aSessionConnected;
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SetSessionConnected() End\n");
	}

/** Return the connection status
*/
TBool CSuplFsmSessionBase::IsSessionConnected()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::IsSessionConnected() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::IsSessionConnected() End\n");
	return iSessionConnected;
	}

/** Cancel state machine
@param aCancelSource The source that decided to cancel.
*/
void CSuplFsmSessionBase::CancelMachine(const TCancelSource& aCancelSource, const TCancelReason& aReason)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::CancelMachine() Begin\n");
	TBool forcedCancel = EFalse;
	if ((aCancelSource == CSuplFsmSessionBase::ECancelClosing))
		{
		// Cancel requested from RunError.
		// Set the machine as disconnected so there is no attempt to
		// send a SUPL END. This is to prevent the execution 
		// of leaving methods by the state machine from this point onwards.
		//
		iSessionConnected = EFalse;
		
		forcedCancel = ETrue;
		}
		
	// Always cancel if the cancel is forced.
	// Otherwise, only cancel if no cancel is in progress
	// and no cancel has already been requested
	// (only the first cancel source is considered)
	// and if cancel is allowed.
	if (forcedCancel ||
		((EStateCancelling != iProtocolState) && !iIsCancelPending && !iCancelNoLongerAllowed))
		{
		iIsCancelPending = ETrue;

		// Cancel the active procedure
		CancelProcedure();

        StoreCancelInfo(aCancelSource,aReason);

		// Perform state transition
		PerformTransition();
		}
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::CancelMachine() End\n");
	}

/** Sets state machine attributes to represent cancelling
*/  
void CSuplFsmSessionBase::SetMachineAsCancelling()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SetMachineAsCancelling() Begin\n");
	iIsCancelPending = EFalse;
	iProtocolState = EStateCancelling;
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SetMachineAsCancelling() End\n");
	}

/** Sets state machine to not-cancellable
Derived state machines call this method after entering
some of the last states of the procedure when it no longer
make sense to accept cancel requests
*/  
void CSuplFsmSessionBase::SetMachineAsNotCancellable()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SetMachineAsNotCancellable() Begin\n");
	iCancelNoLongerAllowed = ETrue;	
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SetMachineAsNotCancellable() End\n");
	}


/** Check for a stored events.
Determine if there is an event stored and, if so, trigger
a transition to see if the event needs handling from
current state.
*/
void CSuplFsmSessionBase::CheckForStoredEvents()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::CheckForStoredEvents() Begin\n");
	ASSERT(!IsActive()); // method only called when the AO is inactive
	
	// Check for a pending event when the machine is active
	if ((iProtocolState == EStateActive) && (!iEventStore->IsStoreEmpty()))
		{
		PerformTransition();
		}
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::CheckForStoredEvents() End\n");
	}
	
/** Active object completion handler
*/
void CSuplFsmSessionBase::RunL()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RunL() Begin\n");
	SUPLLOG3(ELogP1, " iStatus=%d, iTransitionState=&d\n", iStatus.Int(), iTransitionState);
	
	TBool stateChanged = EFalse;
	TBool redoEntryActions = EFalse;
	TBool entryActionsConducted = EFalse;
	// Check we have completed ok
	if (KErrNone == iStatus.Int())
		{
		// Which stage of the transition is this?
		switch (iTransitionState)
			{

		// About to enter a state
		case ETransitionEnter:

			// Reset transition state value
			iTransitionState = ETransitionNull;

			// Perform state machine transition.
			stateChanged = SelectNextState(redoEntryActions);

			// Execute next state entry actions only if a
			// cancel has not been requested from 
			// SelectNextState() due to unexpected message.
			if (!iIsCancelPending)
				{
				if (stateChanged || redoEntryActions)  
					{
					entryActionsConducted = NextStateEntryActionsL();				
					}
				}
			
			// Perform state machine post-transition actions
			// Note: this will initiate a transition out of states
			// that perform self-transition.
			PostTransition();

			// When the state machine is not active (it is waiting
			// for an external trigger) check if there are
			// any stored events to be handled that may provoke
			// a transition.
			if (!IsActive())
				{
				// If the last transition neither changed state
				// nor was conductive to any actions then it can
				// be assumed that there are no pending events that 
				// can be handled from the current state (a transition  
				// can only be triggered by a new event)
				if (stateChanged || entryActionsConducted)
					{
					CheckForStoredEvents();
					}
				}
			break;

		// Not changing state just performing asynchronous activities
		// or waiting for new events
		case ETransitionNull:
		default:
			// Empty intentionally
			break;
			};
		}
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RunL() End\n");
	}

/** Active object completion handler
*/
void CSuplFsmSessionBase::RunError()
{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RunError() Begin\n");
	CancelMachine(ECancelClosing,EReasonNone);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RunError() End\n");
}


/** Active object cancellation handler
*/
void CSuplFsmSessionBase::DoCancel()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::DoCancel() Begin\n");
	iStatus = KErrCancel;
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::DoCancel() End\n");
	}

/** Retrieve Stored Message Session Id
@return Pointer to the stored message session ID object
*/
CSuplSessionId* CSuplFsmSessionBase::MessageSessionId()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::MessageSessionId() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::MessageSessionId() End\n");
	return iMessageSessionId;
	}

/** Store Message Session Id
@return Pointer to the stored message session ID object
*/
void CSuplFsmSessionBase::SetMessageSessionId(CSuplSessionId* aMessageSessionId)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SetMessageSessionId() Begin\n");
	
	// Delete old session Id, take ownership of the new one
	if (iMessageSessionId != NULL)
		{
		delete iMessageSessionId;		
		}

	
	iMessageSessionId = aMessageSessionId;
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SetMessageSessionId() End\n");
	}
	
/** Store assistance data mask from an assistance data request
@param aFilter Requested assistance data types
*/
void CSuplFsmSessionBase::StoreAssistanceDataRequest(const TLbsAssistanceDataGroup& aFilter)
{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreAssistanceDataRequest() Begin\n");
	iEventStore->StoreEvent(aFilter);	
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreAssistanceDataRequest() End\n");
}

/** Store location request positioning method
@param aOptions Location request options
@return KErrNone or KErrNoMemory
*/
TInt CSuplFsmSessionBase::StoreLocationRequest(const TLbsNetPosRequestOptionsBase& aOptions)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreLocationRequest() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreLocationRequest() End\n");
	return iEventStore->StoreEvent(aOptions);
	}

/** Store a SUPL message in the event store
@param aSuplMessage a pointer to the SUPL message being stored (ownership transferred)
*/
void CSuplFsmSessionBase::StoreSuplMessage(const CSuplMessageBase* aSuplMessage)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreSuplMessage() Begin\n");
	iEventStore->StoreEvent(aSuplMessage);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreSuplMessage() End\n");
	}

/** Store a Positioning Protocol payload in the event store
@param aPositioningPayload a pointer to the payload being stored (ownership transferred)
*/
void CSuplFsmSessionBase::StorePosPayload(const CSuplPosPayload* aPositioningPayload)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StorePosPayload() Begin\n");
	iEventStore->StoreEvent(aPositioningPayload);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StorePosPayload() End\n");
	}

/** Store reason for cancelling the procedure and source of the cancel
@param aSource the originator of the cancel
@param aReason the reason for cancelling the procedure
*/
void CSuplFsmSessionBase::StoreCancelInfo(const CSuplFsmSessionBase::TCancelSource& aSource,
							const CSuplFsmSessionBase::TCancelReason& aReason)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreCancelInfo() Begin\n");
	iEventStore->StoreEvent(aSource,aReason);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreCancelInfo() End\n");
	}

void CSuplFsmSessionBase::StoreNetInfo(const RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
					  const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea)
{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreNetInfo() Begin\n");
	//We store only information for two modes supported - GSM & WCDMA
	if(aNetworkInfo.iMode==RMobilePhone::ENetworkModeGsm || aNetworkInfo.iMode==RMobilePhone::ENetworkModeWcdma)
		{
		iEventStore->StoreEvent(aNetworkInfo, aLocationArea);
		}
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreNetInfo() End\n");
}
void CSuplFsmSessionBase::StoreMccMnc(TUint aMcc, TUint aMnc)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreMccMnc() Begin\n");
	iMcc = aMcc;
	iMnc = aMnc;
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreMccMnc() End\n");
	}

void CSuplFsmSessionBase::StoreMsisdn(const TDesC& aTelNumber)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreMsisdn() Begin\n");
	delete iTelNumber;
	iTelNumber = HBufC::New(aTelNumber.Length());
	if(iTelNumber)
		{
		(*iTelNumber) = aTelNumber;
		}
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreMsisdn() End\n");
	}

void CSuplFsmSessionBase::StoreCellInfo(const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo)
{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreCellInfo() Begin\n");
	iEventStore->StoreEvent(aCellInfo);	
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StoreCellInfo() End\n");
}

void CSuplFsmSessionBase::StorePositioningEnded(TBool aPositioningEnded)
{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StorePositioningEnded() Begin\n");
	iEventStore->SetPositioningEnded(aPositioningEnded);	
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StorePositioningEnded() End\n");
}



/** Retrieve from event store options associated to a location request from LBS.
@param aOptions upon return, contains the options of a previous location request.
	   When the method is from a Cell-Based state machine, aOptions must be of type
	   TLbsNetPosRequestOptions. When called from an AGPS state machine, aOptions 
	   must be of type TLbsNetPosRequestOptionsAssistance.
*/
TBool CSuplFsmSessionBase::RetrieveLocationRequestOptions(TLbsNetPosRequestOptions& aOptions)
{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveLocationRequestOptions() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveLocationRequestOptions() End\n");
	return iEventStore->GetLocationRequestOptions(aOptions);
}

/** Retrieve an assistance data request mask from the event store
@return EFalse if no assistance data request mask is stored
@param aPositioningPayload a pointer to the payload being stored (ownership transferred)
*/
TBool CSuplFsmSessionBase::RetrieveAssistanceDataRequest(TLbsAssistanceDataGroup& aAssitDataMask)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveAssistanceDataRequest() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveAssistanceDataRequest() End\n");
	return iEventStore->GetAssistanceDataRequest(aAssitDataMask);
	}

/**  Retrieve the oldest stored SUPL message from the event store
Ownership of the message is transferred to the caller.
*/
const CSuplMessageBase* CSuplFsmSessionBase::RetrieveSuplMessage()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveSuplMessage() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveSuplMessage() End\n");
	return iEventStore->SuplMessage();
	}

/**  Retrieve the oldest stored Positioning payload from the event store
Ownership of the payload object is transferred to the caller.
*/
const CSuplPosPayload* CSuplFsmSessionBase::RetrievePosPayload()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrievePosPayload() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrievePosPayload() End\n");
	return iEventStore->PosPayload();
	}

void CSuplFsmSessionBase::RetrieveCancelInfo(CSuplFsmSessionBase::TCancelSource& aSource,
    					CSuplFsmSessionBase::TCancelReason& aReason)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveCancelInfo() Begin\n");
	iEventStore->GetCancelInfo(aSource,aReason);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveCancelInfo() End\n");
	}

const TInetAddr& CSuplFsmSessionBase::RetrieveLocalIpAddress()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveLocalIpAddress() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveLocalIpAddress() End\n");
	return iEventStore->Address();
	}	


void CSuplFsmSessionBase::RetrieveStoredNetworkInfo(RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
													RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveStoredNetworkInfo() Begin\n");
	iEventStore->GetNetworkInfo(aNetworkInfo, aLocationArea);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveStoredNetworkInfo() End\n");
	}

TBool CSuplFsmSessionBase::RetrieveStoredCellInfo(RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveStoredCellInfo() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveStoredCellInfo() End\n");
	return iEventStore->GetCellInfo(aCellInfo);
	}

void CSuplFsmSessionBase::RetrievePrivacyAdvice(TLbsNetPosRequestPrivacy& aPrivacy)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrievePrivacyAdvice() Begin\n");
	 iEventStore->GetPrivacyAdvice(aPrivacy);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrievePrivacyAdvice() End\n");
	}

void CSuplFsmSessionBase::RetrieveRequestorInfo(TLbsExternalRequestInfo& aRequestor)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveRequestorInfo() Begin\n");
	iEventStore->GetRequestorInfo(aRequestor);		
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveRequestorInfo() End\n");
	}

void CSuplFsmSessionBase::RetrieveInitPosMethod(TLbsNetPosRequestMethod& aPosMethod)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveInitPosMethod() Begin\n");
	iEventStore->GetInitPosMethod(aPosMethod);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveInitPosMethod() End\n");
	}

void CSuplFsmSessionBase::RetrieveInitQuality(TLbsNetPosRequestQuality& aQuality)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveInitQuality() Begin\n");
	iEventStore->GetInitQuality(aQuality);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveInitQuality() End\n");
	}

void CSuplFsmSessionBase::RetrieveVer(TDes8& ver)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveVer() Begin\n");
	iEventStore->GetVer(ver);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrieveVer() End\n");
	}

TBool CSuplFsmSessionBase::RetrievePositioningSessionEnded()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PositioningSessionEnded() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PositioningSessionEnded() End\n");	
	return iEventStore->PositioningEnded();
	}

TBool CSuplFsmSessionBase::IsPositionStored()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::IsPositionStored() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::IsPositionStored() End\n");
	return iEventStore->IsPositionStored();
	}
	
/** Returns pointer to the stored position information
*/
TPositionInfoBase* CSuplFsmSessionBase::RetrievePositionL()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrievePositionL() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::RetrievePositionL() End\n");
	return iEventStore->PositionL();	
	}

/** Retrieve from event store whether an unexpected SUPL END was received or not
*/
TBool CSuplFsmSessionBase::IsUnexpectedSuplEndStored()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::IsUnexpectedSuplEndStored() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::IsUnexpectedSuplEndStored() End\n");
	return iEventStore->UnexpectedSuplEndStored();
	}
	
TBool CSuplFsmSessionBase::IsAssistanceDataRequestStored()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::IsAssistanceDataRequestStored() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::IsAssistanceDataRequestStored() End\n");
	return iEventStore->IsAssistanceDataReqStored();
	}

TBool CSuplFsmSessionBase::IsNetworkInfoAvailable()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::IsNetworkInfoAvailable() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::IsNetworkInfoAvailable() End\n");
	return iEventStore->IsNetworkInfoAvailable();
	}

TBool CSuplFsmSessionBase::PosSessionConducted()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PosSessionConducted() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PosSessionConducted() End\n");
	return iEventStore->PosSessionConducted();
	}

TInt CSuplFsmSessionBase::GenerateHostId(TLbsHostSettingsId& aHostId)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::GenerateHostId() Begin\n");
	TRAPD(err, GenerateHostIdL(aHostId));
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::GenerateHostId() End\n");
	return err;
	}

void CSuplFsmSessionBase::GenerateHostIdL(TLbsHostSettingsId& aHostId)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::GenerateHostIdL() Begin\n");
	//Set the UID to zero first of all
	aHostId.Null();
	
	//Generating the address
	_LIT8(KHslpAddresFormat, "h-slp.mnc%03d.mcc%03d.pub.3gppnetwork.org");
	TLbsHostNameAddress hslpAddress;
	
	hslpAddress.Format(KHslpAddresFormat, iMnc, iMcc);
	
	//Creating a new record
	CLbsHostSettingsStore* settingsStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(settingsStore);
	
	TLbsHostSettingsSupl settings;
	_LIT8(KDefSuplServer, "defSuplServer");
	settings.SetName(KDefSuplServer);
	_LIT8(KDefaultConfig, "defaultConfig");
	settings.SetProviderId(KDefaultConfig);
	 
	settings.SetHostNameAddress(hslpAddress);
	settings.SetPortId(KLbsSuplUseDefaultPortId);
	//To tell the Connection Manager to use the default connection
	settings.SetConnectionPoint(0, TLbsHostSettingsSupl::ELbsConnectionTypeInvalid);
	
	settings.SetAuthModesMOLR(TLbsHostSettingsSupl::EAuthAcaTls);
	settings.SetAuthModesMTLR(TLbsHostSettingsSupl::EAuthAcaTls);
	
	TInt err= settingsStore->CreateHostSettings(settings, KUidLbsHostSettingsCreator, iDefSettingsId);
	if(err!=KErrNone)
		{
		iDefSettingsId.Null();
		User::Leave(err);
		}
	aHostId = iDefSettingsId;
	
	CleanupStack::PopAndDestroy(settingsStore);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::GenerateHostIdL() End\n");
	}


//-----------------------------------------------------------------------------
// State Machine Event Store Class Methods
//-----------------------------------------------------------------------------

/** Static constructor
@param aMaxEntries Maximum requests to be held in the intenal queues.
@return A new instance of the CStateQueue class.
*/
CSuplFsmEventStore* CSuplFsmEventStore::NewL(TInt aMaxEntries)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::NewL() Begin\n");
	CSuplFsmEventStore* self = new (ELeave) CSuplFsmEventStore(aMaxEntries);
	SUPLLOG(ELogP1, "CSuplFsmEventStore::NewL() End\n");
	return self;
	}

/** Default private constructor
@param aMaxEntries Maximum requests to be held in the queue.
*/
CSuplFsmEventStore::CSuplFsmEventStore(TInt aMaxEntries)
: iLocRequestOptions (NULL),
  iCancelSource(CSuplFsmSessionBase::ECancelNone),
  iCancelReason(CSuplFsmSessionBase::EReasonNone),
  iMessageQueue(aMaxEntries),
  iPayloadQueue(aMaxEntries),
  iMaxEntries(aMaxEntries)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::CSuplFsmEventStore() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmEventStore::CSuplFsmEventStore() End\n");
	}


/** Destructor
*/
CSuplFsmEventStore::~CSuplFsmEventStore()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::~CSuplFsmEventStore() Begin\n");
	// Delete stored messages
	iMessageQueue.ResetAndDestroy();
	iPayloadQueue.ResetAndDestroy();
	delete iLocRequestOptions;
	iLocRequestOptions = NULL;
	delete iPosition;
	iPosition = NULL;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::~CSuplFsmEventStore() End\n");
	}

/** Clear all the events from the store
*/
void CSuplFsmEventStore::ClearStore()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::ClearStore() Begin\n");
	if (iMessageQueue.Count() > 0)
		{
		iMessageQueue.ResetAndDestroy();
		}
	if (iPayloadQueue.Count() > 0)
		{
		iPayloadQueue.ResetAndDestroy();
		}

	iAssistanceDataStored = EFalse;
	
	if (iLocationRequestStored)
		{
		delete iLocRequestOptions;
		iLocRequestOptions = NULL;
		iLocationRequestStored = EFalse;
		}	

	iUnexpectedSuplEndReceived = EFalse;
	if (iPositionStored)
		{
		iPositionStored = EFalse;
		delete iPosition;
		iPosition = NULL;
		}
		
	iPositioningEnded = EFalse;
	iPosSessionConducted = EFalse;
	iCancelSource = CSuplFsmSessionBase::ECancelNone;
	iPrivacyRespStored = EFalse;
	iPositionStored = EFalse;
	iPositioningEnded = EFalse;
	iPosSessionConducted = EFalse;
	
	SUPLLOG(ELogP1, "CSuplFsmEventStore::ClearStore() End\n");
	}

/** Return true if there is any event in the store
*/
TBool CSuplFsmEventStore::IsStoreEmpty()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::IsStoreEmpty() Begin\n");
	TBool empty = ETrue;

	if (iAssistanceDataStored ||
		iMessageQueue.Count() > 0 ||
		iPayloadQueue.Count() > 0)
		{
		empty = EFalse;
		}
	SUPLLOG(ELogP1, "CSuplFsmEventStore::IsStoreEmpty() End\n");
	return empty;
	}


/** Return true if there are any SUPL message in the store. If
there are any, return a pointer to the oldest stored message
in aSuplMessage but keep the message in the store (ownership
is not transferred).
@param aSuplMessage Reference to the oldest stored message.
*/
TBool CSuplFsmEventStore::IsSuplMessageStored(const CSuplMessageBase*& aSuplMessage)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::IsSuplMessageStored() Begin\n");
	TBool ret = EFalse;
	if (iMessageQueue.Count() > 0)
		{
		aSuplMessage = (iMessageQueue[iMessageQueue.Count() - 1]);
		ret = ETrue;
		}
	SUPLLOG(ELogP1, "CSuplFsmEventStore::IsSuplMessageStored() End\n");
	return ret;
	}

/** Return true if assistance data request is stored
*/
TBool CSuplFsmEventStore::IsAssistanceDataReqStored()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::IsAssistanceDataReqStored() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmEventStore::IsAssistanceDataReqStored() End\n");
	return iAssistanceDataStored;
	}

/** Return true if network data is stored
*/
TBool CSuplFsmEventStore::IsNetworkInfoAvailable()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::IsNetworkInfoAvailable() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmEventStore::IsNetworkInfoAvailable() End\n");
	return iNetworkDataStored;
	}

TBool CSuplFsmEventStore::IsPositionStored()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::IsPositionStored() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmEventStore::IsPositionStored() End\n");
	return iPositionStored;	
	}

/** Returns ETrue if assistance data request is stored. In such case
return the requested assistance data mask.
@return TBool ETrue if the assistance data mask of a previous request is stored
@param aAssistDataMask variable that after the call will hold the stored assistance data mask
*/
TBool CSuplFsmEventStore::GetAssistanceDataRequest(TLbsAssistanceDataGroup& aAssistDataMask)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetAssistanceDataRequest() Begin\n");
	TBool requested = EFalse;
	if (iAssistanceDataStored)
		{
		aAssistDataMask = iAssistanceDataMask;
		iAssistanceDataStored = EFalse;
		requested = ETrue;
		}
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetAssistanceDataRequest() End\n");
	return requested;
	}

/** Returns ETrue if the parameters of a location request are stored. In such case
return the parameters of request.
@return TBool ETrue if the paramaters of a previous location request are stored.
@param aOptions variable that after the call will hold the stored parameters of the previous location request.
*/
TBool CSuplFsmEventStore::GetLocationRequestOptions(TLbsNetPosRequestOptionsBase& aOptions)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetLocationRequestOptions() Begin\n");
	TBool ret = EFalse;
	if (iLocationRequestStored)
		{
		if (aOptions.ClassType() == iLocRequestOptions->ClassType())
			{
			Mem::Copy(&aOptions, iLocRequestOptions, iLocRequestOptions->ClassSize());
			ret = ETrue;
			}
		}
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetLocationRequestOptions() End\n");
	return ret;
	}

/** Returns a pointer to the oldest stored SUPL message
or NULL if no message was avaible.
Ownership of the message is transferred.
*/
const CSuplMessageBase* CSuplFsmEventStore::SuplMessage()
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::SuplMessage() Begin\n");
	const CSuplMessageBase* msgPtr = NULL;

	// Pop last message off the array (FIFO)
	if (iMessageQueue.Count() > 0)
		{
		msgPtr = iMessageQueue[iMessageQueue.Count() - 1];
		iMessageQueue.Remove(iMessageQueue.Count() - 1);
		}
	SUPLLOG(ELogP1, "CSuplFsmEventStore::SuplMessage() End\n");
	return msgPtr;
}


const TInetAddr& CSuplFsmEventStore::Address()
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::Address() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmEventStore::Address() End\n");
	return iAddress;
}

/** Returns a pointer to the oldest stored Positioning Payload
or NULL if no message was avaible.

Ownership of the message is transferred.
*/
const CSuplPosPayload* CSuplFsmEventStore::PosPayload()
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::PosPayload() Begin\n");
	const CSuplPosPayload* payloadPtr = NULL;

	// Pop last payload off the array (FIFO)
	if (iPayloadQueue.Count() > 0)
		{
		payloadPtr = iPayloadQueue[iPayloadQueue.Count() - 1];
		iPayloadQueue.Remove(iPayloadQueue.Count() - 1);
		}
	SUPLLOG(ELogP1, "CSuplFsmEventStore::PosPayload() End\n");
	return payloadPtr;
}

/** Returns the stored source and reason of a previous cancel
*/
void CSuplFsmEventStore::GetCancelInfo(CSuplFsmSessionBase::TCancelSource& aSource,CSuplFsmSessionBase::TCancelReason& aReason)
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetCancelInfo() Begin\n");
	aSource = iCancelSource;
	aReason = iCancelReason;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetCancelInfo() End\n");
}

/** Returns the stored current network parameters
*/
void CSuplFsmEventStore::GetNetworkInfo(RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
										RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetNetworkInfo() Begin\n");
	aNetworkInfo = iNetworkInfo;
	aLocationArea = iLocationArea;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetNetworkInfo() End\n");
	}
	
/** Returns the stored cell info with timing advance
 network parameters
*/
TBool CSuplFsmEventStore::GetCellInfo(RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetCellInfo() Begin\n");
	if (iCellInfoStored)
		aCellInfo = iCellInfo;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetCellInfo() End\n");
	return iCellInfoStored;
	}
	
/** Returns privacy advice that was received in a SUPL INIT
"notification" field
*/
void CSuplFsmEventStore::GetPrivacyAdvice(TLbsNetPosRequestPrivacy& aPrivacy)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetPrivacyAdvice() Begin\n");
	aPrivacy = iPrivacyInfo;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetPrivacyAdvice() End\n");
	}
	
/** Returns requestor info that was received in a SUPL INIT
"notification" field
*/	
void CSuplFsmEventStore::GetRequestorInfo(TLbsExternalRequestInfo& aRequestor)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetRequestorInfo() Begin\n");
	aRequestor = iRequestorInfo;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetRequestorInfo() End\n");
	}

/** Returns tentative positioning methods that were received in a SUPL INIT
*/
void CSuplFsmEventStore::GetInitPosMethod(TLbsNetPosRequestMethod& aPosMethod)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetInitPosMethod() Begin\n");
	aPosMethod = iInitPosMethod;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetInitPosMethod() End\n");
	}

/** Returns quality of position that was received in a SUPL INIT
*/
void CSuplFsmEventStore::GetInitQuality(TLbsNetPosRequestQuality& aQuality)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetInitQuality() Begin\n");
	aQuality = iInitQuality;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetInitQuality() End\n");
	}
	
/** Returns the stored HMAC of the received SUPL INIT
*/
void CSuplFsmEventStore::GetVer(TDes8& aVer)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetVer() Begin\n");
	aVer.Copy(iVer);
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetVer() End\n");
	}
	
TBool CSuplFsmEventStore::GetPrivacyResp(CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetPrivacyResp() Begin\n");
	TBool ret = ETrue;
	
	if (iPrivacyRespStored)
		{
		aResponse = iPrivacyResp;	
		}
	else
		{
		ret = EFalse;
		}
	
	SUPLLOG(ELogP1, "CSuplFsmEventStore::GetPrivacyResp() End\n");
	return ret;
	}

/** Returns pointer to the stored position information
*/
TPositionInfoBase* CSuplFsmEventStore::PositionL()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::PositionL() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmEventStore::PositionL() End\n");
	return iPosition;
	}

/** Store an assistance data request mask
*/
void CSuplFsmEventStore::StoreEvent(const TLbsAssistanceDataGroup& aAssistDataMask)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() Begin\n");
	iAssistanceDataStored = ETrue;
	iAssistanceDataMask = aAssistDataMask;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() End\n");
	}

/** Store a SUPL Message
*/
TInt CSuplFsmEventStore::StoreEvent(const CSuplMessageBase* aSuplMessage)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() Begin\n");
	//TInt err = KErrOverflow;
	TInt err = iMessageQueue.Insert(aSuplMessage,0);
	
	//if (iMaxEntries > iMessageQueue.Count())
	//	{
	//	TRAP(err, iMessageQueue.InsertL(aSuplMessage,0));
	//	}

	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() End\n");
	return err;
	}

/** Store a positioning protocol payload
*/
TInt CSuplFsmEventStore::StoreEvent(const CSuplPosPayload* aPosPayload)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() Begin\n");
	//TInt err = KErrOverflow;
	TInt err = iPayloadQueue.Insert(aPosPayload,0);
	//if (iMaxEntries > iPayloadQueue.Count())
	//	{
	//	TRAP(err, iPayloadQueue.InsertL(aPosPayload,0));
	//	}
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() End\n");
	return err;
	}

/** Store options of a location request received from LBS
*/
TInt CSuplFsmEventStore::StoreEvent(const TLbsNetPosRequestOptionsBase& aOptions)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() Begin\n");

	if (iLocationRequestStored)
		{
		delete iLocRequestOptions;
		iLocationRequestStored = EFalse;
		}

	switch (aOptions.ClassType())
		{
			case ELbsNetPosRequestOptionsClass: //Expected from LBS in Cell Based MO-LRs
				{
				TLbsNetPosRequestOptions* optionsPtr = new TLbsNetPosRequestOptions();
				if (optionsPtr)
					{
					*optionsPtr = static_cast<const TLbsNetPosRequestOptions&>(aOptions);
					iLocRequestOptions = optionsPtr;
					iLocationRequestStored = ETrue;
					}
				}
			break;
			
			case ELbsNetPosRequestOptionsAssistanceClass: //Expected from LBS in AGPS MO-LRs
				{
				TLbsNetPosRequestOptionsAssistance* optionsAssistPtr = new TLbsNetPosRequestOptionsAssistance();
				if (optionsAssistPtr)
					{
					*optionsAssistPtr = static_cast<const TLbsNetPosRequestOptionsAssistance&>(aOptions);
					iLocRequestOptions = optionsAssistPtr;
					iLocationRequestStored = ETrue;
					}
				}
			break;
			
			default://Intentionally empty
			break;
		}
	
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() End\n");
	if (iLocationRequestStored)
		return KErrNone;
	else 
		return KErrNoMemory;
	}

/** Store source and reason for cancelling the procedure
*/
void CSuplFsmEventStore::StoreEvent(const CSuplFsmSessionBase::TCancelSource& aSource, const CSuplFsmSessionBase::TCancelReason& aReason)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() Begin\n");
	iCancelSource = aSource;
	iCancelReason = aReason;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() End\n");
	}


/** Store local IP address assigned to the connection
for this session. Needed for sending in the SetSessionID
*/
void CSuplFsmEventStore::StoreEvent(const TInetAddr& aAddress)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() Begin\n");
	iAddress = aAddress;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() End\n");
	}
	
	
/** Store a position provided by LBS
*/
void CSuplFsmEventStore::StoreEventL(const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEventL() Begin\n");
	// Delete old position if present
	iPositionStored = EFalse;
	if (iPosition)
		{
		delete iPosition;
		iPosition = NULL;	
		}
	if (aPosInfo.PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass))
        {
        TPositionExtendedSatelliteInfo* posPtr = new(ELeave)TPositionExtendedSatelliteInfo();
        if (posPtr)
            {
            *posPtr = static_cast<const TPositionExtendedSatelliteInfo&>(aPosInfo);
            iPosition = posPtr;
            iPositionStored = ETrue;    
            }
        }
	else if (aPosInfo.PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass))
		{
		TPositionSatelliteInfo* posPtr = new(ELeave)TPositionSatelliteInfo();
		if (posPtr)
			{
			*posPtr = static_cast<const TPositionSatelliteInfo&>(aPosInfo);
			iPosition = posPtr;
			iPositionStored = ETrue;	
			}
		}
	else if (aPosInfo.PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass))
		{
		TPositionCourseInfo* posPtr = new(ELeave)TPositionCourseInfo();
		if (posPtr)
			{
			*posPtr = static_cast<const TPositionCourseInfo&>(aPosInfo);
			iPosition = posPtr;
			iPositionStored = ETrue;	
			}		
		}
	else if (aPosInfo.PositionClassType() == (EPositionInfoClass))
		{
		TPositionInfo* posPtr = new(ELeave)TPositionInfo();
		if (posPtr)
			{
			*posPtr = static_cast<const TPositionInfo&>(aPosInfo);
			iPosition = posPtr;
			iPositionStored = ETrue;	
			}		
		}
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEventL() End\n");
	}

/** Store the fact that a SUPL END has ended the session unexpectedly
*/
void CSuplFsmEventStore::SetUnexpectedSuplEnd()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::SetUnexpectedSuplEnd() Begin\n");
	iUnexpectedSuplEndReceived =  ETrue;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::SetUnexpectedSuplEnd() End\n");
	}

/** Check if a SUPL END has ended the session unexpectedly
*/
TBool CSuplFsmEventStore::UnexpectedSuplEndStored()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::UnexpectedSuplEndStored() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmEventStore::UnexpectedSuplEndStored() End\n");
	return iUnexpectedSuplEndReceived;
	}

/** Store the fact that a positioning session occurred
*/
void CSuplFsmEventStore::SetPosSessionConducted()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::SetPosSessionConducted() Begin\n");
	iPosSessionConducted =  ETrue;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::SetPosSessionConducted() End\n");
	}

/** Check if a SUPL END has ended the session unexpectedly
*/
TBool CSuplFsmEventStore::PosSessionConducted()
	{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::PosSessionConducted() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmEventStore::PosSessionConducted() End\n");
	return iPosSessionConducted;
	}


void CSuplFsmEventStore::StoreEvent(const RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
				  const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea)
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() Begin\n");
	iNetworkDataStored = ETrue;
	iNetworkInfo = aNetworkInfo;
	iLocationArea = aLocationArea;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() End\n");
}

void CSuplFsmEventStore::StoreEvent(const RMobilePhone::TMobilePhoneSubscriberId& aImsi)
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() Begin\n");
	iImsiStored = ETrue;
	iImsi = aImsi;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() End\n");
}

void CSuplFsmEventStore::StoreEvent(const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo)
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() Begin\n");
	iCellInfoStored = ETrue;
	iCellInfo = aCellInfo;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() End\n");
}

void CSuplFsmEventStore::StoreEvent(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse)
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() Begin\n");
	iPrivacyRespStored = ETrue;
	iPrivacyResp = aResponse;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreEvent() End\n");
}


void CSuplFsmEventStore::StoreExternalLocationRequest(const TLbsNetPosRequestPrivacy& aPrivacy,
													  const TLbsExternalRequestInfo& aRequestor,
													  const TLbsNetPosRequestQuality& aQuality,
													  const TLbsNetPosRequestMethod& aPosMethod,
													  const TDesC8& aVer)
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreExternalLocationRequest() Begin\n");
	iPrivacyInfo = aPrivacy;
	iRequestorInfo = aRequestor;
	iInitQuality = aQuality;
	iInitPosMethod = aPosMethod;
    iVer.Copy(aVer);
	SUPLLOG(ELogP1, "CSuplFsmEventStore::StoreExternalLocationRequest() End\n");
}

void CSuplFsmEventStore::SetPositioningEnded(const TBool& aPositioningEnded)
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::SetPositioningEnded() Begin\n");
	iPositioningEnded = aPositioningEnded;
	SUPLLOG(ELogP1, "CSuplFsmEventStore::SetPositioningEnded() End\n");
}

TBool CSuplFsmEventStore::PositioningEnded()
{
	SUPLLOG(ELogP1, "CSuplFsmEventStore::PositioningEnded() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmEventStore::PositioningEnded() End\n");
	return iPositioningEnded;
} 

// ---------------- METHODS THAT ARE COMMON TO THE CONCRETE STATE MACHINES ---------------------------

/** Start SUPL POS timer
The state handler calls this method when it sends a SUPL POS INIT.
A SUPL POS is expected before the timer expires.
*/
void CSuplFsmSessionBase::StartSuplPosTimer()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StartSuplPosTimer() Begin\n");
	iSuplProtTimer->EventAfter(TTimeIntervalSeconds(KSuplPosTimerDurationInSecTimerDurationInSec), KSuplPosTimerEvent);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StartSuplPosTimer() End\n");
	}

/** Start SUPL END timer
The state handler calls this when it has sent the last SUPL POS.
A SUPL END is expected before the timer expires.
*/
void CSuplFsmSessionBase::StartSuplEndTimer()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StartSuplEndTimer() Begin\n");
	iSuplProtTimer->EventAfter(TTimeIntervalSeconds(KSuplEndTimerDurationInSecTimerDurationInSec), KSuplEndTimerEvent);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StartSuplEndTimer() End\n");
	}

/** Start the connection timer
The state handler calls this when it has sent a connection request
to the Connection Manager and requires a result before the timer expires.
*/
void CSuplFsmSessionBase::StartConnectionTimer()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StartConnectionTimer() Begin\n");
	iSuplProtTimer->EventAfter(TTimeIntervalSeconds(KSessionConnectedTimerDurationInSec), KSessionConnectedTimerEvent);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::StartConnectionTimer() End\n");
	}

void CSuplFsmSessionBase::SetSessionInProgress(TBool aSessionInProgress)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SetSessionInProgress() Begin\n");
	iSessionInProgress = aSessionInProgress;		
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::SetSessionInProgress() End\n");
	}
	
/** Timer callback error handler.
This is called if the timer expiry callback leaves.
@param aTimerId The timer event identifier.
@param aError Error value.
*/
TInt CSuplFsmSessionBase::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::OnTimerEventL() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::OnTimerEventL() End\n");
	return aError;
	}
	
/** Retrieve current MO-Lr state
@return TMoLrState Current MO-LR state
*/
CSuplFsmSessionBase::TSuplProcedureState CSuplFsmSessionBase::CurrentState()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::CurrentState() Begin\n");
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::CurrentState() End\n");
	return iCurrentState;
	}


//----------------------------- EVENTS FROM CONNECTION MANAGER (Network) ------------------------
	
/** Handle a SUPL message fromt the Connection Manager
@param aSuplMessage  SUPL message received from network (ownership transferred to this state machine)
*/
void CSuplFsmSessionBase::MessageReceived(CSuplMessageBase* aSuplMessage)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::MessageReceived() Begin\n");

	iSuplProtTimer->Cancel();
	
	StoreSuplMessage(aSuplMessage);
	// A new SUPL message always prompts a transition if one is not
	// already underway.
	PerformTransition();
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::MessageReceived() End\n");
	}

/** Handle a connection notification from the Connection Manager
*/
void CSuplFsmSessionBase::Connected(const TInetAddr& aIpAddress)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::Connected() Begin\n");
	// Kill the timer related to connection creation
	iSuplProtTimer->Cancel();
	SetSessionConnected(ETrue);
	
	// Keep the IP address (may be used later in 
	// SET session ID of SUPL messages)
	iEventStore->StoreEvent(aIpAddress);
	
	// This event only needs handling while in state EStateNetConnectionStarted
	if (iCurrentState == EStateNetConnectionStarted)
		{
		PerformTransition();
		}
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::Connected() End\n");
	}

/** Handle a connection error notification from the Connection Manager

The errors notified by the connection manager are: connection unavailable,
insufficient security or error asn1 (encoding/decoding). The first two
imply that the connection is down.

MSuplConnectionManagerObserver::TSuplConnectionError
*/
void CSuplFsmSessionBase::ConnectionError(const TSuplConnectionError& aError)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::ConnectionError() Begin\n");
	TBool suplEndReceived = EFalse;
	if (aError != MSuplConnectionManagerObserver::EDecodeMessageFailed)
		SetSessionConnected(EFalse);
	
	// If this ConnectionError has happened after a SUPL END, it must
	// be due to a normal disconnection initiated by the SUPL server and
	// it can be safely ignored, which allows the handling of a position
	// that may be present in the SUPL END.
	const CSuplMessageBase* msg = NULL;
	TBool messageAvailable = iEventStore->IsSuplMessageStored(msg);
	if (messageAvailable)
		{
		CSuplMessageBase* message = const_cast<CSuplMessageBase*> (msg);	
		if (CSuplMessageBase::ESuplEnd == message->MessageType())
			{
			suplEndReceived = ETrue;	
			}
		}
		
	if (!suplEndReceived)
		{
		if (aError == MSuplConnectionManagerObserver::EDecodeMessageFailed)
			{
			CancelMachine(ECancelNetwork, EReasonParsingError);
			}
		else
			{
			CancelMachine(ECancelNetwork, EReasonDisconnected);
			}
		}
		
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::ConnectionError() End\n");
	}

//----------------------------- EVENTS FROM POSITIONING STATE MACHINE (RRLP) ------------------------

void CSuplFsmSessionBase::PositioningSessionEnded()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PositioningSessionEnded() Begin\n");
	iEventStore->SetPositioningEnded(ETrue);
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::PositioningSessionEnded() End\n");	
	}

//---------------------------------------------------------------------------------------------------------------------
// --------------------------------         STATE CHANGE METHODS          ---------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

/** Decide next state from EStatePosInitSent
@return TBool ETrue if the state has changed
*/
TBool CSuplFsmSessionBase::DetermineStateFromPosInitSent()
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::DetermineStateFromPosInitSent() Begin\n");
	TBool stateChanged = ETrue;
 	
	//Determine what SUPL Message was received.
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
				SUPLLOG(ELogP1, "CSuplFsmSessionBase::DetermineStateFromPosInitSent() Next State: EStatePositionReceived\n");
				}
			else
				{
				// Unexpected session termination.
				// Remember this fact before switching state.
				iEventStore->SetUnexpectedSuplEnd();
				iCurrentState = EStateLbsSessionEnded;
				SUPLLOG(ELogP1, "CSuplFsmSessionBase::DetermineStateFromPosInitSent() Next State: EStateLbsSessionEnded\n");
				SetMachineAsNotCancellable();
				}
			SetSessionInProgress(EFalse);
			}
		else if (CSuplMessageBase::ESuplPos == message->MessageType())
			{
			iCurrentState = EStatePositioningInProgress;
			SUPLLOG(ELogP1, "CSuplFsmSessionBase::DetermineStateFromPosInitSent() Next State: EStatePositioningInProgress\n");
			}
		else
			{
			// Unexpected SUPL Message was received
			//
			CancelMachine(ECancelSuplProt,EReasonUnexpectedMessage);
			}
		}
	else	
		{
		stateChanged = EFalse;
		}
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::DetermineStateFromPosInitSent() End\n");
	return stateChanged;
	}

/** Decide next state from EStatePositioningInProgress
@return TBool ETrue if the state has changed
@param aForceRedo, indicates that the states entry action must be re-performed when there has been no change in state
*/
TBool CSuplFsmSessionBase::DetermineStateFromPositioningInProgress(TBool& aForceRedo)
	{
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::DetermineStateFromPositioningInProgress() Begin\n");
	TBool stateChanged = ETrue;
	aForceRedo = ETrue;

	//Determine what SUPL Message was received.
	//
	const CSuplMessageBase* msg = NULL;

	TBool messageAvailable = iEventStore->IsSuplMessageStored(msg);

	if (messageAvailable)
		{
		CSuplMessageBase* message = const_cast <CSuplMessageBase*> (msg);
		if (CSuplMessageBase::ESuplEnd == message->MessageType())
			{
			iSessionInProgress = EFalse;
			CSuplEnd *suplEnd = static_cast <CSuplEnd*> (message);
			
			if (suplEnd->PositionPresent())
				{
				iCurrentState = EStatePositionReceived;
					
				// Remember that the position was received
				// after a positioning session (Terminal Assisted).
				iEventStore->SetPosSessionConducted();
				
				SUPLLOG(ELogP1, "CSuplFsmSessionBase::DetermineStateFromPositioningInProgress() Next State: EStatePositionReceived\n");
				}
			else
				{
				// A SUPL END is expected in this state
				// but if the SUPL END contains a reason
				// code, that is indicative of an error.
				if(suplEnd->StatusCodePresent())
					{
					iEventStore->SetUnexpectedSuplEnd();
					}
				iCurrentState = EStatePosSessionEnded;
				SUPLLOG(ELogP1, "CSuplFsmSessionBase::DetermineStateFromPositioningInProgress() Next State: EStatePosSessionEnded\n");
				SetMachineAsNotCancellable();
				}
			SetSessionInProgress(EFalse);
			}
		else if (CSuplMessageBase::ESuplPos != message->MessageType()) 
			{
			// Unexpected SUPL Message was received
			//
			CancelMachine(ECancelSuplProt,EReasonUnexpectedMessage);
			}
		}
	else
		{
		stateChanged = EFalse;	
		}
	SUPLLOG(ELogP1, "CSuplFsmSessionBase::DetermineStateFromPositioningInProgress() End\n");
	return stateChanged;	
	}
