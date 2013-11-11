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

#include "teststepciunderflowautostopcontrol.h"


TVerdict CTestStepCIUnderflowAutoStopControl::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0091-HP")) == 0)
		{
		iTestStepResult = DoTestStep0091L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0092-HP")) == 0)
		{
		iTestStepResult = DoTestStep0092L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIUnderflowAutoStopControl::DoTestStep0091L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("UnderflowAutoStopControl - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidUnderflowAutoStopControlTestDevice};
	#else
	TFourCC testUID('T','0','2','1');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidunderflowAutoStopControl
	MMMFUnderflowAutoStopControl* ptr = static_cast <MMMFUnderflowAutoStopControl*> (iMMFDevSound->CustomInterface(KUidUnderflowAutoStopControl));

	if (ptr)
		{
		INFO_PRINTF1(_L("MMMFUnderflowAutoStopControl instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MMMFUnderflowAutoStopControl failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIUnderflowAutoStopControl::DoTestStep0092L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("MMMFUnderflowAutoStopControl - SetConversionFormat"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidUnderflowAutoStopControlTestDevice};
	#else
	TFourCC testUID('T','0','2','1');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidUnderflowAutoStopControl
	MMMFUnderflowAutoStopControl* ptr = static_cast <MMMFUnderflowAutoStopControl*> (iMMFDevSound->CustomInterface(KUidUnderflowAutoStopControl));

	if (ptr)
		{
		result = ptr->MmuascTurnOffUnderflowAutoStop(); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MMMFUnderflowAutoStopControl::MmuascTurnOffUnderflowAutoStop finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MMMFUnderflowAutoStopControl::MmuascTurnOffUnderflowAutoStop failed with error %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFUnderflowAutoStopControl failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
