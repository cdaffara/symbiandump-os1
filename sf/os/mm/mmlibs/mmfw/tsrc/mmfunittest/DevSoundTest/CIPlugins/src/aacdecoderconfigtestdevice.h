// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef AACDECODERCONFIGTESTDEVICE_H
#define AACDECODERCONFIGTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFAacDecoderConfig : public CBase,
							 public MAacDecoderConfig
	{
public:
	//return Pointer to the CMMFAacDecoderConfig class
	static CMMFAacDecoderConfig* NewL();

	//Destructor
	~CMMFAacDecoderConfig();

	// from MAacDecoderConfig
	TInt SetAudioConfig(TAudioConfig& aAudioConfig);
	TInt GetSupportedAudioConfigs(RArray<TAudioConfig>& aSupportedAudioConfigs);
	};


/*
CAacDecoderConfigTestDevice

Test hw device used by the
SU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CAacDecoderConfigTestDevice : public CMMFSwCodecWrapper,
									public MAacDecoderConfig
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CAacDecoderConfigTestDevice();
	virtual TInt Start(TDeviceFunc aFuncCmd, TDeviceFlow aFlowCmd);
	virtual TInt Stop();
	virtual TInt Pause();
	virtual TInt Init(THwDeviceInitParams& aDevInfo);
	virtual TAny* CustomInterface(TUid aInterfaceId);
	virtual TInt ThisHwBufferFilled(CMMFBuffer& aFillBufferPtr);
	virtual TInt ThisHwBufferEmptied(CMMFBuffer& aEmptyBufferPtr);
	virtual TInt SetConfig(TTaskConfig& aConfig);
	virtual TInt StopAndDeleteCodec();
	virtual TInt DeleteCodec();

	// from CMMFSwCodecWrapper
	virtual CMMFSwCodec& Codec();

	// MAacDecoderConfig
	TInt SetAudioConfig(TAudioConfig& aAudioConfig);
	TInt GetSupportedAudioConfigs(RArray<MAacDecoderConfig::TAudioConfig>& aSupportedAudioConfigs);

private:
	CAacDecoderConfigTestDevice();
	void ConstructL();

private:
	CMMFAacDecoderConfig* iAacDecoderConfig;
	};

#endif 
