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
#include "MmfClientAudioEditorPlay.h"
//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepConvertOpen.
 *
 *
 * @return	"CTestStepConvertOpen*"
 *			The constructed CTestStepConvertOpen
 *
 * @xxxx
 * 
 */
CTestStepConvertType* CTestStepConvertType::NewL()
	{
	CTestStepConvertType* self = new(ELeave) CTestStepConvertType;
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
CTestStepConvertType::CTestStepConvertType() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0555-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepConvertType::~CTestStepConvertType()
	{
	}

void CTestStepConvertOpen::MoscoStateChangeEvent(CBase* aObject,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertType::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0555-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::TypeL() "));

	
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName2) )
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Recorder utility
	if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;
	iConvert->OpenL(iFileName, iFileName2);
	if (iConvert->State() != CMdaAudioConvertUtility::EOpen)
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
enum TVerdict CTestStepConvertType::DoTestStepPostambleL(void)
	{
	//delete the output file 
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName2);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	delete iAudioType;
	iAudioType = NULL;
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

TVerdict CTestStepConvertType::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	__MM_HEAP_MARK;
	TRAP(err, (iAudioType = iConvert->TypeL()) );

	if (iConvert == NULL ||
		iError != KErrNone ||
		err != KErrNone ||
		iAudioType == NULL ||
		iConvert->State() != CMdaAudioConvertUtility::EOpen  )
		iTestStepResult = EFail;

	if ( iAudioType->iFormat != TMdaWavClipFormat ||
		 iAudioType->iiCodec != TMdaWavCodec )
		 iTestStepResult = EFail;

	//iConvert->PlayL();
	//iConvert->RecordL();
	//User::After(2000000);
	iConvert->Close();
	delete iConvert;
	iConvert = NULL;
	__MM_HEAP_MARKEND;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

