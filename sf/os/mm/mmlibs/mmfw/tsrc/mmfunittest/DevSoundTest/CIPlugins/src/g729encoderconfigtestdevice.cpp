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
#include "g729encoderconfigtestdevice.h"
#include "devsoundciutestdevices.hrh"

/*
CMMFG729EncoderConfig implementation
*/
CMMFG729EncoderConfig* CMMFG729EncoderConfig::NewL()
	{
	CMMFG729EncoderConfig* self = new(ELeave) CMMFG729EncoderConfig();
	return self;
	}

CMMFG729EncoderConfig::~CMMFG729EncoderConfig()
	{
	}

//Actual implementation of method SetVadMode
TInt CMMFG729EncoderConfig::SetVadMode(TBool aVadModeOn)
	{
	RFs		fs;
	RFile	file;
	TInt	err = KErrNone;

	if ( KErrNone != (err = fs.Connect()) )
		{
		return err;
		}

	// this file name will be use on the testStep to compare the stored value.
	_LIT(KFileName, "c:\\temp\\g729EncoderConfig.txt");
	fs.MkDirAll(KFileName);

	if ( KErrNone != (err = file.Replace(fs, KFileName, EFileWrite)) )
		{
		return err;	
		}

	TBuf8<1> data;
	data.Format(_L8("%d"), aVadModeOn);

	file.Write(data);
	file.Close();
	fs.Close();

	return err;
	}

//Actual implementation of method GetVadMode
TInt CMMFG729EncoderConfig::GetVadMode(TBool& aVadModeOn)
	{
	RFs		fs;
	RFile	file;
	TInt	err = KErrNone;

	if ( KErrNone != (err = fs.Connect()) )
		{
		return err;
		}

	// this file name will be use on the testStep to compare the stored value.
	_LIT(KFileName, "c:\\temp\\g729EncoderConfig.txt");

	if ( KErrNone != (err = file.Open(fs, KFileName, EFileRead)) )
		{
		return err;	
		}

	TBuf8<4> data;
	file.Read(data);
	file.Close();

	fs.Delete(KFileName);
	fs.Close();

	aVadModeOn = (data.Compare(_L8("0")) == 0) ? EFalse : ETrue;

	return err;
	}


/*
CG729EncoderConfigTestDevice implementation
*/
CMMFHwDevice* CG729EncoderConfigTestDevice::NewL()
	{
	CG729EncoderConfigTestDevice* self=new(ELeave) CG729EncoderConfigTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CG729EncoderConfigTestDevice::~CG729EncoderConfigTestDevice()
	{
	delete iG729EncoderConfig;
	}

CG729EncoderConfigTestDevice::CG729EncoderConfigTestDevice()
	{
	}

void CG729EncoderConfigTestDevice::ConstructL()
	{
	}

TInt CG729EncoderConfigTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CG729EncoderConfigTestDevice::Stop()
	{
	return 0;
	}

TInt CG729EncoderConfigTestDevice::Pause()
	{
	return 0;
	}

TInt CG729EncoderConfigTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CG729EncoderConfigTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidG729EncoderIntfc)
		{
		if (!iG729EncoderConfig)
			{
			TRAPD(err, iG729EncoderConfig = CMMFG729EncoderConfig::NewL());
			if (err == KErrNone && iG729EncoderConfig)
				{
				MG729EncoderIntfc* ptr = this;
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

TInt CG729EncoderConfigTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CG729EncoderConfigTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CG729EncoderConfigTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CG729EncoderConfigTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CG729EncoderConfigTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CG729EncoderConfigTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CG729EncoderConfigTestDevice::SetVadMode(TBool aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iG729EncoderConfig)
		{
		result = iG729EncoderConfig->SetVadMode(aVadModeOn);
		}

	return result;
	}

TInt CG729EncoderConfigTestDevice::GetVadMode(TBool& aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iG729EncoderConfig)
		{
		result = iG729EncoderConfig->GetVadMode(aVadModeOn);
		}

	return result;
	}
