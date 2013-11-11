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

#ifndef __MMF_PCMS16_TO_PCMS8_HW_DEVICE_H__
#define __MMF_PCMS16_TO_PCMS8_HW_DEVICE_H__

#include <mmf/server/mmfswcodecwrapper.h>
#include "MMFAudioCodec.h"
#include "MMFAudioPcmS16ToPcmS8Codec.h"

const TInt KPCM16ToPCM8SourceBufferSize = 0x1000;
const TInt KPCM16ToPCM8SinkBufferSize   = 0x800; 

/*
*
* class CMMFPcmS16ToPcmS8HwDevice
*
*/
class CMMFPcmS16ToPcmS8HwDevice: public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcmS16ToPcmS8HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFPcmS16ToPcmS8HwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcmS16ToPcmS8Codec converts Pcms16 to Pcm 8 signed
*
*/
class CMMFPcmS16ToPcmS8Codec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCM16ToPCM8SourceBufferSize;};
	virtual TUint SinkBufferSize() {return KPCM16ToPCM8SinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioPcmS16ToPcmS8Codec iAudioPcmS16ToPcmS8;
	};

#endif //__MMF_PCMS16_TO_PCMS8_HW_DEVICE_H__

