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


#ifndef __MMF_SWAP_ENDIAN_HW_DEVICE_H__
#define __MMF_SWAP_ENDIAN_HW_DEVICE_H__
#include <mmf/server/mmfswcodecwrapper.h>
#include "MMFAudioCodec.h"
#include "MMFAudioPcm16SwapEndianCodecCommon.h"

const TInt KPCM16ToPCM16BBufferSize = 0x2000;

/*
*
* class CMMFPcm16SwapEndianHwDevice 
*
*/
class CMMFPcm16SwapEndianHwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcm16SwapEndianHwDevice* NewL();
	virtual CMMFSwCodec& Codec();
    ~CMMFPcm16SwapEndianHwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcm16SwapEndianCodec coverts PCM16 to PCM16Big Endian
*  
*/
class CMMFPcm16SwapEndianCodec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCM16ToPCM16BBufferSize;};
	virtual TUint SinkBufferSize() {return KPCM16ToPCM16BBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioPcm16SwapEndianCodec iPcm16EndianSwap;
	};

#endif //__MMF_SWAP_ENDIAN_HW_DEVICE_H__
