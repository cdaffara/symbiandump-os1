// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "HWRMServer.h"
#include "HWRMServerActiveScheduler.h"
#include "HWRMConfiguration.h"  
#include "HWRMtrace.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
TInt E32Main(void); // Process entry point

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Function that starts the HWRMServer.
// -----------------------------------------------------------------------------
//
static TInt RunServer()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - RunServer()" ));

    RSemaphore startLock;
    CHWRMServerActiveScheduler* scheduler = NULL;
    CHWRMServer* server = NULL;

    // Create lock
    TInt err = startLock.CreateGlobal( KServerSemaphoreName, 1, EOwnerProcess );
    
    if ( err == KErrAlreadyExists )
        {
        COMPONENT_TRACE1(_L( "HWRM Server - RunServer() - Semaphore already exists, opening existing one." ));
        err = startLock.OpenGlobal( KServerSemaphoreName );
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Server - RunServer() - Created a new semaphore." ));
        }
    
    // If there is error, server doesn't need to be started as it is either
    // very recently started (i.e. semaphore doesn't exist anymore even though creating it failed)
    // or there is something very wrong in system.
    if ( err == KErrNone )
        {
        COMPONENT_TRACE1(_L( "HWRM Server - RunServer() - Waiting on a semaphore." ));
        startLock.Wait();
        COMPONENT_TRACE1(_L( "HWRM Server - RunServer() - Semaphore wait over." ));
        
        // Check if HWRMServer process is already running.  
        TFindProcess processFinder(KServerFindPattern);
        TFullName processName;       
        err = processFinder.Next( processName );
        
        COMPONENT_TRACE2(_L( "HWRM Server - RunServer() - Process find result: %d." ), err);
        
        // If HWRMServer process is not yet running, properly start it
        if ( err == KErrNotFound )
            {
            // Naming the server process and thread after the startup helps to debug panics
            TInt renameErr1 = User::RenameProcess(KServerProcessName);
            if (renameErr1 != KErrNone)
            	{
            	COMPONENT_TRACE2(_L( "HWRM Server - RunServer() - Error %d renaming server process" ), renameErr1);
            	}
            TInt renameErr2 = User::RenameThread(KServerProcessName);
            if (renameErr2 != KErrNone)
            	{
            	COMPONENT_TRACE2(_L( "HWRM Server - RunServer() - Error %d renaming server thread" ), renameErr2);
            	}
           
            // Create and install the active scheduler we need
            if ( ( renameErr1 == KErrNone ) && (renameErr2 == KErrNone) )
                {
                TRAP( err, scheduler = CHWRMServerActiveScheduler::NewL() );
                          
                // Note: Cannot use leaves and cleanupStack to reduce ifs here, because semaphore needs to be signaled in any case.
                if ( err == KErrNone )
                    {
                    CActiveScheduler::Install( scheduler );
                    
                    // Now we are ready to instantiate the actual CHWRMServer instance
                    TRAP( err, server = CHWRMServer::NewL( KServerCActivePriority ) );

                    if ( err == KErrNone )
                        {
                    	// Initialisation complete, now signal the client
                    	RProcess::Rendezvous(KErrNone);

                        // Server is now up and running, so signal semaphore
                        COMPONENT_TRACE1(_L( "HWRM Server - RunServer() - Signaling semaphore." ));
                        startLock.Signal();
                        startLock.Close(); // close already here to delete unnecessary reference

                    	// Ready to run
                        COMPONENT_TRACE1(_L( "HWRM Server - RunServer() - Starting scheduler..." ));
                    	CActiveScheduler::Start();
                    	
                        COMPONENT_TRACE1(_L( "HWRM Server - RunServer() - Scheduler stopped" ));
                        }
                    }
                }

            // In case there was any error, signal the semaphore as it has not been yet signaled
            if ( ( err != KErrNone ) || ( renameErr1 != KErrNone ) || ( renameErr2 != KErrNone ) )
                {
                COMPONENT_TRACE1(_L( "HWRM Server - RunServer() - error occurred - signaling semaphore." ) );
                startLock.Signal();
                // make sure we return an error code if there was an error
                if (err == KErrNone)
                	{
                	err = (renameErr1 == KErrNone) ? renameErr2 : renameErr1;
                	}
                }
            }
        else // Probably KErrNone
            {
            // Already running, do not restart
            COMPONENT_TRACE1(_L( "HWRM Server - RunServer() - HWRM Server already started, signaling semaphore" ) );
            startLock.Signal();
            }
        }
    else
        {
        COMPONENT_TRACE2(_L( "HWRM Server - RunServer() - Problems opening startup semaphore: %d." ), err);
        }
        
    // Cleanup
    startLock.Close();
    delete server;
    delete scheduler;
    
    COMPONENT_TRACE2(_L( "HWRM Server - RunServer() - return %d" ), err);
    return err;
    }

// -----------------------------------------------------------------------------
// Main function
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - E32Main()" ));

	__UHEAP_MARK;

  	TInt ret(KErrNone);

    // No process with same name found, start server.
	CTrapCleanup* cleanup = CTrapCleanup::New();
	ret = KErrNoMemory;

	if ( cleanup )
		{
		ret = RunServer();
		
		delete cleanup;
		}
    
	__UHEAP_MARKEND;

    COMPONENT_TRACE2(_L( "HWRM Server - E32Main - return %d" ), ret );

	return ret;
    }

//  End of File  
