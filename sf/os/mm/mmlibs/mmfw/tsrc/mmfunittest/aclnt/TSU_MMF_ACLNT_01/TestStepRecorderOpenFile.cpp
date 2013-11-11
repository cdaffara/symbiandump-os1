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
 * Static constructor for CTestStepRecorderOpenFile.
 *
 *
 * @return	"CTestStepRecorderOpenFile*"
 *			The constructed CTestStepRecorderOpenFile
 *
 * @xxxx
 * 
 */
CTestStepRecorderOpenFile* CTestStepRecorderOpenFile::NewL(TBool aOpenFileTwice)
	{
	CTestStepRecorderOpenFile* self = new(ELeave) CTestStepRecorderOpenFile(aOpenFileTwice);
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
CTestStepRecorderOpenFile::CTestStepRecorderOpenFile(TBool aOpenFileTwice)
	{
	iOpenFileTwice = aOpenFileTwice;
	
	if (aOpenFileTwice)
		{
		iTestStepName = _L("MM-MMF-ACLNT-U-0202-CP");
		}
	else
		{
		// store the name of this test case
		// this is the name that is used by the script file
		iTestStepName = _L("MM-MMF-ACLNT-U-0203-CP");
		}
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderOpenFile::~CTestStepRecorderOpenFile()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderOpenFile::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	if (iOpenFileTwice)
		{
		INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0202-CP"));
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::OpenFileL() twice without waiting for an Open complete event"));
		}
	else
		{
		INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0203-CP"));
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::OpenFileL()"));
		}
	
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
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName5"), iFileNames[2])) // raw file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
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
enum TVerdict CTestStepRecorderOpenFile::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
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
void CTestStepRecorderOpenFile::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderOpenFile::OpenAndStartSchedulerL(TInt i)
	{
	iRecorder->OpenFileL( iFileNames[i] );
	
	if (iOpenFileTwice)
		{
		iRecorder->OpenFileL( iFileNames[i] );
		}

	CActiveScheduler::Start(); // wait for open to complete
	User::LeaveIfError(iError);
	iRecorder->PlayL();
	CActiveScheduler::Start(); // wait once for start of play
	User::LeaveIfError(iError);
	CActiveScheduler::Start(); // and once for end of play
	User::LeaveIfError(iError);
	}

const TInt KRawTest = 2;
	
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
TVerdict CTestStepRecorderOpenFile::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	for (TInt type=0; type<3; type++)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TRAP( err, OpenAndStartSchedulerL(type) );

		// RAW files cannot be opened if no format is specified, so unsupported is the
		// correct return code
		if (type == KRawTest)
			{
			if (err != KErrNotSupported)
				return EFail;
			}
		else 
			{
			if (iRecorder == NULL ||
				err != KErrNone ||
				iError != KErrNone ||
				iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
				return EFail;
			}

		delete iRecorder;
		iRecorder = NULL;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


/**
 *
 * Static constructor for CTestStepRecorderOpenMp3File.
 *
 *
 * @return	"CTestStepRecorderOpenMp3File*"
 *			The constructed CTestStepRecorderOpenMp3File
 *
 * @xxxx
 * 
 */
CTestStepRecorderOpenMp3File* CTestStepRecorderOpenMp3File::NewL()
	{
	CTestStepRecorderOpenMp3File* self = new(ELeave) CTestStepRecorderOpenMp3File;
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
CTestStepRecorderOpenMp3File::CTestStepRecorderOpenMp3File()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0823-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderOpenMp3File::~CTestStepRecorderOpenMp3File()
	{
	}



/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderOpenMp3File::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0823-HP"));
	INFO_PRINTF1(_L("this tests opening a clip by extension only"));


	
	// raw mp3 file
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayMp3FName1"), iFileName)) 
		{
		INFO_PRINTF2(_L("file name %S not found..."), &iFileName);
		return EInconclusive;
		}
	iFileInfo.Append(TMp3FileInfo(iFileName, EPass, KErrNone));

	// id3 mp3 file
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayMp3FName2"), iFileName)) 
		{
		INFO_PRINTF2(_L("file name %S not found..."), &iFileName);
		return EInconclusive;
		}
	iFileInfo.Append(TMp3FileInfo(iFileName, EPass, KErrNone));

	// wav mp3 file
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayMp3FName3"), iFileName)) 
		{
		INFO_PRINTF2(_L("file name %S not found..."), &iFileName);
		return EInconclusive;
		}
	iFileInfo.Append(TMp3FileInfo(iFileName, EPass, KErrNone));

	// wav pcm file
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName)) 
		{
		INFO_PRINTF2(_L("file name %S not found..."), &iFileName);
		return EInconclusive;
		}
	iFileInfo.Append(TMp3FileInfo(iFileName, EPass, KErrNone));

	// dummy mp3 file
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayMp3FName4"), iFileName)) 
		{
		INFO_PRINTF2(_L("file name %S not found..."), &iFileName);
		return EInconclusive;
		}
	iFileInfo.Append(TMp3FileInfo(iFileName, EFail, KErrNotSupported));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderOpenMp3File::DoTestStepPostambleL(void)
	{
	iFileInfo.Close();

	delete iRecorder;
	iRecorder = NULL;
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
void CTestStepRecorderOpenMp3File::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	iError = aErrorCode;
	iState = aCurrentState;
	INFO_PRINTF4(_L("MoscoStateChangeEvent() : Prev state = %d, curr state = %d, error = %d"), aPreviousState, aCurrentState, aErrorCode);
	CActiveScheduler::Stop();
	}

void CTestStepRecorderOpenMp3File::OpenAndStartSchedulerL()
	{
	iRecorder->OpenFileL( iFileName );
	CActiveScheduler::Start(); // wait for open to complete
	User::LeaveIfError(iError);
	

	iRecorder->PlayL();
	CActiveScheduler::Start(); // wait once for start of play
	User::LeaveIfError(iError);
	CActiveScheduler::Start(); // and once for end of play
	User::LeaveIfError(iError);

	}


TVerdict CTestStepRecorderOpenMp3File::DoTestStepL()
	{
	TInt n;

	TVerdict testVerdict = EPass;

	for (n=0; n<iFileInfo.Count(); n++)
		{
		iFileName.Set(iFileInfo[n].iFileName);
		INFO_PRINTF3(_L("Test #%d: Opening %S"), n, &iFileName);
		TVerdict stepVerdict = DoNextTestStepL();
		if (stepVerdict != iFileInfo[n].iExpectedVerdict)
			{
			testVerdict = stepVerdict;
			INFO_PRINTF3(_L("Unexpected verdict = %d, error = %d"), stepVerdict, iError);
			}
		}
	return testVerdict;
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
TVerdict CTestStepRecorderOpenMp3File::DoNextTestStepL()
	{
	TVerdict verdict = EPass;
    TInt err = KErrNone;
	iError = KErrNone;

	// create the Recorder utility
	if ((iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL)
		return EInconclusive;

	TRAP( err, OpenAndStartSchedulerL() );

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		{
		INFO_PRINTF4(_L("err = %d, iError = %d, state = iState"), err, iError, iState);
		
		// "return" error to caller
		if (iError == KErrNone)	
			iError = err;
		
		verdict = EFail;
		}

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));

	// test steps return a result
	return verdict;
	}


