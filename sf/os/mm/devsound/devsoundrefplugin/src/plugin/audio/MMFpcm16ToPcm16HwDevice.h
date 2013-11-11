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
// include\mmf\src\plugin\Hwdevice\audio\pcm16ToPcm16HwDevice.h
// 
//

#ifndef __MMF_PCM16_TO_PCM16_HW_DEVICE_H__
#define __MMF_PCM16_TO_PCM16_HW_DEVICE_H__
#include <mmf/server/mmfswcodecwrapper.h>

#ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
	#include "mdasoundadapter.h"
#else
	#include <mdasound.h>
#endif

#include "../../swcodecwrapper/mmfswaudioinput.h"

#include "../../swcodecwrapper/mmfSwCodecDataPath.h"

// for the bitrate custom interface
#include <mmf/server/devsoundstandardcustominterfaces.h>

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
class CMMFPcm16ToPcm16HwDevice : public CMMFSwCodecWrapper,
								 public MMMFDevSoundCustomInterfaceBitRate
	{
public:
	static  CMMFPcm16ToPcm16HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	virtual TAny* CustomInterface(TUid aInterfaceId);
	~CMMFPcm16ToPcm16HwDevice(); 

	TUint CalculateBufferSize();
private:
	// from MMMFDevSoundCustomInterfaceBitRate
	virtual void GetSupportedBitRatesL(RArray<TInt>& aSupportedBitRates);
	virtual TInt BitRateL();
	virtual void SetBitRateL(TInt aBitRate);
	
	void BitRatesFromSampleRatesL(RArray<TInt>& aSupportedBitRates, const RArray<TInt>& aSupportedSampleRates);

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

#endif //__MMF_PCM16_TO_PCM16_HW_DEVICE_H__

