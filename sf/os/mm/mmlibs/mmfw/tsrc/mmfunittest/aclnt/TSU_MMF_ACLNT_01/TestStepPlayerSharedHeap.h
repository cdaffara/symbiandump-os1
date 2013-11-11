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
// These test steps create audio player utilities whose
// controllers share a single heap.
// 
//


#ifndef TESTSTEPLAYERSHAREDHEAP_H
#define TESTSTEPLAYERSHAREDHEAP_H

#include<mdaaudiosampleplayer.h>
/**
 *
 * Creates multiple file players whose controllers share a single heap.
 * Plays one to make sure they work OK.
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerSharedHeapMultiFilePlayer :	public	CTestStepUnitMMFAudClient,
													public  MMdaAudioPlayerCallback	
	{
protected:
	enum TTestType
		{
		ENone,
		EPlay,
		EPanic
		};
	enum TMMFAudioPlayerState
		{
		EStopped,
		EOpening,
		EPaused,
		EPlaying		
		};
	
public:
	static CTestStepPlayerSharedHeapMultiFilePlayer* NewL( const TDesC& aName, TBool aMixHeapStyle = EFalse );
	~CTestStepPlayerSharedHeapMultiFilePlayer();
	enum TVerdict DoTestStepPreambleL(void);
	enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();	
	
	TInt GetAudPlayerState(CMdaAudioPlayerUtility*, TInt& );
	TInt GetAudPlayerSourceHandle(CMdaAudioPlayerUtility*, TMMFMessageDestination* );
	
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

protected:
	CTestStepPlayerSharedHeapMultiFilePlayer( const TDesC& aName, TBool aMixHeapStyle );
	enum TVerdict DoTestL( TTestType aTestType );

private:
	CMdaAudioPlayerUtility* CreateAndOpenPlayerL( TInt aIteration );

protected:
	TInt iError;
	TBool iMixHeapStyle;
	
private:
	RPointerArray<CMdaAudioPlayerUtility> iPlayers; // array of players
	TMMFAudioPlayerState iState;
	};


/**
 *
 * Creates and deletes multiple players several times in order to check for
 * chunk and/or memory leaks.
 * 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerSharedHeapRepeatMultiFilePlayer :	public CTestStepPlayerSharedHeapMultiFilePlayer
	{
public:
	static CTestStepPlayerSharedHeapRepeatMultiFilePlayer* NewL( const TDesC& aName, TBool aMixHeapStyle = EFalse );
	~CTestStepPlayerSharedHeapRepeatMultiFilePlayer();

	virtual TVerdict DoTestStepL();

    
protected:
	CTestStepPlayerSharedHeapRepeatMultiFilePlayer( const TDesC& aName, TBool aMixHeapStyle );

	};


/**
 *
 * Creates multiple players that share a heap.
 * Creates a custom controller that shares the heap too and panicks it.
 * Checks that subsequent clean-up is OK.
 * 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerSharedHeapPanicMultiFilePlayer :	public CTestStepPlayerSharedHeapMultiFilePlayer
	{
public:
	static CTestStepPlayerSharedHeapPanicMultiFilePlayer* NewL( const TDesC& aName, TBool aMixHeapStyle = EFalse );
	~CTestStepPlayerSharedHeapPanicMultiFilePlayer();

	virtual TVerdict DoTestStepL();

    
protected:
	CTestStepPlayerSharedHeapPanicMultiFilePlayer( const TDesC& aName, TBool aMixHeapStyle );

	};


#endif // TESTSTEPLAYERSHAREDHEAP_H
