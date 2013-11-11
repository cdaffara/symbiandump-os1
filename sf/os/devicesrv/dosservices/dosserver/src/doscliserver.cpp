/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
*    Implementation for the RDosServer class.
*
*/


#include <e32svr.h>
#include <DosSvrServices.h>
#include "dosclientserver.h"
#include "dosserver.h"
#include "dos_debug.h"

// CONSTANTS
const TUid KDosServerUid = {0x101f6efa};
const TInt KConnectionTries = 5;
#if defined(__WINS__)
_LIT( KDosSrvLibName, "DosSrv.dll" );
const TInt KDosSrvThreadStartPos = 103;
#endif

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// RDosServer::StartServer
// Creates the server thread/process
// -----------------------------------------------------------------------------
EXPORT_C TInt RDosServer::StartServer() const
{
    API_TRACE_( "[DOSSERVER] RDosServer::StartServer()" );

    TInt ret(KErrNone);
	TRequestStatus status;
    // IPCv2: TSignal no longer used, but still passed to threadfunction as 
	// otherwise API change would be required.
	CDosServer::TSignal signal( status );

	// Create startup semaphore
	RSemaphore startupSemaphore;
	ret = startupSemaphore.CreateGlobal( KServerStartupSemaphoreName, 0 );

	if ( ret == KErrAlreadyExists )
		{
		// The server is starting up, but has not yet started 
		startupSemaphore.OpenGlobal( KServerStartupSemaphoreName );
		startupSemaphore.Wait(); // wait until the server has started up.
		startupSemaphore.Close();
		return ret;
		}

	// launch server thread (emulator) or process (Target platform)

	RProcess server;

	ret = server.Create( KDosServerExe, signal.Get(),
                       TUidType( KNullUid, KNullUid, KDosServerUid ),
                       EOwnerThread );

	if ( ret )
	{
		startupSemaphore.Close();
		return ret;
	}

	server.SetPriority(EPriorityHigh);

	server.Resume();
	server.Close();

	startupSemaphore.Wait();
	startupSemaphore.Close();

	return KErrNone;
}

// -----------------------------------------------------------------------------
// RDosServer::Connect
// -----------------------------------------------------------------------------
EXPORT_C TInt RDosServer::Connect()
{
    API_TRACE_( "[DOSSERVER] RDosServer::Connect()" );

    TInt err( KErrNone );

    for ( TInt tries = 0; tries < KConnectionTries ; tries++ )
    {
    COM_TRACE_1( "[DOSSERVER]\t RDosServer::Connect() tries: %d", tries );

        err = CreateSession( KDosServerName, TVersion(1,0,0));
        COM_TRACE_1( "[DOSSERVER]\t RDosServer::Connect()-CreateSession err: %d", err );

        // Break if connected to existing server or if the problem is
        // other than missing server.

        if ( err == KErrNone || 
           ( err != KErrNotFound && err != KErrServerTerminated ) )
        {
            break;
        }

        // do not try to start server on last round (since if it failed before with KErrNone
        // (i.e. failure in server side on launch),
        // it is likely to fail again and reset the error code to KErrNone)
        if ( tries < (KConnectionTries - 1) )
		{
	        err = StartServer();    // try start a new server
            COM_TRACE_1( "[DOSSERVER]\t RDosServer::Connect()-StartServer err: %d", err );

			// If server launched ok or someone else got to launch it first,
			// try connect again.

			if ( err != KErrNone && err != KErrAlreadyExists )
			{
				break; // server not launched: don't cycle round again
			}
		}
    }

    return err;
}

// FLAGS STRUCTURE (It always goes in the third (2) position of the parameter array)
//
// KAutoComplete:
// 
// Position: 0 0 0 0 0 0 0 x
// Values:	0 -> AutoComplete is not active.
//			1 -> AutoComplete is active.


// -----------------------------------------------------------------------------
// RDosServer::Close()
// Closes the session.
// -----------------------------------------------------------------------------

EXPORT_C void RDosServer::Close() 
{
    API_TRACE_( "[DOSSERVER] RDosServer::Close()" );

    RSessionBase::Close();  // deletes session object
}


// -----------------------------------------------------------------------------
// RDosServer::ServerShutdown()
// Shutdowns the server
// -----------------------------------------------------------------------------

EXPORT_C void RDosServer::ServerShutdown() const
{
    API_TRACE_( "[DOSSERVER] RDosServer::ServerShutdown()" );

    Send(EServerShutdown, TIpcArgs(TIpcArgs::ENothing, TIpcArgs::ENothing, KAutoComplete));
}

