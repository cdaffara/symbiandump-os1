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
* Description:  CJ2kSynthesis class used to perform inverse quantization
*                and inverse DWT
*
*/


#ifndef __JP2KSYNTHESIS_H__
#define __JP2KSYNTHESIS_H__

//  INCLUDES
#include <e32base.h>
#include "JP2KUtils.h"

// CONSTANTS

const TUint8 KFilterShift  = 14;  // Filter shift for the 9x7 filter taps
const TUint8 KWaveletShift = 4;   // Shift for the fractional part of wavelet coefficients

// The synthesis taps:
// 9x7 High-pass synthesis shifted by 14
const TPrecInt KFixedHigh9x70 = 9879;
const TPrecInt KFixedHigh9x71 = -4372;
const TPrecInt KFixedHigh9x72 = -1282;
const TPrecInt KFixedHigh9x73 = 276;
const TPrecInt KFixedHigh9x74 = 438;

// 9x7 Low-pass synthesis filter 
const TPrecInt KFixedLow9x70 = 18270;
const TPrecInt KFixedLow9x71 = 9687;
const TPrecInt KFixedLow9x72 = -943;
const TPrecInt KFixedLow9x73 = -1495;

const TUint8 KStepBits = 11;
const TUint8 KFilterExtension = 4;
const TPrecInt KMinimumPrecisionInteger = (TPrecInt)( -1 << ( KImplementationPrecision - 1 ) );
const TPrecInt KMaximumPrecisionInteger = (TPrecInt)( ~KMinimumPrecisionInteger );

// The coefficients we need to perform an inverse wavelet for 256 samples is at most (9x7 filter, first sample is odd) 132.
// Thus we allocate 264 sized block to perform the inverse wavelet transform.
const TInt32 KMaxBlockSupportSize = 264;
const TInt32 KWaveletBlockSize = 256;
const TInt32 KWaveletBlockMidPoint = 132;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CJ2kImageWriter;
class CJ2kEntropyDecoder;
class CJ2kImageInfo;
class CJ2kTileInfo;
class CJ2kSubband;
class CJ2kCodeBlock;
class CJ2kComponentInfo;

// CLASS DECLARATION


