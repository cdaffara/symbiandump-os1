// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
// 
//

// EPOC includes
#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>

#include <mdaaudiosampleplayer.h>
//#include <MdaAudioSampleEditor.h>
//#include <MdaAudioTonePlayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepPlayerManip.h"

#include "mmfclientaudioplayer.h"



// --------------------------------------------

/**
 *
 * Static constructor for CTestStepPlayerRepeats.
 *
 *
 * @return	"CTestStepPlayerRepeats*"
 *			The constructed CTestStepPlayerRepeats
 *
 * @xxxx
 * 
 */
CTestStepPlayerRepeats* CTestStepPlayerRepeats::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestStepPlayerRepeats* self = new(ELeave) CTestStepPlayerRepeats(aTestName, aSectName, aKeyName);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepPlayerRepeats::CTestStepPlayerRepeats(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerRepeats::~CTestStepPlayerRepeats()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerRepeats::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerRepeats::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepPlayerRepeats::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	  INFO_PRINTF1(_L("this test is playing a sound with several repeat and trailing silence params. SetRepeats(...)"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(iSectName, iKeyName, iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	 iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	 CActiveScheduler::Start();
	 User::After(1000000);

	 if (iPlayer == NULL ||
		 iError != KErrNone )
		 return EInconclusive;

	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerRepeats::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerRepeats::DoTestStepL()
	{
	//__MM_HEAP_MARK;
	// check if player opened the file successfully
	TInt err1 = KErrNone;
	TInt err2 = KErrNone;
	TInt state = 0;
	TMMFMessageDestination* srcHandle = NULL;
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		return EInconclusive;

	
	// try 0, 0 args
	iPlayer->SetRepeats(0, TTimeIntervalMicroSeconds(0) );

	// use the following line to check that test silence > KMaxTInt is supported
	// removed because it will take > 35 minutes
	
	// iPlayer->SetRepeats(1, TTimeIntervalMicroSeconds(TInt64(KMaxTInt) + 10000000));

	iPlayer->Play();
	CActiveScheduler::Start();
	delete iPlayer;
	iPlayer = NULL;

	// try several combination of repeats and trailing silence
	for (TInt rep=0 ; rep<=3 ; rep++) // repeats loop
		{
		iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
		CActiveScheduler::Start();
		iPlayer->SetRepeats(rep, TTimeIntervalMicroSeconds(0) );
		iPlayer->Play();
		CActiveScheduler::Start();
		User::After((rep+1)*1200000); // time enough to play it rep times
		INFO_PRINTF1(_L("external loop"));
		err1 = GetAudPlayerState(iPlayer, state);
		if ( err1 != KErrNone ||
			 state != CMMFMdaAudioPlayerUtility::EStopped ||
			 iError != KErrNone)
			{
				iTestStepResult = EFail;
				return iTestStepResult;
			}
		delete iPlayer;
		iPlayer = NULL;
		
		for ( TInt64 trail=1; trail<=100000; trail*=10) // trailing silence loop
			{
			iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
			CActiveScheduler::Start();
			iPlayer->SetRepeats(rep, TTimeIntervalMicroSeconds(trail) );
			iPlayer->Play();
			CActiveScheduler::Start();
			INFO_PRINTF1(_L("internal loop"));
			err1 = GetAudPlayerState(iPlayer, state);
			if ( err1 != KErrNone ||
				 state != CMMFMdaAudioPlayerUtility::EStopped ||
				 iError != KErrNone)
				{
				iTestStepResult = EFail;
				return iTestStepResult;
				}
			delete iPlayer;
			iPlayer = NULL;
			//User::After(100000); // to release the file
			} // end for trailing silence
			
		} // end for repeats

	//__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}




// Callback timer used because we want client utility to 
// receive callbacks from controller while timer is running
CTestStepPlayerRepeatsState::CCallbackTimer::CCallbackTimer(CTestStepPlayerRepeatsState& aTest) 
	: CTimer(EPriorityNormal), iTest(aTest)
	{
	}

CTestStepPlayerRepeatsState::CCallbackTimer::~CCallbackTimer()
	{
	}

CTestStepPlayerRepeatsState::CCallbackTimer* CTestStepPlayerRepeatsState::CCallbackTimer::NewL(CTestStepPlayerRepeatsState& aTest)
	{
	CCallbackTimer* timer = new(ELeave) CCallbackTimer(aTest);
	CleanupStack::PushL(timer);
	timer->ConstructL();
	CleanupStack::Pop(timer);
	return timer;
	}

void CTestStepPlayerRepeatsState::CCallbackTimer::RunL()
	{
	iTest.iSchedulerStopReason = ETimer;
	CActiveScheduler::Stop();
	}

/**
 *
 * Static constructor for CTestStepPlayerRepeatsState.
 *
 *
 * @return	"CTestStepPlayerRepeatsState*"
 *			The constructed CTestStepPlayerRepeatsState
 *
 * @xxxx
 * 
 */
CTestStepPlayerRepeatsState* CTestStepPlayerRepeatsState::NewL()
	{
	CTestStepPlayerRepeatsState* self = new(ELeave) CTestStepPlayerRepeatsState;
	return self;
	}

/*
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepPlayerRepeatsState::CTestStepPlayerRepeatsState()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0821-LP");
	}

/*
 * Test step destructor.
 */
CTestStepPlayerRepeatsState::~CTestStepPlayerRepeatsState()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerRepeatsState::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;

	iSchedulerStopReason = EPlayerInitialized;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerRepeatsState::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	iSchedulerStopReason = EPlayComplete;
	CActiveScheduler::Stop();
	}

/*
 * Test step Preamble.
 */
 enum TVerdict CTestStepPlayerRepeatsState::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("this test calls SetRepeats() & tests the state during trailing silence period"));

	iError = KErrNone;

	// one second file for a quicker test
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		return EInconclusive;

	// nine second file for a slower but perhaps more reliable test :
	// but shouldn't need file to be so long for this test (CTestStepPlayerRepeats 
	// uses AudioPlayFName1 too and demands a similar level of accuracy).
	//	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName))
	//		return EInconclusive;

	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	CActiveScheduler::Start();

	if (iPlayer == NULL || iError != KErrNone )
		return EInconclusive;

	// initialize a callback timer
	TRAPD(err, iTimer = CCallbackTimer::NewL(*this));
	if (err != KErrNone)
		return EInconclusive;
	CActiveScheduler::Add(iTimer);

	return verdict;
	}

/**
 * Test step Postamble.
 */
enum TVerdict CTestStepPlayerRepeatsState::DoTestStepPostambleL(void)
	{
	delete iTimer;
	iTimer = NULL;

	delete iPlayer;
	iPlayer = NULL;

	// Destroy the scheduler 
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerRepeatsState::DoTestStepL()
	{
	// check if player openned the file successfully
	TInt err1 = KErrNone;
	TInt err2 = KErrNone;
	TInt state = 0;
	TMMFMessageDestination* srcHandle = NULL;
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		return EInconclusive;

	TInt err;

	// we're going to play the clip & then have a period of trailing silence
	// equal to the duration of the clip,
	// so calculate when to expire a timer so that it expires 
	// half way through the trailing silence period
	const TTimeIntervalMicroSeconds duration = iPlayer->Duration();
	const TInt timeOfSilence = I64INT(duration.Int64());
	const TInt timeToMidSilence = I64INT(duration.Int64()) * 3 / 2;


	iPlayer->SetRepeats(1, TTimeIntervalMicroSeconds(timeOfSilence));
	INFO_PRINTF1(_L("Calling Play() & waiting for trailing silence..."));
	iPlayer->Play();

	// wait until we're in the trailing silent period
	iTimer->After(timeToMidSilence);
	iSchedulerStopReason = EUnknown;
	CActiveScheduler::Start();
	if (iSchedulerStopReason != ETimer)
		{
		INFO_PRINTF1(_L("Unexpected callback..."));
		return EInconclusive;
		}

	INFO_PRINTF1(_L("Checking state during trailing silence period..."));
	err = GetAudPlayerState(iPlayer, state);
	if (err != KErrNone || 
		state != CMMFMdaAudioPlayerUtility::EPlaying ||
		iError != KErrNone)
		{
		INFO_PRINTF1(_L("Player is in wrong state during silence period :"));
		INFO_PRINTF4(_L("err = %d, state = %d, iError = %d :"), err, state, iError);
		return EFail;
		}

	// call Stop() & check repeat timer has been cancelled
	// by checking player state==EStopped a bit after silence period
	INFO_PRINTF1(_L("Calling Stop() during trailing silence period & verifying trailing silence timer not running..."));
	iPlayer->Stop();
	iTimer->After(timeOfSilence);	// half way into next play period
	INFO_PRINTF1(_L("Waiting to make sure clip does not replay..."));
	iSchedulerStopReason = EUnknown;
	CActiveScheduler::Start();
	if (iSchedulerStopReason != ETimer)
		{
		INFO_PRINTF1(_L("Unexpected callback..."));
		return EInconclusive;
		}
	err = GetAudPlayerState(iPlayer, state);
	if (err != KErrNone || 
		state != CMMFMdaAudioPlayerUtility::EStopped ||
		iError != KErrNone)
		{
		INFO_PRINTF1(_L("Player is in wrong state after Stop() :"));
		INFO_PRINTF4(_L("err = %d, state = %d, iError = %d :"), err, state, iError);
		return EFail;
		}

	// call Play() again
	iPlayer->SetRepeats(1, TTimeIntervalMicroSeconds(timeOfSilence));
	INFO_PRINTF1(_L("Calling Play() again & waiting for trailing silence......"));
	iPlayer->Play();
	// wait until we're in the trailing silent period
	iTimer->After(timeToMidSilence);
	iSchedulerStopReason = EUnknown;
	CActiveScheduler::Start();
	if (iSchedulerStopReason != ETimer)
		{
		INFO_PRINTF1(_L("Unexpected callback..."));
		return EInconclusive;
		}

	INFO_PRINTF1(_L("Checking state during trailing silence period..."));
	err = GetAudPlayerState(iPlayer, state);
	if (err != KErrNone || 
		state != CMMFMdaAudioPlayerUtility::EPlaying ||
		iError != KErrNone)
		{
		INFO_PRINTF1(_L("Player is in wrong state during silence period :"));
		INFO_PRINTF4(_L("err = %d, state = %d, iError = %d :"), err, state, iError);
		return EFail;
		}

	// For good measure, try to call Play() again during the silence period -
	// this should result in a MapcPlayComplete callback with KErrNotReady
	INFO_PRINTF1(_L("Calling Play() during trailing silence period & verifying KErrnotReady callback..."));
	iPlayer->Play();

	iSchedulerStopReason = EUnknown;
	CActiveScheduler::Start();
	if (iSchedulerStopReason != EPlayComplete)
		{
		INFO_PRINTF1(_L("Unexpected callback..."));
		return EInconclusive;
		}

	err = GetAudPlayerState(iPlayer, state);
	if (err != KErrNone ||
		state != CMMFMdaAudioPlayerUtility::EPlaying ||
		iError != KErrNotReady)
		{
		INFO_PRINTF1(_L("Player is in wrong state after second call to Play() :"));
		INFO_PRINTF4(_L("err = %d, state = %d, iError = %d :"), err, state, iError);
		return EFail;
		}
	INFO_PRINTF1(_L("KErrnotReady callback received."));

	INFO_PRINTF1(_L("Waiting for first Play() to repeat & complete normally..."));
	iSchedulerStopReason = EUnknown;
	CActiveScheduler::Start();
	if (iSchedulerStopReason != EPlayComplete)
		{
		INFO_PRINTF1(_L("Unexpected callback..."));
		return EInconclusive;
		}

	err = GetAudPlayerState(iPlayer, state);
	if (err != KErrNone ||
		state != CMMFMdaAudioPlayerUtility::EStopped ||
		iError != KErrNone)
		{
		INFO_PRINTF1(_L("Player is in wrong state after second call to Play() :"));
		INFO_PRINTF4(_L("err = %d, state = %d, iError = %d :"), err, state, iError);
		return EFail;
		}
	INFO_PRINTF1(_L("Play() completed normally."));

	delete iPlayer;
	iPlayer = NULL;

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Failed : Error = %d"), iError);
		return EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
	
	
	

// DEF 90666 -T:Aclnt - Pausing during repeat causes the file to close itself
// Callback timer used because we want client utility to 
// receive callbacks from controller while timer is running

CTestStepPlayerRepeatsPause::CCallbackTimer::CCallbackTimer(CTestStepPlayerRepeatsPause& aTest) 
	: CTimer(EPriorityNormal), iTest(aTest)
	{
	}

CTestStepPlayerRepeatsPause::CCallbackTimer::~CCallbackTimer()
	{
	}

CTestStepPlayerRepeatsPause::CCallbackTimer* CTestStepPlayerRepeatsPause::CCallbackTimer::NewL(CTestStepPlayerRepeatsPause& aTest)
	{
	CCallbackTimer* timer = new(ELeave) CCallbackTimer(aTest);
	CleanupStack::PushL(timer);
	timer->ConstructL();
	CleanupStack::Pop(timer);
	return timer;
	}

void CTestStepPlayerRepeatsPause::CCallbackTimer::RunL()
	{
	iTest.iSchedulerStopReason = ETimer;
	CActiveScheduler::Stop();
	}

/**
 *
 * Static constructor for CTestStepPlayerRepeatsState.
 *
 *
 * @return	"CTestStepPlayerRepeatsPause*"
 *			The constructed CTestStepPlayerRepeatsPause
 *
 * @xxxx
 * 
 */
CTestStepPlayerRepeatsPause* CTestStepPlayerRepeatsPause::NewL()
	{
	CTestStepPlayerRepeatsPause* self = new(ELeave) CTestStepPlayerRepeatsPause;
	return self;
	}

/*
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepPlayerRepeatsPause::CTestStepPlayerRepeatsPause()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0968-HP");
	}

/*
 * Test step destructor.
 */
CTestStepPlayerRepeatsPause::~CTestStepPlayerRepeatsPause()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerRepeatsPause::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	iSchedulerStopReason = EPlayerInitialized;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerRepeatsPause::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	iSchedulerStopReason = EPlayComplete;
	CActiveScheduler::Stop();
	}

/*
 * Test step Preamble.
 */
 enum TVerdict CTestStepPlayerRepeatsPause::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("this test calls SetRepeats() & calls Pause() and test the Paused state during trailing silence period"));

	iError = KErrNone;

	// one second file for a quicker test
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		return EInconclusive;
		}
	
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	CActiveScheduler::Start();

	if (iPlayer == NULL || iError != KErrNone )
		{
		return EInconclusive;
		}

	// initialize a callback timer
	TRAPD(err, iTimer = CCallbackTimer::NewL(*this));
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	CActiveScheduler::Add(iTimer);

	return verdict;
	}

