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

#include "teststepciilbcencoderconfig.h"


TVerdict CTestStepCIIlbcEncoderConfig::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0058-HP")) == 0)
		{
		iTestStepResult = DoTestStep0058L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0059-HP")) == 0)
		{
		iTestStepResult = DoTestStep0059L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0060-HP")) == 0)
		{
		iTestStepResult = DoTestStep0060L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0061-HP")) == 0)
		{
		iTestStepResult = DoTestStep0061L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0096-HP")) == 0)
		{
		iTestStepResult = DoTestStep0096L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIIlbcEncoderConfig::DoTestStep0058L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("IlbcEncoderIntfc - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidIlbcEncoderIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','8');
	#endif
		
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidCustomInterfaceDevSoundIlbcEncoderConfig
	MIlbcEncoderIntfc* ptr = static_cast <MIlbcEncoderIntfc*> (iMMFDevSound->CustomInterface(KUidIlbcEncoderIntfc));

	if (ptr)
		{
		INFO_PRINTF1(_L("MIlbcEncoderIntfc instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MIlbcEncoderIntfc failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIIlbcEncoderConfig::DoTestStep0059L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("IlbcEncoderIntfc - SetEncoderMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidIlbcEncoderIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','8');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidIlbcEncoderIntfc
	MIlbcEncoderIntfc* ptr = static_cast <MIlbcEncoderIntfc*> (iMMFDevSound->CustomInterface(KUidIlbcEncoderIntfc));

	if (ptr)
		{
		MIlbcEncoderIntfc::TEncodeMode encodeMode = MIlbcEncoderIntfc::E30msFrame;

		result = ptr->SetEncoderMode(MIlbcEncoderIntfc::E20msFrame); // call method

		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MIlbcEncoderIntfc::SetEncoderMode finished successfully"));

			result = ptr->GetEncoderMode(encodeMode);

			if (MIlbcEncoderIntfc::E20msFrame == encodeMode)
				{
				INFO_PRINTF1(_L("MIlbcEncoderIntfc::GetEncoderMode finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF3(_L("MIlbcEncoderIntfc::GetEncoderMode failed with result %d  encodeMode %d"), result, encodeMode);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MIlbcEncoderIntfc::SetEncoderMode failed with result %d"), result);
			}
		}
	else
		{
		ERR_PRINTF1(_L("MIlbcEncoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIIlbcEncoderConfig::DoTestStep0096L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("IlbcEncoderIntfc - GetEncoderMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidIlbcEncoderIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','8');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidIlbcEncoderIntfc
	MIlbcEncoderIntfc* ptr = static_cast <MIlbcEncoderIntfc*> (iMMFDevSound->CustomInterface(KUidIlbcEncoderIntfc));

	if (ptr)
		{
		MIlbcEncoderIntfc::TEncodeMode encodeMode = MIlbcEncoderIntfc::E20msFrame;

		result = ptr->SetEncoderMode(MIlbcEncoderIntfc::E30msFrame);

		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MIlbcEncoderIntfc::SetEncoderMode finished successfully"));

			result = ptr->GetEncoderMode(encodeMode); // call method

			if (MIlbcEncoderIntfc::E30msFrame == encodeMode)
				{
				INFO_PRINTF1(_L("MIlbcEncoderIntfc::GetEncoderMode finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF2(_L("MIlbcEncoderIntfc::GetEncoderMode failed with encodeMode %d"), encodeMode);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MIlbcEncoderIntfc::SetEncoderMode failed with result %d"), result);
			}
		}
	else
		{
		ERR_PRINTF1(_L("MIlbcEncoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIIlbcEncoderConfig::DoTestStep0060L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("IlbcEncoderIntfc - SetVadMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidIlbcEncoderIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','8');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidIlbcEncoderIntfc
	MIlbcEncoderIntfc* ptr = static_cast <MIlbcEncoderIntfc*> (iMMFDevSound->CustomInterface(KUidIlbcEncoderIntfc));

	if (ptr)
		{
		TBool vadModeOn = EFalse;

		result = ptr->SetVadMode(ETrue); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MIlbcEncoderIntfc::SetVadMode finished successfully"));
			
			result = ptr->GetVadMode(vadModeOn);

			if ( (result == KErrNone) && vadModeOn)
				{
				INFO_PRINTF1(_L("MIlbcEncoderIntfc::GetVadMode finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF3(_L("MIlbcEncoderIntfc::GetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
				}
			}
		else
			{
			ERR_PRINTF3(_L("MIlbcEncoderIntfc::SetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);	
			}
		}
	else
		{
		INFO_PRINTF1(_L("MIlbcEncoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIIlbcEncoderConfig::DoTestStep0061L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("IlbcEncoderIntfc - GetVadMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidIlbcEncoderIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','8');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidIlbcEncoderIntfc
	MIlbcEncoderIntfc* ptr = static_cast <MIlbcEncoderIntfc*> (iMMFDevSound->CustomInterface(KUidIlbcEncoderIntfc));

	if (ptr)
		{
		TBool vadModeOn = EFalse;

		result = ptr->SetVadMode(ETrue); // call method

		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MIlbcEncoderIntfc::SetVadMode finished successfully"));

			result = ptr->GetVadMode(vadModeOn);

			if ( (result == KErrNone) && vadModeOn)
				{
				INFO_PRINTF1(_L("MIlbcEncoderIntfc::GetVadMode finished successfully"));

				result = ptr->SetVadMode(EFalse);

				if (result == KErrNone)
					{
					INFO_PRINTF1(_L("MIlbcEncoderIntfc::SetVadMode finished successfully"));

					result = ptr->GetVadMode(vadModeOn);

					if ( (result == KErrNone) && !vadModeOn)
						{
						INFO_PRINTF1(_L("MIlbcEncoderIntfc::GetVadMode finished successfully"));

						iTestStepResult = EPass;
						}
					else
						{
						ERR_PRINTF3(_L("MIlbcEncoderIntfc::GetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
						}
					}
				else
					{
					ERR_PRINTF3(_L("MIlbcEncoderIntfc::SetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
					}
				}
			else
				{
				ERR_PRINTF3(_L("MIlbcEncoderIntfc::GetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
				}
			}
		else
			{
			ERR_PRINTF3(_L("MIlbcEncoderIntfc::SetVadMode failed with result %d  vadModeOn %d"), result, vadModeOn);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MIlbcEncoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
