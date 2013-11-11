// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CBerTlv.cpp
// Part of     : Series 60 / utility
// Based on 3GPP TS 11.14 V8.8.0 
// Version     : 1.0
// EXTERNAL RESOURCES  
//



//  Include Files  

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cbertlvTraces.h"
#endif

#include "CBerTlv.h"				// Header of this class

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
// CBerTlv::CBerTlv
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CBerTlv::CBerTlv
        (
        // None
        )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CBerTlv::TlvByTagValue
// Find TLV by tag value. aTlv is set to point to found TLV. If there are more 
// than one TLV with the same tag then itemNbr can be used to select which one 
// is returned. If no tags are found return KErrNotFound else KErrNone.  
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CBerTlv::TlvByTagValue
        (
        CTlv* aTlv,                 // TLV
        TUint8 aTlvTagValue,        // TLV identifier
        TInt itemNbr                // Item number
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CBERTLV_TLVBYTAGVALUE_1, "UTILITY: CBerTlv::TlvByTagValue");
    TInt currentTlv( 0 );
    TInt tlvLength( 0 );
    TInt index( 0 );

    // TLV start index
    TInt ind( KCommonTlvHeaderLength );
    TUint8 berTlvLength( 0 );
    if ( KTwoByteLengthCoding == iData[KTlvLengthStartPosition] )
        {
        berTlvLength = iData[KTlvLengthStartPosition + 1];
        ind = ind + 1;
        }
    else
        {
        berTlvLength = iData[KTlvLengthStartPosition];
        }

    for ( TInt i = 0; i < berTlvLength; i += tlvLength )
        {
        // First determine if the length of the TLV is coded with 1 or 2 bytes.
        if ( KTwoByteLengthCoding == iData[ind + 1] )
            {
            // Length is coded with 2 bytes -> 1 extra byte required to be
            // added to total length. Also TLV header bytes (2) must be added
            // to total length
            tlvLength = iData[ind + 2] + KTlvHeaderLength + 1;            
            }
        else
            {
            // TLV header bytes (2) must be added to total length
            tlvLength = iData[ind + 1] + KTlvHeaderLength;
            }

        currentTlv = iData[ind]&KTagValueMask;

        // Check if TLV in current index is the one that we are searching for
        if ( aTlvTagValue == currentTlv )
            {
            if( index == itemNbr)
                {
                TPtrC8 data = ( iData.Mid( ind , tlvLength ) );
                aTlv->SetData( data );
                return KErrNone;
                }
            else
                {
                index++;
                }
            }
            
        ind += tlvLength;
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CBerTlv::TlvByTagValueMulti
// Find several TLVs by tag value. It is assumed that TLVs are sequential. aTlv 
// is set to point to TLV sequence. If no tags are found return KErrNotFound 
// else KErrNone.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CBerTlv::TlvByTagValueMulti
        (
        CTlv* aTlv,                 // TLV to fill
        TUint8 aTlvTagValue         // Tag of TLV to find
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CBERTLV_TLVBYTAGVALUEMULTI_1, "UTILITY: CBerTlv::TlvByTagValueMulti");
    TInt currentTlv( 0 );
    TInt16 tlvLength( 0 );
    TInt indMulti( 0 );
    TInt16 multiLength( 0 );
    // TLV start index
    TInt ind( KCommonTlvHeaderLength );
    TUint8 berTlvLength;
    // First determine if the length of the BerTLV is coded with 1 or 2 bytes.
    if ( KTwoByteLengthCoding == iData[KTlvLengthStartPosition] )
        {
        berTlvLength = iData[KTlvLengthStartPosition + 1];
        ind = ind + 1;
        }
    else
        {
        berTlvLength = iData[KTlvLengthStartPosition];
        }

    for ( TInt i = 0; i < berTlvLength; i+= tlvLength )
        {
        // First determine if the length of the TLV is coded with 1 or 2 bytes.
        if ( KTwoByteLengthCoding == iData[ind + 1] )
            {
            // Length is coded with 2 bytes -> real length is in second byte.
            // first byte is 81 and it is tag for 2 byte length coding.
            tlvLength = static_cast<TInt16>( iData[ind + 0x02] + 
                KTlvHeaderLength + 1 );
            }        
        else
            {
            // TLV header bytes (2) must be added to total length
            tlvLength = static_cast<TInt16>( iData[ind + 1]  + 
                KTlvHeaderLength );
            }

        currentTlv = iData[ind]&KTagValueMask;

        // Check if TLV in current index is the one that we are searching for
        if ( aTlvTagValue == currentTlv && multiLength == 0x00 )
            {
            // Save first tag position
            indMulti = ind;
            multiLength = tlvLength;            
            }
        // Add length
        if ( aTlvTagValue == currentTlv && indMulti != ind )
            {
            multiLength = static_cast<TInt16>( multiLength + tlvLength );
            }

        // If last tag, return
        if ( (i + tlvLength) >= berTlvLength )
            {
            TPtrC8 data = ( iData.Mid( indMulti , multiLength ) );
            aTlv->SetData( data );
            break;
            }
        ind += tlvLength;
        }
    
    if( 0 == indMulti)
    	{
    	return KErrNotFound;
    	}
    else
    	{
    	return KErrNone;
    	}    
    }


//End Of File
