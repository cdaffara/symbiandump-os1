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



#ifndef __PCM16_IMAADPCM_CODEC_H__
#define __PCM16_IMAADPCM_CODEC_H__

#include "MMFCodecBaseDefinitions.h"
#include "MMFAudioCodecBase.h"

//mono IMA - i16PcmToImaAdpcm with 1 (mono)
class CMMFPcm16ImaAdPcmCodec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFPcm16ImaAdPcmCodec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFPcm16ImaAdPcmCodec();
	void ConstructL(TAny* aInitParams);

	virtual void ResetL();
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TUint iTempSrcBufferCount;
	TUint8* iTempSrcBufferPtr;
	TUint8 iTempSrcBuffer[KImaAdpcmTempBufferSize];
	TMMF16PcmToImaAdpcmCodecOld i16PcmToImaAdpcm;
	TUint iLastFrameNumber;
	};


#endif 
