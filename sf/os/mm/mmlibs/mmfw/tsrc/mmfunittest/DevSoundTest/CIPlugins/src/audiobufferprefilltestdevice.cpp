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
#include <ecom/ecom.h>
#include <f32file.h>
#include "audiobufferprefilltestdevice.h"
#include "devsoundciutestdevices.hrh"

const TInt32 KTenSeconds = 10000000;	// Ten seconds

/*
CMMFSampleBuffering Implementation
*/
CMMFSampleBuffering* CMMFSampleBuffering::NewL()
	{
	CMMFSampleBuffering* self = new(ELeave) CMMFSampleBuffering();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMMFSampleBuffering::CMMFSampleBuffering() : CActive(0)
	{
	CActiveScheduler::Add(this);
	}

void CMMFSampleBuffering::ConstructL()
	{
	User::LeaveIfError( iTimer.CreateLocal() );
	}

CMMFSampleBuffering::~CMMFSampleBuffering()
	{
	iTimer.Close();
	Cancel();
	}

//Actual implementation of method MmsbEnableSampleBufferingBeforePlayback
TInt CMMFSampleBuffering::MmsbEnableSampleBufferingBeforePlayback()
	{
	return KErrNone;
	}

//Actual implementation of method MmsbDisableSampleBufferingBeforePlayback
TInt CMMFSampleBuffering::MmsbDisableSampleBufferingBeforePlayback()
	{
	return KErrNone;
	}

//Actual implementation of method MmsbNotifyPlayStarted
void CMMFSampleBuffering::MmsbNotifyPlayStarted(TRequestStatus& aStatus)
	{
	// check not already active
	ASSERT(!IsActive());

	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;

	iTimer.After(iStatus, KTenSeconds);

	SetActive();
	}

//Actual implementation of method MmsbCancelNotifyPlayStarted
void CMMFSampleBuffering::MmsbCancelNotifyPlayStarted()
	{
	CActive::Cancel();
	}

//Actual implementation of method RunL
void CMMFSampleBuffering::RunL()
	{
	TRequestStatus* status = iClientStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

//Actual implementation of method DoCancel
void CMMFSampleBuffering::DoCancel()
	{
	iTimer.Cancel();
	TRequestStatus* status = iClientStatus;
	User::RequestComplete(status, KErrCancel);
	}


/*
CSampleBufferingTestDevice Implementation
*/
CMMFHwDevice* CSampleBufferingTestDevice::NewL()
	{
	CSampleBufferingTestDevice* self=new(ELeave) CSampleBufferingTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSampleBufferingTestDevice::~CSampleBufferingTestDevice()
	{
	delete iSampleBuffering;
	}

CSampleBufferingTestDevice::CSampleBufferingTestDevice()
	{
	}

void CSampleBufferingTestDevice::ConstructL()
	{
	}

TInt CSampleBufferingTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CSampleBufferingTestDevice::Stop()
	{
	return 0;
	}

TInt CSampleBufferingTestDevice::Pause()
	{
	return 0;
	}

TInt CSampleBufferingTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CSampleBufferingTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidSampleBuffering)
		{
		if (!iSampleBuffering)
			{
			TRAPD(err, iSampleBuffering = CMMFSampleBuffering::NewL());
			if (err == KErrNone && iSampleBuffering)
				{
				MMMFSampleBuffering* ptr = this;
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

TInt CSampleBufferingTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CSampleBufferingTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CSampleBufferingTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CSampleBufferingTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CSampleBufferingTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CSampleBufferingTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CSampleBufferingTestDevice::MmsbEnableSampleBufferingBeforePlayback()
	{
	TInt result = KErrBadHandle;

	if (iSampleBuffering)
		{
		result = iSampleBuffering->MmsbEnableSampleBufferingBeforePlayback();
		}

	return result;
	}

TInt CSampleBufferingTestDevice::MmsbDisableSampleBufferingBeforePlayback()
	{
	TInt result = KErrBadHandle;

	if (iSampleBuffering)
		{
		result = iSampleBuffering->MmsbDisableSampleBufferingBeforePlayback();
		}

	return result;
	}

void CSampleBufferingTestDevice::MmsbNotifyPlayStarted(TRequestStatus& aStatus)
	{
	if (iSampleBuffering)
		{
		iSampleBuffering->MmsbNotifyPlayStarted(aStatus);
		}
	}

void CSampleBufferingTestDevice::MmsbCancelNotifyPlayStarted()
	{
	if (iSampleBuffering)
		{
		iSampleBuffering->MmsbCancelNotifyPlayStarted();
		}
	}
