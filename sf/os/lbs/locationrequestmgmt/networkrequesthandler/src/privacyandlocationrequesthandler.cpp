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
//

#include <e32std.h>
#include <e32property.h>
#include <centralrepository.h>

#ifdef SYMBIAN_FEATURE_MANAGER
    #include <featdiscovery.h>
    #include <featureuids.h>
#endif

// LBS-specific
#include <lbs.h>
#include <lbs/lbsadmin.h>
#include <lbs/lbslocerrors.h>
#include <lbs/lbslocclasstypes.h>

#include "nrhpanic.h"
#include "lbsdevloggermacros.h"
#include "lbsqualityprofile.h"
#include "lbsrootcenrepdefs.h"
#include "lbspositioningstatusprops.h"

#include "privacyandlocationrequesthandler.h"

// Special 'invalid session' SessionId.
const TLbsNetSessionIdInt KInvalidSessionId(TUid::Uid(0), 0);

const TPositionModuleInfo::TTechnologyType KTerminalAssistedMode = (TPositionModuleInfo::ETechnologyNetwork |
																	TPositionModuleInfo::ETechnologyAssisted);
	
// ----------------------------------------------------------------------------- 
// 
// ----------------------- Class CPrivacyAndLocationHandler --------------------
//
// State Machine class which owns the states of the Privacy and Location Handler
//
// ----------------------------------------------------------------------------- 
//

// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::NewL
// Description: CPrivacyAndLocationHandler static constructor 
// ----------------------------------------------------------------------------- 
//
CPrivacyAndLocationHandler* CPrivacyAndLocationHandler::NewL(CNGMessageSwitch& aMessageSwitch,
											CLbsAdmin& aLbsAdmin,
											RLbsNetworkRegistrationStatus& aNetRegStatus)
	{
	CPrivacyAndLocationHandler* self; 
	self = new (ELeave) CPrivacyAndLocationHandler(aMessageSwitch, aNetRegStatus);
	CleanupStack::PushL(self);
	self->ConstructL(&aLbsAdmin);
	CleanupStack::Pop(self);
	return(self);	
	}

// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::CPrivacyAndLocationHandler
// Description: CPrivacyAndLocationHandler constructor 
// ----------------------------------------------------------------------------- 
//
CPrivacyAndLocationHandler::CPrivacyAndLocationHandler(CNGMessageSwitch& aMessageSwitch, 
													   RLbsNetworkRegistrationStatus& aNetRegStatus)
:	iNetRegStatus(aNetRegStatus),
	iMessageSwitch(&aMessageSwitch),
	iNumActiveSessions(0)
	{	
	}
	
// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::~CPrivacyAndLocationHandler
// Description: CPrivacyAndLocationHandler destructor 
// ----------------------------------------------------------------------------- 
//
CPrivacyAndLocationHandler::~CPrivacyAndLocationHandler()
	{
	// If iEmergencyFsm has been used by any outstanding request,
	// it needs to be removed before calling ResetAndDestroy(),
	// otherwise it will get double-deleted when delete iEmergencyFsm
	// is called.
			
	TInt index = iFsmArray.Find(iEmergencyFsm);
	if (index >= 0)
		{
		iFsmArray.Remove(index);
		}
	
	iFsmArray.ResetAndDestroy();

    // force the count of active network initiated positioning sessions to 0
	// this supports the pre-APE centric architecture wherein the NRH is
	// destroyed on completion of network initiated positioning. 
    RProperty::Set(iPosStatusCategory, KLbsNiPositioningStatusKey, 0);

	delete iEmergencyFsm;
    delete iAgpsInterface;
    delete iPrivacyHandler;
	}

// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::ConstructL
// Description: CPrivacyAndLocationHandler second-phase constructor.
//              Creates the states of the system and the Privacy Handler.
// ----------------------------------------------------------------------------- 
//

const TInt KLbsDefaultMaxNumLocationRequests   = 4;

void CPrivacyAndLocationHandler::ConstructL(CLbsAdmin* aLbsAdmin)
	{
	iLbsAdmin = aLbsAdmin;
	
	iPrivacyHandler = CPrivacyHandler::CreateL(this, *iLbsAdmin, iNetRegStatus);
	iMessageSwitch->RegisterObserver(this);

	// Get the behaviour mode and device gps mode capabilities
	TInt err = iLbsAdmin->Get(KLbsSettingBehaviourMode, iLbsBehaviourMode);
	if (err != KErrNone)
		{
		iLbsBehaviourMode = CLbsAdmin::ELbsBehaviourCustom1;
		}
	// get device mode capabilities:
	err = LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, iDeviceGpsModeCaps);
	if(err != KErrNone || (iDeviceGpsModeCaps==TPositionModuleInfoExtended::EDeviceGpsModeNone))
		{
		// Assume module supports hybrid if it has not reported its capabilities in module info file
		iDeviceGpsModeCaps = TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB;
		}


	err = iLbsAdmin->Get(KLbsSettingMaximumExternalLocateRequests, iMaxNumSessions);
	if (err != KErrNone)
		{
		iMaxNumSessions = KLbsDefaultMaxNumLocationRequests;
		}
	
	iAgpsInterface = CAgpsInterfaceHandler::NewL(*this, *iLbsAdmin, iNetRegStatus);


#ifdef NRH_UNIT_TEST
	// For testing use the Uid of the dummy NG
	const TInt KTestNgUidInt = 0x1028226B;
	const TUid KTestNgUid = {KTestNgUidInt};
	iProtocolModuleUid = KTestNgUid;
#else
	ReadProtocolModuleAdminSetting();
#endif
	
	iEmergencyFsm = CLbsPrivLocFsm::NewL(*this, KInvalidSessionId);
	
	// Reserve space for FSMs. Note "+1" because a pointer to the emergency Fsm gets added to this array
	iFsmArray.ReserveL(iMaxNumSessions+1);

	CLbsAdmin::TSpecialFeature specialFeature(CLbsAdmin::ESpecialFeatureOff);
	err = iLbsAdmin->Get(KLbsSpecialFeatureIntermediateFutileUpdate, specialFeature);
	if (err != KErrNone)
		{
		LBSLOG_ERR2(ELogP3, "Failed to get KLbsSpecialFeatureIntermediateFutileUpdate (err %d)", err);
		}
	LBSLOG2(ELogP3, "Using KLbsSpecialFeatureIntermediateFutileUpdate = %d", specialFeature);
	iSpecialFeatureIntermediateFutileUpdate = (specialFeature == CLbsAdmin::ESpecialFeatureOn) ? ETrue : EFalse;

#if defined __WINSCW__ && defined SYMBIAN_CELLMO_CENTRIC
	iLocationManagementSupported = EFalse;
#else
#ifdef SYMBIAN_FEATURE_MANAGER
    iLocationManagementSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KLocationManagement);
#else
    __ASSERT_ALWAYS(EFalse, User::Invariant()); // Would happen on older versions of symbian OS if this code ever backported
#endif // SYMBIAN_FEATURE_MANAGER
#endif // __WINSCW__ && defined SYMBIAN_CELLMO_CENTRIC
    
    // Get the CategoryUid from the cenrep file owned by LbsRoot for accessing Positioning Status P&S Keys
    CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
    TInt posStatusCategory;
    err = rep->Get(KNiPositioningStatusAPIKey, posStatusCategory);
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(rep);
    iPosStatusCategory = TUid::Uid(posStatusCategory);
	}



/**
Reads the Uid of a current Protocol Module from the Admin Settings.
*/
void CPrivacyAndLocationHandler::ReadProtocolModuleAdminSetting()
	{
	LBSLOG(ELogP1, "CPrivacyAndLocationHandler::ReadProtocolModuleAdminSetting()");
	TLbsProtocolModuleId protUid(KLbsProtocolNullModuleId);
	
	TInt err = iLbsAdmin->Get(KLbsSettingHomeProtocolModule, protUid);
	if (err != KErrNone)
		{
		LBSLOG_ERR2(ELogP4, "Failed to get KLbsSettingHomeProtocolModule (err %d)", err);
		}

	iProtocolModuleUid = protUid;
	}

/** Compares sessionId for RPointerArray::Find().
*/
TBool CPrivacyAndLocationHandler::IsSessionIdEqual(
		const TLbsNetSessionIdInt* aSessionId,
		const CLbsPrivLocFsm& aFsm)
	{
	return (*aSessionId == aFsm.SessionId());
	}

/** Compares session type for RPointerArray::Find().
*/
TBool CPrivacyAndLocationHandler::IsSessionTypeEqual(
		const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt* aSessionType,
		const CLbsPrivLocFsm& aFsm)
	{
	return (*aSessionType == const_cast<CLbsPrivLocFsm&>(aFsm).SessionType());
	}

// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::LookupFsm
// Description: Lookup CLbsPrivLocFsm object by session ID.
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocFsm* CPrivacyAndLocationHandler::LookupFsm(const TLbsNetSessionIdInt& aSessionId)
	{
	LBSLOG2(ELogP3, "LookupFsm session=%d", aSessionId.SessionNum());

	// Standard sessions always use the standard state machines.
	TInt index = iFsmArray.Find(aSessionId, IsSessionIdEqual);
	if (index >= 0)
		{
		LBSLOG(ELogP3, "LookupFsm: Existing standard FSM found");
		return iFsmArray[index];
		}
	else
		{
		LBSLOG(ELogP3, "LookupFsm: No standard FSM found");
		return NULL;		
		}
	}

/** Get a new state machine to use for a new request.

The state machine can either be re-using an existing FSM,
or allocating a new one from the heap or, for emergencies one thats was prepared earlier is used.
*/
CLbsPrivLocFsm* CPrivacyAndLocationHandler::GetNewFsm(
		const TLbsNetSessionIdInt& aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
		TBool aEmergency)
	{
	LBSLOG2(ELogP3, "LookupFsm session=%d", aSessionId.SessionNum());
	
	CLbsPrivLocFsm* fsm(NULL);

	// If this is an emergency request, use the emergency FSM  
	// Note, we only have to support ONE emergency at a time
	// this implies that only one Protcol module may deal with emergencies
	// So we do NOT support TWO emergencies .. one from each of the PMs
	// Note: only MT-LR or NI-LR requests can be emergency requests.
	if (aEmergency
		&& (aSessionType == MLbsNetworkProtocolObserver::EServiceMobileTerminated
		|| aSessionType == MLbsNetworkProtocolObserver::EServiceNetworkInduced))
		{					
		TInt index = iFsmArray.Find(iEmergencyFsm);
		if (index >= 0)
			{
			iFsmArray.Remove(index);
			iNumActiveSessions--;
			}
		// clean out Fsm
		iEmergencyFsm->RefPosProcessed() = EFalse;
		iEmergencyFsm->LocReqReceived() = EFalse;
		iEmergencyFsm->LocationFixReceived()= EFalse;
		iEmergencyFsm->TapMode() = EFalse;
		iEmergencyFsm->WasPrivacyResponseReceivedStateExited() = EFalse;
		iEmergencyFsm->NetSessionId()= aSessionId;
		fsm = iEmergencyFsm;
		}
	else
		{
		if (iNumActiveSessions <= iMaxNumSessions)
			{
			// Create a new session to handle this privacy request
			LBSLOG2(ELogP3, "Creating FSM for new standard request %d",aSessionId.SessionNum());
			TRAPD(err, fsm = CLbsPrivLocFsm::NewL(*this, aSessionId)); 
			if (err != KErrNone)
				{
				LBSLOG_ERR2(ELogP3, "Failed to create new FSM, error code : %d", err);
				}
			}
		else
			{
			LBSLOG_ERR3(ELogP3, "Session start rejected! iNumActiveSessions=%d > iMaxNumSessions=%d", iNumActiveSessions, iMaxNumSessions);
			}
		}

	if (fsm)
		{
		// Add the state machine to the buffer.
		TInt err = iFsmArray.Append(fsm);
		if( err != KErrNone )
			{
			delete fsm;
			return NULL;
			}
	
		iNumActiveSessions++; // conceptually, a session starts when a Fsm is created for it

		}
	
	return fsm;
	}


// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::PrivacyHandler
// Description: Return a pointer to the privacy handler implementation
//              (controller or notifier).
// ----------------------------------------------------------------------------- 
//
CPrivacyHandler* CPrivacyAndLocationHandler::PrivacyHandler()
	{
	return iPrivacyHandler;
	}
	
// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::MessageSwitch
// Description: Return a pointer to the Network Gateway Message Switch
// ----------------------------------------------------------------------------- 
//
CNGMessageSwitch* CPrivacyAndLocationHandler::MessageSwitch()
	{
	return iMessageSwitch;
	}
	
// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::LbsAdmin
// Description: Return a pointer to the Admin settings database
// ----------------------------------------------------------------------------- 
//
CLbsAdmin* CPrivacyAndLocationHandler::LbsAdmin()
	{
	return iLbsAdmin;
	}
	
// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::SetServerObserver
// Description: Store a pointer to the NRH server which comunicates with the 
// Privacy Controller.
// ----------------------------------------------------------------------------- 
//
void CPrivacyAndLocationHandler::SetServerObserver(MLbsSessionObserver* aNrhServer)
    {
    PrivacyHandler()->SetServerObserver(aNrhServer);
    }

// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::OnRespondNetworkLocationRequest
// Description: Called by the Privacy Handler to report the result of a privacy
// check. Handling of the response is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
void CPrivacyAndLocationHandler::OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId, 
                            TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
                            TInt aResponseReason)
	{
	LBSLOG2(ELogP3, "Received response %d to privacy request", aRequestResult);
	CLbsPrivLocFsm* fsm = LookupFsm(aRequestId);
	
	if (NULL != fsm)
		{
		fsm->OnRespondNetworkLocationRequest(aRequestId, aRequestResult, aResponseReason);
		}
	else
		{
		LBSLOG_WARN(ELogP3, "Couldn't find a FSM with matching session Id");
		}
    }

// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::OnCancelNetworkLocationRequest
// Description: Called by the Privacy Handler to report that a privacy check 
// has been rejected. This may occur after it has already been accepted.
// Handling of the response is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
void CPrivacyAndLocationHandler::OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId)
    {
	LBSLOG2(ELogP3, "Received cancellation to privacy request %d", aRequestId.SessionNum());
	CLbsPrivLocFsm* fsm = LookupFsm(aRequestId);
	
	if (NULL != fsm)
		{
		fsm->OnCancelNetworkLocationRequest(aRequestId);
		}
	else
		{
		LBSLOG_WARN(ELogP3, "Couldn't find a FSM with matching session Id");
		}
    }

// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::OnMTLRRequest
// Description: The Message Switch has forwarded a request to start an MTLR 
// session.
// Handling of the request is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
void CPrivacyAndLocationHandler::OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
					   TBool aIsEmergency,
					   const TLbsExternalRequestInfo& aExternalRequestInfo,
					   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy)
	{
	LBSLOG2(ELogP3, "Received privacy request with id %d", aSessionId.SessionNum());
	CLbsPrivLocFsm* fsm = LookupFsm(aSessionId);

	if (fsm==NULL)
		{
		fsm = GetNewFsm(aSessionId, aSessionType, aIsEmergency);
		}
	
	if (NULL != fsm)
		{
		fsm->OnMTLRRequest(aSessionId, 
						   aSessionType, 
						   aIsEmergency, 
						   aExternalRequestInfo, 
						   aNetPosRequestPrivacy);
		}
	else
		{					
		// Failed to create a state machine for this request,
		// so simply reply with a privacy rejection.
		iMessageSwitch->SendMTLRResponse(aSessionId, 
										 TLbsNetworkEnumInt::EPrivacyResponseRejected,
										 KErrGeneral, EFalse); // can't be an emergency cuase we know we have a Fsm for these!
		}
	}
	
// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::OnSessionComplete
// Description: The Message Switch has reported that the session is
// over (complete or aborted due to some error).
// Handling of the message is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
void CPrivacyAndLocationHandler::OnSessionComplete(
									const TLbsNetSessionIdInt& aSessionId,
									TInt aReason)
	{
	LBSLOG3(ELogP3, "Received Session Complete for id %d, reason %d", aSessionId.SessionNum(), aReason);
	CLbsPrivLocFsm* fsm = LookupFsm(aSessionId);
	
	if (NULL != fsm)
		{
		fsm->OnSessionComplete(aSessionId, aReason);

		// The session complete marks the end of a session.
		TInt index = iFsmArray.Find(fsm);
		if (index != KErrNotFound)
			{ 
			
			if (fsm->SessionType()== TLbsNetworkEnumInt::EServiceSelfLocation)
				{
				iMolRFsm = NULL;
				}
			else if (fsm->SessionType()== TLbsNetworkEnumInt::EServiceTransmitThirdParty)
				{
				iX3pFsm = NULL;
				}
			
			// We should never delete the emergency FSM.
			iFsmArray.Remove(index);
			iNumActiveSessions--;
			
			if (fsm != iEmergencyFsm)
				{
				delete fsm;
				}
			}
		}
	else
		{
		LBSLOG_WARN(ELogP3, "Couldn't find a FSM with matching session Id");
		}
	}

// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::OnNetLocRequest
// Description: The Message Switch has passed on a request for a position update
// Handling of the request is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
void CPrivacyAndLocationHandler::OnNetLocRequest(
						const TLbsNetSessionIdInt& aSessionId, 
						const TLbsNetPosRequestMethodInt& aPosRequestMethod,
						TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						TBool aIsEmergency,
						const TLbsNetPosRequestQualityInt& aQuality)
	{
	LBSLOG2(ELogP3, "Received position update request for id %d", aSessionId.SessionNum());
	
	TLbsNetSessionIdInt sessionId;
	TPositionInfo    posInfo;
	TPosition		 pos;
	TTime 			 timeStamp;
	TInt 			 err;
	
	TBool tapMode = EFalse;
	TInt numMethods = aPosRequestMethod.NumPosMethods();
    if (numMethods==1)
    	{
    	TLbsNetPosMethodInt netPosMethod;
    	aPosRequestMethod.GetPosMethod(0,netPosMethod);
                 
    	if (netPosMethod.PosMode()== (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted))
    		{
    		tapMode = ETrue;
    		}
    	}
	
	// This filtering used to be in the NG Message Switch, but has been moved
	// here to get an access to the ref position bus
	
	if( (aSessionType == TLbsNetworkEnumInt::EServiceNetworkLocation) && !tapMode)
		{
		// A Network-based location request generates a location 
		// request to the network request handler, but there's no point 
		// passing it any further - the AGPS manager & privacy 
		// controller aren't interested.
		// Simply return the saved reference location
		err = iMessageSwitch->GetNetworkReferencePosition(aSessionId, posInfo);			
		posInfo.GetPosition(pos);
		timeStamp = pos.Time();
		TLbsNetPosRequestQualityInt dummyQuality;
		MessageSwitch()->SendNetLocResponse(aSessionId, err, dummyQuality, posInfo, timeStamp, EFalse);
		}
	else
		{
		// we note that a self locate MoLr session can be implicitly
		// cancelled by the start of a new session for a new client.
		// In this case we complete the session before creating a new
		// fsm for the new client 
		CLbsPrivLocFsm* fsm = LookupFsm(aSessionId);
		if (!fsm)
			{
			// here, we need to create a new fsm
			// We note that only one self locate MolR (or X3p) is supported
			// a new one will implicitly cancel any ongoing
			if(aSessionType == TLbsNetworkEnumInt::EServiceSelfLocation)
				{
				if (iMolRFsm)
					{
					TInt index = iFsmArray.Find(iMolRFsm);
					if (index != KErrNotFound)
						{ 
						iFsmArray.Remove(index);
						iNumActiveSessions--;
						delete iMolRFsm;
						iMolRFsm = NULL;
						}
					}
					
				}
			else if(aSessionType == TLbsNetworkEnumInt::EServiceTransmitThirdParty)
				{
				if (iX3pFsm)
					{
					TInt index = iFsmArray.Find(iX3pFsm);
					if (index != KErrNotFound)
						{ 
						iFsmArray.Remove(index);
						iNumActiveSessions--;
						delete iX3pFsm;
						iX3pFsm = NULL;
						}
					}
				}
				
			fsm = GetNewFsm(aSessionId, aSessionType, aIsEmergency);
			}
		
		if (NULL != fsm)
			{
			if(aSessionType == TLbsNetworkEnumInt::EServiceSelfLocation)
				{
				iMolRFsm = fsm;
				}
			else if(aSessionType == TLbsNetworkEnumInt::EServiceTransmitThirdParty)
				{
				iX3pFsm = fsm;
				}
			
			fsm->OnNetLocRequest(aSessionId,
					aPosRequestMethod, 
			 					aSessionType, 
								aIsEmergency, 
								aQuality);
			}			
		else
			{
			// TODO: Return a dummy loc response with error code?
			LBSLOG_WARN(ELogP3, "Couldn't find a FSM with matching session Id");
			}
		}
	}

/** Called when a reference position arrives from the network.
*/
void CPrivacyAndLocationHandler::OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& aSessionId, 
		const TPositionInfoBase& aPosInfo)
	{
	LBSLOG2(ELogP3, "Received reference position update for id %d", aSessionId.SessionNum());
	CLbsPrivLocFsm* fsm = LookupFsm(aSessionId);
	
	if (NULL != fsm)
		{
		fsm->OnNetLocReferenceUpdate(aSessionId, aPosInfo);
		}
	else
		{
		LBSLOG_WARN(ELogP3, "Couldn't find a FSM with matching session Id");
		}	
	}

/** Callend when a final location arrives from the network.
*/
void CPrivacyAndLocationHandler::OnNetLocFinalUpdate(
		const TLbsNetSessionIdInt& aSessionId, 
		const TPositionInfoBase& aPosInfo)
	{
	LBSLOG2(ELogP3, "Received final network position update for id %d", aSessionId.SessionNum());
	CLbsPrivLocFsm* fsm = LookupFsm(aSessionId);
	
	if (NULL != fsm)
		{
		fsm->OnNetLocFinalUpdate(aSessionId, aPosInfo);
		}
	else
		{
		LBSLOG_WARN(ELogP3, "Couldn't find a FSM with matching session Id");
		}
	}

/** Callback when a GPS position update arrives from AGPS manager.
*/
void CPrivacyAndLocationHandler::OnAgpsPositionUpdate(
	TInt aReason,
	const TPositionExtendedSatelliteInfo& aPosInfo,
	const TTime& aTimeStamp)
	{
	// Broadcast the update to all state machines.
	const TInt count = iFsmArray.Count();
	for (TInt i = 0; i < count; i++)
		{
		iFsmArray[i]->OnAgpsPositionUpdate(aReason, aPosInfo, aTimeStamp);
		}
	}

/** Callback when a GPS measurement results update arrives from AGPS manager.
*/
void CPrivacyAndLocationHandler::OnAgpsMeasurementUpdate(
	TInt aReason,
	const TPositionGpsMeasurementInfo& aPosInfo,
	const TTime& aTimeStamp)
	{
	// Broadcast the update to all state machines
	const TInt count = iFsmArray.Count();
	for (TInt i = 0; i < count; i++)
		{
		iFsmArray[i]->OnAgpsMeasurementUpdate(aReason, aPosInfo, aTimeStamp);
		}
	}

/**
*/
CAgpsInterfaceHandler* CPrivacyAndLocationHandler::AgpsHandler()
	{
	return iAgpsInterface;
	}
	
// -----------------------------------------------------------------------------
// CPrivacyAndLocationHandler::DeviceGpsModeCaps
// Description: Return the device mode capabilities
// -----------------------------------------------------------------------------
//
TPositionModuleInfoExtended::TDeviceGpsModeCapabilities CPrivacyAndLocationHandler::DeviceGpsModeCaps()
	{
	return iDeviceGpsModeCaps;
	}

// -----------------------------------------------------------------------------
// CPrivacyAndLocationHandler::BehaviourMode
// Description: Return the behaviour mode setting
// -----------------------------------------------------------------------------
//
CLbsAdmin::TLbsBehaviourMode CPrivacyAndLocationHandler::BehaviourMode()
	{
	return iLbsBehaviourMode;
	}

RLbsNetworkRegistrationStatus& CPrivacyAndLocationHandler::NetworkRegistrationStatus()
	{
	return iNetRegStatus;
	}

// increments the P&S key tracking mobile terminated positioning requests
void CPrivacyAndLocationHandler::IncrementPositioningStatus()
    {
    TInt count;     
    RProperty::Get(iPosStatusCategory, KLbsNiPositioningStatusKey, count);
    RProperty::Set(iPosStatusCategory, KLbsNiPositioningStatusKey, count+1);
    }

