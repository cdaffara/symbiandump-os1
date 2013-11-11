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
// These test steps create audio player utilities whose
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

#include <mdaaudiosampleplayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepPlayerSharedHeap.h"

#include "mmfclientaudioplayer.h"

// For custom controller
#include "../../ctlfrm/TS_CMMFTestController/CmmfTestControllerUIDs.hrh"
#include "../../ctlfrm/TS_Codes.h"

// --------------------------------------------

const TInt KMaxPlayers = 24;
// would definitely exceed the chunk limit if there players were
// using separate heaps (which they shouldn't in this test)


const TInt KNoSharedPlayer = 2;
// which of the created players should be given its own heap on mixed style tests
// This must always be true: KNoSharedPlayer < KMaxPlayers


const TInt KRepeatAmount = 16;
// how often CTestStepPlayerSharedHeapRepeatMultiFilePlayer should create and delete players


const TUid KTstControllerUid = {KMmfTestControllerUid}; //(0x101F88D8)
// custom controller ID

// --------------------------------------------


/**
 *
 * Static constructor for CTestStepPlayerSharedHeapMultiFilePlayer.
 *
 *
 * @return	"CTestStepPlayerSharedHeapMultiFilePlayer*"
 *			The constructed CTestStepPlayerSharedHeapMultiFilePlayer
 *
 * @xxxx
 * 
 */
