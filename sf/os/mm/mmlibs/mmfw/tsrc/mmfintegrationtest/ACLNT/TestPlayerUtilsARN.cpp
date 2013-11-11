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
// CPP file: Audio Resource Notification tests for Audio Player.
// 
//

/**
 @file TestPlayerUtilsARN.cpp
*/

#include "TestAudioClientUtilitiesARN.h"

//------------------------------------------------------------------------------
//********************PREQ 797 - Audio Resource Notification********************
//------------------------------------------------------------------------------

//
//CTestMdaAudioPlayerCallbackSupport
//

/**
 * NewL
 */
CTestMdaAudioPlayerCallbackSupport* CTestMdaAudioPlayerCallbackSupport::NewL(MTestAudioPlayerCallback& aCallback)
	{
	return new(ELeave) CTestMdaAudioPlayerCallbackSupport(aCallback);
	}

/**
 * MapcInitComplete
 */
void CTestMdaAudioPlayerCallbackSupport::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
	{
	iCallback.MapcInitCompleteTest(aError, aDuration, *this);
	}

/**
 * MapcPlayComplete
 */
void CTestMdaAudioPlayerCallbackSupport::MapcPlayComplete(TInt aError)
	{
	iCallback.MapcPlayCompleteTest(aError, *this);
	}

/**
 * MarncResourceAvailable
 */
void CTestMdaAudioPlayerCallbackSupport::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	iCallback.MarncResourceAvailableTest(aNotificationEventId, aNotificationData, *this);
	}

/**
 * Constructor
 */
CTestMdaAudioPlayerCallbackSupport::CTestMdaAudioPlayerCallbackSupport(MTestAudioPlayerCallback& aCallback):iCallback(aCallback)
	{

	}

//
//CTestMmfAclntAudioPlayerARN
//

/**
 * Constructor
 */
CTestMmfAclntAudioPlayerARN::CTestMmfAclntAudioPlayerARN(const TDesC& aTestName, const TDesC& aSectName)
	{
	// store the name of this test case
	iTestStepName=aTestName;
	iHeapSize = 3600000;
	iSectName = aSectName;
	}


/**
 * MapcInitCompleteTest
 */
void CTestMmfAclntAudioPlayerARN::MapcInitCompleteTest(TInt aError, const TTimeIntervalMicroSeconds& aDuration, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport)
	{
	iDuration = aDuration;
	if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport1)
		{
		INFO_PRINTF2(_L("iAudioPlayer1 -> MMdaAudioPlayerCallback InitComplete (%d)"),aError);
		TRAP(iError, FsmL(EMapcInitComplete1, aError));
		}
	else if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport2)
		{
		INFO_PRINTF2(_L("iAudioPlayer2 -> MMdaAudioPlayerCallback InitComplete (%d)"),aError);
		TRAP(iError, FsmL(EMapcInitComplete2, aError));
		}	
	else if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport3)
		{
		INFO_PRINTF2(_L("iAudioPlayer3 -> MMdaAudioPlayerCallback InitComplete (%d)"),aError);
		TRAP(iError, FsmL(EMapcInitComplete3, aError));
		}
	}


/**
 * MapcPlayCompleteTest
 */
void CTestMmfAclntAudioPlayerARN::MapcPlayCompleteTest(TInt aError, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport)
	{
	if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport1)
		{
		INFO_PRINTF2(_L("iAudioPlayer1 -> MMdaAudioPlayerCallback PlayComplete (%d)"), aError);
		TRAP(iError, FsmL(EMapcPlayComplete1, aError));
		}
	else if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport2)
		{
		INFO_PRINTF2(_L("iAudioPlayer2 -> MMdaAudioPlayerCallback PlayComplete (%d)"), aError);
		TRAP(iError, FsmL(EMapcPlayComplete2, aError));
		}
	else if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport3)
		{
		INFO_PRINTF2(_L("iAudioPlayer3 -> MMdaAudioPlayerCallback PlayComplete (%d)"), aError);
		TRAP(iError, FsmL(EMapcPlayComplete3, aError));
		}
	}


/**
 * MarncResourceAvailableTest
 */
void CTestMmfAclntAudioPlayerARN::MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport)
	{
	if (aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		INFO_PRINTF1(_L("iAudioPlayer::MMMFAudioResourceNotificationCallback -> KMMFEventCategoryAudioResourceAvailable"));

		//Convert TDesc8 to TInt64
		TMMFTimeIntervalMicroSecondsPckg timeIntvbuf;
		timeIntvbuf.Copy(aNotificationData);
		TTimeIntervalMicroSeconds intPos = timeIntvbuf();
		iPosition = intPos;
		if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport1)
			{
			INFO_PRINTF1(_L("iAudioPlayer1 -> MMMFAudioResourceNotificationCallback : AudioResourceAvailable..waiting for playcomplete"));
			TRAP(iError, FsmL(EMarncResourceAvailable1, KErrNone));
			}
		else if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport2)
			{
			INFO_PRINTF1(_L("iAudioPlayer2 -> MMMFAudioResourceNotificationCallback : AudioResourceAvailable"));
			TRAP(iError, FsmL(EMarncResourceAvailable2, KErrNone));
			}
		else if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport3)
			{
			INFO_PRINTF1(_L("iAudioPlayer3 -> MMMFAudioResourceNotificationCallback : AudioResourceAvailable"));
			TRAP(iError, FsmL(EMarncResourceAvailable3, KErrNone));
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFAudioResourceNotificationCallback, but NOT KMMFEventCategoryAudioResourceAvailable"));
		}
	}

/**
 * DoTestStepPreambleL
 */
TVerdict CTestMmfAclntAudioPlayerARN::DoTestStepPreambleL()
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
	
	iEventChecker = CMultiEventChecker::NewL();
	// Initialise 1st Audio Player
	INFO_PRINTF1(_L("Initialise CMdaAudioPlayerUtility1 : EMdaPriorityNormal"));
	iMdaAudioPlayerCallbackSupport1 = CTestMdaAudioPlayerCallbackSupport::NewL(*this);
	iTestAudioPlayer1 = CMdaAudioPlayerUtility::NewL(*iMdaAudioPlayerCallbackSupport1, EMdaPriorityNormal);
	
	// Get 2nd file name
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile2"), filename))
		{
		return EInconclusive;
		}
	GetDriveName(iFilename2);
	iFilename2.Append(filename);

	// Initialise 2nd Audio Player
	INFO_PRINTF1(_L("Initialise CMdaAudioPlayerUtility2 : EMdaPriorityMax"));
	iMdaAudioPlayerCallbackSupport2 = CTestMdaAudioPlayerCallbackSupport::NewL(*this);
	iTestAudioPlayer2 = CMdaAudioPlayerUtility::NewL(*iMdaAudioPlayerCallbackSupport2, EMdaPriorityMax);
	
	// Get 3rd file name
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile3"), filename))
		{
		return EInconclusive;
		}
	GetDriveName(iFilename3);
	iFilename3.Append(filename);

	// Initialise 3rd Audio Player
	INFO_PRINTF1(_L("Initialise CMdaAudioPlayerUtility3 : EMdaPriorityMin"));
	iMdaAudioPlayerCallbackSupport3 = CTestMdaAudioPlayerCallbackSupport::NewL(*this);
	iTestAudioPlayer3 = CMdaAudioPlayerUtility::NewL(*iMdaAudioPlayerCallbackSupport3, EMdaPriorityMin);
		
	// Initialise the CCallBackTimer.
	iCallBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	
	return ret;
	}

