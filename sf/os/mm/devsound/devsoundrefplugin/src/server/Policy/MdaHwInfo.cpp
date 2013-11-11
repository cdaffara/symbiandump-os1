// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MdaHwInfo.h"
#include <e32svr.h>

#ifndef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
	_LIT(KPddFileName,"ESDRV.PDD");
	_LIT(KLddFileName,"ESOUND.LDD");
#endif

CMdaHwInfo::~CMdaHwInfo()
	{
	}

void CMdaHwInfo::ConstructL()
	{
	GetHwInfoL();
	}

CMdaHwInfo* CMdaHwInfo::NewL()
	{	
	CMdaHwInfo* self = new(ELeave)CMdaHwInfo();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return(self);
	}

CMdaHwInfo::CMdaHwInfo()
	{
	}

void CMdaHwInfo::GetHwInfoL()
	{
//#ifdef __WINS__ @@@ Sound drivers must be present!!!
#ifndef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
	TInt ret = User::LoadPhysicalDevice(KPddFileName);
	if ((ret!=KErrNone) && (ret!=KErrAlreadyExists))
		User::Leave(ret);
	ret = User::LoadLogicalDevice(KLddFileName);
	if ((ret!=KErrNone) && (ret!=KErrAlreadyExists))
		User::Leave(ret);
#endif//Adapter loads the drivers too.
	User::LeaveIfError(iDevice.Open());
	if (!iDevice.Handle())
		User::Leave(KErrBadHandle);

	iDevice.PlayFormatsSupported(iPlayFormatsSupported);
	iDevice.GetPlayFormat(iPlayFormat);
	iDevice.RecordFormatsSupported(iRecordFormatsSupported);
	iDevice.GetRecordFormat(iRecordFormat);

	iDevice.Close();

//#endif

	}
