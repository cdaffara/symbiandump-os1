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

#include "MMFAudioSPcm16ToMuLawCodec.h"

/***
*
* Convert
*
*/
EXPORT_C void TMMFAudioSPcm16ToMuLawCodec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	TInt pcm;
	while (aSamples--)
		{
		pcm = (aSrc[0]+(aSrc[1]<<8));
		*aDst++ = PcmSampleToMuLaw(STATIC_CAST(TInt16 , pcm));
		aSrc+=2;
		}
	}

//[ conversion look up table pcm16 to Mulaw ]
const TInt8 TMMFAudioSPcm16ToMuLawCodec::KExpLut[] = 
	{
	0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
	};

/***
*
* PcmSampleToMulaw
* @param Mulaw
* @return TUint8
*
*/
TUint8 TMMFAudioSPcm16ToMuLawCodec::PcmSampleToMuLaw(TInt aPcm)
	{
    TInt sign, exponent, mantissa;
    TUint8 ulawbyte;

    sign = (aPcm >> 8) & KMaskSign8bit;		
    if ( sign != 0 )
		aPcm = STATIC_CAST( TUint16,-aPcm);
	if ( STATIC_CAST(TUint , aPcm) > STATIC_CAST(TUint , KMda16PcmToMulawClip ))
		aPcm = KMda16PcmToMulawClip;	

    aPcm = STATIC_CAST(TInt16 , aPcm + KMda16PcmToMulawBias);
    exponent = KExpLut[( aPcm >> 7 ) & KAndMask8bit];
    mantissa = ( aPcm >> ( exponent + 3 ) ) & 0x0F;
    ulawbyte = STATIC_CAST( TUint8, ~ ( sign | ( exponent << 4 ) | mantissa ));

    return ulawbyte;
	}
