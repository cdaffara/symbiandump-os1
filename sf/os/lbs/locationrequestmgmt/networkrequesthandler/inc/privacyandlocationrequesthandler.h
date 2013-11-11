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
// CPrivacyAndLocationHandler.h
// 
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef PRIVACYANDLOCATIONHANDLER_H 
#define PRIVACYANDLOCATIONHANDLER_H

#include <e32property.h>
#include "nrhpanic.h"
#include "ngmessageswitch.h"
#include "agpsinterfacehandler.h"

// Forward declarations
class TPrivLocCommonParams;
class CLbsPrivLocStateBase;
class CLbsPrivLocFsm;

// Panic Category
_LIT(KPrivLocFault,"Lbs-Nrh PrivLoc");

/**
 * 
 * Creates each of the states and sets the curent state to be idle.
 * Passes on any event to the current state, and handles requests
 * from states to change the current state.
 */	
class CPrivacyAndLocationHandler : public CBase, 
								   public MNGMessageSwitchObserver,
								   public MPrivacyHandlerObserver,
								   public MAgpsInterfaceHandlerObserver
	{
public:
	static CPrivacyAndLocationHandler* NewL(CNGMessageSwitch& aMessageSwitch,
											CLbsAdmin& aLbsAdmin,
											RLbsNetworkRegistrationStatus& aNetRegStatus);
	~CPrivacyAndLocationHandler();
	
protected:
	void ConstructL(CLbsAdmin* aLbsAdmin);
	CPrivacyAndLocationHandler(CNGMessageSwitch& aMessageSwitch, RLbsNetworkRegistrationStatus& aNetRegStatus);
	
	void ReadProtocolModuleAdminSetting();
	
	CLbsPrivLocFsm* LookupFsm(const TLbsNetSessionIdInt& aSessionId);
	CLbsPrivLocFsm* GetNewFsm(const TLbsNetSessionIdInt& aSessionId,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
			TBool aEmergency);

protected: // From MNGMessageSwitchObserver. Delegated to current state
	void OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
					   TBool aIsEmergency,
					   const TLbsExternalRequestInfo& aExternalRequestInfo,
					   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy);
	
	void OnSessionComplete(const TLbsNetSessionIdInt& aSessionId, TInt aReason);

	void OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId, 
						 const TLbsNetPosRequestMethodInt& aPosRequestMethod,
						 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						 TBool aIsEmergency,
						 const TLbsNetPosRequestQualityInt& aQuality);

	void OnNetLocResponse(const TLbsNetSessionIdInt&,
							 const TLbsNetPosRequestQualityInt&){}
	
	void OnNetLocReferenceUpdate(const TLbsNetSessionIdInt& aSessionId, 
								 const TPositionInfoBase& aPosInfo);
	
	void OnNetLocFinalUpdate(const TLbsNetSessionIdInt& aSessionId, 
									 const TPositionInfoBase& aPosInfo);

public: // From MPrivacyHandlerObserver
	void SetServerObserver(MLbsSessionObserver* aNrhPrivacyServer);
    void OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId, 
                            TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
                            TInt aResponseReason);
    void OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId);

public:
	void OnAgpsPositionUpdate(
		TInt aReason,
		const TPositionExtendedSatelliteInfo& aPosInfo,
		const TTime& aTimeStamp);

	void OnAgpsMeasurementUpdate(
		TInt aReason,
		const TPositionGpsMeasurementInfo& aPosInfo,
		const TTime& aTimeStamp);

public:
	CAgpsInterfaceHandler* AgpsHandler();
	MX3pStatusHandler& X3pStatusHandler();

private: // Allow access for friend classes (the states, the NRH, and the FSMs) 
friend class CLbsPrivLocStateBase;
friend class CLbsNetworkRequestHandler;
friend class CLbsPrivLocFsm;
	CPrivacyHandler* PrivacyHandler();
	CLbsAdmin* LbsAdmin();	
	CNGMessageSwitch* MessageSwitch();
	TPositionModuleInfoExtended::TDeviceGpsModeCapabilities DeviceGpsModeCaps();
	CLbsAdmin::TLbsBehaviourMode BehaviourMode();
	RLbsNetworkRegistrationStatus& NetworkRegistrationStatus();
	void IncrementPositioningStatus();
	void DecrementPositioningStatus();

