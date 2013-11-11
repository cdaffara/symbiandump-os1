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
 * Static constructor for CTestStepPlayerVol.
 *
 *
 * @return	"CTestStepPlayerVol*"
 *			The constructed CTestStepPlayerVol
 *
 * @xxxx
 * 
 */
CTestStepPlayerVol* CTestStepPlayerVol::NewL()
	{
	CTestStepPlayerVol* self = new(ELeave) CTestStepPlayerVol;
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
CTestStepPlayerVol::CTestStepPlayerVol()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0039-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerVol::~CTestStepPlayerVol()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerVol::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerVol::MapcPlayComplete(TInt aError)
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
 enum TVerdict CTestStepPlayerVol::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0039-CP"));
	 INFO_PRINTF1(_L("this test is playing a sound with several Volume levels. SetVolume(TInt)"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) )
		{
		  return EInconclusive;
		}
	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName2) )
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
enum TVerdict CTestStepPlayerVol::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	//[ Destroy the scheduler ]
	delete iPlayer;
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
TVerdict CTestStepPlayerVol::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt refVol;
	TInt err1 = KErrNone;
	TInt err2 = KErrNone;
	TInt state = 0;
	TMMFMessageDestination* srcHandle = NULL;
	
	TInt maxVol;
		
	// get the max Volume bu oppening a test file
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	CActiveScheduler::Start();
	if (iPlayer == NULL ||
		iError != KErrNone )
		return EInconclusive;

	User::After(1000000);
	maxVol = iPlayer->MaxVolume();
	delete iPlayer;
	iPlayer = NULL;

	// try several volume values and play
	for (TInt vol=0 ; vol<=maxVol ; vol+=(maxVol/4)) // repeats loop
		{
		iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
		CActiveScheduler::Start();
		User::After(1000000);
		if (iPlayer == NULL ||
			iError != KErrNone )
			return EInconclusive;

		iPlayer->SetVolume(vol);
		iPlayer->Play();
		CActiveScheduler::Start();
		User::After(1200000); // time enough to play it 
		INFO_PRINTF1(_L("loop"));
		// get the volume
		err = iPlayer->GetVolume(refVol);
		err1 = GetAudPlayerState(iPlayer, state);
		if ( err != KErrNone ||
			 err1 != KErrNone ||
			 refVol != vol ||
			 state != CMMFMdaAudioPlayerUtility::EStopped )
			return EFail;
		User::After(100000);
		delete iPlayer;
		iPlayer = NULL;
		} // end for 

	// set the volume while playing
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName2, *this);
	CActiveScheduler::Start();
	User::After(1000000);
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		return EInconclusive;
	maxVol = iPlayer->MaxVolume();
	iPlayer->SetVolume(maxVol);
	iPlayer->Play();
	
	err = iPlayer->GetVolume(refVol);
	err1 = GetAudPlayerState(iPlayer, state);
	if ( err != KErrNone ||
		 err1 != KErrNone ||
		refVol != maxVol ||
		 state != CMMFMdaAudioPlayerUtility::EPlaying )
		return EFail;
	User::After(1000000);
	iPlayer->SetVolume(maxVol/2);
	User::After(1000000);
	err = iPlayer->GetVolume(refVol);
	err1 = GetAudPlayerState(iPlayer, state);
	if ( err != KErrNone ||
		 err1 != KErrNone ||
		 refVol != maxVol/2 ||
		 state != CMMFMdaAudioPlayerUtility::EPlaying )
		return EFail;
	iPlayer->SetVolume(maxVol/3);
	User::After(1000000);
	err = iPlayer->GetVolume(refVol);
	err1 = GetAudPlayerState(iPlayer, state);
	if ( err != KErrNone ||
		 err1 != KErrNone ||
		 refVol != maxVol/3 ||
		 state != CMMFMdaAudioPlayerUtility::EPlaying )
		return EFail;
	iPlayer->SetVolume(maxVol/5);
	User::After(1000000);
	err = iPlayer->GetVolume(refVol);
	err1 = GetAudPlayerState(iPlayer, state);
	if ( err != KErrNone ||
		 err1 != KErrNone ||
		 refVol != maxVol/5 ||
		 state != CMMFMdaAudioPlayerUtility::EPlaying )
		return EFail;
	iPlayer->SetVolume(maxVol/10);
	User::After(1000000);
	err = iPlayer->GetVolume(refVol);
	err1 = GetAudPlayerState(iPlayer, state);
	if ( err != KErrNone ||
		 err1 != KErrNone ||
		 refVol != maxVol/10 ||
		 state != CMMFMdaAudioPlayerUtility::EPlaying )
		return EFail;
	User::After(3000000);
	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

