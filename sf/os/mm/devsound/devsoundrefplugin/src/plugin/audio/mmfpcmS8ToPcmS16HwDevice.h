/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MMF_PCMS8_TO_PCMS16_HW_DEVICE_H__
#define __MMF_PCMS8_TO_PCMS16_HW_DEVICE_H__

#include <mmf/server/mmfswcodecwrapper.h>
#include "MMFAudioCodec.h"
#include "MMFAudioPcm8ToPcm16Codec.h"

const TInt KPCM8ToPCM16SourceBufferSize = 0x0800;
const TInt KPCM8ToPCM16SinkBufferSize = 0x1000; //sink buffer length is twice source

/*
*
* class CMMFPcm8ToPcm16HwDevice
*
*/
class CMMFPcm8ToPcm16HwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcm8ToPcm16HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFPcm8ToPcm16HwDevice();
private:
	void ConstructL();
	};

/*
*
* class CMMFPcm8ToPcm16Codec converts pcm8 to pcm16
*
*/
class CMMFPcm8ToPcm16Codec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCM8ToPCM16SourceBufferSize;};
	virtual TUint SinkBufferSize() {return KPCM8ToPCM16SinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioPcm8ToPcm16Codec iAudioPcm8ToPcm16 ;
	};

#endif //__MMF_PCMS8_TO_PCMS16_HW_DEVICE_H__

