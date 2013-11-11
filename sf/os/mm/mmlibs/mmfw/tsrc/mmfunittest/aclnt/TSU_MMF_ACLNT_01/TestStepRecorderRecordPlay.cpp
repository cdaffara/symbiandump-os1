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

#include <mdaaudiosampleeditor.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepRecorder.h"


#include "MmfAudioController.h"
#include "mmfclientaudiorecorder.h"
// --------------------------------------------

const TInt KRecordSectionTime = 3000000;	// 3 seconds
const TInt64 KPlayVarianceTime = 500000;	// 0.5 seconds

/**
 *
 * Static constructor for CTestStepRecorderRecordPlay.
 *
 *
 * @return	"CTestStepRecorderRecord*"
 *			The constructed CTestStepRecorderRecordPlay
 *
 * @xxxx
 * 
 */
CTestStepRecorderRecordPlay* CTestStepRecorderRecordPlay::NewL()
	{
	CTestStepRecorderRecordPlay* self = new(ELeave) CTestStepRecorderRecordPlay;
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
CTestStepRecorderRecordPlay::CTestStepRecorderRecordPlay() : iError(0), iHasRecorded(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0235-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderRecordPlay::~CTestStepRecorderRecordPlay()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderRecordPlay::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::RecordL()-Stop()-PlayL()-RecordL()-Stop()-PlayL()-Close()"));
	INFO_PRINTF1(_L("Also checks DEF091996: T: GSM610 codec: Timing related issues while recording and playing back"));

	if (!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord"), iFileName))
		{
		return EInconclusive;
		}

	if ((iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL)
		{
		return EInconclusive;
		}

	// ensure the test file does not exist
	User::LeaveIfError(iFs.Connect());
	CFileMan* fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	// open a non-existing file to record to
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start(); 
	if (iError != KErrNone || iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
		INFO_PRINTF1(_L("OpenFileL() failed"));
		return EInconclusive;
		}

	TRAPD(err, iRecorder->SetDestinationDataTypeL(KMMFFourCCCodeGSM610));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("SetDestinationDataTypeL() failed"));
		return EFail;
		}

	iRecorder->SetGain(iRecorder->MaxGain() / 2);
	iRecorder->SetPosition(TTimeIntervalMicroSeconds(0));
	iRecorder->CropL();
	if (iError != KErrNone || iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
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
enum TVerdict CTestStepRecorderRecordPlay::DoTestStepPostambleL(void)
	{
	//delete the output file 
	User::LeaveIfError(iFs.Connect());
	CFileMan* fileMan = CFileMan::NewL(iFs);
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
void CTestStepRecorderRecordPlay::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

// this can return a TBool hasPlayed to prove that it has played
// but the EPlaying state seems to get preserved for some reason...
void CTestStepRecorderRecordPlay::RecordAndStartSchedulerL() 
	{
	iRecorder->RecordL();
	CActiveScheduler::Start(); // open -> record

	// wait for a while (probably recording silence)
	User::After(KRecordSectionTime); 
	if (iRecorder->State() == CMdaAudioRecorderUtility::ERecording)
		{
		iHasRecorded = ETrue;
		}
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
TVerdict CTestStepRecorderRecordPlay::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// record first section
	TRAP( err, RecordAndStartSchedulerL() );

	// check result
	if (iError != KErrNone || err != KErrNone || !iHasRecorded || iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
		INFO_PRINTF1(_L("First record failed"));
		return EFail;
		}

	err = iRecorder->SetThreadPriorityRecord(EPriorityAbsoluteHigh); 
	TInt err2 = iRecorder->SetThreadPriorityPlayback(EPriorityAbsoluteHigh);
	if (err != KErrNone || err2 != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to change the recorder/player thread priority"));
		return EFail;
		}
	
	// play the first section
	iRecorder->PlayL();
	CActiveScheduler::Start(); // open -> play
	if (iError != KErrNone || iRecorder->State() != CMdaAudioRecorderUtility::EPlaying)
		{
		INFO_PRINTF1(_L("First play failed"));
		return EFail;
		}

	CActiveScheduler::Start(); // play -> open
	if (iError != KErrNone || iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
		return EFail;
		}

	// record the second section
	TRAP( err, RecordAndStartSchedulerL() );

	// check result
	if (iError != KErrNone || err != KErrNone || !iHasRecorded || iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
		INFO_PRINTF1(_L("Second record failed"));
		return EFail;
		}

	// time the combined play of both recorded sections
	TTime startTime;
	startTime.HomeTime();
	
	iRecorder->PlayL();
	CActiveScheduler::Start(); // open -> play
	if (iError != KErrNone || iRecorder->State() != CMdaAudioRecorderUtility::EPlaying)
		{
		INFO_PRINTF1(_L("Second play failed"));
		return EFail;
		}

	CActiveScheduler::Start(); // play -> open
	if (iError != KErrNone || iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
		return EFail;
		}

	TTime endTime;
	endTime.HomeTime();
	TTimeIntervalMicroSeconds playTime = endTime.MicroSecondsFrom(startTime);
	TInt64 playTimeDiscrepancy = iRecorder->Duration().Int64() - playTime.Int64();

	if (Abs(playTimeDiscrepancy) > KPlayVarianceTime)
		{
		INFO_PRINTF2(_L("Discrepancy between playing time and Duration(), %d"), playTimeDiscrepancy);
		return EFail;
		}

	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;
		
	INFO_PRINTF1(_L("finished with this test step"));

	return iTestStepResult;
	}

