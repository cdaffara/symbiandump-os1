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

#include <mdaaudiotoneplayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepTone.h"


//#include <mmfaudiocontroller.h>
#include "mmfclienttoneplayer.h"
//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepToneCancelPlay.
 *
 *
 * @return	"CTestStepToneCancelPlay*"
 *			The constructed CTestStepToneCancelPlay
 *
 * @xxxx
 * 
 */
CTestStepToneCancelPlay* CTestStepToneCancelPlay::NewL()
	{
	CTestStepToneCancelPlay* self = new(ELeave) CTestStepToneCancelPlay;
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
CTestStepToneCancelPlay::CTestStepToneCancelPlay() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0458-CP");
	
	// need a bigger heap size on EKA2 HW
#if !defined __WINS__
	iHeapSize = KHeapSizeToneTestEKA2;
#endif // EKA2
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepToneCancelPlay::~CTestStepToneCancelPlay()
	{
	}

void CTestStepToneCancelPlay::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepToneCancelPlay::MatoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepToneCancelPlay::DoTestStepPreambleL(void)
	{
    TInt err       = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0458-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioToneUtility::CancelPlay() "));
	
	// create the Tone utility
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	if ( err != KErrNone ||
		 iTone->State() != EMdaAudioToneUtilityNotReady	)
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
enum TVerdict CTestStepToneCancelPlay::DoTestStepPostambleL(void)
	{
	delete iTone;
	iTone = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

LOCAL_C TInt stopActiveScheduler(TAny*)
	{
	CActiveScheduler::Stop();
	return FALSE;
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
TVerdict CTestStepToneCancelPlay::DoTestStepL()
	{
	iTestStepResult = EPass;

	INFO_PRINTF1(_L("SEQUENCE #1 - Call PrepareToPlayTone()->Play()->CancelPlay()"));

	INFO_PRINTF1(_L("Calling PrepareToPlayTone()"));
	iTone->PrepareToPlayTone(400,TTimeIntervalMicroSeconds(5000000));
	CActiveScheduler::Start(); // Wait for prepare to complete
	if (iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityPrepared	)
		return EInconclusive;

	INFO_PRINTF1(_L("Calling Play()"));
	iTone->Play();
	CCallBackTimer* callbackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	// wait 1 second and cancel the sound after 1/5 of its duration
	callbackTimer->After(1000000);
	CActiveScheduler::Start();
	if (callbackTimer->IsActive() || iTone->State() != EMdaAudioToneUtilityPlaying)
		{
		INFO_PRINTF1(_L("Something else has stopped the AS"));
		return EFail; // something else has stopped the AS
		}

	delete callbackTimer;

	INFO_PRINTF1(_L("Calling CancelPlay()"));
	iTone->CancelPlay();
	if (iTone->State() == EMdaAudioToneUtilityPlaying)
		{
		INFO_PRINTF2(_L("iTone->State() == %d"), iTone->State());
		return EFail;
		}

	// Testing the fix for DEF044077
	INFO_PRINTF1(_L("SEQUENCE #2 - Call PrepareToPlayTone()->Play()->CancelPlay()->PrepareToPlayTone()"));

	INFO_PRINTF1(_L("Calling PrepareToPlayTone()"));
	iTone->PrepareToPlayTone(400,TTimeIntervalMicroSeconds(500));
	CActiveScheduler::Start(); // Wait for prepare to complete
	if (iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityPrepared	)
		return EInconclusive;

	INFO_PRINTF1(_L("Calling Play()"));
	iTone->Play();

	INFO_PRINTF1(_L("Calling CancelPlay()"));
	iTone->CancelPlay();
	if (iTone->State() == EMdaAudioToneUtilityPlaying)
		{
		INFO_PRINTF2(_L("iTone->State() == %d"), iTone->State());
		return EFail;
		}

	INFO_PRINTF1(_L("Calling PrepareToPlayTone()"));	
	iTone->PrepareToPlayTone(400,TTimeIntervalMicroSeconds(500));

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

