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
* Description:  Power save mode client class
*
*/


#include <e32svr.h>
#include <data_caging_path_literals.hrh>
#include "psmsession.h"
#include "psmdefines.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// RPsmServer::Connect
// Connects to PSM server
// -----------------------------------------------------------------------------
//
TInt RPsmServer::Connect()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::Connect()" ) ) );

    // Try this twice
    TInt retry( KPsmClientTryCount );
    TInt err( KErrNone );

    while (retry > 0)
        {
        // Try to create a PSM Server session
        err = CreateSession( KPsmProcessName, 
                             ServerVersion(), 
                             KPsmServerDefaultAsyncSlots );

        COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::Connect - CreateSession returned: %d" ), err ) );        
        
        if ( err != KErrNotFound && err != KErrServerTerminated )
            {
            // KErrNone or unrecoverable error
            if (err != KErrNone)
                {
                ERROR_TRACE( ( _L( "PSM Client - RPsmServer::Connect - ERROR: CreateSession unrecoverable error: %d" ), err ) );
                }
            
            retry = 0;
            }
        else
            {
            // Return code was KErrNotFound or KErrServerTerminated.
            // Try to start a new PSM Server
            err = StartServer();

            COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::Connect - StartServer returned: %d" ), err ) );        

            if ( err != KErrNone && err != KErrAlreadyExists )
                {
                // Unrecoverable error
                ERROR_TRACE( ( _L( "PSM Client - RPsmServer::Connect - ERROR: StartServer unrecoverable error: %d" ), err ) );
                retry = 0;
                }
            }
                
        retry--;
        }
        
    COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::Connect - return %d" ), err ) );        
    return err;
    }

// -----------------------------------------------------------------------------
// RPsmServer::SendReceiveSync
// Send message to server
// -----------------------------------------------------------------------------
//
TInt RPsmServer::SendReceiveSync( 
    TPsmServerRequest aRequest, const TIpcArgs& aArgs ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::SendReceiveSync()" ) ) );
    return SendReceive( aRequest, aArgs );
    }

// -----------------------------------------------------------------------------
// RPsmServer::SendReceiveSync
// Send asynchronous message to server
// -----------------------------------------------------------------------------
//
void RPsmServer::SendReceiveAsync( 
    TPsmServerRequest aRequest, const TIpcArgs& aArgs, TRequestStatus& aStatus ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::SendReceiveAsync()" ) ) );
    SendReceive( aRequest, aArgs, aStatus );
    }

// -----------------------------------------------------------------------------
// RPsmServer::StartServer
// Starts server.
// -----------------------------------------------------------------------------
//
TInt RPsmServer::StartServer() const
    {
    COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::StartServer()" ) ) );

    RProcess server;
    const TUidType serverUid( KNullUid, KPsmServerUid2, KNullUid );
    TInt err = server.Create( ServerLocation(), // psmserver.exe
                              KNullDesC, // A descriptor containing data passed as 
                                         // an argument to the thread function of 
                                         // the new process's main thread, when it 
                                         // is first scheduled.
                              serverUid, // PSM server UID
                              EOwnerProcess ); // Ownership of this process handle 

    // Return error code if we couldn't create a process
    if ( err == KErrNone )
        {
        // Rendezvous is used to detect server start
        TRequestStatus stat;
        server.Rendezvous( stat );

        if ( stat != KRequestPending )
            {
            ERROR_TRACE( ( _L( "PSM Client - RPsmServer::StartServer - ERROR: Rendezvous failure: %d" ), stat.Int() ) );
            server.Kill( 0 ); // Abort startup
            }
        else
            {
            server.Resume();  // Logon OK - start the server
            }

        COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::StartServer - Waiting server startup" ) ) );

        User::WaitForRequest( stat ); // Wait for start or death

        COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::StartServer - Server startup wait finished" ) ) );
        
        // We can't use the 'exit reason' if the server paniced as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone
        if ( server.ExitType() == EExitPanic ) 
            {
            ERROR_TRACE( ( _L( "PSM Client - RPsmServer::StartServer - ERROR: Server paniced at startup" ) ) );
            err = KErrGeneral;
            }
        else
            {
            err = stat.Int();
            }

        // We can close the handle now
        server.Close();        
        }

    COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::StartServer - return %d" ), err ) );

    return err;
    }

// -----------------------------------------------------------------------------
// RPsmServer::ServerLocation
// Returns server location
// -----------------------------------------------------------------------------
//
TFullName RPsmServer::ServerLocation() const
    {
    TFullName fullPathAndName( KPsmExeDrive );
    
    fullPathAndName.Append( KDC_PROGRAMS_DIR );
    
    fullPathAndName.Append( KPsmExeName );
    
    COMPONENT_TRACE( ( _L( "PSM Client - RPsmServer::ServerLocation - return %S" ), &fullPathAndName ) );

    return fullPathAndName;
    }

// -----------------------------------------------------------------------------
// RPsmServer::ServerVersion
// Return version of server
// -----------------------------------------------------------------------------
//
TVersion RPsmServer::ServerVersion() const 
    {
    return TVersion( KPsmVersionMajor, KPsmVersionMinor, KPsmVersionBuild );
    }


// End of file
