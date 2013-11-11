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
 * Static constructor for CTestStepTonePrepareToPlayDTMFString.
 *
 *
 * @return	"CTestStepTonePrepareToPlayDTMFString*"
 *			The constructed CTestStepTonePrepareToPlayDTMFString
 *
 * @xxxx
 * 
 */
CTestStepTonePrepareToPlayDTMFString* CTestStepTonePrepareToPlayDTMFString::NewL()
	{
	CTestStepTonePrepareToPlayDTMFString* self = new(ELeave) CTestStepTonePrepareToPlayDTMFString;
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
CTestStepTonePrepareToPlayDTMFString::CTestStepTonePrepareToPlayDTMFString() : iError(KErrNone),
																			   iPlayComplete(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0454-CP");
	
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
CTestStepTonePrepareToPlayDTMFString::~CTestStepTonePrepareToPlayDTMFString()
	{
	}

void CTestStepTonePrepareToPlayDTMFString::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepTonePrepareToPlayDTMFString::MatoPlayComplete(TInt aError)
	{
	iPlayComplete = ETrue;
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
enum TVerdict CTestStepTonePrepareToPlayDTMFString::DoTestStepPreambleL(void)
	{
    TInt err       = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0454-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioToneUtility::PrepareToPayDTMFString() "));
	
	// create the Tone utility
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	if ( err != KErrNone )
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
enum TVerdict CTestStepTonePrepareToPlayDTMFString::DoTestStepPostambleL(void)
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
TVerdict CTestStepTonePrepareToPlayDTMFString::DoTestStepL()
	{
	iTestStepResult = EPass;

	_LIT(KDTMFStr1, "AB,C, DE, F"); // hex digits are all valid
	_LIT(KDTMFStr2, "125774 6788* #A *d ");

	// test1 //
	iTone->PrepareToPlayDTMFString( KDTMFStr1 );
	CActiveScheduler::Start();

	TInt err1 = KErrNone;
	TInt type = 0;
	err1 = GetAudToneType(iTone, type);
	if (err1 != KErrNone ||
		type != CMMFToneConfig::EMmfToneTypeDTMF ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityPrepared)
		return EFail;
	else
		{
		// play just for confirmation
		iPlayComplete = EFalse;
		iTone->Play();
		CActiveScheduler::Start();
		if  (iError!= KErrNone || !iPlayComplete)
			return EInconclusive;
		}
	//delete iTone;
	//iTone = NULL;

  // test 2 //
	//TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	//if ( err != KErrNone )
	//	return EInconclusive;
	iTone->PrepareToPlayDTMFString( KDTMFStr2 );
	CActiveScheduler::Start();

	err1 = GetAudToneType(iTone, type);
	if (err1 != KErrNone ||
		type != CMMFToneConfig::EMmfToneTypeDTMF ||
		iError != KErrNone ||
		iTone->State() != EMdaAudioToneUtilityPrepared)
		iTestStepResult = EFail;
	else
		{
		// play just for confirmation
		iPlayComplete = EFalse;
		iTone->Play();
		CActiveScheduler::Start();
		if  (iError!= KErrNone || !iPlayComplete)
			return EFail;
		}

	delete iTone;
	iTone = NULL;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

