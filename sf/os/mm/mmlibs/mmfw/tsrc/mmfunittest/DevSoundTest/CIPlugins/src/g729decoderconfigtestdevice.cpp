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
#include "g729decoderconfigtestdevice.h"
#include "devsoundciutestdevices.hrh"


/*
CMMFG729DecoderConfig implementation
*/
CMMFG729DecoderConfig* CMMFG729DecoderConfig::NewL()
	{
	CMMFG729DecoderConfig* self = new(ELeave) CMMFG729DecoderConfig();
	return self;
	}

CMMFG729DecoderConfig::~CMMFG729DecoderConfig()
	{
	}

//Actual implementation of method BadLsfNextBuffer
TInt CMMFG729DecoderConfig::BadLsfNextBuffer()
	{
	return KErrNone;
	}


/*
CG729DecoderConfigTestDevice implementation
*/
CMMFHwDevice* CG729DecoderConfigTestDevice::NewL()
	{
	CG729DecoderConfigTestDevice* self=new(ELeave) CG729DecoderConfigTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CG729DecoderConfigTestDevice::~CG729DecoderConfigTestDevice()
	{
	delete iG729DecoderConfig;
	}

CG729DecoderConfigTestDevice::CG729DecoderConfigTestDevice()
	{
	}

void CG729DecoderConfigTestDevice::ConstructL()
	{
	}

TInt CG729DecoderConfigTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CG729DecoderConfigTestDevice::Stop()
	{
	return 0;
	}

TInt CG729DecoderConfigTestDevice::Pause()
	{
	return 0;
	}

TInt CG729DecoderConfigTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CG729DecoderConfigTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidG729DecoderIntfc)
		{
		if (!iG729DecoderConfig)
			{
			TRAPD(err, iG729DecoderConfig = CMMFG729DecoderConfig::NewL());
			if (err == KErrNone && iG729DecoderConfig)
				{
				MG729DecoderIntfc* ptr = this;
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

TInt CG729DecoderConfigTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CG729DecoderConfigTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CG729DecoderConfigTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CG729DecoderConfigTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CG729DecoderConfigTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CG729DecoderConfigTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CG729DecoderConfigTestDevice::BadLsfNextBuffer()
	{
	TInt result = KErrBadHandle;

	if (iG729DecoderConfig)
		{
		result = iG729DecoderConfig->BadLsfNextBuffer();
		}

	return result;
	}
