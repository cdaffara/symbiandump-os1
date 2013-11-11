/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Creator and user of ASY threads
*
*/


// INCLUDE FILES
#include "ASYProxyMainService.h"
#include "ASYProxyCommsService.h"
#include "ASYProxyAsyHandler.h"
#include "acc_debug.h"
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
// CASYProxyMainService::CASYProxyMainService
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CASYProxyMainService::CASYProxyMainService( CASYProxyASYHandler* aAsyHandler ) :
                      CActive( CActive::EPriorityStandard ),
                      iAsyHandler( aAsyHandler )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::CASYProxyMainService(aAsyHandler == 0x%x)", aAsyHandler );

    CActiveScheduler::Add(this);

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::CASYProxyMainService" );
    }

// -----------------------------------------------------------------------------
// CASYProxyMainService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyMainService::ConstructL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::ConstructL()" );

    iAsyHandler->UpdateASYStateL( EASYProxyASYInitial );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyMainService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYProxyMainService* CASYProxyMainService::NewL( CASYProxyASYHandler* aAsyHandler )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::NewL(0x%x)", aAsyHandler );

    CASYProxyMainService* self = new ( ELeave ) CASYProxyMainService( aAsyHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::NewL - return 0x%x", self );

    return self;
    }

// Destructor
CASYProxyMainService::~CASYProxyMainService()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::~CASYProxyMainService()" );
    Cancel();
    iThread.Close ();
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::~CASYProxyMainService" );
    }

// -----------------------------------------------------------------------------
// CASYProxyMainService::RunL
// Has three different cases:
// - case for TASYThreadSyncLoadOkSignalNOk
// - case for TASYThreadSyncLoadOkSignalOk
// - case for TASYProxyASYStateThreadKills
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyMainService::RunL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::RunL()" );

#ifdef _DEBUG
/* coverity[var_compare_op] */ 
    if ( iAsyHandler )
        {
        COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::RunL - iAsyIndex == %d", iAsyHandler->Index() );
        }

#endif // _DEBUG

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::RunL - iStatus == %d", iStatus.Int() );

    if ( iStatus.Int() == EASYThreadSyncLoadOkSignalNOk )
        {
        // Asynchronous ASY :
        // Synchronous load part is complete
        iAsyHandler->UpdateASYStateL( EASYProxyASYSynchronousLoadComplete );

        // ---------------------------------------------------------------------
        // Rendez vous for waiting...
        //
        // Case of Asynchronous ASY :
        // - Make this AO active waiting for Async complete after Sync complete
        // ---------------------------------------------------------------------
        //
        // ---------------------------------------------------------------------
        // Rendez vous
        // ---------------------------------------------------------------------

        iStatus = KRequestPending;
        SetActive();
        iThread.Rendezvous( iStatus );

        // ---------------------------------------------------------------------
        // End of Rendez vous for...
        // ---------------------------------------------------------------------
        }

    else if ( iStatus.Int() == EASYThreadSyncLoadOkSignalOk )
        {
        // Synchronous ASY :
        // Synchronous load part is now complete
        // Also ASY loading is totally complete

        // Asynchronous ASY :
        // Synchronous load part was earlier complete
        // Asynchronous load part is now complete
        // Also ASY loading is totally complete
        
        iAsyHandler->UpdateASYStateL( EASYProxyASYConnectionReady );

        // ---------------------------------------------------------------------
        // Rendez vous for waiting...
        //
        // Case of Synchronous ASY :
        // - Make this AO active waiting for ASY Panic
        //
        // Case of Asynchronous ASY :
        // - Make this AO active waiting for ASY Panic
        // ---------------------------------------------------------------------
        //

        // ---------------------------------------------------------------------
        // Logon
        // ---------------------------------------------------------------------

        iStatus = KRequestPending;
        SetActive();
        iThread.Logon( iStatus );
        
        // ---------------------------------------------------------------------
        // End of Logon...
        // ---------------------------------------------------------------------
        }

    else
        {
        iAsyHandler->UpdateASYStateL( EASYProxyASYThreadKills );
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::RunL - return void" );
    }
    
// -----------------------------------------------------------------------------
// CASYProxyMainService::RunError
// Returns KErrNone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyMainService::RunError( TInt aError )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::RunError(%d)", aError );

     // Avoid Panic in CActiveScheduler
    aError = KErrNone;

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::RunError - return KErrNone" );

    return aError;
    }

