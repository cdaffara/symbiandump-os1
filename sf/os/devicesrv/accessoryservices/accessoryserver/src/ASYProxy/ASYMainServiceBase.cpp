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
* Description:  Pure virtual main service base class for ASY
*
*/


// INCLUDE FILES
#include "ASYProxyCommsService.h"
#include "ASYProxyMainService.h"
#include "acc_debug.h"
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
// CASYMainServiceBase::CASYMainServiceBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CASYMainServiceBase::CASYMainServiceBase()
    {
    API_TRACE_( "[AccFW:AsyProxy] CASYMainServiceBase::CASYMainServiceBase()" );

    iGlobalParams.iStartlFlag = EFalse;
    iGlobalParams.iSignalFlag = EFalse;

    API_TRACE_( "[AccFW:AsyProxy] CASYMainServiceBase::CASYMainServiceBase" );
    }

// -----------------------------------------------------------------------------
// CASYMainServiceBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYMainServiceBase::ConstructL( CASYProxyCommsService* aCommsService )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYMainServiceBase::ConstructL(0x%x)", aCommsService );

    iCommsService = aCommsService;

    COM_TRACE_( "[AccFW:AsyProxy] CASYMainServiceBase::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYMainServiceBase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYMainServiceBase* CASYMainServiceBase::NewL(
                     CASYProxyCommsService* aCommsService,
                     TUid aImplementationUidAsyService )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYMainServiceBase::NewL(0x%x)", aCommsService );

    TAny* tmpEcom = NULL;
    TInt err(0);
    COM_TRACE_1( "[AccFW:AsyProxy] CASYMainServiceBase::NewL - Before REComSession::CreateImplementationL - 0x%x", aImplementationUidAsyService.iUid );

    TRAP( err, tmpEcom = REComSession::CreateImplementationL(
                         aImplementationUidAsyService,
                         _FOFF( CASYMainServiceBase, iDtor_ID_Key ) ) );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYMainServiceBase::NewL - After REComSession::CreateImplementationL - err == %d", err );

    CASYMainServiceBase* theChosenOne = REINTERPRET_CAST(
                                        CASYMainServiceBase*,
                                        tmpEcom );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYMainServiceBase::::NewL - theChosenOne == 0x%x", theChosenOne );

    theChosenOne->ConstructL( aCommsService );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYMainServiceBase::NewL - return 0x%x", theChosenOne );

    return theChosenOne;
    }

// Destructor
EXPORT_C CASYMainServiceBase::~CASYMainServiceBase()
    {
    API_TRACE_( "[AccFW:AsyProxy] CASYMainServiceBase::~CASYMainServiceBase()" );

    if ( iCommsService )
        {
        delete iCommsService;
        iCommsService = NULL;
        }

    REComSession::DestroyedImplementation(iDtor_ID_Key);

    API_TRACE_( "[AccFW:AsyProxy] CASYMainServiceBase::~CASYMainServiceBase" );
    }


// -----------------------------------------------------------------------------
// CASYMainServiceBase::Session
// Returns session for server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessoryServer& CASYMainServiceBase::Session()
    {
    API_TRACE_( "[AccFW:AsyProxy] CASYMainServiceBase::Session()" );

    return iCommsService->GetServerSession();
    }

// -----------------------------------------------------------------------------
// CASYMainServiceBase::Signal
// Changes ASY's status flag state and informs Accessory Server thread
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CASYMainServiceBase::Signal()
    {
    API_TRACE_( "[AccFW:AsyProxy] CASYMainServiceBase::Signal()" );

    if ( iGlobalParams.iStartlFlag != EFalse ) // Asynchronous ASY
        {
        iGlobalParams.iThreadStatus = EASYThreadSyncLoadOkSignalOk;
        RThread::Rendezvous( iGlobalParams.iThreadStatus );
        }
    else // Synchronous ASY
        {
        iGlobalParams.iSignalFlag = ETrue;
        }

    API_TRACE_( "[AccFW:AsyProxy] CASYMainServiceBase::Signal - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
