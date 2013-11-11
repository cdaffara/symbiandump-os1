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

//if frame size is 4096 bytes, then a sample file of 8KHz, 16bit, mono, 
// would be 16000 bytes per second, therefore a frame would represent 256 millisecs
// we make sure the deviation is not greater than a frame since we can only crop frames.
const TInt KExpectedDeviation	 = (256000); 

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepRecorderAllocFailCrop.
 *
 *
 * @return	"CTestStepRecorderAllocFailCrop*"
 *			The constructed CTestStepRecorderAllocFailCrop
 *
 * @xxxx
 * 
 */
CTestStepRecorderAllocFailCrop* CTestStepRecorderAllocFailCrop::NewL()
	{
	CTestStepRecorderAllocFailCrop* self = new(ELeave) CTestStepRecorderAllocFailCrop;
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
CTestStepRecorderAllocFailCrop::CTestStepRecorderAllocFailCrop() : iError(0),
																   iHasCropped(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0307-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderAllocFailCrop::~CTestStepRecorderAllocFailCrop()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderAllocFailCrop::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0307-CP"));
	INFO_PRINTF1(_L("this is Alloc failure test of CMdaAudioRecorderUtility::CropL"));

	// open the file to be cropped
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToCrop"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	// create a temp copy of the original file
	iFileNameTmp = iFileName;
	_LIT(KExtTmp,".tmp");
	iFileNameTmp.Append(KExtTmp);
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Copy(iFileName, iFileNameTmp);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	iRecorder->OpenFileL(iFileNameTmp);
	CActiveScheduler::Start(); 
	if (iError != KErrNone)
		return EInconclusive;


	//iRecorder->SetGain(iRecorder->MaxGain()/.2);
	iOrigDuration = iRecorder->Duration().Int64();
	const TTimeIntervalMicroSeconds middle( iOrigDuration / 2 );
	
	// since iRecorder is not currently active this will not set the position
	// immediately - instead the position requested will be stored for later
	iRecorder->SetPosition(middle);
	
//	INFO_PRINTF2(_L("position : %d"), );
	if ( iError != KErrNone || 
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
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
enum TVerdict CTestStepRecorderAllocFailCrop::DoTestStepPostambleL(void)
	{
	iRecorder->Close();
	//delete the temp, cropped file 
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileNameTmp);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;
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
void CTestStepRecorderAllocFailCrop::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

// this can return a TBool hasPlayed to prove that it has played
// but the EPlaying state seems to get preserved for some reason...
void CTestStepRecorderAllocFailCrop::CropAndStartSchedulerL() 
	{
	// crop to end [chuck away all data after current position]
	iRecorder->CropL();

	// Check if the new duration is half of the original one
	
	// N.B. the file will be cropped to the nearest frame
	// this is calculated by integer division i.e. to the previous
	// frame boundary - this frame boundary is KExpectedDeviation micro secs
	TInt64 cropDur = iRecorder->Duration().Int64();
	TInt64 difDur  = iOrigDuration / 2;
	TInt delta = Abs(I64INT(cropDur - difDur));
		
	if ( delta <= KExpectedDeviation )
		iHasCropped = ETrue;
	
	iRecorder->Stop();
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
TVerdict CTestStepRecorderAllocFailCrop::DoTestStepL()
	{
	iTestStepResult = EPass;
	iError = KErrNone;
    TInt err = KErrNone;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		//__MM_HEAP_MARK;

		TRAP( err, CropAndStartSchedulerL() );

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
			//INFO_PRINTF2(_L("state CMdaAudioRecorderUtility %d"), iRecorder->State() );
			if (iError != KErrNone ||
				!iHasCropped ||
				iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
					badResult = ETrue;

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
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

