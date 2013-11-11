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
#include "g711decoderconfigtestdevice.h"
#include "devsoundciutestdevices.hrh"


/*
CMMFG711DecoderConfig implementation
*/
CMMFG711DecoderConfig* CMMFG711DecoderConfig::NewL()
	{
	CMMFG711DecoderConfig* self = new(ELeave) CMMFG711DecoderConfig();
	return self;
	}

CMMFG711DecoderConfig::~CMMFG711DecoderConfig()
	{
	}

//Actual implementation of method SetDecoderMode
TInt CMMFG711DecoderConfig::SetDecoderMode(TDecodeMode aDecodeMode)
	{
	iDecodeMode = aDecodeMode;
	return KErrNone;
	}

//Actual implementation of method SetDecoderMode
TInt CMMFG711DecoderConfig::GetDecoderMode(TDecodeMode& aDecodeMode)
	{
	aDecodeMode = iDecodeMode;
	return KErrNone;
	}

//Actual implementation of method SetCng
TInt CMMFG711DecoderConfig::SetComfortNoiseGeneration(TBool aCng)
	{
	iCng = aCng;
	return KErrNone;
	}

//Actual implementation of method GetCng
TInt CMMFG711DecoderConfig::GetComfortNoiseGeneration(TBool& aCng)
	{
	aCng = iCng;
	return KErrNone;
	}

//Actual implementation of method SetPlc
TInt CMMFG711DecoderConfig::SetPacketLossConcealment(TBool aPlc)
	{
	iPlc = aPlc;
	return KErrNone;
	}
//Actual implementation of method SetPlc
TInt CMMFG711DecoderConfig::GetPacketLossConcealment(TBool& aPlc)
	{
	aPlc = iPlc;
	return KErrNone;
	}

/*
CG711DecoderConfigTestDevice implementation
*/
CMMFHwDevice* CG711DecoderConfigTestDevice::NewL()
	{
	CG711DecoderConfigTestDevice* self = new(ELeave) CG711DecoderConfigTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CG711DecoderConfigTestDevice::~CG711DecoderConfigTestDevice()
	{
	delete iG711DecoderConfig;
	}

CG711DecoderConfigTestDevice::CG711DecoderConfigTestDevice()
	{
	}

void CG711DecoderConfigTestDevice::ConstructL()
	{
	}

TInt CG711DecoderConfigTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CG711DecoderConfigTestDevice::Stop()
	{
	return 0;
	}

TInt CG711DecoderConfigTestDevice::Pause()
	{
	return 0;
	}

TInt CG711DecoderConfigTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CG711DecoderConfigTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidG711DecoderIntfc)
		{
		if (!iG711DecoderConfig)
			{
			TRAPD(err, iG711DecoderConfig = CMMFG711DecoderConfig::NewL());
			if (err == KErrNone && iG711DecoderConfig)
				{
				MG711DecoderIntfc* ptr = this;
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

TInt CG711DecoderConfigTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CG711DecoderConfigTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CG711DecoderConfigTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CG711DecoderConfigTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CG711DecoderConfigTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CG711DecoderConfigTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CG711DecoderConfigTestDevice::SetDecoderMode(TDecodeMode aDecodeMode)
	{
	TInt result = KErrBadHandle;

	if (iG711DecoderConfig)
		{
		result = iG711DecoderConfig->SetDecoderMode(aDecodeMode);
		}

	return result;
	}

TInt CG711DecoderConfigTestDevice::GetDecoderMode(TDecodeMode& aDecodeMode)
	{
	TInt result = KErrBadHandle;

	if (iG711DecoderConfig)
		{
		result = iG711DecoderConfig->GetDecoderMode(aDecodeMode);
		}

	return result;
	}

TInt CG711DecoderConfigTestDevice::SetComfortNoiseGeneration(TBool aCng)
	{
	TInt result = KErrBadHandle;

	if (iG711DecoderConfig)
		{
		result = iG711DecoderConfig->SetComfortNoiseGeneration(aCng);
		}

	return result;
	}

TInt CG711DecoderConfigTestDevice::GetComfortNoiseGeneration(TBool& aCng)
	{
	TInt result = KErrBadHandle;

	if (iG711DecoderConfig)
		{
		result = iG711DecoderConfig->GetComfortNoiseGeneration(aCng);
		}

	return result;
	}

TInt CG711DecoderConfigTestDevice::SetPacketLossConcealment(TBool aPlc)
	{
	TInt result = KErrBadHandle;

	if (iG711DecoderConfig)
		{
		result = iG711DecoderConfig->SetPacketLossConcealment(aPlc);
		}

	return result;
	}

TInt CG711DecoderConfigTestDevice::GetPacketLossConcealment(TBool& aPlc)
	{
	TInt result = KErrBadHandle;

	if (iG711DecoderConfig)
		{
		result = iG711DecoderConfig->GetPacketLossConcealment(aPlc);
		}

	return result;
	}
