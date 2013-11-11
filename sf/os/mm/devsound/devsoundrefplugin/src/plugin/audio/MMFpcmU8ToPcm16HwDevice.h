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

#ifndef __MMF_PCMU8_TO_PCM16_HW_DEVICE_H__
#define __MMF_PCMU8_TO_PCM16_HW_DEVICE_H__

#include <mmf/server/mmfswcodecwrapper.h>
#include "MMFAudioCodec.h"
#include "MMFAudioU8ToS16PcmCodec.h"

const TInt KPCMU8ToPCM16SourceBufferSize = 0x0800;
const TInt KPCMU8ToPCM16SinkBufferSize   = 0x1000; //sink buffer length is twice source

/*
* 
* class CMMFPcmU8ToPcm16HwDevice
*
*/
class CMMFPcmU8ToPcm16HwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcmU8ToPcm16HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
    ~CMMFPcmU8ToPcm16HwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcmU8ToPcm16Codec converts Pcm 8 unsigned to Pcm16
*
*/
class CMMFPcmU8ToPcm16Codec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCMU8ToPCM16SourceBufferSize;};
	virtual TUint SinkBufferSize() {return KPCMU8ToPCM16SinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioU8ToS16PcmCodec iAudioU8ToS16Pcm ;
	};

#endif //__MMF_PCMU8_TO_PCM16_HW_DEVICE_H__

