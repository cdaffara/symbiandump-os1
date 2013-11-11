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
#include "utilfunctions.h"

#include "crequesthandler.h"
#include "cresponsehandler.h"
#include "cadaptationpositioner.h"
#include "psypanic.h"
#include "psylogging.h"
#include "lbsprocessuiddefs.h"
#include "lbsdevloggermacros.h"

const TUint KWarmDownTimeout   = 10000000;//10s

CAdaptationPositioner::CAdaptationPositioner()
    {
    LBSLOG(ELogP1, "CAdaptationPositioner::CAdaptationPositioner()");
    iState = ERequestStateInitial;
    }

/**
* Destructor
*/
CAdaptationPositioner::~CAdaptationPositioner()
    {
    LBSLOG(ELogP1, "CAdaptationPositioner::~CAdaptationPositioner()");
    
	StopMaxFixTimer();
	delete iMaxFixTimer;
	iMaxFixTimer = NULL;
	StopWarmDownTimer();    
	delete iWarmDownTimer;
	iWarmDownTimer = NULL;
    
	CancelNotifyPositionUpdate();
    
    if(iEnvironment)
    	{
    	iEnvironment->CloseInstance();
    	}
	}

/**
* CancelNotifyPositionUpdate implements CAdaptationPositioner::CancelNotifyPositionUpdate
* Cancels position info request.
*/
void CAdaptationPositioner::CancelNotifyPositionUpdate()
    {
    LBSLOG(ELogP1, "CAdaptationPositioner::CancelNotifyPositionUpdate()");
    // Stop timers
	StopMaxFixTimer();
	StopWarmDownTimer();    
    CancelNotifyPositionUpdate(KErrCancel);
    }

/**
* CancelNotifyPositionUpdate implements CAdaptationPositioner::CancelNotifyPositionUpdate(TInt)
* Cancels position info request with reason. This reason allows the S60 Loc Server to distinguish between a real user cancel
* and a timeout - in the case of the latter the LS calls CancelNotifyPositionUpdate(KErrTimedOut), 
* in the former he passes no error.
*/
void CAdaptationPositioner::CancelNotifyPositionUpdate(TInt aError)
    {
    LBSLOG2(ELogP1, "CAdaptationPositioner::CancelNotifyPositionUpdate(aError) %d", aError);

    if( iActive )
        {
        LBSLOG2(ELogP1, "CAdaptationPositioner::Complete request error = %d", aError);

		UpdateFailed(KErrCancel);
        TRAP_IGNORE(iRequestHandler->ReMergeWithCancelRequestL(aError)); // Nothing we can do about a leave here
        if(aError == KErrTimedOut && IsTracking())
        	{
        	// The last tracking request timedout so we need to schedule one for the next interval
    		TTime timeNow;
    		timeNow.UniversalTime();   
    		iActive = ETrue;
        	UpdateTrackingTime(timeNow);
        	TRAPD(error, iRequestHandler->SubmitNewRequestL(this)); // Nothing we can do about a leave here
            if(!error)
                {
                TRAP_IGNORE(StartMaxFixTimerL());// (re)start maxfix timer so that we know when the module has stopped trying
                } 	
        	}
    	}
    }


/**
* Indicate if the PSY has overridden tracking. The default
* implementation returns EFalse.
* @return ETrue if PSY has own timer, otherwise EFalse.
*/
TBool CAdaptationPositioner::TrackingOverridden() const
    {
    return ETrue;
    }


/**
* Initiate a tracking session.
*
* @param aInterval Interval for position requests.
*/
void CAdaptationPositioner::StartTrackingL(const TTimeIntervalMicroSeconds& aInterval)
    {
	LBSLOG2(ELogP1, "CAdaptationPositioner::StartTrackingL, interval = %d", aInterval);

	TTime timeNow;
    
    TTime trackingTime = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(iTargetTime, aInterval);
	timeNow.UniversalTime();
    
	iTrackingInterval = aInterval;
	    
    if(! (trackingTime < timeNow)) // if this is not a default proxy call to StartTracking
	    {
    	LBSLOG(ELogP1, "Non-DefProxy Start");
	    SetupTrackingRequestL();
		iRequestHandler->SubmitNewRequestL(this);
	    }
    }
    
