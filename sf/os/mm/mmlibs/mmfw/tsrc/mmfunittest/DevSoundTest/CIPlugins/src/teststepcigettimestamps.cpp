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
#include "teststepcigettimestamps.h"
#include "cidefine.h"

/*****************************************************************************/
CTestStepCICreateGetTimestamps::CTestStepCICreateGetTimestamps()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0085-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCICreateGetTimestamps::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidGetTimestampsTestDevice}; 
	#else
	TFourCC testUID('T','0','0','6');
	#endif
	
	MMMFGetTimestamps* interface = NULL;
	interface = static_cast<MMMFGetTimestamps*>(
							InitializeAndInstantiateInterfaceL(testUID, KUidGetTimestamps));

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
CTestStepCIGetTimestampsEnable::CTestStepCIGetTimestampsEnable()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0086-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCIGetTimestampsEnable::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidGetTimestampsTestDevice}; 
	#else
	TFourCC testUID('T','0','0','6');
	#endif
	
	MMMFGetTimestamps* interface = NULL;
	interface = static_cast<MMMFGetTimestamps*>(InitializeAndInstantiateInterfaceL(testUID, KUidGetTimestamps));

	if (interface)
		{
		TBool enabled = ETrue;
		INFO_PRINTF1(_L("Calling test CI method"));
		TInt err = interface->MmgtSetRecordSystemTimestampsEnabled(enabled);
		TBool flag = EFalse;
		// Reset test result in case we leave...
		iTestStepResult = EFail;
		DoReadFromFileL(flag);
		if ((err == KErrNone) && (flag == enabled))
			{
			INFO_PRINTF1(_L("Test CI method call was successful"));
			iTestStepResult = EPass;
			}
		else
			{
			INFO_PRINTF4(_L("Test CI method call failed, error: %d, expected value: %d, \
								actual value: %d"), err, enabled, flag);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Failed to retrieve the custom interface."));
		iTestStepResult = EInconclusive;
		}
			
	return iTestStepResult;
	}

void CTestStepCIGetTimestampsEnable::DoReadFromFileL(TBool& aFlag)
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

	// This function is responsible for the deletion of the file after use
	User::LeaveIfError(fs.Delete(KCITestFileName));

	CleanupStack::PopAndDestroy(); // fs
	}

/*****************************************************************************/
CTestStepCIGetTimestampsForBuffer::CTestStepCIGetTimestampsForBuffer()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0087-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCIGetTimestampsForBuffer::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidGetTimestampsTestDevice}; 
	#else
	TFourCC testUID('T','0','0','6');
	#endif
	
	MMMFGetTimestamps* interface = NULL;
	interface = static_cast<MMMFGetTimestamps*>(InitializeAndInstantiateInterfaceL(testUID, KUidGetTimestamps));

	if (interface)
		{
		INFO_PRINTF1(_L("Calling test CI method"));
		// Test device implementation expects this value:
		TTimeIntervalMicroSeconds bufferPosition(KTimestampPosition);
		TTime timestamp;
		TInt err = interface->MmgtGetSystemTimestampForBuffer(bufferPosition, timestamp);
		// and it should return this value:
		TTime testTimestamp(KTimestampValue);
		if ((err == KErrNone) && (testTimestamp == timestamp))
			{
			INFO_PRINTF1(_L("Test CI method call was successful"));
			iTestStepResult = EPass;
			}
		else
			{
			INFO_PRINTF3(_L("Expected timestamp: %d, actual timestamp: %d"), \
							timestamp.Int64(), testTimestamp.Int64());
			iTestStepResult = EFail;
			}				
		}
	else
		{
		INFO_PRINTF1(_L("Failed to retrieve the custom interface."));
		iTestStepResult = EInconclusive;
		}
					
	return iTestStepResult;
	}
