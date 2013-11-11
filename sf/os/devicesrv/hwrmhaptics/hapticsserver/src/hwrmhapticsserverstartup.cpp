/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics server startup bootstrap.
*
*/


#include "hwrmhapticsserver.h"
#include "hwrmhapticsserveractivescheduler.h"
#include "hwrmhapticsconfiguration.h"  
#include "hwrmhapticstrace.h"


TInt E32Main(); // Process entry point


// ---------------------------------------------------------------------------
// Function that starts the HWRMHapticsServer.
// ---------------------------------------------------------------------------
//
static TInt StartServerL()
    {
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - StartServerL()" )) );

    User::LeaveIfError( RThread().RenameMe( KServerProcessName ) );
    RThread().SetPriority( EPriorityAbsoluteHighNormal );

    // create active scheduler
    CHWRMHapticsServerActiveScheduler* scheduler = 
        CHWRMHapticsServerActiveScheduler::NewL();
    
    CleanupStack::PushL( scheduler );
    
    // install scheduler
    CActiveScheduler::Install( scheduler );

    // create server instance
    CHWRMHapticsServer* server = 
        CHWRMHapticsServer::NewL( KServerCActivePriority );

    RProcess::Rendezvous( KErrNone );

    // start scheduling
    CActiveScheduler::Start();
    
    delete server; 
    CleanupStack::PopAndDestroy( scheduler ); 
    
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - StartServerL() - return" )) );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Main function
// ---------------------------------------------------------------------------
//
TInt E32Main()
    {
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - E32Main()" )) );

    __UHEAP_MARK;

    // create cleanupstack
    CTrapCleanup* cleanup = CTrapCleanup::New();

    TInt ret( KErrNone );

    if( cleanup )
        {
        TRAP( ret, StartServerL() );
        delete cleanup;
        }

    __UHEAP_MARKEND;

    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - E32Main - return %d" ), ret ) );

    return ret;
    }

//  End of File  
