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
* Description:  JP2KCodeBlock class used to collect the codeblock
*                related information such as compressed data and data length.
*
*/


#ifndef __JP2KCODEBLOCK_INL__
#define __JP2KCODEBLOCK_INL__

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::LastPass
// Get the last pass.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt8 CJ2kCodeBlock::LastPass() const
    {
    return iLastPass;
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::PassIndex
// Get the pass index.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kCodeBlock::PassIndex() const
    {
    return iPassIndex;
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::EmptyBitplanes
// Get the number of empty bit planes.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kCodeBlock::EmptyBitplanes() const
    {
    return iEmptyBitplane;
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::CodeBlockCanvas
// Get the canvas of the codeblock.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TRect& CJ2kCodeBlock::CodeBlockCanvas() const
    {
    return iCodeBlockCanvas;
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::DataLength
// Get the data length.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint32 CJ2kCodeBlock::DataLength() const
    {
    return iDataLength;
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::CblkLength
// Get the data size at specific segment.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kCodeBlock::CblkLength(TUint16 aIndex) const
    {
    return (*iDataSize)[aIndex];
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::PassesPerSegment
// Get the pass per segment at specific segment.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 CJ2kCodeBlock::PassesPerSegment(TUint16 aIndex) const
    {
    return (*iPassesPerSegment)[aIndex];
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::ResetPassIndex
// Reset the pass index to 0.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kCodeBlock::ResetPassIndex()
    {
    iPassIndex = 0;
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::IncrementPassIndex
// Increment the pass index.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kCodeBlock::IncrementPassIndex()
    {
    ++iPassIndex;
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::SetCodeBlockDecoded
// Set to indicate that the codeblock has been decoded
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CJ2kCodeBlock::SetCodeBlockDecoded()
    {
    iIsDecoded = (TUint8)1;
    }

#endif // __JP2KCODEBLOCK_INL__