private:
	static TBool IsSessionIdEqual(
			const TLbsNetSessionIdInt* aSessionId,
			const CLbsPrivLocFsm& aFsm);
	static TBool IsSessionTypeEqual(
			const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt* aSessionType,
			const CLbsPrivLocFsm& aFsm);
	TBool IsSpecialFeatureIntermediateFutileUpdateOn();
private:
	CPrivacyHandler* iPrivacyHandler;
	CLbsAdmin* iLbsAdmin;
	RLbsNetworkRegistrationStatus& iNetRegStatus;
	CNGMessageSwitch* iMessageSwitch;

	CAgpsInterfaceHandler* iAgpsInterface;
	
	
	TUid iProtocolModuleUid;
	
	// state machine used for progessing a emergency (MTLR or NI) - so as to avoid OOM
	CLbsPrivLocFsm* iEmergencyFsm;
	
	// Array of state machines used for progressing sessions of all types (MTLR,MOLR and NI, including emergencies)
	RPointerArray<CLbsPrivLocFsm> iFsmArray;
	
	CLbsAdmin::TLbsBehaviourMode iLbsBehaviourMode;	
	TPositionModuleInfoExtended::TDeviceGpsModeCapabilities iDeviceGpsModeCaps;	

	// Is the intermediate futile update feature switched on?
	TBool iSpecialFeatureIntermediateFutileUpdate;
	
	// this is the maximum number of non-emergency sessions allowed
	// at any one time
	TUint iMaxNumSessions;
	
	// this is the current number of active non-emergency sessions 
	TUint iNumActiveSessions;

	CLbsPrivLocFsm* iMolRFsm;
	CLbsPrivLocFsm* iX3pFsm;

	// ETrue if location management is supported
	TBool iLocationManagementSupported;
	
    // Category of the Positioning Status P&S Keys
	TUid iPosStatusCategory;
	};


//------------------------------------------------------------------------------
//
// Transport classes used to pass data across state changes
//
//------------------------------------------------------------------------------
/**
 * TPrivLocCommonParams defines the base class for the data passed between
 * states
 */
class TPrivLocCommonParams
	{
protected:
enum TType
    {
    ECommon,
    EPrivacyResponse,
    ELocationRequest,
    ELocationUpdate
    };

public:
	TPrivLocCommonParams();
	TPrivLocCommonParams(TLbsNetSessionIdInt aSessionId);

public:
	static const TPrivLocCommonParams& Cast(const TPrivLocCommonParams& aParamsBase)
		{
		if(aParamsBase.Type() != ECommon)
			{
			__ASSERT_DEBUG(EFalse, Panic(ENrhPanicBadParamType));
			}
		return static_cast<const TPrivLocCommonParams&>(aParamsBase);
		}

public:
	virtual TType Type() const
		{
		return(ECommon);
		};
		
public:
	TLbsNetSessionIdInt 			iSessionId;
	};

/*
 * TPrivLocWaitPrivResponseParams contains the data required on entry
 * to state EStateWaitPrivacyResponse
 */
class TPrivLocWaitPrivResponseParams : public TPrivLocCommonParams
	{
public:
	TPrivLocWaitPrivResponseParams();
	TPrivLocWaitPrivResponseParams(const TLbsNetSessionIdInt& aSessionId,
								   const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
								   const TLbsExternalRequestInfo& aExternalRequestInfo,
								   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy,
								   TBool aIsEmergency);

	static const TPrivLocWaitPrivResponseParams& Cast(const TPrivLocCommonParams& aParamsBase)
		{
		if(aParamsBase.Type() != EPrivacyResponse)
			{
			__ASSERT_DEBUG(EFalse, Panic(ENrhPanicBadParamType));
						
			}
		return static_cast<const TPrivLocWaitPrivResponseParams&>(aParamsBase);
		}
public: // From TPrivLocCommonParams.
	TType Type() const
		{
		return EPrivacyResponse; 
		}

public:
	TLbsExternalRequestInfo2	iExternalRequestInfo;
	TLbsNetPosRequestPrivacyInt	iNetPosRequestPrivacy;
	TBool						iIsEmergency;
	TLbsNetworkEnumInt::TLbsNetProtocolServiceInt iSessionType;
	};

