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
* Description:  Data container for transaction data
*
*/


// INCLUDE FILES
#include "ASYProxyCommandObserver.h"
#include "ASYProxyTRRecord.h"
#include "acc_debug.h"

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
// CASYProxyTRRecord::CASYProxyTRRecord
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CASYProxyTRRecord::CASYProxyTRRecord()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::CASYProxyTRRecord()" );    
    iParams().iTrId = 0;
    iParams().iCmdId = ( TProcessCmdId ) 0;    
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::CASYProxyTRRecord" );
    }

// -----------------------------------------------------------------------------
// CASYProxyTRRecord::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyTRRecord::ConstructL(
                        CASYProxyCommandObserver* aASYProxyCommandObserver )
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::ConstructL()" );

    iASYProxyCommandObserver = aASYProxyCommandObserver;

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyTRRecord::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYProxyTRRecord* CASYProxyTRRecord::NewL(
                   CASYProxyCommandObserver* aASYProxyCommandObserver )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyTRRecord::NewL(0x%x)", aASYProxyCommandObserver );

    CASYProxyTRRecord* self = new ( ELeave ) CASYProxyTRRecord();
    CleanupStack::PushL( self );
    self->ConstructL( aASYProxyCommandObserver );
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyTRRecord::NewL - return 0x%x", self );

    return self;
    }

// Destructor
CASYProxyTRRecord::~CASYProxyTRRecord()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::~CASYProxyTRRecord()" );

    DeleteCmdObserver();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::~CASYProxyTRRecord" );
    }

// -----------------------------------------------------------------------------
// CASYProxyTRRecord::ResetTrId
// Sets transaction id value to KResetRecord
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyTRRecord::ResetTrId()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::ResetTrId()" );
    iParams().iTrId = KResetRecord;
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::ResetTrId - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyTRRecord::TrId
// Returns transaction id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyTRRecord::TrId()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::TrId()" );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyTRRecord::TrId - return %d", iParams().iTrId );
	return iParams().iTrId;
    }

// -----------------------------------------------------------------------------
// CASYProxyTRRecord::iTimeOutTrId
// Returns transaction id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyTRRecord::TimeOutTrId()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::TimeOutTrId()" );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyTRRecord::TimeOutTrId - return %d", iParams().iTimeOutTrId );
	  return iParams().iTimeOutTrId;
    }
    
// -----------------------------------------------------------------------------
// CASYProxyTRRecord::CmdId
// Returns command id of transaction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TProcessCmdId CASYProxyTRRecord::CmdId()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::CmdId()" );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyTRRecord::CmdId - return %d", iParams().iCmdId );
    return iParams().iCmdId;
    }

// -----------------------------------------------------------------------------
// CASYProxyTRRecord::AsyCmdParams
// Returns command parameters of transaction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TASYCmdParams& CASYProxyTRRecord::AsyCmdParams()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::AsyCmdParams()" );
    return iParams().iAsyCmdParams;
    }

// -----------------------------------------------------------------------------
// CASYProxyTRRecord::DeleteCmdObserver
// Deletes iASYProxyCommandObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyTRRecord::DeleteCmdObserver()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::DeleteCmdObserver()" );

    if ( iASYProxyCommandObserver )
        {
        delete iASYProxyCommandObserver;
        iASYProxyCommandObserver = 0;
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyTRRecord::DeleteCmdObserver - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyTRRecord::MarkForFinalDestruction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyTRRecord::MarkForFinalDestruction ()
    {
    iASYProxyCommandObserver->MarkForFinalDestruction ();
    }
	
// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
