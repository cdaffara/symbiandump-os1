// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CPP file: Audio Resource Notification tests for Audio Recorder.
// 
//

/**
 @file TestRecorderARN.cpp
*/

#include "TestAudioClientUtilitiesARN.h"

//------------------------------------------------------------------------------
//********************PREQ 797 - Audio Resource Notification********************
//------------------------------------------------------------------------------

//
//CTestMdaAudioRecorderCallbackSupport
//

/**
 * NewL
 */
CTestMdaAudioRecorderCallbackSupport* CTestMdaAudioRecorderCallbackSupport::NewL(MTestAudioRecorderCallback& aCallback)
	{
	return new(ELeave) CTestMdaAudioRecorderCallbackSupport(aCallback);
	}

/**
 * MoscoStateChangeEvent
 */
void CTestMdaAudioRecorderCallbackSupport::MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode)
	{
	iCallback.MoscoStateChangeEventTest(aObject, aPreviousState, aCurrentState, aErrorCode, *this);
	}

/**
 * MarncResourceAvailable
 */
void CTestMdaAudioRecorderCallbackSupport::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	iCallback.MarncResourceAvailableTest(aNotificationEventId, aNotificationData, *this);
	}

/**
 * Constructor
 */
CTestMdaAudioRecorderCallbackSupport::CTestMdaAudioRecorderCallbackSupport(MTestAudioRecorderCallback& aCallback):iCallback(aCallback)
	{

	}

//
//CTestMmfAclntRecordARN
//

/**
 * Constructor
 */
CTestMmfAclntRecordARN::CTestMmfAclntRecordARN(const TDesC& aTestName, const TDesC& aSectName)
	{
	// store the name of this test case
	iTestStepName=aTestName;
	iHeapSize = 3600000;
	iSectName = aSectName;
	}

/**
 * MarncResourceAvailableTest
 */
