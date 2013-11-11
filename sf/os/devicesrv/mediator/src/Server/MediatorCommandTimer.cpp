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
* Description:  Command expiration timer implementation
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorCommandTimer.h"
#include "Debug.h"

// CONSTANTS
const TInt KMilliSeconds = 1000;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorCommandTimer::CMediatorCommandTimer
// -----------------------------------------------------------------------------
//
CMediatorCommandTimer::CMediatorCommandTimer()
    : CTimer( EPriorityNormal )
    {
    }

// -----------------------------------------------------------------------------
// CMediatorCommandTimer::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorCommandTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
              
    }

// -----------------------------------------------------------------------------
// CMediatorCommandTimer::NewL
// -----------------------------------------------------------------------------
//
CMediatorCommandTimer* CMediatorCommandTimer::NewL()
    {
    CMediatorCommandTimer* self = new( ELeave ) CMediatorCommandTimer();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// -----------------------------------------------------------------------------
// CMediatorCommandTimer::~CMediatorCommandTimer
// -----------------------------------------------------------------------------
//
CMediatorCommandTimer::~CMediatorCommandTimer()
    {
    Cancel();
    iCallBack = NULL;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandTimer::RunL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorCommandTimer::RunL()
    {       
    if ( iCallBack )
        {
        iCallBack->TimerCallBack( iDomain, iCategory, iCommandId );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorCommandTimer::StartTimer
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorCommandTimer::StartTimer( MMediatorTimerCallback* aCallBack,
                                        TUid aDomain, 
                                        TUid aCategory, 
                                        TInt aCommandId, 
                                        TInt aInterval )
    {
    TInt error = KErrNone;
    if ( IsActive() )
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorCommandTimer::StartTimer: failure, command %d in category %d of domain %d\n"), aCommandId,
                                                                                                                                 aCategory.iUid,
                                                                                                                                 aDomain.iUid ));
        error = KErrInUse;
        }
    else
        {
        iCallBack = aCallBack;
        iDomain = aDomain;
        iCategory = aCategory;
        iCommandId = aCommandId;
        CTimer::After( aInterval * KMilliSeconds ); 
        }
    return error;
    }

//  End of File  
