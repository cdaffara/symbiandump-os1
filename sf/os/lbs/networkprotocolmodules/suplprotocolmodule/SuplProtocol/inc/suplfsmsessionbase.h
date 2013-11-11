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
// This file defines the base class for SUPL protocol state machines.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef __SUPLFSMSESSIONBASE_H__
#define __SUPLFSMSESSIONBASE_H__

#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsloccommon.h>
#include "lbstimer.h"
#include <etelmm.h>
#include "suplmessagebase.h"
#include "suplpospayload.h"
#include "suplconnectionmanager.h"
#include "suplpositioningprotocolfsm.h"


//-----------------------------------------------------------------------------
// State Machine Base Class
//-----------------------------------------------------------------------------

class CSuplFsmEventStore;
class CSuplStateHandlerBase;
class MSuplProtocolManagerObserver;

/** KSuplPosTimerEvent
Identity of timer for when SUPL POS is expected from the network
*/
const TInt KSuplPosTimerEvent = 0x200;

/** KSuplPosTimerDurationInSecTimerDurationInSec
Timer duration for when SUPL POS is expected from the network
after having sent out a SUPL POS INIT.
*/
const TInt KSuplPosTimerDurationInSecTimerDurationInSec = 10;

/** KSuplEndTimerEvent
Identity of timer for when SUPL END is expected from the network
*/
const TInt KSuplEndTimerEvent = 0x400;

/** KSuplEndTimerDurationInSecTimerDurationInSec
Timer duration for when SUPL END is expected from the network
after having sent out the last SUPL POS.
*/
const TInt KSuplEndTimerDurationInSecTimerDurationInSec = 10;

/** KSessionConnectedTimerEvent
Identity of timer for when connection confirmation expected from Connection Manager
*/
const TInt KSessionConnectedTimerEvent = 0x300;

/** KSessionConnectedTimerDurationInSec
Timer duration for when connection confirmation expected from Connection Manager
*/
const TInt KSessionConnectedTimerDurationInSec = 15;


/** SUPL State Machine observer mixin definition.
This class used by the SUPL state machines to communicate with
their observer, the Protocol Manager.
*/
class MSuplFsmSessionObserver
{

public:

	virtual MSuplProtocolManagerObserver& Gateway() = 0;
	virtual CSuplConnectionManager& ConnectionManager() = 0;
	virtual const TLbsNetSessionId& NewSessionId() = 0;
	virtual void ProcedureCompleteInd(const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aService) = 0;
	virtual void LocationReq(const TLbsNetSessionId& aSessionId, 
					 		 const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aType, 
					 		 const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod) = 0;
	virtual void ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, 
							   		   const RLbsAssistanceDataBuilderSet& aData,const TInt& aReason,
							  		   const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aService) = 0;
	virtual void PrivacyReq(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestPrivacy& aPrivacy,
							const TLbsExternalRequestInfo& aRequestInfo) = 0;
};


