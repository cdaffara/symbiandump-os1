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
#include "aacdecoderconfigtestdevice.h"
#include "devsoundciutestdevices.hrh"


/*
CMMFAacDecoderConfig implementation
*/
CMMFAacDecoderConfig* CMMFAacDecoderConfig::NewL()
	{
	CMMFAacDecoderConfig* self = new(ELeave) CMMFAacDecoderConfig();
	return self;
	}

CMMFAacDecoderConfig::~CMMFAacDecoderConfig()
	{
	}

// Actual implementation of method
TInt CMMFAacDecoderConfig::SetAudioConfig(TAudioConfig& aAudioConfig)
	{
	RFs		fs;
	RFile	file;
	TInt	err = KErrNone;

	if ( KErrNone != (err = fs.Connect()) )
		{
		return err;
		}

	// this file name will be use on the testStep to compare the stored value.	
	_LIT(KFileName, "c:\\temp\\aacDecoderConfig.txt");
	fs.MkDirAll(KFileName);

	if ( KErrNone != (err = file.Replace(fs, KFileName, EFileWrite)) )
		{
		return err;	
		}

	TBuf8<4> Line;
	Line.Format(_L8("%02d"), aAudioConfig.iAudioObjectType);

	file.Write(Line);
	file.Close();
	fs.Close();

	return err;
	}

// Actual implementation of method
TInt CMMFAacDecoderConfig::GetSupportedAudioConfigs(RArray<TAudioConfig>& aSupportedAudioConfigs)
	{
	TAudioConfig audioConfig;
	audioConfig.iAudioObjectType = TAudioConfig::ESbr;
	aSupportedAudioConfigs.Append(audioConfig);
	audioConfig.iAudioObjectType = TAudioConfig::EAacLtp;
	aSupportedAudioConfigs.Append(audioConfig);
	audioConfig.iAudioObjectType = TAudioConfig::EAacSsr;
	aSupportedAudioConfigs.Append(audioConfig);
	audioConfig.iAudioObjectType = TAudioConfig::EAacLc;
	aSupportedAudioConfigs.Append(audioConfig);
	audioConfig.iAudioObjectType = TAudioConfig::EAacMain;
	aSupportedAudioConfigs.Append(audioConfig);
	return KErrNone;
	}


/*
CAacDecoderConfigTestDevice Implementation
*/
CMMFHwDevice* CAacDecoderConfigTestDevice::NewL()
	{
	CAacDecoderConfigTestDevice* self=new(ELeave) CAacDecoderConfigTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAacDecoderConfigTestDevice::~CAacDecoderConfigTestDevice()
	{
	delete iAacDecoderConfig;
	}

CAacDecoderConfigTestDevice::CAacDecoderConfigTestDevice()
	{
	}

void CAacDecoderConfigTestDevice::ConstructL()
	{
	}

TInt CAacDecoderConfigTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CAacDecoderConfigTestDevice::Stop()
	{
	return 0;
	}

TInt CAacDecoderConfigTestDevice::Pause()
	{
	return 0;
	}

TInt CAacDecoderConfigTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CAacDecoderConfigTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidAacDecoderConfig)
		{
		if (!iAacDecoderConfig)
			{
			TRAPD(err, iAacDecoderConfig = CMMFAacDecoderConfig::NewL());
			if (err == KErrNone && iAacDecoderConfig)
				{
				MAacDecoderConfig* ptr = this;
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

TInt CAacDecoderConfigTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CAacDecoderConfigTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CAacDecoderConfigTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CAacDecoderConfigTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CAacDecoderConfigTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CAacDecoderConfigTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CAacDecoderConfigTestDevice::SetAudioConfig(TAudioConfig& aAudioConfig)
	{
	TInt result = KErrBadHandle;

	if (iAacDecoderConfig)
		{
		result = iAacDecoderConfig->SetAudioConfig(aAudioConfig);
		}

	return result;
	}

TInt CAacDecoderConfigTestDevice::GetSupportedAudioConfigs(RArray<MAacDecoderConfig::TAudioConfig>& aSupportedAudioConfigs)
	{
	TInt result = KErrBadHandle;

	if (iAacDecoderConfig)
		{
		result = iAacDecoderConfig->GetSupportedAudioConfigs(aSupportedAudioConfigs);
		}

	return result;
	}
