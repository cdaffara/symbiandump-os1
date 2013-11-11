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
 * Static constructor for CTestStepRecorderReopen.
 *
 *
 * @return	"CTestStepRecorderReopen*"
 *			The constructed CTestStepRecorderReopen
 *
 * @xxxx
 * 
 */
CTestStepRecorderReopen* CTestStepRecorderReopen::NewL()
	{
	CTestStepRecorderReopen* self = new(ELeave) CTestStepRecorderReopen;
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
CTestStepRecorderReopen::CTestStepRecorderReopen()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0230-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderReopen::~CTestStepRecorderReopen()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderReopen::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0230-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::OpenL()-Close()-Open()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName)) // wav file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Recorder utility
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
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
enum TVerdict CTestStepRecorderReopen::DoTestStepPostambleL(void)
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
void CTestStepRecorderReopen::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderReopen::OpenAndStartSchedulerL()
	{
	iRecorder->OpenFileL( iFileName );
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
TVerdict CTestStepRecorderReopen::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	TRAP( err, OpenAndStartSchedulerL() );

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		return EFail;

	iRecorder->Close();
	User::After(200000);

	// reopen the file
	TRAP( err, OpenAndStartSchedulerL() );

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		return EFail;

	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


// new test for defect DEF022197 :

/**
 *
 * Static constructor for CTestStepRecorderPlayRecordPlay.
 *
 *
 * @return	"CTestStepRecorderPlayRecordPlay*"
 *			The constructed CTestStepRecorderPlayRecordPlay
 *
 * @xxxx
 * 
 */
CTestStepRecorderPlayRecordPlay* CTestStepRecorderPlayRecordPlay::NewL()
	{
	CTestStepRecorderPlayRecordPlay* self = new(ELeave) CTestStepRecorderPlayRecordPlay;
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
CTestStepRecorderPlayRecordPlay::CTestStepRecorderPlayRecordPlay()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0281-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderPlayRecordPlay::~CTestStepRecorderPlayRecordPlay()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderPlayRecordPlay::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0281-HP"));
	INFO_PRINTF1(_L("this is a test of playing a file, recording more data to it, then playing it again"));

	// record_n_delete_after.wav
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord4"), iFileNameWav)) 
		return EInconclusive;

	// record_n_delete_after.au
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord5"), iFileNameAu)) 
		return EInconclusive;

	// we can't play raw files with the CMMFMdaAudioPlayerUtility API !
	// record_n_delete_after.raw
	//if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord6"), iFileNameRaw)) 
	//	return EInconclusive;

	// create the Recorder utility
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	// create the Player utility
	if ( (iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;


	return verdict;

	}

void CTestStepRecorderPlayRecordPlay::DeleteFileL(TPtrC aFileName)
	{
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(aFileName);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderPlayRecordPlay::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	delete iRecorder;
	iRecorder = NULL;

	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);

	iFileMan->Delete(iFileNameWav);
	iFileMan->Delete(iFileNameAu);
	//iFileMan->Delete(iFileNameRaw);
	
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;

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
void CTestStepRecorderPlayRecordPlay::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderPlayRecordPlay::OpenAndStartSchedulerL(TPtrC aFileName)
	{
	iRecorder->OpenFileL( aFileName );
	CActiveScheduler::Start(); 
	}


void CTestStepRecorderPlayRecordPlay::RecordPlayRecordTestL(TPtrC aFileName)
	{
    TInt err       = KErrNone;

	INFO_PRINTF2(_L("Recording to file %S..."), &aFileName);
	
	DeleteFileL(aFileName);
	OpenAndStartSchedulerL(aFileName);

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		{
		iTestStepResult = EFail;
		User::Leave(KErrGeneral);
		}
	iRecorder->SetPosition(TTimeIntervalMicroSeconds(0));

	// record for 2 seconds, 
	iRecorder->RecordL();
	User::After(1000000);
	iRecorder->Stop();
	CActiveScheduler::Start(); 
	iRecorder->Close();


	iPlayer->OpenFileL( aFileName );
	CActiveScheduler::Start(); 

	iPlayer->SetPosition(TTimeIntervalMicroSeconds(0));
	iPlayer->Play();	
	CActiveScheduler::Start(); 
	
	TTimeIntervalMicroSeconds durationPlay1;
	durationPlay1 = iPlayer->Duration();
	INFO_PRINTF2(_L("file play duration after first recording = %u"), durationPlay1.Int64());
	

	// record another 2 seconds
	OpenAndStartSchedulerL(aFileName);
	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		{
		iTestStepResult = EFail;
		User::Leave(KErrGeneral);
		}

	iRecorder->RecordL();
	User::After(1000000);
	iRecorder->Stop();
	CActiveScheduler::Start(); 
	iRecorder->Close();


	// play the file again and get the new duration
	iPlayer->SetPosition(TTimeIntervalMicroSeconds(0));
	iPlayer->Play();	
	CActiveScheduler::Start(); 
	
	TTimeIntervalMicroSeconds durationPlay2;
	durationPlay2 = iPlayer->Duration();
	INFO_PRINTF2(_L("file play duration after second recording = %u"), durationPlay2.Int64());
	
	if (durationPlay2 <= durationPlay1)
		{
		INFO_PRINTF1(_L("Play duration not updated by record"));
		iTestStepResult = EFail;
		}
	
	INFO_PRINTF1(_L("finished with this test step"));
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
TVerdict CTestStepRecorderPlayRecordPlay::DoTestStepL()
	{
	iTestStepResult = EPass;

    TInt err = KErrNone;
	if (err == KErrNone)
		TRAP(err, RecordPlayRecordTestL(iFileNameWav));
	if (err != KErrNone)
		iTestStepResult = EFail;

	if (err == KErrNone)
		TRAP(err, RecordPlayRecordTestL(iFileNameAu));
	if (err != KErrNone)
		iTestStepResult = EFail;

	// test steps return a result
	return iTestStepResult;
	}

void CTestStepRecorderPlayRecordPlay::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderPlayRecordPlay::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}