/**
 * DoTestStepPostambleL
 */
TVerdict CTestMmfAclntAudioPlayerARN::DoTestStepPostambleL()
	{
	// Delete all the iTestAudioPlayer utilities.
	iTestAudioPlayer1->Close();
	delete iTestAudioPlayer1;
	iTestAudioPlayer1=NULL;
	iTestAudioPlayer2->Close();
	delete iTestAudioPlayer2;
	iTestAudioPlayer2=NULL;
	iTestAudioPlayer3->Close();
	delete iTestAudioPlayer3;
	iTestAudioPlayer3=NULL;
	// Delete CTestMdaAudioPlayerCallbackSupport
	delete iMdaAudioPlayerCallbackSupport1;
	delete iMdaAudioPlayerCallbackSupport2;
	delete iMdaAudioPlayerCallbackSupport3;
	// Delete the CCallBackTimer.
	delete iCallBackTimer;
	iCallBackTimer=NULL;
	delete iEventChecker;
	// Base Postamble.
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * DoTestStepL
 */
TVerdict CTestMmfAclntAudioPlayerARN::DoTestStepL()
	{
	return( DoTestL() );
	}

/**
 * StateInit
 */
void CTestMmfAclntAudioPlayerARN::StateInit()
	{
	iExpectedEvent=EIdlePlayer;
	iExpectedError=KErrNone;
	iState=EStateBegin;
	}

/**
 * StateChange
 */
void CTestMmfAclntAudioPlayerARN::StateChange(TMmfAudioPlayerState aState, TMmfAudioPlayerEvents aExpectedEvent, TInt aExpectedError)
	{
	iExpectedEvent=aExpectedEvent;
	iExpectedError=aExpectedError;
	iState=aState;
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntAudioPlayerARN::FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if (((aEventCode == EMapcPlayComplete1) || (aEventCode == EMapcPlayComplete2)) && (aError == KErrUnderflow))
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
//CTestMmfAclntARN8101
//

/**
 * Constructor
 */
CTestMmfAclntARN8101::CTestMmfAclntARN8101(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8101* CTestMmfAclntARN8101::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8101* self = new (ELeave) CTestMmfAclntARN8101(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8101::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8101::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenPlayer2, EMapcInitComplete1);
				break;
			case EStateOpenPlayer2:
				// Register iTestAudioPlayer#1 for Notification
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioPlayer#2
				iTestAudioPlayer2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying2, EMapcInitComplete2);
				break;
			case EStatePlaying2:
				// Play iTestAudioPlayer#2
				iTestAudioPlayer2->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateEndPlay2, EMapcPlayComplete2);
				break;
			case EStateEndPlay2:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				iTestStepResult = EPass;
				// Set ExpectedEvent and change the State
				StateChange(EStateNotifiedPlayer1, EMarncResourceAvailable1);
				break;
			case EStateNotifiedPlayer1:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived"));
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
			}
		}
	}
	
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8101::PerformTestL()
	{
	INFO_PRINTF1(_L("Register for notification, but make no request to play audio"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8102
//

/**
 * Constructor
 */
CTestMmfAclntARN8102::CTestMmfAclntARN8102(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8102* CTestMmfAclntARN8102::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8102* self = new (ELeave) CTestMmfAclntARN8102(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8102::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8102::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenPlayer2, EMapcInitComplete1);
				break;
			case EStateOpenPlayer2:
				// Register iTestAudioPlayer#1 for Notification
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioPlayer#2
				iTestAudioPlayer2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying2, EMapcInitComplete2);
				break;
			case EStatePlaying2:
				// Play iTestAudioPlayer#2
				iTestAudioPlayer2->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUsePlayer1, EMapcPlayComplete1, KErrInUse);
				break;
			case EStateErrInUsePlayer1:
				// Close iTestAudioPlayer#2
				WaitWithTimeout(iRequestStatus,1000000);
				iTestAudioPlayer2->Stop();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Stop()"));
				iTestAudioPlayer2->Close();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Close()"));
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EStateNotifiedPlayer1, EMarncResourceAvailable1);
				break;
			case EStateNotifiedPlayer1:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8102::PerformTestL()
	{
	INFO_PRINTF1(_L("Register (b4 play, after Open) and Receive Notification of a Resource Available Event."));
	INFO_PRINTF1(_L("(when audio resource is currently unavailable and then available)"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8103
//

/**
 * Constructor
 */
CTestMmfAclntARN8103::CTestMmfAclntARN8103(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8103* CTestMmfAclntARN8103::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8103* self = new (ELeave) CTestMmfAclntARN8103(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8103::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN8103::FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if (((aEventCode == EMapcPlayComplete1) || (aEventCode == EMapcPlayComplete2)) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
		if ((aEventCode == EMapcPlayComplete2))
	        {
	        retFsmCheck = ETrue;
	        if (iEventOccured == EMarncResourceAvailable1)
	            {
	            return retFsmCheck;
	            }
            else
                {
                iEventOccured = EMapcPlayComplete2;
                }
	        }
        else if (aEventCode == EMarncResourceAvailable1)
            {
            retFsmCheck = ETrue;
            if (iEventOccured == EMapcPlayComplete2)
                {
                return retFsmCheck;
                }
            else
                {
                iEventOccured = EMarncResourceAvailable1;
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
	else
		{
		retFsmCheck = ETrue;
		}
	return retFsmCheck;
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8103::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying1, EMapcInitComplete1);
				break;
			case EStatePlaying1:
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Register iTestAudioPlayer#1 for Notification
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioPlayer#2
				iTestAudioPlayer2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying2, EMapcInitComplete2);
				break;
			case EStatePlaying2:
				// Play iTestAudioPlayer#2				
				iTestAudioPlayer2->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUsePlayer1, EMapcPlayComplete1, KErrInUse);
				break;
			case EStateErrInUsePlayer1:
				// Set ExpectedEvent and change the State
				StateChange(EStateEndPlay2, EMapcPlayComplete2);
				break;
			case EStateEndPlay2:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EStateNotifiedPlayer1, EMarncResourceAvailable1);
				break;
			case EStateNotifiedPlayer1:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}
	
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8103::PerformTestL()
	{
	INFO_PRINTF1(_L("Register (at the time of Play) and Receive Notification of a Resource Available Event."));
	INFO_PRINTF1(_L("(when audio resource is currently available and subsequently unavailable, and then available)"));
	iTestStepResult = EFail;

	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8104
//

/**
 * Constructor
 */
CTestMmfAclntARN8104::CTestMmfAclntARN8104(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8104* CTestMmfAclntARN8104::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8104* self = new (ELeave) CTestMmfAclntARN8104(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8104::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8104::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt canErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying1, EMapcInitComplete1);
				break;
			case EStatePlaying1:
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Open iTestAudioPlayer#2
				iTestAudioPlayer2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying2, EMapcInitComplete2);
				break;
			case EStatePlaying2:
				// Play iTestAudioPlayer#2
				iTestAudioPlayer2->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUsePlayer1, EMapcPlayComplete1, KErrInUse);
				break;
			case EStateErrInUsePlayer1:
				// Cancel notification for iTestAudioPlayer#1
				canErr = iTestAudioPlayer1->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->CancelRegisterAudioResourceNotification()"));
				if( canErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->CancelRegisterAudioResourceNotification - Failed with Error : %d"), canErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Close iTestAudioPlayer#2
				WaitWithTimeout(iRequestStatus,1000000);
				iTestAudioPlayer2->Stop();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Stop()"));
				iTestAudioPlayer2->Close();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Close()"));
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				iTestStepResult = EPass;
				// Set ExpectedEvent and change the State
				StateChange(EStateNotifiedPlayer1, EMarncResourceAvailable1);
				break;
			case EStateNotifiedPlayer1:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived"));
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
			}
		}
	}
	
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8104::PerformTestL()
	{
	INFO_PRINTF1(_L("Registration (b4 File Open) and Cancellation of  the notification request"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Register iTestAudioPlayer#1 for Notification
	TInt retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()"));
	if( retErr != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
		return EFail;
		}
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8105
//

/**
 * Constructor
 */
CTestMmfAclntARN8105::CTestMmfAclntARN8105(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8105* CTestMmfAclntARN8105::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8105* self = new (ELeave) CTestMmfAclntARN8105(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8105::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN8105::FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if (((aEventCode == EMapcPlayComplete1) || (aEventCode == EMapcPlayComplete2)) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
		if ((aEventCode == EMapcPlayComplete2))
	        {
	        retFsmCheck = ETrue;
	        if (iEventOccured == EMarncResourceAvailable1)
	            {
	            return retFsmCheck;
	            }
            else
                {
                iEventOccured = EMapcPlayComplete2;
                }
	        }
        else if (aEventCode == EMarncResourceAvailable1)
            {
            retFsmCheck = ETrue;
            if (iEventOccured == EMapcPlayComplete2)
                {
                return retFsmCheck;
                }
            else
                {
                iEventOccured = EMarncResourceAvailable1;
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
	else
		{
		retFsmCheck = ETrue;
		}
	return retFsmCheck;
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8105::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying1, EMapcInitComplete1);
				break;
			case EStatePlaying1:
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Open iTestAudioPlayer#2
				iTestAudioPlayer2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying2, EMapcInitComplete2);
				break;
			case EStatePlaying2:
				// Play iTestAudioPlayer#2
				iTestAudioPlayer2->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUsePlayer1, EMapcPlayComplete1, KErrInUse);
				break;
			case EStateErrInUsePlayer1:
				//Get Position of iTestAudioPlayer#1
				retErr = iTestAudioPlayer1->GetPosition(iPositionStop);
				INFO_PRINTF1(_L("iTestAudioPlayer1->GetPosition()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->GetPosition - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				INFO_PRINTF2(_L("value of iPositionStop = %d"),iPositionStop.Int64());//Statement Changed under DEF105143

				// Register iTestAudioPlayer#1 for Notification
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateEndPlay2, EMapcPlayComplete2);
				break;
			case EStateEndPlay2:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EStateNotifiedPlayer1, EMarncResourceAvailable1);
				break;
			case EStateNotifiedPlayer1:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived"));
				iPositionResume = iPosition;
				CActiveScheduler::Stop();
				// Check for the Position. 
				if (iPositionResume >= iPositionStop)
					{
					INFO_PRINTF3(_L("iTestAudioPlayer1->Audio Resource iPositionResume >= iPositionStop, Expected=%Ld, Actual=%Ld"), iPositionStop.Int64(), iPositionResume.Int64());
					iTestStepResult=EPass;
					}
				else
					{
					ERR_PRINTF3(_L("iTestAudioPlayer1->Audio Resource Position is  NOT iPositionResume >= iPositionStop, Expected=%Ld, Actual=%Ld"), iPositionStop.Int64(), iPositionResume.Int64());
					iTestStepResult=EFail;
					}
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8105::PerformTestL()
	{
	INFO_PRINTF1(_L("Register (after Play Error) and Get the actual position from where the paused audio can be resumed. Low priority client is played first."));
	iTestStepResult = EFail;

	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8106
//

/**
 * Constructor
 */
CTestMmfAclntARN8106::CTestMmfAclntARN8106(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8106* CTestMmfAclntARN8106::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8106* self = new (ELeave) CTestMmfAclntARN8106(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8106::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN8106::FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if (((aEventCode == EMapcPlayComplete1) || (aEventCode == EMapcPlayComplete2)) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
		if ((aEventCode == EMapcPlayComplete2))
	        {
	        retFsmCheck = ETrue;
	        if (iEventOccured == EMarncResourceAvailable1)
	            {
	            return retFsmCheck;
	            }
            else
                {
                iEventOccured = EMapcPlayComplete2;
                }
	        }
        else if (aEventCode == EMarncResourceAvailable1)
            {
            retFsmCheck = ETrue;
            if (iEventOccured == EMapcPlayComplete2)
                {
                return retFsmCheck;
                }
            else
                {
                iEventOccured = EMarncResourceAvailable1;
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
	else
		{
		retFsmCheck = ETrue;
		}
	return retFsmCheck;
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8106::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#2
				iTestAudioPlayer2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying2, EMapcInitComplete2);
				break;
			case EStatePlaying2:
				// Play iTestAudioPlayer#2
				iTestAudioPlayer2->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying1, EMapcInitComplete1);
				break;
			case EStatePlaying1:
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUsePlayer1, EMapcPlayComplete1, KErrInUse);
				break;
			case EStateErrInUsePlayer1:
				//Get Position of iTestAudioPlayer#1
				retErr = iTestAudioPlayer1->GetPosition(iPositionStop);
				INFO_PRINTF1(_L("iTestAudioPlayer1->GetPosition()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->GetPosition - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Register iTestAudioPlayer#1 for Notification
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateEndPlay2, EMapcPlayComplete2);
				break;
			case EStateEndPlay2:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EStateNotifiedPlayer1, EMarncResourceAvailable1);
				break;
			case EStateNotifiedPlayer1:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived"));
				iPositionResume = iPosition;
				CActiveScheduler::Stop();
				// Check for the Position
				if (iPositionResume >= iPositionStop)
					{
					INFO_PRINTF3(_L("iTestAudioPlayer1->Audio Resource Position Matches, Expected=%Ld, Actual=%Ld"), iPositionStop.Int64(), iPositionResume.Int64());
					iTestStepResult=EPass;
					}
				else
					{
					ERR_PRINTF3(_L("iTestAudioPlayer1->Audio Resource Position does NOT Match, Expected=%Ld, Actual=%Ld"), iPositionStop.Int64(), iPositionResume.Int64());
					iTestStepResult=EFail;
					}
				break;
			}
		}
	}
	
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8106::PerformTestL()
	{
	INFO_PRINTF1(_L("Register (after Play Error) and Get the actual position from where the paused audio can be resumed. High priority client is played first."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
		
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8107
//

/**
 * Constructor
 */
CTestMmfAclntARN8107::CTestMmfAclntARN8107(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8107* CTestMmfAclntARN8107::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8107* self = new (ELeave) CTestMmfAclntARN8107(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8107::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN8107::FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if (((aEventCode == EMapcPlayComplete1) || (aEventCode == EMapcPlayComplete2)) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
		if ((aEventCode == EMapcPlayComplete2))
	        {
	        retFsmCheck = ETrue;
	        if (iEventOccured == EMarncResourceAvailable1)
	            {
	            return retFsmCheck;
	            }
            else
                {
                iEventOccured = EMapcPlayComplete2;
                }
	        }
        else if (aEventCode == EMarncResourceAvailable1)
            {
            retFsmCheck = ETrue;
            if (iEventOccured == EMapcPlayComplete2)
                {
                return retFsmCheck;
                }
            else
                {
                iEventOccured = EMarncResourceAvailable1;
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
	else
		{
		retFsmCheck = ETrue;
		}
	return retFsmCheck;
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8107::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying1, EMapcInitComplete1);
				break;
			case EStatePlaying1:
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Register iTestAudioPlayer#1 for Notification
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioPlayer#2
				iTestAudioPlayer2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying2, EMapcInitComplete2);
				break;
			case EStatePlaying2:
				// Play iTestAudioPlayer#2
				iTestAudioPlayer2->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUsePlayer1, EMapcPlayComplete1, KErrInUse);
				break;
			case EStateErrInUsePlayer1:
				// Set ExpectedEvent and change the State
				StateChange(EStateEndPlay2, EMapcPlayComplete2);
				break;
            case EStateEndPlay2:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EStateNotifiedPlayer1, EMarncResourceAvailable1);
				break;
			case EStateNotifiedPlayer1:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived => 1"));
				retErr = iTestAudioPlayer1->WillResumePlay();
				INFO_PRINTF1(_L("iTestAudioPlayer1->WillResumePlay()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->WillResumePlay() - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				iTestAudioPlayer1->SetPosition(iPosition);
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Open iTestAudioPlayer#2
				iTestAudioPlayer2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying2A, EMapcInitComplete2);
				break;
			case EStatePlaying2A:
				// Play iTestAudioPlayer#2
				iTestAudioPlayer2->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUsePlayer1A, EMapcPlayComplete1, KErrInUse);
				break;
			case EStateErrInUsePlayer1A:
				// Set ExpectedEvent and change the State
				StateChange(EStateEndPlay2A, EMapcPlayComplete2);
				break;
            case EStateEndPlay2A:
                // Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent and change the State
				StateChange(EStateNotifiedPlayer1A, EMarncResourceAvailable1);
				break;
			case EStateNotifiedPlayer1A:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived => 2"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}
	
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8107::PerformTestL()
	{
	INFO_PRINTF1(_L("Client does not need to Re-register for the same event when the event is received i.e API is not one shot."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8108
//

/**
 * NewL
 */
CTestMmfAclntARN8108* CTestMmfAclntARN8108::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8108* self = new (ELeave) CTestMmfAclntARN8108(aTestName, aSectName);
	return self;
	}

/**
 * Constructor
 */
CTestMmfAclntARN8108::CTestMmfAclntARN8108(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}


/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8108::DoTestL()
	{
	return( PerformTestL() );
	}

void CTestMmfAclntARN8108::DecideFsmState(TInt aClientNumber,CMultiEventChecker::TClientUtilityType aClientType, TInt aEventCode, TInt aError)
	{
	if (iEventChecker->CheckFsm(aClientNumber, aClientType, aEventCode, aError))
		{
		iTestStepResult = (this->*(iStateHandlers[iEventChecker->NextState()]))();
 		if(iTestStepResult != EPass)
 			{
 			iEventChecker->StopTest();
 			}
		}
	}

void CTestMmfAclntARN8108::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (aEventCode == EMapcInitComplete1 || aEventCode == EMapcPlayComplete1 || aEventCode == EMarncResourceAvailable1)
		{	//Client 1
			DecideFsmState(1,CMultiEventChecker::EAudioPlayer,aEventCode,aError);
		}
	if (aEventCode == EMapcInitComplete2 || aEventCode == EMapcPlayComplete2 || aEventCode == EMarncResourceAvailable2)
		{	//Client 2
			DecideFsmState(2,CMultiEventChecker::EAudioPlayer,aEventCode,aError);
		}
	if (aEventCode == EMapcInitComplete3 || aEventCode == EMapcPlayComplete3 || aEventCode == EMarncResourceAvailable3)
		{	//Client 3
			DecideFsmState(3,CMultiEventChecker::EAudioPlayer,aEventCode,aError);
		}
	}


void CTestMmfAclntARN8108::InitTest()
	{
	iStateHandlers[EStateOpenPlayer1]=&CTestMmfAclntARN8108::HandleStateOpenPlayer1;
	iStateHandlers[EStateOpenPlayer3]=&CTestMmfAclntARN8108::HandleStateOpenPlayer3;
	iStateHandlers[EStateOpenPlayer2]=&CTestMmfAclntARN8108::HandleStateOpenPlayer2;
	iStateHandlers[EStateErrInUsePlayer1]=&CTestMmfAclntARN8108::HandleStateErrInUsePlayer1;
	iStateHandlers[EStateErrInUsePlayer3]=&CTestMmfAclntARN8108::HandleStateErrInUsePlayer3;
	iStateHandlers[EStateNotifiedPlayer1]=&CTestMmfAclntARN8108::HandleStateNotifiedPlayer1;
	iStateHandlers[EStateNotifiedPlayer3]=&CTestMmfAclntARN8108::HandleStateNotifiedPlayer3;
	iStateHandlers[EStatePlaying1]=&CTestMmfAclntARN8108::HandleStatePlaying1;
	}
	
void CTestMmfAclntARN8108::StartTestL()
	{
	// Open iTestAudioPlayer#1
	iTestAudioPlayer1->OpenFileL(iFilename1);
	INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
	iEventChecker->SetExpectedEvent(1,EMapcInitComplete1,KErrNone);
	iEventChecker->SetExpectedState(EStateOpenPlayer1);
	}

TVerdict CTestMmfAclntARN8108::HandleStateOpenPlayer1()
	{
	TInt retErr;
	// Play iTestAudioPlayer#1
	iTestAudioPlayer1->Play();
	INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
	// Register iTestAudioPlayer#1 for Notification
	retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()"));
	if( retErr != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		return EFail;
		}
	// Register iTestAudioPlayer#3 for Notification
	retErr = iTestAudioPlayer3->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport3, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF1(_L("iTestAudioPlayer3->RegisterAudioResourceNotification()"));
	if( retErr != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer3->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		return EFail;
		}
	// Open iTestAudioPlayer#3
	iTestAudioPlayer3->OpenFileL(iFilename3);
	INFO_PRINTF1(_L("iTestAudioPlayer3->OpenFileL()"));
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(3,EMapcInitComplete3,KErrNone);
	iEventChecker->SetExpectedState(EStateOpenPlayer3);
	return EPass;
	}
	
TVerdict CTestMmfAclntARN8108::HandleStateOpenPlayer3()
	{
	iTestAudioPlayer2->OpenFileL(iFilename2);
	INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(2,EMapcInitComplete2,KErrNone);
	iEventChecker->SetExpectedState(EStateOpenPlayer2);
	return EPass;
	}

TVerdict CTestMmfAclntARN8108::HandleStateOpenPlayer2()
	{
	// Play iTestAudioPlayer#2
	iTestAudioPlayer2->Play();
	INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(1,EMapcPlayComplete1,KErrInUse);
	iEventChecker->SetExpectedState(EStateErrInUsePlayer1);
	return EPass;
	}
TVerdict CTestMmfAclntARN8108::HandleStateErrInUsePlayer1()
	{
	// Play iTestAudioPlayer#3
	iTestAudioPlayer3->Play();
	INFO_PRINTF1(_L("iTestAudioPlayer3->Play()"));
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(3,EMapcPlayComplete3,KErrInUse);
	iEventChecker->SetExpectedState(EStateErrInUsePlayer3);
	return EPass;
	}
	
TVerdict CTestMmfAclntARN8108::HandleStateErrInUsePlayer3()
	{
	// Close iTestAudioPlayer#2
	WaitWithTimeout(iRequestStatus,1000000);
	iTestAudioPlayer2->Stop();
	INFO_PRINTF1(_L("iTestAudioPlayer2->Stop()"));
	iTestAudioPlayer2->Close();
	INFO_PRINTF1(_L("iTestAudioPlayer2->Close()"));
	// Timer to check if Notification Event Occurs.
	iCallBackTimer->After(2000000);
	// Set ExpectedEvent and change the State
	iEventChecker->SetExpectedEvent(1,EMarncResourceAvailable1,KErrNone);
	iEventChecker->SetExpectedState(EStateNotifiedPlayer1);
	return EPass;
	}

TVerdict CTestMmfAclntARN8108::HandleStateNotifiedPlayer1()
	{
	iCallBackTimer->Cancel(); // in case something else stopped the AS
	INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived"));
	TInt retErr = iTestAudioPlayer1->WillResumePlay();
	INFO_PRINTF1(_L("iTestAudioPlayer1->WillResumePlay()"));
	if( retErr != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioPlayer1->WillResumePlay() - Failed with Error : %d"), retErr);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		return EFail;
		}
	iTestAudioPlayer1->SetPosition(iPosition);
	// Play iTestAudioPlayer#1
	iTestAudioPlayer1->Play();
	INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
	// Set ExpectedEvent and change the State
	iEventChecker->RegisterClientEventToMonitor(1,EMapcPlayComplete1,KErrNone,EStateNotifiedPlayer3);
	iEventChecker->RegisterClientEventToMonitor(3,EMarncResourceAvailable3,KErrNone,EStatePlaying1);
	iEventChecker->EnableMultipleEventMonitor();
	return EPass;
	}
	
TVerdict CTestMmfAclntARN8108::HandleStateNotifiedPlayer3()
	{
	INFO_PRINTF1(_L("iTestAudioPlayer3->NotificationReceived"));
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

TVerdict CTestMmfAclntARN8108::HandleStatePlaying1()
	{
	INFO_PRINTF1(_L("iTestAudioPlayer1->AudioPlayerComplete"));
	iCallBackTimer->After(2000000);
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

TVerdict CTestMmfAclntARN8108::PerformTestL()
	{
	INFO_PRINTF1(_L("Call WillResumePlay()."));
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
//CTestMmfAclntARN8109
//

/**
 * NewL
 */
CTestMmfAclntARN8109* CTestMmfAclntARN8109::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN8109* self = new (ELeave) CTestMmfAclntARN8109(aTestName, aSectName);
	return self;
	}

/**
 * Constructor
 */
CTestMmfAclntARN8109::CTestMmfAclntARN8109(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName), iPlayComplete2(EFalse), iResourceAvailable1(EFalse)
	{}


/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8109::DoTestL()
	{
	return( PerformTestL() );
	}
/**
 * MapcPlayCompleteTest
 */
void CTestMmfAclntARN8109::MapcPlayCompleteTest(TInt aError, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport)
	{
	if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport1)
		{
		INFO_PRINTF2(_L("iAudioPlayer1 -> MMdaAudioPlayerCallback PlayComplete (%d)"), aError);
		TRAP(iError, FsmL(EMapcPlayComplete1, aError));
		}
	else if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport2)
		{
		INFO_PRINTF2(_L("iAudioPlayer2 -> MMdaAudioPlayerCallback PlayComplete (%d)"), aError);
		iPlayComplete2= ETrue;
		TRAP(iError, FsmL(EMapcPlayComplete2, aError));
		}
	else if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport3)
		{
		INFO_PRINTF2(_L("iAudioPlayer3 -> MMdaAudioPlayerCallback PlayComplete (%d)"), aError);
		TRAP(iError, FsmL(EMapcPlayComplete3, aError));
		}
	}


/**
 * MarncResourceAvailableTest
 */
void CTestMmfAclntARN8109::MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport)
	{
	if (aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		INFO_PRINTF1(_L("iAudioPlayer::MMMFAudioResourceNotificationCallback -> KMMFEventCategoryAudioResourceAvailable"));

		//Convert TDesc8 to TInt64
		TMMFTimeIntervalMicroSecondsPckg timeIntvbuf;
		timeIntvbuf.Copy(aNotificationData);
		TTimeIntervalMicroSeconds intPos = timeIntvbuf();
		iPosition = intPos;
		if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport1)
			{
			INFO_PRINTF1(_L("iAudioPlayer1 -> MMMFAudioResourceNotificationCallback : AudioResourceAvailable..waiting for playcomplete"));
			iResourceAvailable1 = ETrue;
			TRAP(iError, FsmL(EMarncResourceAvailable1, KErrNone));
			}
		else if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport2)
			{
			INFO_PRINTF1(_L("iAudioPlayer2 -> MMMFAudioResourceNotificationCallback : AudioResourceAvailable"));
			TRAP(iError, FsmL(EMarncResourceAvailable2, KErrNone));
			}
		else if (&aMdaAudioPlayerCallbackSupport == iMdaAudioPlayerCallbackSupport3)
			{
			INFO_PRINTF1(_L("iAudioPlayer3 -> MMMFAudioResourceNotificationCallback : AudioResourceAvailable"));
			TRAP(iError, FsmL(EMarncResourceAvailable3, KErrNone));
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFAudioResourceNotificationCallback, but NOT KMMFEventCategoryAudioResourceAvailable"));
		}
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN8109::FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if (((aEventCode == EMapcPlayComplete1) || (aEventCode == EMapcPlayComplete2)) && (aError == KErrUnderflow))
		{
		INFO_PRINTF1(_L("Underflow !"));
		}
	else if (iExpectedEvent != aEventCode)
		{
		if(aEventCode == EMarncResourceAvailable1)
            {
            retFsmCheck = EFalse;
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
	else
		{
		retFsmCheck = ETrue;
		}
	return retFsmCheck;
	}
/**
 * FsmL
 */
void CTestMmfAclntARN8109::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TTimeIntervalMicroSeconds silence(0); // 3 sec of silence
		TInt retErr = KErrNone;
		switch (iState)
			{
			
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying1, EMapcInitComplete1);
				break;
			case EStatePlaying1:
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Register iTestAudioPlayer#1 for Notification
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioPlayer#2
				iTestAudioPlayer2->OpenFileL(iFilename2);
				INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStatePlaying2, EMapcInitComplete2);
				break;
			case EStatePlaying2:
				// Play iTestAudioPlayer#2
				iDuration = I64INT(iTestAudioPlayer2->Duration().Int64());
				INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
				INFO_PRINTF2( _L("Repeats = %d\n"),NUMBER_OF_REPEATS);
				iTestAudioPlayer2->SetRepeats(NUMBER_OF_REPEATS,silence);
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
								
				iTestAudioPlayer2->Play();
				iStart.HomeTime();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUsePlayer1, EMapcPlayComplete1, KErrInUse);
				break;
			case EStateErrInUsePlayer1:
				// Set ExpectedEvent and change the State
				StateChange(EStateEndPlay2, EMapcPlayComplete2);
				break;
			case EStateEndPlay2:
				iStop.HomeTime();
				iActualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(NUMBER_OF_REPEATS));
				if(iPlayComplete2 && iResourceAvailable1)
					{	
					#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
					if(TimeComparison(iActualDuration, iDuration * (NUMBER_OF_REPEATS + 1),
													   KExpectedDeviation))
						{
						iCallBackTimer->Cancel(); // in case something else stopped the AS
						INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived"));
						iTestStepResult = EPass;
						CActiveScheduler::Stop();
						}
					else
						{
						iTestStepResult = EFail;
						CActiveScheduler::Stop();
						}
					#else
					if(TimeComparison(iActualDuration, iDuration * (NUMBER_OF_REPEATS + 1),
													   KExpectedDeviation*NUMBER_OF_REPEATS))
						{
						iCallBackTimer->Cancel(); // in case something else stopped the AS
						INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived"));
						iTestStepResult = EPass;
						CActiveScheduler::Stop();
						// Timer to check if Notification Event Occurs.
						}
					else
						{
						iTestStepResult = EFail;
						CActiveScheduler::Stop();
						}
					#endif	
					}
				else if(iPlayComplete2 && ! iResourceAvailable1)
					{
					#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
					if(TimeComparison(iActualDuration, iDuration * (NUMBER_OF_REPEATS + 1),
													   KExpectedDeviation))
						{
						// Timer to check if Notification Event Occurs.
						iCallBackTimer->After(2000000);
						// Set ExpectedEvent and change the State
						StateChange(EStateNotifiedPlayer1, EMarncResourceAvailable1);
						}
					else
						{
						iTestStepResult = EFail;
						CActiveScheduler::Stop();
						}
					#else
					if(TimeComparison(iActualDuration, iDuration * (NUMBER_OF_REPEATS + 1),
													   KExpectedDeviation*NUMBER_OF_REPEATS))
						{
						// Timer to check if Notification Event Occurs.
						iCallBackTimer->After(2000000);
						// Set ExpectedEvent and change the State
						StateChange(EStateNotifiedPlayer1, EMarncResourceAvailable1);
						// Timer to check if Notification Event Occurs.
						}
					else
						{
						iTestStepResult = EFail;
						CActiveScheduler::Stop();
						}
					#endif	
					}	
				break;
			case EStateNotifiedPlayer1:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioPlayer1->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}
TBool CTestMmfAclntARN8109::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
	{
	// save unnessary conditions
	if(aActual == aExpected)
		return ETrue;	

	// Prevent unsigned wrapping errors 
	TUint difference;
	if(aActual > aExpected)
		difference = aActual - aExpected;
	else
		difference = aExpected - aActual;

	// comapare
	if(difference < aDeviation)
		return ETrue;
	return EFalse;
	}
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8109::PerformTestL()
	{
	INFO_PRINTF1(_L("High priority client plays with repeats and low priority client doesn't get resources."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}


//
//CTestMmfAclntARN8110 //test to cover CR id: 
//

/**
 * Constructor
 */
CTestMmfAclntARN8110::CTestMmfAclntARN8110(const TDesC& aTestName, const TDesC& aSectName)
    : CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
    {}

/**
 * NewL
 */
CTestMmfAclntARN8110* CTestMmfAclntARN8110::NewL(const TDesC& aTestName, const TDesC& aSectName)
    {
    CTestMmfAclntARN8110* self = new (ELeave) CTestMmfAclntARN8110(aTestName, aSectName);
    return self;
    }

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8110::DoTestL()
    {
    return( PerformTestL() );
    }

/**
 * FsmL
 */
void CTestMmfAclntARN8110::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
    {
    if (FsmCheck(aEventCode, aError))
        {
        TTimeIntervalMicroSeconds32 wait2secs(2000000);
        TTimeIntervalMicroSeconds oneNhalf(MAKE_TINT64(0,1500000));
        TInt retErr = KErrNone;
        switch (iState)
            {
            case EStateBegin:
                // Open iTestAudioPlayer#1 // next event to play iTestAudioPlayer#1
                iTestAudioPlayer1->OpenFileL(iFilename1);
                INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
                // Set ExpectedEvent and change the State
                StateChange(EStatePlaying1, EMapcInitComplete1);
                break;
            case EStatePlaying1:
                // Play iTestAudioPlayer#1
                iTestAudioPlayer1->Play();
                INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
                // Open iTestAudioPlayer#2
                 iTestAudioPlayer2->OpenFileL(iFilename2);
                 INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
                 // Set ExpectedEvent and change the State
                 StateChange(EStatePlaying2, EMapcInitComplete2);
                 break;
            case EStatePlaying2:
                // Play iTestAudioPlayer#2
                User::After(wait2secs);
                iTestAudioPlayer2->Play();
                INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
                // Set ExpectedEvent and change the State
                StateChange(EStateErrInUsePlayer1, EMapcPlayComplete1, KErrInUse);
                break;
            case EStateErrInUsePlayer1:
                retErr = iTestAudioPlayer1->GetPosition(iPlayer1Pos);
                if(retErr != KErrNone)
                    {
                    ERR_PRINTF2(_L("iTestAudioPlayer1->Position() - Failed with Error : %d"), retErr);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                INFO_PRINTF1(_L("iTestAudioPlayer1->Position()"));
                //using 1.5 sec for comparison as sometimes playback is slow.
                if(iPlayer1Pos < oneNhalf)
                    {
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                // Set ExpectedEvent and change the State
                StateChange(EStateEndPlay2, EMapcPlayComplete2 );
                break;
            case EStateEndPlay2:
                iTestStepResult = EPass;
                // Play iTestAudioPlayer#1 again
                TTimeIntervalMicroSeconds pos;
                INFO_PRINTF1(_L("iTestAudioPlayer1->Position()"));
                retErr = iTestAudioPlayer1->GetPosition(pos);
                if(retErr != KErrNone)
                    {
                    ERR_PRINTF2(_L("iTestAudioPlayer1->Position() - Failed with Error : %d"), retErr);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                if(pos == 0 || pos != iPlayer1Pos )
                    {
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                iTestAudioPlayer1->Play();
                INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
                retErr = iTestAudioPlayer1->GetPosition(pos);
                if(retErr != KErrNone)
                    {
                    ERR_PRINTF2(_L("iTestAudioPlayer1->Position() - Failed with Error : %d"), retErr);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                INFO_PRINTF1(_L("iTestAudioPlayer1->Position()"));
                if(pos < iPlayer1Pos)
                    {
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                iTestAudioPlayer1->Stop();
                INFO_PRINTF1(_L("iTestAudioPlayer1->Stop()"));
                iTestStepResult = EPass;
                CActiveScheduler::Stop();
                // Set ExpectedEvent and change the State
                break;
            }
        }
    }
    
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8110::PerformTestL()
    {
    INFO_PRINTF1(_L("Register for notification, but make no request to play audio"));
    iTestStepResult = EFail;
    
    // Initialise the state variables
    StateInit();
    
    // Begin the process
    FsmL(EIdlePlayer, KErrNone);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }


//
//CTestMmfAclntARN8111 //test to cover CR id: 
//

/**
 * Constructor
 */
CTestMmfAclntARN8111::CTestMmfAclntARN8111(const TDesC& aTestName, const TDesC& aSectName)
    : CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
    {}

/**
 * NewL
 */
CTestMmfAclntARN8111* CTestMmfAclntARN8111::NewL(const TDesC& aTestName, const TDesC& aSectName)
    {
    CTestMmfAclntARN8111* self = new (ELeave) CTestMmfAclntARN8111(aTestName, aSectName);
    return self;
    }

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8111::DoTestL()
    {
    return( PerformTestL() );
    }

/**
 * FsmL
 */
void CTestMmfAclntARN8111::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
    {
    if (FsmCheck(aEventCode, aError))
        {
        TTimeIntervalMicroSeconds32 wait2secs(2000000);
        TTimeIntervalMicroSeconds oneNhalf(MAKE_TINT64(0,1500000));
        TTimeIntervalMicroSeconds zeroSecs(MAKE_TINT64(0,0));
        TInt retErr = KErrNone;
        switch (iState)
            {
            case EStateBegin:
                // Open iTestAudioPlayer#1 // next event to play iTestAudioPlayer#1
                iTestAudioPlayer1->OpenFileL(iFilename1);
                INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
                // Set ExpectedEvent and change the State
                StateChange(EStatePlaying1, EMapcInitComplete1);
                break;
            case EStatePlaying1:
                // Play iTestAudioPlayer#1
                iTestAudioPlayer1->Play();
                INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
                // Open iTestAudioPlayer#2
                 iTestAudioPlayer2->OpenFileL(iFilename2);
                 INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
                 // Set ExpectedEvent and change the State
                 StateChange(EStatePlaying2, EMapcInitComplete2);
                 break;
            case EStatePlaying2:
                // Play iTestAudioPlayer#2
                User::After(wait2secs);
                iTestAudioPlayer2->Play();
                INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
                // Set ExpectedEvent and change the State
                StateChange(EStateErrInUsePlayer1, EMapcPlayComplete1, KErrInUse);
                break;
            case EStateErrInUsePlayer1:
                retErr = iTestAudioPlayer1->GetPosition(iPlayer1Pos);
                if(retErr != KErrNone)
                    {
                    ERR_PRINTF2(_L("iTestAudioPlayer1->Position() - Failed with Error : %d"), retErr);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                INFO_PRINTF1(_L("iTestAudioPlayer1->Position()"));
                //using 1.5 sec for comparison as sometimes playback is slow.
                if(iPlayer1Pos < oneNhalf)
                    {
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                //stop the iTestAudioPlayer#1 after pre-emption and check if the position is reset.
                iTestAudioPlayer1->Stop();
                INFO_PRINTF1(_L("iTestAudioPlayer1->Stop()"));
                retErr = iTestAudioPlayer1->GetPosition(iPlayer1Pos);
                if(retErr != KErrNone)
                    {
                    ERR_PRINTF2(_L("iTestAudioPlayer1->Position() - Failed with Error : %d"), retErr);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                INFO_PRINTF1(_L("iTestAudioPlayer1->Position()"));
                if(iPlayer1Pos != zeroSecs)
                    {
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                // Set ExpectedEvent and change the State
                StateChange(EStateEndPlay2, EMapcPlayComplete2 );
                break;
            case EStateEndPlay2:
                iTestStepResult = EPass;
                // Play iTestAudioPlayer#1 again
                iTestAudioPlayer1->Play();
                INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
                User::After(wait2secs);
                iTestAudioPlayer1->Stop();
                INFO_PRINTF1(_L("iTestAudioPlayer1->Stop()"));
                CActiveScheduler::Stop();
                // Set ExpectedEvent and change the State
                break;
            }
        }
    }
    
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8111::PerformTestL()
    {
    INFO_PRINTF1(_L("Register for notification, but make no request to play audio"));
    iTestStepResult = EFail;
    
    // Initialise the state variables
    StateInit();
    
    // Begin the process
    FsmL(EIdlePlayer, KErrNone);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

//
//CTestMmfAclntARN8112 //test to cover CR id: 
//

/**
 * Constructor
 */
CTestMmfAclntARN8112::CTestMmfAclntARN8112(const TDesC& aTestName, const TDesC& aSectName)
    : CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
    {}

/**
 * NewL
 */
CTestMmfAclntARN8112* CTestMmfAclntARN8112::NewL(const TDesC& aTestName, const TDesC& aSectName)
    {
    CTestMmfAclntARN8112* self = new (ELeave) CTestMmfAclntARN8112(aTestName, aSectName);
    return self;
    }

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8112::DoTestL()
    {
    return( PerformTestL() );
    }

/**
 * FsmL
 */
void CTestMmfAclntARN8112::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
    {
    if (FsmCheck(aEventCode, aError))
        {
        TTimeIntervalMicroSeconds32 wait1secs(1000000);
        TTimeIntervalMicroSeconds zeroSecs(MAKE_TINT64(0,0));
        TInt retErr = KErrNone;
        switch (iState)
            {
            case EStateBegin:
                // Open iTestAudioPlayer#2 // next event to play iTestAudioPlayer#2
                iTestAudioPlayer2->OpenFileL(iFilename2);
                INFO_PRINTF1(_L("iTestAudioPlayer2->OpenFileL()"));
                // Set ExpectedEvent and change the State
                StateChange(EStatePlaying2, EMapcInitComplete2);
                break;
            case EStatePlaying2:
                // Play iTestAudioPlayer#1
                iTestAudioPlayer2->Play();
                INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
                // Open iTestAudioPlayer#1
                 iTestAudioPlayer1->OpenFileL(iFilename1);
                 INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
                 // Set ExpectedEvent and change the State
                 StateChange(EStatePlaying1, EMapcInitComplete1);
                 break;
            case EStatePlaying1:
                // Play iTestAudioPlayer#1
                User::After(wait1secs);
                iTestAudioPlayer1->Play();
                INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
                // Set ExpectedEvent and change the State
                StateChange(EStateErrInUsePlayer1, EMapcPlayComplete1, KErrInUse);
                break;
            case EStateErrInUsePlayer1:
                retErr = iTestAudioPlayer1->GetPosition(iPlayer1Pos);
                if(retErr != KErrNone)
                    {
                    ERR_PRINTF2(_L("iTestAudioPlayer1->Position() - Failed with Error : %d"), retErr);
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                INFO_PRINTF1(_L("iTestAudioPlayer1->Position()"));
                if(iPlayer1Pos != zeroSecs)
                    {
                    iTestStepResult = EFail;
                    CActiveScheduler::Stop();
                    break;
                    }
                //stop the iTestAudioPlayer#1 after pre-emption and check if the position is reset.
                iTestAudioPlayer2->Stop();
                INFO_PRINTF1(_L("iTestAudioPlayer2->Stop()"));
                iTestAudioPlayer1->Play();
                INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
                
                // Set ExpectedEvent and change the State
                StateChange(EStateEndPlay1, EMapcPlayComplete1 );
                break;
            case EStateEndPlay1:
                iTestStepResult = EPass;
                // Play iTestAudioPlayer#1 again
                CActiveScheduler::Stop();
                // Set ExpectedEvent and change the State
                break;
            }
        }
    }
    
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8112::PerformTestL()
    {
    INFO_PRINTF1(_L("Register for notification, but make no request to play audio"));
    iTestStepResult = EFail;
    
    // Initialise the state variables
    StateInit();
    
    // Begin the process
    FsmL(EIdlePlayer, KErrNone);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

//
//NEGATIVE TESTS//
//

//
//CTestMmfAclntARN1301
//

/**
 * Constructor
 */
CTestMmfAclntARN1301::CTestMmfAclntARN1301(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1301* CTestMmfAclntARN1301::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN1301* self = new (ELeave) CTestMmfAclntARN1301(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1301::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1301::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenPlayer1, EMapcInitComplete1);
				break;
			case EStateOpenPlayer1:
				// Register iTestAudioPlayer#1 for Notification
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryPlaybackComplete);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification(KMMFEventCategoryPlaybackComplete)"));
				if( retErr == KErrNotSupported )
					{
					INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Returned KErrNotSupported ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
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
TVerdict CTestMmfAclntARN1301::PerformTestL()
	{
	INFO_PRINTF1(_L("Register for notification with an unsupported event type."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN1302
//

/**
 * Constructor
 */
CTestMmfAclntARN1302::CTestMmfAclntARN1302(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1302* CTestMmfAclntARN1302::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN1302* self = new (ELeave) CTestMmfAclntARN1302(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1302::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1302::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenPlayer1, EMapcInitComplete1);
				break;
			case EStateOpenPlayer1:
				// Register iTestAudioPlayer#1 for Notification
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Cancel notification for iTestAudioPlayer#1
				retErr = iTestAudioPlayer1->CancelRegisterAudioResourceNotification(KMMFEventCategoryPlaybackComplete);
				INFO_PRINTF1(_L("iTestAudioPlayer1->CancelRegisterAudioResourceNotification(KMMFEventCategoryPlaybackComplete)"));
				if( retErr == KErrNotSupported )
					{
					ERR_PRINTF1(_L("iTestAudioPlayer1->CancelRegisterAudioResourceNotification - Returned KErrNotSupported ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->CancelRegisterAudioResourceNotification - Failed with Error : %d"), retErr);
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
TVerdict CTestMmfAclntARN1302::PerformTestL()
	{
	INFO_PRINTF1(_L("Cancel the request for notification with an unsupported event type."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN1303
//

/**
 * Constructor
 */
CTestMmfAclntARN1303::CTestMmfAclntARN1303(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1303* CTestMmfAclntARN1303::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN1303* self = new (ELeave) CTestMmfAclntARN1303(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1303::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1303::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenPlayer1, EMapcInitComplete1);
				break;
			case EStateOpenPlayer1:
				// Cancel notification for iTestAudioPlayer#1
				retErr = iTestAudioPlayer1->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable)"));
				if( retErr == KErrCancel )
					{
					ERR_PRINTF1(_L("iTestAudioPlayer1->CancelRegisterAudioResourceNotification - Returned KErrCancel ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->CancelRegisterAudioResourceNotification - Failed with Error : %d"), retErr);
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
TVerdict CTestMmfAclntARN1303::PerformTestL()
	{
	INFO_PRINTF1(_L("Cancel when no resource Notification is requested."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN1304
//

/**
 * Constructor
 */
CTestMmfAclntARN1304::CTestMmfAclntARN1304(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerARN(aTestName, aSectName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1304* CTestMmfAclntARN1304::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntARN1304* self = new (ELeave) CTestMmfAclntARN1304(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1304::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1304::FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				// Open iTestAudioPlayer#1
				iTestAudioPlayer1->OpenFileL(iFilename1);
				INFO_PRINTF1(_L("iTestAudioPlayer1->OpenFileL()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenPlayer1, EMapcInitComplete1);
				break;
			case EStateOpenPlayer1:
				// Register iTestAudioPlayer#1 for Notification
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()#1"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification #1 - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Register iTestAudioPlayer#1 for Notification (AGAIN)
				retErr = iTestAudioPlayer1->RegisterAudioResourceNotification(*iMdaAudioPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification()#2"));
				if( retErr == KErrAlreadyExists )
					{
					INFO_PRINTF1(_L("iTestAudioPlayer1->RegisterAudioResourceNotification #2 - Returned KErrAlreadyExists ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioPlayer1->RegisterAudioResourceNotification #2 - Failed with Error : %d"), retErr);
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
TVerdict CTestMmfAclntARN1304::PerformTestL()
	{
	INFO_PRINTF1(_L("Multiple Registration for notification for the same event type."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}
