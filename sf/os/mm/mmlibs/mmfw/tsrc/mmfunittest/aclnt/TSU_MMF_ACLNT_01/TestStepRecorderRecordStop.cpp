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
// TestStepRecorderRecord.cpp
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

_LIT(KTestRecord, "c:\\RecStop.wav");

/**

 Static constructor for CTestStepRecorderRecordStop.


 @return "CTestStepRecorderRecordStop*"
		 The constructed CTestStepRecorderRecordStop

 
*/
CTestStepRecorderRecordStop* CTestStepRecorderRecordStop::NewL()
	{
	CTestStepRecorderRecordStop* self = new(ELeave) CTestStepRecorderRecordStop;
	return self;
	}

/**

 Test step constructor.
 Each test step initialises its own name.

 
 */
CTestStepRecorderRecordStop::CTestStepRecorderRecordStop() : iError(0)

	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0826-HP");
	}

/**

 Test step destructor.


 
 */
CTestStepRecorderRecordStop::~CTestStepRecorderRecordStop()
	{
	}

/**

 Test step Preamble.


 
 */
enum TVerdict CTestStepRecorderRecordStop::DoTestStepPreambleL()
	{
	 enum TVerdict verdict = EPass;

	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0826-HP"));
	INFO_PRINTF1(_L("this is a test of the fix for DEF 42395"));

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	iTimer = CTestStepRecorderRecordStopTimer::NewL(this, iRecorder);

	// Delete the output file.
	User::LeaveIfError(iFs.Connect());
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(KTestRecord);


	iClipFileName = new TMdaFileClipLocation(KTestRecord);

	// open a new file to record to.
	iStopping = ETrue;
	iRecorder->OpenL(iClipFileName, &iClipFormat, &iClipCodec);
	CActiveScheduler::Start(); 
	if (iError)
		{
		INFO_PRINTF2(_L("Open returned %d"), iError);
		return EFail;
		}

	if (iError != KErrNone || iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()/2);
	iBeginning = new TTimeIntervalMicroSeconds(0);
	iRecorder->SetPosition(*iBeginning);
	iRecorder->CropL();
	return verdict;
	}

/**

 Test step Postamble.



 */
enum TVerdict CTestStepRecorderRecordStop::DoTestStepPostambleL()
	{
	// Delete the output file.
	iFileMan->Delete(KTestRecord);
	iFs.Close();

	delete iFileMan;
	iFileMan = NULL;

	delete iRecorder;
	iRecorder = NULL;

	delete iTimer;
	iTimer = NULL;

	delete iBeginning;
	iBeginning = NULL;

	// Destroy the scheduler.
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**

 Callback Handle.


 
 */
void CTestStepRecorderRecordStop::MoscoStateChangeEvent(CBase* /* aObject */, TInt /* aPreviousState */, TInt /* aCurrentState */, TInt aErrorCode)
	{
	iError = aErrorCode;

	if (iStopping || (iError != KErrNone))
		CActiveScheduler::Stop();
	else
		{
		if (iDelay>2400000)
			{
			iRecorder->Stop();
			CActiveScheduler::Stop();
			}
		else if (iDelay == 0)
			{
			iRecorder->Stop();

			IncrementAndLogDelay();
			iRecorder->RecordL();
			}
		else
			iTimer->After(iDelay);
		}
	}


CTestStepRecorderRecordStopTimer* CTestStepRecorderRecordStopTimer::NewL(CTestStepRecorderRecordStop* aTestStep, CMMFMdaAudioRecorderUtility* aRecorder)
	{
	CTestStepRecorderRecordStopTimer* self = new(ELeave) CTestStepRecorderRecordStopTimer(aTestStep, aRecorder);
	self->ConstructL();
	return self;
	}

CTestStepRecorderRecordStopTimer::CTestStepRecorderRecordStopTimer(CTestStepRecorderRecordStop* aTestStep, CMMFMdaAudioRecorderUtility* aRecorder)
: CTimer(0), iTestStep(aTestStep), iRecorder(aRecorder)
	{
	CActiveScheduler::Add(this);
	}

void CTestStepRecorderRecordStopTimer::RunL()
	{
	iRecorder->Stop();

	iTestStep->IncrementAndLogDelay();
	iRecorder->RecordL();
	}

// This can return a TBool hasPlayed to prove that it has played
// but the EPlaying state seems to get preserved for some reason...
void CTestStepRecorderRecordStop::RecordAndStartSchedulerL() 
	{
	INFO_PRINTF2(_L("iDelay = %d"), iDelay);
	iRecorder->RecordL();

	iStopping = EFalse;
	CActiveScheduler::Start(); 
	}
	
/**
 
 Do the test step.
 Each test step must supply an implementation for DoTestStepL.
 
 @return	"TVerdict"
 			The result of the test step
 
 
  
 */
TVerdict CTestStepRecorderRecordStop::DoTestStepL()
	{
	iTestStepResult = EPass;
	TRAPD(err, RecordAndStartSchedulerL());

	// check result
	if ((iError != KErrNone) || (err != KErrNone) || (iRecorder->State() != CMdaAudioRecorderUtility::EOpen))
		{
		INFO_PRINTF4(_L("Unexpected result (%d, %d, %d)"), iError, err, iRecorder->State());
		iTestStepResult = EFail;
		}

	delete iRecorder;
	iRecorder = NULL;

	return iTestStepResult;
	}

void CTestStepRecorderRecordStop::IncrementAndLogDelay()
	{
	if (iDelay<100)
		iDelay += 10;
	else if(iDelay<1000)
		iDelay += 100;
	else if (iDelay<10000)
		iDelay += 1000;
	else if (iDelay<100000)
		iDelay += 10000;
	else
		iDelay += 100000;

	INFO_PRINTF2(_L("iDelay = %d"), iDelay);
	}

