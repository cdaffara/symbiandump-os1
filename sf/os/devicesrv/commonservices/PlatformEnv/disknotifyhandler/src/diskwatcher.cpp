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
* Description:  Class for notify disk watcher
*
*/



//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include <driveinfo.h>
#include "diskwatcher.h"
#include "disknotifyhandler.h"
#include "disknotifyhandlerdebug.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// SetEvent
// ---------------------------------------------------------------------------
//
inline static void SetEvent(
        MDiskNotifyHandlerCallback::TDiskEvent& aEvent,
        MDiskNotifyHandlerCallback::TDiskEventType aType,
        TInt aDrive,
        const TDriveInfo& aInfo,
        const TDriveInfo& aPrevInfo )
    {
    aEvent.iType = aType;
    aEvent.iDrive = aDrive;
    aEvent.iInfo = aInfo;
    aEvent.iPrevInfo = aPrevInfo;
    }

// ---------------------------------------------------------------------------
// SetEmptyDriveInfo
// ---------------------------------------------------------------------------
//
inline static void SetEmptyDriveInfo( TDriveInfo& aInfo )
    {
    aInfo.iType = EMediaNotPresent;
    aInfo.iBattery = EBatNotSupported;
    aInfo.iDriveAtt = 0;
    aInfo.iMediaAtt = 0;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiskWatcher::CDiskWatcher
// ---------------------------------------------------------------------------
//
CDiskWatcher::CDiskWatcher(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs ) :
    CDiskWatcherBase( aCallback, aFs )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::ConstructL
// ---------------------------------------------------------------------------
//
void CDiskWatcher::ConstructL()
    {
    FUNC_LOG

    Activate();

    RFs& fs( Fs() );
    TDriveList drvList;
    User::LeaveIfError( fs.DriveList( drvList, KDriveAttAll ) );

    TInt count( Min( drvList.Length(), iInfoList.Count() ) );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( drvList[ i ] )
            {
            iInfoList[ i ].Read( fs, i );
            }
        }
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::NewL
// ---------------------------------------------------------------------------
//
CDiskWatcher* CDiskWatcher::NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs )
    {
    FUNC_LOG

    CDiskWatcher* self = new ( ELeave ) CDiskWatcher( aCallback, aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::~CDiskWatcher
// ---------------------------------------------------------------------------
//
CDiskWatcher::~CDiskWatcher()
    {
    FUNC_LOG

    Cancel();
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::WatcherError
// ---------------------------------------------------------------------------
//
void CDiskWatcher::WatcherError(
        TInt aError,
        const TBool& /*aKilled*/ )
    {
    FUNC_LOG

    LOG_IF_ERROR1( aError, "CDiskWatcher::WatcherError-aError=%d",
        aError )

    MDiskNotifyHandlerCallback::TDiskEvent event;
    TDriveInfo emptyDrvInfo;
    SetEmptyDriveInfo( emptyDrvInfo );

    SetEvent(
        event,
        MDiskNotifyHandlerCallback::EDiskError,
        KErrNotFound,
        emptyDrvInfo,
        emptyDrvInfo );

    Callback().HandleNotifyDisk( aError, event );
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::ReactivateWatcher
// ---------------------------------------------------------------------------
//
void CDiskWatcher::ReactivateWatcher()
    {
    FUNC_LOG

    Activate();
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::RunWatcher
// ---------------------------------------------------------------------------
//
void CDiskWatcher::RunWatcher( const TBool& aKilled )
    {
    FUNC_LOG

    RFs& fs( Fs() );
    MDiskNotifyHandlerCallback& callback( Callback() );

    TDriveList drvList;
    TInt err( fs.DriveList( drvList, KDriveAttAll ) );
    if ( err != KErrNone )
        {
        ERROR_LOG1( "CDiskWatcher::RunWatcher-err1=%d", err )
        return;
        }

    TInt count( Min( drvList.Length(), iInfoList.Count() ) );
    TDriveInfo emptyDrvInfo;
    SetEmptyDriveInfo( emptyDrvInfo );
    MDiskNotifyHandlerCallback::TDiskEvent event;
    TWatcherInfo newInfo;

    for( TInt i( 0 ); i < count; ++i )
        {
        TWatcherInfo& info( iInfoList[ i ] );
        if ( !drvList[ i ] && info.iDriveStatus )
            {
            INFO_LOG1( "CDiskWatcher::RunWatcher-Drive %d removed", i )

            // Inform drives removed from drive list
            SetEvent(
                event,
                MDiskNotifyHandlerCallback::EDiskRemoved,
                i,
                emptyDrvInfo,
                info.iDriveInfo );

            callback.HandleNotifyDisk( KErrNone, event );
            if ( aKilled )
                {
                return; // Deleted from callback
                }
            info.Reset(); // Set to empty
            }
        else if ( drvList[ i ] && !info.iDriveStatus )
            {
            INFO_LOG1( "CDiskWatcher::RunWatcher-Drive %d added", i )

            // Inform drives added to drive list
            info.Read( fs, i );
            if ( info.iDriveStatus )
                {
                SetEvent(
                    event,
                    MDiskNotifyHandlerCallback::EDiskAdded,
                    i,
                    info.iDriveInfo,
                    emptyDrvInfo );
    
                callback.HandleNotifyDisk( KErrNone, event );
                if ( aKilled )
                    {
                    return;  // Deleted from callback
                    }
                }
            }
        else if ( drvList[ i ] && info.iDriveStatus )
            {
            // Inform drive status changes
            newInfo.Read( fs, i );
            if ( info.IsChanged( newInfo ) )
                {
                INFO_LOG1( "CDiskWatcher::RunWatcher-Drive %d changed", i )

                SetEvent(
                    event,
                    MDiskNotifyHandlerCallback::EDiskStatusChanged,
                    i,
                    newInfo.iDriveInfo,
                    info.iDriveInfo );

                callback.HandleNotifyDisk( KErrNone, event );
                if ( aKilled )
                    {
                    return; // Deleted from callback
                    }
                info = newInfo; // Save changed info
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::CancelWatcher
// ---------------------------------------------------------------------------
//
void CDiskWatcher::CancelWatcher()
    {
    FUNC_LOG

    Fs().NotifyChangeCancel( iStatus );
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::Activate
// ---------------------------------------------------------------------------
//
void CDiskWatcher::Activate()
    {
    FUNC_LOG

    if ( !IsActive() )
        {
        INFO_LOG( "CDiskWatcher::Activate" )
        Fs().NotifyChange( ENotifyDisk, iStatus );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::TWatcherInfo::Read
// ---------------------------------------------------------------------------
//
void CDiskWatcher::TWatcherInfo::Read( RFs& aFs, TInt aDrive )
    {
    FUNC_LOG

    TInt err( aFs.Drive( iDriveInfo, aDrive ) );
    LOG_IF_ERROR2( err, "CDiskWatcher::TWatcherInfo::Read-aDrive=%d,err1=%d",
        aDrive, err )
    if ( err == KErrNone )
        {
        // Needed for detecting USB mass storage mode transition
        err = DriveInfo::GetDriveStatus( aFs, aDrive, iDriveStatus );
        LOG_IF_ERROR2( err, "CDiskWatcher::TWatcherInfo::Read-aDrive=%d,err2=%d",
            aDrive, err )
        }
    if ( err != KErrNone )
        {
        Reset();
        }
    INFO_LOG3(
        "CDiskWatcher::TWatcherInfo::Read-Drive %d,Status 0x%x,Type %d",
        aDrive, iDriveStatus, iDriveInfo.iType )
    INFO_LOG3(
        "CDiskWatcher::TWatcherInfo::Read-Drive %d,DriveAtt 0x%x,MediaAtt 0x%x",
        aDrive, iDriveInfo.iDriveAtt, iDriveInfo.iMediaAtt )
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::TWatcherInfo::IsChanged
// ---------------------------------------------------------------------------
//
TBool CDiskWatcher::TWatcherInfo::IsChanged(
        const TWatcherInfo& aNewInfo ) const
    {
    FUNC_LOG

    TBool ret( iDriveStatus != aNewInfo.iDriveStatus ||
               iDriveInfo.iType != aNewInfo.iDriveInfo.iType ||
               iDriveInfo.iDriveAtt != aNewInfo.iDriveInfo.iDriveAtt ||
               iDriveInfo.iMediaAtt != aNewInfo.iDriveInfo.iMediaAtt );
    INFO_LOG3(
        "CDiskWatcher::TWatcherInfo::IsChanged %d,Status 0x%x,NewStatus 0x%x",
        ret, iDriveStatus, aNewInfo.iDriveStatus )
    return ret;
    }

// ---------------------------------------------------------------------------
// CDiskWatcher::TWatcherInfo::Reset
// ---------------------------------------------------------------------------
//
void CDiskWatcher::TWatcherInfo::Reset()
    {
    FUNC_LOG

    SetEmptyDriveInfo( iDriveInfo );
    iDriveStatus = 0;
    }

// End of File
