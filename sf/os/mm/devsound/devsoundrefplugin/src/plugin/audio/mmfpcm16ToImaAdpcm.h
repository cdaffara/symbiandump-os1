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

#ifndef __MMF_PCM16_TO_IMAADPCM_HW_DEVICE_H__
#define __MMF_PCM16_TO_IMAADPCM_HW_DEVICE_H__

#include <mmf/server/mmfswcodecwrapper.h>
#include "MMFAudioCodecBase.h"
#include "MMFAudioCodec.h"
#include "MMFAudioPcm16ToImaAdpcmCodec.h"

const TInt KPCM16ToImaSrcBufferSize  = 0x3F20;
const TInt KPCM16ToImaSinkBufferSize = 0x1000;
const TUint KSourceFrameSize = KImaAdpcmSamplesPerBlock * 2;
const TUint KCodedFrameSize  = KImaAdpcmBlockAlign;

/*
*
* class CMMFPcm16ToMulawHwDevice 
*
*/
class CMMFPcm16ToImaAdpcmHwDevice : public CMMFSwCodecWrapper
	{	
public:
	static  CMMFPcm16ToImaAdpcmHwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFPcm16ToImaAdpcmHwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcm16ToALawCodec coverts Pcm16 to Imaad
*  
*/
class CMMFPcm16ToImaAdpcmCodec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCM16ToImaSrcBufferSize;};
	virtual TUint SinkBufferSize() {return   KPCM16ToImaSinkBufferSize;};
private:
	virtual void ResetL();
	void ProcessBuffers(const CMMFDataBuffer& aSource, CMMFDataBuffer& aDestination, CMMFSwCodec::TCodecProcessResult& aResult );
	TBool BuffersStatus( const CMMFDataBuffer* source, const CMMFDataBuffer* destination );
private:
	TMMFAudioPcm16ToImaAdpcmCodec iPcm16ToImaAdpcm;
	TMMFAudioPcm16ToImaAdpcmCodec i16PcmToImaAdpcm;
	TUint iLastFrameNumber;
	};

#endif //__MMF_PCM16_TO_IMAADPCM_HW_DEVICE_H__
