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
* Description:  Communication channel to Accessory server
*
*/


// INCLUDE FILES
#include "ASYProxyCommsService.h"
#include "ASYProxyCommsStack.h"
#include "ASYProxyAsyLoader.h"
#include "acc_debug.h"
#include <AsyServiceUids.h>
#include <AsyCommandHandlerBase.h>
#include <AsyMainServiceBase.h>
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

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CASYProxyCommsService::CASYProxyCommsService
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CASYProxyCommsService::CASYProxyCommsService( const TUid aHandlerUid )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommsService::CASYProxyCommsService(0x%x)", aHandlerUid.iUid );

    iHandlerUid = aHandlerUid;

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::CASYProxyCommsService" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyCommsService::ConstructL( const TUid aMainServiceUid )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommsService::ConstructL(0x%x)", aMainServiceUid.iUid );

    TInt connectErr( KErrGeneral );
    connectErr = iServer.Connect();

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommsService::ConstructL - connectErr == %d", connectErr );

    User::LeaveIfError( connectErr );

    iAsyComms.CreateSubSession( iServer, aMainServiceUid );

    iAsyProxyCommsStack = CASYProxyCommsStack::NewL();

    UpdateAsyCommsStackL();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyCommsService::ConstructL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::ConstructL()" );

    // Zero ( non existing ) handler does not need server connections
    // Zero handler needs CASYProxyCommsStack just to get simple Active Object
    //  because CActiveScheduler will be start later in any way

    iZeroObserver = CASYProxyCommandObserver::NewL( NULL );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYProxyCommsService* CASYProxyCommsService::NewL(
                       const TUid aHandlerUid,
                       const TUid aMainServiceUid )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommsService::NewL(aHandlerUid == 0x%x)", aHandlerUid.iUid );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommsService::NewL(aMainServiceUid == 0x%x)", aMainServiceUid.iUid );

    CASYProxyCommsService* self = new (ELeave) CASYProxyCommsService(
                                               aHandlerUid );

    CleanupStack::PushL( self );
    if ( aHandlerUid.iUid != ZEROHANDLERUID )
        {
        self->ConstructL( aMainServiceUid );
        }
    else
        {
        self->ConstructL();
        }

    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommsService::NewL - return 0x%x", self );

    return self;
    }

// Destructor
CASYProxyCommsService::~CASYProxyCommsService()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::~CASYProxyCommsService()" );

    if ( iAsyProxyCommsStack )
        {
        Destroy( KDeleteAllRecords );

        delete iAsyProxyCommsStack;
        iAsyProxyCommsStack = 0;
        }
    if ( iZeroObserver )
        {
        delete iZeroObserver;
        }
	
	iAsyComms.CloseSubSession();
	iServer.Disconnect();
	iServer.Close();
    
	COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::~CASYProxyCommsService" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsService::UpdateAsyCommsStackL
// Creates a new CASYProxyCommandObserver -object
//   and adds it to transaction stack
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommsService::UpdateAsyCommsStackL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::UpdateAsyCommsStackL()" );

    CASYProxyCommandObserver* cmdObserver = CASYProxyCommandObserver::NewL( this );

    CleanupStack::PushL( cmdObserver );
    iAsyProxyCommsStack->AddL( cmdObserver );
    CleanupStack::Pop( cmdObserver );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::UpdateAsyCommsStackL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsService::CreateAsyCommandHandlerL
// Creates a new CASYCommandHandlerBase object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommsService::CreateAsyCommandHandlerL(
                            CASYProxyCommandObserver* aASYProxyCommandObserver )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommsService::CreateAsyCommandHandlerL(aASYProxyCommandObserver == 0x%x)", aASYProxyCommandObserver );

    // Called from CASYProxyCommandObserver::RunL()
    iAsyCmdHndlr = CASYCommandHandlerBase::NewL(
                   iAsyMainServiceBase,
                   aASYProxyCommandObserver,
                   iHandlerUid );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::CreateAsyCommandHandlerL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsService::Destroy
// Calls iAsyProxyCommsStack's clean up functions
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommsService::Destroy( TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::Destroy()" );

    if ( aTrId != KDeleteAllRecords )
        {
        // Remove just resets the TRRecord.
        // Delete is made in Cleanup.
        iAsyProxyCommsStack->Remove( aTrId );
        }
    else
        {
        if ( iZeroObserver )
            {
            delete iZeroObserver;
            iZeroObserver = NULL;
            }

        iAsyProxyCommsStack->RemoveAll();
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::Destroy - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsService::GetServerSession
// Returns session for server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RAccessoryServer& CASYProxyCommsService::GetServerSession()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsService::GetServerSession()" );

    return iServer;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
