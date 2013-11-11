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
* Description:  JP2KPacket class used to collect packet related
*                information such as tag tree and list of codeblocks.
*
*/


// INCLUDE FILES
#include <icl/imagecodec.h>
#include "JP2KImageUtils.h"
#include "JP2KStreamReader.h"
#include "JP2KTileInfo.h"
#include "JP2KImageInfo.h"
#include "JP2KCodeBlock.h"
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

// -----------------------------------------------------------------------------
// CJ2kPacket::CJ2kPacket
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJ2kPacket::CJ2kPacket( TUint16 aLayer ) : 
    iTagTreeLevel(1),
    iLayer(aLayer), 
    iCodeBlockList(1)
    {
    }

// Destructor
CJ2kPacket::~CJ2kPacket()
    {
    delete iIncluded;
    iIncluded = 0;

    delete iTagTreeInfo;
    iTagTreeInfo = 0;

    delete iIncTagTreeValue;
    iIncTagTreeValue = 0;

    delete iIncTagTreeState;
    iIncTagTreeState = 0;

    delete iMsbTagTreeValue;
    iMsbTagTreeValue = 0;

    delete iMsbTagTreeState;
    iMsbTagTreeState = 0;

    iCodeBlockList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetPacketCanvas
// Set the canvas of the packet
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::SetPacketCanvas( TInt32 aX, TInt32 aY, TInt32 aWidth, TInt32 aHeight )
    {
    iPacketCanvas.iTl = TPoint( aX, aY );
    iPacketCanvas.SetWidth( aWidth );
    iPacketCanvas.SetHeight( aHeight );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetNumOfBlocksL
// Set the number of blocks
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::SetNumOfBlocksL( TSize& aCodeBlock )
    {
    TInt32 x0 = TJ2kUtils::FloorL( iPacketCanvas.iTl.iX, aCodeBlock.iWidth ) * aCodeBlock.iWidth;
    TInt32 y0 = TJ2kUtils::FloorL( iPacketCanvas.iTl.iY, aCodeBlock.iHeight ) * aCodeBlock.iHeight;
    TInt32 x1 = TJ2kUtils::CeilL( iPacketCanvas.iTl.iX + iPacketCanvas.Width(), aCodeBlock.iWidth ) * aCodeBlock.iWidth;
    TInt32 y1 = TJ2kUtils::CeilL( iPacketCanvas.iTl.iY + iPacketCanvas.Height(), aCodeBlock.iHeight ) * aCodeBlock.iHeight;
    iCodeBlockSize.iWidth  = ( x1 - x0 ) / aCodeBlock.iWidth;
    iCodeBlockSize.iHeight = ( y1 - y0 ) / aCodeBlock.iHeight;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::BuildInclusiveInfoL
// Build the inclusive information of the packet
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::BuildInclusiveInfoL()
    {
    TInt entries = ( iLayer % 8 ) ? ( ( iLayer / 8 ) + 1 ) : ( iLayer / 8 );
    iIncluded = HBufC8::NewMaxL( entries );
    iIncluded->Des().FillZ();
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::BuildCodeBlocksL
// Build the codeblocks in the packet
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::BuildCodeBlocksL( TInt32 aX, TInt32 aY, TSize& aCodeBlock )
    {
    TUint16 numOfBlocks = NumOfBlocks();

    if ( numOfBlocks )
        {
        BuildTagtreeL();

        TInt32 x0 = 0;
        TInt32 y0 = 0;
        TInt32 x1 = 0;
        TInt32 y1 = 0;
        TInt32 origX = aX;
        TUint16 index = 0;

        while ( index < numOfBlocks )
            {
            x0 = Max( aX, iPacketCanvas.iTl.iX );
            y0 = Max( aY, iPacketCanvas.iTl.iY );
            x1 = Min( aX + aCodeBlock.iWidth, iPacketCanvas.iTl.iX + iPacketCanvas.Width() ) - x0;
            y1 = Min( aY + aCodeBlock.iHeight, iPacketCanvas.iTl.iY + iPacketCanvas.Height() ) - y0;
            if ( x1 > 0 && y1 > 0 )
                {
                CJ2kCodeBlock *block = new ( ELeave ) CJ2kCodeBlock;
                CleanupStack::PushL( block );
                User::LeaveIfError( iCodeBlockList.Append( block ) );
                CleanupStack::Pop( 1 );
                iCodeBlockList[index]->SetCodeBlockCanvas( x0, y0, x1, y1 );

                // Initialize the codeblock
                iCodeBlockList[index]->InitializeL( iLayer );
                ++index;
                }

            aX += aCodeBlock.iWidth;
            if ( aX >= iPacketCanvas.iTl.iX + iPacketCanvas.Width() )
                {
                aX = origX;
                aY += aCodeBlock.iHeight;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ReadPacketHeaderL
// Read the packet header
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::ReadPacketHeaderL( CJ2kTileInfo& aTile, CJ2kComponentInfo& aComponent, CJ2kSubband& aSubband )
    {
    if ( IsIncludedL( aTile.LastLayerProcessed() ) )
        {
        return EFalse;
        }

    TUint8 incomplete = ETrue;
    if ( LastCodeBlockProcessed() == 0 )
        {
        if ( ReadSOP_EPHMarker( aComponent, aTile ) )
            {
            return incomplete;
            }
        }

    TJ2kStreamReader& reader = CONST_CAST( TJ2kStreamReader&, aTile.StreamReader() );
    TUint8 *rollbackPtr = CONST_CAST( TUint8*, reader.iPtr );

    if ( !StartReadBit( aTile ) )
        {
        TUint8 bit = 0;

        if ( LastCodeBlockProcessed() == 0 )
            {
            if ( !IsRecoverFromIncomplete() )
                {
                ReadBit( bit, aTile );
                if ( bit == 0 )
                    {
                    // Empty packet
                    incomplete = EFalse;
                    }
                }
            }

        if ( incomplete )
            {
            // Packet header has not been processed yet
            if ( !IsHeader() )
                {
                TUint8 isBackup = IsBackupNeeded( aTile );

                CJ2kSubband *subband = &aSubband;
                if ( subband->SubbandResLevel() > 0 )
                    {
                    if ( subband->LastSubbandProcessed() == CJ2kSubband::EBandLL )
                        {
                        subband->SetLastSubbandProcessed( CJ2kSubband::EBandHL );
                        }
                    else
                        {
                        // Move to last subband processed
                        while ( subband->SubbandType() != aSubband.LastSubbandProcessed() )
                            {
                            subband = subband->NextSubbandRaster();
                            User::LeaveIfNull(subband);
                            }
                        }
                    }

                TUint16 bandIndex = 0;
                TUint16 cbIndex = 0;
                TUint16 cbYIndex = 0;
                TUint16 xIndex = 0;
                TUint16 yIndex = 0;
                TUint16 incValue = 0;
                TUint16 msbValue = 0;
                TUint16 index = 0;
                TUint16 layer = aTile.LastLayerProcessed();
                TUint16 numPass = 0;
                TUint16 codePass = 0;
                TUint16 numSegment = 0;
                TUint8  recoverIncTag = 0;
                TUint8  recoverMsbTag = 0;
                TUint8  firstRecovery = ETrue;
                TUint8  recoverData = 0;
                TUint8  recoverPos = 0;
                TUint8  recoverNextPos = 0;
                TUint8  origPassIndex = 0;
                TUint8  origLengthBits = 0;
                TInt8   origLastPass = 0;
                TUint32 origDataLength = 0;

                HBufC16 *recoverIncTagTreeValue = 0;
                HBufC16 *recoverIncTagTreeState = 0;
                HBufC16 *recoverMsbTagTreeValue = 0;
                HBufC16 *recoverMsbTagTreeState = 0;

                if ( IsRecoverFromIncomplete() )
                    {
                    recoverData = reader.iData;
                    recoverPos = reader.iPos;
                    recoverNextPos = reader.iNextPos;
                    }

                CJ2kPacket *packet = CONST_CAST( CJ2kPacket*, &subband->PacketAt( aSubband.LastPacketProcessed() ) );
                CJ2kCodeBlock *codeBlock;
                incomplete = EFalse;
                do
                    {
                    if ( aComponent.QuantizationStyle() != 1 )
                        {
                        bandIndex = subband->SubbandTreeIndex();
                        }
                    
                    for ( yIndex = 0; yIndex < packet->NumOfBlocksY(); ++yIndex )
                        {
                        cbYIndex = (TUint16)( yIndex * packet->NumOfBlocksX() );
                        for ( xIndex = 0; xIndex < packet->NumOfBlocksX(); ++xIndex )
                            {
                            cbIndex = (TUint16)( cbYIndex + xIndex );
                            if ( cbIndex < packet->LastCodeBlockProcessed() ||
                                ( packet->IsMatch() && cbIndex == packet->LastCodeBlockProcessed() ) )
                                {
                                // Has been processed, skip to the next codeblock
                                continue;   //lint !e960    Using continue saves a lot of code here.
                                }
                            else
                                {
                                packet->ResetMatch();
                                recoverIncTag = recoverMsbTag = EFalse;
                                codeBlock = CONST_CAST( CJ2kCodeBlock*, &packet->CodeBlockAt( cbIndex ) );

                                TPtr16 numSegPtr = codeBlock->iNumSegment->Des();

                                origPassIndex = codeBlock->iPassIndex;
                                origLastPass  = codeBlock->iLastPass;
                                if ( codeBlock->iLastPass < 0 && codeBlock->iPassIndex == 0 )
                                    {
                                    if ( isBackup )
                                        {
                                        packet->BackupIncTagtreeL( recoverIncTagTreeValue, recoverIncTagTreeState );
                                        recoverIncTag = ETrue;
                                        }
                                    msbValue = (TUint16)0;
                                    incomplete = packet->DecodeIncTagtree( aTile, xIndex, yIndex, incValue );
                                    if ( !incomplete )
                                        {
                                        if ( incValue <= layer )
                                            {
                                            if ( isBackup )
                                                {
                                                packet->BackupMsbTagtreeL( recoverMsbTagTreeValue, recoverMsbTagTreeState );
                                                recoverMsbTag = ETrue;
                                                }
                                            incomplete = packet->DecodeMsbTagtree( aTile, xIndex, yIndex, msbValue );
                                            }
                                        else
                                            {
                                            packet->SetLastCodeBlockProcessed( cbIndex );
                                            firstRecovery  = EFalse;
                                            recoverData    = reader.iData;
                                            recoverPos     = reader.iPos;
                                            recoverNextPos = reader.iNextPos;
                                            recoverIncTag = recoverMsbTag = EFalse;
                                            rollbackPtr   =  CONST_CAST( TUint8*, reader.iPtr );

                                            // Move on to the next codeblock
                                            continue;   //lint !e960    Using continue saves a lot of code here.
                                            }
                                        }

                                    if ( incomplete )
                                        {
                                        // Underflow, should backup the iterator position
                                        if ( cbIndex > packet->LastCodeBlockProcessed() )
                                            {
                                            // Next read should match the codeblock index
                                            packet->SetMatch();
                                            }
                                        
                                        if ( !firstRecovery )
                                            {
                                            SetRecoverFromIncomplete();
                                            reader.iData    = recoverData;
                                            reader.iPos     = recoverPos;
                                            reader.iNextPos = recoverNextPos;
                                            }
                                        else
                                            {
                                            if ( IsRecoverFromIncomplete() )
                                                {
                                                reader.iData    = recoverData;
                                                reader.iPos     = recoverPos;
                                                reader.iNextPos = recoverNextPos;
                                                }
                                            }
                                        
                                        if ( isBackup )
                                            {
                                            if ( recoverIncTag )
                                                {
                                                packet->RestoreIncTagtree( recoverIncTagTreeValue, recoverIncTagTreeState );
                                                }
                                            if ( recoverMsbTag )
                                                {
                                                packet->RestoreMsbTagtree( recoverMsbTagTreeValue, recoverMsbTagTreeState );
                                                }
                                            // Release memory
                                            packet->FreeBackupTagtree( recoverIncTagTreeValue, recoverIncTagTreeState, 
                                                                       recoverMsbTagTreeValue, recoverMsbTagTreeState );
                                            }
                                        reader.iPtr = rollbackPtr;

                                        // Return from the packet header decoding
                                        return incomplete;
                                        }

                                    codeBlock->iLastPass = 0;
                                    numSegPtr[layer] = 1;

                                    codeBlock->iEmptyBitplane = (TUint8)msbValue;

                                    if ( aComponent.RoiShift() > 0 )
                                        {
                                        numPass = (TUint16)( ( aComponent.MagnitudeBits( bandIndex ) + aComponent.RoiShift() ) * 3 + 1 );
                                        }
                                    else
                                        {
                                        numPass = (TUint16)( ( aComponent.MagnitudeBits( bandIndex ) - msbValue  ) * 3 + 1 );
                                        }

                                    if ( codeBlock->iDataSize == 0 || codeBlock->iDataSize->Length() < numPass )
                                        {
                                        if ( codeBlock->iDataSize == 0 )
                                            {
                                            codeBlock->iDataSize = HBufC16::NewMaxL( numPass );
                                            codeBlock->iDataSize->Des().FillZ();
                                            codeBlock->iPassesPerSegment = HBufC16::NewMaxL( numPass );
                                            codeBlock->iPassesPerSegment->Des().FillZ();
                                            }
                                        else
                                            {
                                            incValue = ( TUint16 )( numPass - codeBlock->iDataSize->Length() );
                                            codeBlock->iDataSize = codeBlock->iDataSize->ReAllocL( numPass );
                                            codeBlock->iPassesPerSegment = codeBlock->iPassesPerSegment->ReAllocL( numPass );
                                            while ( incValue-- )
                                                {
                                                codeBlock->iDataSize->Des().Append( 0 );
                                                codeBlock->iPassesPerSegment->Des().Append( 0 );
                                                }
                                            }
                                        }
                                    }
                                else
                                    {
                                    incomplete = ReadBit( bit, aTile );
                                    if ( incomplete )
                                        {
                                        // Underflow, should backup the iterator position
                                        if ( cbIndex > packet->LastCodeBlockProcessed() )
                                            {
                                            // Next read should match the codeblock index
                                            packet->SetMatch();
                                            }
                                        
                                        if ( !firstRecovery )
                                            {
                                            SetRecoverFromIncomplete();
                                            reader.iData = recoverData;
                                            reader.iPos = recoverPos;
                                            reader.iNextPos = recoverNextPos;
                                            }
                                        else
                                            {
                                            if ( IsRecoverFromIncomplete() )
                                                {
                                                reader.iData    = recoverData;
                                                reader.iPos     = recoverPos;
                                                reader.iNextPos = recoverNextPos;
                                                }
                                            }
                                        
                                        if ( isBackup )
                                            {
                                            // Release memory
                                            packet->FreeBackupTagtree( recoverIncTagTreeValue, recoverIncTagTreeState, 
                                                                       recoverMsbTagTreeValue, recoverMsbTagTreeState );
                                            }
                                        reader.iPtr = rollbackPtr;

                                        // Return from the packet header decoding
                                        return incomplete;
                                        }

                                    numSegPtr[layer] = bit;

                                    if ( bit == 0 )
                                        {
                                        packet->SetLastCodeBlockProcessed( cbIndex );
                                        firstRecovery  = EFalse;
                                        recoverData = reader.iData;
                                        recoverPos  = reader.iPos;
                                        recoverNextPos = reader.iNextPos;
                                        recoverIncTag = recoverMsbTag = EFalse;
                                        rollbackPtr   =  CONST_CAST( TUint8*, reader.iPtr );

                                        // Move on to the next codeblock
                                        continue;   //lint !e960    Using continue saves a lot of code here.
                                        }
                                    }

                                incomplete = ReadBit( bit, aTile );
                                codePass = (TUint16)0;
                                if ( !incomplete )
                                    {
                                    if ( bit ) // 1
                                        {
                                        incomplete = ReadBit( bit, aTile );
                                        if ( !incomplete )
                                            {
                                            if ( bit ) // 11
                                                {
                                                incomplete = ReadBits( bit, 2, aTile );
                                                if ( !incomplete )
                                                    {
                                                    if ( bit == 0x03 ) // 1111
                                                        {
                                                        incomplete = ReadBits( bit, 5, aTile );
                                                        if ( !incomplete )
                                                            {
                                                            if ( bit == 0x1f ) // 1111 11111
                                                                {
                                                                incomplete = ReadBits( bit, 7, aTile );
                                                                if ( !incomplete )
                                                                    {
                                                                    codePass = (TUint16)( 0x25 + bit );
                                                                    }
                                                                }
                                                            else
                                                                {
                                                                // 1111 XXXXX, at least one of them is not 1
                                                                codePass = (TUint16)( 6 + bit );
                                                                }
                                                            }
                                                        }
                                                    else
                                                        {
                                                        // 11XX
                                                        codePass = (TUint16)( 3 + bit );
                                                        }
                                                    }
                                                }
                                            else
                                                {
                                                // 10
                                                codePass = 2;
                                                }
                                            }
                                        }
                                    else
                                        {
                                        // 0
                                        codePass = 1;
                                        }
                                    }

                                if ( incomplete )
                                    {
                                    // Underflow, restore the original codeblock information
                                    codeBlock->iPassIndex = origPassIndex;
                                    codeBlock->iLastPass = origLastPass;

                                    // Underflow, should backup the iterator position
                                    if ( cbIndex > packet->LastCodeBlockProcessed() )
                                        {
                                        // Next read should match the codeblock index
                                        packet->SetMatch();
                                        }
                                    
                                    if ( !firstRecovery )
                                        {
                                        SetRecoverFromIncomplete();
                                        reader.iData = recoverData;
                                        reader.iPos = recoverPos;
                                        reader.iNextPos = recoverNextPos;
                                        }                                    
                                    else
                                        {
                                        if ( IsRecoverFromIncomplete() )
                                            {
                                            reader.iData = recoverData;
                                            reader.iPos  = recoverPos;
                                            reader.iNextPos = recoverNextPos;
                                            }
                                        }

                                    if ( isBackup )
                                        {
                                        if ( recoverIncTag )
                                            {
                                            packet->RestoreIncTagtree( recoverIncTagTreeValue, 
                                                                       recoverIncTagTreeState );
                                            }
                                        if ( recoverMsbTag )
                                            {
                                            packet->RestoreMsbTagtree( recoverMsbTagTreeValue, 
                                                                       recoverMsbTagTreeState );
                                            }
                                        // Release memory
                                        packet->FreeBackupTagtree( recoverIncTagTreeValue, recoverIncTagTreeState, 
                                                                   recoverMsbTagTreeValue, recoverMsbTagTreeState );
                                        }
                                    reader.iPtr = rollbackPtr;
                                    
                                    // Return from the packet header decoding
                                    return incomplete;
                                    }

                                if ( codeBlock->iPassIndex == 0 )
                                    {
                                    codeBlock->iLastPass = -1;
                                    }

                                numSegment = 1;

                                if ( aComponent.IsTermination() )
                                    {
                                    numSegment = codePass;
                                    }
                                else if ( aComponent.IsACByPass() )
                                    {
                                    if ( ( codeBlock->iLastPass + codePass ) >= KFirstLazyPassIndex )
                                        {
                                        for ( index = 1; index < codePass; ++index )
                                            {
                                            if ( ( index + codeBlock->iLastPass ) >= KFirstLazyPassIndex )
                                                {
                                                if ( ( ( index + codeBlock->iLastPass ) % 3 ) != 1 )
                                                    {
                                                    ++numSegment;
                                                    }
                                                }
                                            }
                                        }
                                    }

                                numSegPtr[layer] = numSegment;   //lint !e961    no else is needed here at the end of if...else if

                                origLengthBits = codeBlock->iLengthBits;
                                origDataLength = codeBlock->iDataLength;
                                incomplete = ReadBit( bit, aTile );
                                if ( !incomplete )
                                    {
                                    // Skip all 1 bits
                                    while ( bit )
                                        {
                                        ++codeBlock->iLengthBits;
                                        incomplete = ReadBit( bit, aTile );
                                        if ( incomplete )
                                            {
                                            // Underflow
                                            // Set bit to zero to break from the while-loop
                                            bit = 0;
                                            }
                                        }
                                    }

                                if ( !incomplete )
                                    {
                                    TUint8 bits = 0;
                                    TUint32 tmpBits = 0;
                                    TPtr16 numPassesPerSegPtr = codeBlock->iPassesPerSegment->Des();
                                    TPtr16 numDataSizePtr = codeBlock->iDataSize->Des();

                                    if ( numSegment == 1 )
                                        {
                                        bits = (TUint8)( codeBlock->iLengthBits + TJ2kUtils::Log2( codePass ) );

                                        incomplete = ReadBits( tmpBits, bits, aTile );
                                        if ( !incomplete )
                                            {
                                            numPassesPerSegPtr[codeBlock->iPassIndex] = codePass;
                                            numDataSizePtr[codeBlock->iPassIndex++] = (TUint16)tmpBits;

                                            codeBlock->iDataLength += tmpBits;
                                            codeBlock->iLastPass = (TUint8)( codeBlock->iLastPass + codePass );
                                            if ( tmpBits > 0 )
                                                {
                                                SetBodyIncomplete();
                                                }
                                            }
                                        }
                                    else
                                        {
                                        TUint16 totalCodePass = codePass;
                                        TUint16 tmpCodePass;
                                        for ( index = 0; index < numSegment; ++index )
                                            {
                                            if ( aComponent.IsTermination() )
                                                {
                                                tmpCodePass = 1;
                                                }
                                            else
                                                {
                                                if ( codeBlock->iLastPass < KFirstLazyPassIndex )
                                                    {
                                                    tmpCodePass = (TUint16)( KFirstLazyPassIndex - ( TInt )codeBlock->iLastPass );
                                                    }
                                                else
                                                    {
                                                    tmpCodePass = (TUint16)( ( codeBlock->iLastPass % 3 ) ? 1 : ( ( totalCodePass > 1 ) ? 2 : 1 ) );
                                                    }
                                                }
                                            bits = (TUint8)( codeBlock->iLengthBits + TJ2kUtils::Log2( tmpCodePass ) );
                                            incomplete = ReadBits( tmpBits, bits, aTile );
                                            if ( !incomplete )
                                                {
                                                numPassesPerSegPtr[codeBlock->iPassIndex] = tmpCodePass;
                                                numDataSizePtr[codeBlock->iPassIndex++] = (TUint16)tmpBits;

                                                codeBlock->iDataLength += tmpBits;
                                                codeBlock->iLastPass = (TUint8)( codeBlock->iLastPass + tmpCodePass );
                                                totalCodePass = (TUint16)( totalCodePass - tmpCodePass );
                                                if ( tmpBits > 0 )
                                                    {
                                                    SetBodyIncomplete();
                                                    }
                                                }
                                            else
                                                {
                                                index = numSegment;     // break from the for-loop
                                                }
                                            }
                                        }
                                    }

                                if ( incomplete )
                                    {
                                    // Underflow, restore the original codeblock information
                                    codeBlock->iPassIndex = origPassIndex;
                                    codeBlock->iLastPass = origLastPass;
                                    codeBlock->iLengthBits = origLengthBits;
                                    codeBlock->iDataLength = origDataLength;

                                    // Underflow, should backup the iterator position
                                    if ( cbIndex > packet->LastCodeBlockProcessed() )
                                        {
                                        // Next read should match the codeblock index
                                        packet->SetMatch();
                                        }
                                    
                                    if ( !firstRecovery )
                                        {
                                        SetRecoverFromIncomplete();
                                        reader.iData = recoverData;
                                        reader.iPos = recoverPos;
                                        reader.iNextPos = recoverNextPos;
                                        }
                                    else
                                        {
                                        if ( IsRecoverFromIncomplete() )
                                            {
                                            reader.iData = recoverData;
                                            reader.iPos = recoverPos;
                                            reader.iNextPos = recoverNextPos;
                                            }
                                        }
                                    
                                    if ( isBackup )
                                        {
                                        if ( recoverIncTag )
                                            {
                                            packet->RestoreIncTagtree( recoverIncTagTreeValue, 
                                                                       recoverIncTagTreeState );
                                            }
                                        if ( recoverMsbTag )
                                            {
                                            packet->RestoreMsbTagtree( recoverMsbTagTreeValue, 
                                                                       recoverMsbTagTreeState );
                                            }

                                        // Release memory
                                        packet->FreeBackupTagtree( recoverIncTagTreeValue, 
                                                                   recoverIncTagTreeState, 
                                                                   recoverMsbTagTreeValue, 
                                                                   recoverMsbTagTreeState );
                                        }
                                    reader.iPtr = rollbackPtr;
                                    
                                    // Return from the packet header decoding
                                    return incomplete;
                                    }

                                }

                            packet->SetLastCodeBlockProcessed( cbIndex );
                            firstRecovery  = EFalse;
                            recoverData = reader.iData;
                            recoverPos = reader.iPos;
                            recoverNextPos = reader.iNextPos;
                            recoverIncTag = recoverMsbTag = EFalse;
                            rollbackPtr = CONST_CAST( TUint8*, reader.iPtr );
                            } // end of xIndex
                        } // end of yIndex

                    packet->SetLastCodeBlockProcessed( 0 );
                    packet->ResetMatch();
                    ResetRecoverFromIncomplete();
                    subband = subband->NextSubbandRaster();
                    if ( subband )
                        {
                        if ( isBackup )
                            {
                            // Sibling may have different tag tree level, so release memory here
                            packet->FreeBackupTagtree( recoverIncTagTreeValue, recoverIncTagTreeState, 
                                                       recoverMsbTagTreeValue, recoverMsbTagTreeState );
                            }
                        packet = CONST_CAST( CJ2kPacket*, &subband->PacketAt( aSubband.LastPacketProcessed() ) );
                        aSubband.SetLastSubbandProcessed( (TUint8)subband->SubbandType() );
                        }

                    } while ( subband );

                if ( !incomplete )
                    {
                    // Packet header has been completely processed
                    SetHeader();
                    SetIncluded( aTile.LastLayerProcessed() );
                    AlignReader( aTile );
                    aSubband.SetLastSubbandProcessed( CJ2kSubband::EBandLL );
                    }
                if ( isBackup )
                    {
                    // Release memory
                    packet->FreeBackupTagtree( recoverIncTagTreeValue, recoverIncTagTreeState, 
                                               recoverMsbTagTreeValue, recoverMsbTagTreeState );
                    }
                }
            else
                {
                // Packet has been processed
                reader.iPtr = rollbackPtr;
                }
            }

        if ( !incomplete )
            {
            if ( aComponent.IsEPHMarkerUsed() )
                {
                ReadEPHMarker( aTile );
                }
            }
        }
    return incomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ReadPacketBodyL
// Read the packet body
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::ReadPacketBodyL( CJ2kTileInfo& aTile, CJ2kComponentInfo& aComponent, CJ2kSubband& aSubband )
    {
    TUint8 incomplete = EFalse;
    if ( LastCodeBlockProcessed() == 0 )
        {
        if ( ReadSOP_EPHMarker( aComponent, aTile, EFalse ) )
            {
            return ETrue;
            }
        }

    TJ2kStreamReader& reader = CONST_CAST( TJ2kStreamReader&, aTile.StreamReader() );

    // Packet body has not been processed yet
    if ( !IsBody() )
        {
        SetBodyIncomplete();

        CJ2kSubband *subband = &aSubband;
        if ( subband->SubbandResLevel() > 0 )
            {
            if ( subband->LastSubbandProcessed() == CJ2kSubband::EBandLL )
                {
                subband->SetLastSubbandProcessed( CJ2kSubband::EBandHL );
                }
            else
                {
                // Move to last subband processed
                while ( subband->SubbandType() != aSubband.LastSubbandProcessed() )
                    {
                    subband = subband->NextSubbandRaster();
                    User::LeaveIfNull(subband); 
                    }
                }
            }
        TUint16 xIndex = 0;
        TUint16 yIndex = 0;
        TUint16 cbIndex = 0;
        TUint16 cbYIndex = 0;
        TUint16 index = 0;
        TUint16 cblkLength = 0;
        TUint16 layer = aTile.LastLayerProcessed();
        TInt32  totalLength = 0;
        TUint8 *rollbackPtr = CONST_CAST( TUint8*, reader.iPtr );
        CJ2kCodeBlock *codeBlock = 0;
        
        CJ2kPacket * packet = CONST_CAST( CJ2kPacket*, &subband->PacketAt( aSubband.LastPacketProcessed() ) );
        
        do
            {
        
            for ( yIndex = 0; yIndex < packet->NumOfBlocksY(); ++yIndex )
                {
                cbYIndex = (TUint16)( yIndex * packet->NumOfBlocksX() );
                for ( xIndex = 0; xIndex < packet->NumOfBlocksX(); ++xIndex )
                    {
                    cbIndex = (TUint16)( cbYIndex + xIndex );
                    if ( cbIndex < packet->LastCodeBlockProcessed() ||
                        ( packet->IsMatch() && cbIndex == packet->LastCodeBlockProcessed() ) )
                        {
                        // Has been processed, skip to the next codeblock
                        continue;   //lint !e960    Using continue saves a lot of code here.
                        }
                    else
                        {
                        packet->ResetMatch();
                        codeBlock = CONST_CAST( CJ2kCodeBlock*, &packet->CodeBlockAt( cbIndex ) );
                        
                        if ( ( *codeBlock->iNumSegment )[layer] > 0 )
                            {
                            TUint tempLength = 0;
                            TUint8 skip = 0;
                            
                            if ( !aTile.IsSpeedup() )
                            {
                                // If this resolution level is not required in decoding, skip storing the data to save memory
                                if ( aTile.LastLevelProcessed() > (TUint8)( aTile.NumOfLevelsPOC() - aTile.ImageInfo().LevelDrop() ) )
                                    skip = 1;
                            }
                            
                            tempLength = codeBlock->iDataLength;
                            
                            if ( tempLength )
                                {
                                // Find out which segment has been processed
                                totalLength = codeBlock->iDataLength - tempLength;
                                    {
                                    for ( index = 1; index <= ( *codeBlock->iNumSegment )[layer]; ++index )
                                        {
                                        totalLength -= ( *codeBlock->iDataSize )[codeBlock->iPassIndex - index];
                                        if ( totalLength <= 0 )
                                            {
                                            break;   //lint !e960    break usage saves code here.
                                            }
                                        }
                                    }
                                }
                            else
                                {
                                index = ( *codeBlock->iNumSegment )[layer];
                                }

                            for ( ; index > 0; --index )
                                {
                                cblkLength = ( *codeBlock->iDataSize )[codeBlock->iPassIndex - index];
                                if ( ( reader.iPtrEnd - reader.iPtr ) < cblkLength )
                                    {
                                    // Underflow, rollback the iterator position
                                    reader.iPtr = rollbackPtr;
                                    incomplete = ETrue;
                                    if ( cbIndex > packet->LastCodeBlockProcessed() )
                                        {
                                        // Next read should match the codeblock index
                                        packet->SetMatch();
                                        }

                                    return incomplete;
                                    }
                                else
                                    {
                                    
                                        // Store data only if the resolution level is not skipped
                                        if( !skip )
                                        {
                                    	    TPtr8 data = codeBlock->DataL()->Des();
	                                        data.Append( reader.iPtr, cblkLength );
                                        }
                                    
                                    reader.iPtr += cblkLength;
                                    }
                                // Save the iterator position
                                rollbackPtr = CONST_CAST( TUint8*, reader.iPtr );
                                }
                            }
                        }
                    packet->SetLastCodeBlockProcessed( cbIndex );

                    // Save the iterator position
                    rollbackPtr = CONST_CAST( TUint8*, reader.iPtr );
                    } // end of xIndex
                } // end of yIndex

            packet->SetLastCodeBlockProcessed( 0 );
            packet->ResetMatch();
            subband = subband->NextSubbandRaster();
            if ( subband )
                {
                packet = CONST_CAST( CJ2kPacket*, &subband->PacketAt( aSubband.LastPacketProcessed() ) );
                aSubband.SetLastSubbandProcessed( (TUint8)subband->SubbandType() );
                }

            } while ( subband );

        SetBody();
        ResetBodyIncomplete();
        aSubband.SetLastSubbandProcessed( CJ2kSubband::EBandLL );
        }

    return incomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ResetInternalFlags
// Reset the internal flags
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::ResetInternalFlags()
    {
    ResetHeader();
    ResetBody();
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::BuildTagtreeL
// Build the tag tree
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::BuildTagtreeL()
    {
    TUint32 width  = iCodeBlockSize.iWidth;
    TUint32 height = iCodeBlockSize.iHeight;

    while ( width != 1 || height != 1 )
        {
        width  = ( width + 1 ) >> 1;
        height = ( height + 1 ) >> 1;
        ++iTagTreeLevel;
        }

    TInt size = 0;
    TInt sizeIndex = 0;
    width = iCodeBlockSize.iWidth;
    height = iCodeBlockSize.iHeight;

    iTagTreeInfo = HBufC16::NewMaxL( iTagTreeLevel + 1 );
    TPtr16 tmpPtr = ( iTagTreeInfo->Des() );
    tmpPtr[0] = (TUint16)sizeIndex;

    for ( TUint8 index = 0; index < iTagTreeLevel; ++index )
        {
        size = width * height;
        sizeIndex += size;
        tmpPtr[index + 1] = (TUint16)sizeIndex;
        }

    TChar fillChar( 0xffff );

    iIncTagTreeValue = HBufC16::NewMaxL( sizeIndex );
    iIncTagTreeValue->Des().Fill( fillChar, iIncTagTreeValue->Length() );
    iIncTagTreeState = HBufC16::NewMaxL( sizeIndex );
    iIncTagTreeState->Des().FillZ();

    iMsbTagTreeValue = HBufC16::NewMaxL( sizeIndex );
    iMsbTagTreeValue->Des().Fill( fillChar, iIncTagTreeValue->Length() );
    iMsbTagTreeState = HBufC16::NewMaxL( sizeIndex );
    iMsbTagTreeState->Des().FillZ();
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::DecodeIncTagtree
// Decode the included tag tree
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::DecodeIncTagtree( CJ2kTileInfo& aTile, TUint32 aWidth, TUint32 aHeight, TUint16& aValue )
    {
    TUint8 bit = 2;
    TInt16 parentState = 0;
    TUint8 contCoding = 1;
    TUint16 idx = 0;
    TUint16 layer = (TUint16)( aTile.LastLayerProcessed() + 1 );

    for ( TInt16 index = TInt16( iTagTreeLevel - 1 ); index >= 0; --index )
        {
        idx = (TUint16)( ( aHeight >> index ) * ( (TUint32)( iCodeBlockSize.iWidth + ( 1 << index ) - 1 ) >> index ) + ( aWidth >> index ) );
        if ( ( IncTagState( index, idx ) < parentState ) && IncTagValue( index, idx ) == 0xffff )
            {
            SetIncTagState( index, idx, (TUint16)( parentState - contCoding ) );
            }

        while ( contCoding && IncTagState( index, idx ) < layer && IncTagValue( index, idx ) == 0xffff )
            {
            if ( ReadBit( bit, aTile ) )
                {
                return ETrue;
                }

            if ( bit == 1 )
                {
                SetIncTagValue( index, idx, IncTagState( index, idx ) );
                }
            IncrementIncTagState( index, idx );
            }
        parentState = IncTagState( index, idx );
        if ( bit == 0 || IncTagValue( index, idx ) == 0xffff )
            {
            contCoding = 0;
            }
        }
    aValue = IncTagValue( 0, idx );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::DecodeMsbTagtree
// Decode the msb tag tree
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::DecodeMsbTagtree( CJ2kTileInfo& aTile, TUint32 aWidth, TUint32 aHeight, TUint16& aValue )
    {
    TUint8 bit = 2;
    TInt16 parentState = 0;
    TUint8 contCoding = 1;
    TUint16 idx = 0;
    TUint16 layer = 0xffff;

    for ( TInt16 index = (TUint16)( iTagTreeLevel - 1 ); index >= 0; --index )
        {
        idx = (TUint16)( ( aHeight >> index ) * ( (TUint32)( iCodeBlockSize.iWidth + ( 1 << index ) - 1 ) >> index ) + ( aWidth >> index ) );

        if ( ( MsbTagState( index, idx ) < parentState ) && MsbTagValue( index, idx ) == 0xffff )
            {
            SetMsbTagState( index, idx, (TUint16)( ( parentState - contCoding ) ) );
            }

        while ( contCoding && MsbTagState( index, idx ) < layer && MsbTagValue( index, idx ) == 0xffff )
            {
            if ( ReadBit( bit, aTile ) )
                {
                return ETrue;
                }

            if ( bit == 1 )
                {
                SetMsbTagValue( index, idx, MsbTagState( index, idx ) );
                }
            IncrementMsbTagState( index, idx );
            }
        parentState = MsbTagState( index, idx );
        if ( bit == 0 || MsbTagValue( index, idx ) == 0xffff )
            {
            contCoding = 0;
            }
        }
    aValue = MsbTagValue( 0, idx );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::BackupIncTagtreeL
// Backup the included tag tree
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::BackupIncTagtreeL( HBufC16*& aTagValue, HBufC16*& aTagState )
    {
    if ( aTagValue && aTagState )
        {
        if ( aTagValue->Des().MaxLength() < iIncTagTreeValue->Length() )
            {
            aTagValue = aTagValue->ReAllocL( iIncTagTreeValue->Length() );
            }
        if ( aTagState->Des().MaxLength() < iIncTagTreeState->Length() )
            {
            aTagState = aTagState->ReAllocL( iIncTagTreeState->Length() );
            }
        }
    else
        {
        aTagValue = HBufC16::NewMaxLC( iIncTagTreeValue->Length() );
        aTagState = HBufC16::NewMaxLC( iIncTagTreeState->Length() );
        }
    *aTagValue = *iIncTagTreeValue;
    *aTagState = *iIncTagTreeState;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::BackupMsbTagtreeL
// Backup the msb tag tree
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::BackupMsbTagtreeL( HBufC16*& aTagValue, HBufC16*& aTagState )
    {
    if ( aTagValue && aTagState )
        {
        if ( aTagValue->Des().MaxLength() < iMsbTagTreeValue->Length() )
            {
            aTagValue = aTagValue->ReAllocL( iMsbTagTreeValue->Length() );
            }
        if ( aTagState->Des().MaxLength() < iMsbTagTreeState->Length() )
            {
            aTagState = aTagState->ReAllocL( iMsbTagTreeState->Length() );
            }
        }
    else
        {
        aTagValue = HBufC16::NewMaxLC( iMsbTagTreeValue->Length() );
        aTagState = HBufC16::NewMaxLC( iMsbTagTreeState->Length() );
        }
    *aTagValue = *iMsbTagTreeValue;
    *aTagState = *iMsbTagTreeState;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::RestoreIncTagtree
// Restore the included tag tree
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::RestoreIncTagtree( HBufC16 *aTagValue, HBufC16 *aTagState )
    {
    *iIncTagTreeValue = *aTagValue;
    *iIncTagTreeState = *aTagState;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::RestoreMsbTagtree
// Restore the msb tag tree
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::RestoreMsbTagtree( HBufC16 *aTagValue, HBufC16 *aTagState )
    {
    *iMsbTagTreeValue = *aTagValue;
    *iMsbTagTreeState = *aTagState;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::FreeBackupTagtree
// Release the temporary backup memory
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::FreeBackupTagtree( HBufC16*& aIncTagValue, HBufC16*& aIncTagState,
                                    HBufC16*& aMsbTagValue, HBufC16*& aMsbTagState )
    {
    TInt count = 0;
    if ( aIncTagValue )
        {
        delete aIncTagValue;
        aIncTagValue = 0;
        ++count;
        }
    if ( aIncTagState )
        {
        delete aIncTagState;
        aIncTagState = 0;
        ++count;
        }
    if ( aMsbTagValue )
        {
        delete aMsbTagValue;
        aMsbTagValue = 0;
        ++count;
        }
    if ( aMsbTagState )
        {
        delete aMsbTagState;
        aMsbTagState = 0;
        ++count;
        }
    CleanupStack::Pop( count );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IsIncludedL
// Is the layer has been included in previous packet
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::IsIncludedL( TUint16 aLayer )
    {
    TInt entries = ( iLayer % 8 ) ? ( ( iLayer / 8 ) + 1 ) : ( iLayer / 8 );
    if ( iIncluded == 0 || iIncluded->Des().MaxLength() < entries )
        {
        if ( iIncluded )
            {
            TInt diff = entries - iIncluded->Length();
            iIncluded = iIncluded->ReAllocL( entries );
            while ( diff-- )
                {
                iIncluded->Des().Append( (TUint8)0 );
                }
            }
        else
            {
            iIncluded = HBufC8::NewMaxL( entries );
            iIncluded->Des().FillZ();
            }
        }
    return IncludedAt( aLayer );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IncludedAt
// Get the inclusive information at specific layer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::IncludedAt( TUint16 aLayer ) const
    {
    TInt entry = aLayer / 8;
    TInt bit   = aLayer % 8;
    return (TUint8)( ( iIncluded->Des() )[entry] & ( 0x01 << bit ) );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetIncluded
// Set the inclusive information at specific layer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::SetIncluded( TUint16 aLayer )
    {
    TInt entry = aLayer / 8;
    TInt bit   = aLayer % 8;
    TPtr8 tmpPtr = iIncluded->Des();
    tmpPtr[entry] |= ( 0x01 << bit );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IsBackupNeeded
// Is tag tree backup is required for underflow recovery
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::IsBackupNeeded( CJ2kTileInfo& aTile )
    {
    return !( aTile.IsPPT() || aTile.ImageInfo().IsPPM() );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::StartReadBit
// Read a bit from the packet header stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::StartReadBit( CJ2kTileInfo& aTile )
    {
    return ( !IsRecoverFromIncomplete() ) ?
        aTile.PacketHeaderReader().StartReadBit() : 
        (TUint8)EFalse;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ReadBit
// Read a bit from the packet header stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::ReadBit( TUint8& aBit, CJ2kTileInfo& aTile )
    {
    return aTile.PacketHeaderReader().ReadBit( aBit );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ReadBits
// Read some bits from the packet header stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::ReadBits( TUint8& aBit, TUint8 aBitLen, CJ2kTileInfo& aTile )
    {
    return aTile.PacketHeaderReader().ReadBits( aBit, aBitLen );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ReadBits
// Read some bits from the packet header stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::ReadBits( TUint32& aBit, TUint8 aBitLen, CJ2kTileInfo& aTile )
    {
    return aTile.PacketHeaderReader().ReadBits( aBit, aBitLen );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::AlignReader
// Align the stream to the next byte boundary if necessary
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kPacket::AlignReader( CJ2kTileInfo& aTile )
    {
    aTile.PacketHeaderReader().AlignReader();
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ReadSOPMarker
// Try to consume the SOP marker if there is one
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::ReadSOPMarker( CJ2kTileInfo& aTile )
    {
    // SOP may be inside the CodeStream only
    return CONST_CAST( TJ2kStreamReader&, aTile.StreamReader() ).ReadSOPMarker();
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ReadEPHMarker
// Try to consume the EPH marker if there is one
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::ReadEPHMarker( CJ2kTileInfo& aTile )
    {
    // EPH may be inside PPT or PPM or CodeStream
    return aTile.PacketHeaderReader().ReadEPHMarker();
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ReadSOP_EPHMarker
// Try to re-align stream to next byte boundary if necessary,
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kPacket::ReadSOP_EPHMarker( CJ2kComponentInfo& aComponent, CJ2kTileInfo& aTile, TBool aSOP )
    {
    TJ2kStreamReader& reader = CONST_CAST( TJ2kStreamReader&, aTile.StreamReader() );

    TUint8 incomplete = reader.TryReAlignReader();
    incomplete = reader.TryReadEPHMarker();

    if ( !incomplete && aSOP && aComponent.IsSOPMarkerUsed() && ( !IsHeader() || !IsBody() ) )
        {
        incomplete = ReadSOPMarker( aTile );
        }

    return incomplete;
    }
