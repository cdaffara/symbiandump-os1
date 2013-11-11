// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Part of the TSI_MMFACLNT suite that tests CR1566 (TruePause) on AudioToneUtility
// 
//

/**
 @file ToneTruePause.cpp
*/

#include "tonetruepause.h"
#include "toneTest.h"

/*
 *
 * RMdaToneUtilityTestBase - Test step constructor
 *
 */
RMdaToneUtilityTestBase::RMdaToneUtilityTestBase(const TDesC& aTestName, const TDesC& aSectName)
	:	iToneUtilityState(EStateInitial), 
		iToneUtility(NULL),
		iTimer(NULL),
		iWait(EFalse),
		iStop(EFalse),
		iConfig(EFalse),
		iPrepare(EFalse),
		iNegative(EFalse),
		iCount(0),
		iVolume(0),	
		iBalance(0),
		iDuration(0),
		iPause(0),
		iExpected(0),
		iFilename(KNullDesC)
	{
	iTestStepName = aTestName;
	iSectName = aSectName;
	}

/*
 *
 * ~RMdaToneUtilityTestBase - Test step destructor
 *
 */
RMdaToneUtilityTestBase::~RMdaToneUtilityTestBase()
	{
	if (iToneUtility)
		{
		delete iToneUtility;
		}
	if(iTimer)
		{
		delete iTimer;
		}
	iFile.Close();
	iFs.Close();
	}

/*
 *
 * KickoffTestL - Starts the test
 *
 */
void RMdaToneUtilityTestBase::KickoffTestL()
	{
	User::LeaveIfError(iFs.Connect());
	INFO_PRINTF1(_L("__________  Creating Tone Utility object ___________"));

	TRAPD(err,  iToneUtility = CMdaAudioToneUtility::NewL(*this));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create Tone Utility object. Error = %d"), err);
		StopTest(err);
		return;
		}
	INFO_PRINTF1(_L("Tone Utility State: EStateCreated"));
	iToneUtilityState = EStateCreated;

	DoKickoffTestL();
	
	INFO_PRINTF1(_L("Tone Utility Event: EEventInitialize"));
	Fsm(EEventInitialize, KErrNone);
	}

/*
 *
 * CloseTest
 *
 */
void RMdaToneUtilityTestBase::CloseTest()
	{
	INFO_PRINTF1(_L("Deleting Tone Utility object"));
	delete iToneUtility;
	delete iTimer;
	}

/*
 *
 * StartTimer - Starts timer and timer callback
 *
 */
void RMdaToneUtilityTestBase::StartTimer(TTimeIntervalMicroSeconds32 aWaitTime)
	{
	TTimeIntervalMicroSeconds32 timeInterval;
	
	if(aWaitTime <= TTimeIntervalMicroSeconds32(0))
		{
		timeInterval = KOneSecond;
		}
	else
		{
		timeInterval = aWaitTime;
		}
	TCallBack callback (TimerCallback, this);
	iTimer->Start(timeInterval, timeInterval, callback);
	INFO_PRINTF1(_L("Timer has been started"));
	}

/*
 *
 * TimerCallback
 *
 */
