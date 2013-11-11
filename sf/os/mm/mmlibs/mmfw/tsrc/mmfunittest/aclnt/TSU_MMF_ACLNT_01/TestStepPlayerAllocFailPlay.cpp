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
#include "TestStepPlayerAllocFailNew.h"

#include "mmfclientaudioplayer.h"

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepPlayerAllocFailPlay.
 *
 *
 * @return	"CTestStepPlayerAllocFailPlay*"
 *			The constructed CTestStepPlayerAllocFailPlay
 *
 * @xxxx
 * 
 */
CTestStepPlayerAllocFailPlay* CTestStepPlayerAllocFailPlay::NewL()
	{
	CTestStepPlayerAllocFailPlay* self = new(ELeave) CTestStepPlayerAllocFailPlay;
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
CTestStepPlayerAllocFailPlay::CTestStepPlayerAllocFailPlay() : iHasPlayed(EFalse)
	{
	iTestStepResult = EFail;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0102-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerAllocFailPlay::~CTestStepPlayerAllocFailPlay()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerAllocFailPlay::MapcInitComplete(TInt aError, 
													const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerAllocFailPlay::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	if (iError == KErrNone)
		iHasPlayed = ETrue;
	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerAllocFailPlay::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0102-CP"));
	INFO_PRINTF1(_L("this is an Alloc failure test of the CMdaAudioPlayerUtility::Play() "));

	iError = KErrNone; //KErrTimedOut;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	// Wait for initialisation callback
	CActiveScheduler::Start();
	TInt state = 0;
	TInt err = KErrNone;
	err = GetAudPlayerState(iPlayer, state);
	if (iError != KErrNone ||
		err != KErrNone ||
		state != CMMFMdaAudioPlayerUtility::EStopped )
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
enum TVerdict CTestStepPlayerAllocFailPlay::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * The commands to be included in the TRAP command
 *
 * @xxxx
 * 
 */
void CTestStepPlayerAllocFailPlay::PlayAndStartSchedulerL()
	{
	iPlayer->Play();
	CActiveScheduler::Start();
	User::LeaveIfError(iError);
	//INFO_PRINTF2(_L("played: %d times"),(static_cast<CMMFMdaAudioPlayerUtility*>(iPlayer)->iNumberOfTimesPlayed) );
	//INFO_PRINTF2(_L("position: %d microsecs"),(static_cast<CMMFMdaAudioPlayerUtility*>(iPlayer)->iPosition.Int64() ) );
	//INFO_PRINTF2(_L("State: %d "),(static_cast<CMMFMdaAudioPlayerUtility*>(iPlayer)->iState ) );
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
TVerdict CTestStepPlayerAllocFailPlay::DoTestStepL()
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
		if (err == KErrNone && iError == KErrNone)
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
			TInt err1 = KErrNone;
			TInt err2 = KErrNone;
			TInt state = 0;
			TMMFMessageDestination* srcHandle = NULL;
			err1 = GetAudPlayerState(iPlayer, state);
			err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
			if ( err1 != KErrNone ||
				 err2 != KErrNone ||
				 !iHasPlayed ||
				 &srcHandle == NULL ||
				 state != CMMFMdaAudioPlayerUtility::EStopped ||
				 iError != KErrNone)
				badResult = ETrue;
			if (iPlayer != NULL)
				{
				delete iPlayer;
				iPlayer = NULL;
				}
			completed = reachedEnd || badResult;
			}
		else if ((err && err != KErrNoMemory) || (err == KErrNone && iError != KErrNoMemory)) // bad error code
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

