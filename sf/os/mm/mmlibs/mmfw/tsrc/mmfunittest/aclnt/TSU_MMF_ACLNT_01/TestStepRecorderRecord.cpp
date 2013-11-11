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

const TInt KTestRecordTime = 10000000;				// 10 sec
const TInt KTestRecordTimeTolerance = 2000000;		// 2 sec
const TInt KTestPointerValue = 0xdeadbeef;
const TInt KTestPointerRecorder1 = 0xdadadada;
const TInt KTestPointerRecorder2 = 0xbabababa;

_LIT(KTestRecordOne, "c:\\R1.wav");
_LIT(KTestRecordTwo, "c:\\R2.wav");
_LIT(KTestRecordThree, "c:\\R3.wav");

/**
 *
 * Static constructor for CTestStepRecorderRecord.
 *
 *
 * @return	"CTestStepRecorderRecord*"
 *			The constructed CTestStepRecorderRecord
 *
 * @xxxx
 *
 */
CTestStepRecorderRecord* CTestStepRecorderRecord::NewL()
	{
	CTestStepRecorderRecord* self = new(ELeave) CTestStepRecorderRecord;
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
CTestStepRecorderRecord::CTestStepRecorderRecord() : iError(0), iHasRecorded(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0207-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderRecord::~CTestStepRecorderRecord()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderRecord::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0207-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::RecordL"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord2"), iFileName2))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord3"), iFileName3))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	if (SetAudRecorderParent(iRecorder, REINTERPRET_CAST(CMdaAudioRecorderUtility*, KTestPointerValue)) != KErrNone)
		return EFail; // check value in MoscoStateChangeEvent

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iError)
		{
		INFO_PRINTF2(_L("open returned %d"), iError);
		return EFail;
		}

	if (iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()/2);
	iBeginning = new TTimeIntervalMicroSeconds(0);
	iRecorder->SetPosition(*iBeginning);
	iRecorder->CropL();

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
enum TVerdict CTestStepRecorderRecord::DoTestStepPostambleL(void)
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
void CTestStepRecorderRecord::MoscoStateChangeEvent(CBase* aObject,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorCode)
	{
	if (aObject != REINTERPRET_CAST(CBase*, KTestPointerValue))
		iError = KErrArgument;
	else
		iError = aErrorCode;
	CActiveScheduler::Stop();
	}

