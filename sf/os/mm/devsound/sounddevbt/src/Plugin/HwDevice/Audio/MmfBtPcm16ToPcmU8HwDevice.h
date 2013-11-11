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
//

#ifndef __MMF_BT_PCM16_TO_PCMU8_HW_DEVICE_H__
#define __MMF_BT_PCM16_TO_PCMU8_HW_DEVICE_H__

#include <mmfbtswcodecwrapper.h>
#include "MmfBtAudioCodec.h"

#include "MMFAudioS16ToU8PcmCodec.h"

const TInt KPCM16ToPCMU8SourceBufferSize = 0x2000;
const TInt KPCM16ToPCMU8SinkBufferSize = 0x1000; //sink buffer length is half source

/*
*
* class CMMFPcm16ToPcmU8HwDevice
*
*/
class CMMFPcm16ToPcmU8HwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcm16ToPcmU8HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFPcm16ToPcmU8HwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcm16ToPcmU8Codec converts Pcm16 to Pcm unsigned 8 bit
*
*/
class CMMFPcm16ToPcmU8Codec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCM16ToPCMU8SourceBufferSize;};
	virtual TUint SinkBufferSize() {return KPCM16ToPCMU8SinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioS16ToU8PcmCodec iAudioS16ToU8Pcm ;
	};

#endif //__MMF_BT_PCMS16_TO_PCMS8_HW_DEVICE_H__

