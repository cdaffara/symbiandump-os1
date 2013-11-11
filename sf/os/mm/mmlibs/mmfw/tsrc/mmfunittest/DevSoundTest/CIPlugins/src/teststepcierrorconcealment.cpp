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
// teststepciErrorConcealmentIntfc.cpp
// 
//

#include "teststepcierrorconcealment.h"


TVerdict CTestStepCIErrorConcealmentIntfc::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0062-HP")) == 0)
		{
		iTestStepResult = DoTestStep0062L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0063-HP")) == 0)
		{
		iTestStepResult = DoTestStep0063L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0064-HP")) == 0)
		{
		iTestStepResult = DoTestStep0064L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0065-HP")) == 0)
		{
		iTestStepResult = DoTestStep0065L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIErrorConcealmentIntfc::DoTestStep0062L()
	{
	iTestStepResult = EFail;
	
	INFO_PRINTF1(_L("ErrorConcealmentIntfc - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidErrorConcealmentIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','9');
	#endif

	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;  

	// KUidCustomInterfaceDevSoundErrorConcealmentIntfc
	MMMFErrorConcealmentIntfc* ptr = static_cast <MMMFErrorConcealmentIntfc*> (iMMFDevSound->CustomInterface(KUidErrorConcealmentIntfc));

	if (ptr)
		{
		INFO_PRINTF1(_L("MMMFErrorConcealmentIntfc successfully retrieved the interface"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MMMFErrorConcealmentIntfc failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIErrorConcealmentIntfc::DoTestStep0063L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;
	
	INFO_PRINTF1(_L("MMMFErrorConcealmentIntfc - ConcealErrorForNextBuffer"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidErrorConcealmentIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','9');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to initialize with the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidErrorConcealmentIntfc
	MMMFErrorConcealmentIntfc* ptr = static_cast <MMMFErrorConcealmentIntfc*> (iMMFDevSound->CustomInterface(KUidErrorConcealmentIntfc));

	if (ptr)
		{
		result = ptr->ConcealErrorForNextBuffer(); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MMMFErrorConcealmentIntfc::ConcealErrorForNextBuffer finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MMMFErrorConcealmentIntfc::ConcealErrorForNextBuffer failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFErrorConcealmentIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIErrorConcealmentIntfc::DoTestStep0064L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("MMMFErrorConcealmentIntfc - SetFrameMode"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidErrorConcealmentIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','9');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidErrorConcealmentIntfc
	MMMFErrorConcealmentIntfc* ptr = static_cast <MMMFErrorConcealmentIntfc*> (iMMFDevSound->CustomInterface(KUidErrorConcealmentIntfc));

	TBool frameModeOn = EFalse;
	TBool retframeModeOn = ETrue;

	if (ptr)
		{
		result = ptr->SetFrameMode(frameModeOn); // call method
		
		if (result == KErrNone)
			{
			// This file is created by the test stub, the plugin device
			_LIT(KFileName, "c:\\temp\\errorConcealment.txt");

			ReadFileL(KFileName, retframeModeOn);

			if (retframeModeOn == frameModeOn)
				{
				INFO_PRINTF1(_L("MMMFErrorConcealmentIntfc::SetFrameMode finished successfully"));

				iTestStepResult = EPass;
				}
			else
				{
				ERR_PRINTF2(_L("MMMFErrorConcealmentIntfc::SetFrameMode failed with frameModeRqrd %d"), retframeModeOn);
				}
			}
		else
			{
			ERR_PRINTF2(_L("MMMFErrorConcealmentIntfc::SetFrameMode failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFErrorConcealmentIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}
		
	return iTestStepResult;
	}


TVerdict CTestStepCIErrorConcealmentIntfc::DoTestStep0065L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("MMMFErrorConcealmentIntfc - FrameModeRqrdForEC"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidErrorConcealmentIntfcTestDevice};
	#else
	TFourCC testUID('T','0','1','9');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiated the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidErrorConcealmentIntfc
	MMMFErrorConcealmentIntfc* ptr = static_cast <MMMFErrorConcealmentIntfc*> (iMMFDevSound->CustomInterface(KUidErrorConcealmentIntfc));

	if (ptr)
		{
		TBool frameModeRqrd = EFalse;
		TInt setFrameModeRqrd = ETrue;

		// This file is created by the test stub, the plugin device
		_LIT(KFileName, "c:\\temp\\errorConcealment.txt");

		// store a value in a file, for the test to read from. 
		WriteFileL(KFileName, setFrameModeRqrd);

		result = ptr->FrameModeRqrdForEC(frameModeRqrd); // call method

		if ( (result == KErrNone) && (setFrameModeRqrd == frameModeRqrd) )
			{
			INFO_PRINTF1(_L("MMMFErrorConcealmentIntfc::FrameModeRqrdForEC finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF3(_L("MMMFErrorConcealmentIntfc::FrameModeRqrdForEC failed with result %d frameModeRqrd %d"), result, frameModeRqrd);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFErrorConcealmentIntfc failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
