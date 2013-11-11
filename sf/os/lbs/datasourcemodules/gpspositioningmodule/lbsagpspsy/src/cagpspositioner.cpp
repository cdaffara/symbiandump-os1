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
//

/**
 @file
 @InternalComponent
*/

#include <e32base.h>
#include <lbs/epos_mpositionerstatus.h>
#include <ecom/implementationproxy.h>
#include <lbssatellite.h>
#include <lbs/lbsadmin.h>

#include "cagpspositioner.h"
#include "agpspsy.hrh"
#include "agpspsypanic.h"
#include "cpositionerq.h"
#include "crequesthandler.h"
#include "cagpsenvironment.h"
#include "utilfunctions.h"
#include "tagpsrequestparameters.h"
#include "agpsrequestconstants.h"
#include "psylogging.h"
#include "psypanic.h"

/*
* 
*/
CAgpsPositioner::CAgpsPositioner()
    {
	iState = ERequestStateInitial;
	iTerminalAssisted = EFalse;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CAgpsPositioner::~CAgpsPositioner()
    {
	TRACESTRING("CAgpsPositioner::~CAgpsPositioner")

	if(iEnvironment)
		{
		iEnvironment->UnregisterPSY(this);
		}
	
	iNetRegStatus.Close();
    }

/**
 * Creates a new instance of a CPositioner implementation.
 *
 * @param aConstructionParameters Construction parameters for CPositioner.
 * @return The positioner implementation.
 */
CAgpsPositioner* CAgpsPositioner::NewL(TAny* aConstructionParameters)
    {
    CAgpsPositioner* self = new( ELeave ) CAgpsPositioner;

    CleanupStack::PushL( self );
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }

/**
 * 2nd phase constructor
 *
 * @param aConstructionParameters Construction parameters for CPositioner.
 */
void CAgpsPositioner::ConstructL(TAny* aConstructionParameters)
	{
	TRACESTRING("CAgpsPositioner::ConstructL")

	// Must call BaseConstructL first thing during construction.
	BaseConstructL(aConstructionParameters);

	iEnvironment = CAgpsEnvironment::NewL();
	iEnvironment->RegisterPSYL(this);
	iEnvironment->GetEnvCriteriaL(iCriteria, TUid::Uid(KPosAGPSPSYImplUid));
	iInactivityTimeout = iEnvironment->GetInactivityTimeoutL(TUid::Uid(KPosAGPSPSYImplUid));
	iRequestHandler = iEnvironment->GetRequestHandler();

	// Get the request timeout from the LS:
	if(iEnvironment->ProfileInUse())	// default timeout was taken from profile, so let LS know
		{
		TPositionQuality quality;
		iCriteria.GetRequiredQuality(quality);
		iTimeOutInterval = quality.TimeToFirstFix();	
		SetPsyDefaultUpdateTimeOut(iTimeOutInterval);
		}	    

	iNetRegStatus.OpenL();
	}

/**
* Initiate a tracking session.
*
* @param aInterval Interval for position requests.
*/
void CAgpsPositioner::StartTrackingL(const TTimeIntervalMicroSeconds& aInterval)
    {
	TRACESTRING2("CAdaptationPositioner::StartTrackingL, interval = %d", aInterval)

	TTime timeNow;
    
    TTime trackingTime = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(iTargetTime, aInterval);
	timeNow.UniversalTime();
    
	iTrackingInterval = aInterval;
	    
    if(! (trackingTime < timeNow)) // if this is not a default proxy call to StartTracking
	    {
    	TRACESTRING("Non-DefProxy Start")
	    SetupTrackingRequestL();
        SetAGPSMode();
		iRequestHandler->SubmitNewRequestL(this);
	    }
    }
    
/**
* Requests position info asynchronously. 
*
* @param aPosInfo A reference to a position info object. This object
*                 must be in scope until the request has completed.
*                 The position (TPosition) of this position info object
*                 must be in WGS84 geodetic datum.
* @param aClientStatus The request status
*
*/
void CAgpsPositioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aClientStatus)
    {
    TRACESTRING("CAdaptationPositioner::NotifyPositionUpdate")

	// we're about to submit a new request, so stop warmdown timer if running: 
	StopMaxFixTimer();
    StopWarmDownTimer();
	
    // Clear position info
    ClearPositionInfo(aPosInfo);
    
    aClientStatus = KRequestPending;
    iPosInfo = &aPosInfo;
    iClientStatus = &aClientStatus;
    
    if(!UseLastLocation(aPosInfo)) // Attempt to complete the client immediately with an old position
		{
		TTime timeNow;
		timeNow.UniversalTime();
	    iActive = ETrue;
	
		GetUpdateTimeOut(iTimeOutInterval);

		TDateTime debugTime = timeNow.DateTime();
		TRACESTRING38("Time Now = %d.%d", debugTime.Second(), debugTime.MicroSecond());
		
		// Add on the lag time
		if(iTerminalAssisted)
			{
        	CAgpsEnvironment* agpsEnvironment = static_cast<CAgpsEnvironment*>(iEnvironment);
        	ExtendUpdateTimeOut(agpsEnvironment->GetFinalNetPositionLag());
			}
				
		if( !IsTracking() ||
			(IsTracking() && (iTargetTime <= timeNow)) )
			{
			iTargetTime.UniversalTime();
	        TRACESTRING("CAdaptationPositioner::NotifyPositionUpdate submitting new request")

	        SetAGPSMode();
	        
	        // (re)start maxfix timer so that we know when the module has stopped trying	        
	        TRAPD(error,StartMaxFixTimerL());	 
	        if(!error)
	        	{
	        	TRAP(error, iRequestHandler->SubmitNewRequestL(this));
	        	}
	        if(error)
		        {
				UpdateFailed(error);
		        }
			}
			
		debugTime = iTargetTime.DateTime();
		TRACESTRING38("Target Time = %d.%d", debugTime.Second(), debugTime.MicroSecond());
		}
    }


