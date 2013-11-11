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

#define KMmfUidTestController 0x101F7D2A

//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepRecorderVol.
 *
 *
 * @return	"CTestStepRecorderVol*"
 *			The constructed CTestStepRecorderVol
 *
 * @xxxx
 * 
 */
CTestStepRecorderVol* CTestStepRecorderVol::NewL()
	{
	CTestStepRecorderVol* self = new(ELeave) CTestStepRecorderVol;
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
CTestStepRecorderVol::CTestStepRecorderVol() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0244-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderVol::~CTestStepRecorderVol()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepRecorderVol::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0244-CP"));
	 INFO_PRINTF1(_L("this test is playing a sound from a recorder utility with several Volume levels. SetVolume(TInt)"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) )
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	iRecorder->OpenFileL(iFileName);
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderVol::DoTestStepPostambleL(void)
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
void CTestStepRecorderVol::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	iHasStartedPlaying = EFalse;
	iHasFinishedPlaying = ETrue;

	if (aPreviousState == CMdaAudioRecorderUtility::EOpen &&
		aCurrentState == CMdaAudioRecorderUtility::EPlaying)	
		iHasStartedPlaying = ETrue;
	if (aPreviousState == CMdaAudioRecorderUtility::EPlaying &&
		aCurrentState == CMdaAudioRecorderUtility::EOpen)	
		iHasFinishedPlaying = ETrue;

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
TVerdict CTestStepRecorderVol::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt maxVol = 0; 
	TInt tmpVol;
	TInt currVol;
	// getting the MAx Volume without an open file would result in panic
	
	// get the max Volume
	TRAP(err, maxVol = iRecorder->MaxVolume());
	if (err != KErrNone)
		{
		return EFail;
		}
	
	// test whether the volume set back to maxvolume in the case of volume is greater than maxvolume 
	INFO_PRINTF1(_L("Test setting more than Max Volume"));
	tmpVol = maxVol + 10;
	err = iRecorder->SetVolume(tmpVol);
	if (err != KErrNone)
		{
		return EFail;
		}
	
	// get the current volume
	iRecorder->GetVolume(currVol);

	// maxvol and currvol should be same 
	if (maxVol != currVol)
		{
		return EFail;
		}
		
	// test whether the volume set back to zero in the case of volume is less than zero 
	INFO_PRINTF1(_L("Test setting less than Min Volume"));
	tmpVol = -10;
	err = iRecorder->SetVolume(tmpVol);
	if (err != KErrNone)
		{
		return EFail;
		}

	// get the current volume
	iRecorder->GetVolume(currVol);
		
	// currVol should be zero
	if (currVol != 0)
		{
		return EFail;
		}
	
	tmpVol = maxVol;
	
	// try several volume values and play
	INFO_PRINTF1(_L("Test setting various volume levels"));
	if (maxVol >0)
		{
		for (TInt vol=0 ; vol<=maxVol; vol+=(maxVol/10)) // repeats loop
			{
			TRAP(err, iRecorder->SetVolume(vol));
			if (err != KErrNone ||
				iRecorder->GetVolume(tmpVol) != KErrNone)
				return EFail;
			if (tmpVol != vol)
				return EFail;
			}
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
	
/**
 *
 * Static constructor for CTestStepRecorderLowMaxVol.
 *
 *
 * @return	"CTestStepRecorderLowMaxVol*"
 *			The constructed CTestStepRecorderLowMaxVol
 *
 * @xxxx
 * 
 */
CTestStepRecorderLowMaxVol* CTestStepRecorderLowMaxVol::NewL()
	{
	CTestStepRecorderLowMaxVol* self = new(ELeave) CTestStepRecorderLowMaxVol;
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
CTestStepRecorderLowMaxVol::CTestStepRecorderLowMaxVol() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0965-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderLowMaxVol::~CTestStepRecorderLowMaxVol()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepRecorderLowMaxVol::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0965-HP"));
	 INFO_PRINTF1(_L("this test is playing a sound from a recorder utility to test the volume if it is < 0 Volume"));
	 INFO_PRINTF1(_L(" that sets back to 0 and if it is > max volume that sets back to max volume"));


	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) )
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}
	
     iRecorder = CMdaAudioRecorderUtility::NewL(*this);
	 iRecorder->OpenFileL(iFileName);
	 CActiveScheduler::Start();
	 if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;	
	 
	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderLowMaxVol::DoTestStepPostambleL(void)
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
void CTestStepRecorderLowMaxVol::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	iHasStartedPlaying = EFalse;
	iHasFinishedPlaying = ETrue;

	if (aPreviousState == CMdaAudioRecorderUtility::EOpen &&
		aCurrentState == CMdaAudioRecorderUtility::EPlaying)	
		iHasStartedPlaying = ETrue;
	if (aPreviousState == CMdaAudioRecorderUtility::EPlaying &&
		aCurrentState == CMdaAudioRecorderUtility::EOpen)	
		iHasFinishedPlaying = ETrue;

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
TVerdict CTestStepRecorderLowMaxVol::DoTestStepL()
	{
	TInt maxVol = 0; 
	TInt tmpVol;
	TInt currVol;
	// getting the MAx Volume without an open file would result in panic
	// get the max Volume
	maxVol = iRecorder->MaxVolume();

	// test whether the volume set back to maxvolume in the case of volume is greater than maxvolume 
	tmpVol = maxVol + 10;
	iRecorder->SetVolume(tmpVol);
	
	// get the current volume
	iRecorder->GetVolume(currVol);

	// maxvol and currvol should be same 
	if (maxVol != currVol)
		{
		return EFail;
		}
	// test whether the volume set back to zero in the case of volume is less than zero 
	tmpVol = -10;
	iRecorder->SetVolume(tmpVol);

	// get the current volume
	iRecorder->GetVolume(currVol);
	// currVol should be zero
	if (currVol != 0)
		{
		return EFail;
		}
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return EPass;
	}

