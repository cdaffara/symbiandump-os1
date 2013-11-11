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

#include <mdaaudiotoneplayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepTone.h"


//#include <mmfaudiocontroller.h>
#include "mmfclienttoneplayer.h"
//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepTonePrepareToPlayFileSeq.
 *
 *
 * @return	"CTestStepTonePrepareToPlayFileSeq*"
 *			The constructed CTestStepTonePrepareToPlayFileSeq
 *
 * @xxxx
 * 
 */
CTestStepTonePrepareToPlayFileSeq* CTestStepTonePrepareToPlayFileSeq::NewL()
	{
	CTestStepTonePrepareToPlayFileSeq* self = new(ELeave) CTestStepTonePrepareToPlayFileSeq;
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
CTestStepTonePrepareToPlayFileSeq::CTestStepTonePrepareToPlayFileSeq() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0455-CP");
	
	// need a bigger heap size on EKA2 HW
#if !defined __WINS__
	iHeapSize = KHeapSizeToneTestEKA2;
#endif // EKA2
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepTonePrepareToPlayFileSeq::~CTestStepTonePrepareToPlayFileSeq()
	{
	}

void CTestStepTonePrepareToPlayFileSeq::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepTonePrepareToPlayFileSeq::MatoPlayComplete(TInt aError)
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
enum TVerdict CTestStepTonePrepareToPlayFileSeq::DoTestStepPreambleL(void)
	{
    TInt err       = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0455-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioToneUtility::PrepareToPlayFileSequence() "));

	// get the file with the DTMF string
	if(!GetStringFromConfig(_L("SectionOne"), _L("DTMFStringFName1"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Tone utility
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	if ( err != KErrNone ||
		 iTone->State() != EMdaAudioToneUtilityNotReady	)
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
enum TVerdict CTestStepTonePrepareToPlayFileSeq::DoTestStepPostambleL(void)
	{
	delete iTone;
	iTone = NULL;
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
TVerdict CTestStepTonePrepareToPlayFileSeq::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test start"));
	iTestStepResult = EPass;

	iTone->PrepareToPlayFileSequence(iFileName);
	CActiveScheduler::Start();

	TInt err1 = KErrNone;
	TInt type = 0;
	if (iTone == NULL)
		{
		INFO_PRINTF1(_L("iTone NULL"));
		}

	err1 = GetAudToneType(iTone, type);
	if (err1 != KErrNone ||
		type != CMMFToneConfig::EMmfToneTypeFileSeq  ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityPrepared)
		{
		INFO_PRINTF1(_L("GetAudToneType() test failed"));
		return EFail;
		}

	iTone->CancelPrepare();
	if (iTone->State() != EMdaAudioToneUtilityNotReady	)
		{
		INFO_PRINTF1(_L("CancelPrepare() test failed"));
		return EFail;
		}

	iTone->PrepareToPlayFileSequence(iFileName);
	CActiveScheduler::Start();

	err1 = GetAudToneType(iTone, type);
	if (err1 != KErrNone ||
		type != CMMFToneConfig::EMmfToneTypeFileSeq   ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityPrepared)
		{
		INFO_PRINTF1(_L("GetAudToneType() test 2 failed"));
		return EFail;
		}

	//iTone->Play(); // crashing here!!!
	//CActiveScheduler::Start();
	//User::After(2000000);


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Static constructor for CTestStepTonePrepareToPlayDRMFileSeq.
 *
 *
 * @return	"CTestStepTonePrepareToPlayDRMFileSeq*"
 *			The constructed CTestStepTonePrepareToPlayDRMFileSeq
 *
 * @xxxx
 * 
 */
CTestStepTonePrepareToPlayDRMFileSeq* CTestStepTonePrepareToPlayDRMFileSeq::NewL()
	{
	CTestStepTonePrepareToPlayDRMFileSeq* self = new(ELeave) CTestStepTonePrepareToPlayDRMFileSeq;
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
CTestStepTonePrepareToPlayDRMFileSeq::CTestStepTonePrepareToPlayDRMFileSeq()
 : CTestStepTonePrepareToPlayFileSeq()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0908-HP");
	
	// need a bigger heap size on EKA2 HW
#if !defined __WINS__
	iHeapSize = KHeapSizeToneTestEKA2;
#endif // EKA2
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepTonePrepareToPlayDRMFileSeq::~CTestStepTonePrepareToPlayDRMFileSeq()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepTonePrepareToPlayDRMFileSeq::DoTestStepPreambleL(void)
	{
    TInt err       = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0908-HP"));
	INFO_PRINTF1(_L("INC087436: 3.1. DRM NRT audio files are not working properly as calender alarm tone"));

	// get the protected SingleTone file
	if(!GetStringFromConfig(_L("SectionDRM"), _L("SingleToneContent"), iFileName))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Tone utility
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	if ( err != KErrNone ||
		 iTone->State() != EMdaAudioToneUtilityNotReady	)
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
enum TVerdict CTestStepTonePrepareToPlayDRMFileSeq::DoTestStepPostambleL(void)
	{
	delete iTone;
	iTone = NULL;
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
TVerdict CTestStepTonePrepareToPlayDRMFileSeq::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test start"));
	iTestStepResult = EPass;

	iTone->PrepareToPlayFileSequence(iFileName);
	CActiveScheduler::Start();

	TInt err1 = KErrNone;
	TInt type = 0;
	if (iTone == NULL)
		{
		INFO_PRINTF1(_L("iTone NULL"));
		}

	err1 = GetAudToneTypeSafe(iTone, type);
	if(err1 == KErrNotFound) 
		{
		INFO_PRINTF1(_L("iTone->iToneConfig is null (no rights?)"));
		return EFail;
		}
	if (err1 != KErrNone ||
		type != CMMFToneConfig::EMmfToneTypeFileSeq  ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityPrepared)
		{
		INFO_PRINTF1(_L("GetAudToneType() test failed"));
		return EFail;
		}

	iTone->CancelPrepare();
	if (iTone->State() != EMdaAudioToneUtilityNotReady	)
		{
		INFO_PRINTF1(_L("CancelPrepare() test failed"));
		return EFail;
		}

	iTone->PrepareToPlayFileSequence(iFileName);
	CActiveScheduler::Start();

	err1 = GetAudToneTypeSafe(iTone, type);
	if(err1 == KErrNotFound) 
		{
		INFO_PRINTF1(_L("iTone->iToneConfig is null (no rights?)"));
		return EFail;
		}
	if (err1 != KErrNone ||
		type != CMMFToneConfig::EMmfToneTypeFileSeq   ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityPrepared)
		{
		INFO_PRINTF1(_L("GetAudToneType() test 2 failed"));
		return EFail;
		}

	// we don't need to verify this actually plays, only that we
	// correctly found the rights

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
