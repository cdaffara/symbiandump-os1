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

#include "teststepciaudiovibracontrol.h"


TVerdict CTestStepCIAudioVibraControl::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0066-HP")) == 0)
		{
		iTestStepResult = DoTestStep0066L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0067-HP")) == 0)
		{
		iTestStepResult = DoTestStep0067L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0068-HP")) == 0)
		{
		iTestStepResult = DoTestStep0068L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIAudioVibraControl::DoTestStep0066L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("AudioVibraControl - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAudioVibraControlTestDevice};
	#else
	TFourCC testUID('T','0','1','1');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidAudioVibraControl
	MAudioVibraControl* ptr = static_cast <MAudioVibraControl*> (iMMFDevSound->CustomInterface(KUidAudioVibraControl));

	if (ptr)
		{
		INFO_PRINTF1(_L("MAudioVibraControl instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MAudioVibraControl failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIAudioVibraControl::DoTestStep0067L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("AudioVibraControl - StartVibra"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAudioVibraControlTestDevice};
	#else
	TFourCC testUID('T','0','1','1');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidAudioVibraControl
	MAudioVibraControl* ptr = static_cast <MAudioVibraControl*> (iMMFDevSound->CustomInterface(KUidAudioVibraControl));

	if (ptr)
		{
		result = ptr->StartVibra(); // call method
		
		if(result == KErrNone)
			{
			INFO_PRINTF1(_L("MAudioVibraControl::StartVibra finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MAudioVibraControl::StartVibra failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MAudioVibraControl failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIAudioVibraControl::DoTestStep0068L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("AudioVibraControl - StopVibra"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAudioVibraControlTestDevice};
	#else
	TFourCC testUID('T','0','1','1');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidAudioVibraControl
	MAudioVibraControl* ptr = static_cast <MAudioVibraControl*> (iMMFDevSound->CustomInterface(KUidAudioVibraControl));

	if (ptr)
		{
		result = ptr->StopVibra(); // call method

		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MAudioVibraControl::StopVibra finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MAudioVibraControl::StopVibra failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MAudioVibraControl failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
