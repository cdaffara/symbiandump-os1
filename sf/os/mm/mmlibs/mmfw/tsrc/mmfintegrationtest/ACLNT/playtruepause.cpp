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
// Part of the TSI_MMFACLNT suite that tests CR1566 (TruePause) on AudioPlayerUtility
// 
//

/**
 @file PlayTruePause.cpp
*/

#include "playtruepause.h"

/*
 *
 * RMdaPlayerUtilityTestBase - Test step constructor
 *
 */
RMdaPlayerUtilityTestBase::RMdaPlayerUtilityTestBase(const TDesC& aTestName, const TDesC& aSectName)
	:	iAudioUtilityState(EStateInitial), 
		iPlayerUtility(NULL),
		iTimer(NULL),
		iFilename(KNullDesC),
		iStop(EFalse),
		iQuery(EFalse),
		iVolume(0),
		iBalance(0),
		iResume(EFalse),
		iPauseResumeTimes(0),
		iPauseApplied(0),
		iDuration(0),
		iAllFormat(0),
		iPositionSupported(EFalse),
		iMiliSec(EFalse)
	{
	iTestStepName = aTestName;
	iSectName = aSectName;
	}

/*
 *
 * ~RMdaPlayerUtilityTestBase - Test step destructor
 *
 */
RMdaPlayerUtilityTestBase::~RMdaPlayerUtilityTestBase()
	{
	if (iPlayerUtility)
		{
		delete iPlayerUtility;
		}
	if(iTimer)
		{
		delete iTimer;
		}
	}

/*
 *
 * KickoffTestL - Starts the test
 *
 */
void RMdaPlayerUtilityTestBase::KickoffTestL()
	{
	INFO_PRINTF1(_L("__________  Creating Player Utility object ___________"));
	TRAPD(err,  iPlayerUtility = CMdaAudioPlayerUtility::NewL(*this));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create Player Utility object. Error = %d"), err);
		StopTest(err);
		return;
		}
	INFO_PRINTF1(_L("Player Utility State: EStateCreated"));
	iAudioUtilityState = EStateCreated;
	DoKickoffTestL();
	INFO_PRINTF1(_L("Audio Player Utility Event: EEventInitialize"));
	Fsm(EEventInitialize, KErrNone);
	}

/*
 *
 * CloseTest
 *
 */
void RMdaPlayerUtilityTestBase::CloseTest()
	{
	INFO_PRINTF1(KMsgDelete);
	delete iPlayerUtility;
	delete iTimer;
	}

/*
 *
 * StartTimer - Starts timer and timer callback
 *
 */
