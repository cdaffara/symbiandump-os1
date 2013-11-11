
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic  tests.
// 
//

/**
 @file TestPlayerUtils.h
*/

#ifndef TESTPLAYERUTILS_H
#define TESTPLAYERUTILS_H


#include "TSI_MMFACLNT.h"

// parameter structure for repeat tests
class TRepeatParameters
	{	
public:
	const TText* iFilename;
	TInt	iRepeatCount;
	TInt64 iSilence;
	};
	
const TInt KNegativeRepeat = -4;
/**
 * MDelayedMethodCall & CDelayedMethodCall provide a mechanism whereby a method can be called 
 * from the ActiveScheduler after a certain length of time. 
 *
 */
class MDelayedMethodCall
{
public:
	virtual void DelayedMethodCallL() =0;
};

class CDelayedMethodCall : public CTimer
{
public:
	static CDelayedMethodCall* NewL(MDelayedMethodCall* aThat);

	virtual void RunL();
	virtual TInt RunError(TInt aError);
protected:
	CDelayedMethodCall(MDelayedMethodCall* aThat);

private:
	MDelayedMethodCall* const iThat;
};




/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntFile
 *
 */         
class CTestMmfAclntFile :  public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	static CTestMmfAclntFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError);
	static CTestMmfAclntFile* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError);
	virtual TVerdict DoTestStepL();
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	CTestMmfAclntFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError);
	
private:
	TInt iError;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	const TBool iPlay;					// Does this test require playing the audio
	const TInt	iExpectedError;			// Is this a negative test
	};

/**
 * Load and play an audio sequence file.
 *
 * @class CTestMmfAclntSEQNFile
 *
 */         
class CTestMmfAclntSEQNFile :  public CTestMmfAclntStep, public MMdaAudioPlayerCallback, public MDelayedMethodCall
	{
public:
	static CTestMmfAclntSEQNFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TBool aIsFile);
	static CTestMmfAclntSEQNFile* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TBool aIsFile);
	virtual TVerdict DoTestStepL();
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

	//Used to stop playing after 1 second
	void DelayedMethodCallL();

private:
	CTestMmfAclntSEQNFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TBool aIsFile);
	
private:
	TInt iError;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	const TBool iPlay;					// Does this test require playing the audio
	const TBool iIsFile;			// Is this a negative test
	};




/**
 * Load and initialise an audio descriptor.
 *
 * @class CTestMmfAclntDesc
 *
 */
class CTestMmfAclntDesc : public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	static CTestMmfAclntDesc* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError = KErrNone);
	static CTestMmfAclntDesc* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

protected:
	CTestMmfAclntDesc(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError) ;

private:
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio

protected:
	const TBool iPlay;						// Does this test require playing the audio
	CMdaAudioPlayerUtility * iPlayer;
	HBufC8* iAudio;						// Buffer for holding audio descriptor data
	TInt iError;						// Stores error values for testing
	const TInt	iExpectedError;			// Is this a negative test
	};

/**
 * Load and initialise an audio descriptor (read-only).
 *
 * @class CTestMmfAclntDescReadOnly
 *
 */
class CTestMmfAclntDescReadOnly : public CTestMmfAclntDesc
	{
public:
	static CTestMmfAclntDescReadOnly* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError = KErrNone);
	static CTestMmfAclntDescReadOnly* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError);
	virtual TVerdict DoTestStepL();

private:
	CTestMmfAclntDescReadOnly(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError) ;

	};

/**
 * Load and initialise an audio descriptor (using Open only).
 *
 * @class CTestMmfAclntDescOpen
 *
 */
class CTestMmfAclntDescOpen : public CTestMmfAclntDesc
	{
public:
	static CTestMmfAclntDescOpen* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError = KErrNone);
	static CTestMmfAclntDescOpen* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError);
	virtual TVerdict DoTestStepL();

private:
	CTestMmfAclntDescOpen(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError) ;

	};

/**
 * Open audio form a URL and play.
 *
 * @class CTestMmfAclntUrl
 *
 */
class CTestMmfAclntUrl : public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	static CTestMmfAclntUrl* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	static CTestMmfAclntUrl* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	virtual TVerdict DoTestStepL();
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	CTestMmfAclntUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);

private:
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	TInt iError;						// Stores error values for testing
	const TBool iPlay;					// Does this test require playing the audio
	HBufC8* iAudio;						// Buffer for holding audio descriptor data
	};


/**
 * Base functionality for most audio player tests
 * 
 * @class CTestMmfAclntAudioPlayerUtil
 *
 */
