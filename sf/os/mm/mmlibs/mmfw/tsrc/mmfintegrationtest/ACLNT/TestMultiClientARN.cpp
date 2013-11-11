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
// CPP file: Audio Resource Notification tests for Multiple Audio Clients.
// 
//

/**
 @file TestMultiClientARN.cpp
*/

#include "TestAudioClientUtilitiesARN.h"

//------------------------------------------------------------------------------
//********************PREQ 797 - Audio Resource Notification********************
//------------------------------------------------------------------------------

//
//CTestMdaAudioMultiClientCallbackSupport
//

/**
 * NewL
 */
CTestMdaAudioMultiClientCallbackSupport* CTestMdaAudioMultiClientCallbackSupport::NewL(MTestAudioResourceNotificationCallback& aCallback)
	{
	return new(ELeave) CTestMdaAudioMultiClientCallbackSupport(aCallback);
	}

/**
 * MarncResourceAvailable
 */
void CTestMdaAudioMultiClientCallbackSupport::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	iCallback.MarncResourceAvailableTest(aNotificationEventId, aNotificationData, *this);
	}

/**
 * Constructor
 */
CTestMdaAudioMultiClientCallbackSupport::CTestMdaAudioMultiClientCallbackSupport(MTestAudioResourceNotificationCallback& aCallback):iCallback(aCallback)
	{

	}

//
//CTestMmfAclntMultiClientARN
//

/**
 * Constructor
 */
CTestMmfAclntMultiClientARN::CTestMmfAclntMultiClientARN(const TDesC& aTestName, const TDesC& aSectName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName=aTestName;
	iHeapSize = 800000;
	iSectName = aSectName;
	}

/**
 * MarncResourceAvailableTest
 */
void CTestMmfAclntMultiClientARN::MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioMultiClientCallbackSupport& aMdaAudioMultiClientCallbackSupport)
	{
	if (aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		INFO_PRINTF1(_L("iAudioOutputStream::MMMFAudioResourceNotificationCallback -> KMMFEventCategoryAudioResourceAvailable"));

		//Convert TDesc8 to TInt64
		TMMFTimeIntervalMicroSecondsPckg timeIntvbuf;
		timeIntvbuf.Copy(aNotificationData);
		TTimeIntervalMicroSeconds intPos = timeIntvbuf();
		iPosition = intPos;

		if (&aMdaAudioMultiClientCallbackSupport == iMdaAudioPlayerCallbackSupport)
			{
			INFO_PRINTF1(_L("iAudioPlayer -> MMMFAudioResourceNotificationCallback NotificationReceived"));
			TRAP(iError, FsmL(EMarncResourceAvailablePlayer));
			}
		else if (&aMdaAudioMultiClientCallbackSupport == iMdaAudioRecorderCallbackSupport)
			{
			INFO_PRINTF1(_L("iAudioRecorder -> MMMFAudioResourceNotificationCallback NotificationReceived"));
			TRAP(iError, FsmL(EMarncResourceAvailableRecorder));
			}
		else if (&aMdaAudioMultiClientCallbackSupport == iMdaAudioOutputStreamCallbackSupport)
			{
			INFO_PRINTF1(_L("iAudioOutputStream -> MMMFAudioResourceNotificationCallback NotificationReceived"));
			TRAP(iError, FsmL(EMarncResourceAvailableOS));
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFAudioResourceNotificationCallback, but NOT KMMFEventCategoryAudioResourceAvailable"));
		}
	}

/**
 * MapcInitCompleteTest
 */
void CTestMmfAclntMultiClientARN::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
	{
	iDuration = aDuration;
	INFO_PRINTF2(_L("iAudioPlayer -> MMdaAudioPlayerCallback InitComplete (%d)"),aError);
	TRAP(iError, FsmL(EMapcInitComplete, aError));
	}

/**
 * MapcPlayCompleteTest
 */
void CTestMmfAclntMultiClientARN::MapcPlayComplete(TInt aError)
	{
	INFO_PRINTF2(_L("iAudioPlayer -> MMdaAudioPlayerCallback PlayComplete (%d)"), aError);
	TRAP(iError, FsmL(EMapcPlayComplete, aError));
	}

/**
 * MoscoStateChangeEventTest
 */
void CTestMmfAclntMultiClientARN::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	INFO_PRINTF1(_L("iAudioRecorder -> MMdaObjectStateChangeObserver StateChanged"));
	INFO_PRINTF4(_L("iTestAudioRecorder -> aErrorCode : %d, aPreviousState : %d, aCurrentState : %d"), aErrorCode, aPreviousState, aCurrentState);
	TRAP(iError, FsmL(EMoscoStateChangeEvent, aErrorCode, iTestAudioRecorder->State()));
	}

/**
 * MMdaAudioOutputStreamCallback->MaoscOpenComplete
 */
void CTestMmfAclntMultiClientARN::MaoscOpenComplete(TInt aError)
	{
	INFO_PRINTF2(_L("iAudioOutputStream -> MMdaAudioOutputStreamCallback : MaoscOpenComplete (%d)"),aError);
	TRAP(iError, FsmL(EMaoscOpenComplete, aError));
	}

/**
 * MMdaAudioOutputStreamCallback->MaoscBufferCopied
 */
void CTestMmfAclntMultiClientARN::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	INFO_PRINTF2(_L("iAudioOutputStream -> MMdaAudioOutputStreamCallback : MaoscBufferCopied  (%d)"), aError);
	TRAP(iError, FsmL(EMaoscBufferCopied, aError));
	}

/**
 * MMdaAudioOutputStreamCallback->MaoscPlayComplete
 */
void CTestMmfAclntMultiClientARN::MaoscPlayComplete(TInt aError)
	{
	INFO_PRINTF2(_L("iAudioOutputStream -> MMdaAudioOutputStreamCallback : MaoscPlayComplete  (%d)"), aError);
	TRAP(iError, FsmL(EMaoscPlayComplete, aError));
	}

/**
 * DoTestStepPreambleL
 */
TVerdict CTestMmfAclntMultiClientARN::DoTestStepPreambleL()
	{
	iError = KErrTimedOut;
	TPtrC filename;
	TVerdict verdict;
	
	// Install the scheduler - Call base DoTestStepPreambleL()
	verdict = CTestMmfAclntStep::DoTestStepPreambleL();
	if (verdict == EFail)
		{
		return verdict;
		}
	
	iEventChecker = CMultiEventChecker::NewL();
	//************** Initialise CMdaAudioPlayerUtility***************
	INFO_PRINTF1(_L("Initialise CMdaAudioPlayerUtility : EMdaPriorityNormal"));
	// Get file name
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile1"), filename))
		{
		return EInconclusive;
		}
	GetDriveName(iFilenamePlayer);
	iFilenamePlayer.Append(filename);
	
	// Callback Handler
	iMdaAudioPlayerCallbackSupport = CTestMdaAudioMultiClientCallbackSupport::NewL(*this);
	
	// Initialise Audio Player
	iTestAudioPlayer = CMdaAudioPlayerUtility::NewL(*this, EMdaPriorityNormal);
	
	//************** Initialise CMdaAudioRecorderUtility*************
	INFO_PRINTF1(_L("Initialise CMdaAudioRecorderUtility : EMdaPriorityMax"));
	// Get file name
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile2"), filename))
		{
		return EInconclusive;
		}
	GetDriveName(iFilenameRecorder);
	iFilenameRecorder.Append(filename);
	
	// Callback Handler
	iMdaAudioRecorderCallbackSupport = CTestMdaAudioMultiClientCallbackSupport::NewL(*this);
	
	// Initialise Audio Recorder
	iTestAudioRecorder = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityMax);
	
	//************** Initialise CMdaAudioOutputStream****************
	INFO_PRINTF1(_L("Initialise CMdaAudioOutputStream  : EMdaPriorityMin"));
	// Connect RFs session
	TInt err = iFs.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting to File session (RFs) = %d"), err);
		return EFail;
		}
	// Callback Handler
	iMdaAudioOutputStreamCallbackSupport = CTestMdaAudioMultiClientCallbackSupport::NewL(*this);
	
	// Initialise iTestAudioOutputStream
	TRAP(err, iTestAudioOutputStream = CMdaAudioOutputStream::NewL(*this, EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality));
	if (err != KErrNone || iTestAudioOutputStream == NULL )
		{
		INFO_PRINTF2(_L("Error Creating iTestAudioOutputStream = %d"), err);
		return EInconclusive;
		}
	// Open RFile
	err = iFile.Open(iFs, KStreamRawFile1, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening iFile = %d"), err);
		return EFail;
		}
	// Get size of iFile
	err = iFile.Size(iFileSize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
		return EFail;
		}
	// Initialise iBuf
	iBuf.CreateMaxL(iFileSize);
	
	// Initialise the CCallBackTimer.
	iCallBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	
	return verdict;
	}

