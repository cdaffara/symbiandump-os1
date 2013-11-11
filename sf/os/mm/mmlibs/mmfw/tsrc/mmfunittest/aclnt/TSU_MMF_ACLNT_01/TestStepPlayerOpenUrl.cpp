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
 * Static constructor for CTestStepPlayerOpenUrl.
 *
 *
 * @return	"CTestStepPlayerOpenUrl*"
 *			The constructed CTestStepPlayerOpenUrl
 *
 * @xxxx
 * 
 */
CTestStepPlayerOpenUrl* CTestStepPlayerOpenUrl::NewL()
	{
	CTestStepPlayerOpenUrl* self = new(ELeave) CTestStepPlayerOpenUrl;
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
CTestStepPlayerOpenUrl::CTestStepPlayerOpenUrl()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0025-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerOpenUrl::~CTestStepPlayerOpenUrl()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerOpenUrl::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this is a test of CMdaAudioPlayerUtility::OpenUrlL()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioURLTestController"), iURL) )
		{
		return EInconclusive;
		}
		
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioLongURLTestController"), iLongURL) )
		{
		return EInconclusive;
		}
		
	if(!GetStringFromConfig(_L("SectionOne"), _L("TestControllerPAVMimeType"), iMimeType) )
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
enum TVerdict CTestStepPlayerOpenUrl::DoTestStepPostambleL(void)
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
void CTestStepPlayerOpenUrl::MapcInitComplete(TInt aErrorcCode, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}
	
void CTestStepPlayerOpenUrl::MapcPlayComplete(TInt /*aError*/)
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
TVerdict CTestStepPlayerOpenUrl::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// create the Player utility
	TRAP(err, iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this));

	if (err != KErrNone)
		return EInconclusive;
	
	TRAP(err, SetTestMediaIdL(iPlayer));	
	
	if (err != KErrNone)
		return EInconclusive;

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
		iError != KErrNotSupported)  //since <n>yes tag used.
		{
		INFO_PRINTF2(_L("error %d after opening url"),iError);
		return EFail;	
		}
		
		
	// now use a special custom command to check the sending of these parameters
	TUid uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	
	//since <n>yes tag used.
	/*err = iPlayer->CustomCommandSync(destPckg,
					 					EMMFTestCustomCommandCheckUrl,
										KNullDesC8,
										KNullDesC8);
	if (err!=KErrNone)
		{
		INFO_PRINTF2(_L("error %d after custom command sync"),err);	
		return EFail;	
		}*/
		
	INFO_PRINTF1(_L("finished test url"));
		
	//load a long url
	TRAP(err,iPlayer->OpenUrlL(iLongURL, KTestIap ));
	
	if (err!=KErrNone)
		{
		INFO_PRINTF2(_L("error %d returned from open long url"),err);	
		return EInconclusive;
		}
	
	CActiveScheduler::Start(); 


	if (iPlayer == NULL ||
		err != KErrNone ||
		iError != KErrNotSupported)  //since <n>yes tag used.
		{
		INFO_PRINTF2(_L("error %d after opening long url"),iError);
		return EFail;	
		}	
			
	//since <n>yes tag used.		
	/*err = iPlayer->CustomCommandSync(destPckg,
					 					EMMFTestCustomCommandCheckLongUrl,
										KNullDesC8,
										KNullDesC8);
	if (err!=KErrNone)
		{
		INFO_PRINTF2(_L("error %d after custom command sync for long url"),err);	
		return EFail;	
		}*/
		
	INFO_PRINTF1(_L("finished test long url"));

	// load matching to mime type
	TBuf8<256> mimeType;
	mimeType.Copy(iMimeType);	
	TRAP(err,iPlayer->OpenUrlL( iURL, KTestIap, mimeType ));
	
	if (err!=KErrNone)
		return EInconclusive;
	
	CActiveScheduler::Start(); 


	if (iPlayer == NULL ||
		err != KErrNone ||
		iError != KErrNone)  
		return EFail;
	
	err = iPlayer->CustomCommandSync(destPckg,
					 					EMMFTestCustomCommandCheckUrl,
										KNullDesC8,
										KNullDesC8);
	if (err==KErrNone)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

