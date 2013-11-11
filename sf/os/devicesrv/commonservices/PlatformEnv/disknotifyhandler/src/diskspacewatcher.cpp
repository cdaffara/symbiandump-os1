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
* Description:  Class for disk space watcher
*
*/


//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include "diskspacewatcher.h"
#include "disknotifyhandler.h"
#include "disknotifyhandlerdebug.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// SetEvent
// ---------------------------------------------------------------------------
//
inline static void SetEvent(
        MDiskNotifyHandlerCallback::TDiskSpaceEvent& aEvent,
        TInt aDrive,
        const TInt64& aThreshold )
    {
    aEvent.iDrive = aDrive;
    aEvent.iThreshold = aThreshold;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::CDiskSpaceWatcher
// ---------------------------------------------------------------------------
//
CDiskSpaceWatcher::CDiskSpaceWatcher(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CDiskSpaceWatcher >& aWatcherList,
        TInt aDrive,
        const TInt64& aThreshold ) :
    CDiskWatcherBase( aCallback, aFs ),
    iWatcherList( aWatcherList ),
    iDrive( aDrive ),
    iThreshold( aThreshold )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::ConstructL
// ---------------------------------------------------------------------------
//
void CDiskSpaceWatcher::ConstructL()
    {
    FUNC_LOG

    iWatcherList.InsertInOrderL( this, CompareFindObject );
    Activate();
    }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::NewL
// ---------------------------------------------------------------------------
//
CDiskSpaceWatcher* CDiskSpaceWatcher::NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CDiskSpaceWatcher >& aWatcherList,
        TInt aDrive,
        const TInt64& aThreshold )
    {
    FUNC_LOG

    CDiskSpaceWatcher* self = new ( ELeave ) CDiskSpaceWatcher(
        aCallback, aFs, aWatcherList, aDrive, aThreshold );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::~CDiskSpaceWatcher
// ---------------------------------------------------------------------------
//
CDiskSpaceWatcher::~CDiskSpaceWatcher()
    {
    FUNC_LOG

    Cancel();
    }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::CompareFind
// ---------------------------------------------------------------------------
//
TInt CDiskSpaceWatcher::CompareFind(
        const TInt* aDrive,
        const CDiskSpaceWatcher& aWatcher )
    {
    return Compare( *aDrive, aWatcher.iDrive );
    }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::CompareFindData
// ---------------------------------------------------------------------------
//
TInt CDiskSpaceWatcher::CompareFindData(
        const TFindData* aData,
        const CDiskSpaceWatcher& aWatcher )
    {
    TInt ret( Compare( aData->iDrive, aWatcher.iDrive ) );
    if ( !ret )
        {
        ret = Compare( aData->iThreshold, aWatcher.iThreshold );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::CompareFindObject
// ---------------------------------------------------------------------------
//
TInt CDiskSpaceWatcher::CompareFindObject(
        const CDiskSpaceWatcher& aWatcher1,
        const CDiskSpaceWatcher& aWatcher2 )
   {
    TInt ret( Compare( aWatcher1.iDrive, aWatcher2.iDrive ) );
    if ( !ret )
        {
        ret = Compare( aWatcher1.iThreshold, aWatcher2.iThreshold );
        }
    return ret;
   }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::WatcherError
// ---------------------------------------------------------------------------
//
void CDiskSpaceWatcher::WatcherError(
        TInt aError,
        const TBool& aKilled )
    {
    FUNC_LOG

    LOG_IF_ERROR3(
        aError,
        "CDiskSpaceWatcher::WatcherError-iDrive=%d,iThreshold=%Ld,aError=%d",
        iDrive, iThreshold, aError )

    MDiskNotifyHandlerCallback::TDiskSpaceEvent event;
    SetEvent( event, iDrive, iThreshold );
    Callback().HandleNotifyDiskSpace( aError, event );
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
// CDiskSpaceWatcher::ReactivateWatcher
// ---------------------------------------------------------------------------
//
void CDiskSpaceWatcher::ReactivateWatcher()
    {
    FUNC_LOG

    Activate();
    }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::RunWatcher
// ---------------------------------------------------------------------------
//
void CDiskSpaceWatcher::RunWatcher( const TBool& /*aKilled*/ )
    {
    FUNC_LOG
    
    INFO_LOG2(
        "CDiskSpaceWatcher::RunWatcher-iThreshold=%Ld,iDrive=%d",
        iThreshold, iDrive )

    MDiskNotifyHandlerCallback::TDiskSpaceEvent event;
    SetEvent( event, iDrive, iThreshold );
    Callback().HandleNotifyDiskSpace( KErrNone, event );
    }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::CancelWatcher
// ---------------------------------------------------------------------------
//
void CDiskSpaceWatcher::CancelWatcher()
    {
    FUNC_LOG

    INFO_LOG2(
        "CDiskSpaceWatcher::CancelWatcher-iThreshold=%Ld,iDrive=%d",
        iThreshold, iDrive )

    Fs().NotifyDiskSpaceCancel( iStatus );
    }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::Activate
// ---------------------------------------------------------------------------
//
void CDiskSpaceWatcher::Activate()
    {
    FUNC_LOG

    if( !IsActive() )
        {
        INFO_LOG2(
            "CDiskSpaceWatcher::Activate-iThreshold=%Ld,iDrive=%d",
            iThreshold, iDrive )
        Fs().NotifyDiskSpace( iThreshold, iDrive, iStatus );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CDiskSpaceWatcher::RemoveFromListAndDestroy
// ---------------------------------------------------------------------------
//
void CDiskSpaceWatcher::RemoveFromListAndDestroy()
    {
    FUNC_LOG

    TInt i( iWatcherList.FindInOrder( this, CompareFindObject ) );
    if ( i >= 0 && i < iWatcherList.Count() )
        {
        INFO_LOG3(
            "CDiskSpaceWatcher::RemoveFromListAndDestroy-iThreshold=%Ld,iDrive=%d,i=%d",
            iThreshold, iDrive, i )
        iWatcherList.Remove( i );
        delete this;
        }
    }

// End of File
