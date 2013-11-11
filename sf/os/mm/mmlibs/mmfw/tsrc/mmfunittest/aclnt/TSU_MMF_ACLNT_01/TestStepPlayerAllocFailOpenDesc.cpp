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
#include "TestStepPlayerAllocFailOpen.h"

#include "mmfclientaudioplayer.h"
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepPlayerAllocFailOpenDesc.
 *
 *
 * @return	"CTestStepPlayerAllocFailOpenDesc*"
 *			The constructed CTestStepPlayerAllocFailOpenDesc
 *
 * @xxxx
 * 
 */
CTestStepPlayerAllocFailOpenDesc* CTestStepPlayerAllocFailOpenDesc::NewL()
	{
	CTestStepPlayerAllocFailOpenDesc* self = new(ELeave) CTestStepPlayerAllocFailOpenDesc;
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
CTestStepPlayerAllocFailOpenDesc::CTestStepPlayerAllocFailOpenDesc()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0105-CP");
	iHeapSize = 100000;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerAllocFailOpenDesc::~CTestStepPlayerAllocFailOpenDesc()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerAllocFailOpenDesc::MapcInitComplete(TInt aError, 
														const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerAllocFailOpenDesc::MapcPlayComplete(TInt aError)
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
enum TVerdict CTestStepPlayerAllocFailOpenDesc::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0105-CP"));
	INFO_PRINTF1(_L("this is Alloc failure test of CMdaAudioPlayerUtility::OpenDesL"));

	iError = KErrNone; //KErrTimedOut;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// load the contents of an audio file to a Descriptor
	TInt fSize;

	iFs.Connect();
	if ( iFile.Open(iFs, iFileName, EFileRead) != KErrNone )
		return EInconclusive;
	iFile.Size(fSize);
	// load the sound from the file to a Descriptor
	iBuf = HBufC8::NewL( fSize );
	TPtr8 buf = iBuf->Des();
	iFile.Read(buf);

	iFile.Close();
	iFs.Close();

	// create a CMdaAudioPlayerUtility
	if ( (iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this)) == NULL )
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
enum TVerdict CTestStepPlayerAllocFailOpenDesc::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	delete iBuf;
	iBuf = NULL;
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
void CTestStepPlayerAllocFailOpenDesc::OpenDescStartSchedulerL()
	{
	iPlayer->OpenDesL(iBuf->Des());
	// Wait for initialisation callback
	CActiveScheduler::Start();
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
TVerdict CTestStepPlayerAllocFailOpenDesc::DoTestStepL()
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

		TRAP( err, OpenDescStartSchedulerL() );
		//TRAP( err, 	(iPlayer->OpenDesL(iBuf->Des())) );
		//TRAP( err, CActiveScheduler::Start() );

		completed = EFalse;
		if (err == KErrNone && iError == KErrNone)
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--;
				}
			else
				User::Free(testAlloc);
			// see if valid result and break if wrong - might be premature result
			// check if the source handle (Descriptor here) is NULL and the state not EStopped
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
				 iPlayer->Duration() == TTimeIntervalMicroSeconds(0) || // we know the duration of the file is more than 0
				 iError != KErrNone)
				badResult = ETrue;
			if (!reachedEnd && !badResult && iError == KErrNone)
				{
				// the last run of this loop doesn't attempt to allocate any memory, so check whether
				// the callback completed without error. If it did, we have reached the end.
				reachedEnd = ETrue;
				}
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
			{
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
			}
		else
			{
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
			}
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

