// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MMFAudioPcmU16BeToPcmS16Codec.h"

/**
*
* Convert
* @param aSrc
* @param aDst
* @param aSamples
* Unsigned 16bit big-endian to Signed 16bit (little-endian) 
*/
EXPORT_C void TMMFAudioPcmU16BeToPcmS16Codec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	TInt s16;
	while (aSamples--)
		{ 
		s16 = ((aSrc[0]<<8)+aSrc[1])-KMaskSign16bit;
		*aDst++ = STATIC_CAST( TInt8, s16&KAndMask8bit);
		*aDst++ = STATIC_CAST( TInt8, (s16>>8)&KAndMask8bit);
		aSrc+=2;
		}
	}