class TPrivLocWaitLocationRequestParams : public TPrivLocCommonParams
	{
public:
	TPrivLocWaitLocationRequestParams();
	TPrivLocWaitLocationRequestParams(const TLbsNetSessionIdInt& aSessionId, 
									  TBool aIsEmergency, 
									  TBool aReqCancelled, 
									  TInt aPreviousStateExitInfo = KErrNone);

	static const TPrivLocWaitLocationRequestParams& Cast(const TPrivLocCommonParams& aParamsBase)
		{
		if(aParamsBase.Type() != ELocationRequest)
			{
			__ASSERT_DEBUG(EFalse, Panic(ENrhPanicBadParamType));
						
			}
		return static_cast<const TPrivLocWaitLocationRequestParams&>(aParamsBase);
		}
public: // From TPrivLocCommonParams.
	TType Type() const
		{
		return ELocationRequest; 
		}

public:
	TBool	iIsEmergency;
	TBool   iReqCancelled;
	TInt    iPreviousStateExitInfo; // e. g. KPositionQualityLoss
	};

class TPrivLocWaitLocationUpdateParams : public TPrivLocCommonParams
	{
public:
	TPrivLocWaitLocationUpdateParams();
	TPrivLocWaitLocationUpdateParams(const TLbsNetSessionIdInt& aSessionId,
									 const TLbsNetPosRequestMethodInt& aPosRequestMethod,
									 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
									 TBool aIsEmergency,
									 const TLbsNetPosRequestQualityInt& aQuality);

	static const TPrivLocWaitLocationUpdateParams& Cast(const TPrivLocCommonParams& aParamsBase)
		{
		if(aParamsBase.Type() != ELocationUpdate)
			{
			__ASSERT_DEBUG(EFalse, Panic(ENrhPanicBadParamType));
						
			}
		return static_cast<const TPrivLocWaitLocationUpdateParams&>(aParamsBase);
		}
public: // From TPrivLocCommonParams.
	TType Type() const
		{
		return ELocationUpdate; 
		}

public:
	TLbsNetworkEnumInt::TLbsNetProtocolServiceInt iSessionType;
	TBool						iIsEmergency;
	TLbsNetPosRequestQualityInt	iQuality;
	TLbsNetPosRequestMethodInt	iPosRequestMethod;
	};

//------------------------------------------------------------------------------
//
// End transport classes
//
//------------------------------------------------------------------------------


class TPrivLocStateExitData
	{
public:
enum TPrivLocStateExitReason
	{
	EExitReasonNone,
	// Normal exit from Idle state when MTLR session is started
	EExitPrivacyRequestReceived,
	// normal exit from wait for location request or Idle (for non-MTLR)
	EExitLocReqReceived, 
	// exit from wait for location update when position reeceived
	EExitLocFixReceived, 
	// exit from wait for location update when measurement data received
	EExitLocMeasurementResultsReceived, 
	// normal exit from wait for privacy response
	EExitPrivacyResponseReceived, 
	// session could be completed, cancelled or overridden by a new one	
	EExitSessionComplete, 
	// Privacy Controller cancelled a session
	EExitCancelledByPrivacyController, 
	// Can't action a position update request
	EExitBadQualityProfile, 
	// Update not received 
	EExitTimedOut,
	// The location request was 'bad', for various reasons.
	EExitBadLocationRequest
	};
public:
	TPrivLocStateExitData(){}
	TPrivLocStateExitData(TPrivLocStateExitReason aExitReason, TInt aExitInfo)
		{
		SetExitData(aExitReason, aExitInfo);
		}
public:
	void SetExitData(TPrivLocStateExitReason aExitReason, TInt aExitInfo) 	
		{
		iExitReason = aExitReason;
		iExitInfo = aExitInfo;
		}
public:
	TPrivLocStateExitReason	iExitReason;
	TInt					iExitInfo; // e.g. KErrCancel
	};


