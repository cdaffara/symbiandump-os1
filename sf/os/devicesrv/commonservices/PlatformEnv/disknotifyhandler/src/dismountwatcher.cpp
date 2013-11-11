/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for dismount watcher
*
*/


//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include "dismountwatcher.h"
#include "disknotifyhandler.h"
#include "disknotifyhandlerdebug.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// SetEvent
// ---------------------------------------------------------------------------
//
inline static void SetEvent(
        MDiskNotifyHandlerCallback::TDismountEvent& aEvent,
        TInt aDrive )
    {
    aEvent.iDrive = aDrive;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDismountWatcher::CDismountWatcher
// ---------------------------------------------------------------------------
//
CDismountWatcher::CDismountWatcher(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CDismountWatcher >& aWatcherList,
        TInt aDrive ) :
    CDiskWatcherBase( aCallback, aFs ),
    iWatcherList( aWatcherList ),
    iDrive( aDrive )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::ConstructL
// ---------------------------------------------------------------------------
//
void CDismountWatcher::ConstructL()
    {
    FUNC_LOG

    iWatcherList.InsertInOrderL( this, CompareFindObject );
    Activate();
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::NewL
// ---------------------------------------------------------------------------
//
CDismountWatcher* CDismountWatcher::NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CDismountWatcher >& aWatcherList,
        TInt aDrive )
    {
    FUNC_LOG

    CDismountWatcher* self = new ( ELeave ) CDismountWatcher(
        aCallback, aFs, aWatcherList, aDrive );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::~CDismountWatcher
// ---------------------------------------------------------------------------
//
CDismountWatcher::~CDismountWatcher()
    {
    FUNC_LOG

    Cancel();
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::CompareFind
// ---------------------------------------------------------------------------
//
TInt CDismountWatcher::CompareFind(
        const TInt* aDrive,
        const CDismountWatcher& aWatcher )
    {
    return Compare( *aDrive, aWatcher.iDrive );
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::CompareFindObject
// ---------------------------------------------------------------------------
//
TInt CDismountWatcher::CompareFindObject(
        const CDismountWatcher& aWatcher1,
        const CDismountWatcher& aWatcher2 )
    {
    return Compare( aWatcher1.iDrive, aWatcher2.iDrive );
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::WatcherError
// ---------------------------------------------------------------------------
//
void CDismountWatcher::WatcherError(
        TInt aError,
        const TBool& aKilled )
    {
    FUNC_LOG

    LOG_IF_ERROR2(
        aError,
        "CDismountWatcher::WatcherError-iDrive=%d,aError=%d",
        iDrive, aError )

    MDiskNotifyHandlerCallback::TDismountEvent event;
    SetEvent( event, iDrive );
    Callback().HandleNotifyDismount( aError, event );
    if ( aKilled )
        {
        return;
        }
    // Remove useless watcher if not manually activated from callback
    if ( !IsActive() )
        {
        RemoveFromListAndDestroy();
        }
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::ReactivateWatcher
// ---------------------------------------------------------------------------
//
void CDismountWatcher::ReactivateWatcher()
    {
    FUNC_LOG

    // Automatic reactivation is not required
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::RunWatcher
// ---------------------------------------------------------------------------
//
void CDismountWatcher::RunWatcher( const TBool& aKilled )
    {
    FUNC_LOG

    INFO_LOG1( "CDismountWatcher::RunWatcher-iDrive=%d", iDrive )

    iWaitConfirm = ETrue;
    MDiskNotifyHandlerCallback::TDismountEvent event;
    SetEvent( event, iDrive );
    Callback().HandleNotifyDismount( KErrNone, event );
    if ( aKilled )
        {
        return;
        }
    iWaitConfirm = EFalse;
    // Remove useless watcher if not manually activated from callback
    if ( !IsActive() )
        {
        RemoveFromListAndDestroy();
        }
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::CancelWatcher
// ---------------------------------------------------------------------------
//
void CDismountWatcher::CancelWatcher()
    {
    FUNC_LOG

    INFO_LOG1( "CDismountWatcher::CancelWatcher-iDrive=%d", iDrive )

    Fs().NotifyDismountCancel( iStatus );
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::Activate
// ---------------------------------------------------------------------------
//
void CDismountWatcher::Activate()
    {
    FUNC_LOG

    if ( !IsActive() )
        {
        INFO_LOG1( "CDismountWatcher::Activate-iDrive=%d", iDrive )
        Fs().NotifyDismount( iDrive, iStatus, EFsDismountRegisterClient );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::AllowDismount
// ---------------------------------------------------------------------------
//
TInt CDismountWatcher::AllowDismount()
    {
    FUNC_LOG

    INFO_LOG2(
        "CDismountWatcher::AllowDismount-iDrive=%d,iWaitConfirm=%d",
        iDrive, iWaitConfirm )

    if ( !iWaitConfirm )
        {
        return KErrNone;
        }

    TInt ret( Fs().AllowDismount( iDrive ) );
    LOG_IF_ERROR2(
        ret,
        "CDismountWatcher::AllowDismount-iDrive=%d,ret=%d",
        iDrive, ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// CDismountWatcher::RemoveFromListAndDestroy
// ---------------------------------------------------------------------------
//
void CDismountWatcher::RemoveFromListAndDestroy()
    {
    FUNC_LOG

    TInt i( iWatcherList.FindInOrder( this, CompareFindObject ) );
    if ( i >= 0 && i < iWatcherList.Count() )
        {
        INFO_LOG2(
            "CDismountWatcher::RemoveFromListAndDestroy-iDrive=%d,i=%d",
            iDrive, i )
        iWatcherList.Remove( i );
        delete this;
        }
    }

// End of File
