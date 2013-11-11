// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "../../ctlfrm/TS_Codes.h"


/**
 *
 * Static constructor for CTestStepRecorderOpenTryNextCtrl.
 *
 *
 * @return	"CTestStepRecorderOpenTryNextCtrl*"
 *			The constructed CTestStepRecorderOpenTryNextCtrl
 *
 * @xxxx
 * 
 */
CTestStepRecorderOpenTryNextCtrl* CTestStepRecorderOpenTryNextCtrl::NewL()
	{
	CTestStepRecorderOpenTryNextCtrl* self = new(ELeave) CTestStepRecorderOpenTryNextCtrl;
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
CTestStepRecorderOpenTryNextCtrl::CTestStepRecorderOpenTryNextCtrl()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0960-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderOpenTryNextCtrl::~CTestStepRecorderOpenTryNextCtrl()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderOpenTryNextCtrl::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0960-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::OpenUrlL()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioURLTestNextController"), iURL) )
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
enum TVerdict CTestStepRecorderOpenTryNextCtrl::DoTestStepPostambleL(void)
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
void CTestStepRecorderOpenTryNextCtrl::MoscoStateChangeEvent(CBase* /*aObject*/,
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
TVerdict CTestStepRecorderOpenTryNextCtrl::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// create the Recorder utility
	TRAP(err, iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this));

	if (err != KErrNone)
		{
		return EInconclusive;
		}
	
	TRAP(err, SetTestMediaIdL(iRecorder));	
	
	if (err != KErrNone)
		{
		return EInconclusive;
		}
		
	// load matching to filename 
	TRAP(err,iRecorder->OpenUrlL(iURL, KTestIap ));
	
	if (err!=KErrNone)
		{
		return EInconclusive;
		}
	
	CActiveScheduler::Start(); 

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNotSupported )
		{
		return EFail;
		}
	//need to remove <n>yes tag from 101f7790.RSS to really check for Trying Next Ctrl.
	//in that case, KErrNone returned
	if (iError == KErrNotSupported)
		{
		iTestStepResult = EPass;
		}
	else
		{
		iTestStepResult = EFail;	
		}

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

