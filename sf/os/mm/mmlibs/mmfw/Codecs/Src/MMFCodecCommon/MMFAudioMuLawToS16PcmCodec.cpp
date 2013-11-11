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

#include "MMFAudioMuLawToS16PcmCodec.h"

/***
*
* Convert
*
*/
EXPORT_C void TMMFAudioMuLawToS16PcmCodec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	ASSERT(aSrc);
	ASSERT(aDst);
	
	TInt pcm;
	while (aSamples--)
		{
		pcm = MulawSampleToPcm(*aSrc++);
		*aDst++ = STATIC_CAST( TUint8, pcm&KAndMask8bit);
		*aDst++ = STATIC_CAST( TUint8, pcm>>8);
		}
	
	}

/***
*
* MulawSampleToPcm
* @param aMuLaw
* @return TInt
*
*/
TInt TMMFAudioMuLawToS16PcmCodec::MulawSampleToPcm(TUint8 aMulaw)
	{
	TInt sign, exponent, mantissa, sample;
	
    aMulaw = STATIC_CAST( TUint8, ~aMulaw);
    sign = ( aMulaw & KMaskSign8bit );
    exponent = ( aMulaw >> 4 ) & 0x07;
    mantissa = aMulaw & 0x0F;
    sample = KExpLut[exponent] + ( mantissa << ( exponent + 3 ) );
    if ( sign != 0 )
		sample = -sample;
	
    return sample;
	}

const TInt TMMFAudioMuLawToS16PcmCodec::KExpLut[KMdaMulawTo16PcmExpLutSize] = 
	{
	0, 132, 396, 924, 1980, 4092, 8316, 16764
	};
