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
#include "TestStepRecorderAllocFail.h"


#include "MmfAudioController.h"
#include "mmfclientaudiorecorder.h"
//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepRecorderAllocFailPlay.
 *
 *
 * @return	"CTestStepRecorderAllocFailPlay*"
 *			The constructed CTestStepRecorderAllocFailPlay
 *
 * @xxxx
 * 
 */
CTestStepRecorderAllocFailPlay* CTestStepRecorderAllocFailPlay::NewL()
	{
	CTestStepRecorderAllocFailPlay* self = new(ELeave) CTestStepRecorderAllocFailPlay;
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
CTestStepRecorderAllocFailPlay::CTestStepRecorderAllocFailPlay()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0305-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderAllocFailPlay::~CTestStepRecorderAllocFailPlay()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderAllocFailPlay::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0305-CP"));
	INFO_PRINTF1(_L("this is Alloc failure test of CMdaAudioRecorderUtility::PlayL"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start(); 

	if ( iError != KErrNone || 
		 iRecorder->State() != 1) // EOpened
		verdict = EInconclusive;

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderAllocFailPlay::DoTestStepPostambleL(void)
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
void CTestStepRecorderAllocFailPlay::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

// this can return a TBool hasPlayed to prove that it has played
// but the EPlaying state seems to get preserved for some reason...
void CTestStepRecorderAllocFailPlay::PlayAndStartSchedulerL() 
	{
	iRecorder->PlayL();
	CActiveScheduler::Start(); // open->play 
	User::LeaveIfError(iError);
	CActiveScheduler::Start(); // play->open
	//INFO_PRINTF2(_L("playing state: %d "), iRecorder->State() );
	User::LeaveIfError(iError);
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
TVerdict CTestStepRecorderAllocFailPlay::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		//__MM_HEAP_MARK;

		TRAP( err, PlayAndStartSchedulerL() );

		completed = EFalse;
		if (err == KErrNone)
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				User::Free(testAlloc);
			// see if valid result and break if wrong - might be premature result
			//INFO_PRINTF2(_L("state CMdaAudioRecorderUtility %d"), static_cast<CMMFMdaAudioRecorderUtility*>(iRecorder)->iState);
			if (iError != KErrNone ||
				iRecorder->State() != CMdaAudioRecorderUtility::EOpen) 
				badResult = ETrue;

			completed = reachedEnd || badResult;
			}
		else if (err != KErrNoMemory)
			completed = ETrue;

		//__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	//failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc.

	if (err != KErrNone || badResult)
		{
		if (badResult)
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
		else
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

