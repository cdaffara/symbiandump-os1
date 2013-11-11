// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbs/epos_cpositioner.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_mposmodulesobserver.h>
#include <centralrepository.h>
#include "lbsdevloggermacros.h"
#include "EPos_ServerPanic.h"
#include "EPos_Global.h"
#include "EPos_CPosCallbackTimer.h"
#include "EPos_CPositionRequest.h"
#include "epos_cposmodulessettings.h"


//TODO Verify
#include "EPos_CPosLocMonitorReqHandlerHub.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "EPos_CPositionRequestTraces.h"
#endif
#include "lbsrootcenrepdefs.h"
#include "lbspositioningstatusprops.h"




// CONSTANTS
#ifdef _DEBUG
_LIT(KTraceFileName, "EPos_CPositionRequest.cpp");
#endif

const TInt KParamPositionInfo = 0;
const TTimeIntervalMicroSeconds KIndFlickerTresholdTracking = 30000000; 

// ================= LOCAL FUNCTIONS ========================

void CancelTimerCleanup(TAny* aTimer)
    {
    (static_cast<CPosCallbackTimer*>(aTimer))->Cancel();
    }

inline TPositionInfoBase& PositionInfoBase(HBufC8* aBuffer)
    {
        return reinterpret_cast<TPositionInfoBase&>
        (const_cast<TUint8&>(*aBuffer->Ptr()));
    }
