// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
CTestStepPlayerFileHandle* CTestStepPlayerFileHandle::NewL()
	{
	CTestStepPlayerFileHandle* self = new(ELeave) CTestStepPlayerFileHandle;
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
CTestStepPlayerFileHandle::CTestStepPlayerFileHandle() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0956-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerFileHandle::~CTestStepPlayerFileHandle()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerFileHandle::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerFileHandle::MapcPlayComplete(TInt aError)
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
enum TVerdict CTestStepPlayerFileHandle::DoTestStepPreambleL(void)
	{
  	enum TVerdict verdict;
  	// this installs the scheduler
  	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

  	// Printing to the console and log file
  	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0956-CP"));
  	INFO_PRINTF1(_L("This test opens a file first using a handle and then later using the filename"));
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
TVerdict CTestStepPlayerFileHandle::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestPlayerUtils : File"));
	TVerdict testStepResult = EPass;
	iError = KErrTimedOut;
	// Get the file name.
	TBuf<256> filename;
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName3"), iFileName))
		{
		return EInconclusive;
		}
	// Create CMdaAudioPlayerUtility Object
	CMdaAudioPlayerUtility* player = NULL;
	player = CMdaAudioPlayerUtility::NewL(*this);
	
	CleanupStack::PushL(player);
	
	// Create RFs and RFile Objects
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFile file;
	User::LeaveIfError( file.Open( fs, iFileName, EFileRead ) );
	CleanupClosePushL(file);
	
	// Create TMMFileSource Object
	TMMFileHandleSource filehandlesource(file);
	player->OpenFileL(filehandlesource);//opening the file using a handle
	
	// Wait for initialisation callback
	INFO_PRINTF1(_L("CMdaAudioPlayerUtility->OpenFileL(TMMFileHandleSource)"));
	CActiveScheduler::Start();
	
	// Check for errors.
	if(iError != KErrNone)
		{
		ERR_PRINTF2(_L("CMdaAudioPlayerUtility->OpenFileL(TMMFileHandleSource) Failed with Error : %d"),iError);
		testStepResult=EFail;
		}
	
	player->Close();//close the player and the handle object
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&fs);
	player->OpenFileL(iFileName);//opening the same above file using its filename
	CActiveScheduler::Start();
	if(iError != KErrNone)
		{
		ERR_PRINTF2(_L("CMdaAudioPlayerUtility->OpenFileL(TDesC) Failed with Error : %d"),iError);
		testStepResult=EFail;
		}
	CleanupStack::PopAndDestroy(player);
	return	testStepResult;
	}
	

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerFileHandle::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}


	

