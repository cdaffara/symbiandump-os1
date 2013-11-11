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
* Description:  Startup for and handling statuses of ASYs
*
*/


// INCLUDE FILES
#include "AsyProxyStarter.h"
#include "AccSrvConnectionController.h"
#include "AccSrvServerModel.h"
#include "AsyProxyAPI.h"
#include "acc_debug.h"
#include <e32svr.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// Dummy ASY index for calling LoadCompleteL in case of TimeOut
const TInt KTimeOutIndex = -1;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CASYProxyStarter::CASYProxyStarter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CASYProxyStarter::CASYProxyStarter(
                  CASYProxyAPI* aAsyProxyAPI,
                  CAccSrvConnectionController* aConnectionController )
    : iConnectionController( aConnectionController ),
      iInitCalled( EFalse )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyStarter::CASYProxyStarter(0x%x)", &aAsyProxyAPI );

    iAsyProxyAPI = aAsyProxyAPI;

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::CASYProxyStarter" );
    }

// -----------------------------------------------------------------------------
// CASYProxyStarter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyStarter::ConstructL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::ConstructL()" );

    iAsyProxyAPI->InitializeL( this );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyStarter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYProxyStarter* CASYProxyStarter::NewL(
                  CASYProxyAPI* aAsyProxyAPI,
                  CAccSrvConnectionController* aConnectionController )
    {
    COM_TRACE_2( "[AccFW:AsyProxy] CASYProxyStarter::NewL(0x%x, 0x%x)", &aAsyProxyAPI, &aConnectionController );

    CASYProxyStarter* self = new (ELeave) CASYProxyStarter(
                                            aAsyProxyAPI,
                                            aConnectionController );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyStarter::NewL - return 0x%x", &self );
    return self;
    }

// Destructor
CASYProxyStarter::~CASYProxyStarter()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::~CASYProxyStarter()" );
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::~CASYProxyStarter" );
    }

// -----------------------------------------------------------------------------
// CASYProxyStarter::LoadASYs
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyStarter::LoadASYs()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::LoadASYs()" );

    iAsyProxyAPI->LoadASYs();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::LoadASYs - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyStarter::LoadCompleteL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyStarter::LoadCompleteL( const TInt aASYIndex,
                                      const TBool aAllASYsComplete )
    {
    (void) aASYIndex;
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyStarter::LoadCompleteL(0x%x)", aASYIndex );

    if ( aAllASYsComplete )
        {
        TASYCommandParamRecord emptyParams;

        if ( !iInitCalled )
            {
            iInitCalled = ETrue;

            iConnectionController->ASYProxyHandler().HandleASYCommsL(
                                                       ECmdProcessCommandInit,
                                                       emptyParams );
            }
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::LoadCompleteL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyStarter::ThreadKills
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyStarter::ThreadKills( const TInt aASYIndex )
    {
    (void) aASYIndex;
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyStarter::ThreadKills(%d)", aASYIndex );
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::ThreadKills - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyStarter::LoadingTimedOutL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyStarter::LoadingTimedOutL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::LoadingTimedOutL()" );

    iConnectionController->ServerModel().SetASYsLoadCompleted();
    LoadCompleteL( KTimeOutIndex, ETrue );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyStarter::LoadingTimedOutL - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