void CAgpsPositioner::SetModuleId(TPositionInfoBase* aPosInfo)
	{
	if(aPosInfo)
		{
		aPosInfo->SetModuleId(TUid::Uid(KPosAGPSPSYImplUid)); // Use the PSY module Id rather than the AGPS managers
		}
	}

const TRequestParameters* CAgpsPositioner::PositionRequestParams()
	{
	TBool state = (iState == ERequestStateInitial);
	
	iRequestParameters.iTargetTime = iTargetTime;
	iRequestParameters.iTimeForFix = iTimeOutInterval;
	iRequestParameters.iCriteria = iCriteria;
	iRequestParameters.iNewClient = state;
	iRequestParameters.iTracking = IsTracking();
	iRequestParameters.iAGPSMode = iAGPSMode;
	
	return &iRequestParameters;
	}
	
/**
* Checks for a transition into TA mode and extends the max fix time to allow for 
* the final network position arriving  
*
* @param aMode The mode of the last update.
*
*/
void CAgpsPositioner::ModeUpdate(TUint aMode)
	{
    TRACESTRING("CPositionerSubSession::CheckMode()")

	if ( ((aMode & RLbsPositionUpdates::EGpsModeTerminalAssisted) == RLbsPositionUpdates::EGpsModeTerminalAssisted) 
		&& !iTerminalAssisted )
		{
		iTerminalAssisted = ETrue;
		CAgpsEnvironment* agpsEnvironment = static_cast<CAgpsEnvironment*>(iEnvironment);
		ExtendUpdateTimeOut(agpsEnvironment->GetFinalNetPositionLag());
		}
	else if (((aMode & RLbsPositionUpdates::EGpsModeTerminalAssisted) != RLbsPositionUpdates::EGpsModeTerminalAssisted))
		{
		iTerminalAssisted = EFalse;
		}
	}

void CAgpsPositioner::SetAGPSMode()
	{
    TRACESTRING("CPositionerSubSession::GpsMode()")
	
    CLbsAdmin::TGpsMode mode = CLbsAdmin::EGpsModeUnknown;
	iAGPSMode = KAutonomousMode;
	
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus netRegStatus(RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown);
	
	// Read the current network registration status
	TInt err = iNetRegStatus.GetNetworkRegistrationStatus(netRegStatus);

	if (err == KErrNone)
		{
		// Read the admin setting
		switch (netRegStatus)
			{
			case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
				{
				err = iEnvironment->Admin().Get(KLbsSettingHomeGpsMode, mode);
				break;
				}
			case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
			case RLbsNetworkRegistrationStatus::ENotRegistered:
				{
				err = iEnvironment->Admin().Get(KLbsSettingRoamingGpsMode, mode);
				break;
				}		
			case RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown:
			default:
				{
				TRACESTRING("Unrecognised TLbsNetworkRegistrationStatus")
				break;
				}
			}
		
		// Convert to internal mode
		switch (mode)
			{
			case CLbsAdmin::EGpsPreferTerminalBased:
			case CLbsAdmin::EGpsAlwaysTerminalBased:
				iAGPSMode = KTerminalBasedMode;
			break;
			
			case CLbsAdmin::EGpsPreferTerminalAssisted:
			case CLbsAdmin::EGpsAlwaysTerminalAssisted:
				iAGPSMode = KTerminalAssistedMode;
			break;
			
			case CLbsAdmin::EGpsModeUnknown:
				__ASSERT_DEBUG(EFalse,User::Panic(KAdaptationPanicCategory, KErrArgument));
			break;
			
			case CLbsAdmin::EGpsAutonomous:
			default:
				iAGPSMode = KAutonomousMode;
			break;
			}
		}
	}

//  End of File
