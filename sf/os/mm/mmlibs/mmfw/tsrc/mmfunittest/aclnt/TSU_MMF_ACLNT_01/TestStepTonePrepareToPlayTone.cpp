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
// TestStepToneAllocFailNew.cpp
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

#include <mdaaudiotoneplayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepTone.h"


//#include <mmfaudiocontroller.h>
#include "mmfclienttoneplayer.h"
//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepTonePrepareToPlayTone.
 *
 *
 * @return	"CTestStepTonePrepareToPlayTone*"
 *			The constructed CTestStepTonePrepareToPlayTone
 *
 * @xxxx
 * 
 */
CTestStepTonePrepareToPlayTone* CTestStepTonePrepareToPlayTone::NewL()
	{
	CTestStepTonePrepareToPlayTone* self = new(ELeave) CTestStepTonePrepareToPlayTone;
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
CTestStepTonePrepareToPlayTone::CTestStepTonePrepareToPlayTone() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0451-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepTonePrepareToPlayTone::~CTestStepTonePrepareToPlayTone()
	{
	}

void CTestStepTonePrepareToPlayTone::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepTonePrepareToPlayTone::MatoPlayComplete(TInt aError)
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
enum TVerdict CTestStepTonePrepareToPlayTone::DoTestStepPreambleL(void)
	{
    TInt err       = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0451-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioToneUtility::PrepareToPayTone() "));
	
	// create the Recorder utility
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	if ( err != KErrNone )
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
enum TVerdict CTestStepTonePrepareToPlayTone::DoTestStepPostambleL(void)
	{
	delete iTone;
	iTone = NULL;
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
TVerdict CTestStepTonePrepareToPlayTone::DoTestStepL()
	{
	iTestStepResult = EPass;

	for (TInt seq=10; seq<=40000; seq*=2)
		{
		iTone->PrepareToPlayTone(seq,TTimeIntervalMicroSeconds(500000));
		CActiveScheduler::Start();

		TInt err1 = KErrNone;
		TInt type = 0;
		err1 = GetAudToneType(iTone, type);
		if (err1 != KErrNone ||
			type != CMMFToneConfig::EMmfToneTypeSimple ||
			iError != KErrNone ||
			iTone->State() != EMdaAudioToneUtilityPrepared)
			return EFail;
		iTone->CancelPrepare();
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


/**
 *
 * Static constructor for CTestStepTonePlayZeroLength.
 *
 *
 * @return	"CTestStepTonePlayZeroLength*"
 *			The constructed CTestStepTonePlayZeroLength
 *
 * @xxxx
 * 
 */
CTestStepTonePlayZeroLength* CTestStepTonePlayZeroLength::NewL()
	{
	CTestStepTonePlayZeroLength* self = new(ELeave) CTestStepTonePlayZeroLength;
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
CTestStepTonePlayZeroLength::CTestStepTonePlayZeroLength() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0829-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepTonePlayZeroLength::~CTestStepTonePlayZeroLength()
	{
	}


void CTestStepTonePlayZeroLength::MatoPrepareComplete(TInt aError)
	{
	if(aError == KErrNone)
		{
		TInt vol = iTone->MaxVolume();
		iTone->SetVolume(vol);
		INFO_PRINTF1(_L("CTestStepTonePlayZeroLength::MatoPrepareComplete - About to play note"));
		iTone->Play();
		}
	else
		{
		INFO_PRINTF2(_L("CTestStepTonePlayZeroLength::MatoPrepareComplete - error occured whilst preparing note: %d"), aError);
		if(aError == KErrInUse)
			{
			INFO_PRINTF1(_L("CTestStepTonePlayZeroLength::MatoPrepareComplete - The sound module is already in use by another application"));
			iTestStepResult = EFail;
			CActiveScheduler::Stop();
			}
		}
	}


void CTestStepTonePlayZeroLength::MatoPlayComplete(TInt aError)
	{
	if(aError == KErrNone)
		{
		INFO_PRINTF1(_L("CTestStepTonePlayZeroLength::MatoPlayComplete - Note finished"));
		CActiveScheduler::Stop();
		}
	else
		{
		INFO_PRINTF2(_L("CTestStepTonePlayZeroLength::MatoPrepareComplete - error occured whilst playing note: %d\n"), aError);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepTonePlayZeroLength::DoTestStepPreambleL(void)
	{
    TInt err = KErrNone;
	
	// this installs the scheduler
	TVerdict verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0829-CP"));
	INFO_PRINTF1(_L("this is a test of Playing zero length tone on CMdaAudioToneUtility "));
	
	// create the Tone utility
	TRAP( err, (iTone = CMdaAudioToneUtility::NewL(*this)) );
	if ( err != KErrNone )
		{
		verdict = EInconclusive;
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
TVerdict CTestStepTonePlayZeroLength::DoTestStepPostambleL(void)
	{
	delete iTone;
	iTone = NULL;
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

TVerdict CTestStepTonePlayZeroLength::DoTestStepL()
	{
	iTestStepResult = EPass;

	iTone->PrepareToPlayTone(22000, TTimeIntervalMicroSeconds(0));

	CActiveScheduler::Start();

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}




RTestStepTonePlayBeforeInitialized* RTestStepTonePlayBeforeInitialized::NewL()
	{
	RTestStepTonePlayBeforeInitialized* self = new (ELeave) RTestStepTonePlayBeforeInitialized();
	return self;
	}

RTestStepTonePlayBeforeInitialized::RTestStepTonePlayBeforeInitialized()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0970-HP");
	}

void RTestStepTonePlayBeforeInitialized::MatoPrepareComplete(TInt /*aError*/)
	{
	}
	
void RTestStepTonePlayBeforeInitialized::MatoPlayComplete(TInt aError)
	{
		#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		if(aError == KErrNotSupported)
			{
			ERR_PRINTF1( _L("Play Tone Fixed Sequence is no longer supported"));
			StopTest(KErrNone);
			}
		else
			{
			StopTest (aError);
			}
		#else

		StopTest(aError);
		#endif
	}

void RTestStepTonePlayBeforeInitialized::KickoffTestL()
	{
	iToneActiveObject = new (ELeave) CToneActiveObject(*this);
	
	iTone = CMMFMdaAudioToneUtility::NewL(*this);
	iToneActiveObject->Activate();
	TRequestStatus *status = &(iToneActiveObject->iStatus);
	User::RequestComplete(status, KErrNone);
	}
	
void RTestStepTonePlayBeforeInitialized::CloseTest()
	{
	delete iToneActiveObject;
	delete iTone;
	}
	
CToneActiveObject::CToneActiveObject(RTestStepTonePlayBeforeInitialized& aTest) : 
CActive(EPriorityAbsoluteHigh), iTest(aTest)
	{
	CActiveScheduler::Add(this);
	}
	
CToneActiveObject::~CToneActiveObject()
	{
	}
	
void CToneActiveObject::Activate()
	{
	SetActive();
	}
	
void CToneActiveObject::RunL()
	{
	iTest.iTone->Play();
	}
	
void CToneActiveObject::DoCancel()
	{
	
	}

// Plain tone play

RTestStepTonePlayWhenBusy::RTestStepTonePlayWhenBusy():
	iTone(NULL)
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-2500-CP");
	}

RTestStepTonePlayWhenBusy* RTestStepTonePlayWhenBusy::NewL()
	{
	RTestStepTonePlayWhenBusy* self = new (ELeave) RTestStepTonePlayWhenBusy;
	return self;
	}
	
void RTestStepTonePlayWhenBusy::KickoffTestL()
	{
	// read config parameters
	if (!GetIntFromConfig(iDefaultParamSet, _L("BusyPercent"), iBusyPercent))
		{
		INFO_PRINTF2(_L("%S::BusyPercent not found in config file"), &iDefaultParamSet);
		StopTest(EInconclusive);
		return;
		}
	if (!GetIntFromConfig(iDefaultParamSet, _L("BusyDelay"), iBusyDelay))
		{
		INFO_PRINTF2(_L("%S::BusyDelay not found in config file"), &iDefaultParamSet);
		StopTest(EInconclusive);
		return;
		}
	if (!GetIntFromConfig(iDefaultParamSet, _L("BusyLength"), iBusyLength))
		{
		INFO_PRINTF2(_L("%S::BusyLength not found in config file"), &iDefaultParamSet);
		StopTest(EInconclusive);
		return;
		}
	if (!GetIntFromConfig(iDefaultParamSet, _L("ToneLength"), iToneLength))
		{
		INFO_PRINTF2(_L("%S::ToneLength not found in config file"), &iDefaultParamSet);
		StopTest(EInconclusive);
		return;
		}
		
	INFO_PRINTF4(_L("%S BusyPercent=%d ToneLength=%d"), &iDefaultParamSet, iBusyPercent, iToneLength);
	iTone = CMdaAudioToneUtility::NewL(*this);
	iBusy = CBusyTestUnit::NewL(iBusyPercent, EPriorityAbsoluteRealTime4);
	
	iBusy->Start(iBusyDelay, iBusyLength);
	// from after iBusyDelay this will consume lots of CPU time
	
	iTone->PrepareToPlayTone(250, iToneLength);
	}
	
void RTestStepTonePlayWhenBusy::MatoPrepareComplete(TInt aError)
	{
	if (aError)
		{
		INFO_PRINTF2(_L("MatoPrepareComplete(%d)"), aError);
		StopTest(aError);
		}
	else
		{
		iTone->Play();
		}
	}
	
void RTestStepTonePlayWhenBusy::MatoPlayComplete(TInt aError)
	{
	if (aError)
		{
		INFO_PRINTF2(_L("MatoPlayComplete(%d)"), aError);
		StopTest(aError);
		}
	else
		{
		StopTest(); // success!
		}
	}
	
void RTestStepTonePlayWhenBusy::CloseTest()
	{
	delete iBusy; // calls stop
	iBusy = NULL;
	delete iTone;
	iTone = NULL;
	}

