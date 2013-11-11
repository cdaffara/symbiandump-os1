// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <mdaaudiotoneplayer.h>

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
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepRecorderSimultaneousPlay1.
 *
 *
 * @return	"CTestStepRecorderSimultaneousPlay1*"
 *			The constructed CTestStepRecorderSimultaneousPlay1
 *
 * @xxxx
 *
 */
CTestStepRecorderSimultaneousPlay1* CTestStepRecorderSimultaneousPlay1::NewL()
	{
	CTestStepRecorderSimultaneousPlay1* self = new(ELeave) CTestStepRecorderSimultaneousPlay1;
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
CTestStepRecorderSimultaneousPlay1::CTestStepRecorderSimultaneousPlay1()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0810-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderSimultaneousPlay1::~CTestStepRecorderSimultaneousPlay1()
	{
	}

/**
 *
 * Test step Close
 * handles all deletion
 * @xxxx
 *
 */
void CTestStepRecorderSimultaneousPlay1::Close()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderSimultaneousPlay1::DoTestStepPreambleL(void)
	{
	TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this is a test of calling CMdaAudioRecorderUtility::PlayL() while a tone is playing"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName)) // wav file
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
enum TVerdict CTestStepRecorderSimultaneousPlay1::DoTestStepPostambleL(void)
	{
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
void CTestStepRecorderSimultaneousPlay1::MoscoStateChangeEvent(CBase* aObject,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	INFO_PRINTF5(_L("MoscoStateChangeEvent(object=0x%08x, ps=%d, cs=%d, error = %d)"),
		aObject,
		aPreviousState,
		aCurrentState,
		aErrorCode);
	iError = aErrorCode;
	iCurrentState = aCurrentState;
	CActiveScheduler::Stop();
	}


void CTestStepRecorderSimultaneousPlay1::MatoPrepareComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MatoPrepareComplete(): error = %d"), aError);
	iErrorTone = aError;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderSimultaneousPlay1::MatoPlayComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MatoPlayComplete(): error = %d"), aError);
	iErrorTone = aError;
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
TVerdict CTestStepRecorderSimultaneousPlay1::DoTestStepL()
	{
	iTestStepResult = EFail;

	iToneUtility = CMdaAudioToneUtility::NewL(*this);
	CleanupStack::PushL(iToneUtility);

	iRecorderUtility = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(iRecorderUtility);

	do
		{
		// open the sound file
		INFO_PRINTF1( _L("Initialize CMdaAudioRecorderUtility"));
		iRecorderUtility->OpenFileL(iFileName);
		CActiveScheduler::Start();
		if (iCurrentState != CMdaAudioRecorderUtility::EOpen)
			{
			break;
			}

		// prepare the tone
		INFO_PRINTF1( _L("Initialize CMdaAudioToneUtility"));
		iToneUtility->PrepareToPlayTone(KToneFrequency, TTimeIntervalMicroSeconds(KTwoSeconds));
		// Wait for prepare
		iErrorTone = KErrGeneral;	// set so we know callback has occurred
		CActiveScheduler::Start();
		if (iErrorTone != KErrNone)
			{
			break;
			}

		// start playing the tone & wait 0.25 secs to make sure it gets started
		INFO_PRINTF1( _L("Playing tone..."));
		iToneUtility->Play();
		User::After(KQuarterScond);

		// start playing the file
		INFO_PRINTF1( _L("Playing sound file..."));
		iRecorderUtility->PlayL();
		// wait once for start of play
		// NB this "EPlaying" callback does not occur in 6.1 !
		CActiveScheduler::Start();
		if ((iError != KErrNone) || (iCurrentState != CMdaAudioRecorderUtility::EPlaying))
			{
			break;
			}

		// wait once for end of play - this should produce KErrInUse
		INFO_PRINTF1( _L("Waiting for end of play..."));
		CActiveScheduler::Start();
		if ((iError != KErrInUse) || (iCurrentState != CMdaAudioRecorderUtility::EOpen))
			{
			break;
			}
		// wait once tone to finsh playing
		CActiveScheduler::Start();

		// now that the tone player has finished, try playing
		// the sound file again (this should work now)

		INFO_PRINTF1( _L("Calling iRecorderUtility->PlayL..."));
		iRecorderUtility->PlayL();
		// wait once for start of play
		CActiveScheduler::Start();
		if ((iError != KErrNone) || (iCurrentState != CMdaAudioRecorderUtility::EPlaying))
			{
			break;
			}

		// wait once for end of play - this should produce KErrNone
		CActiveScheduler::Start();
		if ((iError != KErrNone) || (iCurrentState != CMdaAudioRecorderUtility::EOpen))
			{
			break;
			}

		iTestStepResult = EPass;
		}
	while (EFalse);

	CleanupStack::PopAndDestroy(iRecorderUtility);
	CleanupStack::PopAndDestroy(iToneUtility);
	iRecorderUtility = NULL;
	iToneUtility = NULL;

	INFO_PRINTF1(_L("finished with this test step"));

	// test steps return a result
	return iTestStepResult;
	}

//


/**
 *
 * Static constructor for CTestStepRecorderSimultaneousPlay2.
 *
 *
 * @return	"CTestStepRecorderSimultaneousPlay2*"
 *			The constructed CTestStepRecorderSimultaneousPlay2
 *
 * @xxxx
 *
 */
CTestStepRecorderSimultaneousPlay2* CTestStepRecorderSimultaneousPlay2::NewL()
	{
	CTestStepRecorderSimultaneousPlay2* self = new(ELeave) CTestStepRecorderSimultaneousPlay2;
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
CTestStepRecorderSimultaneousPlay2::CTestStepRecorderSimultaneousPlay2()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0811-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderSimultaneousPlay2::~CTestStepRecorderSimultaneousPlay2()
	{
	}

/**
 *
 * Test step Close
 * handles all deletion
 *
 * @xxxx
 *
 */
void CTestStepRecorderSimultaneousPlay2::Close()
	{
	delete iToneUtility;
	delete iRecorderUtility;
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderSimultaneousPlay2::DoTestStepPreambleL(void)
	{
	TInt err;

	TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this is a test of playing a high priority tone over a low priority .wav file"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName)) // wav file
		{
		return EInconclusive;
		}

	// create a record utility
	TRAP(err, iRecorderUtility = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityMin));
	if (err != KErrNone)
		return EInconclusive;
	// create a tone utility
	TRAP(err, iToneUtility = CMdaAudioToneUtility::NewL(*this, NULL, EMdaPriorityMax));
	if (err != KErrNone)
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
enum TVerdict CTestStepRecorderSimultaneousPlay2::DoTestStepPostambleL(void)
	{
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
void CTestStepRecorderSimultaneousPlay2::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	INFO_PRINTF4(_L("MoscoStateChangeEvent(): Previous state = %d, current state = %d, error = %d"),
		aPreviousState,
		aCurrentState,
		aErrorCode);
	iError = aErrorCode;
	iCurrentState = aCurrentState;
	CActiveScheduler::Stop();
	}


void CTestStepRecorderSimultaneousPlay2::MatoPrepareComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MatoPrepareComplete(): error = %d"), aError);
	iErrorTone = aError;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderSimultaneousPlay2::MatoPlayComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MatoPlayComplete(): error = %d"), aError);
	iErrorTone = aError;
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
TVerdict CTestStepRecorderSimultaneousPlay2::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// open the sound file
	INFO_PRINTF1( _L("Initialize CMdaAudioRecorderUtility"));
	TRAP(err, iRecorderUtility->OpenFileL(iFileName));
	if (err != KErrNone)
		return EInconclusive;

	// wait for open to complete
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState == CMdaAudioRecorderUtility::EOpen))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	// prepare the tone
	INFO_PRINTF1( _L("Initialize CMdaAudioToneUtility"));
	iToneUtility->PrepareToPlayTone(KToneFrequency,TTimeIntervalMicroSeconds(KTwoSeconds));
	// Wait for prepare
	iErrorTone = KErrGeneral;	// set so we know tone callback has occurred
	CActiveScheduler::Start();
	if (iErrorTone != KErrNone)
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	// start playing the file and give it time to start
	INFO_PRINTF1( _L("Playing sound file (1)...low priority"));
	TRAP(err, iRecorderUtility->PlayL());
	if (err != KErrNone)
		return EInconclusive;
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	User::After(KQuarterScond);


	// start playing the tone and wait for completion
	// Two observer callbacks to catch here:
	// One is high priority tone starting playing, and the other is the low
	// priority recorder being kicked off. We are ignoring the order of these.
	INFO_PRINTF1( _L("Playing tone...high priority"));
	iToneUtility->Play();
	iErrorTone = KErrGeneral;	// set so we know tone callback has occurred
	CActiveScheduler::Start();
	if (!(iError == KErrInUse && iCurrentState == CMdaAudioRecorderUtility::EOpen) &&
		!(iErrorTone == KErrNone))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	iErrorTone = KErrGeneral;	// set so we know tone callback has occurred
	CActiveScheduler::Start();
	if (!(iError == KErrInUse && iCurrentState == CMdaAudioRecorderUtility::EOpen) &&
		!(iErrorTone == KErrNone))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}


	// start playing the file again and wait for completion
	INFO_PRINTF1( _L("Playing sound file (2)...low priority"));
	TRAP(err, iRecorderUtility->PlayL());
	if (err != KErrNone)
		return EInconclusive;
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	// Wait for end of play
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState == CMdaAudioRecorderUtility::EOpen))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	INFO_PRINTF1(_L("finished with this test step"));

	// test steps return a result
	return iTestStepResult;
	}