class CTestMmfAclntAudioPlayerUtil : public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	CTestMmfAclntAudioPlayerUtil(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestStepL();
	// Pure function to access test specific methods
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer)=0;
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

protected:
	TInt iError;
	TInt iState;
	TBool iStop;
	TTimeIntervalMicroSeconds iDuration;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	} ;

	//
	// RAsyncTestStep
	//
	/**
	 *
	 * Base Async Test class
	 *
	 */	
class CFsmTrigger;
class CAsyncTestMmfAclntAudioPlayerUtil : public RAsyncTestStep, public MMdaAudioPlayerCallback
	{
public:
	CAsyncTestMmfAclntAudioPlayerUtil( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	// state machine to access test specific functions
	virtual void FsmL() = 0;
	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);
protected:
	TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation);

protected:
	TInt iError;
	enum TTestState
		{
		EStateOpen=0,
		EStateOpened,
		EStatePlay,
		EStatePause,
		EStateSetRepeat,
		EStateStop,
		EStateGetPosition,
		EStateStopTest,
		EStateSetPosition
		};
	TTestState iTestState;
	enum TPlayerState
		{
		EOpening = 0,
		EOpened,
		EPlaying,
		EPaused,
		EStopped		
		};
	TPlayerState iPlayerState;
		
	TTimeIntervalMicroSeconds iClipDuration;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KSizeBuf>	iFileName;
	TBuf<KNameBufSize> iKeyName;
		
	TTime iStart;
	TTime iStop;
	
	TInt  iDuration;
	TUint iActualDuration;
	TUint iPausedDuration;
	TUint iResumedDuration;
	TInt  iNumOfRepeatsSet;
	TTimeIntervalMicroSeconds  iSilence;
	CMdaAudioPlayerUtility* iPlayer;
	// AO to invoke the FsmL
	CFsmTrigger* iTrigger;
	} ;	
	
	
class CFsmTrigger:	public CActive
	{
public:
	static CFsmTrigger* NewL(CAsyncTestMmfAclntAudioPlayerUtil& aObserver);
	virtual ~CFsmTrigger();
	
	// From CActive
	virtual void RunL();
	virtual void DoCancel();

	void StartTimer(TTimeIntervalMicroSeconds32 aDelay);
	
protected:
	CFsmTrigger(CAsyncTestMmfAclntAudioPlayerUtil& aObserver);
	void ConstructL();

private:
	RTimer iTimer;
	CAsyncTestMmfAclntAudioPlayerUtil& iObserver;
	};

/*
==========================================
RAsyncTest for Different SetRepeat scenario
==========================================
*/	
/**
 * RAsynctest audio player: repeat play Pause play
 *
 * @class CAsyncTestMmfAclntAudioRepeatPlayPausePlay
 *
 */
class CAsyncTestMmfAclntAudioRepeatPlayPausePlay:	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioRepeatPlayPausePlay* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CTestStepAudInStreamAsyncTest
	void FsmL();
private:
	CAsyncTestMmfAclntAudioRepeatPlayPausePlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	};
		
/**
 * RAsynctest audio player: repeat play SetPosition play
 *
 * @class CAsyncTestMmfAclntAudioRepeatPlayPausePlay
 *
 */
class CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay:	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TTimeIntervalMicroSeconds aPosition);

	// from CTestStepAudInStreamAsyncTest
	void FsmL();
private:
	CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TTimeIntervalMicroSeconds aPosition);
private:
	TTimeIntervalMicroSeconds iPosition;
	};
	
/**
 * RAsynctest audio player: repeat play Pause Repeat play
 *
 * @class CAsyncTestMmfAclntAudioRepeatPlayPausePlay
 *
 */		
class CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay:	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();

private:
	CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	

/**
 * RAsynctest audio player: repeat play Pause Repeat play
 *
 * @class CAsyncTestMmfAclntAudioPlayPauseRepeatPlay
 *
 */		
class CAsyncTestMmfAclntAudioPlayPauseRepeatPlay:	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioPlayPauseRepeatPlay* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();

private:
	CAsyncTestMmfAclntAudioPlayPauseRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/**
 * RAsynctest audio player: Play Repeat i,e setting repeat while playing.
 *
 * @class CAsyncTestMmfAclntAudioPlayRepeat
 *
 */		
class CAsyncTestMmfAclntAudioPlayRepeat:	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioPlayRepeat* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();

