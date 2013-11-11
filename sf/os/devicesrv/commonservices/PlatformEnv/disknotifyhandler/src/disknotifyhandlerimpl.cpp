/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Wrapper implementation for file server notification handling.
*
*/



// INCLUDES
#include <e32std.h>
#include <f32file.h>
#include "disknotifyhandler.h"
#include "disknotifyhandlerimpl.h"
#include "diskwatcher.h"
#include "dismountwatcher.h"
#include "diskspacewatcher.h"
#include "entrywatcher.h"
#include "starteddismountwatcher.h"
#include "disknotifyhandlerdebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CDiskNotifyHandlerImpl
// ---------------------------------------------------------------------------
//
CDiskNotifyHandlerImpl::CDiskNotifyHandlerImpl(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs ) :
    iCallback( aCallback ),
    iFs( aFs )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::NewL
// ---------------------------------------------------------------------------
//
CDiskNotifyHandlerImpl* CDiskNotifyHandlerImpl::NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs )
    {
    FUNC_LOG

    return new ( ELeave ) CDiskNotifyHandlerImpl( aCallback, aFs );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::~CDiskNotifyHandlerImpl
// ---------------------------------------------------------------------------
//
CDiskNotifyHandlerImpl::~CDiskNotifyHandlerImpl()
    {
    FUNC_LOG

    delete iDiskWatcher;

    iDismountWatcherList.ResetAndDestroy();
    iDismountWatcherList.Close();

    iDiskSpaceWatcherList.ResetAndDestroy();
    iDiskSpaceWatcherList.Close();

    iEntryWatcherList.ResetAndDestroy();
    iEntryWatcherList.Close();

    iStartedDismountWatcherList.ResetAndDestroy();
    iStartedDismountWatcherList.Close();
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::NotifyDisk
// ---------------------------------------------------------------------------
//
TInt CDiskNotifyHandlerImpl::NotifyDisk()
    {
    FUNC_LOG

    if ( iDiskWatcher )
        {
        if ( iDiskWatcher->IsActive() )
            {
            ERROR_LOG1( "CDiskNotifyHandlerImpl::NotifyDisk-ret1=%d",
                KErrAlreadyExists )
            return KErrAlreadyExists;
            }
        iDiskWatcher->Activate();
        return KErrNone;
        }

    TRAPD( ret, iDiskWatcher = CDiskWatcher::NewL( iCallback, iFs ) );
    LOG_IF_ERROR1( ret, "CDiskNotifyHandlerImpl::NotifyDisk-ret2=%d", ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelNotifyDisk
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelNotifyDisk()
    {
    FUNC_LOG

    // Delete disk watcher instance to stop receiving pending 
    // callback notifications
    delete iDiskWatcher;
    iDiskWatcher = NULL;
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::NotifyDismount
// ---------------------------------------------------------------------------
//
TInt CDiskNotifyHandlerImpl::NotifyDismount( TInt aDrive )
    {
    FUNC_LOG

    CDismountWatcher* watcher = NULL;
    TInt i( iDismountWatcherList.FindInOrder(
        aDrive, CDismountWatcher::CompareFind ) );
    if ( i >= 0 && i < iDismountWatcherList.Count() )
        {
        watcher = iDismountWatcherList[ i ];
        if ( watcher->IsActive() )
            {
            ERROR_LOG2(
                "CDiskNotifyHandlerImpl::NotifyDismount-aDrive=%d,ret1=%d",
                aDrive, KErrAlreadyExists )
            return KErrAlreadyExists;
            }
        watcher->Activate();
        return KErrNone;
        }

    TRAPD( ret, watcher = CDismountWatcher::NewL(
        iCallback, iFs, iDismountWatcherList, aDrive ) );
    if ( ret != KErrNone )
        {
        ERROR_LOG2(
            "CDiskNotifyHandlerImpl::NotifyDismount-aDrive=%d,ret2=%d",
            aDrive, ret )
        return ret;
        }

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::NotifyDismount-count=%d",
        iDismountWatcherList.Count() )

    return ret;
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelNotifyDismount
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelNotifyDismount( TInt aDrive )
    {
    FUNC_LOG

    TInt i( iDismountWatcherList.FindInOrder(
        aDrive, CDismountWatcher::CompareFind ) );
    if ( i >= 0 && i < iDismountWatcherList.Count() )
        {
        INFO_LOG2(
            "CDiskNotifyHandlerImpl::CancelNotifyDismount-aDrive=%d,i=%d",
            aDrive, i )
        CDismountWatcher* watcher = iDismountWatcherList[ i ];
        iDismountWatcherList.Remove( i );
        delete watcher;
        }

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::CancelNotifyDismount-count=%d",
        iDismountWatcherList.Count() )
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelNotifyDismount
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelNotifyDismount()
    {
    FUNC_LOG

    iDismountWatcherList.ResetAndDestroy();

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::CancelNotifyDismount-count=%d",
        iDismountWatcherList.Count() )
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::AllowDismount
// ---------------------------------------------------------------------------
//
TInt CDiskNotifyHandlerImpl::AllowDismount( TInt aDrive )
    {
    FUNC_LOG

    TInt i( iDismountWatcherList.FindInOrder(
        aDrive, CDismountWatcher::CompareFind ) );
    if ( i >= 0 && i < iDismountWatcherList.Count() )
        {
        return iDismountWatcherList[ i ]->AllowDismount();
        }

    // Suppress not found error
    ERROR_LOG1( "CDiskNotifyHandlerImpl::AllowDismount-aDrive=%d,NOT FOUND",
        aDrive )

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::NotifyDiskSpace
// ---------------------------------------------------------------------------
//
TInt CDiskNotifyHandlerImpl::NotifyDiskSpace(
        const TInt64& aThreshold, TInt aDrive )
    {
    FUNC_LOG

    CDiskSpaceWatcher::TFindData data;
    data.iDrive = aDrive;
    data.iThreshold = aThreshold;
    CDiskSpaceWatcher* watcher = NULL;
    TInt i( iDiskSpaceWatcherList.FindInOrder(
            data, CDiskSpaceWatcher::CompareFindData ) );
    if ( i >= 0 && i < iDiskSpaceWatcherList.Count() )
        {
        watcher = iDiskSpaceWatcherList[ i ];
        if ( watcher->IsActive() )
            {
            ERROR_LOG3(
                "CDiskNotifyHandlerImpl::NotifyDiskSpace-aThreshold=%Ld,aDrive=%d,ret1=%d",
                aThreshold, aDrive, KErrAlreadyExists )
            return KErrAlreadyExists;
            }
        watcher->Activate();
        return KErrNone;
        }

    TRAPD( ret, watcher = CDiskSpaceWatcher::NewL(
        iCallback, iFs, iDiskSpaceWatcherList, aDrive, aThreshold ) );
    if ( ret != KErrNone )
        {
        ERROR_LOG3(
            "CDiskNotifyHandlerImpl::NotifyDiskSpace-aThreshold=%Ld,aDrive=%d,ret2=%d",
            aThreshold, aDrive, ret )
        return ret;
        }

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::NotifyDiskSpace-count=%d",
        iDiskSpaceWatcherList.Count() )

    return ret;
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelNotifyDiskSpace
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelNotifyDiskSpace(
        const TInt64& aThreshold, TInt aDrive )
    {
    FUNC_LOG

    CDiskSpaceWatcher::TFindData data;
    data.iDrive = aDrive;
    data.iThreshold = aThreshold;
    TInt i( iDiskSpaceWatcherList.FindInOrder(
        data, CDiskSpaceWatcher::CompareFindData ) );
    if ( i >= 0 && i < iDiskSpaceWatcherList.Count() )
        {
        INFO_LOG3(
            "CDiskNotifyHandlerImpl::CancelNotifyDiskSpace-aThreshold=%Ld,aDrive=%d,i=%d",
            aThreshold, aDrive, i )
        CDiskSpaceWatcher* watcher = iDiskSpaceWatcherList[ i ];
        iDiskSpaceWatcherList.Remove( i );
        delete watcher;
        }

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::CancelNotifyDiskSpace-count=%d",
        iDiskSpaceWatcherList.Count() )
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelNotifyDiskSpace
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelNotifyDiskSpace( TInt aDrive )
    {
    FUNC_LOG

    for( ;; )
        {
        TInt i( iDiskSpaceWatcherList.FindInOrder(
            aDrive, CDiskSpaceWatcher::CompareFind ) );
        if ( i >= 0 && i < iDiskSpaceWatcherList.Count() )
            {
            INFO_LOG2(
                "CDiskNotifyHandlerImpl::CancelNotifyDiskSpace-aDrive=%d,i=%d",
                aDrive, i )
            CDiskSpaceWatcher* watcher = iDiskSpaceWatcherList[ i ];
            iDiskSpaceWatcherList.Remove( i );
            delete watcher;
            }
        else
            {
            break; // No matching watchers left
            }
        }

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::CancelNotifyDiskSpace-count=%d",
        iDiskSpaceWatcherList.Count() )
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelNotifyDiskSpace
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelNotifyDiskSpace()
    {
    FUNC_LOG

    iDiskSpaceWatcherList.ResetAndDestroy();

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::CancelNotifyDiskSpace-count=%d",
        iDiskSpaceWatcherList.Count() )
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::NotifyEntry
// ---------------------------------------------------------------------------
//
TInt CDiskNotifyHandlerImpl::NotifyEntry(
        TNotifyType aType, const TDesC& aEntry )
    {
    FUNC_LOG

    CEntryWatcher::TFindData data;
    data.iEntry.Set( aEntry );
    data.iType = aType;
    CEntryWatcher* watcher = NULL;
    TInt i( iEntryWatcherList.FindInOrder(
        data, CEntryWatcher::CompareFindData ) );
    if ( i >= 0 && i < iEntryWatcherList.Count() )
        {
        watcher = iEntryWatcherList[ i ];
        if ( watcher->IsActive() )
            {
            ERROR_LOG3(
                "CDiskNotifyHandlerImpl::NotifyEntry-aType=%d,aEntry=%S,ret1=%d",
                aType, &aEntry, KErrAlreadyExists )
            return KErrAlreadyExists;
            }
        watcher->Activate();
        return KErrNone;
        }

    TRAPD( ret, watcher = CEntryWatcher::NewL(
        iCallback, iFs, iEntryWatcherList, aEntry, aType ) );
    if ( ret != KErrNone )
        {
        ERROR_LOG3(
            "CDiskNotifyHandlerImpl::NotifyEntry-aType=%d,aEntry=%S,ret2=%d",
            aType, &aEntry, ret )
        return ret;
        }

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::NotifyEntry-count=%d",
        iEntryWatcherList.Count() )

    return ret;
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelNotifyEntry
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelNotifyEntry(
        TNotifyType aType, const TDesC& aEntry )
    {
    FUNC_LOG

    CEntryWatcher::TFindData data;
    data.iEntry.Set( aEntry );
    data.iType = aType;
    TInt i( iEntryWatcherList.FindInOrder(
        data, CEntryWatcher::CompareFindData ) );
    if ( i >= 0 && i < iEntryWatcherList.Count() )
        {
        INFO_LOG3(
            "CDiskNotifyHandlerImpl::CancelNotifyEntry-aType=%d,aEntry=%S,i=%d",
            aType, &aEntry, i )
        CEntryWatcher* watcher = iEntryWatcherList[ i ];
        iEntryWatcherList.Remove( i );
        delete watcher;
        }

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::CancelNotifyEntry-count=%d",
        iEntryWatcherList.Count() )
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelNotifyEntry
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelNotifyEntry( const TDesC& aEntry )
    {
    FUNC_LOG

    for( ;; )
        {
        TInt i( iEntryWatcherList.FindInOrder(
            aEntry, CEntryWatcher::CompareFind ) );
        if ( i >= 0 && i < iEntryWatcherList.Count() )
            {
            INFO_LOG2(
                "CDiskNotifyHandlerImpl::CancelNotifyEntry-aEntry=%S,i=%d",
                &aEntry, i )
            CEntryWatcher* watcher = iEntryWatcherList[ i ];
            iEntryWatcherList.Remove( i );
            delete watcher;
            }
        else
            {
            break; // No matching watchers left
            }
        }

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::CancelNotifyEntry-count=%d",
        iEntryWatcherList.Count() )
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelNotifyEntry
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelNotifyEntry()
    {
    FUNC_LOG

    iEntryWatcherList.ResetAndDestroy();

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::CancelNotifyEntry-count=%d",
        iEntryWatcherList.Count() )
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::StartDismount
// ---------------------------------------------------------------------------
//
TInt CDiskNotifyHandlerImpl::StartDismount(
        TInt aDrive, TTimeIntervalMicroSeconds32 aForcedTimeout )
    {
    FUNC_LOG

    CStartedDismountWatcher* watcher = NULL;
    TInt i( iStartedDismountWatcherList.FindInOrder(
        aDrive, CStartedDismountWatcher::CompareFind ) );
    if ( i >= 0 && i < iStartedDismountWatcherList.Count() )
        {
        watcher = iStartedDismountWatcherList[ i ];
        if ( watcher->IsActive() )
            {
            ERROR_LOG2(
                "CDiskNotifyHandlerImpl::StartDismount-aDrive=%d,ret1=%d",
                aDrive, KErrAlreadyExists )
            return KErrAlreadyExists;
            }
        watcher->Activate( aForcedTimeout );
        return KErrNone;
        }

    TRAPD( ret, watcher = CStartedDismountWatcher::NewL(
        iCallback, iFs, iStartedDismountWatcherList, aDrive, aForcedTimeout ) );
    if ( ret != KErrNone )
        {
        ERROR_LOG2(
            "CDiskNotifyHandlerImpl::StartDismount-aDrive=%d,ret2=%d",
            aDrive, ret )
        return ret;
        }

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::StartDismount-count=%d",
        iStartedDismountWatcherList.Count() )

    return ret;
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelStartedDismount
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelStartedDismount( TInt aDrive )
    {
    FUNC_LOG

    TInt i( iStartedDismountWatcherList.FindInOrder(
        aDrive, CStartedDismountWatcher::CompareFind ) );
    if ( i >= 0 && i < iStartedDismountWatcherList.Count() )
        {
        INFO_LOG2(
            "CDiskNotifyHandlerImpl::CancelStartedDismount-aDrive=%d,i=%d",
            aDrive, i )
        CStartedDismountWatcher* watcher = iStartedDismountWatcherList[ i ];
        iStartedDismountWatcherList.Remove( i );
        delete watcher;
        }

    INFO_LOG1(
        "CDiskNotifyHandlerImpl::CancelStartedDismount-count=%d",
        iStartedDismountWatcherList.Count() )
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandlerImpl::CancelStartedDismount
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandlerImpl::CancelStartedDismount()
    {
    FUNC_LOG

    iStartedDismountWatcherList.ResetAndDestroy();
    }

// End of File
