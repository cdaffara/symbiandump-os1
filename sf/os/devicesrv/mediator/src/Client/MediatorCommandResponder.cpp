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
* Description:  An interface for responding to Mediator Service commands.
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorCommandResponder.h"
#include    "MediatorCommandResponderBody.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

CMediatorCommandResponder::CMediatorCommandResponder( )
    {
    }

void CMediatorCommandResponder::ConstructL( MMediatorCommandObserver* aObserver )
    {
    iBody = CMediatorCommandResponderBody::NewL( aObserver );
    }

EXPORT_C CMediatorCommandResponder* CMediatorCommandResponder::NewL(
                MMediatorCommandObserver* aObserver )
    {
    CMediatorCommandResponder* self 
            = new( ELeave ) CMediatorCommandResponder( );
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );

    return self;
    }
    
CMediatorCommandResponder::~CMediatorCommandResponder()
    {
    if ( iBody )
        {
        iBody->Cancel();
        }
    delete iBody;
    }

    
// -----------------------------------------------------------------------------
// CMediatorCommandResponder::RegisterCommandL
// Register a command category.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorCommandResponder::RegisterCommand( 
                                        TUid aDomain,
                                        TUid aCategory, 
                                        const RCommandList& aCommands )
    {
    return iBody->RegisterCommand( aDomain, aCategory, aCommands );
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::RegisterCommandL
// Register a command.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt CMediatorCommandResponder::RegisterCommand( TUid aDomain, 
                                                          TUid aCategory, 
                                                          TInt aCommandId, 
                                                          TVersion aVersion,
                                                          TCapabilitySet aCaps, 
                                                          TInt aTimeOut )
    {
    return iBody->RegisterCommand( aDomain, 
                                   aCategory, 
                                   aCommandId, 
                                   aVersion, 
                                   aCaps, 
                                   aTimeOut );
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::UnregisterCommand
// Unregister a command list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt CMediatorCommandResponder::UnregisterCommand( 
                                            TUid aDomain, 
                                            TUid aCategory,
                                            const RCommandList& aCommands )
    {
    return iBody->UnregisterCommand( aDomain, aCategory, aCommands );
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::UnregisterCommand
// Unregister a command.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt CMediatorCommandResponder::UnregisterCommand( 
                                         TUid aDomain, 
                                         TUid aCategory, 
                                         TInt aCommandId )
    {
    return iBody->UnregisterCommand( aDomain, aCategory, aCommandId );
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::IssueResponseL
// Issue a response to a command.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt CMediatorCommandResponder::IssueResponse( TUid aDomain,
                                                        TUid aCategory, 
                                                        TInt aCommandId,
                                                        TInt aStatus, 
                                                        const TDesC8& aData )
    {
    return iBody->IssueResponse( aDomain, aCategory, aCommandId, aStatus, aData );
    }

//  End of File  
