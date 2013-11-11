// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmmessagemanagerbaseTraces.h"
#endif

#include "cmmmessagemanagerbase.h"
#include "ctsydelegates.h"
#include "csatmessagerouterproxy.h"
#include <ctsy/pluginapi/mmessagerouter.h>

// ======== MEMBER FUNCTIONS ========

CMmMessageManagerBase* CMmMessageManagerBase::NewL(MMessageRouter& aMessageRouter)
    {
    CMmMessageManagerBase* messageManager = new ( ELeave ) CMmMessageManagerBase(aMessageRouter);
    CleanupStack::PushL( messageManager );
     messageManager->ConstructL();
    CleanupStack::Pop();
    return messageManager;
    }

CMmMessageManagerBase::~CMmMessageManagerBase()
    {
    // instance was created in this object, so we delete it here
    if ( iTsyDelgates )
        {
        delete iTsyDelgates;
        }
    iTsyDelgates = NULL;
    
    if ( iMessageRouterProxy )
        {
        delete iMessageRouterProxy;
        }
    iMessageRouterProxy = NULL;
    }

CMmMessageManagerBase::CMmMessageManagerBase(MMessageRouter& aMessageRouter)
 : iMessageRouter (aMessageRouter)
    {
    }

void CMmMessageManagerBase::ConstructL()
    {
    //Create instances
    iTsyDelgates = CTsyDelegates::NewL();
    iMessageRouterProxy = CSatMessageRouterProxy::NewL( *iTsyDelgates );
    }
    

// ---------------------------------------------------------------------------
// CMmMessageManagerBase::HandleRequestL
// Forwards a request to DOS layer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMessageManagerBase::HandleRequestL(    
    const TInt aIpc )        
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEMANAGERBASE_HANDLEREQUESTL_1, "TSY: CMmMessageManagerBase::HandleRequestL. IPC = %d", aIpc );
    return HandleRequestL( 
        aIpc, REINTERPRET_CAST( const CMmDataPackage*, NULL ) );
    }

// ---------------------------------------------------------------------------
// CMmMessageManagerBase::HandleRequestL
// Forwards a request to DOS layer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMessageManagerBase::HandleRequestL(    
    const TInt aIpc,      
    const CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEMANAGERBASE_HANDLEREQUESTL1_1, "TSY: CMmMessageManagerBase::HandleRequestL. IPC = %d", aIpc );
   
    return iMessageRouter.ExtFuncL( aIpc, aDataPackage );
    }

// ---------------------------------------------------------------------------
// CMmMessageManagerBase::HandleRequestL
// Forwards a request to DOS layer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMessageManagerBase::HandleRequestL(    
    const TInt aIpc,        
    const TDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEMANAGERBASE_HANDLEREQUESTL2_1, "TSY: CMmMessageManagerBase::HandleRequestL. IPC = %d", aIpc );

    // map the TDataPackage into a CMmDataPackage
    // Non-core Methods from the SOS layer using the DOS layer will perforn
    // always this mapping.
    // TSY core methods will call directly the overloaded HandleRequest method 
    // which uses a CMmDataPackage in the method's signature.
    iDataPackage.SetPacketData ( aDataPackage );

    //call the overloaded HandleRequest of CMmMessageManagerBase
    return HandleRequestL( aIpc, &iDataPackage );
    }

// ---------------------------------------------------------------------------
// CMmMessageManagerBase::RegisterTsyObject
// Registers a Tsy object for request and notification completions
// Stored in Tsy Delegates
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMessageManagerBase::RegisterTsyObject( 
    TTsyObjects aTsyObjectType,
    CBase* aTsyObject )
    {
    iTsyDelgates->RegisterTsyObject( aTsyObjectType, aTsyObject );
    }

// ---------------------------------------------------------------------------
// CMmMessageManagerBase::DeregisterTsyObject
// Remove registered Tsy object stored in Tsy Delegates
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMessageManagerBase::DeregisterTsyObject(
    CBase* aTsyObject )
    {
    iTsyDelgates->DeregisterTsyObject( aTsyObject );
    }
    
// ---------------------------------------------------------------------------
// CMmMessageManagerBase::GetMessageManagerCallback
// Return pointer to MessageManagerCallback object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
MmMessageManagerCallback* 
    CMmMessageManagerBase::GetMessageManagerCallback()    
    {
    return iMessageRouterProxy;
    }

    


//  End of File 