/**
 * DoTestStepPostambleL
 */
TVerdict CTestMmfAclntMultiClientARN::DoTestStepPostambleL()
	{
	// Delete all the Audio Client utilities.
	if (iTestAudioPlayer)
		iTestAudioPlayer->Close();
	delete iTestAudioPlayer;
	iTestAudioPlayer = NULL;
	iTestAudioRecorder->Close();
	delete iTestAudioRecorder;
	iTestAudioRecorder = NULL;
	delete iTestAudioOutputStream;
	iTestAudioOutputStream = NULL;
	// Delete CTestMdaAudioRecorderCallbackSupport
	delete iMdaAudioPlayerCallbackSupport;
	delete iMdaAudioRecorderCallbackSupport;
	delete iMdaAudioOutputStreamCallbackSupport;
	// Close RBuf8
	iBuf.Close();
	// Delete the CCallBackTimer.
	delete iCallBackTimer;
	iCallBackTimer=NULL;
	// Close the File Handles
	iFile.Close();
	iFs.Close();
	delete iEventChecker;
	
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * DoTestStepL
 */
TVerdict CTestMmfAclntMultiClientARN::DoTestStepL()
	{
	return( DoTestL() );
	}

/**
 * StateInit
 */
void CTestMmfAclntMultiClientARN::StateInit()
	{
	iExpectedEvent=EIdleMulti;
	iExpectedRecorderState=CMdaAudioClipUtility::ENotReady;
	iExpectedError=KErrNone;
	iState=0;
	}

/**
 * StateChange
 */
void CTestMmfAclntMultiClientARN::StateChange(TMmfAudioMultiClientEvents aExpectedEvent, TInt aExpectedError, CMdaAudioClipUtility::TState aExpectedRecorderState)
	{
	iExpectedEvent=aExpectedEvent;
	iExpectedRecorderState=aExpectedRecorderState;
	iExpectedError=aExpectedError;
	iState++;
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntMultiClientARN::FsmCheck(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
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
//CTestMmfAclntARN9001
//

/**
 * Constructor
 */
CTestMmfAclntARN9001::CTestMmfAclntARN9001(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntMultiClientARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN9001* CTestMmfAclntARN9001::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN9001* self = new (ELeave) CTestMmfAclntARN9001(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN9001::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN9001::FsmCheck(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	TBool retFsmCheck = EFalse;
	if ((iExpectedError !=KErrUnderflow) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
        if ((aEventCode == EMoscoStateChangeEvent))
	        {
	        retFsmCheck = ETrue;
	        if (iEventOccured == EMarncResourceAvailableOS)
	            {
	            return retFsmCheck;
	            }
            else
                {
                iEventOccured = EMoscoStateChangeEvent;
                }
	        }
        else if (aEventCode == EMarncResourceAvailableOS)
            {
            retFsmCheck = ETrue;
            if (iEventOccured == EMoscoStateChangeEvent)
                {
                return retFsmCheck;
                }
            else
                {
                iEventOccured = EMarncResourceAvailableOS;
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
void CTestMmfAclntARN9001::FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	if (FsmCheck(aEventCode, aError, aRecorderState))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioPlayer
				iTestAudioPlayer->OpenFileL(iFilenamePlayer);
				INFO_PRINTF1(_L("iTestAudioPlayer->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMapcInitComplete);
				break;
			case 1:
				// Play iTestAudioPlayer
				iTestAudioPlayer->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer->Play()"));
				// Open iTestAudioRecorder
				iTestAudioRecorder->OpenFileL(iFilenameRecorder);
				INFO_PRINTF1(_L("iTestAudioRecorder->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EOpen);
				break;
			case 2:
				// Play iTestAudioRecorder
				iTestAudioRecorder->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EPlaying);
				break;
			case 3:
				// iTestAudioPlayer gets PlayError.
				StateChange(EMapcPlayComplete, KErrInUse);
				break;
			case 4:
				// Register iTestAudioPlayer for Notification
				err = iTestAudioPlayer->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification : Status - %d "), err);
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Register iTestAudioOutputStream for Notification
				err = iTestAudioOutputStream->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification : Status - %d "), err);
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioOutputStream
				iTestAudioOutputStream->Open(NULL);
				INFO_PRINTF1(_L("iTestAudioOutputStream->Open()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMaoscOpenComplete);
				break;
			case 5:
				// iFile->Read()
				err = iFile.Read(iBuf); //-synchronous
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Error reading iFile %d"), err);
					User::Leave(err);
					}
				// iTestAudioOutputStream->WriteL()
				INFO_PRINTF1(_L("iTestAudioOutputStream->WriteL()"));
				iTestAudioOutputStream->WriteL(iBuf); //-Should get Play Error.
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscBufferCopied, KErrInUse);
				break;
			case 6:
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete, KErrInUse);
				break;
			case 7:
				// Delete the instance iTestAudioPlayer
				delete iTestAudioPlayer;
				iTestAudioPlayer = NULL;
				INFO_PRINTF1(_L("delete iTestAudioPlayer"));
				User::After(100000);
				// Wait for iTestAudioRecorder to complete playing
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EOpen);
				break;
			case 8:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableOS, KErrNone);
				break;
			case 9:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioOutputStream->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN9001::PerformTestL()
	{
	INFO_PRINTF1(_L("Receipt of Notification (on deleting the Audio Play Client instance of a Notified Client) of a Resource Available Event."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleMulti, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}


//
//CTestMmfAclntARN9002
//

/**
 * Constructor
 */
CTestMmfAclntARN9002::CTestMmfAclntARN9002(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntMultiClientARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN9002* CTestMmfAclntARN9002::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN9002* self = new (ELeave) CTestMmfAclntARN9002(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN9002::DoTestL()
	{
	return( PerformTestL() );
	}

void CTestMmfAclntARN9002::InitTest()
	{
	iEventChecker = CMultiEventChecker::NewL();
	//Keep adding handlers
	iStateHandlers[EStateApInitComplete] = &CTestMmfAclntARN9002::HandleStateApInitComplete;
	iStateHandlers[EStateRecoderOpen] = &CTestMmfAclntARN9002::HandleStateRecorderOpen;
	iStateHandlers[EStateRecoderPlaying] = &CTestMmfAclntARN9002::HandleStateRecorderPlaying;
	iStateHandlers[EStateApPlayCompleteKErrInUse] = &CTestMmfAclntARN9002::HandleApPlayCompleteKErrInUse;
	iStateHandlers[EStateAosOpenComplete] = &CTestMmfAclntARN9002::HandleStateAosOpenComplete;
	iStateHandlers[EStateAosBufferCopiedKErrInUse] = &CTestMmfAclntARN9002::HandleStateAosBufferCopiedKErrInUse;
	iStateHandlers[EStateAosPlayCompleteKErrInUse] = &CTestMmfAclntARN9002::HandleStateAosPlayCompleteKErrInUse;
	iStateHandlers[EStateRecorderrelease] = &CTestMmfAclntARN9002::HandleStateRecorderrelease;
	iStateHandlers[EStateApResourceAvailable] = &CTestMmfAclntARN9002::HandleStateApResourceAvailable;
	iStateHandlers[EStateApPlayComplete] = &CTestMmfAclntARN9002::HandleStateApPlayComplete;
	iStateHandlers[EStateAosResourceAvailable] = &CTestMmfAclntARN9002::HandleStateAosResourceAvailable;
	}
	
void CTestMmfAclntARN9002::StartTestL()
	{
	// Open iTestAudioPlayer
	iTestAudioPlayer->OpenFileL(iFilenamePlayer);
	INFO_PRINTF1(_L("iTestAudioPlayer->OpenFileL()"));
	iEventChecker->SetExpectedEvent(0,EMapcInitComplete,KErrNone);
	iEventChecker->SetExpectedState(EStateApInitComplete);
	}
	
TVerdict CTestMmfAclntARN9002::HandleStateApInitComplete()
	{
	iTestAudioPlayer->Play();
	INFO_PRINTF1(_L("iTestAudioPlayer->Play()"));
	// Open iTestAudioRecorder
	iTestAudioRecorder->OpenFileL(iFilenameRecorder);
	INFO_PRINTF1(_L("iTestAudioRecorder->OpenFileL()"));
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent, KErrNone,CMdaAudioClipUtility::EOpen);
	iEventChecker->SetExpectedState(EStateRecoderOpen);
	return EPass;
	}
	
TVerdict CTestMmfAclntARN9002::HandleStateRecorderOpen()
	{
	iTestAudioRecorder->PlayL();
	INFO_PRINTF1(_L("iTestAudioRecorder->PlayL()"));
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent,KErrNone,CMdaAudioClipUtility::EPlaying);
	iEventChecker->SetExpectedState(EStateRecoderPlaying);
	return EPass;
	}

TVerdict CTestMmfAclntARN9002::HandleStateRecorderPlaying()
	{
	iEventChecker->SetExpectedEvent(0,EMapcPlayComplete,KErrInUse);
	iEventChecker->SetExpectedState(EStateApPlayCompleteKErrInUse);
	return EPass;
	}
	
TVerdict CTestMmfAclntARN9002::HandleApPlayCompleteKErrInUse()
	{
	TInt err = iTestAudioPlayer->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification : Status - %d "), err);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	// Register iTestAudioOutputStream for Notification
	err = iTestAudioOutputStream->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification : Status - %d "), err);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	// Open iTestAudioOutputStream
	iTestAudioOutputStream->Open(NULL);
	INFO_PRINTF1(_L("iTestAudioOutputStream->Open()"));
	// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
	iEventChecker->SetExpectedEvent(2,EMaoscOpenComplete,KErrNone);
	iEventChecker->SetExpectedState(EStateAosOpenComplete);
	return EPass;
	}

TVerdict CTestMmfAclntARN9002::HandleStateAosOpenComplete()
	{
	// iFile->Read()
	TInt err = iFile.Read(iBuf); //-synchronous
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		User::Leave(err);
		}
	// iTestAudioOutputStream->WriteL()
	INFO_PRINTF1(_L("iTestAudioOutputStream->WriteL()"));
	iTestAudioOutputStream->WriteL(iBuf); //-Should get Play Error.
	// Set ExpectedEvent & ExpectedError and change the State
	iEventChecker->SetExpectedEvent(2,EMaoscBufferCopied,KErrInUse);
	iEventChecker->SetExpectedState(EStateAosBufferCopiedKErrInUse);
	return EPass;	
	}

TVerdict CTestMmfAclntARN9002::HandleStateAosBufferCopiedKErrInUse()
	{
	iEventChecker->SetExpectedEvent(2,EMaoscPlayComplete,KErrInUse);
	iEventChecker->SetExpectedState(EStateAosPlayCompleteKErrInUse);
	return EPass;
	}

TVerdict CTestMmfAclntARN9002::HandleStateAosPlayCompleteKErrInUse()
	{
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent,KErrNone,CMdaAudioClipUtility::EOpen);
	iEventChecker->SetExpectedState(EStateRecorderrelease);
	return EPass;
	}
	
TVerdict CTestMmfAclntARN9002::HandleStateRecorderrelease()
	{
	// Timer to check if Notification Event Occurs.
	iCallBackTimer->After(2000000);
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(0,EMarncResourceAvailablePlayer,KErrNone);
	iEventChecker->SetExpectedState(EStateApResourceAvailable);
	return EPass;
	}
	
TVerdict CTestMmfAclntARN9002::HandleStateApResourceAvailable()
	{
	iCallBackTimer->Cancel(); // in case something else stopped the AS
	INFO_PRINTF1(_L("iTestAudioPlayer->NotificationReceived"));
	TInt err = iTestAudioPlayer->WillResumePlay();
	INFO_PRINTF1(_L("iTestAudioPlayer->WillResumePlay()"));
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer->WillResumePlay() - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	iTestAudioPlayer->SetPosition(iPosition);
	INFO_PRINTF1(_L("iTestAudioPlayer->SetPosition()"));
	// Play iTestAudioPlayer
	iTestAudioPlayer->Play();
	// Set ExpectedEvent & ExpectedError and change the State
	iEventChecker->RegisterClientEventToMonitor(0,EMapcPlayComplete,KErrNone,EStateApPlayComplete);
	iEventChecker->RegisterClientEventToMonitor(2,EMarncResourceAvailableOS,KErrNone,EStateAosResourceAvailable);
	iEventChecker->EnableMultipleEventMonitor();
	return EPass;
	}
	
TVerdict CTestMmfAclntARN9002::HandleStateApPlayComplete()
	{
	INFO_PRINTF1(_L("AudioPlayer->PlayComplete"));
	if(iEventChecker->IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		iCallBackTimer->Cancel();
		iEventChecker->StopTest();
		}
	else
		{
		iEventChecker->EnableMultipleEventMonitor();
		}
	return EPass;		
	}
	
TVerdict CTestMmfAclntARN9002::HandleStateAosResourceAvailable()
	{
	INFO_PRINTF1(_L("iTestAudioOutputStream->NotificationReceived"));
	if(iEventChecker->IsMonitoringFinished())
		{
		iCallBackTimer->Cancel();
		iTestStepResult = EPass;
		iEventChecker->StopTest();
		}
	else
		{
		iEventChecker->EnableMultipleEventMonitor();
		}
	return EPass;	
	}
	
void CTestMmfAclntARN9002::DecideFsmState(TInt aClientNumber,CMultiEventChecker::TClientUtilityType aClientType, TInt aEventCode, TInt aError,CMdaAudioClipUtility::TState aRecorderState)
	{
	if (iEventChecker->CheckFsm(aClientNumber, aClientType, aEventCode, aError,aRecorderState))
		{
		iTestStepResult = (this->*(iStateHandlers[iEventChecker->NextState()]))();
 		if(iTestStepResult != EPass)
 			{
 			iEventChecker->StopTest();
 			}
		}
	}

void CTestMmfAclntARN9002::FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	if (aEventCode == EMapcInitComplete || aEventCode == EMapcPlayComplete || aEventCode == EMarncResourceAvailablePlayer)
		{
			DecideFsmState(0,CMultiEventChecker::EAudioPlayer,aEventCode,aError);
		}
	if (aEventCode == EMoscoStateChangeEvent || aEventCode == EMarncResourceAvailableRecorder)
		{
			DecideFsmState(1,CMultiEventChecker::EAudioRecorder,aEventCode,aError,aRecorderState);
		}
	if (aEventCode == EMaoscOpenComplete || aEventCode == EMaoscBufferCopied || aEventCode == EMaoscPlayComplete || aEventCode == EMarncResourceAvailableOS)
		{
			DecideFsmState(2,CMultiEventChecker::EAudioStream,aEventCode,aError);
		}
	}

TVerdict CTestMmfAclntARN9002::PerformTestL()
	{
	INFO_PRINTF1(_L("Receipt of Notification (on completion of play of a Notifed Client) of a Resource Available Event."));
	iTestStepResult = EFail;
	
	// Initialise the handler functions
	InitTest();
	
	// Initiate the process
	StartTestL();
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}


//
//CTestMmfAclntARN9003
//

/**
 * Constructor
 */
CTestMmfAclntARN9003::CTestMmfAclntARN9003(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntMultiClientARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN9003* CTestMmfAclntARN9003::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN9003* self = new (ELeave) CTestMmfAclntARN9003(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN9003::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN9003::FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	if (FsmCheck(aEventCode, aError, aRecorderState))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioPlayer
				iTestAudioPlayer->OpenFileL(iFilenamePlayer);
				INFO_PRINTF1(_L("iTestAudioPlayer->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMapcInitComplete);
				break;
			case 1:
				// Play iTestAudioPlayer
				iTestAudioPlayer->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer->Play()"));
				// Open iTestAudioRecorder
				iTestAudioRecorder->OpenFileL(iFilenameRecorder);
				INFO_PRINTF1(_L("iTestAudioRecorder->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EOpen);
				break;
			case 2:
				// Play iTestAudioRecorder
				iTestAudioRecorder->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EPlaying);
				break;
			case 3:
				// iTestAudioPlayer gets PlayError.
				StateChange(EMapcPlayComplete, KErrInUse);
				break;
			case 4:
				// Register iTestAudioPlayer for Notification
				err = iTestAudioPlayer->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification : Status - %d "), err);
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Register iTestAudioOutputStream for Notification
				err = iTestAudioOutputStream->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification : Status - %d "), err);
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioOutputStream
				iTestAudioOutputStream->Open(NULL);
				INFO_PRINTF1(_L("iTestAudioOutputStream->Open()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMaoscOpenComplete);
				break;
			case 5:
				// iFile->Read()
				err = iFile.Read(iBuf); //-synchronous
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Error reading iFile %d"), err);
					User::Leave(err);
					}
				// iTestAudioOutputStream->WriteL()
				INFO_PRINTF1(_L("iTestAudioOutputStream->WriteL()"));
				iTestAudioOutputStream->WriteL(iBuf); //-Should get Play Error.
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscBufferCopied, KErrInUse);
				break;
			case 6:
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete, KErrInUse);
				break;			
			case 7:
				// Wait for iTestAudioRecorder to complete playing
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EOpen);
				break;
			case 8:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailablePlayer, KErrNone);
				break;
			case 9:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioPlayer->NotificationReceived"));
				err = iTestAudioPlayer->WillResumePlay();
				INFO_PRINTF1(_L("iTestAudioPlayer->WillResumePlay()"));
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer->WillResumePlay() - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				iTestAudioPlayer->SetPosition(iPosition);
				INFO_PRINTF1(_L("iTestAudioPlayer->SetPosition()"));
				// Play iTestAudioPlayer
				iTestAudioPlayer->Play();
				// Stop & Close iTestAudioPlayer
				WaitWithTimeout(iRequestStatus,1000000);
				iTestAudioPlayer->Stop();
				INFO_PRINTF1(_L("iTestAudioPlayer->Stop()"));
				iTestAudioPlayer->Close();
				INFO_PRINTF1(_L("iTestAudioPlayer->Close()"));
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableOS);
				break;
			case 10:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioOutputStream->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN9003::PerformTestL()
	{
	INFO_PRINTF1(_L("Receipt of Notification (on stopping play of a Notitifed Client) of a Resource Available Event."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleMulti, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN9004
//

/**
 * Constructor
 */
CTestMmfAclntARN9004::CTestMmfAclntARN9004(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntMultiClientARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN9004* CTestMmfAclntARN9004::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN9004* self = new (ELeave) CTestMmfAclntARN9004(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN9004::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN9004::FsmCheck(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	TBool retFsmCheck = EFalse;
	if ((iExpectedError !=KErrUnderflow) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
        if ((aEventCode == EMoscoStateChangeEvent))
	        {
	        retFsmCheck = ETrue;
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	        if (iEventOccured == EMarncResourceAvailablePlayer)
#else
   	        if (iEventOccured == EMarncResourceAvailableOS)
#endif
	            {
	            return retFsmCheck;
	            }
            else
                {
                iEventOccured = EMoscoStateChangeEvent;
                }
	        }
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
        else if (aEventCode == EMarncResourceAvailablePlayer)
#else
        else if (aEventCode == EMarncResourceAvailableOS)
#endif
            {
            retFsmCheck = ETrue;
            if (iEventOccured == EMoscoStateChangeEvent)
                {
                return retFsmCheck;
                }
            else
                {
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
                iEventOccured = EMarncResourceAvailablePlayer;
#else
				iEventOccured = EMarncResourceAvailableOS;
#endif
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
void CTestMmfAclntARN9004::FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	if (FsmCheck(aEventCode, aError, aRecorderState))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioPlayer
				iTestAudioPlayer->OpenFileL(iFilenamePlayer);
				INFO_PRINTF1(_L("iTestAudioPlayer->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMapcInitComplete);
				break;
			case 1:
				// Play iTestAudioPlayer
				iTestAudioPlayer->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer->Play()"));
				// Open iTestAudioRecorder
				iTestAudioRecorder->OpenFileL(iFilenameRecorder);
				INFO_PRINTF1(_L("iTestAudioRecorder->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EOpen);
				break;
			case 2:
				// Play iTestAudioRecorder
				iTestAudioRecorder->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EPlaying);
				break;
			case 3:
				// iTestAudioPlayer gets PlayError.
				StateChange(EMapcPlayComplete, KErrInUse);
				break;
			case 4:
				// Register iTestAudioOutputStream for Notification
				err = iTestAudioOutputStream->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification : Status - %d "), err);
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Register iTestAudioPlayer for Notification
				err = iTestAudioPlayer->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification : Status - %d "), err);
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioOutputStream
				iTestAudioOutputStream->Open(NULL);
				INFO_PRINTF1(_L("iTestAudioOutputStream->Open()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMaoscOpenComplete);
				break;
			case 5:
				// iFile->Read()
				err = iFile.Read(iBuf); //-synchronous
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Error reading iFile %d"), err);
					User::Leave(err);
					}
				// iTestAudioOutputStream->WriteL()
				INFO_PRINTF1(_L("iTestAudioOutputStream->WriteL()"));
				iTestAudioOutputStream->WriteL(iBuf); //-Should get Play Error.
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscBufferCopied, KErrInUse);
				break;
			case 6:
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete, KErrInUse);
				break;			
			case 7:
				// Wait for iTestAudioRecorder to complete playing
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EOpen);
				break;
			case 8:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableOS);
				break;
			case 9:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioOutputStream->NotificationReceived"));
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				StateChange(EMarncResourceAvailablePlayer);
#else
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
#endif
				break;
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
			case 10:
				INFO_PRINTF1(_L("iTestAudioPlayer->NotificationReceived"));
				err = iTestAudioPlayer->WillResumePlay();
				INFO_PRINTF1(_L("iTestAudioPlayer->WillResumePlay()"));
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer->WillResumePlay() - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
#endif
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN9004::PerformTestL()
	{
	INFO_PRINTF1(_L("Receipt of Notification (chronological order for equal priority clients) of a Resource Available Event."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Set the priorities
	iTestAudioPlayer->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality);
	INFO_PRINTF1(_L("Reset Priority for CMdaAudioPlayerUtility  : EMdaPriorityMin"));
	iTestAudioOutputStream->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality);
	INFO_PRINTF1(_L("Reset Priority for CMdaAudioOutputStream   : EMdaPriorityMin"));
	
	// Begin the process
	FsmL(EIdleMulti, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN9005
//

/**
 * Constructor
 */
CTestMmfAclntARN9005::CTestMmfAclntARN9005(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntMultiClientARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN9005* CTestMmfAclntARN9005::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN9005* self = new (ELeave) CTestMmfAclntARN9005(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN9005::DoTestL()
	{
	return( PerformTestL() );
	}

void CTestMmfAclntARN9005::InitTest()
	{
	//Keep adding handlers
	iStateHandlers[EStateApInitComplete] = &CTestMmfAclntARN9005::HandleStateApInitComplete;
	iStateHandlers[EStateRecoderOpen] = &CTestMmfAclntARN9005::HandleStateRecorderOpen;
	iStateHandlers[EStateRecoderPlaying] = &CTestMmfAclntARN9005::HandleStateRecorderPlaying;
	iStateHandlers[EStateApPlayCompleteKErrInUse] = &CTestMmfAclntARN9005::HandleApPlayCompleteKErrInUse;
	iStateHandlers[EStateAosOpenComplete] = &CTestMmfAclntARN9005::HandleStateAosOpenComplete;
	iStateHandlers[EStateAosBufferCopiedKErrInUse] = &CTestMmfAclntARN9005::HandleStateAosBufferCopiedKErrInUse;
	iStateHandlers[EStateAosPlayCompleteKErrInUse] = &CTestMmfAclntARN9005::HandleStateAosPlayCompleteKErrInUse;
	iStateHandlers[EStateRecorderrelease] = &CTestMmfAclntARN9005::HandleStateRecorderrelease;
	iStateHandlers[EStateApResourceAvailable] = &CTestMmfAclntARN9005::HandleStateApResourceAvailable;
	iStateHandlers[EStateApPlayComplete] = &CTestMmfAclntARN9005::HandleStateApPlayComplete;
	iStateHandlers[EStateAosResourceAvailable] = &CTestMmfAclntARN9005::HandleStateAosResourceAvailable;
	}

void CTestMmfAclntARN9005::DecideFsmState(TInt aClientNumber,CMultiEventChecker::TClientUtilityType aClientType, TInt aEventCode, TInt aError,CMdaAudioClipUtility::TState aRecorderState)
	{
	if (iEventChecker->CheckFsm(aClientNumber, aClientType, aEventCode, aError,aRecorderState))
		{
		iTestStepResult = (this->*(iStateHandlers[iEventChecker->NextState()]))();
 		if(iTestStepResult != EPass)
 			{
 			iEventChecker->StopTest();
 			}
		}
	}

void CTestMmfAclntARN9005::FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	if (aEventCode == EMapcInitComplete || aEventCode == EMapcPlayComplete || aEventCode == EMarncResourceAvailablePlayer)
		{
			DecideFsmState(0,CMultiEventChecker::EAudioPlayer,aEventCode,aError);
		}
	if (aEventCode == EMoscoStateChangeEvent || aEventCode == EMarncResourceAvailableRecorder)
		{
			DecideFsmState(1,CMultiEventChecker::EAudioRecorder,aEventCode,aError,aRecorderState);
		}
	if (aEventCode == EMaoscOpenComplete || aEventCode == EMaoscBufferCopied || aEventCode == EMaoscPlayComplete || aEventCode == EMarncResourceAvailableOS)
		{
			DecideFsmState(2,CMultiEventChecker::EAudioStream,aEventCode,aError);
		}
	}

void CTestMmfAclntARN9005::StartTestL()
	{
	// Open iTestAudioPlayer
	iTestAudioPlayer->OpenFileL(iFilenamePlayer);
	INFO_PRINTF1(_L("iTestAudioPlayer->OpenFileL()"));
	iEventChecker->SetExpectedEvent(0,EMapcInitComplete,KErrNone);
	iEventChecker->SetExpectedState(EStateApInitComplete);
	}
	
TVerdict CTestMmfAclntARN9005::HandleStateApInitComplete()
	{
	iTestAudioPlayer->Play();
	INFO_PRINTF1(_L("iTestAudioPlayer->Play()"));
	// Open iTestAudioRecorder
	iTestAudioRecorder->OpenFileL(iFilenameRecorder);
	INFO_PRINTF1(_L("iTestAudioRecorder->OpenFileL()"));
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent, KErrNone,CMdaAudioClipUtility::EOpen);
	iEventChecker->SetExpectedState(EStateRecoderOpen);
	return EPass;
	}	

TVerdict CTestMmfAclntARN9005::HandleStateRecorderOpen()
	{
	iTestAudioRecorder->PlayL();
	INFO_PRINTF1(_L("iTestAudioRecorder->PlayL()"));
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent,KErrNone,CMdaAudioClipUtility::EPlaying);
	iEventChecker->SetExpectedState(EStateRecoderPlaying);
	return EPass;
	}

TVerdict CTestMmfAclntARN9005::HandleStateRecorderPlaying()
	{
	iEventChecker->SetExpectedEvent(0,EMapcPlayComplete,KErrInUse);
	iEventChecker->SetExpectedState(EStateApPlayCompleteKErrInUse);
	return EPass;
	}
	
TVerdict CTestMmfAclntARN9005::HandleApPlayCompleteKErrInUse()
	{
	TInt err = iTestAudioPlayer->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification : Status - %d "), err);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	// Register iTestAudioOutputStream for Notification
	err = iTestAudioOutputStream->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification : Status - %d "), err);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	// Open iTestAudioOutputStream
	iTestAudioOutputStream->Open(NULL);
	INFO_PRINTF1(_L("iTestAudioOutputStream->Open()"));
	// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
	iEventChecker->SetExpectedEvent(2,EMaoscOpenComplete,KErrNone);
	iEventChecker->SetExpectedState(EStateAosOpenComplete);
	return EPass;
	}

TVerdict CTestMmfAclntARN9005::HandleStateAosOpenComplete()
	{
	// iFile->Read()
	TInt err = iFile.Read(iBuf); //-synchronous
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		User::Leave(err);
		}
	// iTestAudioOutputStream->WriteL()
	INFO_PRINTF1(_L("iTestAudioOutputStream->WriteL()"));
	iTestAudioOutputStream->WriteL(iBuf); //-Should get Play Error.
	// Set ExpectedEvent & ExpectedError and change the State
	iEventChecker->SetExpectedEvent(2,EMaoscBufferCopied,KErrInUse);
	iEventChecker->SetExpectedState(EStateAosBufferCopiedKErrInUse);
	return EPass;	
	}

TVerdict CTestMmfAclntARN9005::HandleStateAosBufferCopiedKErrInUse()
	{
	iEventChecker->SetExpectedEvent(2,EMaoscPlayComplete,KErrInUse);
	iEventChecker->SetExpectedState(EStateAosPlayCompleteKErrInUse);
	return EPass;
	}

TVerdict CTestMmfAclntARN9005::HandleStateAosPlayCompleteKErrInUse()
	{
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent,KErrNone,CMdaAudioClipUtility::EOpen);
	iEventChecker->SetExpectedState(EStateRecorderrelease);
	return EPass;
	}

TVerdict CTestMmfAclntARN9005::HandleStateRecorderrelease()
	{
	// Timer to check if Notification Event Occurs.
	iCallBackTimer->After(2000000);
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(0,EMarncResourceAvailablePlayer,KErrNone);
	iEventChecker->SetExpectedState(EStateApResourceAvailable);
	return EPass;
	}	
	
TVerdict CTestMmfAclntARN9005::HandleStateApResourceAvailable()
	{
	iCallBackTimer->Cancel(); // in case something else stopped the AS
	INFO_PRINTF1(_L("iTestAudioPlayer->NotificationReceived"));
	TInt err = iTestAudioPlayer->WillResumePlay();
	INFO_PRINTF1(_L("iTestAudioPlayer->WillResumePlay()"));
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer->WillResumePlay() - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		return EFail;
		}
	iTestAudioPlayer->SetPosition(iPosition);
	INFO_PRINTF1(_L("iTestAudioPlayer->SetPosition()"));
	// Play iTestAudioPlayer
	iTestAudioPlayer->Play();
	// Set ExpectedEvent & ExpectedError and change the State
	iEventChecker->RegisterClientEventToMonitor(0,EMapcPlayComplete,KErrNone,EStateApPlayComplete);
	iEventChecker->RegisterClientEventToMonitor(2,EMarncResourceAvailableOS,KErrNone,EStateAosResourceAvailable);
	iEventChecker->EnableMultipleEventMonitor();
	return EPass;
	}

TVerdict CTestMmfAclntARN9005::HandleStateApPlayComplete()
	{
	INFO_PRINTF1(_L("AudioPlayer->PlayComplete"));
	if(iEventChecker->IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		iCallBackTimer->Cancel();
		iEventChecker->StopTest();
		}
	else
		{
		iEventChecker->EnableMultipleEventMonitor();
		}
	return EPass;		
	}
	
TVerdict CTestMmfAclntARN9005::HandleStateAosResourceAvailable()
	{
	INFO_PRINTF1(_L("iTestAudioOutputStream->NotificationReceived"));
	if(iEventChecker->IsMonitoringFinished())
		{
		iCallBackTimer->Cancel();
		iTestStepResult = EPass;
		iEventChecker->StopTest();
		}
	else
		{
		iEventChecker->EnableMultipleEventMonitor();
		}
	return EPass;	
	}	
	
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN9005::PerformTestL()
	{
	INFO_PRINTF1(_L("Non Receipt of Notification (on calling WillResumePlay() of a notified client) of a Resource Available Event."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	InitTest();
	
	// Begin the process
	StartTestL();
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN9006
//

/**
 * Constructor
 */
CTestMmfAclntARN9006::CTestMmfAclntARN9006(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntMultiClientARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN9006* CTestMmfAclntARN9006::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN9006* self = new (ELeave) CTestMmfAclntARN9006(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN9006::DoTestL()
	{
	return( PerformTestL() );
	}

void CTestMmfAclntARN9006::DecideFsmState(TInt aClientNumber,CMultiEventChecker::TClientUtilityType aClientType, TInt aEventCode, TInt aError,CMdaAudioClipUtility::TState aRecorderState)
	{
	if (iEventChecker->CheckFsm(aClientNumber, aClientType, aEventCode, aError,aRecorderState))
		{
		iTestStepResult = (this->*(iStateHandlers[iEventChecker->NextState()]))();
 		if(iTestStepResult != EPass)
 			{
 			iEventChecker->StopTest();
 			}
		}
	}

void CTestMmfAclntARN9006::FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	if (aEventCode == EMapcInitComplete || aEventCode == EMapcPlayComplete || aEventCode == EMarncResourceAvailablePlayer)
		{
			DecideFsmState(0,CMultiEventChecker::EAudioPlayer,aEventCode,aError);
		}
	if (aEventCode == EMoscoStateChangeEvent || aEventCode == EMarncResourceAvailableRecorder)
		{
			DecideFsmState(1,CMultiEventChecker::EAudioRecorder,aEventCode,aError,aRecorderState);
		}
	if (aEventCode == EMaoscOpenComplete || aEventCode == EMaoscBufferCopied || aEventCode == EMaoscPlayComplete || aEventCode == EMarncResourceAvailableOS)
		{
			DecideFsmState(2,CMultiEventChecker::EAudioStream,aEventCode,aError);
		}
	}

void CTestMmfAclntARN9006::InitTest()
	{
	//Keep adding handlers
	iStateHandlers[EStateApInitComplete] = &CTestMmfAclntARN9006::HandleStateApInitComplete;
	iStateHandlers[EStateRecoderOpen] = &CTestMmfAclntARN9006::HandleStateRecorderOpen;
	iStateHandlers[EStateRecoderPlaying] = &CTestMmfAclntARN9006::HandleStateRecorderPlaying;
	iStateHandlers[EStateApPlayCompleteKErrInUse] = &CTestMmfAclntARN9006::HandleApPlayCompleteKErrInUse;
	iStateHandlers[EStateAosOpenComplete] = &CTestMmfAclntARN9006::HandleStateAosOpenComplete;
	iStateHandlers[EStateAosBufferCopiedKErrInUse] = &CTestMmfAclntARN9006::HandleStateAosBufferCopiedKErrInUse;
	iStateHandlers[EStateAosPlayCompleteKErrInUse] = &CTestMmfAclntARN9006::HandleStateAosPlayCompleteKErrInUse;
	iStateHandlers[EStateRecorderrelease] = &CTestMmfAclntARN9006::HandleStateRecorderrelease;
	iStateHandlers[EStateApResourceAvailable] = &CTestMmfAclntARN9006::HandleStateApResourceAvailable;
	iStateHandlers[EStateAosResourceAvailable] = &CTestMmfAclntARN9006::HandleStateAosResourceAvailable;
	}
	
void CTestMmfAclntARN9006::StartTestL()
	{
	// Open iTestAudioPlayer
	iTestAudioPlayer->OpenFileL(iFilenamePlayer);
	INFO_PRINTF1(_L("iTestAudioPlayer->OpenFileL()"));
	iEventChecker->SetExpectedEvent(0,EMapcInitComplete,KErrNone);
	iEventChecker->SetExpectedState(EStateApInitComplete);
	}

TVerdict CTestMmfAclntARN9006::HandleStateApInitComplete()
	{
	iTestAudioPlayer->Play();
	INFO_PRINTF1(_L("iTestAudioPlayer->Play()"));
	// Open iTestAudioRecorder
	iTestAudioRecorder->OpenFileL(iFilenameRecorder);
	INFO_PRINTF1(_L("iTestAudioRecorder->OpenFileL()"));
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent, KErrNone,CMdaAudioClipUtility::EOpen);
	iEventChecker->SetExpectedState(EStateRecoderOpen);
	return EPass;
	}

TVerdict CTestMmfAclntARN9006::HandleStateRecorderOpen()
	{
	iTestAudioRecorder->PlayL();
	INFO_PRINTF1(_L("iTestAudioRecorder->PlayL()"));
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent,KErrNone,CMdaAudioClipUtility::EPlaying);
	iEventChecker->SetExpectedState(EStateRecoderPlaying);
	return EPass;
	}

TVerdict CTestMmfAclntARN9006::HandleStateRecorderPlaying()
	{
	iEventChecker->SetExpectedEvent(0,EMapcPlayComplete,KErrInUse);
	iEventChecker->SetExpectedState(EStateApPlayCompleteKErrInUse);
	return EPass;
	}

TVerdict CTestMmfAclntARN9006::HandleApPlayCompleteKErrInUse()
	{
	TInt err = iTestAudioPlayer->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification : Status - %d "), err);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		return EFail;
		}
	// Register iTestAudioOutputStream for Notification
	err = iTestAudioOutputStream->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification : Status - %d "), err);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		return EFail;
		}
	// Open iTestAudioOutputStream
	iTestAudioOutputStream->Open(NULL);
	INFO_PRINTF1(_L("iTestAudioOutputStream->Open()"));
	// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
	iEventChecker->SetExpectedEvent(2,EMaoscOpenComplete,KErrNone);
	iEventChecker->SetExpectedState(EStateAosOpenComplete);
	return EPass;
	}

TVerdict CTestMmfAclntARN9006::HandleStateAosOpenComplete()
	{
	// iFile->Read()
	TInt err = iFile.Read(iBuf); //-synchronous
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		User::Leave(err);
		}
	// iTestAudioOutputStream->WriteL()
	INFO_PRINTF1(_L("iTestAudioOutputStream->WriteL()"));
	iTestAudioOutputStream->WriteL(iBuf); //-Should get Play Error.
	// Set ExpectedEvent & ExpectedError and change the State
	iEventChecker->SetExpectedEvent(2,EMaoscBufferCopied,KErrInUse);
	iEventChecker->SetExpectedState(EStateAosBufferCopiedKErrInUse);
	return EPass;	
	}

TVerdict CTestMmfAclntARN9006::HandleStateAosBufferCopiedKErrInUse()
	{
	iEventChecker->SetExpectedEvent(2,EMaoscPlayComplete,KErrInUse);
	iEventChecker->SetExpectedState(EStateAosPlayCompleteKErrInUse);
	return EPass;
	}

TVerdict CTestMmfAclntARN9006::HandleStateAosPlayCompleteKErrInUse()
	{
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent,KErrNone,CMdaAudioClipUtility::EOpen);
	iEventChecker->SetExpectedState(EStateRecorderrelease);
	return EPass;
	}

TVerdict CTestMmfAclntARN9006::HandleStateRecorderrelease()
	{
	// Timer to check if Notification Event Occurs.
	iCallBackTimer->After(2000000);
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(0,EMarncResourceAvailablePlayer,KErrNone);
	iEventChecker->SetExpectedState(EStateApResourceAvailable);
	return EPass;
	}

TVerdict CTestMmfAclntARN9006::HandleStateApResourceAvailable()
	{
	iCallBackTimer->Cancel(); // in case something else stopped the AS
	INFO_PRINTF1(_L("iTestAudioPlayer->NotificationReceived"));
	TInt err = iTestAudioPlayer->WillResumePlay();
	INFO_PRINTF1(_L("iTestAudioPlayer->WillResumePlay()"));
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer->WillResumePlay() - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		return EFail;
		}
	iCallBackTimer->After(10000000);
	// Set ExpectedEvent & ExpectedError and change the State
	iEventChecker->SetExpectedEvent(2,EMarncResourceAvailableOS,KErrNone);
	iEventChecker->SetExpectedState(EStateAosResourceAvailable);
	return EPass;
	}

TVerdict CTestMmfAclntARN9006::HandleStateAosResourceAvailable()
	{
	INFO_PRINTF1(_L("iTestAudioOutputStream->NotificationReceived"));
	iCallBackTimer->Cancel();
	iTestStepResult = EPass;
	iEventChecker->StopTest();
	return EPass;
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN9006::PerformTestL()
	{
	INFO_PRINTF1(_L("Receipt of Notification (on timeout of a notified client, 1 sec assumed, call WillResumePlay() after 1 second) of a Resource Available Event."));
	iTestStepResult = EFail;
	
	// Initialise the handler functions
	InitTest();
	
	// Initiate the process
	StartTestL();
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN9007
//

/**
 * Constructor
 */
CTestMmfAclntARN9007::CTestMmfAclntARN9007(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntMultiClientARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN9007* CTestMmfAclntARN9007::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN9007* self = new (ELeave) CTestMmfAclntARN9007(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN9007::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN9007::FsmCheck(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	TBool retFsmCheck = EFalse;
	if ((iExpectedError !=KErrUnderflow) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
        if ((aEventCode == EMoscoStateChangeEvent))
	        {
	        retFsmCheck = ETrue;
	        if (iEventOccured == EMarncResourceAvailableOS)
	            {
	            return retFsmCheck;
	            }
            else
                {
                iEventOccured = EMoscoStateChangeEvent;
                }
	        }
        else if (aEventCode == EMarncResourceAvailableOS)
            {
            retFsmCheck = ETrue;
            if (iEventOccured == EMoscoStateChangeEvent)
                {
                return retFsmCheck;
                }
            else
                {
                iEventOccured = EMarncResourceAvailableOS;
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
void CTestMmfAclntARN9007::FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	if (FsmCheck(aEventCode, aError, aRecorderState))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioPlayer
				iTestAudioPlayer->OpenFileL(iFilenamePlayer);
				INFO_PRINTF1(_L("iTestAudioPlayer->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMapcInitComplete);
				break;
			case 1:
				// Play iTestAudioPlayer
				iTestAudioPlayer->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer->Play()"));
				// Open iTestAudioRecorder
				iTestAudioRecorder->OpenFileL(iFilenameRecorder);
				INFO_PRINTF1(_L("iTestAudioRecorder->OpenFileL()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EOpen);
				break;
			case 2:
				// Play iTestAudioRecorder
				iTestAudioRecorder->PlayL();
				INFO_PRINTF1(_L("iTestAudioRecorder->PlayL()"));
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EPlaying);
				break;
			case 3:
				// iTestAudioPlayer gets PlayError.
				StateChange(EMapcPlayComplete, KErrInUse);
				break;
			case 4:
				// Register iTestAudioPlayer for Notification
				err = iTestAudioPlayer->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification : Status - %d "), err);
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Register iTestAudioOutputStream for Notification
				err = iTestAudioOutputStream->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification : Status - %d "), err);
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioOutputStream
				iTestAudioOutputStream->Open(NULL);
				INFO_PRINTF1(_L("iTestAudioOutputStream->Open()"));
				// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
				StateChange(EMaoscOpenComplete);
				break;
			case 5:
				// iFile->Read()
				err = iFile.Read(iBuf); //-synchronous
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Error reading iFile %d"), err);
					User::Leave(err);
					}
				// iTestAudioOutputStream->WriteL()
				INFO_PRINTF1(_L("iTestAudioOutputStream->WriteL()"));
				iTestAudioOutputStream->WriteL(iBuf); //-Should get Play Error.
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscBufferCopied, KErrInUse);
				break;
			case 6:
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete, KErrInUse);
				break;
			case 7:
				// Cancel the Notification request for iTestAudioPlayer
				err = iTestAudioPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF2(_L("iTestAudioPlayer->CancelRegisterAudioResourceNotification : Return Code = %d"), err);
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer->CancelRegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Wait for iTestAudioRecorder to complete playing
				// Set ExpectedEvent and change the State
				StateChange(EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EOpen);
				break;
			case 8:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EMarncResourceAvailableOS);
				break;
			case 9:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioOutputStream->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN9007::PerformTestL()
	{
	INFO_PRINTF1(_L("Receipt of Notification (on cancellation of the notification of a client that has requested for notification) of a Resource Available Event."));
	INFO_PRINTF1(_L("[Cancel before receipt of Notification]"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleMulti, CMdaAudioClipUtility::ENotReady);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN9008
//

/**
 * Constructor
 */
CTestMmfAclntARN9008::CTestMmfAclntARN9008(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntMultiClientARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN9008* CTestMmfAclntARN9008::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN9008* self = new (ELeave) CTestMmfAclntARN9008(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN9008::DoTestL()
	{
	return( PerformTestL() );
	}

void CTestMmfAclntARN9008::DecideFsmState(TInt aClientNumber,CMultiEventChecker::TClientUtilityType aClientType, TInt aEventCode, TInt aError,CMdaAudioClipUtility::TState aRecorderState)
	{
	if (iEventChecker->CheckFsm(aClientNumber, aClientType, aEventCode, aError,aRecorderState))
		{
		iTestStepResult = (this->*(iStateHandlers[iEventChecker->NextState()]))();
 		if(iTestStepResult != EPass)
 			{
 			iEventChecker->StopTest();
 			}
		}
	}

void CTestMmfAclntARN9008::FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError, CMdaAudioClipUtility::TState aRecorderState)
	{
	if (aEventCode == EMapcInitComplete || aEventCode == EMapcPlayComplete || aEventCode == EMarncResourceAvailablePlayer)
		{
			DecideFsmState(0,CMultiEventChecker::EAudioPlayer,aEventCode,aError);
		}
	if (aEventCode == EMoscoStateChangeEvent || aEventCode == EMarncResourceAvailableRecorder)
		{
			DecideFsmState(1,CMultiEventChecker::EAudioRecorder,aEventCode,aError,aRecorderState);
		}
	if (aEventCode == EMaoscOpenComplete || aEventCode == EMaoscBufferCopied || aEventCode == EMaoscPlayComplete || aEventCode == EMarncResourceAvailableOS)
		{
			DecideFsmState(2,CMultiEventChecker::EAudioStream,aEventCode,aError);
		}
	}

void CTestMmfAclntARN9008::InitTest()
	{
	//Keep adding handlers
	iStateHandlers[EStateApInitComplete] = &CTestMmfAclntARN9008::HandleStateApInitComplete;
	iStateHandlers[EStateRecoderOpen] = &CTestMmfAclntARN9008::HandleStateRecorderOpen;
	iStateHandlers[EStateRecoderPlaying] = &CTestMmfAclntARN9008::HandleStateRecorderPlaying;
	iStateHandlers[EStateApPlayCompleteKErrInUse] = &CTestMmfAclntARN9008::HandleApPlayCompleteKErrInUse;
	iStateHandlers[EStateAosOpenComplete] = &CTestMmfAclntARN9008::HandleStateAosOpenComplete;
	iStateHandlers[EStateAosBufferCopiedKErrInUse] = &CTestMmfAclntARN9008::HandleStateAosBufferCopiedKErrInUse;
	iStateHandlers[EStateAosPlayCompleteKErrInUse] = &CTestMmfAclntARN9008::HandleStateAosPlayCompleteKErrInUse;
	iStateHandlers[EStateRecorderrelease] = &CTestMmfAclntARN9008::HandleStateRecorderrelease;
	iStateHandlers[EStateApResourceAvailable] = &CTestMmfAclntARN9008::HandleStateApResourceAvailable;
	iStateHandlers[EStateApPlayComplete] = &CTestMmfAclntARN9008::HandleStateApPlayComplete;
	iStateHandlers[EStateAosResourceAvailable] = &CTestMmfAclntARN9008::HandleStateAosResourceAvailable;
	iStateHandlers[EStateRecorderResumePlay] = &CTestMmfAclntARN9008::HandleStateRecorderResumePlay;
	iStateHandlers[EStateRecorderPlayComplete]= &CTestMmfAclntARN9008::HandleStateRecorderPlayComplete;
	}
	
void CTestMmfAclntARN9008::StartTestL()
	{
	// Open iTestAudioPlayer
	iTestAudioPlayer->OpenFileL(iFilenamePlayer);
	INFO_PRINTF1(_L("iTestAudioPlayer->OpenFileL()"));
	iEventChecker->SetExpectedEvent(0,EMapcInitComplete,KErrNone);
	iEventChecker->SetExpectedState(EStateApInitComplete);
	}
	
TVerdict CTestMmfAclntARN9008::HandleStateApInitComplete()
	{
	iTestAudioPlayer->Play();
	INFO_PRINTF1(_L("iTestAudioPlayer->Play()"));
	// Open iTestAudioRecorder
	iTestAudioRecorder->OpenFileL(iFilenameRecorder);
	INFO_PRINTF1(_L("iTestAudioRecorder->OpenFileL()"));
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent, KErrNone,CMdaAudioClipUtility::EOpen);
	iEventChecker->SetExpectedState(EStateRecoderOpen);
	return EPass;
	}

TVerdict CTestMmfAclntARN9008::HandleStateRecorderOpen()
	{
	iTestAudioRecorder->PlayL();
	INFO_PRINTF1(_L("iTestAudioRecorder->PlayL()"));
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent,KErrNone,CMdaAudioClipUtility::EPlaying);
	iEventChecker->SetExpectedState(EStateRecoderPlaying);
	return EPass;
	}

TVerdict CTestMmfAclntARN9008::HandleStateRecorderPlaying()
	{
	iEventChecker->SetExpectedEvent(0,EMapcPlayComplete,KErrInUse);
	iEventChecker->SetExpectedState(EStateApPlayCompleteKErrInUse);
	return EPass;
	}

TVerdict CTestMmfAclntARN9008::HandleApPlayCompleteKErrInUse()
	{
	TInt err = iTestAudioPlayer->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification : Status - %d "), err);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	// Register iTestAudioOutputStream for Notification
	err = iTestAudioOutputStream->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification : Status - %d "), err);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioOutputStream->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	// Open iTestAudioOutputStream
	iTestAudioOutputStream->Open(NULL);
	INFO_PRINTF1(_L("iTestAudioOutputStream->Open()"));
	// Set ExpectedEvent, ExpectedError, ExpectedRecorderState and change the State
	iEventChecker->SetExpectedEvent(2,EMaoscOpenComplete,KErrNone);
	iEventChecker->SetExpectedState(EStateAosOpenComplete);
	return EPass;
	}
	
TVerdict CTestMmfAclntARN9008::HandleStateAosOpenComplete()
	{
	// iFile->Read()
	TInt err = iFile.Read(iBuf); //-synchronous
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		User::Leave(err);
		}
	// iTestAudioOutputStream->WriteL()
	INFO_PRINTF1(_L("iTestAudioOutputStream->WriteL()"));
	iTestAudioOutputStream->WriteL(iBuf); //-Should get Play Error.
	// Set ExpectedEvent & ExpectedError and change the State
	iEventChecker->SetExpectedEvent(2,EMaoscBufferCopied,KErrInUse);
	iEventChecker->SetExpectedState(EStateAosBufferCopiedKErrInUse);
	return EPass;	
	}

TVerdict CTestMmfAclntARN9008::HandleStateAosBufferCopiedKErrInUse()
	{
	iEventChecker->SetExpectedEvent(2,EMaoscPlayComplete,KErrInUse);
	iEventChecker->SetExpectedState(EStateAosPlayCompleteKErrInUse);
	return EPass;
	}
	
TVerdict CTestMmfAclntARN9008::HandleStateAosPlayCompleteKErrInUse()
	{
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent,KErrNone,CMdaAudioClipUtility::EOpen);
	iEventChecker->SetExpectedState(EStateRecorderrelease);
	return EPass;
	}

TVerdict CTestMmfAclntARN9008::HandleStateRecorderrelease()
	{
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(0,EMarncResourceAvailablePlayer,KErrNone);
	iEventChecker->SetExpectedState(EStateApResourceAvailable);
	return EPass;
	}

TVerdict CTestMmfAclntARN9008::HandleStateApResourceAvailable()
	{
	// iTestAudioPlayer resumes the playback.
	iTestAudioPlayer->SetPosition(iPosition);
	INFO_PRINTF1(_L("iTestAudioPlayer->SetPosition()"));
	iTestAudioPlayer->Play();
	INFO_PRINTF1(_L("iTestAudioPlayer->Play()"));
	// Cancel the Notification request for iTestAudioPlayer
	TInt err = iTestAudioPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF2(_L("iTestAudioPlayer->CancelRegisterAudioResourceNotification : Return Code = %d"), err);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer->CancelRegisterAudioResourceNotification - Failed with Error : %d"), err);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		return EFail;
		}
	// Play iTestAudioRecorder
	iTestAudioRecorder->PlayL();
	INFO_PRINTF1(_L("iTestAudioRecorder->PlayL()"));
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent,KErrNone,CMdaAudioClipUtility::EPlaying);
	iEventChecker->SetExpectedState(EStateRecorderResumePlay);
	return EPass;
	}
TVerdict CTestMmfAclntARN9008::HandleStateRecorderResumePlay()
	{
	iEventChecker->RegisterClientEventToMonitor(0,EMapcPlayComplete,KErrInUse,EStateApPlayComplete);
	iEventChecker->RegisterClientEventToMonitor(2,EMarncResourceAvailableOS,KErrNone,EStateAosResourceAvailable);
	iEventChecker->EnableMultipleEventMonitor();
	return EPass;
	}
TVerdict CTestMmfAclntARN9008::HandleStateApPlayComplete()
	{
	INFO_PRINTF1(_L("AudioPlayer->PlayComplete"));
	if(iEventChecker->IsMonitoringFinished())
		{
		iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EOpen);
		iEventChecker->SetExpectedState(EStateRecorderPlayComplete);
		}
	else
		{
		iEventChecker->EnableMultipleEventMonitor();
		}
	return EPass;		
	}

TVerdict CTestMmfAclntARN9008::HandleStateAosResourceAvailable()
	{
	INFO_PRINTF1(_L("AudioPlayer->PlayComplete"));
	if(iEventChecker->IsMonitoringFinished())
		{
		iEventChecker->SetExpectedEvent(1,EMoscoStateChangeEvent, KErrNone, CMdaAudioClipUtility::EOpen);
		iEventChecker->SetExpectedState(EStateRecorderPlayComplete);
		}
	else
		{
		iEventChecker->EnableMultipleEventMonitor();
		}
	return EPass;	
	}

TVerdict CTestMmfAclntARN9008::HandleStateRecorderPlayComplete()
	{
	INFO_PRINTF1(_L("AudioRecorder->Complete"));
	iEventChecker->StopTest();
	return EPass;
	}

TVerdict CTestMmfAclntARN9008::PerformTestL()
	{
	INFO_PRINTF1(_L("Receipt of Notification (on cancellation of notification of a client that has just received notification) of a Resource Available Event."));
	INFO_PRINTF1(_L("[Cancel after receipt of Notification]"));
	iTestStepResult = EFail;
		
	// Initialise the handler functions
	InitTest();
	
	// Initiate the process
	StartTestL();
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

CMultiEventChecker* CMultiEventChecker::NewL()
	{
	CMultiEventChecker* s = new(ELeave) CMultiEventChecker();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CMultiEventChecker::CMultiEventChecker()
	{		
	}
	
void CMultiEventChecker::ConstructL()
	{
	iExpectedRecorderState = CMdaAudioClipUtility::ENotReady;	
	iMonitorMultipleEvents = EFalse;
	}

void CMultiEventChecker::SetExpectedState(TInt aState)
	{
	iNextState = aState;
	}
	
void CMultiEventChecker::StopTest()
	{ 
	CActiveScheduler::Stop();
	}

void CMultiEventChecker::SetExpectedEvent(TInt aClientNumber, TInt aExpectedEvent, TInt aExpectedError,CMdaAudioClipUtility::TState aRecorderState)
	{
	iClientEventsToMonitor[aClientNumber]= aExpectedEvent;
	iClientError[aClientNumber] = aExpectedError;
	iExpectedRecorderState = aRecorderState;
	}
	
void CMultiEventChecker::RegisterClientEventToMonitor(TInt aClientNumber, TInt aEventCode, TInt aError, TInt aFsmState,CMdaAudioClipUtility::TState aRecorderState)
	{
	iClientsRegistered++;
	iClientEventsToMonitor[aClientNumber] = aEventCode;
	iClientError[aClientNumber] = aError;
	iFsmStateforClient[aClientNumber] = aFsmState;
	iExpectedRecorderState = aRecorderState;
	}

TBool CMultiEventChecker::IsMonitoringFinished()
	{
	 if(iClientsRegistered == 0)
	 	{
	 	return ETrue;
	 	}
	 else
	 	{
	 	return EFalse;
	 	}
	}
	
TBool CMultiEventChecker::CheckFsm(TInt aClientNumber,TClientUtilityType aClientType, TInt aEventCode, TInt aError,CMdaAudioClipUtility::TState aRecorderState)
	{
	TBool retFsmCheck = ETrue;
	if (iMonitorMultipleEvents)
		{
		if(iClientEventsToMonitor[aClientNumber] == aEventCode && iClientError[aClientNumber] == aError )
			{
			SetExpectedState(iFsmStateforClient[aClientNumber]);
			}
		else
			{
			retFsmCheck = EFalse;
			}
		
		if(retFsmCheck)
			{
			//Client event has finished
			iClientsRegistered--;
			}
		iMonitorMultipleEvents = EFalse;
		}
	else
		{
		if(iClientEventsToMonitor[aClientNumber] != aEventCode)
			{
			retFsmCheck = EFalse;
			}
		else if(iClientError[aClientNumber] != aError)
			{
			retFsmCheck = EFalse;
			} 
		else if (aClientType == EAudioRecorder)
			{
			if (iExpectedRecorderState != aRecorderState)
				{
				retFsmCheck = EFalse;		
				}
			}
		}
	return retFsmCheck;
	}
