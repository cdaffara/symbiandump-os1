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
// Part of the TSI_MMFACLNT suite that tests CR1566 (TruePause) on AudioOutputStream
// 
//

/**
 @file TestOutputStreamTruePause.cpp
*/

#include "testoutputstreamtruepause.h"

/*
 *
 * RMdaOutputStreamTestBase - Test step constructor
 *
 */
RMdaOutputStreamTestBase::RMdaOutputStreamTestBase(const TDesC& aTestName, const TDesC& aSectName)
	:	iAudioOutputStreamState(EStateAOSInitial), 
		iAudioOutputStream(NULL),
		iTimer(NULL),
		iWait(EFalse),
		iConfig(EFalse),
		iInvalidConfig(EFalse),
		iGetBytes(EFalse),
		iCount(0),
		iVolume(0),
		iBalance(0),
		iDuration(0),
		iPause(0),
		iBytes(0),
		iPosition(0),
		iFilename(KNullDesC)
	{
	iTestStepName = aTestName;
	iSectName = aSectName;
	}

/*
 *
 * ~RMdaOutputStreamTestBase - Test step destructor
 *
 */
RMdaOutputStreamTestBase::~RMdaOutputStreamTestBase()
	{
	if (iAudioOutputStream)
		{
		delete iAudioOutputStream;
		}
	if(iTimer)
		{
		delete iTimer;
		}
	iFile.Close();
	iFs.Close();
	iBuffer.Close();
	}

/*
 *
 * KickoffTestL - Starts the test
 *
 */
void RMdaOutputStreamTestBase::KickoffTestL()
	{
	User::LeaveIfError(iFs.Connect());
	INFO_PRINTF1(_L("__________  Creating AudioOutputStream object ___________"));

	TRAPD(err,  iAudioOutputStream = CMdaAudioOutputStream::NewL(*this));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not create Tone Utility object. Error = %d"), err);
		StopTest(err);
		return;
		}
	INFO_PRINTF1(_L("AudioOutputStream State: EStateCreated"));
	iAudioOutputStreamState = EStateAOSCreated;
	
	DoKickoffTestL();
	
	INFO_PRINTF1(_L("AudioOutputStream: EEventInitialize"));
	Fsm(EEventAOSInitialize, KErrNone);
	}

/*
 *
 * CloseTest
 *
 */
void RMdaOutputStreamTestBase::CloseTest()
	{
	INFO_PRINTF1(_L("Deleting AudioOutputStream object"));
	delete iAudioOutputStream;
	delete iTimer;
	}

/*
 *
 * StartTimer - Starts timer and timer callback
 *
 */
