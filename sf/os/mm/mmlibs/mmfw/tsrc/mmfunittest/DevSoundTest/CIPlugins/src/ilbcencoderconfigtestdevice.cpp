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
#include "ilbcencoderconfigtestdevice.h"
#include "devsoundciutestdevices.hrh"

/*
CMMFIlbcEncoderConfig implementation
*/
CMMFIlbcEncoderConfig* CMMFIlbcEncoderConfig::NewL()
	{
	CMMFIlbcEncoderConfig* self = new(ELeave) CMMFIlbcEncoderConfig();
	return self;
	}

CMMFIlbcEncoderConfig::~CMMFIlbcEncoderConfig()
	{
	}

//Actual implementation of method SetEncoderMode
TInt CMMFIlbcEncoderConfig::SetEncoderMode(TEncodeMode aEncodeMode)
	{
	iEncodeMode = aEncodeMode;

	return KErrNone;
	}

//Actual implementation of method GetEncoderMode
TInt CMMFIlbcEncoderConfig::GetEncoderMode(TEncodeMode& aEncodeMode)
	{
	aEncodeMode = iEncodeMode;

	return KErrNone;
	}

//Actual implementation of method SetVadMode
TInt CMMFIlbcEncoderConfig::SetVadMode(TBool aVadModeOn)
	{
	iVadModeOn = aVadModeOn;
	return KErrNone;
	}

//Actual implementation of method GetVadMode
TInt CMMFIlbcEncoderConfig::GetVadMode(TBool& aVadModeOn)
	{
	aVadModeOn = iVadModeOn;
	return KErrNone;
	}


/*
CIlbcEncoderConfigTestDevice implementation
*/
CMMFHwDevice* CIlbcEncoderConfigTestDevice::NewL()
	{
	CIlbcEncoderConfigTestDevice* self=new(ELeave) CIlbcEncoderConfigTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CIlbcEncoderConfigTestDevice::~CIlbcEncoderConfigTestDevice()
	{
	delete iIlbcEncoderConfig;
	}

CIlbcEncoderConfigTestDevice::CIlbcEncoderConfigTestDevice()
	{
	}

void CIlbcEncoderConfigTestDevice::ConstructL()
	{
	}

TInt CIlbcEncoderConfigTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CIlbcEncoderConfigTestDevice::Stop()
	{
	return 0;
	}

TInt CIlbcEncoderConfigTestDevice::Pause()
	{
	return 0;
	}

TInt CIlbcEncoderConfigTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CIlbcEncoderConfigTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidIlbcEncoderIntfc)
		{
		if (!iIlbcEncoderConfig)
			{
			TRAPD(err, iIlbcEncoderConfig = CMMFIlbcEncoderConfig::NewL());
			if (err == KErrNone && iIlbcEncoderConfig)
				{
				MIlbcEncoderIntfc* ptr = this;
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

TInt CIlbcEncoderConfigTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CIlbcEncoderConfigTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CIlbcEncoderConfigTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CIlbcEncoderConfigTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CIlbcEncoderConfigTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CIlbcEncoderConfigTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CIlbcEncoderConfigTestDevice::SetEncoderMode(TEncodeMode aEncodeMode)
	{
	TInt result = KErrBadHandle;

	if (iIlbcEncoderConfig)
		{
		result = iIlbcEncoderConfig->SetEncoderMode(aEncodeMode);
		}

	return result;
	}

TInt CIlbcEncoderConfigTestDevice::GetEncoderMode(TEncodeMode& aEncodeMode)
	{
	TInt result = KErrBadHandle;

	if (iIlbcEncoderConfig)
		{
		result = iIlbcEncoderConfig->GetEncoderMode(aEncodeMode);
		}

	return result;
	}

TInt CIlbcEncoderConfigTestDevice::SetVadMode(TBool aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iIlbcEncoderConfig)
		{
		result = iIlbcEncoderConfig->SetVadMode(aVadModeOn);
		}

	return result;
	}

TInt CIlbcEncoderConfigTestDevice::GetVadMode(TBool& aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iIlbcEncoderConfig)
		{
		result = iIlbcEncoderConfig->GetVadMode(aVadModeOn);
		}

	return result;
	}
