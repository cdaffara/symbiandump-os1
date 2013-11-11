// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_SWCDWRAP_ASYNCTIMER_H__
#define __TSU_MMF_SWCDWRAP_ASYNCTIMER_H__

#include <mmf/server/mmfhwdevice.h>

/**
 * Class used by the test harness to make asynchronous 
 * calls Pause() and Stop() on a hw device plugin
 * from the test harness
 */
class CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall : public CTimer
	{
public:
	static CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* NewL();
	void CallPauseAfter(CMMFHwDevice& aHwDevice, TTimeIntervalMicroSeconds32 aTimeInterval);
	void CallStopAfter(CMMFHwDevice& aHwDevice, TTimeIntervalMicroSeconds32 aTimeInterval);
	void CallStopAndDeleteCodecAfter(CMMFHwDevice& aHwDevice, TTimeIntervalMicroSeconds32 aTimeInterval);
	void CallStopPauseAfter(CMMFHwDevice& aHwDevice, TTimeIntervalMicroSeconds32 aTimeInterval);
	TInt PauseError();
	TInt StopError();
	TInt StopAndDeleteCodecError();
	void CancelCallAction();
	void StopActiveSchedulerOnCall(TBool aStopActiveSchedulerOnStop = ETrue);
private:
	CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall();
	void ConstructL();
	virtual void RunL();
private:
	CMMFHwDevice* iHwDevice;
	TBool iCallPause;
	TBool iCallStopPause;
	TInt  iPauseError;
	TInt  iStopError;
	TBool iCallStop;
	TBool iCallStopAndDeleteCodec;
	TInt  iStopAndDeleteCodecError;
	TBool iCallActionCancelled;
	TBool iStopActiveSchedulerOnCall;
	};

#endif /* __TSU_MMF_SWCDWRAP_ASYNCTIMER_H__ */
