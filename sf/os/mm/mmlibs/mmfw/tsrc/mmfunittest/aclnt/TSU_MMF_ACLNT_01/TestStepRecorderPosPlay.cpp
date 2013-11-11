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

//#include <MdaAudioSamplePlayer.h>
#include <mdaaudiosampleeditor.h>
//#include <MdaAudioTonePlayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepRecorder.h"


#include "MmfAudioController.h"
#include "mmfclientaudiorecorder.h"
//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepRecorderPosPlay.
 *
 *
 * @return	"CTestStepRecorderPosPlay*"
 *			The constructed CTestStepRecorderPosPlay
 *
 * @xxxx
 * 
 */
CTestStepRecorderPosPlay* CTestStepRecorderPosPlay::NewL(const TDesC& aTestName, TBool aIsConverterTest)
	{
	CTestStepRecorderPosPlay* self = new(ELeave) CTestStepRecorderPosPlay(aTestName, aIsConverterTest);
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
CTestStepRecorderPosPlay::CTestStepRecorderPosPlay(const TDesC& aTestName, TBool aIsConverterTest)
: iIsConverterTest(aIsConverterTest)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderPosPlay::~CTestStepRecorderPosPlay()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderPosPlay::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iIsConverterTest)
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::Position()-PlayL()"));
		}
	else
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::Position()-PlayL()"));
		}

	if(!(GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName3) // au
		)) 
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if (!iIsConverterTest)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		iRecorder->OpenFileL( iFileName );
		CActiveScheduler::Start(); 
		if (iRecorder == NULL ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;
		
		iMiddle = TTimeIntervalMicroSeconds( iRecorder->Duration().Int64() / 2 );
		iEnd = iRecorder->Duration();
		}
	else
		{
		// create the Recorder utility
		if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
			return EInconclusive;

		iConvert->OpenL( iFileName, iFileName3 );
		CActiveScheduler::Start(); 
		if (iConvert == NULL ||
			iError != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen)
			return EInconclusive;
		
		iMiddle = TTimeIntervalMicroSeconds( iConvert->Duration().Int64() / 2 );
		iEnd = iConvert->Duration();
		}
		
		iBeginning = TTimeIntervalMicroSeconds( 0 );
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderPosPlay::DoTestStepPostambleL(void)
	{
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileName3);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;
	if (!iIsConverterTest)
		{
		delete iRecorder;
		iRecorder = NULL;
		}
	else
		{
		delete iConvert;
		iConvert = NULL;
		}
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderPosPlay::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
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
TVerdict CTestStepRecorderPosPlay::DoTestStepL()
	{
	iTestStepResult = EPass;

	if (!iIsConverterTest)
		{
		iRecorder->SetPosition(iMiddle);
		if (iRecorder->Position() != iMiddle )
			return EFail;

		iRecorder->SetPosition(iBeginning);
		if (iRecorder->Position() != iBeginning )
			return EFail;

		iRecorder->SetPosition(iEnd);
		if (iRecorder->Position() != iEnd )
			return EFail;

		iRecorder->SetPosition(TTimeIntervalMicroSeconds(1));
		if (iRecorder->Position() != TTimeIntervalMicroSeconds(1) )
			return EFail;

		iRecorder->SetPosition(TTimeIntervalMicroSeconds(1000));
		if (iRecorder->Position() != TTimeIntervalMicroSeconds(1000) )
			return EFail;

		iRecorder->SetPosition(TTimeIntervalMicroSeconds(1000000));
		if (iRecorder->Position() != TTimeIntervalMicroSeconds(1000000) )
			return EFail;
		}
	else // iIsConverterTest == ETrue
		{
		iConvert->SetPosition(iMiddle);
		if (iConvert->Position() != iMiddle )
			return EFail;

		iConvert->SetPosition(iBeginning);
		if (iConvert->Position() != iBeginning )
			return EFail;

		iConvert->SetPosition(iEnd);
		if (iConvert->Position() != iEnd )
			return EFail;

		iConvert->SetPosition(TTimeIntervalMicroSeconds(1));
		if (iConvert->Position() != TTimeIntervalMicroSeconds(1) )
			return EFail;

		iConvert->SetPosition(TTimeIntervalMicroSeconds(1000));
		if (iConvert->Position() != TTimeIntervalMicroSeconds(1000) )
			return EFail;

		iConvert->SetPosition(TTimeIntervalMicroSeconds(1000000));
		if (iConvert->Position() != TTimeIntervalMicroSeconds(1000000) )
			return EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Static constructor for CTestStepRecorderPlayFF.
 *
 *
 * @return	"CTestStepRecorderPlayFF*"
 *			The constructed CTestStepRecorderPlayFF
 *
 * @xxxx
 * 
 */
CTestStepRecorderPlayFF* CTestStepRecorderPlayFF::NewL(const TDesC& aTestName)
	{
	CTestStepRecorderPlayFF* self = new(ELeave) CTestStepRecorderPlayFF(aTestName);
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
CTestStepRecorderPlayFF::CTestStepRecorderPlayFF(const TDesC& aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderPlayFF::~CTestStepRecorderPlayFF()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderPlayFF::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::PlayL(), Rewind & Fast forward"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName))
		{
		INFO_PRINTF2(_L("file name %S not found..."), &iFileName);
		return EInconclusive;
		}

	// create the Recorder utility
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	iRecorder->OpenFileL( iFileName );
	CActiveScheduler::Start(); 

	if (iRecorder == NULL ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;
	
	// initialize a callback timer
	TRAPD(err, iTimer = CCallbackTimer::NewL(*this));
	if (err != KErrNone)
		return EInconclusive;
	CActiveScheduler::Add(iTimer);

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderPlayFF::DoTestStepPostambleL(void)
	{
	delete iTimer;
	iTimer = NULL;

	delete iRecorder;
	iRecorder = NULL;

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderPlayFF::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	iState = aCurrentState;
	iError = aErrorCode;

	INFO_PRINTF3(_L("MoscoStateChangeEvent(): Prev state = %d, current state = %d"), aPreviousState, aCurrentState);

	iSchedulerStopReason = EStateChange;
	
	CActiveScheduler::Stop();
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
TVerdict CTestStepRecorderPlayFF::DoTestStepL()
	{
	const TInt KTenthOfASecond = 100000;
	const TInt KOneSecond = 1000000;
	const TInt KMaxRWs = 20;
	const TInt KMaxFFs = 30;
	TInt expectedState;
	
	iTestStepResult = EPass;

	expectedState = CMdaAudioRecorderUtility::EOpen;
	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}

	TTimeIntervalMicroSeconds duration = iRecorder->Duration();
	INFO_PRINTF2(_L("Duration = %d"), I64INT(duration.Int64()));

	INFO_PRINTF1(_L("Calling PlayL().."));
	iRecorder->PlayL();
	CActiveScheduler::Start(); 
	expectedState = CMdaAudioRecorderUtility::EPlaying;
	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}

	INFO_PRINTF1(_L("state = EPlaying.."));

	INFO_PRINTF1(_L("Waiting for half way thru play.."));
	// wait until we're half way thru play
	iTimer->After(I64INT(duration.Int64() / 2));
	CActiveScheduler::Start();
	INFO_PRINTF1(_L("Finished."));

	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}


	INFO_PRINTF1(_L("Rewinding like mad....."));
	TInt n;
	for (n=0; n < KMaxRWs && iState == CMdaAudioRecorderUtility::EPlaying && iError == KErrNone; n++)
		{
		iSchedulerStopReason = EUnknown;
		iTimer->After(KTenthOfASecond);
		CActiveScheduler::Start();

		TTimeIntervalMicroSeconds position = iRecorder->Position();
		TTimeIntervalMicroSeconds nextPosition = (position.Int64() - KOneSecond);
		if (nextPosition.Int64() < 0)
			nextPosition = 0;

		iRecorder->SetPosition(nextPosition);
		INFO_PRINTF3(_L("Current position = %d, next = %d"), I64INT(position.Int64()), I64INT(iRecorder->Position().Int64()));

		if (iSchedulerStopReason != ETimer)
			break;
		}
	
	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}

	

	INFO_PRINTF1(_L("Waiting for half way thru play.."));
	// wait until we're half way thru play
	iTimer->After(I64INT(duration.Int64() / 2));
	CActiveScheduler::Start();
	INFO_PRINTF1(_L("Finished."));

	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}


	INFO_PRINTF1(_L("Fast forwarding like mad....."));
	for (n=0; n < KMaxFFs && iState == CMdaAudioRecorderUtility::EPlaying && iError == KErrNone; n++)
		{
		iSchedulerStopReason = EUnknown;
		iTimer->After(KTenthOfASecond);
		CActiveScheduler::Start();

		TTimeIntervalMicroSeconds position = iRecorder->Position();
		TTimeIntervalMicroSeconds nextPosition = (position.Int64() + KOneSecond);
		if (nextPosition.Int64() > duration.Int64())
			{
			nextPosition = duration;
			}

		iRecorder->SetPosition(nextPosition);
		INFO_PRINTF4(_L("Current position = %d, next = %d, new = %d"), 
			I64INT(position.Int64()), I64INT(nextPosition.Int64()), I64INT(iRecorder->Position().Int64()));

		if (iSchedulerStopReason != ETimer)
			break;
		}
	
	INFO_PRINTF1(_L("finished playing.."));
	
	expectedState = CMdaAudioRecorderUtility::EOpen;
	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}


	INFO_PRINTF1(_L("finished with this test step"));

	return iTestStepResult;
	}

