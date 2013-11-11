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
 * Static constructor for CTestStepRecorderSetPriority.
 *
 *
 * @return	"CTestStepRecorderSetPriority*"
 *			The constructed CTestStepRecorderSetPriority
 *
 * @xxxx
 * 
 */
CTestStepRecorderSetPriority* CTestStepRecorderSetPriority::NewL()
	{
	CTestStepRecorderSetPriority* self = new(ELeave) CTestStepRecorderSetPriority;
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
CTestStepRecorderSetPriority::CTestStepRecorderSetPriority()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0259-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderSetPriority::~CTestStepRecorderSetPriority()
	{
	}

void CTestStepRecorderSetPriority::MoscoStateChangeEvent(CBase* /*aObject*/,
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
TVerdict CTestStepRecorderSetPriority::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0259-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::SetPriority() "));
	
	__MM_HEAP_MARK;
	TRAP( err, (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) );
	if (err != KErrNone)
		return EInconclusive;

	// test 1
	iRecorder->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTime);

	TMMFPrioritySettings prior;
	TInt err2 = GetAudRecorderPrioritySettings(iRecorder, prior);
	if ( err != KErrNone || 
		 err2 != KErrNone ||
		 prior.iPriority != EMdaPriorityMin ||
		 prior.iPref != EMdaPriorityPreferenceTime)
		 return EFail;
	else 
		 iTestStepResult = EPass;

	// try more values if there is some more time

	delete iRecorder;
	iRecorder = NULL;
	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

