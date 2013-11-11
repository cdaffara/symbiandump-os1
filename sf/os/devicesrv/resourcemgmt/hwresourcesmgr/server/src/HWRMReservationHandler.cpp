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




// INCLUDE FILES
#include "HWRMReservationHandler.h"
#include "HWRMtrace.h"
#include "HWRMClientServer.h" // panic ref
#include "HWRMService.h"


// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
_LIT( KPanicCategory, "HWRMReservationHandler" );

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMReservationHandler::CHWRMReservationHandler
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMReservationHandler::CHWRMReservationHandler(TInt aSubResourceCount)
    : iSubResourceCount(aSubResourceCount)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMReservationHandler::CHWRMReservationHandler()" ) );
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMReservationHandler::CHWRMReservationHandler - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMReservationHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMReservationHandler* CHWRMReservationHandler::NewL(TInt aSubResourceCount, const TDesC& aPolicyFilename)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMReservationHandler::NewL(%d)" ), aSubResourceCount );

    CHWRMReservationHandler* self = new( ELeave ) CHWRMReservationHandler(aSubResourceCount);
    
    CleanupStack::PushL( self );
    
    self->ConstructL(aPolicyFilename);
    
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMReservationHandler::NewL - return 0x%x" ), self );

    return self;
    }
   
