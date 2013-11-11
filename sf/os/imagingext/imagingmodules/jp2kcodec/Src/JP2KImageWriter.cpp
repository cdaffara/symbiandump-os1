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


// INCLUDE FILES
#include <e32math.h>
#include <fbs.h>
#include "JP2KImageUtils.h"
#include "JP2KFormat.h"
#include "JP2KTileInfo.h"
#include "JP2KImageInfo.h"
#include "JP2KSubband.h"
#include "JP2KComponentInfo.h"
#include "JP2KImageWriter.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS
#define INT2BYTE( n ) ( ( n ) < 0 ? (TUint8)0 : ( ( n ) > 255 ? (TUint8)255 : (TUint8)( n ) ) )
#define CLIPINT( n,bitdepth ) ( ( n >= ( 1 << bitdepth ) ) ? ( ( 1<<bitdepth ) - 1 ) : ( n < 0 ) ? 0 : n )
#define CLIP2BITDEPTH( n, maxValue ) ( ( n > maxValue ) ? ( maxValue ) : ( n < 0 ) ? 0 : n )
#define CLIP2RANGE( n, minValue, maxValue ) ( ( n > maxValue ) ? ( maxValue ) : ( n < minValue ) ? minValue : n )

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// LOCAL CONSTANTS AND MACROS



// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CJ2kWriterComponentInfo::~CJ2kWriterComponentInfo()
    {
    iTileStartList.Close();
    FreeData();
    }

// -----------------------------------------------------------------------------
// CJ2kWriterComponentInfo::AllocDataL
// Allocate 2-D data array with a size
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kWriterComponentInfo::AllocDataL( const TSize& aSize )
    {
    FreeData();
    iData = TJ2kUtils::Alloc2DArrayL( aSize.iHeight, aSize.iWidth );
    }

