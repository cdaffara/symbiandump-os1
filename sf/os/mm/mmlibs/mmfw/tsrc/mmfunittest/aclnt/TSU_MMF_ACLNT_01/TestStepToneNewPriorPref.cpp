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
// TestStepToneAllocFailNewPriorPref.cpp
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
 * Static constructor for CTestStepToneNewPriorPref.
 *
 *
 * @return	"CTestStepToneNewPriorPref*"
 *			The constructed CTestStepToneNewPriorPref
 *
 * @xxxx
 * 
 */
CTestStepToneNewPriorPref* CTestStepToneNewPriorPref::NewL()
	{
	CTestStepToneNewPriorPref* self = new(ELeave) CTestStepToneNewPriorPref;
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
CTestStepToneNewPriorPref::CTestStepToneNewPriorPref() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0453-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepToneNewPriorPref::~CTestStepToneNewPriorPref()
	{
	}

void CTestStepToneNewPriorPref::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepToneNewPriorPref::MatoPlayComplete(TInt aError)
	{
	iError = aError;
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
TVerdict CTestStepToneNewPriorPref::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0453-HP"));
	INFO_PRINTF1(_L("this is a test of CMMFMdaAudioToneUtility::NewL(<Priority and Preference>)"));
	
	__MM_HEAP_MARK;
	// test 1 //
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this, NULL, EMdaPriorityMin)) );
	// check for error
	if (err != KErrNone ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityNotReady)
		iTestStepResult = EFail;
	delete iTone;
	iTone = NULL;
	// test 2 //
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this, NULL, EMdaPriorityMax)) );
	// check for error
	if (err != KErrNone ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityNotReady)
		iTestStepResult = EFail;
	delete iTone;
	iTone = NULL;
	// test 3 //
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this, NULL, EMdaPriorityMin,
													EMdaPriorityPreferenceTime)) );
	// check for error
	if (err != KErrNone ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityNotReady)
		iTestStepResult = EFail;
	delete iTone;
	iTone = NULL;
	// test 4 //
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this, NULL, EMdaPriorityMin,
													EMdaPriorityPreferenceQuality)) );
	// check for error
	if (err != KErrNone ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityNotReady)
		iTestStepResult = EFail;
	delete iTone;
	iTone = NULL;
	// test 5 //
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this, NULL, EMdaPriorityMax,
													EMdaPriorityPreferenceTime)) );
	// check for error
	if (err != KErrNone ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityNotReady)
		iTestStepResult = EFail;
	delete iTone;
	iTone = NULL;
	// test 6 //
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this, NULL, EMdaPriorityMax,
													EMdaPriorityPreferenceQuality)) );
	// check for error
	if (err != KErrNone ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityNotReady)
		iTestStepResult = EFail;
	delete iTone;
	iTone = NULL;
	__MM_HEAP_MARKEND;
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

