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
 * Static constructor for CTestStepRecorderOpen.
 *
 *
 * @return	"CTestStepRecorderOpen*"
 *			The constructed CTestStepRecorderOpen
 *
 * @xxxx
 * 
 */
CTestStepRecorderOpen* CTestStepRecorderOpen::NewL()
	{
	CTestStepRecorderOpen* self = new(ELeave) CTestStepRecorderOpen;
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
CTestStepRecorderOpen::CTestStepRecorderOpen()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0205-CP");
	iHeapSize = 120000;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderOpen::~CTestStepRecorderOpen()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderOpen::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0205-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::OpenL(<location>)"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName)) // wav file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName3"), iFileName2)) // au file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName4"), iFileName3)) // raw file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// load the contents of an audio file to a Descriptor
	TInt fSize;

	iFs.Connect();
	if ( iFile.Open(iFs, iFileName, EFileRead) != KErrNone )
		return EInconclusive;
	iFile.Size(fSize);
	// load the sound from the file to a Descriptor
	iBuf = HBufC8::NewL( fSize );
	TPtr8 buf = iBuf->Des();
	iFile.Read(buf);

	iFile.Close();
	iFs.Close();

	// create the Recorder utility
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	iClipLocation = new TMdaFileClipLocation(iFileName);
	iFormat = new TMdaWavClipFormat();

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderOpen::DoTestStepPostambleL(void)
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
void CTestStepRecorderOpen::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderOpen::OpenAndStartSchedulerL()
	{
	iRecorder->OpenL( iClipLocation, iFormat );
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
TVerdict CTestStepRecorderOpen::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// location: WAV file
	TRAP( err, OpenAndStartSchedulerL() );

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		{
		INFO_PRINTF1(_L("WAV test failed"));
		return EFail;
		}

	delete iRecorder;
	iRecorder = NULL;
	delete iClipLocation;
	iClipLocation = NULL;

	// location: Descriptor containg wav sound format
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;
	TPtr8 buf = iBuf->Des();
	iClipLocation = new TMdaDesClipLocation(buf);
	TRAP( err, OpenAndStartSchedulerL() );

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		{
		INFO_PRINTF1(_L("Descriptor test failed"));
		return EFail;
		}

	delete iRecorder;
	iRecorder = NULL;
	delete iClipLocation;
	iClipLocation = NULL;
	delete iFormat;
	iFormat = NULL;

	// location: File containg au sound format
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;
	iClipLocation = new TMdaFileClipLocation(iFileName2);
	iFormat = new TMdaAuClipFormat();
	TRAP( err, OpenAndStartSchedulerL() );

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		{
		INFO_PRINTF1(_L("AU test failed"));
		return EFail;
		}

	delete iRecorder;
	iRecorder = NULL;
	delete iClipLocation;
	iClipLocation = NULL;
	delete iFormat;
	iFormat = NULL;

	// location: File containg raw sound format
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;
	iClipLocation = new TMdaFileClipLocation(iFileName3);
	iFormat = new TMdaRawAudioClipFormat();
	TRAP( err, OpenAndStartSchedulerL() );

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		{
		INFO_PRINTF1(_L("RAW test failed"));
		return EFail;
		}

	delete iRecorder;
	iRecorder = NULL;
	delete iClipLocation;
	iClipLocation = NULL;
	delete iFormat;
	iFormat = NULL;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
	
/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestMmfAclntRecFileInvDesForm::CTestMmfAclntRecFileInvDesForm()
	{	
	iTestStepName = _L("MM-MMF-ACLNT-U-0969-HP");
	iHeapSize = 200000;		// playback part of test loads whole sound file into buffer, so need large heap
	}
	
/**
 *
 * Static constructor for CTestMmfAclntRecFileInvDesForm.
 *
 * @return	"CTestMmfAclntRecFileInvDesForm*"
 *			The constructed CTestMmfAclntRecFileInvDesForm
 *
 * @xxxx
 * 
 */
CTestMmfAclntRecFileInvDesForm* CTestMmfAclntRecFileInvDesForm::NewL()
	{
	CTestMmfAclntRecFileInvDesForm* self = new (ELeave) CTestMmfAclntRecFileInvDesForm();
	return self;
	}
	
/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestMmfAclntRecFileInvDesForm::DoTestStepPreambleL()
	{
	iTestStepResult = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if(iTestStepResult != EPass)
		{
		return EInconclusive;
		}
	if( !GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName4"), iFileName) )
		{
		return EInconclusive;
		}
	if( (iRecUtil = CMdaAudioRecorderUtility::NewL(*this)) == NULL )
		{
		return EInconclusive;
		}
	return EPass;
	}
	
/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestMmfAclntRecFileInvDesForm::DoTestStepPostambleL()
	{
	delete iRecUtil;
	iRecUtil = NULL;
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();	
	}

void CTestMmfAclntRecFileInvDesForm::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF4( _L("MMdaObjectStateChangeObserver: previous state: %d current state: %d error: %d"),aPreviousState, aCurrentState, aErrorCode);
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
TVerdict CTestMmfAclntRecFileInvDesForm::DoTestStepL()
	{
	INFO_PRINTF1( _L("Test the SetDestinationFormatL API with passing wrong UID, so that it should return KErrNotSupported"));
	INFO_PRINTF2(_L("CMdaAudioRecorderUtility->OpenFileL(%S)"), &iFileName);
	iError = KErrTimedOut;
	iTestStepResult = EFail;
	iRecUtil->OpenFileL(iFileName);		
	CActiveScheduler::Start();
	if ( iError == KErrNone )
		{
		// Get current format		
		TUid currentFormat = KNullUid;
		TRAPD(err, currentFormat = iRecUtil->DestinationFormatL());
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationFormatL failed with error %d."), err);
			return EFail;
			}
		else
			{
			INFO_PRINTF2(_L("Current format %x."), currentFormat);
			}			
		// Set invalid format
		TUid newFormat;
		newFormat.iUid = KMmfUidFormatRAWRead;
		TRAP(err, iRecUtil->SetDestinationFormatL(newFormat));
		if ( err != KErrNotSupported )
			{
			ERR_PRINTF2(_L("CMdaAudioRecorderUtility::SetDestinationFormatL failed with unexpected error %d."), err);
			return EFail;
			}
		else
			{
			INFO_PRINTF2(_L("CMdaAudioRecorderUtility::SetDestinationFormatL failed with expected error %d."), err);
			}
		// Get current format
		TRAP(err, currentFormat = iRecUtil->DestinationFormatL());
		if ( err != KErrNotSupported )
			{
			ERR_PRINTF2(_L("Current format %x."), currentFormat);
			return EFail;
			}
		else
			{
			INFO_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationFormatL failed with expected error %d."), err);
			iTestStepResult = EPass;
			}	
		}
	return iTestStepResult;
	}

