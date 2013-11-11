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
* Description:  Exif data parser for both Little and Big Endian formats.
*
*/


#ifndef EXIFENDIAN_H
#define EXIFENDIAN_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Exif data parser base class.
*  Provides the base for Little and Big endian parsers.
*
*  @lib ExifLib
*  @since 2.6 
*/
NONSHARABLE_CLASS( CExifEndianBase ): public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CExifEndianBase* NewBaseL( 
            const TUint8* aExifDataPtr, 
            TUint aDataLength, 
            TBool aIsExif );

        /**
        * Destructor.
        */
        virtual ~CExifEndianBase();

        /**
        * Gets the next 8-bit unsigned integer from the data.
        * @since 2.6 
        * @param aUint8 The 8-bit integer returned.
        * @return Error code.
        */
        TInt ReadUint8( TUint8& aUint8 );

        /**
        * Gets the next 16-bit unsigned integer from the data.
        * @since 2.6 
        * @param aUint16 The 16-bit integer returned.
        * @return Error code.
        */
        virtual TInt ReadUInt16( TUint16& aUint16 ) = 0;

        /**
        * Gets the next 32-bit unsigned integer from the data.
        * @since 2.6 
        * @param aUint32 The 32-bit integer returned.
        * @return Error code.
        */
        virtual TInt ReadUInt32( TUint32& aUint32 ) = 0;

        /**
        * Copies the data from the location, which starts and ends with the 
        * given Jpeg Markers.
        * @since 2.6 
        * @param aStartMarker Jpeg Marker for the beginning of copying.
        * @param aEndMarker Jpeg Marker for the end of copying.
        * @param aDestBuffer Pointer to the target descriptor.
        * @param aIncludeEnd States if the ending Jpeg Marker will be copied 
        *                    or not.
        * @return void
        */
        void CopyBufferL( 
            TUint16 aStartMarker, 
            TUint16 aEndMarker, 
            HBufC8*& aDestBuffer, 
            TBool aIncludeEnd = ETrue );

        /**
        * Copies the next specified amount of data to the given descriptor.
        * @since 2.6 
        * @param aCount Number of words to copy
        * @param aWordSize Word size
        * @param aBuffer Pointer to the target descriptor.
        * @return void
        */
        virtual TInt CopyBuffer( 
            TUint32 aCount, 
            TUint8 aWordSize, 
            HBufC8*& aBuffer ) = 0;

        /**
        * Copies the specified amount of data located in the given offset to 
        * the given descriptor.
        * @since 2.6 
        * @param aOffset Offset for starting copying
        * @param aCount Number of words to copy
        * @param aWordSize Word size
        * @param aBuffer Pointer to the target descriptor.
        * @return void
        */
        virtual TInt CopyBuffer( 
            TUint32 aOffset, 
            TUint32 aCount, 
            TUint8 aWordSize, 
            HBufC8*& aBuffer ) = 0;

        /**
        * Moves to the specified offset.
        * @since 2.6 
        * @param aOffset Target offset.
        * @return Error code.
        */
        TInt MoveTo( TUint aOffset );

        /**
        * Moves the current position forward by the given number of bytes.
        * @since 2.6 
        * @param aNoBytes Number of bytes to skip
        * @return Error code.
        */
        TInt Skip( TUint aNoBytes );

        /**
        * Locates the offset of the next specified 8-bit unsigned integer 
        * @since 2.6 
        * @param aSearch Queried 8-bit unsigned integer
        * @param aOffset The offset of the found location.
        * @return Error code.
        */
        TInt Locate8( TUint8 aSearch, TUint32& aOffset );

        /**
        * Locates the offset of the next specified 16-bit unsigned integer 
        * @since 2.6 
        * @param aSearch Queried 16-bit integer
        * @param aOffset The offset of the found location.
        * @return Error code.
        */
        virtual TInt Locate16( TUint16 aSearch, TUint32& aOffset ) = 0;

        /**
        * Locates the offset of the specified Jpeg Marker, which resides 
        * inside the given interval.
        * @since 2.6 
        * @param aMarker The queried Jpeg Marker
        * @param aOffset The offset of the found location.
        * @param aAfter Offset for starting searching.
        * @param aBefore Offset for ending searching.
        * @return Error code.
        */
        TInt LocateJpegMarker( 
            TUint16 aMarker, 
            TUint32& aOffset, 
            TUint32 aAfter = 0,
            TUint32 aBefore = 0 );

        /**
        * Locates the offset of the last specified Jpeg Marker, which resides
        * inside the given interval
        * @since 2.6 
        * @param aMarker The queried Jpeg Marker
        * @param aOffset The offset of the found location.
        * @param aAfter Offset for starting searching.
        * @param aBefore Offset for ending searching.
        * @return Error code.
        */
        TInt LocateJpegMarkerFromEnd( 
            TUint16 aMarker, 
            TUint32& aOffset, 
            TUint32 aAfter = 0,
            TUint32 aBefore = 0 );

        /**
        * Returns the current offset position
        * @since 3.1 
        * @param aOffset The offset of the location.
        * @return void.
        */
        void CurrentPosOffset( TUint32& aOffset );
        
    protected:

        /**
        * C++ default constructor.
        */
        CExifEndianBase();

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:  // Data

        // The offset of the current position in the Exif data.
        TUint32 iPosOffset;

        // The offset of the Exif header (APP1 header) in the Exif data.
        TUint32 iExifStartOffset;

        // The offset for the end of Exif data.
        TUint32 iExifEndOffset;

        // The pointer to the beginning of the Exif data.
        TUint8* iDataStartPtr;

    };


