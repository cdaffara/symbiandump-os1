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

#include "teststepciaacdecoderconfig.h"


TVerdict CTestStepCIAacDecoderConfig::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0004-HP")) == 0)
		{
		iTestStepResult = DoTestStep0004L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0005-HP")) == 0)
		{
		iTestStepResult = DoTestStep0005L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0093-HP")) == 0)
		{
		iTestStepResult = DoTestStep0093L();
		}
		
	return iTestStepResult;
	}


TVerdict CTestStepCIAacDecoderConfig::DoTestStep0004L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("AacDecoderConfig - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAacDecoderConfigTestDevice}; 
	#else
	TFourCC testUID('T','0','0','8');
	#endif

	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);
	
	if (iTestStepResult != EPass)
		{
		return EInconclusive;
		}
	
	// reset the value as previous test is pass
	iTestStepResult = EFail;  
	
	// KUidCustomInterfaceDevSoundAacConfig
	MAacDecoderConfig* ptr = static_cast <MAacDecoderConfig*> (iMMFDevSound->CustomInterface(KUidAacDecoderConfig));

	if (ptr)
		{
		INFO_PRINTF1(_L("MAacDecoderConfig initialized successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MAacDecoderConfig failed to initialize"));
		}
		
	return iTestStepResult;
	}


TVerdict CTestStepCIAacDecoderConfig::DoTestStep0005L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;	

	INFO_PRINTF1(_L("AacDecoderConfig - SetAudioConfig"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAacDecoderConfigTestDevice}; 
	#else
	TFourCC testUID('T','0','0','8');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		return EInconclusive;
		}
	
	// reset the value as previous test is pass
	iTestStepResult = EFail;  
	
	// KUidCustomInterfaceDevSoundAacConfig
	MAacDecoderConfig* ptr = static_cast <MAacDecoderConfig*> (iMMFDevSound->CustomInterface(KUidAacDecoderConfig));

	if (ptr)
		{
		MAacDecoderConfig::TAudioConfig audioConfig;

		// Set values before call.
		audioConfig.iAudioObjectType = MAacDecoderConfig::TAudioConfig::EAacSsr;
		
		result = ptr->SetAudioConfig(audioConfig); // call method
		
		if (result == KErrNone)
			{
			TInt retAudioConfig;

			// This file is created by the test stub, the plugin device
			_LIT(KFileName, "c:\\temp\\aacDecoderConfig.txt");

			ReadFileL(KFileName, retAudioConfig);

			if (retAudioConfig == audioConfig.iAudioObjectType)
				{
				INFO_PRINTF1(_L("MAacDecoderConfig::SetAudioConfig finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("MAacDecoderConfig::SetAudioConfig failed - expected and received mismatch"));
				}
			}
		}
	else
		{
		INFO_PRINTF1(_L("MAacDecoderConfig failed to initialize"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;	
	}


TVerdict CTestStepCIAacDecoderConfig::DoTestStep0093L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;	

	INFO_PRINTF1(_L("AacDecoderConfig - GetSupportedAudioConfigs"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAacDecoderConfigTestDevice}; 
	#else
	TFourCC testUID('T','0','0','8');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		return EInconclusive;
		}
	
	// reset the value as previous test is pass
	iTestStepResult = EFail;  
	
	// KUidCustomInterfaceDevSoundAacConfig
	MAacDecoderConfig* ptr = static_cast <MAacDecoderConfig*> (iMMFDevSound->CustomInterface(KUidAacDecoderConfig));

	if (ptr)
		{
		RArray<MAacDecoderConfig::TAudioConfig> audioConfig;
		
		result = ptr->GetSupportedAudioConfigs(audioConfig); // call method
		
		if (result == KErrNone)
			{
			if ( (audioConfig[0].iAudioObjectType == MAacDecoderConfig::TAudioConfig::ESbr)		&&
				 (audioConfig[1].iAudioObjectType == MAacDecoderConfig::TAudioConfig::EAacLtp)	&&
				 (audioConfig[2].iAudioObjectType == MAacDecoderConfig::TAudioConfig::EAacSsr)	&&
				 (audioConfig[3].iAudioObjectType == MAacDecoderConfig::TAudioConfig::EAacLc)	&&
				 (audioConfig[4].iAudioObjectType == MAacDecoderConfig::TAudioConfig::EAacMain) )
				{
				INFO_PRINTF1(_L("MAacDecoderConfig::GetSupportedAudioConfigs finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("MAacDecoderConfig::GetSupportedAudioConfigs failed - expected and received mismatch"));
				}
			}
		else
			{
			ERR_PRINTF2(_L("MAacDecoderConfig::GetSupportedAudioConfigs failed with result %d"), result);
			}		
		}
	else
		{
		INFO_PRINTF1(_L("MAacDecoderConfig failed to initialize"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;	
	}
