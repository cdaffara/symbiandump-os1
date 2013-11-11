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

#ifndef G729DECODERCONFIGTESTDEVICE_H
#define G729DECODERCONFIGTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFG729DecoderConfig : public CBase,
							  public MG729DecoderIntfc
	{
public:
	//return Pointer to the CMMFG729DecoderConfig class
	static CMMFG729DecoderConfig* NewL();

	//Destructor
	~CMMFG729DecoderConfig();

	// from MG729DecoderIntfc
	TInt BadLsfNextBuffer();
	};


/*
CG729DecoderConfigTestDevice

Test hw device used by the
TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CG729DecoderConfigTestDevice : public CMMFSwCodecWrapper,
									 public MG729DecoderIntfc
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CG729DecoderConfigTestDevice();
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

	// MG729DecoderIntfc
	TInt BadLsfNextBuffer();

private:
	CG729DecoderConfigTestDevice();
	void ConstructL();

private:
	CMMFG729DecoderConfig* iG729DecoderConfig;
	};

#endif 
