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

#ifndef G729ENCODERCONFIGTESTDEVICE_H
#define G729ENCODERCONFIGTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFG729EncoderConfig : public CBase,
							  public MG729EncoderIntfc
	{
public:
	//return Pointer to the CMMFG729EncoderConfig class
	static CMMFG729EncoderConfig* NewL();

	//Destructor
	~CMMFG729EncoderConfig();

	// from MG729EncoderIntfc
	TInt SetVadMode(TBool aVadModeOn);
	TInt GetVadMode(TBool& aVadModeOn);
	};


/*
CG729EncoderConfigTestDevice

Test hw device used by the
TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CG729EncoderConfigTestDevice : public CMMFSwCodecWrapper,
									 public MG729EncoderIntfc
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CG729EncoderConfigTestDevice();
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

	// MG729EncoderIntfc
	TInt SetVadMode(TBool aVadModeOn);
	TInt GetVadMode(TBool& aVadModeOn);

private:
	CG729EncoderConfigTestDevice();
	void ConstructL();

private:
	CMMFG729EncoderConfig* iG729EncoderConfig;
	};

#endif 
