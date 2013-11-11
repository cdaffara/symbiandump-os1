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

#include "MMFAudioU16ToS16PcmCodec.h"

/**
*
* Convert
* @param aSrc
* @param aDst
* @param aSamples
* Unsigned 16bit to Signed 16bit
*/
EXPORT_C void TMMFAudioU16ToS16PcmCodec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	TInt s16;
	while (aSamples--)
		{ 
		s16 = (aSrc[0]+(aSrc[1]<<8))-KMaskSign16bit;
		*aDst++ = STATIC_CAST( TInt8, s16&KAndMask8bit);
		*aDst++ = STATIC_CAST( TInt8, (s16>>8)&KAndMask8bit);
		aSrc+=2;
		}
	}