/**
 * 
 * Base class for states, not intended for instantiation.
 * Defines functions common to all states
 */
 class CLbsPrivLocStateBase	:	public CBase 
	{

public:
	virtual void OnEntry(const TPrivLocCommonParams&);
	virtual TBool OnExit();

public:
	virtual void OnMTLRRequest(
		const TLbsNetSessionIdInt&,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt,
		TBool,
		const TLbsExternalRequestInfo&,
		const TLbsNetPosRequestPrivacyInt&) {}
	
	virtual void OnSessionComplete(
		const TLbsNetSessionIdInt&, 
		TInt );

	virtual void OnNetLocRequest(
		const TLbsNetSessionIdInt&, 
		const TLbsNetPosRequestMethodInt&,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt,
		TBool,
		const TLbsNetPosRequestQualityInt&) {}
		
	virtual void OnNetLocResponse(
		const TLbsNetSessionIdInt&,
		const TLbsNetPosRequestQualityInt&) {}
	
    virtual void OnRespondNetworkLocationRequest(
    	const TLbsNetSessionIdInt&, 
        TLbsNetworkEnumInt::TLbsPrivacyResponseInt,
        TInt /*aResponseReason*/) {}

    virtual void OnCancelNetworkLocationRequest(
    	const TLbsNetSessionIdInt&);
	
	virtual void OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& aSessionId, 
		const TPositionInfoBase& aPosInfo);

	virtual void OnAgpsPositionUpdate(
		TInt /*aReason*/,
		const TPositionExtendedSatelliteInfo& /*aPosInfo*/,
		const TTime& /*aTimeStamp*/) {}

	virtual void OnAgpsMeasurementUpdate(
		TInt /*aReason*/,
		const TPositionGpsMeasurementInfo& /*aPosInfo*/,
		const TTime& /*aTimeStamp*/) {}

public:
	virtual void OnTimerEventL(TInt aTimerId);
    
protected:
	CLbsPrivLocStateBase(CLbsPrivLocFsm* aFsm);
	
protected: 
	void HandleLocRequest(const TLbsNetSessionIdInt& aSessionId, 
					const TLbsNetPosRequestMethodInt& aPosRequestMethod,
			 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
							TBool aIsEmergency,
				const TLbsNetPosRequestQualityInt& aQuality);
	void HandleSessionCancel(const TLbsNetSessionIdInt& aSessionId,TInt aReason);
	void ReadNetworkInducedAdminSetting(CLbsAdmin::TExternalLocateService& aExternalLocateService);
	CPrivacyHandler* PrivacyHandler();
	CNGMessageSwitch* MessageSwitch();
	CLbsAdmin* LbsAdmin();
	CAgpsInterfaceHandler* AgpsInterface();
	RLbsNetworkRegistrationStatus& LbsNetworkRegistrationStatus();
	TPositionModuleInfoExtended::TDeviceGpsModeCapabilities DeviceGpsModeCaps();
	CLbsAdmin::TLbsBehaviourMode BehaviourMode();	
	void IncrementPositioningStatus();

protected:	
	CLbsPrivLocFsm*			iFsm;
	};

/**
 * Idle state.
 * Handles all events originating from the Network Gateway, but 
 * does nothing with any apart from MTLR (i.e. privacy) Request 
 * and Location Request
 * Exits to states:
 *		EStateWaitPrivacyResponse
 */
class CLbsPrivLocIdleState	:	public CLbsPrivLocStateBase

	{
public:
	static CLbsPrivLocIdleState* NewL(CLbsPrivLocFsm* aFsm);
	
public:
	void OnEntry(const TPrivLocCommonParams& aStateParams);
	TBool OnExit();

	void OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
					   TBool aIsEmergency,
					   const TLbsExternalRequestInfo& aExternalRequestInfo,
					   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy);
	
	void OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId, 
						 const TLbsNetPosRequestMethodInt& aPosRequestMethod,
						 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						 TBool aIsEmergency,
						 const TLbsNetPosRequestQualityInt& aQuality);
	
	void OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& aSessionId, 
		const TPositionInfoBase& aPosInfo);

	
protected:
	CLbsPrivLocIdleState(CLbsPrivLocFsm* aFsm);
	};	

/**
 * Wait for Privacy Response state.
 * Expects an accept/reject response from the privacy handler which 
 * it passes on to the network gateway.
 * Also handles all events originating from the Network Gateway, which may 
 * teminate the current session.
 * Exits to states:
 */
