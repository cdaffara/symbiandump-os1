// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "readers.h"

//
// Creates a new CReader object.
//
CReader::CReader(const TDesC8& aBuffer, TReaderType aType)
 :	iBuffer(aBuffer),
 	iType(aType)
	{
	}

//
// Returns ETrue is aPattern is in the buffer, EFalse otherwise.
//
TBool CReader::Match(const TDesC8& aPattern)
	{
	return (iBuffer.Match(aPattern) != KErrNotFound);
	}


//
// Reads aBuf.Length() bytes into a descriptor.
//
void CReader::ReadBytesL(TDes8& aBuf)
	{
	TInt bufLen = aBuf.Length();
	
	if (!CheckEnoughData(bufLen))
		{
		User::Leave(KErrEof);
		}
	
	for (TInt i = 0; i < bufLen; i++)
		{
		aBuf[i] = iBuffer[iBufPos++];
		}
	}


//
// Reads a byte from the buffer.
//
void CReader::ReadByteL(TUint8& aData)
	{
	if (!CheckEnoughData(sizeof(TUint8)))
		{
		User::Leave(KErrEof);
		}
			
	aData = iBuffer[iBufPos++];
	}

//
// Reads 16 bits of data at the current reader position.
// Some formats (mainly those developed by Microsoft) still use little-endian
// encoding of fields. The file format specifications will specify which
// byte-ordering scheme is used.
//
void CReader::Read16L(TUint16& aData, TBool aLittleEndian)
	{
	if (!CheckEnoughData(sizeof(TUint16)))
		{
		User::Leave(KErrEof);
		}
	
	TUint8 a = iBuffer[iBufPos++];
	TUint8 b = iBuffer[iBufPos++];
	
	if (aLittleEndian)
		{
		aData = (b << 8) | a;
		}
	else
		{
		aData = (a << 8) | b;
		}
	}


//
// Reads 32 bits of data at the current reader position.
// Some formats (mainly those developed by Microsoft) still use little-endian
// encoding of fields. The file format specifications will specify which
// byte-ordering scheme is used.
//
void CReader::Read32L(TUint32& aData, TBool aLittleEndian)
	{
	if (!CheckEnoughData(sizeof(TUint32)))
		{
		User::Leave(KErrEof);
		}
	
	TUint8 a = iBuffer[iBufPos++];
	TUint8 b = iBuffer[iBufPos++];
	TUint8 c = iBuffer[iBufPos++];
	TUint8 d = iBuffer[iBufPos++];
	
	if (aLittleEndian)
		{
		aData = MAKE_INT32(d, c, b, a);
		}
	else
		{
		aData = MAKE_INT32(a, b, c, d);
		}
	}


//
// Reads 64 bits of data at the current reader position.
// Some formats (mainly those developed by Microsoft) still use little-endian
// encoding of fields. The file format specifications will specify which
// byte-ordering scheme is used.
//
void CReader::Read64L(TInt64& aData, TBool aLittleEndian)
	{
	TUint32 high;
	TUint32 low;
	
	Read32L(high, aLittleEndian);
	Read32L(low, aLittleEndian);
	
	if (aLittleEndian)
		{
		aData = MAKE_TINT64(low, high);
		}
	else
		{
		aData = MAKE_TINT64(high, low);
		}
	}


//
//
//
TBool CReader::CheckEnoughData(TInt aNeeded)
	{
	return ((iBufPos + aNeeded) <= iBuffer.Length());
	}


//
// Skips forwards or backwards aOffset number of bytes.
//
TInt CReader::Seek(TInt aOffset)
	{
	TInt newBufPos = iBufPos + aOffset;
	
	if ((newBufPos < 0) || (newBufPos >= iBuffer.Length()))
		{
		// Trying to seek past the bounds of the buffer.
		return KErrUnderflow;
		}
		
	iBufPos += aOffset;
	return KErrNone;
	}
	
//
// Skips forwards or backwards aOffset number of bytes.
//
TInt CReader::Seek(TInt64 aOffset)
    {
    TInt64 newBufPos = iBufPos + aOffset;
    
    if ((newBufPos < 0) || (newBufPos >= iBuffer.Length()))
        {
        // Trying to seek past the bounds of the buffer.
        return KErrUnderflow;
        }
        
    iBufPos += aOffset;
    return KErrNone;
    }

