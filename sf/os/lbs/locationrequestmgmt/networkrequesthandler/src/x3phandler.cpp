/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <e32base.h>
#include <e32std.h>

// LBS-specific
#include <lbs.h>
#include <lbs/lbsx3p.h>
#include "lbsdevloggermacros.h"

#include "lbsnrhserver.h"
#include "x3phandler.h"
#include "ngmessageswitch.h"


const TInt KReservedRequestsNum = 4;


CX3pHandler::CX3pHandler(CNGMessageSwitch& aMessageSwitch, MX3pStatusHandler& aX3pStatusHandler, CLbsAdmin& aLbsAdmin) : 
	iMessageSwitch(aMessageSwitch),
	iNextSessionId(1),
	iAdmin(aLbsAdmin),
	iReceivedFinalNetPosInfo(EFalse),
	iX3pStatusHandler(aX3pStatusHandler)
    {
	
    }

CX3pHandler* CX3pHandler::NewL(CNGMessageSwitch& aMessageSwitch, MX3pStatusHandler& aX3pStatusHandler, CLbsAdmin& aLbsAdmin)
    {
	CX3pHandler* self;
	self = new (ELeave) CX3pHandler(aMessageSwitch, aX3pStatusHandler, aLbsAdmin);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);	
    }

CX3pHandler::~CX3pHandler()
	{
	iX3pObservers.Close();
	iX3pRequests.ResetAndDestroy();
	}
    
void CX3pHandler::ConstructL()
    {
	iMessageSwitch.RegisterObserver(this);
	TInt err = iAdmin.Get(KLbsSettingBehaviourMode, iLbsBehaviourMode);
	if (err != KErrNone)
		{
		iLbsBehaviourMode = CLbsAdmin::ELbsBehaviourCustom1;
		}	
	
	// get device mode capabilities:
	err = LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, iDeviceGpsModeCaps);
	if(err != KErrNone)
		{
		// Assume module supports hybrid if it has not reported its capabilities in module info file
		iDeviceGpsModeCaps = TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB;
		}
	
	iX3pRequests.ReserveL(KReservedRequestsNum);
	iX3pObservers.ReserveL(KReservedRequestsNum);
	
    iActiveRequestIndex = KErrNotFound;
    
    UpdateX3pStatus();
    }

void CX3pHandler::TransmitLocationRequestL(const TDesC& aDestinationID,
										   TUint aTransmitPriority, 
										   const TLbsTransmitPositionOptions& aTransmitOptions,
										   TLbsNetSessionIdInt& aSessionId)
	{
	LBSLOG(ELogP1, "CX3pHandler::TransmitLocationRequestL:");
	
	// Generate a new session Id
	TLbsNetSessionIdInt sessionId(KLbsNetRequestHandlerUid, iNextSessionId++);

    // Create an instance for this request.
    CX3pRequest* newRequest = 0;
	newRequest = CX3pRequest::NewL(this, sessionId,
								   aDestinationID,
								   aTransmitPriority,
								   aTransmitOptions);
	CleanupStack::PushL(newRequest);
	iX3pRequests.AppendL(newRequest);
	CleanupStack::Pop(newRequest);
	
	// Write the session Id back to the caller
	aSessionId = sessionId;

    //
    // The timeout (if specified) runs from the time the request was made, so 
    // start the request timer now, even if we're not going to action the 
    // request yet
    //
	newRequest->StartTimer();
	
	//
	// The new request has been added. Now see if there is a request being
	// actioned at the moment. If not, start this one
	//
	TInt newRequestIndex = iX3pRequests.Count() - 1;
	TInt currentRequestIndex = iActiveRequestIndex;
	if(iActiveRequestIndex == KErrNotFound)
	    {
		currentRequestIndex = newRequestIndex;
	    }
	else
	    {
        // 
        // If there is an active request, see if its priority is lower than 
        // the new one. If so, action the new one.
        // The Network Gateway handles the necessary cancellations.
        //
        if(iActiveRequest->TransmitPriority() > aTransmitPriority)
            {
            LBSLOG3(ELogP2, "New X3P (priority %d) overrides currently active X3P (priority %d)", 
            				aTransmitPriority, 
            				iActiveRequest->TransmitPriority());
            				
		    currentRequestIndex = newRequestIndex;
            }
	    }
	if(currentRequestIndex != iActiveRequestIndex)
	    {
	    iActiveRequestIndex = currentRequestIndex;
		ActivateRequest();
	    }
	}

