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
#include "TestStepPlayerNew.h"

#include "mmfclientaudioplayer.h"

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepPlayerNewFilePlayer.
 *
 *
 * @return	"CTestStepPlayerNewFilePlayer*"
 *			The constructed CTestStepPlayerNewFilePlayer
 *
 * @xxxx
 * 
 */
CTestStepPlayerNewFilePlayer* CTestStepPlayerNewFilePlayer::NewL()
	{
	CTestStepPlayerNewFilePlayer* self = new(ELeave) CTestStepPlayerNewFilePlayer;
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
CTestStepPlayerNewFilePlayer::CTestStepPlayerNewFilePlayer()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0001-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerNewFilePlayer::~CTestStepPlayerNewFilePlayer()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerNewFilePlayer::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerNewFilePlayer::MapcPlayComplete(TInt aError)
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
TVerdict CTestStepPlayerNewFilePlayer::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0001-CP"));
	INFO_PRINTF1(_L("this test is opening a sound file with NewFilePlayerL()"));

	iError = KErrNone; //KErrTimedOut;

	iRes = &iFileName;
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	CMMFMdaAudioPlayerUtility* player = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	CleanupStack::PushL(player);

	CActiveScheduler::Start();
	User::After(500000);

	TInt err1 = KErrNone;
	TInt err2 = KErrNone;
	TInt state = 0;
	TMMFMessageDestination* srcHandle = NULL;
	err1 = GetAudPlayerState(player, state);
	err2 = GetAudPlayerSourceHandle(player, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 player->Duration() == TTimeIntervalMicroSeconds(0) || // we know the duration of the file is more than 0
		 iError != KErrNone)
		 iTestStepResult = EFail;

	CleanupStack::PopAndDestroy(player);

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