/**
* Stop a periodic update session.
*/
void CAdaptationPositioner::StopTracking()
    {
    LBSLOG(ELogP1, "CAdaptationPositioner::StopTracking");
    
    iTrackingInterval = 0;
    }

/**
* Restart tracking after a missed update
*/
void CAdaptationPositioner::UpdateTrackingTime(TTime& aTimeNow)
	{
	LBSLOG(ELogP1, "CAdaptationPositioner::UpdateTrackingTime restarting tracking");
	
	TUint multiplier = ( (aTimeNow.Int64() - iTargetTime.Int64()) / iTrackingInterval.Int64() );
	TTimeIntervalMicroSeconds timeToAdd = ( (multiplier + 1) * iTrackingInterval.Int64() );
	
	if (timeToAdd < 0)
		{
		iTargetTime = Time::MaxTTime();
		}
	else
		{
		iTargetTime = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(iTargetTime, timeToAdd);
		}
	}

/**
* Submit a request with the target time in the future.
* The positionerQ will deal with issuing a merged request from 
* all active positioners.
*/
void CAdaptationPositioner::SetupTrackingRequestL()
	{
    if(IsTracking())
    	{
    	LBSLOG(ELogP1, "CAdaptationPositioner::SetupTrackingRequestL");
    	
	    iPosInfo = NULL; // This will be filled in by the NPUD when it arrives
	    iActive = ETrue;
	    
	    iTargetTime.UniversalTime();

		#ifdef _DEBUG
	    TDateTime timeNow = iTargetTime.DateTime();
		LBSLOG3(ELogP1, "Time Now = %d.%d", timeNow.Second(), timeNow.MicroSecond());
		#endif
		
		iTargetTime = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(iTargetTime, iTrackingInterval);

		#ifdef _DEBUG
		timeNow = iTargetTime.DateTime();
		LBSLOG3(ELogP1, "Target Time = %d.%d", timeNow.Second(), timeNow.MicroSecond());
		#endif
		
		StartMaxFixTimerL();
    	}
	}
	
/**
* Check if we're tracking.
*
* @return ETrue if we are tracking
*/
TBool CAdaptationPositioner::IsTracking()
	{
	return (iTrackingInterval != 0);
	}
	
/**
* Attempt to use an old position
*
* @param aPosInfo The clients position info
* @return ETrue if the old position was used
*/
TBool CAdaptationPositioner::UseLastLocation(TPositionInfoBase& aPosInfo)
	{
    TTime maxAge;
	TBool result = EFalse;

    GetMaxAge(maxAge);

    if(maxAge > 0)
    	{
    	CResponseHandler* responseHandler = iEnvironment->GetResponseHandler(); 
    	
    	result = responseHandler->GetLastPosition(aPosInfo, maxAge, IsPartialUpdateAllowed());
    	
	    if( result ) // Use last location
	        {
	        TInt err = KErrNone;
	  	 	TPosition pos;
	  	 	static_cast<TPositionInfo&>(aPosInfo).GetPosition(pos);
	        if(Partial(pos))
	        	{
	        	err = KPositionPartialUpdate;
	        	}
	        LBSLOG(ELogP1, "CAdaptationPositioner::NotifyPositionUpdate returning old position");
	        // nb: this will deal with tracking requests properly:
	        if(CompleteRequest(static_cast<TPositionInfo&>(aPosInfo), err))
	        	{
	            TRAP_IGNORE(SetupTrackingRequestL());
	        	}
	        }
        }

	return result;
	}