void CX3pHandler::TransmitLocationCancel(const TLbsNetSessionIdInt& aSessionId)
	{
	LBSLOG(ELogP1, "CX3pHandler::TransmitLocationCancel:");

	// There is a chance that we get a 'cancel' for an X3P request
	// that has already finished normally, so first check that
	// the sessionId is one that is currently active.
	TInt requestIndex = FindSessionById(aSessionId);
	if(requestIndex != KErrNotFound)
		{
		// Forward the cancel message onto the network
		iMessageSwitch.SendX3pCancel(aSessionId, KErrCancel);
		}
	}
    
void CX3pHandler::ActivateRequest()	    
	{
	if(iActiveRequestIndex != KErrNotFound)
		{
		iActiveRequest = iX3pRequests[iActiveRequestIndex];
		
		// Get the technology type to use for this request from the admin settings.
		TPositionModuleInfo::TTechnologyType mode;
		GetAdminTechnologyType(mode);
		TLbsNetPosRequestOptionsTechnologyInt options;
		options.SetPosMode(mode);
		
		iMessageSwitch.SendX3pRequest(iActiveRequest->SessionId(),
									  iActiveRequest->DestinationId(),
									  iActiveRequest->TransmitPriority(),
									  options);
		
		// Reset the final network position flag
		iReceivedFinalNetPosInfo = EFalse;
		}
	}

/* Called when a pending request has timed out before it was completed.
 */
void CX3pHandler::OnRequestTimeout(const TLbsNetSessionIdInt& aTimedOutRequestId)
    {
	if(aTimedOutRequestId.SessionNum() != 0)
		{
		HandleTimedOutRequest(aTimedOutRequestId);
		}
    }

/*
*/
TInt CX3pHandler::HighestPriorityIndex()
    {
    TInt index = KErrNotFound;
    const TInt count = iX3pRequests.Count();
    if (count > 0)
    	{
    	CX3pRequest* highPriorityReq = iX3pRequests[0];
    	index = 0;
    	for (TInt i = 1; i < count; i++)
    		{
    		CX3pRequest* request = iX3pRequests[i];
    		if (request->TransmitPriority() > 
    			highPriorityReq->TransmitPriority())
    			{
    			index = i;
    			highPriorityReq = request;
    			}
    		}
   		}

	return (index);
	}


/* Look through the array of requests to find the one with a session Id
   equal to the input aSessionId.
   
   Set the reference parameter aSessionIndex to the array index of the
   request, and return a pointer to the request.
   If no request is found with the specified aSessionId, then return NULL.
*/
TInt CX3pHandler::FindSessionById(const TLbsNetSessionIdInt& aSessionId)
    {
    TInt retVal = KErrNotFound;
	TInt requestCount = iX3pRequests.Count();
	for (TInt i = 0; i < requestCount; ++i)
	    {
		if(iX3pRequests[i]->SessionId() == aSessionId)
		    {
		    retVal = i;
		    break;
		    }
	    }
	return(retVal);    
    }

/* Search the array following a delete to find the index
   of the currently active request, looking for it by its session ID.
*/
void CX3pHandler::RecalculateActiveIndex(const TLbsNetSessionIdInt& aSessionId)
    {
	iActiveRequestIndex = FindSessionById(aSessionId);
    }

void CX3pHandler::HandleTimedOutRequest(const TLbsNetSessionIdInt& aRequestId)
    {
	// Send a cancel message to the network,
	//  with KErrTimedOut as the reason
	iMessageSwitch.SendX3pCancel(aRequestId, KErrTimedOut);	
    }


