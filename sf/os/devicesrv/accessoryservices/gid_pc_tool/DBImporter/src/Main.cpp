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
* Description:  Accessory Database Import Tool
*
*/


// INCLUDE FILES
#include "AccDBImporter.h"
#include "acc_debug.h"
#include <e32base.h>
#include <e32cons.h>
#include <e32std.h>
#include <e32svr.h>
#include <s32strm.h>
#include <s32mem.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT( KPanicNote, "DBImport failed" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// MainL() 
// Main function of the executable - called by E32Main()
// -----------------------------------------------------------------------------
//
void MainL()
    {
    COM_TRACE_( "[ACCPOLICYDBIMP] DB creation started" );

    TBool deinstallActiveScheduler = EFalse;
    CActiveScheduler* myActiveScheduler = NULL;

    if( !CActiveScheduler::Current() )
        {
        myActiveScheduler = new (ELeave) CActiveScheduler;
        CleanupStack::PushL( myActiveScheduler );
        CActiveScheduler::Install( myActiveScheduler );
        deinstallActiveScheduler = ETrue;
        }

    CAccDBImporter* dbImporter = CAccDBImporter::NewL();
    CleanupStack::PushL( dbImporter );
    dbImporter->DoImportL();
    CleanupStack::Pop( dbImporter );
    
    delete dbImporter;
    dbImporter = NULL;

    if( deinstallActiveScheduler )
        {
        CActiveScheduler::Install( NULL );
        CleanupStack::Pop( myActiveScheduler );
        delete myActiveScheduler;
        }

    COM_TRACE_( "[ACCPOLICYDBIMP] All done." );
    }

// -----------------------------------------------------------------------------
// E32Main() 
// E32Main function of the executable.
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    TRAPD( error, MainL() );
    __ASSERT_ALWAYS( !error, User::Panic( KPanicNote, error ) );

    delete cleanupStack;
    return 0;
    }

// End of File
