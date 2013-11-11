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

#ifndef __MMF_PCM16_TO_MULAW_HW_DEVICE_H__
#define __MMF_PCM16_TO_MULAW_HW_DEVICE_H__

#include <mmf/server/mmfswcodecwrapper.h>
#include "MMFAudioCodec.h"
#include "MMFAudioSPcm16ToMuLawCodec.h"
 
const TInt KPCM16ToMuLawSrcBufferSize   = 0x2000;
const TInt KPCM16ToMuLawSinkBufferSize  = 0x1000;

/*
*
* CMMFPcm16ToMulawHwDevice 
*
*/
class CMMFPcm16ToMulawHwDevice : public CMMFSwCodecWrapper
	{	
public:
	static  CMMFPcm16ToMulawHwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFPcm16ToMulawHwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcm16ToALawCodec coverts PCM16 to MuLaw
*  
*/
class CMMFPcm16ToMuLawCodec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCM16ToMuLawSrcBufferSize;};
	virtual TUint SinkBufferSize() {return KPCM16ToMuLawSinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioSPcm16ToMuLawCodec  iPcm16ToMuLaw;
	};

#endif  // __MMF_PCM16_TO_MULAW_HW_DEVICE_H__