class CLbsPrivLocWaitPrivRespState	:	public CLbsPrivLocStateBase 
	{
public:
	static CLbsPrivLocWaitPrivRespState* NewL(CLbsPrivLocFsm* aFsm);
	
public:
	void OnEntry(const TPrivLocCommonParams& aStateParams);
	TBool OnExit();

	void OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
					   TBool aIsEmergency,
					   const TLbsExternalRequestInfo& aExternalRequestInfo,
					   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy);
	
	void OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId, 
						 const TLbsNetPosRequestMethodInt& aPosRequestMethod,
						 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						 TBool aIsEmergency,
						 const TLbsNetPosRequestQualityInt& aQuality);

    void OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId, 
                            TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
                            TInt aResponseReason);

	void OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& aSessionId, 
		const TPositionInfoBase& aPosInfo);
							                             
protected:
	CLbsPrivLocWaitPrivRespState(CLbsPrivLocFsm* aFsm);
	};	


/**
 * Wait for Location Request state.
 * Following a response from the privacy handler to accept a request, expects
 * a request for a position update.
 * Also handles all other events originating from the Network Gateway, 
 * which may terminate the current session.
 * Exits to states:
 */
class CLbsPrivLocWaitLocReqState	:	public CLbsPrivLocStateBase 
	{
public:
	static CLbsPrivLocWaitLocReqState* NewL(CLbsPrivLocFsm* aFsm);

protected:
	CLbsPrivLocWaitLocReqState(CLbsPrivLocFsm* aFsm);

public:
	void OnEntry(const TPrivLocCommonParams& aStateParams);
	TBool OnExit();

	void OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
					   TBool aIsEmergency,
					   const TLbsExternalRequestInfo& aExternalRequestInfo,
					   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy);
	
	void OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId, 
						 const TLbsNetPosRequestMethodInt& aPosRequestMethod,
						 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						 TBool aIsEmergency,
						 const TLbsNetPosRequestQualityInt& aQuality);
						 
	void OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aSessionId);
	
	void OnSessionComplete(const TLbsNetSessionIdInt&, TInt );

	void OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& aSessionId, 
		const TPositionInfoBase& aPosInfo);
	};	

/**
 * Wait for location update state.
 * Handles all events originating from the Network Gwateway, and also 
 * the update response timer and location updates received from the AGPS manager
 * Exits to states:
 */
class CLbsPrivLocWaitLocUpdateState	:	public CLbsPrivLocStateBase
	{
public:
	static CLbsPrivLocWaitLocUpdateState* NewL(CLbsPrivLocFsm* aFsm);
	~CLbsPrivLocWaitLocUpdateState();

protected:
	CLbsPrivLocWaitLocUpdateState(CLbsPrivLocFsm* aFsm);
	void ConstructL();

protected: // From MLbsPrivLocStateTransition
	void OnEntry(const TPrivLocCommonParams& aStateParams); // Need to request the update and start the timer
	TBool OnExit(); // Cancel any outstanding request and its timer

public:
	void OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
					   TBool aIsEmergency,
					   const TLbsExternalRequestInfo& aExternalRequestInfo,
					   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy);
					   
	void OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId, 
						 const TLbsNetPosRequestMethodInt& aPosRequestMethod,
						 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						 TBool aIsEmergency,
						 const TLbsNetPosRequestQualityInt& aQuality);

	void OnTimerEventL(TInt aTimerId);

	void OnAgpsPositionUpdate(
			TInt aReason,
			const TPositionExtendedSatelliteInfo& aPosInfo,
			const TTime& aTimeStamp);

	void OnAgpsMeasurementUpdate(
			TInt aReason,
			const TPositionGpsMeasurementInfo& aPosInfo,
			const TTime& aTimeStamp);
	
	void OnNetLocReferenceUpdate(
			const TLbsNetSessionIdInt& aSessionId, 
			const TPositionInfoBase& aPosInfo);

private:
	void SetExitState();
	TBool ReceivedFixIsAccurate();
	};	
	
	
/**
 * State machine
 */
