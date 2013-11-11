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
// This file contains the implementation of CTestStepToneCancelPrepare class
// that tests fix for DEF044077( - CMdaAudioToneUtility::CancelPrepare() & CancelPlay() 
// do not cancel internal AO) by calling PrepareToPlayTone()->CancelPrepare()->PrepareToPlayTone().
// 
//

// EPOC includes
#include <testframework.h>

#include <mdaaudiotoneplayer.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepTone.h"
#include "mmfclienttoneplayer.h"

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepToneCancelPrepare.
 *
 *
 * @return	"CTestStepToneCancelPrepare*"
 *			The constructed CTestStepToneCancelPrepare
 *
 * @xxxx
 * 
 */
CTestStepToneCancelPrepare* CTestStepToneCancelPrepare::NewL()
	{
	CTestStepToneCancelPrepare* self = new(ELeave) CTestStepToneCancelPrepare;
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
CTestStepToneCancelPrepare::CTestStepToneCancelPrepare() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0480-CP");
	
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
CTestStepToneCancelPrepare::~CTestStepToneCancelPrepare()
	{
	}

void CTestStepToneCancelPrepare::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepToneCancelPrepare::MatoPlayComplete(TInt aError)
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
TVerdict CTestStepToneCancelPrepare::DoTestStepPreambleL(void)
	{
    TInt err = KErrNone;

	// this installs the scheduler
	TVerdict verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0480-CP"));
	INFO_PRINTF1(_L("This is a test of PrepareToPlayTone()->CancelPlay()->PrepareToPlayTone()"));
	
	// create the Tone utility
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	if ( err != KErrNone ||
		 iTone->State() != EMdaAudioToneUtilityNotReady	)
		{
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
TVerdict CTestStepToneCancelPrepare::DoTestStepPostambleL(void)
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
TVerdict CTestStepToneCancelPrepare::DoTestStepL()
	{
	iTestStepResult = EPass;

	INFO_PRINTF1(_L("Calling PrepareToPlayTone()"));
	iTone->PrepareToPlayTone(400,TTimeIntervalMicroSeconds(500));

	// Cancel prepare
	INFO_PRINTF1(_L("Calling CancelPrepare()"));
	iTone->CancelPrepare();

	if (iTone->State() == EMdaAudioToneUtilityPrepared)
		{
		INFO_PRINTF2(_L("iTone->State() == %d"), iTone->State());
		return EFail;
		}

	// Call PrepareToPlayTone() Again.
	INFO_PRINTF1(_L("Calling PrepareToPlayTone()"));
	iTone->PrepareToPlayTone(400,TTimeIntervalMicroSeconds(500));

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