inline TPositionInfo& PositionInfo(HBufC8* aBuffer)
    {
        return reinterpret_cast<TPositionInfo&>
        (const_cast<TUint8&>(*aBuffer->Ptr()));
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPositionRequest::CPositionRequest(
    CPosModuleSettings& aModuleSettings,
    CPosLocMonitorReqHandlerHub& aLocMonitorReqHandlerHub,
    TProxyPositionerConstructParams& aPositionerParams,
    TBool aIsProxy)
:
    CActive(EPriorityStandard),
    iRequestPhase(EPosReqInactive),
    iPositionerParams(aPositionerParams),
    iHasProxyPositioner(aIsProxy),
    iLocMonitorReqHandler(aLocMonitorReqHandlerHub),
    iModuleSettings(aModuleSettings),
    iPositioningActive( EFalse )
    {
    CActiveScheduler::Add(this);
    }

// EPOC default constructor can leave.
void CPositionRequest::ConstructL()
    {
    TCallBack timeoutCallBack(HandleTimeOut, this);
    iTimeoutTimer = CPosCallbackTimer::NewL(timeoutCallBack);

    TCallBack trackingCallBack(TrackingCallback, this);
    iTrackingTimer = CPosCallbackTimer::NewL(trackingCallBack);

    iModuleSettings.PosModules().GetModuleInfoL(
        iPositionerParams.iImplementationUid, 
        iModuleInfo);
        
    if (!iModuleInfo.IsAvailable())
        {
        User::Leave(KErrNotFound);
        }

    // Get the CategoryUid from the cenrep file owned by LbsRoot.
    CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
    TInt posStatusCategory;
    TInt err = rep->Get(KMoPositioningStatusAPIKey, posStatusCategory);
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(rep);
    iPosStatusCategory = TUid::Uid(posStatusCategory);
    
    LoadPositionerL();
    }

/**
 * Two-phased constructor.
 *
 * @param aModules Location Settings reference
 * @param aLocMonitorReqHandlerHub The hub for requests to the location monitor.
 * @param aPositionerParams contruction parameters needed when creating
 *        positioner.
 * @param aIsProxy ETrue if aImplementationUid represents a proxy PSY,
 *        EFalse otherwise.
 */
CPositionRequest* CPositionRequest::NewL(
    CPosModuleSettings& aModuleSettings,
    CPosLocMonitorReqHandlerHub& aLocMonitorReqHandlerHub,
    TProxyPositionerConstructParams& aPositionerParams,
    TBool aIsProxy)
    {
    CPositionRequest* self = new (ELeave) CPositionRequest(
        aModuleSettings,
        aLocMonitorReqHandlerHub,
        aPositionerParams,
        aIsProxy);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Destructor.
 */
CPositionRequest::~CPositionRequest()
    {
    // Panic client if request is outstanding
    if (IsActive())
        {
        iMessage.Panic(KPosClientFault, EPositionRequestsNotCancelled);
        }
    Cancel();
    if (iTrackingState == EPosTracking)
        {
        StopTracking();
        }
    delete iTrackingTimer;
    delete iPositionBuffer;
    delete iTimeoutTimer;
    delete iPositioner;
    
    // Deactivate the positioning status when the object dies.
    DeActivatePositioningStatusIfNeeded();
    }

/**
 * Starts a position request cycle. Should only be called when PSY is 
 * enabled.
 * @param aMessage the request message from the client
 */
void CPositionRequest::MakeRequestL(const RMessage2& aMessage)
    {
    if (!iModuleInfo.IsAvailable())
        {
        // Deactivate positioning status if there are PSY's not found.
        DeActivatePositioningStatusIfNeeded();
        User::Leave(KErrNotFound);
        }

    __ASSERT_DEBUG(iPositioner, DebugPanic(EPosServerPanicPositionerNotInitialized));
    
    

    iMessage = aMessage; // Store parameter here in case of leave.

    // Clear previous position data
    delete iPositionBuffer;
    iPositionBuffer = NULL;

    HBufC8* clientBuf = Global::CopyClientBuffer8LC(aMessage, KParamPositionInfo);
    CleanupStack::Pop(clientBuf);
    iPositionBuffer = clientBuf;

    TPositionInfoBase& infoBase = PositionInfoBase(iPositionBuffer);
    TUint32 classType = infoBase.PositionClassType();

    TUint32 classesSupported = iModuleInfo.ClassesSupported(EPositionInfoFamily);

    // Check that classtype is supported and is of type TPositionInfo
    if ((classType != (classType & classesSupported)) ||
        !(classType & EPositionInfoClass))
        {
        User::Leave(KErrArgument);
        }

 

    // Set ModuleId to KNullId to be able to verify that Id is set by PSY.
    infoBase.SetModuleId(KNullUid);

    CleanupStack::PushL(TCleanupItem(CancelTimerCleanup, iTimeoutTimer));

		ActivatePositioningStatusIfNeeded();

    // Start timer if necessary
    if (iTimeOut.Int64() > 0)
        {
        LBS_RDEBUG_INFO("CPositionRequest::MakeRequestL() Start Timeout Timer");
        iTimeoutTimer->StartTimer(iTimeOut);
        }

    LBS_RDEBUG_VAR_INT("CPositionRequest::MakeRequestL() iTrackingState", iTrackingState);
    switch (iTrackingState)
        {
        case EPosNoTracking:
        case EPosFirstTrackingRequest:
            StartPositionDataRequestPhase();
            break;

        case EPosTracking:
            StartTrackingTimerWaitPhase();
            break;

        case EPosStopTracking:
            // This must have been handled by Cancel or RunL
        default:
            DebugPanic(EPosServerPanicTrackingInconsistency);
        }

    CleanupStack::Pop(iTimeoutTimer);
    }

/**
 * Set the TPositionUpdateOptions object.
 *
 * When this class is constructed, the TPositionUpdateOptions object
 * is constructed using default constructor with no parameters.
 *
 * @param aOptions The update options from the client.
 */
void CPositionRequest::SetUpdateOptions(
    const TPositionUpdateOptionsBase& aOptions)
    {
    iTimeOut = aOptions.UpdateTimeOut();
    TTimeIntervalMicroSeconds newInterval = aOptions.UpdateInterval();
    TTimeIntervalMicroSeconds oldInterval = iTrackingUpdateInterval;

    if (newInterval != iTrackingUpdateInterval)
        {
        iTrackingUpdateInterval = newInterval;

        if (newInterval == 0) // "stop periodic updates"
            {
            switch (iTrackingState)
                {
                case EPosFirstTrackingRequest:
                    iTrackingState = EPosNoTracking;
                    break;

                case EPosTracking:
                    if (!IsActive())
                        {
                        // can stop it right now
                        StopTracking();
                        }
                    else
                        {
                        // mark to stop later
                        iTrackingState = EPosStopTracking;
                        }
                    break;

                case EPosNoTracking:
                case EPosStopTracking:
                    break;
                default:
                    DebugPanic(EPosServerPanicTrackingInconsistency);
                    break;
                }
            }
        else if (oldInterval != 0) // "use another update interval"
            {
            if (iRequestPhase == EPosReqInactive)
                {
                TInt err;
                TRAP(err, RestartTrackingL());
                }
            else
                {
                // can't affect outstanding request
                // postpone until request is completed
                // it will be handled by RunL or DoCancel
                // via HandleTrackingStateL
                iNewTrackingInterval = ETrue;
                }
            }
        else
            {
            // oldInterval == 0
            // newInterval != 0
            // it means - "start periodic updates"
            iTrackingState = EPosFirstTrackingRequest;
            }
        }
    }

/**
 * Get the TPositionUpdateOptions object.
 * @param aOptions The TPositionUpdateOptions object.
 */
void CPositionRequest::GetUpdateOptions(
    TPositionUpdateOptionsBase& aOptions) const
    {
    aOptions.SetUpdateTimeOut(iTimeOut);
    aOptions.SetUpdateInterval(iTrackingUpdateInterval);
    }

/**
 * Stops current tracking session
 */
void CPositionRequest::NewTrackingSessionIfTracking()
    {
    /* Requestor has been changed. Call Privacy Server. */
    }

/**
 * Called when changes in locations settings occur.
 * @param aEvent Event information
 */
void CPositionRequest::HandleSettingsChangeL(TPosModulesEvent aEvent)
    {
    if (aEvent.iModuleId != iModuleInfo.ModuleId())
        {
        return;
        }
	
    switch (aEvent.iType)
        {
        case EPosModulesEventAvailabilityChanged:
        case EPosModulesEventModuleInstalled:
            iModuleSettings.PosModules().GetModuleInfoL(
                iModuleInfo.ModuleId(), 
                iModuleInfo);
            break;
        case EPosModulesEventModuleRemoved:
            iModuleInfo.SetIsAvailable(EFalse);
            break;
        default:
            return;
        }
		if (!iModuleInfo.IsAvailable())
        {
        // Deactivate positioning status if there are PSY's not found.
        DeActivatePositioningStatusIfNeeded();	
        
        if (IsActive())
            {
            CompleteClient(KErrNotFound);
            Cancel();
            }

        // Unuse positioner and unload it
        if (iTrackingState == EPosTracking)
            {
            StopPsyTracking();
            }
        delete iPositioner;
        iPositioner = NULL;
        }
    else if (!iPositioner)
        {
        // psy is re-enabled after being disabled
        LoadPositionerL();
        if (iTrackingState == EPosTracking)
            {
            StartPsyTrackingL();
            }
        }
    else
        {
        // shouldn't happen, but if it does, ignore it
        }
    }

/**
 * Called when the server class is shutting down.
 */
void CPositionRequest::NotifyServerShutdown()
    {
    if (IsActive())
        {
        DEBUG_TRACE("CPositionRequest::NotifyServerShutdown() with active request", __LINE__)
        CompleteClient(KErrServerTerminated);
        Cancel();
        }

    if (iTrackingState == EPosTracking)
        {
        StopTracking();
        }
    delete iPositioner;
    iPositioner = NULL;
    }

/**
 * From CActive
 */
void CPositionRequest::RunL()
    {
    LBS_RDEBUG_VAR_INT("CPositionRequest::RunL() iRequestPhase", iRequestPhase);
    TInt err = iStatus.Int();
    switch (iRequestPhase)
        {
        case EPosReqPositionRequest:
            {
            LBS_RDEBUG_INFO("CPositionRequest::RunL() EPosReqPositionRequest");
            // Position request finished. Cancel timer.
            iTimeoutTimer->Cancel();
            iRequestPhase = EPosReqInactive;

            CompleteRequest(err);

            HandleTrackingStateL(); // don't care if it leaves
            break;
            }

        case EPosWaitForTracking:
            StartPositionDataRequestPhase();
            break;

        default :
        	DEBUG_TRACE("CPositionRequest::RunL() panicing", __LINE__)
            DebugPanic(EPosServerPanicRequestInconsistency);
        }
    }

/**
 * From CActive
 */
TInt CPositionRequest::RunError(TInt /*aError*/)
    {
    // Happens only if HandleTrackingStateL leaves
    // which in turn means that StartTrackingL leaved somewhere
    // As request is already completed, just ignore the error
    return KErrNone;
    }

/**
 * From CActive
 */
void CPositionRequest::DoCancel()
    {
    OstTraceFunctionEntry1( CPOSITIONREQUEST_DOCANCEL_ENTRY, this );
    LBS_RDEBUG_VAR_INT("CPositionRequest::DoCancel() iRequestPhase", iRequestPhase);
    iTimeoutTimer->Cancel();

    switch (iRequestPhase)
        {
        case EPosReqPositionRequest:
            {
            __ASSERT_DEBUG(iPositioner, DebugPanic(EPosServerPanicPositionerNotInitialized));
            DEBUG_TRACE("calling CPositioner::CancelNotifyPositionUpdate()", __LINE__)
            if(iRequestTimedOut)
            	{
            	iPositioner->CancelNotifyPositionUpdate(KErrTimedOut);
            	}
            else
            	{
            	iPositioner->CancelNotifyPositionUpdate();
            	}
            break;
            }
        case EPosWaitForTracking:
            CompleteSelf(KErrCancel);
            break;

        default:
        	DEBUG_TRACE("CPositionRequest::DoCancel() panicing", __LINE__)
            DebugPanic(EPosServerPanicRequestInconsistency);
        }
    
    // Deactive positioning status after cancellation.
    DeActivatePositioningStatusIfNeeded();

    TInt err;
    if (iRequestTimedOut)
        {
        iRequestTimedOut = EFalse;
        CompleteClient(KErrTimedOut);
        TRAP(err, HandleTrackingStateL());
        }
    else
        {
        CompleteClient(KErrCancel);

        // Handle Tracking State
        if (iTrackingState == EPosStopTracking)
            {
            StopTracking();
            }
        }

    iRequestPhase = EPosReqInactive;
    
    OstTraceFunctionExit1( CPOSITIONREQUEST_DOCANCEL_EXIT, this );
    }


void CPositionRequest::CompleteSelf(TInt aReason)
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aReason);
    }

