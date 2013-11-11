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

#include "teststepcig729decoderconfig.h"


TVerdict CTestStepCIG729DecoderConfig::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0049-HP")) == 0)
		{
		iTestStepResult = DoTestStep0049L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0050-HP")) == 0)
		{
		iTestStepResult = DoTestStep0050L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG729DecoderConfig::DoTestStep0049L()
	{
	iTestStepResult = EFail;
	
	INFO_PRINTF1(_L("G729DecoderIntfc - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG729DecoderConfigTestDevice}; 
	#else
	TFourCC testUID('T','0','0','9');
	#endif
		
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG729DecoderIntfc
	MG729DecoderIntfc* ptr = static_cast <MG729DecoderIntfc*> (iMMFDevSound->CustomInterface(KUidG729DecoderIntfc));

	if (ptr)
		{
		INFO_PRINTF1(_L("MG729DecoderIntfc instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		INFO_PRINTF1(_L("MG729DecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIG729DecoderConfig::DoTestStep0050L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("G729DecoderIntfc - BadLsfNextBuffer"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidG729DecoderConfigTestDevice}; 
	#else
	TFourCC testUID('T','0','0','9');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidG729DecoderIntfc
	MG729DecoderIntfc* ptr = static_cast <MG729DecoderIntfc*> (iMMFDevSound->CustomInterface(KUidG729DecoderIntfc));

	if (ptr)
		{
		result = ptr->BadLsfNextBuffer(); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MG729DecoderIntfc::BadLsfNextBuffer finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MG729DecoderIntfc::BadLsfNextBuffer failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MG729DecoderIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
