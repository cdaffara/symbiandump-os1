// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "play_teststep_apr.h"
#include "char_videoplay_common_apr.h"
const TUint KEventUid = NULL;


//Register after opening and before playing the file
//MM-MMF-VCLNT-CHRTZ-I-0125

/**
 * Constructor
 */
RTestVclntAprRegAftrOpenAndB4Play::RTestVclntAprRegAftrOpenAndB4Play(const TDesC& aTestName, const TDesC& aSectName)
	: RTestBaseVideoPlayAPR(aTestName, aSectName)
	{
	}

/**
 * NewL
 */
RTestVclntAprRegAftrOpenAndB4Play* RTestVclntAprRegAftrOpenAndB4Play::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegAftrOpenAndB4Play* self = new (ELeave) RTestVclntAprRegAftrOpenAndB4Play(aTestName, aSectName);
	return self;
	}

/**
 * DoKickoffTestL()
 */
void RTestVclntAprRegAftrOpenAndB4Play::DoKickoffTestL()
	{
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.36 - Register after opening and before playing video player"));
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	}


/**
 * Fsm
 */
void RTestVclntAprRegAftrOpenAndB4Play::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((ValidateEvent(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iVideoPlayer
				TRAP(iError, iVideoPlayer->OpenFileL(iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
    				StopTest(iError, EFail);
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Register iVideoPlayer for Notification
				retErr = iVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					break;
					}
				// Prepare iVideoPlayer
				iVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iVideoPlayer
				iVideoPlayer->Play();
				INFO_PRINTF1(_L("iVideoPlayer->Play()"));
				// Open iAudioPlayer1
				TRAP(iError, iAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
    				StopTest(iError, EFail);
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iAudioPlayer1->Play();
				INFO_PRINTF1(_L("iAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1:
				{
				INFO_PRINTF1(_L("iVideoPlayer receives Notification"));
				// we have received all the events, so pass the test
				StopTest(iError, EPass);
				break;
				}
			}
		}
    }

//Register before opening the file
//MM-MMF-VCLNT-CHRTZ-I-0127

/**
 * Constructor
 */
RTestVclntAprRegBeforeOpen::RTestVclntAprRegBeforeOpen(const TDesC& aTestName, const TDesC& aSectName)
	: RTestBaseVideoPlayAPR(aTestName, aSectName)
	{
	}

/**
 * NewL
 */
RTestVclntAprRegBeforeOpen* RTestVclntAprRegBeforeOpen::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegBeforeOpen* self = new (ELeave) RTestVclntAprRegBeforeOpen(aTestName, aSectName);
	return self;
	}

/**
 * DoKickoffTestL()
 */
void RTestVclntAprRegBeforeOpen::DoKickoffTestL()
	{
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.36 - Register before opening the file"));
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	}


/**
 * Fsm
 */
void RTestVclntAprRegBeforeOpen::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((ValidateEvent(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Register iVideoPlayer for Notification
				retErr = iVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					break;
					}
				// Open iVideoPlayer
				TRAP(iError, iVideoPlayer->OpenFileL(iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
    				StopTest(iError, EFail);
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iVideoPlayer
				iVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iVideoPlayer
				iVideoPlayer->Play();
				INFO_PRINTF1(_L("iVideoPlayer->Play()"));
				// Open iAudioPlayer1
				TRAP(iError, iAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
    				StopTest(iError, EFail);
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iAudioPlayer1->Play();
				INFO_PRINTF1(_L("iAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1:
				{
				INFO_PRINTF1(_L("iVideoPlayer receives Notification"));
				// we have received all the events, so pass the test
				StopTest(iError, EPass);
				break;
				}
			}
		}
    }


//Register with some unsupported event UID
//MM-MMF-VCLNT-CHRTZ-I-0128

/**
 * Constructor
 */
RTestVclntAprRegWithUnSupptdEvntUid::RTestVclntAprRegWithUnSupptdEvntUid(const TDesC& aTestName, const TDesC& aSectName)
	: RTestBaseVideoPlayAPR(aTestName, aSectName)
	{
	}

/**
 * NewL
 */
RTestVclntAprRegWithUnSupptdEvntUid* RTestVclntAprRegWithUnSupptdEvntUid::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegWithUnSupptdEvntUid* self = new (ELeave) RTestVclntAprRegWithUnSupptdEvntUid(aTestName, aSectName);
	return self;
	}

/**
 * DoKickoffTestL()
 */
void RTestVclntAprRegWithUnSupptdEvntUid::DoKickoffTestL()
	{
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.36 - Register with some unsupported event UID"));
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	}


/**
 * Fsm
 */
void RTestVclntAprRegWithUnSupptdEvntUid::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
    {
	if ((ValidateEvent(aEventCode, aError)) && iCanStateChange)
        {
		TInt retErr = KErrNone;
		switch (iState)
            {
			case EStateBegin:
                {
				// Register iVideoPlayer for Notification
				retErr = iVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KUidInterfaceMMFAudioPlayDevice);
				INFO_PRINTF1(_L("iVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNotSupported )
					{
					ERR_PRINTF2(_L("iVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					}
				else
    				{
        			StopTest(retErr, EPass);				    
    				}
					break;    				
                }
            }
        }
    }

//Register multiple times
//MM-MMF-VCLNT-CHRTZ-I-0129

/**
 * Constructor
 */
RTestVclntAprRegMultpleTimes::RTestVclntAprRegMultpleTimes(const TDesC& aTestName, const TDesC& aSectName)
	: RTestBaseVideoPlayAPR(aTestName, aSectName)
	{
	}

/**
 * NewL
 */
RTestVclntAprRegMultpleTimes* RTestVclntAprRegMultpleTimes::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegMultpleTimes* self = new (ELeave) RTestVclntAprRegMultpleTimes(aTestName, aSectName);
	return self;
	}

/**
 * DoKickoffTestL()
 */
void RTestVclntAprRegMultpleTimes::DoKickoffTestL()
	{
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.36 - Register multiple times"));
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	}


/**
 * Fsm
 */
void RTestVclntAprRegMultpleTimes::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((ValidateEvent(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iVideoPlayer
				TRAP(iError, iVideoPlayer->OpenFileL(iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
    				StopTest(iError, EFail);
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Register iVideoPlayer for Notification
				retErr = iVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					break;
					}
				// Prepare iVideoPlayer
				iVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iVideoPlayer
				iVideoPlayer->Play();
				INFO_PRINTF1(_L("iVideoPlayer->Play()"));
				// Open iAudioPlayer1
				TRAP(iError, iAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
    				StopTest(iError, EFail);
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iAudioPlayer1->Play();
				INFO_PRINTF1(_L("iAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Register iVideoPlayer for Notification again
				retErr = iVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->RegisterAudioResourceNotification() again"));
				if( retErr != KErrAlreadyExists )
					{
					ERR_PRINTF2(_L("iVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					}
				else
    				{
    				INFO_PRINTF1(_L("RegisterAudioResourceNotification() returns KErrAlreadyExists"));
    				// we have received all the events, so pass the test
    				StopTest(retErr, EPass);    
    				}
				break;
				}
			}
		}
    }

//Cancellation of notification after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0130

/**
 * Constructor
 */
RTestVclntAprCancelNtfnAftrOpen::RTestVclntAprCancelNtfnAftrOpen(const TDesC& aTestName, const TDesC& aSectName)
	: RTestBaseVideoPlayAPR(aTestName, aSectName)
	{
	}

/**
 * NewL
 */
RTestVclntAprCancelNtfnAftrOpen* RTestVclntAprCancelNtfnAftrOpen::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprCancelNtfnAftrOpen* self = new (ELeave) RTestVclntAprCancelNtfnAftrOpen(aTestName, aSectName);
	return self;
	}

/**
 * DoKickoffTestL()
 */
void RTestVclntAprCancelNtfnAftrOpen::DoKickoffTestL()
	{
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.37 - Cancellation of registration after opening video player"));
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	}


/**
 * Fsm
 */
void RTestVclntAprCancelNtfnAftrOpen::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((ValidateEvent(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iVideoPlayer
				TRAP(iError, iVideoPlayer->OpenFileL(iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
    				StopTest(iError, EFail);
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Register iVideoPlayer for Notification
				retErr = iVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					}
				retErr = iVideoPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->CancelRegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->CancelRegisterAudioResourceNotification() - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					}
				// Prepare iVideoPlayer
				iVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iVideoPlayer
				iVideoPlayer->Play();
				INFO_PRINTF1(_L("iVideoPlayer->Play()"));
				// Open iAudioPlayer1
				TRAP(iError, iAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
    				StopTest(iError, EFail);
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iAudioPlayer1->Play();
				INFO_PRINTF1(_L("iAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1, KErrNone, 2);
				// Pass the test here,else as the registration has been  cancelled the test will get hanged
				StopTest(iError, EPass);
				break;
				}
			case EStateEndAudioPlay1:
				{
				INFO_PRINTF1(_L("iVideoPlayer receives Notification"));
				// If notification is received, fail the test
				StopTest(iError, EFail);
				break;
				}
			}
		}
    }


//Cancellation of notification with some Invalid event Uid
//MM-MMF-VCLNT-CHRTZ-I-0132

/**
 * Constructor
 */
RTestVclntAprCancelNtfnWthInvalidEvntUid::RTestVclntAprCancelNtfnWthInvalidEvntUid(const TDesC& aTestName, const TDesC& aSectName)
	: RTestBaseVideoPlayAPR(aTestName, aSectName)
	{
	}

/**
 * NewL
 */
RTestVclntAprCancelNtfnWthInvalidEvntUid* RTestVclntAprCancelNtfnWthInvalidEvntUid::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprCancelNtfnWthInvalidEvntUid* self = new (ELeave) RTestVclntAprCancelNtfnWthInvalidEvntUid(aTestName, aSectName);
	return self;
	}

/**
 * DoKickoffTestL()
 */
void RTestVclntAprCancelNtfnWthInvalidEvntUid::DoKickoffTestL()
	{
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.37 - Cancellation of registration with invalid event Uid"));
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	}


/**
 * Fsm
 */
void RTestVclntAprCancelNtfnWthInvalidEvntUid::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((ValidateEvent(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		TInt retErr1 = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iVideoPlayer
				TRAP(iError, iVideoPlayer->OpenFileL(iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
    				StopTest(iError, EFail);
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Register iVideoPlayer for Notification
				retErr = iVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					}
				retErr = iVideoPlayer->CancelRegisterAudioResourceNotification(KUidInterfaceMMFAudioRecordDevice);
				INFO_PRINTF1(_L("iVideoPlayer->CancelRegisterAudioResourceNotification() with unsupported event Uid"));
				if( retErr != KErrNotSupported )
					{
					ERR_PRINTF2(_L("iVideoPlayer->CancelRegisterAudioResourceNotification() - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					}
				else
                    {
                    retErr1 = iVideoPlayer->CancelRegisterAudioResourceNotification(TUid::Uid(KEventUid));
                    INFO_PRINTF1(_L("iVideoPlayer->CancelRegisterAudioResourceNotification() with NULL event Uid"));
                    if( retErr1 != KErrNone)
                        {
                        if( retErr1 == KErrNotSupported)
                            {
                            ERR_PRINTF2(_L("iVideoPlayer->CancelRegisterAudioResourceNotification() - Failed with Error : %d"), retErr1);
                            StopTest(retErr1, EPass);
                            }
                        else
                            {
                            StopTest(retErr1, EFail);    				    
                            }
                        }
                    else
                        {
                        ERR_PRINTF2(_L("iVideoPlayer->CancelRegisterAudioResourceNotification() - Failed with Error : %d"), retErr1);
                        StopTest(retErr1, EFail);   
                        }

                    }
				break;
				}
			}
		}
    }


//WiilResumePlay()
//MM-MMF-VCLNT-CHRTZ-I-0133

/**
 * Constructor
 */
RTestVclntAprWillResumePlay::RTestVclntAprWillResumePlay(const TDesC& aTestName, const TDesC& aSectName)
	: RTestBaseVideoPlayAPR(aTestName, aSectName)
	{
	}

/**
 * NewL
 */
RTestVclntAprWillResumePlay* RTestVclntAprWillResumePlay::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprWillResumePlay* self = new (ELeave) RTestVclntAprWillResumePlay(aTestName, aSectName);
	return self;
	}

/**
 * DoKickoffTestL()
 */
void RTestVclntAprWillResumePlay::DoKickoffTestL()
	{
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.38 - WillResumeplay with three clients"));
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	}

/**
 * set finished on callback from a timer
 */
void RTestVclntAprWillResumePlay::Finished()
    {
    // say that no events have occured after calling WillResumePlay()
    // and can proceed to the next step
    Fsm(EIdlePlayer, KErrNone);
    }

//
// CVclntAviAprWillResumeTimer
//

/**
 * NewL
 */
CVclntAviAprWillResumeTimer* CVclntAviAprWillResumeTimer::NewL(RTestVclntAprWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    {
    CVclntAviAprWillResumeTimer* self = new (ELeave) CVclntAviAprWillResumeTimer(aParent, aWaitTime);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
/**
 * Constructor
 */
CVclntAviAprWillResumeTimer::CVclntAviAprWillResumeTimer(RTestVclntAprWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    : CTimer(EPriorityNormal), iParent(aParent), iWaitTime(aWaitTime)
    {    
    CActiveScheduler::Add(this);
    }    
    
/**
 * Start() - Start the timer
 */
void CVclntAviAprWillResumeTimer::Start()
    {
    After(iWaitTime);
    }

/**
 * RunL() - see CActive class for more info
 */
void CVclntAviAprWillResumeTimer::RunL()
    {        
    // (iParent->Finished() should call Stop() or not, as we decide)    
    iParent->Finished();
    }
/**
 * Fsm
 */
void RTestVclntAprWillResumePlay::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((ValidateEvent(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		TInt retErr1 = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iAudioPlayer2 - Low priority
				TRAP(iError, iAudioPlayer2->OpenFileL(iFilename3));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iAudioPlayer2->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);    				    
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer2, EMapcInitComplete2);
				break;
				}
			case EStateOpenAudioPlayer2:
				{
			 	// Play iAudioPlayer2
				iAudioPlayer2->Play();
				INFO_PRINTF1(_L("iAudioPlayer2->Play()"));
				// Open iVideoPlayer
				TRAP(iError, iVideoPlayer->OpenFileL(iFilename1));
				INFO_PRINTF2(_L("iVideoPlayer->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					StopTest(iError, EFail);    				    
					break;
					}	
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iVideoPlayer - Medium priority
				iVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iVideoPlayer
				iVideoPlayer->Play();
				INFO_PRINTF1(_L("iVideoPlayer->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseAudioPlayer2, EMapcPlayComplete2, KErrInUse);
				break;
				}
			case EStateErrInUseAudioPlayer2:
				{
				// Register iAudioPlayer2 for Notification
				retErr = iAudioPlayer2->RegisterAudioResourceNotification(*iAudioPlayerCallbackSupport2, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iAudioPlayer2->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iAudioPlayer2->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					StopTest(iError, EFail);    				    
					break;
					}
				// Open iAudioPlayer1 - High priority
				TRAP(iError, iAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					StopTest(iError, EFail);    				    
					break;
					}				
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iAudioPlayer1
				iAudioPlayer1->Play();
				INFO_PRINTF1(_L("iAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Register iVideoPlayer for Notification
				retErr1 = iVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr1 != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr1);
					StopTest(iError, EFail);    				    
					break;
					}
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1:
				{
				INFO_PRINTF1(_L("iVideoPlayer->NotificationReceived"));
				retErr = iVideoPlayer->WillResumePlay();
				INFO_PRINTF1(_L("iVideoPlayer->WillResumePlay()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->WillResumePlay() - Failed with Error : %d"), retErr);
					StopTest(iError, EFail);    				    
					break;
					}
				// initialise and start the timer (5 Sec), just to check if no other event occurs.
                TRAPD(err, iWillResumePlayTimer = CVclntAviAprWillResumeTimer::NewL(this, 5000000));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CVclntAviAprWillResumeTimer::NewL left with error = %d"), err);
                    StopTest(iError, EFail);    				    
                    break;
                    }
                iWillResumePlayTimer->Start();
				// Check if No other client is notified in 5 seconds.
				StateChange(EStateNoOtherPlayerNotified, EIdlePlayer);
				break;
				}
			case EStateNoOtherPlayerNotified:
				{
				INFO_PRINTF1(_L("No other client has been notified in 5 seconds after calling WillResumePlay()"));
				iVideoPlayer->Play(); // directly calling play, without setting position.
				iExpEventCount = 2;
				StateChange(EStateNotifiedAudioPlayer2, EMarncResourceAvailable3, KErrNone, 1);
				StateChange(EStateEndVideoPlay, EMvpuoPlayComplete, KErrNone, 2);
				break;
				}
			case EStateEndVideoPlay:
				{
				INFO_PRINTF1(_L("iAudioPlayer2->NotificationReceived"));
				StopTest(iError, EPass);    				    
				break;
				}
			}
		}
	}
		

//Cancellation of notification without registration after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0134

/**
 * Constructor
 */
RTestVclntAprCancelNtfnWthRegnAftrOpen::RTestVclntAprCancelNtfnWthRegnAftrOpen(const TDesC& aTestName, const TDesC& aSectName)
	: RTestBaseVideoPlayAPR(aTestName, aSectName)
	{
	}

/**
 * NewL
 */
RTestVclntAprCancelNtfnWthRegnAftrOpen* RTestVclntAprCancelNtfnWthRegnAftrOpen::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprCancelNtfnWthRegnAftrOpen* self = new (ELeave) RTestVclntAprCancelNtfnWthRegnAftrOpen(aTestName, aSectName);
	return self;
	}

/**
 * DoKickoffTestL()
 */
void RTestVclntAprCancelNtfnWthRegnAftrOpen::DoKickoffTestL()
	{
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.37 - Cancellation of notification without registration after opening video player"));
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	}


/**
 * Fsm
 */
void RTestVclntAprCancelNtfnWthRegnAftrOpen::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((ValidateEvent(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iVideoPlayer
				TRAP(iError, iVideoPlayer->OpenFileL(iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
    				StopTest(iError, EFail);
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				retErr = iVideoPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->CancelRegisterAudioResourceNotification()"));
				if( retErr != KErrCancel )
					{
					ERR_PRINTF2(_L("iVideoPlayer->CancelRegisterAudioResourceNotification() - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					}
				else
    				{
    				ERR_PRINTF2(_L("CancelRegisterAudioResourceNotification() - Failed with Error : %d"), retErr);
        			StopTest(retErr, EPass);   
    				}
				break;
				}
			}
		}
    }

//Cancellation of notification multiple times after opening the file
//MM-MMF-VCLNT-CHRTZ-I-0135

/**
 * Constructor
 */
RTestVclntAprCancelNtfnMultTimesAftrOpen::RTestVclntAprCancelNtfnMultTimesAftrOpen(const TDesC& aTestName, const TDesC& aSectName)
	: RTestBaseVideoPlayAPR(aTestName, aSectName)
	{
	}

/**
 * NewL
 */
RTestVclntAprCancelNtfnMultTimesAftrOpen* RTestVclntAprCancelNtfnMultTimesAftrOpen::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprCancelNtfnMultTimesAftrOpen* self = new (ELeave) RTestVclntAprCancelNtfnMultTimesAftrOpen(aTestName, aSectName);
	return self;
	}

/**
 * DoKickoffTestL()
 */
void RTestVclntAprCancelNtfnMultTimesAftrOpen::DoKickoffTestL()
	{
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.37 - Cancellation of registration multiple times after opening video player"));
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	}


/**
 * Fsm
 */
void RTestVclntAprCancelNtfnMultTimesAftrOpen::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((ValidateEvent(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iVideoPlayer
				TRAP(iError, iVideoPlayer->OpenFileL(iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
    				StopTest(iError, EFail);
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Register iVideoPlayer for Notification
				retErr = iVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					}
				retErr = iVideoPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iVideoPlayer->CancelRegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iVideoPlayer->CancelRegisterAudioResourceNotification() - Failed with Error : %d"), retErr);
    				StopTest(retErr, EFail);
					}
				else
    				{
    				retErr = iVideoPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
    				INFO_PRINTF1(_L("CancelRegisterAudioResourceNotification again"));
    				if( retErr != KErrCancel )
    					{
    					ERR_PRINTF2(_L("iVideoPlayer->CancelRegisterAudioResourceNotification() - Failed with Error : %d"), retErr);
        				StopTest(retErr, EFail);
    					}
    				else
        				{
        				ERR_PRINTF2(_L("iVideoPlayer->CancelRegisterAudioResourceNotification() - Failed with Error : %d"), retErr);
            			StopTest(retErr, EPass);   
        				}   
    				}
				break;
				}
			}
		}
    }
