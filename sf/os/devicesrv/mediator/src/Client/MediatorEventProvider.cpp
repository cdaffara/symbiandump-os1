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
* Description:  An interface for raising Mediator Service events.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorEventProvider.h"
#include    "MediatorEventProviderBody.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

CMediatorEventProvider::CMediatorEventProvider()
    {
    }

void CMediatorEventProvider::ConstructL()
    {
    iBody = CMediatorEventProviderBody::NewL();
    }

EXPORT_C CMediatorEventProvider* CMediatorEventProvider::NewL()
    {
    CMediatorEventProvider* self = new( ELeave ) CMediatorEventProvider;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
CMediatorEventProvider::~CMediatorEventProvider()
    {
    delete iBody;
    }

// -----------------------------------------------------------------------------
// CMediatorEventProvider::RegisterEventL
// Registers complete event list using Mediator Server API
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventProvider::RegisterEvent( TUid aDomain,
                                                     TUid aCategory, 
                                                     const REventList& aEvents )
    {
    return iBody->RegisterEvent( aDomain, aCategory, aEvents );                                   
    }

// -----------------------------------------------------------------------------
// CMediatorEventProvider::RegisterEventL
// Registers a single event. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventProvider::RegisterEvent( TUid aDomain,
                                                     TUid aCategory,
                                                     TInt aEventId, 
                                                     TVersion aVersion,
                                                     TCapabilitySet aCaps )
    {
    return iBody->RegisterEvent( aDomain, aCategory, aEventId, aVersion, aCaps ); 
    }   

// -----------------------------------------------------------------------------
// CMediatorEventProvider::UnregisterEvent
// Unregisters a list of events.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventProvider::UnregisterEvent( TUid aDomain,
                                                       TUid aCategory,
                                                       const REventList& aEvents )
    {
    return iBody->UnregisterEvent( aDomain, aCategory, aEvents );                               
    }
       
// -----------------------------------------------------------------------------
// CMediatorEventProvider::UnregisterEvent
// Unregisters a single event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventProvider::UnregisterEvent( TUid aDomain,
                                                       TUid aCategory, 
                                                       TInt aEventId )
    {
    return iBody->UnregisterEvent( aDomain, aCategory, aEventId );
    }
       
// -----------------------------------------------------------------------------
// CMediatorEventProvider::RaiseEvent
// Raises event with parameter data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventProvider::RaiseEvent( TUid aDomain,
                                                  TUid aCategory, 
                                                  TInt aEventId,
                                                  TVersion aVersion,
                                                  const TDesC8& aData )
    {
    return iBody->RaiseEvent( aDomain, aCategory, aEventId, aVersion, aData );
    }


//  End of File  
