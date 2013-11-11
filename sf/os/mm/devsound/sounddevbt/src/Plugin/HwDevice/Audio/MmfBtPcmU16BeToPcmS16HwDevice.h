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
// src\plugin\Hwdevice\audio\MmfBtPcmU16BeToPcmS16HwDevice.h
// 
//

#ifndef __MMF_BT_PCMU16BE_TO_PCM16_HW_DEVICE_H__
#define __MMF_BT_PCMU16BE_TO_PCM16_HW_DEVICE_H__
#include <mmfbtswcodecwrapper.h>
#include "MmfBtAudioCodec.h"
#include "MMFAudioPcmU16BeToPcmS16Codec.h"

const TInt KPCMU16BeToPCM16SourceBufferSize = 0x1000;
const TInt KPCMU16BeToPCM16SinkBufferSize   = KPCMU16BeToPCM16SourceBufferSize; 

/*
*
* class CMMFPcmU16BeToPcmS16HwDevice
*
*/
class CMMFPcmU16BeToPcmS16HwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcmU16BeToPcmS16HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFPcmU16BeToPcmS16HwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcmU16BeToPcmS16Codec converts Pcm16 Big endian to Pcm16
*
*/
class CMMFPcmU16BeToPcmS16Codec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCMU16BeToPCM16SourceBufferSize;};
	virtual TUint SinkBufferSize() {return KPCMU16BeToPCM16SinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioPcmU16BeToPcmS16Codec iAudioPcmU16BeToPcmS16;
	};

#endif //__MMF_BT_PCMU16BE_TO_PCM16_HW_DEVICE_H__

