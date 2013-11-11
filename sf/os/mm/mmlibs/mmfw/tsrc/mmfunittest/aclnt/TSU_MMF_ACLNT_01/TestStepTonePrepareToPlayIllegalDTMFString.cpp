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


/**
 *
 * Static constructor for CTestStepTonePrepareToPlayIllegalDTMFString.
 *
 *
 * @return	"CTestStepTonePrepareToPlayDTMFString*"
 *			The constructed CTestStepTonePrepareToPlayIllegalDTMFString
 *
 * @xxxx
 * 
 */
CTestStepTonePrepareToPlayIllegalDTMFString* CTestStepTonePrepareToPlayIllegalDTMFString::NewL()
	{
	return (new(ELeave) CTestStepTonePrepareToPlayIllegalDTMFString());
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepTonePrepareToPlayIllegalDTMFString::CTestStepTonePrepareToPlayIllegalDTMFString()
	: iError(KErrNone), iPlayComplete(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0800-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepTonePrepareToPlayIllegalDTMFString::~CTestStepTonePrepareToPlayIllegalDTMFString()
	{
	}

void CTestStepTonePrepareToPlayIllegalDTMFString::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepTonePrepareToPlayIllegalDTMFString::MatoPlayComplete(TInt aError)
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
enum TVerdict CTestStepTonePrepareToPlayIllegalDTMFString::DoTestStepPreambleL(void)
	{
    TInt err       = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0800-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioToneUtility::PrepareToPlayDTMFString() "));
	
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
enum TVerdict CTestStepTonePrepareToPlayIllegalDTMFString::DoTestStepPostambleL(void)
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
TVerdict CTestStepTonePrepareToPlayIllegalDTMFString::DoTestStepL()
	{
	iTestStepResult = EPass;
	_LIT(KDTMFStrInvalid1, "123   <>?$%&$%^&");
	_LIT(KDTMFStrInvalid2, "ABCDEFG"); // 'G' is invalid

	iTone->PrepareToPlayDTMFString( KDTMFStrInvalid1 );
	CActiveScheduler::Start();

	if (iError != KErrArgument ||
		iTone->State() != EMdaAudioToneUtilityNotReady)
		{
		// We expect there to be an error
		return EFail;
		}
	
	iError = KErrNone;
	iTone->PrepareToPlayDTMFString( KDTMFStrInvalid2 );
	CActiveScheduler::Start();
	if (iError != KErrArgument ||
		iTone->State() != EMdaAudioToneUtilityNotReady)
		{
		// We expect there to be an error
		return EFail;
		}
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

