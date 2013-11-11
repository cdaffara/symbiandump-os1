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

#include "teststepcigsmconfig.h"


TVerdict CTestStepCIGsmConfig::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0088-HP")) == 0)
		{
		iTestStepResult = DoTestStep0088L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0089-HP")) == 0)
		{
		iTestStepResult = DoTestStep0089L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0090-HP")) == 0)
		{
		iTestStepResult = DoTestStep0090L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIGsmConfig::DoTestStep0088L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("GsmConfig - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidGsmConfigTestDevice};
	#else
	TFourCC testUID('T','0','2','0');
	#endif
		
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidGsmConfig
	MMMFGsmConfig* ptr = static_cast <MMMFGsmConfig*> (iMMFDevSound->CustomInterface(KUidGsmConfig));

	if (ptr)
		{
		INFO_PRINTF1(_L("MMMFGsmConfig instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MMMFGsmConfig failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIGsmConfig::DoTestStep0089L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("GsmConfig - SetConversionFormat"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidGsmConfigTestDevice};
	#else
	TFourCC testUID('T','0','2','0');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidGsmConfig
	MMMFGsmConfig* ptr = static_cast <MMMFGsmConfig*> (iMMFDevSound->CustomInterface(KUidGsmConfig));

	if (ptr)
		{
		result = ptr->SetConversionFormat(MMMFGsmConfig::ENone); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MMMFGsmConfig::SetConversionFormat finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MMMFGsmConfig::SetConversionFormat failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFGsmConfig failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIGsmConfig::DoTestStep0090L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("GsmConfig - ConversionFormat"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidGsmConfigTestDevice};
	#else
	TFourCC testUID('T','0','2','0');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidGsmConfig
	MMMFGsmConfig* ptr = static_cast <MMMFGsmConfig*> (iMMFDevSound->CustomInterface(KUidGsmConfig));
		
	if (ptr)
		{
		MMMFGsmConfig::TMMFGsmConversionFormat convFormat = MMMFGsmConfig::ENone;

		result = ptr->SetConversionFormat(MMMFGsmConfig::EAlawPCM); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MMMFGsmConfig::SetConversionFormat finished successfully"));

			result = ptr->ConversionFormat(convFormat);
			
			if ( (result == KErrNone) && (MMMFGsmConfig::EAlawPCM == convFormat) )
				{
				INFO_PRINTF1(_L("MMMFGsmConfig::ConversionFormat finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF3(_L("MMMFGsmConfig::SetConversionFormat failed with result %d  convFormat"), result, convFormat);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MMMFGsmConfig::SetConversionFormat failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFGsmConfig failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
