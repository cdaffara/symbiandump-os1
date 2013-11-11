// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// src\plugin\Hwdevice\audio\MmfBtPcm16ToPcm16HwDevice.h
// 
//

#ifndef __MMF_BT_PCM16_TO_PCM16_HW_DEVICE_H__
#define __MMF_BT_PCM16_TO_PCM16_HW_DEVICE_H__
#include <mmfbtswcodecwrapper.h>
#include <mdasound.h>

#include "../../../swcodecwrapper/mmfBtSwCodecDataPath.h"

//note we need to keep this buffer at 8K as the tone utility expects 8K
const TInt KPCM16ToPCM16BufferSize = 0x2000;
const TInt KDevSoundMinFrameSize = 0x800; //2K
const TInt KDevSoundMaxFrameSize = 0x4000;  //16K
const TInt KDevSoundDeltaFrameSize = 0x800; //2K
const TInt KDevSoundFramesPerSecond = 4;

/*
*
* class CMMFPcm16ToPcm16HwDevice
*
*/
class CMMFPcm16ToPcm16HwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcm16ToPcm16HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFPcm16ToPcm16HwDevice(); 

	TUint CalculateBufferSize();
private:
	void ConstructL();
	};

/*
*
* class CMMFPcm16ToPcm16Codec converts Pcm16 to pcm16
*
*/
class CMMFPcm16ToPcm16Codec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& /*aSource*/, CMMFBuffer& /*aDest*/);
//	virtual TUint SourceBufferSize() {return KPCM16ToPCM16BufferSize;};
//	virtual TUint SinkBufferSize() {return KPCM16ToPCM16BufferSize;};
	virtual TUint SourceBufferSize();
	virtual TUint SinkBufferSize();
	virtual TBool IsNullCodec() {return ETrue;};

	void SetHwDevice(CMMFPcm16ToPcm16HwDevice* aHwDevice);
private:
	TUint iBufferSize;
	CMMFPcm16ToPcm16HwDevice* iHwDevice;
	};

#endif //__MMF_BT_PCM16_TO_PCM16_HW_DEVICE_H__