/** Base class definition for SUPL protocol state machines.
This class provides support for common features of all state machines.

A significant aspect incorporated in this base class is the active object
mechanism for performing state transitions. This feature is used to allow 
event-handling methods to return inmediately after storing the event, relying
on later execution of RunL for the real handling of the events.
This approach keeps the Network Gateway responsive to new events.
Another benefit of the Active Object paradigm is that it provides state
machines with the ability to perform autonomous and asynchronous actions.

In RunL, state transitions are followed by the execution of the entry actions of the
next state. Each state's entry actions are designed to be completed in a short time.
This staged approach ensures that lengthy protocol module
activity is broken down into shorter steps, thereby releasing the active
scheduler to process other active objects in the NG thread. A self-completion
approach is employed to achieve this. @see CSuplFsmSessionBase::DoTransitionStage()

The base class includes various attributes that describe protocol activity,
together with access methods used to retrieve the value of these attributes.

External events and their associated parameters are kept in the Event Store
to be used for the length of the sessio as required. @see CSuplFsmEventStore
*/
NONSHARABLE_CLASS(CSuplFsmSessionBase) : public CActive, public MSuplConnectionManagerObserver,
										 public MSuplPositioningProtocolFsmObserver,
										 public MLbsCallbackTimerObserver
	{
public:

	/** State machine protocol state.
	This defines the general protocol state for state machines.
	*/
	enum TMachineState
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

	/** SUPL specific states
	These identify the current state within the
	MO-LR or MT-LR protocol procedure.
	*/
	enum TSuplProcedureState
		{
		/** Idle */
		EStateProcedureNull,
		
		// ----------------- MOLR-ONLY STATES ----------------------------------------
		/** Trigger: Connected to SLP. Action: SUPL START sent*/
		EStateStartSent,
		/** Trigger: SUPL RESPONSE received. Action: Request sent to LBS.*/
		EStateResponseReceived,

		// ----------------- MTLR-ONLY STATES ---------------------------------------
		/** Trigger: SUPL INIT message received. Action: Send privacy request to LBS*/
		EStateSuplInitReceived,
		
		//------------------ STATES COMMON TO MOLR AND MTLR --------------------------
		/** Trigger: Privacy Response received from LBS allowing MTLR or self location requested.
		    Action: Initiate a network session */
		EStateNetConnectionStarted,
		/** Trigger: LBS requested Assistance data or provided a result. Action: SUPL POS INIT sent */
		EStatePosInitSent,
		/** Trigger: SUPL END with position received. Action: forward received position to LBS */
		EStatePositionReceived,
		/** Trigger: SUPL POS received. Actions: relay messages between Positioning FSM, LBS and network */
		EStatePositioningInProgress,
		/** Trigger: errors or client cancel while connected. Actions: send SUPL END*/
		EStateSuplSessionEnded,
		/** Trigger: session has terminated. Actions: instruct positioning FSM to halt*/
		EStatePosSessionEnded,
		/** Trigger: session has terminated. Actions: termination of LBS session */
		EStateLbsSessionEnded,
		/** Trigger: session has terminated. Action: termination of network session */
		EStateNetConnectionClosed
		};

	/** State machine cancel source.
	This defines the source of a cancellation.
	*/
	enum TCancelSource
		{
		/** Not cancelling */
		ECancelNone,
		/** The LBS client cancelled the procedure */
		ECancelClient,
		/** A network error occurred */
		ECancelNetwork,
		/** A positioning protocol error occurred*/
		ECancelPosProt,
		/** An SUPL protocol state machine encountered an error*/
		ECancelSuplProt,
		/** State machine is closing down (destruction)*/
		ECancelClosing
		};

	enum TCancelReason
		{
		/** Reason not specified */
		EReasonNone,
		/** A protocol has received an unexpected message */
		EReasonUnexpectedMessage,
		/** A timer has expired */
		EReasonTimerExpiry,
		/** The connection is down */
		EReasonDisconnected,
		/** The connection manager can't connect to remote server
		with LBS's minimun security requirements*/ 
		EReasonInsuficcientSecurity,
		/** A SUPL server could not be found in the host settings DB*/
		EReasonSlpSettingsMissing,
		/** Parsing error decoding a received SUPL message */
		EReasonParsingError,
		/** Mandatory data missing in received SUPL message*/
		EReasonDataMissing,
		/** A value in a received message is out of range or corrupted*/
		EReasonUnexpectedDataValue,
		/** No supported positioning methods in received SUPL message*/
		EReasonMethodMismatch,		
		/** Received SUPL message contains invalid sessionID*/
		EReasonInvalidSessId,
		/** Mandatory info for outgoing message is not available */
		EReasonDataUnavailable,
		/** LBS rejected a privacy request*/
		EReasonPrivacyRejected,
		/** LBS wants a privacy request to be ignored*/
		EReasonPrivacyIgnored,
		/** LBS declares itself unable to calculate the position **/
		EReasonFutilePosCalc
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
		/** Enter new state */
		ETransitionEnter
		};


public:

	virtual ~CSuplFsmSessionBase();


	// Methods that can be overriden in derived classes
	//
	/** Complete state transition.
	Derived classes implement a method to perform actions after a state transition.
	*/
	virtual void PostTransition() = 0;

	/** Change state.
	Derived classes implement a method to select the next state following an event.
	The next state selected may (rarely) be the same as the current state, in which case
	parameter aForceRedo indicates that the entry actions for the state must be
	performed again. 
	*/
	virtual TBool SelectNextState(TBool& aForceRedo) = 0;

	/** Cancel the active procedure.
	Derived classes implement a method to cancel the active procedure.
	*/
	virtual void CancelProcedure() = 0;

	/** A location response from LBS has been received.
	Derived classes implement a method to handle this response, with due
	consideration of the prevailing state machine activity.
	*/
	virtual void LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo) = 0;
	virtual void AssistanceDataReq(const TLbsAssistanceDataGroup& aFilter) = 0;
	
	// Derived from MSuplConnectionManagerObserver. Implemented by concrete state machines
	// with due consideration of the prevailing state machine activity.
	void Connected(const TInetAddr& aIpAddress);
	void ConnectionError(const TSuplConnectionError& aError);
	void MessageReceived(CSuplMessageBase* aSuplMessage);

	// Derived from MSuplPositioningProtocolFsmObserver. 
    virtual void ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, TInt aReason) = 0;
	virtual void ProcessPositioningRequest(const TLbsNetPosRequestQuality& aQuality,
										   const TLbsNetPosRequestMethod& aPosMethod) = 0;					
	virtual void PositioningPayloadToNetwork(const CSuplPosPayload* aPositioningPayload) = 0;
	virtual void PositioningProtocolError(const TInt& aError) = 0;
	void PositioningSessionEnded();

	// Methods called by State Handlers and Protocol Manager
	virtual void CompleteProcedure() = 0;
	virtual const TLbsNetSessionId& LbsSessionId() const;
	void StartConnectionTimer();
	void StartSuplPosTimer();
	void StartSuplEndTimer();
	void SetSessionInProgress(TBool aSessionInProgress);
	CSuplFsmSessionBase::TSuplProcedureState CurrentState();
	CSuplSessionId* MessageSessionId();
	void SetMessageSessionId(CSuplSessionId* aMessageSessionId);
	virtual TInt GetHostId(TLbsHostSettingsId& aHostId) = 0;
	void SetSessionId(const TLbsNetSessionId& aSessionId);
	CSuplPositioningProtocolFsm* PositioningFsm();
	const TLbsNetSessionId& SessionId() const;
	TBool IsSessionConnected();
	virtual void CancelMachine(const TCancelSource& aCancelSource,const TCancelReason& aReason);	
	MSuplFsmSessionObserver& Observer();
	TMachineState State() const;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService LocReqType() const;
	TBool PosMethodSupported(const TLbsNetPosRequestMethod& aPosMethods, TLbsNetPosMethod& aSelectedMethod, const TLbsNetPosCapabilities& aLbsCapabilities);
	
