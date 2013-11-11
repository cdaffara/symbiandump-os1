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

#include <f32file.h>
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>
#include "setdrmprotectedtestdevice.h"
#include "cidefine.h"

/*****************************************************************************/
// Implementation

CMMFHwDevice* CSetDRMProtectedTestDevice::NewL()
	{
	CSetDRMProtectedTestDevice* self = new(ELeave) CSetDRMProtectedTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*****************************************************************************/
CSetDRMProtectedTestDevice::~CSetDRMProtectedTestDevice()
	{
	}

/*****************************************************************************/
CSetDRMProtectedTestDevice::CSetDRMProtectedTestDevice()
	{
	}

/*****************************************************************************/
void CSetDRMProtectedTestDevice::ConstructL()
	{
	}
	
/*****************************************************************************/
TInt CSetDRMProtectedTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

/*****************************************************************************/
TInt CSetDRMProtectedTestDevice::Stop()
	{
	return 0;
	}
	
/*****************************************************************************/
TInt CSetDRMProtectedTestDevice::Pause()
	{
	return 0;
	}

/*****************************************************************************/		
TInt CSetDRMProtectedTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TAny* CSetDRMProtectedTestDevice::CustomInterface(TUid aInterfaceId)
	{
	MMMFSetDRMProtected* interface = NULL;
	
	// DevSound initialisation requires something to be returned for both of
	// these two uids: KMmfPlaySettingsCustomInterface, KMmfRecordSettingsCustomInterface
	if ((aInterfaceId == KUidSetDRMProtected) || // This interface
		(aInterfaceId.iUid == KMmfPlaySettingsCustomInterface) ||
		(aInterfaceId.iUid == KMmfRecordSettingsCustomInterface))
		{
		interface = this;
		}
		
	return interface;
	}

/*****************************************************************************/
TInt CSetDRMProtectedTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CSetDRMProtectedTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CSetDRMProtectedTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CSetDRMProtectedTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CSetDRMProtectedTestDevice::DeleteCodec()
	{
	return 0;
	}

/*****************************************************************************/
CMMFSwCodec& CSetDRMProtectedTestDevice::Codec()
	{
	return *iCodec;
	}

/*****************************************************************************/
TInt CSetDRMProtectedTestDevice::MmsdpMarkDataAsDRMProtected(TBool aDRMProtected)
	{
	TRAPD(err, DoWriteToFileL(aDRMProtected));
	return err;
	}

void CSetDRMProtectedTestDevice::DoWriteToFileL(TBool aFlag)
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	RFile file;
	CleanupClosePushL(file);
	
	// File doesn't yet exist
	// It is the responsibility of the calling test function to delete the file after use
	User::LeaveIfError(file.Create(fs, KCITestFileName, EFileWrite));
	TBuf8<KMaxCITestFileDataLength> outputBuf;
	outputBuf.Num(aFlag);
	User::LeaveIfError(file.Write(outputBuf));
	CleanupStack::PopAndDestroy(2); // fs, file
	}


