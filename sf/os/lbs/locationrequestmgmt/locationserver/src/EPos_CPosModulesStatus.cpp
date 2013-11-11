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
#include <e32property.h>
#include <centralrepository.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleidlist.h>
#include "epos_mposmodulestatuslistener.h"

#include "EPos_CPosModulesStatus.h"
#include "EPos_Global.h"
#include "EPos_ServerPanic.h"
#include "lbsrootcenrepdefs.h"
#include "lbspositioningstatusprops.h"

// CONSTANTS
#ifdef _DEBUG
_LIT(KTraceFileName, "EPos_CPosModulesStatus.cpp");
#endif

const TInt KParamModuleStatusEvent = 0;
const TInt KParamModuleId = 1;

const TTimeIntervalMicroSeconds32  KTimerInterval = 5000000; // microseconds
const TInt KSequenceNoStartValue = 1;

const TInt KStatusInactive = TPositionModuleStatus::EDeviceInactive;
const TInt KStatusDisabled = TPositionModuleStatus::EDeviceDisabled;
const TInt KStatusError = TPositionModuleStatus::EDeviceError;

const TInt KEventInstalled = TPositionModuleStatusEvent::ESystemModuleInstalled;
const TInt KEventRemoved = TPositionModuleStatusEvent::ESystemModuleRemoved;
const TInt KEventStatus = TPositionModuleStatusEvent::EEventDeviceStatus;
const TInt KEventSystem = TPositionModuleStatusEvent::EEventSystemModuleEvent;
const TInt KEventQuality = TPositionModuleStatusEvent::EEventDataQualityStatus;


// LOCAL FUNCTION PROTOTYPES
TBool MatchModuleStatus(const CPosModulesStatus::TPosModule& aFirst, const CPosModulesStatus::TPosModule& aSecond);

TIdentityRelation<CPosModulesStatus::TPosModule> moduleMatcher(MatchModuleStatus);

TBool MatchSubscriber(const CPosModulesStatus::TPosSubscriber& aFirst, const CPosModulesStatus::TPosSubscriber& aSecond);

TIdentityRelation<CPosModulesStatus::TPosSubscriber> subscriberMatcher(MatchSubscriber);

TBool EventMatchesRequested(const TPositionModuleStatusEvent& aOccurredEvent, const CPosModulesStatus::TPosSubscriber& aSubscriber);

// ==================== LOCAL FUNCTIONS ====================

TBool MatchModuleStatus(const CPosModulesStatus::TPosModule& aFirst, const CPosModulesStatus::TPosModule& aSecond)
    {
    return (aFirst.iModuleId == aSecond.iModuleId);
    }

TBool MatchSubscriber(const CPosModulesStatus::TPosSubscriber& aFirst, const CPosModulesStatus::TPosSubscriber& aSecond)
    {
    return (aFirst.iClient == aSecond.iClient);
    }

TBool EventMatchesRequested(const TPositionModuleStatusEvent& aOccurredEvent, const CPosModulesStatus::TPosSubscriber& aSubscriber)
    {
    // Does module id match?
    if ((aSubscriber.iModuleId != KPositionNullModuleId) &&
        (aSubscriber.iModuleId != aOccurredEvent.ModuleId()))
        {
        return EFalse;
        }
    
    // Does requested event match occurred event?
    if (!(aSubscriber.iEvent.RequestedEvents() & 
        aOccurredEvent.OccurredEvents()))
        {
        return EFalse;
        }

    return ETrue;
    }

// ================= MEMBER FUNCTIONS =======================

/**
 * C++ default constructor.
 */
CPosModulesStatus::CPosModulesStatus( CPosModules& aModules, const TTimeIntervalMicroSeconds32& aTimerInterval)
    : iModules(aModules),
    iSequenceNo(KSequenceNoStartValue),
    iTimerInterval(aTimerInterval)
    {
    }

/**
 * Symbian constructor
 */