void CTestMmfAclntRecordARN::MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioRecorderCallbackSupport& aMdaAudioRecorderCallbackSupport)
	{
	if (aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		INFO_PRINTF1(_L("iAudioRecorder::MMMFAudioResourceNotificationCallback -> KMMFEventCategoryAudioResourceAvailable"));

		//Convert TDesc8 to TInt64
		TMMFTimeIntervalMicroSecondsPckg timeIntvbuf;
		timeIntvbuf.Copy(aNotificationData);
		TTimeIntervalMicroSeconds intPos = timeIntvbuf();
		iPosition = intPos;
		if (&aMdaAudioRecorderCallbackSupport == iMdaAudioRecorderCallbackSupport1)
			{
			INFO_PRINTF1(_L("iTestAudioRecorder1 -> MMMFAudioResourceNotificationCallback NotificationReceived"));
			TRAP(iError, FsmL(EMarncResourceAvailableRec1, iTestAudioRecorder1->State()));
			}
		else if (&aMdaAudioRecorderCallbackSupport == iMdaAudioRecorderCallbackSupport2)
			{
			INFO_PRINTF1(_L("iTestAudioRecorder2 -> MMMFAudioResourceNotificationCallback NotificationReceived"));
			TRAP(iError, FsmL(EMarncResourceAvailableRec2, iTestAudioRecorder2->State()));
			}
		else if (&aMdaAudioRecorderCallbackSupport == iMdaAudioRecorderCallbackSupport3)
			{
			INFO_PRINTF1(_L("iTestAudioRecorder3 -> MMMFAudioResourceNotificationCallback NotificationReceived"));
			TRAP(iError, FsmL(EMarncResourceAvailableRec3, iTestAudioRecorder3->State()));
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFAudioResourceNotificationCallback, but NOT KMMFEventCategoryAudioResourceAvailable"));
		}
	}

/**
 * MoscoStateChangeEventTest
 */
void CTestMmfAclntRecordARN::MoscoStateChangeEventTest(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode, CTestMdaAudioRecorderCallbackSupport& aMdaAudioRecorderCallbackSupport)
	{
	if (&aMdaAudioRecorderCallbackSupport == iMdaAudioRecorderCallbackSupport1)
		{
		INFO_PRINTF1(_L("iTestAudioRecorder1 -> MMdaObjectStateChangeObserver StateChanged"));
		INFO_PRINTF4(_L("iTestAudioRecorder1 -> aErrorCode : %d, aPreviousState : %d, aCurrentState : %d"), aErrorCode, aPreviousState, aCurrentState);
		TRAP(iError, FsmL(EMoscoStateChange1, iTestAudioRecorder1->State(), aErrorCode));
		}
	else if (&aMdaAudioRecorderCallbackSupport == iMdaAudioRecorderCallbackSupport2)
		{
		INFO_PRINTF1(_L("iTestAudioRecorder2 -> MMdaObjectStateChangeObserver StateChanged"));
		INFO_PRINTF4(_L("iTestAudioRecorder2 -> aErrorCode : %d, aPreviousState : %d, aCurrentState : %d"), aErrorCode, aPreviousState, aCurrentState);
		TRAP(iError, FsmL(EMoscoStateChange2, iTestAudioRecorder2->State(), aErrorCode));
		}
	else if (&aMdaAudioRecorderCallbackSupport == iMdaAudioRecorderCallbackSupport3)
		{
		INFO_PRINTF1(_L("iTestAudioRecorder3 -> MMdaObjectStateChangeObserver StateChanged"));
		INFO_PRINTF4(_L("iTestAudioRecorder3 -> aErrorCode : %d, aPreviousState : %d, aCurrentState : %d"), aErrorCode, aPreviousState, aCurrentState);
		TRAP(iError, FsmL(EMoscoStateChange3, iTestAudioRecorder3->State(), aErrorCode));
		}
	}

/**
 * DoTestStepPreambleL
 */
TVerdict CTestMmfAclntRecordARN::DoTestStepPreambleL()
	{
	TPtrC filename;
	TVerdict ret = EPass;
	
	// Call base DoTestStepPreambleL()
	ret = CTestMmfAclntStep::DoTestStepPreambleL();
	if (ret == EFail)
		{
		return ret;
		}
	
	// Get 1st file name
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile1"), filename))
		{
		return EInconclusive;
		}
	GetDriveName(iFilename1);
	iFilename1.Append(filename);

	// Initialise CMdaAudioRecorderUtility#1
	INFO_PRINTF1(_L("Initialise CMdaAudioRecorderUtility1 : EMdaPriorityNormal"));
	iMdaAudioRecorderCallbackSupport1 = CTestMdaAudioRecorderCallbackSupport::NewL(*this);
	iTestAudioRecorder1 = CMdaAudioRecorderUtility::NewL(*iMdaAudioRecorderCallbackSupport1, NULL, EMdaPriorityNormal);
	
	
	// Get 2nd file name
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile2"), filename))
		{
		return EInconclusive;
		}
	GetDriveName(iFilename2);
	iFilename2.Append(filename);

	// Initialise CMdaAudioRecorderUtility#2
	INFO_PRINTF1(_L("Initialise CMdaAudioRecorderUtility2 : EMdaPriorityMax"));
	iMdaAudioRecorderCallbackSupport2 = CTestMdaAudioRecorderCallbackSupport::NewL(*this);
	iTestAudioRecorder2 = CMdaAudioRecorderUtility::NewL(*iMdaAudioRecorderCallbackSupport2, NULL, EMdaPriorityMax);
	
	// Get 3rd file name
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile3"), filename))
		{
		return EInconclusive;
		}
	GetDriveName(iFilename3);
	iFilename3.Append(filename);

	// Initialise CMdaAudioRecorderUtility#3
	INFO_PRINTF1(_L("Initialise CMdaAudioRecorderUtility3 : EMdaPriorityMin"));
	iMdaAudioRecorderCallbackSupport3 = CTestMdaAudioRecorderCallbackSupport::NewL(*this);
	iTestAudioRecorder3 = CMdaAudioRecorderUtility::NewL(*iMdaAudioRecorderCallbackSupport3, NULL, EMdaPriorityMin);
	
	// Initialise the CCallBackTimer.
	iCallBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	
	return ret;
	}


/**
 * DoTestStepPostambleL
 */
TVerdict CTestMmfAclntRecordARN::DoTestStepPostambleL()
	{
	// Delete all the iTestAudioRecorder utilities.
	iTestAudioRecorder1->Close();
	delete iTestAudioRecorder1;
	iTestAudioRecorder1=NULL;
	iTestAudioRecorder2->Close();
	delete iTestAudioRecorder2;
	iTestAudioRecorder2=NULL;
	iTestAudioRecorder3->Close();
	delete iTestAudioRecorder3;
	iTestAudioRecorder3=NULL;
	// Delete CTestMdaAudioRecorderCallbackSupport
	delete iMdaAudioRecorderCallbackSupport1;
	delete iMdaAudioRecorderCallbackSupport2;
	delete iMdaAudioRecorderCallbackSupport3;
	// Delete the CCallBackTimer.
	delete iCallBackTimer;
	iCallBackTimer=NULL;
	// Base Postamble.
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * DoTestStepL
 */
TVerdict CTestMmfAclntRecordARN::DoTestStepL()
	{
	return( DoTestL() );
	}

/**
 * StateInit
 */
void CTestMmfAclntRecordARN::StateInit()
	{
	iExpectedEvent=EIdleRecorder;
	iExpectedRecorderState=CMdaAudioClipUtility::ENotReady;
	iExpectedError=KErrNone;
	iState=0;
	}

/**
 * StateChange
 */
void CTestMmfAclntRecordARN::StateChange(TMmfAudioRecorderEvents aExpectedEvent, CMdaAudioClipUtility::TState aExpectedRecorderState, TInt aExpectedError)
	{
	iExpectedEvent=aExpectedEvent;
	iExpectedRecorderState=aExpectedRecorderState;
	iExpectedError=aExpectedError;
	iState++;
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntRecordARN::FsmCheck(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if ((iExpectedError !=KErrUnderflow) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
		ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedEvent, aEventCode);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else if (iExpectedError != aError)
		{
		ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, aError);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else if (iExpectedRecorderState != aRecorderState)
		{
		ERR_PRINTF3(_L("ExpectedRecorderState : %d, ActualRecorderState : %d"), iExpectedRecorderState, aRecorderState);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else
		{
		retFsmCheck = ETrue;
		}
	return retFsmCheck;
	}
	
//
//POSITIVE TESTS//
//

//
//CTestMmfAclntARN8011
//

/**
 * Constructor
 */
CTestMmfAclntARN8011::CTestMmfAclntARN8011(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntRecordARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8011* CTestMmfAclntARN8011::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8011* self = new (ELeave) CTestMmfAclntARN8011(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8011::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8011::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	if (FsmCheck(aEventCode, aRecorderState, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioRecorder#1
				iTestAudioRecorder1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioRecorder1->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 1:
				// Register iTestAudioRecorder#1 for Notification
				retErr = iTestAudioRecorder1->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioRecorder#2
				iTestAudioRecorder2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioRecorder2->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EOpen);
				break;
			case 2:
				// Play iTestAudioRecorder#2
				iTestAudioRecorder2->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder2->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EPlaying);
				break;
			case 3:
				// Wait for Completion of Play - iTestAudioRecorder#2
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EOpen);
				break;
			case 4:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				iTestStepResult = EPass;
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableRec1,CMdaAudioClipUtility::EOpen);
				break;
			case 5:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioRecorder1->NotificationReceived"));
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8011::PerformTestL()
	{
	INFO_PRINTF1(_L("Register for notification, but make no request to play audio"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8012
//

/**
 * Constructor
 */
CTestMmfAclntARN8012::CTestMmfAclntARN8012(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntRecordARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8012* CTestMmfAclntARN8012::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8012* self = new (ELeave) CTestMmfAclntARN8012(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8012::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8012::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	if (FsmCheck(aEventCode, aRecorderState, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioRecorder#1
				iTestAudioRecorder1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioRecorder1->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 1:
				// Register iTestAudioRecorder#1 for Notification
				retErr = iTestAudioRecorder1->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioRecorder#2
				iTestAudioRecorder2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioRecorder2->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EOpen);
				break;
			case 2:
				// Play iTestAudioRecorder#2
				iTestAudioRecorder2->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder2->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EPlaying);
				break;
			case 3:
				// Play iTestAudioRecorder#1 (get Play Error as iTestAudioRecorder#2 is still playing)
				iTestAudioRecorder1->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder1->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EPlaying);
				break;
			case 4:
				// Get Play Error for iTestAudioRecorder#1
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen,KErrInUse);
				break;
			case 5:
				// Stop & Close iTestAudioRecorder#2
				WaitWithTimeout(iRequestStatus,1000000);
				iTestAudioRecorder2->Stop();
				INFO_PRINTF1(_L("iTestAudioRecorder2->Stop()"));
				iTestAudioRecorder2->Close();
				INFO_PRINTF1(_L("iTestAudioRecorder2->Close()"));
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableRec1,CMdaAudioClipUtility::EOpen);
				break;
			case 6:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioRecorder1->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8012::PerformTestL()
	{
	INFO_PRINTF1(_L("Register (b4 play, after Open) and Receive Notification of a Resource Available Event."));
	INFO_PRINTF1(_L("(when audio resource is currently unavailable and then available)"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8013
//

/**
 * Constructor
 */
CTestMmfAclntARN8013::CTestMmfAclntARN8013(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntRecordARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8013* CTestMmfAclntARN8013::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8013* self = new (ELeave) CTestMmfAclntARN8013(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8013::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8013::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	if (FsmCheck(aEventCode, aRecorderState, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioRecorder#1
				iTestAudioRecorder1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioRecorder1->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 1:
				// Play iTestAudioRecorder#1
				iTestAudioRecorder1->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder1->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EPlaying);
				break;
			case 2:
				// Register iTestAudioRecorder#1 for Notification
				retErr = iTestAudioRecorder1->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioRecorder#2
				iTestAudioRecorder2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioRecorder2->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EOpen);
				break;
			case 3:
				// Play iTestAudioRecorder#2
				iTestAudioRecorder2->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder2->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EPlaying);
				break;
			case 4:
				// Get Play Error for iTestAudioRecorder#1
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen,KErrInUse);
				break;
			case 5:
				// Stop & Close iTestAudioRecorder#2
				iTestAudioRecorder2->Stop();
				INFO_PRINTF1(_L("iTestAudioRecorder2->Stop()"));
				iTestAudioRecorder2->Close();
				INFO_PRINTF1(_L("iTestAudioRecorder2->Close()"));
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableRec1,CMdaAudioClipUtility::EOpen);
				break;
			case 6:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioRecorder1->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8013::PerformTestL()
	{
	INFO_PRINTF1(_L("Register (at the time of Play) and Receive Notification of a Resource Available Event."));
	INFO_PRINTF1(_L("(when audio resource is currently available and subsequently unavailable, and then available)"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8014
//

/**
 * Constructor
 */
CTestMmfAclntARN8014::CTestMmfAclntARN8014(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntRecordARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8014* CTestMmfAclntARN8014::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8014* self = new (ELeave) CTestMmfAclntARN8014(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8014::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8014::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	if (FsmCheck(aEventCode, aRecorderState, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioRecorder#1
				iTestAudioRecorder1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioRecorder1->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 1:
				// Play iTestAudioRecorder#1
				iTestAudioRecorder1->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder1->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EPlaying);
				break;
			case 2:
				// Open iTestAudioRecorder#2
				iTestAudioRecorder2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioRecorder2->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EOpen);
				break;
			case 3:
				// Play iTestAudioRecorder#2
				iTestAudioRecorder2->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder2->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EPlaying);
				break;
			case 4:
				// Get Play Error for iTestAudioRecorder#1
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen,KErrInUse);
				break;
			case 5:
				// Cancel notification for iTestAudioRecorder#1
				retErr = iTestAudioRecorder1->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder1->CancelRegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->CancelRegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Wait for Completion of Play - iTestAudioRecorder#2
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EOpen);
				break;
			case 6:
				// Stop & Close iTestAudioRecorder#2
				iTestAudioRecorder2->Stop();
				INFO_PRINTF1(_L("iTestAudioRecorder2->Stop()"));
				iTestAudioRecorder2->Close();
				INFO_PRINTF1(_L("iTestAudioRecorder2->Close()"));
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				iTestStepResult = EPass;
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableRec1,CMdaAudioClipUtility::EOpen);
				break;
			case 7:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioRecorder1->NotificationReceived"));
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8014::PerformTestL()
	{
	INFO_PRINTF1(_L("Registration (b4 File Open) and Cancellation of  the notification request"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Register iTestAudioRecorder#1 for Notification
	TInt retErr = iTestAudioRecorder1->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification()"));
	if( retErr != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioRecorder1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
		return EFail;
		}
		
	// Begin the process
	FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8015
//

/**
 * Constructor
 */
CTestMmfAclntARN8015::CTestMmfAclntARN8015(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntRecordARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8015* CTestMmfAclntARN8015::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8015* self = new (ELeave) CTestMmfAclntARN8015(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8015::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN8015::FsmCheck(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if ((iExpectedError !=KErrUnderflow) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
		if ((aEventCode == EMoscoStateChange2))
	        {
	        retFsmCheck = ETrue;
	        if (iEventOccured == EMarncResourceAvailableRec1)
	            {
	            return retFsmCheck;
	            }
            else
                {
                iEventOccured = EMoscoStateChange2;
                }
	        }
        else if (aEventCode == EMarncResourceAvailableRec1)
            {
            retFsmCheck = ETrue;
            if (iEventOccured == EMoscoStateChange2)
                {
                return retFsmCheck;
                }
            else
                {
                iEventOccured = EMarncResourceAvailableRec1;
                }
            }
        else
            {
    		ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedEvent, aEventCode);
    		iTestStepResult = EFail;
    		CActiveScheduler::Stop();
            }
		}
	else if (iExpectedError != aError)
		{
		ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, aError);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else if (iExpectedRecorderState != aRecorderState)
		{
		ERR_PRINTF3(_L("ExpectedRecorderState : %d, ActualRecorderState : %d"), iExpectedRecorderState, aRecorderState);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else
		{
		retFsmCheck = ETrue;
		}
	return retFsmCheck;
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8015::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	if (FsmCheck(aEventCode, aRecorderState, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioRecorder#1
				iTestAudioRecorder1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioRecorder1->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 1:
				// Play iTestAudioRecorder#1
				iTestAudioRecorder1->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder1->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EPlaying);
				break;
			case 2:
				// Register iTestAudioRecorder#1 for Notification
				retErr = iTestAudioRecorder1->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioRecorder#2
				iTestAudioRecorder2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioRecorder2->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EOpen);
				break;
			case 3:
				// Play iTestAudioRecorder#2
				iTestAudioRecorder2->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder2->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EPlaying);
				break;
			case 4:
				// Get Play Error for iTestAudioRecorder#1
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen,KErrInUse);
				break;
			case 5:
				//Get Position of iTestAudioRecorder#1
				iPositionStop = iTestAudioRecorder1->Position();
				// Stop & Close iTestAudioRecorder#2
				iTestAudioRecorder2->Stop();
				INFO_PRINTF1(_L("iTestAudioRecorder2->Stop()"));
				iTestAudioRecorder2->Close();
				INFO_PRINTF1(_L("iTestAudioRecorder2->Close()"));
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableRec1,CMdaAudioClipUtility::EOpen);
				break;
			case 6:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioRecorder1->NotificationReceived"));
				iPositionResume = iPosition;
				CActiveScheduler::Stop();
				// Check for the Position
				if (iPositionResume >= iPositionStop)
					{
					INFO_PRINTF3(_L("iTestAudioRecorder1->Audio Resource iPositionResume >= iPositionStop, Expected=%Ld, Actual=%Ld"), iPositionStop.Int64(), iPositionResume.Int64());
					iTestStepResult=EPass;
					}
				else
					{
					ERR_PRINTF3(_L("iTestAudioRecorder1->Audio Resource Position is  NOT iPositionResume >= iPositionStop, Expected=%Ld, Actual=%Ld"), iPositionStop.Int64(), iPositionResume.Int64());
					iTestStepResult=EFail;
					}
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8015::PerformTestL()
	{
	INFO_PRINTF1( _L("Get the actual position from where the paused audio can be resumed."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8016
//

/**
 * Constructor
 */
CTestMmfAclntARN8016::CTestMmfAclntARN8016(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntRecordARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8016* CTestMmfAclntARN8016::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8016* self = new (ELeave) CTestMmfAclntARN8016(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8016::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN8016::FsmCheck(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if ((iExpectedError !=KErrUnderflow) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
		if ((aEventCode == EMoscoStateChange1))
	        {
	        retFsmCheck = ETrue;
	        if (iEventOccured == EMarncResourceAvailableRec3)
	            {
	            return retFsmCheck;
	            }
            else
                {
                iEventOccured = EMoscoStateChange1;
                }
	        }
        else if (aEventCode == EMarncResourceAvailableRec3)
            {
            retFsmCheck = ETrue;
            if (iEventOccured == EMoscoStateChange1)
                {
                return retFsmCheck;
                }
            else
                {
                iEventOccured = EMarncResourceAvailableRec3;
                }
            }
        else
            {
    		ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedEvent, aEventCode);
    		iTestStepResult = EFail;
    		CActiveScheduler::Stop();
            }
		}
	else if (iExpectedError != aError)
		{
		ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, aError);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else if (iExpectedRecorderState != aRecorderState)
		{
		ERR_PRINTF3(_L("ExpectedRecorderState : %d, ActualRecorderState : %d"), iExpectedRecorderState, aRecorderState);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else
		{
		retFsmCheck = ETrue;
		}
	return retFsmCheck;
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8016::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	if (FsmCheck(aEventCode, aRecorderState, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioRecorder#1
				iTestAudioRecorder1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioRecorder1->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 1:
				// Play iTestAudioRecorder#1
				iTestAudioRecorder1->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder1->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EPlaying);
				break;
			case 2:
				// Register iTestAudioRecorder#1 for Notification
				retErr = iTestAudioRecorder1->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Register iTestAudioRecorder#3 for Notification
				retErr = iTestAudioRecorder3->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport3, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder3->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioRecorder3->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioRecorder#3
				iTestAudioRecorder3->OpenFileL(iFilename3);
				INFO_PRINTF1(_L("iTestAudioRecorder3->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange3,CMdaAudioClipUtility::EOpen);
				break;
			case 3:
				// Open iTestAudioRecorder#2
				iTestAudioRecorder2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioRecorder2->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EOpen);
				break;
			case 4:
				// Play iTestAudioRecorder#2
				iTestAudioRecorder2->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder2->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange2,CMdaAudioClipUtility::EPlaying);
				break;
			case 5:
				// Get Play Error for iTestAudioRecorder#1
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen,KErrInUse);
				break;
			case 6:
				// Play iTestAudioRecorder#3
				iTestAudioRecorder3->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder3->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange3,CMdaAudioClipUtility::EPlaying);
				break;
			case 7:
				// Get Play Error for iTestAudioRecorder#3
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange3,CMdaAudioClipUtility::EOpen,KErrInUse);
				break;
			case 8:
				// Stop & Close iTestAudioRecorder#2
				iTestAudioRecorder2->Stop();
				INFO_PRINTF1(_L("iTestAudioRecorder2->Stop()"));
				iTestAudioRecorder2->Close();
				INFO_PRINTF1(_L("iTestAudioRecorder2->Close()"));
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableRec1,CMdaAudioClipUtility::EOpen);
				break;
			case 9:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioRecorder1->NotificationReceived"));
				retErr = iTestAudioRecorder1->WillResumePlay();
				INFO_PRINTF1(_L("iTestAudioRecorder1->WillResumePlay()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->WillResumePlay() - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				iTestAudioRecorder1->SetPosition(iPosition);
				// Play iTestAudioRecorder#1
				iTestAudioRecorder1->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder1->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EPlaying);
				break;
			case 10:
				// Wait for Completion of Play - iTestAudioRecorder#1
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 11:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableRec3,CMdaAudioClipUtility::EOpen);
				break;
			case 12:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioRecorder3->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8016::PerformTestL()
	{
	INFO_PRINTF1( _L("Call WillResumePlay()."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}


//
//NEGATIVE TESTS//
//


//
//CTestMmfAclntARN1311
//

/**
 * Constructor
 */
CTestMmfAclntARN1311::CTestMmfAclntARN1311(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntRecordARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1311* CTestMmfAclntARN1311::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN1311* self = new (ELeave) CTestMmfAclntARN1311(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1311::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1311::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	if (FsmCheck(aEventCode, aRecorderState, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioRecorder#1
				iTestAudioRecorder1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioRecorder1->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 1:
				// Register iTestAudioRecorder#1 for Notification
				retErr = iTestAudioRecorder1->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport1, KMMFEventCategoryPlaybackComplete);
				INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification(KMMFEventCategoryPlaybackComplete)"));
				if( retErr == KErrNotSupported )
					{
					INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification - Returned KErrNotSupported ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN1311::PerformTestL()
	{
	INFO_PRINTF1( _L("Register for notification with an unsupported event type."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN1312
//

/**
 * Constructor
 */
CTestMmfAclntARN1312::CTestMmfAclntARN1312(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntRecordARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1312* CTestMmfAclntARN1312::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN1312* self = new (ELeave) CTestMmfAclntARN1312(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1312::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1312::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	if (FsmCheck(aEventCode, aRecorderState, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioRecorder#1
				iTestAudioRecorder1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioRecorder1->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 1:
				// Register iTestAudioRecorder#1 for Notification
				retErr = iTestAudioRecorder1->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				
				// Cancel Notification for iTestAudioRecorder#1
				retErr = iTestAudioRecorder1->CancelRegisterAudioResourceNotification(KMMFEventCategoryPlaybackComplete);
				INFO_PRINTF1(_L("iTestAudioRecorder1->CancelRegisterAudioResourceNotification(KMMFEventCategoryPlaybackComplete)"));
				if( retErr == KErrNotSupported )
					{
					INFO_PRINTF1(_L("iTestAudioRecorder1->CancelRegisterAudioResourceNotification - Returned KErrNotSupported ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->CancelRegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN1312::PerformTestL()
	{
	INFO_PRINTF1( _L("Cancel the request for notification with an unsupported event type."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN1313
//

/**
 * Constructor
 */
CTestMmfAclntARN1313::CTestMmfAclntARN1313(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntRecordARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1313* CTestMmfAclntARN1313::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN1313* self = new (ELeave) CTestMmfAclntARN1313(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1313::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1313::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	if (FsmCheck(aEventCode, aRecorderState, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioRecorder#1
				iTestAudioRecorder1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioRecorder1->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 1:
				// Cancel Notification for iTestAudioRecorder#1
				retErr = iTestAudioRecorder1->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder1->CancelRegisterAudioResourceNotification()"));
				if( retErr == KErrCancel )
					{
					INFO_PRINTF1(_L("iTestAudioRecorder1->CancelRegisterAudioResourceNotification - Returned KErrCancel") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->CancelRegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN1313::PerformTestL()
	{
	INFO_PRINTF1( _L("Cancel when no resource Notification is requested."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN1314
//

/**
 * Constructor
 */
CTestMmfAclntARN1314::CTestMmfAclntARN1314(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntRecordARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1314* CTestMmfAclntARN1314::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN1314* self = new (ELeave) CTestMmfAclntARN1314(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1314::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1314::FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError)
	{
	if (FsmCheck(aEventCode, aRecorderState, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioRecorder#1
				iTestAudioRecorder1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioRecorder1->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChange1,CMdaAudioClipUtility::EOpen);
				break;
			case 1:
				// Register iTestAudioRecorder#1 for Notification #1
				retErr = iTestAudioRecorder1->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification()#1"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->RegisterAudioResourceNotification#1 - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Register iTestAudioRecorder#1 for Notification #2
				retErr = iTestAudioRecorder1->RegisterAudioResourceNotification(*iMdaAudioRecorderCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification()#2"));
				if( retErr == KErrAlreadyExists )
					{
					INFO_PRINTF1(_L("iTestAudioRecorder1->RegisterAudioResourceNotification#2 - Returned KErrAlreadyExists ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioRecorder1->RegisterAudioResourceNotification#2 - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN1314::PerformTestL()
	{
	INFO_PRINTF1( _L("Multiple Registration for notification for the same event type."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleRecorder, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}
