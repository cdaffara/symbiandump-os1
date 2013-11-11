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

#include "parsers.h"

// The ID3 tag has a 10 byte header.
static const TInt KID3HeaderSize = 10;


//
// Checks if an ID3 header is present at the current
// reader position, and skips over it if it is.
// Returns ETrue if a header was found, EFalse otherwise.
//
TBool TID3Parser::ReadAndSkipID3L(CReader& aReader)
	{
	TBuf8<KID3HeaderSize> header;
	TUint8 a;
	TUint8 b;
	TUint8 c;
	TUint8 d;
	
	header.SetLength(KID3HeaderSize);
	aReader.ReadBytesL(header);
	
	do
		{
		// Look for the ID3 header.
		if ((header[0] != 'I') || (header[1] != 'D') || (header[2] != '3'))
			{
			break;
			}
			
		// The last 4 bits of byte[5] should be zero.
		if ((header[5] & 0x0f))	// [00001111]
			{
			break;
			}
			
		// Read the tag size. It's stored in a sync-safe manner
		// (the highest bit is never set so it won't be confused
		// with a frame-sync). If the highest bit is set, it's invalid.
		a = header[6];
		b = header[7];
		c = header[8];
		d = header[9];
		
		// OR the values together and check that the highest bit of
		// the result isn't set. Saves on doing four individual checks.
		if ((a | b | c | d) & 0x80) // [10000000]
			{
			break;
			}
		
		// Convert from sync-safe format to normal format.
		// [0aaaaaaa][0bbbbbbb][0ccccccc][0ddddddd] is changed to
		// [0000aaaa][aaabbbbb][bbcccccc][cddddddd]
		//
		// Copy bit[0] of c into bit[7] of d.
		d |= ((c & 0x01) << 7);
		c >>= 1;
		
		// Copy bit[1,0] of b into bit[7,6] of c.
		c |= ((b & 0x03) << 6);
		b >>= 2;
		
		// Copy bit[2,1,0] of a into bit[7,6,5] of b.
		b |= ((a & 0x07) << 5);
		a >>= 3;
		
		TInt length = MAKE_INT32(a, b, c, d);
		aReader.SeekL(length);
		return ETrue;
		}
	while (EFalse);
	
	// It's not an ID3 header.
	// Undo the header read.
	aReader.SeekL(-KID3HeaderSize);
	return EFalse;
	}