// -----------------------------------------------------------------------------
// CHWRMServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMReservationHandler::ConstructL(const TDesC& aPolicyFilename)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMReservationHandler::ConstructL()" ) );
 
    // Initialize subtarget array with NULLS and create iAllMask
    iAllMask = 0x0;
    for ( TInt i=0; i < iSubResourceCount; i++ )
        {
        iReservations.AppendL(NULL);
        
        iAllMask <<= 1;  // initial shift is ignored as mask is zero on first loop
        iAllMask |= 0x1;
        }

    // Initialize owned pointers
    iPolicy = CHWRMPolicy::NewL(aPolicyFilename);
    
    // Consider all targets active by default
    iActiveMask = iAllMask;

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMReservationHandler::ConstructL - return" ) );
    }   

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMReservationHandler::~CHWRMReservationHandler()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMReservationHandler::~CHWRMReservationHandler()" ) );

	delete iPolicy;
	iPolicy = NULL;
	
    // destroy any reservations
    for ( TInt i=0; i < iReservations.Count(); i++ )
        {
        while ( iReservations[i] )
            {
            TReservationData* deleteData = iReservations[i];
            iReservations[i] = iReservations[i]->iSuspendedData;
            delete deleteData;
            }
        }
    iReservations.ResetAndDestroy();

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMReservationHandler::~CHWRMReservationHandler - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMReservationHandler::ReserveL
// Reserves resource
// -----------------------------------------------------------------------------
//
TInt CHWRMReservationHandler::ReserveL(const RMessagePtr2& aMessage, TBool aForceNoCCoeEnv, CHWRMService* aServiceCallback, TInt aSubResourceMask)
    {
    COMPONENT_TRACE5(_L( "HWRM Server - CHWRMReservationHandler::ReserveL(0x%x, 0x%x, 0x%x, 0x%x)" ), aMessage.Handle(), aForceNoCCoeEnv, aServiceCallback, aSubResourceMask );

    __ASSERT_ALWAYS(aServiceCallback, User::Panic(KPanicCategory, EPanicBadHandle));  

    TBool trusted(EFalse);
    TInt subMask(aSubResourceMask);  // Mask of subresources that caller wants to reserve
    TInt reservedSuspendedMask(0x0); // Mask of subresources reserved in suspended mode
    TInt alreadyReservedMask(0x0);   // Mask of subresources already reserved by this caller

    // Get priority
    TInt priority = iPolicy->GetPriority(aMessage.SecureId(), trusted);
   
    // If no CCoeEnv is forced, check that client is trusted
    if ( aForceNoCCoeEnv && !trusted )
        {
        User::Leave(KErrAccessDenied);
        }

    // New reservation data(s) might be required for suspended reservations
    // If multiple targets are suspended, datas create linked list
    TReservationData* newDatas = NULL;
    TInt newDataCount(0);

    // If whole resource specified, use iAllMask
    if ( aSubResourceMask == KHWRMAllSubResources)
        {
        subMask = iAllMask;
        }

    // Check reservations of all desired subtargets
    TInt compareMask(0x1);
    for ( TInt i=0; i < iSubResourceCount; i++ )
        {
        if ( (compareMask & subMask) )
            {
            TReservationData* reservationCheckData = iReservations[i];

            // Check if caller is already reserving this subresource
            while( reservationCheckData && !(alreadyReservedMask & compareMask) )
                {
                if ( reservationCheckData->iCallback == aServiceCallback )
                    {
                    alreadyReservedMask |= compareMask;
                    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMReservationHandler::ReserveL - Duplicate reservation for subresource 0x%x - Ignoring" ), compareMask );
                    }
                reservationCheckData = reservationCheckData->iSuspendedData;
                }
            
            // If caller is not yet reserving this subresource, reserve it.
            if ( !(alreadyReservedMask & compareMask) )
                {
                if ( iReservations[i] && iReservations[i]->iPriority <= priority )
                    {
                    // Add to reserve suspended mask
                    reservedSuspendedMask |= compareMask;
                    }

                // Create data objects at this point so that any OOM leaves 
                // will not cause reservation state to be uncertain
                TReservationData* newOne = new (ELeave) TReservationData;
                CleanupStack::PushL(newOne);
                ++newDataCount;

                if ( !newDatas )
                    {
                    newDatas = newOne;
                    }
                else
                    {
                    // There is already at least one newDatas, so link the new one up with old ones
                    TReservationData* compareData = newDatas;
                    while ( compareData->iSuspendedData )
                        {
                        compareData = compareData->iSuspendedData;	
                        }
                    compareData->iSuspendedData = newOne;
                    }
                }             
            }
            
        compareMask <<= 1;
        }

    // Pop the TReservationData structures from the cleanup stack. This can be done here
    // because no leaving functions are called in the following loop.
    CleanupStack::Pop(newDataCount);

    // Now we have done all memory allocations we need to, 
    // so we can reserve specified subresources with no risk of OOM errors
    // and therefore be sure that reservations array is never left in undetermined state.
    compareMask = 0x1;
    for ( TInt j=0; j < iSubResourceCount; j++ )
        {
        if ( (compareMask & subMask) && !(alreadyReservedMask & compareMask) )
            {
            // if things work right, this assert will never trigger
            __ASSERT_ALWAYS(newDatas, User::Panic(KPanicCategory, EPanicBadHandle));  

            TReservationData* nextData = newDatas->iSuspendedData;
            newDatas->iCallback = aServiceCallback;
            newDatas->iPriority = priority;
            
            if ( compareMask & reservedSuspendedMask )
                {
                // Reserve suspended, i.e. go through list until next one has lower priority
                TReservationData* priorityCheckData = iReservations[j]->iSuspendedData;
                TReservationData* priorityPreviousData = iReservations[j];
                
                while ( priorityCheckData 
                        && priorityCheckData->iPriority <= priority )
                    {
                    priorityPreviousData = priorityCheckData;
                    priorityCheckData = priorityCheckData->iSuspendedData;
                    }
                    
                // insert new data
                newDatas->iSuspendedData = priorityCheckData;
                priorityPreviousData->iSuspendedData = newDatas;
                }
            else 
                {
                // Reserve as active, i.e. first in list
                newDatas->iSuspendedData = iReservations[j];
                iReservations[j] = newDatas;
                
                // Notify callback of current reserver, if any
                if ( newDatas->iSuspendedData )
                    {
                    (newDatas->iSuspendedData->iCallback)->SuspendSubResource(j);
                    }
                }

            newDatas = nextData; 
            }
            
        compareMask <<= 1;
        }

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMReservationHandler::ReserveL - return: 0x%x" ), reservedSuspendedMask );
    
    return reservedSuspendedMask;
    }