// decrements the P&S key tracking mobile terminated positioning requests
// if location management is supported. In the alternative architecture,
// the NRH is not aware of the positioning session's progress, but is 
// transient. Therefore the positioning status is set to zero in the 
// class destructor.
void CPrivacyAndLocationHandler::DecrementPositioningStatus()
    {
    if (iLocationManagementSupported)
        {
        TInt count;     
        RProperty::Get(iPosStatusCategory, KLbsNiPositioningStatusKey, count);
        if(count>0)
            {
            RProperty::Set(iPosStatusCategory, KLbsNiPositioningStatusKey, count-1);
            }
        else
            {
            LBSLOG_ERR(ELogP3, "CPrivacyAndLocationHandler::DecrementPositioningStatus() - Incorrect Positioning Status count\n");
            }
        }
    }
	

/**
*/
MX3pStatusHandler& CPrivacyAndLocationHandler::X3pStatusHandler()
	{
	return *iAgpsInterface;
	}

/** Returns ETrue if KLbsSpecialFeatureIntermediateFutileUpdate is on.
@return ETrue if the special feature is on, EFalse otherwise.
*/
TBool CPrivacyAndLocationHandler::IsSpecialFeatureIntermediateFutileUpdateOn()
	{
	return iSpecialFeatureIntermediateFutileUpdate;
	}

// ----------------------------------------------------------------------------- 
// 
// ----------------------- Class CLbsPrivLocStateBase --------------------
//
// This class is not intended for instantiation. Implemented functions are
// those common to multiple derived states
//
// ----------------------------------------------------------------------------- 
//

// ----------------------------------------------------------------------------- 
// CLbsPrivLocStateBase::OnCancelNetworkLocationRequest
// Description: Pass on a received privacy request cancel to the network gateway, 
// if it relates to the current session. 
// This behaviour is common to states EStateWaitLocationRequest,
// EStateWaitLocationUpdate and EStateWaitPrivacyResponse.
// Other states ignore the event.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocStateBase::OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aSessionId)
	{
	/* Ignore the cancel if this is an emergency request */
	if(!iFsm->IsEmergency())
		{
		// Also ignore it if the cancel doesn't relate to this session.
		if(aSessionId == iFsm->SessionId())
			{
			iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitCancelledByPrivacyController, KErrCancel);
			iFsm->ChangeState(CLbsPrivLocFsm::EStateIdle, aSessionId);
			}
		}
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocStateBase::OnEntry
// Description: Handles initialisation actions which are common to multiple states. 
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocStateBase::OnEntry(const TPrivLocCommonParams& /* aStateParams */)
	{
	// Exit reason should always be explicitly set by a state, 
	// otherwise OnExit() will panic
	iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitReasonNone, KErrNone);
	}
	
	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocStateBase::OnExit
// Description: Handles exit actions which are common to multiple states. 
// Any exit reason not handled here is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
TBool CLbsPrivLocStateBase::OnExit()
	{
	TBool consumed = ETrue;
	switch(iFsm->iExitData.iExitReason)
		{
		case TPrivLocStateExitData::EExitSessionComplete:
			{
			// Tell the AGPS interface handle this location request has finished.
			AgpsInterface()->StopPositioning(iFsm->SessionId());

			// Tell the privacy controller this session is finished.
			PrivacyHandler()->ProcessRequestComplete(iFsm->SessionId(),
													 iFsm->ExitData().iExitInfo);
			break;
			}

		case TPrivLocStateExitData::EExitCancelledByPrivacyController:
			{
			// Send a cancel to the network gateway
			TPositionInfo dummyPosInfo;
			TTime dummyTime;
			TLbsNetPosRequestQualityInt dummyQuality;
			MessageSwitch()->SendNetLocResponse(iFsm->SessionId(),
												iFsm->ExitData().iExitInfo,
												dummyQuality,
												dummyPosInfo,
												dummyTime,
												iFsm->IsEmergency());
			}
			break;

		case TPrivLocStateExitData::EExitLocReqReceived:
			// No action required - request is issued on entry to next state.
		case TPrivLocStateExitData::EExitPrivacyRequestReceived:
			// No action required, state moves to waiting for loc request.
			{
			consumed = ETrue;
			break;			
			}
			
		default:
			{
			// Don't know what to do with it.
			consumed = EFalse;
			break;							
			}
		}
	return(consumed);
	}
	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocStateBase::HandleLocRequest
// Description: Common handling of a location request received while the 
// Privacy and Location Handler is dealing with a session.
// 
// If the session type is anything but MTLR, then it is processed, otherwise
// a privacy request is generated
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocStateBase::HandleLocRequest(const TLbsNetSessionIdInt& aSessionId, 
						 const TLbsNetPosRequestMethodInt& aPosRequestMethod,
						 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						 TBool aIsEmergency,
						 const TLbsNetPosRequestQualityInt& aQuality)
	{
	// MTLR.
	if (aSessionType == TLbsNetworkEnumInt::EServiceMobileTerminated)
		{
		// An MTLR with out a prior privacy request is not supported, report error via
		// RespondLocationRequest(dummy position).
		TPositionInfo dummyPosInfo;
		TTime dummyTime;
		TLbsNetPosRequestQualityInt dummyQuality;

		MessageSwitch()->SendNetLocResponse(aSessionId,
											KErrNotSupported,
											dummyQuality,
											dummyPosInfo,
											dummyTime, aIsEmergency); 
		}

	// Network Induced.	
	else if (aSessionType == TLbsNetworkEnumInt::EServiceNetworkInduced)
		{
		// If a request for a position update has been received without
		// a privacy request, then there's nothing to say how the user
		// should be informed or what do do if there is no response.
		// The safest thing is to get the user to confirm (verify)
		// the request, and in the absence of confirmation to reject the
		// request. For emergency requests we notify and accept.
		
		// Store the loc req.
		iFsm->LocReqReceived() = ETrue;

		iFsm->IsEmergency() = aIsEmergency;
		iFsm->NetRequestQuality() = aQuality;
		iFsm->PosRequestMethod() = aPosRequestMethod;


		// The following notification types are chosen based on the emergency and network requests admin status.
		//
		// Emergency = On, Admin = Any, gives ENotifyLocationAccepted 
		// Emergency = Off,	Admin = On, gives ENotifyLocationAccepted
		// Emergency = Off,	Admin = OnButAlwayVerify, gives ENotifyAndVerifyLocationRejectedIfNoResponse
		// Emergency = Off, Admin = Off, N/A the notifier or controller will not be called
		// Emergency = Off, Admin = OffButNotify, gives ENotifyLocationRejected
		TLbsNetPosRequestPrivacyInt requestPrivacy;
		
		requestPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
		requestPrivacy.SetRequestAction(TLbsNetPosRequestPrivacyInt::ERequestActionAllow);

		// Verifications are rejected after timeout.
		CLbsAdmin::TExternalLocateService externalLocate(CLbsAdmin::EExternalLocateOff);

		ReadNetworkInducedAdminSetting(externalLocate);
		if ((externalLocate == CLbsAdmin::EExternalLocateOnButAlwaysVerify) && (!aIsEmergency))
			{
	    	requestPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify);
	    	requestPrivacy.SetRequestAction(TLbsNetPosRequestPrivacyInt::ERequestActionReject);
			}

		// Similarly, default values have to be assigned to the external request info.
		TLbsExternalRequestInfo requestInfo;
		_LIT8(KUnknownExternalReqInfoField, "");
		requestInfo.SetRequesterId(KUnknownExternalReqInfoField);
		requestInfo.SetClientName(KUnknownExternalReqInfoField);
		requestInfo.SetClientExternalId(KUnknownExternalReqInfoField);


		// Process the privacy request.		
		iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocReqReceived, KErrNone);
		
		TPrivLocWaitPrivResponseParams privacyRequestParams(aSessionId, aSessionType, requestInfo, requestPrivacy, aIsEmergency);
		iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitPrivacyResponse, privacyRequestParams);
		}
	
	// All other location requests.
	else
		{
		TPrivLocWaitLocationUpdateParams updateRequestParams(aSessionId,
															aPosRequestMethod,
															aSessionType,
															aIsEmergency,
															aQuality);
		iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitSessionComplete, KErrCancel);
		iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitLocationUpdate, updateRequestParams);
		}
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocStateBase::OnSessionComplete
// Description: Common handling of a session complete message received other
// than when it is expected as normal session completion.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocStateBase::OnSessionComplete(const TLbsNetSessionIdInt& aSessionId,
																TInt aReason)
	{
	if(aSessionId == iFsm->SessionId())
		{
		iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitSessionComplete, aReason);
		iFsm->ChangeState(CLbsPrivLocFsm::EStateIdle, aSessionId);
		}		
	}

/** Called when a reference position arrives from the network.
*/
void CLbsPrivLocStateBase::OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& /*aSessionId*/ , 
		const TPositionInfoBase& aPosInfo)
	{
	TLbsNetworkEnumInt::TLbsNetProtocolServiceInt sessionType = iFsm->SessionType();
	__ASSERT_DEBUG((sessionType != MLbsNetworkProtocolObserver::EServiceNone), Panic(ENrhPanicBadParamType));

	if( ((sessionType == MLbsNetworkProtocolObserver::EServiceNetworkInduced) || 
											   (sessionType == MLbsNetworkProtocolObserver::EServiceMobileTerminated)))
		{
		TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		
		// Set the module Id and position mode for the reference position.
		// These values are not 'real' values, since this position
		// came directly from the network and not one of the location
		// managers within LBS.
		posInfo.SetModuleId(KLbsGpsLocManagerUid);
		posInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);
		posInfo.SetPositionModeReason(EPositionModeReasonNone);
		posInfo.SetUpdateType(EPositionUpdateGeneral);
		
		if (!iFsm->RefPosProcessed())
			{
			iFsm->RefPosProcessed() = ETrue;
			PrivacyHandler()->ProcessNetworkPositionUpdate(iFsm->SessionId(), posInfo);
			}

		}	
	}

/* Timer callback called when the MaxFixTime for a gps location update request has expired. 

The default action is to ignore this callback. Any state interested in it must
implement its own version.
*/
void CLbsPrivLocStateBase::OnTimerEventL(TInt /*aTimerId*/)
	{
	}


