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

#ifndef GSMCONFIGTESTDEVICE_H
#define GSMCONFIGTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFGsmConfig : public CBase,
					  public MMMFGsmConfig
	{
public:
	//return Pointer to the CMMFGsmConfig class
	static CMMFGsmConfig* NewL();

	//Destructor
	~CMMFGsmConfig();

	// from MMMFGsmConfig
	TInt SetConversionFormat(TMMFGsmConversionFormat aConvFormat);
	TInt ConversionFormat(TMMFGsmConversionFormat& aConvFormat) const;

private:
	TMMFGsmConversionFormat iConvFormat;
	};


/*
CGsmConfigTestDevice

Test hw device used by the
TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CGsmConfigTestDevice : public CMMFSwCodecWrapper,
							 public MMMFGsmConfig
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CGsmConfigTestDevice();
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

	// MMMFGsmConfig
	TInt SetConversionFormat(MMMFGsmConfig::TMMFGsmConversionFormat aConvFormat);
	TInt ConversionFormat(MMMFGsmConfig::TMMFGsmConversionFormat& aConvFormat) const;

private:
	CGsmConfigTestDevice();
	void ConstructL();

private:
	CMMFGsmConfig* iGsmConfig;
	};

#endif 
