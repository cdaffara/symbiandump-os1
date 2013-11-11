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
* Description:  Exif IFD wrapper class for handling the storage of the tags 
*                and manipulating IFDs.
*
*/


#ifndef EXIFIFD_H
#define EXIFIFD_H

//  INCLUDES
#include "ExifRead.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CExifTagImpl;

/**
*  Exif IFD wrapper base class
*  Provides the base for the other IFD structures.
*
*  @lib ExifLib
*  @since 2.6 
*/
NONSHARABLE_CLASS( CExifIfd ): public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CExifIfd* NewBaseL( TExifIfdType aIfdType );

        /**
        * Destructor.
        */
        virtual ~CExifIfd();

    public: // New functions

        /**
        * Checks if the IFD is in valid format, and contains all mandatory tags.
        * @since 2.6 
        * @return Boolean stating if the IFD is valid.
        */        
        virtual TBool IsValid() const;

        /**
        * Checks if specified tag exists in the IFD.
        * @since 2.6 
        * @param aTagId The queried tag ID
        * @return Boolean stating if the tag exists.
        */        
        TBool TagExists( TUint16 aTagId ) const;

        /**
        * Returns the size of the IFD structure in bytes.
        * @since 2.6 
        * @return Size of the IFD in bytes.
        */   
        TUint16 Size() const;

        /**
        * Sets the next IFD offset.
        * @since 2.6 
        * @param aIfdOffset Next IFD offset
        * @return void
        */        
        void SetNextIfdOffset( TUint32 aIfdOffset );

        /**
        * Writes the tag data in the IFD structure to the location defined by 
        * the given pointer and the offset.
        * @since 2.6 
        * @param aExifDataPtr Pointer to the Exif data
        * @param aPos Offset for the beginning of the IFD data.
        * @return void
        */        
        void WriteTagsL( TUint16*& aExifDataPtr, TUint& aPos ) const;

        /**
        * Inserts/Replaces the given tag in the IFD structure.
        * @since 2.6 
        * @param aExifTag Pointer to the tag instance updated.
        * @param aCheckValidity When ETrue then tag validity is checked
        * @return void
        */        
        virtual void InsertTagL( CExifTagImpl* aExifTag, TBool aCheckValidity );

        /**
        * Removes the specified tag from the IFD structure
        * @since 2.6 
        * @param aTagId The target tag ID.
        * @return Error code.
        */        
        TInt DeleteTag( TUint16 aTagId );

        /**
        * Gets the unmodifiable specified tag instance from the IFD structure.
        * @since 2.6 
        * @param aTagId The queried tag ID.
        * @return Constant pointer to the tag instance.
        */        
        const CExifTagImpl* GetTagL( TUint16 aTagId ) const;

        /**
        * Gets the IDs and amount of the tags existing in the IFD structure.
        * @since 2.6 
        * @param aNoTags Number of tag IDs returned.
        * @return Pointer to the tag IDs.
        */        
        TUint16* GetTagIdsL( TInt& aNoTags ) const;

        /**
        * Checks if the given tag ID is one of the IDs that can be stored in 
        * the IFD structure.
        * @since 2.6 
        * @param aTagId The queried tag ID.
        * @return Boolean stating if the specified tag ID is valid for the IFD.
        */        
        TBool IsAcceptableTagId( TUint16 aTagId ) const;
        
    protected:  // New functions

        /**
        * C++ default constructor.
        */
        CExifIfd();

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:

        // The tag instances stored in the IFD.
        CArrayPtrSeg< CExifTagImpl > iTagArray;

        // Type of the IFD.
        TExifIfdType iIfdType;

        // Next IFD's offset.
        TUint32 iNextIfdOffset;

        // Size of the IFD data in bytes.
        TUint16 iSize;

    };


/**
*  Exif IFD0 wrapper class
*  
*
*  @lib ExifLib
*  @since 2.6 
*/
NONSHARABLE_CLASS( CExifIfd0 ) : public CExifIfd
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CExifIfd0* NewL();

        /**
        * C++ default constructor.
        */        
        CExifIfd0();

        /**
        * Destructor.
        */      
        virtual ~CExifIfd0();

    public: // Functions from base classes

        /**
        * From CExifIfd Checks if the IFD is in valid format, and contains all 
        * mandatory tags.
        * @since 2.6 
        * @return Boolean stating if the IFD is valid.
        */            
        TBool IsValid() const;

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    };


