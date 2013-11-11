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
#include "g711encoderconfigtestdevice.h"
#include "devsoundciutestdevices.hrh"


/*
CMMFG711EncoderConfig implementation
*/
CMMFG711EncoderConfig* CMMFG711EncoderConfig::NewL()
	{
	CMMFG711EncoderConfig* self = new(ELeave) CMMFG711EncoderConfig();
	return self;
	}

CMMFG711EncoderConfig::~CMMFG711EncoderConfig()
	{
	}

//Actual implementation of method SetEncoderMode
TInt CMMFG711EncoderConfig::SetEncoderMode(TEncodeMode aEncodeMode)
	{
	RFs		fs;
	RFile	file;
	TInt	err = KErrNone;

	if ( KErrNone != (err = fs.Connect()) )
		{
		return err;
		}

	// this file name will be use on the testStep to compare the stored value.	
	_LIT(KFileName, "c:\\temp\\g711EncoderConfig.txt");
	fs.MkDirAll(KFileName);

	if ( KErrNone != (err = file.Replace(fs, KFileName, EFileWrite)) )
		{
		return err;	
		}

	TBuf8<4> data;
	data.Format(_L8("%d"), aEncodeMode);

	file.Write(data);
	file.Close();
	fs.Close();

	return err;
	}

//Actual implementation of method SetVadMode
TInt CMMFG711EncoderConfig::SetVadMode(TBool aVadModeOn)
	{
	iVadModeOn = aVadModeOn;
	return KErrNone;
	}

//Actual implementation of method GetVadMode
TInt CMMFG711EncoderConfig::GetVadMode(TBool& aVadModeOn)
	{
	aVadModeOn = iVadModeOn;
	return KErrNone;
	}


/*
CG711EncoderConfigTestDevice implementation
*/
CMMFHwDevice* CG711EncoderConfigTestDevice::NewL()
	{
	CG711EncoderConfigTestDevice* self=new(ELeave) CG711EncoderConfigTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CG711EncoderConfigTestDevice::~CG711EncoderConfigTestDevice()
	{
	delete iG711EncoderConfig;
	}

CG711EncoderConfigTestDevice::CG711EncoderConfigTestDevice()
	{
	}

void CG711EncoderConfigTestDevice::ConstructL()
	{
	}

TInt CG711EncoderConfigTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CG711EncoderConfigTestDevice::Stop()
	{
	return 0;
	}

TInt CG711EncoderConfigTestDevice::Pause()
	{
	return 0;
	}

TInt CG711EncoderConfigTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CG711EncoderConfigTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidG711EncoderIntfc)
		{
		if (!iG711EncoderConfig)
			{
			TRAPD(err, iG711EncoderConfig = CMMFG711EncoderConfig::NewL());
			if (err == KErrNone && iG711EncoderConfig)
				{
				MG711EncoderIntfc* ptr = this;
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

TInt CG711EncoderConfigTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CG711EncoderConfigTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CG711EncoderConfigTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CG711EncoderConfigTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CG711EncoderConfigTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CG711EncoderConfigTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CG711EncoderConfigTestDevice::SetEncoderMode(TEncodeMode aEncodeMode)
	{
	TInt result = KErrBadHandle;

	if (iG711EncoderConfig)
		{
		result = iG711EncoderConfig->SetEncoderMode(aEncodeMode);
		}

	return result;
	}

TInt CG711EncoderConfigTestDevice::SetVadMode(TBool aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iG711EncoderConfig)
		{
		result = iG711EncoderConfig->SetVadMode(aVadModeOn);
		}

	return result;
	}

TInt CG711EncoderConfigTestDevice::GetVadMode(TBool& aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iG711EncoderConfig)
		{
		result = iG711EncoderConfig->GetVadMode(aVadModeOn);
		}

	return result;
	}
