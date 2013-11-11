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
* Description:  A stub implementation of an interface for issuing Mediator Service commands.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorCommandInitiator.h"
#include    "MediatorCommandInitiatorBody.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorCommandInitiator::CMediatorCommandInitiator
// -----------------------------------------------------------------------------
//
CMediatorCommandInitiator::CMediatorCommandInitiator()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorCommandInitiator::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorCommandInitiator::ConstructL(MMediatorCommandResponseObserver* /*aObserver*/ )
    {
    }

// -----------------------------------------------------------------------------
// CMediatorCommandInitiator::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMediatorCommandInitiator* CMediatorCommandInitiator::NewL(MMediatorCommandResponseObserver* aObserver )
    {
    CMediatorCommandInitiator* self = new( ELeave ) CMediatorCommandInitiator();
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandInitiator::~CMediatorCommandInitiator
// -----------------------------------------------------------------------------
//
CMediatorCommandInitiator::~CMediatorCommandInitiator()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorCommandInitiator::IssueCommandL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorCommandInitiator::IssueCommand( TUid /*aDomain*/,
                                                       TUid /*aCategory*/, 
                                                       TInt /*aCommandId*/,
                                                       TVersion /*aVersion*/, 
                                                       const TDesC8& /*aData*/ )
    {
    return KErrNotSupported;
    }
    
// -----------------------------------------------------------------------------
// CMediatorCommandInitiator::CancelCommand
// -----------------------------------------------------------------------------
//
EXPORT_C void CMediatorCommandInitiator::CancelCommand( TUid /*aDomain*/, 
                                                        TUid /*aCategory*/, 
                                                        TInt /*aCommandId*/ )
    {
    }
