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
#include "TestStepRecorder.h"


#include "MmfAudioController.h"
#include "mmfclientaudiorecorder.h"
//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepRecorderOpenDesc.
 *
 *
 * @return	"CTestStepRecorderOpenDesc*"
 *			The constructed CTestStepRecorderOpenDesc
 *
 * @xxxx
 * 
 */
CTestStepRecorderOpenDesc* CTestStepRecorderOpenDesc::NewL()
	{
	CTestStepRecorderOpenDesc* self = new(ELeave) CTestStepRecorderOpenDesc;
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
CTestStepRecorderOpenDesc::CTestStepRecorderOpenDesc()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0204-CP");
    // set the heap size to 300k 
	iHeapSize = 300000;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderOpenDesc::~CTestStepRecorderOpenDesc()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderOpenDesc::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0204-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::OpenDescL()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileNames[0])) // wav file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName3"), iFileNames[1])) // au file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
//	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName5"), iFileNames[2])) // raw file
//		{
//		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
//		return EInconclusive;
//		}

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderOpenDesc::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	delete iDes8;
	iDes8 = NULL;
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
void CTestStepRecorderOpenDesc::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderOpenDesc::OpenAndStartSchedulerL()
	{
	iRecorder->OpenDesL( *iDes8 );
	CActiveScheduler::Start(); 
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
TVerdict CTestStepRecorderOpenDesc::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	for (TInt type=0; type<2; type++)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		// load the contents of an audio file to a Descriptor
		TInt fSize;

		iFs.Connect();
		if ( iFile.Open(iFs, iFileNames[type], EFileRead) != KErrNone )
			return EInconclusive;
		iFile.Size(fSize);
		// load the sound from the file to a Descriptor
		iBuf = HBufC8::NewL( fSize );

		//[ convert one descriptor to another so that it can be used in the interface ]
		iDes8 = new (ELeave) TPtr8( NULL, 0 );
	    iDes8->Set( iBuf->Des());

		iFile.Read( *iDes8 );

	
		iFile.Close();
		iFs.Close();

		TRAP( err, OpenAndStartSchedulerL() );

		if (iRecorder == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EFail;

		delete iBuf;
		iBuf = NULL;
		delete iRecorder;
		iRecorder = NULL;
		User::After(200000);
		}


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

