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

#ifndef __MMF_TMMFAUDIO_SPCM16_TO_ALAW_CODEC_H__
#define __MMF_TMMFAUDIO_SPCM16_TO_ALAW_CODEC_H__
#include <e32std.h>
#include "MMFAudioCodecBase.h"

const TInt KMda16PcmToAlawExpLutSize = 128;
const TInt KMda16PcmToAlawClip       = 31744;

/*
*
* class TMMFAudioSPcm16ToAlawCodec
*
*/
class TMMFAudioSPcm16ToAlawCodec : public TMMFAudioCodecBase
	{
public:
	IMPORT_C virtual void Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples);
private:
	TUint8 PcmSampleToAlaw(TInt aPcm);
private:
	const static TInt8 KExpLut[KMda16PcmToAlawExpLutSize];
	};

#endif //__MMF_TMMFAUDIO_SPCM16_TO_ALAW_CODEC_H__
