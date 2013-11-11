// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//#include <MdaAudioSamplePlayer.h>
#include <mdaaudiosampleeditor.h>
//#include <MdaAudioTonePlayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

#include "MmfAudioController.h"
#include "mmfclientaudioplayer.h"

// Specific includes for these test steps
#include "TestStepPlayerManip.h"

#include "../../ctlfrm/TS_Codes.h"

/**
 *
 * Static constructor for CTestStepPlayerOpenviaUriTag.
 *
 *
 * @return	"CTestStepPlayerOpenviaUriTag*"
 *			The constructed CTestStepPlayerOpenviaUriTag
 *
 * @xxxx
 * 
 */
CTestStepPlayerOpenviaUriTag* CTestStepPlayerOpenviaUriTag::NewL()
	{
	CTestStepPlayerOpenviaUriTag* self = new(ELeave) CTestStepPlayerOpenviaUriTag;
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
CTestStepPlayerOpenviaUriTag::CTestStepPlayerOpenviaUriTag()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0958-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerOpenviaUriTag::~CTestStepPlayerOpenviaUriTag()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerOpenviaUriTag::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this is a test of CMdaAudioPlayerUtility::OpenUrlL() with uri tag"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioURLTagTestController"), iURL) )
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
enum TVerdict CTestStepPlayerOpenviaUriTag::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepPlayerOpenviaUriTag::MapcInitComplete(TInt aErrorcCode, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}
	
void CTestStepPlayerOpenviaUriTag::MapcPlayComplete(TInt /*aError*/)
	{
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
TVerdict CTestStepPlayerOpenviaUriTag::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// create the Player utility
	TRAP(err, iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this));

	if (err != KErrNone)
		{
		return EInconclusive;
		}
	
	TRAP(err, SetTestDummyMediaIdL(iPlayer));	
	
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	
	// load matching to filename 
	TRAP(err,iPlayer->OpenUrlL(iURL, KTestIap ));
	
	if (err!=KErrNone)
		{
		INFO_PRINTF2(_L("error %d returned from open url"),err);	
		return EInconclusive;
		}
		
	CActiveScheduler::Start(); 
	
	if (iPlayer == NULL ||
		err != KErrNone ||
		iError != KErrExtensionNotSupported)
		{
		INFO_PRINTF2(_L("error %d after opening url"),iError);
		return EFail;	
		}
			
	if (iError == KErrExtensionNotSupported)
		{
		INFO_PRINTF1(_L("controller plugin used uri tag for matching"));
		iTestStepResult = EPass;
		}

	else
		{
		iTestStepResult = EFail;	
		}
	
	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

