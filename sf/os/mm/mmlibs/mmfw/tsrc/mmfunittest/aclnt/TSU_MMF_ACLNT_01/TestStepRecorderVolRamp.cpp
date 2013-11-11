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
 * Static constructor for CTestStepRecorderVolRamp.
 *
 *
 * @return	"CTestStepRecorderVolRamp*"
 *			The constructed CTestStepRecorderVolRamp
 *
 * @xxxx
 * 
 */
CTestStepRecorderVolRamp* CTestStepRecorderVolRamp::NewL()
	{
	CTestStepRecorderVolRamp* self = new(ELeave) CTestStepRecorderVolRamp;
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
CTestStepRecorderVolRamp::CTestStepRecorderVolRamp() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0242-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderVolRamp::~CTestStepRecorderVolRamp()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepRecorderVolRamp::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0242-HP"));
	 INFO_PRINTF1(_L("this test is playing a sound from a Recorder with several Volume Ramp levels. SetVolumeRamp(...)"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	User::After(100000);
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
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
enum TVerdict CTestStepRecorderVolRamp::DoTestStepPostambleL(void)
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
void CTestStepRecorderVolRamp::MoscoStateChangeEvent(CBase* /*aObject*/,
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
TVerdict CTestStepRecorderVolRamp::DoTestStepL()
	{
	//
	// GETS STACK - callbacks NEED FIXING 
	//
	TInt err;
	// try zero ramp
	iRecorder->SetVolumeRamp(TTimeIntervalMicroSeconds(0));
		INFO_PRINTF1(_L("volume ramp: 0"));
	TRAP(err, iRecorder->PlayL() );
	if (err != KErrNone)
		return EFail;
	CActiveScheduler::Start(); // for start playing
	if (iError != KErrNone || !iHasStartedPlaying)
		return EFail;
	CActiveScheduler::Start(); // for finishing playing
	if( iError != KErrNone || !iHasFinishedPlaying)
		return EFail;
	iRecorder->SetPosition(TTimeIntervalMicroSeconds(0));
	if (iRecorder->Position() != TTimeIntervalMicroSeconds(0))
		return EInconclusive;

	// try several volume values and play
	for (TInt ramp=1 ; ramp<=1000000 ; ramp*=10 )
		{
		iRecorder->SetVolumeRamp(TTimeIntervalMicroSeconds(ramp));
		iRecorder->PlayL();

		CActiveScheduler::Start();
		if (iError != KErrNone || !iHasStartedPlaying)
			return EFail;
		CActiveScheduler::Start();
		if( iError != KErrNone || !iHasFinishedPlaying)
		return EFail;
		INFO_PRINTF2(_L("loop, volume ramp: %d"), ramp);
		iRecorder->SetPosition(TTimeIntervalMicroSeconds(0));
		if (iRecorder->Position() != TTimeIntervalMicroSeconds(0))
			return EInconclusive;
		} // end for ramp

	// try ramp > sound Duration
	//iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	//iRecorder->OpenFileL(iFileName);
	//if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
	//	return EInconclusive;
	iRecorder->SetVolumeRamp(TTimeIntervalMicroSeconds(2000000));
	iRecorder->PlayL();
	CActiveScheduler::Start();
	if (iError != KErrNone || !iHasStartedPlaying)
		return EFail;
	CActiveScheduler::Start();
	if( iError != KErrNone || !iHasFinishedPlaying)
		return EFail;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