/**
 *
 * Static constructor for CTestStepPlayerLowMaxVol.
 *
 *
 * @return	"CTestStepPlayerLowMaxVol*"
 *			The constructed CTestStepPlayerLowMaxVol
 *
 * @xxxx
 * 
 */
CTestStepPlayerLowMaxVol* CTestStepPlayerLowMaxVol::NewL()
	{
	CTestStepPlayerLowMaxVol* self = new(ELeave) CTestStepPlayerLowMaxVol;
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
CTestStepPlayerLowMaxVol::CTestStepPlayerLowMaxVol() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0967-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerLowMaxVol::~CTestStepPlayerLowMaxVol()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepPlayerLowMaxVol::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0965-HP"));
	 INFO_PRINTF1(_L("this test is playing a sound from a recorder utility to test the volume if it is < 0 Volume"));
	 INFO_PRINTF1(_L(" that sets back to 0 and if it is > max volume that sets back to max volume"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) )
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}
	
	 iPlayer = CMdaAudioPlayerUtility::NewL(*this);
	 iPlayer->OpenFileL(iFileName);
	 CActiveScheduler::Start();

	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerLowMaxVol::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
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
TVerdict CTestStepPlayerLowMaxVol::DoTestStepL()
	{
	TInt maxVol = 0; 
	TInt tmpVol;
	TInt currVol;
	// getting the MAx Volume without an open file would result in panic	
	// get the max Volume
	maxVol = iPlayer->MaxVolume();

	// test whether the volume set back to maxvolume in the case of volume is greater than maxvolume 
	tmpVol = maxVol + 10;

	// first check if an error occurred whilst setting the volume
	TInt err = iPlayer->SetVolume(tmpVol);
	if (err != KErrNone)		
		{
		return EFail;
		}

	// get the current volume
	iPlayer->GetVolume(currVol);

	// maxvol and currvol should be same 
	if (maxVol != currVol)
		{
		return EFail;
		}
		
	// test whether the volume set back to zero in the case of volume is less than zero 
	tmpVol = -10;
	iPlayer->SetVolume(tmpVol);

	// get the current volume
	iPlayer->GetVolume(currVol);
		
	// currVol should be zero
	if (currVol != 0)
		{
		return EFail;
		}
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return EPass;
	}

void CTestStepPlayerLowMaxVol::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerLowMaxVol::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

/**
 *
 * Static constructor for CTestStepRecorderVolError
 *
 *
 * @return	"CTestStepRecorderVolError*"
 *			The constructed CTestStepRecorderVolError
 *
 * @xxxx
 * 
 */
CTestStepRecorderVolError* CTestStepRecorderVolError::NewL()
	{
	CTestStepRecorderVolError* self = new(ELeave) CTestStepRecorderVolError;
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
CTestStepRecorderVolError::CTestStepRecorderVolError() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0277-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderVolError::~CTestStepRecorderVolError()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepRecorderVolError::DoTestStepPreambleL(void)
	 {
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	 // Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0277-CP"));
	 INFO_PRINTF1(_L("this test is confirming that an error in SetVolume(TInt) is returned properly (see INC115421)"));

	 iError = KErrNone; 

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) )
		 {
		 return EInconclusive;
		 }

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	iRecorder->OpenFileL(iFileName,
			   TUid::Uid(KMmfUidTestController), //Open test controller to ensure proper error code returned
			   KNullUid,
			   TUid::Uid(KMmfUidFormatWAVWrite),
			   TFourCC(KMMFFourCCCodeNULL));
			   
	CActiveScheduler::Start();
	if (iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
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
enum TVerdict CTestStepRecorderVolError::DoTestStepPostambleL(void)
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
void CTestStepRecorderVolError::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	iHasStartedPlaying = EFalse;
	iHasFinishedPlaying = ETrue;

	if (aPreviousState == CMdaAudioRecorderUtility::EOpen &&
		aCurrentState == CMdaAudioRecorderUtility::EPlaying)
		{
		iHasStartedPlaying = ETrue;
		}
	if (aPreviousState == CMdaAudioRecorderUtility::EPlaying &&
		aCurrentState == CMdaAudioRecorderUtility::EOpen)	
		{
		iHasFinishedPlaying = ETrue;
		}
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
TVerdict CTestStepRecorderVolError::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt errVol = -999; 
    //Volume set to -999 to ensure test controller returns proper error code -12345
	//Test controller modified to look for -999 volume setting and return -12345
	
	INFO_PRINTF1(_L("Test setting volume to -999, expect error -12345"));

	err = iRecorder->SetVolume(errVol);
	if (err != -12345)
		{
		INFO_PRINTF2(_L("Returned Error: %d not -12345"), err);
		return EFail;
		}
	
	return iTestStepResult;
	}