private:
	CAsyncTestMmfAclntAudioPlayRepeat( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/**
 * RAsynctest audio player: Play Repeat i,e setting repeats before open .
 *
 * @class CAsyncTestMmfAclntAudioRepeatBeforeOpen
 *
 */		
class CAsyncTestMmfAclntAudioRepeatBeforeOpen:	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioRepeatBeforeOpen* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();
	void KickoffTestL();

private:
	CAsyncTestMmfAclntAudioRepeatBeforeOpen( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/**
 * RAsynctest audio player: Play Repeat i,e setting repeats before open .
 *
 * @class CAsyncTestMmfAclntAudioRepeatBeforeOpen
 *
 */		
class CAsyncTestMmfAclntRepeatMultipleFiles:	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntRepeatMultipleFiles* NewL( const TDesC& aTestName, const TDesC& aSectName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();
	void KickoffTestL();

	// PDEF131534 SPPR_PERF: Music player crashes when opening corrupted wav file.
	// WAV file of 96000 Hz is not supported.	
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration);

private:
	CAsyncTestMmfAclntRepeatMultipleFiles( const TDesC& aTestName, const TDesC& aSectName);
	TInt iCount;
	};
	
/**
 * RAsynctest audio player: Pplay Pause repeat play
 *
 * @class CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay
 *
 */		
class CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay :	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();
	
private:
	CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	};
				
/**
 * RAsynctest audio player: Pplay Pause repeat play
 *
 * @class CAsyncTestMmfAclntAudioRepeatPlayStopPlay
 *
 */		
class CAsyncTestMmfAclntAudioRepeatPlayStopPlay :	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioRepeatPlayStopPlay* NewL( const TDesC& aTestName, const TDesC& aSectName,	const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();
		
private:
	CAsyncTestMmfAclntAudioRepeatPlayStopPlay( const TDesC& aTestName, const TDesC& aSectName,	const TDesC& aKeyName);
	};

/**
 * RAsynctest audio player: Repeat Play Repeat play
 *
 * @class CAsyncTestMmfAclntAudioRepeatPlayRepeatPlay
 *
 */		
class CAsyncTestMmfAclntAudioRepeatPlayRepeatPlay :	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioRepeatPlayRepeatPlay* NewL( const TDesC& aTestName, const TDesC& aSectName,	const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();	

private:
	CAsyncTestMmfAclntAudioRepeatPlayRepeatPlay( const TDesC& aTestName, const TDesC& aSectName,	const TDesC& aKeyName);
	};
	
/**
 * RAsynctest audio player: Play/Repeat/Repeat/Play/
 *
 * @class CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay
 *
 */		
class CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay :	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();

private:
	CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay( const TDesC& aTestName, const TDesC& aSectName,	const TDesC& aKeyName);
	};				
		
/**
 * RAsynctest audio player: Play/Repeat/SetVolRamp/Play/
 *
 * @class CAsyncTestMmfAclntAudioPlayRepeatRamp
 *
 */		
class CAsyncTestMmfAclntAudioPlayRepeatRamp :	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioPlayRepeatRamp* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();

private:
	CAsyncTestMmfAclntAudioPlayRepeatRamp( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
private:
	TInt iNumOfRepeats;
	};	
	
/**
 * RAsynctest audio player: Play/SetRepeatForever/Play/
 *
 * @class CAsyncTestMmfAclntAudioRepeatForeverPlay
 *
 */		
class CAsyncTestMmfAclntAudioRepeatForeverPlay :	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioRepeatForeverPlay* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TTimeIntervalMicroSeconds aTrailingSilence);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();

private:
	CAsyncTestMmfAclntAudioRepeatForeverPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TTimeIntervalMicroSeconds aTrailingSilence);
private:
	TTimeIntervalMicroSeconds iTrailingSilence;	
	};

/**
 * RAsynctest audio player: 
 *
 * @class CAsyncTestMmfAclntAudioRepeatPlayGetPosition
 *
 */		

class CAsyncTestMmfAclntAudioRepeatPlayGetPosition :	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioRepeatPlayGetPosition* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();

private:
	CAsyncTestMmfAclntAudioRepeatPlayGetPosition( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
private:
	TTimeIntervalMicroSeconds iPosition;	
	};
		
/**
 * RAsynctest audio player: Play/SetRepeatForever/Play/
 *
 * @class CAsyncTestMmfAclntAudioRepeatForeverPlay
 *
 */		

class CAsyncTestMmfAclntDRMRepeatPlay:	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntDRMRepeatPlay* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId, const TInt aRepeatCount);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();
	void KickoffTestL();
	
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration);
	void MapcPlayComplete(TInt aError);

private:
	CAsyncTestMmfAclntDRMRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName,const TDesC& aUniqueId, const TInt aRepeatCount);