/**
* Attempt to complete an outstanding request if there is one
*
* @param aPositionInfo The retrieved fix.
* @param aError The error code if the request is failed.
* @param aActualTime time of the location update
*/
void CAdaptationPositioner::RequestCompleteNotify(const TPositionInfo& aPositionInfo, TInt aError, TTime& aActualTime)
    {
    LBSLOG(ELogP1, "CAdaptationPositioner::RequestCompleteNotify start...");
    
    TBool startWarmDown = EFalse;
    TBool completed = EFalse;
    
    if(iActive)
	    {
	    TPositionModuleInfo::TTechnologyType mode = aPositionInfo.PositionMode();
	    
	    if( (aActualTime >= iTargetTime) || (aError == KPositionCalculationFutile) )
	       	{
        	// Check if the update is partial and if partial updates are allowed
    	 	TPosition pos;
			aPositionInfo.GetPosition(pos);

        	TBool partial = Partial(pos);
        	
			TBool isFNP = (aPositionInfo.PositionMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
			
			if(isFNP)
				{
        		LBSLOG(ELogP1, "Complete with FNP");
        		/* removing this for now until we decide how to deal with the qualityloss issue on s60 
        		 * since on s60 qualityloss means no position was generated
        		 	if(partial)
        			{
        			CompleteRequest(aPositionInfo, KPositionQualityLoss);
        			}
        		
        		else
        		*/
        			{
        			completed = CompleteRequest(aPositionInfo, KErrNone);
        			
        			// Stop the AGPS Manager if there are no other outstanding requests
        	        TRAP_IGNORE(iRequestHandler->CancelRequestL());
        			}
				}
			else if(partial && IsPartialUpdateAllowed())
	        	{
        		LBSLOG(ELogP1, "Complete with partial");
        		completed = CompleteRequest(aPositionInfo, KPositionPartialUpdate);
        		startWarmDown = ETrue;
	        	}
	        else if(!partial)
		        {
    			LBSLOG(ELogP1, "Complete with non-partial");
    			completed = CompleteRequest(aPositionInfo, KErrNone);
    			
        		// did the module produce an accurate update or is it still trying?
        		if(!IsAccurate(aPositionInfo))
        			{
        			startWarmDown = ETrue;         			
        			}
        		else	// it might be an accurate reference position
        			{
        			TBool isRefPos = (aPositionInfo.ModuleId() == KLbsGpsLocManagerUid) && 
        								(aPositionInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
        			if(isRefPos)
        				{
        				startWarmDown = ETrue;
        				}
        			}
	        	}
	        else if(aError == KPositionCalculationFutile)
	        	{
	        	// if autonomous return immediately otherwise wait for FNP
	        	if(mode & TPositionModuleInfo::ETechnologyTerminal)
		        	{
	        		LBSLOG(ELogP1, "Complete with futile");
	        		completed = CompleteRequest(aPositionInfo, KPositionQualityLoss);
		        	}
	        	}
	        else
	        	{
		        // position is not futile and is partial but partials are not
		        // allowed. In this case do nothing and wait for further updates
	        	}
        	}
		else
			{
        	LBSLOG(ELogP1, "CAdaptationPositioner::RequestCompleteNotify Ignoring early response");
			}
    	}
    
    if (completed)
    	{
        TRAP_IGNORE(SetupTrackingRequestL());
    	}
    
    if(startWarmDown)	// we need to start a warmdown timer
    	{
    	TRAP_IGNORE(StartWarmDownTimerL()); // not much we can do here if warmdown timer not started
    	}
    LBSLOG(ELogP1, "CAdaptationPositioner::RequestCompleteNotify end");
    }
    
/**
* Attempt to complete an outstanding request if there is one with an error
*
* @param aError The error code if the request is failed.
*/
void CAdaptationPositioner::UpdateFailed(TInt aError)
	{
	LBSLOG(ELogP1,  "CAdaptationPositioner::UpdateFailed()");
	if(iActive)
	    {
		LBSLOG2(ELogP1, "CAdaptationPositioner::Complete request error = %d", aError);

		iActive = EFalse;
		ClearPositionInfo(*iPosInfo);
		
		if(iClientStatus)
			{
			User::RequestComplete(iClientStatus, aError);
			iClientStatus = NULL;
			}
		LBSLOG2(ELogP1, "iState -> ERequestStatePostInitial from %d", iState);
	    iState = ERequestStatePostInitial;
	    }
	}

/**
* Complete the outstanding request and reset internal state
*
* @param aPositionInfo The retrieved fix.
* @param aError The error code if the request is failed.
*/
TBool CAdaptationPositioner::CompleteRequest(const TPositionInfo& aPositionInfo, TInt aError)
	{
	LBSLOG2(ELogP1, "CAdaptationPositioner::CompleteRequest() with error %d", aError);
	TBool completed = EFalse;
	iActive = EFalse; // Stop another request being sent for this instance of the PSY

	if(iPosInfo)
		{
		TInt error = CopyPositionTypes(*iPosInfo, aPositionInfo);
		__ASSERT_DEBUG(error == KErrNone,
						User::Panic(KAdaptationPanicCategory, EPanicPositionCopyFailed));
		error = error; // this is to stop arm build warnings

		SetModuleId(iPosInfo); // Delegate to the derived class
		User::RequestComplete(iClientStatus, aError);
		iClientStatus = NULL;
		LBSLOG2(ELogP1, "iState -> ERequestStatePostInitial from %d", iState);
	    iState = ERequestStatePostInitial;
        completed = ETrue;
		}
	else
		{
    	LBSLOG(ELogP1, "CAdaptationPositioner::RequestCompleteNotify iPosInfo not set");
		}
	
	return completed;
	}

/**
* CAdaptationPositioner::ReportStatus
* Reports the PSY status to the MLFW
*
* @param aStatus The new position module status
*/
void CAdaptationPositioner::ReportStatus(const TPositionModuleStatus& aStatus)
    {
    MPositionerStatus* statusInf = this->PositionerStatus();
    statusInf->ReportStatus(aStatus);
    }

/**
 * Clears the fields of the aPosInfo 
 * @param aPosInfo The position information to return
 */
void CAdaptationPositioner::ClearPositionInfo(TPositionInfoBase& aPosInfo)
    {
    if(&aPosInfo != NULL)
	    {
		if (aPosInfo.PositionClassType() & EPositionSatelliteInfoClass)
	        {
	        // TPositionSatelliteInfo
	        (void) new (&aPosInfo) (TPositionSatelliteInfo);
	        }
	    else if (aPosInfo.PositionClassType() & EPositionCourseInfoClass)
	        {
	        // TPositionCourseInfo
	        (void) new (&aPosInfo) (TPositionCourseInfo);
	        }
	    else if (aPosInfo.PositionClassType() & EPositionGenericInfoClass)
	        {
	        // HPositionGenericInfo
	        HPositionGenericInfo* genInfo =
	            static_cast<HPositionGenericInfo*> ( &aPosInfo );
	        
	        genInfo->ClearPositionData();
	        }
	    else if (aPosInfo.PositionClassType() & EPositionInfoClass)
	        {
	        // TPositionInfo
	        (void) new (&aPosInfo) (TPositionInfo);
	        }
	    else
	        {
	        // Unknown type, this should never happen
	        // --> Panic if we get here
			__ASSERT_DEBUG(0, User::Panic(KAdaptationPanicCategory, EPanicUnknownPositioningClass));
	        }

	    aPosInfo.SetModuleId(ImplementationUid());
	    }
    }


TBool CAdaptationPositioner::IsActive()
	{
	return iActive;
	}

TBool CAdaptationPositioner::IsWarmingDown()
	{
	if(iWarmDownTimer)
		{
		return iWarmDownTimer->IsActive();
		}
	return EFalse;
	}

void CAdaptationPositioner::StartMaxFixTimerL()
	{
	LBSLOG(ELogP1, "CAdaptationPositioner::StartMaxFixTimerL()");
	if(iMaxFixTimer)	// stop any that's currently running
		{
		iMaxFixTimer->Cancel();
		}
	else
		{
		iMaxFixTimer = CLbsCallbackTimer::NewL(*this);
		}
	
	iMaxFixTimer->EventAfter(iTimeOutInterval, EMaxFixTimerEvent);
	}

void CAdaptationPositioner::StopMaxFixTimer()
	{
	LBSLOG(ELogP1, "CAdaptationPositioner::StopMaxFixTimer()");
	if(iMaxFixTimer)
		{
		iMaxFixTimer->Cancel();
		}
	}

void CAdaptationPositioner::StartWarmDownTimerL()
	{
	TTimeIntervalMicroSeconds32 timeout = KWarmDownTimeout;
	LBSLOG(ELogP1, "CAdaptationPositioner::StartWarmDownTimerL()");
	if(iWarmDownTimer)	// stop any that's currently running
		{
		iWarmDownTimer->Cancel();
		}	
	else
		{
		iWarmDownTimer = CLbsCallbackTimer::NewL(*this);
		}
	iWarmDownTimer->EventAfter(timeout, EWarmDownTimerEvent);
	}

void CAdaptationPositioner::StopWarmDownTimer()
	{
	LBSLOG(ELogP1, "CAdaptationPositioner::StopWarmDownTimer()");
	if(iWarmDownTimer)
		{
		iWarmDownTimer->Cancel();
		}
	}

/**
MaxFixTimerEvent handling

Stop warmdown timer
*/	
void CAdaptationPositioner::MaxFixTimerEvent()
	{
    LBSLOG(ELogP1, "CAdaptationPositioner::MaxFixTimerEvent()");

    // Cancel the warmdown timer since the module has stopped trying
    StopWarmDownTimer();	
	}
	
/**
MaxFixTimer error handling
*/	
TInt CAdaptationPositioner::MaxFixTimerError(TInt /*aError*/)
	{
	return KErrNone;
	}

/**
MaxFixTimerEvent handling

Stop warmdown timer
*/	
void CAdaptationPositioner::WarmDownTimerEvent()
	{
    LBSLOG(ELogP1, "CAdaptationPositioner::MaxFixTimerEvent()");

    // tell positioner Q that our warmdown timer has fired
    iRequestHandler->WarmDownTimerExpired();
	}
	
/**
WarmDown error handling
*/	
TInt CAdaptationPositioner::WarmDownTimerError(TInt /*aError*/)
	{
	return KErrNone;
	}


/**
from MLbsCallbackTimerObserver

@param aTimerId Time event ID to identify different time events
*/
void CAdaptationPositioner::OnTimerEventL(TInt aTimerId)	
	{
	switch(aTimerId)
		{
	case EMaxFixTimerEvent:
		MaxFixTimerEvent();
		break;
	case EWarmDownTimerEvent:
		WarmDownTimerEvent();
		break;
	default:
		__ASSERT_DEBUG(0, User::Panic(KAdaptationPanicCategory, EPanicUnknownTimerEventId));
		}
	}
/**
Timer error handling

@param aError Time error code
@param aTimerId Time event ID to identify two different time events
*/	
TInt CAdaptationPositioner::OnTimerError(TInt aError, TInt aTimerId)
	{
	switch(aTimerId)
		{
	case EMaxFixTimerEvent:
		MaxFixTimerError(aError);
		break;
	case EWarmDownTimerEvent:
		WarmDownTimerError(aError);
		break;
	default:
		__ASSERT_DEBUG(0, User::Panic(KAdaptationPanicCategory, EPanicUnknownTimerEventId));
		}
	return KErrNone; // we have handled the error locally	
	}


TBool CAdaptationPositioner::IsAccurate(const TPositionInfo& aPositionInfo)
	{
	TBool accurate = ETrue;
	TPositionQuality reqQuality; 
	iCriteria.GetRequiredQuality(reqQuality);
	TPosition pos;
	
	aPositionInfo.GetPosition(pos);
	if(pos.HorizontalAccuracy() > reqQuality.HorizontalAccuracy() )
		{
		accurate = EFalse;
		}
	return accurate;
	}

TUint CAdaptationPositioner::InactivityTimeout()
	{
	return iInactivityTimeout;
	}


//  End of File
