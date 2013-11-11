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
* Description:  Sensor server client side connector class
*
*/



#include <e32svr.h>
#include <data_caging_path_literals.hrh>  
#include "sensrvdefines.h"
#include "sensrvclient.h"
#include "sensrvclientserver.h"
#include "sensrvtrace.h"


// -----------------------------------------------------------------------------
// RSensrvClient::Connect
// Connects to server
// -----------------------------------------------------------------------------
//
TInt RSensrvClient::Connect()
    {
   COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::Connect()" ) ) );

    // Try this twice
    TInt retry(KSensrvClientTryCount);
    TInt err(KErrNone);

    while (retry > 0)
        {
        // Try to create a Sensor Server session
        err = CreateSession( KSensrvProcessName, 
                             ServerVersion(), 
                             KSensrvDefaultAsyncSlots );

        COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::Connect - CreateSession returned: %d" ), err ) );        
        
        if ( err != KErrNotFound && err != KErrServerTerminated )
            {
            // KErrNone or unrecoverable error
            if (err != KErrNone)
                {
                ERROR_TRACE( ( _L( "Sensrv Client - RSensrvClient::Connect - ERROR: CreateSession unrecoverable error: %d" ), err ) );
                }
            
            retry = 0;
            }
        else
            {
            // Return code was KErrNotFound or KErrServerTerminated.
            // Try to start a new Sensor Server
            err = StartServer();

            COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::Connect - StartServer returned: %d" ), err ) );        

            if ( err != KErrNone && err != KErrAlreadyExists )
                {
                // Unrecoverable error
                ERROR_TRACE( ( _L( "Sensrv Client - RSensrvClient::Connect - ERROR: StartServer unrecoverable error: %d" ), err ) );
                retry = 0;
                }
            }
                
        retry--;
        }
        
    COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::Connect - return %d" ), err ) );        
    return err;
    }
    

// -----------------------------------------------------------------------------
// RSensrvClient::SendReceiveSync
// Send message to server
// -----------------------------------------------------------------------------
//
TInt RSensrvClient::SendReceiveSync( TSensrvServerRequest aRequest, const TIpcArgs& aArgs ) const
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::SendReceiveSync()" ) ) );
    return SendReceive( aRequest, aArgs );
    }

// -----------------------------------------------------------------------------
// RSensrvClient::SendReceiveSync
// Send asynchronous message to server
// -----------------------------------------------------------------------------
//
void RSensrvClient::SendReceiveAsync( TSensrvServerRequest aRequest, const TIpcArgs& aArgs, TRequestStatus& aStatus ) const
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::SendReceiveAsync()" ) ) );
    SendReceive( aRequest, aArgs, aStatus );
    }
    
    
// -----------------------------------------------------------------------------
// RSensrvClient::StartServer
// Starts server.
// -----------------------------------------------------------------------------
//
TInt RSensrvClient::StartServer() const
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::StartServer()" ) ) );

    RProcess server;
    const TUidType serverUid( KNullUid, KSensrvUid2, KNullUid );
    TInt err = server.Create( ServerLocation(), // sensorserver.exe
                           KNullDesC, // A descriptor containing data passed as 
                                      // an argument to the thread function of 
                                      // the new process's main thread, when it 
                                      // is first scheduled.
                           serverUid, // Sensor server UID
                           EOwnerProcess ); // Ownership of this process handle 

    // Return error code if we couldn't create a process
    if ( err == KErrNone )
        {
        // Rendezvous is used to detect server start
        TRequestStatus stat;
        server.Rendezvous( stat );

        if ( stat != KRequestPending )
            {
            ERROR_TRACE( ( _L( "Sensrv Client - RSensrvClient::StartServer - ERROR: Rendezvous failure: %d" ), stat.Int() ) );
            server.Kill( 0 ); // Abort startup
            }
        else
            {
            server.Resume();  // Logon OK - start the server
            }

        COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::StartServer - Waiting server startup" ) ) );

        User::WaitForRequest( stat ); // Wait for start or death

        COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::StartServer - Server startup wait finished" ) ) );
        
        // We can't use the 'exit reason' if the server paniced as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone
        if ( server.ExitType() == EExitPanic ) 
            {
            ERROR_TRACE( ( _L( "Sensrv Client - RSensrvClient::StartServer - ERROR: Server paniced at startup" ) ) );
            err = KErrGeneral;
            }
        else
            {
            err = stat.Int();
            }

        // We can close the handle now
        server.Close();        
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::StartServer - return %d" ), err ) );

    return err;
    }

// -----------------------------------------------------------------------------
// RSensrvClient::ServerLocation
// Returns server location
// -----------------------------------------------------------------------------
//
TFullName RSensrvClient::ServerLocation() const
    {
    TFullName fullPathAndName( KSensrvExeDrive );
    
    fullPathAndName.Append( KDC_PROGRAMS_DIR );
    
    fullPathAndName.Append( KSensrvExeName );
    
    COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::ServerLocation - return %S" ), &fullPathAndName ) );

    return fullPathAndName;
    }

// -----------------------------------------------------------------------------
// RSensrvClient::ServerVersion
// Return version of server
// -----------------------------------------------------------------------------
//
TVersion RSensrvClient::ServerVersion() const 
    {
    return TVersion( KSensrvVersionMajor, KSensrvVersionMinor, KSensrvVersionBuild );
    }


//  End of File  
