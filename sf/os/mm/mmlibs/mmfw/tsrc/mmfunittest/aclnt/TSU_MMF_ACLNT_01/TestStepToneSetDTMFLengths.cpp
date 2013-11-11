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
 * Static constructor for CTestStepToneSetDTMFLengths.
 *
 *
 * @return	"CTestStepToneSetDTMFLengths*"
 *			The constructed CTestStepToneSetDTMFLengths
 *
 * @xxxx
 * 
 */
CTestStepToneSetDTMFLengths* CTestStepToneSetDTMFLengths::NewL()
	{
	CTestStepToneSetDTMFLengths* self = new(ELeave) CTestStepToneSetDTMFLengths;
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
CTestStepToneSetDTMFLengths::CTestStepToneSetDTMFLengths() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0459-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepToneSetDTMFLengths::~CTestStepToneSetDTMFLengths()
	{
	}

void CTestStepToneSetDTMFLengths::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepToneSetDTMFLengths::MatoPlayComplete(TInt aError)
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
enum TVerdict CTestStepToneSetDTMFLengths::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0459-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioToneUtility::SetDTMFLengths() "));
	
	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepToneSetDTMFLengths::DoTestStepPostambleL(void)
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
TVerdict CTestStepToneSetDTMFLengths::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;

	_LIT(KDTMFStr1, "AB,C, "); 
	//_LIT(KDTMFStr2, "125774 6788* #A *d GHET"); //HIJKLMNOPQRSTUVWXYZ");
	//_LIT(KDTMFStrInvalid, "123   <>?$%&$%^&");

	// test1 //
	for (TInt dur = 1; dur<3000000; dur*=2)
		{
		// create the Tone utility
		TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
		if ( err != KErrNone )
			return EInconclusive;

		iTone->SetDTMFLengths( TTimeIntervalMicroSeconds32(dur), 
							   TTimeIntervalMicroSeconds32(dur),
							   TTimeIntervalMicroSeconds32(dur) );
		//if (iTone->State() != EMdaAudioToneUtilityPrepared)
		//	{
		//	return EFail;
		//	}
		TRAP(err, iTone->PrepareToPlayDTMFString( KDTMFStr1 ) );
		CActiveScheduler::Start();
		if (err != KErrNone ||
			iError != KErrNone ||
			iTone->State() != EMdaAudioToneUtilityPrepared	)
			return EInconclusive;

		// play just for confirmation
		//iTone->Play();
		//CActiveScheduler::Start();
		//User::After(2000000); //6*dur);
		//if (iError != KErrNone)
		//	iTestStepResult = EInconclusive;
		delete iTone;
		iTone = NULL;
		}

	//iTestStepResult = EFail;
	//INFO_PRINTF1(_L("!*****FAILED ON PURPOSE, problem with corrupt CMdaAudioToneUtility object after playing"));

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

