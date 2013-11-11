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
* Description:  Exif tag wrapper implementation
*
*/



#ifndef EXIFTAGIMPL_H
#define EXIFTAGIMPL_H

//  INCLUDES
#include "ExifTag.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* CExifTagImpl
* Implementation class for manipulating Exif Tags. An instance of this class 
* can be instantiated 
* providing the internal data to insert into the Exif data. 
* 
* @lib ExifLib
* @since 2.6 
*/
NONSHARABLE_CLASS( CExifTagImpl ): public CExifTag
    {
	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
    	static CExifTagImpl* NewL( 
            TUint16 aTagId, 
            TExifTagDataType aDataType, 
            TUint32 aDataCount, 
            HBufC8* aDataBuffer,
            TBool aCheckValidity );

        /**
        * Two-phased constructor.
        */
    	static CExifTagImpl* NewL( TExifTagInfo aTagInfo, HBufC8* aDataBuffer, TBool aCheckValidity );

        /**
        * Destructor.
        */
        virtual ~CExifTagImpl();

	public:   // Functions from base classes

        /**
        * Duplicate constructor. Creates an exact copy instance of the tag.
        * @since 2.6
        * @return Pointer to the new tag instance.
        */
    	CExifTag* DuplicateL() const;

        /**
        * Returns the informative fields of a tag.
        * @since 2.6
        * @return Informative fields of the tag in associated structure.
        */
    	TExifTagInfo TagInfo() const;

        /**
        * Returns the ID of the tag.
        * @since 2.6
        * @return The tag ID
        */
    	TUint16 Id() const;

        /**
        * Returns the data type of the tag.
        * @since 2.6
        * @return The tag data type
        */
    	TExifTagDataType DataType() const;

        /**
        * Returns the data count of the tag. Data count is not in bytes, 
        * it specifies the number of elements in the data type.
        * @since 2.6
        * @return The number of tag data elements
        */
    	TUint32 DataCount() const;

        /**
        * Returns the data stored in the tag.
        * @since 2.6
        * @return Unmodifiable pointer to the tag data.
        */
    	TPtrC8 Data() const;

        /**
        * Returns the total size of the tag in bytes.
        * @since 2.6
        * @return The total size of the tag in bytes
        */
        TUint Size() const;

	private:

        /**
        * Constructor.
        */        
    	CExifTagImpl( TExifTagInfo aTagInfo );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
    	void ConstructL( HBufC8* aDataBuffer, TBool aCheckValidity );

	private:

        // Informative fields of a tag
    	TExifTagInfo iTagInfo;

        // Data of the tag.
    	HBufC8* iData;
    
    };

#endif      // EXIFTAGIMPL_H
// End of File
