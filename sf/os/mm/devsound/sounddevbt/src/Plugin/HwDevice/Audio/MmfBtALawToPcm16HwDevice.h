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
// MmfBtpcm16ToPcmU8HwDevice.h
// 
//

#ifndef __MMF_BT_ALAW_TO_PCM16_HW_DEVICE_H__
#define __MMF_BT_ALAW_TO_PCM16_HW_DEVICE_H__
#include <mmfbtswcodecwrapper.h>
#include "MMFAudioCodecBase.h"
#include "MMFAudioALawToS16PcmCodec.h"

const TInt KALawToCM16SrcBufferSize  = 0x1000;
const TInt KALawToCM16SinkBufferSize = 0x2000;

/*
*
* CMMFAlawToPcm16CodecHwDevice 
*
*/
class CMMFAlawToPcm16CodecHwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFAlawToPcm16CodecHwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFAlawToPcm16CodecHwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFAlawToPcm16Codec coverts ALaw to PCM
*  
*/
class CMMFAlawToPcm16Codec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KALawToCM16SrcBufferSize;};
	virtual TUint SinkBufferSize() {return KALawToCM16SinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
	TInt AlawSampleToPcm(TUint8 aAlaw);
private:
	TMMFAudioALawToS16PcmCodec iAlawTo16Pcm;
	};

#endif //__MMF_BT_ALAW_TO_PCM16_HW_DEVICE_H__

