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
#include "teststepcisetdrmprotected.h"
#include "cidefine.h"

/*****************************************************************************/
CTestStepCICreateSetDRMProtected::CTestStepCICreateSetDRMProtected()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0080-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCICreateSetDRMProtected::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSetDrmProtectedTestDevice};
	#else
	TFourCC testUID('T','0','0','4');
	#endif
	
	MMMFSetDRMProtected* interface = NULL;
	interface = static_cast<MMMFSetDRMProtected*>(InitializeAndInstantiateInterfaceL(testUID, KUidSetDRMProtected));

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
CTestStepCISetDRMProtected::CTestStepCISetDRMProtected()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0081-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCISetDRMProtected::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSetDrmProtectedTestDevice};
	#else
	TFourCC testUID('T','0','0','4');
	#endif
	
	MMMFSetDRMProtected* interface = NULL;
	interface = static_cast<MMMFSetDRMProtected*>(InitializeAndInstantiateInterfaceL(testUID, KUidSetDRMProtected));

	if (interface)
		{
		TBool DRMProtected = ETrue;
		INFO_PRINTF1(_L("Calling test CI method"));
		TInt err = interface->MmsdpMarkDataAsDRMProtected(DRMProtected);
		TBool flag = EFalse;
		// Reset test result in case we leave...
		iTestStepResult = EFail;
		DoReadFromFileL(flag);
		if ((err == KErrNone) && (flag == DRMProtected))
			{
			INFO_PRINTF1(_L("Test CI method call was successful"));
			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF4(_L("Test CI method failed, error: %d, expected value: %d, actual value: %d"), \
							err, DRMProtected, flag);
			}
		}
	else
		{
		WARN_PRINTF1(_L("Failed to retrieve the custom interface."));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}

void CTestStepCISetDRMProtected::DoReadFromFileL(TBool& aFlag)
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
