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
// TestStepRecorderSetMaxWriteLen.cpp
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
 * Static constructor for CTestStepRecorderSetMaxWriteLen.
 *
 *
 * @return	"CTestStepRecorderSetMaxWriteLen*"
 *			The constructed CTestStepRecorderSetMaxWriteLen
 *
 * @xxxx
 * 
 */

CTestStepRecorderSetMaxWriteLen* CTestStepRecorderSetMaxWriteLen::NewL(const TDesC& aTestName, TBool aIsConverterTest)
	{
	CTestStepRecorderSetMaxWriteLen* self = new(ELeave) CTestStepRecorderSetMaxWriteLen(aTestName, aIsConverterTest);
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
CTestStepRecorderSetMaxWriteLen::CTestStepRecorderSetMaxWriteLen(const TDesC& aTestName, TBool aIsConverterTest)
: iIsConverterTest(aIsConverterTest)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderSetMaxWriteLen::~CTestStepRecorderSetMaxWriteLen()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderSetMaxWriteLen::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	 // Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iIsConverterTest)
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::SetMaxWriteLength()"));
		}
	else
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::SetMaxWriteLength()"));
		}

	if(!(GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName3) // au
		)) 
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if (!iIsConverterTest)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		iRecorder->OpenFileL( iFileName3 );
		CActiveScheduler::Start(); 
		if (iRecorder == NULL ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;
		
		}
	else // isConverterTest == ETrue
		{
		// create the Recorder utility
		if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
			return EInconclusive;

		iConvert->OpenL( iFileName, iFileName3 );
		CActiveScheduler::Start(); 
		if (iConvert == NULL ||
			iError != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen)
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
enum TVerdict CTestStepRecorderSetMaxWriteLen::DoTestStepPostambleL(void)
	{
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileName3);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;
	if (!iIsConverterTest)
		{
		delete iRecorder;
		iRecorder = NULL;
		}
	else
		{
		delete iConvert;
		iConvert = NULL;
		}
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

void CTestStepRecorderSetMaxWriteLen::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
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
const TInt KDefaultFileSize = 8192; //An arbitrary size
TVerdict CTestStepRecorderSetMaxWriteLen::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt64 time, refTime;

	//__MM_HEAP_MARK;

	if (!iIsConverterTest)
		{
		//Limit to arbitrary size
		iRecorder->SetMaxWriteLength(KDefaultFileSize);
		if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
			return EFail;

		refTime = iRecorder->RecordTimeAvailable().Int64();
		if (refTime <= 0 )
			return EInconclusive;

		//Halve the size
		iRecorder->SetMaxWriteLength(KDefaultFileSize/2);
		if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
			return EFail;

		//Expect the record time to be less than half the original (headers/metadata)
		time = iRecorder->RecordTimeAvailable().Int64();
		if (time <= 0 || time > refTime/2)
			return EFail;
		}
	else // iIsConverterTest == ETrue
		{
		//Limit to arbitrary size
		iConvert->SetMaxWriteLength(KDefaultFileSize);

		refTime = iConvert->RecordTimeAvailable().Int64();
		if (refTime <= 0 )
			return EInconclusive;

		//Halve the file size
		iConvert->SetMaxWriteLength(KDefaultFileSize/2);
		if (iConvert->State() != CMdaAudioConvertUtility::EOpen )
			return EFail;

		//Expect the record time to be less than half the original (headers/metadata)
		time = iConvert->RecordTimeAvailable().Int64();
		if (time <= 0 || time > refTime/2)
			return EFail;
		}

	//__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

