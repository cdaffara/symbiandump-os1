/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A stub implementation of an interface for receiving Mediator events.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorEventConsumer.h"
#include    "MediatorEventConsumerBody.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorEventConsumer::CMediatorEventConsumer
// -----------------------------------------------------------------------------
//
CMediatorEventConsumer::CMediatorEventConsumer()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumer::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorEventConsumer::ConstructL( MMediatorEventObserver* /*aObserver*/ )
    {
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumer::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMediatorEventConsumer* CMediatorEventConsumer::NewL( MMediatorEventObserver* aObserver )
    {
    CMediatorEventConsumer* self = new( ELeave ) CMediatorEventConsumer();
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumer::~CMediatorEventConsumer
// -----------------------------------------------------------------------------
//    
CMediatorEventConsumer::~CMediatorEventConsumer()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumer::SubscribeEvent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventConsumer::SubscribeEvent( TUid /*aDomain*/,
                                                      TUid /*aCategory*/,
                                                      const REventList& /*aEvents*/ )
    {
    return KErrNotSupported;
    }
    
// -----------------------------------------------------------------------------
// CMediatorEventConsumer::SubscribeEvent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventConsumer::SubscribeEvent( TUid /*aDomain*/,
                                                      TUid /*aCategory*/, 
                                                      TInt /*aEventId*/,
                                                      TVersion /*aVersion*/ )
    {
    return KErrNotSupported;
    }
    
// -----------------------------------------------------------------------------
// CMediatorEventConsumer::UnsubscribeEvent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventConsumer::UnsubscribeEvent( TUid /*aDomain*/,
                                                        TUid /*aCategory*/,
                                                        const REventList& /*aEvents*/ )
    {
    return KErrNotSupported;
    }
    
// -----------------------------------------------------------------------------
// CMediatorEventConsumer::UnsubscribeEvent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventConsumer::UnsubscribeEvent( TUid /*aDomain*/,
                                                        TUid /*aCategory*/, 
                                                        TInt /*aEventId*/ )
    {
    return KErrNotSupported;
    }