public:

	// Methods for storing and retrieving event-related data from the store
	//
	TInt StoreLocationRequest(const TLbsNetPosRequestOptionsBase& aOptions);
	void StoreAssistanceDataRequest(const TLbsAssistanceDataGroup& aFilter);
	void StoreSuplMessage(const CSuplMessageBase* aMessage);
	void StorePosPayload(const CSuplPosPayload* aPayload);
	void StoreCancelInfo(const CSuplFsmSessionBase::TCancelSource& aSource,
							const CSuplFsmSessionBase::TCancelReason& aReason);
	void StoreNetInfo(const RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
					  const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea);
	void StoreMccMnc(TUint aMcc, TUint aMnc);
	void StoreMsisdn(const TDesC& aTelNumber);
	void StoreCellInfo(const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo);
	void StorePositioningEnded(TBool aPosEnded);

	TBool RetrieveLocationRequestOptions(TLbsNetPosRequestOptions& aOptions);
	TBool RetrieveAssistanceDataRequest(TLbsAssistanceDataGroup& aAssitDataMask);
	void RetrieveStoredNetworkInfo(RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
								   RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea);
	TBool RetrieveStoredCellInfo(RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo);
	const CSuplMessageBase* RetrieveSuplMessage();
	const CSuplPosPayload* RetrievePosPayload();
	const TInetAddr& RetrieveLocalIpAddress();
	void RetrieveCancelInfo(CSuplFsmSessionBase::TCancelSource& aSource,
							CSuplFsmSessionBase::TCancelReason& aReason);
	void RetrievePrivacyAdvice(TLbsNetPosRequestPrivacy& aPrivacy);
	void RetrieveRequestorInfo(TLbsExternalRequestInfo& aRequestor);
	void RetrieveInitPosMethod(TLbsNetPosRequestMethod& aPosMethod);
	void RetrieveInitQuality(TLbsNetPosRequestQuality& aQuality);
	void RetrieveVer(TDes8& aVer);
	TPositionInfoBase* RetrievePositionL();
	TBool RetrievePositioningSessionEnded();
							
	TBool IsUnexpectedSuplEndStored();
	TBool IsAssistanceDataRequestStored();
	TBool IsNetworkInfoAvailable();
	TBool IsPositionStored();
	TBool PosSessionConducted();
	inline const TDesC* Msisdn() const;

