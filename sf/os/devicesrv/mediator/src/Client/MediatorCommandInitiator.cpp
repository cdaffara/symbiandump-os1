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
* Description:  An interface for issuing Mediator Service commands.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorCommandInitiator.h"
#include    "MediatorCommandInitiatorBody.h"


// ============================ MEMBER FUNCTIONS ===============================

CMediatorCommandInitiator::CMediatorCommandInitiator()
    {
    }

void CMediatorCommandInitiator::ConstructL(
        MMediatorCommandResponseObserver* aObserver )
    {
    iBody = CMediatorCommandInitiatorBody::NewL( aObserver );
    }

EXPORT_C CMediatorCommandInitiator* CMediatorCommandInitiator::NewL(
                MMediatorCommandResponseObserver* aObserver )
    {
    CMediatorCommandInitiator* self 
                = new( ELeave ) CMediatorCommandInitiator();
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );

    return self;
    }
    
CMediatorCommandInitiator::~CMediatorCommandInitiator()
    {
    if ( iBody )
        {
        iBody->Cancel();
        }
    delete iBody;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandInitiator::IssueCommandL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorCommandInitiator::IssueCommand( TUid aDomain,
                                                       TUid aCategory, 
                                                       TInt aCommandId,
                                                       TVersion aVersion, 
                                                       const TDesC8& aData )
    {
    return iBody->IssueCommand( aDomain, aCategory, aCommandId, aVersion, aData );
    }
// -----------------------------------------------------------------------------
// CMediatorCommandInitiator::CancelCommand
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMediatorCommandInitiator::CancelCommand( TUid aDomain, 
                                                        TUid aCategory, 
                                                        TInt aCommandId )
    {
    iBody->CancelCommand( aDomain, aCategory, aCommandId );
    }

//  End of File  
