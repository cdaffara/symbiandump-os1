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
* Description:  Core Exif file format handler class providing services to the 
*                API classes
*
*/


#ifndef EXIFCORE_H
#define EXIFCORE_H

//  INCLUDES
#include <e32base.h>
#include "ExifRead.h"
#include "ExifTagImpl.h"
#include "ExifCommon.h"

// FORWARD DECLARATIONS
class CExifIfd;
class CExifTagImpl;

// CLASS DECLARATION

/**
*  Exif core handler class.
*  Handles writing and reading Exif v2.2 format, and stores Exif data structures.
*
*  @lib ExifLib
*  @since 2.6 
*/
NONSHARABLE_CLASS( CExifCore ): public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CExifCore* NewL();

        /**
        * Destructor.
        */       
        virtual ~CExifCore();

    public: // New functions

        /**
        * Inserts/Updates the given tag into the specified IFD structure.
        * @since 2.6 
        * @param aIfdType The IFD type
        * @param aExifTag The tag instance that is updated
        * @param aCheckValidity When ETrue then tag validity is checked
        * @return void
        */
        void InsertTagL( TExifIfdType aIfdType, CExifTagImpl* aExifTag, TBool aCheckValidity );

        /**
        * Sets the given 8-bit integer to the data of a tag specified by the 
        * given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID
        * @param aTagData The tag data that is updated
        * @return void
        */
        void SetTagDataL( TUint16 aTagId, TInt8 aTagData );

        /**
        * Sets the given 16-bit unsigned integer to the data of a tag specified 
        * by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID
        * @param aTagData The tag data that is updated
        * @return void
        */
        void SetTagDataL( TUint16 aTagId, TUint16 aTagData );

        /**
        * Sets the given 32-bit integer to the data of a tag specified by the 
        * given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID
        * @param aTagData The tag data that is updated
        * @return void
        */
        void SetTagDataL( TUint16 aTagId, TUint32 aTagData );

        /**
        * Sets the given data buffer to the data of a tag specified by the 
        * given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID
        * @param aTagData The tag data that is updated
        * @return void
        */
        void SetTagDataL( TUint16 aTagId, const TDesC8& aTagData );

        /**
        * Sets the given 64-bit (2 x 32-bit ) unsigned integers to the data of 
        * a tag specified by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID
        * @param aNumerator The first 32-bit unsigned integer (the numerator) 
        *                   that is updated 
        * @param aDenominator The second 32-bit unsigned integer (the 
        *                   Denominator) that is updated 
        * @return void
        */
        void SetTagDataL( 
            TUint16 aTagId, 
            TUint32 aNumerator, 
            TUint32 aDenominator );
            

        /**
        * Sets the given 64-bit (2 x 32-bit ) integers to the data of a tag 
        * specified by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID
        * @param aNumerator The first 32-bit integer (the numerator) that is 
        *                   updated 
        * @param aDenominator The second 32-bit integer (the Denominator) that 
        *                     is updated 
        * @return void
        */
        void SetTagDataL( 
            TUint16 aTagId, 
            TInt32 aNumerator, 
            TInt32 aDenominator );
            

        /**
        * Sets the given 16-bit unsigned integer to the data of a thumbnail 
        * (IFD1) tag specified by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID
        * @param aTagData The tag data that is updated
        * @return void
        */
        void SetThumbnailTagDataL( TUint16 aTagId, TUint16 aTagData );

        /**
        * Sets the given 32-bit unsigned integer to the data of a thumbnail 
        * (IFD1) tag specified by the given tag ID,
        * @since 2.6 
        * @param aTagId The tag ID
        * @param aTagData The tag data that is updated
        * @return void
        */
        void SetThumbnailTagDataL( TUint16 aTagId, TUint32 aTagData );

        /**
        * Sets the given 64-bit (2 x 32-bit ) unsigned integers to the data 
        * of a thumbnail (IFD1) tag specified by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID.
        * @param aNumerator The first 32-bit unsigned integer (the numerator) 
        *                   that is updated.
        * @param aDenominator The second 32-bit unsigned integer (the 
        *                     Denominator) that is updated.
        * @return void
        */
        void SetThumbnailTagDataL( 
            TUint16 aTagId, 
            TUint32 aNumerator, 
            TUint32 aDenominator );

        /**
        * Gets the 8-bit integer data of a tag specified by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID.
        * @param aTagData The tag data that is returned.
        * @return Error code. KErrNotFound if tag doesn't exist.
        */
        TInt GetTagData( TUint16 aTagId, TInt8& aTagData ) const;

        /**
        * Gets the 16-bit unsigned integer data of a tag specified by the 
        * given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID.
        * @param aTagData The tag data that is returned.
        * @return Error code. KErrNotFound if tag doesn't exist.
        */
        TInt GetTagData( TUint16 aTagId, TUint16& aTagData ) const;

        /**
        * Gets the 32-bit unsigned integer data of a tag specified by the given 
        * tag ID.
        * @since 2.6 
        * @param aTagId The tag ID.
        * @param aTagData The tag data that is returned.
        * @return Error code. KErrNotFound if tag doesn't exist.
        */
        TInt GetTagData( TUint16 aTagId, TUint32& aTagData ) const;

        /**
        * Gets the data buffer of a tag specified by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID.
        * @return The tag data buffer.
        */
        HBufC8* GetTagDataL( TUint16 aTagId ) const;

        /**
        * Gets the 64-bit (2 x 32-bit) unsigned integer data of a tag specified 
        * by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID.
        * @param aNumerator The first 32-bit unsigned integer (the numerator) 
        * that is returned.
        * @param aDenominator The second 32-bit unsigned integer (the 
        * Denominator) that is returned.
        * @return Error code. KErrNotFound if tag doesn't exist.
        */
        TInt GetTagData( 
            TUint16 aTagId, 
            TUint32& aNumerator, 
            TUint32& aDenominator ) const;
        
        /**
        * Gets the 64-bit (2 x 32-bit) integer data of a tag specified by the 
        * given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID.
        * @param aNumerator The first 32-bit integer (the numerator) that is 
        *                   returned.
        * @param aDenominator The second 32-bit integer (the Denominator) that 
        *                   is returned.
        * @return Error code. KErrNotFound if tag doesn't exist.
        */
        TInt GetTagData( 
            TUint16 aTagId, 
            TInt32& aNumerator, 
            TInt32& aDenominator ) const;

        /**
        * Gets the 16-bit unsigned integer data of a thumbnail (IFD1) tag 
        * specified by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID.
        * @param aTagData The tag data that is returned.
        * @return Error code. KErrNotFound if tag doesn't exist.
        */
        TInt GetThumbnailTagData( TUint16 aTagId, TUint16& aTagData ) const;

        /**
        * Gets the 32-bit unsigned integer data of a thumbnail (IFD1) tag 
        * specified by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID.
        * @param aTagData The tag data that is returned.
        * @return Error code. KErrNotFound if tag doesn't exist.
        */
        TInt GetThumbnailTagData( TUint16 aTagId, TUint32& aTagData ) const;

        /**
        * Gets the 64-bit (2 x 32-bit) unsigned integer data of a thumbnail 
        * (IFD1) tag specified by the given tag ID.
        * @since 2.6 
        * @param aTagId The tag ID.
        * @param aNumerator The first 32-bit integer (the numerator) that is 
        *                   returned.
        * @param aDenominator The second 32-bit integer (the Denominator) that 
        *                   is returned.
        * @return Error code. KErrNotFound if tag doesn't exist.
        */
        TInt GetThumbnailTagData( 
            TUint16 aTagId, 
            TUint32& aNumerator, 
            TUint32& aDenominator ) const;
        
        /**
        * Gets the tag instance having the given tag ID from the specified 
        * IFD structure.
        * @since 2.6 
        * @param aIfdType The hosting IFD type
        * @param aTagId The tag ID
        * @return Constant tag instance pointer
        */
        const CExifTagImpl* GetTagL( 
            TExifIfdType aIfdType, 
            TUint16 aTagId ) const;

        /**
        * Removes the tag instance having the given tag ID from the specified 
        * IFD structure.
        * @since 2.6 
        * @param aIfdType The hosting IFD type
        * @param aTagId The tag ID
        * @return Error code. KErrNotFound if tag or IFD doesn't exist. 
        */
        TInt DeleteTag( TExifIfdType aIfdType, TUint16 aTagId );

        /**
        * Gets the tag IDs and the number of tags in the specified IFD 
        * structure.
        * @since 2.6 
        * @param aIfdType The hosting IFD type
        * @param aNoTags The number of tag IDs returned
        * @return Pointer to tag IDs
        */
        TUint16* GetTagIdsL( TExifIfdType aIfdType, TInt& aNoTags ) const;

        /**
        * Instantiates the specified IFD structure.
        * @since 2.6 
        * @param aIfdType The target IFD type
        * @return void
        */
        void CreateIfdL( TExifIfdType aIfdType );

        /**
        * Removes the specified IFD structure and all its tags.
        * @since 2.6 
        * @param aIfdType The target IFD type
        * @return Error code. KErrNotFound if IFD doesn't exist.
        */
        TInt DeleteIfd( TExifIfdType aIfdType );

        /**
        * Gets the IFD types and number of IFDs in the file format.
        * @since 2.6 
        * @param aNoIfd The number of IFD types returned
        * @return Pointer to IFD types
        */
        TExifIfdType* GetIfdTypesL( TInt& aNoIfd ) const;

        /**
        * Gets the Exif thumbnail image data from the IFD1 structure.
        * @since 2.6 
        * @return Pointer to heap descriptor containing a copy of thumbnail 
        * image.
        */
        HBufC8* GetThumbnailL() const;

        /**
        * Inserts/Updates the given Exif thumbnail image data in the IFD1 
        * structure.
        * @since 2.6 
        * @param aThumbnailData Pointer to descriptor containing new thumbnail 
        *                       image.
        * @return void
        */
        void InsertThumbnailL( TDesC8* aThumbnailData );

        /**
        * Removes the Exif thumbnail image data from the IFD1 structure.
        * @since 2.6 
        * @return Error code. KErrNotFounc if thumbnail image doesn't exist.
        */
        TInt RemoveThumbnail();

        /**
        * Writes the Exif data to the given descriptor starting from the 
        * specified position/offset.
        * @since 2.6 
        * @param aExifData Pointer reference to the target descriptor 
        * @param aPos Offset for the start of the target data.
        * @return void
        */
        void WriteExifDataL( HBufC8*& aExifData, TUint& aPos );
        
        /**
        * Writes the Jpeg header to the given descriptor starting from the 
        * specified position/offset.
        * @since 2.6 
        * @param aExifData Pointer reference to the target descriptor 
        * @param aPos Offset for the start of the target data.
        * @return void
        */
        void WriteJpegHeaderL( HBufC8*& aExifData, TUint& aPos );

        /**
        * Finalizes the Exif data to ensure the validity, updates the internal 
        * offsets.
        * @since 2.6 
        * @return Error code.
        */
        TInt Finalize();

        /**
        * Returns the total size of the Exif file format in bytes.
        * @since 2.6 
        * @return Total size
        */
        TUint TotalSize() const;

        /**
        * Returns the size of the Jpeg image in the Exif file format, excluding 
        * SOI and APP markers in bytes.
        * @since 2.6 
        * @return Jpeg data size
        */
        TUint JpegSize() const;

        /**
        * Returns the size of the APP1 marker, which includes the Exif-specific 
        * data in bytes
        * @since 2.6 
        * @return APP1 marker size
        */
        TUint16 App1Size() const;

        /**
        * Checks if the Exif data is in valid Exif v2.2 format and contains all 
        * mandatory information.
        * @since 2.6 
        * @return Boolean stating if the Exif data is in valid format or not.
        */
        TBool IsValid() const;

        /**
        * Checks if the specified tag information conforms to the Full Validity 
        * characteristics (Data count is correct, data type matches tag ID and 
        * data value falls into the predefined range).
        * @since 2.6 
        * @param aTagInfo The Tag information structure that will be verified.
        * @param aIfdType The Exif Ifd for which the validity of the tag is 
        * verified.
        * @return Boolean stating if the given Tag information is valid.
        */
        TBool TagIsValid( TExifTagInfo aTagInfo, TExifIfdType aIfdType ) const;

        /**
        * Checks if the specified IFD structure exists in the Exif data.
        * @since 2.6 
        * @param aIfdType The queried IFD type
        * @return Boolean stating if the specified IFD type exists or not.
        */
        TBool IfdExists( TExifIfdType aIfdType ) const;

        /**
        * Checks if the tag having the given tag ID exists in the specified 
        * IFD structure.
        * @since 2.6 
        * @param aTagId The queried tag ID
        * @param aIfdType The hosting IFD type
        * @return Boolean stating if the specified tag exists or not.
        */
        TBool TagExists( TUint16 aTagId, TExifIfdType aIfdType ) const;

        /**
        * Sets the given Jpeg data start and end offsets.
        * @since 2.6 
        * @param aJpgStartOffset Jpeg data start offset
        * @param aJpgEndOffset Jpeg data end offset
        * @return void
        */
        void SetJpgOffsets( TUint32 aJpgStartOffset, TUint32 aJpgEndOffset );

        /**
        * Gets the pure Jpeg image data excluding the SOI and APP1 markers.
        * @since 2.6 
        * @param aJpegPtr Pointer to a copy of Jpeg data.
        * @return TInt Error code. 
        */
        TInt GetJpegData( TPtr8 aJpegPtr ) const;

        /**
        * Sets the Exif data start pointer.
        * @since 2.6 
        * @param aPtr Pointer to the beginning of Exif data.
        * @return void
        */
        void SetDataStartPtr(const TUint8* aPtr );

    private:

        /**
        * C++ default constructor.
        */
        CExifCore();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Writes the Exif header to the location, which is defined by the given 
        * pointer and the offset.
        * @since 2.6 
        * @param aExifDataPtr Pointer to the target Exif data.
        * @param aPos Offset of the Exif header.
        * @return void
        */
        void WriteExifHeaderL( TUint16*& aExifDataPtr, TUint& aPos );

        /**
        * Writes the specified IFD data to the location, which is defined by the
        * given pointer and the offset.
        * @since 2.6 
        * @param aExifDataPtr Pointer to the target Exif data.
        * @TExifIfdType aIfdType Written IFD structure.
        * @param aPos Offset of the specified IFD structure.
        * @return void
        */
        void WriteIfdL( 
            TUint16*& aExifDataPtr, 
            TExifIfdType aIfdType, 
            TUint& aPos );

        /**
        * Returns the offset of the specified IFD structure in the Exif data.
        * @since 2.6 
        * @param aIfdType The queried IFD type.
        * @return Found IFD offset.
        */
        TUint32 FindIfdOffset( TExifIfdType aIfdType ) const;

    private:    // Data

        // IFD structure array, containing 0th, EXIF, 1st, GPS and 
        // Interoperability IFDs.
        TFixedArray< CExifIfd*, KIfdNo > iIfdArray;

        // Start Offset of the Jpeg image in the Exif data 
        TUint32 iJpgStartOffset;

        // End Offset of the Jpeg image in the Exif data 
        TUint32 iJpgEndOffset;

        // Pointer to the beginning of the Exif data.
        const TUint8* iDataStartPtr;
    };

#endif // EXIFCORE_H

// End of File

