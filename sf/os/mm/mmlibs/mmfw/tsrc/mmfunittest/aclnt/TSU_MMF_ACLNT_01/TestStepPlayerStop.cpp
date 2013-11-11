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

#include <mdaaudiosampleplayer.h>
//#include <MdaAudioSampleEditor.h>
//#include <MdaAudioTonePlayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepPlayerManip.h"

#include "mmfclientaudioplayer.h"

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepPlayerStop.
 *
 *
 * @return	"CTestStepPlayerStop*"
 *			The constructed CTestStepPlayerStop
 *
 * @xxxx
 * 
 */
CTestStepPlayerStop* CTestStepPlayerStop::NewL()
	{
	CTestStepPlayerStop* self = new(ELeave) CTestStepPlayerStop;
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
CTestStepPlayerStop::CTestStepPlayerStop() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0032-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerStop::~CTestStepPlayerStop()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerStop::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerStop::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	if (iError == KErrNone)
		iHasPlayed = ETrue;
	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepPlayerStop::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0032-CP"));
	 INFO_PRINTF1(_L("this test is playing and stopping a sound file after openning it. Stop()"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	 iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	 CActiveScheduler::Start();
	 User::After(1000000);

	 if (iPlayer == NULL ||
		 iError != KErrNone )
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
enum TVerdict CTestStepPlayerStop::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
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
TVerdict CTestStepPlayerStop::DoTestStepL()
	{
	TInt err;
	//__MM_HEAP_MARK;

	// check if player openned the file successfully
	TInt err1 = KErrNone;
	TInt state = 0;
	err1 = GetAudPlayerState(iPlayer, state);
	if ( state != CMMFMdaAudioPlayerUtility::EStopped )
		return EInconclusive;

	iPlayer->Play();
	//CActiveScheduler::Start();
	// stop it after 500 milisecs
	User::After(500000);
	err = KErrNone;
	TRAP(err, iPlayer->Stop());

	// check 
	TInt err2 = KErrNone;
	TMMFMessageDestination* srcHandle = NULL;
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 err != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		 iTestStepResult = EFail;


	delete iPlayer;
	iPlayer = NULL;
	//__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

