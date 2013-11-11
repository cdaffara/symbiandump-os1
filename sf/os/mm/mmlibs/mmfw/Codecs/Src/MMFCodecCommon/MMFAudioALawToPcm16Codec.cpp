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

#include "MMFAudioALawToS16PcmCodec.h"

/***
*
* Convert
*
*/
EXPORT_C void TMMFAudioALawToS16PcmCodec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	ASSERT(aSrc);
	ASSERT(aDst);
	
	TInt pcm;
	while (aSamples--)
		{
		pcm = AlawSampleToPcm(*aSrc++);
		*aDst++ = STATIC_CAST( TUint8, pcm&KAndMask8bit);
		*aDst++ = STATIC_CAST( TUint8, pcm>>8);
		}
	
	}

/***
*
* AlawSampleToPcm
* @param aLaw
* @return TInt
*
*/
TInt TMMFAudioALawToS16PcmCodec::AlawSampleToPcm(TUint8 aAlaw)
	{
    TInt sign, exponent, mantissa, sample;
	
    aAlaw ^= 0x55;	//will leave this as is, more readable
    sign = ( aAlaw & KMaskSign8bit );
    aAlaw &= 0x7f;			/* get magnitude */
    if (aAlaw >= 16)
		{
		exponent = (aAlaw >> 4 ) & 0x07;
		mantissa = aAlaw & 0x0F;
		sample = KExpLut[exponent] + ( mantissa << ( exponent + 3 ) );
		}
    else
		sample = (aAlaw << 4) + 8;
	
    if ( sign == 0 )
		sample = -sample;
	
    return sample;
	}

const TInt TMMFAudioALawToS16PcmCodec::KExpLut[] = 
	{
	0, 264, 528, 1056, 2112, 4224, 8448, 16896
	};
