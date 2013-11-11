/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics server reservation handler implementation
*
*/


#include "hwrmhapticsreservationhandler.h"
#include "hwrmhapticstrace.h"
#include "hwrmhapticsclientserver.h"
#include "hwrmhapticsservice.h"
#include "hwrmhapticspolicy.h"

const TInt KDefaultPriority = 0;

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsReservationHandler* CHWRMHapticsReservationHandler::NewL(
    const TDesC& aPolicyFilename )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::NewL()" ) ) );

    CHWRMHapticsReservationHandler* self = 
        new ( ELeave ) CHWRMHapticsReservationHandler();
    
    CleanupStack::PushL( self );
    
    self->ConstructL( aPolicyFilename );
    
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::NewL - return 0x%x" ), self ) );

    return self;
    }
   
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsReservationHandler::~CHWRMHapticsReservationHandler()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::~CHWRMHapticsReservationHandler()" ) ) );

    delete iPolicy;
    iPolicy = NULL;
    
    // delete reservations
    while ( iReservation )
        {
        TReservationData* deleteData = iReservation;
        iReservation = iReservation->iSuspendedData;

        delete deleteData;
        }

    iPriorities.Close();
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::~CHWRMHapticsReservationHandler - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Reserve's resource.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsReservationHandler::ReserveL( 
                                    TSecureId aSid, 
                                    TBool aForceNoCCoeEnv, 
                                    CHWRMHapticsService* aServiceCallback )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::ReserveL(0x%x, 0x%x)" ),
                       aForceNoCCoeEnv, aServiceCallback ) );

    TBool trusted( EFalse );
    TInt priority = KDefaultPriority;
    
    // get priority and trusted/not-trusted value
    GetPriority( aSid, trusted, priority );
    
    // If no CCoeEnv is forced, check that client is trusted
    if ( aForceNoCCoeEnv && !trusted )
        {
        User::Leave( KErrAccessDenied );
        }

    // keeps track whether or not the made reservation was suspended
    TBool reserveSuspended = EFalse;
    
    // If caller is not yet reserving this subresource, reserve it.
    if ( !AlreadyReserved( aServiceCallback ) )
        {
        // if active reservation has higher priority, reservation
        // becomes suspended
        if ( iReservation && iReservation->iPriority >= priority )
            {
            reserveSuspended = ETrue;
            }

        // Create data objects at this point so that any OOM leaves 
        // will not cause reservation state to be uncertain
        TReservationData* newData = new ( ELeave ) TReservationData();

        // Now we have done the memory allocation we need to, 
        // so we can reserve haptics with no risk of OOM errors
        // and therefore be sure that reservations array is never left in 
        // undetermined state.
        newData->iCallback = aServiceCallback;
        newData->iPriority = priority;

        if ( reserveSuspended )
            {
            // Reserve suspended, i.e. go through list until next one has lower priority
            TReservationData* priorityCheckData = iReservation->iSuspendedData;
            TReservationData* priorityPreviousData = iReservation;
            
            while ( priorityCheckData && 
                    priorityCheckData->iPriority <= priority )
                {
                priorityPreviousData = priorityCheckData;
                priorityCheckData = priorityCheckData->iSuspendedData;
                }
                
            // insert new data
            newData->iSuspendedData = priorityCheckData;
            priorityPreviousData->iSuspendedData = newData;
            
            // Notify callback of the reserver that it is suspended
            NotifySuspend( newData );
            }
        else 
            {
            // Reserve as active, i.e. first in list
            newData->iSuspendedData = iReservation;
            iReservation = newData;
            
            // Notify callback of current reserver
            NotifySuspend( newData->iSuspendedData );
            }
        }             
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::ReserveL - return" ) ) );
    
    return reserveSuspended;
    }


// ---------------------------------------------------------------------------
// Releases reservation of a service.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsReservationHandler::Release( 
                                    CHWRMHapticsService* aServiceCallback )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::Release(0x%x)" ), aServiceCallback) );

    // find the reservation data from the list
    TReservationData* found = iReservation;
    TReservationData* prev = NULL;

    // continue searching while there are items in the list, or until 
    // the item is found
    while ( found && found->iCallback != aServiceCallback )
        {
        prev = found;
        found = found->iSuspendedData;
        }

    // remove reservation if it was found
    if ( found )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::Release - Releasing: 0x%x, suspendedData = 0x%x" ), found, found->iSuspendedData ) );
        
        if ( !prev )
            {
            // reservation is the first one in the list, activate next item
            iReservation = found->iSuspendedData;
            }
        else
            {
            // reservation is in the middle of the list, reset list's links
            prev->iSuspendedData = found->iSuspendedData;
            }

        // delete released service
        delete found;
        found = NULL;

        // Reactivate first service on list
        NotifyResume( iReservation );
        }
    else
        {
        // reservation not found
        COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::Release - Reservation not found, releasing nothing" ) ) );
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::Release - return" ) ) );
    
    return (iReservation ? ETrue : EFalse);
    }