CTestStepRecorderPlayFF::CCallbackTimer::CCallbackTimer(CTestStepRecorderPlayFF& aTest) 
	: CTimer(EPriorityNormal), iTest(aTest)
	{
	}

CTestStepRecorderPlayFF::CCallbackTimer::~CCallbackTimer()
	{
	}

CTestStepRecorderPlayFF::CCallbackTimer* CTestStepRecorderPlayFF::CCallbackTimer::NewL(CTestStepRecorderPlayFF& aTest)
	{
	CCallbackTimer* timer = new(ELeave) CCallbackTimer(aTest);
	CleanupStack::PushL(timer);
	timer->ConstructL();
	CleanupStack::Pop(timer);
	return timer;
	}

void CTestStepRecorderPlayFF::CCallbackTimer::RunL()
	{
	iTest.iSchedulerStopReason = ETimer;
	CActiveScheduler::Stop();
	}


/**
 *
 * Static constructor for CTestStepRecorderPlayEOF.
 *
 *
 * @return	"CTestStepRecorderPlayEOF*"
 *			The constructed CTestStepRecorderPlayEOF
 *
 * @xxxx
 * 
 */
CTestStepRecorderPlayEOF* CTestStepRecorderPlayEOF::NewL(const TDesC& aTestName)
	{
	CTestStepRecorderPlayEOF* self = new(ELeave) CTestStepRecorderPlayEOF(aTestName);
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
CTestStepRecorderPlayEOF::CTestStepRecorderPlayEOF(const TDesC& aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderPlayEOF::~CTestStepRecorderPlayEOF()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderPlayEOF::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility..."));
	INFO_PRINTF1(_L("Play a clip from the middle to EOF, then set the position to the clip's"));
	INFO_PRINTF1(_L("duration, call play and then immediately read the position and check it's"));
	INFO_PRINTF1(_L("not longer than the file's duration"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName))
		{
		INFO_PRINTF2(_L("file name %S not found..."), &iFileName);
		return EInconclusive;
		}

	// create the Recorder utility
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	iRecorder->OpenFileL( iFileName );
	CActiveScheduler::Start(); 

	if (iRecorder == NULL ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;
	
	// initialize a callback timer
	TRAPD(err, iTimer = CCallbackTimer::NewL(*this));
	if (err != KErrNone)
		return EInconclusive;
	CActiveScheduler::Add(iTimer);

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderPlayEOF::DoTestStepPostambleL(void)
	{
	delete iTimer;
	iTimer = NULL;

	delete iRecorder;
	iRecorder = NULL;

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderPlayEOF::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	iState = aCurrentState;
	iError = aErrorCode;

	INFO_PRINTF3(_L("MoscoStateChangeEvent(): Prev state = %d, current state = %d"), aPreviousState, aCurrentState);

	iSchedulerStopReason = EStateChange;
	
	CActiveScheduler::Stop();
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
TVerdict CTestStepRecorderPlayEOF::DoTestStepL()
	{
	TInt expectedState;
	
	iTestStepResult = EPass;

	expectedState = CMdaAudioRecorderUtility::EOpen;
	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}

	TTimeIntervalMicroSeconds duration = iRecorder->Duration();
	INFO_PRINTF2(_L("Duration = %d"), I64INT(duration.Int64()));

	
	TTimeIntervalMicroSeconds midPosition = TTimeIntervalMicroSeconds(duration.Int64() / 2);
	INFO_PRINTF2(_L("Setting position to %d"), I64INT(midPosition.Int64()));
	iRecorder->SetPosition(midPosition);

	INFO_PRINTF1(_L("Calling PlayL().."));
	iRecorder->PlayL();
	CActiveScheduler::Start(); 
	expectedState = CMdaAudioRecorderUtility::EPlaying;
	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}

	INFO_PRINTF1(_L("Waiting for EOF.."));
	CActiveScheduler::Start(); 
	INFO_PRINTF1(_L("finished playing.."));
	expectedState = CMdaAudioRecorderUtility::EOpen;
	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}

	INFO_PRINTF2(_L("Setting position = Duration = %d"), I64INT(duration.Int64()));
	iRecorder->SetPosition(duration);

	INFO_PRINTF1(_L("Calling PlayL() again.."));
	iRecorder->PlayL();
	
	
	TTimeIntervalMicroSeconds position = iRecorder->Position();
	INFO_PRINTF2(_L("Current position = %d"), I64INT(position.Int64()));
	if (position.Int64() > duration.Int64())
		{
		INFO_PRINTF1(_L("Position is longer than the duration"));
		return EFail;
		}


	CActiveScheduler::Start(); 

	expectedState = CMdaAudioRecorderUtility::EPlaying;
	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}
	INFO_PRINTF1(_L("state = EPlaying.."));

	INFO_PRINTF1(_L("Waiting for EOF.."));
	CActiveScheduler::Start();
	INFO_PRINTF1(_L("Finished."));
	expectedState = CMdaAudioRecorderUtility::EOpen;
	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}
	
	expectedState = CMdaAudioRecorderUtility::EOpen;
	if (iState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}