class CLbsPrivLocFsm :	public CBase,
						public MNGMessageSwitchObserver,
						public MPrivacyHandlerObserver,
						public MLbsCallbackTimerObserver,
						public MAgpsInterfaceHandlerObserver
	{
public:

	enum TLocPrivacyHandlerState 
		{
		EStateIdle,
		EStateWaitPrivacyResponse,
		EStateWaitLocationRequest,
		EStateWaitLocationUpdate,
		ETotalStates,
		ENoState = ETotalStates
		};

	static CLbsPrivLocFsm* NewL(CPrivacyAndLocationHandler& aPrivLocHandler,
								const TLbsNetSessionIdInt& aSessionId);
	~CLbsPrivLocFsm();

	const TLbsNetSessionIdInt& SessionId() const;	
										
	TBool IsSpecialFeatureIntermediateFutileUpdateOn();
	
public: // From MLbsFsmObserver
	void ChangeState(TLocPrivacyHandlerState aNewStateId, 
						const TPrivLocCommonParams& aStateParams);		
	void ChangeState(TLocPrivacyHandlerState aNewStateId,
						const TLbsNetSessionIdInt& aSessionId);

protected:
	void ConstructL();
	CLbsPrivLocFsm(CPrivacyAndLocationHandler& aPrivLocHandler,
				   const TLbsNetSessionIdInt& aSessionId);

public: // From MNGMessageSwitchObserver. Delegated to current state
	void OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
				   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt  aSessionType, 
				   TBool aIsEmergency,
				   const TLbsExternalRequestInfo& aExternalRequestInfo,
				   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy);

	void OnSessionComplete(const TLbsNetSessionIdInt& aSessionId, TInt aReason);

	void OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId, 
					 const TLbsNetPosRequestMethodInt& aPosRequestMethod,
					 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
					 TBool aIsEmergency,
					 const TLbsNetPosRequestQualityInt& aQuality);
	void OnNetLocResponse(const TLbsNetSessionIdInt&,
						 const TLbsNetPosRequestQualityInt&){}
	
	void OnNetLocReferenceUpdate(const TLbsNetSessionIdInt& aSessionId, 
								 const TPositionInfoBase& aPosInfo);
	
	void OnNetLocFinalUpdate(const TLbsNetSessionIdInt& aSessionId, 
							 const TPositionInfoBase& aPosInfo);

public: // From MPrivacyHandlerObserver
	void SetServerObserver(MLbsSessionObserver* aNrhPrivacyServer);
	void OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId, 
                        TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
                        TInt aResponseReason);
	void OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId);

public: // from MAgpsInterfaceHandlerObserver
	void OnAgpsPositionUpdate(
			TInt aReason,
			const TPositionExtendedSatelliteInfo& aPosInfo,
			const TTime& aTimeStamp);

	void OnAgpsMeasurementUpdate(
			TInt aReason,
			const TPositionGpsMeasurementInfo& aPosInfo,
			const TTime& aTimeStamp);

protected: // From MLbsCallbackTimerObserver
	/** Called when the timer expires */
	void OnTimerEventL(TInt aTimerId);
	/** Called if OnTimerEventL leaves */
	TInt OnTimerError(TInt aTimerId, TInt aError);

public:
	// These accessor functions can be used as 
	// both getters and setters.

	TBool& IsEmergency() 
		{ return iIsEmergency; }

	TBool& TapMode()
		{ return iTapMode; }

	TLbsNetworkEnumInt::TLbsNetProtocolServiceInt&  SessionType() 
		{ return iSessionType; }

	TPrivLocStateExitData& ExitData() { return iExitData; }

	TBool& RefPosProcessed() 
			{ return iRefPosProcessed; }
	
	TLbsNetSessionIdInt& NetSessionId()
			{ return iSessionId; }
	
	TLbsNetPosRequestQualityInt& NetRequestQuality()
		{ return iNetRequestQuality; }

	TBool& LocReqReceived()
		{ return iLocReqReceived; }
	
	TLbsNetworkEnumInt::TLbsPrivacyResponseInt& PrivacyResponse()
		{ return iPrivacyResponse; }

	TInt& PrivacyResponseReason()
		{ return iPrivacyResponseReason; }
	
	TLbsNetPosRequestMethodInt& PosRequestMethod()
		{ return iPosRequestMethod; }
	
	TBool& PrivacyRequestCancelled()
		{ return iReqCancelled; }
	
	TLbsNetPosRequestQualityInt& GpsRequestQuality()
		{ return iGpsRequestQuality; }
	
	TBool& LocationFixReceived()
		{ return iLocationFixReceived; }
	
	TTime& ActualTime()
		{ return iActualTime; }
	
	TPositionExtendedSatelliteInfo& GpsPosition()
		{ return iPosInfo; }
	
	TBool& MeasurementInfoReceived()
		{ return iMeasurementInfoReceived; }
	
	TInt& MeasurementInfoError()
		{ return iMeasurementInfoErr; }
	
	TPositionGpsMeasurementInfo& GpsMeasurementInfo()
		{ return iMeasurementResults; }
		
	TInt& PreviousStateExitInfo()
		{ return iPreviousStateExitInfo; }
	
	CLbsCallbackTimer& LocationUpdateTimer()
		{ return *iLocationUpdateTimer; }
	
	TLbsNetSessionIdInt& LastLocReqSessionId()
		{ return iLastLocReqSessionId; }
	
	TBool& WasPrivacyResponseReceivedStateExited() 
	    { return iWasPrivacyResponseReceivedStateExited; }
	
	TLbsExternalRequestInfo::TRequestType& ExternalRequestType()
	    { return iExternalRequestType; }
	
	TBool& WasPositioningStatusIncremented()
        { return iPositioningStatusIncremented; }
	   
	
