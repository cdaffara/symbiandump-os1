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
* Description:  Stack for handling transactions
*
*/


// INCLUDE FILES
#include "ASYProxyCommsStack.h"
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
// CASYProxyCommsStack::CASYProxyCommsStack
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CASYProxyCommsStack::CASYProxyCommsStack()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::CASYProxyCommsStack()" );
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::CASYProxyCommsStack - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsStack::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyCommsStack::ConstructL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::ConstructL()" );
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsStack::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYProxyCommsStack* CASYProxyCommsStack::NewL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::NewL()" );

    CASYProxyCommsStack* self = new ( ELeave ) CASYProxyCommsStack();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommsStack::NewL - return 0x%x", self );

    return self;
    }

// Destructor
CASYProxyCommsStack::~CASYProxyCommsStack()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::~CASYProxyCommsStack()" );

    TInt lastIndex( iTRRecordArray.Count() - 1 );
    for ( TInt iCounter = lastIndex; iCounter >= 0; iCounter-- )
        {
        iTRRecordArray[iCounter]->MarkForFinalDestruction ();
        }
    iTRRecordArray.ResetAndDestroy();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::~CASYProxyCommsStack" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsStack::AddL
// Handles array of CASYProxyTRRecord -instances
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommsStack::AddL( CASYProxyCommandObserver* aASYProxyCommandObserver )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommsStack::AddL(0x%x)", aASYProxyCommandObserver );

    TInt lastIndex( iTRRecordArray.Count() - 1 );
    for ( TInt iCounter = lastIndex; iCounter >= 0; iCounter-- )
        {
        if ( iTRRecordArray.operator[]( iCounter )->TrId() == KResetRecord )
            {
            iTRRecordArray.operator[]( iCounter )->DeleteCmdObserver();
            delete iTRRecordArray.operator[]( iCounter );
            iTRRecordArray.Remove( iCounter );
            }
        }

    CASYProxyTRRecord* asyProxyTRRecord = CASYProxyTRRecord::NewL( aASYProxyCommandObserver );
    CleanupStack::PushL ( asyProxyTRRecord );
    iTRRecordArray.AppendL( asyProxyTRRecord );
	CleanupStack::Pop(1);
			
    aASYProxyCommandObserver->InitializeRef( asyProxyTRRecord );
    aASYProxyCommandObserver->IssueRequestL();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::AddL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsStack::Remove
// Marks current transaction to be removed from array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommsStack::Remove( TInt aTrId )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommsStack::Remove(%d)", aTrId );

    TInt lastIndex( iTRRecordArray.Count() - 1 );
    for ( TInt iCounter = 0; iCounter <= lastIndex; iCounter++ )
        {
        if ( iTRRecordArray.operator[]( iCounter )->TrId() == aTrId )
            {
            iTRRecordArray.operator[]( iCounter )->ResetTrId();
            break;
            }
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::Remove - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommsStack::RemoveAll
// Deletes all transactions regardless of their statuses
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommsStack::RemoveAll()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::RemoveAll()" );

    TInt lastIndex( iTRRecordArray.Count() - 1 );
    for ( TInt iCounter = lastIndex; iCounter >= 0; iCounter-- )
        {
        iTRRecordArray.operator[]( iCounter )->DeleteCmdObserver();
        delete iTRRecordArray.operator[]( iCounter );
        iTRRecordArray.Remove( iCounter );
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommsStack::RemoveAll - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