// this can return a TBool hasPlayed to prove that it has played
// but the EPlaying state seems to get preserved for some reason...
void CTestStepRecorderRecord::RecordAndStartSchedulerL()
	{
	iRecorder->RecordL();
	CActiveScheduler::Start();
	// record 2 secs, ...probably silence
	User::After(2000000);

    if (iRecorder->State() == CMdaAudioRecorderUtility::ERecording)
		iHasRecorded = ETrue;
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
TVerdict CTestStepRecorderRecord::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	TRAP( err, RecordAndStartSchedulerL() );

	// check result
	if (iError != KErrNone ||
		err != KErrNone ||
		!iHasRecorded ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iHasRecorded = EFalse;
	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;

	/*
	// record AU file //
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName2);
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()*.75);
	iRecorder->SetPosition(*iBeginning);
	iRecorder->CropL();

	if ( iError != KErrNone ||
		 iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	TRAP( err, RecordAndStartSchedulerL() );

	// check result
	if (iError != KErrNone ||
		err != KErrNone ||
		!iHasRecorded ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iHasRecorded = EFalse;
	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;

	// record RAW file //
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName3);
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()*.75);
	iRecorder->SetPosition(*iBeginning);
	iRecorder->CropL();

	if ( iError != KErrNone ||
		 iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	TRAP( err, RecordAndStartSchedulerL() );

	// check result
	if (iError != KErrNone ||
		err != KErrNone ||
		!iHasRecorded ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iHasRecorded = EFalse;
	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;
	*/

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



/**
 *
 * Static constructor for CTestStepRecorderRecordWav.
 *
 *
 * @return	"CTestStepRecorderRecordWav*"
 *			The constructed CTestStepRecorderRecordWav
 *
 * @xxxx
 *
 */
CTestStepRecorderRecordWav* CTestStepRecorderRecordWav::NewL()
	{
	CTestStepRecorderRecordWav* self = new(ELeave) CTestStepRecorderRecordWav;
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
CTestStepRecorderRecordWav::CTestStepRecorderRecordWav() : iError(0), iHasRecorded(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0269-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderRecordWav::~CTestStepRecorderRecordWav()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderRecordWav::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0269-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::RecordL with non existent wav file"));

	// set the file name to something weird
    iFileName.Set( _L("c:\\szhgefdsfe.wav"));

	//[ ensure the file does not exist]
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	if (SetAudRecorderParent(iRecorder, REINTERPRET_CAST(CMdaAudioRecorderUtility*, KTestPointerValue)) != KErrNone)
		return EFail; // check value in MoscoStateChangeEvent

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iError)
		{
		INFO_PRINTF2(_L("open returned %d"), iError);
		return EFail;
		}

	if (iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()/2);
	iBeginning = new TTimeIntervalMicroSeconds(0);
	iRecorder->SetPosition(*iBeginning);

	TRAPD( errorCode, iRecorder->CropL() );

	if ( iError != KErrNone ||
		 iRecorder->State() != CMdaAudioRecorderUtility::EOpen ||
		 errorCode != KErrNone )
		{
		 INFO_PRINTF3( _L("Failed after Crop in preamble: (iError, errorCode)  %x, %x"), iError, errorCode );
		 verdict = EInconclusive;
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
enum TVerdict CTestStepRecorderRecordWav::DoTestStepPostambleL(void)
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
void CTestStepRecorderRecordWav::MoscoStateChangeEvent(CBase* aObject,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorCode)
	{
	if (aObject != REINTERPRET_CAST(CBase*, KTestPointerValue))
		iError = KErrArgument;
	else
		iError = aErrorCode;
	CActiveScheduler::Stop();
	}

// this can return a TBool hasPlayed to prove that it has played
// but the EPlaying state seems to get preserved for some reason...
void CTestStepRecorderRecordWav::RecordAndStartSchedulerL()
	{
	iRecorder->RecordL();
	CActiveScheduler::Start();
	// record 2 secs, ...probably silence
	User::After(2000000);

	if (iRecorder->State() == CMdaAudioRecorderUtility::ERecording)
		iHasRecorded = ETrue;
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
TVerdict CTestStepRecorderRecordWav::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	TRAP( err, RecordAndStartSchedulerL() );

	// check result
	if (iError != KErrNone ||
		err != KErrNone ||
		!iHasRecorded ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iHasRecorded = EFalse;
	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



/**
*
* CTestStepRecorderRecordAu
*
*/

CTestStepRecorderRecordAu::CTestStepRecorderRecordAu()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0270-HP");
	}

/**
 *
 * Static constructor for CTestStepRecorderRecordAu.
 *
 *
 * @return	"CTestStepRecorderRecordAu*"
 *			The constructed CTestStepRecorderRecordAu
 *
 * @xxxx
 *
 */
CTestStepRecorderRecordAu* CTestStepRecorderRecordAu::NewL()
	{
	CTestStepRecorderRecordAu* self = new(ELeave) CTestStepRecorderRecordAu();
	return self;
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderRecordAu::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0270-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::RecordL with non existent wav file"));

	// set the file name to something weird
    iFileName.Set( _L("c:\\szhgefdsfe.au"));

	//[ ensure the file does not exist]
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	if (SetAudRecorderParent(iRecorder, REINTERPRET_CAST(CMdaAudioRecorderUtility*, KTestPointerValue)) != KErrNone)
		return EFail; // check value in MoscoStateChangeEvent

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iError)
		{
		INFO_PRINTF2(_L("open returned %d"), iError);
		return EFail;
		}

	if (iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()/2);
	iBeginning = new TTimeIntervalMicroSeconds(0);
	iRecorder->SetPosition(*iBeginning);

	iRecorder->CropL(); //debug code remove

	TRAPD( errorCode, iRecorder->CropL() );

	if ( iError != KErrNone ||
		 iRecorder->State() != CMdaAudioRecorderUtility::EOpen ||
		 errorCode != KErrNone )
		{
	    INFO_PRINTF3(_L("Test Failed after Crop operation in preamble: (iError, errorCode) (%X, %X)"),iError, errorCode );
		verdict = EInconclusive;
		}

	return verdict;

	}


CTestStepRecorderRecordRaw::CTestStepRecorderRecordRaw()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0271-CP");
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderRecordRaw::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0271-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::RecordL with non existent raw file"));

	// set the file name to something weird
    iFileName.Set( _L("c:\\szhgefdsfe.raw"));

	//[ ensure the file does not exist]
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	if (SetAudRecorderParent(iRecorder, REINTERPRET_CAST(CMdaAudioRecorderUtility*, KTestPointerValue)) != KErrNone)
		return EFail; // check value in MoscoStateChangeEvent

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iError)
		{
		INFO_PRINTF2(_L("open returned %d"), iError);
		return EFail;
		}

	if (iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()/2);
	iBeginning = new TTimeIntervalMicroSeconds(0);
	iRecorder->SetPosition(*iBeginning);
	TRAPD( errorCode, iRecorder->CropL())
	if ( iError != KErrNone ||
		 iRecorder->State() != CMdaAudioRecorderUtility::EOpen ||
		 errorCode != KErrNone )
		{
		INFO_PRINTF3(_L("Test Failed after Crop operation in preamble: (iError, errorCode)(%X, %X)"),iError, errorCode );
		verdict = EInconclusive;
		}

	return verdict;

	}


/**
 *
 * Static constructor for CTestStepRecorderRecordAu.
 *
 *
 * @return	"CTestStepRecorderRecordRaw*"
 *			The constructed CTestStepRecorderRecordRaw
 *
 * @xxxx
 *
 */
CTestStepRecorderRecordRaw* CTestStepRecorderRecordRaw::NewL()
	{
	CTestStepRecorderRecordRaw* self = new(ELeave) CTestStepRecorderRecordRaw;
	return self;
	}


/**
*
* CTestStepRecorderRecordWavGsm
*/

CTestStepRecorderRecordWavGsm::CTestStepRecorderRecordWavGsm() 
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0275-HP");
	}

/**
 *
 * Static constructor for CTestStepRecorderRecordWavGsm.
 *
 *
 * @return	"CTestStepRecorderRecordWavGsm*"
 *			The constructed CTestStepRecorderRecordWavGsm
 *
 * @xxxx
 * 
 */
CTestStepRecorderRecordWavGsm* CTestStepRecorderRecordWavGsm::NewL()
	{
	CTestStepRecorderRecordWavGsm* self = new(ELeave) CTestStepRecorderRecordWavGsm();
	return self;
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderRecordWavGsm::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0275-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::RecordL with non existent wav Gsm file"));

	// set the file name to something weird
    iFileName.Set( _L("c:\\asmeraGSM610.wav"));
	
	//[ ensure the file does not exist]
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		{
		verdict = EInconclusive;
		}
	
	TInt result = SetAudRecorderParent(iRecorder, REINTERPRET_CAST(CMdaAudioRecorderUtility*, KTestPointerValue));
	
	if (result != KErrNone)
		{
		INFO_PRINTF2(_L("SetAudRecorderParent returned %d"), result);
		return EFail; // check value in MoscoStateChangeEvent
		}


	// open an existing file to record and crop all its contents if any
	TRAPD( errorCode, iRecorder->OpenFileL(iFileName,
						 				   TUid::Uid(KMmfUidControllerAudio),
										   TUid::Null(),
										   TUid::Uid(KMmfUidFormatWAVWrite),
										   TFourCC(KMMFFourCCCodeGSM610)) );
	CActiveScheduler::Start(); 
	
	if (iError)
		{
		INFO_PRINTF2(_L("open returned %d"), iError);
		return EFail;
		}

	if (iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()/2);
	iBeginning = new TTimeIntervalMicroSeconds(0);
	iRecorder->SetPosition(*iBeginning);	
	
	if ( errorCode != KErrNone || iError != KErrNone || iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		{
	    INFO_PRINTF3(_L("Test Failed after open operation in preamble: (iError, errorCode) (%d, %d)"),iError, errorCode );
		verdict = EInconclusive;
		}
		
	return verdict;

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
TVerdict CTestStepRecorderRecordWavGsm::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
	TInt count;
	
	for (count = 0; count < 3; count++)
		{
		iHasRecorded = EFalse;
		
		TRAP( err, iRecorder->RecordL() );

		User::After(KTestRecordTime);
	
		if (iRecorder->State() == CMdaAudioRecorderUtility::ERecording)
			{
			iHasRecorded = ETrue;
			}
			
		iRecorder->Stop();
		CActiveScheduler::Start();
		}
	
	TInt recordedDuration;
	TInt expectedDuration;
	
	recordedDuration = iRecorder->Duration().Int64() + (KTestRecordTimeTolerance * count);
	expectedDuration = KTestRecordTime * count;
	
	if ( iHasRecorded && (recordedDuration < expectedDuration) )
		{
		INFO_PRINTF4(_L("Test failed Recorded: %d and Expected: %d duration mismatch. iHasRecorded %d"),
					     recordedDuration, expectedDuration, iHasRecorded);
		return EFail;
		}

	// check result
	if (iError != KErrNone || err != KErrNone || !iHasRecorded ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
		INFO_PRINTF3(_L("Test Failed to record: (iError, errorCode) (%d, %d)"),iError, err);
		return EFail;
		}

	iHasRecorded = EFalse;
	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


/**
*
* CTestStepRecorderRecordWavGsmFail
*/

CTestStepRecorderRecordWavGsmFail::CTestStepRecorderRecordWavGsmFail() 
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0276-HP");
	}

/**
 *
 * Static constructor for CTestStepRecorderRecordWavGsmFail.
 *
 *
 * @return	"CTestStepRecorderRecordWavGsmFail*"
 *			The constructed CTestStepRecorderRecordWavGsmFail
 *
 * @xxxx
 * 
 */
CTestStepRecorderRecordWavGsmFail* CTestStepRecorderRecordWavGsmFail::NewL()
	{
	CTestStepRecorderRecordWavGsmFail* self = new(ELeave) CTestStepRecorderRecordWavGsmFail();
	return self;
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderRecordWavGsmFail::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0276-HP"));
	INFO_PRINTF1(_L("This is a test to append to a corrupted wav Gsm file"));

	// set the file name
    _LIT(KSoundFile, "c:\\mm\\mmf\\testfiles\\aclnt\\corruptedGsm610Mono8khz.wav");

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		{
		verdict = EInconclusive;
		}
	if (SetAudRecorderParent(iRecorder, REINTERPRET_CAST(CMdaAudioRecorderUtility*, KTestPointerValue)) != KErrNone)
		return EFail; // check value in MoscoStateChangeEvent

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(KSoundFile);
	CActiveScheduler::Start();
	
	if (iError)
		{
		INFO_PRINTF2(_L("open returned %d"), iError);
		return EFail;
		}	

	if (iError != KErrNone || iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
		return EInconclusive;
		}

	TRAPD(errorCode, iRecorder->PlayL());
	CActiveScheduler::Start();					// state change call back
	
	if (errorCode != KErrNone || iError == KErrCorrupt || (iRecorder->State() != CMdaAudioRecorderUtility::EPlaying) )
		{
		INFO_PRINTF4(_L("Test Failed attempting to paly: (iError, errorCode, State) (%d, %d, %d)"),
					iError, errorCode, iRecorder->State() );		
		return EFail;	
		}
		
	CActiveScheduler::Start();					// PlayL()
	iRecorder->Stop();
	
	
	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(KSoundFile,
						 TUid::Uid(KMmfUidControllerAudio),
						 TUid::Null(),
						 TUid::Uid(KMmfUidFormatWAVWrite),
						 TFourCC(KMMFFourCCCodeGSM610) );
	CActiveScheduler::Start();
	
	if (iError)
		{
		INFO_PRINTF2(_L("open returned %d"), iError);
		return EFail;
		}
	
	if ( errorCode != KErrNone || iError != KErrNone || iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
	    INFO_PRINTF3(_L("Test Failed in CTestStepRecorderRecordWavGsmFail::DoTestStepPreambleL: (iError, errorCode) (%d, %d)"),iError, errorCode );
		verdict = EInconclusive;
		}
		
	return verdict;

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
TVerdict CTestStepRecorderRecordWavGsmFail::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	
	TRAP( err, iRecorder->RecordL() );
	CActiveScheduler::Start();

	iRecorder->Stop();
	
	if (iError == KErrCorrupt)
	{
		iError = KErrNone;
		iTestStepResult = EPass;	
	}

	// check result
	if (iError != KErrNone ||
		err != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iHasRecorded = EFalse;
	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Static constructor for CTestStepRecorderRecordPause.
 *
 *
 * @return	"CTestStepRecorderRecordPause*"
 *			The constructed CTestStepRecorderRecordPause
 *
 * @xxxx
 *
 */
CTestStepRecorderRecordPause* CTestStepRecorderRecordPause::NewL(const TDesC& aTestName)
	{
	CTestStepRecorderRecordPause* self = new(ELeave) CTestStepRecorderRecordPause(aTestName);
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
CTestStepRecorderRecordPause::CTestStepRecorderRecordPause(const TDesC& aTestName) : iError(0), iHasRecorded(EFalse)
	{
	// store the name of this test case

	// this is the name that is used by the script file
	//iTestStepName = _L("MM-MMF-ACLNT-U-0210-CP");
	//iTestStepName = _L("MM-MMF-ACLNT-U-0211-CP");
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderRecordPause::~CTestStepRecorderRecordPause()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderRecordPause::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::RecordL"));
	
	_LIT(K210Test,"MM-MMF-ACLNT-U-0210-CP");
	_LIT(K211Test,"MM-MMF-ACLNT-U-0211-CP");    //  Test step added for DEF140670
	
	if(iTestStepName == K210Test)
	    {
	    if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord"), iFileName))
            {
            //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
            return EInconclusive;
            }
	    }
	else if(iTestStepName == K211Test)
	    {
       if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord1"), iFileName))
            {
            return EInconclusive;
            }
	    }	
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord2"), iFileName2))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord3"), iFileName3))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	if (SetAudRecorderParent(iRecorder, REINTERPRET_CAST(CMdaAudioRecorderUtility*, KTestPointerValue)) != KErrNone)
		return EFail; // check value in MoscoStateChangeEvent

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iError)
		{
		INFO_PRINTF2(_L("open returned %d"), iError);
		return EFail;
		}

	if (iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()/2);
	iBeginning = new TTimeIntervalMicroSeconds(0);
	iRecorder->SetPosition(*iBeginning);
	iRecorder->CropL();

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
enum TVerdict CTestStepRecorderRecordPause::DoTestStepPostambleL(void)
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
void CTestStepRecorderRecordPause::MoscoStateChangeEvent(CBase* aObject,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorCode)
	{
	if (aObject != REINTERPRET_CAST(CBase*, KTestPointerValue))
		iError = KErrArgument;
	else
		iError = aErrorCode;
	CActiveScheduler::Stop();
	}


// this can return a TBool hasPlayed to prove that it has played
// but the EPlaying state seems to get preserved for some reason...
void CTestStepRecorderRecordPause::RecordAndStartSchedulerL()
	{
	iRecorder->RecordL();
	CActiveScheduler::Start();
	// record 2 secs, ...probably silence
	User::After(10000000);

	// Stop the recording
	iRecorder->Stop();
	User::After(2000000);
	iRecorder->RecordL();

	User::After(10000000);


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
TVerdict CTestStepRecorderRecordPause::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	TRAP( err, RecordAndStartSchedulerL() );

	// check result
	if (iError != KErrNone ||
		err != KErrNone ||
		!iHasRecorded ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iHasRecorded = EFalse;
	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;

	/*
	// record AU file //
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName2);
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()*.75);
	iRecorder->SetPosition(*iBeginning);
	iRecorder->CropL();

	if ( iError != KErrNone ||
		 iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	TRAP( err, RecordAndStartSchedulerL() );

	// check result
	if (iError != KErrNone ||
		err != KErrNone ||
		!iHasRecorded ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iHasRecorded = EFalse;
	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;

	// record RAW file //
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName3);
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iRecorder->SetGain(iRecorder->MaxGain()*.75);
	iRecorder->SetPosition(*iBeginning);
	iRecorder->CropL();

	if ( iError != KErrNone ||
		 iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	TRAP( err, RecordAndStartSchedulerL() );

	// check result
	if (iError != KErrNone ||
		err != KErrNone ||
		!iHasRecorded ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iHasRecorded = EFalse;
	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;
	*/

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Static constructor for CTestStepRecorderError.
 *
 *
 * @return	"CTestStepRecorderError*"
 *			The constructed CTestStepRecorderError
 *
 * @xxxx
 *
 */
CTestStepRecorderError* CTestStepRecorderError::NewL()
	{
	CTestStepRecorderError* self = new(ELeave) CTestStepRecorderError;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

/**
 *
 * Static constructor for CTestStepRecorderError.
 *
 *
 * @return	"CTestStepRecorderError*"
 *			The constructed CTestStepRecorderError
 *
 * @xxxx
 *
 */
void CTestStepRecorderError::ConstructL()
	{
     //[ lets initialise the dispatch table
	 //with a default method for now ]
	ASSERT( TestStateTerminal < KMaxMethods );

	 iStates[TestStateIdle]		= &CTestStepRecorderError::IdleStateChangeEventL;
	 iStates[TestStateOne]		= &CTestStepRecorderError::StateOneChangeEventL;
	 iStates[TestStateTwo]		= &CTestStepRecorderError::StateTwoChangeEventL;
	 iStates[TestStateThree]	= &CTestStepRecorderError::StateThreeChangeEventL;
	 iStates[TestStateFour]		= &CTestStepRecorderError::StateFourChangeEventL;
	 iStates[TestStateTerminal]	= &CTestStepRecorderError::StateTerminalChangeEventL;

	 //[ set the initial state to idle ]
	 iTestState = TestStateIdle;
	}

/**
 * Enters here as iRecorder goes from ENotReady to EOpen
 *
 * Set the priority of iRecorder to EMdaPriorityMin
 * Set the parent of iRecorderHighPriority to be KTestPointerRecorder2 so we can distinguish state changes
 *
 * Next state TestStateOne
 */
void CTestStepRecorderError::IdleStateChangeEventL(CBase* aObject,
												  TInt aPreviousState,
												  TInt aCurrentState,
												  TInt aErrorCode )
	{
	//[ precondition the state is valid ]
	ASSERT(IsValidState() );

	//This is state change from iRecorder
	if(REINTERPRET_CAST(void*,aObject) != REINTERPRET_CAST(void*,KTestPointerRecorder1))
		{
		INFO_PRINTF2( _L("IdleStateChangeEventL error from wrong object 0x%x"), aObject);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}

	//Correct state change
	if((aPreviousState != CMdaAudioClipUtility::ENotReady) ||
		(aCurrentState != CMdaAudioClipUtility::EOpen))
		{
		INFO_PRINTF1( _L("IdleStateChangeEventL incorrect state change"));
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}


	//[fail if there was an error]
	if(aErrorCode != KErrNone)
		{
		INFO_PRINTF2( _L("IdleStateChangeEventL error %d"),aErrorCode);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioRecord2"), iFileName2))
		{
		INFO_PRINTF1( _L("IdleStateChangeEventL couldn't get filename from config  AudioRecord2"));
		iTestStepResult = EInconclusive;
		CActiveScheduler::Stop();
		}


	if (SetAudRecorderParent(iRecorderHighPriority, REINTERPRET_CAST(CMdaAudioRecorderUtility*,KTestPointerRecorder2 )) != KErrNone)
		{
		INFO_PRINTF1( _L("IdleStateChangeEventL failed to set parent"));
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}


	//[ open the second recorder and update state ]
	// open an existing file to record and crop all its contents if any
	iRecorderHighPriority->OpenFileL(iFileName2);

	//[lets do some configuration on the first recorder]
	iRecorder->SetGain(iRecorder->MaxGain()/2);
	iRecorder->SetPriority( EMdaPriorityMin, EMdaPriorityPreferenceNone );
	iRecorder->SetPosition(TTimeIntervalMicroSeconds(0));

	//[ update the state ]
	TestState newState = TestStateOne;
	SetState( newState );

	//[ post condition state is valid ]
	ASSERT(IsValidState() );
	}

/**
 * Enters here as iRecorderHighPriority goes from ENotReady to EOpen
 *
 * Set the priority of iRecorderHighPriority to EMdaPriorityMax
 * Start recording to iRecorder
 *
 * Next state TestStateTwo
 *
 */
void CTestStepRecorderError::StateOneChangeEventL(CBase* aObject,
												  TInt aPreviousState,
												  TInt aCurrentState,
												  TInt aErrorCode )
	{
	//[ precondition state is valid ]
	ASSERT( IsValidState() );

	//[fail if there was an error]
	if(aErrorCode != KErrNone)
		{
		INFO_PRINTF2( _L("StateOneChangeEventL error %d"),aErrorCode);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}


	//This is state change from iRecorderHighPriority
	if(REINTERPRET_CAST(void*,aObject) != REINTERPRET_CAST(void*,KTestPointerRecorder2))
		{
		INFO_PRINTF2( _L("StateOneChangeEventL error from wrong object 0x%x"), aObject);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}

	//Correct state change
	if((aPreviousState != CMdaAudioClipUtility::ENotReady) ||
		(aCurrentState != CMdaAudioClipUtility::EOpen))
		{
		INFO_PRINTF1( _L("StateOneChangeEventL incorrect state change"));
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}




	//[ start recording with recorder 1]
	iRecorder->RecordL();
	User::After(2500000);

	//[lets do some configuration on the first recorder]
	iRecorderHighPriority->SetGain(iRecorder->MaxGain()/2);
	iRecorderHighPriority->SetPriority( EMdaPriorityMax, EMdaPriorityPreferenceNone );
	iRecorderHighPriority->SetPosition(TTimeIntervalMicroSeconds(0));

	TestState newState = TestStateTwo;
	//[ update the state ]
	SetState(newState);

	//[ post condition state is valid ]
	ASSERT( IsValidState() );
	}

/**
 *
 * Enters here as iRecorder goes from EOpen to ERecording
 *
 * Start recording to iRecorderHighPriority
 *
 * Next state TestStateThree
 *
 */
void CTestStepRecorderError::StateTwoChangeEventL(CBase* aObject,
												  TInt aPreviousState,
												  TInt aCurrentState,
												  TInt aErrorCode )
	{
	//[ precondition state is valid ]
	ASSERT( IsValidState() );

	//[fail if there was an error]
	if(aErrorCode != KErrNone)
		{
		INFO_PRINTF2( _L("StateTwoChangeEventL error %d"),aErrorCode);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}

	//This is state change from iRecorder
	if(REINTERPRET_CAST(void*,aObject) != REINTERPRET_CAST(void*,KTestPointerRecorder1))
		{
		INFO_PRINTF2( _L("StateTwoChangeEventL error from wrong object 0x%x"), aObject);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}

	//Correct state change
	if((aPreviousState != CMdaAudioClipUtility::EOpen) ||
		(aCurrentState != CMdaAudioClipUtility::ERecording))
		{
		INFO_PRINTF1( _L("StateTwoChangeEventL incorrect state change"));
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}



	//[ start recording with recorder 1]
	iRecorderHighPriority->RecordL();

	//[ update state ]
	TestState newState = TestStateThree;
	SetState(newState);

	//[ post condition state is valid ]
	ASSERT( IsValidState() );
	}

/**
 * Enters here because of two reasons:-
 *
 * 1. iRecorder goes from ERecording to EOpen as it has been thrown off by
 *    iRecorderHighPriority starting to record
 *
 *    aErrorCode == KErrInUse
 *
 * 2. iRecorderHighPriority goes from EOpen to ERecording
 *    aErrorCode == KErrNone
 *
 *
 * Next state TestStateTerminal
 *
 */
void CTestStepRecorderError::StateThreeChangeEventL(CBase* aObject,
												  TInt aPreviousState,
												  TInt aCurrentState,
												  TInt aErrorCode )
	{
     //[does nothing for now ]
	 //[ precondition state is valid ]
	 ASSERT( IsValidState() );


	 //This is a state change from iRecorder
	if(REINTERPRET_CAST(void*,aObject) == REINTERPRET_CAST(void*,KTestPointerRecorder1))
		{
		 if( aErrorCode != KErrInUse )
			{
 			INFO_PRINTF2( _L("StateThreeChangeEventL incorrect error received %d"), aErrorCode);
			iTestStepResult = EFail;
			CActiveScheduler::Stop();
			}

		//Correct state change
		if((aPreviousState != CMdaAudioClipUtility::ERecording) ||
			(aCurrentState != CMdaAudioClipUtility::EOpen))
			{
			INFO_PRINTF1( _L("StateThreeChangeEventL incorrect state change"));
			iTestStepResult = EFail;
			CActiveScheduler::Stop();
			}
		}
	//This is a state change from iRecorderHighPriority
	else if(REINTERPRET_CAST(void*,aObject) == REINTERPRET_CAST(void*,KTestPointerRecorder2))
		{
		//Correct state change
		if((aPreviousState != CMdaAudioClipUtility::EOpen) ||
			(aCurrentState != CMdaAudioClipUtility::ERecording))
			{
			INFO_PRINTF1( _L("StateThreeChangeEventL incorrect state change"));
			iTestStepResult = EFail;
			CActiveScheduler::Stop();
			}
		}
	else
		{
		INFO_PRINTF1( _L("StateThreeChangeEventL state change from unknown object"));
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}


	 //[Set state]
	 TestState newState = TestStateTerminal;
	 SetState(newState);

	//[ post condition state is valid ]
	ASSERT( IsValidState() );
	}

/**
 * Un used state
 *
 * @xxxx
 *
 */
void CTestStepRecorderError::StateFourChangeEventL(CBase* /*aObject*/,
												  TInt /*aPreviousState*/,
												  TInt /*aCurrentState*/,
												  TInt /*aErrorCode*/ )
	{
    //[ precondition state is valid ]
	ASSERT( IsValidState() );


	//[Update next State]
	TestState newState = TestStateTerminal;
	SetState(newState);

	//[ post condition state is valid ]
	ASSERT( IsValidState() );
	}


/**
 * Enters here because of two reasons:-
 *
 * 1. iRecorder goes from ERecording to EOpen as it has been thrown off by
 *    iRecorderHighPriority starting to record
 *
 *    aErrorCode == KErrInUse
 *
 * 2. iRecorderHighPriority goes from EOpen to ERecording
 *    aErrorCode == KErrNone
 *
 * Which one is the other to that which occured in StateThreeChangeEventL
 * as there is a possible race condition on which event we receive first
 *
 */
void CTestStepRecorderError::StateTerminalChangeEventL(CBase* aObject,
												  TInt aPreviousState,
												  TInt aCurrentState,
												  TInt aErrorCode )
	{
	 //[ pre condition state is valid ]
	 ASSERT( IsValidState() );

	 //This is a state change from iRecorder
	if(REINTERPRET_CAST(void*,aObject) == REINTERPRET_CAST(void*,KTestPointerRecorder1))
		{
		 if( aErrorCode != KErrInUse )
			{
 			INFO_PRINTF2( _L("StateTerminalChangeEventL incorrect error received %d"), aErrorCode);
			iTestStepResult = EFail;
			CActiveScheduler::Stop();
			}

		//Correct state change
		if((aPreviousState != CMdaAudioClipUtility::ERecording) ||
			(aCurrentState != CMdaAudioClipUtility::EOpen))
			{
			INFO_PRINTF1( _L("StateTerminalChangeEventL incorrect state change"));
			iTestStepResult = EFail;
			CActiveScheduler::Stop();
			}
		}
	//This is a state change from iRecorderHighPriority
	else if(REINTERPRET_CAST(void*,aObject) == REINTERPRET_CAST(void*,KTestPointerRecorder2))
		{
		//Correct state change
		if((aPreviousState != CMdaAudioClipUtility::EOpen) ||
			(aCurrentState != CMdaAudioClipUtility::ERecording))
			{
			INFO_PRINTF1( _L("StateTerminalChangeEventL incorrect state change"));
			iTestStepResult = EFail;
			CActiveScheduler::Stop();
			}
		}
	else
		{
		INFO_PRINTF1( _L("StateTerminalChangeEventL state change from unknown object"));
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}

	//[ stop recording with recorder 1]
	INFO_PRINTF1( _L("Stopping iRecorderHighPriority"));

	iRecorderHighPriority->Stop();

	INFO_PRINTF1( _L("iRecorderHighPriority stopped"));


	CActiveScheduler::Stop();

	 //[ post condition state is valid ]
	 ASSERT( IsValidState() );
	}


/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 *
 */
void CTestStepRecorderError::PrintState(CBase* aObject,
										TInt aPreviousState,
										TInt aCurrentState,
										TInt aErrorCode )
{
 INFO_PRINTF6( _L("Object %x aPrevState %d CurrentState %d, ErrorCode %d FsmState %d"), aObject, aPreviousState, aCurrentState, aErrorCode, iTestState );
}


/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 *
 */
TBool CTestStepRecorderError::IsValidState() const
	{
	// provide implementation here
    TBool result = ETrue;
    if( (iTestState < TestStateIdle ) || ( iTestState > TestStateTerminal ) )
		{
         result = EFalse;
		}
	return result;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 *
 */
 CTestStepRecorderError::TestState CTestStepRecorderError::GetTestState() const
	{
	 return iTestState;
	}

 /**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 *
 */
void CTestStepRecorderError::SetState( CTestStepRecorderError::TestState& aState )
	{
	iTestState = aState ;
	}


/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 *
 */
CTestStepRecorderError::CTestStepRecorderError() : iError(0), iHasRecorded(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0014-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderError::~CTestStepRecorderError()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderError::DoTestStepPreambleL(void)
	{

	enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderError::DoTestStepPostambleL(void)
	{
	//delete the output file
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	delete iRecorder;
	iRecorder = NULL;

	delete iRecorderHighPriority;
	iRecorderHighPriority = NULL;

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
void CTestStepRecorderError::MoscoStateChangeEvent(CBase* aObject,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	//[ precondition state is valid ]
	ASSERT( IsValidState());

	//[ print incoming state ]
	PrintState( aObject, aPreviousState, aCurrentState, aErrorcCode );

	INFO_PRINTF2(_L("Dispatching into state %d"), iTestState);


	//dispatch the reponse
    (this->*iStates[iTestState])( aObject, aPreviousState, aCurrentState, aErrorcCode );

	//[ post condition state is valid ]
	ASSERT( IsValidState());
	}

/**
*
* IsValidTestState()
*
*/


// this can return a TBool hasPlayed to prove that it has played
// but the EPlaying state seems to get preserved for some reason...
void CTestStepRecorderError::RecordAndStartSchedulerL()
	{
	//[ This method has been deprecated ]
     ASSERT(0);
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

TInt aFunction(TAny* /*aPtr*/)
{
 return 0;
}

TVerdict CTestStepRecorderError::DoTestStepL()
	{
	//[temporary contruction to test the fsm ]
	iTestStepResult = EPass;

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0014-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility on error"));

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	if ( (iRecorderHighPriority = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return  EInconclusive;

	//[ copy files from back up file ]
	//[ ensure files do not exist ]
	iFs.Connect();
	CFileMan* fileMan = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan);

    fileMan->Delete( KTestRecordTwo );
	fileMan->Delete( KTestRecordOne );
	User::LeaveIfError(fileMan->Copy( KTestRecordThree, KTestRecordTwo ));
	User::LeaveIfError(fileMan->Copy( KTestRecordThree, KTestRecordOne ));
	CleanupStack::PopAndDestroy(fileMan);

	//set up file name

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioRecord1"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if (SetAudRecorderParent(iRecorder, REINTERPRET_CAST(CMdaAudioRecorderUtility*,KTestPointerRecorder1 )) != KErrNone)
		return EInconclusive;; // check value in MoscoStateChangeEvent

	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();

	//[ if we got here we are ok ]

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


/**
 *
 * Static constructor for CTestStepRecorderMaxWriteLength.
 *
 *
 * @return	"CTestStepRecorderMaxWriteLength*"
 *			The constructed CTestStepRecorderMaxWriteLength
 *
 * @xxxx
 *
 */
CTestStepRecorderMaxWriteLength* CTestStepRecorderMaxWriteLength::NewL()
	{
	CTestStepRecorderMaxWriteLength* self = new(ELeave) CTestStepRecorderMaxWriteLength;
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
CTestStepRecorderMaxWriteLength::CTestStepRecorderMaxWriteLength() : iError(0)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0290-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderMaxWriteLength::~CTestStepRecorderMaxWriteLength()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderMaxWriteLength::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0290-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::SetMaxWriteLength with argument = -1"));

	// set the file name to something weird
    iFileName.Set( _L("c:\\szhgefdsfe.wav"));

	//[ ensure the file does not exist]
	
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	if (SetAudRecorderParent(iRecorder, REINTERPRET_CAST(CMdaAudioRecorderUtility*, KTestPointerValue)) != KErrNone)
		return EFail; // check value in MoscoStateChangeEvent

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iError)
		{
		INFO_PRINTF2(_L("open returned %d"), iError);
		return EFail;
		}

	if (iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	// if it gets here "-1" is accepted
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderMaxWriteLength::DoTestStepPostambleL(void)
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
void CTestStepRecorderMaxWriteLength::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt /*aErrorCode*/)
	{
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
TVerdict CTestStepRecorderMaxWriteLength::DoTestStepL()
	{
	iTestStepResult = EPass;

	// set some limit on bytes to record
	iRecorder->SetMaxWriteLength(10024);
	
	// reset the record limit. 
	// This is the function WHICH is being tested
	iRecorder->SetMaxWriteLength(-1); 
		
	iRecorder->Close();
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	_LIT(KRecorderThread1,"RecorderThread1");
	_LIT(KRecorderThread2,"RecorderThread2");
	_LIT(KRecorderThread3,"RecorderThread3");
	const TInt KDefaultHeapSize=0x100000;
#endif	

// factory function
CRecordAudio* CRecordAudio::NewL()
	{
	CRecordAudio* self = new (ELeave) CRecordAudio();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Destructor
CRecordAudio::~CRecordAudio()
	{
	if (iUtility)
		{
		// if it exists, always stop the utility - null action if not running
		iUtility->Stop();
		delete iUtility;
		}
	}

// 2nd phase constructor
void CRecordAudio::ConstructL()
	{
	iUtility = CMdaAudioRecorderUtility::NewL(*this);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}
	
// constructor
CRecordAudio::CRecordAudio()
	{
	}
	
void CRecordAudio::RecordL(const TDesC& aFileName, TInt aPriority)
	{
	// Note: in this scenario, the calling code can handle the leave here
	// but alternatively we would have to trap and (using one-shot AO) arrange
	// for the callback to be subsequently called.
	iUtility->SetPriority(aPriority, EMdaPriorityPreferenceNone);
	iUtility->OpenFileL(aFileName); 
	}
	
void CRecordAudio::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	__ASSERT_DEBUG(aObject==iUtility, User::Invariant());
	aObject = aObject; // shutup warning
	
	iError = aErrorCode;
	
	if (iError==KErrNone)
		{
		// only interested in some state transitions...
		if (aPreviousState==CMdaAudioRecorderUtility::ENotReady && 
		        aCurrentState==CMdaAudioRecorderUtility::EOpen)
			{
			// have opened the file
			iUtility->RecordL();
			}
		else if (aPreviousState==CMdaAudioRecorderUtility::EOpen && 
		         aCurrentState==CMdaAudioRecorderUtility::ERecording)
			{
			//start a timer
			TCallBack callback (TimerCallback, this);
			const TInt KWaitTime = 2000000; // 2s
			iTimer->Start(KWaitTime, 0, callback);
			}
		}
	else
		{
		if (aPreviousState==CMdaAudioRecorderUtility::ERecording && 
		    aCurrentState==CMdaAudioRecorderUtility::EOpen)
			{
			iTimer->Cancel();
			CActiveScheduler::Stop();
			}
		}
	}

// having opened the file, give settings and then start to record
void CRecordAudio::OnOpenL()
	{
	iUtility->RecordL();
	}

// timer callback
TInt CRecordAudio::TimerCallback(TAny* aPtr)
	{
	static_cast<CRecordAudio*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

void CRecordAudio::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iUtility->Stop();
	CActiveScheduler::Stop();
	}
	
RAudioConcurrentRecTest* RAudioConcurrentRecTest::NewL(const TDesC& aStepName, TInt aRecPriority1, TInt aRecPriority2, TInt aRecPriority3)
	{
	RAudioConcurrentRecTest* self = new (ELeave) RAudioConcurrentRecTest(aStepName, aRecPriority1, aRecPriority2, aRecPriority3);
	return self;
	}
	
RAudioConcurrentRecTest::RAudioConcurrentRecTest(const TDesC& aStepName, TInt aRecPriority1, TInt aRecPriority2, TInt aRecPriority3)
	:iRecPriority1(aRecPriority1), iRecPriority2(aRecPriority2), iRecPriority3(aRecPriority3) 
	{
	iTestStepName = aStepName; 
	}

TInt StartThreadL(TAny* aData)
	{
	TInt* priority = static_cast<TInt*> (aData);
	// Create active scheduler (to run active objects)
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
	CRecordAudio* recorder = CRecordAudio::NewL();
	CleanupStack::PushL(recorder);
	
	TBuf<50> fileName;
	fileName.Append(_L("c:\\"));
	fileName.Append(RThread().Name());
	fileName.Append(_L(".wav"));
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	fs.Delete(fileName); // ensure we start with no file. Ignore errors.
		
	recorder->RecordL(fileName, *priority);
	CActiveScheduler::Start();
	fs.Delete(fileName); // ensure we start with no file. Ignore errors.
	TInt err = recorder->Error();
	CleanupStack::PopAndDestroy(3,scheduler);
	return err;
	}
	
TInt StartThread(TAny* aData)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();	
	TInt result=KErrNone;
	TRAPD(error, result = StartThreadL(aData));
	delete cleanup;
	if(error)
		{
		return error;
		}
	else if(result)
		{
		return result;
		}
	else
		{
		return KErrNone;
		}	
	}

// start test
void RAudioConcurrentRecTest::KickoffTestL()
	{
	INFO_PRINTF1( _L("DEF121928:This test creates three threads and starts recording concurrently from all the three threads"));
	INFO_PRINTF4( _L("RecorderUtility1 has Priority[%d] RecorderUtility2 has Priority[%d] RecorderUtility3 has Priority[%d]"), iRecPriority1, iRecPriority2, iRecPriority3);
	
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		INFO_PRINTF1( _L("This test is only for A3F: Just making it pass without testing on old devsound"));
		StopTest(KErrNone);
	#else
		User::LeaveIfError(iThread1.Create(KRecorderThread1, StartThread, KDefaultStackSize, KDefaultHeapSize, KDefaultHeapSize, &iRecPriority1));
	    User::LeaveIfError(iThread2.Create(KRecorderThread2, StartThread, KDefaultStackSize, KDefaultHeapSize, KDefaultHeapSize, &iRecPriority2));
	    User::LeaveIfError(iThread3.Create(KRecorderThread3, StartThread, KDefaultStackSize, KDefaultHeapSize, KDefaultHeapSize, &iRecPriority3));
		
		TRequestStatus status1;
	    TRequestStatus status2;
	    TRequestStatus status3;
	    iThread1.Logon(status1);
	    iThread2.Logon(status2);
	    iThread3.Logon(status3);
	    iThread1.Resume();
	    iThread2.Resume();
	    iThread3.Resume();
		User::WaitForRequest(status1);
	    User::WaitForRequest(status2);
	    User::WaitForRequest(status3);
	    
	    INFO_PRINTF2( _L("Thread1 is terminated with error[%d]"), status1.Int());
	    INFO_PRINTF2( _L("Thread2 is terminated with error[%d]"), status2.Int());
	    INFO_PRINTF2( _L("Thread3 is terminated with error[%d]"), status3.Int());
	    
	    TVerdict result = EPass;
	    //Check first that the thread with the max priority is successful
	   	if(status1.Int() != KErrNone && iRecPriority1 == EMdaPriorityMax)
	   		{
	   		INFO_PRINTF1( _L("Thread1 has failed to record inspite of having EMdaPriorityMax"));
	   		result = EFail;
	   		}
	   	if(status2.Int() != KErrNone && iRecPriority2 == EMdaPriorityMax)
	   		{
	   		INFO_PRINTF1( _L("Thread2 has failed to record inspite of having EMdaPriorityMax"));
	   		result = EFail;
	   		}
	   	if(status3.Int() != KErrNone && iRecPriority3 == EMdaPriorityMax)
	   		{
	   		INFO_PRINTF1( _L("Thread3 has failed to record inspite of having EMdaPriorityMax"));
	   		result = EFail;
	   		}
	   	//Check if the lower priority gets the KErrInUse
	   	if(status1.Int()!= KErrInUse && (iRecPriority1== EMdaPriorityMin || iRecPriority1== EMdaPriorityNormal))
			{
			INFO_PRINTF1( _L("Thread1 has failed to receive KErrInUse inspite of having EMdaPriorityMin or EMdaPriorityNormal"));
	   		result = EFail;
			}
		if(status2.Int()!= KErrInUse && (iRecPriority2== EMdaPriorityMin || iRecPriority2== EMdaPriorityNormal))
			{
			INFO_PRINTF1( _L("Thread2 has failed to receive KErrInUse inspite of having EMdaPriorityMin or EMdaPriorityNormal"));
	   		result = EFail;
			}
		if(status3.Int()!= KErrInUse && (iRecPriority3== EMdaPriorityMin || iRecPriority3== EMdaPriorityNormal))
			{
			INFO_PRINTF1( _L("Thread1 has failed to receive KErrInUse inspite of having EMdaPriorityMin or EMdaPriorityNormal"));
	   		result = EFail;
			}
		StopTest(KErrNone, result);
	#endif
	}
	
// cleanup at end
void RAudioConcurrentRecTest::CloseTest()
	{
	iThread1.Close();
    iThread2.Close();
    iThread3.Close();
	}

// factory function
CRecorder* CRecorder::NewL(MRecordAudioObserver* aObserver)
	{
	CRecorder* self = new (ELeave) CRecorder(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Destructor
CRecorder::~CRecorder()
	{
	if (iUtility)
		{
		// if it exists, always stop the utility - null action if not running
		iUtility->Stop();
		delete iUtility;
		}
	}

// 2nd phase constructor
void CRecorder::ConstructL()
	{
	iUtility = CMdaAudioRecorderUtility::NewL(*this);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}
	
// constructor
CRecorder::CRecorder(MRecordAudioObserver* aObserver)
	:iObserver(aObserver)
	{
	}
	
void CRecorder::RecordL(const TDesC& aFileName, TInt aPriority)
	{
	iUtility->SetPriority(aPriority, EMdaPriorityPreferenceNone);
	const TUid KPanicControllerUid = {0x10286661};
	iUtility->OpenFileL(aFileName, KPanicControllerUid);
	}
	
void CRecorder::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	__ASSERT_DEBUG(aObject==iUtility, User::Invariant());
	aObject = aObject; // shutup warning
	
	iError = aErrorCode;
	
	if (iError==KErrNone)
		{
		// only interested in some state transitions...
		if (aPreviousState==CMdaAudioRecorderUtility::ENotReady && 
		        aCurrentState==CMdaAudioRecorderUtility::EOpen)
			{
			// have opened the file
			iUtility->RecordL();
			}
		else if (aPreviousState==CMdaAudioRecorderUtility::EOpen && 
		         aCurrentState==CMdaAudioRecorderUtility::ERecording)
			{
			//start a timer
			TCallBack callback (TimerCallback, this);
			const TInt KWaitTime = 2000000; // 2s
			iTimer->Start(KWaitTime, 0, callback);
			}
		}
	else
		{
		if (aPreviousState==CMdaAudioRecorderUtility::ERecording && 
		    aCurrentState==CMdaAudioRecorderUtility::EOpen)
			{
			iTimer->Cancel();
			}
		iObserver->RecordError(iError);
		}
	}

// having opened the file, give settings and then start to record
void CRecorder::OnOpenL()
	{
	iUtility->RecordL();
	}

// timer callback
TInt CRecorder::TimerCallback(TAny* aPtr)
	{
	static_cast<CRecorder*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

void CRecorder::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iUtility->Stop();
	iObserver->RecordError(KErrNone);
	}

RAudioRecPanicTest* RAudioRecPanicTest::NewL(const TDesC& aStepName, const TDesC& aFileName)
	{
	RAudioRecPanicTest* self = new (ELeave) RAudioRecPanicTest(aStepName, aFileName);
	return self;
	}
	
RAudioRecPanicTest::RAudioRecPanicTest(const TDesC& aStepName, const TDesC& aFileName) :
	iRecorder(NULL),
	iFileName (aFileName)
	{
	iTestStepName = aStepName;
	}
	
// start test
void RAudioRecPanicTest::KickoffTestL()
	{
	INFO_PRINTF1(_L("DEF127630:Fix for INC108007 appears to close "));
	// re-initialise data - orphan any pointer, as would relate to
	// previous run's heap
	iRecorder = NULL; 
	
	User::LeaveIfError(iFs.Connect());
	iFs.Delete(iFileName); // ensure we start with no file. Ignore errors.
	
	iRecorder = CRecorder::NewL(this);
	iSuite->Log(ESevrInfo, _L("Recording Started...."));
	iRecorder->RecordL(iFileName, EMdaPriorityNormal);
	}
	
// cleanup at end
void RAudioRecPanicTest::CloseTest()
	{
	delete iRecorder;
	iRecorder = NULL;
	iFs.Close();
	}
	
// recording stopped
void RAudioRecPanicTest::RecordError(TInt aError)
	{
	iSuite->Log(ESevrInfo, _L("Recording Finished...."));
	if(aError!=KErrServerTerminated)
		{
		StopTest(aError, EFail);
		}
	else
		{
		StopTest(aError, EPass);
		}
	}
