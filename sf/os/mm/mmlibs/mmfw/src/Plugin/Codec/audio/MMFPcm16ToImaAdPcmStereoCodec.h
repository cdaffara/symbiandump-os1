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


#ifndef __PCM16_IMAADPCMSTEREO_CODEC_H__
#define __PCM16_IMAADPCMSTEREO_CODEC_H__
#include <mmf/server/mmfcodec.h>
#include "MMFAudioPcm16ToImaAdpcmCodec.h"		  

//this is almost the same as the mono IMA, expect i16PcmToImaAdpcm is construct with 2 Channels for Stereo
class CMMFPcm16ImaAdPcmStereoCodec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFPcm16ImaAdPcmStereoCodec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFPcm16ImaAdPcmStereoCodec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TUint iTempSrcBufferCount;
	TUint8* iTempSrcBufferPtr;
	TUint8 iTempSrcBuffer[KImaAdpcmStereoTempBufferSize];
	TMMFImaAdpcmTo16PcmCodecOld i16PcmToImaAdpcm;
	TUint iLastFrameNumber;
	};


#endif 