//##############################################################

	iRecorder->Stop();

	TTimeIntervalMicroSeconds sillyPosition = 
		TTimeIntervalMicroSeconds(duration.Int64() + 1000000);
	INFO_PRINTF2(_L("Setting position past EOF = %d"), I64INT(sillyPosition.Int64()));
	iRecorder->SetPosition(sillyPosition);

	INFO_PRINTF1(_L("Calling PlayL() again.."));
	iRecorder->PlayL();
	
	
	position = iRecorder->Position();
	INFO_PRINTF2(_L("Current position = %d"), I64INT(position.Int64()));
	if (position.Int64() > duration.Int64())
		{
		INFO_PRINTF1(_L("Position is longer than the duration (before play has started - OK)"));
//		return EFail;
		}


	CActiveScheduler::Start(); 

	expectedState = CMdaAudioRecorderUtility::EOpen;
	if (iState != expectedState && iError != KErrArgument)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iState, iError);
		return EFail;
		}
//##############################################################
	INFO_PRINTF1(_L("finished with this test step"));

	return iTestStepResult;
	}

CTestStepRecorderPlayEOF::CCallbackTimer::CCallbackTimer(CTestStepRecorderPlayEOF& aTest) 
	: CTimer(EPriorityNormal), iTest(aTest)
	{
	}