protected:

	CSuplFsmSessionBase(MSuplFsmSessionObserver& aObserver);
	void InitialiseMachineBase();
	void CompleteMachineBase();
	void  SetSessionConnected(TBool aSessionConnected); 

	TBool IsCancelPending() const;
	void SetMachineAsCancelling();
	void SetMachineAsNotCancellable();
	TInt GenerateHostId(TLbsHostSettingsId& aHostId);
	
	TInt OnTimerError(TInt aTimerId, TInt aError);
	
	TBool DetermineStateFromPosInitSent();
	TBool DetermineStateFromPositioningInProgress(TBool& aForceRedo);
	
	void PerformTransition();

private:

	void ResetSessionId();
	
	void DoTransitionStage();
	void CheckForStoredEvents();
	TBool NextStateEntryActionsL();
	void GenerateHostIdL(TLbsHostSettingsId& aHostId);

	// CActive derived methods
	void RunL();
	void RunError();
	void DoCancel();
	
protected:

	/** Reference to State machine observer
	*/
	MSuplFsmSessionObserver& iObserver;
	
	/** Local store for buffering external events
		that are likely to trigger state transitions.
	*/
	CSuplFsmEventStore* iEventStore;
	
	/** Pointer to state handler.
	This is owned by the derived classes that create state handlers.
	*/
	CSuplStateHandlerBase* iStateHandler;
	
	/** Current machine state.
	*/
	TMachineState iProtocolState;
	
	/** Current state of the SUPL procedure (MO-LR or MT-LR)
	*/
	TSuplProcedureState iCurrentState;
	
	/** Session ID.
	Session Id at the beginning of the session. This number is always used
	in messages sent to the network.
	*/
	TLbsNetSessionId iSessionId;
		
	/** State transition stage enumeration.
	*/
	TTransitionStage iTransitionState;
	
	/** Flag to indicate if state machine is to be cancelled.
	*/
	TBool iIsCancelPending;
	
	/** Flag to indicate that the machine cannot be cancelled. Derived
	state machines set this variable to true when they have reached
	late stages of the procedure and no longer makes sense to cancel.
	*/
	TBool iCancelNoLongerAllowed;
	
	/** Location request type.
	*/
	MLbsNetworkProtocolObserver::TLbsNetProtocolService iLocReqType;

	/** Flag to show that this session has an active
	    connection with a server
	*/
	TBool iSessionConnected;

	/** Positioning Protocol State Machine (RRLP)
	*/
	CSuplPositioningProtocolFsm* iPositioningProtocol;

	/** Assistance Data Builder 
	Passed to the positioning FSM for it to set assistance data in.
	*/
	RLbsAssistanceDataBuilderSet iAssistanceDataBuilderSet;
	
	/** The id of default settings generated using MCC and MNC*/
	TLbsHostSettingsId iDefSettingsId; 
	
	/** Store of settings for available SLPs to contact */
	CLbsHostSettingsStore* iSlpSettingsStore;
	
	/** When this flag is true, all the messages sent to LBS must contain
	the session ID in iReplacementSessionId, otherwise iSessionId.
	*/
	TBool iSessionIdReplaced;
	
	/** LBS replacement session ID.
	Session Id to be used in all communications with LBS when iSessionIdReplaced
	is true. Changing the session Id halfway through an MOLR session may 
	be required due to conflicting self-locate requests.
	*/
	TLbsNetSessionId iReplacementSessionId;	
	
	/** Timer used for monitoring arrival of 
	SUPL messages.
	*/
	CLbsCallbackTimer* iSuplProtTimer;
		
	/** Flag that is set to true from the moment a SUPL START
    is sent or a SUPL INIT is received until the moment a
    SUPL END is received or sent.
    */
	TBool iSessionInProgress;
	
	/** Used to keep the SLP session Id */
	CSuplSessionId* iMessageSessionId;

	/** The MCC of the home network */
	TUint iMcc;
	
	/** The MNC of the home network */
	TUint iMnc;
	
	/** The telephone number */
	HBufC* iTelNumber;
	};


