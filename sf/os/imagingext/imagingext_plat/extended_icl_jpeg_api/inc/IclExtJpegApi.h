/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of CVisualFrame, CExtJpegDecoder and CExtJpegEncoder
*
*/

#ifndef _ICLEXTJPEGAPI_
#define _ICLEXTJPEGAPI_

#include "imageconversion.h"
#include <icl/icl_uids.hrh>

#include "iclexifimageframe.h"
const TInt KMaxNumberOfPlanes = KMaxPlanesInFrame;
const TUid KUidExtFormatJpeg = { 0 };
const TUid KUidExtIclApiSupport = { 0 };
const TInt KExtensionUID = 0x01010101;

// FORWARD DECLARATIONS
class CVisualFrameImpl;

// CLASS DECLARATION

/**
* CVisualFrame
* @lib IclExtJpegApi.lib
* @since 3.2
*/
class CVisualFrame : public CBase
    {
    public:

        // TFrameLayout contains the scan line lengths and offsets for each component in planar YUV case
        class TFrameLayout
            {
            public:
                TInt iNumberOfPlanes;
                TInt iScanLineLength[KMaxNumberOfPlanes];
                TInt iOffset[KMaxNumberOfPlanes];
                TInt iLength[KMaxNumberOfPlanes];
                TInt iMaxLength[KMaxNumberOfPlanes];                
            };

        enum TDataContainer
            {
            EInChunk,
            EInDescriptor
            };
            
        enum TDataFormat
            {
            /* Copied from CCamera::TFormat: */
            
            /** 8 bit greyscale values, 0=black, 255=white. */
            EFormatMonochrome            = 0x0001,//+
            /** Packed RGB triplets, 4 bits per pixel with red in the least significant bits
            and the 4 most significant bits unused. */
            EFormat16bitRGB444            = 0x0002,//+
            /** Packed RGB triplets, 5 bits per pixel for red and blue and 6 bits for green,
            with red in the least significant bits. */
            EFormat16BitRGB565            = 0x0004,//+
            /** Packed RGB triplets, 8 bits per pixel with red in the least significant bits
            and the 8 most significant bits unused. */
            EFormat32BitRGB888            = 0x0008,//+
            /** JFIF JPEG. */
            EFormatJpeg                    = 0x0010,
            /** EXIF JPEG */
            EFormatExif                    = 0x0020,
            /** CFbsBitmap object with display mode EColor4K. */
            EFormatFbsBitmapColor4K        = 0x0040,
            /** CFbsBitmap object with display mode EColor64K. */
            EFormatFbsBitmapColor64K    = 0x0080,
            /** CFbsBitmap object with display mode EColor16M. */
            EFormatFbsBitmapColor16M    = 0x0100,
            /** Implementation dependent. */
            EFormatUserDefined            = 0x0200,
            /** 4:2:0 format, 8 bits per sample, Y00Y01Y10Y11UV. */
            EFormatYUV420Interleaved    = 0x0400,//+
            /** 4:2:0 format, 8 bits per sample, Y00Y01Y02Y03...U0...V0... */
            EFormatYUV420Planar            = 0x0800,//+
            /** 4:2:2 format, 8 bits per sample, UY0VY1. */
            EFormatYUV422                = 0x1000,//+
            /** 4:2:2 format, 8 bits per sample, Y1VY0U. */
            EFormatYUV422Reversed        = 0x2000,//+
            /** 4:4:4 format, 8 bits per sample, Y00U00V00 Y01U01V01... */
            EFormatYUV444                = 0x4000,//+
            /** 4:2:0 format, 8 bits per sample, Y00Y01Y02Y03...U0V0... */
            EFormatYUV420SemiPlanar        = 0x8000,//+

            /* Other formats: */
            
            /** 4:2:2 format, 8 bits per sample, Y0Y1UV. */
            EFormatExtYUV422Interleaved = 0x00100000,
            /** 4:2:2 format, 8 bits per sample, Y0Y1Y2...U0U1...V0V1. */
            EFormatExtYUV422Planar      = 0x00200000,
            /** 4:4:4 format, 8 bits per sample, Y0U0V0Y1U1V1. */
            EFormatExtYUV444Planar = 0x00400000,
            /** DCT coefficients */
            EFormatExtDctCoeff = 0x00800000,            
                        
            };
    
    public: // New functions

        /**
        * Symbian C++ Two-phased constructor.
        * @since 3.2
        * @param aBuffer        A descriptor reference to buffer containing the image data.
        * @param aDimension     The dimensions of the corresponding image data.
        * @param aFrameFormat	The frame format of the corresponding image data.
        * @return CVisualFrame* A pointer to frame object.
        */
        IMPORT_C static CVisualFrame* NewL( 
                                    TDes8& aBuffer, 
                                    const TSize& aDimension,
                                    TInt aFrameFormat );

        /**
        * Symbian C++ Two-phased constructor.
        * @since 3.2
        * @param aChunk         An RChunk reference to buffer containing the image data.
        * @param aSize          The amount of image data in bytes.
        * @param amaxSize       The maximum size of the memory reserved in the chunk.
        * @param aDataOffset    The offset value specifying the location of the image 
        *                       data in the chunk.
        * @param aDimension     The dimensions of the corresponding image data.
        * @param aFrameFormat   The frame format of the corresponding image data.
        * @return CVisualFrame* A pointer to frame object.
        */
        IMPORT_C static CVisualFrame* NewL( 
                                    RChunk& aChunk, 
                                    TInt aSize,
                                    TInt aMaxSize, 
                                    TInt aDataOffset,
                                    const TSize& aDimension, 
                                    TInt aFrameFormat );

        /**
        * Symbian C++ Two-phased constructor.
        * @since 3.2
        * @param aBuffer        A descriptor reference to buffer containing the image data.
        * @param aDimension     The dimensions of the corresponding image data.
        * @param aFrameLayout   The layout of the image color components.
        * @return CVisualFrame* A pointer to frame object.
        */
        IMPORT_C static CVisualFrame* NewL( 
                                    TDes8& aBuffer, 
                                    const TSize& aDimension, 
                                    TInt aFrameFormat, 
                                    const TFrameLayout& aFrameLayout ); 

        /**
        * Symbian C++ Two-phased constructor.
        * @since 3.2
        * @param aChunk         An RChunk reference to buffer containing the image data.
        * @param aSize          The amount of image data in bytes.
        * @param aMaxSize       The maximum size of the memory reserved in the chunk for 
        *                       the image data.
        * @param aDimension     The dimensions of the corresponding image data.
        * @param aFrameFormat   The frame format of the corresponding image data.
        * @param aFrameLayout   The layout of the image color components. TFrameLayout 
        * 						contains the scan line lengths and offsets for each component 
        *						in planar YUV case
        * @return CVisualFrame* A pointer to frame object.
        */
        IMPORT_C static CVisualFrame* NewL( 
                                    RChunk& aChunk, 
                                    TInt aSize, 
                                    TInt aMaxSize,
                                    const TSize& aDimension, 
                                    TInt aFrameFormat, 
                                    const TFrameLayout& aFrameLayout );        
       
       
        /**
        * Returns a constant reference to the chunk containing 
        * the image data, if exists.
        * @since 3.2
        * @return RChunk
        */
        IMPORT_C const RChunk& DataChunkL() const;
        
        /**
        * Returns a descriptor pointer to the image data location, if exists
        * @since 3.2
        * @return TPtr8
        */        
        IMPORT_C TPtr8 DataPtrL() const;

        /**
        * Returns the dimensions of the image.
        * @since 3.2
        * @return TSize
        */        
        IMPORT_C TSize Dimension() const;

        /**
        * Return the image data format.
        * @since 3.2
        * @return TInt
        */        
        IMPORT_C TInt FrameFormat() const;
        
        /**
        * Returns the offset of the image data for non-planar, 
        * single-component or compressed images. 
        * @since 3.2
        * @return TInt
        */        
        IMPORT_C TInt DataOffset() const;

        /**
        * Returns the amount of stored image data in bytes.
        * @since 3.2
        * @return TInt
        */        
        IMPORT_C TInt Size() const;

        /**
        * Sets the amount of image data to the given value. 
        * Should be called when the image data amount is modified explicitly.
        * @since 3.2
        * @return void
        */
        IMPORT_C void SetSizeL( TInt aSize );

        /**
        * Returns the maximum size of the memory space reserved for image data
        * @since 3.2
        * @return TInt
        */
        IMPORT_C TInt MaxSize() const;

        /**
        * Returns the storage type of the image data.
        * @since 3.2
        * @return TDataContainer
        */        
        IMPORT_C TDataContainer DataContainer() const;

        /**
        * Returns the layout of the image data for planar compressed images.
        * @since 3.2
        * @return TFrameLayout
        */        
        IMPORT_C const TFrameLayout& FrameLayout() const;    

        /**
        * Destructor
        */
        IMPORT_C virtual ~CVisualFrame();

        /**
        * Symbian C++ Two-phased constructor.
        */        
        IMPORT_C static CVisualFrame* NewL( const CImageFrame* aImageFrame );

        /**
        * Get image frame
        * @since 3.2
        * @return CImageFrame
        */
        IMPORT_C CImageFrame* GetImageFrameL();
       
        
    private:
    
        CVisualFrameImpl* iVisualFrameImpl;

    };
    