void CPositionRequest::CompleteClient(TInt aReason)
    {
    if (!iMessage.IsNull())
        {
		LBS_RDEBUG_ARGINT("LBS","Client", "RunL", aReason);
        iMessage.Complete(aReason);
        }
    }

void CPositionRequest::CompleteRequest(TInt aReason)
    {
    // Return fix to the client
    if (aReason == KErrNone || aReason == KPositionPartialUpdate)
        {
        TInt err = PackPositionData();
        // err - client cannot receive result data
        CompleteClient((err != KErrNone) ? err : aReason);

        // Save current fix to LastKnownPosition handler
        // partial updates are not stored
        if ( aReason == KErrNone )
            {
            SaveAsLastKnownPosition();
            }
        }
    else
        {
        CompleteClient(aReason);
        }
    
    // If the request is not tracking mode, deactivate the indicator
    // Tracking mode check and the last check for the error scenarios other than
    // KErrNone and KPositionPartialUpdate.
    if( ( ( iTrackingState == EPosFirstTrackingRequest || iTrackingState == EPosTracking ) &&
    	     iTrackingUpdateInterval >  KIndFlickerTresholdTracking ) ||
          iTrackingUpdateInterval == TTimeIntervalMicroSeconds ( 0 ) ||
          !( aReason == KErrNone || aReason == KPositionPartialUpdate ) )
        {
        DeActivatePositioningStatusIfNeeded();
        }
    }

