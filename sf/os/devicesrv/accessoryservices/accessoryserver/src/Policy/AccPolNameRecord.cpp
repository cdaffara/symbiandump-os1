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
* Description:  Public, Accessory Policy Name Record
*
*/



// INCLUDE FILES
#include <AccPolNameRecord.h>
#include "acc_debug.h"

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
// TAccPolNameRecord::TAccPolNameRecord
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolNameRecord::TAccPolNameRecord()
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolNameRecord::TAccPolNameRecord()" );
    iName = 0;
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolNameRecord::TAccPolNameRecord() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolNameRecord::GetName
// Copy the member name to the reference.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolNameRecord::GetName( TUint32& aName ) const
    {
    API_TRACE_RAW_1( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::GetName() ", aName );
    
    aName = iName;
    
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::GetName - name=0x%x", aName );
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::GetName() return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolNameRecord::SetNameL
// Set the member name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolNameRecord::SetNameL( const TUint32& aName )
    {
    API_TRACE_1( "[ACCPOLICY] TAccPolGenericIDArray::SetNameL(name=0x%x) ", aName );
    
    SetName( aName);
        
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::SetNameL() return void" );
    }


// -----------------------------------------------------------------------------
// TAccPolNameRecord::SetName
// Set the member name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolNameRecord::SetName( const TUint32& aName )
    {
    API_TRACE_1( "[ACCPOLICY] TAccPolGenericIDArray::SetName(name=0x%x) ", aName );
    
    iName = aName;
        
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::SetName() return void" );
    }

//  End of File
