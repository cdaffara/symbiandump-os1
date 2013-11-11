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
#include "teststepcibufferframesconfig.h"
#include "cidefine.h"

/*****************************************************************************/
CTestStepCICreateBufferFramesConfig::CTestStepCICreateBufferFramesConfig()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0082-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCICreateBufferFramesConfig::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidBufferFramesConfigTestDevice};
	#else
	TFourCC testUID('T','0','0','7');
	#endif
	
	MMMFBufferFramesConfig* interface = NULL;
	interface = static_cast<MMMFBufferFramesConfig*>(InitializeAndInstantiateInterfaceL(testUID, KUidBufferFramesConfig));
	
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
CTestStepCIBufferFramesConfigSetBase::CTestStepCIBufferFramesConfigSetBase()
	{
	}
	
TVerdict CTestStepCIBufferFramesConfigSetBase::DoTestStepL()
	{
	return EPass;	
	}

void CTestStepCIBufferFramesConfigSetBase::DoReadFromFileL(TInt& aFrames, TInt& aSamples)
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	RFile file;
	CleanupClosePushL(file);
	
	// File should already exist
	User::LeaveIfError(file.Open(fs, KCITestFileName, EFileRead));
	TBuf8<KMaxCITestFileTimeDataLength> inputBuf;
	User::LeaveIfError(file.Read(inputBuf));
	// Do some lexical analysis
	TLex8 input(inputBuf);
	input.Mark();
	input.SkipCharacters();
	if (input.TokenLength() != 0)
		{
		TInt value;
		TLex8 temp(input.MarkedToken());
		User::LeaveIfError(temp.Val(value));
		// succesfully read the first value (the frames count)
		aFrames = value;
		
		input.SkipSpaceAndMark();
		input.SkipCharacters();
		if (input.TokenLength() != 0)
			{
			temp = input.MarkedToken();
			User::LeaveIfError(temp.Val(value));
			// succesfully read the seconde value (the samples count)
			aSamples = value;
			}
		}

	CleanupStack::PopAndDestroy(); // file	

	// It is the responsibility of this function to delete the file after use	
	User::LeaveIfError(fs.Delete(KCITestFileName));

	CleanupStack::PopAndDestroy(); // fs
	}


/*****************************************************************************/
CTestStepCIBufferFramesConfigSetInput::CTestStepCIBufferFramesConfigSetInput()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0083-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCIBufferFramesConfigSetInput::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidBufferFramesConfigTestDevice};
	#else
	TFourCC testUID('T','0','0','7');
	#endif
	
	MMMFBufferFramesConfig* interface = NULL;
	interface = static_cast<MMMFBufferFramesConfig*>(InitializeAndInstantiateInterfaceL(testUID, KUidBufferFramesConfig));
	
	if (interface)
		{
		TInt frameCount = 6;
		TInt samplesPerFrame = 66;
		INFO_PRINTF1(_L("Calling test CI method"));
		TInt err = interface->MmbfcSetNumberOfFramesPerInputBuffer(frameCount, samplesPerFrame);
		TInt testCount = 0;
		TInt testSamples = 0;
		// Reset test result in case we leave...
		iTestStepResult = EFail;
		DoReadFromFileL(testCount, testSamples);
		if ((err == KErrNone) && (frameCount == testCount) && (samplesPerFrame == testSamples))
			{
			INFO_PRINTF1(_L("Test CI method call was successful"));
			iTestStepResult = EPass;
			}
		else
			{
			INFO_PRINTF4(_L("Test CI method call failed, error: %d, expected frames: %d, \
							 expected samples: %d"), err, frameCount, samplesPerFrame);
			INFO_PRINTF3(_L("Actual values: frames: %d, samplesPerFrame: %d"), testCount, testSamples);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Failed to retrieve the custom interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}

/*****************************************************************************/
CTestStepCIBufferFramesConfigSetOutput::CTestStepCIBufferFramesConfigSetOutput()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0084-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCIBufferFramesConfigSetOutput::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidBufferFramesConfigTestDevice};
	#else
	TFourCC testUID('T','0','0','7');
	#endif
	
	MMMFBufferFramesConfig* interface = NULL;
	interface = static_cast<MMMFBufferFramesConfig*>(InitializeAndInstantiateInterfaceL(testUID, KUidBufferFramesConfig));
	
	if (interface)
		{
		TInt frameCount = 9;
		TInt samplesPerFrame = 99;
		INFO_PRINTF1(_L("Calling test CI method"));
		TInt err = interface->MmbfcSetNumberOfFramesPerOutputBuffer(frameCount, samplesPerFrame);
		TInt testCount = 0;
		TInt testSamples = 0;
		// Reset test result in case we leave...
		iTestStepResult = EFail;
		DoReadFromFileL(testCount, testSamples);
		if ((err == KErrNone) && (frameCount == testCount) && (samplesPerFrame == testSamples))
			{
			INFO_PRINTF1(_L("Test CI method call was successful"));
			iTestStepResult = EPass;
			}
		else
			{
			INFO_PRINTF4(_L("Test CI method call failed, error: %d, expected frames: %d, \
							 expected samples: %d"), err, frameCount, samplesPerFrame);
			INFO_PRINTF3(_L("Actual values: frames: %d, samplesPerFrame: %d"), testCount, testSamples);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Failed to retrieve the custom interface"));
		iTestStepResult = EInconclusive;
		}
					
	return iTestStepResult;
	}

