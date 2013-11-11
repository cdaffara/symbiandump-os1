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
 * Static constructor for CTestStepPlayerVolRamp.
 *
 *
 * @return	"CTestStepPlayerVolRamp*"
 *			The constructed CTestStepPlayerVolRamp
 *
 * @xxxx
 * 
 */
CTestStepPlayerVolRamp* CTestStepPlayerVolRamp::NewL()
	{
	CTestStepPlayerVolRamp* self = new(ELeave) CTestStepPlayerVolRamp;
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
CTestStepPlayerVolRamp::CTestStepPlayerVolRamp()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0037-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerVolRamp::~CTestStepPlayerVolRamp()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerVolRamp::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerVolRamp::MapcPlayComplete(TInt aError)
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
 enum TVerdict CTestStepPlayerVolRamp::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0037-HP"));
	 INFO_PRINTF1(_L("this test is playing a sound with several Volume Ramp levels. SetVolumeRamp(...)"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	CActiveScheduler::Start();
	User::After(1000000);
	TInt err1 = KErrNone;
	TInt err2 = KErrNone;
	TInt state = 0;
	TMMFMessageDestination* srcHandle = NULL;
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iPlayer->Duration() == TTimeIntervalMicroSeconds(0) || //we know the sound file has duration >0
		 iError != KErrNone)
		return EInconclusive;

	iDuration = new TTimeIntervalMicroSeconds(iPlayer->Duration());

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerVolRamp::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerVolRamp::DoTestStepL()
	{
	iTestStepResult = EPass;
	//__MM_HEAP_MARK;
	// check if player openned the sound file successfully
	TInt err1 = KErrNone;
	TInt err2 = KErrNone;
	TInt state = 0;
	TMMFMessageDestination* srcHandle = NULL;
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		return EInconclusive;

	// try zero ramp
	iPlayer->SetVolumeRamp(TTimeIntervalMicroSeconds(0));
	iPlayer->Play();
	CActiveScheduler::Start();
	User::After(1200000); // time enough to play it 
	err1 = GetAudPlayerState(iPlayer, state);
	if ( err1 != KErrNone ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		return EFail;
	delete iPlayer;
	iPlayer = NULL;

	// problem of the Audio Clients to be fixed
	// Cannot Play several TImes sound if the player object is not created again....

	// try several volume values and play
	for (TInt ramp=1 ; ramp<=1000000 ; ramp*=10 )
		{
		iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
		CActiveScheduler::Start();
		User::After(1000000);
		err1 = GetAudPlayerState(iPlayer, state);
		err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
		if ( err1 != KErrNone ||
			 err2 != KErrNone ||
			 &srcHandle == NULL ||
			 state != CMMFMdaAudioPlayerUtility::EStopped ||
			 iPlayer->Duration() == TTimeIntervalMicroSeconds(0) || //we know the sound file has duration >0
			 iError != KErrNone)
			return EInconclusive;

		iPlayer->SetVolumeRamp(TTimeIntervalMicroSeconds(ramp));
		iPlayer->Play();
		CActiveScheduler::Start();
		User::After(1200000); // time enough to play it 
		INFO_PRINTF1(_L("loop"));
		err1 = GetAudPlayerState(iPlayer, state);
		if ( err1 != KErrNone ||
			 state != CMMFMdaAudioPlayerUtility::EStopped ||
			 iError != KErrNone)
			return EFail;
		delete iPlayer;
		iPlayer = NULL;
		} // end for repeats

	// try ramp > sound Duration
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	CActiveScheduler::Start();
	User::After(1000000);
	iPlayer->SetVolumeRamp(TTimeIntervalMicroSeconds(2000000));
	iPlayer->Play();
	CActiveScheduler::Start();
	User::After(1200000); // time enough to play it 
	err1 = GetAudPlayerState(iPlayer, state);
	if ( err1 != KErrNone ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		return EFail;
	delete iPlayer;
	iPlayer = NULL;

			
	//__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