// -----------------------------------------------------------------------------
// CHWRMReservationHandler::Release
// Releases resource
// -----------------------------------------------------------------------------
//
TInt CHWRMReservationHandler::Release(CHWRMService* aServiceCallback, TInt aSubResourceMask)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMReservationHandler::Release(0x%x, 0x%x)" ), aServiceCallback, aSubResourceMask );

    __ASSERT_ALWAYS(aServiceCallback, User::Panic(KPanicCategory, EPanicBadHandle));  

    TInt unreservedMask(0x0);  // Return value, mask of all subresources that are unreserved.
    TInt subMask(aSubResourceMask);    

    // If whole resource specified, use iAllMask
    if ( aSubResourceMask == KHWRMAllSubResources)
        {
        subMask = iAllMask;
        }

    // Check reservations of all desired subtargets
    TInt compareMask(0x1);
    for ( TInt i=0; i < iSubResourceCount; i++ )
        {
        if ( compareMask & subMask )
            {
            TReservationData* releasedService = NULL;
            // Check if released client is active or suspended (or not reserved at all)
            if ( iReservations[i] && iReservations[i]->iCallback == aServiceCallback )
                {
                // Released service was active, remove it
                COMPONENT_TRACE3(_L( "HWRM Server - CHWRMReservationHandler::Release - Releasing: 0x%x, suspendedData = 0x%x" ), iReservations[i], iReservations[i]->iSuspendedData );
                releasedService = iReservations[i];
                iReservations[i] = iReservations[i]->iSuspendedData;
                delete releasedService;
                releasedService = NULL;
                
                // Reactivate first service on list if there still is one
                if ( iReservations[i] && iReservations[i]->iCallback )
                    {
                    (iReservations[i]->iCallback)->ResumeSubResource(i);
                    }
                }
            else if ( iReservations[i] )
                {
                // Released service is not active so it is either suspended or not reserved at all.
                // Find the service from suspended services
                TReservationData* checkData = iReservations[i]->iSuspendedData;
                TReservationData* previousData = iReservations[i];
                
                while ( checkData && checkData->iCallback != aServiceCallback )
                    {
                    previousData = checkData;
                    checkData = checkData->iSuspendedData;
                    }
                    
                // Reset links and delete released service. 
                // checkData is either NULL or the calling service at this point.
                // If it is NULL, no need to delete anything, the service was not reserved.
                if ( checkData )
                    {                    
                    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMReservationHandler::Release - Releasing: 0x%x, suspendedData = 0x%x" ), checkData, checkData->iSuspendedData );                
                    previousData->iSuspendedData = checkData->iSuspendedData;
                    delete checkData;
                    checkData = NULL;
                    }
                else
                    {
                    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMReservationHandler::Release - Reservation not found, releasing nothing" ) );                
                    }
                }
            else
                {
                // Nothing reserved
                }
                
            }

        // update return value if no more reservations for this subresource
        if ( !iReservations[i] )
            {
            unreservedMask |= compareMask;
            }
            
        compareMask <<= 1;            
        }

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMReservationHandler::Release - return: 0x%x" ), unreservedMask );
    
    return unreservedMask;
    }