// ---------------------------------------------------------------------------
// Returns whether or not haptics is reserved for some other service than
// given one.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsReservationHandler::IsReserved( 
                        const CHWRMHapticsService* aServiceCallback ) const
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::IsReserved(0x%x)" ), aServiceCallback ) );

    TBool retval = EFalse;
    
    // Check if haptics is reserved to somebody other than caller
    if ( iReservation && iReservation->iCallback != aServiceCallback )
        {
        retval = ETrue;
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::IsReserved - return: 0x%x" ), retval ) );
    
    return retval;
    }

// ---------------------------------------------------------------------------
// Checks whether or not haptics is reserved for the given client.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsReservationHandler::ActiveReservation( 
                        const CHWRMHapticsService* aServiceCallback ) const
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::ActiveReservation(0x%x)" ), aServiceCallback ) );

    TBool retval = EFalse;
    
    // Check if haptics is reserved for the given service
    if ( iReservation && iReservation->iCallback == aServiceCallback )
        {
        retval = ETrue;
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::ActiveReservation - return: 0x%x" ), retval ) );
    
    return retval;
    }

// ---------------------------------------------------------------------------
// Checks whether current haptics client's priority is higher than new 
// request making client's priority.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsReservationHandler::ReservedPriorityHigher( 
                                                    TSecureId aSid ) const
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::ReservedPriorityHigher" ) ) );

    TBool ret = EFalse;

    if( iReservation )
        {
        // active reservation is the first reservation data
        TInt activePriority = iReservation->iPriority;
        
        // get priority of the calling service
        TBool trusted = EFalse;
        TInt reqPriority = KDefaultPriority;
        GetPriority( aSid, trusted, reqPriority );
        
        // check priority order
        if ( activePriority >= reqPriority )
            {
            ret = ETrue;
            }
        }
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::ReservedPriorityHigher - ret %d" ), ret ) );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// Sets the priority of a client (used when executing priority setting
// requested by the client using SetDeviceProperty()).
// ---------------------------------------------------------------------------
//
void CHWRMHapticsReservationHandler::SetPriorityL( TSecureId aSid, 
                                                   TInt aPriority )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::SetPriorityL" ) ) );

    // do not insert new priority, if already set; just update old value
    TInt exists = EFalse;
    for ( TInt i = 0; i < iPriorities.Count() && !exists; ++i )
        {
        if ( iPriorities[i].iSid == aSid )
            {
            iPriorities[i].iPriority = aPriority;
            exists = ETrue;
            }
        }

    // if not found, insert new data
    if ( !exists )
        {
        TPriority priority( aSid, aPriority );
        iPriorities.AppendL( priority );
        }
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::SetPriorityL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Removes the priority of a client.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsReservationHandler::RemovePriority( TSecureId aSid )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::RemovePriority" ) ) );

    TInt removed = EFalse;
    for ( TInt i = 0; i < iPriorities.Count() && !removed; ++i )
        {
        if ( iPriorities[i].iSid == aSid )
            {
            iPriorities.Remove( i );
            removed = ETrue;
            }
        }

    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::RemovePriority - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Fetches the priority of a client identified using the given secure id.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsReservationHandler::GetPriority( TSecureId aSid, 
                                               TBool& aTrusted, TInt& aPriority ) const
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::GetPriority" ) ) );

    // initialize return value with the default priority
    TInt priority = KDefaultPriority;
    
    // search from the priority array
    TInt found = EFalse;
    for ( TInt i = 0; i < iPriorities.Count() && !found; ++i )
        {
        if ( iPriorities[i].iSid == aSid )
            {
            priority = iPriorities[i].iPriority;
            found = ETrue;
            }
        }

    TInt policyPriority;
    // search priority from the policy file data
    iPolicy->GetPriority( aSid, aTrusted, policyPriority );

    // set priority from policy file, if not found from priority array
    // and found from the policy file (i.e. trusted client)
    if ( !found && aTrusted )
        {
        priority = policyPriority;
        }
    
    aPriority = priority;
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::GetPriority - return %d" ), priority ) );
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsReservationHandler::CHWRMHapticsReservationHandler()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::CHWRMHapticsReservationHandler()" ) ) );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsReservationHandler::ConstructL( 
                                            const TDesC& aPolicyFilename )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::ConstructL()" ) ) );

    // Initialize policy object
    iPolicy = CHWRMHapticsPolicy::NewL( aPolicyFilename );
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsReservationHandler::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Returns whether or not a reservation exists for the given service.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsReservationHandler::AlreadyReserved( 
                        const CHWRMHapticsService* aService ) const
    {
    TBool reserved = EFalse;

    // go through the reservations
    TReservationData* reservationData = iReservation;
    while ( reservationData && !reserved )
        {
        if ( reservationData->iCallback == aService )
            {
            // reservation found for the service
            reserved = ETrue;
            }
        
        reservationData = reservationData->iSuspendedData;
        }
    
    return reserved;
    }

// ---------------------------------------------------------------------------
// Notifies callback that its reservations has been suspended.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsReservationHandler::NotifySuspend( 
                                            TReservationData* aData ) const
    {
    if ( aData && aData->iCallback )
        {
        aData->iCallback->SuspendResource();
        }
    }

// ---------------------------------------------------------------------------
// Notifies callback that its reservations has been resumed.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsReservationHandler::NotifyResume( 
                                            TReservationData* aData ) const
    {
    if ( aData && aData->iCallback )
        {
        aData->iCallback->ResumeResource();
        }
    }

//  End of File  
