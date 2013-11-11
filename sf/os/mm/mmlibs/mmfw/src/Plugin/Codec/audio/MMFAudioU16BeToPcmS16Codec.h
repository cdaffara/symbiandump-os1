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
// MMFAudioU16BEPcmS16Codec.h
// 
//


#ifndef __AUDIOU16BE_PCMS16_CODEC_H__
#define __AUDIOU16BE_PCMS16_CODEC_H__
#include <mmf/server/mmfcodec.h>
#include "MMFAudioPcmU16BeToPcmS16Codec.h"


class CMMFAudioU16BEPcmS16Codec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFAudioU16BEPcmS16Codec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFAudioU16BEPcmS16Codec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TMMFAudioPcmU16BeToPcmS16Codec iAudioU16BEToS16Pcm ;
	};


#endif 
