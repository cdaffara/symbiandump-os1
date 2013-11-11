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
* Description:  CJp2kReadCodec class implements the ICL read codec interface.
*
*/


#ifndef __JP2KCODEC_H__
#define __JP2KCODEC_H__

//  INCLUDES
#include <icl/imageprocessor.h>
#include <icl/imagecodec.h>
#include <icl/imagecodecdata.h>

// CONSTANTS

// Delimiting marker segments
const TUint16 KSOC = 0xFF4F;    // Start of codestream
const TUint16 KSOT = 0xFF90;    // Start of tile-part
const TUint16 KSOD = 0xFF93;    // Start of data
const TUint16 KEOC = 0xFFD9;    // End of codestream

// Fixed information marker segments
const TUint16 KSIZ = 0xFF51;    // Image and tile size

// Functional marker segments
const TUint16 KCOD = 0xFF52;    // Coding style default
const TUint16 KCOC = 0xFF53;    // Coding style component
const TUint16 KRGN = 0xFF5E;    // Region of interest 
const TUint16 KQCD = 0xFF5C;    // Quantization default
const TUint16 KQCC = 0xFF5D;    // Quantization component
const TUint16 KPOC = 0xFF5F;    // Progression order default

// Pointer marker segments 
const TUint16 KTLM = 0xFF55;    // Tile-part lengths, main header
const TUint16 KPLM = 0xFF57;    // Packet length, main header
const TUint16 KPLT = 0xFF58;    // Packet length, tile-part header
const TUint16 KPPM = 0xFF60;    // Packed packet headers, main header 
const TUint16 KPPT = 0xFF61;    // Packed packet headers, tile-part header 

// Informational marker segments
const TUint16 KCME = 0xFF64;    // Comment and extension 
const TUint16 KCRG = 0xFF63;    // Component registration 

// Packet information 
const TUint16 KSOP = 0xFF91;    // Start of packet
const TUint16 KEPH = 0xFF92;    // End of packet header 
const TUint16 KSOP_LEN = 0x04;  // Length of the SOP 

// Extensions marker segments 
const TUint16 KEXTS = 0xFF30;    // Start of extensions
const TUint16 KEXTE = 0xFF3F;    // End Of extensions

const TUint16 KMarkerSize   = 2; // Size of a marker - 2 bytes 
const TUint16 KMarkerLength = 2; // Size of a length - 2 bytes 
const TUint16 KMarkerMinLength = KMarkerSize + KMarkerLength;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TJ2kInfo;
class CJ2kImageInfo;
class TJ2kStreamReader;
class CJ2kEntropyDecoder;
class CJ2kImageWriter;
class CJ2kSynthesis;
class CJ2kTileInfo;

// CLASS DECLARATION

