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

#ifndef G711ENCODERCONFIGTESTDEVICE_H
#define G711ENCODERCONFIGTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFG711EncoderConfig : public CBase,
							  public MG711EncoderIntfc
	{
public:
	//return Pointer to the CMMFG711EncoderConfig class
	static CMMFG711EncoderConfig* NewL();

	//Destructor
	~CMMFG711EncoderConfig();

	// from MG711EncoderIntfc
	TInt SetEncoderMode(TEncodeMode aEncodeMode);
	TInt SetVadMode(TBool aVadModeOn);
	TInt GetVadMode(TBool& aVadModeOn);

private:
	TBool iVadModeOn;
	};


/*
CG711EncoderConfigTestDevice

Test hw device used by the
TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CG711EncoderConfigTestDevice : public CMMFSwCodecWrapper,
									 public MG711EncoderIntfc
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CG711EncoderConfigTestDevice();
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

	// MG711EncoderIntfc
	TInt SetEncoderMode(TEncodeMode aEncodeMode);
	TInt SetVadMode(TBool aVadModeOn);
	TInt GetVadMode(TBool& aVadModeOn);

private:
	CG711EncoderConfigTestDevice();
	void ConstructL();

private:
	CMMFG711EncoderConfig* iG711EncoderConfig;
	};

#endif 