// -----------------------------------------------------------------------------
// CHWRMReservationHandler::IsReserved
// -----------------------------------------------------------------------------
//
TBool CHWRMReservationHandler::IsReserved(const CHWRMService* aServiceCallback, TInt aSubResourceMask) const
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMReservationHandler::IsReserved(0x%x, 0x%x)" ), aServiceCallback, aSubResourceMask );

    __ASSERT_ALWAYS(aServiceCallback != NULL, User::Panic(KPanicCategory, EPanicBadHandle));  

    TBool retval(EFalse);
    TInt subMask(aSubResourceMask);    

    // If whole resource specified, use iAllMask
    if ( aSubResourceMask == KHWRMAllSubResources)
        {
        subMask = iAllMask;
        }
    
    // Check if any of the specified subresources is reserved to somebody other than caller
    TInt compareMask(0x1);
    for ( TInt i=0; i < iSubResourceCount && !retval; i++ )
        {
        if ( (compareMask & subMask) && iReservations[i] && iReservations[i]->iCallback != aServiceCallback )
            {            
            retval = ETrue;
            }
            
        compareMask <<= 1;            
        }
        

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMReservationHandler::IsReserved - return: 0x%x" ), retval );
    
    return retval;
    }


// -----------------------------------------------------------------------------
// CHWRMReservationHandler::GetUnreservedTargets
// Returns target mask containing unreserved targets.
// -----------------------------------------------------------------------------
//
TInt CHWRMReservationHandler::GetUnreservedTargets()
    {
    TInt compareMask(0x1);
    TInt returnMask(0x0);
    for ( TInt i=0; i < iSubResourceCount; i++ )
        {
        if ( !iReservations[i] )
            {            
            returnMask |= compareMask;
            }
            
        compareMask <<= 1;            
        }
        
    return returnMask;
    }
    

// -----------------------------------------------------------------------------
// CHWRMReservationHandler::UpdateActiveMask
// Active targets mask indicates targets that are active (i.e. are not stripped
// from target by target modifier plugin).
// This is obviously not realtime information, but we are only interested in
// targets that someone wants to modify anyway. I.e. if nobody wants to
// adjust the state of some target after device state modification, we do not need
// to know about it anyway. 
// -----------------------------------------------------------------------------
//
void CHWRMReservationHandler::UpdateActiveMask(TInt aOriginalTarget, TInt aModifiedTarget, CHWRMService* aUpdater)
    {
    // All modified targets are activated
    // All targets that were dropped from original target are deactivated.
    TInt newActiveMask( (iActiveMask | aModifiedTarget) & ~(aOriginalTarget & ~aModifiedTarget) );
    
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMReservationHandler::UpdateActiveMask - Updated mask: 0x%x, old mask: 0x%x" ), newActiveMask, iActiveMask );

    // Get bits that are different in old and new mask    
    TInt differenceMask( iActiveMask ^ newActiveMask );
    
    iActiveMask = newActiveMask;

    // Notify sessions with active reservation on modified targets
    if ( differenceMask )
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMReservationHandler::UpdateActiveMask - Notifying active sessions" ) );
        TInt compareMask(0x1);
        for ( TInt i=0; i < iSubResourceCount; i++ )
            {
            if ( compareMask & differenceMask )
                {
                TBool activate(compareMask & newActiveMask);
                
                // Skip activation notifications to whoever called this method as they are redundant.
                // Deactivations are notified in any case.
                if ( iReservations[i] && iReservations[i]->iCallback && (iReservations[i]->iCallback != aUpdater || !activate) )
                    {
                    (iReservations[i]->iCallback)->ActivateSubResource(i, activate);
                    }
                }
                
            compareMask <<= 1; 
            }
        }
    
    }
    
// -----------------------------------------------------------------------------
// CHWRMReservationHandler::GetActiveReserver
// Returns active reserver for the specified target
// -----------------------------------------------------------------------------
//
CHWRMService* CHWRMReservationHandler::GetActiveReserver(TInt aTarget)    
    {
    CHWRMService* retval = NULL;
    
    if ( iReservations[aTarget] )
        {
        retval = iReservations[aTarget]->iCallback;
        }
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMReservationHandler::GetActiveReserver - return: 0x%x" ), retval );
    return retval;
    }

// -----------------------------------------------------------------------------
// CHWRMReservationHandler::GetPolicy
// Returns policy instance
// -----------------------------------------------------------------------------
//
CHWRMPolicy* CHWRMReservationHandler::GetPolicy()
    {
    return iPolicy;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