void RMdaOutputStreamTestBase::StartTimer(TTimeIntervalMicroSeconds32 aWaitTime)
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
TInt RMdaOutputStreamTestBase::TimerCallback(TAny* aPtr)
	{
	static_cast<RMdaOutputStreamTestBase*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

/*
 *
 * Fsm - Executes playing events of AudioToneUtility in sequence
 *
 */
void RMdaOutputStreamTestBase::Fsm(TMdaAOSEvent aMdaAudioOutputStreamEvent, TInt aError)
	{
	TInt error = 0;
	TTimeIntervalMicroSeconds resumeposition;
	switch (iAudioOutputStreamState)
		{
		case EStateAOSCreated:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSInitialize)
				{
				TMdaAudioDataSettings settings;
				//Default settings
				settings.Query();
				settings.iChannels = TMdaAudioDataSettings::EChannelsMono;
				settings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Open"));
				iAudioOutputStream->Open(&settings);
				INFO_PRINTF1(_L("MdaAudioOutputStream State: EStateInitializing"));
				iAudioOutputStreamState = EStateAOSInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioOutputStream EEventInitialize not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSInitializing:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSOpenComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling SetVolume using MaxVolume/2"));
				iAudioOutputStream->SetVolume(iAudioOutputStream->MaxVolume()/2);
				INFO_PRINTF1(_L("Starting playback"));
				iAudioOutputStream->WriteL(iBuffer);
				iAudioOutputStreamState = EStateAOSPlaying;
				StartTimer(iPause*KOneSecond);
				}
			else if (aMdaAudioOutputStreamEvent == EEventAOSOpenComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("MaoscOpenComplete returned with error = %d"), aError);
				StopTest(aError);	
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioOutputStream EEventOpenComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSPlaying:
			{
			if(aMdaAudioOutputStreamEvent == EEventAOSTimerComplete)
				{
				if(!iGetBytes)
					{
					INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Pause"));
					error = iAudioOutputStream->Pause();
					if(error == KErrNone)
						{
						INFO_PRINTF1(_L("Pause returned with KErrNone as expected"));
						}
					else
						{
						ERR_PRINTF2(_L("Pause returned with %d instead of KErrNone as expected"),error);
						StopTest(error);
						}
					iPosition = iAudioOutputStream->Position();
					INFO_PRINTF2(_L("Stream was paused at %Ld"),iPosition.Int64());
					INFO_PRINTF1(_L("MdaAudioOutputStream State: EStatePause"));
					iAudioOutputStreamState = EStateAOSPause;
					iWait = EFalse;
					}
				if(iGetBytes)
					{
					INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Pause"));
					error = iAudioOutputStream->Pause();
					if(error == KErrNone)
						{
						INFO_PRINTF1(_L("Pause returned with KErrNone as expected"));
						}
					else
						{
						ERR_PRINTF2(_L("Pause returned with %d instead of KErrNone as expected"),error);
						StopTest(error);
						}					
					INFO_PRINTF1(_L("MdaAudioOutputStream State: EStatePause"));
					iPosition = iAudioOutputStream->Position();
					INFO_PRINTF2(_L("Stream was paused at %Ld"),iPosition.Int64());
					INFO_PRINTF1(_L("MdaAudioOutputStream State: EStatePause"));
					iAudioOutputStreamState = EStateAOSPause;
					iWait = EFalse;
					INFO_PRINTF1(_L("Calling GetBytes"));
					iBytes = iAudioOutputStream->GetBytes();
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSPause:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSTimerComplete)
				{
				if(!iWait && !iConfig && !iInvalidConfig && !iGetBytes)
					{
					if(iPosition == iAudioOutputStream->Position())
						{
						INFO_PRINTF2(_L("Paused position was maintained at %Ld"),iPosition.Int64());
						}
					else
						{
						ERR_PRINTF3(_L("Position was not maintained during pause. Expected %Ld Retrieved %Ld"),iPosition.Int64(),iAudioOutputStream->Position().Int64());
						StopTest(KErrGeneral);
						}
					INFO_PRINTF1(_L("Resuming playback. Calling CMdaAudioOutputStream::Resume"));
					error = iAudioOutputStream->Resume();
					resumeposition = iAudioOutputStream->Position();
					if(error == KErrNone)
						{
						INFO_PRINTF1(_L("Resume returned with KErrNone as expected"));
						iAudioOutputStreamState = EStateAOSPlaying;
						if(Abs(resumeposition.Int64() - iPosition.Int64()) <= KOneSecond/2)
							{
							INFO_PRINTF2(_L("Playback resumed from expected position %Ld"),iPosition.Int64());
							}
						else
							{
							ERR_PRINTF3(_L("Playback did not resume from expected position. Expected %Ld Retrieved %Ld"),iPosition.Int64(),resumeposition.Int64());
							StopTest(KErrGeneral);
							}
						}
					else
						{
						ERR_PRINTF2(_L("Resume returned with %d instead of KErrNone as expected"),error);
						StopTest(error);
						}
					}
				else if(!iWait && iConfig)
					{
					if(iPosition == iAudioOutputStream->Position())
						{
						INFO_PRINTF2(_L("Paused position was maintained at %Ld"),iPosition.Int64());
						}
					else
						{
						ERR_PRINTF3(_L("Position was not maintained during pause. Expected %Ld Retrieved %Ld"),iPosition.Int64(),iAudioOutputStream->Position().Int64());
						StopTest(KErrGeneral);
						}					
					INFO_PRINTF2(_L("Playback paused for %d seconds"),iPause);
					INFO_PRINTF3(_L("Changing Volume and Balance while paused to Volume = %d and Balance = %d"),iVolume,iBalance);
					iAudioOutputStream->SetVolume(iVolume);
					iAudioOutputStream->SetBalanceL(iBalance);
					INFO_PRINTF1(_L("Resuming playback. Calling CMdaAudioOutputStream::Resume"));
					error = iAudioOutputStream->Resume();
					resumeposition = iAudioOutputStream->Position();
					if(error == KErrNone)
						{
						INFO_PRINTF1(_L("Resume returned with KErrNone as expected"));
						iAudioOutputStreamState = EStateAOSPlaying;
						if(Abs(resumeposition.Int64() - iPosition.Int64()) <= KOneSecond/2)
							{
							INFO_PRINTF2(_L("Playback resumed from expected position %Ld"),iPosition.Int64());
							}
						else
							{
							ERR_PRINTF3(_L("Playback did not resume from expected position. Expected %Ld Retrieved %Ld"),iPosition.Int64(),resumeposition.Int64());
							StopTest(KErrGeneral);
							}
						}
					else
						{
						ERR_PRINTF2(_L("Resume returned with %d instead of KErrNone as expected"),error);
						StopTest(error);
						}
					iConfig = EFalse;
					}
				else if(!iWait && iInvalidConfig)
					{
					if(iPosition == iAudioOutputStream->Position())
						{
						INFO_PRINTF2(_L("Paused position was maintained at %Ld"),iPosition.Int64());
						}
					else
						{
						ERR_PRINTF3(_L("Position was not maintained during pause. Expected %Ld Retrieved %Ld"),iPosition.Int64(),iAudioOutputStream->Position().Int64());
						StopTest(KErrGeneral);
						}					
					INFO_PRINTF2(_L("Playback paused for %d seconds"),iPause);
					INFO_PRINTF1(_L("Changing AudioProperties while paused"));
					TRAPD(err,iAudioOutputStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate48000Hz, TMdaAudioDataSettings::EChannelsStereo));
					if(err == KErrNotReady)
						{
						INFO_PRINTF1(_L("SetAudioPropertiesL returned with KErrNotReady as expected"));
						INFO_PRINTF1(_L("Resuming playback. Calling CMdaAudioOutputStream::Resume"));
						error = iAudioOutputStream->Resume();
						resumeposition = iAudioOutputStream->Position();
						if(error == KErrNone)
							{
							INFO_PRINTF1(_L("Resume returned with KErrNone as expected"));
							iAudioOutputStreamState = EStateAOSPlaying;
							if(Abs(resumeposition.Int64() - iPosition.Int64()) <= KOneSecond/2)
								{
								INFO_PRINTF2(_L("Playback resumed from expected position %Ld"),iPosition.Int64());
								}
							else
								{
								ERR_PRINTF3(_L("Playback did not resume from expected position. Expected %Ld Retrieved %Ld"),iPosition.Int64(),resumeposition.Int64());
								StopTest(KErrGeneral);
								}							
							}
						else
							{
							ERR_PRINTF2(_L("Resume returned with %d instead of KErrNone as expected"),err);
							StopTest(err);
							}
						iInvalidConfig = EFalse;
						}
					else
						{
						ERR_PRINTF2(_L("SetAudioPropertiesL did not return with KErrNotSupported as expected, returned with %d instead"), err);
						StopTest(KErrGeneral);
						}
					}
				else if(!iWait && iGetBytes)
					{
					if(iPosition == iAudioOutputStream->Position())
						{
						INFO_PRINTF2(_L("Paused position was maintained at %Ld"),iPosition.Int64());
						}
					else
						{
						ERR_PRINTF3(_L("Position was not maintained during pause. Expected %Ld Retrieved %Ld"),iPosition.Int64(),iAudioOutputStream->Position().Int64());
						StopTest(KErrGeneral);
						}					
					INFO_PRINTF2(_L("Playback paused for %d seconds"),iPause);
					INFO_PRINTF1(_L("Calling GetBytes to verify the value hasn't changed"));
					TInt myBytes = iAudioOutputStream->GetBytes();
					if(myBytes == iBytes)
						{
						INFO_PRINTF1(_L("GetBytes value did not change while in pause, this is expected."));
						INFO_PRINTF1(_L("Resuming playback. Calling CMdaAudioOutputStream::Resume"));
						error = iAudioOutputStream->Resume();
						resumeposition = iAudioOutputStream->Position();
						if(error == KErrNone)
							{
							INFO_PRINTF1(_L("Resume returned with KErrNone as expected"));
							iAudioOutputStreamState = EStateAOSPlaying;
							if(Abs(resumeposition.Int64() - iPosition.Int64()) <= KOneSecond/2)
								{
								INFO_PRINTF2(_L("Playback resumed from expected position %Ld"),iPosition.Int64());
								}
							else
								{
								ERR_PRINTF3(_L("Playback did not resume from expected position. Expected %Ld Retrieved %Ld"),iPosition.Int64(),resumeposition.Int64());
								StopTest(KErrGeneral);
								}							
							}
						else
							{
							ERR_PRINTF2(_L("Resume returned with %d instead of KErrNone as expected"),error);
							StopTest(error);
							}
						iGetBytes = EFalse;
						}
					else
						{
						ERR_PRINTF3(_L("GetBytes value changed while in pause, this is unexpected. Expected = %d Retrieved = %d"),iBytes,myBytes);
						StopTest(KErrGeneral);
						}
					}
				}
			else
				{
				ERR_PRINTF2(_L("EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid MdaAudioOutputStream state received: %d"), iAudioOutputStreamState);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *
 * MaoscOpenComplete - From MMdaAudioOutputStreamObserver
 *
 */
void RMdaOutputStreamTestBase::MaoscOpenComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== AudioOutputStream MaoscOpenComplete() callback =========="));
	if(iAudioOutputStreamState == EStateAOSInitializing)
		{
		INFO_PRINTF1(_L("AudioOutputStream MaoscOpenComplete"));
		Fsm(EEventAOSOpenComplete, aError);
		}
	}

/*
 *
 * MaoscBufferCopied - From MMdaAudioOutputStreamObserver
 *
 */
void RMdaOutputStreamTestBase::MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	INFO_PRINTF1(_L("========== AudioOutputStream MaoscBufferCopied() callback =========="));
	INFO_PRINTF1(_L("Reading file"));
	TInt err = iFile.Read(iBuffer);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		StopTest(err);
		}
	if (iBuffer != KNullDesC8 && iAudioOutputStreamState != EStateAOSStopped)
		{
		INFO_PRINTF1(_L("Writing to stream"));
		iAudioOutputStream->WriteL(iBuffer);
		}
	if(iAudioOutputStreamState == EStateAOSPlaying)
		{
		INFO_PRINTF1(_L("AudioOutputStream MaoscBufferCopied"));
		}
	else if(iAudioOutputStreamState == EStateAOSPause)
		{
		ERR_PRINTF1(_L("AudioOutputStream MaoscBufferCopied in Paused. This is unexpected"));
		StopTest(KErrGeneral);
		}		
	}

/*
 *
 * MaoscPlayComplete - From MMdaAudioOutputStreamObserver
 *
 */
void RMdaOutputStreamTestBase::MaoscPlayComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== AudioOutputStream MaoscPlayComplete() callback =========="));
	if (aError == KErrUnderflow)
		{
		INFO_PRINTF2(_L("AudioOutputStream called MaoscPlayComplete with error = %d as expected"), aError);
		StopTest(EPass);
		}
	else
		{
		ERR_PRINTF2(_L("AudioOutputStream called MaoscPlayComplete with error = %d"), aError);
		ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
		StopTest(aError);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0175-HP
 *========================================================================================================
 */
RMdaOutputStreamPauseResumeBeforeWriteTest::RMdaOutputStreamPauseResumeBeforeWriteTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaOutputStreamTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaOutputStreamPauseResumeBeforeWriteTest* RMdaOutputStreamPauseResumeBeforeWriteTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaOutputStreamPauseResumeBeforeWriteTest * self = new(ELeave)RMdaOutputStreamPauseResumeBeforeWriteTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaOutputStreamPauseResumeBeforeWriteTest::DoKickoffTestL()
	{
	TPtrC filename;
	// Get the filename of the audio file to play
	if (!GetStringFromConfig(iSectName, KRawFile, filename))
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
		StopTest(err);
		return;
		}
	// Get size of iFile
	TInt filesize = 0;
	err = iFile.Size(filesize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
		StopTest(err);
		return;
		}
	// Initialise iBuf
	iBuffer.CreateMaxL(filesize/KFileDivision);
	
	//Read first half of the file
	err = iFile.Read(iBuffer);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		StopTest(err);
		}	
			
	iWait = ETrue;
	iPause = KTimeout; //Set timeout to stop test if playback was paused
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * Fsm - Executes playing events of AudioToneUtility in sequence
 *
 */
void RMdaOutputStreamPauseResumeBeforeWriteTest::Fsm(TMdaAOSEvent aMdaAudioOutputStreamEvent, TInt aError)
	{
	TInt error = 0;
	TTimeIntervalMicroSeconds resumeposition;
	switch (iAudioOutputStreamState)
		{
		case EStateAOSCreated:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSInitialize)
				{
				TMdaAudioDataSettings settings;
				//Default settings
				settings.Query();
				settings.iChannels = TMdaAudioDataSettings::EChannelsMono;
				settings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Pause before Open"));
				error = iAudioOutputStream->Pause();
				if(error == KErrNotReady)
					{
					INFO_PRINTF1(_L("Pause before open returned with KErrNotReady as expected"));
					}
				else
					{
					ERR_PRINTF2(_L("Pause before open returned with %d instead of KErrNotReady as expected"),error);
					StopTest(error);
					}
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Open"));
				iAudioOutputStream->Open(&settings);
				INFO_PRINTF1(_L("MdaAudioOutputStream State: EStateInitializing"));
				iAudioOutputStreamState = EStateAOSInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioOutputStream EEventInitialize not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSInitializing:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSOpenComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling SetVolume using MaxVolume/2"));
				iAudioOutputStream->SetVolume(iAudioOutputStream->MaxVolume()/2);
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Pause before first WriteL"));
				error = iAudioOutputStream->Pause();
				if(error == KErrNotReady)
					{
					INFO_PRINTF1(_L("Pause before first WriteL returned with KErrNotReady as expected"));
					}
				else
					{
					ERR_PRINTF2(_L("Pause before first WriteL returned with %d instead of KErrNotReady as expected"),error);
					StopTest(error);
					}
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Resume before first WriteL"));
				error = iAudioOutputStream->Resume();
				if(error == KErrNotReady)
					{
					INFO_PRINTF1(_L("Resume before first WriteL returned with KErrNotReady as expected"));
					}
				else
					{
					ERR_PRINTF2(_L("Resume before first WriteL returned with %d instead of KErrNotReady as expected"),error);
					StopTest(error);
					}						
				INFO_PRINTF1(_L("Starting playback"));
				iAudioOutputStream->WriteL(iBuffer);
				iAudioOutputStreamState = EStateAOSPlaying;
				StartTimer(iPause*KOneSecond);
				}
			else if (aMdaAudioOutputStreamEvent == EEventAOSOpenComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("MaoscOpenComplete returned with error = %d"), aError);
				StopTest(aError);	
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioOutputStream EEventOpenComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid MdaAudioOutputStream state received: %d"), iAudioOutputStreamState);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaOutputStreamPauseResumeBeforeWriteTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Cancelling timer"));
	iTimer->Cancel();
	ERR_PRINTF1(_L("Playback was paused when it shouldn't"));
	StopTest(KErrGeneral);
	}
	