void CPosModulesStatus::ConstructL()
    {
    CPosModuleIdList* idList = iModules.ModuleIdListLC();
    TInt count = idList->Count();
    for (TInt i = 0; i < count; i++)
        {
        // Set module id
        TPosModule module;
        module.iModuleId = (*idList)[i];

        // Set device status
        TPositionModuleInfo moduleInfo;
        TRAPD(err, iModules.GetModuleInfoL((*idList)[i], moduleInfo));
        if (err == KErrNone) 
            {
            module.iStatus.SetDeviceStatus( moduleInfo.IsAvailable() ? 
                KStatusInactive : KStatusDisabled);
            }
        else 
            {
            module.iStatus.SetDeviceStatus(KStatusError);
            }

        // Store module status
        User::LeaveIfError(iModuleStatusArray.Append(module));
        }
    CleanupStack::PopAndDestroy(idList);

    iTimer = CPeriodic::NewL(EPriorityLow);
    }

/**
 * Two-phased constructor.
 */
CPosModulesStatus* CPosModulesStatus::NewL(CPosModules& aModules)
    {
    return NewL(aModules, KTimerInterval);
    }

/**
 * Two-phased constructor.
 */
CPosModulesStatus* CPosModulesStatus::NewL(CPosModules& aModules, const TTimeIntervalMicroSeconds32& aTimerInterval)
    {
    CPosModulesStatus* self = new (ELeave) CPosModulesStatus(aModules, aTimerInterval);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Destructor.
 */
CPosModulesStatus::~CPosModulesStatus()
    {
    iListenerArray.Close();
    iModuleStatusArray.Close();
    iSubscribers.Close();
    delete iTimer;
    }

/**
 * From MPosModuleStatusManager
 */
void CPosModulesStatus::AddListenerL(MPosModuleStatusListener& aListener)
    {
    User::LeaveIfError( iListenerArray.Append( &aListener ) );
    }

/**
 * From MPosModuleStatusManager
 */
void CPosModulesStatus::RemoveListener(MPosModuleStatusListener& aListener)
    {
    TInt count = iListenerArray.Count();
    for ( TInt i = count-1; i >= 0; i-- )
        {
        if ( iListenerArray[i] == &aListener )
            {
            iListenerArray.Remove( i );
            }
        }
    }


/**
 * From MPosModuleStatusManager
 */
TInt CPosModulesStatus::GetModuleStatus(const TPositionModuleId& aModuleId, TPositionModuleStatusBase& aStatus) const
    {
    if (!(aStatus.PositionClassType() == EPositionModuleStatusClass))
        {
        return KErrNotSupported;
        }

    // Set the module status
    TPositionModuleStatus& status = 
        static_cast<TPositionModuleStatus&>(aStatus);

    // Find our stored module status
    TPosModule module;
    module.iModuleId = aModuleId;
    TInt index = iModuleStatusArray.Find(module, moduleMatcher);
    if (index == KErrNotFound)
        {
        return KErrNotFound;
        }

    TPosModule storedModule = iModuleStatusArray[index];

    status.SetDeviceStatus(storedModule.iStatus.DeviceStatus());
    status.SetDataQualityStatus(storedModule.iStatus.DataQualityStatus());
    
    return KErrNone;
    }

/**
 * Requests for status change notification.
 *
 * @param aMessage The message that will be completed when the  
 *                 requested event occurs. The message also contains  
 *                 all the request information.
 * @param aClient  A pointer to the client that handles the request.
 */ 
void CPosModulesStatus::NotifyModuleStatusEventL(const RMessage2& aMessage, TAny* aClient)
    {
    TPositionModuleId id;
    TPositionModuleStatusEvent event;

    ValidateRequestL(aMessage, id, event);

    TPosSubscriber subscriber;
    subscriber.iClient = aClient;

    // Is the client previously known?
    TInt index = iSubscribers.Find(subscriber, subscriberMatcher);
    if (index == KErrNotFound)
        {
        subscriber.iToBeRemoved = EFalse;
        subscriber.iHasRequest = ETrue;
        subscriber.iMsg = aMessage;
        subscriber.iModuleId = id;
        subscriber.iEvent = event;
        User::LeaveIfError(iSubscribers.Append(subscriber));
        }
    else
        {
        TPosSubscriber& storedSubscriber = iSubscribers[index];
        if (storedSubscriber.iHasRequest)
            {
            iSubscribers.Remove(index);
            aMessage.Panic(KPosClientFault, EPositionDuplicateRequest);
            return;
            }
        storedSubscriber.iToBeRemoved = EFalse;
        storedSubscriber.iMsg = aMessage;
        storedSubscriber.iModuleId = id;
        storedSubscriber.iEvent = event;
        if (!HasValidEventsQueued(storedSubscriber))
            {
            storedSubscriber.iHasRequest = ETrue;
            }
        }
    }

/**
 * Cancels a requested status change notification.
 *
 * @param aClient  A pointer to the client to cancel.
 */
void CPosModulesStatus::CancelNotifyModuleStatusEventL(TAny* aClient)
    {
    TPosSubscriber subscriber;
    subscriber.iClient = aClient;

    TInt index = iSubscribers.Find(subscriber, subscriberMatcher);
    if (index == KErrNotFound)
        {
        User::Leave(KErrNotFound);
        }

    TPosSubscriber storedSubscriber = iSubscribers[index];

    iSubscribers.Remove(index);

    if (!storedSubscriber.iHasRequest)
        {
        User::Leave(KErrNotFound);
        }

    RequestComplete(storedSubscriber.iMsg, KErrCancel);
    }

/**
 * This function is called by a session when it is closed. It panics the
 * client if it has any open request. A request subscriber to the
 * closing session is removed.
 *
 * @param aClient  A pointer to the client to cancel.
 */
void CPosModulesStatus::NotifySessionClosed(TAny* aClient)
    {
    TPosSubscriber subscriber;
    subscriber.iClient = aClient;

    TInt index = iSubscribers.Find(subscriber, subscriberMatcher);
    if (index == KErrNotFound)
        {
        return;
        }

    TPosSubscriber storedSubscriber = iSubscribers[index];

    iSubscribers.Remove(index);

    if (storedSubscriber.iHasRequest)
        {
        storedSubscriber.iMsg.Panic(KPosClientFault, EPositionRequestsNotCancelled);
        }
    }

/**
 * Called when a change has been detected in the location settings.
 * @param aEvent Event information
 */
void CPosModulesStatus::HandleSettingsChangeL(TPosModulesEvent aEvent)
    {
    DEBUG_TRACE("HandleSettingsChange", __LINE__)
    
    TPosModule module;
    module.iModuleId = aEvent.iModuleId;

    TPositionModuleStatusEvent event;
    event.SetModuleId(aEvent.iModuleId);

    switch (aEvent.iType)
        {
        case EPosModulesEventModuleInstalled:
            {
            module.iStatus.SetDeviceStatus(KStatusInactive);
            module.iDeviceTimeStamp = iSequenceNo;
            module.iDataQualityTimeStamp = iSequenceNo++;

            User::LeaveIfError(iModuleStatusArray.Append(module));

            event.SetSystemModuleEvent(KEventInstalled);
            event.SetOccurredEvents(KEventSystem | KEventStatus);
            event.SetModuleStatus(module.iStatus);
            break;
            }
        case EPosModulesEventModuleRemoved:
            {
            TInt pos = iModuleStatusArray.Find(module, moduleMatcher);
            if (pos != KErrNotFound)
                {
                iModuleStatusArray.Remove(pos);

                event.SetSystemModuleEvent(KEventRemoved);
                event.SetOccurredEvents(KEventSystem);
                }
            break;
            }
        case EPosModulesEventAvailabilityChanged:
            {
            TPositionModuleInfo moduleInfo;
            iModules.GetModuleInfoL(aEvent.iModuleId, moduleInfo);

            TInt pos = iModuleStatusArray.Find(module, moduleMatcher);
            if (pos != KErrNotFound)
                {
                TPosModule& storedModule = iModuleStatusArray[pos];

                if (moduleInfo.IsAvailable() && 
                    storedModule.iStatus.DeviceStatus() == KStatusDisabled)
                    {
                    storedModule.iStatus.SetDeviceStatus(KStatusInactive);
                    event.SetOccurredEvents(KEventStatus);
                    }
                else if (!moduleInfo.IsAvailable() &&
                    storedModule.iStatus.DeviceStatus() != KStatusDisabled)
                    {
                    storedModule.iStatus.SetDeviceStatus(KStatusDisabled);
                    event.SetOccurredEvents(KEventStatus);
                    }
                else 
                    {
                    }
                    
                if (event.OccurredEvents())
                    {
                    storedModule.iDeviceTimeStamp = iSequenceNo++;
                    event.SetModuleStatus(storedModule.iStatus);
                    }
                }

            break;
            }
        
        case EPosModulesEventVisibilityChanged:
        case EPosModulesEventPrioritiesChanged:
        case EPosModulesEventCostChanged:
            break;

        default:
            DebugPanic(EPosServerPanicUnexpectedEvent);
            break;
        }

    // Has any event occurred?
    if (event.OccurredEvents())
        {
        NotifySubscribers(event);
        NotifyListeners( event );
        }
    }

/**
 * From MPosStatusObserver.
 *
 * Reports that the positioner status has changed.
 *
 * @param aStatus The new status.
 */
void CPosModulesStatus::ReportStatus(
    const TPositionModuleId& aImplementationUid,
    const TPositionModuleStatus& aStatus)
    {
#ifdef _DEBUG
    _LIT(KTracePoint, "ReportStatus()");
    Global::Trace(KTracePoint, KTraceFileName, __LINE__);
#endif

    TPosModule module;
    module.iModuleId = aImplementationUid;
    TInt index = iModuleStatusArray.Find(module, moduleMatcher);
    if (index == KErrNotFound)
        {
        // This should never happen if PSYs are implemented correctly.
        // Ignore the event.
        return;
        }

    TPosModule& storedModule = iModuleStatusArray[index];
    TPositionModuleStatus::TDeviceStatus storedDeviceStatus = 
        storedModule.iStatus.DeviceStatus();
    if (storedDeviceStatus == KStatusDisabled)
        {
        // Ignore event if module is disabled
        return;
        }

    TPositionModuleStatusEvent event;
    TPositionModuleStatus::TDeviceStatus newDeviceStatus = 
        aStatus.DeviceStatus();
    if (storedDeviceStatus != newDeviceStatus)
        {
        storedModule.iStatus.SetDeviceStatus(newDeviceStatus);
        storedModule.iDeviceTimeStamp = iSequenceNo;
        event.SetOccurredEvents(KEventStatus);
        }
    if (storedModule.iStatus.DataQualityStatus() !=
        aStatus.DataQualityStatus())
        {
        storedModule.iStatus.SetDataQualityStatus(aStatus.DataQualityStatus());
        storedModule.iDataQualityTimeStamp = iSequenceNo;
        event.SetOccurredEvents(KEventQuality | (event.OccurredEvents()));
        }
    
    // Has any event occurred?
    if (event.OccurredEvents())
        {
        // In order to get the same time stamp on both device status and data
        // quality status, the counter is not incremented until here.
        iSequenceNo++;
        event.SetModuleId(aImplementationUid);
        event.SetModuleStatus(storedModule.iStatus);
        NotifySubscribers(event);
        NotifyListeners( event );
        }
    }

/**
 * Validate request
 */
void CPosModulesStatus::ValidateRequestL(
    const RMessage2& aMessage,
    TPositionModuleId& aId,
    TPositionModuleStatusEvent& aEvent)
    {
DEBUG_TRACE("ValidateRequestL in", __LINE__);
    HBufC8* buffer = Global::CopyClientBuffer8LC(aMessage, KParamModuleStatusEvent);

    // Read module status event 
    const TPositionModuleStatusEventBase* moduleStatusEventBase =
        reinterpret_cast<const TPositionModuleStatusEventBase*>(buffer->Ptr());

    Global::ValidatePositionClassBufferL(*moduleStatusEventBase, buffer->Des());
    Global::ValidatePositionClassTypeL(*moduleStatusEventBase, EPositionModuleStatusEventClass);

    CleanupStack::PopAndDestroy(buffer);

DEBUG_TRACE("Reading event object", __LINE__);
    // Event type is ok so store the client data
	TPckg<TPositionModuleStatusEvent> eventBuf(aEvent);
    User::LeaveIfError(Global::Read(aMessage, KParamModuleStatusEvent, eventBuf));

DEBUG_TRACE("Reading module id", __LINE__);
    // Unpack the module id
	TPckg<TPositionModuleId> moduleId(aId);
	User::LeaveIfError(Global::Read(aMessage, KParamModuleId, moduleId));

    // Is module id valid?
    if (aId != KPositionNullModuleId)
        {
        TPosModule module;
        module.iModuleId = aId;

        TInt index = iModuleStatusArray.Find(module, moduleMatcher);
        if (index == KErrNotFound)
            {
            User::Leave(KErrNotFound);
            }
        }
DEBUG_TRACE("ValidateRequestL out", __LINE__);
    }

/**
 * Check if valid event queued
 */
TBool CPosModulesStatus::HasValidEventsQueued(TPosSubscriber& aSubscriber)
    {
    if (aSubscriber.iClientTimeStamp == (iSequenceNo - 1))
        {
        // No event has occurred
        return EFalse;
        }

    if (aSubscriber.iModuleId != KPositionNullModuleId)
        {
        // Only one module needs to be checked
        TPosModule module;
        module.iModuleId = aSubscriber.iModuleId;
        TInt index = iModuleStatusArray.Find(module, moduleMatcher);
        TPosModule storedModule = iModuleStatusArray[index];
        
        TPositionModuleStatusEvent event;
        if ((aSubscriber.iEvent.RequestedEvents() & KEventStatus) &&
            (storedModule.iDeviceTimeStamp > aSubscriber.iClientTimeStamp))
            {
            event.SetOccurredEvents(KEventStatus);
            }
        if ((aSubscriber.iEvent.RequestedEvents() & KEventQuality) &&
            (storedModule.iDataQualityTimeStamp > aSubscriber.iClientTimeStamp))
            {
            event.SetOccurredEvents(KEventQuality | event.OccurredEvents());
            }

        if (event.OccurredEvents())
            {
            aSubscriber.iClientTimeStamp = 
                Max(storedModule.iDeviceTimeStamp,
                    storedModule.iDataQualityTimeStamp);

            event.SetModuleId(storedModule.iModuleId);
            event.SetModuleStatus(storedModule.iStatus);

            NotifyEventToSubscriber(event, aSubscriber);

            return ETrue;
            }

        return EFalse;
        }

    // Search all matching modules for lowest time stamp later than client
    TInt lowestValidTimeStamp = iSequenceNo;
    TPositionModuleStatusEvent lowestValidEvent;
    TInt count = iModuleStatusArray.Count();
    for (TInt j = 0; j < count; j++)
        {
        TPosModule storedModule = iModuleStatusArray[j];

        TPositionModuleStatusEvent event;
        if ((aSubscriber.iEvent.RequestedEvents() & KEventStatus) &&
            (storedModule.iDeviceTimeStamp > aSubscriber.iClientTimeStamp) &&
            (storedModule.iDeviceTimeStamp < lowestValidTimeStamp))
            {
            event.SetOccurredEvents(KEventStatus);
            lowestValidTimeStamp = storedModule.iDeviceTimeStamp;

            // Check if data quality status change occurred at the same time
            if ((aSubscriber.iEvent.RequestedEvents() & KEventQuality) &&
                (storedModule.iDataQualityTimeStamp == storedModule.iDeviceTimeStamp))
                {
                event.SetOccurredEvents(KEventQuality | event.OccurredEvents());
                }
            }

        if ((aSubscriber.iEvent.RequestedEvents() & KEventQuality) &&
            (storedModule.iDataQualityTimeStamp > aSubscriber.iClientTimeStamp) &&
            (storedModule.iDataQualityTimeStamp < lowestValidTimeStamp))
            {
            event.SetOccurredEvents(KEventQuality);
            lowestValidTimeStamp = storedModule.iDataQualityTimeStamp;
            }

        if (event.OccurredEvents())
            {
            lowestValidEvent = event;

            lowestValidEvent.SetModuleId(storedModule.iModuleId);
            lowestValidEvent.SetModuleStatus(storedModule.iStatus);
            }
        }

    if (lowestValidEvent.OccurredEvents())
        {
        aSubscriber.iClientTimeStamp = lowestValidTimeStamp;

        NotifyEventToSubscriber(lowestValidEvent, aSubscriber);

        return ETrue;
        }

    return EFalse;
    }

/**
 * Notify subscribers
 */
void CPosModulesStatus::NotifySubscribers(
    TPositionModuleStatusEvent& aOccurredEvent)
    {
    TInt count = iSubscribers.Count();
    for (TInt i = 0; i < count; i++)
        {
        TPosSubscriber& subscriber = iSubscribers[i];
        if (subscriber.iHasRequest)
            {
            if (EventMatchesRequested(aOccurredEvent, subscriber))
                {
                NotifyEventToSubscriber(aOccurredEvent, subscriber);

                subscriber.iClientTimeStamp = iSequenceNo++;
                subscriber.iHasRequest = EFalse;
                subscriber.iToBeRemoved = EFalse;

                if (!iTimer->IsActive())
                    {
                    iTimer->Start(iTimerInterval, iTimerInterval, 
                                  TCallBack(TimerCallback, this));
                    }
                }
            }
        }
    }

/**
 * Notify listener
 */
void CPosModulesStatus::NotifyListeners(TPositionModuleStatusEvent& aOccurredEvent)
    {
    TInt count = iListenerArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iListenerArray[i]->HandleModuleStatusChanged(
            aOccurredEvent );
        }
    }

