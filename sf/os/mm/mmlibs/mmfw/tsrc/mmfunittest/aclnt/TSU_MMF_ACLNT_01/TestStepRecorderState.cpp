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
 * Static constructor for CTestStepRecorderState.
 *
 *
 * @return	"CTestStepRecorderState*"
 *			The constructed CTestStepRecorderState
 *
 * @xxxx
 * 
 */
CTestStepRecorderState* CTestStepRecorderState::NewL()
	{
	CTestStepRecorderState* self = new(ELeave) CTestStepRecorderState;
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
CTestStepRecorderState::CTestStepRecorderState() : iError(KErrNone), iHasRecorded(EFalse), iHasPlayed(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0254-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderState::~CTestStepRecorderState()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderState::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0254-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::State()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderState::DoTestStepPostambleL(void)
	{
	//delete the output file 
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

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
void CTestStepRecorderState::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	if (iRecorder->State() == CMdaAudioRecorderUtility::EPlaying &&
		aCurrentState == CMdaAudioRecorderUtility::EPlaying)	
		iHasPlayed = ETrue;

	CActiveScheduler::Stop();
	}

// this can return a TBool hasPlayed to prove that it has played
// but the EPlaying state seems to get preserved for some reason...
void CTestStepRecorderState::RecordAndStartSchedulerL() 
	{
	iRecorder->RecordL();
	CActiveScheduler::Start(); 
	// record 2 secs, ...probably silence
	User::After(2000000); 
	if (iRecorder->State() == CMdaAudioRecorderUtility::ERecording)
		iHasRecorded = ETrue;
	iRecorder->Stop();
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
TVerdict CTestStepRecorderState::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	if (iRecorder->State() != CMdaAudioRecorderUtility::ENotReady)
		return EFail;

	// open an existing file to record and crop all its contents if any
	TRAP( err, iRecorder->OpenFileL(iFileName) );
	if (err != KErrNone )
		return EInconclusive;

	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;
	
	iRecorder->SetGain(iRecorder->MaxGain()/2);
	iRecorder->SetPosition(TTimeIntervalMicroSeconds(0));
	iRecorder->CropL();
	
	if ( iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	TRAP( err, RecordAndStartSchedulerL() );

	if (err != KErrNone ||
		!iHasRecorded ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iRecorder->SetPosition(TTimeIntervalMicroSeconds(0));
	iRecorder->PlayL();
	CActiveScheduler::Start();
	User::After(1000000); 
	if (iError != KErrNone ||
		!iHasPlayed)
		return EFail;

	iRecorder->Stop();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iRecorder->Close();
	if (iRecorder == NULL ||
		iRecorder->State() != CMdaAudioRecorderUtility::ENotReady)
		return EFail;

	delete iRecorder;
	iRecorder = NULL;

		
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

