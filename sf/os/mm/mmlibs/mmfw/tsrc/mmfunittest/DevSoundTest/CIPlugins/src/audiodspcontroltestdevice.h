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

#ifndef AUDIODSPCONTROLTESTDEVICE_H
#define AUDIODSPCONTROLTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFAudioDspControl : public CBase,
							public MMMFDSPControl
	{
public:
	//return Pointer to the CMMFAudioDspControl class
	static CMMFAudioDspControl* NewL();

	//Destructor
	~CMMFAudioDspControl();

	//from MMMFDSPControl
	TInt MmdspcGetAudioPlaybackInfo (TInt64& aSamplesPlayed, TInt64& aSystemTime, TUint& aB, TUint& aT);
	TInt MmdspcAcceptRecordBuffersInvalidFollowingStop();
	TInt MmdspcAcceptPlaybackBuffersInvalidFollowingStop();
	};


/*
CAudioDspControlTestDevice

Test hw device used by the
TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CAudioDspControlTestDevice : public CMMFSwCodecWrapper,
								   public MMMFDSPControl
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CAudioDspControlTestDevice();
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

	// MMMFDSPControl
	TInt MmdspcGetAudioPlaybackInfo (TInt64& aSamplesPlayed, TInt64& aSystemTime, TUint& aB, TUint& aT);
	TInt MmdspcAcceptRecordBuffersInvalidFollowingStop();
	TInt MmdspcAcceptPlaybackBuffersInvalidFollowingStop();

private:
	CAudioDspControlTestDevice();
	void ConstructL();

private:
	CMMFAudioDspControl* iAudioDspControl;
	};

#endif 
