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

#include "teststepciilbcdecoderconfig.h"


TVerdict CTestStepCIIlbcDecoderConfig::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0054-HP")) == 0)
		{
		iTestStepResult = DoTestStep0054L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0055-HP")) == 0)
		{
		iTestStepResult = DoTestStep0055L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0056-HP")) == 0)
		{
		iTestStepResult = DoTestStep0056L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0057-HP")) == 0)
		{
		iTestStepResult = DoTestStep0057L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIIlbcDecoderConfig::DoTestStep0054L()
	{
	iTestStepResult = EFail;
	
	INFO_PRINTF1(_L("IlbcDecoderIntfc - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidIlbcDecoderIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','7');
	#endif

	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidIlbcDecoderIntfc
	MIlbcDecoderIntfc* ptr = static_cast <MIlbcDecoderIntfc*> (iMMFDevSound->CustomInterface(KUidIlbcDecoderIntfc));

	if (ptr)
		{
		INFO_PRINTF1(_L("MIlbcDecoderIntfc instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MIlbcDecoderIntfc failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIIlbcDecoderConfig::DoTestStep0055L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("IlbcDecoderIntfc - SetDecoderMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidIlbcDecoderIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','7');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidIlbcDecoderIntfc
	MIlbcDecoderIntfc* ptr = static_cast <MIlbcDecoderIntfc*> (iMMFDevSound->CustomInterface(KUidIlbcDecoderIntfc));

	if (ptr)
		{
		MIlbcDecoderIntfc::TDecodeMode decodeMode = MIlbcDecoderIntfc::E30msFrame;
		TInt setDecodeMode = MIlbcDecoderIntfc::E20msFrame;

		result = ptr->SetDecoderMode(decodeMode); // call method

		// This file is created by the test stub, the plugin device
		_LIT(KFileName, "c:\\temp\\IlbcDecoderConfig.txt");

		ReadFileL(KFileName, setDecodeMode);

		if (result == KErrNone)
			{
			if (static_cast<MIlbcDecoderIntfc::TDecodeMode>(setDecodeMode) == decodeMode)
				{
				INFO_PRINTF1(_L("MIlbcDecoderIntfc::SetDecoderMode finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF2(_L("MIlbcDecoderIntfc::SetDecoderMode failed with decodeMode %d"), decodeMode);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MIlbcDecoderIntfc::SetDecoderMode failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MIlbcDecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIIlbcDecoderConfig::DoTestStep0056L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("IlbcDecoderIntfc - SetComfortNoiseGeneration"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidIlbcDecoderIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','7');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidIlbcDecoderIntfc
	MIlbcDecoderIntfc* ptr = static_cast <MIlbcDecoderIntfc*> (iMMFDevSound->CustomInterface(KUidIlbcDecoderIntfc));

	if (ptr)
		{
		TBool cng = ETrue;

		result = ptr->SetComfortNoiseGeneration(cng); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MIlbcDecoderIntfc::SetComfortNoiseGeneration finished successfully"));

			result = ptr->GetComfortNoiseGeneration(cng);

			if ( (result == KErrNone) && cng)
				{
				INFO_PRINTF1(_L("MIlbcDecoderIntfc::GetComfortNoiseGeneration finished successfully"));

				result = ptr->SetComfortNoiseGeneration(EFalse);

				if (result == KErrNone)
					{
					INFO_PRINTF1(_L("MIlbcDecoderIntfc::SetComfortNoiseGeneration finished successfully"));

					result = ptr->GetComfortNoiseGeneration(cng);

					if ( (result == KErrNone) && !cng)
						{
						INFO_PRINTF1(_L("MIlbcDecoderIntfc::GetComfortNoiseGeneration finished successfully"));

						iTestStepResult = EPass;
						}
					else
						{
						ERR_PRINTF3(_L("MIlbcDecoderIntfc::GetComfortNoiseGeneration failed with result %d  cng %d"), result, cng);
						}
					}
				else
					{
					ERR_PRINTF2(_L("MIlbcDecoderIntfc::SetComfortNoiseGeneration failed with result %d"), result);
					}
				}
			else
				{
				ERR_PRINTF3(_L("MIlbcDecoderIntfc::GetComfortNoiseGeneration failed with result %d  cng %d"), result, cng);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MIlbcDecoderIntfc::SetComfortNoiseGeneration failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MIlbcDecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIIlbcDecoderConfig::DoTestStep0057L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("IlbcDecoderIntfc - GetComfortNoiseGeneration"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidIlbcDecoderIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','7');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidIlbcDecoderIntfc
	MIlbcDecoderIntfc* ptr = static_cast <MIlbcDecoderIntfc*> (iMMFDevSound->CustomInterface(KUidIlbcDecoderIntfc));

	if (ptr)
		{
		TBool cng = ETrue;

		result = ptr->SetComfortNoiseGeneration(cng); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MIlbcDecoderIntfc::SetCng finished successfully"));
			
			result = ptr->GetComfortNoiseGeneration(cng);

			if ( (result == KErrNone) && cng)
				{
				INFO_PRINTF1(_L("MIlbcDecoderIntfc::GetComfortNoiseGeneration finished successfully"));
				
				result = ptr->SetComfortNoiseGeneration(EFalse);
				
				if (result == KErrNone)
					{
					INFO_PRINTF1(_L("MIlbcDecoderIntfc::SetComfortNoiseGeneration finished successfully"));
					
					result = ptr->GetComfortNoiseGeneration(cng);

					if ( (result == KErrNone) && !cng)
						{
						INFO_PRINTF1(_L("MIlbcDecoderIntfc::GetComfortNoiseGeneration finished successfully"));
						
						iTestStepResult = EPass;
						}
					else
						{
						ERR_PRINTF3(_L("MIlbcDecoderIntfc::GetComfortNoiseGeneration failed with result %d  cng %d"), result, cng);
						}
					}
				else
					{
					ERR_PRINTF2(_L("MIlbcDecoderIntfc::SetComfortNoiseGeneration failed with result %d"), result);
					}
				}
			else
				{
				ERR_PRINTF3(_L("MIlbcDecoderIntfc::GetCng failed with result %d  cng %d"), result, cng);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MIlbcDecoderIntfc::SetCng failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MIlbcDecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