// ----------------------------------------------------------------------------- 
// CLbsPrivLocStateBase::ReadNetworkInducedAdminSetting
// Description: Determine the external location value from the admin settings for network induced location requests.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocStateBase::ReadNetworkInducedAdminSetting(CLbsAdmin::TExternalLocateService& aExternalLocateService)
	{
	CLbsAdmin::TExternalLocateService serviceStatus(CLbsAdmin::EExternalLocateOff);
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus netRegStatus(RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown);	
	TInt err = LbsNetworkRegistrationStatus().GetNetworkRegistrationStatus(netRegStatus);
	if (err == KErrNone)
		{
		switch (netRegStatus)
			{
			case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
				{
				err = LbsAdmin()->Get(KLbsSettingHomeNetworkInducedLocate, serviceStatus);
				break;
				}
			case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
			case RLbsNetworkRegistrationStatus::ENotRegistered:
				{
				err = LbsAdmin()->Get(KLbsSettingRoamingNetworkInducedLocate, serviceStatus);
				break;
				}		
			case RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown:
			default:
				{
				LBSLOG_WARN2(ELogP4, "Unrecognised TLbsNetworkRegistrationStatus (%d), defaulting to EExternalLocateOff",
							 netRegStatus);
				serviceStatus = CLbsAdmin::EExternalLocateOff;
				break;
				}
			}
		}
	else
		{
		LBSLOG_WARN2(ELogP4, "Failed to get TExternalLocateService, couldn't read roaming status (err %d), defaulting to EExternalLocateOff",  
					 err);		
		}
	
	aExternalLocateService = serviceStatus;
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocStateBase::CLbsPrivLocStateBase
// Description: Constructor
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocStateBase::CLbsPrivLocStateBase(CLbsPrivLocFsm* aFsm)
:	iFsm(aFsm)
	{	
	}
// ----------------------------------------------------------------------------- 
// CLbsPrivLocStateBase::PrivacyHandler, MessageSwitch, LbsAdmin
// Description: Allows concrete states access to NRH resources passed to
// the FSM
// Returns: pointers.
// ----------------------------------------------------------------------------- 
//
CPrivacyHandler* CLbsPrivLocStateBase::PrivacyHandler()
	{
	return iFsm->PrivLocHandler().PrivacyHandler();
	}
CNGMessageSwitch* CLbsPrivLocStateBase::MessageSwitch()
	{
	return iFsm->PrivLocHandler().MessageSwitch();
	}
CLbsAdmin* CLbsPrivLocStateBase::LbsAdmin()
	{
	return iFsm->PrivLocHandler().LbsAdmin();
	}
CAgpsInterfaceHandler* CLbsPrivLocStateBase::AgpsInterface()
	{
	return iFsm->PrivLocHandler().AgpsHandler();
	}


TPositionModuleInfoExtended::TDeviceGpsModeCapabilities CLbsPrivLocStateBase::DeviceGpsModeCaps()
	{
	return iFsm->PrivLocHandler().DeviceGpsModeCaps();
	}

CLbsAdmin::TLbsBehaviourMode CLbsPrivLocStateBase::BehaviourMode()
	{
	return iFsm->PrivLocHandler().BehaviourMode();
	}

RLbsNetworkRegistrationStatus& CLbsPrivLocStateBase::LbsNetworkRegistrationStatus()
	{
	return iFsm->PrivLocHandler().NetworkRegistrationStatus();
	}

/*
 * increments the network initiated positioning status count
 * and remembers that it has done
 */
void CLbsPrivLocStateBase::IncrementPositioningStatus()
    {
    iFsm->PrivLocHandler().IncrementPositioningStatus();
    iFsm->WasPositioningStatusIncremented() = ETrue;
    }


// ----------------------------------------------------------------------------- 
// 
// ----------------------- Class CLbsPrivLocIdleState --------------------
//
// Implements the Idle state of the Privacy and Location Request Handler
//
// ----------------------------------------------------------------------------- 
//

// ----------------------------------------------------------------------------- 
// CLbsPrivLocIdleState::NewL
// Description: CLbsPrivLocIdleState static constructor 
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocIdleState* CLbsPrivLocIdleState::NewL(CLbsPrivLocFsm* aFsm)
	{
	return new (ELeave) CLbsPrivLocIdleState(aFsm);
	}
	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocIdleState::CLbsPrivLocIdleState
// Description: CLbsPrivLocIdleState constructor.
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocIdleState::CLbsPrivLocIdleState(CLbsPrivLocFsm* aFsm) 
: CLbsPrivLocStateBase(aFsm)
	{
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocIdleState::OnEntry
// Description: Carries out tasks required on entry to the state.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocIdleState::OnEntry(const TPrivLocCommonParams& aStateParams)
	{
	CLbsPrivLocStateBase::OnEntry(aStateParams);
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocIdleState::OnExit
// Description: Carries out tasks required on exit from the state.
// Panics if the exit reason is not handled by the base state exit
// ----------------------------------------------------------------------------- 
//
TBool CLbsPrivLocIdleState::OnExit()
	{
	TBool consumed = CLbsPrivLocStateBase::OnExit();
	// If the exit reason wasn't handled, panic (should only happen in development)
	__ASSERT_DEBUG(consumed, Panic(ENrhPanicIdleUnknownExitReason));
	
	return(consumed);
	}
	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocIdleState::OnNetLocRequest
// Description: The Message Switch has forwarded a request for a control 
// measurement.
// If the session type is anything but MTLR, then it is processed, otherwise
// a privacy request is generated
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocIdleState::OnNetLocRequest(
		const TLbsNetSessionIdInt& aSessionId,
		const TLbsNetPosRequestMethodInt& aPosRequestMethod,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
		TBool aIsEmergency,
        const TLbsNetPosRequestQualityInt& aQuality)
    {
    iFsm->SessionType() = aSessionType;
    if(iFsm->WasPrivacyResponseReceivedStateExited())
        {
        // The request relates to a rejected privacy request
        // or a request for this session which has already been answered. 
        // In either case, it should be refused. The message is sent to the
        // network gateway as a part of exit from the state, but we want to 
        // remain in Idle state.
        iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitCancelledByPrivacyController, KErrAccessDenied);
        iFsm->ChangeState(CLbsPrivLocFsm::EStateIdle, aSessionId);
        }
    else
        {
        TInt numMethods = aPosRequestMethod.NumPosMethods();
        if (numMethods==1)
            {
            TLbsNetPosMethodInt netPosMethod;
            aPosRequestMethod.GetPosMethod(0,netPosMethod);
                    
            if (netPosMethod.PosMode()== (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted))
                {
                iFsm->TapMode() = ETrue;
                }
            }
        
            
        if ((aSessionType != MLbsNetworkProtocolObserver::EServiceMobileTerminated) &&
            (aSessionType != MLbsNetworkProtocolObserver::EServiceNetworkInduced))
            {
            iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocReqReceived, KErrNone);
            TPrivLocWaitLocationUpdateParams updateRequestParams(aSessionId,
                                                            aPosRequestMethod,
                                                            aSessionType,
                                                            aIsEmergency,
                                                            aQuality);
            iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitLocationUpdate, updateRequestParams);
            }
            else
            {
            // It's a request for a different session. Need to find out what to do with it.
            HandleLocRequest(aSessionId, aPosRequestMethod, 
                                    aSessionType,aIsEmergency,
                                    aQuality);
            }
        }
    }

// ----------------------------------------------------------------------------- 
// CLbsPrivLocIdleState::OnMTLRRequest
// Description: The Message Switch has forwarded a request for a location update
// (a privacy request) 
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocIdleState::OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
				TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
				TBool aIsEmergency,
				const TLbsExternalRequestInfo& aExternalRequestInfo,
				const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy)
	{
	iFsm->SessionType() = aSessionType;
	iFsm->ExternalRequestType() = aExternalRequestInfo.RequestType();
	iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitPrivacyRequestReceived, KErrNone);
	TPrivLocWaitPrivResponseParams privacyRequestParams(	aSessionId, 
													aSessionType,
													aExternalRequestInfo, 
													aNetPosRequestPrivacy,
													aIsEmergency);
	iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitPrivacyResponse, privacyRequestParams);
	}

/** Called when a reference position arrives from the network.
 * 
*/
void CLbsPrivLocIdleState::OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& /*aSessionId*/, 
		const TPositionInfoBase& /* aPosInfo */)
	{
	// note that the reference postion is stored by the message switch
	// so here we don't need to save it again!
	}
	
// ----------------------------------------------------------------------------- 
// 
// ----------------------- Class CLbsPrivLocWaitPrivRespState --------------------
//
// Implements the Idle state of the Privacy and Location Request Handler
//
// ----------------------------------------------------------------------------- 
//

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitPrivRespState::NewL
// Description: CLbsPrivLocIdleState static constructor 
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocWaitPrivRespState* CLbsPrivLocWaitPrivRespState::NewL(CLbsPrivLocFsm* aFsm)
	{
	return new (ELeave) CLbsPrivLocWaitPrivRespState(aFsm);
	}
	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitPrivRespState::CLbsPrivLocWaitPrivRespState
// Description: CLbsPrivLocWaitPrivRespState constructor.
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocWaitPrivRespState::CLbsPrivLocWaitPrivRespState(CLbsPrivLocFsm* aFsm)
: CLbsPrivLocStateBase(aFsm)
	{
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitPrivRespState::OnEntry
// Description: Actions performed when the state is entered.
// Unpack the parameters and issue the privacy request.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocWaitPrivRespState::OnEntry(const TPrivLocCommonParams& aStateParams)
	{
	CLbsPrivLocStateBase::OnEntry(aStateParams);
	const TPrivLocWaitPrivResponseParams& params = TPrivLocWaitPrivResponseParams::Cast(const_cast<TPrivLocCommonParams&>(aStateParams));	
	iFsm->SessionType() = params.iSessionType;
	iFsm->IsEmergency() = params.iIsEmergency;

	PrivacyHandler()->ProcessNetworkLocationRequest(iFsm->SessionId(), 
													iFsm->SessionType(),
													params.iExternalRequestInfo, 
													params.iNetPosRequestPrivacy,
													iFsm->IsEmergency());	
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitPrivRespState::OnExit
// Description: Actions performed on leaving the state.
// ----------------------------------------------------------------------------- 
//
TBool CLbsPrivLocWaitPrivRespState::OnExit()
	{
	TInt consumed = EFalse;
	switch(iFsm->ExitData().iExitReason)
		{
		case TPrivLocStateExitData::EExitPrivacyResponseReceived:
			{
            // Remember that we exited the privacy response received state 
			// so that we can deny the network location requests in the idle staet.

            iFsm->WasPrivacyResponseReceivedStateExited() = ETrue;

			
			// For the NI case a Reference position may have arrived by now
			// So we must pass this onto the privacy handler.

			if (iFsm->SessionType() == MLbsNetworkProtocolObserver::EServiceNetworkInduced) 
				{
				if (iFsm->PrivacyResponse() == CLbsNetworkProtocolBase::EPrivacyResponseAccepted)
					{
					TPositionInfo posInfo;
					TInt err = MessageSwitch()->GetNetworkReferencePosition(iFsm->SessionId(), posInfo);
					if (KErrNone == err)
						{
						if (!iFsm->RefPosProcessed())
							{
							iFsm->RefPosProcessed() = ETrue;
							PrivacyHandler()->ProcessNetworkPositionUpdate(iFsm->SessionId(), posInfo);
							}
				
						}
					
					}
				}		
				
			// For MtLrs the Protocol module should not
			// send a REF position until after we have sent the Priv response to the PM 
	            
			// Inform network of the privacy response for normal privacy requests.
			if (iFsm->SessionType() == TLbsNetworkEnumInt::EServiceMobileTerminated)
				{
				MessageSwitch()->SendMTLRResponse(iFsm->SessionId(), iFsm->PrivacyResponse(), iFsm->PrivacyResponseReason(), iFsm->IsEmergency());	
				}

			// Inform network of a rejected privacy response via a "RespondLocationRequest" for faked privacy requests (generated internaly).
			else if ((iFsm->SessionType() == TLbsNetworkEnumInt::EServiceNetworkInduced) && (iFsm->PrivacyResponse() == TLbsNetworkEnumInt::EPrivacyResponseRejected))
				{
				// The faked privacy request was rejected, so reject the location request.
				TPositionInfo dummyPosInfo;
				TTime dummyTime;
				TLbsNetPosRequestQualityInt dummyQuality;

				MessageSwitch()->SendNetLocResponse(iFsm->SessionId(),
													KErrAccessDenied,
													dummyQuality,
													dummyPosInfo,
													dummyTime,
													iFsm->IsEmergency());
				}
			
			consumed = ETrue;
			break;
			}

		case TPrivLocStateExitData::EExitCancelledByPrivacyController:
			{
			// Send a cancel to the network gateway
			iFsm->PrivacyResponse() = TLbsNetworkEnumInt::EPrivacyResponseRejected;
			iFsm->PrivacyResponseReason() = KErrCancel;
			MessageSwitch()->SendMTLRResponse(iFsm->SessionId(), iFsm->PrivacyResponse(), iFsm->PrivacyResponseReason(), iFsm->IsEmergency());
			consumed = ETrue;
			}
			break;

		default:
			{
			consumed = CLbsPrivLocStateBase::OnExit();
			// If the exit reason wasn't handled, panic (should only happen in development)
			__ASSERT_DEBUG(consumed, Panic(ENrhPanicWaitPrivRespUnknownExitReason));
			break;
			}
		}
		
	iFsm->LocReqReceived() = EFalse;

	return(consumed);
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitPrivRespState::OnRespondNetworkLocationRequest
// Description: Pass on a received privacy response to the network gateway, if
// it relates to the current session.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocWaitPrivRespState::OnRespondNetworkLocationRequest(
											const TLbsNetSessionIdInt& aSessionId, 
											TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
											TInt /* aResponseReason*/)
	{

	if(aSessionId == iFsm->SessionId())
		{
		iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitPrivacyResponseReceived, KErrNone);
		iFsm->PrivacyResponse() = aRequestResult;
		if(iFsm->PrivacyResponse() == TLbsNetworkEnumInt::EPrivacyResponseAccepted)
			{
			// Tell the AGPS handler that we are going to start a location request soon.
			AgpsInterface()->PreStartPositioning(iFsm->SessionId(), iFsm->IsEmergency());
			
			// Set the Positioning Status for the UI indicator.
			// Not done for silent requests.
			if (iFsm->ExternalRequestType() < TLbsExternalRequestInfo::ERequestSingleShotSilent)
			    {
                IncrementPositioningStatus();
			    }
			
			if(iFsm->LocReqReceived())
				{				
				TPrivLocWaitLocationUpdateParams updateRequestParams(iFsm->SessionId(),
																iFsm->PosRequestMethod(),
																iFsm->SessionType(),
																iFsm->IsEmergency(),
																iFsm->NetRequestQuality());
				iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitLocationUpdate, updateRequestParams);	            
				}
			else
				{
				TPrivLocWaitLocationRequestParams locationRequestParams(iFsm->SessionId(),
																		iFsm->IsEmergency(),
																		EFalse);
				iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitLocationRequest, locationRequestParams);
				}
			}
		else
			{
			iFsm->ChangeState(CLbsPrivLocFsm::EStateIdle, aSessionId);
			}
		}
	}
	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitPrivRespState::OnNetLocRequest