void RMdaPlayerUtilityTestBase::StartTimer(TTimeIntervalMicroSeconds32 aWaitTime)
	{
	TTimeIntervalMicroSeconds32 timeInterval;
	
	if(aWaitTime <= TTimeIntervalMicroSeconds32(0))
		{
		timeInterval = KMicroSecInOneSec;
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
TInt RMdaPlayerUtilityTestBase::TimerCallback(TAny* aPtr)
	{
	static_cast<RMdaPlayerUtilityTestBase*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaPlayerUtilityTestBase::DoTimerCallback()
	{
	INFO_PRINTF1(_L("DevSound Event: EEventTimerComplete"));
	Fsm (EEventTimerComplete, KErrNone);
	}

/*
 *
 * Fsm - Executes playing events of AudioPlayerUtility in sequence
 *
 */
void RMdaPlayerUtilityTestBase::Fsm(TMdaAudioUtilityEvent aMdaAudioUtilityEvent, TInt aError)
	{
	switch (iAudioUtilityState)
		{
		case EStateCreated:
			{
			__ASSERT_ALWAYS((aError == KErrNone), Panic(iTestStepName.Right(KPanicLength), EFsmIncorrectErrorPassed));
			if (aMdaAudioUtilityEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::OpenFileL"));
				TRAPD(err,iPlayerUtility->OpenFileL(iFilename));
				if(err)
					{
					ERR_PRINTF2(_L("CMdaAudioPlayerUtility::OpenFileL left with err = %d"),err);
					StopTest(err);
					break;
					}
				INFO_PRINTF1(_L("AudioPlayerUtility State: EStateInitializing"));
				iAudioUtilityState = EStateInitializing;
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
			if (aMdaAudioUtilityEvent == EEventInitComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::Play"));
				iPlayerUtility->Play();
				INFO_PRINTF1(_L("AudioPlayerUtility State: EStatePlaying"));
				iAudioUtilityState = EStatePlaying;
				if(iMiliSec)
					{
					StartTimer(iDuration*KMiliSecInOneSec);
					}
				else
					{
					StartTimer(iDuration*KMicroSecInOneSec);
					}
				}
			else if (aMdaAudioUtilityEvent == EEventInitComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("MapcInitComplete returned with error = %d"), aError);
				StopTest(aError);	
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioUtility EEventInitComplete not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePlaying:
			{
			if(aMdaAudioUtilityEvent == EEventTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::Pause"));
				TInt err = iPlayerUtility->Pause();
				iPauseApplied = ETrue;
				INFO_PRINTF1(_L("AudioPlayerUtility State: EStatePause"));
				iAudioUtilityState = EStatePause;
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioUtility EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStatePause:
			{
			if (aMdaAudioUtilityEvent == EEventTimerComplete)
				{
				if(iQuery)
					{
					INFO_PRINTF2(_L("Setting AudioPlayerUtility volume = %d"), iVolume);
					TInt err = iPlayerUtility->SetVolume(iVolume);
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Setting volume failed. It returned with error = %d"), err);
						StopTest(err);
						break;
						}
					TInt volume = 0;
					INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::GetVolume for verifying"));
					err = iPlayerUtility->GetVolume(volume);
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMdaAudioPlayerUtility::GetVolume returned with error = %d"), err);
						StopTest(err);
						break;
						}
					if(iVolume != volume)
						{
						ERR_PRINTF2(_L("CMdaAudioPlayerUtility::SetVolume returned different set value = %d"), volume);
						StopTest(KErrGeneral);
						break;
						}
					INFO_PRINTF2(_L("Setting AudioPlayerUtility balance = %d"), iBalance);
					err = iPlayerUtility->SetBalance(iBalance);
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Setting balance failed. It returned with error = %d"), err);
						StopTest(err);
						break;
						}
					TInt balance = 0;
					INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::GetBalance for verifying"));
					err = iPlayerUtility->GetBalance(balance);
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMdaAudioPlayerUtility::GetBalance returned with error = %d"), err);
						StopTest(err);
						break;
						}
					if(Abs(iBalance - balance)<KBalanceTolerance)
						{
						INFO_PRINTF2(_L("CMdaAudioPlayerUtility::SetBalance returned expected set value %d"),iBalance);
						}
					else
						{
						ERR_PRINTF3(_L("CMdaAudioPlayerUtility::SetBalance returned different set value %d. Retrieved %d"), iBalance, balance);
						StopTest(KErrGeneral);
						break;
						}
					INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::Play"));
					iPlayerUtility->Play();
					iTimer->Cancel();
					INFO_PRINTF1(_L("Audio Player Utility State: EStatePlaying"));
					iAudioUtilityState = EStatePlaying;
					INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::GetVolume for verifying"));
					err = iPlayerUtility->GetVolume(volume);
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMdaAudioPlayerUtility::GetVolume returned with error = %d"), err);
						StopTest(err);
						break;
						}
					if (volume == iVolume)
						{
						INFO_PRINTF1(_L("CMdaAudioPlayerUtility::GetVolume returned equal previous set value as expected"));
						}
					else
						{
						ERR_PRINTF2(_L("CMdaAudioPlayerUtility::GetVolume returned different set value = %d"), iVolume);
						StopTest (KErrGeneral);
						break;
						}
					INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::GetBalance for verifying"));
					err = iPlayerUtility->GetBalance(balance);
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("CMdaAudioPlayerUtility::GetBalance returned with error = %d"), err);
						StopTest(err);
						break;
						}
					if(Abs(iBalance - balance)<KBalanceTolerance)
						{
						INFO_PRINTF2(_L("CMdaAudioPlayerUtility::GetBalance returned expected set value %d"),iBalance);
						}
					else
						{
						ERR_PRINTF3(_L("CMdaAudioPlayerUtility::GetBalance returned different set value = %d. Retrieved %d"), iBalance, balance);
						StopTest(KErrGeneral);
						break;
						}
					}
				if(iResume || iStop)
					{
					TTimeIntervalMicroSeconds position1 = 0;
					if(iPositionSupported)
						{
						INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::GetPosition"));
						TInt err = iPlayerUtility->GetPosition(position1);
						if(err)
							{
							ERR_PRINTF2(_L("CMdaAudioPlayerUtility::GetPosition returned with err = %d"), err);
							StopTest(err);
							break;
							}
						INFO_PRINTF2(_L("AudioPlayerUtility position = %Ld"), position1.Int64());
						if(position1.Int64() == 0)
							{
							INFO_PRINTF1(_L("AudioPlayerUtility position must be longer than 0"));
							StopTest(KErrGeneral);
							break;
							}
						}
					if(iStop)
						{
						INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::Stop"));
						iPlayerUtility->Stop();
						}
					INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::Play"));
					iPlayerUtility->Play();
					if((--iPauseResumeTimes) <= 0)
						{
						iTimer->Cancel();
						}
					INFO_PRINTF1(_L("AudioPlayerUtility State: EStatePlaying"));
					iAudioUtilityState = EStatePlaying;
					TTimeIntervalMicroSeconds position2 = 0;
					if(iPositionSupported)
						{
						INFO_PRINTF1(_L("Calling CMdaAudioPlayerUtility::GetPosition"));
						TInt err = iPlayerUtility->GetPosition(position2);
						if(err)
							{
							ERR_PRINTF2(_L("CMdaAudioPlayerUtility::GetPosition returned with err = %d"), err);
							StopTest(err);
							break;
							}
						INFO_PRINTF2(_L(" AudioPlayerUtility position = %Ld"), position2.Int64());
						}
					if(iStop)
						{
						if(position2.Int64() == 0)
							{
							INFO_PRINTF1(_L("AudioPlayerUtility position was reset as expected"));
							}
						else
							{
							INFO_PRINTF1(_L("AudioPlayerUtility position was not reset"));
							StopTest(KErrGeneral);
							break;
							}
						}
					else if(iPositionSupported)
						{
						if(Abs(position2.Int64() - position1.Int64()) <= KOneSecond/2)
							{
							INFO_PRINTF2(_L("AudioPlayerUtility position continued from expected position %Ld"),position2.Int64());
							}
						else
							{
							ERR_PRINTF3(_L("Playback did not resume from expected position. Expected %Ld Retrieved %Ld"),position1.Int64(),position2.Int64());
							StopTest(KErrGeneral);
							break;
							}
						}
					else
						{
						INFO_PRINTF1(_L("AudioPlayerUtility continue playing "));
						}
					
					}
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioUtility EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioUtilityEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid MdaAudioUtility state received: %d"), aMdaAudioUtilityEvent);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *
 * MapcInitComplete - From MMdaAudioPlayerCallback
 *
 */
void RMdaPlayerUtilityTestBase::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	INFO_PRINTF1(_L("========== Audio Player Utility MapcInitComplete() callback =========="));
	if(iAudioUtilityState == EStateInitializing)
		{
		INFO_PRINTF1(_L(" Audio Player Utility Event: EEventInitComplete"));
		Fsm(EEventInitComplete, aError);
		}
	}

/*
 *
 * MapcPlayComplete - From MMdaAudioPlayerCallback
 *
 */
void RMdaPlayerUtilityTestBase::MapcPlayComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== Audio Player Utility MapcPlayComplete() callback =========="));
	if (aError == KErrNone)
		{
		INFO_PRINTF2(_L("AudioPlayerUtility called MapcPlayComplete with error = %d as expected"), aError);
		StopTest();
		}
	else
		{
		ERR_PRINTF2(_L("AudioPlayerUtility called MapcPlayComplete with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrNone);
		StopTest(aError);
		}
	}

/*
 *
 * Panic
 *
 */
void RMdaPlayerUtilityTestBase::Panic(const TDesC &aCategory, TInt aReason)
	{
	User::Panic(aCategory, aReason);
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0185-HP
 *========================================================================================================
 */
RMdaPlayerUtiliyPauseStopAndPlayWavTest::RMdaPlayerUtiliyPauseStopAndPlayWavTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaPlayerUtilityTestBase(aTestName, aSectName)
	{
	iStop = ETrue;
	iPositionSupported = ETrue;
	}

/*
 *
 * NewL
 *
 */
RMdaPlayerUtiliyPauseStopAndPlayWavTest* RMdaPlayerUtiliyPauseStopAndPlayWavTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaPlayerUtiliyPauseStopAndPlayWavTest * self = new(ELeave)RMdaPlayerUtiliyPauseStopAndPlayWavTest(aTestName, aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaPlayerUtiliyPauseStopAndPlayWavTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KFilenameWAV, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	if (!GetIntFromConfig(iSectName, KDuration1, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}
/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0186-HP
 *========================================================================================================
 */
RMdaPlayerUtilityQueryPauseAndPlayWavTest::RMdaPlayerUtilityQueryPauseAndPlayWavTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaPlayerUtilityTestBase(aTestName, aSectName)
	{
	iQuery = ETrue;
	}

/*
 *
 * NewL
 *
 */
RMdaPlayerUtilityQueryPauseAndPlayWavTest* RMdaPlayerUtilityQueryPauseAndPlayWavTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaPlayerUtilityQueryPauseAndPlayWavTest * self = new(ELeave)RMdaPlayerUtilityQueryPauseAndPlayWavTest(aTestName, aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaPlayerUtilityQueryPauseAndPlayWavTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KFilenameWAV, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	if (!GetIntFromConfig(iSectName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iSectName, KBalance, iBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KBalance);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iSectName, KDuration1, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0187-HP
 *========================================================================================================
 */
RMdaPlayerUtilityPauseAndPlaySqnTest::RMdaPlayerUtilityPauseAndPlaySqnTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaPlayerUtilityTestBase(aTestName, aSectName)
	{
	iResume = ETrue;
	}

/*
 *
 * NewL
 *
 */
RMdaPlayerUtilityPauseAndPlaySqnTest* RMdaPlayerUtilityPauseAndPlaySqnTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaPlayerUtilityPauseAndPlaySqnTest * self = new(ELeave)RMdaPlayerUtilityPauseAndPlaySqnTest(aTestName, aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaPlayerUtilityPauseAndPlaySqnTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KFilenameSQN, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	if (!GetIntFromConfig(iSectName, KDuration1, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0188-HP
 *========================================================================================================
 */
RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest::RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaPlayerUtilityTestBase(aTestName, aSectName)
	{
	iResume = ETrue;
	iPauseResumeTimes = KRepeatThrice;
	}

/*
 *
 * NewL
 *
 */
RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest* RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest * self = new(ELeave)RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest(aTestName, aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KFilenameSQN, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	if (!GetIntFromConfig(iSectName, KDuration1, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0189-HP
 *========================================================================================================
 */
RMdaPlayerUtilityPauseStopAndPlaySqnTest::RMdaPlayerUtilityPauseStopAndPlaySqnTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaPlayerUtilityTestBase(aTestName, aSectName)
	{
	iStop = ETrue;
	}

/*
 *
 * NewL
 *
 */
RMdaPlayerUtilityPauseStopAndPlaySqnTest* RMdaPlayerUtilityPauseStopAndPlaySqnTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaPlayerUtilityPauseStopAndPlaySqnTest * self = new(ELeave)RMdaPlayerUtilityPauseStopAndPlaySqnTest(aTestName, aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaPlayerUtilityPauseStopAndPlaySqnTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KFilenameSQN, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	if (!GetIntFromConfig(iSectName, KDuration1, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0190-HP
 *========================================================================================================
 */
RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest::RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaPlayerUtilityTestBase(aTestName, aSectName)
	{
	iQuery = ETrue;
	}

/*
 *
 * NewL
 *
 */
RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest* RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest * self = new(ELeave)RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest(aTestName, aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KFilenameSQN, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	if (!GetIntFromConfig(iSectName, KVolume, iVolume))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KVolume);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iSectName, KBalance, iBalance))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KBalance);
		StopTest(KErrNotFound);
		return;
		}
	if (!GetIntFromConfig(iSectName, KDuration1, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDuration1);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0191-HP
 *========================================================================================================
 */
RMdaPlayerUtilityPauseAndPlayFormatsTest::RMdaPlayerUtilityPauseAndPlayFormatsTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaPlayerUtilityTestBase(aTestName, aSectName)
	{
	iResume = ETrue;
	iPositionSupported = ETrue;
	iMiliSec = ETrue;
	}

/*
 *
 * NewL
 *
 */
RMdaPlayerUtilityPauseAndPlayFormatsTest* RMdaPlayerUtilityPauseAndPlayFormatsTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaPlayerUtilityPauseAndPlayFormatsTest * self = new(ELeave)RMdaPlayerUtilityPauseAndPlayFormatsTest(aTestName, aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaPlayerUtilityPauseAndPlayFormatsTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KFilenamePCM16, filename))
		{
		ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
		StopTest(KErrNotFound);
		return;
		}
	// open using RFile for playback
	iFilename.Copy(filename);
	INFO_PRINTF2(_L("File \"PCM16\" under test  -> %S"), &iFilename);
	if (!GetIntFromConfig(iSectName, KDurationMiliSec, iDuration))
		{
		ERR_PRINTF2(KMsgErrorGetParameter, &KDurationMiliSec);
		StopTest(KErrNotFound);
		return;
		}
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * MapcPlayComplete - From MMdaAudioPlayerCallback
 *
 */
void RMdaPlayerUtilityPauseAndPlayFormatsTest::MapcPlayComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== Audio Player Utility MapcPlayComplete() callback =========="));
	if (aError == KErrNone)
		{
		if(iPauseApplied)
			{
			INFO_PRINTF2(_L("AudioPlayerUtility called MapcPlayComplete with error = %d as expected"), aError);
			TBuf<KFileFormatSize> fileFormat;
			switch(iAllFormat)
				{
				case EPCM8Format:
					{
					fileFormat.Copy(KFilenamePCM8);
					INFO_PRINTF2(_L("File \"PCM8\" under test  -> %S"), &fileFormat);
					break;
					}
				case EPCMU16Format:
					{
					fileFormat.Copy(KFilenamePCMU16);
					INFO_PRINTF2(_L("File \"PCMU16\" under test  -> %S"), &fileFormat);
					break;
					}
				case EPCMU16BEFormat:
					{
					fileFormat.Copy(KFilenamePCMU16BE);
					INFO_PRINTF2(_L("File \"PCMU16BE\" under test  -> %S"), &fileFormat);
					break;
					}
				case EALAWFormat:
					{
					fileFormat.Copy(KFilenameAlaw);
					INFO_PRINTF2(_L("File \"ALAW\" under test  -> %S"), &fileFormat);
					break;
					}
				case EMULAWFormat:
					{
					fileFormat.Copy(KFilenameMulaw);
					INFO_PRINTF2(_L("File \"MULAW\" under test  -> %S"), &fileFormat);
					break;
					}
				case EGSM610Format:
					{
					fileFormat.Copy(KFilenameGSM610);
					INFO_PRINTF2(_L("File \"GSM610\" under test  -> %S"), &fileFormat);
					break;
					}
				case EIMADFormat:
					{
					fileFormat.Copy(KFilenameIMAD);
					INFO_PRINTF2(_L("File \"IMAD\" under test  -> %S"), &fileFormat);
					break;
					}
				case EPCMU8Format:
					{
					fileFormat.Copy(KFilenamePCMU8);
					INFO_PRINTF2(_L("File \"PCMU8\" under test  -> %S"), &fileFormat);
					break;
					}
				case EOGGFormat:
					{
					fileFormat.Copy(KFilenameOGG);
					INFO_PRINTF2(_L("File \"OGG\" under test  -> %S"), &fileFormat);
					break;
					}
				default:
					{
					break;
					}
				}
			if(iAllFormat > EOGGFormat)
				{
				StopTest();
				}
			else
				{
				++iAllFormat;
				iPauseApplied = EFalse;
				TPtrC filename;
				if (!GetStringFromConfig(iSectName, fileFormat, filename))
					{
					ERR_PRINTF1(_L("Filename could not be retrieved from ini file"));
					StopTest(KErrNotFound);
					return;
					}
				iFilename.Copy(filename);
				INFO_PRINTF1(_L("Player Utility State: EStateCreated"));
				iAudioUtilityState = EStateCreated;
				INFO_PRINTF1(_L("Audio Player Utility Event: EEventInitialize"));
				Fsm(EEventInitialize, KErrNone);
				}
			}
		else
			{
			ERR_PRINTF2(_L("AudioPlayerUtility called MapcPlayComplete with error = %d"), aError);
			ERR_PRINTF1(_L("Audio file was finished before continuing"));
			StopTest(KErrGeneral);
			}
		}
	else
		{
		ERR_PRINTF2(_L("AudioPlayerUtility called MapcPlayComplete with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrNone);
		StopTest(aError);
		}
	}