/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0176-HP
 *========================================================================================================
 */
RMdaOutputStreamResumeTest::RMdaOutputStreamResumeTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaOutputStreamTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaOutputStreamResumeTest* RMdaOutputStreamResumeTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaOutputStreamResumeTest * self = new(ELeave)RMdaOutputStreamResumeTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaOutputStreamResumeTest::DoKickoffTestL()
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
	if (!GetStringFromConfig(iSectName, KRawFile, filename))
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
		StopTest(err);
		return;
		}
	// Get size of iFile
	TInt filesize = 0;
	err = iFile.Size(filesize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
		StopTest(err);
		return;
		}
	// Initialise iBuf
	iBuffer.CreateMaxL(filesize/KFileDivision);
	
	//Read first half of the file
	err = iFile.Read(iBuffer);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		StopTest(err);
		}	
			
	iWait = ETrue;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaOutputStreamResumeTest::DoTimerCallback()
	{
	if(!iWait)
		{
		INFO_PRINTF1(_L("Cancelling timer"));
		iTimer->Cancel();
		}
	else
		{
		INFO_PRINTF1(_L("MdaAudioOutputStream Event: EEventTimerComplete"));
		}
	Fsm (EEventAOSTimerComplete, KErrNone);
	}
	
/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0177-HP
 *========================================================================================================
 */
