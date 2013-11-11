/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Thread creater for SAE
*
*/


// INCLUDE FILES
#include "SaeThread.h"
#include "sae_debug.h"
#include <e32svr.h>
#include <e32std.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSaeThread::CSaeThread
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSaeThread::CSaeThread()
    {
    COM_TRACE_( "[DosServer:SAE] CSaeThread::CSaeThread()" );

    COM_TRACE_( "[DosServer:SAE] CSaeThread::CSaeThread" );
    }

// -----------------------------------------------------------------------------
// CSaeThread::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSaeThread::ConstructL()
    {
    COM_TRACE_( "[DosServer:SAE] CSaeThread::ConstructL()" );


    COM_TRACE_( "[DosServer:SAE] CSaeThread::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CSaeThread::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSaeThread* CSaeThread::NewL()
    {
    COM_TRACE_( "[DosServer:SAE] CSaeThread::NewL()" );

    CSaeThread* self = new ( ELeave ) CSaeThread();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[DosServer:SAE] CSaeThread::NewL - return 0x%x", &self );

    return self;
    }

// Destructor
CSaeThread::~CSaeThread()
    {
    COM_TRACE_( "[DosServer:SAE] CSaeThread::~CSaeThread()" );

    // CActiveScheduler::Stop();

    COM_TRACE_( "[DosServer:SAE] CSaeThread::~CSaeThread" );
    }

// -----------------------------------------------------------------------------
// CSaeThread::StartThreadL
// Creates a new thread
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSaeThread::StartThreadL()
    {
    COM_TRACE_( "[DosServer:SAE] CSaeThread::StartThreadL()" );
    
    // Generate unique thread name
    HBufC* bufThreadName = HBufC::NewL( KSAEThreadName().Length() );

    CleanupStack::PushL( bufThreadName );
    TPtr ptrThreadName = bufThreadName->Des();
    ptrThreadName = KSAEThreadName;

    COM_TRACE_1( "[DosServer:SAE] CSaeThread::StartThreadL(%S)", &ptrThreadName );
 
    User::LeaveIfError( iThread.Create(
                        ptrThreadName,
                        ThreadEntryPoint,
                        KDefaultStackSize,
                        NULL,
                        NULL ) );

    CleanupStack::Pop ( bufThreadName );
    
    iThread.Resume();

    delete bufThreadName;

    COM_TRACE_( "[DosServer:SAE] CSaeThread::StartThreadL - return void" );
    }

// -----------------------------------------------------------------------------
// CSaeThread::ThreadEntryPoint
// Entry point of new thread
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSaeThread::ThreadEntryPoint( TAny* /*aParameter*/ )
    {
    COM_TRACE_( "[DosServer:SAE] CSaeThread::ThreadEntryPoint()" );

    TInt err = KErrNone;

    //create cleanupstack
    CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
    if ( !cleanup )
        err = KErrNoMemory;

    if ( !err )
        {
        TRAP( err, DoStartThreadL() );
        }
    delete cleanup;

    COM_TRACE_1( "[DosServer:SAE] CSaeThread::ThreadEntryPoint - return %d", err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSaeThread::DoStartThreadL
// Starts new thread by creating and starting active scheduler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSaeThread::DoStartThreadL()
    {
    COM_TRACE_( "[DosServer:SAE] CSaeThread::DoStartThreadL()" );

    CActiveScheduler* actSched = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( actSched );
    CActiveScheduler::Install( actSched );

    TInt res = CreateSaeLC();

    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( 2 ); // saeObj, actSched

    COM_TRACE_1( "[DosServer:SAE] CSaeThread::DoStartThreadL - return %d", res );

    return ( res );
    }

// -----------------------------------------------------------------------------
// CSaeThread::CreateSaeLC
// Creates a new instance of SAE
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSaeThread::CreateSaeLC()
    {
    COM_TRACE_( "[DosServer:SAE] CSaeThread::CreateSaeLC()" );

    TInt res( KErrNone );

    CSae* saeObj;
    saeObj = CSae::NewL();
    
    CleanupStack::PushL( saeObj );

    COM_TRACE_1( "[DosServer:SAE] CSaeThread::CreateSaeLC - return %d", res );

    return res;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
