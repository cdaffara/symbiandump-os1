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
// MMFAudioS16PcmU16BeCodec.h
// 
//


#ifndef __AUDIOS16_PCMU16BE_CODEC_H__
#define __AUDIOS16_PCMU16BE_CODEC_H__
#include <mmf/server/mmfcodec.h>
#include "MMFAudioPcm16ToPcmU16BECodec.h"


class CMMFAudioS16PcmU16BECodec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFAudioS16PcmU16BECodec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFAudioS16PcmU16BECodec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TMMFAudioPcm16ToPcmU16BECodec iAudioS16ToU16BEPcm ;

	};


#endif 