// CLASS DECLARATION

/**
* CExtJpegDecoder
* @lib IclExtJpegApi.lib
* @since 3.2
*/
class CExtJpegDecoder : public CJPEGImageFrameDecoder
    {
    public:

        enum TDecoderCapability
            {
            ECapNone = 0x0000,
            ECapCropping = 0x0001,
            ECapStreaming = 0x0002,
            ECapRotation = 0x0004,
            ECapFlipping = 0x0008,
            ECapMirroring = 0x0010,
            ECapDctDecoding = 0x0020,
            ECapExifData = 0x0040
            };
            
        enum TDecoderOperations
            {
            EEnd = 0x00010000,
            ESupportedFormats = 0x00020000,
            ECapabilities = 0x00040000,
            EReadyForAsync = 0x00080000,
            EConvert = 0x00100000,
            EContinueConvert = 0x00200000
            };            
            
        enum TDecoderType
            {
            ESwImplementation = 0,
            EHwImplementation
            };
            
    public:

        /**
        * Symbian C++ Two-phased constructor.
        * @since 3.2
        */
        IMPORT_C static CExtJpegDecoder* NewL();

        
        /**
        * Creates and initializes CExtJpegDecoder
        * @since 3.2
        * @param aFs                A reference to a file server session to use.
        * @param aSourceFilename    The name of the Jpeg file to decode.
        * @param aMIMEType          The Jpeg MIME type for matching the decoder plugin.
        * @param aOptions           Decoder options defined in ICL.
        * @return CExtJpegDecoder*  A pointer to decoder object.
        */
        IMPORT_C static CExtJpegDecoder* FileNewL( 
                                    RFs& aFs, 
                                    const TDesC& aSourceFilename, 
                                    const TDesC8& aMIMEType, 
                                    const TOptions aOptions = EOptionNone );

        /**
        * Creates and initializes CExtJpegDecoder
        * @since 3.2
        * @param aFs                A reference to a file server session to use.
        * @param aSourceFilename    The name of the Jpeg file to decode.
        * @param aOptions           Decoder options defined in ICL.
        * @param aImageType         The Jpeg image type.    
        * @param aImageSubType      The Jpeg image subtype (Null UID).  
        * @param aDecoderUid        The decoder plugin UID.
        * @return CExtJpegDecoder*  A pointer to decoder object.
        */      
        IMPORT_C static CExtJpegDecoder* FileNewL( 
                                    RFs& aFs, 
                                    const TDesC& aSourceFilename, 
                                    const TOptions aOptions = EOptionNone, 
                                    const TUid aImageType = KNullUid, 
                                    const TUid aImageSubType = KNullUid, 
                                    const TUid aDecoderUid = KNullUid );

        /**
        * Creates and initializes CExtJpegDecoder
        * @since 3.2
        * @param aFs                A reference to a file server session to use.
        * @param aSourceData        The descriptor reference for the Jpeg image data to decode.
        * @param aMIMEType          The Jpeg MIME type for matching the decoder plugin.
        * @param aOptions           Decoder options defined in ICL.
        * @return CExtJpegDecoder*  A pointer to decoder object.
        */      
        IMPORT_C static CExtJpegDecoder* DataNewL( 
                                    RFs& aFs,
                                    const TDesC8& aSourceData,
                                    const TDesC8& aMIMEType,
                                    const TOptions aOptions = EOptionNone );
        /**
        * Creates and initializes CExtJpegDecoder
        * @since 3.2
        * @param aFs                A reference to a file server session to use.
        * @param aSourceData        The descriptor reference for the Jpeg image data to decode.
        * @param aOptions           Decoder options defined in ICL.
        * @param aImageType         The Jpeg image type.
        * @param aImageSubType      The Jpeg image subtype (Null UID).
        * @param aDecoderUid        The decoder plugin UID.
        * @return CExtJpegDecoder*  A pointer to decoder object.
        */        
        IMPORT_C static CExtJpegDecoder* DataNewL( 
                                    RFs& aFs, 
                                    const TDesC8& aSourceData,
                                    const TOptions aOptions = EOptionNone,
                                    const TUid aImageType  = KNullUid,
                                    const TUid aImageSubType = KNullUid,
                                    const TUid aDecoderUid = KNullUid );

        /**
        * Creates and initializes CExtJpegDecoder
        * @since 3.2
        * @param aFs                A reference to a file server session to use.    
        * @param aSourceData        Pointer to the visual frame structure keeping 
        *                           the Jpeg image data to decode.
        * @param aMIMEType          The Jpeg MIME type for matching the decoder plugin.
        * @param aOptions           Decoder options defined in ICL.
        * @return CExtJpegDecoder*  A pointer to decoder object.
        */
        IMPORT_C static CExtJpegDecoder* DataNewL( 
                                    RFs& aFs, 
                                    const CVisualFrame* aSourceData,
                                    const TDesC8& aMIMEType,
                                    const TOptions aOptions = EOptionNone );

        /**
        * Creates and initializes CExtJpegDecoder
        * @since 3.2
        * @param aFs                A reference to a file server session to use.    
        * @param aSourceData        Pointer to the visual frame structure keeping 
        *                           the Jpeg image data to decode.  
        * @param aOptions           Decoder options defined in ICL.
        * @param aImageType         The Jpeg image type.
        * @param aImageSubType      The Jpeg image subtype (Null UID).
        * @param aDecoderUid        The decoder plugin UID.
        * @return CExtJpegDecoder*  A pointer to decoder object.
        */        
        IMPORT_C static CExtJpegDecoder* DataNewL( 
                                    RFs& aFs,
                                    const CVisualFrame* aSourceData,
                                    const TOptions aOptions = EOptionNone,
                                    const TUid aImageType  = KNullUid,
                                    const TUid aImageSubType = KNullUid,
                                    const TUid aDecoderUid = KNullUid );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CExtJpegDecoder();
    
        /**
        * Enables the use of cropping feature and specifies the cropping region. 
        * @since 3.2
        * @param aCropRect  The rectangular cropping region that will be decoded.
        * @return void
        */
        IMPORT_C void SetCroppingL( TRect aCropRect );

        /**
        * Enables the use of streaming feature and retrieves the macroblock 
        * dimensions in the Jpeg data.
        * @since 3.2
        * @param aMacroBlockSize    The macroblock dimensions in the 
        *                           Jpeg image that is set by the decoder
        * @return void
        */
        IMPORT_C void SetStreamingL( TSize& aMacroBlockSize );

        /**
        * Enables the use of rotation feature and specifies the rotating degree. 
        * @since 3.2
        * @param aDegree    The rotation degree. Can take any values between 1 and 359.
        * @return void
        */
        IMPORT_C void SetRotationL( TInt aDegree );

        /**
        * Enables the use of flipping feature.
        * @since 3.2
        * @return void
        */
        IMPORT_C void SetFlippingL();

        /**
        * Enables the use of mirroring feature.
        * @since 3.2
        * @return void
        */
        IMPORT_C void SetMirroringL();
        
        /**
        * SEnables the use of DCT decoding feature.
        * @since 3.2
        * @return void
        */        
        IMPORT_C void SetDctDecodingL();

        /**
        * Asynchronous function for initiating the decoding
        * @since 3.2
        * @param aRequestStatus         The pointer to the status of the operation 
        *                               that is set by the decoder after it is completed.
        *                               After a complete successful operation, the status 
        *                               is KErrNone.
        * @param aDestinationFrame      Pointer to the visual frame structure that is keeping 
        *                               the destination image data. The client should do the 
        *                               proper allocation of the destination location.
        * @param aNoOfDecodedMBlocks    Reference to the number of decoded macroblocks that is 
        *                               set by the decoder after the decoding is completed.
        * @param aFrameNumber           The frame index in the Jpeg image.
        * @return void
        */
        IMPORT_C void ConvertL( 
                        TRequestStatus* aRequestStatus, 
                        const CVisualFrame* aDestinationFrame, 
                        TInt& aNoOfDecodedMBlocks,
                        TInt aFrameNumber = 0 );

        /**
        * Asynchronous function for initiating one of the iterations of streamed decoding.
        * @since 3.2
        * @param aRequestStatus         The pointer to the status of the operation 
        *                               that is set by the decoder after it is completed.
        *                               After a complete successful operation, the status 
        *                               is KErrNone.
        * @param aDestinationFrame      Pointer to the visual frame structure that is keeping 
        *                               the destination image data. The client should do the 
        *                               proper allocation of the destination location.
        * @param aNoOfDecodedMBlocks    Reference to the number of decoded macroblocks that is 
        *                               set by the decoder after the decoding is completed.
        * @param aFrameNumber           The frame index in the Jpeg image.
        * @return void
        */
        IMPORT_C void ContinueConvertL( 
                        TRequestStatus* aRequestStatus, 
                        const CVisualFrame* aDestinationFrame, 
                        TInt& aNoOfDecodedMBlocks,
                        TInt aFrameNumber = 0 );

        /**
        * Returns the destination (uncompressed) data formats that are supported by the decoder
        * @since 3.2
        * @return TInt  The supported destination (uncompressed) data formats. 
        *               The value is a combination of the flags defined in 
        *               CVisualFrame::TDataFormat.
        */
        IMPORT_C TInt SupportedFormatsL();

        /**
        * Returns the Extended API features (capabilities) that are supported by the decoder.
        * @since 3.2
        * @return TInt  The supported Extended API features. 
        *               The value is a combination of the flags defined in TDecoderCapabilities.
        */
        IMPORT_C TInt CapabilitiesL();

        /**
        * Creates and initializes CExtJpegDecoder
        * @since 3.2
        * @param aDecoderType       Decoder implementation type: HW or SW.  
        * @param aFs                A reference to a file server session to use.
        * @param aSourceFilename    The name of the Jpeg file to decode.
        * @param aOptions           Decoder options defined in ICL.
        * @return CExtJpegDecoder*  A pointer to decoder object.
        */        
        IMPORT_C static CExtJpegDecoder* FileNewL( 
                                    const TDecoderType aDecoderType,
                                    RFs& aFs, 
                                    const TDesC& aSourceFilename, 
                                    const TOptions aOptions = EOptionNone );
                  
        /**
        * Creates and initializes CExtJpegDecoder
        * @since 3.2
        * @param aDecoderType       Decoder implementation type: HW or SW.
        * @param aFs                A reference to a file server session to use.
        * @param aSourceData        The descriptor reference for the Jpeg image data to decode.
        * @param aOptions           Decoder options defined in ICL.
        * @return CExtJpegDecoder*  A pointer to decoder object.
        */                                            
        IMPORT_C static CExtJpegDecoder* DataNewL( 
                                    const TDecoderType aDecoderType,
                                    RFs& aFs, 
                                    const TDesC8& aSourceData, 
                                    const TOptions aOptions = EOptionNone );

        /**
        * Creates and initializes CExtJpegDecoder
        * @since 3.2
        * @param aDecoderType       Decoder implementation type: HW or SW.
        * @param aFs                A reference to a file server session to use.
        * @param aSourceData        Pointer to the visual frame structure keeping 
        *                           the Jpeg image data to decode.
        * @param aOptions           Decoder options defined in ICL.
        * @return CExtJpegDecoder*  A pointer to decoder object.
        */                                            
        IMPORT_C static CExtJpegDecoder* DataNewL( 
                                    const TDecoderType aDecoderType, 
                                    RFs& aFs, 
                                    const CVisualFrame* aSourceData, 
                                    const TOptions aOptions = EOptionNone );
                      
        /**
        * Convert
        * @since 3.2
        * @param aRequestStatus     The pointer to the status of the operation 
        *                           that is set by the decoder after it is completed.
        *                           After a complete successful operation, the status 
        *                           is KErrNone.
        * @param aDestination       
        * @param aFrameNumber       The frame index in the Jpeg image.
        * @return void
        */                            
        IMPORT_C void Convert(  TRequestStatus* aRequestStatus, 
                                CFbsBitmap& aDestination, TInt aFrameNumber = 0);
                                
        /**
        * Convert
        * @since 3.2
        * @param aRequestStatus     The pointer to the status of the operation 
        *                           that is set by the decoder after it is completed.
        *                           After a complete successful operation, the status 
        *                           is KErrNone.
        * @param aDestination       Bitmap destination
        * @param aDestinationMask   Bitmap destination mask
        * @param aFrameNumber       The frame index in the Jpeg image.
        * @return void
        */                              
        IMPORT_C void Convert(  TRequestStatus* aRequestStatus, 
                                CFbsBitmap& aDestination, 
                                CFbsBitmap& aDestinationMask, 
                                TInt aFrameNumber = 0 );
                                
        /**
        * Continue convert
        * @since 3.2
        * @param aRequestStatus     The pointer to the status of the operation 
        *                           that is set by the decoder after it is completed.
        *                           After a complete successful operation, the status 
        *                           is KErrNone.
        * @return void
        */
        IMPORT_C void ContinueConvert( TRequestStatus* aRequestStatus );

    private:

        /**
        * C++ default constructor.
        */
        CExtJpegDecoder();
        
        /**
        * Get an uid by version range
        * @since 3.2
        * @param aMinVersion
        * @param aMaxVersion
        * @return TUid
        */
        static TUid GetUidByVersionRangeL( TInt aMinVersion, TInt aMaxVersion );
        
        TBool iIsExtConvert;

    };


