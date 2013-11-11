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
* Description:  An interface for receiving command/event notifications and info.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorNotifications.h"
#include    "MediatorNotificationsBody.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

CMediatorNotifications::CMediatorNotifications()
     
    {
    }

void CMediatorNotifications::ConstructL()
    {
    LOG(_L("[Mediator Server]\t CMediatorNotifications::ConstructL\n"));
    iBody = CMediatorNotificationsBody::NewL();
    }

EXPORT_C CMediatorNotifications* CMediatorNotifications::NewL()
    {
    CMediatorNotifications* self = new( ELeave ) CMediatorNotifications;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
CMediatorNotifications::~CMediatorNotifications()
    {
    if ( iBody )
        {
        iBody->Cancel();
        }
    delete iBody;
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::RegisterNotificationObserver
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::RegisterNotificationObserver( 
            MMediatorNotifications* aObserver )
    {
    return iBody->RegisterNotificationObserver( aObserver );
    }
            
// -----------------------------------------------------------------------------
// CMediatorNotifications::UnregisterNotificationObserver
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::UnregisterNotificationObserver()
    {
    return iBody->UnregisterNotificationObserver();
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::GetDomains
// Gets a list of registered domains
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::GetDomains( RDomainList& aDomains )
    {
    return iBody->GetDomains( aDomains );
    }
    

// -----------------------------------------------------------------------------
// CMediatorNotifications::GetCategories
// Gets a list of registered domains
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::GetCategories( TUid aDomain, 
                              RCategoryList& aCategories )
    {
    return iBody->GetCategories( aDomain, aCategories );
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::GetEvents
// Gets a list of registered events withing the domain/category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::GetEvents( TUid aDomain, 
                          TUid aCategory,
                          REventList& aEvents )
    {
    return iBody->GetEvents( aDomain, aCategory, aEvents );
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::GetCommands
// Gets a list of registered commands withing the domain/category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::GetCommands( TUid aDomain, 
                            TUid aCategory,
                            RCommandList& aCommands )
    {
    return iBody->GetCommands( aDomain, aCategory, aCommands );
    }

//  End of File  
