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


#ifndef __JP2KPACKET_INL__
#define __JP2KPACKET_INL__

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kPacket::LastCodeBlockProcessed
// Get the last codeblock processed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kPacket::LastCodeBlockProcessed() const
    {
    return iLastCodeBlock;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetLastCodeBlockProcessed
// Set the last codeblock processed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::SetLastCodeBlockProcessed( TUint16 aLastCodeBlock )
    {
    iLastCodeBlock = aLastCodeBlock;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::NumOfBlocks
// Get number of blocks
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kPacket::NumOfBlocks() const
    {
    return (TUint16)( iCodeBlockSize.iWidth * iCodeBlockSize.iHeight );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::NumOfBlocksX
// Get number of blocks in horizontal direction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kPacket::NumOfBlocksX() const
    {
    return (TUint16)iCodeBlockSize.iWidth;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::NumOfBlocksY
// Get number of blocks in vertical direction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kPacket::NumOfBlocksY() const
    {
    return (TUint16)iCodeBlockSize.iHeight;
    }


// -----------------------------------------------------------------------------
// CJ2kPacket::CodeBlockAt
// Get the codeblock at specific location
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const CJ2kCodeBlock& CJ2kPacket::CodeBlockAt( TUint16 aIndex ) const
    {
    return *iCodeBlockList[aIndex];
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IsBodyIncomplete
// Is packet body read underflow
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kPacket::IsBodyIncomplete() const
    {
    return (TUint8)( iReadControl & EReadBodyIncomplete );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IncTagValue
// Get the included tag tree value at specific level and width
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kPacket::IncTagValue( TUint16 aLevel, TUint16 aWidth ) const
    {
    return ( iIncTagTreeValue->Des() )[( iTagTreeInfo->Des() )[aLevel] + aWidth];
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IncTagState
// Get the included tag tree state at specific level and width
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kPacket::IncTagState( TUint16 aLevel, TUint16 aWidth ) const
    {
    return ( iIncTagTreeState->Des() )[( iTagTreeInfo->Des() )[aLevel] + aWidth];
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::MsbTagValue
// Get the msb tag tree value at specific level and width
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kPacket::MsbTagValue( TUint16 aLevel, TUint16 aWidth ) const
    {
    return ( iMsbTagTreeValue->Des() )[( iTagTreeInfo->Des() )[aLevel] + aWidth];
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::MsbTagState
// Get the msb tag tree state at specific level and width
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kPacket::MsbTagState( TUint16 aLevel, TUint16 aWidth ) const
    {
    return ( iMsbTagTreeState->Des() )[( iTagTreeInfo->Des() )[aLevel] + aWidth];
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetIncTagValue
// Set the included tag tree value at specific level and width
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::SetIncTagValue( TUint16 aLevel, TUint16 aWidth, TUint16 aValue ) 
    {
    TPtr16 tmpPtr = ( iIncTagTreeValue->Des() );
    tmpPtr[( iTagTreeInfo->Des() )[aLevel] + aWidth] = aValue;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetIncTagState
// Set the included tag tree state at specific level and width
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::SetIncTagState( TUint16 aLevel, TUint16 aWidth, TUint16 aState )
    {
    TPtr16 tmpPtr = ( iIncTagTreeState->Des() );
    tmpPtr[( iTagTreeInfo->Des() )[aLevel] + aWidth] = aState;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IncrementIncTagState
// Increment the included tag tree state at specific level and width
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::IncrementIncTagState( TUint16 aLevel, TUint16 aWidth )
    {
    TPtr16 tmpPtr = ( iIncTagTreeState->Des() );
    tmpPtr[( iTagTreeInfo->Des() )[aLevel] + aWidth]++;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetMsbTagValue
// Set the msb tag tree value at specific level and width
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::SetMsbTagValue( TUint16 aLevel, TUint16 aWidth, TUint16 aValue ) 
    {
    TPtr16 tmpPtr = ( iMsbTagTreeValue->Des() );
    tmpPtr[( iTagTreeInfo->Des() )[aLevel] + aWidth] = aValue;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetMsbTagState
// Set the msb tag tree state at specific level and width
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::SetMsbTagState( TUint16 aLevel, TUint16 aWidth, TUint16 aState )
    {
    TPtr16 tmpPtr = ( iMsbTagTreeState->Des() );
    tmpPtr[( iTagTreeInfo->Des() )[aLevel] + aWidth] = aState;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IncrementMsbTagState
// Increment the msb tag tree state at specific level and width
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::IncrementMsbTagState( TUint16 aLevel, TUint16 aWidth )
    {
    TPtr16 tmpPtr = ( iMsbTagTreeState->Des() );
    tmpPtr[( iTagTreeInfo->Des() )[aLevel] + aWidth]++;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IsHeader
// Is packet header has been read
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kPacket::IsHeader() const
    {
    return (TUint8)( iReadControl & EReadHeader );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetHeader
// Set the flag to indicate that packet header has been read
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::SetHeader()
    {
    iReadControl |= EReadHeader;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ResetHeader
// Reset the flag to indicate that packet header has not been read
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::ResetHeader()
    {
    iReadControl &= ~EReadHeader;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IsBody
// Is packet body has been read
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kPacket::IsBody() const
    {
    return (TUint8)( iReadControl & EReadBody );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetBody
// Set the flag to indicate that packet body has been read
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::SetBody()
    {
    iReadControl |= EReadBody;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ResetBody
// Reset the flag to indicate that packet body has not been read
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::ResetBody()
    {
    iReadControl &= ~EReadBody;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetBodyIncomplete
// Set the flag to indicate that packet body read underflow
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::SetBodyIncomplete()
    {
    iReadControl |= EReadBodyIncomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ResetBodyIncomplete
// Reset the flag to indicate that packet body read is not underflow
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::ResetBodyIncomplete()
    {
    iReadControl &= ~EReadBodyIncomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IsRecoverFromIncomplete
// Is the read recover from previous underflow
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kPacket::IsRecoverFromIncomplete() const
    {
    return (TUint8)( iReadControl & EReadRecoverFromIncomplete );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetRecoverFromIncomplete
// Set the flag to indicate that the next read is recover from underflow
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::SetRecoverFromIncomplete()
    {
    iReadControl |= EReadRecoverFromIncomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ResetRecoverFromIncomplete
// Reset the flag to indicate that the next read is not recover from underflow
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::ResetRecoverFromIncomplete()
    {
    iReadControl &= ~EReadRecoverFromIncomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::IsMatch
// Is codeblock number should iLastCodeBlock
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kPacket::IsMatch() const
    {
    return (TUint8)( iReadControl & EReadMatchLastCodeBlock );
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::ResetMatch
// Reset the flag to indicate that the next codeblock should be greater than 
// iLastCodeBlock
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::ResetMatch()
    {
    iReadControl &= ~EReadMatchLastCodeBlock;
    }

// -----------------------------------------------------------------------------
// CJ2kPacket::SetMatch
// Set the flag to indicate that the next codeblock should match iLastCodeBlock
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kPacket::SetMatch()
    {
    iReadControl |= EReadMatchLastCodeBlock;
    }

#endif // __JP2KPACKET_INL__