void CPositionRequest::StartPositionDataRequestPhase()
    {  
    LBS_RDEBUG_VAR_INT("CPositionRequest::StartPositionDataRequestPhase() iRequestPhase", iRequestPhase);
    
    __ASSERT_DEBUG(iPositioner,
        DebugPanic(EPosServerPanicPositionerNotInitialized));

    iReqStartTime.UniversalTime();

    TPositionInfo& info = PositionInfo(iPositionBuffer);

    // Set datum type to WGS84
    TPosition position;
    info.GetPosition(position);
    position.SetDatum(KPositionDatumWgs84);
    info.SetPosition(position);

    TPositionInfoBase& infoBase = reinterpret_cast<TPositionInfoBase&>(info);

    // issue request to psy
    DEBUG_TRACE("calling CPositioner::NotifyPositionUpdate()", __LINE__)

    iStatus = KRequestPending;
    iPositioner->NotifyPositionUpdate(infoBase, iStatus);

    iRequestPhase = EPosReqPositionRequest;
    SetActive();
    }

void CPositionRequest::StartTrackingTimerWaitPhase()
{
	LBS_RDEBUG_VAR_INT("CPositionRequest::StartTrackingTimerWaitPhase() iRequestPhase", iRequestPhase);
    iRequestPhase = EPosWaitForTracking;
    iStatus = KRequestPending;
    SetActive();
}