//


/**
 *
 * Static constructor for CTestStepRecorderSimultaneousPlay3.
 *
 *
 * @return	"CTestStepRecorderSimultaneousPlay3*"
 *			The constructed CTestStepRecorderSimultaneousPlay3
 *
 * @xxxx
 *
 */
CTestStepRecorderSimultaneousPlay3* CTestStepRecorderSimultaneousPlay3::NewL()
	{
	CTestStepRecorderSimultaneousPlay3* self = new(ELeave) CTestStepRecorderSimultaneousPlay3;
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
CTestStepRecorderSimultaneousPlay3::CTestStepRecorderSimultaneousPlay3()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0812-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderSimultaneousPlay3::~CTestStepRecorderSimultaneousPlay3()
	{
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
void CTestStepRecorderSimultaneousPlay3::Close()
	{
	delete iRecorderUtility1;
	delete iRecorderUtility2;
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderSimultaneousPlay3::DoTestStepPreambleL(void)
	{
	TInt err;

	TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this is a test of playing a high priority .wav file over a low priority .wav file"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName1)) // long wav file
		{
		return EInconclusive;
		}

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName2)) // short wav file
		{
		return EInconclusive;
		}

	// create record utilities. A low priority and a high priority
	TRAP(err, iRecorderUtility1 = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityMin));
	if (err != KErrNone)
		return EInconclusive;

	TRAP(err, iRecorderUtility2 = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityMax));
	if (err != KErrNone)
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
enum TVerdict CTestStepRecorderSimultaneousPlay3::DoTestStepPostambleL(void)
	{
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
void CTestStepRecorderSimultaneousPlay3::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	INFO_PRINTF4(_L("MoscoStateChangeEvent(): Previous state = %d, current state = %d, error = %d"),
		aPreviousState,
		aCurrentState,
		aErrorCode);
	iError = aErrorCode;
	iCurrentState = aCurrentState;
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
TVerdict CTestStepRecorderSimultaneousPlay3::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// open the sound file
	INFO_PRINTF1( _L("Initialize CMdaAudioRecorderUtility1"));
	TRAP(err, iRecorderUtility1->OpenFileL(iFileName1));
	if (err != KErrNone)
		return EInconclusive;
	// wait for open to complete
	CActiveScheduler::Start();
	if (iCurrentState != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	// open the sound file
	INFO_PRINTF1( _L("Initialize CMdaAudioRecorderUtility2"));
	TRAP(err, iRecorderUtility2->OpenFileL(iFileName2));
	if (err != KErrNone)
		return EInconclusive;
	// wait for open to complete
	CActiveScheduler::Start();
	if (iCurrentState != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	// start playing the first file and give it time to start
	INFO_PRINTF1( _L("Playing sound file (1)...low priority"));
	TRAP(err, iRecorderUtility1->PlayL());
	if (err != KErrNone)
		return EInconclusive;
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	User::After(KQuarterScond);

	// start playing the second file and wait for completion
	// Two observer callbacks to catch here:
	// One is high priority recorder starting playing, and the other is the low
	// priority recorder being kicked off. We are ignoring the order of these.
	INFO_PRINTF1( _L("Playing sound file (2)...high priority"));
	TRAP(err, iRecorderUtility2->PlayL());
	if (err != KErrNone)
		return EFail;
	CActiveScheduler::Start();
	if (!(iError == KErrInUse && iCurrentState == CMdaAudioRecorderUtility::EOpen) &&
		!(iError == KErrNone && iCurrentState == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	CActiveScheduler::Start();
	if (!(iError == KErrInUse && iCurrentState == CMdaAudioRecorderUtility::EOpen) &&
		!(iError == KErrNone && iCurrentState == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	// Wait for end of play for file 2
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState == CMdaAudioRecorderUtility::EOpen))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	// start playing the first file again and wait for completion
	INFO_PRINTF1( _L("Playing sound file (3)...low priority"));
	TRAP(err, iRecorderUtility1->PlayL());
	if (err != KErrNone)
		return EFail;
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	User::After(KQuarterScond);

	// Wait for end of play
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState == CMdaAudioRecorderUtility::EOpen))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	INFO_PRINTF1(_L("finished with this test step"));

	// test steps return a result
	return iTestStepResult;
	}


//


/**
 *
 * Static constructor for CTestStepRecorderSimultaneousPlay4.
 *
 *
 * @return	"CTestStepRecorderSimultaneousPlay4*"
 *			The constructed CTestStepRecorderSimultaneousPlay4
 *
 * @xxxx
 *
 */
CTestStepRecorderSimultaneousPlay4* CTestStepRecorderSimultaneousPlay4::NewL()
	{
	CTestStepRecorderSimultaneousPlay4* self = new(ELeave) CTestStepRecorderSimultaneousPlay4;
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
CTestStepRecorderSimultaneousPlay4::CTestStepRecorderSimultaneousPlay4()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0813-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderSimultaneousPlay4::~CTestStepRecorderSimultaneousPlay4()
	{
	}

/**
 *
 * Test step Close
 * handles all deletion
 * @xxxx
 *
 */
void CTestStepRecorderSimultaneousPlay4::Close()
	{
	delete iRecorderUtility1;
	delete iRecorderUtility2;
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderSimultaneousPlay4::DoTestStepPreambleL(void)
	{
	TInt err;

	TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this is a test of playing a high priority .wav file over the same low priority .wav file"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName1)) // long wav file
		{
		return EInconclusive;
		}

	// create record utilities. A low priority and a high priority
	TRAP(err, iRecorderUtility1 = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityMin));
	if (err != KErrNone)
		return EInconclusive;

	TRAP(err, iRecorderUtility2 = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityMax));
	if (err != KErrNone)
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
enum TVerdict CTestStepRecorderSimultaneousPlay4::DoTestStepPostambleL(void)
	{
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
void CTestStepRecorderSimultaneousPlay4::MoscoStateChangeEvent(CBase* aObject,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	_LIT(KRecordUtility1, "RecorderUtility1");
	_LIT(KRecordUtility2, "RecorderUtility2");
	INFO_PRINTF5(_L("MoscoStateChangeEvent(): %S : Previous state = %d, current state = %d, error = %d"),
		(aObject == iRecorderUtility1)?&KRecordUtility1:&KRecordUtility2,
		aPreviousState,
		aCurrentState,
		aErrorCode);
	iError = aErrorCode;

	if (aObject == iRecorderUtility1)
		iCurrentState1 = aCurrentState;
	else if (aObject == iRecorderUtility2)
		iCurrentState2 = aCurrentState;


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
TVerdict CTestStepRecorderSimultaneousPlay4::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	INFO_PRINTF2(_L("File to play = %S"), &iFileName1);

	// Get the first recorder utility to open the sound file
	INFO_PRINTF1( _L("Initialize CMdaAudioRecorderUtility1"));
	TRAP(err, iRecorderUtility1->OpenFileL(iFileName1));
	if (err != KErrNone)
		return EInconclusive;
	// wait for open to complete
	CActiveScheduler::Start();
	if (iCurrentState1 != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	// start playing the first file and give it time to start
	INFO_PRINTF1( _L("Playing sound file (Utililty #1)...low priority"));
	TRAP(err, iRecorderUtility1->PlayL());
	if (err != KErrNone)
		return EInconclusive;
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState1 == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	User::After(KQuarterScond);

	// Get the second recorder utility to open the sound file
	INFO_PRINTF1( _L("Initialize CMdaAudioRecorderUtility2"));
	TRAP(err, iRecorderUtility2->OpenFileL(iFileName1));
	if (err != KErrNone)
		return EInconclusive;
	// wait for open to complete
	CActiveScheduler::Start();
	if (iCurrentState2 != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	// start playing the second file and wait for completion
	// Two observer callbacks to catch here:
	// One is high priority recorder starting playing, and the other is the low
	// priority recorder being kicked off. We are ignoring the order of these.
	INFO_PRINTF1( _L("Playing sound file (Utililty #2)...high priority"));
	TRAP(err, iRecorderUtility2->PlayL());
	if (err != KErrNone)
		return EFail;
	CActiveScheduler::Start();
	if (!(iError == KErrInUse && iCurrentState1 == CMdaAudioRecorderUtility::EOpen) &&
		!(iError == KErrNone && iCurrentState2 == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	CActiveScheduler::Start();
	if (!(iError == KErrInUse && iCurrentState1 == CMdaAudioRecorderUtility::EOpen) &&
		!(iError == KErrNone && iCurrentState2 == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	// Wait for end of play for second recorder utility
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState2 == CMdaAudioRecorderUtility::EOpen))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	// start playing the first file again and wait for completion
	INFO_PRINTF1( _L("Playing sound file (Utililty #1 again)...low priority"));
	TRAP(err, iRecorderUtility1->PlayL());
	if (err != KErrNone)
		return EFail;
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState1 == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	// Wait for end of play
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState1 == CMdaAudioRecorderUtility::EOpen))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	INFO_PRINTF1(_L("finished with this test step"));

	// test steps return a result
	return iTestStepResult;
	}


//


/**
 *
 * Static constructor for CTestStepRecorderSimultaneousPlay5
 *
 *
 * @return	"CTestStepRecorderSimultaneousPlay5*"
 *			The constructed CTestStepRecorderSimultaneousPlay5
 *
 * @xxxx
 *
 */
CTestStepRecorderSimultaneousPlay5* CTestStepRecorderSimultaneousPlay5::NewL()
	{
	CTestStepRecorderSimultaneousPlay5* self = new(ELeave) CTestStepRecorderSimultaneousPlay5;
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
CTestStepRecorderSimultaneousPlay5::CTestStepRecorderSimultaneousPlay5()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0814-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderSimultaneousPlay5::~CTestStepRecorderSimultaneousPlay5()
	{
	}

/**
 *
 * Test step Close
 * handles all deletion
 * @xxxx
 *
 */
void CTestStepRecorderSimultaneousPlay5::Close()
	{
	delete iRecorderUtility1;
	delete iRecorderUtility2;
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderSimultaneousPlay5::DoTestStepPreambleL(void)
	{
	TInt err;

	TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this is a test of Open a low priority recorder and a higher priority recorder with the same file & get the duration in each case"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName1)) // long wav file
		{
		return EInconclusive;
		}

	// create record utilities. A low priority and a high priority
	TRAP(err, iRecorderUtility1 = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityMin));
	if (err != KErrNone)
		return EInconclusive;

	TRAP(err, iRecorderUtility2 = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityMax));
	if (err != KErrNone)
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
enum TVerdict CTestStepRecorderSimultaneousPlay5::DoTestStepPostambleL(void)
	{
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
void CTestStepRecorderSimultaneousPlay5::MoscoStateChangeEvent(CBase* aObject,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	INFO_PRINTF4(_L("MoscoStateChangeEvent(): Previous state = %d, current state = %d, error = %d"),
		aPreviousState,
		aCurrentState,
		aErrorCode);
	iError = aErrorCode;

	if (aObject == iRecorderUtility1)
		iCurrentState1 = aCurrentState;
	else if (aObject == iRecorderUtility2)
		iCurrentState2 = aCurrentState;


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
TVerdict CTestStepRecorderSimultaneousPlay5::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	INFO_PRINTF2(_L("File to play = %S"), &iFileName1);

	// Get the first recorder utility to open the sound file
	INFO_PRINTF1( _L("Initialize CMdaAudioRecorderUtility1"));
	TRAP(err, iRecorderUtility1->OpenFileL(iFileName1));
	if (err != KErrNone)
		return EInconclusive;
	// wait for open to complete
	CActiveScheduler::Start();
	if (iCurrentState1 != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	// start playing the first file and give it time to start
	INFO_PRINTF1( _L("Playing sound file (Utililty #1)...low priority"));
	TRAP(err, iRecorderUtility1->PlayL());
	if (err != KErrNone)
		return EInconclusive;
	CActiveScheduler::Start();
	if (!(iError == KErrNone && iCurrentState1 == CMdaAudioRecorderUtility::EPlaying))
		{
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	User::After(KQuarterScond);

	// Get the second recorder utility to open the sound file
	INFO_PRINTF1( _L("Initialize CMdaAudioRecorderUtility2"));
	TRAP(err, iRecorderUtility2->OpenFileL(iFileName1));
	if (err != KErrNone)
		return EInconclusive;
	// wait for open to complete
	CActiveScheduler::Start();
	if (iCurrentState2 != CMdaAudioRecorderUtility::EOpen)
		return EFail;


	// get the duration of each utility
	TTimeIntervalMicroSeconds duration1, duration2;

	duration1 = iRecorderUtility1->Duration();
	duration2 = iRecorderUtility2->Duration();

	INFO_PRINTF2( _L("CMdaAudioRecorderUtility1: reported duration = %d"), I64INT(duration1.Int64()));
	INFO_PRINTF2( _L("CMdaAudioRecorderUtility1: reported duration = %d"), I64INT(duration2.Int64()));

	if (duration1 != duration2)
		iTestStepResult = EFail;

	// stop playing
	iRecorderUtility1->Stop();

	INFO_PRINTF1(_L("finished with this test step"));

	// test steps return a result
	return iTestStepResult;
	}


//


/**
 *
 * Static constructor for CTestStepRecorderPlayTwice.
 *
 *
 * @return	"CTestStepRecorderPlayTwice*"
 *			The constructed CTestStepRecorderPlayTwice
 *
 * @xxxx
 *
 */
CTestStepRecorderPlayTwice* CTestStepRecorderPlayTwice::NewL()
	{
	CTestStepRecorderPlayTwice* self = new(ELeave) CTestStepRecorderPlayTwice;
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
CTestStepRecorderPlayTwice::CTestStepRecorderPlayTwice()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0820-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderPlayTwice::~CTestStepRecorderPlayTwice()
	{
	}

/**
 *
 * Test step Close
 * handles all deletion
 * @xxxx
 *
 */
void CTestStepRecorderPlayTwice::Close()
	{
	delete iRecorderUtility;
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderPlayTwice::DoTestStepPreambleL(void)
	{
	TInt err;

	TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this is a test of calling CMdaAudioRecorderUtility::PlayL() twice"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName)) // wav file
		{
		return EInconclusive;
		}

	// create a record utility
	TRAP(err, iRecorderUtility = CMdaAudioRecorderUtility::NewL(*this));
	if (err != KErrNone)
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
enum TVerdict CTestStepRecorderPlayTwice::DoTestStepPostambleL(void)
	{
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
void CTestStepRecorderPlayTwice::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	INFO_PRINTF4(_L("MoscoStateChangeEvent(): Previous state = %d, current state = %d, error = %d"),
		aPreviousState,
		aCurrentState,
		aErrorCode);
	iError = aErrorCode;
	iCurrentState = aCurrentState;
	CActiveScheduler::Stop();
	}


void CTestStepRecorderPlayTwice::MatoPrepareComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MatoPrepareComplete(): error = %d"), aError);
	iErrorTone = aError;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderPlayTwice::MatoPlayComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MatoPlayComplete(): error = %d"), aError);
	iErrorTone = aError;
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
TVerdict CTestStepRecorderPlayTwice::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;


	// open the sound file
	TRAP(err, iRecorderUtility->OpenFileL(iFileName));
	if (err != KErrNone)
		return EInconclusive;
	// wait for open to complete
	CActiveScheduler::Start();
	if (iCurrentState != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	TTime timeStart;
	TTime timeEnd;
	TTimeIntervalMicroSeconds playDuration1;
	TTimeIntervalMicroSeconds playDuration2;
	TTimeIntervalMicroSeconds position;
	const TInt KQuarterSecond = 250000;
	const TInt KHalfSecond = 500000;
	const TInt KTwoSeconds = 2000000;

	// Start playing the file and after two seconds get the position
	INFO_PRINTF1( _L("Playing file for the first time..."));
	timeStart.UniversalTime();	// start the timer
	TRAP(err, iRecorderUtility->PlayL());
	if (err != KErrNone)
		return EInconclusive;
	// wait once for start of play
	CActiveScheduler::Start();
	if (iError != KErrNone)
		iTestStepResult = EFail;
	if (iCurrentState != CMdaAudioRecorderUtility::EPlaying)
		return EFail;

	// check the position is close to the start of the file
	position = iRecorderUtility->Position();
	INFO_PRINTF2(_L("Start position = %d microseconds"), I64INT(position.Int64()));
	if (position.Int64() > KHalfSecond)
		{
		INFO_PRINTF1(_L("Start position is too high"));
		return EFail;
		}

	User::After(KTwoSeconds);
	// make sure we're still playing
	if (iCurrentState != CMdaAudioRecorderUtility::EPlaying)
		return EInconclusive;
	position = iRecorderUtility->Position();

	// wait once for end of play - this should produce KErrNone
	CActiveScheduler::Start();
	if (iError != KErrNone)
		iTestStepResult = EFail;
	if (iCurrentState != CMdaAudioRecorderUtility::EOpen)
		return EFail;
	timeEnd.UniversalTime();	// stop the timer

	// save the first play's duration
	playDuration1 = timeEnd.MicroSecondsFrom(timeStart);


	// Play the file again
	INFO_PRINTF1( _L("Playing file for the second time..."));
	timeStart.UniversalTime();	// start the timer
	TRAP(err, iRecorderUtility->PlayL());
	if (err != KErrNone)
		return EInconclusive;
	// wait once for start of play
	CActiveScheduler::Start();
	if (iError != KErrNone)
		iTestStepResult = EFail;
	if (iCurrentState != CMdaAudioRecorderUtility::EPlaying)
		return EFail;

	// check the position is close to the start of the file
	// and much less than 2 seconds
	position = iRecorderUtility->Position();
	INFO_PRINTF2(_L("Start position = %d microseconds"), I64INT(position.Int64()));
	if (position.Int64() > KHalfSecond)
		{
		INFO_PRINTF1(_L("Start position is too high"));
		return EFail;
		}

	// wait once for end of play - this should produce KErrNone
	CActiveScheduler::Start();
	if (iError != KErrNone)
		iTestStepResult = EFail;
	if (iCurrentState != CMdaAudioRecorderUtility::EOpen)
		return EFail;
	timeEnd.UniversalTime();	// stop the timer

	// save the second play's durarion
	playDuration2 = timeEnd.MicroSecondsFrom(timeStart);

	// Now compare how long it took to play the file in both cases
	// calculate the difference (hopefully pretty small)
	TInt64 playDurationDiff = playDuration2.Int64() - playDuration1.Int64();
	// make sure it's a positive number
	if (playDurationDiff < 0)
		playDurationDiff = -playDurationDiff;

	INFO_PRINTF2(_L("Duration #1 = %d microseconds"), playDuration1.Int64());
	INFO_PRINTF2(_L("Duration #2 = %d microseconds"), playDuration2.Int64());
	INFO_PRINTF2(_L("Difference  = %d microseconds"), playDurationDiff);
	if (playDurationDiff > KQuarterSecond)
		{
		INFO_PRINTF1(_L("Difference is too high"));
		iTestStepResult = EFail;
		}




	INFO_PRINTF1(_L("finished with this test step"));

	// test steps return a result
	return iTestStepResult;
	}



//


/**
 *
 * Static constructor for CTestStepPlayerPlayAndClearWindow.
 *
 *
 * @return	"CTestStepPlayerPlayAndClearWindow*"
 *			The constructed CTestStepPlayerPlayAndClearWindow
 *
 * @xxxx
 *
 */
CTestStepPlayerPlayAndClearWindow* CTestStepPlayerPlayAndClearWindow::NewL()
	{
	CTestStepPlayerPlayAndClearWindow* self = new(ELeave) CTestStepPlayerPlayAndClearWindow;
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
CTestStepPlayerPlayAndClearWindow::CTestStepPlayerPlayAndClearWindow()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0824-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepPlayerPlayAndClearWindow::~CTestStepPlayerPlayAndClearWindow()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepPlayerPlayAndClearWindow::DoTestStepPreambleL(void)
	{
	TInt err;

	TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this is a test of calling Play, clearing the play window"));
	INFO_PRINTF1(_L("and then playing again & verifying the whole clip is played"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName)) // wav file
		{
		return EInconclusive;
		}

	// create a play utility
	TRAP(err, iPlayerUtility = CMdaAudioPlayerUtility::NewL(*this));
	if (err != KErrNone)
		return EInconclusive;

	// create a recorder utility
	TRAP(err, iRecorderUtility = CMdaAudioRecorderUtility::NewL(*this));
	if (err != KErrNone)
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
enum TVerdict CTestStepPlayerPlayAndClearWindow::DoTestStepPostambleL(void)
	{
	delete iPlayerUtility;
	iPlayerUtility = NULL;
	delete iRecorderUtility;
	iRecorderUtility = NULL;

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
void CTestStepPlayerPlayAndClearWindow::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	INFO_PRINTF2(_L("MapcInitComplete(): error = %d"), aError);
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerPlayAndClearWindow::MapcPlayComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MapcPlayComplete(): error = %d"), aError);
	iError = aError;
	CActiveScheduler::Stop();
	}


void CTestStepPlayerPlayAndClearWindow::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	INFO_PRINTF4(_L("MoscoStateChangeEvent(): Previous state = %d, current state = %d, error = %d"),
		aPreviousState,
		aCurrentState,
		aErrorCode);
	iError = aErrorCode;
	iCurrentState = aCurrentState;
	CActiveScheduler::Stop();
	}


TVerdict CTestStepPlayerPlayAndClearWindow::DoTestStepPlayerL()
	{
	iTestStepResult = EPass;
    TInt err = KErrNone;

	const TInt KHalfSecond = 500000;

	INFO_PRINTF1(_L("Opening file using player utility..."));

	// open the sound file
	TRAP(err, iPlayerUtility->OpenFileL(iFileName));
	if (err != KErrNone)
		return EInconclusive;
	// wait for open to complete
	CActiveScheduler::Start();
	if (iError != KErrNone)
		return EFail;

	TTimeIntervalMicroSeconds duration = iPlayerUtility->Duration();
	TTimeIntervalMicroSeconds position;
	TTimeIntervalMicroSeconds playStart = duration.Int64()/2;
	TTimeIntervalMicroSeconds playEnd = duration;

	// sequence #1A
	// SetPlayWindow, Play & verify we play from start of window,stop
	INFO_PRINTF1(_L("Sequence #1A..."));
	INFO_PRINTF1(_L("calling SetPlayWindow..."));
	iPlayerUtility->SetPlayWindow(playStart, playEnd);
	INFO_PRINTF1(_L("calling play..."));
	iPlayerUtility->Play();

	INFO_PRINTF1(_L("calling GetPosition..."));
	iPlayerUtility->GetPosition(position);
	INFO_PRINTF2(_L("play position = %d"), position.Int64());
	if (position.Int64() < playStart.Int64())
		{
		INFO_PRINTF2(_L("play position is not at start of play window - %d"), position.Int64());
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	INFO_PRINTF1(_L("calling stop..."));
	iPlayerUtility->Stop();

	// sequence #1B
	// Play & verify we play from start of window,stop
	INFO_PRINTF1(_L("Sequence #1B..."));
	INFO_PRINTF1(_L("calling play..."));
	iPlayerUtility->Play();
	iPlayerUtility->GetPosition(position);
	INFO_PRINTF2(_L("play position = %d"), position.Int64());
	if (position.Int64() < playStart.Int64())
		{
		INFO_PRINTF2(_L("play position is not at start of play window - %d"), position.Int64());
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	// sequence #1C
	// ClearPlayWindow, Stop, play & verify we play from start of clip,stop
	INFO_PRINTF1(_L("Sequence #1C..."));
	INFO_PRINTF1(_L("calling ClearPlayWindow..."));
	iPlayerUtility->ClearPlayWindow();
	INFO_PRINTF1(_L("calling stop..."));
	iPlayerUtility->Stop();
	INFO_PRINTF1(_L("calling play..."));
	iPlayerUtility->Play();
	INFO_PRINTF1(_L("calling GetPosition..."));
	iPlayerUtility->GetPosition(position);
	INFO_PRINTF2(_L("play position = %d"), position.Int64());
	if (position.Int64() > KHalfSecond)
		{
		INFO_PRINTF2(_L("play position is not at start of clip - %d"), position.Int64());
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	INFO_PRINTF1(_L("calling stop..."));
	iPlayerUtility->Stop();

	// sequence #2
	// play, SetPlayWindow,stop,ClearPlayWindow,Play & verify we play from
	// start of clip
	INFO_PRINTF1(_L("Sequence #2..."));
	INFO_PRINTF1(_L("calling play..."));
	iPlayerUtility->Play();
	INFO_PRINTF1(_L("calling SetPlayWindow..."));
	iPlayerUtility->SetPlayWindow(playStart, playEnd);
	INFO_PRINTF1(_L("calling stop..."));
	iPlayerUtility->Stop();
	INFO_PRINTF1(_L("calling ClearPlayWindow..."));
	iPlayerUtility->ClearPlayWindow();
	INFO_PRINTF1(_L("calling play..."));
	iPlayerUtility->Play();
	INFO_PRINTF1(_L("calling GetPosition..."));
	iPlayerUtility->GetPosition(position);
	INFO_PRINTF2(_L("play position = %d"), position.Int64());
	if (position.Int64() > KHalfSecond)
		{
		INFO_PRINTF2(_L("play position is not at start of clip - %d"), position.Int64());
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	INFO_PRINTF1(_L("calling stop..."));
	iPlayerUtility->Stop();


	INFO_PRINTF1(_L("finished with this test step"));

	return iTestStepResult;
	}

TVerdict CTestStepPlayerPlayAndClearWindow::DoTestStepRecorderL()
	{
	iTestStepResult = EPass;
    TInt err = KErrNone;

	const TInt KHalfSecond = 500000;

	INFO_PRINTF1(_L("Opening file using recorder utility..."));

	// open the sound file
	TRAP(err, iRecorderUtility->OpenFileL(iFileName));
	if (err != KErrNone)
		return EInconclusive;
	// wait for open to complete
	CActiveScheduler::Start();
	if (iError != KErrNone)
		return EFail;

	TTimeIntervalMicroSeconds duration = iRecorderUtility->Duration();
	TTimeIntervalMicroSeconds position;
	TTimeIntervalMicroSeconds playStart = duration.Int64()/2;
	TTimeIntervalMicroSeconds playEnd = duration;

	// sequence #1A
	// SetPlayWindow, Play & verify we play from start of window,stop
	INFO_PRINTF1(_L("Sequence #1A..."));
	INFO_PRINTF1(_L("calling SetPlayWindow..."));
	iRecorderUtility->SetPlayWindow(playStart, playEnd);
	INFO_PRINTF1(_L("calling play..."));
	iRecorderUtility->PlayL();
	// wait for play to start
	CActiveScheduler::Start();
	if (iError != KErrNone)
		return EFail;

	INFO_PRINTF1(_L("calling GetPosition..."));
	position = iRecorderUtility->Position();
	INFO_PRINTF2(_L("play position = %d"), position.Int64());
	if (position.Int64() < playStart.Int64())
		{
		INFO_PRINTF2(_L("play position is not at start of play window - %d"), position.Int64());
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	INFO_PRINTF1(_L("calling stop..."));
	iRecorderUtility->Stop();

	// sequence #1B
	// Play & verify we play from start of window,stop
	INFO_PRINTF1(_L("Sequence #1B..."));
	INFO_PRINTF1(_L("calling play..."));
	iRecorderUtility->PlayL();
	// wait for play to start
	CActiveScheduler::Start();
	if (iError != KErrNone)
		return EFail;
	position = iRecorderUtility->Position();
	INFO_PRINTF2(_L("play position = %d"), position.Int64());
	if (position.Int64() < playStart.Int64())
		{
		INFO_PRINTF2(_L("play position is not at start of play window - %d"), position.Int64());
		iTestStepResult = EFail;
		return iTestStepResult;
		}

	// sequence #1C
	// ClearPlayWindow, Stop, play & verify we play from start of clip,stop
	INFO_PRINTF1(_L("Sequence #1C..."));
	INFO_PRINTF1(_L("calling ClearPlayWindow..."));
	iRecorderUtility->ClearPlayWindow();
	INFO_PRINTF1(_L("calling stop..."));
	iRecorderUtility->Stop();
	INFO_PRINTF1(_L("calling play..."));
	iRecorderUtility->PlayL();
	// wait for play to start
	CActiveScheduler::Start();
	if (iError != KErrNone)
		return EFail;
	INFO_PRINTF1(_L("calling GetPosition..."));
	position = iRecorderUtility->Position();
	INFO_PRINTF2(_L("play position = %d"), position.Int64());
	if (position.Int64() > KHalfSecond)
		{
		INFO_PRINTF2(_L("play position is not at start of clip - %d"), position.Int64());
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	INFO_PRINTF1(_L("calling stop..."));
	iRecorderUtility->Stop();

	// sequence #2
	// play, SetPlayWindow,stop,ClearPlayWindow,Play & verify we play from
	// start of clip
	INFO_PRINTF1(_L("Sequence #2..."));
	INFO_PRINTF1(_L("calling play..."));
	iRecorderUtility->PlayL();
	// wait for play to start
	CActiveScheduler::Start();
	if (iError != KErrNone)
		return EFail;
	INFO_PRINTF1(_L("calling SetPlayWindow..."));
	iRecorderUtility->SetPlayWindow(playStart, playEnd);
	INFO_PRINTF1(_L("calling stop..."));
	iRecorderUtility->Stop();
	INFO_PRINTF1(_L("calling ClearPlayWindow..."));
	iRecorderUtility->ClearPlayWindow();
	INFO_PRINTF1(_L("calling play..."));
	iRecorderUtility->PlayL();
	// wait for play to start
	CActiveScheduler::Start();
	if (iError != KErrNone)
		return EFail;
	INFO_PRINTF1(_L("calling GetPosition..."));
	position = iRecorderUtility->Position();
	INFO_PRINTF2(_L("play position = %d"), position.Int64());
	if (position.Int64() > KHalfSecond)
		{
		INFO_PRINTF2(_L("play position is not at start of clip - %d"), position.Int64());
		iTestStepResult = EFail;
		return iTestStepResult;
		}
	INFO_PRINTF1(_L("calling stop..."));
	iRecorderUtility->Stop();


	INFO_PRINTF1(_L("finished with this test step"));

	return iTestStepResult;
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
TVerdict CTestStepPlayerPlayAndClearWindow::DoTestStepL()
	{

	// run the same test with the player utility & then the recorded utility
	iTestStepResult = DoTestStepPlayerL();

	if (iTestStepResult == EPass)
		iTestStepResult = DoTestStepRecorderL();

	return iTestStepResult;
	}



//


/**
 *
 * Static constructor for CTestStepRecorderSetPosition.
 *
 *
 * @return	"CTestStepRecorderSetPosition*"
 *			The constructed CTestStepRecorderSetPosition
 *
 * @xxxx
 *
 */
CTestStepRecorderSetPosition* CTestStepRecorderSetPosition::NewL()
	{
	CTestStepRecorderSetPosition* self = new(ELeave) CTestStepRecorderSetPosition;
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
CTestStepRecorderSetPosition::CTestStepRecorderSetPosition()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0825-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 *
 */
CTestStepRecorderSetPosition::~CTestStepRecorderSetPosition()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 *
 */
enum TVerdict CTestStepRecorderSetPosition::DoTestStepPreambleL(void)
	{
	TInt err;

	TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this tests the responsiveness of CMdaAudioRecorderUtility Setposition()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName)) // wav file
		{
		return EInconclusive;
		}

	// create a record utility
	TRAP(err, iRecorderUtility = CMdaAudioRecorderUtility::NewL(*this));
	if (err != KErrNone)
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
enum TVerdict CTestStepRecorderSetPosition::DoTestStepPostambleL(void)
	{
	delete iRecorderUtility;
	iRecorderUtility = NULL;
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
void CTestStepRecorderSetPosition::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	iError = aErrorCode;
	iCurrentState = aCurrentState;
	CActiveScheduler::Stop();
	}


void CTestStepRecorderSetPosition::MatoPrepareComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MatoPrepareComplete(): error = %d"), aError);
	iErrorTone = aError;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderSetPosition::MatoPlayComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MatoPlayComplete(): error = %d"), aError);
	iErrorTone = aError;
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
TVerdict CTestStepRecorderSetPosition::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	const TInt KFiveSeconds = 5000000;
	const TInt KTwoSeconds = 2000000;
	const TInt KReplayedTime = KFiveSeconds - KTwoSeconds;

	INFO_PRINTF2( _L("Playing file for %d microseconds..."), KFiveSeconds);
	INFO_PRINTF2( _L("and then resetting position to %d microseconds from start of file..."), KTwoSeconds);

	// open the sound file
	TRAP(err, iRecorderUtility->OpenFileL(iFileName));
	if (err != KErrNone)
		return EInconclusive;
	// wait for open to complete
	CActiveScheduler::Start();
	if (iCurrentState != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	TTime timeStart;
	TTime timeEnd;
	TTimeIntervalMicroSeconds playDuration;
	TTimeIntervalMicroSeconds position;
// usually the WINS emulator responds within 300 ms, but about 30%
// of the time it takes significantly longer. Have chosen to allow this
// for emulator only (presumably it's down to other processes on the
// PC slowing things down...)
#ifdef __WINS__
	const TInt KAllowedDeviation = 750000;
#else
	const TInt KAllowedDeviation = 350000;
#endif

	// Start playing the file and after a while change the position
	TRAP(err, iRecorderUtility->PlayL());
	if (err != KErrNone)
		return EInconclusive;
	// wait once for start of play
	CActiveScheduler::Start();
	if (iError != KErrNone)
		iTestStepResult = EFail;
	if (iCurrentState != CMdaAudioRecorderUtility::EPlaying)
		return EFail;
	timeStart.UniversalTime();	// start the timer
	User::After(KFiveSeconds);
	// make sure we're still playing
	if (iCurrentState != CMdaAudioRecorderUtility::EPlaying)
		return EInconclusive;
	position = iRecorderUtility->Position();
	iRecorderUtility->SetPosition(TTimeIntervalMicroSeconds(KTwoSeconds));

	// wait once for end of play - this should produce KErrNone
	CActiveScheduler::Start();
	if (iError != KErrNone)
		iTestStepResult = EFail;
	if (iCurrentState != CMdaAudioRecorderUtility::EOpen)
		return EFail;
	timeEnd.UniversalTime();	// stop the timer

	// save the first play's duration
	playDuration = timeEnd.MicroSecondsFrom(timeStart);


	TTimeIntervalMicroSeconds clipDuration = iRecorderUtility->Duration();
	TTimeIntervalMicroSeconds expectedDuration =
		TTimeIntervalMicroSeconds(clipDuration.Int64() + KReplayedTime);

	// Now compare how long it took to play the file
	TInt64 playDurationDiff = expectedDuration.Int64() - playDuration.Int64();
	// make sure it's a positive number
	if (playDurationDiff < 0)
		playDurationDiff = -playDurationDiff;

	INFO_PRINTF2(_L("Clip duration = %d microseconds"), I64LOW(clipDuration.Int64()));
	INFO_PRINTF2(_L("Expected play duration = %d microseconds"), I64LOW(expectedDuration.Int64()));
	INFO_PRINTF2(_L("Time to play = %d microseconds"), playDuration.Int64());

	INFO_PRINTF2(_L("Difference  = %d microseconds"), playDurationDiff);
	if (playDurationDiff > KAllowedDeviation)
		{
		INFO_PRINTF1(_L("Difference is too high"));
		iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));

	// test steps return a result
	return iTestStepResult;
	}



