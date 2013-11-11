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
 * Static constructor for CTestStepRecorderGain.
 *
 *
 * @return	"CTestStepRecorderGain*"
 *			The constructed CTestStepRecorderGain
 *
 * @xxxx
 * 
 */
CTestStepRecorderGain* CTestStepRecorderGain::NewL()
	{
	CTestStepRecorderGain* self = new(ELeave) CTestStepRecorderGain;
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
CTestStepRecorderGain::CTestStepRecorderGain() : iError(0)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0249-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderGain::~CTestStepRecorderGain()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderGain::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0249-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::[Get]GainL"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	// open an existing file to record and crop all its contents if any
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start(); 

	if (iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	iGain = iRecorder->MaxGain()/2;
	iRecorder->SetGain(iGain);
	
	if ( iError != KErrNone || 
		 iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
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
enum TVerdict CTestStepRecorderGain::DoTestStepPostambleL(void)
	{
	//delete the output file 
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

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
void CTestStepRecorderGain::MoscoStateChangeEvent(CBase* /*aObject*/,
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
TVerdict CTestStepRecorderGain::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
	TInt gain;

	err = iRecorder->GetGain(gain);

	// check result
	if (err != KErrNone ||
		gain != iGain ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EFail;

	iRecorder->Close();
	delete iRecorder;
	iRecorder = NULL;

	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

