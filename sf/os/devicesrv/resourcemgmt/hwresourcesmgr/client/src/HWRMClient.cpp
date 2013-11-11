// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDE FILES
#include <e32svr.h>
#include "HWRMClient.h"
#include "HWRMConfiguration.h"
#include "HWRMClientServer.h"
#include "HWRMtrace.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
_LIT(KSysBin, "\\sys\\bin\\");

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

/*
// -----------------------------------------------------------------------------
// RHWRMClient::RHWRMClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RHWRMClient::RHWRMClient()
    : RSessionBase()
    {
    }
*/

// -----------------------------------------------------------------------------
// RHWRMClient::Connect
// Connects to server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHWRMClient::Connect(THWRMResourceType aType)
    {
   COMPONENT_TRACE2(_L( "HWRM Client - RHWRMClient::Connect(0x%x)" ), aType);

    // Try this twice
    TInt retry(3);
    TInt err(KErrNone);

    while (retry > 0)
        {
        // Try to create a HWRM Server session
        COMPONENT_TRACE2(_L( "HWRM Client - RHWRMClient::Connect - CreateSession retry : %d" ), retry );        
        err = CreateSession( KServerProcessName, 
                                  ServerVersion(), 
                                  KDefaultAsyncSlots );

        COMPONENT_TRACE2(_L( "HWRM Client - RHWRMClient::Connect - CreateSession returned: %d" ), err );        
        
        if ( err != KErrNotFound && err != KErrServerTerminated )
            {
            // KErrNone or unrecoverable error
            break;
            }
        else
            {
            // Return code was KErrNotFound or KErrServerTerminated.
            // Try to start a new HWRM Server
            err = StartServer();

            COMPONENT_TRACE2(_L( "HWRM Client - RHWRMClient::Connect - StartServer returned: %d" ), err );        

            if ( err != KErrNone && err != KErrAlreadyExists )
                {
                // Unrecoverable error
                break;
                }
            }
                
        retry--;
        }
        
    // Create proper service on server side
    if ( err == KErrNone )
        {
        switch ( aType )
            {
            case HWRMResourceTypeVibra:
                err = SendReceive(EHWRMCreateVibraService, TIpcArgs());
                break;
            case HWRMResourceTypeLight:
                err = SendReceive(EHWRMCreateLightService, TIpcArgs());
                break;
            case HWRMResourceTypePower:
            	err = SendReceive(EHWRMCreatePowerService, TIpcArgs());
				break;
            case HWRMResourceTypeFmTx:
            	err = SendReceive(EHWRMCreateFmTxService, TIpcArgs());
            	break;
            default:
                err = KErrNotSupported;
            }
        }
        
    COMPONENT_TRACE2( _L( "HWRM Client - RHWRMClient::Connect - return %d" ), err );        
    return err;
    }


// -----------------------------------------------------------------------------
// RHWRMClient::ExecuteOperation
// Calls SendReceive with given parameters.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHWRMClient::ExecuteOperation(TInt aCommand, const TIpcArgs& aArgs) const
    {
    COMPONENT_TRACE2(_L( "HWRM Client - RHWRMClient::ExecuteOperation(0x%x, <aArgs>)" ), aCommand );
    
    TInt retval = SendReceive(aCommand, aArgs);
    
    COMPONENT_TRACE2(_L( "HWRM Client - RHWRMClient::ExecuteOperation - return %d" ), retval );
    
    return retval;

    }

// -----------------------------------------------------------------------------
// RHWRMClient::ExecuteAsyncOperation
// Calls SendReceive with given parameters.
// -----------------------------------------------------------------------------
//
EXPORT_C void RHWRMClient::ExecuteAsyncOperation(TInt aCommand, const TIpcArgs& aArgs, TRequestStatus& aStatus ) const
	{
	COMPONENT_TRACE3( _L( "HWRM Client - RHWRMClient::ExecuteAsyncOperation(0x%x, <aArgs>, 0x%x)" ), aCommand, aStatus.Int() );

	SendReceive(aCommand, aArgs, aStatus);

	COMPONENT_TRACE1( _L( "HWRM Client - RHWRMClient::ExecuteAsyncOperation - return" ) );
	}
// -----------------------------------------------------------------------------
// RHWRMClient::StartServer
// Starts server.
// -----------------------------------------------------------------------------
//
TInt RHWRMClient::StartServer() const
    {
    COMPONENT_TRACE1(_L( "HWRM Client - RHWRMClient::StartServer()" ) );

    RProcess server;
    const TUidType serverUid( KNullUid, KServerUid2, KNullUid );
    TInt err = server.Create( ServerLocation(), // HWRMServer.exe
                           KNullDesC, // A descriptor containing data passed as 
                                      // an argument to the thread function of 
                                      // the new process's main thread, when it 
                                      // is first scheduled.
                           serverUid, // HWRM server UID
                           EOwnerProcess ); // Ownership of this process handle 

    // Return error code if we couldn't create a process
    if ( err == KErrNone )
        {
        // Rendezvous is used to detect server start
        TRequestStatus stat;
        server.Rendezvous( stat );

        if ( stat != KRequestPending )
            {
            server.Kill( stat.Int() ); // Abort startup
            }
        else
            {
            server.Resume();  // Logon OK - start the server
            }

        COMPONENT_TRACE1(_L( "HWRM Client - RHWRMClient::StartServer - Waiting server startup" ) );

        User::WaitForRequest( stat ); // Wait for start or death

        COMPONENT_TRACE1(_L( "HWRM Client - RHWRMClient::StartServer - Server startup wait finished" ) );
        
        // We can't use the 'exit reason' if the server paniced as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone
        err = (server.ExitType() == EExitPanic)? KErrGeneral : stat.Int();

        // We can close the handle now
        server.Close();        
        }

    COMPONENT_TRACE2(_L( "HWRM Client - RHWRMClient::StartServer - return %d" ), err );

    return err;
    }

// -----------------------------------------------------------------------------
// RHWRMClient::ServerLocation
// Returns server location
// -----------------------------------------------------------------------------
//
TFullName RHWRMClient::ServerLocation() const
    {
    TFullName fullPathAndName( KServerExeDrive );
    fullPathAndName.Append( KSysBin );
    fullPathAndName.Append( KServerExeName );
    
    COMPONENT_TRACE2(_L( "HWRM Client - RHWRMClient::ServerLocation - return %S" ), &fullPathAndName );

    return fullPathAndName;
    }

// -----------------------------------------------------------------------------
// RHWRMClient::ServerVersion
// Return version of server
// -----------------------------------------------------------------------------
//
TVersion RHWRMClient::ServerVersion() const 
    {
    return TVersion( KServerVersionMajor, KServerVersionMinor, KServerVersionBuild );
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File  
