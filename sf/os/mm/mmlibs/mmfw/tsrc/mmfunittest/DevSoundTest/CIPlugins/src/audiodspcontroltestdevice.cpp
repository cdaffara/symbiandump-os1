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

#include <ecom/implementationproxy.h>
#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <f32file.h>
#include "audiodspcontroltestdevice.h"
#include "devsoundciutestdevices.hrh"


/*
CMMFAudioDspControl implementation
*/
CMMFAudioDspControl* CMMFAudioDspControl::NewL()
	{
	CMMFAudioDspControl* self = new(ELeave) CMMFAudioDspControl();
	return self;
	}

CMMFAudioDspControl::~CMMFAudioDspControl()
	{
	}

//Actual implementation of method MmdspcGetAudioPlaybackInfo
TInt CMMFAudioDspControl::MmdspcGetAudioPlaybackInfo (TInt64& aSamplesPlayed, TInt64& aSystemTime, TUint& aB, TUint& aT)
	{
	aSamplesPlayed = 999;
	aSystemTime = 999;
	aB = 999;
	aT = 999;

	return KErrNone;
	}

//Actual implementation of method MmdspcAcceptRecordBuffersInvalidFollowingStop
TInt CMMFAudioDspControl::MmdspcAcceptRecordBuffersInvalidFollowingStop()
	{
	return KErrNone;
	}

//Actual implementation of method MmdspcAcceptPlaybackBuffersInvalidFollowingStop
TInt CMMFAudioDspControl::MmdspcAcceptPlaybackBuffersInvalidFollowingStop()
	{
	return KErrNone;
	}


/*
implementation
*/
CMMFHwDevice* CAudioDspControlTestDevice::NewL()
	{
	CAudioDspControlTestDevice* self=new(ELeave) CAudioDspControlTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAudioDspControlTestDevice::~CAudioDspControlTestDevice()
	{
	delete iAudioDspControl;
	}

CAudioDspControlTestDevice::CAudioDspControlTestDevice()
	{
	}

void CAudioDspControlTestDevice::ConstructL()
	{
	}

TInt CAudioDspControlTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CAudioDspControlTestDevice::Stop()
	{
	return 0;
	}

TInt CAudioDspControlTestDevice::Pause()
	{
	return 0;
	}

TInt CAudioDspControlTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CAudioDspControlTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidDSPControl)
		{
		if (!iAudioDspControl)
			{
			TRAPD(err, iAudioDspControl = CMMFAudioDspControl::NewL());
			if (err == KErrNone && iAudioDspControl)
				{
				MMMFDSPControl* ptr = this;
				ret = static_cast<TAny*>(ptr);
				}
			else
				{
				ret = NULL;
				}
			}
		}

	return ret;
	}

TInt CAudioDspControlTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CAudioDspControlTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CAudioDspControlTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CAudioDspControlTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CAudioDspControlTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CAudioDspControlTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CAudioDspControlTestDevice::MmdspcGetAudioPlaybackInfo (TInt64& aSamplesPlayed, TInt64& aSystemTime, TUint& aB, TUint& aT)
	{
	TInt result = KErrBadHandle;

	if (iAudioDspControl)
		{
		result = iAudioDspControl->MmdspcGetAudioPlaybackInfo (aSamplesPlayed, aSystemTime, aB, aT);
		}

	return result;
	}

TInt CAudioDspControlTestDevice::MmdspcAcceptRecordBuffersInvalidFollowingStop()
	{
	TInt result = KErrBadHandle;

	if (iAudioDspControl)
		{
		result = iAudioDspControl->MmdspcAcceptRecordBuffersInvalidFollowingStop();
		}

	return result;
	}

TInt CAudioDspControlTestDevice::MmdspcAcceptPlaybackBuffersInvalidFollowingStop()
	{
	TInt result = KErrBadHandle;

	if (iAudioDspControl)
		{
		result = iAudioDspControl->MmdspcAcceptPlaybackBuffersInvalidFollowingStop();
		}

	return result;
	}
