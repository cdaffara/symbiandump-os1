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
* Description:  An implementation class for raising Mediator Service events.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorEventProviderBody.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

CMediatorEventProviderBody::CMediatorEventProviderBody()
    {
    }

void CMediatorEventProviderBody::ConstructL()
    {
    User::LeaveIfError( iMediatorServer.Connect() );
    }

CMediatorEventProviderBody* CMediatorEventProviderBody::NewL()
    {
    CMediatorEventProviderBody* self = new( ELeave ) CMediatorEventProviderBody;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
CMediatorEventProviderBody::~CMediatorEventProviderBody()
    {
    iMediatorServer.Close();
    }

// -----------------------------------------------------------------------------
// CMediatorEventProviderBody::RegisterEventL
// Registers complete event list using Mediator Server API
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorEventProviderBody::RegisterEvent( TUid aDomain,
                                                TUid aCategory, 
                                                const REventList& aEvents )
    {
    LOG(_L("[Mediator Server]\t CMediatorEventProviderBody::RegisterEvent list\n")); 
    
    return iMediatorServer.RegisterEventList( aDomain,
                                              aCategory,
                                              aEvents );                                     
    }

// -----------------------------------------------------------------------------
// CMediatorEventProviderBody::RegisterEventL
// Registers a single event. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorEventProviderBody::RegisterEvent( TUid aDomain,
                                                TUid aCategory,
                                                TInt aEventId, 
                                                TVersion aVersion,
                                                TCapabilitySet aCaps )
    {
    LOG(_L("[Mediator Server]\t CMediatorEventProviderBody::RegisterEventL\n"));
    __UHEAP_MARK; 
    
    // Create a list to contain one event
    REventList eventList;
    TEvent newEvent;
    newEvent.iCaps      = aCaps;
    newEvent.iEventId   = aEventId;
    newEvent.iVersion   = aVersion;
    TInt error = eventList.Append( newEvent );
        
    if ( !error )
        {
        // Register event
        error = iMediatorServer.RegisterEventList( aDomain,
                                                   aCategory,
                                                   eventList );
        }
    else
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorEventProviderBody::RegisterEvent: error=%d\n"), error ) );
        }
        
    // Clean up                                        
    eventList.Reset();
    eventList.Close();
    
    __UHEAP_MARKEND;
    return error;    
    }   

// -----------------------------------------------------------------------------
// CMediatorEventProviderBody::UnregisterEvent
// Unregisters a list of events.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorEventProviderBody::UnregisterEvent( TUid aDomain,
                                                       TUid aCategory,
                                                       const REventList& aEvents )
    {
    LOG(_L("[Mediator Server]\t CMediatorEventProviderBody::UnregisterEvent\n")); 
    
    // Unegister eventlist
    return iMediatorServer.UnregisterEventList( aDomain,
                                                aCategory,
                                                aEvents );                                         
    }
       
// -----------------------------------------------------------------------------
// CMediatorEventProviderBody::UnregisterEvent
// Unregisters a single event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorEventProviderBody::UnregisterEvent( TUid aDomain,
                                                  TUid aCategory, 
                                                  TInt aEventId )
    {
    LOG(_L("[Mediator Server]\t CMediatorEventProviderBody::UnregisterEvent\n"));
    __UHEAP_MARK;
    
    // Create a list to contain one event
    REventList eventList;
    TEvent newEvent;
    newEvent.iEventId = aEventId;
    TInt error = eventList.Append( newEvent );
        
    if ( !error )
        {
        // Unregister eventlist
        error = iMediatorServer.UnregisterEventList( aDomain,
                                                     aCategory,
                                                     eventList );
        }
    else
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorEventProviderBody::UnregisterEvent: error=%d\n"), error ) );
        }
                
    // Clean up                                        
    eventList.Reset();
    eventList.Close();
    
    __UHEAP_MARKEND;
    return error;
    }
       
// -----------------------------------------------------------------------------
// CMediatorEventProviderBody::RaiseEvent
// Raises event with parameter data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorEventProviderBody::RaiseEvent( TUid aDomain,
                                                  TUid aCategory, 
                                                  TInt aEventId,
                                                  TVersion aVersion,
                                                  const TDesC8& aData )
    {
    LOG(_L("[Mediator Server]\t CMediatorEventProviderBody::RaiseEvent\n")); 
    __UHEAP_MARK;
    
    TInt error = iMediatorServer.RaiseEvent( aDomain, 
                                            aCategory, 
                                            aEventId, 
                                            aVersion, 
                                            aData );
    __UHEAP_MARKEND;
    return error;
    }


//  End of File  
