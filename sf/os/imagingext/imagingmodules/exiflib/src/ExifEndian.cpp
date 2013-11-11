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
* Description:  Exif data parser, that can parse both Little and Big endian 
*                formats
*
*/


// INCLUDE FILES
#include "ExifEndian.h"
#include "ExifCommon.h"

// ============================ CLASS CExifEndianBase ==========================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifEndianBase::CExifEndianBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifEndianBase::CExifEndianBase()
    {
    }

// -----------------------------------------------------------------------------
// CExifEndianBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifEndianBase::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CExifEndianBase::NewBaseL
// Two-phased constructor. Instantiates and returns relevant derived class.
// -----------------------------------------------------------------------------
//
CExifEndianBase* CExifEndianBase::NewBaseL( 
    const TUint8* aExifDataPtr, 
    TUint aDataLength, 
    TBool aIsExif )
    {
    // 
    // Parse header. 
    // Find SOI (FFD8 )
    // ptr = SOI pos.
    // Then check the following 10 bytes:
    // APP1 Marker (FFE1) + APP1 Length (no check) + Identifier ("Exif"00) + 
    // Pad (00) (no check)

    //TUint8* dataStartPtr = CONST_CAST( TUint8*, aExifData.Ptr() ); 
    if ( !aExifDataPtr )
        {
        User::Leave( KErrGeneral );
        }
    TUint8* ptr = CONST_CAST( TUint8*, aExifDataPtr ) - 1;
    TUint8 *pE = CONST_CAST( TUint8*, aExifDataPtr ) + aDataLength; 

    // Locate the SOI marker.
    do
        {
        if ( ptr == pE )
            {
            User::Leave( KErrCorrupt );
            }
        } 
    while ( *++ptr != KMarkerStart );
    
    if ( *( ptr + 1 ) != KSoiEnd )
        {
        User::Leave( KErrCorrupt );
        }
    // ptr is now at SOI.

    // If original is Exif, check whether it is Little- or Big-endian.
    // Otherwise create Little-endian reader.
    if ( aIsExif )
        {
        // Locate the APP1 marker
        do
            {
            do
                {
                if ( ptr == pE )
                    {
                    User::Leave( KErrCorrupt );
                    }
                } 
            while ( *++ptr != KMarkerStart );
            } 
        while ( *( ptr + 1 ) != KApp1End );
        // ptr is now at APP1
        // Skip App1 length.
        ptr += 4;

        // Check the APP1 and Exif header.
        if ( TExifCommon::Uint32L( ptr ) != KExifIdentifierRev ) 
            {
            User::Leave( KErrCorrupt );
            }
        ptr += 4;

        // Padding 0 after 8 bytes of data. No need to check, skip.
        //if ( ( *ptr ) != 0)
        //    {
        //    User::Leave( KErrCorrupt );
        //    }
        ptr += 2;

        // Check if little- or big- endian, and create the reader.
        if ( TExifCommon::Uint16L( ptr ) == KLittleEndian )
            {
            return CExifLittleEndian::NewL( aExifDataPtr, ptr, pE, aIsExif );
            }
        else if ( TExifCommon::Uint16L( ptr ) == KBigEndian )
            {
            return CExifBigEndian::NewL( aExifDataPtr, ptr, pE, aIsExif );
            }
        else // None of the formats match!!
            {
            User::Leave( KErrCorrupt );
            }
        }
    else
        {
        return CExifLittleEndian::NewL( aExifDataPtr, ptr, pE, aIsExif );
        }

    return NULL;
    }

// Destructor
CExifEndianBase::~CExifEndianBase()
    {
    }

