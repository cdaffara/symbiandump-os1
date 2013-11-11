/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server side server
*
*/


// INCLUDE FILES
#include "AccServer.h"
#include "AccClientServerMessages.h"
#include "AccSrvMainSession.h"
#include "AccSrvConnectionController.h"
#include "AsyProxyStarter.h"
#include "AsyProxyAPI.h"
#include "AccSrvServerModel.h"
#include "AccSrvIdleListener.h"
#include "acc_debug.h"
#include <e32svr.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PanicClient Causes client to panic.
// Returns: void
// -----------------------------------------------------------------------------
//
void PanicClient(
    const RMessage2& aMessage,  // Message
    TAccServerError aPanic )    // Panic error
    {
    COM_TRACE_1( "[AccFW:AccServer] PanicClient(0x%x)", aPanic );

    aMessage.Panic( KAccServerName, aPanic );

    COM_TRACE_( "[AccFW:AccServer] PanicClient - return void" );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccServer::CAccServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccServer::CAccServer( TInt aPriority, TServerType aType ) :
    CPolicyServer( aPriority, accessoryServerPolicy, aType),
    iObjectConIx( NULL ),
    iProxy( NULL ),
    iProxyStarter( NULL ),
    iConnectionController( NULL )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccServer::CAccServer(0x%x, 0x%x)", aPriority, aType );

    COM_TRACE_( "[AccFW:AccServer] CAccServer::CAccServer" );
    }

// -----------------------------------------------------------------------------
// CAccServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccServer::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccServer::ConstructL()" );

    iObjectConIx = CObjectConIx::NewL();

    iServerModel          = CAccSrvServerModel::NewL();
    iConnectionController = CAccSrvConnectionController::NewL( iServerModel );
 	  //self destroy itself when idle state is once detect
	  CAccSrvIdleListener::NewL( iServerModel);

    CreateASYProxyL();

    // CASYProxyStarter exists because server wants to know
    // when the asys are ready to go
    iProxyStarter = CASYProxyStarter::NewL( iProxy, iConnectionController );

    iProxyStarter->LoadASYs();

    StartL( KAccServerName ); // Should be the last line

    COM_TRACE_( "[AccFW:AccServer] CAccServer::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccServer* CAccServer::NewLC()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccServer::NewLC()" );

    CAccServer* self = new ( ELeave ) CAccServer( EPriorityNormal,
                                                ESharableSessions );
    CleanupStack::PushL( self );
    self->ConstructL();

    COM_TRACE_1( "[AccFW:AccServer] CAccServer::NewLC - return 0x%x", &self );
    return self;
    }

// Destructor
CAccServer::~CAccServer()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccServer::~CAccServer()" );

    if( iObjectConIx )
        {
        delete iObjectConIx;
        }
    if( iConnectionController )
        {
        delete iConnectionController;
        }
    if( iServerModel )
        {
        delete iServerModel;
        }
    if( iProxy )
        {
        delete iProxy;
        }
    if( iProxyStarter )
        {
        delete iProxyStarter;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccServer::~CAccServer" );
    }

// -----------------------------------------------------------------------------
// CAccServer::CreateASYProxyL
// Creates instance of ASYProxy
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccServer::CreateASYProxyL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccServer::CreateASYProxyL()" );

    TUid AsyProxyUid3;
    AsyProxyUid3.iUid = KAsyProxyTInt32;

    // Dynamically load the DLL
    const TUidType AsyProxyUid( KNullUid, KNullUid, AsyProxyUid3 );

    // Use RLibrary object to interface to the DLL
    RLibrary library;
    TInt result = library.Load( KAsyProxyName, AsyProxyUid );

    COM_TRACE_1( "[AccFW:AccServer] CAccServer::CreateASYProxyL - result = 0x%x ", result );

#ifdef _DEBUG

    if ( result != KErrNone )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccServer::CreateASYProxyL - Panics now" );
        User::Panic( KASYProxyNotFound, KErrNotFound );
        }