// Description: The Message Switch has forwarded a request for a control 
// measurement.
// If the session Id is the same as the current one, then save the parameters
// so that the request can be issued when privacy is accepted.
// Otherwise (the session Id is different) a cancel is implied and we cancel 
// the current session and start another, which may or may not require a new 
// privacy query.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocWaitPrivRespState::OnNetLocRequest(
		const TLbsNetSessionIdInt& aSessionId,
		const TLbsNetPosRequestMethodInt& aPosRequestMethod,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
		TBool aIsEmergency,
        const TLbsNetPosRequestQualityInt& aQuality)
	{	
	TInt numMethods = aPosRequestMethod.NumPosMethods();
	if (numMethods==1)
		{
		TLbsNetPosMethodInt netPosMethod;
		aPosRequestMethod.GetPosMethod(0,netPosMethod);
				
		if (netPosMethod.PosMode()== (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted))
			{
			iFsm->TapMode() = ETrue;
			}
		}
	if(aSessionId == iFsm->SessionId() && 
		aIsEmergency == iFsm->IsEmergency() &&
		aSessionType == iFsm->SessionType())
		{
		iFsm->PosRequestMethod() = aPosRequestMethod;
		iFsm->NetRequestQuality() = aQuality;
		iFsm->LocReqReceived() = ETrue;
		}
	else
		{
		// It's a request for different session. Need to find out what to do with it.
		HandleLocRequest(aSessionId,aPosRequestMethod, 
							aSessionType,aIsEmergency,
							aQuality);
		}
	}
	
	
void CLbsPrivLocWaitPrivRespState::OnMTLRRequest(const TLbsNetSessionIdInt& /*aSessionId*/,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt /*aSessionType*/, 
					   TBool /*aIsEmergency*/,
					   const TLbsExternalRequestInfo& /*aExternalRequestInfo*/,
					   const TLbsNetPosRequestPrivacyInt& /*aNetPosRequestPrivacy*/)
	{
	// this can never happen. If the Fsm is in the WaitPrivRespState then 
	// any arrival of a MTLR request would start a new session (no implicit cancel!)
	// and the OnMTLRRequest()would be directed to that session not this one
	__ASSERT_DEBUG(EFalse, Panic(ENrhPanicBadParamType));
	}

/** Called when a reference position arrives from the network.
 *  
*/
void CLbsPrivLocWaitPrivRespState::OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& /*aSessionId*/, 
		const TPositionInfoBase& /*aPosInfo*/)
	{
	// note that the reference postion is stored by the message switch
	// so here we don't need to save it again!
	}
	
	
// ----------------------------------------------------------------------------- 
// 
// ----------------------- Class CLbsPrivLocWaitLocUpdateState --------------------
//
// Implements the Wait For Location Update state of the Privacy and Location 
// Request Handler
//
// On entry, issues a location update request then starts a timer and waits for 
// a response.
//
// ----------------------------------------------------------------------------- 
//

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocUpdateState::NewL
// Description: CLbsPrivLocWaitLocUpdateState static constructor 
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocWaitLocUpdateState* CLbsPrivLocWaitLocUpdateState::NewL(CLbsPrivLocFsm* aFsm)
	{
	CLbsPrivLocWaitLocUpdateState* self; 
	self = new (ELeave) CLbsPrivLocWaitLocUpdateState(aFsm);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);	
	}
	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocUpdateState::CLbsPrivLocWaitLocUpdateState
