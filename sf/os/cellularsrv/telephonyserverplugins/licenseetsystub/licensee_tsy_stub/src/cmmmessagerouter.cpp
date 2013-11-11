// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//  INCLUDE FILES
#include "cmmmessagerouter.h"
#include "mmmmesshandlerbase.h"
#include "SatMessaging.h"       // SAT messaging inerface
#include <ctsy/rmmcustomapi.h>
#include <pcktcs.h>             // for packet data IPCs

// ======== MEMBER FUNCTIONS ========

CMmMessageRouter::CMmMessageRouter()
    : iMmMockMessHandler( NULL ),
    iMessageManager( NULL ),
    iSatService( NULL )    
    {
    }

void CMmMessageRouter::ConstructL()
    {
    InitL();
    InitModulesL(); 
    }

CMmMessageRouter* CMmMessageRouter::NewL()
    {
    CMmMessageRouter* self = new( ELeave ) CMmMessageRouter();
    CleanupStack::PushL( self );
    self->ConstructL();   
    CleanupStack::Pop();
    return self;
    }

CMmMessageRouter::~CMmMessageRouter()
    {
    delete iMmMockMessHandler;
    iMmMockMessHandler = NULL;
    iSatService = NULL;
     
    }

// ---------------------------------------------------------------------------
// CMmMessageRouter::InitL
// First initialization phase
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMessageRouter::InitL()
    {
    }

// ---------------------------------------------------------------------------
// CMmMessageRouter::InitModulesL
// Second initialization phase
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMessageRouter::InitModulesL()
    {
    // only one commmon message handler, minimum functionality   
    iMmMockMessHandler = CMmMockMessHandler::NewL( this );
    }

// ---------------------------------------------------------------------------
// CMmMessageRouter::RouteRequest
// Returns pointer to the message handler for this request,based on IPC number
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
MMmMessHandlerBase* CMmMessageRouter::RouteRequest(    
    const TInt /*aIpc*/ ) 
    {
    MMmMessHandlerBase* messHandler = NULL;
    messHandler = iMmMockMessHandler;
    
    return messHandler;
    }

// ---------------------------------------------------------------------------
// CMmMessageRouter::MessageManager
// Returns pointer to the MessageManager
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
MmMessageManagerCallback* CMmMessageRouter::MessageManager()
    {
    return iMessageManager;
    }

// ---------------------------------------------------------------------------
// CMmMessageRouter::GetMessageRouter
// Returns pointer to the message router object.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
MMessageRouter* CMmMessageRouter::GetMessageRouter( 
    MmMessageManagerCallback &aMessageManager )
    {    
    iMessageManager = &aMessageManager; 
    return this;
    }

// ---------------------------------------------------------------------------
// CMmMessageRouter::GetSatMessagingService
// Create and return pointer to the message router object.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTsySatMessagingBase* CMmMessageRouter::GetSatMessagingService()
    {    
    CTsySatMessaging* satService =  NULL; 
        
    // check has the SIM ATK TSY STUB already initialized
    if ( !iSatService )
        {
        // create instanse of the SIM ATK TSY STUB
        TRAPD( error, satService = CTsySatMessaging::NewL( this ) );
            
        if ( error != KErrNone )
            {
            iSatService = NULL;
            } 
        else
            {
            iSatService = static_cast<CTsySatMessagingBase*>( satService );
            }
        }
        
    // NOTE: client (ETEL) will handle the destruction of created object
    return iSatService; 
    }

// ---------------------------------------------------------------------------
// CMmMessageRouter::GetSatMessagingService
// Create and return pointer to the message router object.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTsySatMessagingBase* CMmMessageRouter::GetSatMessagingService(MCtsySatService & /*aSatMessagingService*/)
	{
		// No need to use MCtsySatService argument since the stub won't call it.
		return GetSatMessagingService();
	}
    
// ---------------------------------------------------------------------------
// CMmMessageRouter::Version
// Returns the version of the interface
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
MLtsyFactoryBase::TCtsyInterfaceVersion CMmMessageRouter::Version()
    {
    // currently support for version 1 only
    return MLtsyFactoryBase::EIfVersion1;
    }
    
// ---------------------------------------------------------------------------
// CMmMessageRouter::ExtFuncL
// Directs requests to messhandler
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMessageRouter::ExtFuncL( 
    TInt aIpc,
    const CMmDataPackage* aDataPackage )    
    {
    TInt ret( KErrNone );
    
    //check if the IPC is supported
    MMmMessHandlerBase* messageHandler = RouteRequest( aIpc );
   
    if ( NULL == messageHandler )
        {
        ret = KErrNotSupported;
        }
    else
        {
        // call to DOS layer
        ret = messageHandler->ExtFuncL( aIpc, aDataPackage ); 
        }
        
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmMessageRouter::Release
// Release the created LTSY objects
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMessageRouter::Release() 
    { 
    // call destructor
    delete this; 
    } 

// ======== EXPORTED FUNCTIONS ========

// ---------------------------------------------------------------------------
// MLtsyFactoryBase* LTsyFactoryL
// Libraty entry function
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C MLtsyFactoryBase* LTsyFactoryL()
    {
    // create new instance of the message router object and return it
    CMmMessageRouter* router = NULL;
    router = CMmMessageRouter::NewL();
    return router;
    }        
        
//  End of File 
