// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Graphics Resource - logical device driver implementation
//

#include "sgdeviceimpl.h"

_LIT(KSgDevicePanicCategory, "SGDEV");

void Panic(TSgDevicePanicReason aReason)
	{
	Kern::PanicCurrentThread(KSgDevicePanicCategory, aReason);
	}

DECLARE_STANDARD_LDD()
	{
	return new DSgDevice;
	}

TInt DSgDevice::Install()
	{
	return SetName(&KSgDeviceName);
	}

void DSgDevice::GetCaps(TDes8& aDes) const
	{
	aDes.Zero();
	}

TInt DSgDevice::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel = new DSgChannel;
	return aChannel ? KErrNone : KErrNoMemory;
	}
