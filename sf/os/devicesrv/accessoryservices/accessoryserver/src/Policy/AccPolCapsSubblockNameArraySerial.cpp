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
* Description:  Private, Accessory Policy Capability Subblock Name Array Serialization
*
*/


// INCLUDE FILES
#include    <s32mem.h>
#include    <AccPolCommonNameValuePairs.h>
#include    "AccPolCapsSubblockNameArraySerial.h"
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
// TAccPolCapsSubblockNameArraySerial::InternalizeL
// Internalize name array data from the read stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolCapsSubblockNameArraySerial::InternalizeL( 
    RReadStream& aStream, 
    CAccPolSubblockNameArray& aArray)
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolCapsSubblockNameArraySerial::InternalizeL()" );

    TUint16 count( aStream.ReadUint16L() );
    TAccPolSubblockNameArrayAccessor::ResetArray( aArray );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TUint16 name( );
     	TAccPolSubblockNameArrayAccessor::AppendNameL( aArray, aStream.ReadUint16L() );
        }
    
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolCapsSubblockNameArraySerial::InternalizeL() - return void" );    
    }

// -----------------------------------------------------------------------------
// TAccPolCapsSubblockNameArraySerial::ExternalizeL
// Externalize name array data to the write stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolCapsSubblockNameArraySerial::ExternalizeL( 
    const CAccPolSubblockNameArray& aArray, 
    RWriteStream& aStream )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolCapsSubblockNameArraySerial::ExternalizeL()" );

    TUint16 count( TAccPolSubblockNameArrayAccessor::GetCount( aArray ) );
    aStream.WriteUint16L( count );
    for ( TInt16 i( 0 ); i < count; ++i )
        {
          TUint32 name;
      	  TAccPolSubblockNameArrayAccessor::GetName( aArray, i, name );
		  aStream.WriteUint16L( name );
        }
        
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolCapsSubblockNameArraySerial::ExternalizeL() - return void" );          
    }

//  End of File
