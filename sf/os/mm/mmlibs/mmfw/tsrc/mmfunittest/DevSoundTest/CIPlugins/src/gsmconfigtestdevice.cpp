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
#include "gsmconfigtestdevice.h"
#include "devsoundciutestdevices.hrh"


/*
CMMFGsmConfig implementation
*/
CMMFGsmConfig* CMMFGsmConfig::NewL()
	{
	CMMFGsmConfig* self = new(ELeave) CMMFGsmConfig();
	return self;
	}

CMMFGsmConfig::~CMMFGsmConfig()
	{
	}

// Actual implementation of method
TInt CMMFGsmConfig::SetConversionFormat(TMMFGsmConversionFormat aConvFormat)
	{
	iConvFormat = aConvFormat;
	return KErrNone;
	}

// Actual implementation of method
TInt CMMFGsmConfig::ConversionFormat(TMMFGsmConversionFormat& aConvFormat) const
	{
	aConvFormat = iConvFormat;
	return KErrNone;
	}


/*
CGsmConfigTestDevice Implementation
*/
CMMFHwDevice* CGsmConfigTestDevice::NewL()
	{
	CGsmConfigTestDevice* self=new(ELeave) CGsmConfigTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CGsmConfigTestDevice::~CGsmConfigTestDevice()
	{
	delete iGsmConfig;
	}

CGsmConfigTestDevice::CGsmConfigTestDevice()
	{
	}

void CGsmConfigTestDevice::ConstructL()
	{
	}

TInt CGsmConfigTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CGsmConfigTestDevice::Stop()
	{
	return 0;
	}

TInt CGsmConfigTestDevice::Pause()
	{
	return 0;
	}

TInt CGsmConfigTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CGsmConfigTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidGsmConfig)
		{
		if (!iGsmConfig)
			{
			TRAPD(err, iGsmConfig = CMMFGsmConfig::NewL());
			if (err == KErrNone && iGsmConfig)
				{
				MMMFGsmConfig* ptr = this;
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

TInt CGsmConfigTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CGsmConfigTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CGsmConfigTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CGsmConfigTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CGsmConfigTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CGsmConfigTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CGsmConfigTestDevice::SetConversionFormat(TMMFGsmConversionFormat aConvFormat)
	{
	TInt result = KErrBadHandle;

	if (iGsmConfig)
		{
		result = iGsmConfig->SetConversionFormat(aConvFormat);
		}

	return result;
	}

TInt CGsmConfigTestDevice::ConversionFormat(TMMFGsmConversionFormat& aConvFormat) const
	{
	TInt result = KErrBadHandle;

	if (iGsmConfig)
		{
		result = iGsmConfig->ConversionFormat(aConvFormat);
		}

	return result;
	}