// -----------------------------------------------------------------------------
// CExifEndianBase::CopyBufferL
// Copies the data from the location, which starts and ends with the given 
// Jpeg Markers.
// -----------------------------------------------------------------------------
//
void CExifEndianBase::CopyBufferL( 
    TUint16 aStartMarker, 
    TUint16 aEndMarker, 
    HBufC8*& aDestBuffer, 
    TBool aIncludeEnd )
    {
    TUint32 startOffset = 0;
    if ( LocateJpegMarker( aStartMarker, startOffset ) )
        {
        User::Leave( KErrCorrupt );
        }
    TUint32 endOffset = 0;
    if ( LocateJpegMarker( aEndMarker, endOffset ) )
        {
        User::Leave( KErrCorrupt );
        }
    TUint32 size =  endOffset - startOffset;
    if ( aIncludeEnd )
        {
        size += 2;
        }
    aDestBuffer = HBufC8::NewL( size );
    CleanupStack::PushL( aDestBuffer );

    iPosOffset = startOffset;

    CopyBuffer( size, 1 , aDestBuffer );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CExifEndianBase::ReadUint8
// Gets the next 8-bit unsigned integer from the data.
// -----------------------------------------------------------------------------
//
TInt CExifEndianBase::ReadUint8( TUint8& aInt8 )
    {
    if ( iPosOffset + 1 >= iExifEndOffset )
        {
        return KErrCorrupt;
        }
    aInt8 = *( iDataStartPtr + iPosOffset );
    ++iPosOffset;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifEndianBase::MoveTo
// Moves to the specified offset.
// -----------------------------------------------------------------------------
//
TInt CExifEndianBase::MoveTo( TUint aOffset )
    {
    if ( iExifStartOffset + aOffset >= iExifEndOffset )
        {
        return KErrCorrupt;
        }
    iPosOffset = iExifStartOffset + aOffset;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifEndianBase::Skip
// Moves the current position forward by the given number of bytes.
// -----------------------------------------------------------------------------
//
TInt CExifEndianBase::Skip( TUint aNoBytes )
    {
    if ( iPosOffset + aNoBytes >= iExifEndOffset )
        {
        return KErrCorrupt;
        }
    iPosOffset += aNoBytes;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifEndianBase::Locate8
// Locates the offset of the next specified 8-bit unsigned integer 
// -----------------------------------------------------------------------------
//
TInt CExifEndianBase::Locate8( TUint8 aSearch, TUint32& aOffset )
    {
    TUint8 *pBuf = iDataStartPtr + iPosOffset;
    TUint8 *pB = pBuf - 1;
    TUint8* pE = iDataStartPtr + iExifEndOffset;
    do
        {
        if ( pB == pE )
            {
            return KErrCorrupt;
            }
        } 
    while ( *++pB != aSearch );
    aOffset = pB - iDataStartPtr;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifEndianBase::LocateJpegMarker
// Locates the offset of the specified Jpeg Marker, which resides after the 
// given offset.
// -----------------------------------------------------------------------------
//
TInt CExifEndianBase::LocateJpegMarker( 
    TUint16 aMarker, 
    TUint32& aOffset, 
    TUint32 aAfter,
    TUint32 aBefore )
    {
    if ( aAfter < iPosOffset )
        {
        aAfter = iPosOffset;
        }
    TUint8* startPtr = iDataStartPtr + aAfter;
    if ( !aBefore )
        {
        aBefore = iExifEndOffset;
        }
    TUint8* endPtr = iDataStartPtr + aBefore;
    TUint8* pB = TExifCommon::LocateJpegMarkerPtr( 
        aMarker, startPtr, endPtr );
    if ( !pB )
        {
        return KErrCorrupt;
        }
    aOffset = pB - iDataStartPtr;
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CExifEndianBase::LocateJpegMarkerFromEnd
// Locates the offset of the last specified Jpeg Marker.
// -----------------------------------------------------------------------------
//
TInt CExifEndianBase::LocateJpegMarkerFromEnd( 
    TUint16 aMarker, 
    TUint32& aOffset,
    TUint32 aAfter,
    TUint32 aBefore )
    {
    if ( aAfter < iPosOffset)
        {
        aAfter = iPosOffset;
        }
    TUint8* startPtr = iDataStartPtr + aAfter;
    if ( !aBefore )
        {
        aBefore = iExifEndOffset;
        }
    TUint8* endPtr = iDataStartPtr + aBefore;
    TUint8* pB = TExifCommon::LocateJpegMarkerPtrFromEnd( aMarker, 
        startPtr, endPtr );
    if ( !pB )
        {
        return KErrCorrupt;
        }
    aOffset = pB - iDataStartPtr;
    return KErrNone;    
    }


// ============================ CLASS CExifLittleEndian ========================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifLittleEndian::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifLittleEndian* CExifLittleEndian::NewL( 
    const TUint8* aDataStartPtr, 
    TUint8* aExifStartPtr, 
    TUint8* aExifEndPtr, 
    TBool aIsExif )
    {
    CExifLittleEndian* self = new( ELeave ) CExifLittleEndian();
    CleanupStack::PushL( self );
    self->ConstructL( CONST_CAST( TUint8*, aDataStartPtr ), aExifStartPtr, 
        aExifEndPtr, aIsExif );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CExifLittleEndian::CExifLittleEndian
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifLittleEndian::CExifLittleEndian()
    {
    }

// -----------------------------------------------------------------------------
// CExifLittleEndian::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifLittleEndian::ConstructL( 
    TUint8* aDataStartPtr, 
    TUint8* aExifStartPtr, 
    TUint8* aExifEndPtr, 
    TBool aIsExif )
    {
    if ( !aDataStartPtr || !aExifStartPtr || !aExifEndPtr )
        {
        User::Leave( KErrGeneral );
        }
    iDataStartPtr = aDataStartPtr;
    iExifEndOffset = aExifEndPtr - iDataStartPtr;
    if ( aIsExif )
        {
        iExifStartOffset = aExifStartPtr - iDataStartPtr;
        if ( TExifCommon::Uint16L( aExifStartPtr + 2 ) != KExifDummyRev )
            {
            User::Leave( KErrCorrupt );
            }
        TUint32 ifdOffset = TExifCommon::Uint32L( aExifStartPtr + 4 );
        iPosOffset = iExifStartOffset + ifdOffset; //Locate 0th IFD position
        if ( iPosOffset >= iExifEndOffset )
            {
            User::Leave( KErrCorrupt );
            }
        }
    else
        {
        iPosOffset = aExifStartPtr - iDataStartPtr;
        }
    }

// Destructor
CExifLittleEndian::~CExifLittleEndian()
    {
    }

// -----------------------------------------------------------------------------
// CExifLittleEndian::Locate16
// Locates the offset of the next specified 16-bit unsigned integer.
// -----------------------------------------------------------------------------
//
TInt CExifLittleEndian::Locate16( TUint16 aSearch, TUint32& aOffset )
    {
    TUint8* pB = iDataStartPtr + iPosOffset - 1;
    TUint16 atHand = *pB;
    TUint8* pE = iDataStartPtr + iExifEndOffset;
    TInt error = KErrNone;
    do
        {
        if ( pB >= pE )
            {
            return KErrCorrupt;
            }
        TRAP( error, atHand =TExifCommon::Uint16L( ++pB ) );
        } 
    while ( ( atHand != aSearch ) && ( error == KErrNone ) );
    if ( error )
        {
        return error;
        }
    aOffset = pB - iDataStartPtr;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifLittleEndian::ReadUint16
// Gets the next 16-bit unsigned integer from the data.
// -----------------------------------------------------------------------------
//
TInt CExifLittleEndian::ReadUInt16( TUint16& aInt16 )
    {
    if ( iPosOffset + 2 >= iExifEndOffset )
        {
        return KErrCorrupt;
        }
    
    TRAPD( error, aInt16 = TExifCommon::Uint16L( iDataStartPtr + iPosOffset ) );

    if ( error )
        {
        return error;
        }
    
    iPosOffset += 2;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifLittleEndian::ReadUint32
// Gets the next 32-bit unsigned integer from the data.
// -----------------------------------------------------------------------------
//
TInt CExifLittleEndian::ReadUInt32( TUint32& aInt32 )
    {
    if ( iPosOffset + 4 >= iExifEndOffset )
        {
        return KErrCorrupt;
        }
    
    TRAPD( error, aInt32 = TExifCommon::Uint32L( iDataStartPtr + iPosOffset ) );
    if ( error )
        {
        return error;
        }
    
    iPosOffset += 4;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifLittleEndian::CopyBuffer
// Copies the next specified amount of data to the given descriptor.
// -----------------------------------------------------------------------------
//
TInt CExifLittleEndian::CopyBuffer( 
    TUint32 aCount, 
    TUint8 aWordSize, 
    HBufC8*& aBuffer )
    {
    if ( !aBuffer )
        {
        return KErrCorrupt;
        }
    TUint bufferSize = aCount * aWordSize;
    if ( iPosOffset + bufferSize >= iExifEndOffset )
        {
        return KErrCorrupt;
        }
    
    aBuffer->Des().Copy( iDataStartPtr + iPosOffset, bufferSize );

    iPosOffset += bufferSize;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifLittleEndian::CopyBuffer
// Copies the specified amount of data located in the given offset to the 
// given descriptor.
// -----------------------------------------------------------------------------
//
TInt CExifLittleEndian::CopyBuffer( 
    TUint32 aOffset, 
    TUint32 aCount, 
    TUint8 aWordSize, 
    HBufC8*& aBuffer )
    {
    if ( !aBuffer )
        {
        return KErrCorrupt;
        }
    TUint bufferSize = aCount * aWordSize;
    if ( ( aOffset + iExifStartOffset + bufferSize ) >= iExifEndOffset )
        {
        return KErrCorrupt;
        }
    
    TUint8* ptr = iDataStartPtr + iExifStartOffset + aOffset;
    aBuffer->Des().Copy( ptr, bufferSize );

    return KErrNone;
    }


// ============================ CLASS CExifBigEndian ===========================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifBigEndian::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifBigEndian* CExifBigEndian::NewL( 
    const TUint8* aDataStartPtr, 
    TUint8* aExifStartPtr, 
    TUint8* aExifEndPtr, 
    TBool aIsExif )
    {
    CExifBigEndian* self = new( ELeave ) CExifBigEndian();
    CleanupStack::PushL( self );
    self->ConstructL( CONST_CAST( TUint8*, aDataStartPtr ), aExifStartPtr, 
        aExifEndPtr, aIsExif );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CExifBigEndian::CExifBigEndian
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifBigEndian::CExifBigEndian()
    {
    }

// -----------------------------------------------------------------------------
// CExifBigEndian::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifBigEndian::ConstructL( 
    TUint8* aDataStartPtr, 
    TUint8* aExifStartPtr, 
    TUint8* aExifEndPtr, 
    TBool aIsExif )
    {
    if ( !aDataStartPtr || !aExifStartPtr || !aExifEndPtr )
        {
        User::Leave( KErrGeneral );
        }
    iDataStartPtr = aDataStartPtr;
    iExifEndOffset = aExifEndPtr - iDataStartPtr;
    if ( aIsExif )
        {
        iExifStartOffset = aExifStartPtr - iDataStartPtr;
        if ( TExifCommon::Uint16L( aExifStartPtr + 2 ) != KExifDummy )
            {
            User::Leave( KErrCorrupt );
            }

        TUint8* tmpPtr = aExifStartPtr + 4;
        TUint32 ifdOffset = STATIC_CAST( TUint32,  ( *tmpPtr << 24 ) 
            + ( ( *( tmpPtr + 1 ) ) << 16 ) +  ( ( *( tmpPtr + 2 ) ) << 8 ) 
            + ( *( tmpPtr + 3 ) ) ); 
        iPosOffset = ( iExifStartOffset + ifdOffset ); //Locate 0th IFD pos.
        if ( iPosOffset >= iExifEndOffset )
            {
            User::Leave( KErrCorrupt );
            }
        }
    else
        {
        iPosOffset = aExifStartPtr - iDataStartPtr;
        }
    }

// Destructor
CExifBigEndian::~CExifBigEndian()
    {
    }

// -----------------------------------------------------------------------------
// CExifBigEndian::Locate16
// Locates the offset of the next specified 16-bit unsigned integer.
// -----------------------------------------------------------------------------
//
TInt CExifBigEndian::Locate16( TUint16 aSearch, TUint32& aOffset )
    {
    TUint8* pB = iDataStartPtr + iPosOffset;
    TUint16 atHand = *pB;
    TUint8* pE = iDataStartPtr + iExifEndOffset;
    do
        {
        if ( pB >= pE )
            {
            return KErrCorrupt;
            }
        atHand = STATIC_CAST( TUint16, ( atHand << 8 ) + *++pB );
        } 
    while ( atHand != aSearch );
    aOffset = ( pB - 1 ) - iDataStartPtr;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifBigEndian::ReadUInt16
// Gets the next 16-bit unsigned integer from the data.
// -----------------------------------------------------------------------------
//
TInt CExifBigEndian::ReadUInt16( TUint16& aInt16 )
    {
    if ( iPosOffset + 2 >= iExifEndOffset )
        {
        return KErrCorrupt;
        }

    aInt16 = STATIC_CAST( TUint16,  ( *( iDataStartPtr + iPosOffset ) << 8 ) 
        + *( iDataStartPtr + iPosOffset + 1 ) ); 
    
    iPosOffset += 2;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifBigEndian::ReadUInt32
// Gets the next 32-bit unsigned integer from the data.
// -----------------------------------------------------------------------------
//
TInt CExifBigEndian::ReadUInt32( TUint32& aInt32 )
    {
    if ( iPosOffset + 4 >= iExifEndOffset )
        {
        return KErrCorrupt;
        }

    aInt32 = STATIC_CAST( TUint32,  ( *( iDataStartPtr + iPosOffset ) << 24 ) 
        + ( *( iDataStartPtr + iPosOffset + 1 ) << 16 ) 
        + ( *( iDataStartPtr + iPosOffset + 2 ) << 8 ) 
        + *( iDataStartPtr + iPosOffset + 3 ) ); 

    iPosOffset += 4;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifBigEndian::CopyBuffer
// Copies the next specified amount of data to the given descriptor.
// -----------------------------------------------------------------------------
//
TInt CExifBigEndian::CopyBuffer( 
    TUint32 aCount, 
    TUint8 aWordSize, 
    HBufC8*& aBuffer )
    {
    if ( !aBuffer )
        {
        return KErrCorrupt;
        }
    TInt bufferSize = aCount * aWordSize;
    if ( iPosOffset + bufferSize >= iExifEndOffset )
        {
        return KErrCorrupt;
        }
    
    TUint8* posPtr = iDataStartPtr + iPosOffset;
    if ( aWordSize == 1 )
        {
        aBuffer->Des().Copy( posPtr, bufferSize );
        iPosOffset += bufferSize;
        }
    else if ( aWordSize == 2 )
        {    
        TUint8* destPtr = CONST_CAST( TUint8*, aBuffer->Ptr() );
        for( TUint i = 0; i < aCount; ++i)
            {
            destPtr[0] = posPtr[1];
            destPtr[1] = *posPtr;
            posPtr += 2;
            iPosOffset += 2;
            destPtr += 2;
            }
        }
    else
        {    
        TUint8* destPtr = CONST_CAST( TUint8*, aBuffer->Ptr() );
        for( TInt i = 0; i < bufferSize / 4; ++i)
            {
            destPtr[0] = posPtr[3];
            destPtr[1] = posPtr[2];
            destPtr[2] = posPtr[1];
            destPtr[3] = *posPtr;
            posPtr += 4;
            iPosOffset += 4;
            destPtr += 4;
            }
        }
    aBuffer->Des().SetLength( bufferSize );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifBigEndian::CopyBuffer
// Copies the specified amount of data located in the given offset to the 
// given descriptor.
// -----------------------------------------------------------------------------
//
TInt CExifBigEndian::CopyBuffer( 
    TUint32 aOffset, 
    TUint32 aCount, 
    TUint8 aWordSize, 
    HBufC8*& aBuffer )
    {
    if ( !aBuffer )
        {
        return KErrCorrupt;
        }
    TInt bufferSize = aCount * aWordSize;
    if ( ( aOffset + iExifStartOffset + bufferSize ) >= iExifEndOffset )
        {
        return KErrCorrupt;
        }
    
    TUint8* ptr = iDataStartPtr + iExifStartOffset + aOffset;
    if ( aWordSize == 1 )
        {
        aBuffer->Des().Copy( ptr, bufferSize );
        }
    else if ( aWordSize == 2 )
        {
        TUint8* destPtr = CONST_CAST( TUint8*, aBuffer->Ptr() );
        for( TUint i = 0; i < aCount; ++i)
            {
            destPtr[0] = ptr[1];
            destPtr[1] = *ptr;
            destPtr += 2;
            ptr += 2;
            }
        }
    else
        {
        TUint8* destPtr = CONST_CAST( TUint8*, aBuffer->Ptr() );
        for( TInt i = 0; i < bufferSize / 4; ++i)
            {
            destPtr[0] = ptr[3];
            destPtr[1] = ptr[2];
            destPtr[2] = ptr[1];
            destPtr[3] = *ptr;
            destPtr += 4;
            ptr += 4;
            }
        }
    aBuffer->Des().SetLength( bufferSize );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifEndianBase::CurrentPosOffset
// Returns the current position of decoded data
// -----------------------------------------------------------------------------
//
void CExifEndianBase::CurrentPosOffset( TUint32& aOffset )
    {
    aOffset = iPosOffset;
    }
