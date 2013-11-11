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

#ifndef __MMF_TMMFAUDIO_CODEC_BASE_H__
#define __MMF_TMMFAUDIO_CODEC_BASE_H__
#include <e32std.h>
#include "MMFAudioDataSettings.h"

const TInt KClamp = 32768;
const TInt KMaskSign16bit = 0x8000;
const TInt KMaskSign8bit = 0x80;
const TInt KAndMask8bit = 0xff;
const TInt KMaxImaAdpcmTableEntries = 88;

const TUint KImaAdpcmBlockAlign = 256;
//samples per block = [((block align - (4 * nChannels)) * 8) / (bits per sample * nChannels)] + 1
const TUint KImaAdpcmSamplesPerBlock = ((KImaAdpcmBlockAlign - 4)*2) + 1;	//505
const TUint KImaAdpcmStereoSamplesPerBlock = (KImaAdpcmBlockAlign - 8) + 1; //249
const TUint KImaAdpcmTempBufferSize = KImaAdpcmSamplesPerBlock*2;
const TUint KImaAdpcmStereoTempBufferSize = KImaAdpcmStereoSamplesPerBlock*4;

//Gsm coded Frame size
const TInt KCodedBufferSize = 152;

// Base of Audio codecs
/*
*
* class TMMFAudioCodecBase
*
*/
class TMMFAudioCodecBase
	{
public:
	inline TMMFAudioCodecBase();
	//
	virtual void Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)=0;
	};

inline TMMFAudioCodecBase::TMMFAudioCodecBase()
	{}

#endif //__MMF_TMMFAUDIO_CODEC_BASE_H__