// -----------------------------------------------------------------------------
// CASYProxyMainService::DoCancel
// Kills the ASY thread
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyMainService::DoCancel()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::DoCancel()" );

    iThread.RendezvousCancel( iStatus );
    TExitType threadExitType = iThread.ExitType();
    if ( EExitPending == threadExitType)
        {
        iThread.Kill( KErrCancel );
        iThread.Close();
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::DoCancel - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyMainService::StartThreadL
// Creates a new thread
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyMainService::StartThreadL(
                           const TUid aMainServiceUid,
                           const TUid aHandlerUid,
                           const TInt aHeapSize,
                           const TInt aStackSize )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::StartThreadL(aMainServiceUid == 0x%x)", aMainServiceUid.iUid );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::StartThreadL(aHandlerUid == 0x%x)", aHandlerUid.iUid );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::StartThreadL(aHeapSize == 0x%x)", aHeapSize );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::StartThreadL(aStackSize == 0x%x)", aStackSize );

    iAsyHandler->UpdateASYStateL( EASYProxyASYThreadCreating );

    TASYProxyThreadParam param;
    RSemaphore semaphore;
    User::LeaveIfError( semaphore.CreateLocal( 0 ) );
    CleanupClosePushL( semaphore );
    param.iSemaphore = &semaphore;
    param.iThreadMainServiceUid = aMainServiceUid;
    param.iThreadHandlerUid     = aHandlerUid;

    // Generate unique thread name
    HBufC* bufThreadName = HBufC::NewL( KASYThreadName().Length() + KMaxUidName );

    CleanupStack::PushL( bufThreadName );
    TPtr ptrThreadName = bufThreadName->Des();
    ptrThreadName = KASYThreadName;
    ptrThreadName.Append( aMainServiceUid.Name() );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::StartThreadL(%S)", &ptrThreadName );
 
    User::LeaveIfError( iThread.Create(
                        ptrThreadName,
                        ThreadEntryPoint,
                        aStackSize,
                        KMinHeapSize,
                        aHeapSize,
                        &param ) );

    CleanupStack::Pop ( bufThreadName );

    delete bufThreadName;

    if ( aHandlerUid.iUid != ZEROHANDLERUID )
        {
        iThread.SetPriority( EPriorityMore );
        }

    // ---------------------------------------------------------------------
    // Rendez vous
    // ---------------------------------------------------------------------

    iStatus = KRequestPending;
    iThread.Rendezvous( iStatus );

    // ---------------------------------------------------------------------
    // End of Rendez vous
    // ---------------------------------------------------------------------

    SetActive();
    iThread.Resume();
    semaphore.Wait();
    CleanupStack::PopAndDestroy( &semaphore );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::StartThreadL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyMainService::ThreadEntryPoint
// Entry point of new thread
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyMainService::ThreadEntryPoint( TAny* aParameter )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::ThreadEntryPoint(0x%x)", &aParameter );

    TInt err = KErrNone;

    //create cleanupstack
    CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
    if ( !cleanup )
        err = KErrNoMemory;

    if ( !err )
        {
        TASYProxyThreadParam* Param = ( TASYProxyThreadParam* ) aParameter;

        TUid MainServiceUid = Param->iThreadMainServiceUid;
        TUid HandlerUid     = Param->iThreadHandlerUid;

        Param->iSemaphore->Signal();

        TRAP( err, DoStartThreadL( MainServiceUid, HandlerUid ) );
        }
    delete cleanup;

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::ThreadEntryPoint - return %d", err );

    return err;
    }

// -----------------------------------------------------------------------------
// CASYProxyMainService::DoStartThreadL
// Starts new thread by creating and starting active scheduler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyMainService::DoStartThreadL(
                           const TUid aMainServiceUid,
                           const TUid aHandlerUid )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::DoStartThreadL(aMainServiceUid == 0x%x)", aMainServiceUid.iUid );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::DoStartThreadL(aHandlerUid == 0x%x)", aHandlerUid.iUid );

    CActiveScheduler* actSched = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( actSched );
    CActiveScheduler::Install( actSched );

    TInt res = CreateAsyServicesLC( aMainServiceUid, aHandlerUid );

    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( 2, actSched ); // asyMainService, actSched

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::DoStartThreadL - return %d", res );

    return ( res );
    }

// -----------------------------------------------------------------------------
// CASYProxyMainService::CreateAsyServicesLC
// Creates a new ASYProxyCommsService and CASYMainServiceBase -instances
// Starts the instance of CASYMainServiceBase
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyMainService::CreateAsyServicesLC(
                           const TUid aMainServiceUid,
                           const TUid aHandlerUid )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::CreateAsyServicesLC(aMainServiceUid == 0x%x)", aMainServiceUid.iUid );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::CreateAsyServicesLC(aHandlerUid == 0x%x)", aHandlerUid.iUid );
    
    CASYProxyCommsService* asyProxyCommsService = NULL;

    asyProxyCommsService = CASYProxyCommsService::NewL( aHandlerUid, aMainServiceUid );
    CleanupStack::PushL( asyProxyCommsService );

    // asyProxyCommsService will be deleted in CASYMainServiceBase's destructor
    CASYMainServiceBase* asyMainService = CASYMainServiceBase::NewL(
                                               asyProxyCommsService,
                                               aMainServiceUid );

    asyProxyCommsService->iAsyMainServiceBase = asyMainService; 
    CleanupStack::Pop( asyProxyCommsService );

    CleanupStack::PushL( asyMainService );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyMainService::CreateAsyServicesLC - next call StartL" );
    TInt res = asyMainService->StartL();

    asyMainService->iGlobalParams.iStartlFlag = ETrue;

    if ( asyMainService->iGlobalParams.iSignalFlag != EFalse ) // Synchronous ASY
    // --------------------------------------------------------------------------
        {
        asyMainService->iGlobalParams.iThreadStatus = EASYThreadSyncLoadOkSignalOk;
        RThread::Rendezvous( asyMainService->iGlobalParams.iThreadStatus );
        }
    else // Asynchronous ASY
    // --------------------------------------------------------------------------
        {
        asyMainService->iGlobalParams.iThreadStatus = EASYThreadSyncLoadOkSignalNOk;
        RThread::Rendezvous( asyMainService->iGlobalParams.iThreadStatus );
        }

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyMainService::CreateAsyServicesLC - return %d", res );

    return res;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
