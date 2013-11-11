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

#include "MMFAudioSPcm16ToALawCodec.h"

/***
*
* Convert
*
*/
EXPORT_C void TMMFAudioSPcm16ToAlawCodec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	TInt16* pcm = (TInt16*)aSrc;
	TInt16* pcmLimit = pcm + aSamples; 

	while (pcm < pcmLimit)
		{
		*aDst++ = PcmSampleToAlaw(*pcm++);
		}

	}

//[ conversion look up table pcm16 to alaw ]
const TInt8 TMMFAudioSPcm16ToAlawCodec::KExpLut[] = 
	{
	1,1,2,2,3,3,3,3,
	4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7
	};

/***
*
* PcmSampleToAlaw
* @param aLaw
* @return TUint8
*
*/
TUint8 TMMFAudioSPcm16ToAlawCodec::PcmSampleToAlaw(TInt aPcm)
	{
    TInt sign;
	TInt exponent;
	TInt mantissa;
	TUint8 alawbyte;

    sign = ((~aPcm) >> 8) & KMaskSign8bit;		
    if (sign == 0)
		aPcm = -aPcm;	
    if (aPcm > KMda16PcmToAlawClip) 
		aPcm = KMda16PcmToAlawClip;

    if (aPcm >= 256)
		{
		exponent = KExpLut[( aPcm >> 8 ) & 0x7F];
		mantissa = ( aPcm >> ( exponent + 3 ) ) & 0x0F;
		alawbyte = STATIC_CAST( TUint8, (( exponent << 4 ) | mantissa));
		}
    else
		alawbyte = STATIC_CAST( TUint8, aPcm >> 4);

    alawbyte ^= (sign ^ 0x55);

    return alawbyte;
	}

