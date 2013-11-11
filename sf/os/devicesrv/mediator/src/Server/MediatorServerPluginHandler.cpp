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
* Description:  Handles Mediator pluging resolving and loading to new thread.
*
*/


// INCLUDE FILES
#include    <e32base.h>

#include    "MediatorServerPluginHandler.h"
#include    "MediatorPluginBase.h"
#include    "MediatorUids.hrh"

#include    "MediatorDebug.h"
#include    "Debug.h"

// CONSTANTS
_LIT( KMediatorThreadName, "MediatorThread - " );

const TInt KMediatorHeapSize = 0x80000;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorServerPluginHandler::CMediatorServerPluginHandler
// -----------------------------------------------------------------------------
//
CMediatorServerPluginHandler::CMediatorServerPluginHandler()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorServerPluginHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorServerPluginHandler::ConstructL()
    {
    iIdle = CIdle::NewL( CActive::EPriorityIdle );
   	iIdle->Start( TCallBack( LoadPlugins, this ) ); 
    }

// -----------------------------------------------------------------------------
// CMediatorServerPluginHandler::NewL
// -----------------------------------------------------------------------------
//
CMediatorServerPluginHandler* CMediatorServerPluginHandler::NewL()
    {
    CMediatorServerPluginHandler* self = new( ELeave ) CMediatorServerPluginHandler;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMediatorServerPluginHandler::~CMediatorServerPluginHandler
// -----------------------------------------------------------------------------
//    
CMediatorServerPluginHandler::~CMediatorServerPluginHandler()
    {
    iPluginImplInfoArray.ResetAndDestroy();
    delete iIdle;
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumer::LoadPlugins
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorServerPluginHandler::LoadPlugins( TAny* aPtr )
    {
    CMediatorServerPluginHandler* me = (CMediatorServerPluginHandler*) aPtr;
    TRAPD( error, me->LoadPluginsL() );
    
    if ( error != KErrNone )
        {
        ERROR_TRACE(Print(_L("CMediatorServerPluginHandler::LoadPlugins: error=%d\n"), error ) );    
        }
    
    return error;
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumer::LoadPluginsL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerPluginHandler::LoadPluginsL()
    {
    FindImplementationsL();
    for ( TInt index = 0; index < iPluginImplInfoArray.Count(); index ++ )
        {
        RMediatorDebug::LogPlugin(iPluginImplInfoArray[index]);
        StartThreadL( iPluginImplInfoArray[index]->ImplementationUid() );
        } 
    }
              
// -----------------------------------------------------------------------------
// CMediatorServerPluginHandler::FindImplementationsL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerPluginHandler::FindImplementationsL()
    {
    LOG(_L("[Mediator Server]\t CMediatorServerPluginHandler::FindImplementationsL"));
    
     // Mediator interface uid as argument
    TUid pluginInterfaceUid;
    pluginInterfaceUid.iUid = KMediatorPluginInterfaceUid;
    
    TEComResolverParams emptyParams;
    // List the plugins to array. Note ROM-only plugins are accepted!
    REComSession::ListImplementationsL( pluginInterfaceUid,
                                        emptyParams,
                                        KRomOnlyResolverUid,
                                        iPluginImplInfoArray );
       
    }

// -----------------------------------------------------------------------------
// CMediatorServerPluginHandler::ThreadEntryPoint
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorServerPluginHandler::ThreadEntryPoint( TAny* aParameter )
    {
    TRACE(Print( _L("[Mediator Server]\t CMediatorServerPluginHandler::ThreadEntryPoint(0x%x)" ), &aParameter ) );

    TInt err = KErrNone;
    
    //create cleanupstack
    CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
    if ( !cleanup )
        {
        ERROR_LOG(_L("CMediatorServerPluginHandler::ThreadEntryPoint: KErrNoMemory\n"));
        err = KErrNoMemory;
        }

    if ( !err )
        {
        // Read the parameters from started
        TMediatorThreadParam* parameters = ( TMediatorThreadParam* ) aParameter;     
        TUid pluginUid = parameters->iMediatorPluginUid;
        
        // Just trap the leave in construction phase
        TRAP( err, DoStartThreadL( pluginUid ) );
       
        if ( err != KErrNone )
            {
            ERROR_TRACE(Print(_L("CMediatorServerPluginHandler::ThreadEntryPoint: pluginUid=0x%x, err=%d\n"), pluginUid.iUid, err ) );    
            }
        }
    
    delete cleanup;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorServerPluginHandler::StartThreadL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerPluginHandler::StartThreadL( TUid aMediatorPluginUid )
    {
    TRACE(Print( _L("[Mediator Server]\t CMediatorServerPluginHandler::StartThreadL(0x%x)" ), aMediatorPluginUid ) );
        
    // Then create start parameters
    TMediatorThreadParam param;
    param.iMediatorPluginUid = aMediatorPluginUid;
    
    // Generate unique thread name
    HBufC* bufThreadName = HBufC::NewLC( KMediatorThreadName().Length() + KMaxUidName );
    TPtr ptrThreadName = bufThreadName->Des();
    ptrThreadName = KMediatorThreadName;
    ptrThreadName.Append( aMediatorPluginUid.Name() );
    
    TRACE(Print( _L("[Mediator Server]\t CMediatorServerPluginHandler::StartThreadL(%S)" ), &ptrThreadName ) );
    RThread newThread;
    
    User::LeaveIfError( newThread.Create(
                        ptrThreadName,
                        CMediatorServerPluginHandler::ThreadEntryPoint,
                        KDefaultStackSize,
                        KMinHeapSize,
                        KMediatorHeapSize,
                        &param ) );
                        
    CleanupStack::PopAndDestroy( bufThreadName );
    
    // Rendezvous
    TRequestStatus status = KRequestPending;
    newThread.Rendezvous( status );
    newThread.Resume();
    User::WaitForRequest( status );
    TRACE(Print( _L("[Mediator Server]\t Thread resumed with status %d" ), status.Int() ) );
    
    newThread.Close();
    }

// -----------------------------------------------------------------------------
// CMediatorServerPluginHandler::DoStartThreadL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerPluginHandler::DoStartThreadL( TUid aMediatorPluginUid )
    {
    TRACE(Print( _L("[Mediator Server]\t CMediatorServerPluginHandler::DoStartThreadL(0x%x)" ), aMediatorPluginUid ) );
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    
    CMediatorPluginBase* newPlugin = CMediatorPluginBase::NewL( aMediatorPluginUid );
    CleanupStack::PushL( newPlugin );
    RThread::Rendezvous( KErrNone );
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( newPlugin );
    CleanupStack::PopAndDestroy( scheduler );

    }

//  End of File  