void CX3pHandler::HandleCompletedRequest(const TLbsNetSessionIdInt& aRequestId, TInt aReason)
    {
    TInt err = aReason;
    TBool isTerminalAssistedPos = EFalse;
    //
    // There's a possibility this request ID won't be found, if a Transmit
    // Cancel was received after the request had been sent off. If this
    // is the case, just ignore the completion.
    //
	TInt requestIndex = FindSessionById(aRequestId);
	if(requestIndex != KErrNotFound)
		{
		if (!iReceivedFinalNetPosInfo)
			{
			TInt ret = iMessageSwitch.GetNetworkFinalPosition(aRequestId, iFinalNetPosInfo);
			if (err == KErrNone && ret != KErrNone)
				{
				err = ret;
				}
			}

		// Add the ID of the module supplying the update. This is either 
		// the GPS Location Manager or the Network Location Manager (if 
		// the Position Mode is not one of those below, leave the Module Id
		// unchanged).
		TPositionModuleInfo::TTechnologyType posMode = iFinalNetPosInfo.PositionMode();
		if(posMode == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted))
			{
			iFinalNetPosInfo.SetModuleId(KLbsGpsLocManagerUid);
			}
		else
			if(posMode == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted))
				{
				iFinalNetPosInfo.SetModuleId(KLbsNetLocManagerUid);
				isTerminalAssistedPos = ETrue;
				}

		// It may be that the network has modified the position update, 
		// so we need to check that it satisfies the quality criteria and report
		// it if not.
		if(aReason == KErrNone) // network thinks it's OK
			{
			if(!iX3pRequests[requestIndex]->FixIsAccurate(iFinalNetPosInfo, isTerminalAssistedPos))
				{
				err = KPositionQualityLoss;
				}
			}
				
		// Notify observers of the completed session
		const TInt count = iX3pObservers.Count();
		for (TInt i = 0; i < count; i++)
			{
			iX3pObservers[i]->OnTransmitLocationComplete(
									aRequestId, 
									iFinalNetPosInfo,
									err);
			}
		
		// Remove the request from the list
		delete iX3pRequests[requestIndex];
		iX3pRequests.Remove(requestIndex);

		// If the completed request was being actioned, start the next
		// one if there is one.
		if(requestIndex == iActiveRequestIndex)
			{
			iActiveRequestIndex = HighestPriorityIndex();
			if(iActiveRequestIndex != KErrNotFound)
				{
				ActivateRequest();
				}
		}
		else
			{
			// The array has been changed by the removal of an entry.
			// Make sure iActiveRequestIndex is correct for the active
			// entry.
			TLbsNetSessionIdInt activeSessionId = iActiveRequest->SessionId();
			RecalculateActiveIndex(activeSessionId);
			}    
		}
	
 	// Tell the AGPS interface handler the current state
 	// of the X3P handler.
 	UpdateX3pStatus();
	}

/*
*/
void CX3pHandler::OnMTLRRequest(const TLbsNetSessionIdInt& /*SessionId*/,
				  TLbsNetworkEnumInt::TLbsNetProtocolServiceInt  /*aSessionType*/, 
				   TBool /*aIsEmergency*/,
				   const TLbsExternalRequestInfo& /*aExternalRequestInfo*/,
				   const TLbsNetPosRequestPrivacyInt& /*aNetPosRequestPrivacy*/)
	{
	// We do not need to know about MT-LR requests so ignore this call.
	}

void CX3pHandler::OnNetLocResponse(const TLbsNetSessionIdInt& aSessionId,
								 const TLbsNetPosRequestQualityInt& aQuality)
	{
	// Check if this Network Location Response
	// is for a X3P that is currently in progress
	TInt requestIndex = FindSessionById(aSessionId);
	if (requestIndex != KErrNotFound)
		{
		iX3pRequests[requestIndex]->SetRequestQuality(aQuality);
		}
	}

/*
*/
void CX3pHandler::OnSessionComplete(const TLbsNetSessionIdInt& aSessionId, TInt aReason)
	{
	// Check if this Network Location Response
	// is for a X3P that is currently in progress
	TInt requestIndex = FindSessionById(aSessionId);
	if (requestIndex != KErrNotFound)
		{
		HandleCompletedRequest(aSessionId, aReason);
		}
	}

