// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : TTlv.cpp
// Part of     : Series 60 / utility
// Based on 3GPP TS 11.14 V8.8.0 
// Version     : 1.0
// EXTERNAL RESOURCES  
//



//  Include Files  

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ttlvTraces.h"
#endif

#include "TTlv.h"				    // Header of this class

//  External Data Structures  
//    none

//  External Function Prototypes  
//    none

//  LOCAL CONSTANTS AND MACROS  
//    none

//  MODULE DATA STRUCTURES  
//    none

//  Local Data Structures  
//    none

//  Local Function Prototypes  
//    none

//  LOCAL FUNCTIONS
//    none

//  MEMBER FUNCTIONS


//=============================================================================



// -----------------------------------------------------------------------------
// TTlvBase::TTlvBase
// constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TTlvBase::TTlvBase
        ( 
        TDes8& aData
        ) : iData(aData)
    {
    // Do nothing.
    }

// -----------------------------------------------------------------------------
// TTlvBase::Begin
// Start BER-TLV construction
// -----------------------------------------------------------------------------
//
EXPORT_C void TTlvBase::Begin
        ( 
        TUint8 aTag 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TTLVBASE_BEGIN_1, "UTILITY: TTlvBase::Begin");
    iData.Zero();
    iData.Append( aTag );
    iData.Append( 0 );
    }

// -----------------------------------------------------------------------------
// TTlvBase::AddTag
// Start new simple TLV
// -----------------------------------------------------------------------------
//
EXPORT_C void TTlvBase::AddTag
        ( 
        TUint8 aTag 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TTLVBASE_ADDTAG_1, "UTILITY: TTlvBase::AddTag");
    iData.Append( aTag );           // tag
    iData.Append( 0 );              // length
    iLenIndex = iData.Length() - 1; // index to tag length
    }

// -----------------------------------------------------------------------------
// TTlvBase::AddByte
// Add byte data to simple TLV
// -----------------------------------------------------------------------------
//
EXPORT_C void TTlvBase::AddByte
        ( 
        TUint8 aValue  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TTLVBASE_ADDBYTE_1, "UTILITY: TTlvBase::AddByte");
    iData.Append( aValue );
    iData[iLenIndex]++;
    }

// -----------------------------------------------------------------------------
// TTlvBase::AddData
// Add desctiptor data to simple TLV
// -----------------------------------------------------------------------------
//
EXPORT_C void TTlvBase::AddData
        ( 
        const TDesC8& aValue 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TTLVBASE_ADDDATA_1, "UTILITY: TTlvBase::AddData");
    iData.Append( aValue );
    iData[ iLenIndex ] = static_cast<TUint8>( iData[ iLenIndex ] + 
        aValue.Length() );
    }

// -----------------------------------------------------------------------------
// TTlvBase::End
// End BER-TLV construction. Adds extra length byte to all simple TLVs that 
// are longer than KMaxOneByteLength (0x7f) and returns descriptor to the new 
// valid BER-TLV.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& TTlvBase::End
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TTLVBASE_END_1, "UTILITY: TTlvBase::End");
    _LIT8( KLenTag, "\x81" );

    TInt i( 3 ); // Index to tag length

    // Scan through all lenghts and insert 'length tag'
    while ( i < iData.Length() )
        {
        if ( iData[i] > KMaxOneByteLength )
            {
            iData.Insert( i, KLenTag );
            i++;
            }
        i += iData[i] + 2; // jump to next tag length
        }

    // Calculate total BER-TLV length
    iData[1] = static_cast<TUint8>( iData.Length() - 2 );
    // Insert 'length tag'
    if ( iData[1] > KMaxOneByteLength )
        {
        iData.Insert( 1, KLenTag );
        }

    // Done, return BER-TLV
    return iData;
    }

// -----------------------------------------------------------------------------
// TTlvBase::GetDataWithoutTopLevelTag
// End TLV construction. Adds extra length byte to all simple TLVs that are 
// longer than KMaxOneByteLength (0x7f) and returns descriptor to the valid 
// TLVs. To be used without Begin(), nor End() in situations where TLV simple 
// tags are used without top level tag.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& TTlvBase::GetDataWithoutTopLevelTag
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TTLVBASE_GETDATAWITHOUTTOPLEVELTAG_1, "UTILITY: TTlvBase::GetDataWithoutTopLevelTag");
    _LIT8( KLenTag, "\x81" );

    TInt i( 1 ); // Index to tag length

    // Scan through all lenghts and insert 'length tag'
    while ( i < iData.Length() )
        {
        if ( iData[i] > KMaxOneByteLength )
            {
            iData.Insert( i, KLenTag );
            i++;
            }
        i += iData[i] + 2; // Jump to next tag length
        }

    // Done, return BER-TLV
    return iData;

    }

//End Of File
