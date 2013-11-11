/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Event handling logic for the Mediator Server
*
*/


// INCLUDE FILES
#include    <e32base.h>

#include    "MediatorServerEventHandler.h"
#include    "MediatorServerObjects.h"
#include    "MediatorServiceDefs.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::CMediatorServerEventHandler
// -----------------------------------------------------------------------------
//
CMediatorServerEventHandler::CMediatorServerEventHandler( 
            CMediatorServerObjectHandler& aObjectHandler )
    : CActive( EPriorityStandard ), iObjectHandler( aObjectHandler )         
    {
    }

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorServerEventHandler::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::NewL
// -----------------------------------------------------------------------------
//
CMediatorServerEventHandler* CMediatorServerEventHandler::NewL(
            CMediatorServerObjectHandler& aObjectHandler )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerEventHandler::NewL"));
    CMediatorServerEventHandler* self 
            = new( ELeave ) CMediatorServerEventHandler( aObjectHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::~CMediatorServerEventHandler
// -----------------------------------------------------------------------------
//    
CMediatorServerEventHandler::~CMediatorServerEventHandler()
    {
    Cancel();
    // Clear event list
    iEventServiceList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::DoCancel
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerEventHandler::DoCancel()
    {
    TRACE(Print(_L("[Mediator Server]\t CMediatorServerEventHandler::DoCancel\n")));  
    }

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::RunL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerEventHandler::RunL()
    {
    TRACE(Print(_L("[Mediator Server]\t CMediatorServerEventHandler::RunL status %d\n"), iStatus.Int() ));  
    
    // Should be no errors unless cancel
    if ( iStatus == KErrNone )
        {
        if ( iEventServiceList.Count() > 0 )
            {
            CEvent* serviceEvent = iEventServiceList[0];    // Take the first
            iEventServiceList.Remove(0); // remove event from array
            if ( serviceEvent )
                {
                // Get event information
                TUid domain = serviceEvent->Domain();
                TUid category = serviceEvent->Category();
                TInt eventId = serviceEvent->Id();
                const TDesC8& data = serviceEvent->ParameterData();
                
                RPointerArray<MMediatorServerEventObserver>& observers
                    = serviceEvent->GetObservers();
                
                // Loop through the observer array and send notifications
                for ( TInt index = 0; index < observers.Count(); index++ )
                    {
                    MMediatorServerEventObserver* observer = observers[index]; 
                    if ( observer )
                        {
                        TRAP_IGNORE( observer->MediatorEventL( domain,
                                                               category,
                                                               eventId,
                                                               data ) );
                        }
                    }
                delete serviceEvent;
                }
            // Continue if there's still more to serve
            if ( iEventServiceList.Count() > 0 )
                {
                SetActive();
                iStatus = KRequestPending;
                TRequestStatus* stat = &iStatus;
                User::RequestComplete( stat, KErrNone );
                }
            }
        }
    LOG(_L("[Mediator Server]\t CMediatorServerEventHandler::RunL end"));
    }

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::RegisterEventListL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerEventHandler::RegisterEventListL( 
                TMediatorCategory aCategory, 
                const REventList& aEvents,
                TSecureId aSecureId )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerEventHandler::RegisterEventListL"));
    // Check that domain exists --> if not add new
    CDomain* domain = iObjectHandler.FindDomain( aCategory.iDomain );
    if ( !domain )
        {
        domain = iObjectHandler.AddDomainL( aCategory.iDomain );
        }
    
    // Check that category exists --> if not add new
    TInt ignore = 0; // not used here    
    CCategory* category = domain->FindCategory( aCategory.iCategory, ignore );
    if ( !category )
        {
        category = domain->AddCategoryL( aCategory.iCategory );
        }
    
    // Loop through the events and add them to list
    // Take the possible error to variable
    TInt error = KErrNone;
    TBool stop = EFalse;
    TInt index = 0;
    for ( index = 0; index < aEvents.Count() && !stop; index++ )
        {
        CEvent* newEvent = CEvent::NewL( aEvents[index] );
        CleanupStack::PushL( newEvent );
        newEvent->SetSecureId( aSecureId );
        newEvent->SetCommitState( CItem::EAdded );
        TInt addError = category->AddEvent( newEvent );
        if ( addError )
            {
            ERROR_TRACE(Print(_L("[Mediator] CMediatorServerEventHandler::RegisterEventListL: addError=%d\n"), addError ) );
            ERROR_TRACE(Print(_L("[Mediator] Failed to add event %d to category %d of domain %d\n"), newEvent->Id(), 
                                                                                                     aCategory.iCategory.iUid,
                                                                                                     aCategory.iDomain.iUid ) );
            // in case of error, delete event and take error
            CleanupStack::PopAndDestroy( newEvent );
            error = addError;
            stop = ETrue;
            }
        else
            {
            // Event has been added properly --> just pop
            CleanupStack::Pop( newEvent );
            }     
        }  
    
    TRACE(Print(_L("[Mediator Server]\t Events registered:\n")));
    TRACE(Print(_L("[Mediator Server]\t Success/count: %d/%d \tstatus: %d"), index, aEvents.Count(), error ));    
    // Check error if we need to do partial recovery
    if ( error != KErrNone  ) 
        {
        // Remove events that already have been added
        category->RollbackEvents();
        }
    else
        {
        // Complete event list update
        category->CommitEvents();
        
        // Use the object handler to notify events
        iObjectHandler.EventsAdded( aCategory.iDomain, 
                                    aCategory.iCategory, 
                                    aEvents );
        }
    // In the end leave if error --> client gets error code
    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::UnregisterEventListL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerEventHandler::UnregisterEventListL( TMediatorCategory aCategory, 
                                                        const REventList& aEvents,
                                                        TSecureId aSecureId )
    {
    CCategory* category = iObjectHandler.CategoryL( aCategory );
    TInt error = KErrNone;
    if ( category )
        {
        
        TBool stop = EFalse;
        TInt index = 0;
        // Loop through the list of events and unregister those.
        for ( index = 0; index < aEvents.Count() && !stop; index++ )
            {
            TInt eventIndex = 0;
            TEvent removeEvent = aEvents[index];
            CEvent* eventPtr = category->FindEvent( removeEvent.iEventId, eventIndex );
            if ( !eventPtr )
                {
                ERROR_LOG(_L("[Mediator] CMediatorServerEventHandler::UnregisterEventListL: Event not found\n") );
                ERROR_TRACE(Print(_L("[Mediator] Failed to remove event %d in category %d of domain %d\n"), removeEvent.iEventId, 
                                                                                                       aCategory.iCategory.iUid,
                                                                                                       aCategory.iDomain.iUid ) );
                error = KErrMediatorEventNotFound;
                stop = ETrue;
                }
            else
                {
                // Found the event --> is it own registration
                if ( eventPtr->SecureId() != aSecureId )
                    {
                    ERROR_LOG(_L("[Mediator] CMediatorServerEventHandler::UnregisterEventListL: Secure ID mismatch\n") );
                    ERROR_TRACE(Print(_L("[Mediator] eventPtr->SecureId()=0x%x, aSecureId=0x%x\n"), eventPtr->SecureId().iId, 
                                                                                                    aSecureId.iId ) );
                    error = KErrMediatorSecureIdMismatch;
                    stop = ETrue;
                    }
                else    // Should be ok to unregister
                    {
                    eventPtr->SetCommitState( CItem::ERemoved );
                    }
                }
            }
        
        TRACE(Print(_L("[Mediator Server]\t Events unregistered:\n")));
        TRACE(Print(_L("[Mediator Server]\t Success/count: %d/%d \tstatus: %d"), index, aEvents.Count(), error ));    
        
        // Check error status --> if there's error, need to roll back
        if ( error != KErrNone )
            {
            // Cancel event removal
            category->RollbackEvents();
            }
        else
            {
            // Complete event removal
            category->CommitEvents();
            // Use the object handler to notify event removal
            iObjectHandler.EventsRemoved( aCategory.iDomain, 
                                          aCategory.iCategory, 
                                          aEvents );
            }
        }
    // Leave in case of error situation
    User::LeaveIfError( error );
    }                                                        

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::SubscribeEventListL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerEventHandler::SubscribeEventListL( 
                            TMediatorCategory aCategory, 
                            const REventList& aEvents,
                            TCapabilitySet aCaps,
                            MMediatorServerEventObserver* aObserver )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerEventHandler::SubscribeEventListL"));
    TInt error = KErrNone;
    TBool stop = EFalse;
    TInt index = 0;
    // Find domain & category
    CCategory* category = iObjectHandler.CategoryL( aCategory ); 
    if ( category )
        {
        // Loop through the list of subscribed events
        for ( index = 0; index < aEvents.Count() && !stop; index++ )
            {
            TEvent newEvent = aEvents[index];
            TInt ignore = 0; // Not used
            CEvent* eventPtr = category->FindEvent( newEvent.iEventId, ignore );
            if ( !eventPtr )
                {
                error = KErrMediatorEventNotFound;
                stop = ETrue;   // Partial failure --> stop processing
                }
            else
                {   
                // Check capabilities 
                // Capabilities are checked so that boolean ETrue is returned
                // when all parameter caps can be found from aCaps
                if ( !aCaps.HasCapabilities( eventPtr->Policy() ) )
                    {
#ifdef _DEBUG
                    for ( TInt index = 0; index < ECapability_Limit; index++ )
                        {
                        TCapabilitySet eventCaps = eventPtr->Policy();
                        TBool event = eventCaps.HasCapability( (TCapability) index );
                        TBool requestor = aCaps.HasCapability( (TCapability) index );
                        if ( event && !requestor )
                            {
                            ERROR_TRACE(Print(_L("[Mediator] CMediatorServerEventHandler::SubscribeEventListL: capability %d missing \n"), index ));
                            ERROR_TRACE(Print(_L("[Mediator] Failed to subscribe event %d in category %d of domain %d\n"), newEvent.iEventId, 
                                                                                                                           aCategory.iCategory.iUid,
                                                                                                                           aCategory.iDomain.iUid ) );
                            }
                        }
#endif  // _DEBUG                  
                    error = KErrPermissionDenied;
                    stop = ETrue;
                    }
                // Check (major) version match
                if ( newEvent.iVersion.iMajor != eventPtr->Version().iMajor )
                    {
                    ERROR_LOG(_L("[Mediator] CMediatorServerEventHandler::SubscribeEventListL: Version mismatch\n"));
                    ERROR_TRACE(Print(_L("[Mediator] Registered=%d, subscribed=%d \n"), eventPtr->Version().iMajor,
                                                                                        newEvent.iVersion.iMajor ) );
                    ERROR_TRACE(Print(_L("[Mediator] Failed to subscribe event %d in category %d of domain %d\n"), newEvent.iEventId, 
                                                                                                                   aCategory.iCategory.iUid,
                                                                                                                   aCategory.iDomain.iUid ) );
                    // There's a major version mismatch
                    error = KErrMediatorVersionMismatch;
                    stop = ETrue;
                    }
                if ( !error )   // Everything ok?
                    {
                    // Everything is ok, register subscriber
                    TInt status = eventPtr->AddObserver( aObserver );
                    // If there's double subscription, ignore error
                    // otherwise return the error
                    if ( status &&  status != KErrMediatorAlreadySubscribed )
                        {
                        ERROR_TRACE(Print(_L("[Mediator] CMediatorServerEventHandler::SubscribeEventListL: error=%d\n"), status) );
                        ERROR_TRACE(Print(_L("[Mediator] Failed to subscribe event %d in category %d of domain %d\n"), newEvent.iEventId, 
                                                                                                                       aCategory.iCategory.iUid,
                                                                                                                       aCategory.iDomain.iUid ) );
                        error = status;
                        stop = ETrue;
                        }
                    TRACE(Print(_L("[Mediator Server]\t subscription status: %d\n"), status));
                    }
                }
            }
        
        TRACE(Print(_L("[Mediator Server]\t Events subscribed:\n")));
        TRACE(Print(_L("[Mediator Server]\t Processed/Total: %d/%d \tstatus: %d"), index, aEvents.Count(), error ));    
        // Check error status --> if there's error, need to roll back
        if ( error != KErrNone )
            {
            // Loop the subscribed events and unsubscribe those
            // Index - 1 is because index points to failed item --> stop before processing it again
            for ( TInt newIndex = 0; newIndex < index - 1; newIndex++ )
                {
                TEvent event = aEvents[newIndex];
                TInt ignore = 0; // Not used
                CEvent* eventPtr = category->FindEvent( event.iEventId, ignore );
                if ( eventPtr )
                    {
                    // Nothing can be done in case of error --> ignore return value
                    TInt ignore = eventPtr->RemoveObserver( aObserver );
                    }
                }
            }
        }
    // In the end leave if we have error
    User::LeaveIfError( error );
      
    }

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::UnsubscribeEventListL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerEventHandler::UnsubscribeEventListL( 
                            TMediatorCategory aCategory, 
                            const REventList& aEvents,
                            MMediatorServerEventObserver* aObserver )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerEventHandler::UnsubscribeEventListL"));
    // Find correct category
    CCategory* category = iObjectHandler.CategoryL( aCategory );
    TInt error = KErrNone;
    TInt index = 0;
    
    if ( category )
        {
        TBool stop = EFalse;
        // Loop through the list of events to be unsubscribed
        for ( index = 0; index < aEvents.Count() && !stop; index++ )
            {
            TEvent event = aEvents[index];
            TInt ignore = 0; // Not used
            CEvent* eventPtr = category->FindEvent( event.iEventId, ignore );
            if ( !eventPtr )
                {
                ERROR_LOG(_L("[Mediator] CMediatorServerEventHandler::UnsubscribeEventListL: event not found\n"));
                ERROR_TRACE(Print(_L("[Mediator] Failed to unsubscribe event %d in category %d of domain %d\n"), event.iEventId, 
                                                                                                               aCategory.iCategory.iUid,
                                                                                                               aCategory.iDomain.iUid ) );

                error = KErrMediatorEventNotFound;
                stop = ETrue;   // Partial failure --> stop processing
                }
            else
                {   
                // Everything is ok, unsubscribe
                TInt status = eventPtr->RemoveObserver( aObserver );
                // If there's no subscription existing, ignore error
                // otherwise return the error
                if ( status != KErrMediatorNoSubscription )
                    {
                    ERROR_TRACE(Print(_L("[Mediator] CMediatorServerEventHandler::UnsubscribeEventListL: status=%d\n"), status ) );
                    ERROR_TRACE(Print(_L("[Mediator] Failed to unsubscribe event %d in category %d of domain %d\n"), event.iEventId, 
                                                                                                               aCategory.iCategory.iUid,
                                                                                                               aCategory.iDomain.iUid ) );
                    error = status;
                    stop = ETrue;
                    }
                TRACE(Print(_L("[Mediator Server]\t unsubscribe status: %d\n"), status));
                }
            }
        
        TRACE(Print(_L("[Mediator Server]\t Events unsubscribed:\n")));
        TRACE(Print(_L("[Mediator Server]\t Success/count: %d/%d \tstatus: %d"), index, aEvents.Count(), error ));    
        
        // Check error status --> if there's error, need to roll back
        if ( error != KErrNone )
            {
            // Loop the unsubscribed events and subscribe those
            for ( TInt newIndex = 0; newIndex < index; newIndex++ )
                {
                TEvent event = aEvents[newIndex];
                TInt ignore = 0; // Not used
                CEvent* eventPtr = category->FindEvent( event.iEventId, ignore );
                if ( eventPtr )
                    {
                    // Nothing can be done in case of error --> ignore return value
                    TInt ignore = eventPtr->AddObserver( aObserver );
                    }
                }
            }
        }
    // In the end leave if we have error
    User::LeaveIfError( error );  
    }

