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


// INCLUDE FILES
#include <icl/imagecodec.h>
#include "JP2KImageUtils.h"
#include "JP2KFormat.h"
#include "JP2KStreamReader.h"
#include "JP2KTileInfo.h"
#include "JP2KImageInfo.h"
#include "JP2KCodec.h"
#include "JP2KPacket.h"
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

// Destructor
CJ2kSubband::~CJ2kSubband()
    {
    iPacketList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::BuildSubbandTreeL
// Static method to set up the tree structure of the DWT subbands.
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
CJ2kSubband* CJ2kSubband::BuildSubbandTreeL( TUint8 aMaxLevel, CJ2kComponentInfo& aComponent )
    {
    CJ2kSubband *root = CJ2kSubbandLL::NewLC();
    root->iLevel = 0;

    // The root subband, LLO, original image
    root->iResLevel = (TUint8)( aMaxLevel + 1 ); 
    root->iSubbandCanvas = aComponent.ComponentCanvas();
    root->iHighPassFirst = TPoint( root->iSubbandCanvas.iTl.iX % 2,
                                   root->iSubbandCanvas.iTl.iY % 2 );
    CJ2kSubband *start = root;
    CJ2kSubband *tmpLL = 0;
    CJ2kSubband *tmpHL = 0;
    CJ2kSubband *tmpLH = 0;
    CJ2kSubband *tmpHH = 0;

    for ( TUint8 index = 1; index <= aMaxLevel; ++index )
        {
        // Must be built in the following order - LL, HL, LH, and HH
        // where all of them share the same parent - next lower LL
        // but only the LL subband can be further sub-divided into
        // lower resolution subband
        // create LL subband
        TUint8 highPassFirstX = (TUint8)( start->iHighPassFirst.iX );
        TUint8 highPassFirstY = (TUint8)( start->iHighPassFirst.iY );

        tmpLL = CJ2kSubbandLL::NewLC();
        tmpLL->iLevel = index;
        tmpLL->iResLevel = (TUint8)( aMaxLevel - index + 1 );

        tmpLL->iSubbandCanvas.iTl = TPoint( (TUint32)( start->iSubbandCanvas.iTl.iX + 1 ) >> 1,
                                            (TUint32)( start->iSubbandCanvas.iTl.iY + 1 ) >> 1 );

        tmpLL->iSubbandCanvas.SetWidth( ( start->iSubbandCanvas.Width() + ( 1-highPassFirstX ) ) / 2 );

        tmpLL->iSubbandCanvas.SetHeight( ( start->iSubbandCanvas.Height() + ( 1-highPassFirstY ) ) / 2 );

        tmpLL->iHighPassFirst = TPoint( tmpLL->iSubbandCanvas.iTl.iX % 2,
                                        tmpLL->iSubbandCanvas.iTl.iY % 2 );
        start->AddChildL( tmpLL );
        CleanupStack::Pop();

        // Create HL subband
        tmpHL = CJ2kSubbandNLL::NewLC();
        tmpHL->iType = EBandHL;
        tmpHL->iLevel = index;
        tmpHL->iGain  = 1;
        tmpHL->iResLevel = tmpLL->iResLevel;
        tmpHL->iSubbandCanvas.iTl = TPoint( (TUint32)start->iSubbandCanvas.iTl.iX >> 1,
                                            (TUint32)( start->iSubbandCanvas.iTl.iY + 1 ) >> 1 );
        tmpHL->iSubbandCanvas.SetWidth( ( start->iSubbandCanvas.Width() + highPassFirstX ) / 2 );
        tmpHL->iSubbandCanvas.SetHeight( tmpLL->iSubbandCanvas.Height() );
        tmpHL->iSubbandOrigin.iX = start->iSubbandOrigin.iX + ( ( start->iSubbandCanvas.Width() + ( 1-highPassFirstX ) ) / 2 );
        tmpHL->iSubbandOrigin.iY = start->iSubbandOrigin.iY;
        start->AddChildL( tmpHL );
        CleanupStack::Pop();

        // Create LH subband
        tmpLH = CJ2kSubbandNLL::NewLC();
        tmpLH->iType = EBandLH;
        tmpLH->iLevel = index;
        tmpLH->iGain  = 1;
        tmpLH->iResLevel = tmpLL->iResLevel;
        tmpLH->iSubbandCanvas.iTl = TPoint( (TUint32)( start->iSubbandCanvas.iTl.iX + 1 ) >> 1,
                                           (TUint32)start->iSubbandCanvas.iTl.iY >> 1 );
        tmpLH->iSubbandCanvas.SetWidth( tmpLL->iSubbandCanvas.Width() );

        tmpLH->iSubbandCanvas.SetHeight( ( start->iSubbandCanvas.Height() + highPassFirstY ) / 2 );
        tmpLH->iSubbandOrigin.iX = start->iSubbandOrigin.iX;

        tmpLH->iSubbandOrigin.iY = start->iSubbandOrigin.iY + ( ( start->iSubbandCanvas.Height() + ( 1-highPassFirstY ) ) / 2 );
        start->AddChildL( tmpLH );
        CleanupStack::Pop();

        // Create HH subband
        tmpHH = CJ2kSubbandNLL::NewLC();
        tmpHH->iType = EBandHH;
        tmpHH->iLevel = index;
        tmpHH->iGain  = 2;
        tmpHH->iResLevel = tmpLL->iResLevel;
        tmpHH->iSubbandCanvas.iTl = TPoint( ( TUint32 )start->iSubbandCanvas.iTl.iX >> 1,
                                           ( TUint32 )start->iSubbandCanvas.iTl.iY >> 1 );
        tmpHH->iSubbandCanvas.SetWidth( tmpHL->iSubbandCanvas.Width() );
        tmpHH->iSubbandCanvas.SetHeight( tmpLH->iSubbandCanvas.Height() );
        tmpHH->iSubbandOrigin.iX = tmpHL->iSubbandOrigin.iX;
        tmpHH->iSubbandOrigin.iY = tmpLH->iSubbandOrigin.iY;
        start->AddChildL( tmpHH );
        CleanupStack::Pop();

        start = start->ChildAt( EBandLL );
        }

    // The lowest resolution must be 0, so reset
    // the resolution of NLL from 1 to 0.
    start->iResLevel = 0;

    CleanupStack::Pop();
    return root;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::BuildPacketsL
// Build the possible packets that may be in the subband
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSubband::BuildPacketsL( CJ2kComponentInfo& aComponent, HBufC8 *aPrecinctSiz, TUint16 aLayer )
    {
    CJ2kSubband *subband = 0;
    CJ2kPacket  *packet = 0;
    TUint16 packetIndex = 0;
    TUint16 xIndex = 0;
    TUint16 yIndex = 0;
    TInt    denom = 0;
    TInt    ppx = 0;
    TInt    ppy = 0;
    TInt    ppxL = 0;
    TInt    ppyL = 0;
    TInt32  trx0 = 0;
    TInt32  try0 = 0;
    TInt32  trx1 = 0;
    TInt32  try1 = 0;
    TInt32  n = 0;
    TInt32  m = 0;
    TInt32  nOrig = 0;
    TInt32  mOrig = 0;
    TInt32  xCoord = 0;
    TInt32  yCoord = 0;
    TInt32  numOfPackets = 0;
    TInt32  tmpX = 0;
    TInt32  tmpY = 0;
    TSize   cblkSize( 0, 0 );
    TSize   tmpSize( 0, 0 );

    const TSize &compSize = aComponent.CodeBlockSize();
    TInt  compSizeLogWidth = TJ2kUtils::Log2( compSize.iWidth );
    TInt  compSizeLogHeight = TJ2kUtils::Log2( compSize.iHeight );

    for ( TUint8 resLevel = 0; resLevel <= aComponent.Levels(); ++resLevel )
        {
        subband = SubbandAt( resLevel );
        numOfPackets = aComponent.NumOfPackets( resLevel );
        if ( numOfPackets )
            {
            do
                {
                for ( packetIndex = 0; packetIndex < numOfPackets; ++packetIndex )
                    {
                    CJ2kPacket *packet = new ( ELeave ) CJ2kPacket( aLayer );   //lint !e578  must be declared also earlier.
                    CleanupStack::PushL( packet );
                    
                    User::LeaveIfError( subband->iPacketList.Append( packet ) );
                    CleanupStack::Pop( 1 );

                    if ( subband->iType == EBandLL || subband->iType == EBandHL )
                        {
                        subband->iPacketList[packetIndex]->BuildInclusiveInfoL();
                        }
                    }
                subband = subband->NextSubbandRaster();
                } while ( subband );

            // Set to the default
            ppx = ppy = 15;

            if ( aPrecinctSiz )
                {
                ppxL = ( *aPrecinctSiz )[resLevel] & 0x0f;
                ppyL = ( ( *aPrecinctSiz )[resLevel] & 0xf0 ) >> 4;
                }
            else
                {
                ppxL = ppyL = 15;
                }

            if ( ppxL != 15 || ppyL != 15 )
                {
                ppx = ( resLevel ) ? ppxL - 1 : ppxL;
                ppy = ( resLevel ) ? ppyL - 1 : ppyL;
                cblkSize.iWidth = ( compSizeLogWidth < ppx ) ? compSize.iWidth : 1 << ppx;
                cblkSize.iHeight = ( compSizeLogHeight < ppy ) ? compSize.iHeight : 1 << ppy;
                }
            else
                {
                cblkSize = compSize;
                }

            denom = 1 << ( aComponent.Levels() - resLevel );

            trx0 = TJ2kUtils::CeilL( aComponent.ComponentCanvas().iTl.iX, denom );
            try0 = TJ2kUtils::CeilL( aComponent.ComponentCanvas().iTl.iY, denom );
            trx1 = TJ2kUtils::CeilL( aComponent.ComponentCanvas().iBr.iX, denom );
            try1 = TJ2kUtils::CeilL( aComponent.ComponentCanvas().iBr.iY, denom );

            nOrig = ( trx0 / (  1 << ppxL  ) ) * ( 1 << ppxL );
            mOrig = ( try0 / (  1 << ppyL  ) ) * ( 1 << ppyL );

            // First subband at this resolution level
            // at r = 0, it will be nLL
            // at r > 0, it will be ( n-r+1 )HL

            tmpSize = aComponent.PrecinctSizeAt( resLevel );
            tmpX = 1 << ppx;
            tmpY = 1 << ppy;
            subband = SubbandAt( resLevel ); 
            do
                {
                if ( subband->SubbandCanvasSize().iWidth > 0 &&
                    subband->SubbandCanvasSize().iHeight > 0 )
                    {
                    n = nOrig;
                    m = mOrig;

                    if ( subband->iType != EBandLL )
                        {
                        n /= 2;
                        m /= 2;
                        }

                    yCoord = m;
                    for ( yIndex = 0; yIndex < tmpSize.iHeight; ++yIndex )
                        {
                        xCoord = n;
                        for ( xIndex = 0; xIndex < tmpSize.iWidth; ++xIndex )
                            {
                            // Reuse the variables trx0, trx1, try0, try1
                            trx0 = Max( subband->SubbandCanvas().iTl.iX, xCoord );
                            try0 = Max( subband->SubbandCanvas().iTl.iY, yCoord );
                            trx1 = Min( xCoord + tmpX, subband->SubbandCanvas().iBr.iX ) - trx0;
                            try1 = Min( yCoord + tmpY, subband->SubbandCanvas().iBr.iY ) - try0;

                            if ( trx1 > 0 && try1 > 0 )
                                {
                                packetIndex = ( TUint16 )( yIndex * tmpSize.iWidth + xIndex );
                                packet = subband->iPacketList[packetIndex];
                                packet->SetPacketCanvas( trx0, try0, trx1, try1 );
                                packet->SetNumOfBlocksL( cblkSize );
                                packet->BuildCodeBlocksL( xCoord, yCoord, cblkSize );
                                }
                            xCoord += tmpX;
                            } // end of xIndex
                        yCoord += tmpY;
                        } // end of yIndex
                    } // precinct size is greater than 0
                subband = subband->NextSubbandRaster();
                } while ( subband );
            } // num of packets is greater than 0
        } // end of resolution level
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::LRCPProgressionL
// At each subband, parse the bitstream with LRCP progression order
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kSubband::LRCPProgressionL( CJ2kTileInfo& aTile, CJ2kComponentInfo& aComponent )
    {
    TUint8  incomplete = EFalse;
    TUint16 marker = 0;
    const TJ2kStreamReader& reader = aTile.StreamReader();
    while ( iLastPacket < iPacketList.Count() )
        {       
        if ( aTile.IsPOC() )
            {
            marker = PtrReadUtil::ReadBigEndianUint16( reader.iPtr );
            if ( marker == KSOT || marker == KEOC )
                {
                break;   //lint !e960    Break is OK.
                }
            }

        incomplete = iPacketList[iLastPacket]->ReadPacketHeaderL( aTile, aComponent, *this );
        if ( incomplete )
            {
            // Underflow
            break;   //lint !e960    Break is OK.
            }
        if ( iPacketList[iLastPacket]->IsBodyIncomplete() )
            {
            incomplete = iPacketList[iLastPacket]->ReadPacketBodyL( aTile, aComponent, *this );
            if ( incomplete )
                {
                // Underflow
                break;   //lint !e960    Break is OK.
                }
            }
        iPacketList[iLastPacket]->ResetInternalFlags();
        ++iLastPacket;
        }

    if ( !incomplete && iLastPacket == iPacketList.Count() )
        {
        // Reset it back to 0 for the next layer
        iLastPacket = 0;
        }

    return incomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::RPCLProgressionL
// At each subband, parse the bitstream with RPCL progression order
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kSubband::RPCLProgressionL( CJ2kTileInfo& aTile, CJ2kComponentInfo& aComponent )
    {
    TUint8  incomplete = EFalse;
    TUint16 numOfLayers = aTile.NumOfLayersPOC();
    TUint16 marker = 0;
    const TJ2kStreamReader& reader = aTile.StreamReader();
    while ( aTile.LastLayerProcessed() < numOfLayers )
        {
        if ( iPacketList.Count() )
            {
            if ( aTile.IsPOC() )
                {
                marker = PtrReadUtil::ReadBigEndianUint16( reader.iPtr );
                if ( marker == KSOT || marker == KEOC )
                    {
                    break;   //lint !e960    Break is OK.
                    }
                }

            incomplete = iPacketList[iLastPacket]->ReadPacketHeaderL( aTile, aComponent, *this );
            if ( incomplete )
                {
                // Underflow
                break;   //lint !e960    Break is OK.
                }
            if ( iPacketList[iLastPacket]->IsBodyIncomplete() )
                {
                incomplete = iPacketList[iLastPacket]->ReadPacketBodyL( aTile, aComponent, *this );
                if ( incomplete )
                    {
                    // Underflow
                    break;   //lint !e960    Break is OK.
                    }
                }
            iPacketList[iLastPacket]->ResetInternalFlags();
            }
        aTile.IncrementLastLayerProcessed();
        }

    if ( !incomplete && aTile.LastLayerProcessed() == numOfLayers )
        {
        aTile.ResetLastLayerProcessed();
        ++iLastPacket;
        if ( iLastPacket == iPacketList.Count() )
            {
            iLastPacket = 0;
            }
        }
    return incomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::CPRLProgressionL
// At each subband, parse the bitstream with CPRL progression order.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kSubband::CPRLProgressionL( CJ2kTileInfo& aTile, CJ2kComponentInfo& aComponent )
    {
    TUint8  incomplete = EFalse;
    TUint16 numOfLayers = aTile.NumOfLayersPOC();
    TUint16 marker = 0;
    const TJ2kStreamReader& reader = aTile.StreamReader();
    while ( aTile.LastLayerProcessed() < numOfLayers )
        {
        if ( iPacketList.Count() )
            {
            if ( aTile.IsPOC() )
                {
                marker = PtrReadUtil::ReadBigEndianUint16( reader.iPtr );
                if ( marker == KSOT || marker == KEOC )
                    {
                    break;   //lint !e960    Break is OK.
                    }
                }

            incomplete = iPacketList[iLastPacket]->ReadPacketHeaderL( aTile, aComponent, *this );
            if ( incomplete )
                {
                // Underflow
                break;   //lint !e960    Break is OK.
                }
            if ( iPacketList[iLastPacket]->IsBodyIncomplete() )
                {
                incomplete = iPacketList[iLastPacket]->ReadPacketBodyL( aTile, aComponent, *this );
                if ( incomplete )
                    {
                    // Underflow
                    break;   //lint !e960    Break is OK.
                    }
                }
            iPacketList[iLastPacket]->ResetInternalFlags();
            }
        aTile.IncrementLastLayerProcessed();
        }

    if ( !incomplete && aTile.LastLayerProcessed() == numOfLayers )
        {
        aTile.ResetLastLayerProcessed();
        ++iLastPacket;
        if ( iLastPacket == iPacketList.Count() )
            {
            iLastPacket = 0;
            }
        }
    return incomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::ChildAt
// Get the child subband at specific index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CJ2kSubband* CJ2kSubband::ChildAt( TInt /*aBand*/ )
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::NextSubbandRaster
// Get the sibling subband
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CJ2kSubband* CJ2kSubband::NextSubbandRaster()
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SubbandAt
// Get the subband at specific resolution level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CJ2kSubband* CJ2kSubband::SubbandAt( TUint8 /*aResLevel*/ )
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::AddChildL
// Add the child subband into the list of child subbands ( No-op )
// -----------------------------------------------------------------------------
//
void CJ2kSubband::AddChildL( CJ2kSubband* /*aChild*/ )
    {    }

// -----------------------------------------------------------------------------
// CJ2kSubband::SubbandTreeIndex
// Get the subband tree index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint16 CJ2kSubband::SubbandTreeIndex() const
    {
    TUint16 bandIndex = 0;
    if ( iResLevel )
        {
        bandIndex = ( TUint16 )( ( iResLevel - 1 ) * 3 + iType );
        }

    return bandIndex;
    }

// -----------------------------------------------------------------------------
// CJ2kSubband::CJ2kSubband
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJ2kSubband::CJ2kSubband() : 
    iPacketList( 1 )
    {
    }

// -----------------------------------------------------------------------------
// CJ2kSubbandLL::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJ2kSubbandLL* CJ2kSubbandLL::NewLC()
    {
    CJ2kSubbandLL* self = new ( ELeave ) CJ2kSubbandLL();

    CleanupStack::PushL( self );
    self->iType = EBandLL;

    return self;
    }
    
// Destructor
CJ2kSubbandLL::~CJ2kSubbandLL()
    {
    iChildList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CJ2kSubbandLL::ChildAt
// Get the child subband at specific index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CJ2kSubband* CJ2kSubbandLL::ChildAt( TInt aBand )
    {
    if ( iChildList.Count() > 0 )
        {
        return iChildList[aBand];
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// CJ2kSubbandLL::SubbandAt
// Get the subband at specific resolution level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CJ2kSubband* CJ2kSubbandLL::SubbandAt( TUint8 aResLevel )
    {
    if ( iResLevel > aResLevel )
        {
        return ChildAt( EBandLL )->SubbandAt( aResLevel );
        }
    if ( iResLevel == 0 && aResLevel == 0 )
        {
        return this;
        }
    else
        {
        return Parent()->ChildAt( EBandHL );
        }
    }

// -----------------------------------------------------------------------------
// CJ2kSubbandLL::AddChildL
// Add the child subband into the list of child subbands
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kSubbandLL::AddChildL( CJ2kSubband* aChild )
    {
    aChild->SetParent( this );
    User::LeaveIfError( iChildList.Append( aChild ) );
    }

// -----------------------------------------------------------------------------
// CJ2kSubbandNLL::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJ2kSubbandNLL* CJ2kSubbandNLL::NewLC()
    {
    CJ2kSubbandNLL* self = new ( ELeave ) CJ2kSubbandNLL();
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CJ2kSubbandNLL::~CJ2kSubbandNLL()
    { 
    }

// -----------------------------------------------------------------------------
// CJ2kSubbandNLL::NextSubbandRaster
// Get the sibling subband
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CJ2kSubband* CJ2kSubbandNLL::NextSubbandRaster()
    {
    CJ2kSubband *nextSubband = 0;
    switch ( iType )
        {
        case EBandHL:
            {
            nextSubband = Parent()->ChildAt( EBandLH );
            break;
            }
        case EBandLH:
            {
            nextSubband = Parent()->ChildAt( EBandHH );
            break;
            }
        default:
            {
            break;
            }
        }

    return nextSubband;
    }
