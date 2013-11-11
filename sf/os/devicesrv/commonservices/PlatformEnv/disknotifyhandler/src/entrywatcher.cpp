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
* Description:  Class for entry watcher
*
*/


//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include "entrywatcher.h"
#include "disknotifyhandler.h"
#include "disknotifyhandlerdebug.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// SetEvent
// ---------------------------------------------------------------------------
//
inline static void SetEvent(
        MDiskNotifyHandlerCallback::TEntryEvent& aEvent,
        const TDesC& aEntry,
        TNotifyType aType )
    {
    aEvent.iEntry = aEntry;
    aEvent.iType = aType;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CEntryWatcher::CEntryWatcher
// ---------------------------------------------------------------------------
//
CEntryWatcher::CEntryWatcher(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CEntryWatcher >& aWatcherList,
        TNotifyType aType ) :
    CDiskWatcherBase( aCallback, aFs ),
    iWatcherList( aWatcherList ),
    iType( aType )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// CEntryWatcher::ConstructL
// ---------------------------------------------------------------------------
//
void CEntryWatcher::ConstructL( const TDesC& aEntry )
    {
    FUNC_LOG

    iEntry = aEntry.AllocL();
    iWatcherList.InsertInOrderL( this, CompareFindObject );
    Activate();
    }

// ---------------------------------------------------------------------------
// CEntryWatcher::NewL
// ---------------------------------------------------------------------------
//
CEntryWatcher* CEntryWatcher::NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CEntryWatcher >& aWatcherList,
        const TDesC& aEntry,
        TNotifyType aType )
    {
    FUNC_LOG

    CEntryWatcher* self = new ( ELeave ) CEntryWatcher(
        aCallback, aFs, aWatcherList, aType );
    CleanupStack::PushL( self );
    self->ConstructL( aEntry );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CEntryWatcher::~CEntryWatcher
// ---------------------------------------------------------------------------
//
CEntryWatcher::~CEntryWatcher()
    {
    Cancel();
    delete iEntry;
    }

// ---------------------------------------------------------------------------
// CEntryWatcher::CompareFind
// ---------------------------------------------------------------------------
//
TInt CEntryWatcher::CompareFind(
        const TDesC* aEntry,
        const CEntryWatcher& aWatcher )
    {
    return aEntry->CompareF( *( aWatcher.iEntry ) );
    }

// ---------------------------------------------------------------------------
// CEntryWatcher::CompareFindData
// ---------------------------------------------------------------------------
//
TInt CEntryWatcher::CompareFindData(
        const TFindData* aData,
        const CEntryWatcher& aWatcher )
    {
    TInt ret( aData->iEntry.CompareF( *( aWatcher.iEntry ) ) );
    if ( !ret )
        {
        ret = Compare( aData->iType, aWatcher.iType );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CEntryWatcher::CompareFindObject
// ---------------------------------------------------------------------------
//
TInt CEntryWatcher::CompareFindObject(
        const CEntryWatcher& aWatcher1,
        const CEntryWatcher& aWatcher2 )
   {
    TInt ret( aWatcher1.iEntry->Des().CompareF( *( aWatcher2.iEntry ) ) );
    if ( !ret )
        {
        ret = Compare( aWatcher1.iType, aWatcher2.iType );
        }
    return ret;
   }

// ---------------------------------------------------------------------------
// CEntryWatcher::WatcherError
// ---------------------------------------------------------------------------
//
void CEntryWatcher::WatcherError(
        TInt aError,
        const TBool& aKilled )
    {
    FUNC_LOG

    LOG_IF_ERROR3(
        aError,
        "CEntryWatcher::WatcherError-iType=%d,iEntry=%S,aError=%d",
        iType, iEntry, aError )

    MDiskNotifyHandlerCallback::TEntryEvent event;
    SetEvent( event, *iEntry, iType );
    Callback().HandleNotifyEntry( aError, event );
    if( aKilled )
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
// CEntryWatcher::ReactivateWatcher
// ---------------------------------------------------------------------------
//
void CEntryWatcher::ReactivateWatcher()
    {
    FUNC_LOG

    Activate();
    }

// ---------------------------------------------------------------------------
// CEntryWatcher::RunWatcher
// ---------------------------------------------------------------------------
//
void CEntryWatcher::RunWatcher( const TBool& /*aKilled*/ )
    {
    FUNC_LOG

    INFO_LOG2(
        "CEntryWatcher::RunWatcher-iType=%d,iEntry=%S",
        iType, iEntry )

    MDiskNotifyHandlerCallback::TEntryEvent event;
    SetEvent( event, *iEntry, iType );
    Callback().HandleNotifyEntry( KErrNone, event );
    }

// ---------------------------------------------------------------------------
// CEntryWatcher::CancelWatcher
// ---------------------------------------------------------------------------
//
void CEntryWatcher::CancelWatcher()
    {
    FUNC_LOG

    INFO_LOG2(
        "CEntryWatcher::CancelWatcher-iType=%d,iEntry=%S",
        iType, iEntry )

    Fs().NotifyChangeCancel( iStatus );
    }

// ---------------------------------------------------------------------------
// CEntryWatcher::Activate
// ---------------------------------------------------------------------------
//
void CEntryWatcher::Activate()
    {
    FUNC_LOG

    if( !IsActive() )
        {
        INFO_LOG2(
            "CEntryWatcher::Activate-iType=%d,iEntry=%S",
            iType, iEntry )
        Fs().NotifyChange( iType, iStatus, *iEntry );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CEntryWatcher::RemoveFromListAndDestroy
// ---------------------------------------------------------------------------
//
void CEntryWatcher::RemoveFromListAndDestroy()
    {
    FUNC_LOG

    TInt i( iWatcherList.FindInOrder( this, CompareFindObject ) );
    if ( i >= 0 && i < iWatcherList.Count() )
        {
        INFO_LOG3(
            "CEntryWatcher::RemoveFromListAndDestroy-iType=%d,iEntry=%S,i=%d",
            iType, iEntry, i )
        iWatcherList.Remove( i );
        delete this;
        }
    }

// End of File