private:
	TBuf<KIDBufSize> iUniqueId;
	};		
		
//===================================================================================
//NEGATIVE TESTS
//=======================================================
/**
 * RAsynctest audio player: Set _ve Repeat/Play
 *
 * @class CAsyncTestMmfAclntAudioPlayRepeatRamp
 *
 */		
class CAsyncTestMmfAclntAudioNegativeRepeatPlay :	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioNegativeRepeatPlay* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();
	
private:
	CAsyncTestMmfAclntAudioNegativeRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	};		
		
		
/**
 * RAsynctest audio player: Set _ve Repeat/Play
 *
 * @class CAsyncTestMmfAclntAudioPlayRepeatRamp
 *
 */		
class CAsyncTestMmfAclntAudioPlayNegativeRepeat :	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioPlayNegativeRepeat* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();
	
private:
	CAsyncTestMmfAclntAudioPlayNegativeRepeat( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	};			
/**
 * RAsynctest audio player: SetPlayWindow and Repeat play
 *
 * @class CAsyncTestMmfAclntAudioPlayRepeatRamp
 *
 */		
class CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay :	public CAsyncTestMmfAclntAudioPlayerUtil
	{
public:
	static CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	// from CAsyncTestMmfAclntAudioPlayerUtil
	void FsmL();
	
private:
	CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
private:
	TInt iWindowDuration;
	};	

//==================================================================================================	
	
/**
 * test audio pausing.
 *
 * @class CTestMmfAclntAudioPlayStopStart
 *
 */
class CTestMmfAclntAudioPlayStopStart : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioPlayStopStart(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntAudioPlayStopStart* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};

/**
 * test audio pausing.
 *
 * @class CTestMmfAclntAudioPlayStopStart
 *
 */
class CTestMmfAclntAudioPlayPauseStart : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioPlayPauseStart(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntAudioPlayPauseStart* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};

/**
 * Further testing of audio pausing.
 *
 * @class CTestMmfAclntAudioPlayPausePlayTest 
 *
 */
class CTestMmfAclntAudioPlayPausePlayTest : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioPlayPausePlayTest(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntAudioPlayPausePlayTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};

/**
 * test audio volume
 *
 * @class CTestMmfAclntAudioPlayVolume
 *
 */
class CTestMmfAclntAudioPlayVolume : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioPlayVolume(const TDesC& aTestName, const TDesC& aSectName, const TInt aVolume,TBool aNegativeTest);
	static CTestMmfAclntAudioPlayVolume* NewL(const TDesC& aTestName, const TDesC& aSectName, const TInt aVolume=-1, TBool aNegativeTest = EFalse);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);

private:
	TInt iVolume;
	TBool iNegativeTest;
	};

/**
 * test audio player repeat play
 *
 * @class CTestMmfAclntAudioRepeat
 *
 */
class CTestMmfAclntAudioRepeat : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioRepeat(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntAudioRepeat* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};


/**
 * test audio balance
 *
 * @class CTestMmfAclntAudioBalance
 *
 */
class CTestMmfAclntAudioBalance : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioBalance(const TDesC& aTestName, const TDesC& aSectName, const TInt aValue, const TInt aExpectedError);
	static CTestMmfAclntAudioBalance* NewL(const TDesC& aTestName, const TDesC& aSectName, const TInt aExpectedError, const TInt aValue);//=-1,);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);

private:
	TInt iBalance;
	TInt iExpectedError;
	};

/**
 * test audio duration
 *
 * @class CTestMmfAclntAudioDuration
 *
 */
class CTestMmfAclntAudioDuration : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioDuration(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntAudioDuration* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};


/**
 * test audio postions
 *
 * @class CTestMmfAclntAudioPosition
 *
 */
class CTestMmfAclntAudioPosition : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioPosition(const TDesC& aTestName, const TDesC& aSectName, const TTimeIntervalMicroSeconds& aPosition);
	static CTestMmfAclntAudioPosition* NewL(const TDesC& aTestName, const TDesC& aSectName, const TTimeIntervalMicroSeconds& aPosition);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);

private:
	TTimeIntervalMicroSeconds iPosition;
	};

/**
 * test audio play window
 *
 * @class CTestMmfAclntAudioPlayWindow
 *
 */
class CTestMmfAclntAudioPlayWindow : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioPlayWindow(const TDesC& aTestName, const TDesC& aSectName, const TBool aNegative);
	static CTestMmfAclntAudioPlayWindow* NewL(const TDesC& aTestName, const TDesC& aSectName, const TBool aNegative = EFalse);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
