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

#include <mdaaudiotoneplayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepTone.h"
#include "mmfclienttoneplayer.h"


// --------------------------------------------

/**
 *
 * Static constructor for CTestStepToneAllocFailNew.
 *
 *
 * @return	"CTestStepToneAllocFailNew*"
 *			The constructed CTestStepToneAllocFailNew
 *
 * @xxxx
 * 
 */
CTestStepToneAllocFailNew* CTestStepToneAllocFailNew::NewL()
	{
	CTestStepToneAllocFailNew* self = new(ELeave) CTestStepToneAllocFailNew;
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
CTestStepToneAllocFailNew::CTestStepToneAllocFailNew() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0400-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepToneAllocFailNew::~CTestStepToneAllocFailNew()
	{
	}

void CTestStepToneAllocFailNew::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepToneAllocFailNew::MatoPlayComplete(TInt aError)
	{
	iError = aError;
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
TVerdict CTestStepToneAllocFailNew::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0400-CP"));
	INFO_PRINTF1(_L("this is Alloc Failure test of CMMFMdaAudioToneUtility::NewL() "));
		
	__MM_HEAP_MARK;
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	if (iTone == NULL ||
		err != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityNotReady  )
		return EFail;
	delete iTone;
	iTone = NULL;
	__MM_HEAP_MARKEND;
	
	// fail it explicitly cause of CMMFDevSound::ContructL() failure
	iTestStepResult = EFail;
	

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );

		completed = EFalse;
		if (err == KErrNone)
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // Failcount of 1 equates to 0 successful allocs, etc.
				}
			else
				{
				User::Free(testAlloc);
				}

			// see if valid result and break if wrong - might be premature result
			if (iTone == NULL || (iTone->State() != EMdaAudioToneUtilityNotReady) )
				{
				badResult = ETrue;
				}
			else
				{
				if (iTone != NULL)
					{
					delete iTone;
					iTone = NULL;
					}
				}
			completed = reachedEnd || badResult;
			}
		else if (err != KErrNoMemory) // bad error code
			{
			completed = ETrue;
			}

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}


	if ((err != KErrNone) || badResult)
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
	return iTestStepResult;
	}

