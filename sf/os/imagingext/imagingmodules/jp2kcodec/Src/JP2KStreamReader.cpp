/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  TJ2kStreamReader class is to implement the
*                MJ2kPacketHeaderReader interface for reading the
*                packet header and packet body from the stream
*                buffer provided by ICL framework.
*
*/


// INCLUDE FILES
#include <icl/imageprocessor.h>
#include <icl/imagecodec.h>
#include <icl/imagecodecdata.h>
#include "JP2KImageUtils.h"
#include "JP2KFormat.h"
#include "JP2KStreamReader.h"
#include "JP2KCodec.h"

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
// TJ2kStreamReader::UpdateMainHeader
// Update the codestream length read and offset of SOT marker
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TJ2kStreamReader::UpdateMainHeader()
    {
    if ( iCSLength )
        {
        // Codestream left
        iCSLength -= ( iPtr - iPtrStartMarker );
        }
    iStartSOT += ( iPtr - iPtrStartMarker );
    }

// -----------------------------------------------------------------------------
// TJ2kStreamReader::UpdateTileHeader
// Update the codestream length read and length of data consumed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TJ2kStreamReader::UpdateTileHeader()
    {
    if ( iCSLength )
        {
        // Codestream left
        iCSLength -= ( iPtr - iPtrStartMarker );
        }
    iDataUsed += ( iPtr - iPtrStartMarker );
    }

// -----------------------------------------------------------------------------
// TJ2kStreamReader::ReadEPHMarker
// Try to consume the EPH marker if there is one
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 TJ2kStreamReader::ReadEPHMarker()
    {
    if ( ( iPtrEnd - iPtr ) < 2 )
        {
        // EPH marker should be tried on next read
        iTryEPHMarker = ETrue;
        }
    else
        {
        iTryEPHMarker = EFalse;

        // Try to consume the EPH marker
        if ( PtrReadUtil::ReadBigEndianUint16( iPtr ) == KEPH )
            {
            iPtr += 2;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TJ2kStreamReader::ReadBit
// Read a bit from the packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 TJ2kStreamReader::ReadBit( TUint8& aBit )
    {
    if ( iPos == 0 )
        {
        if ( iPtr < iPtrEnd )
            {
            // Buffer contains some packet header data
            iData = *iPtr++;
            iPos = iNextPos;
            if ( iNextPos == 0x08 && iData == 0xff )
                {
                iNextPos = 0x07;
                }
            else
                {
                iNextPos = 0x08;
                }
            }
        else
            {
            // No more data
            return ETrue;
            }
        }
    aBit = (TUint8)( ( iData >> ( --iPos ) ) & 0x01 );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TJ2kStreamReader::ReadBits
// Read some bits from the packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 TJ2kStreamReader::ReadBits( TUint8& aBit, TUint8 aBitLen )
    {
    aBit = (TUint8)0;
    TUint8 bit;
    for ( TInt8 index = (TInt8)( aBitLen - 1 ); index >= 0; --index )
        {
        if ( !ReadBit( bit ) )
            {
            aBit |= ( bit << index );
            }
        else
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TJ2kStreamReader::ReadBits
// Read some bits from the packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 TJ2kStreamReader::ReadBits( TUint32& aBit, TUint8 aBitLen )
    {
    aBit = (TUint32)0;
    TUint8 bit = 0;
    for ( TInt8 index = ( TInt8 )( aBitLen - 1 ); index >= 0; --index )
        {
        if ( !ReadBit( bit ) )
            {
            aBit |= ( bit << index );
            }
        else
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TJ2kStreamReader::StartReadBit
// Start reading from packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 TJ2kStreamReader::StartReadBit()
    {
    if ( iAlign )
        {
        // Byte alignment is required
        ++iPtr;
        iAlign = 0;
        }
    if ( iPtr < iPtrEnd )
        {
        // Buffer contains some packet header data
        iData = *iPtr++;
        iPos = iNextPos = 0x08;
        if ( iData == 0xff )
            {
            iNextPos = 0x07;
            }

        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// TJ2kStreamReader::AlignReader
// Align the stream to the next byte boundary if necessary.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TJ2kStreamReader::AlignReader()
    {
    iAlign = 0;
    if ( iNextPos == 0x07 )
        {
        if ( iPtr < iPtrEnd )
            {
            ++iPtr;
            }
        else
            {
            // byte alignment is required on next read
            iAlign = 1;
            }
        }
    iData = iPos = iNextPos = 0;
    }

// -----------------------------------------------------------------------------
// TJ2kStreamReader::ReadSOPMarker
// Try to consume the SOP marker if there is one
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 TJ2kStreamReader::ReadSOPMarker()
    {
    if ( ( iPtrEnd - iPtr ) < 6 )
        {
        return ETrue;
        }
    else
        {
        if ( PtrReadUtil::ReadBigEndianUint16( iPtr ) == KSOP )
            {
            iPtr += 2;
            if ( PtrReadUtil::ReadBigEndianUint16( iPtr ) == KSOP_LEN )
                {
                // SOP marker found
                iPtr += 4;
                }
            else
                {
                // SOP marker not found, backup the iterator
                iPtr -= 2;
                }
            }
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// TJ2kStreamReader::TryReadEPHMarker
// Try to consume the EPH marker after failure on previous try
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 TJ2kStreamReader::TryReadEPHMarker()
    {
    if ( iTryEPHMarker )
        {
        if ( ( iPtrEnd - iPtr ) < 2 )
            {
            return ETrue;
            }
        else
            {
            if ( PtrReadUtil::ReadBigEndianUint16( iPtr ) == KEPH )
                {
                iPtr += 2;
                }
            iTryEPHMarker = EFalse;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TJ2kStreamReader::TryReAlignReader
// Try to align to byte boundary after failure on previous try
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 TJ2kStreamReader::TryReAlignReader()
    {
    if ( iAlign )
        {
        if ( ( iPtrEnd - iPtr ) < 1 )
            {
            return ETrue;
            }
        else
            {
            ++iPtr;
            iAlign = 0;
            }
        }
    return EFalse;
    }