CTestStepRecorderPlayEOF::CCallbackTimer::~CCallbackTimer()
	{
	}

CTestStepRecorderPlayEOF::CCallbackTimer* CTestStepRecorderPlayEOF::CCallbackTimer::NewL(CTestStepRecorderPlayEOF& aTest)
	{
	CCallbackTimer* timer = new(ELeave) CCallbackTimer(aTest);
	CleanupStack::PushL(timer);
	timer->ConstructL();
	CleanupStack::Pop(timer);
	return timer;
	}

void CTestStepRecorderPlayEOF::CCallbackTimer::RunL()
	{
	iTest.iSchedulerStopReason = ETimer;
	CActiveScheduler::Stop();
	}

/**
 *
 * Static constructor for CTestStepRecorderPlayStopPosStop.
 *
 *
 * @return	"CTestStepRecorderPlayStopPosStop*"
 *			The constructed CTestStepRecorderPlayStopPosStop
 *
 * @xxxx
 * 
 */
CTestStepRecorderPlayStopPosStop* CTestStepRecorderPlayStopPosStop::NewL()
	{
	CTestStepRecorderPlayStopPosStop* self = new(ELeave) CTestStepRecorderPlayStopPosStop;
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

CTestStepRecorderPlayStopPosStop::CTestStepRecorderPlayStopPosStop()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0857-HP");

	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */

CTestStepRecorderPlayStopPosStop::~CTestStepRecorderPlayStopPosStop()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */

TVerdict CTestStepRecorderPlayStopPosStop::DoTestStepPreambleL(void)
	{
	TVerdict verdict=EPass;
	 // this installs the scheduler
	if((verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL())	!= EPass)
		{
		return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this is a test of calling Play, Stop, SetPosition"));
	INFO_PRINTF1(_L("and then call Stop again & verifying the position is set to the beginning"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName10"), iFileName)) 
		{
		return EInconclusive;
		}
	// create the Recorder utility
	TRAPD(error, iRecorderUtility = CMdaAudioRecorderUtility::NewL(*this));
	if (error != KErrNone)
		{
		return EInconclusive;
		}
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderPlayStopPosStop::DoTestStepPostambleL(void)	
	{
	delete iRecorderUtility;
	iRecorderUtility = NULL;

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderPlayStopPosStop::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	iCurrentState = aCurrentState;
	iError = aErrorCode;

	INFO_PRINTF3(_L("MoscoStateChangeEvent(): Prev state = %d, current state = %d"), aPreviousState, aCurrentState);

	CActiveScheduler::Stop();
	}



TVerdict CTestStepRecorderPlayStopPosStop::DoTestStepL()
	{
	iTestStepResult = EPass;

	INFO_PRINTF1(_L("Opening file using player utility..."));
	
	// open the sound file
	TRAP(iError, iRecorderUtility->OpenFileL(iFileName));
	if (iError != KErrNone)
		{
		return EInconclusive;
		}
	// wait for open to complete
	CActiveScheduler::Start(); 
	
	TInt expectedState = CMdaAudioRecorderUtility::EOpen;
	if (iCurrentState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iCurrentState, iError);
		return EFail;
		}
	

	TTimeIntervalMicroSeconds playStart = TTimeIntervalMicroSeconds(0);
	TTimeIntervalMicroSeconds playEnd = iRecorderUtility->Duration();
	
	iRecorderUtility->SetPlayWindow(playStart, playEnd);
	
	INFO_PRINTF1(_L("calling play..."));
	
	iRecorderUtility->PlayL();
			
	CActiveScheduler::Start(); 
	
	expectedState = CMdaAudioRecorderUtility::EPlaying;
	if (iCurrentState != expectedState && iError != KErrNone)
		{
		INFO_PRINTF4(_L("Unexpected state; expected = %d, actual = %d, error = %d"), expectedState, iCurrentState, iError);
		return EFail;
		}
	
	User::After(500000);

	iRecorderUtility->Stop();
	
	TTimeIntervalMicroSeconds step = TTimeIntervalMicroSeconds(200000);
	
	iRecorderUtility->SetPosition(step);

	iRecorderUtility->Stop();

	TTimeIntervalMicroSeconds position = iRecorderUtility->Position();
	
	if (position.Int64() != playStart.Int64())
		{
		return EFail;
		}
	

	iRecorderUtility->Close();
	
	return iTestStepResult;
	
}

