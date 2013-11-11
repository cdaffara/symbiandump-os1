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

#include "teststepcispeechencoderconfig.h"


TVerdict CTestStepCISpeechEncoderConfig::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0001-HP")) == 0)
		{
		iTestStepResult = DoTestStep0001L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0002-HP")) == 0)
		{
		iTestStepResult = DoTestStep0002L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0003-HP")) == 0)
		{
		iTestStepResult = DoTestStep0003L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCISpeechEncoderConfig::DoTestStep0001L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("SpeechEncoderConfig - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSpeechEncoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','4');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidSpeechEncoderConfig
	MSpeechEncoderConfig* ptr = static_cast <MSpeechEncoderConfig*> (iMMFDevSound->CustomInterface(KUidSpeechEncoderConfig));

	if (ptr)
		{
		INFO_PRINTF1(_L("MSpeechEncoderConfig instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MSpeechEncoderConfig failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCISpeechEncoderConfig::DoTestStep0002L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("SpeechEncoderConfig - SetVadMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSpeechEncoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','4');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidSpeechEncoderConfig
	MSpeechEncoderConfig* ptr = static_cast <MSpeechEncoderConfig*> (iMMFDevSound->CustomInterface(KUidSpeechEncoderConfig));

	if (ptr)
		{
		TBool vadModeOn = EFalse;

		result = ptr->SetVadMode(vadModeOn); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MSpeechEncoderConfig::SetVadMode finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MSpeechEncoderConfig::SetVadMode failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MSpeechEncoderConfig failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCISpeechEncoderConfig::DoTestStep0003L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("SpeechEncoderConfig - GetVadMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSpeechEncoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','4');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidSpeechEncoderConfig
	MSpeechEncoderConfig* ptr = static_cast <MSpeechEncoderConfig*> (iMMFDevSound->CustomInterface(KUidSpeechEncoderConfig));

	if (ptr)
		{
		TBool vadModeOn = ETrue;

		result = ptr->SetVadMode(vadModeOn); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MSpeechEncoderConfig::SetVadMode finished successfully"));

			result = ptr->GetVadMode(vadModeOn);
			
			if ( (result == KErrNone) && vadModeOn )
				{
				INFO_PRINTF1(_L("MSpeechEncoderConfig::GetVadMode finished successfully"));

				iTestStepResult = EPass;
				}
			else if (result != KErrNone)
				{
				INFO_PRINTF2(_L("MSpeechEncoderConfig::GetVadMode failed with error %d"), result);
				}
			else
				{
				ERR_PRINTF1(_L("MSpeechEncoderConfig::GetVadMode failed - expected and received mismatch"));
				}
			}
		else
			{
			ERR_PRINTF2(_L("MSpeechEncoderConfig::SetVadMode failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MSpeechEncoderConfig failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
