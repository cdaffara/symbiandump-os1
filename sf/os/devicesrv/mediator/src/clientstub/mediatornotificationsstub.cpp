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
* Description:  A stub implementation of an interface for receiving command/event notifications and info.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorNotifications.h"
#include    "MediatorNotificationsBody.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorNotifications::CMediatorNotifications
// -----------------------------------------------------------------------------
//
CMediatorNotifications::CMediatorNotifications()
     
    {
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorNotifications::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMediatorNotifications* CMediatorNotifications::NewL()
    {
    CMediatorNotifications* self = new( ELeave ) CMediatorNotifications;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::~CMediatorNotifications
// -----------------------------------------------------------------------------
//
CMediatorNotifications::~CMediatorNotifications()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::RegisterNotificationObserver
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::RegisterNotificationObserver( MMediatorNotifications* /*aObserver*/ )
    {
    return KErrNotSupported;
    }
            
// -----------------------------------------------------------------------------
// CMediatorNotifications::UnregisterNotificationObserver
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::UnregisterNotificationObserver()
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::GetDomains
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::GetDomains( RDomainList& /*aDomains*/ )
    {
    return KErrNotSupported;
    }
    

// -----------------------------------------------------------------------------
// CMediatorNotifications::GetCategories
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::GetCategories( TUid /*aDomain*/, 
                                                     RCategoryList& /*aCategories*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::GetEvents
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::GetEvents( TUid /*aDomain*/, 
                                                 TUid /*aCategory*/,
                                                 REventList& /*aEvents*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMediatorNotifications::GetCommands
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMediatorNotifications::GetCommands( TUid /*aDomain*/, 
                                                   TUid /*aCategory*/,
                                                   RCommandList& /*aCommands*/ )
    {
    return KErrNotSupported;
    }
