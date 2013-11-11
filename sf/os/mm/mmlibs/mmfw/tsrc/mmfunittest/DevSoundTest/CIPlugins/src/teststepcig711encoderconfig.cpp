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

#include "teststepcig711encoderconfig.h"


TVerdict CTestStepCIG711EncoderConfig::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0045-HP")) == 0)
		{
		iTestStepResult = DoTestStep0045L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0046-HP")) == 0)
		{
		iTestStepResult = DoTestStep0046L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0047-HP")) == 0)
		{
		iTestStepResult = DoTestStep0047L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0048-HP")) == 0)
		{
		iTestStepResult = DoTestStep0048L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711EncoderConfig::DoTestStep0045L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("G711EncoderIntfc - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711EncoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','6');
	#endif
		
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG711EncoderIntfc
	MG711EncoderIntfc* ptr = static_cast <MG711EncoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711EncoderIntfc));

	if (ptr)
		{
		INFO_PRINTF1(_L("MG711EncoderIntfc instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MG711EncoderIntfc failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711EncoderConfig::DoTestStep0046L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G711EncoderIntfc - SetEncoderMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711EncoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','6');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidG711EncoderIntfc
	MG711EncoderIntfc* ptr = static_cast <MG711EncoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711EncoderIntfc));

	if (ptr)
		{
		MG711EncoderIntfc::TEncodeMode encodeMode = MG711EncoderIntfc::EEncULaw;
		TInt setEncodeMode = MG711EncoderIntfc::EEncALaw;

		result = ptr->SetEncoderMode(encodeMode) ; // call method
		
		if (result == KErrNone)
			{
			// This file is created by the test stub, the plugin device
			_LIT(KFileName, "c:\\temp\\g711EncoderConfig.txt");

			ReadFileL(KFileName, setEncodeMode);

			if (static_cast<MG711EncoderIntfc::TEncodeMode>(setEncodeMode) == encodeMode)
				{
				INFO_PRINTF1(_L("MG711EncoderIntfc::SetEncoderMode finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF2(_L("MIlbcEncoderIntfc::SetEncoderMode failed with encodeMode %d"), encodeMode);
				}	
			}
		else
			{
			ERR_PRINTF2(_L("MG711DecoderIntfc::SetEncoderMode failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG711DecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711EncoderConfig::DoTestStep0047L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G711EncoderIntfc - SetVadMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711EncoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','6');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidG711EncoderIntfc
	MG711EncoderIntfc* ptr = static_cast <MG711EncoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711EncoderIntfc));

	if (ptr)
		{
		TBool vadModeOn = EFalse;

		result = ptr->SetVadMode(ETrue); // call method

		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MG711EncoderIntfc::SetVadMode finished successfully"));

			result = ptr->GetVadMode(vadModeOn);

			if ( (result == KErrNone) && vadModeOn)
				{
				INFO_PRINTF1(_L("MG711EncoderIntfc::GetVadMode finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF3(_L("MG711EncoderIntfc::GetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
				}	
			}
		else
			{
			ERR_PRINTF2(_L("MG711DecoderIntfc::SetVadMode failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG711EncoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711EncoderConfig::DoTestStep0048L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G711EncoderIntfc - GetVadMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711EncoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','6');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidG711EncoderIntfc
	MG711EncoderIntfc* ptr = static_cast <MG711EncoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711EncoderIntfc));

	if (ptr)
		{
		TBool vadModeOn = EFalse;

		result = ptr->SetVadMode(ETrue); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MG711EncoderIntfc::SetVadMode finished successfully"));

			result = ptr->GetVadMode(vadModeOn);

			if ( (result == KErrNone) && vadModeOn)
				{
				INFO_PRINTF1(_L("MG711EncoderIntfc::GetVadMode finished successfully"));

				result = ptr->SetVadMode(EFalse);
				
				if (result == KErrNone)
					{
					INFO_PRINTF1(_L("MG711EncoderIntfc::SetVadMode finished successfully"));
	
					result = ptr->GetVadMode(vadModeOn);

					if ( (result == KErrNone) && !vadModeOn)
						{
						INFO_PRINTF1(_L("MG711EncoderIntfc::GetVadMode finished successfully"));

						iTestStepResult = EPass;
						}
					else
						{
						ERR_PRINTF3(_L("MG711EncoderIntfc::GetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
						}
					}
				else
					{
					ERR_PRINTF2(_L("MG711EncoderIntfc::SetVadMode failed with result %d"), result);
					}
				}
			else
				{
				ERR_PRINTF3(_L("MG711EncoderIntfc::GetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MG711EncoderIntfc::SetVadMode failed with result %d"), result);	
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG711EncoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
