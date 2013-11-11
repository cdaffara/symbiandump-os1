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

#ifndef AUDIOBUFFERFILLTESTDEVICE_H
#define AUDIOBUFFERFILLTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFSampleBuffering : public CActive,
							public MMMFSampleBuffering
	{
public:
	//return Pointer to the CMMFSampleBuffering class
	static CMMFSampleBuffering* NewL();

	//Destructor
	~CMMFSampleBuffering();

	// from MMMFSampleBuffering
	TInt MmsbEnableSampleBufferingBeforePlayback();
	TInt MmsbDisableSampleBufferingBeforePlayback();
	void MmsbNotifyPlayStarted(TRequestStatus& aStatus);
	void MmsbCancelNotifyPlayStarted();

	// from CActive
	void RunL();
	void DoCancel();

private:
	CMMFSampleBuffering();
	void ConstructL();

private:
	RTimer iTimer;
	TRequestStatus* iClientStatus;
	};


/*
CSampleBufferingTestDevice

Test hw device used by the
TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CSampleBufferingTestDevice : public CMMFSwCodecWrapper,
								   public MMMFSampleBuffering
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CSampleBufferingTestDevice();
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

	// MMMFSampleBuffering
	TInt MmsbEnableSampleBufferingBeforePlayback();
	TInt MmsbDisableSampleBufferingBeforePlayback();
	void MmsbNotifyPlayStarted(TRequestStatus& aStatus);
	void MmsbCancelNotifyPlayStarted();

private:
	CSampleBufferingTestDevice();
	void ConstructL();

private:
	CMMFSampleBuffering* iSampleBuffering;
	};

#endif 
