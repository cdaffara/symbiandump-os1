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
* Description:
* Implementation of CSsmActivityCmd class.
*
*/

#include "ssmactivitycmd.h"
#include "trace.h"

#include <activitymanager.h>
#include <barsread2.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmActivityCmd::NewL
// ---------------------------------------------------------------------------
//
CSsmActivityCmd* CSsmActivityCmd::NewL()
	{
    FUNC_LOG;
	return new ( ELeave ) CSsmActivityCmd();
	}


// ---------------------------------------------------------------------------
// CSsmActivityCmd::~CSsmActivityCmd
// ---------------------------------------------------------------------------
//
CSsmActivityCmd::~CSsmActivityCmd()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmActivityCmd::Initialize
// ---------------------------------------------------------------------------
//
TInt CSsmActivityCmd::Initialize( CSsmCustomCommandEnv* /*aCmdEnv*/ )
    {
    FUNC_LOG;

    TInt err( KErrNone );
    TRAP( err, iActivityMgr = CUserActivityManager::NewL( EPriorityNormal ) );
    ERROR( err, "Failed to create activity manager." );
    return err;
    }


// ---------------------------------------------------------------------------
// CSsmActivityCmd::Execute
// ---------------------------------------------------------------------------
//
void CSsmActivityCmd::Execute( const TDesC8& aParams, 
                               TRequestStatus& aRequest )
    {
    FUNC_LOG;
    
    aRequest = KRequestPending;
    iClientStatus = &aRequest;
	
    TInt err( KErrNone );
    TInt timeout( 0 );
    
    TRAP( err, timeout = ExtractParamsL( aParams ) );
    
    ERROR( err, "Failed to parse parameters for execution" );
    
    if ( KErrNone == err )
        {
        TInt inactiveTime( User::InactivityTime().Int() );
        
        INFO_2( "inactiveTime=%d, timeout=%d", inactiveTime, timeout );        
        // already inactive, complete immediately
        if ( inactiveTime >= timeout ) 
            {
            INFO( "Already inactive" );
            
            Complete( KErrNone );
            }
        else
            {
            INFO( "Start waiting for inactivity" );
           
            iActivityMgr->Start( TTimeIntervalSeconds( timeout ), 
                                 TCallBack( InactiveCallback, this ), 
                                 TCallBack( ActiveCallback, this ) );
            }
        }
    else
        {
        Complete( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// CSsmActivityCmd::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CSsmActivityCmd::ExecuteCancel()
    {
    FUNC_LOG;

    if ( iActivityMgr )
        {
        iActivityMgr->Cancel();
        }
    
    Complete( KErrCancel );
    }


// ---------------------------------------------------------------------------
// CSsmActivityCmd::Close
// ---------------------------------------------------------------------------
//
void CSsmActivityCmd::Close()
    {
    FUNC_LOG;

    if ( iActivityMgr ) 
        {
        iActivityMgr->Cancel();
        delete iActivityMgr;
        iActivityMgr = NULL;
        }
    }


// ---------------------------------------------------------------------------
// CSsmActivityCmd::Release
// ---------------------------------------------------------------------------
//
void CSsmActivityCmd::Release()
    {
    FUNC_LOG;

	delete this;
    }

// ---------------------------------------------------------------------------
// CSsmActivityCmd::InactiveCallback
//
// ---------------------------------------------------------------------------
//
TInt CSsmActivityCmd::InactiveCallback( TAny* aActivityMonitor )
    {
    FUNC_LOG;
    
    CSsmActivityCmd* mon = static_cast<CSsmActivityCmd*>( aActivityMonitor );
    
    // This would be a good place for cancelling CUserActivityManager
    // However, it reactivates its timer after client callback has been made,
    // so cancel won't have any effect
    // Just complete the request, Complete() checks whether there exists one
    mon->Complete( KErrNone ); // allow proceeding
        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSsmActivityCmd::ActiveCallback
//
// ---------------------------------------------------------------------------
//
TInt CSsmActivityCmd::ActiveCallback( TAny* /*aActivityMonitor*/ )
    {
    FUNC_LOG;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSsmActivityCmd::Complete
//
// ---------------------------------------------------------------------------
//    
void CSsmActivityCmd::Complete( TInt aErrorCode )
    {
    FUNC_LOG;
    
    INFO_2( "iClientStatus=0x%x, aErrorCode=%d", iClientStatus, aErrorCode );
    
    if ( iClientStatus )
        {
        INFO( "Allow startlist execution to continue" );
        
        User::RequestComplete( iClientStatus, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CSsmActivityCmd::Complete
//
// ---------------------------------------------------------------------------
//    
TInt CSsmActivityCmd::ExtractParamsL( const TDesC8& aParams )
    {
    FUNC_LOG;
    
    RResourceReader reader;
    reader.OpenLC( aParams );
    TInt timeout = reader.ReadInt16L();
    CleanupStack::PopAndDestroy(); // Reader
    return timeout;
    }