// -----------------------------------------------------------------------------
// CJ2kWriterComponentInfo::FreeData
// Free the 2-D data array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kWriterComponentInfo::FreeData()
    {
    if ( iData )
        {
        TJ2kUtils::Free2DArray( iData );
        iData = 0;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kWriterComponentInfo::Data
// Get the 2-D data array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPrecInt** CJ2kWriterComponentInfo::Data()
    {
    return iData;
    }

// -----------------------------------------------------------------------------
// CJ2kWriterComponentInfo::TileStartAt
// Get the starting point of a tile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPoint& CJ2kWriterComponentInfo::TileStartAt( TUint16 aTileIndex )
    {
    return iTileStartList[aTileIndex];
    }

// -----------------------------------------------------------------------------
// CJ2kWriterComponentInfo::UpdateNextTileStartAtL
// Update the starting point of next tile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kWriterComponentInfo::UpdateNextTileStartAtL( TUint16 aTileIndex, 
                                                     const TSize& aSize,
                                                     CJ2kImageInfo& aImageInfo )
    {
    TUint16 numOfHorizTiles = aImageInfo.NumOfHorizTilesL();
    TUint16 numOfVertTiles  = aImageInfo.NumOfVertTilesL();

    // Calculate the p and q of a tile
    TDiv tDiv = TJ2kUtils::DivL( aTileIndex, numOfHorizTiles );
    if ( tDiv.rem != ( numOfHorizTiles - 1 ) )
        {
        iTileStartList[aTileIndex + 1].iX = iTileStartList[aTileIndex].iX + aSize.iWidth;
        }

    if ( tDiv.quot != ( numOfVertTiles - 1 ) )
        {
        iTileStartList[aTileIndex + numOfHorizTiles].iY = iTileStartList[aTileIndex].iY + aSize.iHeight;
        }
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kImageWriter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJ2kImageWriter* CJ2kImageWriter::NewL( CImageProcessor* aImageProcessor,
                                        CJ2kImageInfo& aImageInfo,
                                        TJ2kInfo& aJ2kInfo )
    {
    CJ2kImageWriter *self = new ( ELeave ) CJ2kImageWriter( aImageProcessor, aImageInfo, aJ2kInfo );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }

// Destructor
CJ2kImageWriter::~CJ2kImageWriter()
    {
    iComponents.ResetAndDestroy();
    
    delete iLinearsRGBLut;
    iLinearsRGBLut = 0;

    User::Free( iGrayTRCLut );
    User::Free( iRedTRCLut );
    User::Free( iGreenTRCLut );
    User::Free( iBlueTRCLut ); 
    User::Free( iMonoPixelBlock );
    User::Free( iColorPixelBlock );
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::WriterComponentAt
// Get the component of the image writer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const CJ2kWriterComponentInfo& CJ2kImageWriter::WriterComponentAt( TUint16 aIndex ) const
    {
    return *iComponents[aIndex];
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::OutputImageL
// Output the image related to the component of the tile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::OutputImageL( CJ2kTileInfo& aTile, TUint16 aComponentIndex )
    {
    TUint8  bitdepth = 0;
    TUint16 c = 0;
    CJ2kComponentInfo& componentInfo = CONST_CAST( CJ2kComponentInfo&, aTile.ComponentAt( aComponentIndex ) );

    TInt16 reducedLevels = (TInt16)( componentInfo.Levels() - iImageInfo.LevelDrop() );
    if ( reducedLevels < 0 )
        {
        reducedLevels = 0;
        }

    CJ2kSubband* subband = CONST_CAST( CJ2kSubband*, componentInfo.SubbandAt( (TUint8)reducedLevels ) );

    if ( subband->SubbandResLevel() != 0 )
        {
        subband = subband->Parent();
        }

    TSize subbandSize = subband->SubbandCanvasSize();
  
    // Note that in case of component truncation color transform is not performed
    if ( iImageInfo.ComponentDrop() )
        {
        iNumComponents = 1;    
        }
    else  // Only perform color transform if we don't drop components
        {
        // Perform the color transfrom
        if ( aComponentIndex == 2 && aTile.ColorTransformation() )
            {
            // Perform the inverse color transform
            if ( aTile.ComponentAt( 0 ).IsReversible() )  // If RCT is used
                {
                PerformInverseRCT( subbandSize );
                }
            else
                {
                PerformInverseICT( subbandSize );
                }
            }
        }

    // Check if palettes are used. If so, get the number of output channels 
    if ( iJ2kInfo.iCMPList.Count() )
        {
        TUint16 numCSComp = iImageInfo.NumOfComponents();

        // Allocate more memory for data if needed
        if ( iNumComponents > numCSComp )
            {
            for ( c = numCSComp; c < iNumComponents; c++ )
                {
                // Allocate memory for component data
                iComponents[c]->AllocDataL( subbandSize );
                }
            }
        // Check if we have all the necessary channels for component mapping
        if ( aComponentIndex == ( numCSComp - 1 ) )
            {
            MapComponentsL( numCSComp, reducedLevels, subbandSize, aTile );
            }
        }

    // We start the output of files:
    if ( iSingleFileOutput )
        {
        if ( iNumComponents == 3 ) // 3 comp to combine
            {
            // Compute the subbandSize from the first component since others might be downsampled.
            TInt16 tempNumLevels = (TUint16)( aTile.ComponentAt( 0 ).Levels() - iImageInfo.LevelDrop() );

            if ( tempNumLevels < 0 )
                {
                tempNumLevels = 0;
                }

            CJ2kSubband* tempSubband = CONST_CAST( CJ2kSubband*, aTile.ComponentAt( 0 ).SubbandAt( (TUint8)tempNumLevels ) );
    
            if ( tempSubband->SubbandResLevel() != 0 )
                {
                tempSubband = tempSubband->Parent();
                }

            subbandSize = tempSubband->SubbandCanvasSize();

            if( !iColorPixelBlock )
                {
                // Allocate memory for block of color pixels
                iColorPixelBlock = STATIC_CAST( TRgb*, User::AllocL( 2 * KPixelsBlock * sizeof( TRgb ) ) );
                }

            CombineOutputFileL( aTile, subbandSize );
            for ( c = 0; c < iNumComponents; c++ )
                {
                iComponents[c]->FreeData();
                }
            }
        else // 1 comp to output
            {
            if ( iImageInfo.ComponentDrop() )
                {
                c = ( TUint16 )( iImageInfo.ComponentDrop() - 1 );
                }
            else
                {
                c = aComponentIndex;
                }

            bitdepth = iImageInfo.DepthOfComponent( c );

            if( !iMonoPixelBlock )
                {
                // Allocate memory for block of grayscale pixels
                iMonoPixelBlock = STATIC_CAST( TUint32*, User::AllocL( KPixelsBlock * sizeof( TUint32 ) ) );
                }

            // Output a single component
            WriteOutputFileL( aTile, c, subbandSize, bitdepth );
            iComponents[c]->FreeData();
            }
        }
    else
        {
        // Write out three independent output files
        if( !iMonoPixelBlock )
            {
            // Allocate memory for block of grayscale pixels
            iMonoPixelBlock = STATIC_CAST( TUint32*, User::AllocL( KPixelsBlock * sizeof( TUint32 ) ) );
            }

        if ( aComponentIndex == 2 && aTile.ColorTransformation() )
            {
            for ( c = 0; c < 3; c++ )
                {
                bitdepth = iImageInfo.DepthOfComponent( c );

                // Output single files 
                WriteOutputFileL( aTile, c, subbandSize, bitdepth );
                iComponents[c]->FreeData();
                }
            }
        else if ( iJ2kInfo.iCMPList.Count() )
            {
            for ( c = 0; c < iNumComponents; c++ ) 
                {
                // Bitdepth is the lowest seven bits plus one for palettes
                bitdepth = ( TUint8 )( ( iJ2kInfo.iPalette.iBList[0] & 0x7f )+1 );

                // Output single files 
                WriteOutputFileL( aTile, c, subbandSize, bitdepth );
                iComponents[c]->FreeData();
                }
            }
        else
            {
            bitdepth = iImageInfo.DepthOfComponent( aComponentIndex );

            // Output only the first component to screen
            if( aComponentIndex == 0 )
                {
                WriteOutputFileL( aTile, aComponentIndex, subbandSize, bitdepth );
                }
            iComponents[aComponentIndex]->FreeData();
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::OutputImageL
// Output the image related to the component of the tile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::OutputImageL( CJ2kTileInfo& aTile, TUint16 aComponentIndex,
                                    const TSize& aSize )
    {
    TUint8  bitdepth = 0;
    TUint16 c = 0;
    CJ2kComponentInfo& componentInfo = CONST_CAST( CJ2kComponentInfo&, aTile.ComponentAt( aComponentIndex ) );

    TInt16 reducedLevels = (TInt16)( componentInfo.Levels() - iImageInfo.LevelDrop() );
    if ( reducedLevels < 0 )
        {
        reducedLevels = 0;
        }

    CJ2kSubband* subband = CONST_CAST( CJ2kSubband*, componentInfo.SubbandAt( (TUint8)reducedLevels ) );
    if ( subband->SubbandResLevel() != 0 )
        {
        subband = subband->Parent();
        }

    TSize subbandSize = aSize;
  
    // Note that in case of component truncation color transform is not performed
    if ( iImageInfo.ComponentDrop() )
        {
        iNumComponents = 1;    
        }
    else  // Only perform color transform if we don't drop components
        {
        // Perform the color transfrom
        if ( aComponentIndex == 2 && aTile.ColorTransformation() )
            {
            // Perform the inverse color transform
            if ( aTile.ComponentAt( 0 ).IsReversible() )  // If RCT is used
                {
                PerformInverseRCT( subbandSize );
                }
            else
                {
                PerformInverseICT( subbandSize );
                }
            }
        }

    // Check if palettes are used. If so, get the number of output channels 
    if ( iJ2kInfo.iCMPList.Count() )
        {
        TUint16 numCSComp = iImageInfo.NumOfComponents();

        // Allocate more memory for data if needed
        if ( iNumComponents > numCSComp )
            {
            for ( c = numCSComp; c < iNumComponents; c++ )
                {
                // Allocate memory for component data
                iComponents[c]->AllocDataL( subbandSize );
                }
            }
        // Check if we have all the necessary channels for component mapping
        if ( aComponentIndex == ( numCSComp - 1 ) )
            {
            MapComponentsL( numCSComp, reducedLevels, subbandSize, aTile );
            }
        }

    // We start the output of files:
    if ( iSingleFileOutput )
        {
        if ( iNumComponents == 3 ) // 3 comp to combine
            {
            if( !iColorPixelBlock )
                {
                // Allocate memory for block of color pixels
                iColorPixelBlock = STATIC_CAST( TRgb*, User::AllocL( 2 * KPixelsBlock * sizeof( TRgb ) ) );
                }

            CombineOutputFileL( aTile, subbandSize );
            for ( c = 0; c < iNumComponents; c++ )
                {
                iComponents[c]->FreeData();
                }
            }
        else // 1 comp to output
            {
            if ( iImageInfo.ComponentDrop() )
                {
                c = (TUint16)( iImageInfo.ComponentDrop() - 1 );
                }
            else
                {
                c = aComponentIndex;
                }

            bitdepth = iImageInfo.DepthOfComponent( c );

            if( !iMonoPixelBlock )
                {
                // Allocate memory for block of grayscale pixels
                iMonoPixelBlock = STATIC_CAST( TUint32*, User::AllocL( KPixelsBlock * sizeof( TUint32 ) ) );
                }

            // Output a single component
            WriteOutputFileL( aTile, c, subbandSize, bitdepth );
            iComponents[c]->FreeData();
            }
        }
    else
        {
        // Write out three independent output files
        if( !iMonoPixelBlock )
            {
            // Allocate memory for block of grayscale pixels
            iMonoPixelBlock = STATIC_CAST( TUint32*, User::AllocL( KPixelsBlock * sizeof( TUint32 ) ) );
            }

        if ( aComponentIndex == 2 && aTile.ColorTransformation() )
            {
            for ( c = 0; c < 3; c++ )
                {
                bitdepth = iImageInfo.DepthOfComponent( c );

                // Output single files 
                WriteOutputFileL( aTile, c, subbandSize, bitdepth );
                iComponents[c]->FreeData();
                }
            }
        else if ( iJ2kInfo.iCMPList.Count() )
            {
            for ( c = 0; c < iNumComponents; c++ ) 
                {
                // Bitdepth is the lowest seven bits plus one for palettes
                bitdepth = (TUint8)( ( iJ2kInfo.iPalette.iBList[0] & 0x7f )+1 );

                // Output single files 
                WriteOutputFileL( aTile, c, subbandSize, bitdepth );
                iComponents[c]->FreeData();
                }
            }
        else
            {
            bitdepth = iImageInfo.DepthOfComponent( aComponentIndex );

            // Output only the first component to screen
            if( aComponentIndex == 0 )
                {
                WriteOutputFileL( aTile, aComponentIndex, subbandSize, bitdepth );
                }

            iComponents[aComponentIndex]->FreeData();
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::OutputBlockL
// Output the image related to the component of the tile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::OutputBlockL( CJ2kTileInfo& aTile, TUint16 aComponentIndex, 
                                    TInt32 aBlockXCoord, TInt32 aBlockYCoord, 
                                    TSize aFirstCompSize, TSize aThisCompSize )
    {
    TUint32 tileIndex = 0;
    CJ2kWriterComponentInfo* currentComponent = iComponents[aComponentIndex];
    TPoint tmpTileStart( 0, 0 );
    TPoint tmpTileStart1( 0, 0 );
    TPoint tmpTileStart2( 0, 0 );

    CJ2kComponentInfo& componentInfo = CONST_CAST( CJ2kComponentInfo&, aTile.ComponentAt( aComponentIndex ) );
    TSize outputSize( 0, 0 );

    tileIndex = aTile.SotMarker().iIsot;

    // Update the subband size ( which will be used to compute output size )
    TInt16 reducedLevels = (TInt16)( componentInfo.Levels() - iImageInfo.LevelDrop() );
    if ( reducedLevels < 0 )
        {

        TInt32 i;
        TInt32 stepSize = 1;

        // Compute the output step size, the stepSize indicates how much more 
        // resolution has to be dropped if the image didn't have enough wavelet
        // levels to begin with. One indicates no extra resolution drop (write
        // each sample) and for each extra drop skip half of the samples, i.e.
        // stepSize is 2^extraLevels in case extra drop is needed.

        // Compute the stepSize
        for ( i = 0; i < (-reducedLevels); i++ )
            {
            // Double the step size for every extra level dropped.
            stepSize *= 2;
            }
        
        // Adjust the block coordinates, so that next block is drawn to the right coordinates
        aBlockXCoord /= stepSize;
        aBlockYCoord /= stepSize;
        
        reducedLevels = 0;
        }

    CJ2kSubband* subband = CONST_CAST( CJ2kSubband*, componentInfo.SubbandAt( (TUint8)reducedLevels ) );
    if ( subband->SubbandResLevel() != 0 )
        {
        subband = subband->Parent();
        }

    // If we are going to combine the output into a single file we must use the first components size and 
    // tile-start values ( other components might be sub sampled ).
    if( iSingleFileOutput && iNumComponents == 3 )
        {
        // Update the tileStartCoordinates
        tmpTileStart = iComponents[0]->TileStartAt( aTile.SotMarker().iIsot );
        iComponents[0]->iTileStartList[tileIndex].iX += aBlockXCoord;
        iComponents[0]->iTileStartList[tileIndex].iY += aBlockYCoord;

        // Also store and update the tileStartCoordinates of the other two components, as they may be output to file
        tmpTileStart1 = iComponents[1]->TileStartAt( aTile.SotMarker().iIsot );
        iComponents[1]->iTileStartList[tileIndex].iX += aBlockXCoord;
        iComponents[1]->iTileStartList[tileIndex].iY += aBlockYCoord;
        tmpTileStart2 = iComponents[2]->TileStartAt( aTile.SotMarker().iIsot );
        iComponents[2]->iTileStartList[tileIndex].iX += aBlockXCoord;
        iComponents[2]->iTileStartList[tileIndex].iY += aBlockYCoord;

        outputSize = aFirstCompSize;
        }
    else
        {
        // Update the tileStartCoordinates
        tmpTileStart = currentComponent->TileStartAt( aTile.SotMarker().iIsot );
        currentComponent->iTileStartList[tileIndex].iX += aBlockXCoord;
        currentComponent->iTileStartList[tileIndex].iY += aBlockYCoord;

        outputSize = aThisCompSize;
        }

    // Call OutputImageL with the changed parameters
    OutputImageL( aTile, aComponentIndex, outputSize );

    // Restore the original values
    if( iSingleFileOutput && iNumComponents == 3 )
        {
        iComponents[0]->iTileStartList[tileIndex] = tmpTileStart;
        iComponents[1]->iTileStartList[tileIndex] = tmpTileStart1;
        iComponents[2]->iTileStartList[tileIndex] = tmpTileStart2;
        }
    else
        {
        currentComponent->iTileStartList[tileIndex] = tmpTileStart;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::SetNewImageProcessor
// Set the image processor of the image write
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::SetNewImageProcessor( CImageProcessor* aImageProcessor )
    {
    iImageProcessor = aImageProcessor;
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::SingleFileOutput
// Get the single output file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kImageWriter::SingleFileOutput() const
    {
    return iSingleFileOutput;
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::CSCode
// Get the EnumCS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kImageWriter::CSCode() const
    {
    return (TUint8)iJ2kInfo.iEnumCS;
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::CJ2kImageWriter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJ2kImageWriter::CJ2kImageWriter( CImageProcessor* aImageProcessor,
                                  CJ2kImageInfo&   aImageInfo,
                                  TJ2kInfo&        aJ2kInfo ) : 
    iImageProcessor( aImageProcessor ), 
    iImageInfo( aImageInfo ), 
    iJ2kInfo( aJ2kInfo )
    {
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::ConstructL()
    {
    TInt32 i = 0;

    TSizMarker& sizMarker = CONST_CAST( TSizMarker&, iImageInfo.SizMarker() );

    iNumComponents = iImageInfo.NumOfComponents();

    if ( iJ2kInfo.iCMPList.Count() )
        {
        iNumComponents = (TUint16)( iJ2kInfo.iCMPList.Count() );

        if(iNumComponents < iImageInfo.NumOfComponents())
            {
            // Every component in the codestream must have a mapping defined
            User::Leave( KErrCorrupt );
            }
        }

    for ( i = 0; i < iNumComponents; i++ )
        {
        CJ2kWriterComponentInfo *info = new ( ELeave ) CJ2kWriterComponentInfo;
        CleanupStack::PushL( info );
        User::LeaveIfError( iComponents.Append( info ) );
        CleanupStack::Pop(1);
        }

    if ( iNumComponents == 3 )
        {
        iSingleFileOutput = 1;
        if ( sizMarker.iXRsiz[1] == 2 * sizMarker.iXRsiz[0] &&
            sizMarker.iXRsiz[2] == 2 * sizMarker.iXRsiz[0] )
            {
            if ( sizMarker.iYRsiz[1] == 2 * sizMarker.iYRsiz[0] &&
                sizMarker.iYRsiz[2] == 2 * sizMarker.iYRsiz[0] )
                {
                iFileType = KYUV420;
                }
            else
                {
                iFileType = KYUV422;
                }
            }
        }
    else if ( iNumComponents == 1 )
        {
        iFileType = KRGB;
        }
  
    if ( iImageInfo.ComponentDrop() )   //lint !e961    no else is needed here at the end of if...else if
        {
        iFileType = KRGB;
        }
  
    if ( iJ2kInfo.iICCProfile )
        {
        iICCProfile = ETrue;
        InitializeICCProfileL();
        }

    // Initialize the output parameters
    if ( iImageInfo.Crop() )
        {
        // Currently do nothing.
        }
    else
        {
        InitializeOutputParametersL();
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::PerformInverseRCT
// Perform the inverse reversible color transformation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::PerformInverseRCT( const TSize& aSize )
    {
    TInt32     col = 0;
    TPrecInt   red = 0;
    TPrecInt   green = 0;
    TPrecInt   blue = 0;
    TPrecInt** block1 = iComponents[0]->iData;
    TPrecInt** block2 = iComponents[1]->iData;
    TPrecInt** block3 = iComponents[2]->iData;

    for ( TInt32 row = 0; row < aSize.iHeight; row++ )
        {
        for ( col = 0; col < aSize.iWidth; col++ )
            {
            red   = block1[row][col];
            green = block2[row][col];
            blue  = block3[row][col];

            block2[row][col] = red - ( ( blue + green ) >> 2 ); //lint !e704 shifting is OK.
            block1[row][col] = blue + block2[row][col];
            block3[row][col] = green + block2[row][col];
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::PerformInverseICT
// Perform the inverse irreversible color transformation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::PerformInverseICT( const TSize& aSize )
    {
    TInt32     col = 0;
    TPrecInt   red = 0;
    TPrecInt   green = 0;
    TPrecInt   blue = 0;
    TPrecInt** block1 = iComponents[0]->iData;
    TPrecInt** block2 = iComponents[1]->iData;
    TPrecInt** block3 = iComponents[2]->iData;

    for ( TInt32 row = 0; row < aSize.iHeight; row++ )
        {
        for ( col = 0; col < aSize.iWidth; col++ )
            {
            red   = block1[row][col];
            green = block2[row][col];
            blue  = block3[row][col];
            InverseICTTransform( red, green, blue, block1[row][col], block2[row][col], block3[row][col] );            
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::InverseICTTransform
// Inverse irreversible color transformation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::InverseICTTransform( TPrecInt aY, TPrecInt aU, TPrecInt aV,
                                           TPrecInt& aR, TPrecInt& aG, TPrecInt& aB )
    {
    aR = ( TPrecInt )( ( ( ( aY << KFractionBits ) + 
                     ( KIctCoefficient11 * aV ) ) + KOffset ) >> KFractionBits ); //lint !e704 shifting is OK.
    aG = ( TPrecInt )( ( ( ( aY << KFractionBits ) + 
                     ( KIctCoefficient21 * aU ) + ( KIctCoefficient22 * aV ) ) + KOffset ) >> KFractionBits ); //lint !e704 shifting is OK.
    aB = ( TPrecInt )( ( ( ( aY << KFractionBits ) + 
                     ( KIctCoefficient31 * aU ) ) + KOffset ) >> KFractionBits ); //lint !e704 shifting is OK.
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::InverseICTTransform
// Inverse irreversible color transformation.
// Performs fast transform and outputs even and odd samples at the same time
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::InverseICTTransform( TPrecInt aY1, TPrecInt aY2, TPrecInt aU, TPrecInt aV,
                                           TPrecInt& aR1, TPrecInt& aG1, TPrecInt& aB1,
                                           TPrecInt& aR2, TPrecInt& aG2, TPrecInt& aB2)
    {
    TInt32 y1Shifted;
    TInt32 y2Shifted;
    TInt32 rTempValue;
    TInt32 gTempValue;
    TInt32 bTempValue;

    y1Shifted = ( aY1 << KFractionBits );
    y2Shifted = ( aY2 << KFractionBits );
    rTempValue = ( KIctCoefficient11 * aV ) + KOffset;
    gTempValue = ( KIctCoefficient21 * aU ) + ( KIctCoefficient22 * aV ) + KOffset;
    bTempValue = ( KIctCoefficient31 * aU ) + KOffset;

    aR1 = ( TPrecInt )( ( y1Shifted + rTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aG1 = ( TPrecInt )( ( y1Shifted + gTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aB1 = ( TPrecInt )( ( y1Shifted + bTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aR2 = ( TPrecInt )( ( y2Shifted + rTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aG2 = ( TPrecInt )( ( y2Shifted + gTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aB2 = ( TPrecInt )( ( y2Shifted + bTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::InverseICTFastYUV420Transform
// Inverse irreversible color transformation
// Performs fast transform and outputs even and odd samples on even and odd
// rows (four samples) at the same time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::InverseICTTransform( TPrecInt aY1, TPrecInt aY2, TPrecInt aY3, TPrecInt aY4, 
                                           TPrecInt aU, TPrecInt aV,
                                           TPrecInt& aR1, TPrecInt& aG1, TPrecInt& aB1,
                                           TPrecInt& aR2, TPrecInt& aG2, TPrecInt& aB2,
                                           TPrecInt& aR3, TPrecInt& aG3, TPrecInt& aB3,
                                           TPrecInt& aR4, TPrecInt& aG4, TPrecInt& aB4)
    {
    TInt32 y1Shifted;
    TInt32 y2Shifted;
    TInt32 y3Shifted;
    TInt32 y4Shifted;
    TInt32 rTempValue;
    TInt32 gTempValue;
    TInt32 bTempValue;

    y1Shifted = ( aY1 << KFractionBits );
    y2Shifted = ( aY2 << KFractionBits );
    y3Shifted = ( aY3 << KFractionBits );
    y4Shifted = ( aY4 << KFractionBits );
    rTempValue = ( KIctCoefficient11 * aV ) + KOffset;
    gTempValue = ( KIctCoefficient21 * aU ) + ( KIctCoefficient22 * aV ) + KOffset;
    bTempValue = ( KIctCoefficient31 * aU ) + KOffset;

    aR1 = ( TPrecInt )( ( y1Shifted + rTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aG1 = ( TPrecInt )( ( y1Shifted + gTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aB1 = ( TPrecInt )( ( y1Shifted + bTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aR2 = ( TPrecInt )( ( y2Shifted + rTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aG2 = ( TPrecInt )( ( y2Shifted + gTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aB2 = ( TPrecInt )( ( y2Shifted + bTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aR3 = ( TPrecInt )( ( y3Shifted + rTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aG3 = ( TPrecInt )( ( y3Shifted + gTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aB3 = ( TPrecInt )( ( y3Shifted + bTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aR4 = ( TPrecInt )( ( y4Shifted + rTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aG4 = ( TPrecInt )( ( y4Shifted + gTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    aB4 = ( TPrecInt )( ( y4Shifted + bTempValue ) >> KFractionBits ); //lint !e704 shifting is OK.
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::InitializeICCProfileL
// Initialize the ICC profile from JP2 file format ( iJ2kInfo )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::InitializeICCProfileL()
    {   
    const TUint8* iterator = iJ2kInfo.iICCProfile->Ptr();
    TUint8* origin = CONST_CAST( TUint8*, iJ2kInfo.iICCProfile->Ptr() );

    // Skip the first 128 bytes
    iterator += KICCSkipBytes;

    // Get the tag count
    TUint32 tagCount = PtrReadUtil::ReadBigEndianUint32Inc( iterator );

    TUint32 tagSignature = 0;
    TUint32 tagSize = 0;
    TUint32 redXYZOffset = 0;
    TUint32 greenXYZOffset = 0;
    TUint32 blueXYZOffset = 0;
    TUint32 trcOffset = 0;
    TUint32 index = 0;
    TInt32 i = 0;
    TInt entries = 0;
    HBufC16* redTRC = 0;
    HBufC16* greenTRC = 0;
    HBufC16* blueTRC = 0;
    HBufC16* grayTRC = 0;
    TUint8 isColor = EFalse;
    TReal value = 0.0;

    for ( index = 0; index < tagCount; ++index )
        {
        tagSignature = PtrReadUtil::ReadBigEndianUint32Inc( iterator );
        switch ( tagSignature )
            {
            case KRedMatrixTag:
                {
                redXYZOffset = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) + 8;
                iterator += 4;
                break;
                }
            case KGreenMatrixTag:
                {
                greenXYZOffset = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) + 8;
                iterator += 4;
                break;
                }
            case KBlueMatrixTag:
                {
                blueXYZOffset = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) + 8;
                iterator += 4;
                break;
                }
            case KRedTrcTag:
                {
                isColor = ETrue;
                trcOffset = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) + 12;
                tagSize = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) - 12;
                entries = tagSize >> 1;
                redTRC = HBufC16::NewLC( entries );
                TPtr16 tmpPtr = ( redTRC->Des() );
                
                // Read bytes into the TRC values array
                for( i = 0; i < entries; i++ )
                    {
                    tmpPtr.Append( (TUint16)( ( origin[trcOffset + 2 * i]  << 8 ) | 
                                                origin[trcOffset + 2 * i + 1] ) );
                    }
                
                break;
                }
            case KGreenTrcTag:
                {
                isColor = ETrue;
                trcOffset = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) + 12;
                tagSize = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) - 12;
                entries = tagSize >> 1;
                greenTRC = HBufC16::NewLC( entries );
                TPtr16 tmpPtr = ( greenTRC->Des() );

                // Read bytes into the TRC values array
                for( i = 0; i < entries; i++ )
                    {
                    tmpPtr.Append( (TUint16)( ( origin[trcOffset + 2 * i]  << 8 ) |
                                                origin[trcOffset + 2 * i + 1] ) );
                    }
                
                break;
                }
            case KBlueTrcTag:
                {
                isColor = ETrue;
                trcOffset = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) + 12;
                tagSize = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) - 12;
                entries = tagSize >> 1;
                blueTRC = HBufC16::NewLC( entries );
                TPtr16 tmpPtr = ( blueTRC->Des() );

                // Read bytes into the TRC values array
                for( i = 0; i < entries; i++ )
                    {
                    tmpPtr.Append( (TUint16)( ( origin[trcOffset+2*i]  << 8 ) | 
                                                origin[trcOffset+2*i+1] ) );
                    }
                
                break;
                }
            case KGrayTrcTag:
                {
                trcOffset = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) + 12;
                tagSize = PtrReadUtil::ReadBigEndianUint32Inc( iterator ) - 12;
                entries = tagSize >> 1;
                grayTRC = HBufC16::NewLC( entries );
                TPtr16 tmpPtr = ( grayTRC->Des() );

                // Read bytes into the TRC values array
                for( i = 0; i < entries; i++ )
                    {
                    tmpPtr.Append( (TUint16)( ( origin[trcOffset + 2 * i]  << 8 ) | 
                                                origin[trcOffset + 2 * i + 1] ) );
                    }
                
                break;
                }
            default:
                {
                iterator += 8;
                break;
                }
            }
        }

    TReal gamma = 0.0;
    TReal maxInput = 0.0;
    TReal src = 0.0;
    TUint32 numPerSample = 0;
    TUint32 lutSize = 0;
    if ( iJ2kInfo.iBPCList.Count() )
        {
        lutSize = 1 << ( iJ2kInfo.iBPCList[0] & 0x7f + 1 );
        }
    else
        {
        lutSize = 1 << iImageInfo.DepthOfComponent( 0 );
        }

    if ( isColor )
        {
        // Read the RGB( lin. ) -> XYZ conversion matrix coefficients
        // The matrix is the following:
        //  _                 _
        // | redX greenX blueX |
        // | redY greenY blueY |
        // | redZ greenZ blueZ |
        // |_                 _|
        //
        TReal redX = ( TReal )PtrReadUtil::ReadBigEndianUint32( origin + redXYZOffset ) / KDivisor;
        TReal redY = ( TReal )PtrReadUtil::ReadBigEndianUint32( origin + redXYZOffset + 4 ) / KDivisor;
        TReal redZ = ( TReal )PtrReadUtil::ReadBigEndianUint32( origin + redXYZOffset + 8 ) / KDivisor;

        TReal greenX = ( TReal )PtrReadUtil::ReadBigEndianUint32( origin + greenXYZOffset ) / KDivisor;
        TReal greenY = ( TReal )PtrReadUtil::ReadBigEndianUint32( origin + greenXYZOffset + 4 ) / KDivisor;
        TReal greenZ = ( TReal )PtrReadUtil::ReadBigEndianUint32( origin + greenXYZOffset + 8 ) / KDivisor;

        TReal blueX = ( TReal )PtrReadUtil::ReadBigEndianUint32( origin + blueXYZOffset ) / KDivisor;
        TReal blueY = ( TReal )PtrReadUtil::ReadBigEndianUint32( origin + blueXYZOffset + 4 ) / KDivisor;
        TReal blueZ = ( TReal )PtrReadUtil::ReadBigEndianUint32( origin + blueXYZOffset + 8 ) / KDivisor;

        // Combine the RGB( lin. ) -> XYZ and the XYZ -> sRGB( lin. ) conversions, i.e. 
        // perform the matrix multiplication. Store the result in "iMatrix".
        iMatrix[0] = ( TInt32 )( KSRGBMaxIntShifted * ( KSRGB00 * redX   + KSRGB01 * redY   + KSRGB02 * redZ ) );
        iMatrix[1] = ( TInt32 )( KSRGBMaxIntShifted * ( KSRGB00 * greenX + KSRGB01 * greenY + KSRGB02 * greenZ ) );
        iMatrix[2] = ( TInt32 )( KSRGBMaxIntShifted * ( KSRGB00 * blueX  + KSRGB01 * blueY  + KSRGB02 * blueZ ) );

        iMatrix[3] = ( TInt32 )( KSRGBMaxIntShifted * ( KSRGB10 * redX   + KSRGB11 * redY   + KSRGB12 * redZ ) );
        iMatrix[4] = ( TInt32 )( KSRGBMaxIntShifted * ( KSRGB10 * greenX + KSRGB11 * greenY + KSRGB12 * greenZ ) );
        iMatrix[5] = ( TInt32 )( KSRGBMaxIntShifted * ( KSRGB10 * blueX  + KSRGB11 * blueY  + KSRGB12 * blueZ ) );

        iMatrix[6] = ( TInt32 )( KSRGBMaxIntShifted * ( KSRGB20 * redX   + KSRGB21 * redY   + KSRGB22 * redZ ) );
        iMatrix[7] = ( TInt32 )( KSRGBMaxIntShifted * ( KSRGB20 * greenX + KSRGB21 * greenY + KSRGB22 * greenZ ) );
        iMatrix[8] = ( TInt32 )( KSRGBMaxIntShifted * ( KSRGB20 * blueX  + KSRGB21 * blueY  + KSRGB22 * blueZ ) );
        
        iRedTRCLut = STATIC_CAST( TInt32*, User::AllocL( lutSize * sizeof( TInt32 ) ) );
        //ASSERT(redTRC);
        User::LeaveIfNull(redTRC);
        if ( redTRC->Length() == 1 )
            {
            gamma = (TReal)( *redTRC )[0] / KGamma;
            maxInput = (TReal)( lutSize - 1 );
            for ( index = 0; index < lutSize; ++index )
                {
                src = index / maxInput;
                Math::Pow( value, src, gamma );
                iRedTRCLut[index] = (TInt32)( value * KTRCShiftMultiplier + 0.5 );
                }
            }
        else
            {
            numPerSample = lutSize / (TUint32)( redTRC->Length() );
            if ( numPerSample )
                {
                for ( index = 0; index < lutSize; ++index )
                    {
                    value = ( *redTRC )[index / numPerSample] / KDivisor;
                    iRedTRCLut[index] = (TInt32)( value * KTRCShiftMultiplier + 0.5 );
                    }
                }
            }

        if ( iJ2kInfo.iBPCList.Count() )
            {
            lutSize = 1 << ( iJ2kInfo.iBPCList[1] & 0x7f + 1 );
            }
        else
            {
            lutSize = 1 << iImageInfo.DepthOfComponent( 1 );
            }

        iGreenTRCLut = STATIC_CAST( TInt32*, User::AllocL( lutSize * sizeof( TInt32 ) ) );
        //ASSERT(greenTRC);
        User::LeaveIfNull(greenTRC);
        if ( greenTRC->Length() == 1 )
            {
            gamma = (TReal)( *greenTRC )[0] / KGamma;
            maxInput = (TReal)( lutSize - 1 );
            for ( index = 0; index < lutSize; ++index )
                {
                src = index / maxInput;
                Math::Pow( value, src, gamma );
                iGreenTRCLut[index] = (TInt32)( value * KTRCShiftMultiplier + 0.5 );
                }
            }
        else
            {
            numPerSample = lutSize / (TUint32)( greenTRC->Length() );
            if ( numPerSample )
                {
                for ( index = 0; index < lutSize; ++index )
                    {
                    value = ( *greenTRC )[index / numPerSample] / KDivisor;
                    iGreenTRCLut[index] = (TInt32)( value * KTRCShiftMultiplier + 0.5 );
                    }
                }
            }

        if ( iJ2kInfo.iBPCList.Count() )
            {
            lutSize = 1 << ( iJ2kInfo.iBPCList[2] & 0x7f + 1 );
            }
        else
            {
            lutSize = 1 << iImageInfo.DepthOfComponent( 2 );
            }

        iBlueTRCLut = STATIC_CAST( TInt32*, User::AllocL( lutSize * sizeof( TInt32 ) ) );
        //ASSERT(blueTRC);
        User::LeaveIfNull(blueTRC);
        if ( blueTRC->Length() == 1 )
            {
            gamma = (TReal)( *blueTRC )[0] / KGamma;
            maxInput = (TReal)( lutSize - 1 );
            for ( index = 0; index < lutSize; ++index )
                {
                src = index / maxInput;
                Math::Pow( value, src, gamma );
                iBlueTRCLut[index] = (TInt32)( value * KTRCShiftMultiplier + 0.5 );
                }
            }
        else
            {
            numPerSample = lutSize / (TUint32)( blueTRC->Length() );
            if ( numPerSample )
                {
                for ( index = 0; index < lutSize; ++index )
                    {
                    value = ( *blueTRC )[index / numPerSample] / KDivisor;
                    iBlueTRCLut[index] = (TInt32)( value * KTRCShiftMultiplier + 0.5 );
                    }
                }
            }
        }
    else
        {

        iGrayTRCLut = STATIC_CAST( TInt32*, User::AllocL( lutSize * sizeof( TInt32 ) ) );
        //ASSERT(grayTRC);
        User::LeaveIfNull(grayTRC);
        if ( grayTRC->Length() == 1 )
            {

            gamma = (TReal)( *grayTRC )[0] / KGamma;
            maxInput = (TReal)( lutSize - 1 );
            for ( index = 0; index < lutSize; ++index )
                {
                src = index / maxInput;
                Math::Pow( value, src, gamma );
                iGrayTRCLut[index] = (TInt32)( value*KSRGBMax+0.5 );
                }
            }
        else
            {
            numPerSample = lutSize / (TUint32)( grayTRC->Length() );
            if ( numPerSample )
                {
                for ( index = 0; index < lutSize; ++index )
                    {
                    value = ( *grayTRC )[index / numPerSample] / KDivisor;
                    iGrayTRCLut[index] = (TInt32)( value*KSRGBMax + 0.5 );
                    }
                }
            }
        }

    TUint16 cutoffIdx = 0;           // Cutoff index for linear portion of output LUT 
    TReal linearSlope = 0.0;         // Slope of output LUT in the linear region 
    TReal normalisation = 0.0;       // Scale factor to normalize sRGB linear value to [0,1]

    // Generate the final output LUT for converting linear sRGB to non-linear sRGB.
    // Input values are in the range [0,KSRGBMax] and output will be 8-bit [0,255].
    // Conversation values can be obtained in e.g. "JPEG2000 - Image compression
    // fundamentals, standards and practice" book by Taubman and Marcellin.
    cutoffIdx = (TUint16)( KSRGB_CUTOFF * KSRGBMax );
    linearSlope = 255.0 * KSRGB_SLOPE / KSRGBMax;
    normalisation = 1.0 / KSRGBMax;
    
    iLinearsRGBLut = HBufC8::NewL( KSRGBMaxInt + 1 );

    // Generate the output lin.sRGB -> non-lin.sRGB LUT
    // Linear part
    for ( index = 0; index <= cutoffIdx; ++index )
        {
        iLinearsRGBLut->Des().Append( (TUint8)( linearSlope * index ) );
        }

    // Non-linear part
    for ( ; index <= (TUint32)KSRGBMaxInt; ++index )
        {
        gamma = index * normalisation;
        Math::Pow( src, gamma , KSRGB_EXPONENT );
        iLinearsRGBLut->Des().Append( (TUint8)( 255 * ( KSRGB_MULTIPLIER * src - KSRGB_SUBTRACT ) ) );
        }

    // We do not need iJ2kInfo.iICCProfile data anymore
    delete iJ2kInfo.iICCProfile;
    iJ2kInfo.iICCProfile = 0;
    
    if ( isColor )
        {
        CleanupStack::PopAndDestroy( 3 );
        }
    else
        {
        CleanupStack::PopAndDestroy( 1 );
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::InitializeOutputParametersL
// Initialize the output parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::InitializeOutputParametersL()
    {
    CJ2kWriterComponentInfo* component = 0;
    TUint16 l = 0;
    TUint16 m = 0;                 
    TUint16 tileIndex = 0;            
    TUint16 tileYIndex = 0;
    TPoint tileStart( 0, 0 );
    TUint16 numOfHorizTiles = iImageInfo.NumOfHorizTilesL();
    TUint16 numOfVertTiles  = iImageInfo.NumOfVertTilesL();
    TRect tileCanvas( 0, 0, 0, 0 );
    TRect componentCanvas( 0, 0, 0, 0 );
    const TSizMarker &sizMarker = iImageInfo.SizMarker();
    TInt  useHeight = 0;
    TInt  useWidth = 0;

    // Prepare the tile coordinates for output image 
    for ( TUint16 compIndex = 0; compIndex < iNumComponents; compIndex++ )
        {    
        component = iComponents[compIndex];
      
        for ( l = 0; l < numOfVertTiles; l++ )
            {
            tileYIndex = (TUint16)( l * numOfHorizTiles );

            for ( m = 0; m < numOfHorizTiles; m++ )
                {
                // Tile index
                tileIndex = (TUint16)( tileYIndex + m );

                // Tile canvas
                tileCanvas.iTl = TPoint( Max( sizMarker.iXTOsiz + m * sizMarker.iXTsiz, sizMarker.iXOsiz ),
                                         Max( sizMarker.iYTOsiz + l * sizMarker.iYTsiz, sizMarker.iYOsiz ) );

                tileCanvas.iBr = TPoint( Min( sizMarker.iXTOsiz + ( m + 1 ) * sizMarker.iXTsiz, sizMarker.iXsiz ),
                                         Min( sizMarker.iYTOsiz + ( l + 1 ) * sizMarker.iYTsiz, sizMarker.iYsiz ) );
                // Component canvas
                componentCanvas.iTl = TPoint( TJ2kUtils::CeilL( tileCanvas.iTl.iX, sizMarker.iXRsiz[compIndex] ),
                                              TJ2kUtils::CeilL( tileCanvas.iTl.iY, sizMarker.iYRsiz[compIndex] ) );

                componentCanvas.iBr = TPoint( TJ2kUtils::CeilL( tileCanvas.iBr.iX, sizMarker.iXRsiz[compIndex] ),
                                              TJ2kUtils::CeilL( tileCanvas.iBr.iY, sizMarker.iYRsiz[compIndex] ) );

                if ( m )
                    {
                    tileStart.iX = component->iTileStartList[tileIndex - 1].iX + useWidth;
                    }
                else
                    {
                    tileStart.iX = 0;
                    }

                // Width to be used on the next horizontal tile
                useWidth = componentCanvas.Width();

                if ( l )          
                    {
                    tileStart.iY = component->iTileStartList[tileIndex - 1].iY + useHeight;
                    useHeight = 0;
                    }
                else
                    {
                    tileStart.iY = 0;
                    }

                User::LeaveIfError( component->iTileStartList.Append( tileStart ) );
                }
            // Height to be used on the next vertical tile
            useHeight = componentCanvas.Height();
            }
        }        
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::DoICCConversion
// Perform XYZ to sRGB conversion with the ICC profile.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::DoICCConversion( TInt32 aX,
                                      TInt32 aY,
                                      TInt32 aZ,
                                      TPrecInt& aR,
                                      TPrecInt& aG,
                                      TPrecInt& aB )
    {
    TInt32 tmpX = 0;
    TInt32 tmpY = 0;
    TInt32 tmpZ = 0;
    TInt32 tmpR = 0;
    TInt32 tmpG = 0;
    TInt32 tmpB = 0;

    // First perform the input linearization 
    tmpX = iRedTRCLut[aX];
    tmpY = iGreenTRCLut[aY];
    tmpZ = iBlueTRCLut[aZ];

    // Then perform the matrix multiplication to obtain the nonlinear RGB 
    tmpR = ( iMatrix[0] * tmpX + iMatrix[1] * tmpY + iMatrix[2] * tmpZ ) >> KICCDownshift; //lint !e704 shifting is OK.
    tmpG = ( iMatrix[3] * tmpX + iMatrix[4] * tmpY + iMatrix[5] * tmpZ ) >> KICCDownshift; //lint !e704 shifting is OK.
    tmpB = ( iMatrix[6] * tmpX + iMatrix[7] * tmpY + iMatrix[8] * tmpZ ) >> KICCDownshift; //lint !e704 shifting is OK.

    // Get rid of values outside the range of legal values 
    tmpR = CLIP2RANGE( tmpR, 0, KSRGBMaxInt );
    tmpG = CLIP2RANGE( tmpG, 0, KSRGBMaxInt );
    tmpB = CLIP2RANGE( tmpB, 0, KSRGBMaxInt );

    // De-linearize the output RGB values 
    aR = ( *iLinearsRGBLut )[tmpR];
    aG = ( *iLinearsRGBLut )[tmpG];
    aB = ( *iLinearsRGBLut )[tmpB];
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::MapToEightBits
// Map data less than 8 bits to 8 bits data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::MapToEightBits( CJ2kWriterComponentInfo& aComponent,
                                      const TSize& aSize,
                                      TUint16 aBitDepth )
    {
    TPrecInt* imageRow;
    TInt32 i = 0;
    TInt32 j = 0;

    // DC-shift is always non zero, since we map signed data to unsigned 
    if ( aBitDepth < 8 )
        {
        TPrecInt upshift = KByteBits - aBitDepth;
        TPrecInt dcShiftUp = 1 << ( KByteBits - 1 );

        for ( i = aSize.iHeight - 1; i >= 0; i-- )
            {
            imageRow = aComponent.Data()[i];
            for ( j = aSize.iWidth - 1; j >= 0; j-- )
                {
                imageRow[j] = ( imageRow[j] * ( 1 << upshift ) ) + dcShiftUp;
                }
            }
        }
    else  // The original bitdepth is more than eight
        {
        TPrecInt dcShift = 1 << ( aBitDepth - 1 );
        TPrecInt downshift = aBitDepth - KByteBits;
        for ( i = aSize.iHeight - 1; i >= 0; --i )
            {
            imageRow = aComponent.Data()[i];
            for ( j = aSize.iWidth - 1; j >= 0; j-- )
                {
                imageRow[j] = ( imageRow[j] + dcShift ) >> downshift; //lint !e704 shifting is OK.
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::MapComponentsL
// Map data using the component mapping box from JP2 file format
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::MapComponentsL( TUint16 aNumCSComp,
                                      TUint16  aReducedLevels,
                                      const TSize& aSize,
                                      CJ2kTileInfo& aTile )
    {
    TInt32 i = 0;
    TInt32 j = 0;
    TInt32 k = 0;              // Indices
    TInt32 compIndex = 0;      // Index of the component in the codestream 
    TInt32 paletteIndex = 0;   // Index of the component in the palette
    TInt32 value = 0;
    TInt32 dcShift = 0;
    TPrecInt*** tempData = 0;  // temporary storage for codestream data
    TUint8 bitdepth = 0;

    CJ2kWriterComponentInfo* componentFrom = 0;
    CJ2kWriterComponentInfo* componentTo = 0;
    CJ2kSubband* subband = 0;

    TSize subbandSize( 0, 0 );

    // First allocate the temporary storage
    tempData = STATIC_CAST( TPrecInt***, User::Alloc( aNumCSComp * sizeof( TPrecInt** ) ) );  
    if ( !tempData )
        {
        User::Leave( KErrNoMemory );
        }
    CleanupStack::PushL( tempData );
    for ( i = 0; i < aNumCSComp; i++ )
        {
        subband = CONST_CAST( CJ2kSubband*, aTile.ComponentAt( (TUint16)i ).SubbandAt( (TUint8)aReducedLevels ) );

        if( subband->SubbandResLevel() != 0 )
            {
            subband = subband->Parent();
            }
        subbandSize = subband->SubbandCanvasSize();

        // Check that this component's size doesn't exceed the output size
        if( subbandSize.iWidth > aSize.iWidth )
            {
            subbandSize.iWidth = aSize.iWidth;
            }
        if( subbandSize.iHeight > aSize.iHeight )
            {
            subbandSize.iHeight = aSize.iHeight;
            }
    
        // Allocate the temporary storage
        tempData[i] = TJ2kUtils::Alloc2DArrayL( subbandSize.iHeight, subbandSize.iWidth );
        CleanupStack::PushL( TCleanupItem( ( TCleanupOperation )&( TJ2kUtils::Free2DArray ),  ( TAny* )tempData[i] ) );
        componentFrom = iComponents[i];

        for ( j = subbandSize.iHeight - 1; j >= 0; j-- )
            {
            Mem::Copy( tempData[i][j], componentFrom->Data()[j], subbandSize.iWidth * sizeof( TPrecInt ) );
            }
        }

    for ( k = 0; k < iNumComponents; k++ )
        {
        // Get the channel in the codestream from which to map
        compIndex = iJ2kInfo.iCMPList[k].iCmp;
        componentFrom = iComponents[compIndex];
    
        subband = CONST_CAST( CJ2kSubband*, aTile.ComponentAt( (TUint16)compIndex ).SubbandAt( (TUint8)aReducedLevels ) );

        if( subband->SubbandResLevel() != 0 )
            {
            subband = subband->Parent();
            }

        subbandSize = subband->SubbandCanvasSize();
    
        // Check that this component's size doesn't exceed the output size
        if( subbandSize.iWidth > aSize.iWidth )
            {
            subbandSize.iWidth = aSize.iWidth;
            }
        if( subbandSize.iHeight > aSize.iHeight )
            {
            subbandSize.iHeight = aSize.iHeight;
            }
    
        // Get the output channel
        componentTo = iComponents[k];
    
        // Check if palette is used for mapping
        if ( iJ2kInfo.iCMPList[k].iMtyp == 1 )
            {
            // Get the right palette channel for mapping
            paletteIndex = iJ2kInfo.iCMPList[k].iPcol;

            // Bitdepth is the lowest seven bits plus one for palettes
            bitdepth = (TUint8)( ( iJ2kInfo.iPalette.iBList[paletteIndex] & 0x7f ) + 1 );
            dcShift = 1 << ( bitdepth - 1 );

            for ( i = 0; i < subbandSize.iHeight; i++ )
                {
                for ( j = 0; j < subbandSize.iWidth; j++ )
                    {
                    // Map the input codestream channel into the output component
                    // using the palette value.
                    value = tempData[compIndex][i][j] + dcShift;
                    if ( value < 0 ) 
                        {
                        value = 0;
                        }
                    if ( value >= iJ2kInfo.iPalette.iC2DArray.Count() )
                        {
                        value = iJ2kInfo.iPalette.iC2DArray.Count() - 1;
                        }

                    componentTo->iData[i][j] = ( *iJ2kInfo.iPalette.iC2DArray[value] )[paletteIndex] - dcShift;
                    }
                }
            }
        else  // Direct mapping 
            {
            for ( i = subbandSize.iHeight - 1; i >= 0; --i )
                {
                for ( j = subbandSize.iWidth - 1; j >= 0; --j )
                    {
                    // Map the input codestream channel into the output component directly.
                    componentTo->iData[i][j] = componentFrom->iData[i][j];
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy( aNumCSComp + 1 );
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::WriteOutputFileL
// Write the component to the single output file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::WriteOutputFileL( CJ2kTileInfo& aTile,
                                       TUint16 aCompIndex,
                                       const TSize& aSize,
                                       TUint16 aBitDepth )
    {
    TPrecInt* imageRow = 0;
    CJ2kWriterComponentInfo* currentComponent = iComponents[aCompIndex];
    TSize outputSize = aSize;

    TUint16 numLevels = aTile.ComponentAt( aCompIndex ).Levels();
    TInt16 tempNumLevels = (TUint16)( numLevels - iImageInfo.LevelDrop() );
   
    TPoint tileStartCoord = currentComponent->TileStartAt( aTile.SotMarker().iIsot );
    currentComponent->UpdateNextTileStartAtL( aTile.SotMarker().iIsot, aSize, iImageInfo );

    if ( tempNumLevels < 0 )
        {
   
        TInt32 i;
        TInt32 stepSize = 1;

        // Compute the output step size, the stepSize indicates how much more 
        // resolution has to be dropped if the image didn't have enough wavelet
        // levels to begin with. One indicates no extra resolution drop (write
        // each sample) and for each extra drop skip half of the samples, i.e.
        // stepSize is 2^extraLevels in case extra drop is needed.

        // Adjust the tile starting points and the stepSize
        for ( i = 0; i < (-tempNumLevels); i++ )
            {
            // Double the step size for every extra level dropped.
            stepSize *= 2;
            }
        
        // Also take care of next tile's starting position
        outputSize.iHeight /= stepSize;
        outputSize.iWidth /= stepSize;
        iComponents[0]->UpdateNextTileStartAtL( aTile.SotMarker().iIsot, outputSize, iImageInfo );
        }

    TInt32 dcShift = 0;
    TInt32 j = 0;
    if ( aBitDepth == 8 )
        {
        if ( !( iImageInfo.SignOfComponent( aCompIndex ) ) )
            {
            dcShift = 1 << ( aBitDepth - 1 );
            }
        }
    
    // Convert with an ICC profile if necessary
    if( iICCProfile )
        {
        TInt32 value = 0;
        TInt32 outputValue = 0;
        
        // Compute the dcShift again, we must have dcShift != 0 for other than 8-bit input.
        dcShift = 1 << ( iImageInfo.DepthOfComponent( 0 )-1 );
        
        for ( TInt32 i = 0; i < outputSize.iHeight; i++ )
            {
            imageRow = currentComponent->Data()[i];
            
            SetPixelPos( tileStartCoord.iX, tileStartCoord.iY +  i );
            for ( j = 0; j < outputSize.iWidth; j++ )
                {
                // Use a clipped value of input sample for an index to the grayscale TRC LUT.
                value = iGrayTRCLut[CLIPINT( ( imageRow[j]+dcShift ), aBitDepth )];
                
                // Get rid of values outside the range of legal values.
                value = CLIP2RANGE( value, 0, KSRGBMaxInt );
                
                // De-linearize the output values.
                outputValue = ( *iLinearsRGBLut )[value];
                
                iMonoPixelBlock[j] = (TUint32)( INT2BYTE( outputValue ) );
                }

            // Flush the row of grayscale pixels to image processor
            iImageProcessor->SetMonoPixels( iMonoPixelBlock, outputSize.iWidth );
            }

        SetPixelPos( tileStartCoord );
        }
    else    // No ICC profile was used.
        {
        
        // Take care of bitdepths != 8
        if ( aBitDepth != 8  )
            {
            MapToEightBits( *currentComponent, outputSize, aBitDepth );
            }
        
        SetPixelPos( tileStartCoord );
        
        for ( TInt32 i = 0; i < outputSize.iHeight; i++ )
            {
            imageRow = currentComponent->Data()[i];
            

            SetPixelPos( tileStartCoord.iX, tileStartCoord.iY +  i );

            for ( j = 0; j < outputSize.iWidth; j++ )
                {
                // Store the value in the RGB "block"
                iMonoPixelBlock[j] = (TUint32)( INT2BYTE( ( imageRow[j] + dcShift ) ) );
                }

            // Flush the row of grayscale pixels to image processor
            iImageProcessor->SetMonoPixels( iMonoPixelBlock, outputSize.iWidth );
            }

        SetPixelPos( tileStartCoord );
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::CombineOutputFileL
// Write all components of the tile to the single output file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::CombineOutputFileL( CJ2kTileInfo& aTile, const TSize& aSize )
    {
    TInt32 i = 0;
    TInt32 j = 0;
    TUint16 numLevels = aTile.ComponentAt( 0 ).Levels();
    TInt16 tempNumLevels = (TUint16)( numLevels - iImageInfo.LevelDrop() );

    TSize outputSize = aSize;
    TPoint& tileStartCoord = iComponents[0]->TileStartAt( aTile.SotMarker().iIsot );
    iComponents[0]->UpdateNextTileStartAtL( aTile.SotMarker().iIsot, outputSize, iImageInfo );

    if ( tempNumLevels < 0 )
        {
        TInt32 stepSize = 1;
    
        // Compute the output step size, the stepSize indicates how much more 
        // resolution has to be dropped if the image didn't have enough wavelet
        // levels to begin with. One indicates no extra resolution drop (write
        // each sample) and for each extra drop skip half of the samples, i.e.
        // stepSize is 2^extraLevels in case extra drop is needed.
        
        // Adjust the tile starting points and the stepSize
        for ( i = 0; i < (-tempNumLevels); i++ )
            {
            // Double the step size for every extra level dropped.
            stepSize *= 2;
            }
        
        // Also take care of next tile's starting position
        outputSize.iHeight /= stepSize;
        outputSize.iWidth /= stepSize;
        iComponents[0]->UpdateNextTileStartAtL( aTile.SotMarker().iIsot, outputSize, iImageInfo );
        
        tempNumLevels = 0;
        }

    TInt32 dcShift = 0;
    if ( iImageInfo.DepthOfComponent( 0 ) == 8 )
        {
        if ( !( iImageInfo.SignOfComponent( 0 ) ) )
            {
            dcShift = 1 << ( iImageInfo.DepthOfComponent( 0 ) - 1 );
            }
        }

    TSize subSamplingSize( 0, 0 );

    // Due to subsampling we might have one more sample than the result of
    // height/2 or width/2 will give thus we have to check whether we an
    // extra row/column .
    //
    if ( iImageInfo.NumOfComponents() == 3 && iJ2kInfo.iEnumCS == 18 ) 
        {
        CJ2kSubband* subband = CONST_CAST( CJ2kSubband*, aTile.ComponentAt( 1 ).SubbandAt( (TUint8)tempNumLevels ) );

        if ( subband->SubbandResLevel() != 0 )
            {
            subband = subband->Parent();
            }

        subSamplingSize = subband->SubbandCanvasSize();

        if ( subSamplingSize.iWidth )
            {
            --subSamplingSize.iWidth;
            }

        if ( subSamplingSize.iHeight )
            {
            --subSamplingSize.iHeight;
            }
        }

    TPrecInt** c1 = iComponents[0]->Data();
    TPrecInt** c2 = iComponents[1]->Data();
    TPrecInt** c3 = iComponents[2]->Data();

    TPrecInt* c1Row = c1[0];
    TPrecInt* c2Row = c2[0];
    TPrecInt* c3Row = c3[0];

    // For taking care of channel definition box order
    if ( iJ2kInfo.iCNList.Count() > 0 )
        {
        for ( i = 0; i < 3; i++ )
            {
            if ( iJ2kInfo.iCNList[i].iAsoc == 1 )
                {
                c1 = iComponents[i]->Data();
                }
            else if ( iJ2kInfo.iCNList[i].iAsoc == 2 )
                {
                c2 = iComponents[i]->Data();
                }
            else if( iJ2kInfo.iCNList[i].iAsoc == 3 )
                {
                c3 = iComponents[i]->Data();
                }
            }   //lint !e961    no else is needed here at the end of if...else if
        }

    // If dithering is needed the following lines should be used:
    //    TInt32 A = KDitherCoeffcient11 + dcShift; 
    //    TInt32 B = KDitherCoeffcient12 + dcShift;  
    //    TInt32 C = KDitherCoeffcient21 + dcShift; 
    //    TInt32 D = KDitherCoeffcient22 + dcShift;
    // Without dithering, use dcShift only.
    //
    TInt32 ditherA = dcShift; 
    TInt32 ditherB = dcShift;  
    TInt32 ditherC = dcShift; 
    TInt32 ditherD = dcShift;

    TUint8 values[6];

    // Convert with the ICC profile values if necessary.
    if( iICCProfile )
        {
        TInt32 x = 0;
        TInt32 y = 0;
        TInt32 z = 0;
        TPrecInt sR = 0;
        TPrecInt sG = 0;
        TPrecInt sB = 0;
        TInt32 maxValue = 0;
        
        // Compute the dcShift again, we must have dcShift != 0 for other than 8-bit input.
        dcShift = 1 << ( iImageInfo.DepthOfComponent( 0 ) - 1 );
        maxValue = ( 1 << iImageInfo.DepthOfComponent( 0 ) ) - 1;   // Maximum value for this bitdepth

        for( i = 0; i < outputSize.iHeight; i++ )
            {
            
            // Set the current coordinate in the output image
            SetPixelPos( tileStartCoord.iX, tileStartCoord.iY +  i );
            
            for( j = 0; j < outputSize.iWidth; j++ )
                {
                x = CLIP2BITDEPTH( ( c1[i][j] + dcShift ),maxValue );
                y = CLIP2BITDEPTH( ( c2[i][j] + dcShift ),maxValue ); 
                z = CLIP2BITDEPTH( ( c3[i][j] + dcShift ),maxValue );
                
                DoICCConversion( x,y,z,sR,sG,sB );
                
                values[0] = INT2BYTE( sB );          
                values[1] = INT2BYTE( sG );
                values[2]= INT2BYTE( sR );
                
                WritePixel( values[2], values[1], values[0] );
                }
            }

        // Set the current coordinate in the output image
        SetPixelPos( tileStartCoord );
        }
    else
        {

        // To speed up output writing (for subsampled samples), compute the number of half the samples
        TInt32 halfOfWidth = outputSize.iWidth / 2;

        // If the bitdepth is not eight, shift values so that output is unsigned eight bit
        for ( i = 0; i < iImageInfo.NumOfComponents(); i++ )
            {
            if ( iImageInfo.DepthOfComponent( ( TUint16 )i ) != 8 )
                {
                MapToEightBits( *iComponents[i], outputSize, iImageInfo.DepthOfComponent( (TUint16)i ) );
                }
            }
        
        // Do the YUV to RGB conversion if needed
        if ( iJ2kInfo.iEnumCS == 18 )
            {
            TPrecInt y1 = 0;
            TPrecInt y2 = 0;
            TPrecInt u = 0;
            TPrecInt v = 0;
            TPrecInt r = 0;
            TPrecInt g = 0;
            TPrecInt b = 0;
            TPrecInt r1 = 0;
            TPrecInt g1 = 0;
            TPrecInt b1 = 0;
            TPrecInt r2 = 0;
            TPrecInt g2 = 0;
            TPrecInt b2 = 0;
            
            if ( iFileType == KYUV420 ) // YUV 4:2:0
                {

                TPrecInt* c1EvenRow = c1[0];
                TPrecInt* c1OddRow = c1[0];
                TPrecInt y3 = 0;
                TPrecInt y4 = 0;
                TPrecInt r3 = 0;
                TPrecInt g3 = 0;
                TPrecInt b3 = 0;
                TPrecInt r4 = 0;
                TPrecInt g4 = 0;
                TPrecInt b4 = 0;

                for ( i = 0; i < outputSize.iHeight / 2; i++ )
                    { 
                    SetPixelPos( tileStartCoord.iX, ( tileStartCoord.iY + 2 * i ) );
                    
                    c1EvenRow = c1[2 * i];
                    c1OddRow = c1[2 * i + 1];
                    c2Row = c2[i];
                    c3Row = c3[i];

                    for ( j = 0; j < halfOfWidth; j++ )
                        {
                        y1 = *c1EvenRow++;
                        y2 = *c1EvenRow++;
                        y3 = *c1OddRow++;
                        y4 = *c1OddRow++;
                        u = *c2Row++; 
                        v = *c3Row++;        
                        
                        InverseICTTransform(y1,y2,y3,y4,u,v,r1,g1,b1,r2,g2,b2,r3,g3,b3,r4,g4,b4);

                        // Even rows, even columns
                        values[0] = INT2BYTE( ( r1 + ditherA ) );          
                        values[1] = INT2BYTE( ( g1 + ditherA ) );
                        values[2] = INT2BYTE( ( b1 + ditherA ) );
                        
                        // Even rows, odd columns:
                        values[3] = INT2BYTE( ( r2 + ditherB ) );          
                        values[4] = INT2BYTE( ( g2 + ditherB ) );
                        values[5] = INT2BYTE( ( b2 + ditherB ) );  
                        
                        // Store the values in the RGB "block"
                        iColorPixelBlock[2 * j] = TRgb( values[0], values[1], values[2] );
                        iColorPixelBlock[2 * j + 1] = TRgb( values[3], values[4], values[5] );

                        // Now the odd row:
                        // Odd rows, even column
                        values[0] = INT2BYTE( ( r3 + ditherC ) );          
                        values[1] = INT2BYTE( ( g3 + ditherC ) );
                        values[2] = INT2BYTE( ( b3 + ditherC ) );
                        
                        // Odd rows, odd columns:
                        values[3] = INT2BYTE( ( r4 + ditherD ) );          
                        values[4] = INT2BYTE( ( g4 + ditherD ) );
                        values[5] = INT2BYTE( ( b4 + ditherD ) );  
                    
                        // Store the values in the RGB "block"
                        iColorPixelBlock[KPixelsBlock + 2 * j] = TRgb( values[0], values[1], values[2] );
                        iColorPixelBlock[KPixelsBlock + 2 * j + 1] = TRgb( values[3], values[4], values[5] );
                        }
                    
                    if ( outputSize.iWidth & 1 ) // If the width is odd:
                        {
                        // The even row:
                        y1 = c1[2 * i][outputSize.iWidth - 1];
                        // And the odd row:
                        y2 = c1[2 * i + 1][outputSize.iWidth - 1];
                        u  = c2[i][subSamplingSize.iWidth];
                        v  = c3[i][subSamplingSize.iWidth];
                        
                        InverseICTTransform(y1,y2,u,v,r1,g1,b1,r2,g2,b2); 

                        values[0] = INT2BYTE( ( r1 + ditherA ) );          
                        values[1] = INT2BYTE( ( g1 + ditherA ) );
                        values[2] = INT2BYTE( ( b1 + ditherA ) );
                        
                        // Store the value in the RGB "block"
                        iColorPixelBlock[outputSize.iWidth - 1] = TRgb( values[0], values[1], values[2] );

                        values[0] = INT2BYTE( ( r2 + ditherC ) );
                        values[1] = INT2BYTE( ( g2 + ditherC ) );
                        values[2] = INT2BYTE( ( b2 + ditherC ) );
                        
                        // Store the value in the RGB "block"
                        iColorPixelBlock[KPixelsBlock + outputSize.iWidth - 1] = TRgb( values[0], values[1], values[2] );
                        }
                    
                    // Flush the row of color pixels to image processor
                    iImageProcessor->SetPixels( iColorPixelBlock, outputSize.iWidth );

                    // Duplicated processing for the odd rows
                    SetPixelPos( tileStartCoord.iX, ( tileStartCoord.iY + 2 * i + 1 ) );

                    // Flush the row of color pixels to image processor
                    iImageProcessor->SetPixels( (iColorPixelBlock + KPixelsBlock), outputSize.iWidth );

                    } // End of loop on rows
                
                if ( outputSize.iHeight & 1 )  // If the height is odd:
                    { 
                    SetPixelPos( tileStartCoord.iX, ( tileStartCoord.iY + outputSize.iHeight - 1 ) );
                    
                    c1Row = c1[outputSize.iHeight - 1];
                    c2Row = c2[subSamplingSize.iHeight];
                    c3Row = c3[subSamplingSize.iHeight];

                    for ( i = 0; i < halfOfWidth; i++ ) 
                        {
                        y1 = *c1Row++;  
                        y2 = *c1Row++; 
                        u = *c2Row++;
                        v = *c3Row++;
                        
                        InverseICTTransform(y1,y2,u,v,r1,g1,b1,r2,g2,b2); 

                        values[0] = INT2BYTE( ( r1 + ditherA ) );          
                        values[1] = INT2BYTE( ( g1 + ditherA ) );
                        values[2] = INT2BYTE( ( b1 + ditherA ) );
                        
                        values[3] = INT2BYTE( ( r2 + ditherB ) );          
                        values[4] = INT2BYTE( ( g2 + ditherB ) );
                        values[5] = INT2BYTE( ( b2 + ditherB ) );  
                        
                        // Store the value in the RGB "block"
                        iColorPixelBlock[2 * i] = TRgb( values[0], values[1], values[2] );
                        iColorPixelBlock[2 * i + 1] = TRgb( values[3], values[4], values[5] );
                        }
                    
                    if ( outputSize.iWidth & 1 ) // if the width is odd:
                        {
                        y1 = c1[outputSize.iHeight - 1][outputSize.iWidth - 1];
                        u = c2[subSamplingSize.iHeight][subSamplingSize.iWidth];
                        v = c3[subSamplingSize.iHeight][subSamplingSize.iWidth];
                        
                        InverseICTTransform( y1, u, v, r, g, b );
                        values[0] = INT2BYTE( ( r + ditherA ) );          
                        values[1] = INT2BYTE( ( g + ditherA ) );
                        values[2] = INT2BYTE( ( b + ditherA ) );
                        
                        // Store the value in the RGB "block"
                        iColorPixelBlock[outputSize.iWidth - 1] = TRgb( values[0], values[1], values[2] );
                        }

                    // Flush the row of color pixels to image processor
                    iImageProcessor->SetPixels( iColorPixelBlock, outputSize.iWidth );

                    }

                SetPixelPos( tileStartCoord );
                } 
            else if ( iFileType == KYUV422 )
                {

                for ( i = 0; i < outputSize.iHeight; i++ )
                    {
                    SetPixelPos( tileStartCoord.iX, ( tileStartCoord.iY + 2 * i ) );

                    c1Row = c1[i];
                    c2Row = c2[i];
                    c3Row = c3[i];

                    for ( j = 0; j < halfOfWidth; j++ )
                        {
                        y1 = *c1Row++;
                        y2 = *c1Row++;
                        u = c2Row[j]; 
                        v = c3Row[j];      

                        InverseICTTransform(y1,y2,u,v,r1,g1,b1,r2,g2,b2); 

                        values[0] = INT2BYTE( ( r1 + dcShift ) );          
                        values[1] = INT2BYTE( ( g1 + dcShift ) );
                        values[2] = INT2BYTE( ( b1 + dcShift ) );
                        
                        values[3] = INT2BYTE( ( r2 + dcShift ) );          
                        values[4] = INT2BYTE( ( g2 + dcShift ) );
                        values[5] = INT2BYTE( ( b2 + dcShift ) );  
                        
                        // Store the value in the RGB "block"
                        iColorPixelBlock[j] = TRgb( values[0], values[1], values[2] );
                        iColorPixelBlock[j+1] = TRgb( values[3], values[4], values[5] );
                        }
                    
                    if ( outputSize.iWidth & 1 ) // if the width is odd:
                        {
                        y1 = c1Row[outputSize.iWidth - 1];
                        u  = c2Row[subSamplingSize.iWidth];
                        v  = c3Row[subSamplingSize.iWidth];
                        
                        InverseICTTransform( y1, u, v, r, g, b );
                        
                        values[0] = INT2BYTE( ( r + dcShift ) );          
                        values[1] = INT2BYTE( ( g + dcShift ) );
                        values[2] = INT2BYTE( ( b + dcShift ) );
                        
                        // Store the value in the RGB "block"
                        iColorPixelBlock[outputSize.iWidth-1] = TRgb( values[0], values[1], values[2] );
                        }
                    
                    // Flush the row of color pixels to image processor
                    iImageProcessor->SetPixels( iColorPixelBlock, outputSize.iWidth );
                    }

                SetPixelPos( tileStartCoord );
                }
            else // To take care of YUV 4:4:4
                {

                for ( i = 0; i < outputSize.iHeight; i++ )
                    {
                    SetPixelPos( tileStartCoord.iX, ( tileStartCoord.iY + i ) );

                    c1Row = c1[i];
                    c2Row = c2[i];
                    c3Row = c3[i];

                    for ( j = 0; j < outputSize.iWidth; j++ )
                        {       
                        y1 = *c1Row++;
                        u = *c2Row++; 
                        v = *c3Row++;
                        
                        InverseICTTransform( y1, u, v, r, g, b );
                        
                        values[0] = INT2BYTE( ( r + dcShift ) );          
                        values[1] = INT2BYTE( ( g + dcShift ) );
                        values[2] = INT2BYTE( ( b + dcShift ) );
                        
                        // Store the value in the RGB "block"
                        iColorPixelBlock[j] = TRgb( values[0], values[1], values[2] );
                        }

                    // Flush the row of color pixels to image processor
                    iImageProcessor->SetPixels( iColorPixelBlock, outputSize.iWidth );
                    }

                SetPixelPos( tileStartCoord );
                }
            }
        else // RGB 
            {
            TInt32 r = 0;
            TInt32 g = 0;
            TInt32 b = 0;

            for ( i = 0; i < outputSize.iHeight; i++ )
                {

                SetPixelPos( tileStartCoord.iX, tileStartCoord.iY +  i );

                c1Row = c1[i];
                c2Row = c2[i];
                c3Row = c3[i];

                for ( j = 0; j < outputSize.iWidth; j++ )
                    {
                    r = *c1Row++; 
                    g = *c2Row++; 
                    b = *c3Row++; 
                    
                    values[0] = INT2BYTE( r + dcShift );
                    values[1] = INT2BYTE( g + dcShift );
                    values[2] = INT2BYTE( b + dcShift );
                    
                    // Store the value in the RGB "block"
                    iColorPixelBlock[j] = TRgb( values[0], values[1], values[2] );
                    }

                // Flush the row of color pixels to image processor
                iImageProcessor->SetPixels( iColorPixelBlock, outputSize.iWidth );
                }

            SetPixelPos( tileStartCoord );
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::WritePixel
// Write out a color pixel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::WritePixel( TUint8 aR, TUint8 aG, TUint8 aB )
    {
    iImageProcessor->SetPixel( TRgb( aR, aG, aB ) );
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::WritePixel
// Write out a grayscale pixel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::WritePixel( TUint8 aGray256 )
    {
    iImageProcessor->SetMonoPixel( aGray256 );
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::SetPixelPos
// Set the position of the pixel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::SetPixelPos( const TPoint& aPosition )
    {
    iImageProcessor->SetPos( aPosition );
    }

// -----------------------------------------------------------------------------
// CJ2kImageWriter::SetPixelPos
// Set the position of the pixel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageWriter::SetPixelPos( const TInt aX, const TInt aY )
    {
    iImageProcessor->SetPos( TPoint( aX, aY ) );
    }

