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
#include "audiovibracontroltestdevice.h"
#include "devsoundciutestdevices.hrh"


/*
CMMFAudioVibraControl implementation
*/
CMMFAudioVibraControl* CMMFAudioVibraControl::NewL()
	{
	CMMFAudioVibraControl* self = new(ELeave) CMMFAudioVibraControl();
	return self;
	}

CMMFAudioVibraControl::~CMMFAudioVibraControl()
	{
	}

//Actual implementation of method StartVibra
TInt CMMFAudioVibraControl::StartVibra()
	{
	return KErrNone;
	}

//Actual implementation of method StopVibra
TInt CMMFAudioVibraControl::StopVibra()
	{
	return KErrNone;
	}


/*
CAudioVibraControlTestDevice implementation
*/
CMMFHwDevice* CAudioVibraControlTestDevice::NewL()
	{
	CAudioVibraControlTestDevice* self=new(ELeave) CAudioVibraControlTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAudioVibraControlTestDevice::~CAudioVibraControlTestDevice()
	{
	delete iAudioVibraControl;
	}

CAudioVibraControlTestDevice::CAudioVibraControlTestDevice()
	{
	}

void CAudioVibraControlTestDevice::ConstructL()
	{
	}

TInt CAudioVibraControlTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CAudioVibraControlTestDevice::Stop()
	{
	return 0;
	}

TInt CAudioVibraControlTestDevice::Pause()
	{
	return 0;
	}

TInt CAudioVibraControlTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CAudioVibraControlTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidAudioVibraControl)
		{
		if (!iAudioVibraControl)
			{
			TRAPD(err, iAudioVibraControl = CMMFAudioVibraControl::NewL());
			if (err == KErrNone && iAudioVibraControl)
				{
				MAudioVibraControl* ptr = this;
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

TInt CAudioVibraControlTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CAudioVibraControlTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CAudioVibraControlTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CAudioVibraControlTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CAudioVibraControlTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CAudioVibraControlTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CAudioVibraControlTestDevice::StartVibra()
	{
	TInt result = KErrBadHandle;

	if (iAudioVibraControl)
		{
		result = iAudioVibraControl->StartVibra();
		}

	return result;
	}

TInt CAudioVibraControlTestDevice::StopVibra()
	{
	TInt result = KErrBadHandle;

	if (iAudioVibraControl)
		{
		result = iAudioVibraControl->StopVibra();
		}

	return result;
	}
