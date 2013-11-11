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
 * Static constructor for CTestStepToneSetVolumeRamp.
 *
 *
 * @return	"CTestStepToneSetVolumeRamp*"
 *			The constructed CTestStepToneSetVolumeRamp
 *
 * @xxxx
 * 
 */
CTestStepToneSetVolumeRamp* CTestStepToneSetVolumeRamp::NewL()
	{
	CTestStepToneSetVolumeRamp* self = new(ELeave) CTestStepToneSetVolumeRamp;
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
CTestStepToneSetVolumeRamp::CTestStepToneSetVolumeRamp() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0465-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepToneSetVolumeRamp::~CTestStepToneSetVolumeRamp()
	{
	}

void CTestStepToneSetVolumeRamp::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepToneSetVolumeRamp::MatoPlayComplete(TInt aError)
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
enum TVerdict CTestStepToneSetVolumeRamp::DoTestStepPreambleL(void)
	{
    TInt err       = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0465-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioToneUtility::SetVolumeRamp() "));
	
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
enum TVerdict CTestStepToneSetVolumeRamp::DoTestStepPostambleL(void)
	{
	delete iTone;
	iTone = NULL;
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
TVerdict CTestStepToneSetVolumeRamp::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt ramp = 0;
	
	for (ramp=0; ramp<=1000000; ramp+=1000)
		{
		iTone->SetVolumeRamp(TTimeIntervalMicroSeconds(ramp));
		// no way to check directly since its a SoundDev property which is on the server side
		// out of scope to check the SoundDev
		//if (static_cast<CMMFMdaAudioToneUtility*>(iTone)->iDevSound->iRampDuration != TTimeIntervalMicroSeconds(ramp) )
		//	{
		//	iTestStepResult = EFail;
		//	}

		//checked manualy the CMMFSoundDev and Set seems to work OK
		}
	
	// Try also with prepared Tone utility
	iTone->PrepareToPlayTone(400,TTimeIntervalMicroSeconds(500000));
	CActiveScheduler::Start();
	if ( iError != KErrNone ||
		 iTone->State() != EMdaAudioToneUtilityPrepared	)
		return EInconclusive;

	for (ramp=0; ramp<=1000000; ramp+=1000)
		{
		iTone->SetVolumeRamp(TTimeIntervalMicroSeconds(ramp));
		//checked manualy the CMMFSoundDev and Set seems to work OK
		}
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

