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

#include <mdaaudiosampleplayer.h>
//#include <MdaAudioSampleEditor.h>
//#include <MdaAudioTonePlayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepPlayerManip.h"

#include "mmfclientaudioplayer.h"
#include "../../ctlfrm/TS_Codes.h"

_LIT(KAudioControllerName,"Symbian Audio Wav Test controller");

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepPlayerPosition.
 *
 *
 * @return	"CTestStepPlayerPosition*"
 *			The constructed CTestStepPlayerPosition
 *
 * @xxxx
 * 
 */
CTestStepPlayerPosition* CTestStepPlayerPosition::NewL()
	{
	CTestStepPlayerPosition* self = new(ELeave) CTestStepPlayerPosition;
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
CTestStepPlayerPosition::CTestStepPlayerPosition() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0035-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerPosition::~CTestStepPlayerPosition()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerPosition::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerPosition::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	if (iError == KErrNone)
		iHasPlayed = ETrue;
	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepPlayerPosition::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0035-CP"));
	 INFO_PRINTF1(_L("this test is getting and setting the postion of a sound file after openning it, and playing it. GetPosition()"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	 iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	 CActiveScheduler::Start();
	 User::After(1000000);

	 if (iPlayer == NULL ||
		 iError != KErrNone )
		 return EInconclusive;

	 //store Duration
	 iDuration = iPlayer->Duration();
	 if (iDuration <= TTimeIntervalMicroSeconds(0) )
		 return EInconclusive;

	// check if player openned the file successfully
	TInt err1 = KErrNone;
	TInt state = 0;
	err1 = GetAudPlayerState(iPlayer, state);
	if ( err1 != KErrNone ||
		 state != CMMFMdaAudioPlayerUtility::EStopped )
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
enum TVerdict CTestStepPlayerPosition::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	//[ Destroy the scheduler ]
	delete iPlayer;
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
TVerdict CTestStepPlayerPosition::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err; 
	TTimeIntervalMicroSeconds pos;
	//__MM_HEAP_MARK;

	// check if position 0 before playing
	err = iPlayer->GetPosition(pos);
	if (err != KErrNone ||
		pos != TTimeIntervalMicroSeconds(0))
		return EFail;

	// set position and check
	iPlayer->SetPosition(TTimeIntervalMicroSeconds(500000));
	err = iPlayer->GetPosition(pos);
	if (err != KErrNone ||
		pos != TTimeIntervalMicroSeconds(500000))
		return EFail;

	// set position and check
	iPlayer->SetPosition(TTimeIntervalMicroSeconds(10000));
	err = iPlayer->GetPosition(pos);
	if (err != KErrNone ||
		pos != TTimeIntervalMicroSeconds(10000))
		return EFail;

	//  play pause and check
	iPlayer->Play();
	CActiveScheduler::Start();


	//INC24264
	err = iPlayer->GetPosition(pos);
	if ((err == KErrNone) || (pos <= TTimeIntervalMicroSeconds(0)))
		return iTestStepResult = EPass;


	// stop it after 200 milisecs, not accurate though
	/*User::After(200000);
	iPlayer->Pause();

	// check position again, we don't have an accurate opsition
	err = iPlayer->GetPosition(pos);
	if (err != KErrNone ||
		pos <= TTimeIntervalMicroSeconds(0) ||
		pos > iDuration )
		return EFail;*/

	delete iPlayer;
	iPlayer = NULL;
	//__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// class CTestStepPlayerPlayWindow
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerPlayWindow.
 *
 *
 * @return	"CTestStepPlayerPlayWindow*"
 *			The constructed CTestStepPlayerPlayWindow
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayWindow* CTestStepPlayerPlayWindow::NewL()
	{
	CTestStepPlayerPlayWindow* self = new(ELeave) CTestStepPlayerPlayWindow;
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
CTestStepPlayerPlayWindow::CTestStepPlayerPlayWindow() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0050-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayWindow::~CTestStepPlayerPlayWindow()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerPlayWindow::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerPlayWindow::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	if (iError == KErrNone)
		iHasPlayed = ETrue;
	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepPlayerPlayWindow::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0050-CP"));
	 INFO_PRINTF1(_L("this test is setting and clearing the Play Window of a sound file after openning it [Set|Clear]PlayWindow()"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	 iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	 CActiveScheduler::Start();
	 User::After(1000000);

	 if (iPlayer == NULL ||
		 iError != KErrNone )
		 return EInconclusive;

	 //store Duration
	 iDuration = iPlayer->Duration();
	 if (iDuration <= TTimeIntervalMicroSeconds(0) )
		 return EInconclusive;

	iBeginning = TTimeIntervalMicroSeconds( 0 );
	iMiddle = TTimeIntervalMicroSeconds( iPlayer->Duration().Int64() / 2 );
	iEnd = iPlayer->Duration();

	// check if player openned the file successfully
	TInt err1 = KErrNone;
	TInt state = 0;
	err1 = GetAudPlayerState(iPlayer, state);
	if ( err1 != KErrNone ||
		 state != CMMFMdaAudioPlayerUtility::EStopped )
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
enum TVerdict CTestStepPlayerPlayWindow::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerPlayWindow::DoTestStepL()
	{
	TInt err; 
	//__MM_HEAP_MARK;


	TRAP(err, iPlayer->SetPlayWindow(iBeginning, iMiddle) ); 
	iPlayer->Play();
	// Wait for a MapcPlayComplete() callback
	// calling User::After(1200000) is no good because the active scheduler
	// is not running so no RunL()'s can work 
	// i.e. nothing can come back from controller & the AudioPlayer utility's
	// state will not get updated.
	//User::After(1200000); // wrong 
	CActiveScheduler::Start();
	if (err != KErrNone) // no way to check programmatically without access to the controller
		//|| iPlayer->Duration() != TTimeIntervalMicroSeconds(iMiddle.Int64()-iBeginning.Int64()) )
		return EInconclusive;

	TRAP(err, iPlayer->ClearPlayWindow() );
	if (err != KErrNone ) 
		return EFail;
	iPlayer->Play();
	CActiveScheduler::Start();

	if (iError!= KErrNone)
		{
		INFO_PRINTF2(_L("iPlayer->Play() failed, iError = %d"), iError);
		return EFail;
		}

	delete iPlayer;
	iPlayer = NULL;
	//__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// class CTestStepPlayerBalance
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerBalance.
 *
 *
 * @return	"CTestStepPlayerBalance*"
 *			The constructed CTestStepPlayerBalance
 *
 * @xxxx
 * 
 */
CTestStepPlayerBalance* CTestStepPlayerBalance::NewL()
	{
	CTestStepPlayerBalance* self = new(ELeave) CTestStepPlayerBalance;
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
CTestStepPlayerBalance::CTestStepPlayerBalance()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0052-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerBalance::~CTestStepPlayerBalance()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerBalance::MapcInitComplete(TInt aError, 
											  const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerBalance::MapcPlayComplete(TInt aError)
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
 enum TVerdict CTestStepPlayerBalance::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0052-HP"));
	 INFO_PRINTF1(_L("this test is setting and getting the balance of a sound: [Set|Get]Balance()"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	 iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	 CActiveScheduler::Start();
	 User::After(1000000);

	 if (iPlayer == NULL ||
		 iError != KErrNone )
		 return EInconclusive;

	 //store Duration
	 iDuration = iPlayer->Duration();
	 if (iDuration <= TTimeIntervalMicroSeconds(0) )
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
enum TVerdict CTestStepPlayerBalance::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerBalance::DoTestStepL()
	{
	TInt err; 
	TInt balance = 0; // KMMFBalanceCenter
	
	// test 1
	err = iPlayer->SetBalance(); // default KMMFBalanceCenter = 0
	if (err != KErrNone)	// we expect this to return KErrNone in Typhoon
		return EFail;

	err = iPlayer->GetBalance(balance);
	if (err != KErrNone ||
		balance != 0)
		return EFail;


	// test 2
	err = iPlayer->SetBalance(KMMFBalanceMaxLeft); // -100
	if (err != KErrNone)
		return EFail;

	err = iPlayer->GetBalance(balance);
	if (err != KErrNone ||
		balance != KMMFBalanceMaxLeft)
		return EFail;

	// test 3
	err = iPlayer->SetBalance(KMMFBalanceMaxRight);
	if (err != KErrNone)
		return EFail;

	err = iPlayer->GetBalance(balance);
	if (err != KErrNone ||
		balance != KMMFBalanceMaxRight)
		return EFail;

	// test 4
	err = iPlayer->SetBalance(KMMFBalanceMaxRight/2);
	if (err != KErrNone)
		return EFail;

	err = iPlayer->GetBalance(balance);
	if (err != KErrNone ||
		balance != KMMFBalanceMaxRight/2)
		return EFail;


	// test 4 out of bounds
	/* until we check the return codes...
	TRAP(err, iPlayer->SetBalance(KMMFBalanceMaxRight+5));
	if (err != KErrArgument ||
		err != KErrTooBig )
		return EFail;
	*/

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;	}

//
// class CTestStepPlayerClose
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerClose.
 *
 *
 * @return	"CTestStepPlayerClose*"
 *			The constructed CTestStepPlayerClose
 *
 * @xxxx
 * 
 */
CTestStepPlayerClose* CTestStepPlayerClose::NewL()
	{
	CTestStepPlayerClose* self = new(ELeave) CTestStepPlayerClose;
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
CTestStepPlayerClose::CTestStepPlayerClose()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0044-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerClose::~CTestStepPlayerClose()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerClose::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerClose::MapcPlayComplete(TInt aError)
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
 enum TVerdict CTestStepPlayerClose::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0044-CP"));
	 INFO_PRINTF1(_L("this test is closing an open sound: Close()"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	 iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	 CActiveScheduler::Start();
	 User::After(1000000);

	 if (iPlayer == NULL ||
		 iError != KErrNone )
		 return EInconclusive;

	 //store Duration
	 iDuration = iPlayer->Duration();
	 if (iDuration <= TTimeIntervalMicroSeconds(0) )
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
enum TVerdict CTestStepPlayerClose::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	//User::After(1000000);
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
TVerdict CTestStepPlayerClose::DoTestStepL()
	{
	TInt err; 
	
	// test 1
	TRAP(err, iPlayer->Close());
	if (iPlayer == NULL ||  // better check condition?
		err != KErrNone)
		return EFail;

	// re-open file
	iPlayer->OpenFileL(iFileName);
	CActiveScheduler::Start();
	User::After(1000000);
	// check
	if (iPlayer == NULL ||
		iError != KErrNone )
		return EInconclusive;

	// test 2
	//TRAP(err, iPlayer->Close());
	if (iPlayer == NULL || // better check condition?
		err != KErrNone)
		return EFail;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	
	}

//
// class CTestStepPlayerMeta
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerMeta.
 *
 *
 * @return	"CTestStepPlayerMeta*"
 *			The constructed CTestStepPlayerMeta
 *
 * @xxxx
 * 
 */
CTestStepPlayerMeta* CTestStepPlayerMeta::NewL()
	{
	CTestStepPlayerMeta* self = new(ELeave) CTestStepPlayerMeta;
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
CTestStepPlayerMeta::CTestStepPlayerMeta()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0045-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerMeta::~CTestStepPlayerMeta()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerMeta::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerMeta::MapcPlayComplete(TInt aError)
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
 enum TVerdict CTestStepPlayerMeta::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0045-HP"));
	 INFO_PRINTF1(_L("this test is about metadata manipulation"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameMeta"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	 iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	 CActiveScheduler::Start();
	 User::After(1000000);

	 if (iPlayer == NULL ||
		 iError != KErrNone )
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
enum TVerdict CTestStepPlayerMeta::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerMeta::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err, num; 
	
	err = iPlayer->GetNumberOfMetaDataEntries(iNum);

#ifdef _TYPHOON_TEST_
	if (err != KErrNotSupported)
		return EFail;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone || iNum == 0 )
		return EFail;  // check a known number of meta data
#endif

	_LIT(KMetaName, "To be changed N");
	_LIT(KMetaValue, "To be changed V");
	CMMFMetaDataEntry* meta = CMMFMetaDataEntry::NewL(KMetaName, KMetaValue);
	CleanupStack::PushL(meta);

	for (num = 0; num <iNum; num++)
		{
		TRAP(err, meta = iPlayer->GetMetaDataEntryL(num) ); // check if they count from 0
		#ifdef _TYPHOON_TEST_
			if (err != KErrNotSupported)
				return EFail;
		#endif

		#ifndef _TYPHOON_TEST_
			if (err != KErrNone ||
				meta->Name() == KMetaName || 
				meta->Value() == KMetaValue )
				return EFail;
		#endif
		}

	CleanupStack::PopAndDestroy(meta);


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	
	}

//
// class CTestStepPlayerPriority
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerPriority.
 *
 *
 * @return	"CTestStepPlayerPriority*"
 *			The constructed CTestStepPlayerPriority
 *
 * @xxxx
 * 
 */
CTestStepPlayerPriority* CTestStepPlayerPriority::NewL()
	{
	CTestStepPlayerPriority* self = new(ELeave) CTestStepPlayerPriority;
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
CTestStepPlayerPriority::CTestStepPlayerPriority()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0053-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerPriority::~CTestStepPlayerPriority()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerPriority::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerPriority::MapcPlayComplete(TInt aError)
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
 enum TVerdict CTestStepPlayerPriority::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0053-HP"));
	 INFO_PRINTF1(_L("this test is about the priority of the audio player"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameMeta"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	 iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	 CActiveScheduler::Start();
	 User::After(1000000);

	 if (iPlayer == NULL ||
		 iError != KErrNone )
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
enum TVerdict CTestStepPlayerPriority::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerPriority::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone; 
	
	iPlayer->SetPriority(EMdaPriorityNormal, EMdaPriorityPreferenceQuality);

	TMMFPrioritySettings priorSets;
	err = GetAudPlayerPriority(iPlayer, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityNormal ||
		priorSets.iPref != EMdaPriorityPreferenceQuality)
		return EFail;

	iPlayer->SetPriority(EMdaPriorityNormal, EMdaPriorityPreferenceTime);
	err = GetAudPlayerPriority(iPlayer, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityNormal ||
		priorSets.iPref != EMdaPriorityPreferenceTime)
		return EFail;

	iPlayer->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality);
	err = GetAudPlayerPriority(iPlayer, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityMin ||
		priorSets.iPref != EMdaPriorityPreferenceTimeAndQuality)
		return EFail;

	iPlayer->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality);
	err = GetAudPlayerPriority(iPlayer, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityMin ||
		priorSets.iPref != EMdaPriorityPreferenceTimeAndQuality)
		return EFail;

	iPlayer->SetPriority(EMdaPriorityMax, EMdaPriorityPreferenceTimeAndQuality);
	err = GetAudPlayerPriority(iPlayer, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityMax ||
		priorSets.iPref != EMdaPriorityPreferenceTimeAndQuality)
		return EFail;

	iPlayer->SetPriority(EMdaPriorityMax, EMdaPriorityPreferenceTimeAndQuality);
	err = GetAudPlayerPriority(iPlayer, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityMax ||
		priorSets.iPref != EMdaPriorityPreferenceTimeAndQuality)
		return EFail;

	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	
	}

// CTestStepPlayerPauseSetPosPlay
CTestStepPlayerPauseSetPosPlay* CTestStepPlayerPauseSetPosPlay::NewL()
	{
	CTestStepPlayerPauseSetPosPlay* self = new(ELeave) CTestStepPlayerPauseSetPosPlay();
	return self;
	}

CTestStepPlayerPauseSetPosPlay::CTestStepPlayerPauseSetPosPlay()
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-0054-HP");
	}

CTestStepPlayerPauseSetPosPlay::~CTestStepPlayerPauseSetPosPlay()
	{
	}

void CTestStepPlayerPauseSetPosPlay::Close()
	{
	delete iPlayer;
	}

TVerdict CTestStepPlayerPauseSetPosPlay::DoTestStepL()
	{
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0054-HP"));
	INFO_PRINTF1(_L("This test is playing, then pausing, setting the position and playing again."));
	INFO_PRINTF1(_L("Audio should start playing immediately from the second position after the pause."));

	TPtrC filename;
	if (!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), filename))
		{
		INFO_PRINTF1(_L("Audio test filename not found in INI file"));
		return EInconclusive;
		}

	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(filename, *this);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error creating file player %d"), iError);
		return EInconclusive;
		}

	// Start playing...
	iPlayer->Play();
	User::After(1000000);

	// Pause after 2 secs
	iPlayer->Pause();
	User::After(2000000);

	// Set new position
	TTimeIntervalMicroSeconds newPos(4000000);
	iPlayer->SetPosition(newPos);

	// Play to the end
	iPlayer->Play();
	CActiveScheduler::Start();

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error playing back sound %d"), iError);
		return EFail;
		}

	return EPass;
	}

// from MMdaAudioPlayerCallback
void CTestStepPlayerPauseSetPosPlay::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerPauseSetPosPlay::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}


//
//  CTestStepPlayerPlayWaitSetPos
//

CTestStepPlayerPlayWaitSetPos* CTestStepPlayerPlayWaitSetPos::NewL()
	{
	return new (ELeave) CTestStepPlayerPlayWaitSetPos();
	}

CTestStepPlayerPlayWaitSetPos::CTestStepPlayerPlayWaitSetPos()
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-0989-CP");
	}
	
CTestStepPlayerPlayWaitSetPos::~CTestStepPlayerPlayWaitSetPos()
	{
	delete iPlayer;	
	delete iSetPositionCountDown;
	}


/* 
 *
 * Stages:
 * 1) Setup the utility and timer objects.
 * 2) Start the playback and start the countdown of 6 seconds.
 * 3) Upon countdown completion set the playback position to the 2 second point.
 * 4) Check the file playback completed.
 * 
 */
TVerdict CTestStepPlayerPlayWaitSetPos::DoTestStepL()
	{
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0984-CP"));
	INFO_PRINTF1(_L("This test is playing an audio file then after 6 seconds setting the position to the 2 second point."));
	INFO_PRINTF1(_L("Audio should play to completion after the call to SetPosition()."));
	
	//Stage 1
	TCallBack callbackFunction(TimerCallback, this);
	iSetPositionCountDown = CPeriodic::NewL(CActive::EPriorityHigh);
	
	TPtrC filename;
	if (!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), filename))
		{
		ERR_PRINTF1(_L("Audio test filename not found in INI file"));
		return EInconclusive;
		}

	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(filename, *this);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		{
		ERR_PRINTF2(_L("Error creating file player %d"), iError);
		return EFail;
		}

	//Stage 2	
	INFO_PRINTF1(_L("Starting file playback."));
	iSetPositionCountDown->Start(6000000, 6000000, callbackFunction);
	iPlayer->Play();
	
	CActiveScheduler::Start();
	
	//Stage 4
	if (iError != KErrNone)
		{
		ERR_PRINTF2(_L("Error during file playback %d"), iError);
		return EFail;
		}
	else
		{
		INFO_PRINTF1(_L("File playback completed."));
		return EPass;
		}		
	
	}

void CTestStepPlayerPlayWaitSetPos::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerPlayWaitSetPos::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

TInt CTestStepPlayerPlayWaitSetPos::TimerCallback(TAny* aParentObject)
	{
	CTestStepPlayerPlayWaitSetPos* target = static_cast<CTestStepPlayerPlayWaitSetPos*>(aParentObject);
	target->DoTimerCallback();
	return 0;
	}

void CTestStepPlayerPlayWaitSetPos::DoTimerCallback()
	{
	//Stage 3
	INFO_PRINTF1(_L("Setting the position to 2 seconds in."));
	iSetPositionCountDown->Cancel();
	iPlayer->SetPosition(2000000);
	}

//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerGetImplementationInfo.
 *
 *
 * @return	"CTestStepPlayerGetImplementationInfo*"
 *			The constructed CTestStepPlayerGetImplementationInfo
 *
 * @xxxx
 * 
 */
CTestStepPlayerGetImplementationInfo* CTestStepPlayerGetImplementationInfo::NewL()
	{
	CTestStepPlayerGetImplementationInfo* self = new(ELeave) CTestStepPlayerGetImplementationInfo;
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
CTestStepPlayerGetImplementationInfo::CTestStepPlayerGetImplementationInfo() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0061-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerGetImplementationInfo::~CTestStepPlayerGetImplementationInfo()
	{
	}

/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerGetImplementationInfo::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerGetImplementationInfo::MapcPlayComplete(TInt aError)
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
enum TVerdict CTestStepPlayerGetImplementationInfo::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test tests CMMFMdaAudioPlayerUtility::ControllerImplementationInformationL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerGetImplementationInfo::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerGetImplementationInfo::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	CActiveScheduler::Start();

	if (iError != KErrNone) 
		return EInconclusive;

	const CMMFControllerImplementationInformation* info = NULL;
	
	TRAP(err, info = &iPlayer->ControllerImplementationInformationL());
	if (err != KErrNone)
		return EFail;

	if (info->DisplayName().Compare(KAudioControllerName)!=0)
		iTestStepResult = EFail;
	else
		iTestStepResult = EPass;
	
	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerSendCustomCommandSync.
 *
 *
 * @return	"CTestStepPlayerSendCustomCommandSync*"
 *			The constructed CTestStepPlayerSendCustomCommandSync
 *
 * @xxxx
 * 
 */
CTestStepPlayerSendCustomCommandSync* CTestStepPlayerSendCustomCommandSync::NewL()
	{
	CTestStepPlayerSendCustomCommandSync* self = new(ELeave) CTestStepPlayerSendCustomCommandSync;
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
CTestStepPlayerSendCustomCommandSync::CTestStepPlayerSendCustomCommandSync()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0062-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerSendCustomCommandSync::~CTestStepPlayerSendCustomCommandSync()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepPlayerSendCustomCommandSync::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepPlayerSendCustomCommandSync::MapcPlayComplete(TInt aError)
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
enum TVerdict CTestStepPlayerSendCustomCommandSync::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameTestController"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CMMFMdaAudioPlayerUtility::CustomCommandSync()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerSendCustomCommandSync::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerSendCustomCommandSync::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this);
	
	// special command to open the test controller, since it uses a different Media ID and will not be found with the normal Open
	err = OpenFileWithTestControllerL(iPlayer,iFileName);
	// no callback, since we skipped this
	if (err!=KErrNone)
		return EInconclusive;	

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	// test without a return value
	err = iPlayer->CustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronous,
								KFirstParameter,
								KSecondParameter);
								
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	TBuf8<256> buf;
		
	err = iPlayer->CustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronousWithReturn,
								KFirstParameter,
								KSecondParameter,
								buf);
	
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	if (buf.Compare(KReturnParameter)==0)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
	
	
//
//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerSendCustomCommandAsync.
 *
 *
 * @return	"CTestStepPlayerSendCustomCommandAsync*"
 *			The constructed CTestStepPlayerSendCustomCommandAsync
 *
 * @xxxx
 * 
 */
CTestStepPlayerSendCustomCommandAsync* CTestStepPlayerSendCustomCommandAsync::NewL()
	{
	CTestStepPlayerSendCustomCommandAsync* self = new(ELeave) CTestStepPlayerSendCustomCommandAsync;
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
CTestStepPlayerSendCustomCommandAsync::CTestStepPlayerSendCustomCommandAsync()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0063-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerSendCustomCommandAsync::~CTestStepPlayerSendCustomCommandAsync()
	{
	}

/**
 *
 * Test step Close
 * handles all deletion
 * @xxxx
 * 
 */
void CTestStepPlayerSendCustomCommandAsync::Close()
	{
	delete iAsyncObserver;
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepPlayerSendCustomCommandAsync::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepPlayerSendCustomCommandAsync::MapcPlayComplete(TInt aError)
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
enum TVerdict CTestStepPlayerSendCustomCommandAsync::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameTestController"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}
		
	iAsyncObserver = new (ELeave) CAsyncObserver;

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test tests CMMFMdaAudioPlayerUtility::CustomCommandAsync"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerSendCustomCommandAsync::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerSendCustomCommandAsync::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this);
	
	// special command to open the test controller, since it uses a different Media ID and will not be found with the normal Open
	err = OpenFileWithTestControllerL(iPlayer,iFileName);
	// no callback, since we skipped this
	if (err!=KErrNone)
		return EInconclusive;	

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	// destinationPckg can be temporary as the asynchronous custom command returns before the temporary variable goes out of scope
	TMMFMessageDestinationPckg destinationPckg(uid);
	
	// test without a return value
	iPlayer->CustomCommandAsync(destinationPckg,
								EMMFTestCustomCommandGeneralAsynchronous,
								KFirstParameter,
								KSecondParameter,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
								
	TBuf8<256> buf;
		
	iPlayer->CustomCommandAsync(destinationPckg,
								EMMFTestCustomCommandGeneralAsynchronousWithReturn,
								KFirstParameter,
								KSecondParameter,
								buf,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
		
	if (buf.Compare(KReturnParameter)==0)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerReloadingTest.
 *
 *
 * @return	"CTestStepPlayerReloadingTest*"
 *			The constructed CTestStepPlayerReloadingTest
 *
 * @xxxx
 * 
 */
CTestStepPlayerReloadingTest* CTestStepPlayerReloadingTest::NewL()
	{
	CTestStepPlayerReloadingTest* self = new(ELeave) CTestStepPlayerReloadingTest;
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
CTestStepPlayerReloadingTest::CTestStepPlayerReloadingTest()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0064-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerReloadingTest::~CTestStepPlayerReloadingTest()
	{
	}

void CTestStepPlayerReloadingTest::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepPlayerReloadingTest::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepPlayerReloadingTest::MaloLoadingStarted()
	{
	iLoadingStarted = ETrue;
	CActiveScheduler::Stop();
	}
	
void CTestStepPlayerReloadingTest::MaloLoadingComplete()
	{
	iLoadingComplete = ETrue;
	CActiveScheduler::Stop();
	}




/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerReloadingTest::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameTestController"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test simulates Audio Reloading and tests CMMFMdaAudioPlayerUtility::GetAudioLoadingProgressL"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerReloadingTest::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerReloadingTest::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iPlayer = CMMFMdaAudioPlayerUtility::NewL(*this);

	err = OpenFileWithTestControllerL(iPlayer, iFileName);

	if (err!=KErrNone)
		return EInconclusive;
		
	iPlayer->RegisterForAudioLoadingNotification(*this);

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	// test without a return value
	err = iPlayer->CustomCommandSync(destPckg,
								EMMFTestCustomCommandSimulateReloading,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		return EInconclusive;
		
	CActiveScheduler::Start();
	
	if (!iLoadingStarted)
		return EFail;
		
	TInt percentageComplete = 0;
	TInt oldPercentageComplete =0;
	while (percentageComplete!=100 && percentageComplete>=oldPercentageComplete)
		{
		oldPercentageComplete = percentageComplete;
		iPlayer->GetAudioLoadingProgressL(percentageComplete);
		INFO_PRINTF2(_L("Loading progress: %d"),percentageComplete);
		User::After(100);
		}

	CActiveScheduler::Start();
		
	if (iLoadingComplete && percentageComplete==100)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Static constructor for CTestStepPlayerPlayStopFFPlayNext.
 *
 *
 * @return	"CTestStepPlayerPlayStopFFPlayNext*"
 *			The constructed CTestStepPlayerPlayStopFFPlayNext
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayStopFFPlayNext* CTestStepPlayerPlayStopFFPlayNext::NewL()
	{
	CTestStepPlayerPlayStopFFPlayNext* self = new(ELeave) CTestStepPlayerPlayStopFFPlayNext;
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

CTestStepPlayerPlayStopFFPlayNext::CTestStepPlayerPlayStopFFPlayNext()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0856-HP");

	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */

CTestStepPlayerPlayStopFFPlayNext::~CTestStepPlayerPlayStopFFPlayNext()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */

TVerdict CTestStepPlayerPlayStopFFPlayNext::DoTestStepPreambleL(void)
	{
	TVerdict verdict=EPass;
	 // this installs the scheduler
	if((verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL())	!= EPass)
		{
		return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this is a test of calling Play, Stop fast forward close"));
	INFO_PRINTF1(_L("and then open the next song playing again & verifying the clip is played from the start"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName10"), iFileName)) 
		{
		return EInconclusive;
		}
	// create a play utility
	TRAPD(error, iPlayerUtility = CMdaAudioPlayerUtility::NewL(*this));
	if (error != KErrNone)
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
TVerdict CTestStepPlayerPlayStopFFPlayNext::DoTestStepPostambleL(void)	
	{
	delete iPlayerUtility;
	iPlayerUtility = NULL;

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
void CTestStepPlayerPlayStopFFPlayNext::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	INFO_PRINTF2(_L("MapcInitComplete(): error = %d"), aError);
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerPlayStopFFPlayNext::MapcPlayComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MapcPlayComplete(): error = %d"), aError);
	iError = aError;
	CActiveScheduler::Stop();
	}


TVerdict CTestStepPlayerPlayStopFFPlayNext::DoTestStepL()
	{
	iTestStepResult = EPass;

	INFO_PRINTF1(_L("Opening file using player utility..."));
	
	// open the sound file
	TRAP(iError, iPlayerUtility->OpenFileL(iFileName));
	if (iError != KErrNone)
		{
		return EInconclusive;
		}
	// wait for open to complete
	CActiveScheduler::Start(); 
	if (iError != KErrNone)
		{
		return EFail;
		}

	TTimeIntervalMicroSeconds playStart = TTimeIntervalMicroSeconds(0);
	TTimeIntervalMicroSeconds playEnd = iPlayerUtility->Duration();
	// SetPlayWindow, Play & verify we play from start of window
	INFO_PRINTF1(_L("calling SetPlayWindow..."));
	iPlayerUtility->SetPlayWindow(playStart, playEnd);
	
	INFO_PRINTF1(_L("calling play..."));
	iPlayerUtility->Play();
		
	if (iError != KErrNone)
		{
		  return EFail;
		}
	const TInt KTwoSeconds = 2000000;
	User::After(KTwoSeconds);

	TTimeIntervalMicroSeconds position = TTimeIntervalMicroSeconds(0);
	iPlayerUtility->GetPosition(position);
	INFO_PRINTF1(_L("calling stop..."));
	iPlayerUtility->Stop();
	FastForward(position);
	iPlayerUtility->GetPosition(position);
	FastForward(position);
	iPlayerUtility->GetPosition(position);
	FastForward(position);
	Next();
	if (iError != KErrNone)
		{
		return EFail;
		}
	iPlayerUtility->Play();
	User::After(KTwoSeconds);
	position = 0;	
	iPlayerUtility->GetPosition(position);
	TInt64 halfSecond = playEnd.Int64() / 2;
	if (position.Int64() > halfSecond)
		{
		INFO_PRINTF2(_L("play position is not at start of clip - %d"), position.Int64());
		return EFail;
		}
	iPlayerUtility->Close();
	return iTestStepResult;
	}

void CTestStepPlayerPlayStopFFPlayNext::FastForward(const TTimeIntervalMicroSeconds aPosition)
	{
	TTimeIntervalMicroSeconds step = TTimeIntervalMicroSeconds(200000);
	iPlayerUtility->SetPosition(aPosition.Int64() + step.Int64());
	}

void CTestStepPlayerPlayStopFFPlayNext::Next()
	{
	iPlayerUtility->Close();
	iError = KErrNone;
	TRAP(iError, iPlayerUtility->OpenFileL(iFileName));
	}


//
// class CTestStepMultipleSetPlayerPriority
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepMultipleSetPlayerPriority.
 *
 *
 * @return	"CTestStepMultipleSetPlayerPriority*"
 *			The constructed CTestStepMultipleSetPlayerPriority
 *
 * @xxxx
 * 
 */
CTestStepMultipleSetPlayerPriority* CTestStepMultipleSetPlayerPriority::NewL()
	{
	CTestStepMultipleSetPlayerPriority* self = new(ELeave) CTestStepMultipleSetPlayerPriority;
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
CTestStepMultipleSetPlayerPriority::CTestStepMultipleSetPlayerPriority()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0957-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepMultipleSetPlayerPriority::~CTestStepMultipleSetPlayerPriority()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepMultipleSetPlayerPriority::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepMultipleSetPlayerPriority::MapcPlayComplete(TInt aError)
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
 enum TVerdict CTestStepMultipleSetPlayerPriority::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0957-HP"));
	 INFO_PRINTF1(_L("This test checks for setting the priorities more than once, as part of  DEF067052"));

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameMeta"), iFileName))
		{
		 return EInconclusive;
		}

	 iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	 CActiveScheduler::Start();
	 User::After(1000000);

	 if (iPlayer == NULL ||
		 iError != KErrNone )
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
enum TVerdict CTestStepMultipleSetPlayerPriority::DoTestStepPostambleL(void)
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
TVerdict CTestStepMultipleSetPlayerPriority::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone; 
	
	iPlayer->SetPriority(EMdaPriorityNormal, EMdaPriorityPreferenceQuality);

	TMMFPrioritySettings priorSets;
	err = GetAudPlayerPriority(iPlayer, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityNormal ||
		priorSets.iPref != EMdaPriorityPreferenceQuality)
		{
		return EFail;
		}
	iPlayer->Play();
	User::After(1000000);
	iPlayer->Pause();
	iPlayer->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality);
	err = GetAudPlayerPriority(iPlayer, priorSets);
	if (err != KErrNone ||
		priorSets.iPriority != EMdaPriorityMin ||
		priorSets.iPref != EMdaPriorityPreferenceTimeAndQuality)
		{
		return EFail;
		}
	iPlayer->Play();
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



//
// class CTestStepPlayerPlayPausePlay
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerPlayPausePlay.
 *
 *
 * @return	"CTestStepPlayerPlayPausePlay*"
 *			The constructed CTestStepPlayerPlayPausePlay
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayPausePlay* CTestStepPlayerPlayPausePlay::NewL()
	{
	CTestStepPlayerPlayPausePlay* self = new(ELeave) CTestStepPlayerPlayPausePlay;
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
CTestStepPlayerPlayPausePlay::CTestStepPlayerPlayPausePlay()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0958-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayPausePlay::~CTestStepPlayerPlayPausePlay()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerPlayPausePlay::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerPlayPausePlay::MapcPlayComplete(TInt aError)
	{
	INFO_PRINTF1(_L("finished with this test step"));
	iError = aError;
	
	if (!iError)
		iTestStepResult = EPass;
	
	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepPlayerPlayPausePlay::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0958-HP"));
	 INFO_PRINTF1(_L("This test checks that a paused playback at position 0 will resume and complete, as part of DEF070060"));	 

	 iError = KErrNone; //KErrTimedOut;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameMeta"), iFileName))
		{
		 return EInconclusive;
		}

	 iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	 CActiveScheduler::Start();
	 User::After(1000000);

	 if (iPlayer == NULL ||
		 iError != KErrNone )
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
enum TVerdict CTestStepPlayerPlayPausePlay::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlayerPlayPausePlay::DoTestStepL()
	{
	iTestStepResult = EFail;
	
	iPlayer->Play();
	iPlayer->Pause();
	iPlayer->Play();
	CActiveScheduler::Start();
	// test steps return a result
	return iTestStepResult;
	}
	

//
// class CTestStepPlaybackDurationInformation
//

/**
 *
 * Static constructor for CTestStepPlaybackDurationInformation.
 *
 *
 * @return	"CTestStepPlaybackDurationInformation*"
 *			The constructed CTestStepPlaybackDurationInformation
 *
 * @xxxx
 * 
 */
CTestStepPlaybackDurationInformation* CTestStepPlaybackDurationInformation::NewL()
	{
	CTestStepPlaybackDurationInformation* self = new(ELeave) CTestStepPlaybackDurationInformation;
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
CTestStepPlaybackDurationInformation::CTestStepPlaybackDurationInformation()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0960-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlaybackDurationInformation::~CTestStepPlaybackDurationInformation()
	{
	}
	
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlaybackDurationInformation::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlaybackDurationInformation::MapcPlayComplete(TInt)
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlaybackDurationInformation::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0960-HP"));
	
	INFO_PRINTF1(_L("This test checks that CMdaAudioPlayerUtility::Duration(TTimeIntervalMicroSeconds &) returns TMMFDurationInfo::EMMFDurationInfoValid"));
	
	iError = KErrNone;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameMeta"), iFileName))
		{
		return EInconclusive;
		}

	//
	// Setup player with dummy file
	iPlayer = CMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	
	CActiveScheduler::Start();
	User::After(1000000);

	if (iPlayer == NULL ||
	 	iError != KErrNone )
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
enum TVerdict CTestStepPlaybackDurationInformation::DoTestStepPostambleL(void)
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
TVerdict CTestStepPlaybackDurationInformation::DoTestStepL()
	{
	iTestStepResult = EFail;
	
	// Check that the duration state is TMMFDurationInfo::EMMFDurationInfoValid - known.
	TTimeIntervalMicroSeconds  result2 = 0;
	TMMFDurationInfo result = iPlayer->Duration(result2);
	
	if ( result == EMMFDurationInfoValid )
		{
		iTestStepResult = EPass;		
		}
	else
		{
		iTestStepResult = EFail;
		}
	
	// test steps return a result
	return iTestStepResult;
	}


//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepPlayerCheckImplementationInfo.
 *
 *
 * @return	"CTestStepPlayerCheckImplementationInfo*"
 *			The constructed CTestStepPlayerCheckImplementationInfo
 *
 * @xxxx
 * 
 */
CTestStepPlayerCheckImplementationInfo* CTestStepPlayerCheckImplementationInfo::NewL()
	{
	CTestStepPlayerCheckImplementationInfo* self = new(ELeave) CTestStepPlayerCheckImplementationInfo;
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
CTestStepPlayerCheckImplementationInfo::CTestStepPlayerCheckImplementationInfo() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-1004-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerCheckImplementationInfo::~CTestStepPlayerCheckImplementationInfo()
	{
	}

/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerCheckImplementationInfo::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerCheckImplementationInfo::MapcPlayComplete(TInt aError)
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
TVerdict CTestStepPlayerCheckImplementationInfo::DoTestStepPreambleL(void)
	{
	// this installs the scheduler
	TVerdict verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;	
		}

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test tests CMMFMdaAudioPlayerUtility::ControllerImplementationInformationL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerCheckImplementationInfo::DoTestStepPostambleL(void)
	{
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
TVerdict CTestStepPlayerCheckImplementationInfo::DoTestStepL()
	{
	TInt err = KErrNone;
	iTestStepResult = EFail;
	
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
	CActiveScheduler::Start();

	if (iError != KErrNone) 
		{
		return EInconclusive;	
		}

	const CMMFControllerImplementationInformation* info = NULL;
	
	TRAP(err, info = &iPlayer->ControllerImplementationInformationL());
	if (err != KErrNone)
		{
		return EFail;	
		}
	
	TBuf<40> displayName;
	displayName.Copy(info->DisplayName());
	
	iPlayer->Close(); 

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameTestController"), iFileName))
		{ 
		return EInconclusive;
		}
		
	err = OpenFileWithTestControllerL(iPlayer,iFileName);
	// no callback, since we skipped this
	if (err != KErrNone)
		{
		return EInconclusive;		
		}
			
	TRAP(err, info = &iPlayer->ControllerImplementationInformationL());
	
	if(info->DisplayName().Compare(displayName) != 0)
		{
		iTestStepResult = EPass;	
		}
	else
		{
		iTestStepResult = EFail;	
		}
		
	delete iPlayer;
	iPlayer = NULL;
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

