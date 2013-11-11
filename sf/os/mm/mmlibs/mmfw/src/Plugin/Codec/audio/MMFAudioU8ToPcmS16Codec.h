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
// MMFAudioU8PcmS16Codec.h
// 
//


#ifndef __AUDIOU8_PCMS16_CODEC_H__
#define __AUDIOU8_PCMS16_CODEC_H__
#include <mmf/server/mmfcodec.h>
#include "MMFAudioU8ToS16PcmCodec.h"


class CMMFAudioU8PcmS16Codec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFAudioU8PcmS16Codec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFAudioU8PcmS16Codec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath	
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TMMFAudioU8ToS16PcmCodec iAudioU8ToS16Pcm ;

	};


#endif 
