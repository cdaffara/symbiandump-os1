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


#ifndef MMFIMAADPCMTOPCM16CODEC_H
#define MMFIMAADPCMTOPCM16CODEC_H

#include "MMFCodecBaseDefinitions.h"
#include "MMFAudioCodecBase.h"
#include <mmf/server/devsoundstandardcustominterfaces.h>

const TUint KMaxImaAdpcmBlockAlign = 2048;
const TUint KImaAdpcmBitsPerSample = 4;

//mono so iImaAdpcmTo16Pcm constructed with 1 (mono)
class CMMFImaAdPcmPcm16Codec : public CMMFCodec, public MMMFDevSoundCustomInterfaceFileBlockLength
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFImaAdPcmPcm16Codec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
	void ConfigureL(TUid aConfigType, const TDesC8& aConfigData);
	// from MMMFDevSoundCustomInterfaceFileBlockLength
	void SetFileBlockLength(TUint aBlockAlign);
protected:
	virtual TInt Extension_(TUint aExtensionId, TAny*& aExtPtr, TAny*);	
private:
	CMMFImaAdPcmPcm16Codec();
	void ConstructL(TAny* aInitParams);
	virtual void ResetL();
	
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;

	TUint iTempSrcBufferCount;
	TUint8* iTempSrcBufferPtr;
	TUint8 iTempSrcBuffer[KMaxImaAdpcmBlockAlign];
	TMMFImaAdpcmTo16PcmCodecOld iImaAdpcmTo16Pcm ;
	TUint iLastFrameNumber;
	
	// allow configuring sample rate
	TUint iChannels;
	TUint iSamplesRate;
	TUint iSamplesPerBlock;
	TUint iBlockAlign;
	};


#endif 
