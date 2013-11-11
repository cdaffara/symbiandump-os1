/*
* Copyright (c) 1997-2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef __IMAADSTEREOPCM_PCM16_CODEC_H__
#define __IMAADSTEREOPCM_PCM16_CODEC_H__

#include "MMFAudioImaAdpcmToS16PcmCodec.h"

//stereo so iImaAdpcmTo16Pcm constructed with 2 (Stereo)
class CMMFImaAdStereoPcmPcm16Codec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFImaAdStereoPcmPcm16Codec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFImaAdStereoPcmPcm16Codec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TUint iTempSrcBufferCount;
	TUint8* iTempSrcBufferPtr;
	TUint8 iTempSrcBuffer[KImaAdpcmBlockAlign];

	TMMFAudioImaAdpcmToS16PcmCodec iImaAdpcmTo16Pcm ;
	TUint iLastFrameNumber;
	};


#endif 
