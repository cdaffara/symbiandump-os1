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

#include "teststepcig729encoderconfig.h"


TVerdict CTestStepCIG729EncoderConfig::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0051-HP")) == 0)
		{
		iTestStepResult = DoTestStep0051L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0052-HP")) == 0)
		{
		iTestStepResult = DoTestStep0052L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0053-HP")) == 0)
		{
		iTestStepResult = DoTestStep0053L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG729EncoderConfig::DoTestStep0051L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("G729EncoderIntfc - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG729EncoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','0');
	#endif

	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG729EncoderIntfc
	MG729EncoderIntfc* ptr = static_cast <MG729EncoderIntfc*> (iMMFDevSound->CustomInterface(KUidG729EncoderIntfc));

	if (ptr)
		{
		INFO_PRINTF1(_L("MG729EncoderIntfc instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MG729EncoderIntfc failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG729EncoderConfig::DoTestStep0052L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G729EncoderIntfc - SetVadMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG729EncoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','0');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidG729EncoderIntfc
	MG729EncoderIntfc* ptr = static_cast <MG729EncoderIntfc*> (iMMFDevSound->CustomInterface(KUidG729EncoderIntfc));

	if (ptr)
		{
		TBool vadModeOn = EFalse;

		result = ptr->SetVadMode(ETrue); // call method
		
		if (result == KErrNone)
			{
			result = ptr->GetVadMode(vadModeOn);

			if ( (result == KErrNone) && vadModeOn)
				{
				INFO_PRINTF1(_L("MG729EncoderIntfc::GetVadMode finished successfully"));
				return EPass;
				}
			else
				{
				ERR_PRINTF3(_L("MG729EncoderIntfc::GetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MG729EncoderIntfc::SetVadMode failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG729EncoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG729EncoderConfig::DoTestStep0053L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G729EncoderIntfc - GetVadMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG729EncoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','0');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidG729EncoderIntfc
	MG729EncoderIntfc* ptr = static_cast <MG729EncoderIntfc*> (iMMFDevSound->CustomInterface(KUidG729EncoderIntfc));

	if (ptr)
		{
		TBool vadModeOn = EFalse;
		
		result = ptr->SetVadMode(ETrue); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MG729EncoderIntfc::SetVadMode finished successfully"));
			
			result = ptr->GetVadMode(vadModeOn);

			if ( (result == KErrNone) && vadModeOn)
				{
				INFO_PRINTF1(_L("MG729EncoderIntfc::GetVadMode finished successfully"));
				
				result = ptr->SetVadMode(EFalse);
				
				if (result == KErrNone)
					{
					INFO_PRINTF1(_L("MG729EncoderIntfc::SetVadMode finished successfully"));
					
					result = ptr->GetVadMode(vadModeOn);

					if ( (result == KErrNone) && !vadModeOn)
						{
						INFO_PRINTF1(_L("MG729EncoderIntfc::GetVadMode finished successfully"));

						iTestStepResult = EPass;
						}
					else
						{
						ERR_PRINTF3(_L("MG729EncoderIntfc::GetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
						}
					}
				else
					{
					ERR_PRINTF2(_L("MG729EncoderIntfc::SetVadMode failed with result %d"), result);
					}
				}
			else
				{
				ERR_PRINTF3(_L("MG729EncoderIntfc::GetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MG729EncoderIntfc::SetVadMode failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG729EncoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
