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

#ifndef __MMF_BT_PCMS16_TO_PCMU16_HW_DEVICE_H__
#define __MMF_BT_PCMS16_TO_PCMU16_HW_DEVICE_H__

#include <mmfbtswcodecwrapper.h>
#include "MmfBtAudioCodec.h"
#include "MMFAudioPcm16ToPcmU16Codec.h"

const TInt KPCM16ToPCMU16SourceBufferSize = 0x1000;
const TInt KPCM16ToPCMU16SinkBufferSize   = KPCM16ToPCMU16SourceBufferSize; 

/*
*
* class CMMFPcm16ToPcmU16HwDevice
*
*/
class CMMFPcm16ToPcmU16HwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcm16ToPcmU16HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFPcm16ToPcmU16HwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcm16ToPcmU16CodecCodec converts Pcm16 to Pcm16 unsigned
*
*/
class CMMFPcm16ToPcmU16CodecCodec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCM16ToPCMU16SourceBufferSize;};
	virtual TUint SinkBufferSize() {return KPCM16ToPCMU16SinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioPcm16ToPcmU16Codec iAudioPcm16ToPcmU16;
	};

#endif //__MMF_BT_PCMS16_TO_PCMU16_HW_DEVICE_H__

