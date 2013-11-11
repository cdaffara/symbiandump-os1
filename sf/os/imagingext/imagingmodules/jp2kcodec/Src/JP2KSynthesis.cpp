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
* Description:  CJ2kSynthesis class used to perform inverse quantization and
*                inverse DWT.
*
*/


// INCLUDE FILES
#include "JP2KTileInfo.h"
#include "JP2KImageInfo.h"
#include "JP2KImageWriter.h"
#include "JP2KEntropyDecoder.h"
#include "JP2KCodeBlock.h"
#include "JP2KPacket.h"
#include "JP2KSubband.h"
#include "JP2KComponentInfo.h"
#include "JP2KSynthesis.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kSynthesis::CJ2kSynthesis
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJ2kSynthesis::CJ2kSynthesis() 
    {
    // Set up the filter taps for irreversible filter
    iTapsLow[0] = KFixedLow9x70;
    iTapsLow[1] = KFixedLow9x71;
    iTapsLow[2] = KFixedLow9x72;
    iTapsLow[3] = KFixedLow9x73;
  
    iTapsHigh[0] = KFixedHigh9x70;
    iTapsHigh[1] = KFixedHigh9x71;
    iTapsHigh[2] = KFixedHigh9x72;
    iTapsHigh[3] = KFixedHigh9x73;
    iTapsHigh[4] = KFixedHigh9x74;
    }

