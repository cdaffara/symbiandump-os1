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
* Description:  Contains methods handling startup of the test process.
*
*/


//#include <e32base.h>
//#include "sensrvserver.h"
//#include "sensrvdefines.h"  
#include "sensrvtrace.h"
//#include "sensrvssyactivescheduler.h"
#include "SensrvTestObserver.h" 

TInt E32Main(void); // Process entry point

_LIT( KSensrvTestProcessName, "SenSrvTestProcess%Ld" );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Function that starts the test process.
// -----------------------------------------------------------------------------
//
static void RunServerL( const TDesC &aParameter )
    {
    COMPONENT_TRACE( ( _L( "SenSrvTestProcess - RunServerL(name=%S)" ),&aParameter ) );
   
    TInt processID( 0 );
    TLex parameter( aParameter );
    parameter.Val( processID );   
    
    TTime now;        
    now.HomeTime();
    TBuf<50> processName;           
    processName.Format( KSensrvTestProcessName, now.Int64() ); 
    User::RenameProcess( processName );
   
   
    // Set process priority
    RProcess svrProcess;
    svrProcess.SetPriority( EPriorityHigh );
    
    // Create and install the active scheduler we need
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
              
    CActiveScheduler::Install( scheduler );
        
    CSensrvTestObserver* observer = CSensrvTestObserver::NewLC( processID );
    
    
    // Initialisation complete, now signal the client
    RProcess::Rendezvous(KErrNone);

    // Ready to run
    COMPONENT_TRACE( ( _L( "SenSrvTestProcess - RunServer() - Starting scheduler..." )) );
    CActiveScheduler::Start();
    
    COMPONENT_TRACE( ( _L( "SenSrvTestProcess - RunServer() - Scheduler stopped" )) );

    // Cleanup
    CleanupStack::PopAndDestroy(observer);
    CleanupStack::PopAndDestroy(scheduler);
    
    COMPONENT_TRACE( ( _L( "SenSrvTestProcess - RunServer() - return" )) );
    }

// -----------------------------------------------------------------------------
// Main function
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    COMPONENT_TRACE( ( _L( "SenSrvTestProcess - E32Main()" )) );

    __UHEAP_MARK;

    TInt err(KErrNone);
    TBuf<3> parameter;
    User::CommandLine( parameter );

    // Start server.
    CTrapCleanup* cleanup = CTrapCleanup::New();
    err = KErrNoMemory;

    if ( cleanup )
    {
        TRAP(err, RunServerL( parameter ));

        delete cleanup;
        cleanup = NULL;
    }

    COMPONENT_TRACE( ( _L( "SenSrvTestProcess - E32Main - return %d" ), err ) );

    __UHEAP_MARKEND;
    
    return err;
    }

//  End of File  