// -----------------------------------------------------------------------------
// CMediatorServerEventHandler::RaiseEventL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerEventHandler::RaiseEventL( TMediatorCategory aCategory, 
                                               MediatorService::TEvent aEvent,
                                               const TDesC8& aData )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerEventHandler::RaiseEventL"));
    // Find correct category
    CCategory* category = iObjectHandler.CategoryL( aCategory );
    if ( category )
        {
        TInt ignore = 0; // Not used
        CEvent* eventPtr = category->FindEvent( aEvent.iEventId, ignore );
        
        // Check that event was found
        if ( !eventPtr )
            {
            ERROR_LOG(_L("[Mediator] CMediatorServerEventHandler::RaiseEventL: event not found\n"));
            ERROR_TRACE(Print(_L("[Mediator] Failed to raise event %d in category %d of domain %d\n"), aEvent.iEventId, 
                                                                                                       aCategory.iCategory.iUid,
                                                                                                       aCategory.iDomain.iUid ) );
            User::Leave( KErrMediatorEventNotFound );
            }
        
        // Check the version information match (major
        if ( eventPtr->Version().iMajor != aEvent.iVersion.iMajor )
            {
            ERROR_LOG(_L("[Mediator] CMediatorServerEventHandler::RaiseEventL: Version mismatch\n"));
            ERROR_TRACE(Print(_L("[Mediator] Registered=%d, subscribed=%d \n"), eventPtr->Version().iMajor,
                                                                                aEvent.iVersion.iMajor ) );
            ERROR_TRACE(Print(_L("[Mediator] Failed to raise event %d in category %d of domain %d\n"), aEvent.iEventId, 
                                                                                                       aCategory.iCategory.iUid,
                                                                                                       aCategory.iDomain.iUid ) );                                                                                
            User::Leave( KErrMediatorVersionMismatch );
            }
        
        
        RPointerArray<MMediatorServerEventObserver>& observers
                    = eventPtr->GetObservers();
                
        // Loop through the observer array and send notifications
        for ( TInt index = 0; index < observers.Count(); index++ )
            {
            MMediatorServerEventObserver* observer = observers[index]; 
            if ( observer )
                {
                
                TRAP_IGNORE( observer->MediatorEventL( aCategory.iDomain,
                                                       aCategory.iCategory,
                                                       aEvent.iEventId,
                                                       aData ) );
                }
            }
        }
    }

//  End of File  
