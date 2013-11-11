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
 * Static constructor for CTestStepRecorderType.
 *
 *
 * @return	"CTestStepRecorderType*"
 *			The constructed CTestStepRecorderType
 *
 * @xxxx
 * 
 */
CTestStepRecorderType* CTestStepRecorderType::NewL()
	{
	CTestStepRecorderType* self = new(ELeave) CTestStepRecorderType;
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
CTestStepRecorderType::CTestStepRecorderType()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0253-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderType::~CTestStepRecorderType()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderType::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0253-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::TypeL()"));
	INFO_PRINTF1(_L("function deprecated testing returns INCONCLUSIVE without a test"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName)) // wav file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName3"), iFileName2)) // au file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
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
enum TVerdict CTestStepRecorderType::DoTestStepPostambleL(void)
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
void CTestStepRecorderType::MoscoStateChangeEvent(CBase* /*aObject*/,
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
TVerdict CTestStepRecorderType::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// test WAV TYPE
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	iRecorder->OpenFileL( iFileName );
	CActiveScheduler::Start(); 
	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;
	/*
	TRAP( err, iAudType = iRecorder->TypeL()) );
	if (iAudType == NULL ||
		err != KErrNone ||
		iRecorder->iFormat != TMdaWavClipFormat )
		return EFail;
		delete iAudType;
		iAudType = NULL;
	*/
		delete iRecorder;
		iRecorder = NULL;

	// test AU TYPE
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	iRecorder->OpenFileL( iFileName2 );
	CActiveScheduler::Start(); 
	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;
	/*
	TRAP( err, iAudType = iRecorder->TypeL()) );
	if (iAudType == NULL ||
		err != KErrNone ||
		iRecorder->iFormat != TMdaAuClipFormat )
		return EFail;
		delete iAudType;
		iAudType = NULL;
	*/
		delete iRecorder;
		iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

