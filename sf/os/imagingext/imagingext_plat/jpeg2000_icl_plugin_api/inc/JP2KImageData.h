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
* Description:  Collection of classes derived from TImageDataBlock used
*                to store and represent the Image data.
*
*/


#ifndef __JP2KIMAGEDATA_H__
#define __JP2KIMAGEDATA_H__

//  INCLUDES
#include <icl/imagedata.h>

// CONSTANTS

// Identification UIDs for TImageDataBlock-derived classes 
#define KJ2KCommentUid      0x101F865F // UID for COM marker
#define KJ2KIprBoxUid       0x101F8660 // UID for IPR box
#define KJ2KXmlBoxUid       0x101F8661 // UID for XML box
#define KJ2KUuidBoxUid      0x101F8662 // UID for UUID box
#define KJ2KUuidInfoBoxUid  0x101F8663 // UID for UUIDINFO box

#define KJ2KUuidIDSize      16  // ID size in UUID box

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFrameImageData;

// CLASS DECLARATION

/**
 * JP2K specific image data variant which holds JP2K COM marker.
 *
 * @JP2KCodec.dll
 * @since 2.6
 */
class TJp2kComment : public TImageDataBlock
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TJp2kComment();         

    public: // New functions
        
    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:  // New functions
        
    private:  // Functions from base classes

        /**
        * From TImageDataBlock
        * @since 2.6
        * @param aFrameImageData: Image data which hold current duplication.
        */
        TImageDataBlock* DuplicateL( CFrameImageData& aFrameImageData ) const;

    public:     // Data

        // The comment to set/query
        HBufC8 *iComment;

    protected:  // Data

    private:    // Data

    public:     // Friend classes
  
    protected:  // Friend classes
      
    private:    // Friend classes
    };

/**
 * JP2K specific image data variant which holds JP2K IPR box.
 *
 * @JP2KCodec.dll
 * @since 2.6
 */
class TJp2kIprBox : public TImageDataBlock
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TJp2kIprBox();         

    public: // New functions
        
    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:  // New functions
        
    private:  // Functions from base classes

        /**
        * From TImageDataBlock
        * @since 2.6
        * @param aFrameImageData: Image data which hold current duplication.
        */
        TImageDataBlock* DuplicateL( CFrameImageData& aFrameImageData ) const;

    public:     // Data

        // The ipr data to set/query
        HBufC8 *iIprData;

    protected:  // Data

    private:    // Data

    public:     // Friend classes
  
    protected:  // Friend classes
      
    private:    // Friend classes
    };

/**
 * JP2K specific image data variant which holds JP2K XML box.
 *
 * @JP2KCodec.dll
 * @since 2.6
 */
class TJp2kXmlBox : public TImageDataBlock
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TJp2kXmlBox();         

    public: // New functions
        
    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:  // New functions
        
    private:  // Functions from base classes

        /**
        * From TImageDataBlock
        * @since 2.6
        * @param aFrameImageData: Image data which hold current duplication.
        */
        TImageDataBlock* DuplicateL( CFrameImageData& aFrameImageData ) const;

    public:     // Data

        // The xml data to set/query
        HBufC8 *iXmlData;

    protected:  // Data

    private:    // Data

    public:     // Friend classes
  
    protected:  // Friend classes
      
    private:    // Friend classes
    };

/**
 * JP2K specific image data variant which holds JP2K UUID box.
 *
 * @JP2KCodec.dll
 * @since 2.6
 */
class TJp2kUuidBox : public TImageDataBlock
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TJp2kUuidBox();         

    public: // New functions
        
    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:  // New functions
        
    private:  // Functions from base classes

        /**
        * From TImageDataBlock
        * @since 2.6
        * @param aFrameImageData: Image data which hold current duplication.
        */
        TImageDataBlock* DuplicateL( CFrameImageData& aFrameImageData ) const;

    public:     // Data

        // The id field to set/query
        TBuf8<KJ2KUuidIDSize> iUuidId;

        // The uuid data to set/query
        HBufC8 *iUuidData;

    protected:  // Data

    private:    // Data

    public:     // Friend classes
  
    protected:  // Friend classes
      
    private:    // Friend classes
    };

/**
 * JP2K specific image data variant which holds JP2K UUID Info box.
 *
 * @JP2KCodec.dll
 * @since 2.6
 */
class TJp2kUuidInfoBox : public TImageDataBlock
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TJp2kUuidInfoBox();         

    public: // New functions
        
    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:  // New functions
        
    private:  // Functions from base classes

        /**
        * From TImageDataBlock
        * @since 2.6
        * @param aFrameImageData: Image data which hold current duplication.
        */
        TImageDataBlock* DuplicateL( CFrameImageData& aFrameImageData ) const;

    public:     // Data

        // The uuid info nu field to set/query
        TInt iUuidInfoNu;

        // The uuid info id data field to set/query
        // concatenation of ( nu * 16 bytes ID field )
        // ID(0) | ID(1) | .. | ID(n-1) 
        // where n = nu field and ID(x) = 16 bytes data
        HBufC8 *iUuidInfoId;

        // The uuid info version field to set/query
        TInt iUuidInfoVersion;

        // The uuid info flag field to set/query
        TInt iUuidInfoFlag;

        // The uuid info url data field to set/query
        HBufC8 *iUuidInfoData;

    protected:  // Data

    private:    // Data

    public:     // Friend classes
  
    protected:  // Friend classes
      
    private:    // Friend classes
    };

#endif // __JP2KIMAGEDATA_H__

