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
* Description:  A stub implementation of an interface for raising Mediator Service events.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorEventProvider.h"
#include    "MediatorEventProviderBody.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CMediatorEventProvider::CMediatorEventProvider
// -----------------------------------------------------------------------------
//
CMediatorEventProvider::CMediatorEventProvider()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorEventProvider::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorEventProvider::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorEventProvider::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMediatorEventProvider* CMediatorEventProvider::NewL()
    {
    CMediatorEventProvider* self = new( ELeave ) CMediatorEventProvider;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMediatorEventProvider::~CMediatorEventProvider
// -----------------------------------------------------------------------------
//    
CMediatorEventProvider::~CMediatorEventProvider()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorEventProvider::RegisterEvent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventProvider::RegisterEvent( TUid /*aDomain*/,
                                                     TUid /*aCategory*/, 
                                                     const REventList& /*aEvents*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMediatorEventProvider::RegisterEvent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventProvider::RegisterEvent( TUid /*aDomain*/,
                                                     TUid /*aCategory*/,
                                                     TInt /*aEventId*/, 
                                                     TVersion /*aVersion*/,
                                                     TCapabilitySet /*aCaps*/ )
    {
    return KErrNotSupported;
    }   

// -----------------------------------------------------------------------------
// CMediatorEventProvider::UnregisterEvent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventProvider::UnregisterEvent( TUid /*aDomain*/,
                                                       TUid /*aCategory*/,
                                                       const REventList& /*aEvents*/ )
    {
    return KErrNotSupported;
    }
       
// -----------------------------------------------------------------------------
// CMediatorEventProvider::UnregisterEvent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventProvider::UnregisterEvent( TUid /*aDomain*/,
                                                       TUid /*aCategory*/, 
                                                       TInt /*aEventId*/ )
    {
    return KErrNotSupported;
    }
       
// -----------------------------------------------------------------------------
// CMediatorEventProvider::RaiseEvent
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorEventProvider::RaiseEvent( TUid /*aDomain*/,
                                                  TUid /*aCategory*/, 
                                                  TInt /*aEventId*/,
                                                  TVersion /*aVersion*/,
                                                  const TDesC8& /*aData*/ )
    {
    return KErrNotSupported;
    }
