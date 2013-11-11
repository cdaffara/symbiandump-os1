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
#include "errorconcealmenttestdevice.h"
#include "devsoundciutestdevices.hrh"


/*
CMMFErrorConcealment implementation
*/
CMMFErrorConcealment* CMMFErrorConcealment::NewL()
	{
	CMMFErrorConcealment* self = new(ELeave) CMMFErrorConcealment();
	return self;
	}

CMMFErrorConcealment::~CMMFErrorConcealment()
	{
	}

//Actual implementation of method ConcealErrorForNextBuffer
TInt CMMFErrorConcealment::ConcealErrorForNextBuffer()
	{
	return KErrNone;
	}

//Actual implementation of method SetFrameMode
TInt CMMFErrorConcealment::SetFrameMode(TBool aFrameModeOn)
	{
	RFs		fs;
	RFile	file;
	TInt	err = KErrNone;

	if ( KErrNone != (err = fs.Connect()) )
		{
		return err;
		}

	// this file name will be use on the testStep to compare the stored value.	
	_LIT(KFileName, "c:\\temp\\errorConcealment.txt");
	fs.MkDirAll(KFileName);

	if ( KErrNone != (err = file.Replace(fs, KFileName, EFileWrite)) )
		{
		return err;	
		}

	TBuf8<1> data;
	data.Format(_L8("%d"), aFrameModeOn);

	file.Write(data);
	file.Close();
	fs.Close();

	return err;
	}

//Actual implementation of method FrameModeRqrdForEC
TInt CMMFErrorConcealment::FrameModeRqrdForEC(TBool& aFrameModeRqrd)
	{
	RFs		fs;
	RFile	file;
	TInt	err = KErrNone;

	if ( KErrNone != (err = fs.Connect()) )
		{
		return err;
		}

	// this file name will be use on the testStep to compare the stored value.
	_LIT(KFileName, "c:\\temp\\errorConcealment.txt");

	if ( KErrNone != (err = file.Open(fs, KFileName, EFileRead)) )
		{
		return err;	
		}

	TBuf8<1> data;
	file.Read(data);
	file.Close();
	
	fs.Delete(KFileName);
	fs.Close();

	TLex8 lex(data);
	lex.Val(aFrameModeRqrd);

	return err;
	}


/*
CErrorConcealmentTestDevice implementation
*/
CMMFHwDevice* CErrorConcealmentTestDevice::NewL()
	{
	CErrorConcealmentTestDevice* self=new(ELeave) CErrorConcealmentTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CErrorConcealmentTestDevice::~CErrorConcealmentTestDevice()
	{
	delete iErrorConcealment;
	}

CErrorConcealmentTestDevice::CErrorConcealmentTestDevice()
	{
	}

void CErrorConcealmentTestDevice::ConstructL()
	{
	}

TInt CErrorConcealmentTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CErrorConcealmentTestDevice::Stop()
	{
	return 0;
	}

TInt CErrorConcealmentTestDevice::Pause()
	{
	return 0;
	}

TInt CErrorConcealmentTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CErrorConcealmentTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidErrorConcealmentIntfc)
		{
		if (!iErrorConcealment)
			{
			TRAPD(err, iErrorConcealment = CMMFErrorConcealment::NewL());
			if (err == KErrNone && iErrorConcealment)
				{
				MMMFErrorConcealmentIntfc* ptr = this;
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

TInt CErrorConcealmentTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CErrorConcealmentTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CErrorConcealmentTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CErrorConcealmentTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CErrorConcealmentTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CErrorConcealmentTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CErrorConcealmentTestDevice::ConcealErrorForNextBuffer()
	{
	TInt result = KErrBadHandle;

	if (iErrorConcealment)
		{
		result = iErrorConcealment->ConcealErrorForNextBuffer();
		}

	return result;
	}

TInt CErrorConcealmentTestDevice::SetFrameMode(TBool aFrameModeOn)
	{
	TInt result = KErrBadHandle;

	if (iErrorConcealment)
		{
		result = iErrorConcealment->SetFrameMode(aFrameModeOn);
		}

	return result;
	}

TInt CErrorConcealmentTestDevice::FrameModeRqrdForEC(TBool& aFrameModeRqrd)
	{
	TInt result = KErrBadHandle;

	if (iErrorConcealment)
		{
		result = iErrorConcealment->FrameModeRqrdForEC(aFrameModeRqrd);
		}

	return result;
	}
