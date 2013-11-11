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
#include <c32comm.h>
#include <f32file.h>
#include <testframework.h>

#include <mdaaudiosampleplayer.h>


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
 * Static constructor for CTestStepCheckHeaderTag.
 *
 *
 * @return	"CTestStepCheckHeaderTag*"
 *			The constructed CTestStepCheckHeaderTag
 *
 * @xxxx
 * 
 */
CTestStepCheckHeaderTag* CTestStepCheckHeaderTag::NewL()
	{
	CTestStepCheckHeaderTag* self = new(ELeave) CTestStepCheckHeaderTag;
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
CTestStepCheckHeaderTag::CTestStepCheckHeaderTag() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0955-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepCheckHeaderTag::~CTestStepCheckHeaderTag()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepCheckHeaderTag::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepCheckHeaderTag::MapcPlayComplete(TInt aError)
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
enum TVerdict CTestStepCheckHeaderTag::DoTestStepPreambleL(void)
	{
  	enum TVerdict verdict;
  	// this installs the scheduler
  	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

  	// Printing to the console and log file
  	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0955-CP"));
  	INFO_PRINTF1(_L("This test opens a file and checks for supported headertags at the start of the header"));

   	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName11"), iFileName))
		{
		INFO_PRINTF1(_L("File name not found in INI file."));
		return EInconclusive;
		}
	return verdict; 	
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
TVerdict CTestStepCheckHeaderTag::DoTestStepL()
	{
	iError = KErrNone;	 
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	CActiveScheduler::Start();
	User::After(1000000);

	if (iPlayer == NULL)
		{
	   	INFO_PRINTF1(_L("Error initializing the player"));
	   	return EInconclusive;
	 	}
	else if(iError != KErrNotSupported)
	 	{
	 	INFO_PRINTF2(_L("Returned with error %d which is unexpected"),iError);
	 	return EFail;
	 	}
	 
	INFO_PRINTF1(_L("The test checks for the headertag at the start of the header,hence fix for DEF056298"));
	INFO_PRINTF1(_L("finished with this test step"));	 
	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepCheckHeaderTag::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}


	

