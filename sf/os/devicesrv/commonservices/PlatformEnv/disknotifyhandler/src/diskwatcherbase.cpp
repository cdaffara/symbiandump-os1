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
* Description:  Base class for disk watchers.
*
*/



//  INCLUDES
#include <e32base.h>
#include "diskwatcherbase.h"
#include "disknotifyhandler.h"
#include "disknotifyhandlerdebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiskWatcherBase::CDiskWatcherBase
// ---------------------------------------------------------------------------
//
CDiskWatcherBase::CDiskWatcherBase(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs ) :
    CActive( CActive::EPriorityStandard ),
    iCallback( aCallback ),
    iFs( aFs )
    {
    FUNC_LOG

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CDiskWatcherBase::~CDiskWatcherBase
// ---------------------------------------------------------------------------
//
CDiskWatcherBase::~CDiskWatcherBase()
    {
    FUNC_LOG

    if ( iKilled )
        {
        *iKilled = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CDiskWatcherBase::RunL
// ---------------------------------------------------------------------------
//
void CDiskWatcherBase::RunL()
    {
    FUNC_LOG_WITH_CLIENT_NAME

    TInt err( iStatus.Int() );
    if ( err == KErrCancel )
        {
        INFO_LOG( "CDiskWatcherBase::RunL-CANCELED" )
        return; // Supress cancel
        }

    TBool killed( EFalse );
    iKilled = &killed;
    if( err != KErrNone )
        {
        // Nofity error
        ERROR_LOG1( "CDiskWatcherBase::RunL-err=%d", err )
        WatcherError( err, killed );
        }
    else
        {
        // Notify event
        ReactivateWatcher();
        RunWatcher( killed );
        }

    if ( killed )
        {
        INFO_LOG( "CDiskWatcherBase::RunL-KILLED" )
        return;
        }
    iKilled = NULL;
    }

// ---------------------------------------------------------------------------
// CDiskWatcherBase::RunError
// ---------------------------------------------------------------------------
//
TInt CDiskWatcherBase::RunError( TInt /*aError*/ )
    {
    FUNC_LOG

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CDiskWatcherBase::DoCancel
// ---------------------------------------------------------------------------
//
void CDiskWatcherBase::DoCancel()
    {
    FUNC_LOG

    CancelWatcher();
    }

// End of File
