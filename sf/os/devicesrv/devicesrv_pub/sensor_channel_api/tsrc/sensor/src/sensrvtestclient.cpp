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
* Description:   Sensor server client side connector class
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <data_caging_path_literals.hrh>  
#include "sensrvdefines.h"
#include "sensrvtestclient.h"
#include "sensrvclientserver.h"
#include "sensrvtrace.h"


// -----------------------------------------------------------------------------
// RSensrvTestClient::Connect
// Connects to server
// -----------------------------------------------------------------------------
//
TInt RSensrvTestClient::Connect()
    {
   COMPONENT_TRACE( ( _L( "Sensrv Test Client - RSensrvTestClient::Connect()" ) ) );

    // Try this twice
    TInt err(KErrNone);

    // Try to create a Sensor Server session
        err = CreateSession( KSensrvProcessName, 
                             ServerVersion(), 
                             KSensrvDefaultAsyncSlots );

        COMPONENT_TRACE( ( _L( "Sensrv Test Client - RSensrvTestClient::Connect - CreateSession returned: %d" ), err ) );        
        
               
    return err;
    }
    
// -----------------------------------------------------------------------------
// RSensrvTestClient::StartServer
// Starts server.
// -----------------------------------------------------------------------------
//
TInt RSensrvTestClient::StartServer() const
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
// RSensrvTestClient::ServerLocation
// Returns server location
// -----------------------------------------------------------------------------
//
TFullName RSensrvTestClient::ServerLocation() const
    {
    TFullName fullPathAndName( KSensrvExeDrive );
    
    fullPathAndName.Append( KDC_PROGRAMS_DIR );
    
    fullPathAndName.Append( KSensrvExeName );
    
    COMPONENT_TRACE( ( _L( "Sensrv Client - RSensrvClient::ServerLocation - return %S" ), &fullPathAndName ) );

    return fullPathAndName;
    }

// -----------------------------------------------------------------------------
// RSensrvTestClient::ServerVersion
// Return version of server
// -----------------------------------------------------------------------------
//
TVersion RSensrvTestClient::ServerVersion() const 
    {
    return TVersion( KSensrvVersionMajor, KSensrvVersionMinor, KSensrvVersionBuild );
    }


TInt RSensrvTestClient::ShutdownServer()
    {
    return SendReceive( ESensrvSrvReqShutdownServer, TIpcArgs() );
    }


//  End of File  