TInt CPositionRequest::PackPositionData()
    {
    TPositionInfo& info = PositionInfo(iPositionBuffer);

    // Verify that ModuleId, set by PSY, is correct if using specific PSY.
    // If a proxy PSY is used, the proxy is responsible for verifying UID.
    if (!iHasProxyPositioner &&
        info.ModuleId() != iPositionerParams.iImplementationUid)
        {
        return KErrGeneral;
        }

    // Check that datum type still is WGS84
    TPosition position;
    info.GetPosition(position);
    if (position.Datum() != KPositionDatumWgs84)
        {
        return KErrNotSupported;
        }

    TPtr8 ptrToBuffer = iPositionBuffer->Des();
    return Global::Write(iMessage, KParamPositionInfo, ptrToBuffer);
    }

void CPositionRequest::SaveAsLastKnownPosition()
    {
    TPosition pos;
    TPositionInfo& positionInfo = PositionInfo(iPositionBuffer);
    positionInfo.GetPosition(pos);

    // Don't set last known position if the position request is in the past. //TODO check if this is required
    if (iReqStartTime <= pos.Time())
        {
        iLocMonitorReqHandler.SetPositionInfo(positionInfo);
        }
    }

TInt CPositionRequest::HandleTimeOut(TAny* aPositionRequest)
    {
    CPositionRequest* self =
        reinterpret_cast<CPositionRequest*>(aPositionRequest);
    
    LBS_RDEBUG_VAR_INT("CPositionRequest::HandleTimeOut() iRequestPhase", self->iRequestPhase);
    DEBUG_TRACE("CPositionRequest::HandleTimeOut()", __LINE__)    
    __ASSERT_DEBUG(self->iRequestPhase == EPosReqPositionRequest || self->iRequestPhase == EPosWaitForTracking,
        DebugPanic(EPosServerPanicRequestInconsistency));

    self->iRequestTimedOut = ETrue;
    self->Cancel();

    return KErrNone;
    }

TInt CPositionRequest::TrackingCallback(TAny* aPositionRequest)
    {
    CPositionRequest* self =
        reinterpret_cast<CPositionRequest*>(aPositionRequest);

    // continue tracking timer
    if (self->iTrackingState == EPosTracking)
        {
        self->ContinueTrackingTimer();
        }

    if (self->iRequestPhase == EPosWaitForTracking)
        {
        // This is the normal case. The client's request was delayed
        // by the update interval.
        // Complete tracking timer waiting and go next stage
        self->CompleteSelf(KErrNone);
        }

    return KErrNone;
    }

void CPositionRequest::HandleTrackingStateL()
    {
    switch (iTrackingState)
        {
        case EPosNoTracking:
            break;

        case EPosFirstTrackingRequest:
            // start internal tracking
            iTrackingState = EPosTracking;
            ContinueTrackingTimer();

            StartPsyTrackingL();
            break;

        case EPosTracking:
            // if tracking interval was changed
            if (iNewTrackingInterval)
                {
                RestartTrackingL();
                }
            break;

        case EPosStopTracking:
            StopTracking();
            break;

        default:
            DebugPanic(EPosServerPanicTrackingInconsistency);
        }
    }

