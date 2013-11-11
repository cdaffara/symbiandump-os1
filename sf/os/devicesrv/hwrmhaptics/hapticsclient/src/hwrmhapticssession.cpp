/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of client side session.
*
*/


#include <e32svr.h>
#include <data_caging_path_literals.hrh>

#include "hwrmhapticssession.h"
#include "hwrmhapticsconfiguration.h"
#include "hwrmhapticsclientserver.h"
#include "hwrmhapticstrace.h"

// ---------------------------------------------------------------------------
// Connects to server
// ---------------------------------------------------------------------------
//
TInt RHWRMHapticsSession::Connect()
    {
   COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::Connect()" ) ) );

    TInt err = MakeConnection();

    // Create proper service on server side. i.e. initialize
    // server components
    if ( err == KErrNone )
        {
        err = SendReceive( EHWRMHapticsService );
        }
        
    COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::Connect - return(%d)" ), err ) );        
    
    return err;
    }


// ---------------------------------------------------------------------------
// Connects to server (ASYNC)
// ---------------------------------------------------------------------------
//
void RHWRMHapticsSession::Connect( TRequestStatus& aStatus )
    {
   COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::Connect()" ) ) );

    TInt err = MakeConnection();

    // Create proper service on server side. i.e. initialize
    // server components
    if ( err == KErrNone )
        {
		aStatus = KRequestPending;
        SendReceive( EHWRMHapticsService, aStatus );
        }
    else
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, err );
        }

    COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::Connect - return(%d)" ), err ) );
    }


// ---------------------------------------------------------------------------
// Calls SendReceive with given parameters.
// ---------------------------------------------------------------------------
//
TInt RHWRMHapticsSession::ExecuteOperation( 
                                            TInt aCommand, 
                                            const TIpcArgs& aArgs ) const
    {
    COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::ExecuteOperation(0x%x)" ), aCommand ) );
    
    TInt retval = SendReceive( aCommand, aArgs );
    
    COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::ExecuteOperation - return %d" ), retval ) );
    
    return retval;
    }

// ---------------------------------------------------------------------------
// Calls SendReceive with given parameter.
// ---------------------------------------------------------------------------
//
TInt RHWRMHapticsSession::ExecuteOperation( TInt aCommand ) const
    {
    COMPONENT_TRACE( _L( "RHWRMHapticsSession::ExecuteOperation" ) );
    
    TInt retval = SendReceive( aCommand );
    
    COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::ExecuteOperation - return %d" ), retval ) );
    
    return retval;
    }

// ---------------------------------------------------------------------------
// Calls SendReceive with given parameters.
// ---------------------------------------------------------------------------
//
void RHWRMHapticsSession::ExecuteAsyncOperation(
                                            TInt aCommand, 
                                            const TIpcArgs& aArgs, 
                                            TRequestStatus& aStatus ) const
    {
    COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::ExecuteAsyncOperation - commandId(%d), aStatus(%d)" ), aCommand, aStatus.Int() ) );

    aStatus = KRequestPending;

    SendReceive( aCommand, aArgs, aStatus );
    
    COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::ExecuteAsyncOperation - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// Starts server.
// ---------------------------------------------------------------------------
//
TInt RHWRMHapticsSession::StartServer() const
    {
    COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::StartServer()" ) ) );
    
    RProcess server;
    TInt ret = server.Create( ServerLocation(), KNullDesC );

    if ( ret != KErrNone ) // Loading failed.
        {
        return ret;
        }
    
    TRequestStatus status;
    server.Rendezvous( status );

    if ( status != KRequestPending )
        {
        server.Kill( 0 ); // Abort startup.
        server.Close();
        return KErrGeneral;
        }
    else
        {
        server.Resume(); // Logon OK - start the server.
        }
        
    User::WaitForRequest( status );
    	
    // We can't use the 'exit reason' if the server paniced as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    ret = (server.ExitType() == EExitPanic)? KErrGeneral : status.Int();
    	
    server.Close();

    COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::StartServer - return %d" ), ret ) );

    return ret;    
    }

// ---------------------------------------------------------------------------
// Returns the server location.
// ---------------------------------------------------------------------------
//
TFullName RHWRMHapticsSession::ServerLocation() const
    {
    TFullName fullPathAndName( KServerExeDrive );
    fullPathAndName.Append( KDC_PROGRAMS_DIR );
    fullPathAndName.Append( KServerExeName );
    
    COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::ServerLocation - return %S" ), &fullPathAndName ) );

    return fullPathAndName;
    }

// ---------------------------------------------------------------------------
// Returns the version of the server.
// ---------------------------------------------------------------------------
//
TVersion RHWRMHapticsSession::ServerVersion() const 
    {
    return TVersion( KServerVersionMajor, 
                     KServerVersionMinor,
                     KServerVersionBuild );
    }

// ---------------------------------------------------------------------------
// Makes connection to server.
// ---------------------------------------------------------------------------
//
TInt RHWRMHapticsSession::MakeConnection()
    {
    // Try connection twice, if not successful on the first attempt
    TInt retry( 2 );
    TInt err( KErrNone );

    while ( retry > 0 )
        {
        // try to create a session to the server
        err = CreateSession( KServerProcessName, 
                             ServerVersion(), 
                             KDefaultAsyncSlots );

        COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::MakeConnection - CreateSession returned: err(%d)" ), err ) );        
        
        if ( err != KErrNotFound && err != KErrServerTerminated )
            {
            retry = 0; // KErrNone or unrecoverable error
            }
        else
            {
            // Return code was KErrNotFound or KErrServerTerminated.
            // Try to start the Haptics server
            err = StartServer();

            COMPONENT_TRACE( ( _L( "RHWRMHapticsSession::MakeConnection - StartServer returned: err(%d)" ), err ) );        

            if ( err != KErrNone && err != KErrAlreadyExists )
                {
                retry = 0; // Unrecoverable error
                }
            }
        
        retry--;
        }

    return err;    
    }
//  End of File  