// Description: CLbsPrivLocWaitLocUpdateState constructor.
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocWaitLocUpdateState::CLbsPrivLocWaitLocUpdateState(CLbsPrivLocFsm* aFsm)
: CLbsPrivLocStateBase(aFsm)
	{
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocUpdateState::~CLbsPrivLocWaitLocUpdateState
// Description: CLbsPrivLocWaitLocUpdateState destructor.
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocWaitLocUpdateState::~CLbsPrivLocWaitLocUpdateState()
	{
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocIdleState::ConstructL
// Description: CLbsPrivLocIdleState second-phase constructor.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocWaitLocUpdateState::ConstructL()
	{
	}


// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocUpdateState::OnEntry
// Description: Carries out tasks required on entry to the state.
// Issues the location update request and starts a timer.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocWaitLocUpdateState::OnEntry(const TPrivLocCommonParams& aStateParams)
	{
	TInt err(KErrNone);
	
	CLbsPrivLocStateBase::OnEntry(aStateParams);
	const TPrivLocWaitLocationUpdateParams& params = TPrivLocWaitLocationUpdateParams::Cast(aStateParams);	
	iFsm->IsEmergency() = params.iIsEmergency;
	iFsm->SessionType() = params.iSessionType;
	iFsm->PosRequestMethod() = params.iPosRequestMethod;

	// If the network has not specified a positioning method, get the default
	// one from the admin settings.
	TLbsNetPosMethodInt netReqMethod;
	iFsm->PosRequestMethod().GetPosMethod(0, netReqMethod);		
	if (iFsm->PosRequestMethod().NumPosMethods() == 1
		&& (netReqMethod.PosMode() == TPositionModuleInfo::ETechnologyUnknown))
		{
		AgpsInterface()->GetDefaultPosMethod(iFsm->PosRequestMethod());
		}

    // We may use two sources for the required quality for the
    // new location request, either:
    // 1) The quality from the network (aQuality)
    // 2) The quality defined in a quality profile (which profile to
    //    use depends on the service type, e.g. MT-LR or X3P)
    //
    // We decide which to use based on the required quality from the network.
    // Any invalid/unsupplied parameter is read from the quality profile 
    // for the ocation request type.
   	TBool maxFixTimeRequired = params.iQuality.MaxFixTime() == 0;
    TBool minVerticalAccuracyRequired = 
    				Math::IsNaN(params.iQuality.MinVerticalAccuracy());
    TBool minHorizontalAccuracyRequired = 
    				Math::IsNaN(params.iQuality.MinHorizontalAccuracy());
    
    if (maxFixTimeRequired || minVerticalAccuracyRequired || minHorizontalAccuracyRequired)
    	{
    	// Select which LbsAdmin setting to use for the
    	// quality profile Id based on the type of location
    	// request.
	    TLbsAdminSetting adminSetting(KLbsSettingNone);
	    switch (iFsm->SessionType())
	    	{
	    	case TLbsNetworkEnumInt::EServiceMobileTerminated:
	    	case TLbsNetworkEnumInt::EServiceNetworkInduced:
	    		{
	    		adminSetting = KLbsSettingQualityProfileExternalLocate;
	    		break;
	    		}
	    	case TLbsNetworkEnumInt::EServiceTransmitThirdParty:
	    		{
	    		adminSetting = KLbsSettingQualityProfileTransmitLocate;
	    		break;
	    		}
	    	case TLbsNetworkEnumInt::EServiceTriggeredMolr:
                // SUPL 2.0 "Triggered MOLR" request uses Self Locate Quality Profile
	    	case TLbsNetworkEnumInt::EServiceNetworkLocation:
	    		// This type of request should only get here in the case of a TA MOLR. Treat as Self-Locate
	    	case TLbsNetworkEnumInt::EServiceSelfLocation:
	    		{
	    		adminSetting = KLbsSettingQualityProfileSelfLocate;
	    		break;
	    		}
	    	default:
	    		{
				// We must not fail if it is an emergency request
				if (!iFsm->IsEmergency())
					{
					LBSLOG2(ELogP3, 
					"Unable to select quality profile for TLbsNetProtocolService (%d), using quality data from network instead.",
		    				iFsm->SessionType());
					__ASSERT_DEBUG(EFalse, Panic(ENrhPanicNoQualityProfile));
					}
	    		else
	    			{
	   				adminSetting = KLbsSettingQualityProfileExternalLocate;
	   				}
	    		}
	    	}

		// Retrieve the Id of the quality profile to use
	    TLbsQualityProfileId profileId(KLbsNullQualityProfileId);
	    if (adminSetting != KLbsSettingNone)
	    	{
	    	LbsAdmin()->Get(adminSetting, profileId);
	    	}

	    // Retrieve the data for the quality profile
    	TQualityProfile qualityProfile;
    	err = LbsQualityProfile::GetQualityProfileById(profileId, qualityProfile);
    	if (err == KErrNone)
    		{
    		// Use the quality data from the quality profile for any missing/invalid data
			if(maxFixTimeRequired)
				{
				iFsm->GpsRequestQuality().SetMaxFixTime(qualityProfile.MaxFixTime());
				}
			else
				{
				iFsm->GpsRequestQuality().SetMaxFixTime(params.iQuality.MaxFixTime());
				}
			if(minHorizontalAccuracyRequired)
				{
				iFsm->GpsRequestQuality().SetMinHorizontalAccuracy(qualityProfile.MinHorizontalAccuracy());
				}
			else
				{
				iFsm->GpsRequestQuality().SetMinHorizontalAccuracy(params.iQuality.MinHorizontalAccuracy());
				}
			if(minVerticalAccuracyRequired)
				{
				iFsm->GpsRequestQuality().SetMinVerticalAccuracy(qualityProfile.MinVerticalAccuracy());
				}
			else
				{
				iFsm->GpsRequestQuality().SetMinVerticalAccuracy(params.iQuality.MinVerticalAccuracy());				
				}
    		}
		else
			{
			// We should not fail if we are emergency
			if (!iFsm->IsEmergency())
				{
				// We couldn't find the quality profile with the given Id.
				// This is an error, so reject the location request.
				TPositionInfo dummyPosInfo;
				TTime dummyTime;
				TLbsNetPosRequestQualityInt dummyQuality;
				MessageSwitch()->SendNetLocResponse(iFsm->SessionId(),
									KErrAccessDenied,
										dummyQuality,
										dummyPosInfo,
										dummyTime,
										iFsm->IsEmergency());
			
				// if this location request is the result of a privacy request,
				// then notify the privacy handler of the error
				if ((params.iSessionType == TLbsNetworkEnumInt::EServiceMobileTerminated) ||
					(params.iSessionType == TLbsNetworkEnumInt::EServiceNetworkInduced))
					{
					iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitSessionComplete, KErrAccessDenied);
					}
				else
					{
					iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitBadQualityProfile, KErrNone);
					}

				// Whatever the result, this session is finished, so return to idle.
				iFsm->ChangeState(CLbsPrivLocFsm::EStateIdle, iFsm->SessionId());
				return;
				} // if (!iIsEmergency)
			else
				{
				// Just set some defaults
				TTimeIntervalMicroSeconds timeout(30000000);
				iFsm->GpsRequestQuality().SetMinHorizontalAccuracy(50);
				iFsm->GpsRequestQuality().SetMinVerticalAccuracy(1000);
				iFsm->GpsRequestQuality().SetMaxFixTime(timeout);
				}
    		}
    	}
    else
    	{
    	// Use the quality parameters supplied with the request.
    	iFsm->GpsRequestQuality().SetMinHorizontalAccuracy(params.iQuality.MinHorizontalAccuracy());
    	iFsm->GpsRequestQuality().SetMinVerticalAccuracy(params.iQuality.MinVerticalAccuracy());
    	iFsm->GpsRequestQuality().SetMaxFixTime(params.iQuality.MaxFixTime());
    	}


    // Check for any existing position updates in case they meet the
    // MaxFixAge and quality requirements for this request.
    TInt updateReason;
    err = AgpsInterface()->GetPosition(updateReason, 
    								   iFsm->GpsPosition(), 
    								   iFsm->ActualTime());
    if (err == KErrNone)
    	{
		TPrivLocWaitLocationRequestParams locationRequestParams(iFsm->SessionId(),
																iFsm->IsEmergency(),
																EFalse,
																updateReason);

    	// Check the existing update in case it meets the MaxFixAge and quality requirements for this request.
    	if (params.iQuality.MaxFixAge() > 0)
    		{
    		TTime now;
    		now.UniversalTime();
    		TTimeIntervalMicroSeconds age(Max((now.Int64() - iFsm->ActualTime().Int64()), TInt64(0)));
    		if (updateReason == KErrNone
    			&& (age <= params.iQuality.MaxFixAge())
    			&& ReceivedFixIsAccurate())
    			{
    			// Accurate update that is within the MaxFixAge time limit,
    			// so return it straight away.
    			iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocFixReceived, updateReason);
    			iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitLocationRequest, locationRequestParams);
    			return;
    			}
    		}
    		

		// Special feature behaviour!
		// If the admin setting KLbsSpecialFeatureIntermediateFutileUpdate is on,
		// it means that we should check to see if a futile update has happened
		// since the first location request of this session. This might happen
		// in a hybrid session, if the GPS module sends a futile update when there
		// is no outstanding location request in the NRH. E.g. in the gap between
		// sending the response for one hybrid loc request and getting the next
		// loc request from the network.
		//
		// Note: This only really applies to hybrid of TA position modes, because
		//       in TB or autonomous you only have one location request per 
		//       session.
    	else if (iFsm->IsSpecialFeatureIntermediateFutileUpdateOn())
    		{
		    // If this is the first request for a new sessionId, record the current session id.
		    // We need to know this for terminal assisted or hybrid requests, in case
		    // we need to check for a futile update that has happened in the gap between
		    // one location response and the next location update request.
		    if (iFsm->LastLocReqSessionId() != iFsm->SessionId())
		    	{
		    	iFsm->LastLocReqSessionId() = iFsm->SessionId();
		    	}
		    else
				{
				// Before sending the location request, see if a futile update has 
				// happened since the start of the session (in general only terminal-assisted 
				// and hybrid requests should have more than one location request
			    // per session, however the SUPL PM will have more than one for all request modes).
   			    TGpsRequestMode gpsMode = AgpsInterface()->ConvertPosMethodToGpsRequestMode(iFsm->PosRequestMethod());
				if ((updateReason == KPositionCalculationFutile) && 
					((gpsMode == EGpsRequestModeTerminalAssisted) || (gpsMode == EGpsRequestModeHybrid)))
					{
					// Return last measurement straight away.
					iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocMeasurementResultsReceived, updateReason);
					iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitLocationRequest, locationRequestParams);
		    		return;
					}
				}
    		}
    	}

    
	// Issue the request and supply pointers to the data to be updated
    iFsm->LocationFixReceived() = EFalse;
    iFsm->MeasurementInfoReceived() = EFalse;
    err = AgpsInterface()->StartPositioning(iFsm->SessionId(),
								    		iFsm->PosRequestMethod(),
											iFsm->GpsRequestQuality(),
											iFsm->IsEmergency());
	if (KErrNone == err)
		{
		iFsm->LocationUpdateTimer().EventAfter(iFsm->GpsRequestQuality().MaxFixTime(), 1);
    	iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitReasonNone, KErrNone);
		}
	else
		{
		// Error sending the location request, send a location response
		// with the error and go to Idle state.
    	iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitBadLocationRequest, err);
    	iFsm->ChangeState(CLbsPrivLocFsm::EStateIdle, iFsm->SessionId());
		}
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocUpdateState::OnExit
// Description: Carries out tasks required on exit from the state.
// Cancels the location update request and stops the timer.
// ----------------------------------------------------------------------------- 
//
TBool CLbsPrivLocWaitLocUpdateState::OnExit()
	{	
	// Cancel the update timer.
	iFsm->LocationUpdateTimer().Cancel();
	
	TInt consumed = EFalse;
	switch(iFsm->ExitData().iExitReason)
		{
		case TPrivLocStateExitData::EExitLocFixReceived:
			{
			// Don't cancel the location request yet, but tell the AGPS interface
			// handler to put it on 'hold'. If we are in a hybrid or terminal-assisted
			// request then we are going to get another location request very shortly
			// anyway...
			AgpsInterface()->HoldPositioning(iFsm->SessionId(), KErrNone);
	
			// Report the position to the message switch
			MessageSwitch()->SendNetLocResponse(iFsm->SessionId(),
												iFsm->ExitData().iExitInfo,
            	                             	iFsm->GpsRequestQuality(),
            	                             	iFsm->GpsPosition(),
            	                             	iFsm->ActualTime(),
            	                             	iFsm->IsEmergency());

			// For MTLR pass the data to the privacy handler 
			// in case the Privacy Controller wants it.
			if ((iFsm->ExitData().iExitInfo >= KErrNone) &&
				(iFsm->ExitData().iExitInfo != KPositionCalculationFutile) && 
				((iFsm->SessionType() == TLbsNetworkEnumInt::EServiceMobileTerminated) ||
				(iFsm->SessionType() == TLbsNetworkEnumInt::EServiceNetworkInduced)))
				{
				PrivacyHandler()->ProcessNetworkPositionUpdate(iFsm->SessionId(), 
						iFsm->GpsPosition());
				}

			consumed = ETrue;
			break;
			}

		case TPrivLocStateExitData::EExitLocMeasurementResultsReceived:
			{
			// Don't cancel the location request yet, but tell the AGPS interface
			// handler to put it on 'hold'. If we are in a hybrid or terminal-assisted
			// request then we are going to get another location request very shortly
			// anyway...
			AgpsInterface()->HoldPositioning(iFsm->SessionId(), KErrNone);

			// Report the measurement data to the message switch, even if we
			// didn't get any. The error code will indicate that the data
			// is rubbish in that case.
			LBSLOG2(ELogP3, "CLbsPrivLocWaitLocUpdateState:returning with reason %d", iFsm->MeasurementInfoError());

			MessageSwitch()->SendNetLocResponse(iFsm->SessionId(),
												iFsm->ExitData().iExitInfo,
												iFsm->GpsRequestQuality(),
            	                             	iFsm->GpsMeasurementInfo(),
            	                             	iFsm->ActualTime(),
            	                             	iFsm->IsEmergency());
			break;	
			}
		
		case TPrivLocStateExitData::EExitTimedOut:
			{
			// Don't cancel the location request yet, but tell the AGPS interface
			// handler to put it on 'hold'. If we are in a hybrid or terminal-assisted
			// request then we are going to get another location request very shortly
			// anyway...
			AgpsInterface()->HoldPositioning(iFsm->SessionId(), KErrNone);

			// If the request has timed out, then return whatever position 
			// data we have, but make it clear it's not what was requested.
			// If there's an error (probably KErrTimedOut) there's
			// nothing to report, so send dummy data with the error.			
			MessageSwitch()->SendNetLocResponse(iFsm->SessionId(), 
												iFsm->ExitData().iExitInfo,
												iFsm->GpsRequestQuality(),
												iFsm->GpsPosition(),
												iFsm->ActualTime(),
												iFsm->IsEmergency());

			// For MTLR, pass the data to the privacy handler in case the Privacy
			// Controller wants it.
			// NB Don't send the update if the error is KErrTimedOut, as that means there's
			// nothing to report.
			if((iFsm->SessionType() == TLbsNetworkEnumInt::EServiceMobileTerminated) &&
			    (iFsm->ExitData().iExitInfo == KPositionQualityLoss))
				{
				PrivacyHandler()->ProcessNetworkPositionUpdate(iFsm->SessionId(), 
						iFsm->GpsPosition());
				}

			consumed = ETrue;
			break;
			}
			
		case TPrivLocStateExitData::EExitCancelledByPrivacyController:
			{
			// Stop the location request immediately.
			AgpsInterface()->StopPositioning(iFsm->SessionId());
			
			// Send a SendExternalLocateCancel to NetGateWay- if the protcol module does not support this then
			// the Gateway will do nothing
			MessageSwitch()->SendExternalLocateCancel(iFsm->SessionId(), KErrCancel);
				
			// Send a location response with 'cancel' set to the network
			TPositionInfo dummyPosInfo;
			TTime dummyTime;
			TLbsNetPosRequestQualityInt dummyQuality;
			MessageSwitch()->SendNetLocResponse(iFsm->SessionId(),
												iFsm->ExitData().iExitInfo,
												dummyQuality,
												dummyPosInfo,
												dummyTime,
												iFsm->IsEmergency());
			
			consumed = ETrue;
			}
			break;

		case TPrivLocStateExitData::EExitBadQualityProfile:
			{
			// Do nothing; we're just going back to Idle state
			consumed = ETrue;
			break;
			}
		
		case TPrivLocStateExitData::EExitBadLocationRequest:
			{
			// Error processing the location request - 
			// send a dummy response with an error code.
			TPositionInfo dummyPosInfo;
			TTime dummyTime;
			TLbsNetPosRequestQualityInt dummyQuality;
			MessageSwitch()->SendNetLocResponse(iFsm->SessionId(),
												iFsm->ExitData().iExitInfo,
												dummyQuality,
												dummyPosInfo,
												dummyTime,
												iFsm->IsEmergency());
			
			consumed = ETrue;
			break;
			}
		
		default:
			{
			consumed = CLbsPrivLocStateBase::OnExit();
			// If the exit reason wasn't handled, panic (should only happen in development)
			__ASSERT_DEBUG(consumed, Panic(ENrhPanicWaitLocUpdateUnknownExitReason));
			}
		}
	return(consumed);
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocIdleState::OnNetLocRequest
// Description: The Message Switch has forwarded a request for a network 
// location.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocWaitLocUpdateState::OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId, 
						const TLbsNetPosRequestMethodInt& aPosRequestMethod,
						 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						 TBool aIsEmergency,
						 const TLbsNetPosRequestQualityInt& aQuality)
	{
	TInt numMethods = aPosRequestMethod.NumPosMethods();
	if (numMethods==1)
		{
		TLbsNetPosMethodInt netPosMethod;
		aPosRequestMethod.GetPosMethod(0,netPosMethod);
				
		if (netPosMethod.PosMode()== (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted))
			{
			iFsm->TapMode() = ETrue;
			}
		}
	if(aSessionId != iFsm->SessionId())
		{
		/* This request is for a different session. Cancel the current one 
		 * and start a new one.
		 */
		HandleLocRequest(aSessionId,aPosRequestMethod,
							aSessionType,aIsEmergency,
							aQuality);
		}
	else
		{
		LBSLOG(ELogP3, "CLbsPrivLocWaitLocUpdateState::OnNetLocRequest: Matching SessionId.");
		TPrivLocWaitLocationUpdateParams updateRequestParams(aSessionId,
				aPosRequestMethod,
				aSessionType,
				aIsEmergency,
				aQuality);
		iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocReqReceived, KErrNone);
		iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitLocationUpdate, updateRequestParams);
		}
	}


void CLbsPrivLocWaitLocUpdateState::OnMTLRRequest(const TLbsNetSessionIdInt& /*aSessionId*/,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt /*aSessionType*/, 
					   TBool /*aIsEmergency*/,
					   const TLbsExternalRequestInfo& /*aExternalRequestInfo*/,
					   const TLbsNetPosRequestPrivacyInt& /*aNetPosRequestPrivacy*/)
	{
	// this can never happen. If the Fsm is in the WaitLocUpdateState then 
	// any arrival of a MTLR request would start a new session and not
	// implicitly cancel the ongoing MTLR and the OnMTLRRequest()
	// would be directed to that session not this one
		
	}
	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocUpdateState::OnTimerEventL
// Description: The Location Update timer has expired.
// Cancel the request, and pass on the response if any has been received,
// otherwise report failure.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocWaitLocUpdateState::OnTimerEventL(TInt /*aTimerId*/)
	{	
	LBSLOG(ELogP3, "CLbsPrivLocWaitLocUpdateState::OnTimerEventL");
	
	if(iFsm->MeasurementInfoReceived())
		{
		// A position fix may have been received, but it can't be accurate enough
		// (otherwise the request would have been completed before timeout), so 
		// return the most recent measurement info		
		LBSLOG(ELogP3, "OnTimerEventL, measurement data received");
		iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocMeasurementResultsReceived, 
									 iFsm->MeasurementInfoError());
		}
	else if(iFsm->LocationFixReceived())
		{
		// position received, but not accurate enough (or request would already have been completed)
		LBSLOG(ELogP3, "OnTimerEventL, inaccurate location data received");
		iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitTimedOut, 
									 KPositionQualityLoss);
		}
	else
		{
		// we've received no update (position / measurements)
		LBSLOG(ELogP3, "OnTimerEventL, NO measurement data received");
		LBSLOG(ELogP3, "(Setting exit info KErrPositionNoGpsUpdate");
		iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitTimedOut, 
									 KErrPositionNoGpsUpdate);
		}

	SetExitState();	
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocUpdateState::SetExitState
// Description: Works out the next state on the basis of the current session 
// type and whether any update has been received.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocWaitLocUpdateState::SetExitState()
	{
	TPrivLocWaitLocationRequestParams locationRequestParams(iFsm->SessionId(),
															iFsm->IsEmergency(),
															EFalse,
															iFsm->ExitData().iExitInfo);
	iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitLocationRequest, 
					locationRequestParams);
	}

