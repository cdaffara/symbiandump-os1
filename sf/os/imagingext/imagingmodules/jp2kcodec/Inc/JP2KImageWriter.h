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
* Description:  CJ2kImageWriter class used to perform inverse transformation and
*                writing decoded image data to bitmap.
*
*/


#ifndef __JP2KIMAGEWRITER_H__
#define __JP2KIMAGEWRITER_H__

//  INCLUDES
#include <icl/imageprocessor.h>

// CONSTANTS
const TUint8  KByteBits      = 8; // Number of bits in byte
const TUint8  KFractionBits  = 12;
const TUint16 KOffset        = (1 << ( KFractionBits - 1 ) );

// Inverse color transform shifted(by 12 bits) coefficients
const TInt32 KIctCoefficient11 = 5743;
const TInt32 KIctCoefficient21 = -1409;
const TInt32 KIctCoefficient22 = -2925;
const TInt32 KIctCoefficient31 = 7258;

// The ICC profile Tag definitions
const TUint8  KICCSkipBytes   = 128;        // Bytes to skip before reading the ICC profile
const TUint32 KRedMatrixTag   = 0x7258595A; // Tag indicating the place (in ICC profile) of conversion matrix coefficients for red
const TUint32 KGreenMatrixTag = 0x6758595A; // Tag indicating the place of conversion matrix coefficients for green
const TUint32 KBlueMatrixTag  = 0x6258595A; // Tag indicating the place of conversion matrix coefficients for blue 
const TUint32 KRedTrcTag      = 0x72545243; // Tag indicating the place of Tone Reproduction Curve value for red 
const TUint32 KGreenTrcTag    = 0x67545243; // Tag indicating the place of Tone Reproduction Curve value for green
const TUint32 KBlueTrcTag     = 0x62545243; // Tag indicating the place of Tone Reproduction Curve value for blue
const TUint32 KGrayTrcTag     = 0x6B545243; // Tag indicating the place of Tone Reproduction Curve value for gray
const TUint32 KPixelsBlock    = 256;        // The max number of pixel in a block

// Define the XYZ to linear sRGB matrix coefficients
const TReal KSRGB00 =  3.1337;
const TReal KSRGB01 = -1.6173;
const TReal KSRGB02 = -0.4907;
const TReal KSRGB10 = -0.9785;
const TReal KSRGB11 =  1.9162;
const TReal KSRGB12 =  0.0334;
const TReal KSRGB20 =  0.0720;
const TReal KSRGB21 = -0.2290;
const TReal KSRGB22 =  1.4056;
const TReal KSRGBMax = (TReal)4095;
const TInt32 KSRGBMaxInt = (TInt32)4095;
const TInt32 KSRGBShift         = 4;                                            // The shift for the sRGB maximum value
const TInt32 KSRGBMaxIntShifted = (TInt32)( ( 1 << KSRGBShift ) * (TReal)4095 );// The sRGB maximum value shifted by 4
const TInt32 KTRCShift           = 12;                                          // The shift used for TRC LUT values for integer computation
const TInt32 KTRCShiftMultiplier = (TInt32)( 1 << KTRCShift );                  // The shift used for TRC LUT values for integer computation
const TUint32 KICCDownshift     = KTRCShift + KSRGBShift;                       // The total shift used for computing the ICC conversion with integers (12 for TRC LUT and 4 for the matrix coefficients)

const TReal KDivisor = (TReal)65536;
const TReal KGamma   = (TReal)256;

// Define values for performing the linear RGB -> sRGB (non linear) conversion
const TReal KSRGB_CUTOFF     = 0.0031308;
const TReal KSRGB_SLOPE      = 12.92;
const TReal KSRGB_EXPONENT   = 0.4166666; // ( 1.0 / 2.4 );
const TReal KSRGB_MULTIPLIER = 1.055;
const TReal KSRGB_SUBTRACT   = 0.055;

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CJ2kImageWriter;
class CJ2kImageInfo;
class CJ2kTileInfo;
class TJ2kInfo;