RMdaOutputStreamResumeThriceTest::RMdaOutputStreamResumeThriceTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaOutputStreamTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaOutputStreamResumeThriceTest* RMdaOutputStreamResumeThriceTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaOutputStreamResumeThriceTest * self = new(ELeave)RMdaOutputStreamResumeThriceTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaOutputStreamResumeThriceTest::DoKickoffTestL()
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
	if (!GetStringFromConfig(iSectName, KRawFile, filename))
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
		StopTest(err);
		return;
		}
	// Get size of iFile
	TInt filesize = 0;
	err = iFile.Size(filesize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
		StopTest(err);
		return;
		}
	// Initialise iBuf
	iBuffer.CreateMaxL(filesize/KFileDivision);
	
	//Read first half of the file
	err = iFile.Read(iBuffer);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		StopTest(err);
		}	
			
	iWait = ETrue;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaOutputStreamResumeThriceTest::DoTimerCallback()
	{
	iCount++;
	if(!iWait && iCount == 2*KRepeatThrice) //Repeating Pause-Resume cycle three times. Timer should be called 6 times
		{
		iTimer->Cancel();
		Fsm (EEventAOSTimerComplete, KErrNone);
		}
	else
		{
		INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
		Fsm (EEventAOSTimerComplete, KErrNone);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0178-HP
 *========================================================================================================
 */
RMdaOutputStreamConfigInPauseTest::RMdaOutputStreamConfigInPauseTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaOutputStreamTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaOutputStreamConfigInPauseTest* RMdaOutputStreamConfigInPauseTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaOutputStreamConfigInPauseTest * self = new(ELeave)RMdaOutputStreamConfigInPauseTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaOutputStreamConfigInPauseTest::DoKickoffTestL()
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
	if (!GetStringFromConfig(iSectName, KRawFile, filename))
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
		StopTest(err);
		return;
		}
	// Get size of iFile
	TInt filesize = 0;
	err = iFile.Size(filesize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
		StopTest(err);
		return;
		}
	// Initialise iBuf
	iBuffer.CreateMaxL(filesize/KFileDivision);
	
	//Read first half of the file
	err = iFile.Read(iBuffer);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		StopTest(err);
		}	
			
	iWait = ETrue;
	iConfig = ETrue;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaOutputStreamConfigInPauseTest::DoTimerCallback()
	{
	if(!iWait && !iConfig)
		{
		iTimer->Cancel();
		INFO_PRINTF1(_L("Comparing Volume and Balance with values set"));
		if (iVolume == iAudioOutputStream->Volume() && Abs(iBalance - iAudioOutputStream->GetBalanceL())<KBalanceTolerance)
			{
			INFO_PRINTF3(_L("Volume = %d and Balance = %d as expected"),iVolume,iBalance);
			}
		else
			{
			ERR_PRINTF1(_L("Retrieved values are different than expected"));
			ERR_PRINTF5(_L("Retrieved Volume = %d and Balance = %d. Expected Volume = %d and Balance = %d"),iAudioOutputStream->Volume(),iAudioOutputStream->GetBalanceL(),iVolume,iBalance);
			StopTest(KErrGeneral);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
		Fsm (EEventAOSTimerComplete, KErrNone);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0179-HP
 *========================================================================================================
 */
RMdaOutputStreamInvalidConfigInPauseTest::RMdaOutputStreamInvalidConfigInPauseTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaOutputStreamTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaOutputStreamInvalidConfigInPauseTest* RMdaOutputStreamInvalidConfigInPauseTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaOutputStreamInvalidConfigInPauseTest * self = new(ELeave)RMdaOutputStreamInvalidConfigInPauseTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaOutputStreamInvalidConfigInPauseTest::DoKickoffTestL()
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
	if (!GetStringFromConfig(iSectName, KRawFile, filename))
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
		StopTest(err);
		return;
		}
	// Get size of iFile
	TInt filesize = 0;
	err = iFile.Size(filesize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
		StopTest(err);
		return;
		}
	// Initialise iBuf
	iBuffer.CreateMaxL(filesize/KFileDivision);
	
	//Read first half of the file
	err = iFile.Read(iBuffer);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		StopTest(err);
		}	
			
	iWait = ETrue;
	iInvalidConfig = ETrue;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaOutputStreamInvalidConfigInPauseTest::DoTimerCallback()
	{
	if(!iWait)
		{
		INFO_PRINTF1(_L("Cancelling timer"));
		iTimer->Cancel();
		}
	else
		{
		INFO_PRINTF1(_L("MdaAudioOutputStream Event: EEventTimerComplete"));
		}
	Fsm (EEventAOSTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0180-HP
 *========================================================================================================
 */
RMdaOutputStreamGetBytesInPauseTest::RMdaOutputStreamGetBytesInPauseTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaOutputStreamTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaOutputStreamGetBytesInPauseTest* RMdaOutputStreamGetBytesInPauseTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaOutputStreamGetBytesInPauseTest * self = new(ELeave)RMdaOutputStreamGetBytesInPauseTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaOutputStreamGetBytesInPauseTest::DoKickoffTestL()
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
	if (!GetStringFromConfig(iSectName, KRawFile, filename))
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
		StopTest(err);
		return;
		}
	// Get size of iFile
	TInt filesize = 0;
	err = iFile.Size(filesize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
		StopTest(err);
		return;
		}
	// Initialise iBuf
	iBuffer.CreateMaxL(filesize/KFileDivision);
	
	//Read first half of the file
	err = iFile.Read(iBuffer);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		StopTest(err);
		}	
			
	iWait = ETrue;
	iGetBytes = ETrue;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaOutputStreamGetBytesInPauseTest::DoTimerCallback()
	{
	if(!iWait)
		{
		INFO_PRINTF1(_L("Cancelling timer"));
		iTimer->Cancel();
		}
	else
		{
		INFO_PRINTF1(_L("MdaAudioOutputStream Event: EEventTimerComplete"));
		}
	Fsm (EEventAOSTimerComplete, KErrNone);
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0181-HP
 *========================================================================================================
 */
RMdaOutputStreamPlayPauseStopPlayTest::RMdaOutputStreamPlayPauseStopPlayTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaOutputStreamTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaOutputStreamPlayPauseStopPlayTest* RMdaOutputStreamPlayPauseStopPlayTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaOutputStreamPlayPauseStopPlayTest * self = new(ELeave)RMdaOutputStreamPlayPauseStopPlayTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaOutputStreamPlayPauseStopPlayTest::DoKickoffTestL()
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
	if (!GetStringFromConfig(iSectName, KRawFile, filename))
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
		StopTest(err);
		return;
		}
	// Get size of iFile
	TInt filesize = 0;
	err = iFile.Size(filesize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
		StopTest(err);
		return;
		}
	// Initialise iBuf
	iBuffer.CreateMaxL(filesize/KFileDivision);
	
	//Read first half of the file
	err = iFile.Read(iBuffer);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		StopTest(err);
		}	
			
	iWait = ETrue;
	iStop = ETrue;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaOutputStreamPlayPauseStopPlayTest::DoTimerCallback()
	{
	if(!iWait && iStop)
		{
		iAudioOutputStreamState = EStateAOSStopped;
		iStop = EFalse;
		iWait = ETrue;
		INFO_PRINTF1(_L("MAudioOutputStream Event: EEventTimerComplete"));
		Fsm (EEventAOSTimerComplete, KErrNone);
		}
	else if(!iWait && !iStop)
		{
		iTimer->Cancel();
		Fsm (EEventAOSTimerComplete, KErrNone);
		}
	else
		{
		INFO_PRINTF1(_L("MdaToneUtility Event: EEventTimerComplete"));
		Fsm (EEventAOSTimerComplete, KErrNone);
		}
	}

