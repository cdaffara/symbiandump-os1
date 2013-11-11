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

#ifndef __MMF_MULAW_TO_PCM16_HW_DEVICE_H__
#define __MMF_MULAW_TO_PCM16_HW_DEVICE_H__

#include <mmf/server/mmfswcodecwrapper.h>
#include "MMFAudioCodecBase.h"

#include "MMFAudioMuLawToS16PcmCodec.h"

const TInt KMuLawToCM16SrcBufferSize  = 0x1000;
const TInt KMuLawToCM16SinkBufferSize = 0x2000;

/*
*
* class CMMFMulawToPcm16CodecHwDevice 
*
*/
class CMMFMulawToPcm16CodecHwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFMulawToPcm16CodecHwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFMulawToPcm16CodecHwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFMulawToPcm16Codec coverts MuLaw to PCM
*  
*/
class CMMFMulawToPcm16Codec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KMuLawToCM16SrcBufferSize;};
	virtual TUint SinkBufferSize() {return KMuLawToCM16SinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
	TInt MulawSampleToPcm(TUint8 aAlaw);
private:
	TMMFAudioMuLawToS16PcmCodec iMulawTo16Pcm;
	};

#endif //__MMF_MULAW_TO_PCM16_HW_DEVICE_H__
