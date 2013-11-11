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
 * Static constructor for CTestStepRecorderClearWindowPlay.
 *
 *
 * @return	"CTestStepRecorderClearWindowPlay*"
 *			The constructed CTestStepRecorderClearWindowPlay
 *
 * @xxxx
 * 
 */
CTestStepRecorderClearWindowPlay* CTestStepRecorderClearWindowPlay::NewL()
	{
	CTestStepRecorderClearWindowPlay* self = new(ELeave) CTestStepRecorderClearWindowPlay;
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
CTestStepRecorderClearWindowPlay::CTestStepRecorderClearWindowPlay()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0247-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderClearWindowPlay::~CTestStepRecorderClearWindowPlay()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderClearWindowPlay::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0247-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::ClearPlayWindow()-PlayL()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName)) // wav file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
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
	
	iBeginning = TTimeIntervalMicroSeconds( 0 );
	iMiddle = TTimeIntervalMicroSeconds( iRecorder->Duration().Int64() / 2 );
	iEnd = iRecorder->Duration();

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderClearWindowPlay::DoTestStepPostambleL(void)
	{
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
void CTestStepRecorderClearWindowPlay::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;

	iHasStartedPlaying = EFalse;
	iHasFinishedPlaying = EFalse;

	if (aPreviousState == CMdaAudioRecorderUtility::EOpen &&
		aCurrentState == CMdaAudioRecorderUtility::EPlaying)	
		iHasStartedPlaying = ETrue;

	if (aPreviousState == CMdaAudioRecorderUtility::EPlaying &&
		aCurrentState == CMdaAudioRecorderUtility::EOpen)	
		iHasFinishedPlaying = ETrue;	

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
TVerdict CTestStepRecorderClearWindowPlay::DoTestStepL()
	{
	iTestStepResult = EPass;

	// no easy or obvious way to check if command successful
	//if it was a pure unit test it would only matter if the parameters are passed
	// correctly, and the return error code is propagated.
	//duration seems to be the original duration of the clip always.
 	TTime start;
 	TTime stop;
 	TInt64 totalDuration = iRecorder->Duration().Int64();
 	TInt64 windowDuration = iMiddle.Int64() - iBeginning.Int64();
 	TInt64 actualDuration;
 	TInt64 playDurationDiff;
 	const TInt KQuarterSecond = 250000;
 	const TInt KHalfSecond = 500000;
 
 

	iRecorder->SetPlayWindow(iBeginning, iMiddle); 
 	start.HomeTime();
	iRecorder->PlayL();
	CActiveScheduler::Start();
	if (iError != KErrNone || !iHasStartedPlaying)
		return EInconclusive;
	CActiveScheduler::Start();
	if (iError != KErrNone || !iHasFinishedPlaying)
		return EInconclusive;

 	stop.HomeTime();
 	actualDuration = stop.MicroSecondsFrom(start).Int64();
 	INFO_PRINTF3(_L("actualDuration = %lu expectedDuration = %lu"), actualDuration, windowDuration);
 	playDurationDiff = actualDuration - windowDuration;
 	if (playDurationDiff < 0)
 		playDurationDiff = -playDurationDiff;
	//the play window is to a resolution of 1 buffer which is 8k
	//8K @ 8khz sample pcm16 is 0.5seconds + a margin of error for 
	//test completion means more then 0.5S is required.
 	if (playDurationDiff > (KHalfSecond+KQuarterSecond))
 		{
 		INFO_PRINTF1(_L("Difference is too high"));
 		return EFail;
 		}
 
	iRecorder->ClearPlayWindow();
 	start.HomeTime();
	iRecorder->PlayL();
	CActiveScheduler::Start();
	if (iError != KErrNone || !iHasStartedPlaying)
		return EInconclusive;
	CActiveScheduler::Start();
	if (iError != KErrNone || !iHasFinishedPlaying)
		return EInconclusive;


 	stop.HomeTime();
 	actualDuration = stop.MicroSecondsFrom(start).Int64();
 	INFO_PRINTF3(_L("actualDuration = %lu expectedDuration = %lu"), actualDuration, totalDuration);
 	playDurationDiff = actualDuration - totalDuration;
 	if (playDurationDiff < 0)
 		playDurationDiff = -playDurationDiff;

 	if (playDurationDiff > KHalfSecond)
 		{
 		INFO_PRINTF1(_L("Difference is too high"));
 		return EFail;
 		}
 
	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

