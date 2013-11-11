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
#include "ilbcdecoderconfigtestdevice.h"
#include "devsoundciutestdevices.hrh"

/*
CMMFIlbcDecoderConfig implementation
*/
CMMFIlbcDecoderConfig* CMMFIlbcDecoderConfig::NewL()
	{
	CMMFIlbcDecoderConfig* self = new(ELeave) CMMFIlbcDecoderConfig();
	return self;
	}

CMMFIlbcDecoderConfig::~CMMFIlbcDecoderConfig()
	{
	}

//Actual implementation of method SetDecoderMode
TInt CMMFIlbcDecoderConfig::SetDecoderMode(TDecodeMode aDecodeMode)
	{
	RFs		fs;
	RFile	file;
	TInt	err = KErrNone;

	if ( KErrNone != (err = fs.Connect()) )
		{
		return err;
		}

	// this file name will be use on the testStep to compare the stored value.
	_LIT(KFileName, "c:\\temp\\IlbcDecoderConfig.txt");
	fs.MkDirAll(KFileName);

	if ( KErrNone != (err = file.Replace(fs, KFileName, EFileWrite)) )
		{
		return err;	
		}

	TBuf8<4> data;
	data.Format(_L8("%d"), aDecodeMode);

	file.Write(data);
	file.Close();
	fs.Close();

	return err;
	}

//Actual implementation of method SetCng
TInt CMMFIlbcDecoderConfig::SetComfortNoiseGeneration(TBool aCng)
	{
	iCng = aCng;
	return KErrNone;
	}

//Actual implementation of method GetCng
TInt CMMFIlbcDecoderConfig::GetComfortNoiseGeneration(TBool& aCng)
	{
	aCng = iCng;
	return KErrNone;
	}


/*
CIlbcDecoderConfigTestDevice implementation
*/
CMMFHwDevice* CIlbcDecoderConfigTestDevice::NewL()
	{
	CIlbcDecoderConfigTestDevice* self=new(ELeave) CIlbcDecoderConfigTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CIlbcDecoderConfigTestDevice::~CIlbcDecoderConfigTestDevice()
	{
	delete iIlbcDecoderConfig;
	}

CIlbcDecoderConfigTestDevice::CIlbcDecoderConfigTestDevice()
	{
	}

void CIlbcDecoderConfigTestDevice::ConstructL()
	{
	}

TInt CIlbcDecoderConfigTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CIlbcDecoderConfigTestDevice::Stop()
	{
	return 0;
	}

TInt CIlbcDecoderConfigTestDevice::Pause()
	{
	return 0;
	}

TInt CIlbcDecoderConfigTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CIlbcDecoderConfigTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidIlbcDecoderIntfc)
		{
		if (!iIlbcDecoderConfig)
			{
			TRAPD(err, iIlbcDecoderConfig = CMMFIlbcDecoderConfig::NewL());
			if (err == KErrNone && iIlbcDecoderConfig)
				{
				MIlbcDecoderIntfc* ptr = this;
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

TInt CIlbcDecoderConfigTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CIlbcDecoderConfigTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CIlbcDecoderConfigTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CIlbcDecoderConfigTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CIlbcDecoderConfigTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CIlbcDecoderConfigTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CIlbcDecoderConfigTestDevice::SetDecoderMode(TDecodeMode aDecodeMode)
	{
	TInt result = KErrBadHandle;

	if (iIlbcDecoderConfig)
		{
		result = iIlbcDecoderConfig->SetDecoderMode(aDecodeMode);
		}

	return result;
	}

TInt CIlbcDecoderConfigTestDevice::SetComfortNoiseGeneration(TBool aCng)
	{
	TInt result = KErrBadHandle;

	if (iIlbcDecoderConfig)
		{
		result = iIlbcDecoderConfig->SetComfortNoiseGeneration(aCng);
		}

	return result;
	}

TInt CIlbcDecoderConfigTestDevice::GetComfortNoiseGeneration(TBool& aCng)
	{
	TInt result = KErrBadHandle;

	if (iIlbcDecoderConfig)
		{
		result = iIlbcDecoderConfig->GetComfortNoiseGeneration(aCng);
		}

	return result;
	}