/**
 * CJp2kReadCodec JPEG2000 image read codec class.
 * Implement the ICL read codec interface.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class CJp2kReadCodec : public CImageProcessorReadCodec //lint !e768 Will be referenced by framework.      
{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CJp2kReadCodec* NewL( const TJ2kInfo& aJ2kInfo );
        
        /**
        * Destructor.
        */
        virtual ~CJp2kReadCodec();

    public: // New functions

        /**
        * Decoding the current tile-part.
        * @since 2.6
        * @return TFrameState: frame state after decoding the current tile-part.
        */
        TFrameState DecodeTileL();

        /**
        * Is codec in the decoding state.
        * @since 2.6
        * @return TBool: false if codec in the parsing state.
        */
        TBool IsDecodeTile() const;

    public: // Functions from base classes

        /**
        * From CImageReadCodec
        * @since 2.6
        * @param aFrameInfo: Frame information.
        * @param aFrameData: Frame data.
        */
        void InitFrameHeader( TFrameInfo& aFrameInfo, CFrameImageData& aFrameData );

        /**
        * From CImageReadCodec
        * @since 2.6
        * @param aData: Data to be processed.
        */
        TFrameState ProcessFrameHeaderL( TBufPtr8& aData );

        /**
        * From CImageReadCodec
        * @since 2.6
        * @param aFrameInfo: Frame information.
        * @param aFrameData: Frame data.
        * @param aDisableErrorDiffusion: Use error diffusion or not.
        * @param aDestination: Destination bitmap.
        * @param aDestinationMask: Destination bitmap mask.
        */
        void InitFrameL( TFrameInfo& aFrameInfo, CFrameImageData& aFrameData, 
                         TBool aDisableErrorDiffusion, CFbsBitmap& aDestination, 
                         CFbsBitmap *aDestinationMask );

        /**
        * From CImageReadCodec
        * @since 2.6
        * @param aSrc: Source data.
        */
        TFrameState ProcessFrameL( TBufPtr8& aSrc );

        /**
        * From CImageReadCodec
        * @since 2.6
        * @param aPosition: New position in data.
        * @param aLength: Number of bytes to read.
        */
        void GetNewDataPosition(TInt& aPosition, TInt& aLength);
       
    protected:  // New functions

    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CJp2kReadCodec( const TJ2kInfo& aJ2kInfo );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Verify and process Start of Codestream (SOC marker)
        * @since 2.6
        */
        TFrameState ReadSOCL();

        /**
        * Verify and process Image and Tile Size (SIZ marker)
        * @since 2.6
        */
        TFrameState ReadSIZL();

        /**
        * Verify and process Coding Style Default (COD marker)
        * @since 2.6
        * @param aMain: true for Main Header, false for Tile Part Header.
        */
        TFrameState ReadCODL( TBool aMain = ETrue );

        /**
        * Verify and process Coding Style Component (COC marker)
        * @since 2.6
        * @param aMain: true for Main Header, false for Tile Part Header.
        */
        TFrameState ReadCOCL( TBool aMain = ETrue );

        /**
        * Verify and process Quantization Default (QCD marker)
        * @since 2.6
        * @param aMain: true for Main Header, false for Tile Part Header.
        */
        TFrameState ReadQCDL( TBool aMain = ETrue );

        /**
        * Verify and process Quantization Component (QCC marker)
        * @since 2.6
        * @param aMain: true for Main Header, false for Tile Part Header.
        */
        TFrameState ReadQCCL( TBool aMain = ETrue );

        /**
        * Verify and process Region of Interest (RGN marker)
        * @since 2.6
        * @param aMain: true for Main Header, false for Tile Part Header.
        */
        TFrameState ReadRGNL( TBool aMain = ETrue );

        /**
        * Verify and process Progression Order Change (POC marker)
        * @since 2.6
        * @param aMain: true for Main Header, false for Tile Part Header.
        */
        TFrameState ReadPOCL( TBool aMain = ETrue );

        /**
        * Verify and process Packed Packet Headers, Main Header (PPM marker)
        * @since 2.6
        */
        TFrameState ReadPPML();

        /**
        * Verify and process Tile Part Lengths, Main Header (TLM marker)
        * @since 2.6
        */
        TFrameState ReadTLML();

        /**
        * Verify and process Packet Length, Main Header (PLM marker)
        * @since 2.6
        */
        TFrameState ReadPLML();

        /**
        * Verify and process Component Registration, Main Header (CRG marker)
        * @since 2.6
        */
        TFrameState ReadCRGL();

        /**
        * Verify and process Comment (COM marker)
        * @since 2.6
        * @param aMain: true for Main Header, false for Tile Part Header.
        */
        TFrameState ReadCOML( TBool aMain = ETrue );

        /**
        * Verify and process Start of Tile Part (SOT marker)
        * @since 2.6
        */
        TFrameState ReadSOTL();

        /**
        * Verify and process Start of Data (SOD marker)
        * @since 2.6
        */
        TFrameState ReadSODL();

        /**
        * Verify and process BitStream Data
        * @since 2.6
        */
        TFrameState ReadBitStreamL();

        /**
        * Verify and process Packed Packet Headers, Tile Part Header (PPT marker)
        * @since 2.6
        */
        TFrameState ReadPPTL();

        /**
        * Verify and process Packet Length, Tile Part Header (PLT marker)
        * @since 2.6
        */
        TFrameState ReadPLTL();

        /**
        * Ignore the content and advance the iterator to the next marker
        * @since 2.6
        */
        TFrameState ReadSkipTileL();

        /**
        * Update the current state according to the marker type
        * @since 2.6
        */
        TFrameState UpdateStateFromMarkerL();

        /**
        * Decode the tile and delete it after decoding
        * @since 2.6
        * @param aTile: a reference to the tile to decode.
        */
        void DecodeAndDeleteTileL( CJ2kTileInfo& aTile );

        /**
        * Convert metadata from file format to TImageDataBlock derived objects
        * @since 2.6
        */
        void ConvertImageDataL();

    public:     // Data
        
        // State machine represent the state when parsing of the JP2 codestream.        
        enum TFrameHeaderState
            {
            EStateInSOC,
            EStateInSIZ,
            EStateInCOD,
            EStateInCOC,
            EStateInQCD,
            EStateInQCC,
            EStateInRGN,
            EStateInPOC,
            EStateInPPM,
            EStateInTLM,
            EStateInPLM,
            EStateInCRG,
            EStateInCOM,
            EStateInSOT,
            EStateInPPT,
            EStateInPLT,
            EStateInSOD,
            EStateInBITSTREAM,
            EStateInEOC,
            EStateInSkipTile,
            EStateInUnknown
            };

    protected:  // Data

    private:    // Data

        // Decoding style.
        enum TDecoderStyle
        {
        ETileBasedDecoder,   // Tile-based decoding
        EBlockBasedDecoder,  // Block-based wavelet 
        EUnknownDecoder      // Use some logic to choose the style to use
        };

        // JP2 file format information
        const TJ2kInfo& iJ2kInfo;  
        
        // Frame information
        TFrameInfo *iFrame;        

        // Frame data, owned by framework
        CFrameImageData *iFrameData;

        // Index of last tile processed
        TUint16 iLastTileIndex;

        // Current state
        TFrameHeaderState iFHState;    
        
        // Indicate new tile to be processed
        TUint8 iUseNewTile;   

        // Indicate next tile to be processed
        TUint8 iUseNextTile;  

        // Tiles are in sequential order
        TUint8 iSequential;   

        // Reference to stream reader
        TJ2kStreamReader iReader;       

        // .jp2 image information
        CJ2kImageInfo *iImageInfo;    

        // To handle the underflow PPM
        TPPMMarker *iPreviousPPM;

        // To handle the underflow COM
        TCOMMarker *iPreviousCOM;  

        // The entropy decoder
        CJ2kEntropyDecoder *iEntropy;    

        // The image write
        CJ2kImageWriter *iImageWriter;

        // The synthesis
        CJ2kSynthesis *iSynthesis;  

        // The decoding mechanism used
        TDecoderStyle iStyleUsed;  

        // Indicate a progression bar is needed for single tile image
        TBool iProgressBar;

        // Indicate the codec state
        TBool iDecodeTile;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

#endif // __JP2KCODEC_H__

