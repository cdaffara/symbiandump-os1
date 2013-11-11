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
* Description:  Basic utilities to handle conversion and object safe deletion.
*
*/


#ifndef __JP2KIMAGEUTILS_INL__
#define __JP2KIMAGEUTILS_INL__

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PtrReadUtil::ReadBigEndianUint16
// Reads big endian value from given buffer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 PtrReadUtil::ReadBigEndianUint16( const TUint8* aPtr )
    {
    return TUint16( ( aPtr[0] << 8 ) | aPtr[1] );
    }

// -----------------------------------------------------------------------------
// PtrReadUtil::ReadBigEndianUint32
// Reads big endian value from given buffer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint32 PtrReadUtil::ReadBigEndianUint32( const TUint8* aPtr )
    {
    return TUint32( ( aPtr[0] << 24 ) | ( aPtr[1] << 16 ) | ( aPtr[2] << 8 ) | aPtr[3] );
    }

// -----------------------------------------------------------------------------
// PtrReadUtil::ReadBigEndianUint16Inc
// Reads big endian value from given buffer and increments pointer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint16 PtrReadUtil::ReadBigEndianUint16Inc( const TUint8*& aPtr )
    {
    TUint16 result = ReadBigEndianUint16( aPtr );
    aPtr += 2;
    return result;
    }

// -----------------------------------------------------------------------------
// PtrReadUtil::ReadBigEndianUint32Inc
// Reads big endian value from given buffer and increments pointer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint32 PtrReadUtil::ReadBigEndianUint32Inc( const TUint8*& aPtr )
    {
    TUint32 result = ReadBigEndianUint32( aPtr );
    aPtr += 4;
    return result;
    }

#endif
