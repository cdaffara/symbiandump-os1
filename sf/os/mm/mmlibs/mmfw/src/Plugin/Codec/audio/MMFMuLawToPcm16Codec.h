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
// MMFMuLawtoPcm16.h
// 
//


#ifndef __MULAW_PCM16_CODEC_H__
#define __MULAW_PCM16_CODEC_H__
#include <mmf/server/mmfcodec.h>
#include "MMFAudioMuLawToS16PcmCodec.h"


class CMMFMulawPcm16Codec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFMulawPcm16Codec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFMulawPcm16Codec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TMMFAudioMuLawToS16PcmCodec iMulawTo16Pcm ;

	};


#endif 
