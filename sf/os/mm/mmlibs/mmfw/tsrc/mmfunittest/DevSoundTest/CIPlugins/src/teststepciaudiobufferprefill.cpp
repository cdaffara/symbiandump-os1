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

#include "teststepciaudiobufferprefill.h"

const TInt32 KAfterThreeSeconds = 3000000;		// Three seconds
const TInt32 KEveryNinetySeconds = 90000000;	// Ninety seconds


TVerdict CTestStepCISampleBuffering::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0069-HP")) == 0)
		{
		iTestStepResult = DoTestStep0069L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0070-HP")) == 0)
		{
		iTestStepResult = DoTestStep0070L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0071-HP")) == 0)
		{
		iTestStepResult = DoTestStep0071L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0072-HP")) == 0)
		{
		iTestStepResult = DoTestStep0072L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0073-HP")) == 0)
		{
		iTestStepResult = DoTestStep0073L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCISampleBuffering::DoTestStep0069L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("SampleBuffering - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSampleBufferingTestDevice}; 
	#else
	TFourCC testUID('T','0','1','2');
	#endif

	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidMMMFSampleBuffering
	MMMFSampleBuffering* ptr = static_cast <MMMFSampleBuffering*> (iMMFDevSound->CustomInterface(KUidSampleBuffering));

	if (ptr)
		{
		INFO_PRINTF1(_L("MMMFSampleBuffering instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MMMFSampleBuffering failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCISampleBuffering::DoTestStep0070L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("SampleBuffering - MmsbEnableSampleBufferingBeforePlayback"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSampleBufferingTestDevice}; 
	#else
	TFourCC testUID('T','0','1','2');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidMMMFSampleBuffering
	MMMFSampleBuffering* ptr = static_cast <MMMFSampleBuffering*> (iMMFDevSound->CustomInterface(KUidSampleBuffering));

	if (ptr)
		{
		result = ptr->MmsbEnableSampleBufferingBeforePlayback(); // call method
		
		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MMMFSampleBuffering::MmsbEnableSampleBufferingBeforePlayback finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MMMFSampleBuffering::MmsbEnableSampleBufferingBeforePlayback failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFSampleBuffering failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCISampleBuffering::DoTestStep0071L()
	{
	iTestStepResult = EFail;
	TInt result = KErrGeneral;

	INFO_PRINTF1(_L("SampleBuffering - MmsbDisableSampleBufferingBeforePlayback"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSampleBufferingTestDevice}; 
	#else
	TFourCC testUID('T','0','1','2');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidMMMFSampleBuffering
	MMMFSampleBuffering* ptr = static_cast <MMMFSampleBuffering*> (iMMFDevSound->CustomInterface(KUidSampleBuffering));

	if (ptr)
		{

		result = ptr->MmsbDisableSampleBufferingBeforePlayback(); // call method

		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MMMFSampleBuffering::MmsbDisableSampleBufferingBeforePlayback finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MMMFSampleBuffering::MmsbDisableSampleBufferingBeforePlayback failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFSampleBuffering failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCISampleBuffering::DoTestStep0072L()
	{
	iTestStepResult = EFail;
	TInt err;

	INFO_PRINTF1(_L("SampleBuffering - MmsbNotifyPlayStarted"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSampleBufferingTestDevice}; 
	#else
	TFourCC testUID('T','0','1','2');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);
	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidMMMFSampleBuffering
	MMMFSampleBuffering* ptr = static_cast <MMMFSampleBuffering*> (iMMFDevSound->CustomInterface(KUidSampleBuffering));

	if (ptr)
		{
		CActiveListener* listener = new(ELeave)CActiveListener;

		listener->InitialiseActiveListener();

		ptr->MmsbNotifyPlayStarted(listener->iStatus); // call method

		CActiveScheduler::Start();

		err = listener->iStatus.Int();

		delete listener;

		if (err == KErrNotSupported)
			{
			INFO_PRINTF1(_L("MMMFSampleBuffering::MmsbNotifyPlayStarted finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MMMFSampleBuffering::MmsbNotifyPlayStarted failed with err %d"), err);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFSampleBuffering failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}
	
	return iTestStepResult;
	}


TVerdict CTestStepCISampleBuffering::DoTestStep0073L()
	{
	iTestStepResult = EFail;
	TInt err;

	INFO_PRINTF1(_L("SampleBuffering - MmsbCancelNotifyPlayStarted"));

	TCallBack callback(CTestStepCISampleBuffering::Callback, this);

	iTimer = CPeriodic::NewL(EPriorityNormal);

	// Here we use periodic timer to enable us to call MmsbCancelNotifyPlayStarted() after three
	// seconds of issuing MmsbNotifyPlayStarted(). Ninety seconds is long enough to stop the timer
	// before sending a second MmsbCancelNotifyPlayStarted() .
	iTimer->Start(KAfterThreeSeconds, KEveryNinetySeconds, callback);

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSampleBufferingTestDevice}; 
	#else
	TFourCC testUID('T','0','1','2');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidMMMFSampleBuffering
	iInterfacePtr = static_cast <MMMFSampleBuffering*> (iMMFDevSound->CustomInterface(KUidSampleBuffering));

	if (iInterfacePtr)
		{
		CActiveListener* listener = new(ELeave) CActiveListener;
		listener->InitialiseActiveListener();

		iInterfacePtr->MmsbNotifyPlayStarted(listener->iStatus); // call method

		CActiveScheduler::Start();

		err = listener->iStatus.Int();

		delete listener;
		
		if (err == KErrCancel)
			{
			INFO_PRINTF1(_L("MMMFSampleBuffering::MmsbCancelNotifyPlayStarted Successfully completed"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("Device didn't wait for the expected duration  err %d"), err);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFSampleBuffering failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	delete iTimer;

	return iTestStepResult;
	}


TInt CTestStepCISampleBuffering::Callback(TAny* aAny)
	{
	CTestStepCISampleBuffering* me = reinterpret_cast<CTestStepCISampleBuffering*>(aAny);
	me->DoCallback();
	return KErrNone;
	}

void CTestStepCISampleBuffering::DoCallback()
	{
	iTimer->Cancel();
	iInterfacePtr->MmsbCancelNotifyPlayStarted();	// call method
	}
