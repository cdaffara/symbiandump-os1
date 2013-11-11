/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Client-side handle to a session with a Accessory Server.
*
*/



// INCLUDE FILES
#include "AccessoryServer.h"
#include "AccClientServerMessages.h"
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

#ifdef ACC_START_SERVER
//------------------------------------------------------------------------------
// Start accessory server.
//------------------------------------------------------------------------------
//
static TInt StartServer()
    {
	TInt ret(KErrNone);
	// create server - if one of this name does not already exist
	TFindServer findServer( KAccServerName );
	TFullName name;
	if( findServer.Next( name ) != KErrNone ) // we don't exist already
		{
		TRequestStatus status;
		RProcess server;
		// Create the server process
		_LIT(KExServerExe,"AccServer.exe");
		ret = server.Create( KExServerExe, KNullDesC );		
		if( ret != KErrNone ) // thread created ok - now start it going
			{
			return ret;
			}
		// Process created successfully
		server.Rendezvous( status );
		if( status != KRequestPending )
			{
			server.Kill( KErrNone ); // abort startup
			server.Close();
			return KErrGeneral;  // status.Int() can be KErrNone!
			}
		else
			{
			server.Resume();	// logon OK - start the server
			}
		// Wait until the completion of the server creation
		User::WaitForRequest( status );
			
		// We can't use the 'exit reason' if the server paniced as this
		// is the panic 'reason' and may be '0' which cannot be distinguished
		// from KErrNone
		ret = (server.ExitType() == EExitPanic)? KErrGeneral : status.Int();
			
		// Server created successfully
		server.Close(); // we're no longer interested in the other process
		}
    return ret;
    }
#endif//ACC_START_SERVER

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RAccessoryServer::RAccessoryServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessoryServer::RAccessoryServer():RSessionBase()
    {
    API_TRACE_( "[AccFW:Client] RAccessoryServer::RAccessoryServer()" );
    }

// -----------------------------------------------------------------------------
// RAccessoryServer::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryServer::Connect()
    {
    API_TRACE_( "[AccFW:Client] RAccessoryServer::Connect()" );

    TInt error = KErrNone;
    
#ifdef ACC_START_SERVER // Only for testing purposes
    error = CreateSession( KAccServerName, Version(), KDefaultMessageSlots );
    if( error == KErrNotFound )
        {        
		error = StartServer();
		if( error == KErrNone )
		    {
		    error = CreateSession( KAccServerName, Version(), KDefaultMessageSlots );		    
		    }
		}
#else
    error = CreateSession(KAccServerName, Version(), KDefaultMessageSlots);
#endif
    
    API_TRACE_1( "[AccFW:Client] RAccessoryServer::Connect - return %d", error );

    return error;
    }

// -----------------------------------------------------------------------------
// RAccessoryServer::Disconnect
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryServer::Disconnect()
    {
    API_TRACE_( "[AccFW:Client] RAccessoryServer::Disconnect()" );

    RSessionBase::Close();

    API_TRACE_( "[AccFW:Client] RAccessoryServer::Disconnect - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryServer::Version
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion RAccessoryServer::Version() const
    {
    API_TRACE_( "[AccFW:Client] RAccessoryServer::Version()" );

    API_TRACE_3( "[AccFW:Client] RAccessoryServer::Version - return 0x%x 0x%x 0x%x", 
        KAccServMajorVersionNumber,
        KAccServMinorVersionNumber,
        KAccServBuildVersionNumber );

    return( TVersion( 
            KAccServMajorVersionNumber, 
            KAccServMinorVersionNumber, 
            KAccServBuildVersionNumber ) );
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
