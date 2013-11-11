/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:
 * CPDAsyCmdHandler class implementation.
 *
 */

// SYSTEM INCLUDES

// USER INCLUDES
#include "pdasycmdhandler.h"
#include "pdengine.h"
#include "pdasymainservice.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CPDAsyCmdHandler::CPDAsyCmdHandler()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CPDAsyCmdHandler::ConstructL( MPDAsyCmdHandler* aCmdHandler,
    CASYMainServiceBase* aMainServiceBase )
    {
    FUNC_LOG;
    
    // Resolve command handler pointer
    iCmdHandler = aCmdHandler;
    if( !aCmdHandler )
        {
        iCmdHandler = this;
        }
    
    // Resolve main service pointer
    iMainService = aMainServiceBase;
    if( !aMainServiceBase )
        {
        iMainService = ASYMainServiceBase();
        }
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor.
// ---------------------------------------------------------------------------
//
CPDAsyCmdHandler* CPDAsyCmdHandler::NewL()
    {
    FUNC_LOG;

    CPDAsyCmdHandler* self = CPDAsyCmdHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor.
// Leaves pointer in the cleanup stack.
// ---------------------------------------------------------------------------
//
CPDAsyCmdHandler* CPDAsyCmdHandler::NewLC()
    {
    FUNC_LOG;

    CPDAsyCmdHandler* self = new ( ELeave ) CPDAsyCmdHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor for testing purposes.
// Leaves pointer in the cleanup stack.
// ---------------------------------------------------------------------------
//
CPDAsyCmdHandler* CPDAsyCmdHandler::CreateTestInstanceL(
    MPDAsyCmdHandler* aCmdHandler,
    CASYMainServiceBase* aMainServiceBase )
    {
    FUNC_LOG;

    CPDAsyCmdHandler* self = new ( ELeave ) CPDAsyCmdHandler;
    CleanupStack::PushL( self );
    self->ConstructL( aCmdHandler, aMainServiceBase );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CPDAsyCmdHandler::~CPDAsyCmdHandler()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPDAsyCmdHandler::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CPDAsyCmdHandler::ProcessCommandL( const TProcessCmdId aCommand,
    const TASYCmdParams& aCmdParams )
    {
    FUNC_LOG;

    INFO_1( "Received command: Command ID = [%d]", aCommand );
    
    // Get main service
    CPDAsyMainService* mainService = MainService();
    if( mainService )
        {
        // Get the plugged display engine
        CPDEngine& engine = mainService->Engine();
        engine.ProcessCommandL( aCommand, aCmdParams, *iCmdHandler );
        INFO_1( "Processed command: Command ID = [%d]", aCommand );
        }
    else
        {
        // Main service not ready
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// CPDAsyCmdHandler::ProcessResponse
// ---------------------------------------------------------------------------
//
void CPDAsyCmdHandler::ProcessResponse(
    const TAccValueTypeTBool aCmdResponse,
    TInt aErrCode )
    {
    FUNC_LOG;
    if ( iCmdHandler == this )
        {
        TRAP_IGNORE( ProcessResponseL( aCmdResponse, aErrCode ) );
        }
    else
        {
        iCmdHandler->ProcessResponse( aCmdResponse, aErrCode );
        }
    }

// ---------------------------------------------------------------------------
// CPDAsyCmdHandler::ProcessResponse
// ---------------------------------------------------------------------------
//
void CPDAsyCmdHandler::ProcessResponse( const TAccValueTypeTInt aCmdResponse,
    TInt aErrCode )
    {
    FUNC_LOG;
   
    if ( iCmdHandler == this )
        {
        TRAP_IGNORE( ProcessResponseL( aCmdResponse, aErrCode ) );
        }
    else
        {
        iCmdHandler->ProcessResponse( aCmdResponse, aErrCode );
        }
    }

// ---------------------------------------------------------------------------
// CPDAsyCmdHandler::ProcessResponse
// ---------------------------------------------------------------------------
//
void CPDAsyCmdHandler::ProcessResponse( const TDesC8& aCmdResponse,
    TInt aErrCode )
    {
    FUNC_LOG;
    
    
    if ( iCmdHandler == this )
        {
        TRAP_IGNORE( ProcessResponseL( aCmdResponse, aErrCode ) );
        }
    else
        {
        iCmdHandler->ProcessResponse( aCmdResponse, aErrCode );
        }
    }

// ---------------------------------------------------------------------------
// CPDAsyCmdHandler::ProcessResponse
// ---------------------------------------------------------------------------
//
void CPDAsyCmdHandler::ProcessResponse( const CAccPolObjectCon& aObjectCon,
    TInt aErrCode )
    {
    FUNC_LOG;
    
    if ( iCmdHandler == this )
        {
        TRAP_IGNORE( ProcessResponseL( aObjectCon, aErrCode ) );
        }
    else
        {
        iCmdHandler->ProcessResponse( aObjectCon, aErrCode );
        }
    }

// ---------------------------------------------------------------------------
// CPDAsyCmdHandler::ObjectConDataFromProxy
// ---------------------------------------------------------------------------
//
void CPDAsyCmdHandler::ObjectConDataFromProxy(
    CAccPolObjectCon& aObjectCon ) const
    {
    FUNC_LOG;
    
    if ( iCmdHandler == this )
        {
        TRAP_IGNORE( ObjectConDataFromProxyL( aObjectCon ) );
        }
    else
        {
        iCmdHandler->ObjectConDataFromProxy( aObjectCon );
        }
    }

//------------------------------------------------------------------------------
// CPDAsyCmdHandler::MainService
//------------------------------------------------------------------------------
//
CPDAsyMainService* CPDAsyCmdHandler::MainService()
    {
    FUNC_LOG;

    // Double check that the pointer exists
    if( !iMainService )
        {
        iMainService = ASYMainServiceBase();
        }
    
    return static_cast<CPDAsyMainService*>( iMainService );
    }

// End of file