/* Called during a TransmitLocation request, at the point where the network provides the
handset with a network-based Reference Location.

The Reference Location (aRefPosInfo) will be sent back to the client's session, and the session
will forward it back to the client if the client wants it.

Values of aIsEmengency and aQuality are not relevant for the CX3pHandler (they are for
other observers)
*/
void CX3pHandler::OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId,
					const TLbsNetPosRequestMethodInt& aNetPosMethod,
					 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt /*aSessionType*/, 
					 TBool /*aIsEmergency*/,
					 const TLbsNetPosRequestQualityInt& /*aQuality*/)
	{
	TBool hybridMode = EFalse;
	TLbsNetPosMethodInt posMethod;
	
	const TPositionModuleInfo::TTechnologyType KTerminalAssistedMode = (
				TPositionModuleInfo::ETechnologyNetwork |
				TPositionModuleInfo::ETechnologyAssisted);
				
	// Does the hardware support hybrid mode?
	if(iDeviceGpsModeCaps & TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB )
		{
		TInt methodCount = aNetPosMethod.NumPosMethods();
		for(TInt i = 0; (i < methodCount) && !hybridMode; ++i)
			{
			aNetPosMethod.GetPosMethod(i, posMethod);
			hybridMode = (posMethod.PosMode() & KTerminalAssistedMode) == KTerminalAssistedMode;
			}
		}
	
	// Custom1: If the hybrid mode and a first request, then re-start the timer
	if (hybridMode 
		&& (aSessionId.SessionOwner() == KLbsNetRequestHandlerUid) 
		&& (iLbsBehaviourMode == CLbsAdmin::ELbsBehaviourCustom1))
		{
		TInt requestIndex = FindSessionById(aSessionId);
		if(requestIndex != KErrNotFound)
			{
			// The timer can be re-started only once
			iX3pRequests[requestIndex]->ReStartTimerOnce();
	   		}
	   	}
	}

/** Called when a reference position arrives from the network.
*/
void CX3pHandler::OnNetLocReferenceUpdate(const TLbsNetSessionIdInt& aSessionId, 
								 		  const TPositionInfoBase& aPosInfo)
	{
    // There's a possibility this request ID won't be found, if a Transmit
    // Cancel was received after the request had been sent off. If this
    // is the case, just ignore the completion.
	if (aSessionId.SessionOwner() == KLbsNetRequestHandlerUid)
		{
		iRefPosInfo = static_cast<const TPositionInfo&>(aPosInfo);

		TInt requestIndex = FindSessionById(aSessionId);
		if(requestIndex != KErrNotFound)
			{
			iRefPosInfo.SetModuleId(KLbsGpsLocManagerUid);
			iRefPosInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);
			iRefPosInfo.SetPositionModeReason(EPositionModeReasonNone);
			iRefPosInfo.SetUpdateType(EPositionUpdateGeneral);
		
	   		// Notify observers of the completed session
	   		const TInt count = iX3pObservers.Count();
	   		for (TInt i = 0; i < count; i++)
	   			{
	   			iX3pObservers[i]->OnReferenceLocationAvailable(
	   									aSessionId, 
	  									iRefPosInfo);
	   			}
	   		}
	   	}	
	}

/** Callend when a final location arrives from the network.
*/
void CX3pHandler::OnNetLocFinalUpdate(
		const TLbsNetSessionIdInt& aSessionId, 
		const TPositionInfoBase& aPosInfo)
	{
	// If the final network position is for an active X3P, 
	// then store it for when the X3P is completed.
	TInt requestIndex = FindSessionById(aSessionId);
	if (requestIndex != KErrNotFound)
		{
		if (requestIndex == iActiveRequestIndex)
			{
			iReceivedFinalNetPosInfo = ETrue;
			iFinalNetPosInfo = static_cast<const TPositionInfo&>(aPosInfo);
			}
		}
	}

TInt CX3pHandler::OnTransmitPosition(const TDesC& aDestinationId,
									 TUint aTransmitPriority,
									 const TLbsTransmitPositionOptions& aTransmitOptions,
									 TLbsNetSessionIdInt& aSessionId)
	{
	TInt err(KErrNone);
	TRAP(err, TransmitLocationRequestL(aDestinationId, aTransmitPriority,
									   aTransmitOptions, aSessionId));
	
 	// Tell the AGPS interface handler the current state
 	// of the X3P handler.
 	UpdateX3pStatus();
	
	return err;
	}
	
void CX3pHandler::OnCancelTransmitPosition(const TLbsNetSessionIdInt& aSessionId)
	{
	TransmitLocationCancel(aSessionId);
	}

/* Add an observer (actually a client X3P subsession)
*/
void CX3pHandler::AddObserverL(MX3pHandlerObserver* aObserver)
 	{
 	iX3pObservers.AppendL(aObserver);
 	
 	// Tell the AGPS interface handler the current state
 	// of the X3P handler.
 	UpdateX3pStatus();
 	}

