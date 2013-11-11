// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepRecorderSetBalRecordGetBal.
 *
 *
 * @return	"CTestStepRecorderSetBalRecordGetBal*"
 *			The constructed CTestStepRecorderSetBalRecordGetBal
 *
 * @xxxx
 * 
 */
CTestStepRecorderSetBalRecordGetBal* CTestStepRecorderSetBalRecordGetBal::NewL()
	{
	CTestStepRecorderSetBalRecordGetBal* self = new(ELeave) CTestStepRecorderSetBalRecordGetBal;
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
CTestStepRecorderSetBalRecordGetBal::CTestStepRecorderSetBalRecordGetBal()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-1003-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderSetBalRecordGetBal::~CTestStepRecorderSetBalRecordGetBal()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 TVerdict CTestStepRecorderSetBalRecordGetBal::DoTestStepPreambleL(void)
	{
	TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-1003-HP"));
	INFO_PRINTF1(_L("this test is setting/getting the Play balance of the recorder utility with several values. [Set|Get]PlaybackBalance()"));
	iError = KErrNone; 
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) )
		{
		return EInconclusive;
		}
	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
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
 TVerdict CTestStepRecorderSetBalRecordGetBal::DoTestStepPostambleL(void)
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
void CTestStepRecorderSetBalRecordGetBal::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntRecFile : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);	
	if(aPreviousState == CMdaAudioClipUtility::EPlaying || aCurrentState == CMdaAudioClipUtility::EOpen)
		{
		CActiveScheduler::Stop();
		}
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
TVerdict CTestStepRecorderSetBalRecordGetBal::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt balance =KMMFBalanceMaxLeft; 
	
	err = iRecorder->SetPlaybackBalance(balance); // Set Balance to Left Extreme
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("failed to set the playback balance"));
		return EFail;
		}
	
	err = iRecorder->GetPlaybackBalance(balance); // This must be as set earlier
	if ( (err != KErrNone) || (balance != KMMFBalanceMaxLeft ) )
		{
		return EFail;	
		}
	iRecorder->PlayL();
	CActiveScheduler::Start();
	err = iRecorder->GetPlaybackBalance(balance); // balance here must be same as we set before PlayL
	if ( (err != KErrNone) || (balance != KMMFBalanceMaxLeft) )
		{
		return EFail;		
		}
	// Everything Fine
	// Now Lets Try Setting on Extreme Right
	balance = KMMFBalanceMaxRight; 
	err = iRecorder->SetPlaybackBalance(balance); // Set Balance to Right Extreme
	if (err != KErrNone)
		{
		return EFail;	
		}

	err = iRecorder->GetPlaybackBalance(balance); // This must be as set earlier
	if ( (err != KErrNone) || (balance != KMMFBalanceMaxRight) )
		{
		return EFail;	
		}
	iRecorder->PlayL();
	CActiveScheduler::Start();
	err = iRecorder->GetPlaybackBalance(balance); // balance here must be same as we set before PlayL
	if ( (err != KErrNone ) || (balance != KMMFBalanceMaxRight) )
		{
		return EFail;		
		}
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
