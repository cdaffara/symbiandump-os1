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

#include <mda/common/resource.h>
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
 * Static constructor for CTestStepToneSetRepeats.
 *
 *
 * @return	"CTestStepToneSetRepeats*"
 *			The constructed CTestStepToneSetRepeats
 *
 * @xxxx
 * 
 */
CTestStepToneSetRepeats* CTestStepToneSetRepeats::NewL()
	{
	CTestStepToneSetRepeats* self = new(ELeave) CTestStepToneSetRepeats;
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
CTestStepToneSetRepeats::CTestStepToneSetRepeats() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0464-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepToneSetRepeats::~CTestStepToneSetRepeats()
	{
	}

void CTestStepToneSetRepeats::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepToneSetRepeats::MatoPlayComplete(TInt aError)
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
enum TVerdict CTestStepToneSetRepeats::DoTestStepPreambleL(void)
	{
    TInt err       = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0464-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioToneUtility::SetRepeats() "));
	
	// create the Recorder utility
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	if ( err != KErrNone )
		return EInconclusive;
	iTone->PrepareToPlayTone(400,TTimeIntervalMicroSeconds(500000));
	CActiveScheduler::Start();

	if (iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityPrepared)
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
enum TVerdict CTestStepToneSetRepeats::DoTestStepPostambleL(void)
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
TVerdict CTestStepToneSetRepeats::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err;

	for (TInt rep=0; rep<=10; rep++)
		{
		TRAP(err, iTone->SetRepeats(rep,TTimeIntervalMicroSeconds(500000)) );
		//CActiveScheduler::Start();

		// check result
		// we should check the internal status of the CMMFDevSound object
		// but this is out of scope
		if (err != KErrNone ||
			iTone->State() != EMdaAudioToneUtilityPrepared)
			return EFail;
		}
	// try also KMdaRepeatForever
	TRAP(err, iTone->SetRepeats(KMdaRepeatForever,TTimeIntervalMicroSeconds(500000)) );
	//CActiveScheduler::Start();

	// check result
	// we should check the internal status of the CMMFDevSound object
	// but this is out of scope
	if (err != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityPrepared)
		return EFail;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

