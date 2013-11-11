/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <f32file.h>
#include "speechencoderconfigtestdevice.h"
#include "devsoundciutestdevices.hrh"

/*
CMMFSpeechEncoderConfig implementation
*/
CMMFSpeechEncoderConfig* CMMFSpeechEncoderConfig::NewL()
	{
	CMMFSpeechEncoderConfig* self = new(ELeave) CMMFSpeechEncoderConfig();
	return self;
	}

CMMFSpeechEncoderConfig::~CMMFSpeechEncoderConfig()
	{
	}

//Actual implementation of method SetBitrate
TInt CMMFSpeechEncoderConfig::SetBitrate(TUint aBitrate)
	{
	iBitrate = aBitrate;
	return KErrNone;
	}

//Actual implementation of method GetBitrate
TInt CMMFSpeechEncoderConfig::GetBitrate (TUint& aBitrate)
	{
	aBitrate = iBitrate;
	return KErrNone;
	}

//Actual implementation of method GetSupportedBitrates
TInt CMMFSpeechEncoderConfig::GetSupportedBitrates(RArray<TUint>& aSupportedBitrates)
	{
	aSupportedBitrates.Append(12345);
	aSupportedBitrates.Append(23456);
	aSupportedBitrates.Append(34567);
	aSupportedBitrates.Append(45678);
	aSupportedBitrates.Append(56789);
	return KErrNone;
	}

//Actual implementation of method SetVadMode
TInt CMMFSpeechEncoderConfig::SetVadMode(TBool aVadModeOn)
	{
	iVadModeOn = aVadModeOn;
	return KErrNone;
	}

//Actual implementation of method GetVadMode
TInt CMMFSpeechEncoderConfig::GetVadMode(TBool& aVadModeOn)
	{
	aVadModeOn = iVadModeOn;
	return KErrNone;
	}


/*
CSpeechEncoderConfigTestDevice implementation
*/
CMMFHwDevice* CSpeechEncoderConfigTestDevice::NewL()
	{
	CSpeechEncoderConfigTestDevice* self=new(ELeave) CSpeechEncoderConfigTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSpeechEncoderConfigTestDevice::~CSpeechEncoderConfigTestDevice()
	{
	delete iSpeechEncoderConfig;
	}

CSpeechEncoderConfigTestDevice::CSpeechEncoderConfigTestDevice()
	{
	}

void CSpeechEncoderConfigTestDevice::ConstructL()
	{
	}

TInt CSpeechEncoderConfigTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CSpeechEncoderConfigTestDevice::Stop()
	{
	return 0;
	}

TInt CSpeechEncoderConfigTestDevice::Pause()
	{
	return 0;
	}

TInt CSpeechEncoderConfigTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CSpeechEncoderConfigTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidSpeechEncoderConfig)
		{
		if (!iSpeechEncoderConfig)
			{
			TRAPD(err, iSpeechEncoderConfig = CMMFSpeechEncoderConfig::NewL());
			if (err == KErrNone && iSpeechEncoderConfig)
				{
				MSpeechEncoderConfig* ptr = this;
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

TInt CSpeechEncoderConfigTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CSpeechEncoderConfigTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CSpeechEncoderConfigTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CSpeechEncoderConfigTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CSpeechEncoderConfigTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CSpeechEncoderConfigTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CSpeechEncoderConfigTestDevice::SetBitrate(TUint aBitrate)
	{
	TInt result = KErrBadHandle;

	if (iSpeechEncoderConfig)
		{
		result = iSpeechEncoderConfig->SetBitrate(aBitrate);
		}

	return result;
	}

TInt CSpeechEncoderConfigTestDevice::GetBitrate(TUint& aBitrate)
	{
	TInt result = KErrBadHandle;

	if (iSpeechEncoderConfig)
		{
		result = iSpeechEncoderConfig->GetBitrate(aBitrate);
		}

	return result;
	}

TInt CSpeechEncoderConfigTestDevice::GetSupportedBitrates(RArray<TUint>& aSupportedBitrates)
	{
	TInt result = KErrBadHandle;

	if (iSpeechEncoderConfig)
		{
		result = iSpeechEncoderConfig->GetSupportedBitrates(aSupportedBitrates);
		}

	return result;
	}

TInt CSpeechEncoderConfigTestDevice::SetVadMode(TBool aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iSpeechEncoderConfig)
		{
		result = iSpeechEncoderConfig->SetVadMode(aVadModeOn);
		}

	return result;
	}

TInt CSpeechEncoderConfigTestDevice::GetVadMode(TBool& aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iSpeechEncoderConfig)
		{
		result = iSpeechEncoderConfig->GetVadMode(aVadModeOn);
		}

	return result;
	}
