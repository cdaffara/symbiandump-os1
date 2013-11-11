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

#ifndef __MMF_BT_PCMU16_TO_PCM16_HW_DEVICE_H__
#define __MMF_BT_PCMU16_TO_PCM16_HW_DEVICE_H__

#include <mmfbtswcodecwrapper.h>
#include "MmfBtAudioCodec.h"
#include "MMFAudioU16ToS16PcmCodec.h"


const TInt KPCMU16ToPCM16SourceBufferSize = 0x1000;
const TInt KPCMU16ToPCM16SinkBufferSize = KPCMU16ToPCM16SourceBufferSize; 

/*
*
* class CMMFPcmU16ToPcm16HwDevice
*
*/
class CMMFPcmU16ToPcm16HwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcmU16ToPcm16HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
    ~CMMFPcmU16ToPcm16HwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcmU16ToPcm16Codec converts Pcm16 Unsigned to Pcm16
*
*/
class CMMFPcmU16ToPcm16Codec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCMU16ToPCM16SourceBufferSize;};
	virtual TUint SinkBufferSize() {return KPCMU16ToPCM16SourceBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioU16ToS16PcmCodec iAudioU16ToS16Pcm ;
	};

#endif //__MMF_BT_PCMU16_TO_PCM16_HW_DEVICE_H__

