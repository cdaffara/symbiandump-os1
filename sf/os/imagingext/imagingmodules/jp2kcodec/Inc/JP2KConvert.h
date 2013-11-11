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
* Description:  JPEG2000 image encoder/decoder plugin class
*
*/


#ifndef __JP2KCONVERT_H__
#define __JP2KCONVERT_H__

//  INCLUDES
#include <icl/imageplugin.h>
#include <icl/imagedata.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TJ2kInfo;
class CJp2kReadCodec;

// CLASS DECLARATION

/**
 * CJp2kDecoder JPEG2000 image decoder plugin class.
 * Implement the ICL decoder plugin interface.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class CJp2kDecoder : public CImageDecoderPlugin
{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CJp2kDecoder* NewL();

        /**
        * Destructor.
        */
        virtual ~CJp2kDecoder();

    public: // New functions
        
    public: // Functions from base classes

        /**
        * From CImageDecoderPlugin
        * @since 2.6
        * @param aFrameNumber: Number of frame whose type is needed.
        * @param aImageType: Return image type UID.
        * @param aImageSubType: Returned image sub type UID.
        */
        void ImageType( TInt aFrameNumber, TUid& aImageType, TUid& aImageSubType ) const;

        /**
        * From CImageDecoderPlugin
        * @since 2.6
        */
        TInt NumberOfImageComments() const;

        /**
        * From CImageDecoderPlugin
        * @since 2.6
        * @param aCommentNumber: Number of comment whose comment is needed.
        */
        HBufC* ImageCommentL( TInt aCommentNumber ) const;

        /**
        * From CImageDecoderPlugin
        * @since 2.6
        * @param aFrameNumber: Number of frame whose comment is needed.
        */
        TInt NumberOfFrameComments( TInt aFrameNumber ) const;

        /**
        * From CImageDecoderPlugin
        * @since 2.6
        * @param aFrameNumber: Number of frame whose comment is needed.
        * @param aCommentNumber: Number of comment whose comment is needed.
        */
        HBufC* FrameCommentL( TInt aFrameNumber, TInt aCommentNumber ) const;

    protected:  // New functions
        
    protected:  // Functions from base classes
        
        /**
        * From CImageDecoderPlugin
        */
        CFrameInfoStrings* FrameInfoStringsL( RFs& aFs, TInt aFrameNumber );

        /**
        * From CImageDecoderPlugin
        */
        TInt FrameHeaderBlockSize( TInt aFrameNumber ) const;

        /**
        * From CImageDecoderPlugin
        */
        TInt FrameBlockSize( TInt aFrameNumber ) const;

        /**
        * From CImageDecoderPlugin
        */
        void DoConvert();

    private:

        /**
        * C++ default constructor.
        */
        CJp2kDecoder();

        /**
        * Main state machine
        * Verify and start parsing the JP2 file format
        * or JP2 codestream.
        * @since 2.6
        */
        void ReadFormatL();

        /**
        * Verify and process JP2 Signature box 
        * @since 2.6
        */
        void ReadSignatureBoxL();
        /**
        * Verify and process JP2 File Type box
        * @since 2.6
        */
        void ReadFileTypeBoxL();

        /**
        * Verify and process JP2 Header box (superbox)
        * @since 2.6
        */
        void ReadJp2HeaderBoxL();

        /**
        * Verify and process Image Header box (in JP2 Header box)
        * @since 2.6
        */
        void ReadImageHeaderBoxL();

        /**
        * Verify and process Colour Specification box (in JP2 Header box)
        * @since 2.6
        * @param aBoxLength: the length of the Colour Specification box
        */
        void ReadColorSpecBoxL( const TUint32 aBoxLength );

        /**
        * Verify and process Bits Per Component box (in JP2 Header box)
        * @since 2.6
        * @param aBoxLength: the length of the Bits Per Component box
        */
        void ReadBitsPerCompBoxL( TUint32 aBoxLength );

        /**
        * Verify and process Resolution box (in JP2 Header box, which itself is superbox)
        * @since 2.6
        * @param aPtrLimit: the boundary of the data to be read
        * @param aBoxLength: the length of the Resolution box
        */
        void ReadResolutionBoxL( const TUint8 *aPtrLimit, TUint32 aBoxLength );

        /**
        * Verify and process Default Display Resolution box (in Resolution box)
        * @since 2.6
        */
        void ReadDisplayResBoxL();

        /**
        * Verify and process Capture Resolution box (in Resolution box)
        * @since 2.6
        * @param aBoxLength: the length of the Capture Resolution box
        */
        void ReadCaptureResBoxL( TUint32 aBoxLength );

        /**
        * Verify and process Palette box (in JP2 Header box)
        * @since 2.6
        */
        void ReadPaletteBoxL();

        /**
        * Verify and process Component Mapping box (in JP2 Header box)
        * @since 2.6
        * @param aBoxLength: the length of the Component Mapping box
        */
        void ReadComponentMapBoxL( TUint32 aBoxLength );

        /**
        * Verify and process Channel Definition box (in JP2 Header box)
        * @since 2.6
        * @param aBoxLength: the length of the Channel Definition box
        */
        void ReadChannelDefBoxL( TUint32 aBoxLength );

        /**
        * Verify and process IPR box
        * @since 2.6
        */
        void ReadIPRBoxL();

        /**
        * Verify and process XML box
        * @since 2.6
        */
        void ReadXMLBoxL();

        /**
        * Verify and process UUID box
        * @since 2.6
        */
        void ReadUUIDBoxL();

        /**
        * Verify and process UUID Info box (superbox)
        * @since 2.6
        */
        void ReadUUIDInfoBoxL();

        /**
        * Ignore the content and advance the iterator to the next box
        * @since 2.6
        */
        void IgnoreBoxL();

        /**
        * Update the current state according to the box type
        * @since 2.6
        */
        void UpdateStateFromBoxTypeL();

        /**
        * Read frame headers and instantiate decoder for application.
        * @since 2.6
        */
        void ScanDataL();

    public:     // Data
        
        // State machine represent the state when parsing the JP2 file format.
        enum TDecoderState
            {
            EStateInUnknown,
            EStateInSignatureBox,
            EStateInFileTypeBox,
            EStateInJP2SuperBox,
            EStateInIPRBox,
            EStateInXMLBox,
            EStateInUUIDBox,
            EStateInUUIDInfoBox,
            EStateInCodestreamBox
            };
    
    protected:  // Data

    private:    // Data

        // JP2 file format information
        TJ2kInfo iJ2kInfo;    

        // Descriptor of the buffer
        TPtrC8 iBufferDes;  

        // Length of box read
        TUint32 iBoxLength;  

        // Type of box read
        TUint32 iBoxType;    

        // Position of last ReadDataL() used
        TUint32 iLastRead;   

        // Enter the method from ICL framework
        TBool    iCallFromFramework; 

        // Pointer into the data represented by iBufferDes
        const TUint8 *iPtr;   

        // Current state of the parsing of file or data
        TDecoderState iState; 

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
};

#endif // __JP2KCONVERT_H__
