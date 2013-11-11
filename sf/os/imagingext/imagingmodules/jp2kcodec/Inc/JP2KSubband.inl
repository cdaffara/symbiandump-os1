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
* Description:  JP2KSubband class used to collect the subband related
*                information such as list of packets and list of subbands.
*
*/



#ifndef __JP2KSUBBAND_INL__
#define __JP2KSUBBAND_INL__

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kSubband::Parent
// Get the parent subband of the current subband
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline CJ2kSubband* CJ2kSubband::Parent()
    {
    return iParent;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SetParent
// Set the parent subband of the current subband
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kSubband::SetParent( CJ2kSubband* aParent )
    {
    iParent = aParent;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SetSubbandLevel
// Set the subband level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kSubband::SetSubbandLevel( TUint8 aLevel )
    {
    iLevel = aLevel;
    }


// -----------------------------------------------------------------------------
// CJ2kSubband::SubbandType
// Get the subband type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline CJ2kSubband::TSubbandType CJ2kSubband::SubbandType() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SubbandLevel
// Get the subband level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kSubband::SubbandLevel() const
    {
    return iLevel;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SubbandResLevel
// Get the subband resolution level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kSubband::SubbandResLevel() const
    {
    return iResLevel;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SubbandGain
// Get the subband gain
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kSubband::SubbandGain() const
    {
    return iGain;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::HighPassFirst
// Get the subband high pass first
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TPoint& CJ2kSubband::HighPassFirst() const
    {
    return iHighPassFirst;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SubbandOrigin
// Get the subband origin
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TPoint& CJ2kSubband::SubbandOrigin() const
    {
    return iSubbandOrigin;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SubbandCanvas
// Get the subband canvas
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TRect& CJ2kSubband::SubbandCanvas() const
    {
    return iSubbandCanvas;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SubbandCanvasSize
// Get the subband canvas size
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TSize CJ2kSubband::SubbandCanvasSize() const
    {
    return iSubbandCanvas.Size();
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::PacketList
// Get the list of packets
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const RPointerArray<CJ2kPacket>& CJ2kSubband::PacketList() const
    {
    return iPacketList;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::LastPacketProcessed
// Get the last packet processed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kSubband::LastPacketProcessed() const
    {
    return iLastPacket;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::ResetLastPacketProcessed
// Reset the last packet processed to 0
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kSubband::ResetLastPacketProcessed()
    {
    iLastPacket = 0;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SetLastSubbandProcessed
// Set the last subband processed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kSubband::SetLastSubbandProcessed( TUint8 aSubbandType )
    {
    iLastSubband = aSubbandType;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::LastSubbandProcessed
// Get the last subband processed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kSubband::LastSubbandProcessed() const
    {
    return iLastSubband;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::PacketAt
// Get the packet at specific location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const CJ2kPacket& CJ2kSubband::PacketAt( TUint16 aIndex ) const
    {
    return *iPacketList[aIndex];
    }

#endif // __JP2KSUBBAND_INL__