private: // Allow access for friend classes (the states and the NRH) 
	friend class CLbsPrivLocStateBase;
	friend class CLbsNetworkRequestHandler;
	friend class CPrivacyAndLocationHandler;
	friend class CLbsPrivLocWaitLocUpdateState;
	CPrivacyAndLocationHandler& PrivLocHandler();
	
private:

	// The privacy and location handler
	CPrivacyAndLocationHandler& iPrivLocHandler;
	
	// The current Session ID being used by this FSM
	TLbsNetSessionIdInt iSessionId;

	// Is the current request an emergency request?
	TBool iIsEmergency;

	// Is the current session porcessing a Tap request?
	TBool iTapMode;
	
	// Service type of the current request.
	TLbsNetworkEnumInt::TLbsNetProtocolServiceInt  iSessionType;
	
	// Exit data from the current state.
	TPrivLocStateExitData iExitData;
	
	// Has a reference position been processed -by provacy framework?
	TBool iRefPosProcessed;
		
	// Quality/accuracy requirements for the location request.
	TLbsNetPosRequestQualityInt iNetRequestQuality;
	
	// Record if a location request has arrived while waiting for privacy response.
	TBool iLocReqReceived;
	
	// Response from a privacy request.
	TLbsNetworkEnumInt::TLbsPrivacyResponseInt iPrivacyResponse;
	
	// Reason for the privacy request response.
	TInt iPrivacyResponseReason;
	
	// The location method requested by the network.
	TLbsNetPosRequestMethodInt iPosRequestMethod;
	
	// Was the privacy request cancelled by the user?
	TBool iReqCancelled;
	
	// Request quality sent to agps manager.
	TLbsNetPosRequestQualityInt iGpsRequestQuality;
	
	// GPS position update has arrived?
	TBool iLocationFixReceived;
	
	// GPS position update timestamp.
	TTime iActualTime;
	
	// GPS position update.
	TPositionExtendedSatelliteInfo iPosInfo;
	
	// GPS measurement info update arrived?
	TBool iMeasurementInfoReceived;
	
	// GPS measurement info error code.
	TInt iMeasurementInfoErr;
	
	// GPS measurement info update data.
	TPositionGpsMeasurementInfo iMeasurementResults;
	
	// Exit info code from the previous state ?!?
	TInt iPreviousStateExitInfo;
	
	// MaxFixTime timeout timer for location requests to agps manager.
	CLbsCallbackTimer* iLocationUpdateTimer;
	
	// Record the sessionId of the last/current location request.
	TLbsNetSessionIdInt iLastLocReqSessionId;

	// Has a privacy request been rejected?
	TBool iWasPrivacyResponseReceivedStateExited;
	
	// The type of external request that has been recieved.
	TLbsExternalRequestInfo::TRequestType iExternalRequestType;
	
	// Indicates if this session resulted in the positioning status being incremented.
	TBool iPositioningStatusIncremented;
	
private:
	//  the states
	CLbsPrivLocStateBase* iCurrentState;
	TFixedArray<CLbsPrivLocStateBase*, ETotalStates> iStates;
	};
		
	
	
#endif //PRIVACYANDLOCATIONHANDLER_H