/**
 * Notify event to subscriber
 */
void CPosModulesStatus::NotifyEventToSubscriber(TPositionModuleStatusEvent& aOccurredEvent, const CPosModulesStatus::TPosSubscriber& aSubscriber)
    {
    aOccurredEvent.SetRequestedEvents(aSubscriber.iEvent.RequestedEvents());

    DEBUG_TRACE("Writing event object in", __LINE__);
    // Write back to client
    TPckgC<TPositionModuleStatusEvent> eventPckg(aOccurredEvent);
    TInt res = Global::Write(aSubscriber.iMsg, KParamModuleStatusEvent, eventPckg);
    if (res == KErrNone)
        {
	    RequestComplete(aSubscriber.iMsg, KErrNone);
        }
    DEBUG_TRACE("Writing event object out", __LINE__);
    }

/**
 * Timer callback
 */
TInt CPosModulesStatus::TimerCallback(TAny* aModulesStatus)
    {
    CPosModulesStatus* moduleStatus = 
        reinterpret_cast<CPosModulesStatus*> (aModulesStatus);
    return moduleStatus->DoTimerCallback();
    }

/**
 * timer callback handler function
 */
TInt CPosModulesStatus::DoTimerCallback()
    {
    TBool timerIsNeeded = EFalse;
    for (TInt i = iSubscribers.Count() - 1; i >= 0; i--)
        {
        TPosSubscriber& subscriber = iSubscribers[i];
        if (!subscriber.iHasRequest)
            {
            if (subscriber.iToBeRemoved)
                {
                iSubscribers.Remove(i);
                }
            else
                {
                subscriber.iToBeRemoved = ETrue;
                timerIsNeeded = ETrue;
                }
            }
        }

    if (!timerIsNeeded)
        {
        iTimer->Cancel();
        return EFalse;
        }

    return ETrue;
    }

/**
 * Complete request
 */
void CPosModulesStatus::RequestComplete(const RMessage2& aMessage, TInt aCompleteCode)
    {
    if (!aMessage.IsNull())
        {
        aMessage.Complete(aCompleteCode);
        }
    }

//  End of File
