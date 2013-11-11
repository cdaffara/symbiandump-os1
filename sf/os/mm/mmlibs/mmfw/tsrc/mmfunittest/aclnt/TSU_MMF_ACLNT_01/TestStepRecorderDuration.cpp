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
 * Static constructor for CTestStepRecorderDuration.
 *
 *
 * @return	"CTestStepRecorderDuration*"
 *			The constructed CTestStepRecorderDuration
 *
 * @xxxx
 * 
 */
CTestStepRecorderDuration* CTestStepRecorderDuration::NewL()
	{
	CTestStepRecorderDuration* self = new(ELeave) CTestStepRecorderDuration;
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
CTestStepRecorderDuration::CTestStepRecorderDuration() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0252-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderDuration::~CTestStepRecorderDuration()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderDuration::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0252-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::Duration()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileNames[0])) // wav file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	/*
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName3"), iFileNames[1])) // au file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName5"), iFileNames[2])) // raw file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	*/
	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderDuration::DoTestStepPostambleL(void)
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
void CTestStepRecorderDuration::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderDuration::OpenAndStartSchedulerL(TInt i)
	{
	iRecorder->OpenFileL( iFileNames[i] );
	CActiveScheduler::Start(); 
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
TVerdict CTestStepRecorderDuration::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	//for (TInt type=0; type<3; type++)
	// only one type at the moment
	for (TInt type=0; type<1; type++)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TRAP( err, OpenAndStartSchedulerL(type) );

		if (iRecorder == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;

		INFO_PRINTF2(_L("FileName = %S"), &iFileNames[type]);
		TTimeIntervalMicroSeconds theDuration = iRecorder->Duration();
		TTimeIntervalMicroSeconds expectedDuration;
		switch (type)
			{
			case 0:
				expectedDuration = TTimeIntervalMicroSeconds(1000125);
				break;
			case 1:
				expectedDuration = TTimeIntervalMicroSeconds(8359176);
				break;
			case 2:
				expectedDuration = TTimeIntervalMicroSeconds(1000125);
				break;
			default:
				break;
			}
	
		INFO_PRINTF3(_L("Duration = %d, expected = %d"), I64INT(theDuration.Int64()), I64INT(expectedDuration.Int64()));

		delete iRecorder;
		iRecorder = NULL;

		if (theDuration != expectedDuration)
			return EFail;

		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// class CTestStepRecorderDurationInformation
//

/**
 *
 * Static constructor for CTestStepRecorderDurationInformation.
 *
 *
 * @return	"CTestStepRecorderDurationInformation*"
 *			The constructed CTestStepRecorderDurationInformation
 *
 * @xxxx
 * 
 */
CTestStepRecorderDurationInformation* CTestStepRecorderDurationInformation::NewL()
	{
	CTestStepRecorderDurationInformation* self = new(ELeave) CTestStepRecorderDurationInformation;
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
CTestStepRecorderDurationInformation::CTestStepRecorderDurationInformation()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0961-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderDurationInformation::~CTestStepRecorderDurationInformation()
	{
	}
 	
/**
 * Implementation of the MMdaObjectStateChangeObserver interface functions
 **/
void CTestStepRecorderDurationInformation::MoscoStateChangeEvent(CBase* /*aObject*/, 
																	TInt /*aPreviousState*/, 
																	TInt /*aCurrentState*/, 
																	TInt aErrorCode)
	{
	iError = aErrorCode;
	CActiveScheduler::Stop();
	}

/**

 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderDurationInformation::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0961-HP"));
	
	INFO_PRINTF1(_L("This test checks that CMdaAudioRecorderUtility::Duration(TTimeIntervalMicroSeconds &) returns TMMFDurationInfo::EMMFDurationInfoValid"));
	
	iError = KErrNone;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameMeta"), iFileName))
		{
		return EInconclusive;
		}

	//
	// Setup player with dummy file
	if ( (iRecorder = CMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;
	
	iRecorder->OpenFileL(iFileName);
	
	CActiveScheduler::Start();
	User::After(1000000);

	if (iRecorder == NULL || iError != KErrNone )
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
enum TVerdict CTestStepRecorderDurationInformation::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
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
TVerdict CTestStepRecorderDurationInformation::DoTestStepL()
	{
	iTestStepResult = EFail;
	
	// Check that the duration state is TMMFDurationInfo::EMMFDurationInfoValid - known.
	TTimeIntervalMicroSeconds  result2 = 0;
	TMMFDurationInfo result = iRecorder->Duration(result2);
	
	if ( result == EMMFDurationInfoValid )
		{
		iTestStepResult = EPass;		
		}
	else
		{
		iTestStepResult = EFail;
		}
	
	// test steps return a result
	return iTestStepResult;
	}