/*
 *
 * Fsm - Executes playing events of AudioToneUtility in sequence
 *
 */
void RMdaOutputStreamPlayPauseStopPlayTest::Fsm(TMdaAOSEvent aMdaAudioOutputStreamEvent, TInt aError)
	{
	TInt error = 0;
	TTimeIntervalMicroSeconds resumeposition;
	switch (iAudioOutputStreamState)
		{
		case EStateAOSCreated:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSInitialize)
				{
				TMdaAudioDataSettings settings;
				//Default settings
				settings.Query();
				settings.iChannels = TMdaAudioDataSettings::EChannelsMono;
				settings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Open"));
				iAudioOutputStream->Open(&settings);
				INFO_PRINTF1(_L("MdaAudioOutputStream State: EStateInitializing"));
				iAudioOutputStreamState = EStateAOSInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioOutputStream EEventInitialize not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSInitializing:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSOpenComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling SetVolume using MaxVolume/2"));
				iAudioOutputStream->SetVolume(iAudioOutputStream->MaxVolume()/2);
				INFO_PRINTF1(_L("Starting playback"));
				iAudioOutputStream->WriteL(iBuffer);
				iAudioOutputStreamState = EStateAOSPlaying;
				StartTimer(iPause*KOneSecond);
				}
			else if (aMdaAudioOutputStreamEvent == EEventAOSOpenComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("MaoscOpenComplete returned with error = %d"), aError);
				StopTest(aError);	
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioOutputStream EEventOpenComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSPlaying:
			{
			if(aMdaAudioOutputStreamEvent == EEventAOSTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Pause"));
				error = iAudioOutputStream->Pause();
				if(error == KErrNone)
					{
					INFO_PRINTF1(_L("Pause returned with KErrNone as expected"));
					}
				else
					{
					ERR_PRINTF2(_L("Pause returned with %d instead of KErrNone as expected"),error);
					StopTest(error);
					}
				iPosition = iAudioOutputStream->Position();
				INFO_PRINTF2(_L("Stream was paused at %Ld"),iPosition.Int64());
				INFO_PRINTF1(_L("MdaAudioOutputStream State: EStatePause"));
				iAudioOutputStreamState = EStateAOSPause;
				iWait = EFalse;
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSPause:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSTimerComplete)
				{
				if(iPosition == iAudioOutputStream->Position())
					{
					INFO_PRINTF2(_L("Paused position was maintained at %Ld"),iPosition.Int64());
					}
				else
					{
					ERR_PRINTF3(_L("Position was not maintained during pause. Expected %Ld Retrieved %Ld"),iPosition.Int64(),iAudioOutputStream->Position().Int64());
					StopTest(KErrGeneral);
					}
				INFO_PRINTF1(_L("Resuming playback. Calling CMdaAudioOutputStream::Resume"));
				error = iAudioOutputStream->Resume();
				resumeposition = iAudioOutputStream->Position();
				if(error == KErrNone)
					{
					INFO_PRINTF1(_L("Resume returned with KErrNone as expected"));
					iAudioOutputStreamState = EStateAOSPlaying;
					if(Abs(resumeposition.Int64() - iPosition.Int64()) <= KOneSecond/2)
						{
						INFO_PRINTF2(_L("Playback resumed from expected position %Ld"),iPosition.Int64());
						}
					else
						{
						ERR_PRINTF3(_L("Playback did not resume from expected position. Expected %Ld Retrieved %Ld"),iPosition.Int64(),resumeposition.Int64());
						StopTest(KErrGeneral);
						}
					}
				else
					{
					ERR_PRINTF2(_L("Resume returned with %d instead of KErrNone as expected"),error);
					StopTest(error);
					}				
				}
			else
				{
				ERR_PRINTF2(_L("EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSStopped:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSTimerComplete)
				{
				if(iWait)
					{
					INFO_PRINTF1(_L("Stopping playback for 2 seconds"));
					iAudioOutputStream->Stop();
					iWait = EFalse;
					}
				else
					{
					INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Resume while in Stop"));
					error = iAudioOutputStream->Resume();
					if(error == KErrNotReady)
						{
						INFO_PRINTF1(_L("Resume returned with KErrNotReady as expected"));
						}
					else
						{
						ERR_PRINTF2(_L("Resume returned with %d instead of KErrNotReady as expected"),error);
						StopTest(error);
						}
					TInt filepos = 0;
					INFO_PRINTF1(_L("Restarting file to start position"));
					error = iFile.Seek(ESeekStart, filepos);
					if (error != KErrNone)
						{
						INFO_PRINTF2(_L("Error restarting iFile %d"), error);
						StopTest(error);
						}
					error = iFile.Read(iBuffer);
					if (error != KErrNone)
						{
						INFO_PRINTF2(_L("Error reading iFile %d"), error);
						StopTest(error);
						}
					INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::WriteL while in Stop to restart playback"));
					iAudioOutputStream->WriteL(iBuffer);
					iAudioOutputStreamState = EStateAOSPlaying;
					}
				}
			else
				{
				ERR_PRINTF2(_L("EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid MdaAudioOutputStream state received: %d"), iAudioOutputStreamState);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *
 * MaoscPlayComplete - From MMdaAudioOutputStreamObserver
 *
 */
void RMdaOutputStreamPlayPauseStopPlayTest::MaoscPlayComplete(TInt aError)
	{
	INFO_PRINTF1(_L("========== AudioOutputStream MaoscPlayComplete() callback =========="));
	if(iAudioOutputStreamState == EStateAOSStopped)
		{
		if (aError == KErrCancel)
			{
			INFO_PRINTF2(_L("AudioOutputStream called MaoscPlayComplete with error = %d as expected"), aError);
			}
		else
			{
			ERR_PRINTF2(_L("AudioOutputStream called MaoscPlayComplete with error = %d"), aError);
			ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
			StopTest(aError);
			}
		}
	else if(iAudioOutputStreamState == EStateAOSPlaying)
		{
		if(aError == KErrUnderflow)
			{
			INFO_PRINTF2(_L("AudioOutputStream called MaoscPlayComplete with error = %d as expected"), aError);
			StopTest(EPass);
			}
		else
			{
			ERR_PRINTF2(_L("AudioOutputStream called MaoscPlayComplete with error = %d"), aError);
			ERR_PRINTF2(_L("Expected error = %d"), KErrUnderflow);
			StopTest(aError);
			}
		}
	else
		{
		ERR_PRINTF1(_L("AudioOutputStream called MaoscPlayComplete at invalid state"));
		StopTest(KErrGeneral);
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0182-HP
 *========================================================================================================
 */
RMdaOutputStreamInvalidFormatsTest::RMdaOutputStreamInvalidFormatsTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaOutputStreamTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaOutputStreamInvalidFormatsTest* RMdaOutputStreamInvalidFormatsTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaOutputStreamInvalidFormatsTest * self = new(ELeave)RMdaOutputStreamInvalidFormatsTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaOutputStreamInvalidFormatsTest::DoKickoffTestL()
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
	if (!GetStringFromConfig(iSectName, KOggFile, filename))
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
		StopTest(err);
		return;
		}
	// Get size of iFile
	TInt filesize = 0;
	err = iFile.Size(filesize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
		StopTest(err);
		return;
		}
	// Initialise iBuf
	iBuffer.CreateMaxL(filesize/KFileDivision);
	
	//Read first half of the file
	err = iFile.Read(iBuffer);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		StopTest(err);
		}	
	
	iWait = ETrue;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaOutputStreamInvalidFormatsTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Cancelling timer"));
	iTimer->Cancel();
	Fsm (EEventAOSTimerComplete, KErrNone);
	}
	
/*
 *
 * Fsm - Executes playing events of AudioToneUtility in sequence
 *
 */
void RMdaOutputStreamInvalidFormatsTest::Fsm(TMdaAOSEvent aMdaAudioOutputStreamEvent, TInt aError)
	{
	TInt error = 0;
	TTimeIntervalMicroSeconds resumeposition;
	switch (iAudioOutputStreamState)
		{
		case EStateAOSCreated:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSInitialize)
				{
				TMdaAudioDataSettings settings;
				//setting for OGG file
				settings.Query();
				settings.iChannels = TMdaAudioDataSettings::EChannelsStereo;
				settings.iSampleRate = TMdaAudioDataSettings::ESampleRate44100Hz;
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Open"));
				iAudioOutputStream->Open(&settings);
				INFO_PRINTF1(_L("MdaAudioOutputStream State: EStateInitializing"));
				iAudioOutputStreamState = EStateAOSInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioOutputStream EEventInitialize not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSInitializing:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSOpenComplete && aError == KErrNone)
				{
				TFourCC KVORBDataType('V', 'O', 'R', 'B');
				TRAPD(err,iAudioOutputStream->SetDataTypeL(KVORBDataType));
				if(err != KErrNone)
					{
					INFO_PRINTF1(_L("Failed to set non-PCM Format"));
					StopTest(err);
					}
				INFO_PRINTF1(_L("Calling SetVolume using MaxVolume/2"));
				iAudioOutputStream->SetVolume(iAudioOutputStream->MaxVolume()/2);
				INFO_PRINTF1(_L("Starting playback"));
				iAudioOutputStream->WriteL(iBuffer);
				iAudioOutputStreamState = EStateAOSPlaying;
				StartTimer(iPause*KOneSecond);
				}
			else if (aMdaAudioOutputStreamEvent == EEventAOSOpenComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("MaoscOpenComplete returned with error = %d"), aError);
				StopTest(aError);	
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioOutputStream EEventOpenComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSPlaying:
			{
			if(aMdaAudioOutputStreamEvent == EEventAOSTimerComplete)
				{
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Pause while playing non-PCM format"));
				error = iAudioOutputStream->Pause();
				if(error == KErrNotSupported)
					{
					INFO_PRINTF1(_L("Pause while playing non-PCM format returned with KErrNotSupported as expected"));
					}
				else
					{
					ERR_PRINTF2(_L("Pause while playing non-PCM format returned with %d instead of KErrNotReady as expected"),error);
					StopTest(error);
					}
				}
			else
				{
				ERR_PRINTF2(_L("DevSound EEventTimerComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid MdaAudioOutputStream state received: %d"), iAudioOutputStreamState);
			StopTest(KErrGeneral);
			}
		}
	}

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0184-HP
 *========================================================================================================
 */
RMdaOutputStreamPauseNonA3FTest::RMdaOutputStreamPauseNonA3FTest(const TDesC& aTestName, const TDesC& aSectName)
	:	RMdaOutputStreamTestBase(aTestName,aSectName)
	{
	}

/*
 *
 * NewL
 *
 */
RMdaOutputStreamPauseNonA3FTest* RMdaOutputStreamPauseNonA3FTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RMdaOutputStreamPauseNonA3FTest * self = new(ELeave)RMdaOutputStreamPauseNonA3FTest(aTestName,aSectName);
	return self;
	}

/*
 *
 * DoKickoffTestL
 *
 */
void RMdaOutputStreamPauseNonA3FTest::DoKickoffTestL()
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
	if (!GetStringFromConfig(iSectName, KRawFile, filename))
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
		StopTest(err);
		return;
		}
	// Get size of iFile
	TInt filesize = 0;
	err = iFile.Size(filesize);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
		StopTest(err);
		return;
		}
	// Initialise iBuf
	iBuffer.CreateMaxL(filesize/KFileDivision);
	
	//Read first half of the file
	err = iFile.Read(iBuffer);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile %d"), err);
		StopTest(err);
		}	
	
	iPause = KTimeout; //Set timeout to stop test if playback was paused
	iWait = ETrue;
	INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}

