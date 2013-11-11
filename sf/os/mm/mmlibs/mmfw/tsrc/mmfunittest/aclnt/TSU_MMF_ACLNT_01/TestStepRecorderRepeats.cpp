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
 * Static constructor for CTestStepRecorderRepeats.
 *
 *
 * @return	"CTestStepRecorderRepeats*"
 *			The constructed CTestStepRecorderRepeats
 *
 * @xxxx
 * 
 */
CTestStepRecorderRepeats* CTestStepRecorderRepeats::NewL()
	{
	CTestStepRecorderRepeats* self = new(ELeave) CTestStepRecorderRepeats;
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
CTestStepRecorderRepeats::CTestStepRecorderRepeats() : iHasPlayed(ETrue)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0241-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderRepeats::~CTestStepRecorderRepeats()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepRecorderRepeats::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0241-CP"));
	 INFO_PRINTF1(_L("this test is playing a sound from a recorder utility with several repeat and trailing silence params. SetRepeats(...)"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}
	/*
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	if (iRecorder->State() != CMdaAudioRecorderUtility::ENotReady)
		return EFail;

	// open an existing file to record
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;
	*/
	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderRepeats::DoTestStepPostambleL(void)
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
void CTestStepRecorderRepeats::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	if (iRecorder->State() == CMdaAudioRecorderUtility::EPlaying &&
		aCurrentState == CMdaAudioRecorderUtility::EPlaying)	
		iHasPlayed = ETrue;

	if (!(iError == KErrNone && aCurrentState == CMdaAudioRecorderUtility::EPlaying))
		{
		// if it were not a "start playing" indication, then stop
		CActiveScheduler::Stop();
		}
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
TVerdict CTestStepRecorderRepeats::DoTestStepL()
	{
	// use the following test to check that test silence > KMaxTInt is supported
	// removed because it will take > 35 minutes

	/*
	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
		return EInconclusive;
		}
	iRecorder->SetRepeats(1, TTimeIntervalMicroSeconds(TInt64(KMaxTInt) + 10000000));
	iRecorder->PlayL();
	CActiveScheduler::Start(); // Wait for play to start
	if (iError != KErrNone || !iHasPlayed)
		{
		return EFail;
		}
	iHasPlayed = EFalse;
	delete iRecorder;
	iRecorder = NULL;
	*/
	
	// try several combination of repeats and trailing silence
	for (TInt rep=0 ; rep<=3 ; rep++) // repeats loop
		{
		iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
		iRecorder->OpenFileL(iFileName);
		CActiveScheduler::Start();
		if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;

		iRecorder->SetRepeats(rep, TTimeIntervalMicroSeconds(0) );
		iRecorder->PlayL();
		CActiveScheduler::Start();
		INFO_PRINTF1(_L("external loop"));
		INFO_PRINTF2(_L("  repeat: %d, trail: 0 "), rep);
		if( iError != KErrNone ||
			!iHasPlayed )
			return EFail;
		iHasPlayed = EFalse;
		
		delete iRecorder;
		iRecorder = NULL;
		for ( TInt64 trail=1000; trail<=1000000; trail*=10) // trailing silence loop
			{
			iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
			iRecorder->OpenFileL(iFileName);
			CActiveScheduler::Start();
			if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
				return EInconclusive;
			iRecorder->SetRepeats(rep, TTimeIntervalMicroSeconds(trail) );
			iRecorder->PlayL();
			CActiveScheduler::Start();
			//INFO_PRINTF1(_L("internal loop"));
			INFO_PRINTF3(_L("  repeat: %d, trail: %d "), rep, trail);
			if( iError != KErrNone ||
				!iHasPlayed )
				return EFail;
			iHasPlayed = EFalse;
			delete iRecorder;
			iRecorder = NULL;
			} // end for trailing silence
			
		} // end for repeats

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