// CLASS DECLARATION

/**
* CExtJpegEncoder
* @lib IclExtJpegApi.lib
* @since 3.2
*/
class CExtJpegEncoder : public CJPEGImageFrameEncoder
    {
    public:

        enum TEncoderCapability
            {
            ECapNone = 0x0000,
            ECapStreaming = 0x0001,
            ECapBitmapOverlay = 0x0002,
            ECapImageReplacing = 0x0004,
            ECapBitmapReplacing = 0x0008,
            ECapLosslessRotation = 0x0010,
            ECapLosslessFlipping = 0x0020,
            ECapLosslessMirroring = 0x0040,
            ECapDctEncoding = 0x0080
            };
            
        enum TEncoderOperations
            {
            EEnd = 0x00010000,
            ESupportedFormats = 0x00020000,
            ECapabilities = 0x00040000,
            EReadyForAsync = 0x00080000,
            EConvert = 0x00100000,
            EContinueConvert = 0x00200000,
            EDestVisualFrame = 0x00400000,
            ENewDestData = 0x00800000,
            EDestDataSize = 0x01000000
            };
                 
        enum TEncoderType
            {
            ESwImplementation = 0,
            EHwImplementation
            };                   

    public:
    

        /**
        * Symbian C++ Two-phased constructor.
        */      
        IMPORT_C static CExtJpegEncoder* NewL();
        
        /**
        * Creates and initializes CExtJpegEncoder
        * @since 3.2
        * @param aFs                    A reference to a file server session to use.
        * @param aDestinationFilename   The name of the destination Jpeg file.
        * @param aMIMEType              The Jpeg MIME type for matching the encoder plugin.
        * @param aOptions               Encoder options defined in ICL.
        * @return CExtJpegEncoder*      A pointer to encoder object.
        */
        IMPORT_C static CExtJpegEncoder* FileNewL( 
                                    RFs& aFs, 
                                    const TDesC& aDestinationFilename, 
                                    const TDesC8& aMIMEType, 
                                    const TOptions aOptions = EOptionNone );

        /**
        * Creates and initializes CExtJpegEncoder
        * @param aFs                    A reference to a file server session to use.
        * @param aDestinationFilename   The name of the destination Jpeg file.
        * @param aOptions               Encoder options defined in ICL.
        * @param aImageT                The Jpeg image type.
        * @param aImageSubType          The Jpeg image subtype (Null UID).
        * @param aEncoderUid            The encoder plugin UID.
        * @return CExtJpegEncoder*      A pointer to encoder object.
        */      
        IMPORT_C static CExtJpegEncoder* FileNewL( 
                                    RFs& aFs, 
                                    const TDesC& aDestinationFilename, 
                                    const TOptions aOptions = EOptionNone, 
                                    const TUid aImageType = KNullUid, 
                                    const TUid aImageSubType = KNullUid, 
                                    const TUid aEncoderUid = KNullUid );

        /**
        * Creates and initializes CExtJpegEncoder
        * @param aDestinationData   Pointer reference to the destination 
        *                           Jpeg data location that will be allocated by the encoder.
        * @param aMIMEType          The Jpeg MIME type for matching the encoder plugin.
        * @param aOptions           Encoder options defined in ICL.
        * @return CExtJpegEncoder*  A pointer to encoder object.
        */  
        IMPORT_C static CExtJpegEncoder* DataNewL( 
                                    HBufC8*& aDestinationData,
                                    const TDesC8& aMIMEType,
                                    const TOptions aOptions = EOptionNone);
        /**
        * Creates and initializes CExtJpegEncoder
        * @param aDestinationData   Pointer reference to the destination 
        *                           Jpeg data location that will be allocated by the encoder.
        * @param aOptions           Encoder options defined in ICL.
        * @param aImageType         The Jpeg image type.
        * @param aImageSubType      The Jpeg image subtype (Null UID).
        * @param aEncoderUid        The encoder plugin UID.
        * @return CExtJpegEncoder*  A pointer to encoder object.
        */  
        IMPORT_C static CExtJpegEncoder* DataNewL( 
                                    HBufC8*& aDestinationData,
                                    const TOptions aOptions = EOptionNone,
                                    const TUid aImageType = KNullUid,
                                    const TUid aImageSubType = KNullUid,
                                    const TUid aEncoderUid = KNullUid);
        /**
        * Creates and initializes CExtJpegEncoder
        * @param aDestinationData   Pointer to the visual frame structure keeping the destination 
        *                           Jpeg data allocated with maximum size by the client.
        * @param aMIMEType          The Jpeg MIME type for matching the encoder plugin.
        * @param aOptions           Encoder options defined in ICL.
        * @return CExtJpegEncoder*  A pointer to encoder object.
        */  
        IMPORT_C static CExtJpegEncoder* DataNewL( 
                                    const CVisualFrame* aDestinationData,
                                    const TDesC8& aMIMEType,
                                    const TOptions aOptions = EOptionNone);
        /**
        * Creates and initializes CExtJpegEncoder
        * @param aDestinationData   Pointer to the visual frame structure keeping the destination 
        *                           Jpeg data allocated with maximum size by the client.
        * @param aOptions           Encoder options defined in ICL.
        * @param aImageType         The Jpeg image type.
        * @param aImageSubType      The Jpeg image subtype (Null UID).
        * @param aEncoderUid        The encoder plugin UID.
        * @return CExtJpegEncoder*  A pointer to encoder object.
        */  
        IMPORT_C static CExtJpegEncoder* DataNewL( 
                                    const CVisualFrame* aDestinationData,
                                    const TOptions aOptions = EOptionNone,
                                    const TUid aImageType = KNullUid,
                                    const TUid aImageSubType = KNullUid,
                                    const TUid aEncoderUid = KNullUid);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CExtJpegEncoder();

        /**
        * Enables the use of streaming feature and retrieves the
        * macroblock dimensions in the Jpeg data. 
        * @since 3.2
        * @param aMacroBlockSize    The macroblock dimensions in the Jpeg 
        *                           image that is set by the encoder.
        * @param aFrameImageData    The pointer to the destination Jpeg image data structure.
        * @return void
        */
        IMPORT_C void SetStreamingL( TSize& aMacroBlockSize, 
                                     const CFrameImageData* aFrameImageData= NULL );

        /**
        * Enables the use of replacing feature, where the given image 
        * is replaced starting from the specified up-left corner point.
        * @since 3.2
        * @param aReplaceImage      The image data that will be replaced.
        * @param aReplacePoint      The up-left corner point of the replacing region.
        * @return void
        */
        IMPORT_C void SetImageReplaceL( const CVisualFrame* aReplaceImage, TPoint aReplacePoint );

        /**
        * Enables the use of replacing feature, where the given bitmap 
        * is replaced starting from the specified up-left corner point.
        * @since 3.2
        * @param aReplaceBitmap     The bitmap that will be replaced.
        * @param aReplacePoint      The up-left corner point of the replacing region.
        * @return void
        */
        IMPORT_C void SetBitmapReplaceL( const CFbsBitmap& aReplaceBitmap, TPoint aReplacePoint );

        /**
        * Enables the use of overlay feature, where the given bitmap
        * is overlayed transparently based on the given alpha value
        * starting from the specified up-left corner point.
        * @since 3.2
        * @param aOverlayBitmap     The bitmap that will be overlayed.
        * @param aAlphaValue        The alpha value for the transparency of the overlayed bitmap.
        * @param aOverlayPoint      The up-left corner point of the overlay region.
        * @return void
        */
        IMPORT_C void SetBitmapOverlayL( const CFbsBitmap& aOverlayBitmap, 
                                         TUint aAlphaValue, TPoint aOverlayPoint );

        /**
        * Enables the use of lossless rotation feature and specifies the rotating degree.
        * @since 3.2
        * @param aDegree    The rotation degree. Can take any values between 1 and 359.
        * @return void
        */
        IMPORT_C void SetLosslessRotationL( TInt aDegree );

        /**
        * Enables the use of lossless flipping feature.
        * @since 3.2
        * @return void
        */
        IMPORT_C void SetLosslessFlippingL();

        /**
        * SEnables the use of lossless mirroring feature.
        * @since 3.2
        * @return void
        */
        IMPORT_C void SetLosslessMirroringL();

        /**
        * Enables the use of DCT encoding feature.
        * @since 3.2
        * @return void
        */        
        IMPORT_C void SetDctEncodingL();

        /**
        * Provides a new visual frame structure for the 
        * destination data replacing any previously delivered ones.
        * All the operations coming after this function call should
        * use the provided structure.
        * @since 3.2
        * @param aDestinationData   The new visual frame structure for the destination data.
        * @return void
        */        
        IMPORT_C void ProvideNewDestDataL( const CVisualFrame* aDestinationData );

        /**
        * Asynchronous function for initiating the encoding. 
        * @since 3.2
        * @param aRequestStatus         The pointer to the status of the operation 
        *                               that is set by the encoder after it is completed.
        *                               After a complete successful operation, the status 
        *                               is KErrNone.
        * @param aSourceFrame           Pointer to the visual frame structure that is keeping 
        *                               the source image data. In streaming case, contains a 
        *                               part of the uncompressed image data at the beginning.
        * @param aNoOfEncodedMBlocks    Reference to the number of encoded macroblocks that is 
        *                               set by the encoder after the encoding is completed.
        * @param aFrameImageData        Pointer to optional frame image data structure defined in ICL.
        * @return void
        */
        IMPORT_C void ConvertL( 
                        TRequestStatus* aRequestStatus, 
                        const CVisualFrame* aSourceFrame, 
                        TInt& aNoOfEncodedMBlocks,
                        const CFrameImageData* aFrameImageData = NULL );

        /**
        * Asynchronous function for initiating one of the iterations of streamed encoding.
        * @since 3.2
        * @param aRequestStatus         The pointer to the status of the operation 
        *                               that is set by the encoder after it is completed.
        *                               After a complete successful operation, the status 
        *                               is KErrNone.
        * @param aSourceFrame           Pointer to the visual frame structure that is keeping 
        *                               the source image data. In streaming case, contains a 
        *                               part of the uncompressed image data at the beginning.
        * @param aNoOfEncodedMBlocks    Reference to the number of encoded macroblocks that is 
        *                               set by the encoder after the encoding is completed.
        * @return
        */
        IMPORT_C void ContinueConvertL( 
                        TRequestStatus* aRequestStatus, 
                        const CVisualFrame* aSourceFrame,
                        TInt& aNoOfEncodedMBlocks );

        /**
        * Returns the source (uncompressed) data formats that are supported by the encoder.
        * @since 3.2
        * @return TInt  The supported source (uncompressed) data formats. 
        *               The value is a combination of the flags defined
        *               in CVisualFrame::TDataFormat.
        */
        IMPORT_C TInt SupportedFormatsL();

        /**
        * Returns the Extended API features (capabilities) that are supported by the encoder.
        * @since 3.2
        * @return TInt  The supported Extended API features. 
        *               The value is a combination of the flags defined 
        *               in TEncoderCapabilities.
        */
        IMPORT_C TInt CapabilitiesL();

        /**
        * Creates and initializes CExtJpegEncoder
        * @since 3.2
        * @param aEncoderType           Encoder implementation type: HW or SW.
        * @param aFs                    A reference to a file server session to use.
        * @param aDestinationFilename   The name of the destination Jpeg file.
        * @param aOptions               Encoder options defined in ICL.
        * @return CExtJpegEncoder*      A pointer to encoder object.
        */        
        IMPORT_C static CExtJpegEncoder* FileNewL( 
                                    const TEncoderType aEncoderType,
                                    RFs& aFs, 
                                    const TDesC& aDestinationFilename, 
                                    const TOptions aOptions = EOptionNone );
                
        /**
        * Creates and initializes CExtJpegEncoder
        * @since 3.2
        * @param aEncoderType       Encoder implementation type: HW or SW.
        * @param aDestinationData   Pointer reference to the destination Jpeg
        *                           data location that will be allocated by the encoder.
        * @param aOptions           Encoder options defined in ICL.
        * @return CExtJpegEncoder*  A pointer to encoder object.
        */                                    
        IMPORT_C static CExtJpegEncoder* DataNewL( 
                                    const TEncoderType aEncoderType,
                                    HBufC8*& aDestinationData, 
                                    const TOptions aOptions = EOptionNone );
        /**
        * Creates and initializes CExtJpegEncoder
        * @since 3.2
        * @param aEncoderType       Encoder implementation type: HW or SW.
        * @param aDestinationData   Pointer to the visual frame structure keeping the 
        *                           destination Jpeg data allocated with maximum size by the client.
        * @param aOptions           Encoder options defined in ICL.
        * @return CExtJpegEncoder*  A pointer to encoder object.
        */                                    
        IMPORT_C static CExtJpegEncoder* DataNewL( 
                                    const TEncoderType aEncoderType, 
                                    const CVisualFrame* aDestinationData, 
                                    const TOptions aOptions = EOptionNone );        
        
        /**
        * Returns the actual destination buffer size
        * @since 3.2
        * @return TInt  The actual destination buffer size required. 
        *               -1 means the codec is not able to determine the required buffer size.
        */                                    
        IMPORT_C TInt GetDestinationDataSizeL();                                    


    private:
        
        /**
        * C++ default constructor.
        */
        CExtJpegEncoder();
        
        /**
        * Get an uid by version range
        * @since 3.2
        * @param aMinVersion
        * @param aMaxVersion
        * @return TUid
        */
        static TUid GetUidByVersionRangeL( TInt aMinVersion, TInt aMaxVersion );
    
    };

#endif  // _ICLEXTJPEGAPI_

// End of File
