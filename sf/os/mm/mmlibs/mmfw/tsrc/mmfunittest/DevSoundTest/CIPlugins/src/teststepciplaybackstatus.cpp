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

#include "teststepciplaybackstatus.h"
#include "cidefine.h"

/*****************************************************************************/
CTestStepCIPlaybackStatus::CTestStepCIPlaybackStatus()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0074-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCIPlaybackStatus::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidPlayBackStatusTestDevice}; 
	#else
	TFourCC testUID('T','0','0','1');
	#endif

	MMMFPlaybackStatus* interface = NULL;
	interface = static_cast<MMMFPlaybackStatus*>(InitializeAndInstantiateInterfaceL(testUID, KUidPlaybackStatus));

	if (interface)
		{
		iTestStepResult = EPass;
		INFO_PRINTF1(_L("Successfully retrieved the interface."));
		}
	else
		{
		iTestStepResult = EFail;
		ERR_PRINTF1(_L("Failed to retrieve the interface"));
		}

	return iTestStepResult;
	}

/*****************************************************************************/
CTestStepCIPlaybackStatusInfo::CTestStepCIPlaybackStatusInfo()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0075-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCIPlaybackStatusInfo::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidPlayBackStatusTestDevice}; 
	#else
	TFourCC testUID('T','0','0','1');
	#endif
	
	MMMFPlaybackStatus* interface = NULL;
	interface = static_cast<MMMFPlaybackStatus*>(InitializeAndInstantiateInterfaceL(testUID, KUidPlaybackStatus));

	if (interface)
		{
		INFO_PRINTF1(_L("Successfully retrieved the interface."));
		
		MMMFPlaybackStatus::TMMFPlaybackStatus status;
		
		// Set values before call.
		status.aInputBytes = 0;
		status.aDecodedSamples = 0;
		status.aDecodedSamplesPlayed = 0;
		status.aSystemTime = 0;
		status.aTotalSamplesPlayed = 0;
		
		interface->MmpsGetPlaybackStatusInformation(status); // call method

		if(status.aInputBytes == 999 &&
			status.aDecodedSamples == 999 &&
			status.aDecodedSamplesPlayed == 999 &&
			status.aSystemTime == 999 &&
			status.aTotalSamplesPlayed == 999)
			{
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("Successfully got the status information."));
			}
		else
			{
			iTestStepResult = EFail;
			ERR_PRINTF1(_L("Failed to retrieve the status information"));
			}
		}
	else
		{
		ERR_PRINTF1(_L("Failed to retrieve the interface"));		
		}
		
	return iTestStepResult;
	}


/*****************************************************************************/
CTestStepCIPlaybackStatusRequestNotification::CTestStepCIPlaybackStatusRequestNotification(TBool aIsCancelTest)
											: iIsCancelTest(aIsCancelTest)
	{
	if (iIsCancelTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0098-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0097-HP");
		}
	}

/*****************************************************************************/
TVerdict CTestStepCIPlaybackStatusRequestNotification::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidPlayBackStatusTestDevice}; 
	#else
	TFourCC testUID('T','0','0','1');
	#endif
	
	MMMFPlaybackStatus* interface = NULL;
	interface = static_cast<MMMFPlaybackStatus*>(InitializeAndInstantiateInterfaceL(testUID, KUidPlaybackStatus));

	if (interface)
		{
		INFO_PRINTF1(_L("Successfully retrieved the interface."));

		iTestStepResult = EFail;		
		if (iIsCancelTest)
			{
			DoCancelTestL(*interface);
			}
		else
			{
			DoRequestTestL(*interface);
			}
		}
	else
		{
		ERR_PRINTF1(_L("Failed to retrieve the interface"));		
		}
		
	return iTestStepResult;
	}

void CTestStepCIPlaybackStatusRequestNotification::DoRequestTestL(MMMFPlaybackStatus& aInterface)
	{
	// Make the request.
	TInt err = aInterface.MmpsRequestLossOfSyncNotification();
	
	if (err == KErrNone)
		{
		// Get the result
		TBool testResult = EFalse;
		DoReadFromFileL(testResult);
		
		if (testResult)
			{
			// Value should be ETrue!
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("Successfully requested the notification."));			
			}
		else
			{		
			ERR_PRINTF2(_L("Failed to request the notification"), err);	
			}
		}
	else
		{
		ERR_PRINTF2(_L("Failed to request the notification"), err);		
		}
	}
	
void CTestStepCIPlaybackStatusRequestNotification::DoCancelTestL(MMMFPlaybackStatus& aInterface)
	{
	// Make the request.
	TInt err = aInterface.MmpsCancelLossOfSyncNotification();
	
	if (err == KErrNone)
		{
		// Get the result
		TBool testResult = ETrue;
		DoReadFromFileL(testResult);
		
		if (!testResult)
			{
			// Value should be EFalse!
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("Successfully cancelled the notification."));			
			}
		else
			{		
			ERR_PRINTF2(_L("Failed to cancel the notification"), err);	
			}
		}
	else
		{
		ERR_PRINTF2(_L("Failed to cancel the notification"), err);		
		}
	}

void CTestStepCIPlaybackStatusRequestNotification::DoReadFromFileL(TBool& aFlag)
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	RFile file;
	CleanupClosePushL(file);
	
	// File should already exist
	User::LeaveIfError(file.Open(fs, KCITestFileName, EFileRead));
	// Size should be 1!
	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));
	if (fileSize != KMaxCITestFileDataLength)
		{
		User::Leave(KErrCorrupt);
		}
	TBuf8<KMaxCITestFileDataLength> inputBuf;
	User::LeaveIfError(file.Read(inputBuf));
	TChar val = inputBuf[0];
	aFlag = val.GetNumericValue();

	CleanupStack::PopAndDestroy(); // file	

	// It is the responsibility of this function to delete the file after use	
	User::LeaveIfError(fs.Delete(KCITestFileName));

	CleanupStack::PopAndDestroy(); // fs
	}

