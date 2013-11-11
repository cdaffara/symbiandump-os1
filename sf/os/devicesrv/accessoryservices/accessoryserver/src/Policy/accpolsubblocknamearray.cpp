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
* Description:  Public, Accessory Policy Subblock Name Array
*
*/



// INCLUDE FILES
#include    <AccPolSubblockNameArray.h>
#include    "acc_debug.h"

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
// CAccPolSubblockNameArray::CAccPolSubblockNameArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolSubblockNameArray::CAccPolSubblockNameArray()
    {
    }

// -----------------------------------------------------------------------------
// CAccPolSubblockNameArray::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccPolSubblockNameArray::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CAccPolSubblockNameArray::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolSubblockNameArray* CAccPolSubblockNameArray::NewL()
    {
    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolSubblockNameArray::NewL()" );
    
    CAccPolSubblockNameArray* self = new( ELeave ) CAccPolSubblockNameArray;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolSubblockNameArray::NewL() - return" );
    return self;
    }

// Destructor
EXPORT_C CAccPolSubblockNameArray::~CAccPolSubblockNameArray()
    {
    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolSubblockNameArray::~CAccPolSubblockNameArray()" );

    // Free resources
    iNameArray.Close();
    
    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolSubblockNameArray::~CAccPolSubblockNameArray() - return" );
    }

// -----------------------------------------------------------------------------
// CAccPolSubblockNameArray::HasNameL
// Check if the array containes the defined name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolSubblockNameArray::HasName(
    const TUint32& aName ) const
    {
    API_TRACE_1( "[AccFW: ACCPOLICY] CAccPolSubblockNameArray::HasNameL(name=0x%x)", aName );   
    TBool ret( EFalse );
    
    TInt count( iNameArray.Count() );
    for( TInt i( 0 ); i < count; ++i )
        {
        ret = iNameArray[ i ] == aName;
        if ( ret )
            {
            break;
            }
        }

    API_TRACE_1( "[AccFW: ACCPOLICY] CAccPolSubblockNameArray::HasNameL() - return %d", ret );
    return ret;
    }

//  End of File
