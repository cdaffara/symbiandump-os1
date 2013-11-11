/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMF_TMMFAUDIO_PCM16_TO_IMAADPCM_CODEC_H__
#define __MMF_TMMFAUDIO_PCM16_TO_IMAASPCM_CODEC_H__
#include <e32std.h>
#include "MMFAudioCodecBase.h"
#include "MMFImaAudioCodecState.h"

/**
*
* TMMFAudioPcm16ToImaAdpcmCodec 
*
*/
const TInt KImaAdpcmCodecIndexTableSize = 16;
const TInt KImaAdpcmCodecStepTableSize = 89;

/*
*
* class TMMFAudioPcm16ToImaAdpcmCodec
*
*/
class TMMFAudioPcm16ToImaAdpcmCodec : public TMMFAudioCodecBase
	{
public:
	inline TMMFAudioPcm16ToImaAdpcmCodec();
	IMPORT_C virtual void Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples);
	IMPORT_C void SetState(const TMMFImaAdpcmCodecState& aState);
	IMPORT_C const TMMFImaAdpcmCodecState& GetState();
private:
	TMMFImaAdpcmCodecState iState;	//supports mono only
private:
	static const TInt KIndexTable[KImaAdpcmCodecIndexTableSize];
	static const TInt KStepSizeTable[KImaAdpcmCodecStepTableSize];
	TBool iBufferStep;
	TInt iBuffer;
	};

inline TMMFAudioPcm16ToImaAdpcmCodec::TMMFAudioPcm16ToImaAdpcmCodec() : iBufferStep(ETrue), iBuffer(0) {}
#endif //__MMF_TMMFAUDIO_PCM16_TO_IMAADPCM_CODEC_H__
