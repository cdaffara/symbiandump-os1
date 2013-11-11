// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __AUDIOS16_S16PCM_SE_CODEC_H__
#define __AUDIOS16_S16PCM_SE_CODEC_H__
#include <mmf/server/mmfcodec.h>
#include "MMFAudioPcm16SwapEndianCodecCommon.h"


class CMMFAudio16PcmSwapEndianCodec : public CMMFCodec
	{
public:
	static CMMFAudio16PcmSwapEndianCodec* NewL(TAny* aInitParams);
	virtual ~CMMFAudio16PcmSwapEndianCodec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFAudio16PcmSwapEndianCodec();
	void ConstructL(TAny* aInitParams);
private:
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TMMFAudioPcm16SwapEndianCodec i16PcmTo16PcmSwap ;

	};


#endif 