//-----------------------------------------------------------------------------
// State Machine Event Store Class
//-----------------------------------------------------------------------------

/** State machine event store.

This is used to hold external events that require asynchronous actions until the
state machine is next in a position to be able to service each event, at which
point the event is removed from the store.

All the memory required is allocated on creation. Dynamic arrays are used for
convenience as they provide a useful inteface but they are prevented from allocating
memory at runtime.
*/
NONSHARABLE_CLASS(CSuplFsmEventStore) : public CBase
{

public:
	static CSuplFsmEventStore* NewL(TInt aMaxEntries);
	~CSuplFsmEventStore();

	void  ClearStore();
	TBool IsStoreEmpty();
	TBool IsAssistanceDataReqStored();
	TBool UnexpectedSuplEndStored();
	TBool IsSuplMessageStored(const CSuplMessageBase*& aSuplMessage);
	TBool IsNetworkInfoAvailable();
	TBool IsPositionStored();
	TBool PositioningEnded();
	TBool PosSessionConducted();
	
	TInt StoreEvent(const CSuplMessageBase* aSuplMessage);
	TInt StoreEvent(const CSuplPosPayload* aPosPayload);
	void StoreEvent(const TLbsAssistanceDataGroup& aAssistDataMask);
	TInt StoreEvent(const TLbsNetPosRequestOptionsBase& aOptions);
	void StoreEvent(const CSuplFsmSessionBase::TCancelSource& aSource,
					const CSuplFsmSessionBase::TCancelReason& aReason);
    void StoreEvent(const TInetAddr& aAddress);					
	void StoreEvent(const RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
					  const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea);
	void StoreEvent(const RMobilePhone::TMobilePhoneSubscriberId& aImsi);
	void StoreEvent(const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo);
	void StoreEvent(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse);
	void StoreEventL(const TPositionInfoBase& aPosInfo);
	void StoreExternalLocationRequest(const TLbsNetPosRequestPrivacy& aPrivacy,
									  const TLbsExternalRequestInfo& aRequestor,
									  const TLbsNetPosRequestQuality& quality,
									  const TLbsNetPosRequestMethod& aPosMethod,
									  const TDesC8& ver);
	void SetUnexpectedSuplEnd();
	void SetPosSessionConducted();
	void SetProxyModeRequested(const TBool& aIsProxyMode);
	void SetPositioningEnded(const TBool& aPositioningEnded);
	
	TBool GetAssistanceDataRequest(TLbsAssistanceDataGroup& aAssistDataMask);
	TBool GetLocationRequestOptions(TLbsNetPosRequestOptionsBase& aOptions);
	void GetCancelInfo(CSuplFsmSessionBase::TCancelSource& aSource,
							CSuplFsmSessionBase::TCancelReason& aReason);
	void GetNetworkInfo(RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
						RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea);
	TBool GetCellInfo(RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo);
	void GetPrivacyAdvice(TLbsNetPosRequestPrivacy& aPrivacy);
	void GetRequestorInfo(TLbsExternalRequestInfo& aRequestor);
	void GetInitPosMethod(TLbsNetPosRequestMethod& aPosMethod);
	void GetInitQuality(TLbsNetPosRequestQuality& aQuality);
	void GetVer(TDes8& aVer);
	TPositionInfoBase* PositionL();
	TBool GetPrivacyResp(CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse);
	const CSuplMessageBase* SuplMessage();
	const CSuplPosPayload* PosPayload();
	const TInetAddr& Address();


private:

	CSuplFsmEventStore(TInt aMaxEntries);
	
private:

	// Request assistance data event
	TBool iAssistanceDataStored;
	TLbsAssistanceDataGroup iAssistanceDataMask;

	// Location request event
	TBool iLocationRequestStored;
	TLbsNetPosRequestOptionsBase* iLocRequestOptions;

	// Unexpected SUPL END received
	TBool iUnexpectedSuplEndReceived;

	// Reason the procedure was cancelled (if it was)
	CSuplFsmSessionBase::TCancelSource iCancelSource;
	CSuplFsmSessionBase::TCancelReason iCancelReason;

	/** The internal queue of SUPL messages, represented by an array of pointers to messages.
	*/
	RPointerArray<const CSuplMessageBase> iMessageQueue;

	/** The internal queue of positioning payloads, represented by an array of pointers to payloads.
	*/
	RPointerArray<const CSuplPosPayload> iPayloadQueue;

	/** The maximum number of entries permitted in the queues (fixed at construction).
	*/
	TInt iMaxEntries;

	/** Local IP adddress provided by the Connection Manager
	*/
	TInetAddr iAddress;

	/* Network data required for Location ID in SUPL messages
	*/
	TBool iNetworkDataStored;
	RMobilePhone::TMobilePhoneNetworkInfoV1 iNetworkInfo;
	RMobilePhone::TMobilePhoneLocationAreaV1 iLocationArea;
	
	/* IMSI needed by SUPL state machine for
	autoconfiguration of SLP
	*/	
	TBool iImsiStored;
	RMobilePhone::TMobilePhoneSubscriberId iImsi;
	
	/* Cell info with Timing Advance required for
	enhanced cell-id 
	*/
	TBool iCellInfoStored;
	RMobilePhone::TMobilePhoneCellInfoV9 iCellInfo;	
	
	/** LBS's response to a privacy request
	*/
	TBool iPrivacyRespStored;
	CLbsNetworkProtocolBase::TLbsPrivacyResponse iPrivacyResp;

	/* Parameters received from a SUPL INIT
	*/
	TLbsNetPosRequestPrivacy iPrivacyInfo;
	TLbsExternalRequestInfo iRequestorInfo;
	TLbsNetPosRequestQuality iInitQuality;
	TLbsNetPosRequestMethod iInitPosMethod;
	// HMAC of the received SUPL INIT message
	TBuf8<8> iVer;
	
	// Position received from LBS during an MTLR
	// before a positioning session (SUPL POS/RRLP)
	// has started (a position cached by LBS that
	// satisfies SUPL INIT QoP)
	TBool iPositionStored;
	TPositionInfoBase* iPosition;
	
	TBool iPositioningEnded;
	TBool iPosSessionConducted;
};


inline const TDesC* CSuplFsmSessionBase::Msisdn() const
	{
	return iTelNumber;
	}

#endif // __SUPLFSMSESSIONBASE_H__