TBool CLbsPrivLocWaitLocUpdateState::ReceivedFixIsAccurate()
	{    
	TBool fixIsAccurate = EFalse;

	// Compare the accuracy to the request values.
	// Make sure the location update is (A)GPS and not Network based.
	TPosition latestPosition;
	iFsm->GpsPosition().GetPosition(latestPosition);
	
	if ((latestPosition.HorizontalAccuracy() <= iFsm->GpsRequestQuality().MinHorizontalAccuracy()) &&
		(latestPosition.VerticalAccuracy() <= iFsm->GpsRequestQuality().MinVerticalAccuracy()) &&
		(iFsm->GpsPosition().PositionMode() != TPositionModuleInfo::ETechnologyNetwork))//Pure Reference Location
		{
		fixIsAccurate = ETrue;
		}

	return(fixIsAccurate);
    }

/** Callback when a GPS position update arrives from AGPS manager.
*/
void CLbsPrivLocWaitLocUpdateState::OnAgpsPositionUpdate(
	TInt aReason,
	const TPositionExtendedSatelliteInfo& aPosInfo,
	const TTime& aTimeStamp)
	{
	iFsm->GpsPosition() = aPosInfo;
	iFsm->ActualTime() = aTimeStamp;
	iFsm->LocationFixReceived() = ETrue;
	iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocFixReceived, aReason);

	if (KErrNone == aReason)
		{
		if (iFsm->TapMode())
			{
			LBSLOG(ELogP1,"TAP mode) - NOT sending position to network");	
			return; // do NOT return AGPS postions to TAP mode sessions
			}
		// See if the reported accuracy matches the specified quality.
		// If the accuracy is good enough, report the position
		
		// if this session is TAP then discard the position
		
		if(ReceivedFixIsAccurate())
			{
			SetExitState();
			}
		}
	else if ((aReason <= KErrNone) || (KPositionCalculationFutile == aReason))
		{
		// GPS Manager can't provide a location update. return what we do have.
		if(iFsm->MeasurementInfoReceived())
			{
			LBSLOG(ELogP1,"CLbsPrivLocWaitLocUpdateState::OnPositionUpdate() - measurement received");	
			iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocMeasurementResultsReceived, aReason);
			}
		else
			{
			iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocFixReceived, aReason);				
			}
		SetExitState();
		}
	else if (KPositionEarlyComplete == aReason)
		{
		// Not an error. Report back what was accepted.
		iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocFixReceived, KErrNone);				
		SetExitState();	
		}
	else
		{
		// A real error
		SetExitState();
		}
	}

/** Callback when a GPS measurement results update arrives from AGPS manager.

Only location requests that are 'hybrid' or 'terminal assisted' should record 
the measurement results. Other types of request (autonomous, terminal based)
are only interested in the GPS position update.
*/
void CLbsPrivLocWaitLocUpdateState::OnAgpsMeasurementUpdate(
	TInt aReason,
	const TPositionGpsMeasurementInfo& aPosInfo,
	const TTime& /*aTimeStamp*/)
	{
	// Check that we should be listening for measurement updates.
	
	TBool positionCalculationPossible = aPosInfo.PositionCalculationPossible();
	
	const TInt methodCount = iFsm->PosRequestMethod().NumPosMethods();
	for(TInt i = 0; i < methodCount; ++i)
		{
		TLbsNetPosMethodInt posMethod;
		iFsm->PosRequestMethod().GetPosMethod(i, posMethod);
		if((posMethod.PosMode() & KTerminalAssistedMode) == KTerminalAssistedMode)
			{
			iFsm->MeasurementInfoReceived() = ETrue;
			iFsm->MeasurementInfoError() = aReason;
			iFsm->GpsMeasurementInfo() = aPosInfo;

			// don't wait until alpha2 time expires, instead
			// return measuremnts now
			if (positionCalculationPossible)
				{
				iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocMeasurementResultsReceived, 
											 iFsm->MeasurementInfoError());
				SetExitState();	
				}
			break;
			}
		}
	}

/** Callback when a GPS measurement results update arrives from AGPS manager.

Only location requests that are 'hybrid' or 'terminal assisted' should record 
the measurement results. Other types of request (autonomous, terminal based)
are only interested in the GPS position update.
*/
void CLbsPrivLocWaitLocUpdateState::OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& aSessionId, 
		const TPositionInfoBase& aPosInfo)
	{
	CLbsPrivLocStateBase::OnNetLocReferenceUpdate(aSessionId,aPosInfo);
	}


// ----------------------------------------------------------------------------- 
// 
// ----------------------- Class CLbsPrivLocWaitLocReqState --------------------
//
// Implements the Wait For Location Request state of the Privacy and Location 
// Request Handler
//
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocWaitLocReqState* CLbsPrivLocWaitLocReqState::NewL(CLbsPrivLocFsm* aFsm)
	{
	return new(ELeave)CLbsPrivLocWaitLocReqState(aFsm);
	}
	
CLbsPrivLocWaitLocReqState::CLbsPrivLocWaitLocReqState(CLbsPrivLocFsm* aFsm)	
: CLbsPrivLocStateBase(aFsm)
	{
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocReqState::OnEntry
// Description: Carries out tasks required on entry to the state.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocWaitLocReqState::OnEntry(const TPrivLocCommonParams& aStateParams)
	{
	CLbsPrivLocStateBase::OnEntry(aStateParams);
	const TPrivLocWaitLocationRequestParams& params = TPrivLocWaitLocationRequestParams::Cast(aStateParams);	
	iFsm->IsEmergency() = params.iIsEmergency;
	iFsm->PrivacyRequestCancelled() = params.iReqCancelled;
	iFsm->PreviousStateExitInfo() = params.iPreviousStateExitInfo;
	}

	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocReqState::OnExit
// Description: Carries out tasks required on exit from the state.
// Panics if the exit reason is not handled by the base state exit
// ----------------------------------------------------------------------------- 
//
TBool CLbsPrivLocWaitLocReqState::OnExit()
	{
	TBool consumed = CLbsPrivLocStateBase::OnExit();
	// If the exit reason wasn't handled, panic (should only happen in development)
	__ASSERT_DEBUG(consumed, Panic(ENrhPanicWaitLocReqUnknownExitReason));
	return(consumed);
	}

void CLbsPrivLocWaitLocReqState::OnMTLRRequest(const TLbsNetSessionIdInt& /*aSessionId*/,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt /*aSessionType*/, 
					   TBool /*aIsEmergency*/,
					   const TLbsExternalRequestInfo& /*aExternalRequestInfo*/,
					   const TLbsNetPosRequestPrivacyInt& /*aNetPosRequestPrivacy*/)
	{
	// this can never happen. If the Fsm is in the WaitLocReqState then 
	// any arrival of a MTLR request would start a new session and the OnMTLRRequest()
	// would be directed to that session not this one
	__ASSERT_DEBUG(EFalse, Panic(ENrhPanicBadParamType)); 
	}
	
void CLbsPrivLocWaitLocReqState::OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId, 
						const TLbsNetPosRequestMethodInt& aPosRequestMethod,
						 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						 TBool aIsEmergency,
						 const TLbsNetPosRequestQualityInt& aQuality)
	{
	TInt numMethods = aPosRequestMethod.NumPosMethods();
	if (numMethods==1)
		{
		TLbsNetPosMethodInt netPosMethod;
		aPosRequestMethod.GetPosMethod(0,netPosMethod);
				
		if (netPosMethod.PosMode()== (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted))
			{
			iFsm->TapMode() = ETrue;
			}
		}
	
	
	if(aSessionId == iFsm->SessionId())
		{
		if (iFsm->PrivacyRequestCancelled())
			{
			iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitCancelledByPrivacyController, KErrCancel);
			TPrivLocWaitLocationRequestParams locationRequestParams(iFsm->SessionId(), 
																	iFsm->IsEmergency(), 
																	iFsm->PrivacyRequestCancelled());
			iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitLocationRequest, locationRequestParams);
			}
		else
			{
			iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitLocReqReceived, KErrNone);
			TPrivLocWaitLocationUpdateParams updateRequestParams(aSessionId,
																 aPosRequestMethod,
																 aSessionType,
																 aIsEmergency,
																 aQuality);
			iFsm->ChangeState(CLbsPrivLocFsm::EStateWaitLocationUpdate, updateRequestParams);
			}
		}
	else
		{
		HandleLocRequest(aSessionId,aPosRequestMethod,
							aSessionType,aIsEmergency,
							aQuality);
		}
	}

void CLbsPrivLocWaitLocReqState::OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aSessionId)
	{
	if (!iFsm->IsEmergency() && (aSessionId == iFsm->SessionId()))
			{
			if (!iFsm->PrivacyRequestCancelled() )
				{
				TLbsNetSessionIdInt sessionId = iFsm->SessionId();
				MessageSwitch()->SendExternalLocateCancel(sessionId,KErrCancel);
				}
			iFsm->PrivacyRequestCancelled() = ETrue;
			}
	}