// CLASS DECLARATION

/**
 * CJ2kWriterComponentInfo class is used to store 
 * information about tiles' coordinates for outputting
 * samples and also to store the output data.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class CJ2kWriterComponentInfo : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CJ2kWriterComponentInfo();

    public: // New functions
        
        /**
        * Allocate the 2-D data array with a size
        * @since 2.6
        * @param aSize: the size to allocate.
        */
        void AllocDataL( const TSize& aSize );

        /**
        * Free the 2-D data array
        * @since 2.6
        */
        void FreeData();

        /**
        * Get the 2-D data arry
        * @since 2.6
        * @return TPrecInt**: the pointer to pointer.
        */
        TPrecInt** Data();

        /**
        * Get the starting point of a tile
        * @since 2.6
        * @param aTileIndex: the tile index.
        * @return TPoint&: a reference to the start point of the tile.
        */
        TPoint& TileStartAt( TUint16 aTileIndex );

        /**
        * Update the starting point of next tile
        * @since 2.6
        * @param aTileIndex: the tile index.
        * @param aSize: the size of the tile.
        * @param aImageInfo: a reference to CJ2kImageInfo object.
        */
        void UpdateNextTileStartAtL(TUint16 aTileIndex, const TSize& aSize, CJ2kImageInfo& aImageInfo );

    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data

        // Internal 2-D data array
        TPrecInt **iData;          
        
        // Start position of each tile
        RArray<TPoint> iTileStartList; 

    public:     // Friend classes
        friend class CJ2kImageWriter;

    protected:  // Friend classes

    private:    // Friend classes
    };

