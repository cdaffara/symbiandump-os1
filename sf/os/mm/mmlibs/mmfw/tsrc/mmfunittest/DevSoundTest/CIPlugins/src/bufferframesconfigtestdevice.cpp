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
#include "bufferframesconfigtestdevice.h"
#include "cidefine.h"

/*****************************************************************************/
// Implementation

CMMFHwDevice* CBufferFramesConfigTestDevice::NewL()
	{
	CBufferFramesConfigTestDevice* self = new(ELeave) CBufferFramesConfigTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*****************************************************************************/
CBufferFramesConfigTestDevice::~CBufferFramesConfigTestDevice()
	{
	}

/*****************************************************************************/
CBufferFramesConfigTestDevice::CBufferFramesConfigTestDevice()
	{
	}

/*****************************************************************************/
void CBufferFramesConfigTestDevice::ConstructL()
	{
	}
	
/*****************************************************************************/
TInt CBufferFramesConfigTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

/*****************************************************************************/
TInt CBufferFramesConfigTestDevice::Stop()
	{
	return 0;
	}
	
/*****************************************************************************/
TInt CBufferFramesConfigTestDevice::Pause()
	{
	return 0;
	}

/*****************************************************************************/		
TInt CBufferFramesConfigTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TAny* CBufferFramesConfigTestDevice::CustomInterface(TUid aInterfaceId)
	{
	MMMFBufferFramesConfig* interface = NULL;

	// DevSound initialisation requires something to be returned for both of
	// these two uids: KMmfPlaySettingsCustomInterface, KMmfRecordSettingsCustomInterface
	if ((aInterfaceId == KUidBufferFramesConfig) || // This interface
		(aInterfaceId.iUid == KMmfPlaySettingsCustomInterface) ||
		(aInterfaceId.iUid == KMmfRecordSettingsCustomInterface))
		{
		interface = this;
		}
		
	return interface;
	}

/*****************************************************************************/
TInt CBufferFramesConfigTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CBufferFramesConfigTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CBufferFramesConfigTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CBufferFramesConfigTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}
	
/*****************************************************************************/
TInt CBufferFramesConfigTestDevice::DeleteCodec()
	{
	return 0;
	}

/*****************************************************************************/
CMMFSwCodec& CBufferFramesConfigTestDevice::Codec()
	{
	return *iCodec;
	}

/*****************************************************************************/
TInt CBufferFramesConfigTestDevice::MmbfcSetNumberOfFramesPerInputBuffer(TInt aFrameCount, TInt aSamplesPerFrame)
	{
	TRAPD(err, DoWriteToFileL(aFrameCount, aSamplesPerFrame));
	return err;
	}

TInt CBufferFramesConfigTestDevice::MmbfcSetNumberOfFramesPerOutputBuffer(TInt aFrameCount, TInt aSamplesPerFrame)
	{
	TRAPD(err, DoWriteToFileL(aFrameCount, aSamplesPerFrame));
	return err;
	}

void CBufferFramesConfigTestDevice::DoWriteToFileL(TInt aFrameCount, TInt aSamplesPerFrame)															
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	RFile file;
	CleanupClosePushL(file);
	
	// File doesn't yet exist
	// It is the responsibility of the calling test function to delete the file after use
	User::LeaveIfError(file.Create(fs, KCITestFileName, EFileWrite));
	TBuf8<KMaxCITestFileTimeDataLength> outputBuf;
	outputBuf.Num(aFrameCount);
	outputBuf.Append(' ');
	outputBuf.AppendNum(aSamplesPerFrame);
	User::LeaveIfError(file.Write(outputBuf));
	CleanupStack::PopAndDestroy(2); // fs, file
	}


