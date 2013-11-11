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

#include "cnetpositioner.h"
#include "netpsy.hrh"
#include "psypanic.h"
#include "crequesthandler.h"
#include "cnetenvironment.h"
#include "utilfunctions.h"
#include "psylogging.h"

static const TImplementationProxy KFactoryPtr = 
IMPLEMENTATION_PROXY_ENTRY( KPosNETPSYImplUid, CNETPositioner::NewL);

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount =1;
    return &KFactoryPtr;
    }


/*
* 
*/
CNETPositioner::CNETPositioner()
    {
	iState = ERequestStateInitial;
	iCriteria.ResetCriteria();
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CNETPositioner::~CNETPositioner()
    {
	TRACESTRING("CNETPositioner::~CNETPositioner")

	iEnvironment->UnregisterPSY(this);
    }

/**
 * Creates a new instance of a CPositioner implementation.
 *
 * @param aConstructionParameters Construction parameters for CPositioner.
 * @return The positioner implementation.
 */
CNETPositioner* CNETPositioner::NewL(TAny* aConstructionParameters)
    {
    CNETPositioner* self = new( ELeave ) CNETPositioner;

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
void CNETPositioner::ConstructL(TAny* aConstructionParameters)
    {
//    CLEARTRACELOG;
    TRACESTRING("CNETPositioner::ConstructL")
    
    // Must call BaseConstructL first thing during construction.
    BaseConstructL(aConstructionParameters);
    
	iEnvironment = CNETEnvironment::NewL();
	iEnvironment->RegisterPSYL(this);
    iEnvironment->GetEnvCriteriaL(iCriteria, TUid::Uid(KPosNETPSYImplUid));
	iInactivityTimeout = iEnvironment->GetInactivityTimeoutL(TUid::Uid(KPosNETPSYImplUid));
    iRequestHandler = iEnvironment->GetRequestHandler();
    
	// Get the request timeout from the LS:
    if(iEnvironment->ProfileInUse())	// default timeout was taken from profile, so let LS know
    	{
	    TPositionQuality quality;
	    iCriteria.GetRequiredQuality(quality);
    	iTimeOutInterval = quality.TimeToFirstFix();	
    	SetPsyDefaultUpdateTimeOut(iTimeOutInterval);
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
void CNETPositioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aClientStatus)
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
    
    if(!UseLastLocation(aPosInfo))
		{
		TTime timeNow;
		timeNow.UniversalTime();
	    iActive = ETrue;
	
		GetUpdateTimeOut(iTimeOutInterval);

		TDateTime debugTime = timeNow.DateTime();
		TRACESTRING38("Time Now = %d.%d", debugTime.Second(), debugTime.MicroSecond());
		
		if( !IsTracking()  ||
			(IsTracking() && (iTargetTime <= timeNow)) )
			{
			iTargetTime.UniversalTime();
	        TRACESTRING("CAdaptationPositioner::NotifyPositionUpdate submitting new request")

	        TInt error;
	        // (re)start maxfix timer so that we know when the module has stopped trying	        
	        TRAP(error,StartMaxFixTimerL());	 
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

void CNETPositioner::SetModuleId(TPositionInfoBase* aPosInfo)
	{
	if(aPosInfo)
		{
		aPosInfo->SetModuleId(TUid::Uid(KPosNETPSYImplUid)); // Use the PSY module Id rather than the AGPS managers
		}
	}

const TRequestParameters* CNETPositioner::PositionRequestParams()
	{
	TBool state = (iState == ERequestStateInitial);
	TBool tracking = IsTracking();
	
	iRequestParameters.iTargetTime = iTargetTime;
	iRequestParameters.iTimeForFix = iTimeOutInterval;
	iRequestParameters.iCriteria = iCriteria;
	iRequestParameters.iNewClient = state;
	iRequestParameters.iTracking = tracking;
	
	return &iRequestParameters;
	}
	
//  End of File

