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
* Description:  A stub implementation of an interface for responding to Mediator Service commands.
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorCommandResponder.h"
#include    "MediatorCommandResponderBody.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::CMediatorCommandResponder
// -----------------------------------------------------------------------------
//
CMediatorCommandResponder::CMediatorCommandResponder( )
    {
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorCommandResponder::ConstructL( MMediatorCommandObserver* /*aObserver*/ )
    {
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMediatorCommandResponder* CMediatorCommandResponder::NewL( MMediatorCommandObserver* aObserver )
    {
    CMediatorCommandResponder* self = new( ELeave ) CMediatorCommandResponder( );
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::~CMediatorCommandResponder
// -----------------------------------------------------------------------------
//
CMediatorCommandResponder::~CMediatorCommandResponder()
    {
    }

    
// -----------------------------------------------------------------------------
// CMediatorCommandResponder::RegisterCommand
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorCommandResponder::RegisterCommand( 
                                        TUid /*aDomain*/,
                                        TUid /*aCategory*/, 
                                        const RCommandList& /*aCommands*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::RegisterCommand
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt CMediatorCommandResponder::RegisterCommand( TUid /*aDomain*/, 
                                                          TUid /*aCategory*/, 
                                                          TInt /*aCommandId*/, 
                                                          TVersion /*aVersion*/,
                                                          TCapabilitySet /*aCaps*/, 
                                                          TInt /*aTimeOut*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::UnregisterCommand
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt CMediatorCommandResponder::UnregisterCommand( 
                                            TUid /*aDomain*/, 
                                            TUid /*aCategory*/,
                                            const RCommandList& /*aCommands*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::UnregisterCommand
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt CMediatorCommandResponder::UnregisterCommand( 
                                         TUid /*aDomain*/, 
                                         TUid /*aCategory*/, 
                                         TInt /*aCommandId*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::IssueResponse
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt CMediatorCommandResponder::IssueResponse( TUid /*aDomain*/,
                                                        TUid /*aCategory*/, 
                                                        TInt /*aCommandId*/,
                                                        TInt /*aStatus*/, 
                                                        const TDesC8& /*aData*/ )
    {
    return KErrNotSupported;
    }
