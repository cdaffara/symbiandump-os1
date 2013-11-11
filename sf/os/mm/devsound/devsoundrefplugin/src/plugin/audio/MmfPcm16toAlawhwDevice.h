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

#ifndef __MMF_PCM16_TO_ALAW_HW_DEVICE_H__
#define __MMF_PCM16_TO_ALAW_HW_DEVICE_H__
#include <mmf/server/mmfswcodecwrapper.h>
#include "MMFAudioCodec.h"
#include "MMFAudioSPcm16ToALawCodec.h"

const TInt KPCM16ToALawSrcBufferSize  = 0x2000;
const TInt KPCM16ToAlawSinkBufferSize = 0x1000;

/*
*
* class CMMFPcm16ToAlawHwDevice 
*
*/
class CMMFPcm16ToAlawHwDevice : public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcm16ToAlawHwDevice* NewL();
	virtual CMMFSwCodec& Codec();
    ~CMMFPcm16ToAlawHwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcm16ToALawCodec coverts PCM to ALaw
*  
*/
class CMMFPcm16ToALawCodec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCM16ToALawSrcBufferSize;};
	virtual TUint SinkBufferSize() {return KPCM16ToAlawSinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioSPcm16ToAlawCodec  iPcm16ToALaw;
	};

#endif //__MMF_PCM16_TO_ALAW_HW_DEVICE_H__
