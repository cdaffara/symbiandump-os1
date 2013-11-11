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
// MMFAudioS16PcmU8Codec.h
// 
//


#ifndef __AUDIOS16_PCMU8_CODEC_H__
#define __AUDIOS16_PCMU8_CODEC_H__
#include <mmf/server/mmfcodec.h>
#include "MMFAudioS16ToU8PcmCodec.h"


class CMMFAudioS16PcmU8Codec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFAudioS16PcmU8Codec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFAudioS16PcmU8Codec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TMMFAudioS16ToU8PcmCodec iAudioS16ToU8Pcm ;

	};


#endif 
