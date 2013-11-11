/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef __MMF_TMMF_AUDIO_SPCM16_TO_MULAW_CODEC_H__
#define __MMF_TMMF_AUDIO_SPCM16_TO_MULAW_CODEC_H__
#include "MMFAudioCodecBase.h" 

const TInt KMda16PcmToMulawExpLutSize = 256;
const TInt KMda16PcmToMulawClip		  = 32635;
const TInt KMda16PcmToMulawBias		  = 0x84;

/*
*
* class TMMFAudioSPcm16ToMuLawCodec
*
*/
class TMMFAudioSPcm16ToMuLawCodec : public TMMFAudioCodecBase
	{
public:
	IMPORT_C virtual void Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples);
private:
	TUint8 PcmSampleToMuLaw(TInt aPcm);
private:
	const static TInt8 KExpLut[KMda16PcmToMulawExpLutSize];
	};

#endif //__MMF_TMMF_AUDIO_SPCM16_TO_MULAW_CODEC_H__
