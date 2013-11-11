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
// MMFPcm16MuLawCodec.h
// 
//


#ifndef __PCM16_MULAW_CODEC_H__
#define __PCM16_MULAW_CODEC_H__
#include <mmf/server/mmfcodec.h>
#include "MMFAudioSPcm16ToMuLawCodec.h"

class CMMFPcm16MulawCodec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFPcm16MulawCodec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFPcm16MulawCodec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TMMFAudioSPcm16ToMuLawCodec i16PcmToMulaw ;

	};


#endif 
