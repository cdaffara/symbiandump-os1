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

// Specific includes for these test steps
#include "TestStepConvert.h"


#include "MmfAudioController.h"
#include "mmfclientaudioconverter.h"
//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepConvertOpenLocFormat.
 *
 *
 * @return	"CTestStepConvertOpenLocFormat*"
 *			The constructed CTestStepConvertOpenLocFormat
 *
 * @xxxx
 * 
 */
CTestStepConvertOpenLocFormat* CTestStepConvertOpenLocFormat::NewL()
	{
	CTestStepConvertOpenLocFormat* self = new(ELeave) CTestStepConvertOpenLocFormat;
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
CTestStepConvertOpenLocFormat::CTestStepConvertOpenLocFormat() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0552-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepConvertOpenLocFormat::~CTestStepConvertOpenLocFormat()
	{
	}

void CTestStepConvertOpenLocFormat::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	iHasStartedConverting = EFalse;
	iHasStartedConverting = EFalse;
	if (aPreviousState == CMdaAudioClipUtility::EOpen
		&& aCurrentState == CMdaAudioClipUtility::EPlaying)
		iHasStartedConverting = ETrue;
	if (aPreviousState == CMdaAudioClipUtility::EPlaying
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		iHasFinishedConverting = ETrue;

	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertOpenLocFormat::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0552-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::OpenL(filename,location, format) "));

	
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName2) )
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Recorder utility
	if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;
	// create an empty target file
	iFs.Connect();
	if ( iFile.Replace(iFs, iFileName2, EFileWrite) )
		return EInconclusive;
	if ( iFile.SetSize(0) )
		return EInconclusive;
	iFile.Close();
	iFs.Close();

	iClipLocationSrc = new TMdaFileClipLocation(iFileName2);

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertOpenLocFormat::DoTestStepPostambleL(void)
	{
	//delete the output file 
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName2);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	delete iClipLocationSrc;
	delete iConvert;
	iConvert = NULL;
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
TVerdict CTestStepConvertOpenLocFormat::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
	TInt size = 0;

	iConvert->OpenL(iFileName, iClipLocationSrc, &iAuClipFormat,&iAu16bitCodec);
	CActiveScheduler::Start();

	if (iConvert == NULL ||
		iError != KErrNone ||
		err != KErrNone ||
		iConvert->State() != CMdaAudioConvertUtility::EOpen  )
		{
		iTestStepResult = EFail;
		goto EndTest;
		}

	//iConvert->SetPosition(TTimeIntervalMicroSeconds(0));
	iConvert->RecordL();
	CActiveScheduler::Start();
	if (iError!=KErrNone || !iHasStartedConverting)
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();
	if (iError!=KErrNone || !iHasFinishedConverting)
		{
		iTestStepResult = EFail;
		goto EndTest;
		}

	iConvert->Close();
	delete iConvert;
	iConvert = NULL;
	
	//check if target file OK
	iFs.Connect();
	if ( KErrNone != iFile.Open(iFs, iFileName2, EFileRead) )
		{
		INFO_PRINTF1(_L("****the target file doesn't exit!!!!!****"));
		iTestStepResult = EFail;
		}
	else if (iFile.Size(size) != KErrNone)
		iTestStepResult = EInconclusive;
	if (size == 0)
		iTestStepResult = EFail;

	// need to close the file, otherwise we won't be able to delete it
	iFile.Close();

EndTest:
	if (iError != KErrNone)
		INFO_PRINTF2(_L("Test failed with error %d"), iError);
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

