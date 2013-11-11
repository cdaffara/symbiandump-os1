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
// MMFAudioU16PcmS16Codec.h
// 
//


#ifndef __AUDIOU16_PCMS16_CODEC_H__
#define __AUDIOU16_PCMS16_CODEC_H__
#include <mmf/server/mmfcodec.h>
#include "MMFAudioU16ToS16PcmCodec.h"


class CMMFAudioU16PcmS16Codec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFAudioU16PcmS16Codec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFAudioU16PcmS16Codec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TMMFAudioU16ToS16PcmCodec iAudioU16ToS16Pcm ;

	};


#endif 
