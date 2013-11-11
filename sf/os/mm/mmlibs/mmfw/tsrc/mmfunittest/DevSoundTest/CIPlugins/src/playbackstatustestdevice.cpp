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

#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>

#include "playbackstatustestdevice.h"
#include "cidefine.h"

/*****************************************************************************/
// Implementation

CMMFHwDevice* CPlaybackStatusTestDevice::NewL()
	{
	CPlaybackStatusTestDevice* self=new(ELeave) CPlaybackStatusTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*****************************************************************************/
CPlaybackStatusTestDevice::~CPlaybackStatusTestDevice()
	{
	}

/*****************************************************************************/
CPlaybackStatusTestDevice::CPlaybackStatusTestDevice()
	{
	}

/*****************************************************************************/
void CPlaybackStatusTestDevice::ConstructL()
	{
	}
	
/*****************************************************************************/
TInt CPlaybackStatusTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

/*****************************************************************************/
TInt CPlaybackStatusTestDevice::Stop()
	{
	return 0;
	}
	
/*****************************************************************************/
TInt CPlaybackStatusTestDevice::Pause()
	{
	return 0;
	}

/*****************************************************************************/		
TInt CPlaybackStatusTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TAny* CPlaybackStatusTestDevice::CustomInterface(TUid aInterfaceId)
	{
	MMMFPlaybackStatus* interface = NULL;
	
	// DevSound initialisation requires something to be returned for both of
	// these two uids: KMmfPlaySettingsCustomInterface, KMmfRecordSettingsCustomInterface
	if ((aInterfaceId == KUidPlaybackStatus) || // This interface
		(aInterfaceId.iUid == KMmfPlaySettingsCustomInterface) ||
		(aInterfaceId.iUid == KMmfRecordSettingsCustomInterface))
		{
		interface = this;
		}
	
	return interface;
	}

/*****************************************************************************/
TInt CPlaybackStatusTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CPlaybackStatusTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CPlaybackStatusTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CPlaybackStatusTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CPlaybackStatusTestDevice::DeleteCodec()
	{
	return 0;
	}

/*****************************************************************************/
CMMFSwCodec& CPlaybackStatusTestDevice::Codec()
	{
	return *iCodec;
	}

/*****************************************************************************/
TInt CPlaybackStatusTestDevice::MmpsGetPlaybackStatusInformation(TMMFPlaybackStatus& aStatus)
	{
	aStatus.aInputBytes = 999;
	aStatus.aDecodedSamples = 999;
	aStatus.aDecodedSamplesPlayed = 999;
	aStatus.aSystemTime = 999;
	aStatus.aTotalSamplesPlayed = 999;
	
	return KErrNone;
	}

TInt CPlaybackStatusTestDevice::MmpsRequestLossOfSyncNotification()
	{
	// Set a flag in a file that the test step can read...
	TRAPD(err, WriteSyncNotificationToFileL(ETrue));
	
	return err;
	}
	
TInt CPlaybackStatusTestDevice::MmpsCancelLossOfSyncNotification()
	{
	// Set a flag in a file that the test step can read...
	TRAPD(err, WriteSyncNotificationToFileL(EFalse));
	return err;
	}

// aWriteRequest is set to ETrue when output to file should signify 
// call from MmpsRequestLossOfSyncNotification(), otherwise set it to EFalse
// when called from MmpsCancelLossOfSyncNotification()
void CPlaybackStatusTestDevice::WriteSyncNotificationToFileL(TBool aWriteRequest)
	{
	// File handling
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	RFile file;
	CleanupClosePushL(file);
	
	// File doesn't yet exist
	// It is the responsibility of the calling test function to delete the file after use
	User::LeaveIfError(file.Create(fs, KCITestFileName, EFileWrite));
	TBuf8<KMaxCITestFileDataLength> outputBuf;
	outputBuf.Num(aWriteRequest);
	User::LeaveIfError(file.Write(outputBuf));
	CleanupStack::PopAndDestroy(2); // fs, file	
	}
