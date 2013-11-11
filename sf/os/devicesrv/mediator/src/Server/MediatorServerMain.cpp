/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Main functions to start the server
*
*/


// INCLUDE FILES
#include "MediatorServer.h"
#include "MediatorCommon.h"
#include "Debug.h"

// ============================= LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RunServerL
// 
// Constructs Active Scheduler and starts the server up & running
// -----------------------------------------------------------------------------
//
static void RunServerL()
	{
	LOG(_L("[Mediator Server]\t RunServerL"));
	// Create and install the active scheduler we need
	CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;
	CleanupStack::PushL( scheduler );
	CActiveScheduler::Install( scheduler );

	// Create server
	CMediatorServer* server = CMediatorServer::NewLC();

	// Initialisation complete, now signal the client
	User::LeaveIfError( User::RenameThread( KMediatorServerProcessName ) ); // renaming eases debugging in panic cases
	RProcess::Rendezvous( KErrNone );

	// Ready to run
	CActiveScheduler::Start();

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy( server );
	CleanupStack::PopAndDestroy( scheduler );
	}

// -----------------------------------------------------------------------------
// E32Main
//
// Module entry point
// -----------------------------------------------------------------------------
//
TInt E32Main()
	{
	LOG(_L("[Mediator Server]\t E32Main"));
	
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt error = KErrNoMemory;
    if ( cleanup )
        {
        TRAP( error, RunServerL() );
        delete cleanup;
        ERROR_TRACE(Print(_L("[Mediator] E32Main: error=%d\n"), error ) );
        }
    __UHEAP_MARKEND;
    
    return error;
	}

// End of File
