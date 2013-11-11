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

#ifndef EAACPLUSDECODERTESTDEVICE_H
#define EAACPLUSDECODERTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


/**
*
* CEAacPlusDecoderTestDevice
*
* Test hw device used by the
* TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CEAacPlusDecoderTestDevice : public CMMFSwCodecWrapper, public MEAacPlusDecoderIntfc
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CEAacPlusDecoderTestDevice();
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

	// from MEAacPlusDecoderIntfc
	virtual void SetInputSamplingFrequency(TUint aInputSamplingFrequency);
	virtual void SetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType aAudioObjectType);
	virtual void SetNumOfChannels(TUint aNumOfChannels);
	virtual void SetSbr(TBool aSbrEnabled);
	virtual void SetDownSampledMode(TBool aDsmEnabled);
	virtual TInt ApplyConfig();
	virtual TInt GetInputSamplingFrequency ( TUint& aInputSamplingFrequency );
	virtual TInt GetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType& aAudioObjectType);
	virtual TInt GetNumOfChannels ( TUint& aNumOfChannels );
	virtual TInt GetSbr (TBool& aSbrEnabled);
	virtual TInt GetDownSampledMode (TBool& aDsmEnabled);

	
private:
	CEAacPlusDecoderTestDevice();
	void ConstructL();
private:
	TUint iFrequency;
	MEAacPlusDecoderIntfc::TAudioObjectType iAudioObjectType;
	TUint iNumOfChannels;
	TBool iSbrEnabled;
	TBool iDsmEnabled;
	TBool iApplyConfig;
	};

#endif 
