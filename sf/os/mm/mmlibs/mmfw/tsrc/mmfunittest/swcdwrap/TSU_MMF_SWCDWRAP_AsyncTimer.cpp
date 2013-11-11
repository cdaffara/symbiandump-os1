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

// EPOC includes
#include <e32base.h>
#include "TSU_MMF_SWCDWRAP_AsyncTimer.h"

CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::NewL()
	{
	CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall* self = new(ELeave)CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	
	return self;
	}

void CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	iStopActiveSchedulerOnCall = ETrue;
	}

CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall() : CTimer(EPriorityHigh)
	{
	}

void CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::CallPauseAfter(CMMFHwDevice& aHwDevice, TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	iCallActionCancelled = EFalse;
	iHwDevice = &aHwDevice;
	iCallPause = ETrue;
	iPauseError = KErrNone;
	After(aTimeInterval);	
	}

void CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::CallStopAfter(CMMFHwDevice& aHwDevice, TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	iCallActionCancelled = EFalse;
	iHwDevice = &aHwDevice;
	iCallStop = ETrue;
	iStopError = KErrNone;
	After(aTimeInterval);	
	}

void CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::CallStopPauseAfter(CMMFHwDevice& aHwDevice, TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	iCallActionCancelled = EFalse;
	iHwDevice = &aHwDevice;
	iCallStopPause = ETrue;
	iPauseError = KErrNone;
	iStopError = KErrNone;
	After(aTimeInterval);	
	}

void CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::CallStopAndDeleteCodecAfter(CMMFHwDevice& aHwDevice, TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	iCallActionCancelled = EFalse;
	iHwDevice = &aHwDevice;
	iCallStopAndDeleteCodec = ETrue;
	iStopError = KErrNone;
	After(aTimeInterval);	
	}


void CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::RunL()
	{
	if (!iCallActionCancelled)
		{
		if (iCallStopPause)
			{
			iStopError = iHwDevice->Stop();
			if (iStopError != KErrNone)
				{
				User::Leave(iStopError);
				}
			
			iPauseError = iHwDevice->Pause();
			if (iPauseError != KErrNone)
				{
				User::Leave(iPauseError);
				}
			
			iCallStopPause = EFalse;
			}
		else if (iCallPause)
			{
			iPauseError = iHwDevice->Pause();
			if (iPauseError != KErrNone)
				{
				User::Leave(iPauseError);
				}
			
			iCallPause = EFalse;
			}
		else if (iCallStop)
		   	{
		   	iStopError = iHwDevice->Stop();
		   	if (iStopError != KErrNone)
		   		{
		   		User::Leave(iStopError);
		   		}
		   	iCallStop = EFalse;
		   	}
		else if ( iCallStopAndDeleteCodec)
		   	{
		   	iStopAndDeleteCodecError = iHwDevice->StopAndDeleteCodec();
		   	if (iStopAndDeleteCodecError != KErrNone)
		   		{
		   		User::Leave(iStopAndDeleteCodecError);
		   		}
		   	iCallStopAndDeleteCodec = EFalse;
		   	}
		}

	if (iStopActiveSchedulerOnCall)
		{
		CActiveScheduler::Stop();
		}
	}


TInt CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::PauseError()
	{
	return iPauseError;
	}


TInt CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::StopError()
	{
	return iStopError;
	}


TInt CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::StopAndDeleteCodecError()
	{
	return iStopAndDeleteCodecError;
	}


void CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::CancelCallAction()
	{
	iCallActionCancelled = ETrue;
	}


void CMMF_TSU_SWCDWRAP_MakeAsyncHwDeviceCall::StopActiveSchedulerOnCall(TBool aStopActiveSchedulerOnCall)
	{
	iStopActiveSchedulerOnCall = aStopActiveSchedulerOnCall;
	}






 