/*
 *
 * DoTimerCallback
 *
 */
void RMdaOutputStreamPauseNonA3FTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Cancelling timer"));
	iTimer->Cancel();
	ERR_PRINTF1(_L("Playback was paused when it shouldn't"));
	StopTest(KErrGeneral);	
	}

/*
 *
 * Fsm - Executes playing events of AudioToneUtility in sequence
 *
 */
void RMdaOutputStreamPauseNonA3FTest::Fsm(TMdaAOSEvent aMdaAudioOutputStreamEvent, TInt aError)
	{
	switch (iAudioOutputStreamState)
		{
		case EStateAOSCreated:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSInitialize)
				{
				TMdaAudioDataSettings settings;
				//Default settings
				settings.Query();
				settings.iChannels = TMdaAudioDataSettings::EChannelsMono;
				settings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Open"));
				iAudioOutputStream->Open(&settings);
				INFO_PRINTF1(_L("MdaAudioOutputStream State: EStateInitializing"));
				iAudioOutputStreamState = EStateAOSInitializing;
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioOutputStream EEventInitialize not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		case EStateAOSInitializing:
			{
			if (aMdaAudioOutputStreamEvent == EEventAOSOpenComplete && aError == KErrNone)
				{
				INFO_PRINTF1(_L("Calling SetVolume using MaxVolume/2"));
				iAudioOutputStream->SetVolume(iAudioOutputStream->MaxVolume()/2);
				INFO_PRINTF1(_L("Starting playback"));
				iAudioOutputStream->WriteL(iBuffer);
				StartTimer(iPause*KOneSecond);
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Pause in non-a3f configuration"));
				TInt err = iAudioOutputStream->Pause();
				if(err == KErrNotSupported)
					{
					INFO_PRINTF1(_L("Pause in CMdaAudioOutputStream returned with KErrNotSupported as expected"));
					}
				else
					{
					ERR_PRINTF2(_L("Pause in CMdaAudioOutputStream returned with %d instead of KErrNotSupported as expected"),err);
					StopTest(err);
					}
				INFO_PRINTF1(_L("Calling CMdaAudioOutputStream::Resume in non-a3f configuration"));
				err = iAudioOutputStream->Resume();
				if(err == KErrNotReady)
					{
					INFO_PRINTF1(_L("Resume in CMdaAudioOutputStream returned with KErrNotReady as expected"));
					}
				else
					{
					ERR_PRINTF2(_L("Resume in CMdaAudioOutputStream returned with %d instead of KErrNotReady as expected"),err);
					StopTest(err);
					}					
				}
			else if (aMdaAudioOutputStreamEvent == EEventAOSOpenComplete && aError != KErrNone)
				{
				ERR_PRINTF2(_L("MaoscOpenComplete returned with error = %d"), aError);
				StopTest(aError);	
				}
			else
				{
				ERR_PRINTF2(_L("MdaAudioOutputStream EEventOpenComplete not received as expected. Received event: %d"), aMdaAudioOutputStreamEvent);
				StopTest(aError, EFail);
				}
			break;
			}
		default:
			{
			ERR_PRINTF2(_L("Invalid MdaAudioOutputStream state received: %d"), iAudioOutputStreamState);
			StopTest(KErrGeneral);
			}
		}
	}
