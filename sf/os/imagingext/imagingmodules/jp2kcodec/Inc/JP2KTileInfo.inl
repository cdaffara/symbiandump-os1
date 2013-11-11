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
* Description:  CJ2kTileInfo class used to collect tile related
*                information such as Tile Part Header, SOT marker and
*                list of components.
*
*/


#ifndef __JP2KTILEINFO_INL__
#define __JP2KTILEINFO_INL__

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kTileInfo::SetTileLength
// Set the tile length.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline void CJ2kTileInfo::SetTileLength( TUint32 aLength )
    {
    iTileLength = aLength;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::TileLength
// Get the tile length.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint32& CJ2kTileInfo::TileLength()
    {
    return iTileLength;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::TileCanvas
// Get the canvas of the tile.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline const TRect& CJ2kTileInfo::TileCanvas() const
    {
    return iTileCanvas;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::NumOfComponents
// Get the number of componnets.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kTileInfo::NumOfComponents() const
    {
    return (TUint16)iComponentList.Count();
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::NumOfLayersPOC
// Get the end of layer to process.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kTileInfo::NumOfLayersPOC() const
    {
    return iNumOfLayersPOC;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::NumOfLevelsPOC
// Get the end of level to process.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kTileInfo::NumOfLevelsPOC() const
    {
    return iNumOfLevelsPOC;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::ComponentAt
// Get the component at specific location.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline const CJ2kComponentInfo& CJ2kTileInfo::ComponentAt( TUint16 aIndex ) const
    {
    return *iComponentList[aIndex];
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::LastLayerProcessed
// Get the last layer processed.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kTileInfo::LastLayerProcessed() const
    {
    return iLastLayer;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::LastLevelProcessed
// Get the last resolution level processed.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kTileInfo::LastLevelProcessed() const
    {
    return iLastLevel;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::LastComponentProcessed
// Get the last component processed.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kTileInfo::LastComponentProcessed() const
    {
    return iLastComponent;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::LastN1Processed
// Get the last position processed in vertical direction.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TInt CJ2kTileInfo::LastN1Processed() const
    {
    return iLastN1;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::LastN2Processed
// Get the last position processed in horizontal direction.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TInt CJ2kTileInfo::LastN2Processed() const
    {
    return iLastN2;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::IncrementLastLayerProcessed
// Increment the last layer processed.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline void CJ2kTileInfo::IncrementLastLayerProcessed()
    {
    ++iLastLayer;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::ResetLastLayerProcessed
// Reset the last layer processed to 0.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline void CJ2kTileInfo::ResetLastLayerProcessed()
    {
    iLastLayer = 0;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::ImageInfo
// Get the reference to the image info.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline const CJ2kImageInfo& CJ2kTileInfo::ImageInfo() const
    {
    return iImageInfo;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::StreamReader
// Get the reference to the stream reader.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline const TJ2kStreamReader& CJ2kTileInfo::StreamReader() const
    {
    return iReader;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::SetSotMarker
// Set the SOT marker.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline void CJ2kTileInfo::SetSotMarker( const TSotMarker &aSotMarker )
    {
    iSot = aSotMarker;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::SotMarker
// Get the SOT marker.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline const TSotMarker& CJ2kTileInfo::SotMarker() const
    {
    return iSot;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::TileMarker
// Get the Tile Part Header.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline const TTileMarker& CJ2kTileInfo::TileMarker() const
    {
    return iTileMarker;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::SetCheckMarker
// Set the SOT/EOC check indicator.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline void CJ2kTileInfo::SetCheckMarker( TUint8 aCheckMarker )
    {
    iCheckMarker = aCheckMarker;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::SetPacketHeaderReader
// Set the packet header reader.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline void CJ2kTileInfo::SetPacketHeaderReader( MJ2kPacketHeaderReader* aReader )
    {
    iPacketHeaderReader = aReader;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::PacketHeaderReader
// Get the packet header reader.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline MJ2kPacketHeaderReader& CJ2kTileInfo::PacketHeaderReader()
    {
    return *iPacketHeaderReader;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::IsFirstTilePart
// Is this the first tile part.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kTileInfo::IsFirstTilePart() const
    {
    return ( iSot.iTPsot == 0 );
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::IsPPT
// Is packet header should be read from PPT marker.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kTileInfo::IsPPT() const
    {
    return ( iPptBuffer != 0 || iTileMarker.iPpt.Count() != 0 );
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::IsPOC
// Is POC marker should be used for progression order.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kTileInfo::IsPOC() const
    {
    return ( iPOC != 0 );
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::IsLastTilePart
// Is this the last tile part.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kTileInfo::IsLastTilePart() const
    {
    return ( iSot.iTNsot != 0 && iSot.iTNsot == ( iSot.iTPsot + 1 ) );
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::IsCheckMarker
// Is SOT/EOC marker should be checked during progression.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kTileInfo::IsCheckMarker() const
    {
    return iCheckMarker;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::IsSpeedup
// Is RLCP/RPCL progression order used.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kTileInfo::IsSpeedup() const
    {
    return ( ProgressionOrder() == EProgression_R_L_C_P ||
             ProgressionOrder() == EProgression_R_P_C_L );
    }

#endif // __JP2KTILEINFO_INL__
