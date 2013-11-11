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
// TestStepPlayerPlay.cpp
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
#include "TestStepPlayerNew.h"

#include "mmfclientaudioplayer.h"

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepPlayerPlayAu.
 *
 *
 * @return	"CTestStepPlayerPlayAu*"
 *			The constructed CTestStepPlayerPlayAu
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayAu* CTestStepPlayerPlayAu::NewL()
	{
	CTestStepPlayerPlayAu* self = new(ELeave) CTestStepPlayerPlayAu;
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
CTestStepPlayerPlayAu::CTestStepPlayerPlayAu() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0011-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayAu::~CTestStepPlayerPlayAu()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerPlayAu::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerPlayAu::MapcPlayComplete(TInt aError)
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
 enum TVerdict CTestStepPlayerPlayAu::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0011-CP"));
	 INFO_PRINTF1(_L("this test is playing a sound file after openning it. Play()"));

	 iError = KErrNone;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName3"), iFileName))
		{
		INFO_PRINTF1(_L("File name not found in INI file."));
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
enum TVerdict CTestStepPlayerPlayAu::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerPlayAu::DoTestStepL()
	{
	//__MM_HEAP_MARK;

	// check if player openned the file successfully
	TInt err1 = KErrNone;
	TInt state = 0;
	err1 = GetAudPlayerState(iPlayer, state);
	if ( state != CMMFMdaAudioPlayerUtility::EStopped )
		return EInconclusive;

	iPlayer->Play();
	CActiveScheduler::Start();

	User::After(2000000);

	TInt err2 = KErrNone;
	TMMFMessageDestination* srcHandle = NULL;
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 !iHasPlayed ||
		 iError != KErrNone)
		 iTestStepResult = EFail;


	//__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

