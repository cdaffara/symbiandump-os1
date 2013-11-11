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

#include "teststepcig711decoderconfig.h"


TVerdict CTestStepCIG711DecoderConfig::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0040-HP")) == 0)
		{
		iTestStepResult = DoTestStep0040L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0041-HP")) == 0)
		{
		iTestStepResult = DoTestStep0041L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0042-HP")) == 0)
		{
		iTestStepResult = DoTestStep0042L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0043-HP")) == 0)
		{
		iTestStepResult = DoTestStep0043L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0044-HP")) == 0)
		{
		iTestStepResult = DoTestStep0044L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0094-HP")) == 0)
		{
		iTestStepResult = DoTestStep0094L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0095-HP")) == 0)
		{
		iTestStepResult = DoTestStep0095L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711DecoderConfig::DoTestStep0040L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("G711DecoderIntfc - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711DecoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','5');
	#endif
		
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);
	
	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG711DecoderIntfc
	MG711DecoderIntfc* ptr = static_cast <MG711DecoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711DecoderIntfc));

	if (ptr)
		{
		INFO_PRINTF1(_L("MG711DecoderIntfc instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MG711DecoderIntfc failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711DecoderConfig::DoTestStep0041L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G711DecoderIntfc - SetDecoderMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711DecoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','5');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG711DecoderIntfc
	MG711DecoderIntfc* ptr = static_cast <MG711DecoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711DecoderIntfc));

	if (ptr)
		{
		result = ptr->SetDecoderMode(MG711DecoderIntfc::EDecULaw); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MG711DecoderIntfc::SetDecoderMode finished successfully"));

			iTestStepResult = EPass;		
			}
		else
			{
			ERR_PRINTF2(_L("MG711DecoderIntfc::SetDecoderMode failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG711DecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711DecoderConfig::DoTestStep0094L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G711DecoderIntfc - GetDecoderMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711DecoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','5');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG711DecoderIntfc
	MG711DecoderIntfc* ptr = static_cast <MG711DecoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711DecoderIntfc));

	if (ptr)
		{
		MG711DecoderIntfc::TDecodeMode decodeMode = MG711DecoderIntfc::EDecULaw;

		result = ptr->SetDecoderMode(MG711DecoderIntfc::EDecALaw);
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MG711DecoderIntfc::SetDecoderMode finished successfully"));

			result = ptr->GetDecoderMode(decodeMode); // call method

			if (MG711DecoderIntfc::EDecALaw == decodeMode)
				{
				INFO_PRINTF1(_L("MG711DecoderIntfc::GetDecoderMode finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF3(_L("MG711DecoderIntfc::GetDecoderMode failed with result %d decodeMode %d"), result, decodeMode);
				}			
			}
		else
			{
			ERR_PRINTF2(_L("MG711DecoderIntfc::SetDecoderMode failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG711DecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711DecoderConfig::DoTestStep0042L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G711DecoderIntfc - SetComfortNoiseGeneration"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711DecoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','5');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG711DecoderIntfc
	MG711DecoderIntfc* ptr = static_cast <MG711DecoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711DecoderIntfc));

	if (ptr)
		{
		TBool cng = ETrue;

		result = ptr->SetComfortNoiseGeneration(cng); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MG711DecoderIntfc::SetComfortNoiseGeneration finished successfully"));
			
			result = ptr->GetComfortNoiseGeneration(cng);
			
			if ( (result == KErrNone) && cng)
				{
				INFO_PRINTF1(_L("MG711DecoderIntfc::GetComfortNoiseGeneration finished successfully"));
				
				result = ptr->SetComfortNoiseGeneration(EFalse);
				
				if (result == KErrNone)
					{
					INFO_PRINTF1(_L("MG711DecoderIntfc::SetComfortNoiseGeneration finished successfully"));
					
					result = ptr->GetComfortNoiseGeneration(cng);
			
					if ( (result == KErrNone) && !cng)
						{
						INFO_PRINTF1(_L("MG711DecoderIntfc::GetComfortNoiseGeneration finished successfully"));

						iTestStepResult = EPass;
						}
					else
						{
						ERR_PRINTF3(_L("MG711DecoderIntfc::GetComfortNoiseGeneration failed with result %d  cng %d"), result, cng);
						}
					}
				else
					{
					ERR_PRINTF2(_L("MG711DecoderIntfc::SetComfortNoiseGeneration failed with result %d"), result);
					}
				}
			else
				{
				ERR_PRINTF3(_L("MG711DecoderIntfc::GetComfortNoiseGeneration failed with  result %d  cng %d"), result, cng);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MG711DecoderIntfc::SetComfortNoiseGeneration failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG711DecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711DecoderConfig::DoTestStep0043L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G711DecoderIntfc - GetComfortNoiseGeneration"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711DecoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','5');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG711DecoderIntfc
	MG711DecoderIntfc* ptr = static_cast <MG711DecoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711DecoderIntfc));

	TBool cng = ETrue;

	if (ptr)
		{
		result = ptr->SetComfortNoiseGeneration(cng); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MG711DecoderIntfc::SetComfortNoiseGeneration finished successfully"));

			result = ptr->GetComfortNoiseGeneration(cng);
			
			if ( (result == KErrNone) && cng)
				{
				INFO_PRINTF1(_L("MG711DecoderIntfc::GetComfortNoiseGeneration finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF3(_L("MG711DecoderIntfc::GetComfortNoiseGeneration failed with result %d cng %d"), result, cng);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MG711DecoderIntfc::GetComfortNoiseGeneration failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG711DecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711DecoderConfig::DoTestStep0044L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G711DecoderIntfc - SetPacketLossConcealment"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711DecoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','5');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG711DecoderIntfc
	MG711DecoderIntfc* ptr = static_cast <MG711DecoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711DecoderIntfc));

	if (ptr)
		{
		TBool plc = ETrue;

		result = ptr->SetPacketLossConcealment(EFalse); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MG711DecoderIntfc::SetPacketLossConcealment finished successfully"));

			result = ptr->GetPacketLossConcealment(plc);

			if ( (result == KErrNone) && (EFalse == plc) )
				{
				INFO_PRINTF1(_L("MG711DecoderIntfc::GetPacketLossConcealment finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF3(_L("MG711DecoderIntfc::GetPacketLossConcealment failed with result %d  plc %d"), result, plc);
				}			
			}
		else
			{
			ERR_PRINTF2(_L("MG711DecoderIntfc::SetPacketLossConcealment failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG711DecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG711DecoderConfig::DoTestStep0095L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G711DecoderIntfc - GetPacketLossConcealment"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG711DecoderConfigTestDevice};
	#else
	TFourCC testUID('T','0','1','5');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG711DecoderIntfc
	MG711DecoderIntfc* ptr = static_cast <MG711DecoderIntfc*> (iMMFDevSound->CustomInterface(KUidG711DecoderIntfc));

	if (ptr)
		{
		TBool plc = ETrue;

		result = ptr->SetPacketLossConcealment(EFalse);
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MG711DecoderIntfc::SetPacketLossConcealment finished successfully"));

			result = ptr->GetPacketLossConcealment(plc); // call method

			if ( (result == KErrNone) && (EFalse == plc) )
				{
				INFO_PRINTF1(_L("MG711DecoderIntfc::GetPacketLossConcealment finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF3(_L("MG711DecoderIntfc::GetPacketLossConcealment failed with result %d  plc %d"), result, plc);
				}			
			}
		else
			{
			ERR_PRINTF2(_L("MG711DecoderIntfc::SetPacketLossConcealment failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG711DecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
