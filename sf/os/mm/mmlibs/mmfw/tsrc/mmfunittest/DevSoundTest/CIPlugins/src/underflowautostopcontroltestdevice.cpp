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
#include "underflowautostopcontroltestdevice.h"
#include "devsoundciutestdevices.hrh"


/*
CMMFUnderflowAutoStopControl implementation
*/
CMMFUnderflowAutoStopControl* CMMFUnderflowAutoStopControl::NewL()
	{
	CMMFUnderflowAutoStopControl* self = new(ELeave) CMMFUnderflowAutoStopControl();
	return self;
	}

CMMFUnderflowAutoStopControl::~CMMFUnderflowAutoStopControl()
	{
	}

// Actual implementation of method
TInt CMMFUnderflowAutoStopControl::MmuascTurnOffUnderflowAutoStop()
	{
	return KErrNone;
	}


/*
CUnderflowAutoStopControlTestDevice Implementation
*/
CMMFHwDevice* CUnderflowAutoStopControlTestDevice::NewL()
	{
	CUnderflowAutoStopControlTestDevice* self=new(ELeave) CUnderflowAutoStopControlTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUnderflowAutoStopControlTestDevice::~CUnderflowAutoStopControlTestDevice()
	{
	delete iUnderflowAutoStopCtrl;
	}

CUnderflowAutoStopControlTestDevice::CUnderflowAutoStopControlTestDevice()
	{
	}

void CUnderflowAutoStopControlTestDevice::ConstructL()
	{
	}

TInt CUnderflowAutoStopControlTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

TInt CUnderflowAutoStopControlTestDevice::Stop()
	{
	return 0;
	}

TInt CUnderflowAutoStopControlTestDevice::Pause()
	{
	return 0;
	}

TInt CUnderflowAutoStopControlTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}

TAny* CUnderflowAutoStopControlTestDevice::CustomInterface(TUid aInterfaceId)
	{
	// Just return something non-NULL to keep the
	// DevSound initialisation process happy
	TAny* ret = static_cast<TAny*>(this);

	// Now for the CIs we want to test...
	if (aInterfaceId == KUidUnderflowAutoStopControl)
		{
		if (!iUnderflowAutoStopCtrl)
			{
			TRAPD(err, iUnderflowAutoStopCtrl = CMMFUnderflowAutoStopControl::NewL());
			if (err == KErrNone && iUnderflowAutoStopCtrl)
				{
				MMMFUnderflowAutoStopControl* ptr = this;
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

TInt CUnderflowAutoStopControlTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}

TInt CUnderflowAutoStopControlTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}

TInt CUnderflowAutoStopControlTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}

TInt CUnderflowAutoStopControlTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}

TInt CUnderflowAutoStopControlTestDevice::DeleteCodec()
	{
	return 0;
	}

CMMFSwCodec& CUnderflowAutoStopControlTestDevice::Codec()
	{
	return *iCodec;
	}

TInt CUnderflowAutoStopControlTestDevice::MmuascTurnOffUnderflowAutoStop()
	{
	TInt result = KErrBadHandle;

	if (iUnderflowAutoStopCtrl)
		{
		result = iUnderflowAutoStopCtrl->MmuascTurnOffUnderflowAutoStop();
		}

	return result;
	}
