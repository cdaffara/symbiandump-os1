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
* Description:  An interface for receiving Mediator events.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorEventConsumer.h"
#include    "MediatorEventConsumerBody.h"


// ============================ MEMBER FUNCTIONS ===============================

CMediatorEventConsumer::CMediatorEventConsumer()
    {
    }

void CMediatorEventConsumer::ConstructL( MMediatorEventObserver* aObserver )
    {
    iBody = CMediatorEventConsumerBody::NewL( aObserver ); 
    }

EXPORT_C CMediatorEventConsumer* CMediatorEventConsumer::NewL(
                        MMediatorEventObserver* aObserver )
    {
    CMediatorEventConsumer* self = new( ELeave ) CMediatorEventConsumer();
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );

    return self;
    }
    
CMediatorEventConsumer::~CMediatorEventConsumer()
    {
    if ( iBody )
        {
        iBody->Cancel();
        }
    delete iBody;
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumer::SubscribeEventL
// Subscribe to event category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventConsumer::SubscribeEvent( TUid aDomain,
                                                      TUid aCategory,
                                                      const REventList& aEvents )
    {
    return iBody->SubscribeEvent( aDomain, aCategory, aEvents );
    }
    
// -----------------------------------------------------------------------------
// CMediatorEventConsumer::SubscribeEventL
// Subscribe to event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventConsumer::SubscribeEvent( TUid aDomain,
                                                      TUid aCategory, 
                                                      TInt aEventId,
                                                      TVersion aVersion )
    {
    return iBody->SubscribeEvent( aDomain, aCategory, aEventId, aVersion );
    }
    
// -----------------------------------------------------------------------------
// CMediatorEventConsumer::UnsubscribeEvent
// Unsubscibe an event category.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventConsumer::UnsubscribeEvent( TUid aDomain,
                                                        TUid aCategory,
                                                        const REventList& aEvents )
    {
    return iBody->UnsubscribeEvent( aDomain, aCategory, aEvents );
    }
    
// -----------------------------------------------------------------------------
// CMediatorEventConsumer::UnsubscribeEvent
// Unsubscibe an event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventConsumer::UnsubscribeEvent( TUid aDomain,
                                                        TUid aCategory, 
                                                        TInt aEventId )
    {
    return iBody->UnsubscribeEvent( aDomain, aCategory, aEventId );
    }

//  End of File  
