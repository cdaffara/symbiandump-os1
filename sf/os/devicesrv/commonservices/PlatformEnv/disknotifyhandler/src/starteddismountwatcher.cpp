/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for started dismount watcher
*
*/


//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include "starteddismountwatcher.h"
#include "disknotifyhandler.h"
#include "disknotifyhandlerdebug.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// SetEvent
// ---------------------------------------------------------------------------
//
inline static void SetEvent(
        MDiskNotifyHandlerCallback::TDismountFinishedEvent& aEvent,
        TInt aDrive,
        TBool aForcedDismount )
    {
    aEvent.iDrive = aDrive;
    aEvent.iForcedDismount = aForcedDismount;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::CStartedDismountWatcher
// ---------------------------------------------------------------------------
//
CStartedDismountWatcher::CStartedDismountWatcher(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CStartedDismountWatcher >& aWatcherList,
        TInt aDrive,
        TTimeIntervalMicroSeconds32 aForcedTimeout ) :
    CDiskWatcherBase( aCallback, aFs ),
    iWatcherList( aWatcherList ),
    iDrive( aDrive ),
    iForcedTimeout( aForcedTimeout )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::ConstructL
// ---------------------------------------------------------------------------
//
void CStartedDismountWatcher::ConstructL()
    {
    FUNC_LOG

    iWatcherList.InsertInOrderL( this, CompareFindObject );
    Activate( iForcedTimeout );
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::NewL
// ---------------------------------------------------------------------------
//
CStartedDismountWatcher* CStartedDismountWatcher::NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs,
        RPointerArray< CStartedDismountWatcher >& aWatcherList,
        TInt aDrive,
        TTimeIntervalMicroSeconds32 aForcedTimeout )
    {
    FUNC_LOG

    CStartedDismountWatcher* self = new ( ELeave ) CStartedDismountWatcher(
        aCallback, aFs, aWatcherList, aDrive, aForcedTimeout );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::~CStartedDismountWatcher
// ---------------------------------------------------------------------------
//
CStartedDismountWatcher::~CStartedDismountWatcher()
    {
    FUNC_LOG

    Cancel();
    delete iForcedDismountTimer;
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::CompareFind
// ---------------------------------------------------------------------------
//
TInt CStartedDismountWatcher::CompareFind(
        const TInt* aDrive,
        const CStartedDismountWatcher& aWatcher )
    {
    return Compare( *aDrive, aWatcher.iDrive );
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::CompareFindObject
// ---------------------------------------------------------------------------
//
TInt CStartedDismountWatcher::CompareFindObject(
        const CStartedDismountWatcher& aWatcher1,
        const CStartedDismountWatcher& aWatcher2 )
    {
    return Compare( aWatcher1.iDrive, aWatcher2.iDrive );
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::WatcherError
// ---------------------------------------------------------------------------
//
void CStartedDismountWatcher::WatcherError(
        TInt aError,
        const TBool& aKilled )
    {
    FUNC_LOG

    LOG_IF_ERROR2(
        aError,
        "CStartedDismountWatcher::WatcherError-iDrive=%d,aError=%d",
        iDrive, aError )

    NotifyDismount( aError, aKilled );
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::ReactivateWatcher
// ---------------------------------------------------------------------------
//
void CStartedDismountWatcher::ReactivateWatcher()
    {
    FUNC_LOG

    // Automatic reactivation is not required
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::RunWatcher
// ---------------------------------------------------------------------------
//
void CStartedDismountWatcher::RunWatcher( const TBool& aKilled )
    {
    FUNC_LOG

    INFO_LOG1( "CStartedDismountWatcher::RunWatcher-iDrive=%d", iDrive )

    NotifyDismount( KErrNone, aKilled );
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::CancelWatcher
// ---------------------------------------------------------------------------
//
void CStartedDismountWatcher::CancelWatcher()
    {
    FUNC_LOG

    INFO_LOG1( "CDismountWatcher::CStartedDismountWatcher-iDrive=%d", iDrive )

    delete iForcedDismountTimer;
    iForcedDismountTimer = NULL;
    Fs().NotifyDismountCancel( iStatus );
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::Activate
// ---------------------------------------------------------------------------
//
void CStartedDismountWatcher::Activate( TTimeIntervalMicroSeconds32 aForcedTimeout )
    {
    FUNC_LOG

    if ( !IsActive() )
        {
        INFO_LOG1( "CStartedDismountWatcher::Activate-iDrive=%d", iDrive )

        iForcedTimeout = aForcedTimeout;
        delete iForcedDismountTimer;
        iForcedDismountTimer = NULL;
        iForcedDismount = EFalse;

        // Start forced timer if needed
        TInt err( KErrNone );
        if ( iForcedTimeout.Int() > 0 )
            {
            iForcedDismountTimer = CPeriodic::New( CActive::EPriorityHigh );
            if ( iForcedDismountTimer )
                {
                iForcedDismountTimer->Start( iForcedTimeout, iForcedTimeout,
                    TCallBack( ForcedDismountCB, this ) );
                }
            else
                {
                err = KErrNoMemory;
                }
            }

        if ( err == KErrNone )
            {
            // Do immediate forced dismount if timeout is zero
            if ( iForcedTimeout.Int() == 0 )
                {
                iForcedDismount = ETrue;
                Fs().NotifyDismount( iDrive, iStatus, EFsDismountForceDismount );
                }
            else
                {
                Fs().NotifyDismount( iDrive, iStatus, EFsDismountNotifyClients );
                }
            }
        else
            {
            ERROR_LOG1( "CStartedDismountWatcher::Activate-err=%d", err )
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, err );
            }
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::RemoveFromListAndDestroy
// ---------------------------------------------------------------------------
//
void CStartedDismountWatcher::RemoveFromListAndDestroy()
    {
    FUNC_LOG

    TInt i( iWatcherList.FindInOrder( this, CompareFindObject ) );
    if ( i >= 0 && i < iWatcherList.Count() )
        {
        INFO_LOG2(
            "CStartedDismountWatcher::RemoveFromListAndDestroy-iDrive=%d,i=%d",
            iDrive, i )
        iWatcherList.Remove( i );
        delete this;
        }
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::ForcedDismountCB
// ---------------------------------------------------------------------------
//
TInt CStartedDismountWatcher::ForcedDismountCB( TAny* aPtr )
    {
    FUNC_LOG

    CStartedDismountWatcher* self = static_cast<CStartedDismountWatcher*>( aPtr );
    if ( self )
        {
        self->RequestForcedDismount();
        }
    else
        {
        ERROR_LOG( "CStartedDismountWatcher::ForcedDismountCB-NULL" )
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::RequestForcedDismount
// ---------------------------------------------------------------------------
//
void CStartedDismountWatcher::RequestForcedDismount()
    {
    FUNC_LOG

    // Cancel forced dismount timer
    delete iForcedDismountTimer;
    iForcedDismountTimer = NULL;

    // Cancel soft dismount
    Cancel();

    iForcedDismount = ETrue;
    Fs().NotifyDismount( iDrive, iStatus, EFsDismountForceDismount );    
    SetActive();
    }

// ---------------------------------------------------------------------------
// CStartedDismountWatcher::NotifyDismount
// ---------------------------------------------------------------------------
//
void CStartedDismountWatcher::NotifyDismount( TInt aError, const TBool& aKilled )
    {
    FUNC_LOG

    delete iForcedDismountTimer;
    iForcedDismountTimer = NULL;

    MDiskNotifyHandlerCallback::TDismountFinishedEvent event;
    SetEvent( event, iDrive, iForcedDismount );
    Callback().HandleNotifyDismountFinished( aError, event );
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

// End of File
