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
 * Static constructor for CTestStepRecorderAllocFailOpen.
 *
 *
 * @return	"CTestStepRecorderAllocFailOpen*"
 *			The constructed CTestStepRecorderAllocFailOpen
 *
 * @xxxx
 * 
 */
CTestStepRecorderAllocFailOpen* CTestStepRecorderAllocFailOpen::NewL()
	{
	CTestStepRecorderAllocFailOpen* self = new(ELeave) CTestStepRecorderAllocFailOpen;
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
CTestStepRecorderAllocFailOpen::CTestStepRecorderAllocFailOpen()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0303-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderAllocFailOpen::~CTestStepRecorderAllocFailOpen()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderAllocFailOpen::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0303-CP"));
	INFO_PRINTF1(_L("this is Alloc failure test of CMdaAudioRecorderUtility::OpenDesL"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Recorder utility
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	iClipLocation = new TMdaFileClipLocation(iFileName);
	iFormat = new TMdaWavClipFormat();

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderAllocFailOpen::DoTestStepPostambleL(void)
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
void CTestStepRecorderAllocFailOpen::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderAllocFailOpen::OpenAndStartSchedulerL()
	{
	iRecorder->OpenL( iClipLocation, iFormat );
	CActiveScheduler::Start(); 
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
TVerdict CTestStepRecorderAllocFailOpen::DoTestStepL()
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

		TRAP( err, OpenAndStartSchedulerL() );

		completed = EFalse;
		if ((err == KErrNone) && (iError == KErrNone))
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				{
				User::Free(testAlloc);
				}

			// see if valid result and break if wrong - might be premature result
			INFO_PRINTF2(_L("state CMdaAudioRecorderUtility %d"), iRecorder->State() );
			if (iRecorder == NULL ||
				iError != KErrNone ||
				iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
				badResult = ETrue;
			
			if(iRecorder)
				iRecorder->Close();

			completed = reachedEnd || badResult;
			}
		else if (((err != KErrNone) && (err != KErrNoMemory)) || 
				((iError != KErrNone) && (iError != KErrNoMemory))) // bad error code
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

