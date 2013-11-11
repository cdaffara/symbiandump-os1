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

const TInt KOneSec = 1000000;

/**
 *
 * Static constructor for CTestStepRecorderWindowPlay.
 *
 *
 * @return	"CTestStepRecorderWindowPlay*"
 *			The constructed CTestStepRecorderWindowPlay
 *
 * @xxxx
 * 
 */
CTestStepRecorderWindowPlay* CTestStepRecorderWindowPlay::NewL(const TDesC& aTestName, TBool aIsConverterTest)
	{
	CTestStepRecorderWindowPlay* self = new(ELeave) CTestStepRecorderWindowPlay(aTestName, aIsConverterTest);
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
CTestStepRecorderWindowPlay::CTestStepRecorderWindowPlay(const TDesC& aTestName, TBool aIsConverterTest)
: iIsConverterTest(aIsConverterTest)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderWindowPlay::~CTestStepRecorderWindowPlay()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderWindowPlay::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	 // Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iIsConverterTest)
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::SetPlayWindow()"));
		}
	else
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::SetPlayWindow()"));
		}

	if(!(GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName3) // au
		)) 
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if (!iIsConverterTest)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		iRecorder->OpenFileL( iFileName );
		CActiveScheduler::Start(); 
		if (iRecorder == NULL ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;
		
		iMiddle = TTimeIntervalMicroSeconds( iRecorder->Duration().Int64() / 2 );
		iEnd = iRecorder->Duration();
		}
	else // isConverterTest == ETrue
		{
		// create the Recorder utility
		if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
			return EInconclusive;

		iConvert->OpenL( iFileName, iFileName3 );
		CActiveScheduler::Start(); 
		if (iConvert == NULL ||
			iError != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen)
			return EInconclusive;
		
		iMiddle = TTimeIntervalMicroSeconds( iConvert->Duration().Int64() / 2 );
		iEnd = iConvert->Duration();
		}
		
		iBeginning = TTimeIntervalMicroSeconds( 0 );
	return verdict;	
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderWindowPlay::DoTestStepPostambleL(void)
	{

	if (!iIsConverterTest)
		{
		delete iRecorder;
		iRecorder = NULL;
		}
	else
		{
		delete iConvert;
		iConvert = NULL;
		}

	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileName3);
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
void CTestStepRecorderWindowPlay::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	if (iError != KErrNone)
		{
		__DEBUGGER();
		}
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
TVerdict CTestStepRecorderWindowPlay::DoTestStepL()
	{
	iTestStepResult = EPass;
    iError = KErrNone;
	TInt testNum = 1;

	//
	// check the converter first with no exhaustive tests 
	// and then the recorder more exhaustively
	if (iIsConverterTest)
		{
		INFO_PRINTF1(_L("converter set/clear play window test"));
		iConvert->SetPlayWindow(iBeginning, iMiddle);
		iConvert->PlayL();
		CActiveScheduler::Start();   // open -> play
		if (iConvert == NULL || iError != KErrNone )
			{
			return EFail;
			}
		CActiveScheduler::Start();   // play -> open
		if (iConvert == NULL || iError != KErrNone )
			{
			return EFail;
			}

		// clear the play window now, and play again
		iConvert->ClearPlayWindow();
		iConvert->PlayL();
		CActiveScheduler::Start();   // open -> play
		if (iConvert == NULL || iError != KErrNone )
			{
			return EFail;
			}
		CActiveScheduler::Start();   // play -> open
		if (iConvert == NULL || iError != KErrNone )
			{
			return EFail;
			}
		return iTestStepResult;
		}
	// end converter test //
	 
	// no easy or obvious way to check if command successful
	//if it was a pure unit test it would only matter if the parameters are passed
	// correctly, and the return error code is propagated.

	INFO_PRINTF1(_L("set play window test 1"));
	iRecorder->SetPlayWindow(iBeginning, iBeginning); 
    INFO_PRINTF1(_L("TestStep->PlayL()"));//Statement Changed under DEF105143
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	testNum++;

	INFO_PRINTF1(_L("set play window test 2"));
	iRecorder->SetPlayWindow(iBeginning, TTimeIntervalMicroSeconds(1));
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	testNum++;

	INFO_PRINTF1(_L("set play window test 3"));
	iRecorder->SetPlayWindow(iBeginning, TTimeIntervalMicroSeconds(10));
    INFO_PRINTF1(_L("TestStep->PlayL()"));//Statement Changed under DEF105143
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	testNum++;

	INFO_PRINTF1(_L("set play window test 4"));
	iRecorder->SetPlayWindow(iBeginning, TTimeIntervalMicroSeconds(100));
    INFO_PRINTF1(_L("TestStep->PlayL()"));//Statement Changed under DEF105143
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	testNum++;

	INFO_PRINTF1(_L("set play window test 5"));
	iRecorder->SetPlayWindow(iBeginning, TTimeIntervalMicroSeconds(1000));
    INFO_PRINTF1(_L("TestStep->PlayL()"));//Statement Changed under DEF105143
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	testNum++;

	INFO_PRINTF1(_L("set play window test 6"));
	iRecorder->SetPlayWindow(iBeginning, TTimeIntervalMicroSeconds(10000));
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	testNum++;

	INFO_PRINTF1(_L("set play window test 7"));
	iRecorder->SetPlayWindow(iBeginning, iMiddle);
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	testNum++;
	
	INFO_PRINTF1(_L("set play window test 8"));
	iRecorder->SetPlayWindow(iMiddle, iEnd);
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	testNum++;

	INFO_PRINTF1(_L("set play window test 9"));
	iRecorder->SetPlayWindow(TTimeIntervalMicroSeconds(iEnd.Int64()-100000), iEnd);
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	testNum++;

	INFO_PRINTF1(_L("set play window test 10"));
	iRecorder->SetPlayWindow(TTimeIntervalMicroSeconds(iEnd.Int64()-1000), iEnd);
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	testNum++;

	INFO_PRINTF1(_L("set play window test 11"));
	iRecorder->SetPlayWindow(TTimeIntervalMicroSeconds(iEnd.Int64()-100), iEnd);
	iRecorder->PlayL();
	CActiveScheduler::Start();   // open -> play
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		//iRecorder->Duration() != TTimeIntervalMicroSeconds(100) || // duration remains to original sound length
		{
		iTestStepResult = EFail;
		goto EndTest;
		}

EndTest:
	if (iTestStepResult == EFail)
		INFO_PRINTF3(_L("Test failed at test %d with error %d"), testNum, iError);
	else
		INFO_PRINTF1(_L("All tests completed"));


	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Static constructor for CTestStepRecorderWindowPlayTwice.
 *
 *
 * @return	"CTestStepRecorderWindowPlayTwice*"
 *			The constructed CTestStepRecorderWindowPlayTwice
 *
 * @xxxx
 * 
 */
CTestStepRecorderWindowPlayTwice* CTestStepRecorderWindowPlayTwice::NewL(const TDesC& aTestName)
	{
	CTestStepRecorderWindowPlayTwice* self = new(ELeave) CTestStepRecorderWindowPlayTwice(aTestName);
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
CTestStepRecorderWindowPlayTwice::CTestStepRecorderWindowPlayTwice(const TDesC& aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderWindowPlayTwice::~CTestStepRecorderWindowPlayTwice()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderWindowPlayTwice::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	 // Printing to the console and log file
	INFO_PRINTF1(iTestStepName);

	if( !(GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName)) ) 
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY- SetPlayWindow,while in before open, Opened and Playing an audio file."));
	
	if (GetIntFromConfig(_L("SectionOne"), _L("PlayWindowStart"), iSetPlayWindowStart))
		{
		INFO_PRINTF2(_L("SetPlaywindowStart = %d"), iSetPlayWindowStart );
		}
	else
		{
		return EInconclusive;
		}
		
	if (GetIntFromConfig(_L("SectionOne"),_L("PlayWindowEnd"),iSetPlayWindowEnd)) 
		{
		INFO_PRINTF2(_L("SetPlaywindowEnd = %d"), iSetPlayWindowEnd );
		}
	else
		{
		return EInconclusive;
		}


	// create the Recorder utility
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		{
		return EInconclusive;
		}
		
	iRecorder->OpenFileL( iFileName );
	
	CActiveScheduler::Start();
	
	if (iRecorder == NULL ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
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
enum TVerdict CTestStepRecorderWindowPlayTwice::DoTestStepPostambleL(void)
	{

	delete iRecorder;
	iRecorder = NULL;

	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
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
void CTestStepRecorderWindowPlayTwice::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	if (iError != KErrNone)
		{
		__DEBUGGER();
		}
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
TVerdict CTestStepRecorderWindowPlayTwice::DoTestStepL()
	{
	iTestStepResult = EPass;
    iError = KErrNone;
	TInt testNum = 1;

	// no easy or obvious way to check if command successful
	//if it was a pure unit test it would only matter if the parameters are passed
	// correctly, and the return error code is propagated.

	INFO_PRINTF3(_L("set play window Start=%d End=%d"), iSetPlayWindowStart, iSetPlayWindowEnd);
	iRecorder->SetPlayWindow((iSetPlayWindowStart * KOneSec), (iSetPlayWindowEnd * KOneSec));
	
	RDebug::Print(_L("TestStep->PlayL()"));
	iRecorder->PlayL();
	
	CActiveScheduler::Start();   // open -> play
	
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	// wait till it plays to end of the window
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}

	// Reset play windows to +2
	iSetPlayWindowStart = iSetPlayWindowStart + 2;
	iSetPlayWindowEnd = iSetPlayWindowEnd + 2;


	INFO_PRINTF3(_L("set play window Start=%d End=%d"), iSetPlayWindowStart, iSetPlayWindowEnd);
	iRecorder->SetPlayWindow((iSetPlayWindowStart * KOneSec), (iSetPlayWindowEnd * KOneSec));
	
	RDebug::Print(_L("TestStep->PlayL()"));
	iRecorder->PlayL();
	
	CActiveScheduler::Start();   // open -> play
	
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	// wait till it plays to end of the window
	CActiveScheduler::Start();   // play -> open
	if (iRecorder ==NULL || iError != KErrNone )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}

EndTest:
	if (iTestStepResult == EFail)
		INFO_PRINTF3(_L("Test failed at test %d with error %d"), testNum, iError);
	else
		INFO_PRINTF1(_L("All tests completed"));


	// test steps return a result
	return iTestStepResult;
	}