/**
*  Exif data parser class for Little endian format.
*
*  @lib ExifLib
*  @since 2.6 
*/
NONSHARABLE_CLASS( CExifLittleEndian ): public CExifEndianBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CExifLittleEndian* NewL( 
            const TUint8* aDataStartPtr,
            TUint8* aExifStartPtr, 
            TUint8* aExifEndPtr, 
            TBool aIsExif );

        /**
        * C++ default constructor.
        */
        CExifLittleEndian();

        /**
        * Destructor.
        */        
        virtual ~CExifLittleEndian();

    public: // Functions from base classes

        /**
        * From CExifEndianBase Gets the next 16-bit unsigned integer from the 
        * data.
        * @since 2.6 
        * @param aUint16 The 16-bit integer returned.
        * @return Error code.
        */
        TInt ReadUInt16( TUint16& aUint16 );

        /**
        * From CExifEndianBase Gets the next 32-bit unsigned integer from the 
        * data.
        * @since 2.6 
        * @param aUint32 The 32-bit integer returned.
        * @return Error code.
        */        
        TInt ReadUInt32( TUint32& aUint32 );
        
        /**
        * From CExifEndianBase Copies the next specified amount of data to the 
        * given descriptor.
        * @since 2.6 
        * @param aCount Number of words to copy
        * @param aWordSize Word size
        * @param aBuffer Pointer to the target descriptor.
        * @return void
        */
        TInt CopyBuffer( TUint32 aCount, TUint8 aWordSize, HBufC8*& aBuffer );

        /**
        * From CExifEndianBase Copies the specified amount of data located in 
        * the given offset to the given descriptor.
        * @since 2.6 
        * @param aOffset Offset for starting copying
        * @param aCount Number of words to copy
        * @param aWordSize Word size
        * @param aBuffer Pointer to the target descriptor.
        * @return void
        */
        TInt CopyBuffer( 
            TUint32 aOffset, 
            TUint32 aCount, 
            TUint8 aWordSize, 
            HBufC8*& aBuffer );

        /**
        * From CExifEndianBase Locates the offset of the next specified 16-bit 
        * unsigned integer 
        * @since 2.6 
        * @param aSearch Queried 16-bit integer
        * @param aOffset The offset of the found location.
        * @return Error code.
        */        
        TInt Locate16( TUint16 aSearch, TUint32& aOffset );

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( 
            TUint8* aDataStartPtr,
            TUint8* aExifStartPtr, 
            TUint8* aExifEndPtr, 
            TBool aIsExif );

    };


/**
*  Exif data parser class for Big endian format.
*
*  @lib ExifLib
*  @since 2.6 
*/
NONSHARABLE_CLASS( CExifBigEndian ): public CExifEndianBase
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CExifBigEndian* NewL( 
            const TUint8* aDataStartPtr, 
            TUint8* aExifStartPtr, 
            TUint8* aExifEndPtr, 
            TBool aIsExif );

        /**
        * C++ default constructor.
        */
        CExifBigEndian();

        /**
        * Destructor.
        */        
        virtual ~CExifBigEndian();

    public: // Functions from base classes

        /**
        * From CExifEndianBase Gets the next 16-bit unsigned integer from the 
        * data.
        * @since 2.6 
        * @param aUint16 The 16-bit integer returned.
        * @return Error code.
        */        
        TInt ReadUInt16( TUint16& aUint16 );
        
        /**
        * From CExifEndianBase Gets the next 32-bit unsigned integer from the 
        * data.
        * @since 2.6 
        * @param aUint32 The 32-bit integer returned.
        * @return Error code.
        */        
        TInt ReadUInt32( TUint32& aUint32 );
        
        /**
        * From CExifEndianBase Copies the next specified amount of data to the 
        * given descriptor.
        * @since 2.6 
        * @param aCount Number of words to copy
        * @param aWordSize Word size
        * @param aBuffer Pointer to the target descriptor.
        * @return void
        */
        TInt CopyBuffer( TUint32 aCount, TUint8 aWordSize, HBufC8*& aBuffer );

        /**
        * From CExifEndianBase Copies the specified amount of data located in 
        * the given offset to the given descriptor.
        * @since 2.6 
        * @param aOffset Offset for starting copying
        * @param aCount Number of words to copy
        * @param aWordSize Word size
        * @param aBuffer Pointer to the target descriptor.
        * @return void
        */
        TInt CopyBuffer( 
            TUint32 aOffset, 
            TUint32 aCount, 
            TUint8 aWordSize, 
            HBufC8*& aBuffer );

        /**
        * From CExifEndianBase Locates the offset of the next specified 16-bit 
        * unsigned integer 
        * @since 2.6 
        * @param aSearch Queried 16-bit integer
        * @param aOffset The offset of the found location.
        * @return Error code.
        */        
        TInt Locate16( TUint16 aSearch, TUint32& aOffset );
    
    private:
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( 
            TUint8* aDataStartPtr, 
            TUint8* aExifStartPtr, 
            TUint8* aExifEndPtr, 
            TBool aIsExif );
    };

#endif // EXIFENDIAN_H

// End of File