#endif

    // Function at ordinal 1 creates new CAsyProxy
    TLibraryFunction entry = library.Lookup( 1 );

    // Call the function to create new CASYProxy
    iProxy = ( CASYProxyAPI* ) entry();

    COM_TRACE_1( "[AccFW:AccServer] CAccServer::CreateASYProxyL - iProxy = 0x%x", &iProxy );
    COM_TRACE_( "[AccFW:AccServer] CAccServer::CreateASYProxyL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccServer::NewContainerL
// Creates container for object containers
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CObjectCon* CAccServer::NewContainerL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccServer::NewContainerL()" );

    COM_TRACE_( "[AccFW:AccServer] CAccServer::NewContainerL - return" );
    return iObjectConIx->CreateL();
    }

// -----------------------------------------------------------------------------
// CAccServer::NewSessionL
// Creates main session for server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSession2* CAccServer::NewSessionL( const TVersion& /*aVersion*/,
                                    const RMessage2& aMessage) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccServer::NewSessionL()" );

    RThread clientThread;
    User::LeaveIfError( aMessage.Client( clientThread ) );
    CAccSrvMainSession *self = CAccSrvMainSession::NewL(
                                            clientThread,
                                            *const_cast<CAccServer*> ( this ),
                                            iProxy,
                                            iConnectionController,
                                            iServerModel );
    clientThread.Close();

    COM_TRACE_1( "[AccFW:AccServer] CAccServer::NewSessionL - return 0x%x", &self );
    return self;
    }

// -----------------------------------------------------------------------------
// CAccServer::RunError
// Error handling for AO's RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAccServer::RunError( TInt aError )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccServer::RunError(0x%x)", aError );

    if (aError == KErrBadDescriptor)
        {
        // A bad descriptor error implies a badly programmed client, so panic it;
        // otherwise report the error to the client
        PanicClient( Message(), EAccServerErrorBadDescriptor );
        }
    else
        {
        Message().Complete( aError );
        }

    // The leave will result in an early return from CServer::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart();

    COM_TRACE_1( "[AccFW:AccServer] CAccServer::RunError - return 0x%x", KErrNone );
    return KErrNone; // handled the error fully
    }

// -----------------------------------------------------------------------------
// CAccServer::PanicServer
// Raises panic for this server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccServer::PanicServer( TAccServerError aPanic )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccServer::PanicServer(0x%x)", aPanic );

    User::Panic( KAccServerName, aPanic );

    COM_TRACE_( "[AccFW:AccServer] CAccServer::PanicServer - return void" );
    }

// -----------------------------------------------------------------------------
// CAccServer::ThreadFunctionL
// Constructs the server
// Creates, initializes and starts the ActiveScheduler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccServer::ThreadFunctionL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccServer::ThreadFunctionL()" );

    // Construct active scheduler
    CActiveScheduler* activeScheduler = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( activeScheduler );

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );

    // Rename thread
    User::RenameThread( KAccServerName );
    // Construct our server
    CAccServer::NewLC(); // anonymous

    // Call Rendezvous to improve startup time
    RProcess::Rendezvous( KErrNone );

    // Start handling requests
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( 2, activeScheduler ); // anonymous CAccServer

    COM_TRACE_( "[AccFW:AccServer] CAccServer::ThreadFunctionL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccServer::ThreadFunction
// Server's thread entry function
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAccServer::ThreadFunction()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccServer::ThreadFunction()" );

    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !cleanupStack )
        {
        PanicServer( EAccServerErrorCreateTrapCleanup );
        }

    TRAPD( err, ThreadFunctionL() );
    if ( err != KErrNone )
        {
        PanicServer( EAccServerErrorCreateServer );
        }

    delete cleanupStack;
    cleanupStack = NULL;

    COM_TRACE_1( "[AccFW:AccServer] CAccServer::ThreadFunction - return 0x%x", KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CAccServer::RemoveContainer
// -----------------------------------------------------------------------------
//
void CAccServer::RemoveContainer( CObjectCon* aObjectCon )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccServer::RemoveContainer()" );

    iObjectConIx->Remove( aObjectCon );

    COM_TRACE_( "[AccFW:AccServer] CAccServer::RemoveContainer() - return" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// E32Main implements...
// Binary's main function
// Returns: TInt: Symbian error code
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
{
    API_TRACE_( "[AccFW:AccServer] E32Main()" );

    TInt err = 0;
    err = CAccServer::ThreadFunction();

    API_TRACE_1( "[AccFW:AccServer] E32Main - return 0x%x", err );
    return err;
}

//  End of File