/**
*  CJ2kSynthesis class is used to decode a tile by
*  perform inverse quantization, inverse DWT, and
*  send the data to the component mixer.
*
*   JP2KCodec.dll
*   @since 2.6
*/
class CJ2kSynthesis : public CBase  
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CJ2kSynthesis();

        /**
        * Destructor.
        */
        virtual ~CJ2kSynthesis();

    public: // New functions
        
        /**
        * Decode a single tile
        * @since 2.6
        * @param aImageWriter: a reference to CJ2kImageWriter object.
        * @param aEntropyDecoder: a reference to CJ2kEntropyDecoder object.
        * @param aImageInfo: a reference to CJ2kImageInfo object.
        * @param aTile: a reference to CJ2kTileInfo object.
        */
        void DecodeTileL( CJ2kImageWriter& aImageWriter, CJ2kEntropyDecoder& aEntropyDecoder,
                          CJ2kImageInfo& aImageInfo, CJ2kTileInfo& aTile );

        /**
        * Decode a single tile with block-based wavelet transformation
        * @since 2.6
        * @param aImageWriter: a reference to CJ2kImageWriter object.
        * @param aEntropyDecoder: a reference to CJ2kEntropyDecoder object.
        * @param aImageInfo: a reference to CJ2kImageInfo object.
        * @param aTile: a reference to CJ2kTileInfo object.
        */
        void DecodeTileBlockL( CJ2kImageWriter& aImageWriter, CJ2kEntropyDecoder& aEntropyDecoder,
                               CJ2kImageInfo& aImageInfo, CJ2kTileInfo& aTile );

    public: // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * Perform one dimensional synthesis using reversible 5/3 filter
        * @since 2.6
        * @param aStartPos: the start position.
        * @param aEndPos: the end position.
        */
        void OneDimReversibleFilter( TInt32 aStartPos, TInt32 aEndPos );

        /**
        * Performs one dimensional symmetric extension of the line of pixels
        * @since 2.6
        * @param aStartPos: the start position.
        * @param aEndPos: the end position.
        */
        void PerformExtension( TInt32 aStartPos, TInt32 aEndPos );    

        /**
        * Perform one dimensional synthesis using irreversible 9/7 filter
        * @since 2.6
        * @param aStartPos: the start position.
        * @param aEndPos: the end position.
        * @param aLevel: the resolution level.
        * @param aVertical: flag for the vertical filtering.
        */
        void OneDimIrrevFilter( TInt32 aStartPos, TInt32 aEndPos, TUint8 aLevel, TUint8 aVertical );

        /**
        * Perform one dimensional filtering
        * @since 2.6
        * @param aStartPos: the start position.
        * @param aEndPos: the end position.
        * @param aLevel: the resolution level.
        * @param aVertical: flag for the vertical filtering.
        */
        void OneDimFiltering( TInt32 aStartPos, TInt32 aEndPos, TUint8 aLevel, TUint8 aVertical );

        /**
        * Perform one dimensional horizontal filtering
        * @since 2.6
        * @param aImage: the 2-D image.
        * @param aRow: the index of row.
        * @param aXtcSiz: the length of row.
        * @param aSubband: pointer to CJ2kSubband object.
        */
        void HorizontalFilter( TPrecInt** aImage, TInt32 aRow, TUint32 aXtcSiz, CJ2kSubband* aSubband );

        /**
        * Perform one dimensional vertical filtering
        * @since 2.6
        * @param aImage: the 2-D image.
        * @param aColumn: the index of column.
        * @param aYtcSiz: the length of column.
        * @param aSubband: pointer to CJ2kSubband object.
        */
        void VerticalFilter( TPrecInt** aImage, TInt32 aColumn, TUint32 aYtcSiz, CJ2kSubband* aSubband );

        /**
        * Perform two dimensional inverse wavelet transformation
        * @since 2.6
        * @param aImage: the 2-D image.
        * @param aXtcSiz: the length of row.
        * @param aYtcSiz: the length of column.
        * @param aSubband: pointer to CJ2kSubband object.
        */
        void TwoDimFiltering( TPrecInt** aImage, TInt32 aXtcSiz, TInt32 aYtcSiz, CJ2kSubband* aSubband );

        /**
        * Perform a full inverse wavelet transformation
        * @since 2.6
        * @param aImage: the 2-D image.
        * @param aSubband: pointer to CJ2kSubband object.
        */
        void FullWaveletInverse( TPrecInt** aImage, CJ2kSubband* aSubband );

        /**
        * Compute the quantization parameters for a particular subband
        * in the component
        * @since 2.6
        * @param aComponentInfo: a reference to CJ2kComponentInfo object.
        * @param aBandIndex: the band index.
        * @param aBandGain: the band gain.
        * @param aBitdepth: the bit depth.
        */
        void ComputeQuantizationParameters( const CJ2kComponentInfo& aComponentInfo, TInt16 aBandIndex, 
                                            TUint8 aBandGain, TUint8 aBitDepth );

        /**
        * Apply inverse quantization and ROI shifting on the decoded
        * codeblock and copy to the image writer
        * @since 2.6
        * @param aEntropyDecoder: a reference to CJ2kEntropyDecoder object.
        * @param aImageBlock: the 2-D image block.
        * @param aSubband: a reference to CJ2kSubband object.
        * @param aCodeblock: a reference to CJ2kCodeBlock object.
        * @param aQuantizationStyle: the quantization style.
        */
        void CopyDataToImage( CJ2kEntropyDecoder& aEntropyDecoder,TPrecInt**  aImageBlock, 
                              CJ2kSubband& aSubband, CJ2kCodeBlock& aCodeblock, TUint8 aQuantizationStyle );

        /**
        * Allocate internal buffer based on the requested size
        * @since 2.6
        * @param aSize: the size of buffer to allocate.
        */
        void AllocBufferL( TInt32 aSize );

        /**
        * Perform one dimensional horizontal filtering (block-based)
        * @since 2.6
        * @param aImage: the 2-D image.
        * @param aRow: the index of row.
        * @param aXtcSiz: the length of row.
        * @param aSubband: pointer to CJ2kSubband object.
        * @param aXOffset: the offset in X position.
        * @param aCurrentLevel: the current resolution level.
        */
        void HorizontalBlockFilter( TPrecInt** aImage, TInt32 aRow, TUint32 aXtcSiz, CJ2kSubband* aSubband,
                                    TInt32 aXOffset, TUint8 aCurrentLevel );

        /**
        * Perform one dimensional vertical filtering (block-based)
        * @since 2.6
        * @param aImage: the 2-D image.
        * @param aColumn: the index of column.
        * @param aYtcSiz: the length of column.
        * @param aSubband: pointer to CJ2kSubband object.
        * @param aYOffset: the offset in Y position.
        * @param aCurrentLevel: the current resolution level.
        */
        void VerticalBlockFilter( TPrecInt** aImage, TInt32 aColumn, TUint32 aYtcSiz, CJ2kSubband* aSubband,
                                  TInt32 aYOffset, TUint8 aCurrentLevel );

        /**
        * Perform two dimensional inverse wavelet transformation (block-based)
        * @since 2.6
        * @param aImage: the 2-D image.
        * @param aXtcSiz: the length of row.
        * @param aYtcSiz: the length of column.
        * @param aSubband: pointer to CJ2kSubband object.
        * @param aOffset: the offset in both position.
        * @param aCurrentLevel: the current resolution level.
        */
        void TwoDimBlockFiltering( TPrecInt** aImage, TSize aRegion, CJ2kSubband* aSubband,
                                   TPoint aOffset, TUint8 aCurrentLevel );

        /**
        * Perform a full inverse wavelet transformation (block-based)
        * @since 2.6
        * @param aImage: the 2-D image.
        * @param aSubband: pointer to CJ2kSubband object.
        * @param aOffset: the offset in both position.
        * @param aRegion; the region.
        * @param aCurrentLevel: the current resolution level.
        */
        void SingleLevelWaveletInverse( TPrecInt** aImage, CJ2kSubband* aSubband, TPoint aOffset, 
                                        TSize aRegion, TUint8 aCurrentLevel );

        /**
        * Apply inverse quantization and ROI shifting on the decoded
        * codeblock and copy to the image writer
        * @since 2.6
        * @param aEntropyDecoder: a reference to CJ2kEntropyDecoder object.
        * @param aImageBlock: the 2-D image block.
        * @param aSubband: a reference to CJ2kSubband object.
        * @param aQuantizationStyle: the quantization style.
        * @param aStartRowCblk: the starting row of codeblock.
        * @param aStartColCblk: the starting column of codeblock.
        * @param aStartRowImage: the starting row of image.
        * @param aStartColImage: the starting column of image.
        * @param aCblkHeight: the codeblock height.
        * @param aCblkWidth: the codeblock width.
        */
        void CopyDataToBlock( CJ2kEntropyDecoder& aEntropyDecoder,TPrecInt** aImageBlock, CJ2kSubband& aSubband,
                              TUint8 aQuantizationStyle, TInt32 aStartRowCblk, TInt32 aStartColCblk,
                              TInt32 aStartRowImage, TInt32 aStartColImage, TInt32 aCblkHeight,TInt32 aCblkWidth );

        /**
        * Fill a block in image writer with zeros (corresponding to an 
        * empty block)
        * @since 2.6
        * @param aImageBlock: the 2-D image block.
        * @param aSubband: a reference to CJ2kSubband object.
        * @param aStartRowImage: the starting row of image.
        * @param aStartColImage: the starting column of image.
        * @param aCblkHeight: the codeblock height.
        * @param aCblkWidth: the codeblock width.
        */
        void FillDataWithZeros( TPrecInt** aImageBlock, CJ2kSubband& aSubband, TInt32 aStartRowImage,
                                TInt32 aStartColImage, TInt32 aCblkHeight, TInt32 aCblkWidth );

    public:     // Data
    
    protected:  // Data

    private:    // Data
        
        // Number of filters tap for low-pass and high-pass
        enum TFilterTap
            {
            ELowTap = 4,
            EHighTap
            };

        // Is transformation reversible?
        TUint8 iReversible;         

        // Magnitude bits for this band
        TUint8 iMagnitudeBitsHere;  
        
        // Resolution level for DWT
        TInt16 iWaveletLevels;      

        // Amount of shift for ROI
        TInt32 iROIShift;

        // Downshift for ROI data samples
        TInt32 iROIDataShift;    

        // Downshift for data samples
        TInt32 iDataShift;

        // Quantization step value 
        TInt32 iStepValue;       

        // Quantization step's exponent
        TInt32 iStepExponent;    

        // Internal buffer size
        TInt32 iIOBufferSize;    

        // Internal input buffer
        TInt32* iInputBuffer;     

        // Internal output buffer
        TInt32* iOutputBuffer;    

        // Filter taps for irreversible low-pass filtering
        TPrecInt iTapsLow[ELowTap];    

        // Filter taps for irreversible high-pass filtering
        TPrecInt iTapsHigh[EHighTap];  

    public:     // Friend classes
 
    protected:  // Friend classes
    
    private:    // Friend classes

    };

#endif // __JP2KSYNTHESIS_H__
