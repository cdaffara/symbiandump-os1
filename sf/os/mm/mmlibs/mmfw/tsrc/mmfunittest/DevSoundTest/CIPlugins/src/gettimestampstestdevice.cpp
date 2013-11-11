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
#include "gettimestampstestdevice.h"
#include "cidefine.h"

/*****************************************************************************/
// Implementation

CMMFHwDevice* CGetTimestampsTestDevice::NewL()
	{
	CGetTimestampsTestDevice* self = new(ELeave) CGetTimestampsTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*****************************************************************************/
CGetTimestampsTestDevice::~CGetTimestampsTestDevice()
	{
	}

/*****************************************************************************/
CGetTimestampsTestDevice::CGetTimestampsTestDevice()
	{
	}

/*****************************************************************************/
void CGetTimestampsTestDevice::ConstructL()
	{
	}
	
/*****************************************************************************/
TInt CGetTimestampsTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

/*****************************************************************************/
TInt CGetTimestampsTestDevice::Stop()
	{
	return 0;
	}
	
/*****************************************************************************/
TInt CGetTimestampsTestDevice::Pause()
	{
	return 0;
	}

/*****************************************************************************/		
TInt CGetTimestampsTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TAny* CGetTimestampsTestDevice::CustomInterface(TUid aInterfaceId)
	{
	MMMFGetTimestamps* interface = NULL;

	// DevSound initialisation requires something to be returned for both of
	// these two uids: KMmfPlaySettingsCustomInterface, KMmfRecordSettingsCustomInterface
	if ((aInterfaceId == KUidGetTimestamps) || // This interface
		(aInterfaceId.iUid == KMmfPlaySettingsCustomInterface) ||
		(aInterfaceId.iUid == KMmfRecordSettingsCustomInterface))
		{
		interface = this;
		}	

	return interface;
	}

/*****************************************************************************/
TInt CGetTimestampsTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CGetTimestampsTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CGetTimestampsTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CGetTimestampsTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}
	
/*****************************************************************************/
TInt CGetTimestampsTestDevice::DeleteCodec()
	{
	return 0;
	}

/*****************************************************************************/
CMMFSwCodec& CGetTimestampsTestDevice::Codec()
	{
	return *iCodec;
	}

/*****************************************************************************/
TInt CGetTimestampsTestDevice::MmgtSetRecordSystemTimestampsEnabled(TBool aEnable)
	{
	TRAPD(err, DoWriteToFileL(aEnable));
	return err;
	}

TInt CGetTimestampsTestDevice::MmgtGetSystemTimestampForBuffer(const TTimeIntervalMicroSeconds& aBufferPosition,
																TTime& aTimestamp) const
	{
	// Expected position is...
	TTimeIntervalMicroSeconds expectedPos = TTimeIntervalMicroSeconds(KTimestampPosition);
	if (aBufferPosition == expectedPos)
		{
		// ...found so return the expected value.
		aTimestamp = KTimestampValue;
		return KErrNone;
		}
	else
		{
		return KErrArgument;
		}
	}

void CGetTimestampsTestDevice::DoWriteToFileL(TBool aEnable)
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
	outputBuf.Num(aEnable);
	User::LeaveIfError(file.Write(outputBuf));
	CleanupStack::PopAndDestroy(2); // fs, file
	}
