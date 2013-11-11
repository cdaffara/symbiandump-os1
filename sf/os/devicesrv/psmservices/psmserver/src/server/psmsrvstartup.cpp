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
* Description:  Contains methods handling startup of the server process.
*
*/


#include <e32base.h>
#include "psmsrvserver.h"
#include "psmdefines.h"
#include "psmtrace.h"


TInt E32Main( void ); // Process entry point

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Function that starts the psmserver.
// -----------------------------------------------------------------------------
//
static void RunServerL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - RunServer()" ) ) );

    // Naming the server process and thread after the startup helps to debug panics
    // No error checking as names are not critical for operation
    User::RenameProcess( KPsmProcessName );
    User::RenameThread( KPsmMainThreadName );

    // Use process default priority (EPriorityForgeround)

    // Create and install the active scheduler we need
    CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler();
    CleanupStack::PushL( scheduler );

    CActiveScheduler::Install( scheduler );

    // Now we are ready to instantiate the actual server instance
    CPsmSrvServer* server = CPsmSrvServer::NewLC();

    // Initialisation complete, now signal the client
    RProcess::Rendezvous( KErrNone );

    // Ready to run
    COMPONENT_TRACE( ( _L( "PSM Server - RunServer() - Starting scheduler..." ) ) );
    CActiveScheduler::Start();
    COMPONENT_TRACE( ( _L( "PSM Server - RunServer() - Scheduler stopped" ) ) );

    // Cleanup
    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( scheduler );

    COMPONENT_TRACE( ( _L( "PSM Server - RunServer() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Main function
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - E32Main()" ) ) );

    __UHEAP_MARK;

#ifdef MEMORY_TRACE_DEBUG
    // TRACE heap usage
    TInt heapSize = User::Heap().Size();
    TInt biggestBlock(0);
    TInt heapAvail = User::Heap().Available( biggestBlock );
    TInt used( heapSize-heapAvail );
    MEMORY_TRACE( ( _L( "#### PSM Server, main thread starting - HEAP: Size: %d, Available: %d, Used: %d" ), heapSize, heapAvail, used ) );
#endif

    TInt err( KErrNone );

    // Start server.
    CTrapCleanup* cleanup = CTrapCleanup::New();
    err = KErrNoMemory;

    if ( cleanup )
        {
        TRAP( err, RunServerL() );
        delete cleanup;
        cleanup = NULL;
        }

    COMPONENT_TRACE( ( _L( "PSM Server - E32Main - return %d" ), err ) );

#ifdef MEMORY_TRACE_DEBUG
    // TRACE heap usage
    heapSize = User::Heap().Size();
    heapAvail = User::Heap().Available( biggestBlock );
    TInt newUsed( heapSize-heapAvail );
    MEMORY_TRACE( ( _L( "#### PSM Server, main thread exit - HEAP: Size: %d, Available: %d, Used: %d, Change in used: %d" ), heapSize, heapAvail, newUsed, newUsed - used ) );
    // Print out the heap contents
#endif

    __UHEAP_MARKEND;

    return err;
    }

//  End of File
