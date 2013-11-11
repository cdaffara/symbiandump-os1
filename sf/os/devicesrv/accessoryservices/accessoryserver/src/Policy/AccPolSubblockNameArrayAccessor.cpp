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
* Description:  Private, Accessory Policy Subblock Name Array Accessory
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    <AccPolCommonNameValuePairs.h>
#include    "AccPolSubblockNameArrayAccessor.h"
#include    "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TAccPolSubblockNameArrayAccessor::TAccPolSubblockNameArrayAccessor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TAccPolSubblockNameArrayAccessor::TAccPolSubblockNameArrayAccessor()
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::TAccPolSubblockNameArrayAccessor()" );
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::TAccPolSubblockNameArrayAccessor() - return void");
    }

// -----------------------------------------------------------------------------
// TAccPolSubblockNameArrayAccessor::AppendNameL
// Append a name definition to the array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolSubblockNameArrayAccessor::AppendNameL(
    CAccPolSubblockNameArray& aArray,
    const TUint32& aName )
    {    
    COM_TRACE_1( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::AppendNameL(name=0x%x) ", aName );
    
    aArray.iNameArray.Append( aName );
        
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::AppendNameL() - return void" );
    }
    
// -----------------------------------------------------------------------------
// TAccPolSubblockNameArrayAccessor::ResetArray
// Reset (clear) the array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolSubblockNameArrayAccessor::ResetArray( 
    CAccPolSubblockNameArray& aArray )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::ResetArrayL()" );

#ifdef _DEBUG 
    TInt count( aArray.iNameArray.Count() );
    COM_TRACE_1( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::ResetArrayL - count=%d", count );
#endif // _DEBUG

    aArray.iNameArray.Reset();
    
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::ResetArrayL - return void" );   
    }

// -----------------------------------------------------------------------------
// TAccPolSubblockNameArrayAccessor::GetCount
// Get number of name definitions in the array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccPolSubblockNameArrayAccessor::GetCount( 
    const CAccPolSubblockNameArray& aArray )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::GetCount()" ); 
    
    COM_TRACE_1( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::GetCount() - return %d", 
                  aArray.iNameArray.Count() );
                  
    return aArray.iNameArray.Count();
    }



// -----------------------------------------------------------------------------
// TAccPolSubblockNameArrayAccessor::GetNameL
// Get a certain name from the array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolSubblockNameArrayAccessor::GetName( 
    const CAccPolSubblockNameArray& aArray, 
    TInt aIndex, 
    TUint32& aName )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::GetName()" );
    
    aName = aArray.iNameArray[aIndex];
    
    COM_TRACE_1( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::GetName - name=0x%x", aName );
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::GetName() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolSubblockNameArrayAccessor::ArrayL
// Get a pointer to the member array of a name array instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C RArray<TUint32>* TAccPolSubblockNameArrayAccessor::Array( 
    CAccPolSubblockNameArray* aArray )
    {
    COM_TRACE_1( "[AccFW: ACCPOLICY] TAccPolSubblockNameArrayAccessor::ArrayL() - return (0x%x)", &aArray );
    return &aArray->iNameArray;
    }


//  End of File  