private:
	TBool iNegative;
	};

/**
 * get audio type
 *
 * @class CTestMmfAclntAudioType
 *
 */
class CTestMmfAclntAudioType : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioType(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntAudioType* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};

/**
 * test audio ramp
 *
 * @class CTestMmfAclntAudioRamp
 *
 */
class CTestMmfAclntAudioRamp : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioRamp(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntAudioRamp* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};

/**
 * view meta data
 *
 * @class CTestMmfAclntAudioMeta
 *
 */
class CTestMmfAclntAudioMeta : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioMeta(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntAudioMeta* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};

/**
 * close and then open the audio clip
 *
 * @class CTestMmfAclntAudioCloseOpen
 *
 */
class CTestMmfAclntAudioCloseOpen : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioCloseOpen(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntAudioCloseOpen* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};

/**
 * query audio formats
 *
 * @class CTestMmfAclntAudioQuery
 *
 */
class CTestMmfAclntAudioQuery : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntAudioQuery(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntAudioQuery* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};


/**
 * get audio bit rate
 *
 * @class CTestMmfAclntAudioBitRate
 *
 */

class CTestMmfAclntAudioBitRate : public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	static	CTestMmfAclntAudioBitRate* NewL(const TDesC& aTestName,const TDesC& aKeyName1, const TDesC& aKeyName2);
    virtual TVerdict DoTestStepL();
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);
    
private:
	CTestMmfAclntAudioBitRate(const TDesC& aTestName,const TDesC& aKeyName1, const TDesC& aKeyName2);

private:
	TBuf<KNameBufSize> iKeyFileName;	// Key name for retrieving filename
	TBuf<KNameBufSize> iKeyBitRateName; // Key name for retrieving bit rate
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	TInt iError;						// Stores error values for testing
	TInt iExpectedBitRate;		// Expected bps of the file
	};

/**
 * Audio priority.
 *
 * @class CTestMmfAclntPriority
 *
 */
class CTestMmfAclntPriority : public CTestMmfAclntStep, public MCallbackHandlerObserver, public MDelayedMethodCall
	{
public:
	virtual TVerdict DoTestStepL();
	void MchoComplete(TInt aID, TInt aError);
	static CTestMmfAclntPriority* NewL(const TDesC& aTestName, const TDesC& aSectName, const TMdaPriority aPriority);
	static CTestMmfAclntPriority* NewL(const TDesC& aTestName, const TDesC& aSectName);

	//Used to start playing file 0 after file 1 has already started
	void DelayedMethodCallL();
private:
	CTestMmfAclntPriority(const TDesC& aTestName, const TDesC& aSectName, const TMdaPriority aPriority);
	CTestMmfAclntPriority(const TDesC& aTestName, const TDesC& aSectName);

private:
	CMdaAudioPlayerUtility* audioUtil[2];

	TInt iCallbackCount;
	TInt iCallbackError[2];

	TInt iFirstCallback;
	TInt iError;
	TMdaPriority iPriority;
	TBool iPriorityManual;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	} ;


//
// NEGATIVE TESTS
//

/** 
 *
 * Play with no loaded sound file.
 *
 * @class CTestMmfAclntNoLoad
 *
 */
class CTestMmfAclntNoLoad : public CTestMmfAclntAudioPlayerUtil
	{

public:
	CTestMmfAclntNoLoad(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntNoLoad* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	virtual TVerdict DoTestStepL();
	};

/**
 * Audio clip - Delete object before play operation has completed.
 *
 * @class CTestMmfAclntDeletePlay
 *
 */
class CTestMmfAclntDeletePlay : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntDeletePlay(const TDesC& aTestName, const TDesC& aSectName);
	static CTestMmfAclntDeletePlay* NewL(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
	};

/** 
 * Open new file while audio player is playing.
 *
 * @class CTestMmfAclntPlayPlay
 * 
 */
class CTestMmfAclntPlayPlay : public CTestMmfAclntAudioPlayerUtil
	{
public:
	CTestMmfAclntPlayPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	static CTestMmfAclntPlayPlay* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	virtual TVerdict DoTestL(CMdaAudioPlayerUtility* aPlayer);
private:
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	};

/**
 * Play using a corrupt descriptor.
 *
 * @class CTestMmfAclntCorruptDes
 *
 */
class CTestMmfAclntCorruptDes : public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	CTestMmfAclntCorruptDes(const TDesC& aTestName, const TDesC& aSectName);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	HBufC8* iAudio;
	TInt iError;
	TBuf<KNameBufSize> iSectName;					// Section name for retrieving filename
	} ;


#endif
