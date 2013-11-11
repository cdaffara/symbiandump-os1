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
 * Static constructor for CTestStepToneSetPriority.
 *
 *
 * @return	"CTestStepToneSetPriority*"
 *			The constructed CTestStepToneSetPriority
 *
 * @xxxx
 * 
 */
CTestStepToneSetPriority* CTestStepToneSetPriority::NewL()
	{
	CTestStepToneSetPriority* self = new(ELeave) CTestStepToneSetPriority;
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
CTestStepToneSetPriority::CTestStepToneSetPriority() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0460-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepToneSetPriority::~CTestStepToneSetPriority()
	{
	}

void CTestStepToneSetPriority::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepToneSetPriority::MatoPlayComplete(TInt aError)
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
enum TVerdict CTestStepToneSetPriority::DoTestStepPreambleL(void)
	{
    TInt err       = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0460-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioToneUtility::SetPriority() "));
	
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
enum TVerdict CTestStepToneSetPriority::DoTestStepPostambleL(void)
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
TVerdict CTestStepToneSetPriority::DoTestStepL()
	{
	iTestStepResult = EPass;
	
	iTone->SetPriority(EMdaPriorityNormal, EMdaPriorityPreferenceQuality);

	TInt err = KErrNone;
	TMMFPrioritySettings priorSets;
	err = GetAudTonePriority(iTone, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityNormal ||
		priorSets.iPref != EMdaPriorityPreferenceQuality)
		return EFail;

	iTone->SetPriority(EMdaPriorityNormal, EMdaPriorityPreferenceTime);
	err = GetAudTonePriority(iTone, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityNormal ||
		priorSets.iPref != EMdaPriorityPreferenceTime)
		return EFail;

	iTone->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality);
	err = GetAudTonePriority(iTone, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityMin ||
		priorSets.iPref != EMdaPriorityPreferenceTimeAndQuality)
		return EFail;

	iTone->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality);
	err = GetAudTonePriority(iTone, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityMin ||
		priorSets.iPref != EMdaPriorityPreferenceTimeAndQuality)
		return EFail;

	iTone->SetPriority(EMdaPriorityMax, EMdaPriorityPreferenceTimeAndQuality);
	err = GetAudTonePriority(iTone, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityMax ||
		priorSets.iPref != EMdaPriorityPreferenceTimeAndQuality)
		return EFail;

	iTone->SetPriority(EMdaPriorityMax, EMdaPriorityPreferenceTimeAndQuality);
	err = GetAudTonePriority(iTone, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityMax ||
		priorSets.iPref != EMdaPriorityPreferenceTimeAndQuality)
		return EFail;
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

