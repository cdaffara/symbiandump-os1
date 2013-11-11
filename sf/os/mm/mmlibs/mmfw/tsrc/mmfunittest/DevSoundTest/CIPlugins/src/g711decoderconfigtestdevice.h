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

#ifndef G711DECODERCONFIGTESTDEVICE_H
#define G711DECODERCONFIGTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFG711DecoderConfig : public CBase,
							  public MG711DecoderIntfc
	{
public:
	//return Pointer to the CMMFG711DecoderConfig class
	static CMMFG711DecoderConfig* NewL();

	//Destructor
	~CMMFG711DecoderConfig();

	// from MG711DecoderConfig
	TInt SetDecoderMode(TDecodeMode aDecodeMode);
	TInt GetDecoderMode(TDecodeMode& aDecodeMode);
	TInt SetComfortNoiseGeneration(TBool aCng);
	TInt GetComfortNoiseGeneration(TBool& aCng);
	TInt SetPacketLossConcealment(TBool aPlc);
	TInt GetPacketLossConcealment(TBool& aPlc);

private:
	TDecodeMode iDecodeMode;
	TBool iCng;
	TBool iPlc;
	};


/*
CAacDecoderConfigTestDevice

Test hw device used by the
TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CG711DecoderConfigTestDevice : public CMMFSwCodecWrapper,
									 public MG711DecoderIntfc
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CG711DecoderConfigTestDevice();
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

	// MG711DecoderIntfc
	TInt SetDecoderMode(TDecodeMode aDecodeMode);
	TInt GetDecoderMode(TDecodeMode& aDecodeMode);
	TInt SetComfortNoiseGeneration(TBool aCng);
	TInt GetComfortNoiseGeneration(TBool& aCng);
	TInt SetPacketLossConcealment(TBool aPlc);
	TInt GetPacketLossConcealment(TBool& aPlc);

private:
	CG711DecoderConfigTestDevice();
	void ConstructL();

private:
	CMMFG711DecoderConfig* iG711DecoderConfig;
	};

#endif 
