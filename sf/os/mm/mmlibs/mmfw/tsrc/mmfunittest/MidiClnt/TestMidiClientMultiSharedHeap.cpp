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
//


#include "TestMidiClientSharedHeap.h"


const TInt KMaxPlayers = 24; // easily enough to overflow the chunk if there were one

const TInt KNoSharedPlayer = 2;
// which of the created players should be given its own heap on mixed style tests
// This must always be true: KNoSharedPlayer < KMaxPlayers


const TInt KRepeatAmount = 16;
// how often CTestStepPlayerSharedHeapRepeatMultiFilePlayer should create and delete players


CTestMidiClntMultiSharedHeap::CTestMidiClntMultiSharedHeap(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TBool aMixHeapStyle )
	: CTestMmfMidiClntStep(aTestName, ETestValid)
	, iMixHeapStyle( aMixHeapStyle )
	{
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMidiClntMultiSharedHeap* CTestMidiClntMultiSharedHeap::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TBool aMixHeapStyle )
	{
	CTestMidiClntMultiSharedHeap* self = new(ELeave) CTestMidiClntMultiSharedHeap(aTestName, aSectName, aKeyName, aMixHeapStyle );
	return self;
	}

TVerdict CTestMidiClntMultiSharedHeap::DoTestStepL()
	{
	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;
	
	INFO_PRINTF2(_L("Check we can create lots of clients - creating %d clients"), KMaxPlayers );
	
	// create lots of players
	RPointerArray<CMidiClientUtility> players;
	
	TVerdict ret = EPass;
	
	__MM_HEAP_MARK;
	
	for( TInt i = 0; i < KMaxPlayers; ++i )
		{
		CMidiClientUtility* player = NULL;	
		// create player with shared heap
		
		TBool useSharedHeap = ETrue;
		if( i == KNoSharedPlayer && iMixHeapStyle )
			{
			INFO_PRINTF2(_L("MIDI client on iteration %d has own heap."), i );
			useSharedHeap = EFalse;
			}
		
		TRAPD( err, player = CMidiClientUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality, useSharedHeap ) );
		if( err )
			{
			if( err == KErrNoMemory )
				{
				INFO_PRINTF2(_L("Could not create player #%d due to lack of memory."), i );
				ret = EFail;
				}
			else
				{
				INFO_PRINTF3(_L("Error %d. Could not create player #%d."), err, i );
				ret = EFail;
				}
			
			delete player;
			player = NULL;
			break;
			}
		else
			{
			if( players.Append( player ) )
				{
				// could not add to array
				delete player;
				player = NULL;
				ret = EInconclusive;
				break;
				}
			}
		}

	if( (ret == EPass) && (players.Count() > 0) ) // no errors so far
		{
		// do fake play
		TMMFMessageDestinationPckg dummyPckg;
		TInt dummyFunc = 0; //EDevMidiOff;
		TBuf8<8> dummyBuff;
		(players[0])->CustomCommandSyncL(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff);
		(players[0])->OpenFile(filename);
		
		// Wait for initialisation callback
		INFO_PRINTF1(_L("CMidiClientUtility: Opening file"));
		CActiveScheduler::Start();	

		(players[0])->Play();
		// wait for playback callback
		CActiveScheduler::Start();
		
		if( iError )
			{
			// something went wrong
			INFO_PRINTF2(_L("Error %d during playback."), iError );
			ret = EFail;	
			}
		}


	// cleanup
	for( TInt i = 0; i < players.Count(); ++i )
		{
		delete players[i];
		players[i] = NULL;
		}
	players.Close();

	__MM_HEAP_MARKEND;

	
	return	ret;
	}


// =======================================================================


CTestMidiClntRepeatMultiSharedHeap::CTestMidiClntRepeatMultiSharedHeap(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TBool aMixHeapStyle )
	: CTestMmfMidiClntStep(aTestName, ETestValid)
	, iMixHeapStyle( aMixHeapStyle )
	{
	iSectName = aSectName;
	iKeyName = aKeyName;
	iHeapSize = 512 * 1024; // 512k rather than the usual 64k to avoid KErrNoMemory
	}

CTestMidiClntRepeatMultiSharedHeap* CTestMidiClntRepeatMultiSharedHeap::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TBool aMixHeapStyle )
	{
	CTestMidiClntRepeatMultiSharedHeap* self = new(ELeave) CTestMidiClntRepeatMultiSharedHeap(aTestName, aSectName, aKeyName, aMixHeapStyle );
	return self;
	}

TVerdict CTestMidiClntRepeatMultiSharedHeap::DoTestStepL()
	{
	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;
	
	INFO_PRINTF3(_L("Create and delete %d players %d times"), KMaxPlayers, KRepeatAmount );
	
	TVerdict ret = EPass;
	
	__MM_HEAP_MARK;
	
	for( TInt j = 0; j < KRepeatAmount; ++j )
		{
		INFO_PRINTF2(_L("** Starting outter iteration %d."), j );
		
		// create lots of players
		RPointerArray<CMidiClientUtility> players;
		
		for( TInt i = 0; i < KMaxPlayers; ++i )
			{
			CMidiClientUtility* player = NULL;	
			// create player with shared heap
			
			TBool useSharedHeap = ETrue;
			if( (i == KNoSharedPlayer) && iMixHeapStyle )
				{
				INFO_PRINTF2(_L("MIDI client on iteration %d has own heap."), i );
				useSharedHeap = EFalse;
				}
			
			TRAPD( err, player = CMidiClientUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality, useSharedHeap ) );
			if( err )
				{
				if( err == KErrNoMemory )
					{
					INFO_PRINTF2(_L("Could not create player #%d due to lack of memory."), i );
					ret = EFail;
					}
				else
					{
					INFO_PRINTF3(_L("Error %d. Could not create player #%d."), err, i );
					ret = EFail;
					}
				
				delete player;
				player = NULL;
				break;
				}
			else
				{
				if( players.Append( player ) )
					{
					// could not add to array
					delete player;
					player = NULL;
					ret = EInconclusive;
					break;
					}
				}
			}

		if( (ret == EPass) && (players.Count() > 0) ) // no errors so far
			{
			// do fake play
			TMMFMessageDestinationPckg dummyPckg;
			TInt dummyFunc = 0; //EDevMidiOff;
			TBuf8<8> dummyBuff;
			(players[0])->CustomCommandSyncL(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff);
			(players[0])->OpenFile(filename);
			
			// Wait for initialisation callback
			INFO_PRINTF1(_L("CMidiClientUtility: Opening file"));
			CActiveScheduler::Start();	

			(players[0])->Play();
			// wait for playback callback
			CActiveScheduler::Start();
			
			if( iError )
				{
				// something went wrong
				INFO_PRINTF2(_L("Error %d during playback."), iError );
				ret = EFail;	
				}
			}

		// cleanup
		for( TInt i = 0; i < players.Count(); ++i )
			{
			delete players[i];
			players[i] = NULL;
			}
		players.Close();
			
		if( ret != EPass )
			{
			INFO_PRINTF2(_L("Outter iteration %d failed. Stopping."), j);
			break;
			}
			
		}// end outter loop
	
	__MM_HEAP_MARKEND;
	
	return	ret;
	}
