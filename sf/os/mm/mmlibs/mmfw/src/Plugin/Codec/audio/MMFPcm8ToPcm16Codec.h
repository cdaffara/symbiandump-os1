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
// MMFPcm8Pcm16Codec.h
// 
//


#ifndef __PCM8_PCM16_CODEC_H__
#define __PCM8_PCM16_CODEC_H__
#include <mmf/server/mmfcodec.h>
#include "MMFAudioPcm8ToPcm16Codec.h"

class CMMFPcm8Pcm16Codec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFPcm8Pcm16Codec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFPcm8Pcm16Codec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TMMFAudioPcm8ToPcm16Codec iAudioS8ToS16Pcm ;
	};


#endif 
