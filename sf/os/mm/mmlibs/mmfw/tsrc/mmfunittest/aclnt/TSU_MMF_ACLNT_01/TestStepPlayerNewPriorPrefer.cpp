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
 * Static constructor for CTestStepPlayerNewPriorPrefer.
 *
 *
 * @return	"CTestStepPlayerNewPriorPrefer*"
 *			The constructed CTestStepPlayerNewPriorPrefer
 *
 * @xxxx
 * 
 */
CTestStepPlayerNewPriorPrefer* CTestStepPlayerNewPriorPrefer::NewL()
	{
	CTestStepPlayerNewPriorPrefer* self = new(ELeave) CTestStepPlayerNewPriorPrefer;
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
CTestStepPlayerNewPriorPrefer::CTestStepPlayerNewPriorPrefer() : iTestCase(0), iPlayer(NULL)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0003-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerNewPriorPrefer::~CTestStepPlayerNewPriorPrefer()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerNewPriorPrefer::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();

	TInt err1 = KErrNone;
	TInt err2 = KErrNone;
	TInt state = 0;
	TMMFMessageDestination* srcHandle = NULL;

	switch (iTestCase) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		INFO_PRINTF1(_L("InitComplete3"));
		err1 = GetAudPlayerState(iPlayer, state);
		err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
		if ( err1 != KErrNone ||
			 err2 != KErrNone ||
			 &srcHandle == NULL ||
			 state != CMMFMdaAudioPlayerUtility::EStopped ||
			 iError != KErrNone)
			iTestStepResult = EFail;
		if (iPlayer != NULL)
			{
			delete iPlayer;
			iPlayer = NULL;
			}
		break;
	case 4:
		INFO_PRINTF1(_L("InitComplete4"));
		if ( err1 != KErrNone ||
			 err2 != KErrNone ||
			 &srcHandle == NULL ||
			 state != CMMFMdaAudioPlayerUtility::EStopped ||
			 iError != KErrNone)
			iTestStepResult = EFail;
		if (iPlayer != NULL)
			{
			delete iPlayer;
			iPlayer = NULL;
			}
		break;
	default:
		break;
		}

	}

void CTestStepPlayerNewPriorPrefer::MapcPlayComplete(TInt /*aError*/)
	{
	// not used inthis test
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerNewPriorPrefer::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerNewPriorPrefer::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0003-HP"));// iTestStepName.Ptr());
	INFO_PRINTF1(_L("this test is iniatilising a CMdaAudioPlayerUtility object"));

	iError = KErrNone; //KErrTimedOut;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// test 1 //
	iTestCase = 1;
	iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this, EMdaPriorityMin);
	INFO_PRINTF1(_L("InitComplete1"));
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
		 return EFail;
	delete iPlayer;
	iPlayer = NULL;

	// test 2 //
	iTestCase = 2;
	iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this, EMdaPriorityMax);
	INFO_PRINTF1(_L("InitComplete2"));
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		 return EFail;
	delete iPlayer;
	iPlayer = NULL;

	// test 3 //
	iTestCase = 3;
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this, EMdaPriorityMax);
	CActiveScheduler::Start();

	// test 4 //
	iTestCase = 4;
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this, EMdaPriorityMax);
	CActiveScheduler::Start();

	User::After(500000); 

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


// --------------------------------------------

/**
 *
 * Static constructor for CTestStepPlayerNewPrefer.
 *
 *
 * @return	"CTestStepPlayerNewPrefer*"
 *			The constructed CTestStepPlayerNewPrefer
 *
 * @xxxx
 * 
 */
CTestStepPlayerNewPrefer* CTestStepPlayerNewPrefer::NewL()
	{
	CTestStepPlayerNewPrefer* self = new(ELeave) CTestStepPlayerNewPrefer;
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
CTestStepPlayerNewPrefer::CTestStepPlayerNewPrefer()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0004-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerNewPrefer::~CTestStepPlayerNewPrefer()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerNewPrefer::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerNewPrefer::MapcPlayComplete(TInt /*aError*/)
	{
	// not used inthis test
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerNewPrefer::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerNewPrefer::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0004-HP"));// iTestStepName.Ptr());
	INFO_PRINTF1(_L("this test is iniatilising a CMdaAudioPlayerUtility object with non-default preferences"));

	iError = KErrNone; //KErrTimedOut;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// test 1 //
	iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceQuality);
	// check for errors
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
		 return EFail;
		delete iPlayer;
		iPlayer = NULL;

	// test 2 //
	iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTime);
	// check for errors
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		 return EFail;
		delete iPlayer;
		iPlayer = NULL;
	
	// test 3 //
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this, EMdaPriorityNormal, EMdaPriorityPreferenceQuality);
	CActiveScheduler::Start();
	User::After(500000);
	// check for errors
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		 return EFail;
		delete iPlayer;
		iPlayer = NULL;

	// test 4 //
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this, EMdaPriorityNormal, EMdaPriorityPreferenceTime);
	CActiveScheduler::Start();
	User::After(500000);
	// check for errors
	err1 = GetAudPlayerState(iPlayer, state);
	err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
	if ( err1 != KErrNone ||
		 err2 != KErrNone ||
		 &srcHandle == NULL ||
		 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 iError != KErrNone)
		 return EFail;
		delete iPlayer;
		iPlayer = NULL;

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

