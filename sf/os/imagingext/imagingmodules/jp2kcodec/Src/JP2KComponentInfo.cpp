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
* Description:  CJ2kComponentInfo class used to collect component related
*                information such as precincts size at each resolution level and
*                list of subbands.
*
*/



// INCLUDE FILES
#include "JP2KTileInfo.h"
#include "JP2KImageInfo.h"
#include "JP2KSubband.h"
#include "JP2KComponentInfo.h"

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
// CJ2kComponentInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJ2kComponentInfo* CJ2kComponentInfo::NewLC( CJ2kTileInfo& aTile, TUint16 aIndex )
    {
    CJ2kComponentInfo* self = new ( ELeave ) CJ2kComponentInfo;

    CleanupStack::PushL(self);
    self->ConstructL( aTile, aIndex );

    return self;
    }

// Destructor
CJ2kComponentInfo::~CJ2kComponentInfo()
    {
    delete iRootSubband;
    iRootSubband = 0;

    iPrecinctSizeList.Close();
    iGridList.Close();
    
    // iExponentList and iMantissaList are not owned by this object    
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::LRCPProgressionL
// At each component, parse the bitstream with LRCP progression order
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kComponentInfo::LRCPProgressionL( CJ2kTileInfo& aTile )
    {
    TUint8 incomplete = 0;
    if ( aTile.LastLevelProcessed() <= iNumOfLevels )
        {
        incomplete = iRootSubband->SubbandAt( aTile.LastLevelProcessed() )->LRCPProgressionL( aTile, *this );
        }
    return incomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::RPCLProgressionL
// At each component, parse the bitstream with RPCL progression order
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kComponentInfo::RPCLProgressionL( CJ2kTileInfo& aTile )
    {
    TUint8 incomplete = 0;
    TUint8 level = aTile.LastLevelProcessed();
    TUint8 diff = (TUint8)( iNumOfLevels - level );
    if ( level <= iNumOfLevels )
        {
        TInt trx0 = TJ2kUtils::CeilL( iComponentCanvas.iTl.iX, 1 << diff );
        TInt try0 = TJ2kUtils::CeilL( iComponentCanvas.iTl.iY, 1 << diff );
        const TSizMarker& sizMarker = aTile.ImageInfo().SizMarker();

        if ( ( ( aTile.LastN1Processed() % (TInt)( sizMarker.iYRsiz[aTile.LastComponentProcessed()] * iGridList[level].iHeight ) == 0) ||
             ( ( aTile.LastN1Processed() == aTile.TileCanvas().iTl.iY ) && 
             ( ( try0 << diff) % iGridList[level].iHeight ) ) ) &&
             ( ( aTile.LastN2Processed() % (TInt)( sizMarker.iXRsiz[aTile.LastComponentProcessed()] * iGridList[level].iWidth ) == 0) ||
             ( ( aTile.LastN2Processed() == aTile.TileCanvas().iTl.iX ) && 
             ( ( trx0 << diff ) % iGridList[level].iWidth ) ) ) )
            {
            incomplete = iRootSubband->SubbandAt( level )->RPCLProgressionL( aTile, *this );
            }
        }
    return incomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::CPRLProgressionL
// At each component, parse the bitstream with CPRL progression order
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kComponentInfo::CPRLProgressionL( CJ2kTileInfo& aTile )
    {
    TUint8 incomplete = 0;
    TUint8 level = aTile.LastLevelProcessed();
    TUint8 diff = (TUint8)( iNumOfLevels - level );

    if (level <= iNumOfLevels)
        {
        TInt trx0 = TJ2kUtils::CeilL( iComponentCanvas.iTl.iX, 1 << diff );
        TInt try0 = TJ2kUtils::CeilL( iComponentCanvas.iTl.iY, 1 << diff );
        const TSizMarker& sizMarker = aTile.ImageInfo().SizMarker();

        if ( ( ( aTile.LastN1Processed() % (TInt)( sizMarker.iYRsiz[aTile.LastComponentProcessed()] * iGridList[level].iHeight ) == 0 ) ||
           ( ( aTile.LastN1Processed() == aTile.TileCanvas().iTl.iY ) && 
           ( ( try0 << diff) % iGridList[level].iHeight ) ) ) &&
           ( ( aTile.LastN2Processed() % (TInt)( sizMarker.iXRsiz[aTile.LastComponentProcessed()] * iGridList[level].iWidth ) == 0 ) ||
           ( ( aTile.LastN2Processed() == aTile.TileCanvas().iTl.iX ) && 
           ( ( trx0 << diff) % iGridList[level].iWidth ) ) ) )
            {
            incomplete = iRootSubband->SubbandAt( level )->CPRLProgressionL( aTile, *this );
            }
        }
    return incomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::SubbandAt
// Get the subband at specific resolution level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const CJ2kSubband* CJ2kComponentInfo::SubbandAt( TUint8 aResLevel ) const
    {
    return ( iRootSubband ) ? iRootSubband->SubbandAt( aResLevel ) : 0;
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::NumOfPackets
// Get the number of packets at specific resolution level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CJ2kComponentInfo::NumOfPackets( TUint8 aResLevel ) const
    {
    return (TUint32)( iPrecinctSizeList[aResLevel].iWidth * 
                      iPrecinctSizeList[aResLevel].iHeight );
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::Exponent
// Get the quantization exponnet value for transformation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kComponentInfo::Exponent( TUint16 aIndex ) const
    {
    if ( iQc & 0x01 )
        {
        if ( aIndex > 0 )
            {
            return (TUint8)( ( *iExponentList)[0] - ( ( aIndex - 1 ) / 3 ) );
            }
        }
    return ( *iExponentList )[aIndex];
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::Mantissa
// Get the quantization mantissa value for transformation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint16 CJ2kComponentInfo::Mantissa( TUint16 aIndex ) const
    {
    if ( iQc & 0x01 )
        {
        if ( aIndex > 0 )
            {
            return ( *iMantissaList )[0];
            }
        }
    return ( *iMantissaList )[aIndex];
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::MagnitudeBits
// Get the magnitude bits
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kComponentInfo::MagnitudeBits( TUint16 aIndex ) const
    {
    return (TUint8)( NumGuard() + Exponent( aIndex ) - 1 );
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::ResetLastPacketProcessed
// Reset the last packet processed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kComponentInfo::ResetLastPacketProcessed()
    {
    for ( TUint8 index = 0; index <= iNumOfLevels; ++index )
        {
        iRootSubband->SubbandAt( index )->ResetLastPacketProcessed();
        }
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::CJ2kComponentInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJ2kComponentInfo::CJ2kComponentInfo() : 
    iPrecinctSizeList(4), 
    iGridList(4)
    {
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CJ2kComponentInfo::ConstructL(  CJ2kTileInfo& aTile, TUint16 aIndex  )
    {
    CJ2kImageInfo& imageInfo = CONST_CAST( CJ2kImageInfo&, aTile.ImageInfo() );
    const TSizMarker& sizMarker = imageInfo.SizMarker();
    const TRect& tileCanvas = aTile.TileCanvas();

    iComponentCanvas.iTl = TPoint( TJ2kUtils::CeilL( tileCanvas.iTl.iX, sizMarker.iXRsiz[aIndex] ),
                                   TJ2kUtils::CeilL( tileCanvas.iTl.iY, sizMarker.iYRsiz[aIndex] ) );
    iComponentCanvas.iBr = TPoint( TJ2kUtils::CeilL( tileCanvas.iBr.iX, sizMarker.iXRsiz[aIndex] ),
                                   TJ2kUtils::CeilL( tileCanvas.iBr.iY, sizMarker.iYRsiz[aIndex] ) );

    if ( iComponentCanvas.Width(  ) <= 0 || iComponentCanvas.Height(  ) <= 0 )
        {
        // Empty may be caused by subsampled
        return;
        }

    // Tile part COD > main header COD
    TCODMarker *codMarker = aTile.TileMarker().iCod;
    if ( !codMarker )
        {
        codMarker = CONST_CAST( TCODMarker*, &imageInfo.MainMarker().iCod );
        }

    TCODMarker *cod = 0;
    TCOCMarker *coc = 0;

    // Tile part COC > tile part COD > main header COC > main header COD
    imageInfo.GetCodingStyle( cod, coc, aTile, aIndex );

    iNumOfLevels = ( coc ) ? coc->iNumOfLevels : cod->iNumOfLevels;
    iCodeBlockSiz = ( coc ) ? coc->iCodeBlockSiz : cod->iCodeBlockSiz;
    iCodeBlockStyle = ( coc ) ? coc->iCodeBlockStyle : cod->iCodeBlockStyle;
    iWaveletTransformation = ( coc ) ? coc->iWaveletTransformation : cod->iWaveletTransformation;

    HBufC8 *precinctSiz = ( coc ) ? coc->iPrecinctSiz : cod->iPrecinctSiz;
    iCod = ( coc ) ? (TUint8)( ( codMarker->iScod & 0xfe ) | ( coc->iScoc & 0x01 ) ): cod->iScod;

    TQCDMarker *qcd = 0;
    TQCCMarker *qcc = 0;

    // Tile part QCC > tile part QCD > main header QCC > main header QCD
    imageInfo.GetQuantization( qcd, qcc, aTile, aIndex );

    iQc = ( qcc ) ? qcc->iSqcc : qcd->iSqcd;
    iExponentList = ( qcc ) ? qcc->iExponent : qcd->iExponent;
    iMantissaList = ( qcc ) ? qcc->iMantissa : qcd->iMantissa;

    TRGNMarker *rgn = 0;

    // Tile part RGN > main header RGN
    imageInfo.GetRegion( rgn, aTile, aIndex );
    iRoiShift = ( rgn ) ? rgn->iSPrgn : (TUint8)0;

    TSize& maxBlock = CONST_CAST( TSize&, imageInfo.MaxBlockSize() );
    maxBlock.iWidth = Max( maxBlock.iWidth, iCodeBlockSiz.iWidth );
    maxBlock.iHeight = Max( maxBlock.iHeight, iCodeBlockSiz.iHeight );

    TSize precinct( 0,0 );
    TInt  trx0 = 0;
    TInt  trx1 = 0;
    TInt  try0 = 0;
    TInt  try1 = 0;
    TInt  denom = 0;
    TInt  ppx = 15;
    TInt  ppy = 15;

    for ( TUint8 index = 0; index <= iNumOfLevels; ++index )
        {
        // Resolution level at r = index
        if ( precinctSiz )
            {
            ppx = ( *precinctSiz )[index] & 0x0f;
            ppy = ( ( *precinctSiz )[index] & 0xf0 ) >> 4;
            }
        denom = 1 << ( iNumOfLevels - index );

        trx0 = TJ2kUtils::CeilL( iComponentCanvas.iTl.iX, denom );
        try0 = TJ2kUtils::CeilL( iComponentCanvas.iTl.iY, denom );
        trx1 = TJ2kUtils::CeilL( iComponentCanvas.iBr.iX, denom );
        try1 = TJ2kUtils::CeilL( iComponentCanvas.iBr.iY, denom );

        if ( trx1 > trx0 )
            {
            precinct.iWidth = TJ2kUtils::CeilL( trx1, 1 << ppx ) - TJ2kUtils::FloorL( trx0, 1 << ppx );
            }
        else
            {
            precinct.iWidth = 0;
            }

        if ( try1 > try0 )
            {
            precinct.iHeight = TJ2kUtils::CeilL( try1, 1 << ppy ) - TJ2kUtils::FloorL( try0, 1 << ppy );
            }
        else
            {
            precinct.iHeight = 0;
            }

        User::LeaveIfError( iPrecinctSizeList.Append( precinct ) );

        precinct.iWidth = 1 << ( ppx + iNumOfLevels - index );
        precinct.iHeight = 1 << ( ppy + iNumOfLevels - index );
        User::LeaveIfError( iGridList.Append( precinct ) );

        // Calculate the minimum grid
        if ( iMinGrid.iWidth == 0 || iMinGrid.iWidth > precinct.iWidth ) 
            {
            iMinGrid.iWidth = precinct.iWidth;
            }
        if ( iMinGrid.iHeight == 0 || iMinGrid.iHeight > precinct.iHeight )
            {
            iMinGrid.iHeight = precinct.iHeight;
            }
        }

    if ( imageInfo.Crop() )
        {
        const TRect& crop = imageInfo.CropArea();

        if ( !( crop.iTl.iX >= ( tileCanvas.iBr.iX - ( TInt )sizMarker.iXOsiz ) ||
                crop.iBr.iX <  ( tileCanvas.iTl.iX - ( TInt )sizMarker.iXOsiz ) ||
                crop.iTl.iY >= ( tileCanvas.iBr.iY - ( TInt )sizMarker.iYOsiz ) ||
                crop.iBr.iY <  ( tileCanvas.iTl.iY - ( TInt )sizMarker.iYOsiz ) ) )
            {
            imageInfo.SetTileMask( aTile.SotMarker().iIsot, ETrue );
            }
        else
            {
            // Current tile is not in the cropping area, 
            // just skip setting up the subbands and packets
            return;
            }
        }

    // Build the associated Subband root
    iRootSubband = CJ2kSubband::BuildSubbandTreeL( iNumOfLevels, *this );

    // Build the associated Packet classes
    iRootSubband->BuildPacketsL( *this, precinctSiz, codMarker->iNumOfLayers );
    }