/**
*  Exif Exif-IFD wrapper class
*  
*
*  @lib ExifLib
*  @since 2.6 
*/
NONSHARABLE_CLASS( CExifIfdExif ): public CExifIfd
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CExifIfdExif* NewL();

        /**
        * C++ default constructor.
        */   
        CExifIfdExif();

        /**
        * Destructor.
        */   
        virtual ~CExifIfdExif();
        
    public: // Functions from base classes

        /**
        * From CExifIfd Checks if the IFD is in valid format, and contains all 
        * mandatory tags.
        * @since 2.6 
        * @return Boolean stating if the IFD is valid.
        */      
        TBool IsValid() const;        
        
    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    };


/**
*  Exif IFD1 wrapper class
*  
*
*  @lib ExifLib
*  @since 2.6 
*/
NONSHARABLE_CLASS( CExifIfd1 ): public CExifIfd
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CExifIfd1* NewL();

        /**
        * C++ default constructor.
        */   
        CExifIfd1();

        /**
        * Destructor.
        */   
        virtual ~CExifIfd1();

    public: // New functions

        /**
        * Inserts/Updates the given Exif thumbnail image.
        * @since 2.6 
        * @param aThumbnailData The Exif thumbnail image data that is updated
        * @return Error code.
        */    
        TInt SetThumbnailData( TDesC8* aThumbnailData );

        /**
        * Gets the Exif thumbnail image.
        * @since 2.6 
        * @param aThumbnailData The Exif thumbnail image data returned.
        * @return Error code.
        */            
        TInt GetThumbnailData( TDesC8*& aThumbnailData ) const;

        /**
        * Removes the Exif thumbnail image.
        * @since 2.6 
        * @return Error code.
        */       
        TInt RemoveThumbnailData();

        /**
        * Returns the size of the Exif thumbnail image in bytes.
        * @since 2.6 
        * @return Thumbnail image size
        */  
        TUint16 ThumbnailSize() const;

        /**
        * Writes the Exif thumbnail image to the location defined by the given 
        * pointer and the offset.
        * @since 2.6 
        * @param aExifDataPtr Pointer to the Exif data.
        * @param aPos Offset for the beginning of the thumbnail image data.
        * @return Thumbnail image size
        */  
        void WriteThumbnailL( TUint8*& aExifDataPtr, TUint& aPos ) const;

    public: // Functions from base classes

        /**
        * From CExifIfd Checks if the IFD is in valid format, and contains all 
        * mandatory tags.
        * @since 2.6 
        * @return Boolean stating if the IFD is valid.
        */      
        TBool IsValid() const;

        /**
        * From CExifIfd Inserts/Replaces the given tag in the IFD structure.
        * @since 2.6 
        * @param aExifTag Pointer to the tag instance updated.
        * @param aCheckValidity When ETrue then tag validity is checked
        * @return void
        */    
        void InsertTagL( CExifTagImpl* aExifTag, TBool aCheckValidity );

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        // Pointer to the descriptor containing thumbnail image data.
        TDesC8* iThumbnailBuffer;
    };


/**
*  Exif Interoperability IFD wrapper class
*  
*
*  @lib ExifLib
*  @since 2.6 
*/
NONSHARABLE_CLASS( CExifIfdIntOp ): public CExifIfd
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CExifIfdIntOp* NewL();

        /**
        * C++ default constructor.
        */   
        CExifIfdIntOp();

        /**
        * Destructor.
        */   
        virtual ~CExifIfdIntOp();

    public: // Functions from base classes

        /**
        * From CExifIfd Checks if the IFD is in valid format, and contains all 
        * mandatory tags.
        * @since 2.6 
        * @return Boolean stating if the IFD is valid.
        */      
        TBool IsValid() const;

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    };


/**
*  Exif Gps IFD wrapper class
*  
*
*  @lib ExifLib
*  @since 2.6 
*/

NONSHARABLE_CLASS( CExifIfdGps ): public CExifIfd
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CExifIfdGps* NewL();

        /**
        * C++ default constructor.
        */   
        CExifIfdGps();

        /**
        * Destructor.
        */   
        virtual ~CExifIfdGps();

    public: // Functions from base classes

        /**
        * From CExifIfd Checks if the IFD is in valid format, and contains all 
        * mandatory tags.
        * @since 2.6 
        * @return Boolean stating if the IFD is valid.
        */      
        TBool IsValid() const;

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    };

#endif // EXIFIFD_H

// End of File
