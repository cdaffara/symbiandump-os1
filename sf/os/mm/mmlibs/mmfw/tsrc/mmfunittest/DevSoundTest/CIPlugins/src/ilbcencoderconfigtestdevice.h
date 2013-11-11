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

#ifndef ILBCENCODERCONFIGTESTDEVICE_H
#define ILBCENCODERCONFIGTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFIlbcEncoderConfig : public CBase,
							  public MIlbcEncoderIntfc
	{
public:
	//return Pointer to the CMMFIlbcEncoderConfig class
	static CMMFIlbcEncoderConfig* NewL();

	//Destructor
	~CMMFIlbcEncoderConfig();

	// from MIlbcEncoderConfig
	TInt SetEncoderMode(TEncodeMode aEncodeMode);
	TInt GetEncoderMode(TEncodeMode& aEncodeMode);
	TInt SetVadMode(TBool aVadModeOn);
	TInt GetVadMode(TBool& aVadModeOn);

private:
	TEncodeMode iEncodeMode;
	TBool iVadModeOn;
	};


/*
CIlbcEncoderConfigTestDevice

Test hw device used by the
TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CIlbcEncoderConfigTestDevice : public CMMFSwCodecWrapper,
									 public MIlbcEncoderIntfc
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CIlbcEncoderConfigTestDevice();
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

	// MIlbcEncoderIntfc
	TInt SetEncoderMode(TEncodeMode aEncodeMode);
	TInt GetEncoderMode(TEncodeMode& aEncodeMode);
	TInt SetVadMode(TBool aVadModeOn);
	TInt GetVadMode(TBool& aVadModeOn);

private:
	CIlbcEncoderConfigTestDevice();
	void ConstructL();

private:
	CMMFIlbcEncoderConfig* iIlbcEncoderConfig;
	};

#endif 
