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

#include "teststepciadvancedaacencodesettings.h"
#include "cidefine.h"

/*****************************************************************************/
CTestStepCICreateAdvancedAacEncodeSettings::CTestStepCICreateAdvancedAacEncodeSettings()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0006-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCICreateAdvancedAacEncodeSettings::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAdvancedAacEncodeSettingsTestDevice};
	#else
	TFourCC testUID('T','0','0','5');
	#endif

	MMMFAdvancedAACEncodeSettings* interface = NULL;
	interface = static_cast<MMMFAdvancedAACEncodeSettings*>(
							InitializeAndInstantiateInterfaceL(testUID, KUidAdvancedAACEncoderSettings));

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
CTestStepCIAdvancedAacEncodeSettings::CTestStepCIAdvancedAacEncodeSettings(TBool aIsGetTest)
									: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{		
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0099-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0007-HP");	
		}
	}

/*****************************************************************************/
TVerdict CTestStepCIAdvancedAacEncodeSettings::DoTestStepL()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("Initializing test CI device"));
	
	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAdvancedAacEncodeSettingsTestDevice};
	#else
	TFourCC testUID('T','0','0','5');
	#endif
	
	MMMFAdvancedAACEncodeSettings* interface = NULL;
	interface = static_cast<MMMFAdvancedAACEncodeSettings*>(
							InitializeAndInstantiateInterfaceL(testUID, KUidAdvancedAACEncoderSettings));

	if (interface)
		{
		MMMFAdvancedAACEncodeSettings::TAACBitrateMode bitrateMode = MMMFAdvancedAACEncodeSettings::EAACBitrateModeFixed;
		INFO_PRINTF1(_L("Calling test CI method"));
		TInt err = interface->SetAACEncodeBitrateMode(bitrateMode);
		
		if (iIsGetTest)
			{
			INFO_PRINTF1(_L("Testing GetAACEncodeBitrateMode"));
			}
		else
			{
			INFO_PRINTF1(_L("Testing SetAACEncodeBitrateMode"));
			}
			
		if (err == KErrNone)
			{
			// Successfully set the mode
			INFO_PRINTF1(_L("Set the mode successfully"));
			
			MMMFAdvancedAACEncodeSettings::TAACBitrateMode testMode;
			
			err = interface->GetAACEncodeBitrateMode(testMode);
			
			iTestStepResult = EFail;
			if ((err == KErrNone) && (testMode == bitrateMode))
				{
				INFO_PRINTF1(_L("Test CI method call was successful"));
				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF4(_L("Test CI method failed, error: %d, expected value: %d, actual value: %d"), \
								err, bitrateMode, testMode);
				}			
			}
		else
			{
			ERR_PRINTF2(_L("Test CI method failed, error"), err);
			}
		}
	else
		{
		WARN_PRINTF1(_L("Failed to retrieve the custom interface."));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