/** Called when a reference position arrives from the network.
*/
void CLbsPrivLocWaitLocReqState::OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& aSessionId, 
		const TPositionInfoBase& aPosInfo)
	{
	// if the MTLR is still active (has not been cancelled by the privacy handler)
	if (!iFsm->PrivacyRequestCancelled())
		{
		CLbsPrivLocStateBase::OnNetLocReferenceUpdate(aSessionId, aPosInfo);
		}	
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocReqState::OnSessionComplete
// Description: handling of a session complete message
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocWaitLocReqState::OnSessionComplete(const TLbsNetSessionIdInt& aSessionId,
																TInt aReason)
	{
	
	
	if(aSessionId == iFsm->SessionId())
		{
		// Make sure the reason passed with the Session Complete is sent to the
 		// Privacy Controller EXCEPT when the update previously passed to the 
 		// network didn't meet the quality criteria. In this case use the
 		// KPositionQualityLoss status.
		TInt completionReason = aReason;
		if(aReason == KErrNone)
 			{
 			if(KPositionQualityLoss == iFsm->PreviousStateExitInfo())
 				{
 				completionReason = KPositionQualityLoss;
 				}
 			}
		
		iFsm->ExitData().SetExitData(TPrivLocStateExitData::EExitSessionComplete, completionReason);
		iFsm->ChangeState(CLbsPrivLocFsm::EStateIdle, aSessionId);	            
		}		
	}

// ----------------------------------------------------------------------------- 
//
// Package classes
//
// ----------------------------------------------------------------------------- 
//
TPrivLocCommonParams::TPrivLocCommonParams()
	{	
	}
TPrivLocCommonParams::TPrivLocCommonParams(TLbsNetSessionIdInt aSessionId)
	{
	iSessionId = aSessionId;												
	}

TPrivLocWaitLocationRequestParams::TPrivLocWaitLocationRequestParams()
	{
	}
TPrivLocWaitLocationRequestParams::TPrivLocWaitLocationRequestParams(
			const TLbsNetSessionIdInt& aSessionId,
			TBool	aIsEmergency,
			TBool   aReqCancelled,
			TInt    aPreviousStateExitInfo) :
	TPrivLocCommonParams(aSessionId),
	iIsEmergency(aIsEmergency),
	iReqCancelled(aReqCancelled),
	iPreviousStateExitInfo(aPreviousStateExitInfo)
	{
	}

TPrivLocWaitLocationUpdateParams::TPrivLocWaitLocationUpdateParams()
	{	
	}
TPrivLocWaitLocationUpdateParams::TPrivLocWaitLocationUpdateParams(
			const TLbsNetSessionIdInt& aSessionId,
			const TLbsNetPosRequestMethodInt& aPosRequestMethod,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt  aSessionType,
			TBool aIsEmergency,
			const TLbsNetPosRequestQualityInt& aQuality) :
	TPrivLocCommonParams(aSessionId),
	iSessionType(aSessionType),
	iIsEmergency(aIsEmergency),
	iQuality(aQuality),
	iPosRequestMethod(aPosRequestMethod)
	{
	}

TPrivLocWaitPrivResponseParams::TPrivLocWaitPrivResponseParams()
	{
	
	}
TPrivLocWaitPrivResponseParams::TPrivLocWaitPrivResponseParams(
			const TLbsNetSessionIdInt& aSessionId,
			const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
			const TLbsExternalRequestInfo& aExternalRequestInfo,
			const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy,
			TBool aIsEmergency) :
	TPrivLocCommonParams(aSessionId),
	iNetPosRequestPrivacy(aNetPosRequestPrivacy),
	iIsEmergency(aIsEmergency),
	iSessionType(aSessionType)
	{
	// Need to check the type of aExternalRequestInfo before 
	// copying it into this class.
	if (aExternalRequestInfo.ClassType() == EExternalRequestInfoClass)
		{
		__ASSERT_DEBUG(aExternalRequestInfo.ClassSize() == sizeof(TLbsExternalRequestInfo), 
					   Panic(ENrhPanicInvalidExternalRequestInfoType));
		
		Mem::Copy(&iExternalRequestInfo, 
				  &aExternalRequestInfo, 
				  sizeof(TLbsExternalRequestInfo));
		}
	else 
		{
		if (aExternalRequestInfo.ClassType() == (EExternalRequestInfoClass | EExternalRequestInfoClass2))
			{
			__ASSERT_DEBUG(aExternalRequestInfo.ClassSize() == sizeof(TLbsExternalRequestInfo2), 
						   Panic(ENrhPanicInvalidExternalRequestInfoType));

			Mem::Copy(&iExternalRequestInfo, 
					  &aExternalRequestInfo, 
					  sizeof(TLbsExternalRequestInfo2));
			}
		else
			{
			Panic(ENrhPanicInvalidExternalRequestInfoType);
			}
		}
	}

// ----------------------------------------------------------------------------- 
// 
// ----------------------------- Class CLbsPrivLocFsm --------------------------
//
// State Machine class which owns the states of the Privacy and Location Handler
//
// ----------------------------------------------------------------------------- 
//

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::NewL
// Description: CLbsPrivLocFsm static constructor 
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocFsm* CLbsPrivLocFsm::NewL(
		CPrivacyAndLocationHandler& aPrivLocHandler,
		const TLbsNetSessionIdInt& aSessionId)
	{
	CLbsPrivLocFsm* self; 
	self = new (ELeave) CLbsPrivLocFsm(aPrivLocHandler, aSessionId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);	
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::CLbsPrivLocFsm
// Description: CLbsPrivLocFsm constructor 
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocFsm::CLbsPrivLocFsm(
		CPrivacyAndLocationHandler& aPrivLocHandler,
		const TLbsNetSessionIdInt& aSessionId) :
	iPrivLocHandler(aPrivLocHandler),
	iSessionId(aSessionId),
	iIsEmergency(EFalse),
	iSessionType(TLbsNetworkEnumInt::EServiceNone),
	iRefPosProcessed(EFalse),
	iLocReqReceived(EFalse),
	iReqCancelled(EFalse),
	iWasPrivacyResponseReceivedStateExited(EFalse),
	iPositioningStatusIncremented(EFalse)
	{	
	}
	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::~CLbsPrivLocFsm
// Description: CLbsPrivLocFsm destructor 
// ----------------------------------------------------------------------------- 
//
CLbsPrivLocFsm::~CLbsPrivLocFsm()
	{
	delete iLocationUpdateTimer;
	iStates.DeleteAll();
	iStates.Reset();
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::SessionId
// Description: Get the current session Id for this FSM.
// ----------------------------------------------------------------------------- 
//
const TLbsNetSessionIdInt& CLbsPrivLocFsm::SessionId() const
	{
	return iSessionId;
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::ConstructL
// Description: CLbsPrivLocFsm second-phase constructor.
//              Creates the states of the system and the Privacy Handler.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocFsm::ConstructL()
	{
	// Create the states
	iStates.At(EStateIdle) = CLbsPrivLocIdleState::NewL(this);
	iStates.At(EStateWaitPrivacyResponse) = CLbsPrivLocWaitPrivRespState::NewL(this);
	iStates.At(EStateWaitLocationRequest) = CLbsPrivLocWaitLocReqState::NewL(this);
	iStates.At(EStateWaitLocationUpdate) = CLbsPrivLocWaitLocUpdateState::NewL(this);

	iCurrentState = iStates.At(EStateIdle);
    // When waiting for an update, there is a maximum duration specified by the
    // LBS admin data to avoid the risk of hanging around forever in the event of 
    // a problem with the A-GPS module. Create a timer to deal with this.
    iLocationUpdateTimer = CLbsCallbackTimer::NewL(*this);
	}
	
	
TBool CLbsPrivLocFsm::IsSpecialFeatureIntermediateFutileUpdateOn()
	{
	return PrivLocHandler().IsSpecialFeatureIntermediateFutileUpdateOn();
	}

// ----------------------------------------------------------------------------- 
// CPrivacyAndLocationHandler::SetServerObserver
// Description: Store a pointer to the NRH server which comunicates with the 
// Privacy Controller.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocFsm::SetServerObserver(MLbsSessionObserver* aNrhServer)
    {
    PrivLocHandler().PrivacyHandler()->SetServerObserver(aNrhServer);
    }

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::OnRespondNetworkLocationRequest
// Description: Called by the Privacy Handler to report the result of a privacy
// check. Handling of the response is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocFsm::OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId, 
                            TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
                            TInt aResponseReason)
	{
	LBSLOG3(ELogP3, "FSM(%d) OnRespondNetworkLocationRequest response=%d",iSessionId.SessionNum(),aRequestResult);
	iCurrentState->OnRespondNetworkLocationRequest(aRequestId, aRequestResult, aResponseReason);
    }

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::OnCancelNetworkLocationRequest
// Description: Called by the Privacy Handler to report that a privacy check 
// has been rejected. This may occur after it has already been accepted.
// Handling of the response is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocFsm::OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId)
    {
	LBSLOG2(ELogP3, "FSM(%d) OnCancelNetworkLocationRequest",iSessionId.SessionNum());
	iCurrentState->OnCancelNetworkLocationRequest(aRequestId);
    }

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::OnMTLRRequest
// Description: The Message Switch has forwarded a request to start an MTLR 
// session.
// Handling of the request is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocFsm::OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
					   TBool aIsEmergency,
					   const TLbsExternalRequestInfo& aExternalRequestInfo,
					   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy)
	{
	LBSLOG2(ELogP3, "FSM(%d) OnMTLRRequest",iSessionId.SessionNum());
	iCurrentState->OnMTLRRequest(aSessionId, 
								aSessionType, 
								aIsEmergency, 
								aExternalRequestInfo, 
								aNetPosRequestPrivacy);
	}
	
// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::OnSessionComplete
// Description: The Message Switch has reported that the session is
// over (complete or aborted due to some error).
// Handling of the message is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocFsm::OnSessionComplete(
									const TLbsNetSessionIdInt& aSessionId,
									TInt aReason)
	{
	LBSLOG3(ELogP3, "FSM(%d) OnSessionComplete reason=%d",iSessionId.SessionNum(),aReason);
	iCurrentState->OnSessionComplete(aSessionId, aReason);
	
    // update the positioning status. Note this is updated only if it was previously
    // incremented as a result of this session.
    if (WasPositioningStatusIncremented())
        {
        PrivLocHandler().DecrementPositioningStatus();
        WasPositioningStatusIncremented() = EFalse;
        }
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::OnNetLocRequest
// Description: The Message Switch has passed on a request for a position update
// Handling of the request is delegated to the current state.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocFsm::OnNetLocRequest(
						const TLbsNetSessionIdInt& aSessionId, 
						const TLbsNetPosRequestMethodInt& aPosRequestMethod,
						TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						TBool aIsEmergency,
						const TLbsNetPosRequestQualityInt& aQuality)
	{
	LBSLOG2(ELogP3, "FSM(%d) OnNetLocRequest",iSessionId.SessionNum());
	iCurrentState->OnNetLocRequest(aSessionId,
	 					aPosRequestMethod, 
	 					aSessionType, 
						aIsEmergency, 
						aQuality);
	}

/** Called when a reference position arrives from the network.
*/
void CLbsPrivLocFsm::OnNetLocReferenceUpdate(
		const TLbsNetSessionIdInt& aSessionId, 
		const TPositionInfoBase& aPosInfo)
	{
	LBSLOG2(ELogP3, "FSM(%d) OnNetLocReferenceUpdate",iSessionId.SessionNum());
	iCurrentState->OnNetLocReferenceUpdate(aSessionId, aPosInfo);	
	}

/** Callend when a final location arrives from the network.

Currently the final network position is never used by the 
state machine - it is only needed by the X3P handler. 
So this function just ignores the update.
*/
void CLbsPrivLocFsm::OnNetLocFinalUpdate(
		const TLbsNetSessionIdInt& /*aSessionId*/, 
		const TPositionInfoBase& /*aPosInfo*/)
	{
	// Final network position not used by CLbsPrivLocFsm, so ignore it.
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::ChangeState
// Description: Called by a state of the FSM when a transition is required.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocFsm::ChangeState(TLocPrivacyHandlerState aNewStateId,const TPrivLocCommonParams& aStateParams)
	{
	// Tidy up the old state
	if(iCurrentState)
		{
		// coverity[unchecked_value]
		// We're not interested in whether it was consumed here
		iCurrentState->OnExit();
		}

	// Note, here the session ID has already being set when the Fsm was created (when session first came into being)
	// so no need to do this ... iSessionId = aStateParams.iSessionId;

	// Set the new state
	iCurrentState = iStates.At(aNewStateId);

	LBSLOG3(ELogP3, "FSM(%d) Entering state %d",iSessionId.SessionNum(), aNewStateId);
	
	// Do any initialisation for the new state.
	iCurrentState->OnEntry(aStateParams);
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::ChangeState
// Description: Called by a state of the FSM when a transition is required to a 
// state which only requires the session Id
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocFsm::ChangeState(TLocPrivacyHandlerState aNewStateId,
											const TLbsNetSessionIdInt& aSessionId)
	{
	TPrivLocCommonParams commonParams(aSessionId);
	ChangeState(aNewStateId, commonParams);
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocFsm::PrivLocHandler
// Description: Get the CPrivacyAndLocationHandler object
// ----------------------------------------------------------------------------- 
//
CPrivacyAndLocationHandler& CLbsPrivLocFsm::PrivLocHandler()
	{
	return iPrivLocHandler;
	}

// ----------------------------------------------------------------------------- 
// CLbsPrivLocWaitLocUpdateState::OnTimerEventL
// Description: The Location Update timer has expired.
// Cancel the request, and pass on the response if any has been received,
// otherwise report failure.
// ----------------------------------------------------------------------------- 
//
void CLbsPrivLocFsm::OnTimerEventL(TInt aTimerId)
	{	
	LBSLOG2(ELogP3, "FSM(%d) OnTimerEventL", iSessionId.SessionNum());
	iCurrentState->OnTimerEventL(aTimerId);
	}

/** Called if OnTimerEventL leaves */
TInt CLbsPrivLocFsm::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	__ASSERT_DEBUG(EFalse, Panic(ENrhPanicLocationTimerError));
	return(aError);
	}

/** Callback when a GPS position update arrives from AGPS manager.
*/
void CLbsPrivLocFsm::OnAgpsPositionUpdate(
	TInt aReason,
	const TPositionExtendedSatelliteInfo& aPosInfo,
	const TTime& aTimeStamp)
	{
	LBSLOG2(ELogP3, "FSM(%d) OnAgpsPositionUpdate", iSessionId.SessionNum());
	iCurrentState->OnAgpsPositionUpdate(aReason, aPosInfo, aTimeStamp);
	}

/** Callback when a GPS measurement results update arrives from AGPS manager.
*/
void CLbsPrivLocFsm::OnAgpsMeasurementUpdate(
	TInt aReason,
	const TPositionGpsMeasurementInfo& aPosInfo,
	const TTime& aTimeStamp)
	{
	LBSLOG2(ELogP3, "FSM(%d) OnAgpsMeasurementUpdate", iSessionId.SessionNum());
	iCurrentState->OnAgpsMeasurementUpdate(aReason, aPosInfo, aTimeStamp);
	}