/* Remove an observer (actually a client X3P subsession)
*/
void CX3pHandler::RemoveObserver(MX3pHandlerObserver* aObserver)
 	{
 	TInt index = iX3pObservers.Find(aObserver);
 	if (index != KErrNotFound)
 		{
 		iX3pObservers.Remove(index);
 		}
 
 	// Tell the AGPS interface handler the current state
 	// of the X3P handler.
 	UpdateX3pStatus();
 	}

/** Send the current X3P status to the status handler - for power mode advice.
*/
void CX3pHandler::UpdateX3pStatus()
	{
	
	MX3pStatusHandler::TX3pStatus status(MX3pStatusHandler::EX3pStatusIdle);
	if (iActiveRequestIndex != KErrNotFound)
		{
		// There is a currently active request, so we are 'active'
		status = MX3pStatusHandler::EX3pStatusActive;
		}
	else
		{
	 	if (iX3pObservers.Count() >= 1)
	 		{
		 	// If there is one or more observer (i.e. session) in the array,
		 	// then set the X3P status to 'connected', in case the GPS hardware
		 	// needs to be informed to go into a warm-up state.
	 		status = MX3pStatusHandler::EX3pStatusConnected;
	 		}
	 	else if (iX3pObservers.Count() == 0)
	 		{
	 	 	// If there are no observers (i.e. sessions), then tell
	 	 	// the X3P status handler that we are 'idle', since there are no
	 	 	// outstanding X3P requests or sessions connected.
	 		status = MX3pStatusHandler::EX3pStatusIdle;
	 		}
		}
	// Update the X3P status. 
	iX3pStatusHandler.SetX3pStatus(status);
	}

void CX3pHandler::GetAdminTechnologyType(TPositionModuleInfo::TTechnologyType& aMode)
	{
	aMode = TPositionModuleInfo::ETechnologyUnknown;
	
	// Determine the current technology type from the admin settings
	RLbsNetworkRegistrationStatus lbsNetRegStatus;
	TRAPD(err, lbsNetRegStatus.OpenL());
	if(err != KErrNone)
	    {
        // Netreg status could not be determined mode will be ETechnologyUnknown
        return;
	    }
    RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus netRegStatus;
    err = lbsNetRegStatus.GetNetworkRegistrationStatus(netRegStatus);
	lbsNetRegStatus.Close();			    
    if (err == KErrNone)
    	{
    	TLbsAdminSetting setting;
    	switch (netRegStatus)
    		{
    		case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
    			setting = KLbsSettingHomeGpsMode;
    			break;
    		case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
    			setting = KLbsSettingRoamingGpsMode;
				break;
    		default:
    			// default to using the roaming setting
    			LBSLOG_WARN2(ELogP3, 
    						 "CX3pHandler::GetAdminTechnologyType: unkown network registration status (%d), defaulting to roaming.", 
    						 netRegStatus);
    			setting = KLbsSettingRoamingGpsMode;
    			break;
    		}
    	
		CLbsAdmin::TGpsMode gpsMode;
		err = iAdmin.Get(setting, gpsMode);
		if (err == KErrNone)
			{
			switch (gpsMode)
				{
				case CLbsAdmin::EGpsModeUnknown:
					aMode = (TPositionModuleInfo::ETechnologyUnknown);
					break;
					
				case CLbsAdmin::EGpsPreferTerminalBased:
					aMode = (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
					break;
					
				case CLbsAdmin::EGpsAlwaysTerminalBased:
					aMode = (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
					break;
					
				case CLbsAdmin::EGpsAutonomous:
					aMode = (TPositionModuleInfo::ETechnologyTerminal);
					break;
					
				case CLbsAdmin::EGpsPreferTerminalAssisted:
					aMode = (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
					break;
					
				case CLbsAdmin::EGpsAlwaysTerminalAssisted:
					aMode = (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
					break;
				
				default:
					aMode = (TPositionModuleInfo::ETechnologyUnknown);
					 			// default to using the roaming setting
    				LBSLOG_WARN2(ELogP3, 
    						 "CX3pHandler::GetAdminTechnologyType: unkown TGpsMode (%d), defaulting to TPositionModuleInfo::ETechnologyUnknown.", 
    						 gpsMode);
					break;
				}
			}
	   	}
	}