TInt RMdaToneUtilityTestBase::TimerCallback(TAny* aPtr)
	{
	static_cast<RMdaToneUtilityTestBase*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

/*
 *
 * Fsm - Executes playing events of AudioToneUtility in sequence
 *
 */
void RMdaToneUtilityTestBase::Fsm(TMdaAudioUtilityEvent aMdaAudioUtilityEvent, TInt aError)
	{
	TInt error = 0;
	switch (iToneUtilityState)
		{
		case EStateCreated:
			{
			if (aMdaAudioUtilityEvent == EEventInitialize)
				{
				if(!iNegative)
					{
					INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::PrepareToPlayFileSequence"));
					iToneUtility->PrepareToPlayFileSequence(iFile);					
					}
				else
					{
					INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::PrepareToPlayDTMFString"));				
					iToneUtility->PrepareToPlayDTMFString(KDTMFString);					
					}
				INFO_PRINTF1(_L("MdaAudioUtility State: EStateInitializing"));
				iToneUtilityState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioUtility EEventInitialize not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aMdaAudioUtilityEvent == EEventPrepareComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling SetVolume using MaxVolume/2"));
				iToneUtility->SetVolume(iToneUtility->MaxVolume()/2);
				INFO_PRINTF1(_L("Starting playback"));
				iToneUtility->Play();
				iStartTime.HomeTime();
				iToneUtilityState = EStatePlaying;
				StartTimer(iPause*KOneSecond); //wait to pause
				}
			else if (aMdaAudioUtilityEvent == EEventPrepareComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("MatoPrepareComplete returned with error = %d"), aError);
				StopTest(aError);	
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioUtility EEventPrepareComplete not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if(aMdaAudioUtilityEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::Pause"));
				error = iToneUtility->Pause();
				if(!iNegative)
					{
					INFO_PRINTF1(_L("MdaAudioUtility State: EStatePause"));
					iToneUtilityState = EStatePause;
					iWait = EFalse;
					}
				else
					{
					if(error == KErrNotSupported)
						{
						INFO_PRINTF1(_L("Pause while in a mode different than prepare to play file sequence returned with KErrNotSupported as expected"));
						}
					else
						{
						ERR_PRINTF2(_L("Pause while in a mode different than prepare to play file sequence returned with %d instead of KErrNotSupported as expected"),error);
						StopTest(error);
						}
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aMdaAudioUtilityEvent == EEventTimerComplete)
				{
				if(!iWait && !iConfig && !iPrepare)
					{
					INFO_PRINTF1(_L("Resuming playback. Calling CMdaAudioToneUtility::Resume"));
					error = iToneUtility->Resume();
					if(error == KErrNone)
						{
						INFO_PRINTF1(_L("Resume returned with KErrNone as expected"));
						iToneUtilityState = EStatePlaying;
						}
					else
						{
						ERR_PRINTF2(_L("Resume returned with %d instead of KErrNone as expected"),error);
						StopTest(error);
						}
					}
				else if(!iWait && iConfig)
					{
					INFO_PRINTF2(_L("Playback paused for %d seconds"),iPause);
					INFO_PRINTF3(_L("Changing Volume and Balance while paused to Volume = %d and Balance = %d"),iVolume,iBalance);
					iToneUtility->SetVolume(iVolume);
					iToneUtility->SetBalanceL(iBalance);
					INFO_PRINTF1(_L("Resuming playback. Calling CMdaAudioToneUtility::Resume"));
					error = iToneUtility->Resume();
					if(error == KErrNone)
						{
						INFO_PRINTF1(_L("Resume returned with KErrNone as expected"));
						iToneUtilityState = EStatePlaying;
						}
					else
						{
						ERR_PRINTF2(_L("Resume returned with %d instead of KErrNone as expected"),error);
						StopTest(error);
						}
					iConfig = EFalse;
					}
				else if(!iWait && iPrepare)
					{
					INFO_PRINTF2(_L("Playback paused for %d seconds"),iPause);
					INFO_PRINTF1(_L("Calling PrepareToPlayFileSequence while in Pause"));
					iToneUtility->PrepareToPlayFileSequence(iFile);
					iToneUtilityState = EStatePlaying;
					}
				}
			else
				{
				ERR_PRINTF2(_L("EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateStopped:
			{
			if (aMdaAudioUtilityEvent == EEventTimerComplete)
				{
				if(iWait)
					{
					INFO_PRINTF1(_L("Playback stopped for 2 seconds"));
					iWait = EFalse;
					}
				else
					{
					INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::Resume while stopped"));
					error = iToneUtility->Resume();
					if(error == KErrNotReady)
						{
						INFO_PRINTF1(_L("Resume returned with KErrNotReady as expected"));
						iToneUtilityState = EStatePlaying;
						}
					else
						{
						ERR_PRINTF2(_L("Resume returned with %d instead of KErrNotReady as expected"),error);
						StopTest(error);
						}
					INFO_PRINTF1(_L("Resuming playback. Calling CMdaAudioToneUtility::Play"));
					iToneUtility->Play();
					}
				}
			else
				{
				ERR_PRINTF2(_L("EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid MdaAudioUtility state received: %d"), iToneUtilityState);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *
 * MatoPrepareComplete - From MMdaAudioToneObserver
 *
 */
void RMdaToneUtilityTestBase::MatoPrepareComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== Tone Utility MatoPrepareComplete() callback =========="));
	if(iToneUtilityState == EStateInitializing)
		{
		INFO_PRINTF1(_L("Tone Utility MatoPrepareComplete"));
		Fsm(EEventPrepareComplete, aError);
		}
	}

/*
 *
 * MatoPlayComplete - From MMdaAudioToneObserver
 *
 */
void RMdaToneUtilityTestBase::MatoPlayComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== Tone Utility MatoPlayComplete() callback =========="));
	if (aError == KErrNone)
		{
		INFO_PRINTF2(_L("Tone Utility called MatoPlayComplete with error = %d as expected"), aError);
		iEndTime.HomeTime();
		TTimeIntervalMicroSeconds playduration = iEndTime.MicroSecondsFrom(iStartTime);
		if(playduration > iExpected - KVariation && playduration < iExpected + KVariation)
			{
			INFO_PRINTF2(_L("Tone played for %ld. Value is between acceptable threshold"),playduration.Int64());
			StopTest(aError,EPass);
			}
		else
			{
			ERR_PRINTF2(_L("Tone played for %ld. Value is not between acceptable threshold"),playduration.Int64());
			StopTest(KErrGeneral);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Tone Utility called MatoPlayComplete with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0168-HP
 *========================================================================================================
 */
RMdaToneUtilityResumeTest::RMdaToneUtilityResumeTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaToneUtilityTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaToneUtilityResumeTest* RMdaToneUtilityResumeTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaToneUtilityResumeTest * self = new(ELeave)RMdaToneUtilityResumeTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaToneUtilityResumeTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the duration of the audio file to play
	if (!GetIntFromConfig(iSectName, KDuration, iDuration))
		{
		ERR_PRINTF1(_L("Duration could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// Get the pause time
	if (!GetIntFromConfig(iSectName, KPause, iPause))
		{
		ERR_PRINTF1(_L("Pause time could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}		
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KSequence, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		User::Leave(err);
		}
	iWait = ETrue;
	iExpected = iDuration + iPause*KOneSecond;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaToneUtilityResumeTest::DoTimerCallback()
	{
	if(!iWait)
		{
		INFO_PRINTF1(_L("Cancelling timer"));
		iTimer->Cancel();
		}
	else
		{
		INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
		}
	Fsm (EEventTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0169-HP
 *========================================================================================================
 */
RMdaToneUtilityResumeThreeTest::RMdaToneUtilityResumeThreeTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaToneUtilityTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaToneUtilityResumeThreeTest* RMdaToneUtilityResumeThreeTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaToneUtilityResumeThreeTest * self = new(ELeave)RMdaToneUtilityResumeThreeTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaToneUtilityResumeThreeTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the duration of the audio file to play
	if (!GetIntFromConfig(iSectName, KDuration, iDuration))
		{
		ERR_PRINTF1(_L("Duration could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// Get the pause time
	if (!GetIntFromConfig(iSectName, KPause, iPause))
		{
		ERR_PRINTF1(_L("Pause time could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}	
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KSequence, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		User::Leave(err);
		}
	iWait = ETrue;
	iExpected = iDuration + 3*(iPause*KOneSecond);
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iCount = 0;
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaToneUtilityResumeThreeTest::DoTimerCallback()
	{
	iCount++;
	if(!iWait && iCount == 6) //Repeating Pause-Resume cycle three times
		{
		iTimer->Cancel();
		Fsm (EEventTimerComplete, KErrNone);
		}
	else
		{
		INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
		Fsm (EEventTimerComplete, KErrNone);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0170-HP
 *========================================================================================================
 */
RMdaToneUtilityPlayPauseStopPlayTest::RMdaToneUtilityPlayPauseStopPlayTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaToneUtilityTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaToneUtilityPlayPauseStopPlayTest* RMdaToneUtilityPlayPauseStopPlayTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaToneUtilityPlayPauseStopPlayTest * self = new(ELeave)RMdaToneUtilityPlayPauseStopPlayTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaToneUtilityPlayPauseStopPlayTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the duration of the audio file to play
	if (!GetIntFromConfig(iSectName, KDuration, iDuration))
		{
		ERR_PRINTF1(_L("Duration could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// Get the pause time
	if (!GetIntFromConfig(iSectName, KPause, iPause))
		{
		ERR_PRINTF1(_L("Pause time could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}	
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KSequence, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		User::Leave(err);
		return;
		}
	iWait = ETrue;
	iStop = ETrue;
	iExpected = iDuration + 3*(iPause*KOneSecond);
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iCount = 0;
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaToneUtilityPlayPauseStopPlayTest::DoTimerCallback()
	{
	if(!iWait && iStop)
		{
		INFO_PRINTF1(_L("Stopping playback"));
		iToneUtility->CancelPlay();
		iStop = EFalse;
		iToneUtilityState = EStateStopped;
		iWait = ETrue;
		INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
		Fsm (EEventTimerComplete, KErrNone);
		}
	else if(!iWait && !iStop)
		{
		iTimer->Cancel();
		Fsm (EEventTimerComplete, KErrNone);
		}
	else
		{
		INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
		Fsm (EEventTimerComplete, KErrNone);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0171-HP
 *========================================================================================================
 */
RMdaToneUtilityConfigResumeTest::RMdaToneUtilityConfigResumeTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaToneUtilityTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaToneUtilityConfigResumeTest* RMdaToneUtilityConfigResumeTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaToneUtilityConfigResumeTest * self = new(ELeave)RMdaToneUtilityConfigResumeTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaToneUtilityConfigResumeTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the duration of the audio file to play
	if (!GetIntFromConfig(iSectName, KDuration, iDuration))
		{
		ERR_PRINTF1(_L("Duration could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// Get the pause time
	if (!GetIntFromConfig(iSectName, KPause, iPause))
		{
		ERR_PRINTF1(_L("Pause time could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}	
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KSequence, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// Get the volume
	if (!GetIntFromConfig(iSectName, KVolume, iVolume))
		{
		ERR_PRINTF1(_L("Volume could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// Get the balance
	if (!GetIntFromConfig(iSectName, KBalance, iBalance))
		{
		ERR_PRINTF1(_L("Balance could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		User::Leave(err);
		return;
		}
	iWait = ETrue;
	iConfig = ETrue;
	iExpected = iDuration + iPause*KOneSecond;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaToneUtilityConfigResumeTest::DoTimerCallback()
	{
	if(!iWait && !iConfig)
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("Comparing Volume and Balance with values set"));
		if (iVolume == iToneUtility->Volume() && Abs(iBalance - iToneUtility->GetBalanceL())<KBalanceTolerance)
			{
			INFO_PRINTF3(_L("Volume = %d and Balance = %d as expected"),iVolume,iBalance);
			}
		else
			{
			ERR_PRINTF1(_L("Retrieved values are different than expected"));
			ERR_PRINTF5(_L("Retrieved Volume = %d and Balance = %d. Expected Volume = %d and Balance = %d"),iToneUtility->Volume(),iToneUtility->GetBalanceL(),iVolume,iBalance);
			StopTest(KErrGeneral);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
		Fsm (EEventTimerComplete, KErrNone);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0172-HP
 *========================================================================================================
 */
RMdaToneUtilityPrepareInPauseTest::RMdaToneUtilityPrepareInPauseTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaToneUtilityTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaToneUtilityPrepareInPauseTest* RMdaToneUtilityPrepareInPauseTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaToneUtilityPrepareInPauseTest * self = new(ELeave)RMdaToneUtilityPrepareInPauseTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaToneUtilityPrepareInPauseTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the duration of the audio file to play
	if (!GetIntFromConfig(iSectName, KDuration, iDuration))
		{
		ERR_PRINTF1(_L("Duration could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// Get the pause time
	if (!GetIntFromConfig(iSectName, KPause, iPause))
		{
		ERR_PRINTF1(_L("Pause time could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}	
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KSequence, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		User::Leave(err);
		return;
		}
	iWait = ETrue;
	iPrepare = ETrue;
	iExpected = iDuration;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaToneUtilityPrepareInPauseTest::DoTimerCallback()
	{
	if(!iWait)
		{
		iTimer->Cancel();
		Fsm (EEventTimerComplete, KErrNone);
		}
	else
		{
		INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
		Fsm (EEventTimerComplete, KErrNone);
		}
	}

/*
 *
 * MatoPrepareComplete - From MMdaAudioToneObserver
 *
 */
void RMdaToneUtilityPrepareInPauseTest::MatoPrepareComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== Tone Utility MatoPrepareComplete() callback =========="));
	if(iToneUtilityState == EStateInitializing)
		{
		INFO_PRINTF1(_L("Tone Utility MatoPrepareComplete"));
		Fsm(EEventPrepareComplete, aError);
		}
	else if(iToneUtilityState == EStatePlaying)
		{
		if(aError == KErrNone)
			{
			INFO_PRINTF2(_L("Tone Utility returned with expected error %d"),aError);
			INFO_PRINTF1(_L("Resuming playback. Calling CMdaAudioToneUtility::Play"));
			iStartTime.HomeTime(); //Register start time here. Tone should restart
            iToneUtility->Play();
			}
		else
			{
			ERR_PRINTF1(_L("Preparing tone playback was not successful while paused. This is unexpected"));
			StopTest(aError);
			}
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0173-HP
 *========================================================================================================
 */
RMdaToneUtilityPauseBeforePlayTest::RMdaToneUtilityPauseBeforePlayTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaToneUtilityTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaToneUtilityPauseBeforePlayTest* RMdaToneUtilityPauseBeforePlayTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaToneUtilityPauseBeforePlayTest * self = new(ELeave)RMdaToneUtilityPauseBeforePlayTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaToneUtilityPauseBeforePlayTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the duration of the audio file to play
	if (!GetIntFromConfig(iSectName, KDuration, iDuration))
		{
		ERR_PRINTF1(_L("Duration could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}	
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KSequence, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		User::Leave(err);
		return;
		}
	iExpected = iDuration;		
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * MatoPrepareComplete - From MMdaAudioToneObserver
 *
 */
void RMdaToneUtilityPauseBeforePlayTest::MatoPrepareComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== Tone Utility MatoPrepareComplete() callback =========="));
	if(iToneUtilityState == EStateInitializing)
		{
		INFO_PRINTF1(_L("Tone Utility MatoPrepareComplete"));
		Fsm(EEventPrepareComplete, aError);
		}
	}

/*
 *
 * Fsm - Executes playing events of AudioPlayerUtility in sequence
 *
 */
void RMdaToneUtilityPauseBeforePlayTest::Fsm(TMdaAudioUtilityEvent aMdaAudioUtilityEvent, TInt aError)
	{
	TInt error = 0;
	switch (iToneUtilityState)
		{
		case EStateCreated:
			{
			if (aMdaAudioUtilityEvent == EEventInitialize)
				{
				StartTimer(20*KOneSecond); //wait twenty seconds. If timer callback occurrs then Pause worked while not in play
				INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::Pause before preparing to play"));
				error = iToneUtility->Pause();
				if(error == KErrNotReady)
					{
					INFO_PRINTF1(_L("Pause before preparing to play returned with KErrNotReady as expected"));
					}
				else
					{
					ERR_PRINTF2(_L("Pause before preparing to play returned with %d instead of KErrNotReady as expected"),error);
					StopTest(error);
					}
				INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::PrepareToPlayFileSequence"));
				iToneUtility->PrepareToPlayFileSequence(iFile);

				INFO_PRINTF1(_L("MdaAudioUtility State: EStateInitializing"));
				iToneUtilityState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioUtility EEventInitialize not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aMdaAudioUtilityEvent == EEventPrepareComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling SetVolume using MaxVolume/2"));
				iToneUtility->SetVolume(iToneUtility->MaxVolume()/2);
				INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::Pause before playing"));
				error = iToneUtility->Pause();
				if(error == KErrNotReady)
					{
					INFO_PRINTF1(_L("Pause before preparing to play returned with KErrNotReady as expected"));
					}
				else
					{
					ERR_PRINTF2(_L("Pause before preparing to play returned with %d instead of KErrNotReady as expected"),error);
					StopTest(error);
					}
				INFO_PRINTF1(_L("Starting playback"));
				iToneUtility->Play();
				iStartTime.HomeTime();
				}
			else if (aMdaAudioUtilityEvent == EEventPrepareComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("MatoPrepareComplete returned with error = %d"), aError);
				StopTest(aError);	
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioUtility EEventPrepareComplete not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid MdaAudioUtility state received: %d"), iToneUtilityState);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaToneUtilityPauseBeforePlayTest::DoTimerCallback()
	{
	ERR_PRINTF1(_L("Timer callback received. This is unexpected"));
	StopTest(KErrGeneral);
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0174-HP Negative
 *========================================================================================================
 */
RMdaToneUtilityNegPauseResumeTest::RMdaToneUtilityNegPauseResumeTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaToneUtilityTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaToneUtilityNegPauseResumeTest* RMdaToneUtilityNegPauseResumeTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaToneUtilityNegPauseResumeTest * self = new(ELeave)RMdaToneUtilityNegPauseResumeTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaToneUtilityNegPauseResumeTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the pause time
	if (!GetIntFromConfig(iSectName, KPause, iPause))
		{
		ERR_PRINTF1(_L("Pause time could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}	
	// Get the filename of the audio file to play	
	if (!GetStringFromConfig(iSectName, KSequence, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		User::Leave(err);
		return;
		}
	iWait = ETrue;
	iNegative = ETrue;
	iCount = 0;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaToneUtilityNegPauseResumeTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Cancelling timer"));
	iTimer->Cancel();
	INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
	Fsm (EEventTimerComplete, KErrNone);
	}

/*
 *
 * MatoPlayComplete - From MMdaAudioToneObserver
 *
 */
void RMdaToneUtilityNegPauseResumeTest::MatoPlayComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== Tone Utility MatoPlayComplete() callback =========="));
	iCount++;
	if (aError == KErrNone)
		{
		INFO_PRINTF2(_L("Tone Utility called MatoPlayComplete with error = %d as expected"), aError);
		switch(iCount)
			{
			//NOTE:"PlayFixedSequence is not supported on A3F"
			case 1:
				INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::PrepareToPlayTone"));
				iToneUtility->PrepareToPlayTone(KToneFrequency, KFiveSeconds);
				INFO_PRINTF1(_L("MdaAudioUtility State: EStateInitializing"));
				iToneUtilityState = EStateInitializing;
				break;	
			case 2:
				INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::PrepareToPlayDualTone"));
				iToneUtility->PrepareToPlayDualTone(KToneFrequency, KToneFrequencyTwo, KFiveSeconds);
				INFO_PRINTF1(_L("MdaAudioUtility State: EStateInitializing"));
				iToneUtilityState = EStateInitializing;
				break;
			case 3:
				INFO_PRINTF1(_L("All tone variations have been tested"));
				StopTest(aError);
				break;
			}
		}
	else
		{
		ERR_PRINTF2(_L("Tone Utility called MatoPlayComplete with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0183-HP
 *========================================================================================================
 */
RMdaToneUtilityPauseNonA3fTest::RMdaToneUtilityPauseNonA3fTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaToneUtilityTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaToneUtilityPauseNonA3fTest* RMdaToneUtilityPauseNonA3fTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaToneUtilityPauseNonA3fTest * self = new(ELeave)RMdaToneUtilityPauseNonA3fTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaToneUtilityPauseNonA3fTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KSequence, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	TInt err = iFile.Open(iFs, iFilename, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Could not open input file %S. Error = %d"), &iFilename, err);
		iFs.Close();
		User::Leave(err);
		return;
		}
	// Get the duration of the audio file to play
	if (!GetIntFromConfig(iSectName, KDuration, iDuration))
		{
		ERR_PRINTF1(_L("Duration could not be retrieved from ini file"));
		User::Leave(KErrNotFound);
		return;
		}
	// Get the pause time
	if (!GetIntFromConfig(iSectName, KPause, iPause))
		{
		ERR_PRINTF1(_L("Pause time could not be retrieved from ini file"));
		User::Leave(KErrNotFound);
		return;
		}
	iExpected = iDuration;		
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * MatoPrepareComplete - From MMdaAudioToneObserver
 *
 */
void RMdaToneUtilityPauseNonA3fTest::MatoPrepareComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== Tone Utility MatoPrepareComplete() callback =========="));
	if(iToneUtilityState == EStateInitializing)
		{
		INFO_PRINTF1(_L("Tone Utility MatoPrepareComplete"));
		Fsm(EEventPrepareComplete, aError);
		}
	}

/*
 *
 * Fsm - Executes playing events of AudioToneUtility in sequence
 *
 */
void RMdaToneUtilityPauseNonA3fTest::Fsm(TMdaAudioUtilityEvent aMdaAudioUtilityEvent, TInt aError)
	{
	TInt error = 0;
	switch (iToneUtilityState)
		{
		case EStateCreated:
			{
			if (aMdaAudioUtilityEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::PrepareToPlayFileSequence"));
				iToneUtility->PrepareToPlayFileSequence(iFile);

				INFO_PRINTF1(_L("MdaAudioUtility State: EStateInitializing"));
				iToneUtilityState = EStateInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioUtility EEventInitialize not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateInitializing:
			{
			if (aMdaAudioUtilityEvent == EEventPrepareComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling SetVolume using MaxVolume/2"));
				iToneUtility->SetVolume(iToneUtility->MaxVolume()/2);
				INFO_PRINTF1(_L("Starting playback"));
				iToneUtility->Play();
				iStartTime.HomeTime();
				iToneUtilityState = EStatePlaying;
				StartTimer(iPause*KOneSecond); //wait to pause
				}
			else if (aMdaAudioUtilityEvent == EEventPrepareComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("MatoPrepareComplete returned with error = %d"), aError);
				StopTest(aError);	
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioUtility EEventPrepareComplete not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if(aMdaAudioUtilityEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::Pause in non-a3f configuration"));
				error = iToneUtility->Pause();
				if(error == KErrNotSupported)
					{
					INFO_PRINTF1(_L("Pause in ToneUtility returned with KErrNotSupported as expected"));
					}
				else
					{
					ERR_PRINTF2(_L("Pause in ToneUtility returned with %d instead of KErrNotSupported as expected"),error);
					StopTest(error);
					}
				INFO_PRINTF1(_L("Calling CMdaAudioToneUtility::Resume in non-a3f configuration"));
				error = iToneUtility->Resume();
				if(error == KErrNotReady)
					{
					INFO_PRINTF1(_L("Resume returned with KErrNotReady as expected"));
					iToneUtilityState = EStatePlaying;
					}
				else
					{
					ERR_PRINTF2(_L("Resume returned with %d instead of KErrNotReady as expected"),error);
					StopTest(error);
					}					
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid MdaAudioUtility state received: %d"), iToneUtilityState);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaToneUtilityPauseNonA3fTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
	INFO_PRINTF1(_L("Cancelling timer"));
	iTimer->Cancel();
	Fsm (EEventTimerComplete, KErrNone);
	}
