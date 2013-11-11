/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server connection handler.
*
*/



// INCLUDE FILES
#include    <e32math.h>
#include    <e32uid.h>
#include    "PhCltTypes.h"
#include    "RPhCltServer.h"
#include    "PhCltClientServer.h"

// CONSTANTS

// Information of Phone Server Startup failure, used in panic call.
_LIT( PH_SRV_SU_FAILURE, "PhSrvStartUp" );

// The number of Phone Server startup attempts.
const TInt KPhCltNumberOfRetryAttempts = 4;

// The phase were the Phone Server failure happened.
// Useful in debug purposes if Phone Server startup fails.
enum 
    {
    EPhSrvStartupFailurePhase1,  // First phase
    EPhSrvStartupFailurePhase2   // Second phase
    };     



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RPhCltServer::RPhCltServer
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPhCltServer::RPhCltServer() 
    : RSessionBase()
    {
    }

// -----------------------------------------------------------------------------
// RPhCltServer::Version
// 
// Returns version.
// -----------------------------------------------------------------------------
//
TVersion RPhCltServer::Version() const
    {
    return TVersion( KPhoneServerMajorVersionNumber,
                     KPhoneServerMinorVersionNumber,
                     KPhoneServerBuildVersionNumber );
    }

// -----------------------------------------------------------------------------
// RPhCltServer::Connect
// 
// Creates connection to the server. If server is not running,
// it is also launched.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltServer::Connect( TInt aMessageSlots )
    {
    TInt err = KErrGeneral;
    TInt count = KPhCltNumberOfRetryAttempts;
    FOREVER
        {
        err = CreateSession( PHONE_SERVER_NAME, Version(), aMessageSlots );
        if ( err != KErrNotFound && err != KErrServerTerminated )
            {
            break;
            }
        // Need to restart server
        if ( --count < 0 )
            {
            break;
            }
        err = StartServer();
        if ( err != KErrNone && err != KErrAlreadyExists )
            {
            if ( err != KErrNotFound )
                {
                break;
                }
            Panic( EPhCltPanicServerUnexpectedServerNotFound );
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// RPhCltServer::CreateAll
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltServer::CreateAll( TRequestStatus& aStatus )
    {
    SendReceive( 
        EPhoneCreateAll,
        TIpcArgs(),
        aStatus );
    }

// -----------------------------------------------------------------------------
// RPhCltServer::StartServer
// 
// Launches the phone server, which is assumed to be not
// already running.
// -----------------------------------------------------------------------------
//
TInt RPhCltServer::StartServer()
    {
    // The uid of the DLL/EXE - used to identify the correct target
    const TUidType KPhServerUidType( 
        KExecutableImageUid, 
        KSharedLibraryUid, 
        KPhCltServerUid );
    

    //////////////////////////////////////////////////////////////////////////
    // On MARM, the server is an exe, so it is run inside its own process. 
    // The server itself takes care of creating a thread and finalizing the 
    // construction of the server. We pass the signalling object via the 
    // command line.
    //////////////////////////////////////////////////////////////////////////

    RProcess serverUnitOfExecution;

    TFileName pathName( KPhCltServerZDrive );
    pathName.Append( KDC_PROGRAMS_DIR );
    pathName.Append( KPhServerPathAndNameMARM );
    pathName.ZeroTerminate();

    TInt err = serverUnitOfExecution.Create( 
        pathName,
        KNullDesC, 
        KPhServerUidType );

    if ( err != KErrNone )
        {
        User::Panic( PH_SRV_SU_FAILURE, err );
        }
    serverUnitOfExecution.SetPriority( EPriorityHigh );


    // Now wait for the server to start up, and also observe if the server
    // thread dies for any reason.


	TRequestStatus stat;
	serverUnitOfExecution.Rendezvous( stat );
 	if ( stat != KRequestPending )
        {
        User::Panic( PH_SRV_SU_FAILURE, EPhSrvStartupFailurePhase1 );

 		serverUnitOfExecution.Kill( KErrAbort );	// abort startup
        }
 	else
        {
 		serverUnitOfExecution.Resume();	// logon ok
        }

    // wait for start or death
 	User::WaitForRequest( stat ); 
 	// we can't use the 'exit reason' if the server panicked as this
 	// is the panic 'reason' and may be '0' which cannot be distinguished
 	// from KErrNone
 	err = ( serverUnitOfExecution.ExitType() == EExitPanic ) 
        ? KErrGeneral : stat.Int();

    serverUnitOfExecution.Close();


    return err;
    }


// -----------------------------------------------------------------------------
// RPhCltServer::Panic
// 
// Panics the client thread
// -----------------------------------------------------------------------------
//
void RPhCltServer::Panic( TPhCltPanicTypes aPanic )
    {
    User::Panic( KPhClientPanicCategory, aPanic );
    }




//  End of File  
