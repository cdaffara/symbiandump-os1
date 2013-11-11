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
* Description:  CJ2kImageInfo class used to collect image related
*                information such as Main Header, SIZ marker and
*                list of tiles.
*
*/


#ifndef __JP2KIMAGEINFO_INL__
#define __JP2KIMAGEINFO_INL__

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kImageInfo::NumOfHorizTilesL
// Get total number of tiles in horizontal position
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kImageInfo::NumOfHorizTilesL() const
    {
    return (TUint16)( TJ2kUtils::CeilL( iSizMarker.iXsiz - iSizMarker.iXTOsiz,
                                       iSizMarker.iXTsiz ) );
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::NumOfVertTilesL
// Get total number of tiles in vertical position
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kImageInfo::NumOfVertTilesL() const
    {
    return (TUint16)( TJ2kUtils::CeilL( iSizMarker.iYsiz - iSizMarker.iYTOsiz,
                                       iSizMarker.iYTsiz ) );
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::NumOfComponents
// Get total number of components
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kImageInfo::NumOfComponents() const
    {
    return iSizMarker.iCsiz;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::DepthOfComponent
// Get the bit depth of a component
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kImageInfo::DepthOfComponent( TUint16 aIndex ) const
    {
    return ( aIndex < iSizMarker.iCsiz ) ? 
           (TUint8)( ( iSizMarker.iSsiz[aIndex] & 0x7f ) + 1 ) :
           (TUint8)0;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::SignOfComponent
// Get the sign bit of a component
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kImageInfo::SignOfComponent( TUint16 aIndex ) const
    {
    return ( aIndex < iSizMarker.iCsiz ) ? 
           (TUint8)( iSizMarker.iSsiz[aIndex] & 0x80 ) :
           (TUint8)0;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::SizMarker
// Get the SIZ marker
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TSizMarker& CJ2kImageInfo::SizMarker() const
    {
    return iSizMarker;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::MainMarker
// Get the Main Header marker
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TMainMarker& CJ2kImageInfo::MainMarker() const
    {
    return iMainMarker;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::MaxBlockSize
// Get the maximum block size
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TSize& CJ2kImageInfo::MaxBlockSize() const
    {
    return iMaxBlockSize;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::CropArea
// Get the cropped area
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TRect& CJ2kImageInfo::CropArea() const
    {
    return iCropArea;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::TileCount
// Get the total number of tiles in the tile list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kImageInfo::TileCount() const
    {
    return (TUint16)( iTile ? 1 : 0 );
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::TileAt
// Get the tile at specific location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const CJ2kTileInfo& CJ2kImageInfo::TileAt( TUint16 /*aIndex*/ ) const
    {
    return *iTile;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::Append
// Append tile into the tile list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kImageInfo::Append( CJ2kTileInfo *aTile )
    {
    iTile = aTile; 
    iTileMask = 0;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::Remove
// Remove a tile at specific location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kImageInfo::Remove( TUint16 /*aIndex*/ )
    {
    delete iTile; 
    iTile = 0;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::TileMaskAt
// Get the mask of a tile at specific location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kImageInfo::TileMaskAt( TUint16 /*aIndex*/ ) const
    {
    return iTileMask;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::SetTileMask
// Set the mask of a tile at specific location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kImageInfo::SetTileMask( TUint16 /*aIndex*/, TUint8 aTileMask )
    {
    iTileMask = aTileMask;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::SetLevelDrop
// Set the resolution level to drop
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kImageInfo::SetLevelDrop( TUint8 aDrop )
    {
    iLevelDrop = aDrop;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::SetExtraLevelDrop
// Set the extra resolution level to drop
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kImageInfo::SetExtraLevelDrop( TUint8 aDrop )
    {
    iExtraLevelDrop = aDrop;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::ComponentDrop
// Get the component to drop
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kImageInfo::ComponentDrop() const
    {
    return iComponentDrop;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::LevelDrop
// Get the resolution level to drop
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kImageInfo::LevelDrop() const
    {
    return iLevelDrop;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::ExtraLevelDrop
// Get the extra resolution level to drop
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kImageInfo::ExtraLevelDrop() const
    {
    return iExtraLevelDrop;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::Crop
// Get the crop
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kImageInfo::Crop() const
    {
    return iCrop;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::SetLastTilePartProcessed
// Set the last tile part processed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kImageInfo::SetLastTilePartProcessed( TUint16 aLastTilePart )
    {
    iLastTilePartProcessed = aLastTilePart;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::IncrementLastTilePartProcessed
// Increment the counter for tracking the last tile part processed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kImageInfo::IncrementLastTilePartProcessed()
    {
    ++iLastTilePartProcessed;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::ResetPPM
// Reset packet header pointer to the end of the PPM marker
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kImageInfo::ResetPPM()
    {
    if ( iPpm )
        {
        iPpm->iPtr = iPpm->iPtrEnd;
        }
    }

#endif // __JP2KIMAGEINFO_INL__
