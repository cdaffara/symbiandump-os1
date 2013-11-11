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

#ifndef SPEECHENCODERCONFIGTESTDEVICE_H
#define SPEECHENCODERCONFIGTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFSpeechEncoderConfig : public CBase,
								public MSpeechEncoderConfig
	{
public:
	//return Pointer to the CMMFSpeechEncoderConfig class
	static CMMFSpeechEncoderConfig* NewL();

	//Destructor
	~CMMFSpeechEncoderConfig();

	// from mirror MSpeechEncoderConfig method.
	TInt SetBitrate (TUint aBitrate);
	TInt GetBitrate (TUint& aBitrate);
	TInt GetSupportedBitrates (RArray<TUint>& aSupportedBitrates );
	TInt SetVadMode (TBool aVadModeOn );
	TInt GetVadMode (TBool& aVadModeOn );

private:
	TUint iBitrate;
	TBool iVadModeOn;
	};


/*
CSpeechEncoderConfigTestDevice

Test hw device used by the
TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CSpeechEncoderConfigTestDevice : public CMMFSwCodecWrapper,
									   public MSpeechEncoderConfig
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CSpeechEncoderConfigTestDevice();
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

	// from mirror MSpeechEncoderConfig method.
	TInt SetBitrate ( TUint aBitrate );
	TInt GetBitrate (TUint& aBitrate );
	TInt GetSupportedBitrates (RArray<TUint>& aSupportedBitrates );
	TInt SetVadMode (TBool aVadModeOn );
	TInt GetVadMode (TBool& aVadModeOn );

private:
	CSpeechEncoderConfigTestDevice();
	void ConstructL();

private:
	CMMFSpeechEncoderConfig* iSpeechEncoderConfig;
	};

#endif 