CTestStepPlayerSharedHeapMultiFilePlayer* CTestStepPlayerSharedHeapMultiFilePlayer::NewL(const TDesC& aName, TBool aMixHeapStyle )
	{
	CTestStepPlayerSharedHeapMultiFilePlayer* self = new(ELeave) CTestStepPlayerSharedHeapMultiFilePlayer( aName, aMixHeapStyle );
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
CTestStepPlayerSharedHeapMultiFilePlayer::CTestStepPlayerSharedHeapMultiFilePlayer(const TDesC& aName, TBool aMixHeapStyle )
	: iMixHeapStyle( aMixHeapStyle )
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aName;

	iHeapSize = 128 * 1024; 
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerSharedHeapMultiFilePlayer::~CTestStepPlayerSharedHeapMultiFilePlayer()
	{
	}
	
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerSharedHeapMultiFilePlayer::MapcInitComplete(TInt aError, 
															const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;

	if( aError )
		{
		INFO_PRINTF2(_L("	Callback error (%d) while opening player."), aError);
		}
	/*else
		{
		INFO_PRINTF1(_L("	Player opened successfully."));
		}*/
	CActiveScheduler::Stop();
	}

void CTestStepPlayerSharedHeapMultiFilePlayer::MapcPlayComplete(TInt aError)
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
enum TVerdict CTestStepPlayerSharedHeapMultiFilePlayer::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1( iTestStepName );
	
	iError = KErrNone; //KErrTimedOut;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
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
enum TVerdict CTestStepPlayerSharedHeapMultiFilePlayer::DoTestStepPostambleL(void)
	{
	// close array
	iPlayers.Close();
	
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
CMdaAudioPlayerUtility* CTestStepPlayerSharedHeapMultiFilePlayer::CreateAndOpenPlayerL( TInt aIteration )
	{
	CMdaAudioPlayerUtility* player = NULL;
	
	TRAPD( err, player = CMdaAudioPlayerUtility::NewL(*this) );
	User::LeaveIfError( err );
	CleanupStack::PushL(player);
	
	if( ! (iMixHeapStyle && (aIteration == KNoSharedPlayer)) )
		{
		INFO_PRINTF2(_L("	Player on iteration %2d is using a shared heap."), aIteration );
		player->UseSharedHeap(); // ensure that underlying controller uses a shared heap
		}
	else
		{
		INFO_PRINTF2(_L("	Player on iteration %2d has its own heap."), aIteration );
		}
	
	player->OpenFileL( iFileName ); // this creates a new controller
	
	CActiveScheduler::Start(); // wait for open to complete
	
	User::LeaveIfError( iError ); // if had error return from callback
	
	CleanupStack::Pop(player);
	return player;
	}
	
/**
 *
 * Get's the Audio Player State
 *
 * @xxxx
 * 
 */

TInt CTestStepPlayerSharedHeapMultiFilePlayer::GetAudPlayerState(CMdaAudioPlayerUtility* aPlayer, TInt& aState)
	{
	if (aPlayer == NULL)
		return KErrNotReady;
	aState = this->iState;
	return KErrNone;
	}
	
/**
 *
 * Get's the Audio Player Source Handle
 *
 * @xxxx
 * 
 */
TInt CTestStepPlayerSharedHeapMultiFilePlayer::GetAudPlayerSourceHandle(CMdaAudioPlayerUtility* aPlayer,
														 TMMFMessageDestination* /*aSrcHandle*/)
	{
	if (aPlayer == NULL)
		return KErrNotReady;
	return KErrNone;
	}



/**
 *
 * Creates KMaxPlayers players, performs the desired test on one of them
 * and then closes them all and cleans up.
 * If problems are encountered the returned result will reflect this.
 *
 * @return	"TVerdict"
 *			The result of the tests
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerSharedHeapMultiFilePlayer::DoTestL( TTestType aTestType )
	{
	iTestStepResult = EPass;
    TInt err = KErrNone;
	
	__MM_HEAP_MARK;
	
	TInt i=0;
	for( ; i < KMaxPlayers; ++i )	
		{
		iError = KErrNone;
		
		CMdaAudioPlayerUtility* player = NULL;
		
		TRAPD(err, player = CreateAndOpenPlayerL(i) );
		
		if (err == KErrNone && iError == KErrNone)
			{
			// see if valid result and break if wrong - might be premature result
			TInt err1 = KErrNone;
			TInt err2 = KErrNone;
			TInt state = 0;
			TMMFMessageDestination* srcHandle = NULL;
			err1 = GetAudPlayerState(player, state);
			err2 = GetAudPlayerSourceHandle(player, srcHandle);
			if ( err1 != KErrNone ||
				 err2 != KErrNone ||
				 &srcHandle == NULL ||
				 state != CTestStepPlayerSharedHeapMultiFilePlayer::EStopped ||
				 player->Duration() == TTimeIntervalMicroSeconds(0) || // we know the duration of the file is more than 0
				 iError != KErrNone)
				{
				// dodgy player
				
				INFO_PRINTF2(_L("	Dodgy player on iteration %2d"), i);
				iTestStepResult = EFail;
				
				delete player;
				player = NULL;
				break;
				}
			else
				{
				// no errors				
				// append player to array
				if( (err = iPlayers.Append( player )) != KErrNone )
					{
					INFO_PRINTF2(_L("Fail to append player on iteration %2d"), i);
					// this doesn't indicate a failure in what we're testing here
					iTestStepResult = EInconclusive;
				
					delete player;
					player = NULL;
					break;
					}
				}
			}
		else
			{
			INFO_PRINTF4(_L("Fail to open player on iteration %2d - %d %d"), i, err, iError);
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
				INFO_PRINTF3(_L("Error %d occured while trying to create player #%d"), err, i );
				}
			else
				{
				INFO_PRINTF3(_L("Error %d occured while opening player #%d"), iError, i );
				}
			
			iTestStepResult = EFail;
			}
		else
			{
			// we ran out of memory
			INFO_PRINTF2(_L("Teststep ran out of memory while creating player #%d"), i );
			iTestStepResult = EFail;
			}
		}
	// else no errors


	// perform test on 1st player (unless there were errors above)
	if( (iTestStepResult == EPass) && (iPlayers.Count() > 0 ) )
		{
		switch( aTestType )
			{
			case EPlay:
				{		
				(iPlayers[0])->Play();
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
	for( i = 0; i < iPlayers.Count(); ++i )
		{
		delete iPlayers[i];
		
		iPlayers[i] = NULL;
		}
	iPlayers.Reset();

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
TVerdict CTestStepPlayerSharedHeapMultiFilePlayer::DoTestStepL()
	{
	INFO_PRINTF1(_L("This is a test that several player utilities using a shared heap can be created."));

	return DoTestL( EPlay );
	}


//===========================================================================================

/**
 *
 * Static constructor for CTestStepPlayerSharedHeapRepeatMultiFilePlayer.
 *
 *
 * @return	"CTestStepPlayerSharedHeapRepeatMultiFilePlayer*"
 *			The constructed CTestStepPlayerSharedHeapRepeatMultiFilePlayer
 *
 * @xxxx
 * 
 */
CTestStepPlayerSharedHeapRepeatMultiFilePlayer* CTestStepPlayerSharedHeapRepeatMultiFilePlayer::NewL(const TDesC& aName, TBool aMixHeapStyle )
	{
	CTestStepPlayerSharedHeapRepeatMultiFilePlayer* self = new(ELeave) CTestStepPlayerSharedHeapRepeatMultiFilePlayer( aName, aMixHeapStyle );
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
CTestStepPlayerSharedHeapRepeatMultiFilePlayer::CTestStepPlayerSharedHeapRepeatMultiFilePlayer(const TDesC& aName, TBool aMixHeapStyle )
	: CTestStepPlayerSharedHeapMultiFilePlayer( aName, aMixHeapStyle )
	{
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerSharedHeapRepeatMultiFilePlayer::~CTestStepPlayerSharedHeapRepeatMultiFilePlayer()
	{
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
TVerdict CTestStepPlayerSharedHeapRepeatMultiFilePlayer::DoTestStepL()
	{
	INFO_PRINTF1(_L("This is a test that several player utilities using a shared heap can be created."));

	TVerdict result = EPass;

	__MM_HEAP_MARK;

	for(TInt i=0; i < KRepeatAmount; ++i)
		{
		INFO_PRINTF2(_L("Create and delete iteration %d."), i );
		result = DoTestL( EPlay );
		
		if( result != EPass )
			{
			INFO_PRINTF2(_L("Iteration %d failed. Stopping."), i );
			break;
			}
		}

	__MM_HEAP_MARKEND;

	return result;
	}

// =============================================================

/**
 *
 * Static constructor for CTestStepPlayerSharedHeapPanicMultiFilePlayer.
 *
 *
 * @return	"CTestStepPlayerSharedHeapPanicMultiFilePlayer*"
 *			The constructed CTestStepPlayerSharedHeapPanicMultiFilePlayer
 *
 * @xxxx
 * 
 */
CTestStepPlayerSharedHeapPanicMultiFilePlayer* CTestStepPlayerSharedHeapPanicMultiFilePlayer::NewL(const TDesC& aName, TBool aMixHeapStyle )
	{
	CTestStepPlayerSharedHeapPanicMultiFilePlayer* self = new(ELeave) CTestStepPlayerSharedHeapPanicMultiFilePlayer( aName, aMixHeapStyle );
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
CTestStepPlayerSharedHeapPanicMultiFilePlayer::CTestStepPlayerSharedHeapPanicMultiFilePlayer(const TDesC& aName, TBool aMixHeapStyle )
	: CTestStepPlayerSharedHeapMultiFilePlayer( aName, aMixHeapStyle )
	{
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerSharedHeapPanicMultiFilePlayer::~CTestStepPlayerSharedHeapPanicMultiFilePlayer()
	{
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
TVerdict CTestStepPlayerSharedHeapPanicMultiFilePlayer::DoTestStepL()
	{
	INFO_PRINTF1(_L("This is a test that shared heap layers can recover from a panic."));

	return DoTestL( EPanic );
	}

