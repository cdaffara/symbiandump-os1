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
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepRecorderBalancePlay.
 *
 *
 * @return	"CTestStepRecorderBalancePlay*"
 *			The constructed CTestStepRecorderBalancePlay
 *
 * @xxxx
 * 
 */
CTestStepRecorderBalancePlay* CTestStepRecorderBalancePlay::NewL()
	{
	CTestStepRecorderBalancePlay* self = new(ELeave) CTestStepRecorderBalancePlay;
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
CTestStepRecorderBalancePlay::CTestStepRecorderBalancePlay()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0257-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderBalancePlay::~CTestStepRecorderBalancePlay()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepRecorderBalancePlay::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0257-HP"));
	 INFO_PRINTF1(_L("this test is setting/getting the Play balance of the recorder utility with several values. [Set|Get]PlaybackBalance()"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) )
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
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
enum TVerdict CTestStepRecorderBalancePlay::DoTestStepPostambleL(void)
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
void CTestStepRecorderBalancePlay::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
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
TVerdict CTestStepRecorderBalancePlay::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt balance = 0; // KMMFBalanceCenter
	
	// test 1
	err = iRecorder->SetPlaybackBalance(); // default KMMFBalanceCenter = 0
	if (err != KErrNone)
		return EFail;

	err = iRecorder->GetPlaybackBalance(balance);
	if (err != KErrNone ||
		balance != KMMFBalanceCenter)
		return EFail;

	// test 2
	err = iRecorder->SetPlaybackBalance(KMMFBalanceMaxLeft); // -100
	if (err != KErrNone)
		return EFail;

	err = iRecorder->GetPlaybackBalance(balance);
	if (err != KErrNone ||
		balance != KMMFBalanceMaxLeft)
		return EFail;

	// test 3
	err = iRecorder->SetPlaybackBalance(KMMFBalanceMaxRight);
	if (err != KErrNone)
		return EFail;

	err = iRecorder->GetPlaybackBalance(balance);
	if (err != KErrNone ||
		balance != KMMFBalanceMaxRight)
		return EFail;

	// test 4
	err = iRecorder->SetPlaybackBalance(KMMFBalanceMaxRight/2);
	if (err != KErrNone)
		return EFail;

	err = iRecorder->GetPlaybackBalance(balance);
	if (err != KErrNone ||
		balance != KMMFBalanceMaxRight/2)
		return EFail;


	// test 4 out of bounds
	/* until we check the return codes...
	TRAP(err, iRecorder->SetPlaybackBalance(KMMFBalanceMaxRight+5));
	if (err != KErrArgument ||
		err != KErrTooBig )
		return EFail;
	*/

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderBalanceRecord.
 *
 *
 * @return	"CTestStepRecorderBalanceRecord*"
 *			The constructed CTestStepRecorderBalanceRecord
 *
 * @xxxx
 * 
 */
CTestStepRecorderBalanceRecord* CTestStepRecorderBalanceRecord::NewL()
	{
	CTestStepRecorderBalanceRecord* self = new(ELeave) CTestStepRecorderBalanceRecord;
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
CTestStepRecorderBalanceRecord::CTestStepRecorderBalanceRecord()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0258-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderBalanceRecord::~CTestStepRecorderBalanceRecord()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepRecorderBalanceRecord::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0258-HP"));
	 INFO_PRINTF1(_L("this test is setting/getting the Record balance of the recorder utility with several values. [Set|Get]RecordkBalance()"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) )
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
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
enum TVerdict CTestStepRecorderBalanceRecord::DoTestStepPostambleL(void)
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
void CTestStepRecorderBalanceRecord::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
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
TVerdict CTestStepRecorderBalanceRecord::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt balance = 0; // KMMFBalanceCenter
	
	// test 1
	err = iRecorder->SetRecordBalance(); // default KMMFBalanceCenter = 0
	if (err != KErrNone)
		return EFail;

	err = iRecorder->GetRecordBalance(balance);
	if (err != KErrNone ||
		balance != 0)
		return EFail;

	// test 2
	err = iRecorder->SetRecordBalance(KMMFBalanceMaxLeft); // -100
	if (err != KErrNone)
		return EFail;

	err = iRecorder->GetRecordBalance(balance);
	if (err != KErrNone ||
		balance != KMMFBalanceMaxLeft)
		return EFail;

	// test 3
	err = iRecorder->SetRecordBalance(KMMFBalanceMaxRight);
	if (err != KErrNone)
		return EFail;

	err = iRecorder->GetRecordBalance(balance);
	if (err != KErrNone ||
		balance != KMMFBalanceMaxRight)
		return EFail;

	// test 4
	err = iRecorder->SetRecordBalance(KMMFBalanceMaxRight/2);
	if (err != KErrNone)
		return EFail;

	err = iRecorder->GetRecordBalance(balance);
	if (err != KErrNone ||
		balance != KMMFBalanceMaxRight/2)
		return EFail;

	// test 4 out of bounds
	/* until we check the return codes....
	TRAP(err, iRecorder->SetRecordBalance(KMMFBalanceMaxRight+5));
	if (err != KErrArgument ||
		err != KErrTooBig )
		return EFail;
	*/

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
