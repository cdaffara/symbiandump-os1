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

#ifndef __MMF_BT_PCM16_TO_PCMU16BE_HW_DEVICE_H__
#define __MMF_BT_PCM16_TO_PCMU16BE_HW_DEVICE_H__
#include <mmfbtswcodecwrapper.h>
#include "MmfBtAudioCodec.h"
#include "MMFAudioPcm16ToPcmU16BECodec.h"

const TInt KPCM16ToPCMU16BeSourceBufferSize = 0x1000;
const TInt KPCM16ToPCMU16BeSinkBufferSize   = KPCM16ToPCMU16BeSourceBufferSize; 

/*
*
* CMMFPcm16ToPcmU16BEHwDevice
*
*/      
class CMMFPcm16ToPcmU16BEHwDevice: public CMMFSwCodecWrapper
	{
public:
	static  CMMFPcm16ToPcmU16BEHwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMMFPcm16ToPcmU16BEHwDevice(); 
private:
	void ConstructL();
	};

/*
*
* class CMMFPcm16ToPcmU16BECodec comverts PCM16 to pcm16 unsigned big endian
*
*/
class CMMFPcm16ToPcmU16BECodec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPCM16ToPCMU16BeSourceBufferSize;};
	virtual TUint SinkBufferSize() {return KPCM16ToPCMU16BeSinkBufferSize;};
private:
	TBool CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer );
private:
	TMMFAudioPcm16ToPcmU16BECodec iAudioPcm16ToPcmU16Be ;
	};

#endif //__MMF_BT_PCM16_TO_PCMU16BE_HW_DEVICE_H__