void CPositionRequest::StartPsyTrackingL()
    {
    __ASSERT_DEBUG(iPositioner, DebugPanic(EPosServerPanicPositionerNotInitialized));

    iNewTrackingInterval = EFalse;

    DEBUG_TRACE("calling CPositioner::TrackingOverridden()", __LINE__)
    if (iPositioner->TrackingOverridden())
        {
        DEBUG_TRACE("calling CPositioner::StartTrackingL()", __LINE__)
        iPositioner->StartTrackingL(iTrackingUpdateInterval);
        iPositionerTrackingStarted = ETrue;
        }
    }

void CPositionRequest::RestartTrackingL()
    {
    LBS_RDEBUG_INFO("CPositionRequest::RestartTrackingL()");
    iTrackingTimer->Cancel();
    StopPsyTracking();
    ContinueTrackingTimer();
    StartPsyTrackingL();
    }

void CPositionRequest::StopTracking()
    {
    LBS_RDEBUG_INFO("CPositionRequest::StopTracking()");
    iTrackingTimer->Cancel();
    iTrackingState = EPosNoTracking;

    StopPsyTracking();
    }

void CPositionRequest::StopPsyTracking()
    {
    if (iPositioner && iPositionerTrackingStarted)
        {
        DEBUG_TRACE("calling CPositioner::StopTracking()", __LINE__)
        iPositioner->StopTracking();
        iPositionerTrackingStarted = EFalse;
        }
    }

void CPositionRequest::ContinueTrackingTimer()
    {
    LBS_RDEBUG_INFO("CPositionRequest::ContinueTrackingTimer()");
    iTrackingTimer->StartTimer(iTrackingUpdateInterval);
    }

void CPositionRequest::LoadPositionerL()
    {
    iPositioner = CPositioner::NewL(&iPositionerParams);
    iPositionerTrackingStarted = EFalse;
    }

void CPositionRequest::ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& aAdditionalTime)
	{
	LBS_RDEBUG_INFO("CPositionRequest::ExtendUpdateTimeOut()");
	iTimeoutTimer->ExtendTimeout(aAdditionalTime);
	}

void CPositionRequest::ActivatePositioningStatusIfNeeded()
    {
    // Check for the positioning status, If it has been activated from this object,
    // there is no need to increment the count again.
    if( !iPositioningActive )
        {
        TInt count, err;     
        err = RProperty::Get(iPosStatusCategory, KLbsMoPositioningStatusKey, count);

        if(err == KErrNone)
            {
            err = RProperty::Set(iPosStatusCategory, KLbsMoPositioningStatusKey, count+1);
            if(err == KErrNone)
                {
                iPositioningActive = ETrue; 
                }
            else
                {
                DEBUG_TRACE("CPositionRequest::ActivatePositioningStatusIfNeeded() - Error in  setting Positioning Status", __LINE__)
                }
            }
        else 
            {
            DEBUG_TRACE("CPositionRequest::ActivatePositioningStatusIfNeeded() - Error in  getting Positioning Status", __LINE__)
			
            }
        }
    }

void CPositionRequest::DeActivatePositioningStatusIfNeeded()
    {
    // If the positioning status is activated by this object, deactivate it
    if( iPositioningActive )
        {
        TInt  count, err;     
        err = RProperty::Get(iPosStatusCategory, KLbsMoPositioningStatusKey, count);
        
        if(err == KErrNone)
        	{
            __ASSERT_DEBUG(count > 0, DebugPanic(EPosServerPanicIndDeactivationFailed));
        	
            err = RProperty::Set(iPosStatusCategory, KLbsMoPositioningStatusKey, count-1);
            if(err == KErrNone)
                {
                iPositioningActive = EFalse;
                }
            else
                {
                DEBUG_TRACE("CPositionRequest::DeActivatePositioningStatusIfNeeded() - error in setting Positioning Status", __LINE__)	
                }		
        	}
        else
        	{
        	DEBUG_TRACE("CPositionRequest::DeActivatePositioningStatusIfNeeded() - error in getting Positioning Status", __LINE__)            	
        	}	
        }
    }
//  End of File
