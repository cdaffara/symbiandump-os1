// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// These test steps create audio recorder utilities whose
// controllers share a single heap.
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

#include <mdaaudiosampleeditor.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepRecorderSharedHeap.h"

#include "mmfclientaudiorecorder.h"

// For custom controller
#include "../../ctlfrm/TS_CMMFTestController/CmmfTestControllerUIDs.hrh"
#include "../../ctlfrm/TS_Codes.h"

// --------------------------------------------

const TInt KMaxRecorders = 24;
// would definitely exceed the chunk limit if there recorders were
// using separate heaps (which they shouldn't in this test)


const TInt KNoSharedRecorder = 2;
// which of the created players should be given its own heap on mixed style tests
// This must always be true: KNoSharedRecorder < KMaxRecorders

const TUid KTstControllerUid = {KMmfTestControllerUid}; //(0x101F88D8)
// custom controller ID

// --------------------------------------------


/**
 *
 * Static constructor for CTestStepRecorderSharedHeapMultiFilePlayer.
 *
 *
 * @return	"CTestStepRecorderSharedHeapMultiFilePlayer*"
 *			The constructed CTestStepRecorderSharedHeapMultiFilePlayer
 *
 * @xxxx
 * 
 */
CTestStepRecorderSharedHeapMultiFilePlayer* CTestStepRecorderSharedHeapMultiFilePlayer::NewL(const TDesC& aName, TBool aMixHeapStyle )
	{
	CTestStepRecorderSharedHeapMultiFilePlayer* self = new(ELeave) CTestStepRecorderSharedHeapMultiFilePlayer( aName, aMixHeapStyle );
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
CTestStepRecorderSharedHeapMultiFilePlayer::CTestStepRecorderSharedHeapMultiFilePlayer(const TDesC& aName, TBool aMixHeapStyle )
	: iMixHeapStyle( aMixHeapStyle )
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aName;
	iHeapSize =128* 1024; 
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderSharedHeapMultiFilePlayer::~CTestStepRecorderSharedHeapMultiFilePlayer()
	{
	}
	
/**
 * Implementation of the MoscoStateChangeEvent  interface functions
 **/
 
 void CTestStepRecorderSharedHeapMultiFilePlayer::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
 	{ 
 
    iError=aErrorCode;
	if (iError )
    	{
    	ERR_PRINTF2(_L("AudioRecPlay failed, unexpected error %d."), aErrorCode);
    	}
    CActiveScheduler::Stop();
    }



/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderSharedHeapMultiFilePlayer::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1( iTestStepName );
	
	iError = KErrNone; //KErrTimedOut;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord"), iFileName))
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
enum TVerdict CTestStepRecorderSharedHeapMultiFilePlayer::DoTestStepPostambleL(void)
	{
	// close array
	iRecorders.Close();
	
	INFO_PRINTF1(_L("finished with this test step"));
	
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * The commands to be included in the TRAP command
 *
 * @xxxx
 * 
 */
CMdaAudioRecorderUtility* CTestStepRecorderSharedHeapMultiFilePlayer::CreateAndOpenRecorderL( TInt aIteration )
	{
	CMdaAudioRecorderUtility* recorder = NULL;
	
	TRAPD( err, recorder = CMdaAudioRecorderUtility::NewL(*this) );
	User::LeaveIfError( err );
	CleanupStack::PushL(recorder);
	
	if( ! (iMixHeapStyle && (aIteration == KNoSharedRecorder)) )
		{
		INFO_PRINTF2(_L("Recorder on iteration %2d is using a shared heap."), aIteration );
		recorder->UseSharedHeap(); // ensure that underlying controller uses a shared heap
		}
	else
		{
		INFO_PRINTF2(_L("	Recorder on iteration %2d has its own heap."), aIteration );
		}
	
	recorder->OpenFileL( iFileName ); // this creates a new controller
	
	CActiveScheduler::Start(); // wait for open to complete

	User::LeaveIfError( iError ); // if had error return from callback
	
	CleanupStack::Pop(recorder);
	return recorder;
	}
	
	
/**
 *
 * Get's the Audio Recorder Source Handle
 *
 * @xxxx
 * 
 */
TInt CTestStepRecorderSharedHeapMultiFilePlayer::GetAudRecorderSourceHandle(CMdaAudioRecorderUtility* aRecorder,
														 TMMFMessageDestination* /*aSrcHandle*/)
	{
	if (aRecorder == NULL)
		return KErrNotReady;
	return KErrNone;
	}



/**
 *
 * Creates KMaxRecorders players, performs the desired test on one of them
 * and then closes them all and cleans up.
 * If problems are encountered the returned result will reflect this.
 *
 * @return	"TVerdict"
 *			The result of the tests
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderSharedHeapMultiFilePlayer::DoTestL( TTestType aTestType )
	{
	iTestStepResult = EPass;
    TInt err = KErrNone;
	
	__MM_HEAP_MARK;
	
	TInt i=0;
	for( ; i < KMaxRecorders; ++i )	
		{
		iError = KErrNone;
		
		CMdaAudioRecorderUtility* recorder = NULL;
		
		TRAPD(err, recorder = CreateAndOpenRecorderL(i) );
		
		if (err == KErrNone && iError == KErrNone)
			{
			TInt err2 = KErrNone;
			TMMFMessageDestination* srcHandle = NULL;
			err2 = GetAudRecorderSourceHandle(recorder, srcHandle);
			
			// see if valid result and break if wrong - might be premature result
			if ( err2 != KErrNone ||
				 &srcHandle == NULL ||recorder->State() != CMdaAudioClipUtility::EOpen ||
				 iError != KErrNone)
				{
				// dodgy recorder
				
				INFO_PRINTF2(_L("	Dodgy recorder on iteration %2d"), i);
				iTestStepResult = EFail;
				
				delete recorder;
				recorder = NULL;
				break;
				}
			else
				{
				// no errors				
				// append player to array
				if( (err = iRecorders.Append( recorder )) != KErrNone )
					{
					INFO_PRINTF2(_L("Fail to append recorder on iteration %2d"), i);
					// this doesn't indicate a failure in what we're testing here
					iTestStepResult = EInconclusive;
				
					delete recorder;
					recorder = NULL;
					break;
					}
				}
			}
		else
			{
			INFO_PRINTF4(_L("Fail to open recorder on iteration %2d - %d %d"), i, err, iError);
			break;
			}	
		}

	// check outcome of making players
	if ( err || iError )
		{
		if ((err && err != KErrNoMemory) || (err == KErrNone && iError != KErrNoMemory))
			{
			// a genuine error
			
			if( err )
				{
				INFO_PRINTF3(_L("Error %d occured while trying to create recorder #%d"), err, i );
				}
			else
				{
				INFO_PRINTF3(_L("Error %d occured while opening recorder #%d"), iError, i );
				}
			
			iTestStepResult = EFail;
			}
		else
			{
			// we ran out of memory
			INFO_PRINTF2(_L("Teststep ran out of memory while creating recorder #%d"), i );
			iTestStepResult = EFail;
			}
		}
	// else no errors


	// perform test on 1st player (unless there were errors above)
	if( (iTestStepResult == EPass) && (iRecorders.Count() > 0 ) )
		{
		switch( aTestType )
			{
			case ERecord:
				{		
				(iRecorders[0])->RecordL();
				CActiveScheduler::Start();
				
				if( iError )
					{
					// playback died
					INFO_PRINTF2(_L("Error %d occured while playing"), iError );
					iTestStepResult = EFail;
					}
					
				break;
				}
		
				
			case EPanic:
				{
				// panic player
				
				// load a custom controller (using the share heap)
				// that can be panicked and see if we recover OK
				TMMFPrioritySettings settings;
				TInt error = KErrNone;
				RMMFController controller;

				settings.iPriority = 1;
				settings.iPref = EMdaPriorityPreferenceTime;
				settings.iState = EMMFStateIdle;

				error = controller.Open(KTstControllerUid, settings, ETrue); // use shared heap
				INFO_PRINTF2(_L("Custom controller Open: %d"), error);

				if( ! error )
					{
					// panic the controller
					// this command will complete immediately and
					// then panic
					TUid uid;
					uid = TUid::Uid(0x101f72B4);
					TMMFMessageDestinationPckg destPckg(uid);
					//TRequestStatus status = KRequestPending;
					controller.CustomCommandSync(destPckg,
												EMMFTestCustomCommandPanic,
												KNullDesC8,
												KNullDesC8);
					
					// need to allow some time to make
					// sure the thread has actually been panicked when we close it
					User::After( TTimeIntervalMicroSeconds32(5000000) ); // 5 seconds
					}
					
				// controller should have panicked by now
				controller.Close();
				
				break;
				}


			// default: // do nothing
			}
		}
		
	// cleanup
	for( i = 0; i < iRecorders.Count(); ++i )
		{
		delete iRecorders[i];
		
		iRecorders[i] = NULL;
		}
	iRecorders.Reset();

	__MM_HEAP_MARKEND;

	// test steps return a result
	return iTestStepResult;
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
TVerdict CTestStepRecorderSharedHeapMultiFilePlayer::DoTestStepL()
	{
	INFO_PRINTF1(_L("This is a test that several recorder utilities using a shared heap can be created."));

	return DoTestL( ERecord );
	}


