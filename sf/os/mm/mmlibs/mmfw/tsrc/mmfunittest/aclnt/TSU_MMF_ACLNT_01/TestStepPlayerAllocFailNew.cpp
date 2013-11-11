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
 * Static constructor for CTestStepPlayerAllocFailNew.
 *
 *
 * @return	"CTestStepPlayerAllocFailNew*"
 *			The constructed CTestStepPlayerAllocFailNew
 *
 * @xxxx
 * 
 */
CTestStepPlayerAllocFailNew* CTestStepPlayerAllocFailNew::NewL()
	{
	CTestStepPlayerAllocFailNew* self = new(ELeave) CTestStepPlayerAllocFailNew;
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
CTestStepPlayerAllocFailNew::CTestStepPlayerAllocFailNew()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0100-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerAllocFailNew::~CTestStepPlayerAllocFailNew()
	{
	}

/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerAllocFailNew::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerAllocFailNew::MapcPlayComplete(TInt /*aError*/)
	{
	// not used inthis test
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
TVerdict CTestStepPlayerAllocFailNew::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0100-CP"));
	INFO_PRINTF1(_L("this is Alloc Failure test of iniatilising a CMdaAudioPlayerUtility object with NewL()"));

	//__MM_HEAP_MARK;
	TRAP( err, iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this) );
	delete iPlayer;
	iPlayer = NULL;
	//__MM_HEAP_MARKEND;

	if (err != KErrNone)
		return EFail;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		//__MM_HEAP_MARK;

		TRAP( err, (iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this)) );

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
			TInt err1 = KErrNone;
			TInt err2 = KErrNone;
			TInt state = 0;
			TMMFMessageDestination* srcHandle = NULL;
			err1 = GetAudPlayerState(iPlayer, state);
			err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
			if ( err1 != KErrNone ||
				 err2 != KErrNone ||
				 &srcHandle == NULL ||
				 state != CMMFMdaAudioPlayerUtility::EStopped )
				badResult = ETrue;
			if (iPlayer != NULL)
				{
				delete iPlayer;
				iPlayer = NULL;
				}
			completed = reachedEnd || badResult;
			}
		else if (err != KErrNoMemory) // bad error code
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

	if (iPlayer != NULL)
		{
		delete iPlayer;
		iPlayer = NULL;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