// Destructor
CJ2kSynthesis::~CJ2kSynthesis()
    {
    if ( iInputBuffer )
        {
        iInputBuffer  -= ( KFilterExtension + 1 );
        User::Free( iInputBuffer );
        iInputBuffer = 0;
        }
    if ( iOutputBuffer )
        {
        iOutputBuffer -= ( KFilterExtension + 1 );
        User::Free( iOutputBuffer );
        iOutputBuffer = 0;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::DecodeTileL
// Decode a single tile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::DecodeTileL( CJ2kImageWriter& aImageWriter, 
                                 CJ2kEntropyDecoder& aEntropyDecoder,
                                 CJ2kImageInfo& aImageInfo,
                                 CJ2kTileInfo& aTile )
    {
    if ( aImageInfo.Crop() )
        {
        // If this tile doesn't belong to the crop area, continue
        if ( !aImageInfo.TileMaskAt( aTile.SotMarker().iIsot ) )
            {
            return;
            }
        }

    CJ2kComponentInfo* componentInfo = 0;
    TInt32 maxSize = 0;
    TUint16 compIndex = 0;
    TUint16 numOfComponents = aImageInfo.NumOfComponents();

    // Find the largest width or height
    for ( compIndex = 0; compIndex < numOfComponents; ++compIndex )
        {
        componentInfo = CONST_CAST( CJ2kComponentInfo*, &aTile.ComponentAt( compIndex ) );
        maxSize = Max( maxSize, Max( componentInfo->ComponentCanvas().Height(), 
                                     componentInfo->ComponentCanvas().Width() ) );
        }

    maxSize += 2 * ( KFilterExtension + 1 );
    AllocBufferL( maxSize );

    TSize   subbandSize( 0, 0 );
    TUint16 bandIndex = 0;
    TUint16 blockIndex = 0;

    CJ2kSubband*   subband = 0;
    CJ2kPacket*    packet = 0;
    CJ2kCodeBlock* codeblock = 0;

    TUint8  quantStyle = 0;
    TUint8  bitDepth = 0;
    TUint8  cblkStyle = 0;
    TUint8  levelIndex = 0;
    TUint32 packetIndex = 0;

    CJ2kWriterComponentInfo* component = 0;
    RPointerArray<CJ2kPacket>* packetList;

    aEntropyDecoder.SetNewSizeL( aImageInfo.MaxBlockSize() );

    // For each component in the tile
    for ( compIndex = 0; compIndex < aImageInfo.NumOfComponents(); ++compIndex )
        {
        componentInfo = CONST_CAST( CJ2kComponentInfo*, &aTile.ComponentAt( compIndex ) );

        // Skip the component when height or width is 0
        if ( componentInfo->ComponentCanvas().Height() == 0 ||
             componentInfo->ComponentCanvas().Width() == 0  )
            {
            continue;   //lint !e960    Continue is OK.
            }

        // Check for component truncation
        if ( aImageInfo.ComponentDrop() )
            {
            if ( aImageWriter.SingleFileOutput() )
                {
                if ( compIndex == ( aImageInfo.ComponentDrop() - 1 ) )
                    {
                    numOfComponents = compIndex;
                    }
                else
                    {
                    continue;   //lint !e960    Continue is OK.
                    }
                }
            else
                {
                if ( compIndex != ( aImageInfo.ComponentDrop() - 1 ) )
                    {
                    continue;   //lint !e960    Continue is OK.
                    }
                }
            }

        // Get the resolution level for this component
        iWaveletLevels = (TInt16)( componentInfo->Levels() - aImageInfo.LevelDrop() );

        TInt32 stepSize = 1;
        if ( iWaveletLevels < 0 )
            {
            TInt32 i;

            // Compute the output step size, the stepSize indicates how much more 
            // resolution has to be dropped if the image didn't have enough wavelet
            // levels to begin with. One indicates no extra resolution drop (write
            // each sample) and for each extra drop skip half of the samples, i.e.
            // stepSize is 2^extraLevels in case extra drop is needed.
        
            // Adjust the tile starting points and the stepSize
            for ( i = 0; i < (-iWaveletLevels); i++ )
                {
                // Double the step size for every extra level dropped.
                stepSize *= 2;
                }
        
            iWaveletLevels = 0;
            }

        // Get the top subband ( original image )
        subband = CONST_CAST( CJ2kSubband*, componentInfo->SubbandAt( (TUint8)iWaveletLevels ) );

        if ( subband->SubbandResLevel() != 0 )
            {
            subband = subband->Parent();
            }

        subbandSize = subband->SubbandCanvasSize();

        // Skip the component when subband's height or width is 0
        if ( subbandSize.iWidth == 0 || subbandSize.iHeight == 0 )
            {
            continue;   //lint !e960    Continue is OK.
            }

        component = CONST_CAST( CJ2kWriterComponentInfo*, &aImageWriter.WriterComponentAt( compIndex ) );

        // Have to allocate memory for each component in the image writer
        component->AllocDataL( subbandSize );

        bitDepth = aImageInfo.DepthOfComponent( compIndex );
        
        // Get the right number of levels and transform type 
        // reversible == 1 for 5x3 filter and == 0 for 9x7 filter
        iReversible = componentInfo->IsReversible();
        iROIShift   = componentInfo->RoiShift();

        quantStyle  = componentInfo->QuantizationStyle();
        cblkStyle   = componentInfo->CodeBlockStyle();

        // For each resolution level in the component
        for ( levelIndex = 0; levelIndex <= iWaveletLevels; levelIndex++ )
            {
            subband = CONST_CAST( CJ2kSubband*, componentInfo->SubbandAt( levelIndex ) );

            // For each subband in the resolution level
            do    
                {
                bandIndex = ( quantStyle == 1 ) ? (TUint16)0 : subband->SubbandTreeIndex();

                // Get the right magnitude bits for this band
                iMagnitudeBitsHere = componentInfo->MagnitudeBits( bandIndex );
        
                // Compute the Quantization parameters here, so we don't repeat that for every codeblock/precinct
                ComputeQuantizationParameters( *componentInfo, bandIndex, subband->SubbandGain(), bitDepth );

                // Set the lookup table for entropy decoder
                aEntropyDecoder.SetCurrentZCLUT( (TUint8)( subband->SubbandType() ) );

                packetList = CONST_CAST( RPointerArray<CJ2kPacket>*, &subband->PacketList() );    //lint !e665 the first parameter cannot be parenthesized here

                // For each packet in the subband
                for ( packetIndex = 0; packetIndex < componentInfo->NumOfPackets( levelIndex ); ++packetIndex )
                    {
                    packet = ( *packetList )[packetIndex];

                    // For each codeblock in the packet
                    for ( blockIndex = 0; blockIndex < packet->NumOfBlocks(); ++blockIndex )
                        {
                        codeblock = CONST_CAST( CJ2kCodeBlock*, &packet->CodeBlockAt( blockIndex ) );

                        // There is coded data in the codeblock
                        if ( codeblock->DataLength() )
                            {
                            // Decode the codeblock
                            aEntropyDecoder.DecodeCodeblock( *codeblock, cblkStyle, (TUint8)( iMagnitudeBitsHere + iROIShift ) );

                            // Copy data from the decoded codeblock for inverse quantization and ROI shifting
                            CopyDataToImage( aEntropyDecoder, component->Data(), *subband, *codeblock, quantStyle );
                            }
                        } // end of each codeblock
                    }  // end of each packet

                // Get the sibling subband
                subband = subband->NextSubbandRaster();

                } while ( subband ); // end of each subband in the resolution level

            }  // end of each resolution level in the component
    
        // Point to the LL-band before calling inverse wavelet transform
        subband = CONST_CAST( CJ2kSubband*, componentInfo->SubbandAt( 0 ) );

        // Perform a full inverse wavelet transformation
        FullWaveletInverse( component->Data(), subband );


        // Check whether extra downsampling is needed
        if(stepSize > 1)
            {
            TInt32 i,j;
            TInt32 iStep,jStep;

            // If the stepSize is larger than one it means that we have to downsample
            // the output. This is because there were not enough wavelet levels to do
            // the resolution dropping for this component.

            // The reason why downsampling is done here and not in the ImageWriter is
            // that different components might have different number of wavlet levels
            // and thus it is easier to downsample the components here (so that we can 
            // just write out the samples normally in ImageWriter.
            for(i = 0,iStep = 0; iStep < subbandSize.iHeight; i++,iStep += stepSize)
                {
                for(j = 0,jStep = 0; jStep < subbandSize.iWidth; j++,jStep += stepSize)
                    {
                    // Downsample the component so that downsampled image is in the
                    // upper left-hand corner.
                    component->Data()[i][j] = component->Data()[iStep][jStep];
                    }
                }
            }



        if ( numOfComponents >= 3 ) 
            {
            if ( aTile.ColorTransformation() || 
                ( aImageWriter.CSCode() == 18 || aImageWriter.CSCode() == 16 ) )
                {
                // Wait til we finish decoding all components before writing out the image
                if ( compIndex < 2 )
                    {
                    continue;   //lint !e960    Continue is OK.
                    }
                }
            else if ( aImageWriter.CSCode() == 0 )
                {
                // This case is also valid when no CSCode is defined, e.g. no file format is present
                if( numOfComponents <= 3 )
                    {
                    if ( compIndex < 2 )
                        {
                        continue;   //lint !e960    Continue is OK.
                        }
                    }
                else
                    {
                    // Proceed to outputImageL
                    }
                }
            }   //lint !e961    no else is needed here at the end of if...else if

        // Write out the tile image
        aImageWriter.OutputImageL( aTile, compIndex );

        }   // end of each component in the tile
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::DecodeTileBlockL
// Decode a single tile with lower memory using 256x256 blocks 
// for the inverse wavelet transform
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::DecodeTileBlockL( CJ2kImageWriter& aImageWriter, 
                                      CJ2kEntropyDecoder& aEntropyDecoder,
                                      CJ2kImageInfo& aImageInfo,
                                      CJ2kTileInfo& aTile )
    {
    if ( aImageInfo.Crop() )
        {
        // If this tile doesn't belong to the crop area, continue
        if ( !aImageInfo.TileMaskAt( aTile.SotMarker().iIsot ) )
            {
            return;
            }
        }

    CJ2kComponentInfo* componentInfo = 0;
    TInt32  maxSize = 0;
    TUint16 compIndex = 0;
    TUint16 numOfComponents = aImageInfo.NumOfComponents();

    // Find the largest width or height
    for ( compIndex = 0; compIndex < numOfComponents; ++compIndex )
        {
        componentInfo = CONST_CAST( CJ2kComponentInfo*, &aTile.ComponentAt( compIndex ) );
        maxSize = Max( maxSize, Max( componentInfo->ComponentCanvas().Height(), 
                                     componentInfo->ComponentCanvas().Width() ) );
        }

    maxSize = KMaxBlockSupportSize;

    maxSize += 2 * ( KFilterExtension + 1 );
    AllocBufferL( maxSize );

    TSize subbandSize( 0, 0 );
    TUint16 bandIndex = 0;
    TUint16 blockIndex = 0;

    CJ2kSubband* subband = 0;
    CJ2kPacket* packet = 0;
    CJ2kCodeBlock* codeblock = 0;

    TUint8  quantStyle = 0;
    TUint8  bitDepth = 0;
    TUint8  cblkStyle = 0;
    TUint8  levelIndex = 0;
    TUint32 packetIndex = 0;
    TUint32 blockXCoord = 0;
    TUint32 blockYCoord = 0;
    TUint32 compXCoord = 0;
    TUint32 compYCoord = 0;
    TInt32 blockXEnd = 0;
    TInt32 blockYEnd = 0;
    TRect supportRegion( 0, 0, 0 , 0 );
    TRect parentSupportRegion( 0, 0, 0 , 0 );
    TPoint regionOffset( 0, 0 );
    TInt16 tmpLevelIndex = 0;
    TSize thisCompSize( 0, 0 );
    TSize firstCompSize( 0, 0 );
    TSize regionSize( 0, 0 );
    TPoint tileEndPoint( 0, 0 );
    TPoint tileStartPoint( 0, 0 );
    TPoint blockStepSize( 0, 0 );

    CJ2kWriterComponentInfo* component = 0;
    RPointerArray<CJ2kPacket>* packetList;
    TSizMarker& sizMarker = CONST_CAST( TSizMarker&, aImageInfo.SizMarker() );

    aEntropyDecoder.SetNewSizeL( aImageInfo.MaxBlockSize() );
    componentInfo = CONST_CAST( CJ2kComponentInfo*, &aTile.ComponentAt( 0 ) );
    
    // Get the resolution level for this component
    iWaveletLevels = (TInt16)( componentInfo->Levels() - aImageInfo.LevelDrop() );
    if ( iWaveletLevels < 0 )
        {
        iWaveletLevels = 0;
        }

    // Get the top subband ( original image )
    subband = CONST_CAST( CJ2kSubband*, componentInfo->SubbandAt( (TUint8)iWaveletLevels ) );
    if ( subband->SubbandResLevel() != 0 )
        {
        subband = subband->Parent();
        }

    subbandSize = subband->SubbandCanvasSize();
    tileEndPoint.iX = subbandSize.iWidth;
    tileEndPoint.iY = subbandSize.iHeight;
    tileStartPoint.iX = componentInfo->ComponentCanvas().iTl.iX;
    tileStartPoint.iY = componentInfo->ComponentCanvas().iTl.iY;

    // Loop on 256x256 blocks to reduce the memory required to perform the inverse wavelet
    // First set as starting point the canvas coordinates of this component
    blockXCoord = 0;
    blockYCoord = 0;
    blockStepSize.iX = KWaveletBlockSize;
    blockStepSize.iY = KWaveletBlockSize;

    for ( ; blockYCoord < (TUint32)tileEndPoint.iY; blockYCoord += KWaveletBlockSize )
        {
        // Start from the left border of this tile
        blockXCoord = 0;

        for ( ; blockXCoord < ( TUint32 )tileEndPoint.iX; blockXCoord += KWaveletBlockSize )
            {
            // For each component in the tile
            for ( compIndex = 0; compIndex < aImageInfo.NumOfComponents(); ++compIndex )
                {
                compXCoord = blockXCoord;
                compYCoord = blockYCoord;

                componentInfo = CONST_CAST( CJ2kComponentInfo*, &aTile.ComponentAt( compIndex ) );

                // Skip the component when height or width is 0
                if ( componentInfo->ComponentCanvas().Height() == 0 ||
                     componentInfo->ComponentCanvas().Width() == 0  )
                    {
                    continue;   //lint !e960    Continue is OK.
                    }

                // Here we have to add a check that if we have sub sampled component together with color transform,
                // we have to subsample also the block dimensions ( so that the inverse color transform is performed
                // correctly.
                blockStepSize.iX = KWaveletBlockSize;
                blockStepSize.iY = KWaveletBlockSize;
                if ( aImageInfo.NumOfComponents() == 3 )
                    {
                    if ( sizMarker.iXRsiz[1] == 2 * sizMarker.iXRsiz[0] &&
                         sizMarker.iXRsiz[2] == 2 * sizMarker.iXRsiz[0] )
                        {
                        if ( sizMarker.iXRsiz[1] == 2 * sizMarker.iXRsiz[0] &&
                             sizMarker.iXRsiz[2] == 2 * sizMarker.iXRsiz[0] )
                            {
                            if( compIndex == 1 || compIndex == 2 )
                                {
                                blockStepSize.iX = KWaveletBlockSize >> 1;
                                blockStepSize.iY = KWaveletBlockSize >> 1;
                                compXCoord >>= 1;
                                compYCoord >>= 1;
                                }
                            }
                        else
                            {
                            if( compIndex == 1 || compIndex == 2 )
                                {
                                blockStepSize.iX = KWaveletBlockSize >> 1;
                                compXCoord >>= 1;
                                blockStepSize.iY = KWaveletBlockSize;
                                }
                            }
                        }
                    }

                // Check for component truncation
                if ( aImageInfo.ComponentDrop() )
                    {
                    if ( aImageWriter.SingleFileOutput() )
                        {
                        if ( compIndex == ( aImageInfo.ComponentDrop() - 1 ) )
                            {
                            numOfComponents = compIndex;
                            }
                        else
                            {
                            continue;   //lint !e960    Continue is OK.
                            }
                        }
                    else
                        {
                        if ( compIndex != ( aImageInfo.ComponentDrop() - 1 ) )
                            {
                            continue;   //lint !e960    Continue is OK.
                            }
                        }
                    }

                // Get the resolution level for this component
                iWaveletLevels = (TInt16)( componentInfo->Levels() - aImageInfo.LevelDrop() );

                TInt32 stepSize = 1;
                if ( iWaveletLevels < 0 )
                    {
                    TInt32 i;

                    // Compute the output step size, the stepSize indicates how much more 
                    // resolution has to be dropped if the image didn't have enough wavelet
                    // levels to begin with. One indicates no extra resolution drop (write
                    // each sample) and for each extra drop skip half of the samples, i.e.
                    // stepSize is 2^extraLevels in case extra drop is needed.
                    
                    // Adjust the tile starting points and the stepSize
                    for ( i = 0; i < (-iWaveletLevels); i++ )
                        {
                        // Double the step size for every extra level dropped.
                        stepSize *= 2;
                        }
            
        
                    iWaveletLevels = 0;
                    }

                // Get the top subband ( original image )
                subband = CONST_CAST( CJ2kSubband*, componentInfo->SubbandAt( (TUint8)iWaveletLevels ) );

                if ( subband->SubbandResLevel() != 0 )
                    {
                    subband = subband->Parent();
                    }

                subbandSize = subband->SubbandCanvasSize();

                // Skip the component when subband's height or width is 0
                if ( subbandSize.iWidth == 0 || subbandSize.iHeight == 0 )
                    {
                    continue;   //lint !e960    Continue is OK.
                    }

                // Check that the block doesn't exceed the boundary of this component
                if( (TInt32)compXCoord+blockStepSize.iX > subbandSize.iWidth )
                    {
                    blockXEnd = subbandSize.iWidth;
                    }
                else
                    {
                    blockXEnd = compXCoord+blockStepSize.iX;
                    }
                if( (TInt32)compYCoord+blockStepSize.iY > subbandSize.iHeight )
                    {
                    blockYEnd = subbandSize.iHeight;
                    }
                else
                    {
                    blockYEnd = compYCoord+blockStepSize.iY;
                    }

                // Store the block size on the first component in case the others are sub sampled
                if( compIndex == 0 )
                    {
                    firstCompSize.iWidth = blockXEnd - blockXCoord;
                    firstCompSize.iHeight = blockYEnd - blockYCoord;
                    }

                // Store the block size of this component
                thisCompSize.iWidth = blockXEnd - compXCoord;
                thisCompSize.iHeight = blockYEnd - compYCoord;

                // This component could be sampled so that the block doesn't "exist" 
                // in this component, if so move to next component
                if( thisCompSize.iWidth <= 0 || thisCompSize.iHeight <= 0 )         
                    {
                    continue;   //lint !e960    Continue is OK.
                    }

                component = CONST_CAST( CJ2kWriterComponentInfo*, &aImageWriter.WriterComponentAt( compIndex ) );
                
                // Have to allocate memory for each component in the image writer                
                subbandSize.iWidth = subbandSize.iHeight = KMaxBlockSupportSize;
                component->AllocDataL( subbandSize );

                bitDepth = aImageInfo.DepthOfComponent( compIndex );
        
                // Get the right number of levels and transform type 
                // reversible == 1 for 5x3 filter and == 0 for 9x7 filter
                iReversible = componentInfo->IsReversible();
                iROIShift = componentInfo->RoiShift();

                quantStyle = componentInfo->QuantizationStyle();
                cblkStyle = componentInfo->CodeBlockStyle();

                // The support region is the region needed on current level to 
                // compute the samples in current block.
                supportRegion.iTl.iX = compXCoord;
                supportRegion.iTl.iY = compYCoord;
                supportRegion.iBr.iX = blockXEnd;
                supportRegion.iBr.iY = blockYEnd;

                // For each resolution level in the component
                for ( levelIndex = 0; levelIndex <= iWaveletLevels; levelIndex++ )
                    {
                    // The support region is the region needed on current level to compute the samples in current block.
                    supportRegion.iTl.iX = compXCoord;
                    supportRegion.iTl.iY = compYCoord;
                    supportRegion.iBr.iX = blockXEnd;
                    supportRegion.iBr.iY = blockYEnd;
                    parentSupportRegion = supportRegion;
                    regionOffset.iX = regionOffset.iY = 0;

                    // Compute the support region of the parent level of the bands that are to be processed
                    // The support region is computed depending on the level we process currently.
                    for( tmpLevelIndex = iWaveletLevels; tmpLevelIndex > levelIndex; tmpLevelIndex-- )
                        {
                        // Get the subband on this ( temp )level in order to find out if high-pass first is true or not
                        subband = CONST_CAST( CJ2kSubband*, componentInfo->SubbandAt( (TUint8)( tmpLevelIndex ) ) );

                        // Level zero has the same support as level 1, so don't update the support for level zero.
                        if( tmpLevelIndex != 1 )
                            {
                            // Compute the new crop coordinates for the subbands on this level
                            if( iReversible )
                                {
                                // Support region for low-pass bands for 5x3 filter

                                // If this band is computed high-pass first, then one extra low-pass coefficient is needed from left.
                                // This is due to the fact that for Output[2n+1], we need five samples H[n-1], L[n], H[n], L[n+1] and H[n+1], but when high-pass 
                                // is computed first, we actually need samples H[n-1], L[n-1], H[n], L[n] and H[n+1], where L are the low-pass filtered samples
                                // and H are the high-pass filtered samples.
                                if( subband->Parent()->HighPassFirst().iX )
                                    {
                                    parentSupportRegion.iTl.iX = ( ( ( parentSupportRegion.iTl.iX >> 1 ) - 1 ) > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                                 ( ( parentSupportRegion.iTl.iX >> 1 ) - 1 ) : 0; //lint !e702    The shifted values cannot be negative here
                                    }
                                else
                                    {
                                    parentSupportRegion.iTl.iX = parentSupportRegion.iTl.iX >> 1; //lint !e702    The shifted values cannot be negative here
                                    }
                                // If this band is computed high-pass first, then one extra low-pass coefficient is needed from above.
                                if( subband->Parent()->HighPassFirst().iY )
                                    {
                                    parentSupportRegion.iTl.iY = ( ( ( parentSupportRegion.iTl.iY >> 1 ) - 1 )>0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                                 ( ( parentSupportRegion.iTl.iY >> 1 ) - 1 ) : 0; //lint !e702    The shifted values cannot be negative here
                                    }
                                else
                                    {
                                    parentSupportRegion.iTl.iY = parentSupportRegion.iTl.iY >> 1; //lint !e702    The shifted values cannot be negative here
                                    }

                                parentSupportRegion.iBr.iX = ( ( parentSupportRegion.iBr.iX ) >> 1 ) + 1; //lint !e702    The shifted values cannot be negative here
                                parentSupportRegion.iBr.iY = ( ( parentSupportRegion.iBr.iY ) >> 1 ) + 1; //lint !e702    The shifted values cannot be negative here
                                }
                            else
                                {
                                // Support region for low-pass bands for 9x7 filter
                                // If this band is computed high-pass first, then one extra low-pass coefficient is needed ( from left ).
                                if( subband->Parent()->HighPassFirst().iX )
                                    {
                                    parentSupportRegion.iTl.iX = ( ( parentSupportRegion.iTl.iX >> 1 )-2 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                                 ( parentSupportRegion.iTl.iX >> 1 )-2 : 0; //lint !e702    the shifted values cannot be negative here
                                    }
                                else
                                    {
                                    parentSupportRegion.iTl.iX = ( ( ( parentSupportRegion.iTl.iX >> 1 ) - 1 ) > 0 ) ? //lint !e702    The shifted values cannot be negative here
                                                                 ( ( parentSupportRegion.iTl.iX >> 1 ) - 1 ) : 0; //lint !e702    the shifted values cannot be negative here
                                    }
                                if( subband->Parent()->HighPassFirst().iY )
                                    {
                                    parentSupportRegion.iTl.iY = ( ( ( parentSupportRegion.iTl.iY >> 1 ) - 2 ) > 0 ) ? //lint !e702    The shifted values cannot be negative here
                                                                 ( ( parentSupportRegion.iTl.iY >> 1 ) - 2 ) : 0; //lint !e702    the shifted values cannot be negative here
                                    }
                                else
                                    {
                                    parentSupportRegion.iTl.iY = ( ( ( parentSupportRegion.iTl.iY >> 1 ) - 1 ) > 0 ) ? //lint !e702    The shifted values cannot be negative here
                                                                 ( ( parentSupportRegion.iTl.iY >> 1 ) - 1 ) : 0; //lint !e702    the shifted values cannot be negative here
                                    }

                                parentSupportRegion.iBr.iX = ( ( parentSupportRegion.iBr.iX ) >> 1 ) + 2; //lint !e702    the shifted values cannot be negative here
                                parentSupportRegion.iBr.iY = ( ( parentSupportRegion.iBr.iY ) >> 1 ) + 2; //lint !e702    the shifted values cannot be negative here
                                }
                            }
                        }


                    subband = CONST_CAST( CJ2kSubband*, componentInfo->SubbandAt( levelIndex ) );

                    // For each subband in the resolution level
                    do    
                        {
                        bandIndex = ( quantStyle == 1 ) ? (TUint16)0 : subband->SubbandTreeIndex();

                        // Get the right magnitude_bits for this band
                        iMagnitudeBitsHere = componentInfo->MagnitudeBits( bandIndex );

                        // If iWaveletLevels == 0, do not update the supportRegion, since there is no wavelet decomposition ( zero levels )
                        if( iWaveletLevels != 0 )
                            {
                            // Now compute the support region for this band depending on whether it is LL, LH, HL or HH
                            // Use the support region of the parent for computation.
                            //
                            if( iReversible )
                                {
                                if( ( subband->SubbandType() ) == 0 || ( subband->SubbandType() ) == 2 )
                                    {
                                    if( subband->Parent()->HighPassFirst().iX )
                                        {
                                        supportRegion.iTl.iX = ( ( parentSupportRegion.iTl.iX >> 1 )-1 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                               ( parentSupportRegion.iTl.iX >> 1 )-1 : 0; //lint !e702    the shifted values cannot be negative here
                                        }
                                    else
                                        {
                                        supportRegion.iTl.iX = parentSupportRegion.iTl.iX >> 1;     //lint !e702    the shifted value cannot be negative here
                                        }
                                    }
                                else
                                    {
                                    supportRegion.iTl.iX = ( ( parentSupportRegion.iTl.iX >> 1 )-1 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                           ( parentSupportRegion.iTl.iX >> 1 )-1 : 0; //lint !e702    the shifted values cannot be negative here
                                    }


                                if( ( subband->SubbandType() ) == 0 || ( subband->SubbandType() ) == 1 )
                                    {
                                    if( subband->Parent()->HighPassFirst().iY )
                                        {
                                        supportRegion.iTl.iY = ( ( parentSupportRegion.iTl.iY >> 1 )-1 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                               ( parentSupportRegion.iTl.iY >> 1 )-1 : 0; //lint !e702    the shifted values cannot be negative here
                                        }
                                    else
                                        {
                                        supportRegion.iTl.iY = parentSupportRegion.iTl.iY >>1;      //lint !e702    the shifted value cannot be negative here
                                        }
                                    }
                                else
                                    {
                                    supportRegion.iTl.iY = ( ( parentSupportRegion.iTl.iY >> 1 )-1 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                           ( parentSupportRegion.iTl.iY >> 1 )-1 : 0; //lint !e702    the shifted values cannot be negative here
                                    }

                                // Compute the offset for this level
                                regionOffset.iX = parentSupportRegion.iTl.iX - 2*supportRegion.iTl.iX;
                                regionOffset.iY = parentSupportRegion.iTl.iY - 2*supportRegion.iTl.iY;

                                // Support region's bottom right corner for each band is ( supportParent.iBr>>1 )+1
                                supportRegion.iBr.iX = ( ( parentSupportRegion.iBr.iX ) >> 1 ) + 1; //lint !e702    the shifted value cannot be negative here
                                supportRegion.iBr.iY = ( ( parentSupportRegion.iBr.iY ) >> 1 ) + 1; //lint !e702    the shifted value cannot be negative here
                                }
                            else        // irreversible ( 9x7 ) filter 
                                {
                                // For low-pass filtering, the offset for the output is 1
                                if( ( subband->SubbandType() ) == 0 || ( subband->SubbandType() ) == 2 )
                                    {
                                    if( subband->Parent()->HighPassFirst().iX )
                                        {
                                        supportRegion.iTl.iX = ( ( parentSupportRegion.iTl.iX >> 1 )-2 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                               ( parentSupportRegion.iTl.iX >> 1 )-2 : 0; //lint !e702    the shifted values cannot be negative here
                                        }
                                    else
                                        {
                                        supportRegion.iTl.iX = ( ( parentSupportRegion.iTl.iX >> 1 )-1 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                               ( parentSupportRegion.iTl.iX >> 1 )-1 : 0; //lint !e702    the shifted values cannot be negative here
                                        }

                                    }
                                else
                                    {
                                    supportRegion.iTl.iX = ( ( parentSupportRegion.iTl.iX >> 1 )-2 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                           ( parentSupportRegion.iTl.iX >> 1 )-2 : 0; //lint !e702    the shifted values cannot be negative here
                                    }

                                if( ( subband->SubbandType() ) == 0 || ( subband->SubbandType() ) == 1 )
                                    {
                                    if( subband->Parent()->HighPassFirst().iY )
                                        {
                                        supportRegion.iTl.iY = ( ( parentSupportRegion.iTl.iY >> 1 )-2 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                               ( parentSupportRegion.iTl.iY >> 1 )-2 : 0; //lint !e702    the shifted values cannot be negative here
                                        }
                                    else
                                        {
                                        supportRegion.iTl.iY = ( ( parentSupportRegion.iTl.iY >> 1 )-1 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                               ( parentSupportRegion.iTl.iY >> 1 )-1 : 0; //lint !e702    the shifted values cannot be negative here
                                        }   
                                    }
                                else
                                    {
                                    supportRegion.iTl.iY = ( ( parentSupportRegion.iTl.iY >> 1 )-2 > 0 ) ?  //lint !e702    The shifted values cannot be negative here
                                                           ( parentSupportRegion.iTl.iY >> 1 )-2 : 0; //lint !e702    the shifted values cannot be negative here
                                    }

                                // Compute the offset for this level
                                regionOffset.iY = parentSupportRegion.iTl.iY - 2*supportRegion.iTl.iY;
                                regionOffset.iX = parentSupportRegion.iTl.iX - 2*supportRegion.iTl.iX;

                                // Support region's bottom right corner for each band is ( supportParent.iBr>>1 )+2
                                supportRegion.iBr.iX = ( ( parentSupportRegion.iBr.iX ) >> 1 ) + 2; //lint !e702    the shifted value cannot be negative here
                                supportRegion.iBr.iY = ( ( parentSupportRegion.iBr.iY ) >> 1 ) + 2; //lint !e702    the shifted value cannot be negative here
                                }
                            }

                        // Check that the support region doesn't exceed the band boundaries
                        if( supportRegion.iBr.iX > subband->SubbandCanvasSize().iWidth )
                            {
                            supportRegion.iBr.iX = subband->SubbandCanvasSize().iWidth;
                            }
                        if( supportRegion.iBr.iY > subband->SubbandCanvasSize().iHeight )
                            {
                            supportRegion.iBr.iY = subband->SubbandCanvasSize().iHeight;
                            }

                        // Store the low-band's dimensions here to later compute the size of the whole region ( low-band plus high-band ).
                        if( subband->SubbandType() == 2 )
                            {
                            regionSize.iWidth = supportRegion.Width();
                            }
                        if( subband->SubbandType() == 1 )
                            {
                            regionSize.iHeight = supportRegion.Height();
                            }
        
                        // Compute the Quantization parameters here, so we don't repeat that for every codeblock/precinct
                        ComputeQuantizationParameters( *componentInfo, bandIndex, subband->SubbandGain(), bitDepth );

                        // Set the lookup table for entropy decoder
                        aEntropyDecoder.SetCurrentZCLUT( (TUint8)( subband->SubbandType() ) );

                        packetList = CONST_CAST( RPointerArray<CJ2kPacket>*, &subband->PacketList() );    //lint !e665 the first parameter cannot be parenthesized here
                        
                        // For each packet in the subband
                        for ( packetIndex = 0; packetIndex < componentInfo->NumOfPackets( levelIndex ); 
                              ++packetIndex )
                            {
                            packet = ( *packetList )[packetIndex];

                            // For each codeblock in the packet
                            for ( blockIndex = 0; blockIndex < packet->NumOfBlocks(); ++blockIndex )
                                {
                                codeblock = CONST_CAST( CJ2kCodeBlock*, &packet->CodeBlockAt( blockIndex ) );
                                
                                const TRect& cbCanvas = codeblock->CodeBlockCanvas();
                                
                                TInt32 startRow = cbCanvas.iTl.iY - subband->SubbandCanvas().iTl.iY;
                                TInt32 startCol = cbCanvas.iTl.iX - subband->SubbandCanvas().iTl.iX;
                                TInt32 cblkHeight = cbCanvas.Height();
                                TInt32 cblkWidth = cbCanvas.Width();
                                TInt32 startRowCblk = 0;
                                TInt32 startColCblk = 0;
                                TInt32 startRowImage = 0;
                                TInt32 startColImage = 0;
                                
                                if( startRow >= supportRegion.iBr.iY || startCol >= supportRegion.iBr.iX ||
                                    ( startRow+cblkHeight ) <= supportRegion.iTl.iY ||
                                    ( startCol+cblkWidth ) <= supportRegion.iTl.iX )
                                    { 
                                    }
                                else 
                                    {
                                    // Compute the start column from which to copy from ( in the codeblock ) and where to copy to ( in the image block )
                                    if( startCol <= supportRegion.iTl.iX )
                                        {
                                        startColCblk  = supportRegion.iTl.iX - startCol;    // Ignore samples outside supportRegion
                                        startColImage = 0;              // First block, start from supportRegion's start
                                        cblkWidth -= startColCblk;      // Ignore samples outside supportRegion
                                        }
                                    else
                                        {
                                        startColCblk  = 0;                              // First block, start from supportRegion's start
                                        startColImage = startCol-supportRegion.iTl.iX;  // First block, start from supportRegion's start
                                        }
                                    // If the last block extends outside supportRegion, we have to adjust codeblock's width
                                    if( ( startCol+cbCanvas.Width() ) > supportRegion.iBr.iX )
                                        {
                                        cblkWidth -= ( ( startCol+cbCanvas.Width() ) - supportRegion.iBr.iX );  // Ignore samples outside supportRegion
                                        }
                                    
                                    // Compute the start row from which to copy from ( in the codeblock ) and where to copy to ( in the image block )
                                    if( startRow <= supportRegion.iTl.iY )
                                        {
                                        startRowCblk  = supportRegion.iTl.iY - startRow;    // ignore samples outside supportRegion
                                        startRowImage = 0;                                  // First block, start from supportRegion's start
                                        cblkHeight -= startRowCblk;                         // Ignore samples outside supportRegion
                                        }
                                    else
                                        {
                                        startRowCblk  = 0;                              // First block, start from supportRegion's start
                                        startRowImage = startRow-supportRegion.iTl.iY;  // First block, start from supportRegion's start
                                        }
                                    // If the last block extends outside supportRegion, we have to adjust codeblock's height
                                    if( ( startRow+cbCanvas.Height() ) > supportRegion.iBr.iY )
                                        {
                                        cblkHeight -= ( ( startRow+cbCanvas.Height() ) - supportRegion.iBr.iY );    // ignore samples outside supportRegion
                                        }
                                                                        
                                    // There is coded data in the codeblock
                                    if ( codeblock->DataLength() )
                                        {                                        
                                        // Decode the codeblock
                                        aEntropyDecoder.DecodeCodeblock( *codeblock, cblkStyle, 
                                                                         (TUint8)( iMagnitudeBitsHere + iROIShift ) );
                                            
                                        codeblock->SetCodeBlockDecoded();

                                        CopyDataToBlock( aEntropyDecoder, component->Data(), *subband, quantStyle, 
                                                         startRowCblk, startColCblk, startRowImage, 
                                                         startColImage, cblkHeight, cblkWidth );
                                        }
                                    else
                                        {
                                        // Empty codeblock, fill the corresponding area with zero, since other wise there might be data left from lower levels.
                                        FillDataWithZeros( component->Data(), *subband, startRowImage, startColImage, cblkHeight, cblkWidth );
                                        }
                                    }
                                
                                } // end of each codeblock
                            }  // end of each packet

                        // Get the sibling subband
                        subband = subband->NextSubbandRaster();

                        } while ( subband ); // end of each subband in the resolution level

                    // Now that we have processed all the blocks on this level, we can perform inverse wavelet transform on this level
                    // On resolution level zero, we don't have to inverse transform
                    if( levelIndex != 0 )
                        {
                        // The size of the inverse transformed region is the size of the low- ( stored in "regionSize" ) 
                        // and high-pass ( the supportRegion is always for HH-band, when we reach this point ) parts combined
                        regionSize = regionSize + supportRegion.Size();

                        subband = CONST_CAST( CJ2kSubband*, componentInfo->SubbandAt( levelIndex ) );
                        SingleLevelWaveletInverse( component->Data(), subband, regionOffset, regionSize, levelIndex );
                        }
                    }  // end of each resolution level in the component

                // Check whether extra downsampling is needed
                if( stepSize > 1 )
                    {
                    
                    TInt32 i,j;
                    TInt32 iStep,jStep;
                    
                    // If the stepSize is larger than one it means that we have to downsample
                    // the output. This is because there were not enough wavelet levels to do
                    // the resolution dropping for this component.
                    
                    // The reason why downsampling is done here and not in the ImageWriter is
                    // that different components might have different number of wavlet levels
                    // and thus it is easier to downsample the components here (so that we can 
                    // just write out the samples normally in ImageWriter.
                    for( i = 0,iStep = 0; iStep<KMaxBlockSupportSize; i++,iStep += stepSize)
                        {
                        for( j = 0,jStep = 0; jStep<KMaxBlockSupportSize; j++,jStep += stepSize)
                            {
                            
                            // Downsample the component so that downsampled image is in the
                            // upper left-hand corner.
                            component->Data()[i][j] = component->Data()[iStep][jStep];
                            }
                        }
                    }

                if ( numOfComponents >= 3 ) 
                    {
                    if ( aTile.ColorTransformation() || 
                        ( aImageWriter.CSCode() == 18 || aImageWriter.CSCode() == 16 ) )
                        {
                        // Wait til we finish decoding all components before writing out the image
                        if ( compIndex < 2 )
                            {
                            continue;   //lint !e960    Continue is OK.
                            }
                        }
                    else if ( aImageWriter.CSCode() == 0 )
                        {
                        // This case is also valid when no CSCode is defined, e.g. no file format is present
                        if( numOfComponents <= 3 )
                            {
                            if ( compIndex < 2 )
                                {
                                continue;   //lint !e960    Continue is OK.
                                }
                            }
                        else
                            {
                            // Proceed to outputBlockL
                            }
                        }
                    }   //lint !e961    no else is needed here at the end of if...else if

                aImageWriter.OutputBlockL( aTile, compIndex, blockXCoord, blockYCoord, firstCompSize, thisCompSize );
                }   // end of each component in the tile
            }   // end of loop on the 256x256 blocks
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::OneDimReversibleFilter
// Perform one dimensional synthesis using reversible 5/3 filter
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::OneDimReversibleFilter( TInt32 aStartPos, TInt32 aEndPos )
    {
    if ( ( aEndPos - aStartPos ) == 1 )
        {
        if ( aStartPos )
            {
            iOutputBuffer[aStartPos] = iInputBuffer[aStartPos] / 2;
            }
        else
            {
            iOutputBuffer[aStartPos] = iInputBuffer[aStartPos];
            }
        }
    else
        {
        aEndPos++;
        TInt32 idx = 0;
        for ( idx = 0; idx < aEndPos; idx += 2 )
            {
            iOutputBuffer[idx] = iInputBuffer[idx] - 
                                 ( ( iInputBuffer[idx - 1] + iInputBuffer[idx + 1] + 2 ) >> 2 );    //lint !e704 shifting is OK.
            }

        for ( idx = 1; idx < aEndPos; idx += 2 )
            {
            iOutputBuffer[idx] = iInputBuffer[idx] + 
                                 ( ( iOutputBuffer[idx - 1] + iOutputBuffer[idx + 1] ) >> 1 );      //lint !e704 shifting is OK.
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::PerformExtension
// Performs one dimensional symmetric extension of the line of pixels from
// the 2 extensions of the line.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::PerformExtension( TInt32 aStartPos, TInt32 aEndPos )
    {
    TInt32* high = iInputBuffer + aEndPos - 1;
    TInt32 dir = 1;
    TInt32 posLeft  = aStartPos;
    TInt32 posRight = aEndPos - 1;
  
    for ( TInt32 idx = 1; idx <= KFilterExtension; ++idx )
        {
        posLeft += dir;
        posRight -= dir;
        iInputBuffer[aStartPos - idx] = iInputBuffer[posLeft];
        high[idx] = iInputBuffer[posRight];

        if ( posLeft == ( aEndPos - 1 ) )
            {
            dir = -1;
            }
        if ( dir == -1 )
            {
            if ( posLeft == aStartPos )
                {
                dir = 1;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::OneDimIrrevFilter
// Perform one dimensional synthesis using irreversible 9/7 filter
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::OneDimIrrevFilter( TInt32 aStartPos, 
                                       TInt32 aEndPos,
                                       TUint8 aLevel,
                                       TUint8 aVertical )
    {
    if ( ( aEndPos - aStartPos ) == 1 )
        {
        if ( aStartPos )
            {
            iOutputBuffer[aStartPos] = iInputBuffer[aStartPos] / 2;
            }
        else
            {
            iOutputBuffer[aStartPos] = iInputBuffer[aStartPos];
            }
        }
    else
        {
        // First the low-pass parts
        TInt32 idx = aStartPos + ( aStartPos % 2 );
        for ( ; idx < aEndPos; idx += 2 )
            {
            iOutputBuffer[idx] = iTapsLow[0] * iInputBuffer[idx] + 
                                 iTapsLow[2] * ( iInputBuffer[idx - 2] + iInputBuffer[idx + 2] );
            }

        idx = aStartPos + ( ( aStartPos + 1 ) % 2 );
        for ( ; idx < aEndPos; idx += 2 )
            {
            iOutputBuffer[idx] = iTapsLow[1] * ( iInputBuffer[idx - 1] + iInputBuffer[idx + 1] ) +
                                 iTapsLow[3] * ( iInputBuffer[idx - 3] + iInputBuffer[idx + 3] );
            }
    
        // Then the high-pass parts 
        idx = aStartPos + ( ( aStartPos + 1 ) % 2 );
        for ( ; idx < aEndPos; idx += 2 )
            {
            iOutputBuffer[idx] += iTapsHigh[0] * iInputBuffer[idx] +
                                  iTapsHigh[2] * ( iInputBuffer[idx - 2] + iInputBuffer[idx + 2] ) +
                                  iTapsHigh[4] * ( iInputBuffer[idx - 4] + iInputBuffer[idx + 4] );
            }

        idx = aStartPos + ( aStartPos % 2 );
        for ( ; idx < aEndPos; idx += 2 )
            {
            iOutputBuffer[idx] += iTapsHigh[1] * ( iInputBuffer[idx - 1] + iInputBuffer[idx + 1] ) +
                                  iTapsHigh[3] * ( iInputBuffer[idx - 3] + iInputBuffer[idx + 3] ); 
            }

        TInt32 offset = 0;
        TInt32 downshift = 0;

        // Finally downshift all the samples
        if ( !aVertical || ( aLevel != 0 ) )
            {
            downshift = KFilterShift;
            }
        else
            {
            downshift = KFilterShift + KWaveletShift;
            }

        offset = ( (TUint32)( 1 << downshift ) >> 1 );
        idx = aStartPos;

        for ( ; idx < aEndPos; idx++ )
            {
            iOutputBuffer[idx] = ( iOutputBuffer[idx] + offset ) >> downshift;    //lint !e704 shifting is OK.
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::OneDimFiltering
// Perform one dimensional filtering
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::OneDimFiltering( TInt32 aStartPos, 
                                     TInt32 aEndPos,
                                     TUint8 aLevel,
                                     TUint8 aVertical )
    {
    // Extend the signals ( at the start and end )
    PerformExtension( aStartPos, aEndPos );

    if ( iReversible )
        {
        OneDimReversibleFilter( aStartPos, aEndPos );
        }
    else
        {
        OneDimIrrevFilter( aStartPos, aEndPos, aLevel, aVertical );
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::HorizontalFilter
// Perform one dimensional horizontal filtering
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::HorizontalFilter( TPrecInt** aImage, 
                                      TInt32 aRow,
                                      TUint32 aXtcSiz, 
                                      CJ2kSubband* aSubband )
    {
    TInt32 endPos = 0;
    TInt32 startPos = aSubband->HighPassFirst().iX;
  
    TInt32* rowImage = (TInt32*)aImage[aRow];
    TInt32* rowImageHigh = rowImage + aSubband->ChildAt( CJ2kSubband::EBandLL )->SubbandCanvasSize().iWidth; 
    TInt32* iterator = iInputBuffer;

    // Low-pass is first
    if ( !startPos )  
        {
        for ( endPos = aXtcSiz >> 1; endPos > 0; endPos-- )
            {
            *iterator++ = *rowImage++;
            *iterator++ = *rowImageHigh++;
            }
        if ( aXtcSiz % 2 )  // One extra sample for low-pass
            {
            *iterator = *rowImage;
            }
        }
    else  // High-pass is first
        {
        iterator++;
        for ( endPos = aXtcSiz >> 1; endPos > 0; endPos-- )
            {
            *iterator++ = *rowImageHigh++;
            *iterator++ = *rowImage++;
            }
        if ( aXtcSiz % 2 )  // One extra sample for high-pass
            {
            *iterator = *rowImageHigh;
            }
        }
  
    endPos = aXtcSiz + startPos; 

    OneDimFiltering( startPos, endPos, aSubband->SubbandLevel(), 0 ); 
    Mem::Copy( aImage[aRow], iOutputBuffer + startPos, aXtcSiz * sizeof( TPrecInt ) );
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::VerticalFilter
// Perform one dimensional vertical filtering
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::VerticalFilter( TPrecInt **aImage, 
                                    TInt32 aColumn,
                                    TUint32 aYtcSiz, 
                                    CJ2kSubband *aSubband )
    {
    TInt32 startPos = aSubband->HighPassFirst().iY;
    TInt32 highStart = aSubband->ChildAt( CJ2kSubband::EBandLL )->SubbandCanvasSize().iHeight;
    TInt32 highStop = aSubband->SubbandCanvasSize().iHeight;
    TInt32 *iterator = iInputBuffer;
 
    TInt32 lowIndex = 0;
    TInt32 highIndex = highStart;
    if ( !startPos )
        {
        for ( ; highIndex < highStop; lowIndex++, highIndex++ )
            {
            *iterator++ = aImage[lowIndex][aColumn];
            *iterator++ = aImage[highIndex][aColumn];
            }
        if ( aYtcSiz % 2 )
            {
            *iterator = aImage[lowIndex][aColumn];
            }
        }
    else 
        {
        iterator++;
        for ( ; lowIndex < highStart; highIndex++, lowIndex++ )
            {
            *iterator++ = aImage[highIndex][aColumn];
            *iterator++ = aImage[lowIndex][aColumn];
            }
        if ( aYtcSiz % 2 )
            {
            *iterator = aImage[highIndex][aColumn];
            }
        }

    OneDimFiltering( startPos, startPos + aYtcSiz, aSubband->SubbandLevel(), 1 );  
    iOutputBuffer += ( startPos + aYtcSiz - 1 );

    for ( lowIndex = aYtcSiz - 1; lowIndex >= 0; lowIndex-- )
        {
        aImage[lowIndex][aColumn] = *iOutputBuffer--;
        }
    iOutputBuffer += ( 1 - startPos );
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::TwoDimFiltering
// Perform two dimensional inverse wavelet transformation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::TwoDimFiltering( TPrecInt **aImage, 
                                     TInt32 aXtcSiz, 
                                     TInt32 aYtcSiz, 
                                     CJ2kSubband *aSubband )
    {
    TInt32 index = 0;

    for ( index = aYtcSiz - 1; index >= 0; index-- )
        {
        HorizontalFilter( aImage, index, aXtcSiz, aSubband );
        }

    for ( index = aXtcSiz - 1; index >= 0; index-- )
        {
        VerticalFilter( aImage, index, aYtcSiz, aSubband );
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::FullWaveletInverse
// Perform a full inverse wavelet transformation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::FullWaveletInverse( TPrecInt **aImage, CJ2kSubband *aSubband )
    {
    TSize  canvas( 0, 0 );
    TUint8 reduceLevels = 0;
    
    // If truncating resolution levels, we have to compute the reduced levels,
    // so that the final WAVELET_SHIFT in case of 9x7 filter is performed.
    if ( aSubband->SubbandLevel() > iWaveletLevels )
        {
        reduceLevels = (TUint8)( aSubband->SubbandLevel() - iWaveletLevels );
        }

    for ( TUint8 levelIndex = (TUint8)iWaveletLevels; levelIndex > 0; levelIndex-- )
        {
        // The next lines assume that subband points to the LL-band and
        // it is then moved to point to the parent i.e. to the last 
        // _decomposition level_ of the wavelet transform.
        aSubband = aSubband->Parent();

        // Adjust the level in case of resolution truncation
        aSubband->SetSubbandLevel( (TUint8)( aSubband->SubbandLevel() - reduceLevels ) );

        canvas = aSubband->SubbandCanvasSize();
        if ( canvas.iWidth > 0 && canvas.iHeight > 0 )
            {
            TwoDimFiltering( aImage, canvas.iWidth, canvas.iHeight, aSubband );
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::ComputeQuantizationParameters
// Compute the quantization parameters for a particular subband in the component
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::ComputeQuantizationParameters( const CJ2kComponentInfo& aComponentInfo,
                                                   TInt16 aBandIndex,
                                                   TUint8 aBandGain,
                                                   TUint8 aBitDepth )
    {
    if ( iROIShift != 0 )  
        {
        // ROI shifting is used
        if ( aComponentInfo.QuantizationStyle() == 0 )
            {
            // Compute the shift for the codeblock's data, which is
            // IMPLEMENTATION PRECISION -1( for sign ) - subband's range -
            // the guard bits. 
            iDataShift = ( KImplementationPrecision - 1 ) - iMagnitudeBitsHere;
            }
        else  // Irreversible case
            {  
            // Because we have integer arithmetic only, we subtract WAVELET_SHIFT from the shift.
            // This is to compensate for the fractional bits of the inverse quantized wavelet
            // coefficients.
            iDataShift = ( KImplementationPrecision - 1 ) - iMagnitudeBitsHere - KWaveletShift;

            // The quantization step is computed from the equation
            // step = ( 2^( Range-Exponent ) )*( 1+( Mantissa/( 2^11 ) ) ), where Range is the dynamic 
            // range of this subband ( = bitdepth + gain ). ( see Annex E of the standard for more
            // information ).
            //
            // The "iStepExponent" is the shift needed to remove "iStepValue"'s fractional
            // bits. The "iStepValue" is the shifted quantization step. Shifting back
            // is done right after computing the quantized coefficient.
            iStepExponent = aBitDepth + aBandGain - aComponentInfo.Exponent( aBandIndex ) - KStepBits;
            iStepValue = aComponentInfo.Mantissa( aBandIndex ) + ( 1 << KStepBits );
            }

        // Compute the shift for the data inside the ROI
        iROIDataShift = iDataShift - iROIShift;      
        }
    else  
        {
        // ROI is not present
        if ( aComponentInfo.QuantizationStyle() == 0 )
            {
            // Compute the shift for the codeblock's data, which is
            // IMPLEMENTATION PRECISION-1( for sign ) - subband's range -
            // the guard bits. 
            iDataShift = ( KImplementationPrecision - 1 ) - iMagnitudeBitsHere;
            // Shift all the samples in the codeblock
            }
        else  // Irreversible case 
            {
            // Because we have integer arithmetic only, we subtract WAVELET_SHIFT from the shift.
            // This is to compensate for the fractional bits of the inverse quantized wavelet
            // coefficients.
            iDataShift = ( KImplementationPrecision - 1 ) - iMagnitudeBitsHere - KWaveletShift;

            // To prevent overflows we check if the "shift" is at least 12. The minimum downshift of 12 is 
            // derived from the computation of "value*iStepValue", where "value" is downshifted previously by "shift"
            // and "iStepValue" is at maximum 12 bits ( since iStepValue = mantissa + 1<<KStepBits, where mantissa is
            // a 11-bit value and KStepBits is 11 ).
            //
            TInt32 iExtraBits = 0;
            // If the implementation precision is 16 then we don't have to check if the shift is at least 12.
            // This is because we use 32 bits for the computation and thus have at least 16 zero most significant 
            // bits at the "value".

            
            if ( iDataShift < 12 )    // Test to prevent overflows 
                {
                iExtraBits = 12 - iDataShift;
                iDataShift += iExtraBits;
                }

            // The quantization step is computed from the equation
            // step = ( 2^( Range-Exponent ) )*( 1+( Mantissa/( 2^11 ) ) ), where Range is the dynamic 
            // range of this subband ( = bitdepth + gain ). ( see Annex E of the standard for more
            // information ).
            //
            // The "iStepExponent" is the shift needed to remove "iStepValue"'s fractional
            // bits. The "iStepValue" is the shifted quantization step. Shifting back
            // is done right after computing the quantized coefficient.
            //
            iStepExponent = aBitDepth + aBandGain - aComponentInfo.Exponent( aBandIndex ) - KStepBits;

            // Test to prevent overflows
            iStepExponent += iExtraBits;
      
            iStepValue = aComponentInfo.Mantissa( aBandIndex ) + ( 1 << KStepBits );
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::CopyDataToImage
// Apply inverse quantization and ROI shifting on the decoded
// codeblock and copy to the image writer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::CopyDataToImage( CJ2kEntropyDecoder& aEntropyDecoder, TPrecInt** aImageBlock, 
                                     CJ2kSubband& aSubband, CJ2kCodeBlock& aCodeblock,
                                     TUint8 aQuantizationStyle )
    {
    TPrecInt* buffer = 0;    // To buffer one row of the aImageBlock
    TPrecInt* imageRow = 0;  // One row of the image
    TPrecInt valueInt = 0;
    TInt32 value = 0;
    TInt32 j = 0;

    const TRect& cbCanvas = aCodeblock.CodeBlockCanvas();

    TInt32 startRow = cbCanvas.iTl.iY - aSubband.SubbandCanvas().iTl.iY + aSubband.SubbandOrigin().iY;
    TInt32 startCol = cbCanvas.iTl.iX - aSubband.SubbandCanvas().iTl.iX + aSubband.SubbandOrigin().iX;
    TInt32 endRow = startRow + cbCanvas.Height() - 1;
    TInt32 endCol = startCol + cbCanvas.Width();

    TInt32 cols = endCol - startCol - 1;

    if ( iROIShift )  // ROI shifting is used
        {
        // Compute mask to determine which samples are in ROI
        // for mask ROI coefficients
        TInt32 mask = ( ( 1 << iMagnitudeBitsHere ) - 1 ) << ( ( KImplementationPrecision - 1 ) - iMagnitudeBitsHere );
        TInt32 mask2 = ( ~mask ) & KMaximumPrecisionInteger;
        TInt32 mask3 = ( ( 1 << iROIShift ) - 1 ) << ( ( KImplementationPrecision - 1 ) - iROIShift );
        TInt32 maskShift = ( ~mask3 ) & KMaximumPrecisionInteger;

        if ( !aQuantizationStyle )
            {
            // Shift all the samples in the codeblock
            TInt32 roiDataShift = ( iROIDataShift < 0 ) ? -iROIDataShift : iROIDataShift;
            
            for ( ; endRow >= startRow; endRow-- )
                {
                buffer = aEntropyDecoder.iData[endRow - startRow] + cols;
                for ( j = endCol - 1; j >= startCol; j-- )
                    {
                    value = ( *buffer-- );
                    if ( !( value & mask ) ) // Background
                        {
                        // iROIDataShift can never be negative here!
                        if ( iROIDataShift < 0 )
                            {
                            aImageBlock[endRow][j] = ( value < 0 ) ? -( ( value & KMaximumPrecisionInteger ) << roiDataShift ) 
                                                                    : ( value  << roiDataShift );               //lint !e704 value is positive here
                            }
                        else
                            {
                            aImageBlock[endRow][j] = ( value < 0 ) ? -( ( value & maskShift ) >> roiDataShift )  //lint !e704 value&maskShift is positive, so no risk of right shifting negative values
                                                                    : ( ( value & maskShift ) >> roiDataShift );  //lint !e704 value&maskShift is positive
                            }
                        }
                    else  // ROI
                        {
                        if ( value & mask2 ) 
                            {
                            // Decoded more than magbits bit-planes, set
                            // quantization mid-interval approx. bit just after
                            // the magbits.
                            value &= ( ~mask2 );
                            }

                        aImageBlock[endRow][j] = ( value < 0 ) ? -( ( value & KMaximumPrecisionInteger ) >> iDataShift )
                                                                : ( value >> iDataShift );                      //lint !e704 value is positive here
                        }
                    }
                }
            }
        else  // Irreversible case
            {
            // Shift all the samples in the codeblock
            TInt32 stepExponent = ( iStepExponent < 0 ) ? -iStepExponent : iStepExponent;

            // Divide into two cases depending on the sign of the iStepExponent to speed up coding
            if ( iStepExponent < 0 )
                {
                for ( ; endRow >= startRow; endRow-- )
                    {
                    buffer = aEntropyDecoder.iData[endRow - startRow] + cols;
                    for ( j = endCol - 1; j >= startCol; j-- )
                        {
                        value = ( *buffer-- );

                        // Divide into two cases depending on whether value is negative
                        if ( value < 0 )
                            {
                            if ( !( value & mask ) ) // Background
                                {
                                value = ( ( value & maskShift ) >> iROIDataShift );     //lint !e704 value&maskShift is positive
                                valueInt = -( ( value * iStepValue ) >> stepExponent );  //lint !e704 ( value*iStepValue )&KMaximumPrecisionInteger is positive
                                }
                            else  // ROI
                                {
                                if ( value & mask2 ) 
                                    {
                                    // Decoded more than magbits bit-planes, set
                                    // quantization mid-interval approx. bit just after
                                    // the magbits.
                                    value = ( value & ( ~mask2 ) ) | ( 1 << ( KImplementationPrecision - 2 - iMagnitudeBitsHere ) );
                                    }

                                value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                                valueInt = -( ( value * iStepValue ) >> stepExponent );  //lint !e704 ( value*iStepValue )&KMaximumPrecisionInteger is positive
                                }
                            }
                        else    // Value is non-negative
                            {
                            if ( !( value & mask ) ) // background
                                {
                                value = ( ( value & maskShift ) >> iROIDataShift );     //lint !e704 value&maskShift is positive
                                valueInt = ( ( value * iStepValue ) >> stepExponent );  //lint !e704 value*iStepValue is positive
                                }
                            else  // ROI
                                {
                                if ( value & mask2 ) 
                                    {
                                    // Decoded more than magbits bit-planes, set
                                    // quantization mid-interval approx. bit just after
                                    // the magbits.
                                    value = ( value & ( ~mask2 ) ) | ( 1 << ( KImplementationPrecision - 2 - iMagnitudeBitsHere ) );
                                    }

                                value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                                valueInt = ( ( value * iStepValue ) >> stepExponent );  //lint !e704 value*iStepValue is positive
                                }
                            }

                        aImageBlock[endRow][j] = valueInt;       
                        }
                    }
                }
            else    // iStepExponent is non-negative
                {
                for ( ; endRow >= startRow; endRow-- )
                    {
                    buffer = aEntropyDecoder.iData[endRow - startRow] + cols;
                    for ( j = endCol - 1; j >= startCol; j-- )
                        {
                        value = ( *buffer-- );

                        // Divide into two cases depending on whether value is negative
                        if ( value < 0 )
                            {

                            // Change value to be positive
                            value = -value;

                            if ( !( value & mask ) ) // Background
                                {
                                value = ( ( value & maskShift ) >> iROIDataShift );   //lint !e704 value&maskShift is positive
                                valueInt = ( ( value * iStepValue ) << stepExponent );
                                }
                            else  // ROI
                                {
                                if ( value & mask2 ) 
                                    {
                                    // Decoded more than magbits bit-planes, set
                                    // quantization mid-interval approx. bit just after
                                    // the magbits.
                                    value = ( value & ( ~mask2 ) ) | ( 1 << ( KImplementationPrecision - 2 - iMagnitudeBitsHere ) );
                                    }

                                value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                                valueInt = ( ( value * iStepValue ) << stepExponent );
                                }

                            // Change the sign back to negative
                            aImageBlock[endRow][j] = -valueInt;
                            }
                        else    // Value is non-negative
                            {
                            if ( !( value & mask ) ) // Background
                                {
                                value = ( ( value & maskShift ) >> iROIDataShift );   //lint !e704 value&maskShift is positive
                                valueInt = ( ( value * iStepValue ) << stepExponent );
                                }
                            else  // ROI
                                {
                                if ( value & mask2 ) 
                                    {
                                    // Decoded more than magbits bit-planes, set
                                    // quantization mid-interval approx. bit just after
                                    // the magbits.
                                    value = ( value & ( ~mask2 ) ) | ( 1 << ( KImplementationPrecision - 2 - iMagnitudeBitsHere ) );
                                    }

                                value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                                valueInt = ( ( value * iStepValue ) << stepExponent );
                                }
                            aImageBlock[endRow][j] = valueInt;
                            }
                        }
                    }
                }
            }
        }
    else  // ROI is not present
        {
        if ( !aQuantizationStyle )
            {
            // Shift all the samples in the codeblock
            TInt32 i = endRow - startRow;
            for ( ; endRow >= startRow; endRow--, i-- )
                {
                imageRow = aImageBlock[endRow] + endCol - 1;
                buffer = aEntropyDecoder.iData[i] + cols;
                for ( j = endCol - 1; j >= startCol; j-- )
                    {
                    value = *buffer--;
                    if ( value < 0 )
                        {
                        *imageRow-- = -( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                        }
                    else
                        {
                        *imageRow-- = value >> iDataShift;            //lint !e704 value is positive here
                        }
                    }
                }
            }
        else  // Irreversible case 
            {
            // Shift all the samples in the codeblock 
            TInt32 stepExponent = ( iStepExponent < 0 ) ? -iStepExponent : iStepExponent;
            
            // Divide into two cases depending on the sign of the iStepExponent to speed up coding
            if ( iStepExponent < 0 )
                {
                for ( ; endRow >= startRow; endRow-- )
                    {
                    buffer = aEntropyDecoder.iData[endRow - startRow] + cols;
                    for ( j = endCol - 1;  j >= startCol; j-- )
                        {
                        value = ( *buffer-- );

                        if ( value < 0 )   // Negative value
                            {
                            value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                            aImageBlock[endRow][j] = (TPrecInt)( -( ( value * iStepValue ) >> stepExponent ) );  //lint !e704 value*iStepValue is positive
                            }
                        else
                            {
                            value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                            aImageBlock[endRow][j] = (TPrecInt)( ( value * iStepValue ) >> stepExponent );     //lint !e704 value*iStepValue is positive
                            }
                        }
                    }
                }
            else    // iStepExponent is non-negative
                {
                for ( ; endRow >= startRow; endRow-- )
                    {
                    buffer = aEntropyDecoder.iData[endRow - startRow] + cols;
                    for ( j = endCol - 1;  j >= startCol; j-- )
                        {
                        value = ( *buffer-- );

                        if ( value < 0 )   // Negative value
                            {
                            value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                            aImageBlock[endRow][j] = (TPrecInt)( -( ( value * iStepValue ) << stepExponent ) );
                            }
                        else
                            {
                            value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                            aImageBlock[endRow][j] = (TPrecInt)( ( value * iStepValue ) << stepExponent );
                            }
                        }
                    }
                }
            }
        }
  
    // If number of levels is zero then we have to do the 
    // inverse wavelet shifting here.
    //
    if ( iWaveletLevels == 0 )
        {
        endRow = startRow + cbCanvas.Height();
        endCol = startCol + cbCanvas.Width();

        if ( aQuantizationStyle )
            {
            for ( TInt32 i = endRow - 1; i >= startRow; i-- )
                {
                for ( j = endCol - 1; j >= startCol; j-- )
                    {
                    aImageBlock[i][j] >>= KWaveletShift; //lint !e704 shifting is OK.
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::AllocBufferL
// Allocate internal buffer based on the requested size
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::AllocBufferL( TInt32 aSize )
    {
    // Allocate memory for the input and output buffers
    TInt32 totalSize = aSize * sizeof( TInt32 );

    if ( iIOBufferSize )
        {
        // Resize only when the request buffer is larger than current buffer
        if ( iIOBufferSize < totalSize )
            {
            iInputBuffer -= ( KFilterExtension + 1 );
            TInt32* tmpBuffer = STATIC_CAST( TInt32*, User::ReAlloc( iInputBuffer, totalSize ) );
            if ( !tmpBuffer )
                {
                iInputBuffer += ( KFilterExtension + 1 );
                User::Leave( KErrNoMemory );
                }
            iInputBuffer = tmpBuffer;
            iInputBuffer += ( KFilterExtension + 1 );

            iOutputBuffer -= ( KFilterExtension + 1 );
            tmpBuffer = STATIC_CAST( TInt32*, User::ReAlloc( iOutputBuffer, totalSize ) );
            if ( !tmpBuffer )
                {
                iOutputBuffer += ( KFilterExtension + 1 );
                User::Leave( KErrNoMemory );
                }
            iOutputBuffer = tmpBuffer;
            iOutputBuffer += ( KFilterExtension + 1 );

            iIOBufferSize = totalSize;
            }
        }
    else
        {
        // First time buffer allocation
        iInputBuffer = STATIC_CAST( TInt32*, User::Alloc( totalSize ) );
        if ( !iInputBuffer )
            {
            User::Leave( KErrNoMemory );
            }
        iInputBuffer += ( KFilterExtension + 1 );
        iOutputBuffer = STATIC_CAST( TInt32*, User::Alloc( totalSize ) );
        if ( !iOutputBuffer )
            {
            User::Leave( KErrNoMemory );
            }
        iOutputBuffer += ( KFilterExtension + 1 );
        iIOBufferSize = totalSize;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::HorizontalBlockFilter
// Perform one dimensional horizontal filtering ( block-based ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::HorizontalBlockFilter( TPrecInt** aImage, TInt32 aRow,
                                           TUint32 aXtcSiz, CJ2kSubband* aSubband,
                                           TInt32 aXOffset, TUint8 aCurrentLevel )
    {
    TInt32 endPos = 0;
    TInt32 startPos = aSubband->HighPassFirst().iX;
  
    TInt32* rowImage = (TInt32*)aImage[aRow];
    TInt32* rowImageHigh = rowImage + KWaveletBlockMidPoint; 
    TInt32* iterator = iInputBuffer;

    // Insert one extra ( dummy, i.e. zero-valued ) low-pass sample.
    // This sample is not actually needed in computations, but now we can use the same 
    // functions for filtering for block-based and normal wavelet.
    if( !startPos && ( aXOffset > 0 ) )      
        {

        // The need for one extra sample derives from the fact that the support region
        // for the high-pass is extends one sample further than the low-pass region.
        *iterator++ = 0;
        *iterator++ = *rowImageHigh++;

        // Increment aXtcSiz by one to account for the dummy sample
        aXtcSiz++;
        }

    if ( !startPos )  // Low-pass first
        {
        for ( endPos = aXtcSiz >> 1; endPos > 0; endPos-- )
            {
            *iterator++ = *rowImage++;
            *iterator++ = *rowImageHigh++;
            }
        if ( aXtcSiz % 2 )  // One extra sample for low-pass
            {
            *iterator = *rowImage;
            }
        }
    else  // High-pass first
        {
        iterator++;
        for ( endPos = aXtcSiz >> 1; endPos > 0; endPos-- )
            {
            *iterator++ = *rowImageHigh++;
            *iterator++ = *rowImage++;
            }
        if ( aXtcSiz % 2 )  // One extra sample for high-pass
            {
            *iterator = *rowImageHigh;
            }
        }
  
    endPos = aXtcSiz + startPos; 

    OneDimFiltering( startPos, endPos, (TUint8)( iWaveletLevels - aCurrentLevel ), 0 ); 

    // Copy row back to image, take care of offset
    Mem::Copy( aImage[aRow], iOutputBuffer + startPos + ( aXOffset ), ( aXtcSiz - ( aXOffset ) ) * sizeof( TPrecInt ) );
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::VerticalBlockFilter
// Perform one dimensional vertical filtering ( block-based )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::VerticalBlockFilter( TPrecInt** aImage, TInt32 aColumn, TUint32 aYtcSiz, 
                                         CJ2kSubband* aSubband, TInt32 aYOffset,
                                         TUint8 aCurrentLevel )
    {
    TInt32 startPos = aSubband->HighPassFirst().iY;
    TInt32 highStart = KWaveletBlockMidPoint;
    TInt32* iterator = iInputBuffer;
 
    TInt32 lowIndex = 0;
    TInt32 highIndex = highStart;

    // Insert one extra ( dummy, i.e. zero-valued ) low-pass sample.
    if( !startPos && ( aYOffset > 0 ) )      
        {
        *iterator++ = 0;
        *iterator++ = aImage[highIndex++][aColumn];

        // Increment aYtcSiz by one to account for the dummy sample
        aYtcSiz++;
        }

    TInt32 highStop = KWaveletBlockMidPoint + ( aYtcSiz >> 1 );
    TInt32 lowStop = aYtcSiz >> 1;

    if ( !startPos )
        {
        for ( ; highIndex < highStop; lowIndex++, highIndex++ )
            {
            *iterator++ = aImage[lowIndex][aColumn];
            *iterator++ = aImage[highIndex][aColumn];
            }
        if ( aYtcSiz % 2 )
            {
            *iterator = aImage[lowIndex][aColumn];
            }
        }
    else 
        {
        iterator++;
        for ( ; lowIndex < lowStop; highIndex++, lowIndex++ )
            {
            *iterator++ = aImage[highIndex][aColumn];
            *iterator++ = aImage[lowIndex][aColumn];
            }
        if ( aYtcSiz % 2 )
            {
            *iterator = aImage[highIndex][aColumn];
            }
        }

    OneDimFiltering( startPos, startPos + aYtcSiz, (TUint8)( iWaveletLevels - aCurrentLevel ), 1 );  
    iOutputBuffer += ( startPos + aYtcSiz - 1 );

    // Copy column back to image, take care of offset
    for ( lowIndex = ( aYtcSiz - 1 - aYOffset ); lowIndex >= 0; lowIndex-- )
        {
        aImage[lowIndex][aColumn] = *iOutputBuffer--;
        }
    iOutputBuffer += ( 1 - startPos - aYOffset );
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::TwoDimBlockFiltering
// Perform two dimensional inverse wavelet transformation ( block-based )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::TwoDimBlockFiltering( TPrecInt** aImage, TSize aRegion, CJ2kSubband* aSubband,
                                         TPoint aOffset, TUint8 aCurrentLevel )
    {
    TInt32 index = 0;
    TUint32 xtcSiz = aRegion.iWidth;
    TUint32 ytcSiz = aRegion.iHeight;

    // For block filtering we have the data in two blocks ( column-wise ), 
    // from 0 to ( ytcsiz+1 )>>1 ( +1 to take care of odd number of samples ) 
    // and from KWaveletBlockMidPoint to KWaveletBlockMidPoint+( ( ytcsiz+1 )>>1 ).
    index = KWaveletBlockMidPoint + ( ( ytcSiz+1 ) >> 1 ) - 1;

    // First apply horizontal filter to the ( vertically ) high-pass samples
    for ( ; index >= KWaveletBlockMidPoint; index-- )
        {
        HorizontalBlockFilter( aImage, index, xtcSiz, aSubband, aOffset.iX, aCurrentLevel );
        }

    // Then apply horizontal filter to the ( vertically ) low-pass samples
    index = ( ( ytcSiz+1 ) >> 1 ) - 1;
    for ( ; index >= 0; index-- )
        {
       HorizontalBlockFilter( aImage, index, xtcSiz, aSubband, aOffset.iX, aCurrentLevel );
        }

    // Because of the horizontal filtering, the data is now in one block row-wise, thus
    // two loops are not needed for vertical fitering.
    index = xtcSiz-1-( (TUint32)( aOffset.iX ) >> 1 );
    for ( ; index >= 0; index-- )
        {
        VerticalBlockFilter( aImage, index, ytcSiz, aSubband, aOffset.iY, aCurrentLevel );
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::SingleLevelWaveletInverse
// Perform a full inverse wavelet transformation ( block-based )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::SingleLevelWaveletInverse( TPrecInt **aImage, CJ2kSubband *aSubband, 
                                               TPoint aOffset, TSize aRegion, TUint8 aCurrentLevel )
    {
    if ( aRegion.iWidth > 0 && aRegion.iHeight > 0 )
        {
        aSubband = aSubband->Parent();
        TwoDimBlockFiltering( aImage, aRegion, aSubband, aOffset, aCurrentLevel );
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::CopyDataToBlock
// Apply inverse quantization and ROI shifting on the decoded
// codeblock and copy to the image writer ( block-based ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::CopyDataToBlock( CJ2kEntropyDecoder& aEntropyDecoder, TPrecInt** aImageBlock, 
                                     CJ2kSubband& aSubband, TUint8 aQuantizationStyle,
                                     TInt32 aStartRowCblk, TInt32 aStartColCblk,
                                     TInt32 aStartRowImage, TInt32 aStartColImage,
                                     TInt32 aCblkHeight, TInt32 aCblkWidth )
    {
    TPrecInt* buffer = 0;    // To buffer one row of the image_block
    TPrecInt* imageRow = 0;  // One row of the image
    TPrecInt valueInt = 0;
    TInt32 value = 0;
    TInt32 j = 0;

    TInt32 startRowImageBlock = aStartRowImage; // Start row's index in the image to copy to 
    TInt32 startColImageBlock = aStartColImage; // Start column's index in the image to copy to 

    TUint8 aBandIndex = (TUint8)( aSubband.SubbandType() );

    // Adjust the place where to copy the data according to the subband type 
    // ( i.e. whether we have LL, HL, LH or HH band ).
    if( aBandIndex == 1 )
        {
        startColImageBlock += KWaveletBlockMidPoint;
        }
    else if( aBandIndex == 2 )
        {
        startRowImageBlock += KWaveletBlockMidPoint;
        }
    else if( aBandIndex == 3 )
        {
        startRowImageBlock += KWaveletBlockMidPoint;
        startColImageBlock += KWaveletBlockMidPoint;
        }

    // Compute the end of the copy region
    TInt32 endRowImageBlock = startRowImageBlock + aCblkHeight - 1; //lint !e961    no else is needed here at the end of if...else if
    TInt32 endColImageBlock = startColImageBlock + aCblkWidth;

    // Index of the row to copy from in the codeblock
    TInt32 codeblockRow = aStartRowCblk + aCblkHeight-1;

    // Index of the row to copy from in the codeblock
    TInt32 codeblockColumn = aStartColCblk + aCblkWidth-1;

    if ( iROIShift )  // ROI shifting is used
        {
        // Compute mask to determine which samples are in ROI
        // for mask ROI coefficients
        TInt32 mask = ( ( 1 << iMagnitudeBitsHere ) - 1 ) << ( ( KImplementationPrecision - 1 ) - iMagnitudeBitsHere );
        TInt32 mask2 = ( ~mask ) & KMaximumPrecisionInteger;
        TInt32 mask3 = ( ( 1 << iROIShift ) - 1 ) << ( ( KImplementationPrecision - 1 ) - iROIShift );
        TInt32 maskShift = ( ~mask3 ) & KMaximumPrecisionInteger;

        if ( !aQuantizationStyle )
            {
            // Shift all the samples in the codeblock
            TInt32 roiDataShift = ( iROIDataShift < 0 ) ? -iROIDataShift : iROIDataShift;
            
            for ( ; endRowImageBlock >= startRowImageBlock; endRowImageBlock--,codeblockRow-- )
                {
                buffer = aEntropyDecoder.iData[codeblockRow] + codeblockColumn;
                for ( j = endColImageBlock-1; j >= startColImageBlock; j-- )
                    {
                    value = ( *buffer-- );
                    if ( !( value & mask ) ) // Background
                        {
                        if ( iROIDataShift < 0 )
                            {
                            aImageBlock[endRowImageBlock][j] = ( value < 0 ) ? -( ( value & KMaximumPrecisionInteger ) << roiDataShift ) 
                                                             : ( value  << roiDataShift );
                            }
                        else
                            {
                            aImageBlock[endRowImageBlock][j] = ( value < 0 ) ? -( ( value & maskShift ) >> roiDataShift )     //lint !e704 value&maskShift is positive, so no risk of right shifting negative values 
                                                             : ( ( value & maskShift ) >> roiDataShift );              //lint !e704 value&maskShift is positive, so no risk of right shifting negative values
                            }
                        }
                    else  // ROI
                        {
                        if ( value & mask2 ) 
                            {
                            // Decoded more than magbits bit-planes, set
                            // quantization mid-interval approx. bit just after
                            // the magbits.
                            value &= ( ~mask2 );
                            }
                        aImageBlock[endRowImageBlock][j] = ( value < 0 ) ? -( ( value & KMaximumPrecisionInteger ) >> iDataShift )
                                                         : ( value >> iDataShift );              //lint !e704 value is positive here
                        }
                    }
                }
            }
        else  // Irreversible case
            {
            // Shift all the samples in the codeblock
            TInt32 stepExponent = ( iStepExponent < 0 ) ? -iStepExponent : iStepExponent;

            // Divide into two cases depending on the sign of the iStepExponent to speed up coding
            if ( iStepExponent < 0 )
                {
                for ( ; endRowImageBlock >= startRowImageBlock; endRowImageBlock--, codeblockRow-- )
                    {
                    buffer = aEntropyDecoder.iData[codeblockRow] + codeblockColumn;
                    for ( j = endColImageBlock-1; j >= startColImageBlock; j-- )
                        {
                        value = ( *buffer-- );

                        // Divide into two cases depending on whether value is negative
                        if ( value < 0 )
                            {
                            if ( !( value & mask ) ) // Background
                                {
                                value = ( ( value & maskShift ) >> iROIDataShift );         //lint !e704 value&maskShift is positive, so no risk of right shifting negative values
                                valueInt = -( ( value * iStepValue ) >> stepExponent );     //lint !e704 value*iStepValue is positive
                                }
                            else  // ROI
                                {
                                if ( value & mask2 ) 
                                    {
                                    // Decoded more than magbits bit-planes, set
                                    // quantization mid-interval approx. bit just after
                                    // the magbits.
                                    value = ( value & ( ~mask2 ) ) | ( 1 << ( KImplementationPrecision - 2 - iMagnitudeBitsHere ) );
                                    }

                                value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                                valueInt = -( ( value * iStepValue ) >> stepExponent );      //lint !e704 value*iStepValue is positive
                                }
                            }
                        else    // Value is non-negative
                            {
                            if ( !( value & mask ) ) // Background
                                {
                                value = ( ( value & maskShift ) >> iROIDataShift );         //lint !e704 value&maskShift is positive, so no risk of right shifting negative values
                                valueInt = ( ( value * iStepValue ) >> stepExponent );      //lint !e704 value*iStepValue is positive
                                }
                            else  // ROI
                                {
                                if ( value & mask2 ) 
                                    {
                                    // Decoded more than magbits bit-planes, set
                                    // quantization mid-interval approx. bit just after
                                    // the magbits.
                                    value = ( value & ( ~mask2 ) ) | ( 1 << ( KImplementationPrecision - 2 - iMagnitudeBitsHere ) );
                                    }

                                value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                                valueInt = ( ( value * iStepValue ) >> stepExponent );      //lint !e704 value*iStepValue is positive
                                }
                            }

                        aImageBlock[endRowImageBlock][j] = valueInt;
                        }
                    }
                }
            else    // iStepExponent is non-negative
                {
                for ( ; endRowImageBlock >= startRowImageBlock; endRowImageBlock--,codeblockRow-- )
                    {
                    buffer = aEntropyDecoder.iData[codeblockRow] + codeblockColumn;
                    for ( j = endColImageBlock-1; j >= startColImageBlock; j-- )
                        {
                        value = ( *buffer-- );
                        if ( !( value & mask ) ) // Background
                            {
                            if ( value < 0 )   // Get the sign
                                {
                                value = -( ( value & maskShift ) >> iROIDataShift );          //lint !e704 value&maskShift is positive, so no risk of right shifting negative values
                                }
                            else
                                {
                                value = ( ( value & maskShift ) >> iROIDataShift );           //lint !e704 value&maskShift is positive, so no risk of right shifting negative values
                                }
                            
                            valueInt = ( ( value * iStepValue ) << stepExponent );
                            }
                        else  // ROI
                            {
                            if ( value & mask2 ) 
                                {
                                // Decoded more than magbits bit-planes, set
                                // quantization mid-interval approx. bit just after
                                // the magbits.
                                value = ( value & ( ~mask2 ) ) | ( 1 << ( KImplementationPrecision - 2 - iMagnitudeBitsHere ) );
                                }
                            if ( value < 0 )
                                {
                                value = -( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                                }
                            else
                                {
                                value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                                }
                            
                            valueInt = ( ( value * iStepValue ) << stepExponent );
                            }
                        aImageBlock[endRowImageBlock][j] = valueInt;
                        }
                    }
                }
            }
        }
    else  // ROI is not present
        {
        if ( !aQuantizationStyle )
            {
            // Shift all the samples in the codeblock
            TInt32 i = codeblockRow;
            for ( ; endRowImageBlock >= startRowImageBlock; endRowImageBlock--, i-- )
                {
                imageRow = aImageBlock[endRowImageBlock] + endColImageBlock - 1;
                buffer = aEntropyDecoder.iData[i] + codeblockColumn;

                for ( j = endColImageBlock-1; j >= startColImageBlock; j-- )
                    {
                    value = *buffer--;
                    if ( value < 0 )
                        {
                        *imageRow-- = -( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                        }
                    else
                        {
                        *imageRow-- = value >> iDataShift;          //lint !e704 value is positive here
                        }
                    }
                }
            }
        else  // Irreversible case 
            {
            // Shift all the samples in the codeblock 
            TInt32 stepExponent = ( iStepExponent < 0 ) ? -iStepExponent : iStepExponent;
            
            // Divide into two cases depending on the sign of the iStepExponent to speed up coding
            if ( iStepExponent < 0 )
                {
                for ( ; endRowImageBlock >= startRowImageBlock; endRowImageBlock--,codeblockRow-- )
                    {
                    buffer = aEntropyDecoder.iData[codeblockRow] + codeblockColumn;
                    for ( j = endColImageBlock-1;  j >= startColImageBlock; j-- )
                        {
                        value = ( *buffer-- );

                        if ( value < 0 )   // Negative value
                            {
                            value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                            aImageBlock[endRowImageBlock][j] = (TPrecInt)( -( ( value * iStepValue ) >> stepExponent ) ); //lint !e704 value*iStepValue is positive
                            }
                        else
                            {
                            value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                            aImageBlock[endRowImageBlock][j] = (TPrecInt)( ( value * iStepValue ) >> stepExponent );    //lint !e704 value*iStepValue is positive
                            }
                        }
                    }
                }
            else    // iStepExponent is non-negative
                {
                for ( ; endRowImageBlock >= startRowImageBlock; endRowImageBlock--,codeblockRow-- )
                    {
                    buffer = aEntropyDecoder.iData[codeblockRow] + codeblockColumn;
                    for ( j = endColImageBlock-1;  j >= startColImageBlock; j-- )
                        {
                        value = ( *buffer-- );

                        if ( value < 0 )   // Negative value
                            {
                            value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                            aImageBlock[endRowImageBlock][j] = (TPrecInt)( -( ( value * iStepValue ) << stepExponent ) );
                            }
                        else
                            {
                            value = ( ( value & KMaximumPrecisionInteger ) >> iDataShift );
                            aImageBlock[endRowImageBlock][j] = (TPrecInt)( ( value * iStepValue ) << stepExponent );
                            }
                        }
                    }
                }
            }
        }
  
    // If number of levels is zero then we have to do the 
    // inverse wavelet shifting here.
    //
    if ( iWaveletLevels == 0 )
        {
        endRowImageBlock = startRowImageBlock + aCblkHeight;
        endColImageBlock = startColImageBlock + aCblkWidth;

        if ( aQuantizationStyle )
            {
            for ( TInt32 i = endRowImageBlock - 1; i >= startRowImageBlock; i-- )
                {
                for ( j = endColImageBlock - 1; j >= startColImageBlock; j-- )
                    {
                    aImageBlock[i][j] >>= KWaveletShift; //lint !e704 shifting is OK.
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSynthesis::FillDataWithZeros
// Fill a block in image writer with zeros ( corresponding to an empty block )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSynthesis::FillDataWithZeros( TPrecInt** aImageBlock, CJ2kSubband& aSubband, 
                                       TInt32 aStartRowImage, TInt32 aStartColImage,
                                       TInt32 aCblkHeight, TInt32 aCblkWidth )
    {
    TPrecInt* imageRow = 0; // One row of the image
    TInt32 j = 0;

    TInt32 startRowImageBlock = aStartRowImage; // Start row's index in the image to copy to 
    TInt32 startColImageBlock = aStartColImage; // Start column's index in the image to copy to 

    TUint8 aBandIndex = (TUint8)( aSubband.SubbandType() );

    // Adjust the place where to copy the data according to the subband type 
    // ( i.e. whether we have LL, HL, LH or HH band ).
    if( aBandIndex == 1 )
        {
        startColImageBlock += KWaveletBlockMidPoint;
        }
    else if( aBandIndex == 2 )
        {
        startRowImageBlock += KWaveletBlockMidPoint;
        }
    else if( aBandIndex == 3 )
        {
        startRowImageBlock += KWaveletBlockMidPoint;
        startColImageBlock += KWaveletBlockMidPoint;
        }

    // Compute the end of the copy region
    TInt32 endRowImageBlock = startRowImageBlock + aCblkHeight - 1; //lint !e961    no else is needed here at the end of if...else if
    TInt32 endColImageBlock = startColImageBlock + aCblkWidth;

    // Shift all the samples in the codeblock
    for ( ; endRowImageBlock >= startRowImageBlock; endRowImageBlock-- )
        {
        imageRow = aImageBlock[endRowImageBlock] + endColImageBlock - 1;
        for ( j = endColImageBlock-1; j >= startColImageBlock; j-- )
            {
             *imageRow-- = 0;
            }
        }
    }