/**
 * CJ2kImageWriter class has methods to perform 
 * color, bitdepth and ICC profile conversions to
 * produce 8-bit output data.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class CJ2kImageWriter : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CJ2kImageWriter* NewL( CImageProcessor* aImageProcessor, CJ2kImageInfo& aImageInfo,
                                      TJ2kInfo& aJ2kInfo );
        
        /**
        * Destructor.
        */
        virtual ~CJ2kImageWriter();

    public: // New functions
        
        /**
        * Get the component of the image writer
        * @since 2.6
        * @param aIndex: the component index.
        * @return CJ2kWriterComponentInfo&: a reference to CJ2kWriterComponentInfo.
        */
        const CJ2kWriterComponentInfo& WriterComponentAt( TUint16 aIndex ) const;
         
        /**
        * Output the image related to the component of the tile
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponentIndex: the component index.
        */
        void OutputImageL( CJ2kTileInfo& aTile, TUint16 aComponentIndex );

        /**
        * Output the image related to the component of the tile
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponentIndex: the component index.
        * @param aSize: the size to output.
        */
        void OutputImageL( CJ2kTileInfo& aTile, TUint16 aComponentIndex, 
                           const TSize& aSize );
        /**
        * Output the image related to the component of the tile
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponentIndex: the component index.
        * @param aBlockXCoord: the block X coordinate.
        * @param aBlockYCoord: the block Y coordinate.
        * @param aFirstCompSize: the size of first component.
        * @param aThisCompSize: the size of this component.
        */
        void OutputBlockL( CJ2kTileInfo& aTile, TUint16 aComponentIndex, 
                           TInt32 aBlockXCoord, TInt32 aBlockYCoord, 
                           TSize aFirstCompSize, TSize aThisCompSize );
        /**
        * Set the image processor of the image write
        * @since 2.6
        * @param aImageProcessor: a pointer to CImageProcessor object.
        */
        void SetNewImageProcessor( CImageProcessor* aImageProcessor );

        /**
        * Get the single output file
        * @since 2.6
        * @return TUint8: true if single file output.
        */
        TUint8 SingleFileOutput() const;

        /**
        * Get the EnumCS
        * @since 2.6
        * @return TUint8: the CS code.
        */
        TUint8 CSCode() const;

    public: // Functions from base classes
     
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CJ2kImageWriter( CImageProcessor* aImageProcessor, CJ2kImageInfo& aImageInfo, TJ2kInfo& aJ2kInfo );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Perform the inverse reversible color transformation
        * @since 2.6
        * @param aSize: the size.
        */
        void PerformInverseRCT( const TSize& aSize );

        /**
        * Perform the inverse irreversible color transformation
        * @since 2.6
        * @param aSize: the size.
        */
        void PerformInverseICT( const TSize& aSize );

        /**
        * Inverse irreversible color transformation
        * @since 2.6
        * @param aY: the Y value.
        * @param aU: the U value.
        * @param aV: the V value.
        * @param aR: a reference to red value.
        * @param aG: a reference to green value.
        * @param aB: a reference to blue value.
        */
        void InverseICTTransform( TPrecInt aY, TPrecInt aU, TPrecInt aV,
                                  TPrecInt& aR, TPrecInt& aG, TPrecInt& aB );   
     
        /**
        * Inverse irreversible color transformation
        * @since 2.6
        * @param aY1: the Y value of the even sample.
        * @param aY2: the Y value of the odd sample.
        * @param aU: the U value.
        * @param aV: the V value.
        * @param aR1: a reference to even red value.
        * @param aG1: a reference to even green value.
        * @param aB1: a reference to even blue value.
        * @param aR2: a reference to odd red value.
        * @param aG2: a reference to odd green value.
        * @param aB2: a reference to odd blue value.
        */
        void InverseICTTransform( TPrecInt aY1, TPrecInt aY2, TPrecInt aU, TPrecInt aV,
                                  TPrecInt& aR1, TPrecInt& aG1, TPrecInt& aB1,
                                  TPrecInt& aR2, TPrecInt& aG2, TPrecInt& aB2 );

        /**
        * Inverse irreversible color transformation
        * @since 2.6
        * @param aY1: the Y value of the even sample on even row.
        * @param aY2: the Y value of the odd sample on even row.
        * @param aY3: the Y value of the even sample on odd row.
        * @param aY4: the Y value of the odd sample on odd row.
        * @param aU: the U value.
        * @param aV: the V value.
        * @param aR1: a reference to even red value on even row.
        * @param aG1: a reference to even green value on even row.
        * @param aB1: a reference to even blue value on even row.
        * @param aR2: a reference to odd red value on even row.
        * @param aG2: a reference to odd green value on even row.
        * @param aB2: a reference to odd blue value on even row.
        * @param aR3: a reference to even red value on odd row.
        * @param aG3: a reference to even green value on odd row.
        * @param aB3: a reference to even blue value on odd row.
        * @param aR4: a reference to odd red value on odd row.
        * @param aG4: a reference to odd green value on odd row.
        * @param aB4: a reference to odd blue value on odd row.
        */
        void InverseICTTransform( TPrecInt aY1, TPrecInt aY2, TPrecInt aY3, TPrecInt aY4, 
                                  TPrecInt aU, TPrecInt aV,
                                  TPrecInt& aR1, TPrecInt& aG1, TPrecInt& aB1,
                                  TPrecInt& aR2, TPrecInt& aG2, TPrecInt& aB2,
                                  TPrecInt& aR3, TPrecInt& aG3, TPrecInt& aB3,
                                  TPrecInt& aR4, TPrecInt& aG4, TPrecInt& aB4 );

        /**
        * Initialize the ICC profile from JP2 file format (iJ2kInfo)
        * @since 2.6
        */
        void InitializeICCProfileL();

        /**
        * Initialize the output parameters
        * @since 2.6
        */
        void InitializeOutputParametersL();

        /**
        * Perform the XYZ to sRGB conversion using ICC profile.
        * @since 2.6
        * @param aX: sample x to be converted.
        * @param aY: sample y to be converted.
        * @param aZ: sample z to be converted.
        * @param aR: a reference to red value.
        * @param aG: a reference to green value.
        * @param aB: a reference to blue value.
        */
        void DoICCConversion( TInt32 aX, TInt32 aY, TInt32 aZ,
                              TPrecInt& aR, TPrecInt& aG, TPrecInt& aB );
        /**
        * Map data less than 8 bits to 8 bits data
        * @since 2.6
        * @param aComponent: a reference to CJ2kWriterComponentInfo object.
        * @param aSize: the size of component.
        * @param aBitDepth: the bit depth.
        */
        void MapToEightBits( CJ2kWriterComponentInfo& aComponent,
                             const TSize& aSize, TUint16 aBitDepth );

        /**
        * Map data using the component mapping box from JP2 file format
        * @since 2.6
        * @param aNumCSComp: number of CS component.
        * @param aReducedLevels: the reduced resolution level.
        * @param aSize: the size to output.
        * @param aTile: a reference to CJ2kTileInfo object.
        */
        void MapComponentsL( TUint16 aNumCSComp,TUint16 aReducedLevels,
                             const TSize&  aSize, CJ2kTileInfo& aTile );

        /**
        * Write the component to the single output file
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aCompIndex: the component index.
        * @param aSize: the size of component.
        * @param aBitDepth: the bit depth.
        */
        void WriteOutputFileL( CJ2kTileInfo& aTile, TUint16 aCompIndex, const TSize& aSize,
                              TUint16 aBitDepth );

        /**
        * Write all components of the tile to the single output file
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aSize: the size to output.
        */
        void CombineOutputFileL( CJ2kTileInfo& aTile, const TSize& aSize );

        /**
        * Write out a color pixel
        * @since 2.6
        * @param aR: the red value.
        * @param aG: the green value.
        * @param aB: the blue value.
        */
        void WritePixel( TUint8 aR, TUint8 aG, TUint8 aB );

        /**
        * Write out a grayscale pixel
        * @since 2.6
        * @param aGray256: the gray scale value.
        */
        void WritePixel( TUint8 aGray256 );

        /**
        * Set the position of the pixel
        * @since 2.6
        * @param aPosition: the position of next pixel.
        */
        void SetPixelPos( const TPoint& aPosition );

        /**
        * Set the position of the pixel
        * @since 2.6
        * @param aX: the X position of next pixel.
        * @param aY: the Y position of next pixel.
        */
        void SetPixelPos( const TInt aX, const TInt aY );

    public:     // Data
    
    protected:  // Data

    private:    // Data
        
        // File type
        enum TFileType
            {
            KOurRGB,    //lint !e769    keep here for later use
            KRGB,
            KYUV422,
            KYUV420
            };

        // Number of components
        TUint16 iNumComponents;                           

        // File type
        TFileType iFileType;                                

        // Is single file output?
        TUint8 iSingleFileOutput;                        

        // Is ICC Profile used?
        TUint8 iICCProfile;                              

        // An array of component infos
        RPointerArray<CJ2kWriterComponentInfo> iComponents; 

        // Reference to the image processor
        CImageProcessor* iImageProcessor; 

        // Reference to the image info
        CJ2kImageInfo& iImageInfo;  
        
        // Reference to the file format into
        TJ2kInfo& iJ2kInfo;        

        // ** For ICC Profile **

        // Conversion matrix to perform the RGBin->XYZ->sRGB conversion
        TInt32 iMatrix[9];      

        // Linear sRGB lookup table for sRGB(lin)->sRGB(non-lin) conversion
        HBufC8 *iLinearsRGBLut;  

        // Tone reproduction curve LUT for linearizing Gray input sample
        TInt32 *iGrayTRCLut;

        // Tone reproduction curve LUT for linearizing Red input sample
        TInt32 *iRedTRCLut;

        // Tone reproduction curve LUT for linearizing Green input sample
        TInt32 *iGreenTRCLut;    
     
        // Tone reproduction curve LUT for linearizing Blue input sample
        TInt32 *iBlueTRCLut;

        // Block containing grayscale pixels
        TUint32 *iMonoPixelBlock;

        // Block containing color pixels
        TRgb    *iColorPixelBlock;

    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes

    };

#endif // __JP2KIMAGEWRITER_H__