/**
 * Test step Postamble.
 */
enum TVerdict CTestStepPlayerRepeatsPause::DoTestStepPostambleL(void)
	{
	delete iTimer;
	iTimer = NULL;

	delete iPlayer;
	iPlayer = NULL;

	// Destroy the scheduler 
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerRepeatsPause::DoTestStepL()
	{
	// check if player openned the file successfully
	TInt err1 = KErrNone;
	TInt err2 = KErrNone;
	TInt state = 0;
	TMMFMessageDestination* srcHandle = NULL;
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
			{
			return EInconclusive;
			}

	TInt err;

	// we're going to play the clip & then have a period of trailing silence
	// equal to the duration of the clip,
	// so calculate when to expire a timer so that it expires 
	// half way through the trailing silence period
	const TTimeIntervalMicroSeconds duration = iPlayer->Duration();
	const TInt timeOfSilence = I64INT(duration.Int64());
	const TInt timeToMidSilence = I64INT(duration.Int64()) * 3 / 2;
	iPlayer->SetRepeats(2, TTimeIntervalMicroSeconds(timeOfSilence));
	INFO_PRINTF1(_L("Calling Play() & waiting for trailing silence..."));
	iPlayer->Play();

	// wait until we're in the trailing silent period
	iTimer->After(timeToMidSilence);
	iSchedulerStopReason = EUnknown;
	CActiveScheduler::Start();
	if (iSchedulerStopReason != ETimer)
		{
		INFO_PRINTF1(_L("Unexpected callback..."));
		return EInconclusive;
		}

	INFO_PRINTF1(_L("Checking state during trailing silence period..."));
	err = GetAudPlayerState(iPlayer, state);
	if (err != KErrNone || 
		state != CMMFMdaAudioPlayerUtility::EPlaying ||
		iError != KErrNone)
		{
		INFO_PRINTF1(_L("Player is in wrong state during silence period :"));
		INFO_PRINTF4(_L("err = %d, state = %d, iError = %d :"), err, state, iError);
		return EFail;
		}
		
	// call Pause() & check repeat timer has been cancelled
	// by checking player state==EPaused a bit after silence period	
	INFO_PRINTF1(_L("Calling Pause() during trailing silence period & verifying state is in Paused state."));
	iPlayer->Pause();
	iTimer->After(timeOfSilence);	// half way into next play period
	INFO_PRINTF1(_L("Waiting to make sure clip does not replay..."));
	iSchedulerStopReason = EUnknown;
	CActiveScheduler::Start();
	if (iSchedulerStopReason != ETimer)
		{
		INFO_PRINTF1(_L("Unexpected callback..."));
		return EInconclusive;
		}
	err = GetAudPlayerState(iPlayer, state);
	if (err != KErrNone || 
		state != CMMFMdaAudioPlayerUtility::EPaused ||
		iError != KErrNone)
		{
		INFO_PRINTF1(_L("Player is in wrong state after Pause() :"));
		INFO_PRINTF4(_L("err = %d, state = %d, iError = %d :"), err, state, iError);
		return EFail;
		}

	// call Play() again
	iPlayer->SetRepeats(1, TTimeIntervalMicroSeconds(timeOfSilence));
	INFO_PRINTF1(_L("Calling Play() again & waiting for trailing silence......"));
	iPlayer->Play();
	// wait until we're in the trailing silent period
	iTimer->After(timeToMidSilence);
	iSchedulerStopReason = EUnknown;
	CActiveScheduler::Start();
	if(iSchedulerStopReason == ETimer)
		{
		INFO_PRINTF1(_L("Checking state during trailing silence period..."));
		err = GetAudPlayerState(iPlayer, state);
		if (err != KErrNone || 
			state != CMMFMdaAudioPlayerUtility::EPlaying ||
			iError != KErrNone)
			{
			INFO_PRINTF1(_L("Player is in wrong state during silence period :"));
			INFO_PRINTF4(_L("err = %d, state = %d, iError = %d :"), err, state, iError);
			return EFail;
			}

		// For good measure, try to call Play() again during the silence period -
		// this should result in a MapcPlayComplete callback with KErrNotReady
		INFO_PRINTF1(_L("Calling Play() during trailing silence period & verifying KErrnotReady callback..."));
		iPlayer->Play();

		iSchedulerStopReason = EUnknown;
		CActiveScheduler::Start();
		if (iSchedulerStopReason != EPlayComplete)
			{
			INFO_PRINTF1(_L("Unexpected callback..."));
			return EInconclusive;
			}

		err = GetAudPlayerState(iPlayer, state);
		if (err != KErrNone ||
			state != CMMFMdaAudioPlayerUtility::EPlaying ||
			iError != KErrNotReady)
			{
			INFO_PRINTF1(_L("Player is in wrong state after second call to Play() :"));
			INFO_PRINTF4(_L("err = %d, state = %d, iError = %d :"), err, state, iError);
			return EFail;
			}
		INFO_PRINTF1(_L("KErrnotReady callback received."));

		INFO_PRINTF1(_L("Waiting for first Play() to repeat & complete normally..."));
		iSchedulerStopReason = EUnknown;
		CActiveScheduler::Start();
		if (iSchedulerStopReason != EPlayComplete)
			{
			INFO_PRINTF1(_L("Unexpected callback..."));
			return EInconclusive;
			}

		err = GetAudPlayerState(iPlayer, state);
		if (err != KErrNone ||
			state != CMMFMdaAudioPlayerUtility::EStopped ||
			iError != KErrNone)
			{
			INFO_PRINTF1(_L("Player is in wrong state after second call to Play() :"));
			INFO_PRINTF4(_L("err = %d, state = %d, iError = %d :"), err, state, iError);
			return EFail;
			}
		INFO_PRINTF1(_L("Play() completed normally."));
		}
	else if(iSchedulerStopReason == EPlayComplete)
		{
		INFO_PRINTF1(_L("Play() completed normally...Didnt Play silence as it reached last repeat count"));
		}
	else
		{
		INFO_PRINTF1(_L("Unexpected callback..."));
		return EInconclusive;
		}
	delete iPlayer;
	iPlayer = NULL;

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Failed : Error = %d"), iError);
		return EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



