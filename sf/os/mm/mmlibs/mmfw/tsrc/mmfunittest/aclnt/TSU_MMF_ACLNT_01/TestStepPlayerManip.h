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
// TestStepPlayerManip.cpp
// (manipulation)
// This file contains testing steps for the Unit
// test of the MMF 'Deprecated' Audio Clients.
// Tests opening sounds of various formats
// from a file with various parameters and constructor calls
// 
//


#ifndef __TEST_STEP_PLAYER_MANIP__
#define __TEST_STEP_PLAYER_MANIP__

/**
 *
 * Play and Stop sound
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
const TInt KBufSize		= 25;

class CTestStepPlayerStop :	public	CTestStepUnitMMFAudClient,
							public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerStop* NewL();
	CTestStepPlayerStop();
	~CTestStepPlayerStop();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TBool iHasPlayed;
	};

/**
 *
 * Get sound Duration
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerDuration :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerDuration* NewL();
	CTestStepPlayerDuration();
	~CTestStepPlayerDuration();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	};

/**
 *
 * Set sound Repeats
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerRepeats :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerRepeats* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

	~CTestStepPlayerRepeats();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);
private:
	CTestStepPlayerRepeats(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
private:
	TInt iError;
	TBuf<KBufSize> iSectName;					// Section name for retrieving filename
	TBuf<KBufSize> iKeyName;					// Key name for retrieving filename

	CMMFMdaAudioPlayerUtility* iPlayer;
	};


/**
 *
 * Set sound Repeats & test state during Trailing silence 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerRepeatsState :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	enum TSchedulerStopReasons {EUnknown, EPlayerInitialized, EPlayComplete, ETimer};

public:
	static CTestStepPlayerRepeatsState* NewL();
	CTestStepPlayerRepeatsState();
	~CTestStepPlayerRepeatsState();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

	class CCallbackTimer : public CTimer
		{
	public:
		CCallbackTimer(CTestStepPlayerRepeatsState& aTest);
		~CCallbackTimer();
		static CCallbackTimer* NewL(CTestStepPlayerRepeatsState& aTest);
		void RunL();
	private:
		CTestStepPlayerRepeatsState& iTest;
		};

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;

	CCallbackTimer* iTimer;

public:
	TSchedulerStopReasons iSchedulerStopReason;
	};

/**
 *
 * Set Volume Ramp
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerVolRamp :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerVolRamp* NewL();
	CTestStepPlayerVolRamp();
	~CTestStepPlayerVolRamp();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TTimeIntervalMicroSeconds* iDuration;
	};

/**
 *
 * Set Volume 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerVol :	public	CTestStepUnitMMFAudClient,
							public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerVol* NewL();
	CTestStepPlayerVol();
	~CTestStepPlayerVol();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TPtrC iFileName2;
	};

/**
 *
 * Get/Set position
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerPosition :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPosition* NewL();
	CTestStepPlayerPosition();
	~CTestStepPlayerPosition();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TTimeIntervalMicroSeconds iDuration;
	TBool iHasPlayed;
	};

/**
 *
 * Set/Clear Play WIndow
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerPlayWindow :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPlayWindow* NewL();
	CTestStepPlayerPlayWindow();
	~CTestStepPlayerPlayWindow();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TTimeIntervalMicroSeconds iDuration, iBeginning, iMiddle,iEnd;
	TBool iHasPlayed;
	};

/**
 *
 * Get/Set balance
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerBalance :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerBalance* NewL();
	CTestStepPlayerBalance();
	~CTestStepPlayerBalance();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TTimeIntervalMicroSeconds iDuration;
	};

/**
 *
 * Close
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerClose :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerClose* NewL();
	CTestStepPlayerClose();
	~CTestStepPlayerClose();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TTimeIntervalMicroSeconds iDuration;
	};

/**
 *
 * Metadata
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerMeta :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerMeta* NewL();
	CTestStepPlayerMeta();
	~CTestStepPlayerMeta();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TInt iNum;
	};

/**
 *
 * Priority
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerPriority :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPriority* NewL();
	CTestStepPlayerPriority();
	~CTestStepPlayerPriority();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TInt iNum;
	};
	

//
// CTestStepPlayerPauseSetPosPlay
//
class CTestStepPlayerPauseSetPosPlay : public CTestStepUnitMMFAudClient,
									   public MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPauseSetPosPlay* NewL();
	CTestStepPlayerPauseSetPosPlay();
	~CTestStepPlayerPauseSetPosPlay();
	void Close();

	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);

protected:
	CMMFMdaAudioPlayerUtility* iPlayer;
	TInt iError;
	};

//
// CTestStepPlayerPlayWaitSetPos - Start playing the file, wait and then mid-playback reset the position to a different point.  Ensure playback completes.
//
class CTestStepPlayerPlayWaitSetPos : public CTestStepUnitMMFAudClient,
									   public MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPlayWaitSetPos* NewL();
	CTestStepPlayerPlayWaitSetPos();
	~CTestStepPlayerPlayWaitSetPos();
	
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);

	static TInt TimerCallback(TAny* aParentObject);
	void DoTimerCallback();
	
protected:
	CMMFMdaAudioPlayerUtility* iPlayer;
	CPeriodic* iSetPositionCountDown;
	TInt iError;
	};
	
/**
 *
 * GetImplementationInfo
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerGetImplementationInfo :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerGetImplementationInfo* NewL();
	CTestStepPlayerGetImplementationInfo();
	~CTestStepPlayerGetImplementationInfo();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TInt iNum;
	};


/**
 *
 * SendCustomCommandSync
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerSendCustomCommandSync :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerSendCustomCommandSync* NewL();
	CTestStepPlayerSendCustomCommandSync();
	~CTestStepPlayerSendCustomCommandSync();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TInt iNum;
	};


/**
 *
 * SendCustomCommandSync
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerSendCustomCommandAsync :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerSendCustomCommandAsync* NewL();
	CTestStepPlayerSendCustomCommandAsync();
	~CTestStepPlayerSendCustomCommandAsync();
	void Close();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);
    
   	class CAsyncObserver : public CActive
		{
	public:
		CAsyncObserver() : CActive(EPriorityStandard) {CActiveScheduler::Add(this);}
		void RunL()
			{
			CActiveScheduler::Stop();
			}
		void DoCancel(){;}
		TRequestStatus& ActiveStatus(){SetActive();return iStatus;}
		};


private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	CAsyncObserver* iAsyncObserver;
	TInt iNum;
	};
	
	
/**
 *
 * ReloadingTest
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerReloadingTest :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback,
								public  MAudioLoadingObserver
	{
public:
	static CTestStepPlayerReloadingTest* NewL();
	CTestStepPlayerReloadingTest();
	~CTestStepPlayerReloadingTest();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);
    
    virtual void MaloLoadingStarted();
    virtual void MaloLoadingComplete();

private:
	TInt iError;
	TBool iCorrectEventReceived;
	TBool iEventReceived;
	TBool iLoadingComplete;
	TBool iLoadingStarted;

	CMMFMdaAudioPlayerUtility* iPlayer;
	TInt iNum;
	};
	
/**
 *
 * Test Open sound specifying a url
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerOpenUrl :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerOpenUrl* NewL();
	CTestStepPlayerOpenUrl();
	~CTestStepPlayerOpenUrl();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	void OpenAndStartSchedulerL(TInt);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TPtrC iURL;
	TPtrC iLongURL;
	TPtrC iMimeType;
	};
	
/**
 *
 * Test Open sound specifying Uri Tag
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerOpenviaUriTag :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerOpenviaUriTag* NewL();
	CTestStepPlayerOpenviaUriTag();
	~CTestStepPlayerOpenviaUriTag();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);
	
private:
	void OpenAndStartSchedulerL(TInt);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TPtrC iURL;
	TPtrC iMimeType;
	};

/**
 *
 * Test Open sound Trying next controller
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerOpenTryNextCtrl :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerOpenTryNextCtrl* NewL();
	CTestStepPlayerOpenTryNextCtrl();
	~CTestStepPlayerOpenTryNextCtrl();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);
	
private:
	void OpenAndStartSchedulerL(TInt);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TPtrC iURL;
	TPtrC iMimeType;
	};


/**
 *
 * INC048184 - SetPlayWindow,play,Stop,Fast Farward,Close,play
 * make sure second play starts from start of file
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerPlayStopFFPlayNext :	public	CTestStepUnitMMFAudClient,
									        public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPlayStopFFPlayNext* NewL();
	~CTestStepPlayerPlayStopFFPlayNext();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);

private:
	CTestStepPlayerPlayStopFFPlayNext();
	void FastForward(const TTimeIntervalMicroSeconds aPosition);
	void Next(void);
private:
	TInt iError;
	TInt iCurrentState;
	TPtrC iFileName;
	CMdaAudioPlayerUtility* iPlayerUtility;
	};

/**
 *
 * DEF067052: To setpriorities after Pausing the player
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepMultipleSetPlayerPriority :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepMultipleSetPlayerPriority* NewL();
	CTestStepMultipleSetPlayerPriority();
	~CTestStepMultipleSetPlayerPriority();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	};


/**
 *
 * DEF070060: play-pause-play fails on JSR135 TCK test (setloopcount2)
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerPlayPausePlay :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPlayPausePlay* NewL();
	CTestStepPlayerPlayPausePlay();
	~CTestStepPlayerPlayPausePlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	};

/**
 *
 * DEF078081: TCK fail: CMMFMdaAudioPlayerUtility::Duration can't handle infinite sounds
 * This defect resulted in the call TMMFDurationInfo Duration(TTimeIntervalMicroSeconds& aDuration) 
 * being added to CMdaAudioPlayerUtility, CMdaAudioRecorderUtility
 *
 * This test case checks that the call returns TMMFDurationInfo::EMMFDurationInfoValid
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlaybackDurationInformation :	public	CTestStepUnitMMFAudClient,
							      	    public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlaybackDurationInformation* NewL();
	CTestStepPlaybackDurationInformation();
	~CTestStepPlaybackDurationInformation();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt);

private:
	TInt iError;
	CMdaAudioPlayerUtility* iPlayer;
	};

/**
 *
 * CheckImplementationInfo
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerCheckImplementationInfo :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerCheckImplementationInfo* NewL();
	CTestStepPlayerCheckImplementationInfo();
	~CTestStepPlayerCheckImplementationInfo();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TInt iNum;
	};
	

/* DEF090666 : T:Aclnt - Pausing during repeat causes the file to close itself
*
* @lib "TSU_MMF_ACLNT_01.lib"
*/

class CTestStepPlayerRepeatsPause : public	CTestStepUnitMMFAudClient,
								 public	MMdaAudioPlayerCallback
	{
public:
	enum TSchedulerStopReasons {EUnknown, EPlayerInitialized, EPlayComplete, ETimer};
public:
	static CTestStepPlayerRepeatsPause* NewL();
	CTestStepPlayerRepeatsPause();
	~CTestStepPlayerRepeatsPause();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

	class CCallbackTimer : public CTimer
		{
	public:
		CCallbackTimer(CTestStepPlayerRepeatsPause& aTest);
		~CCallbackTimer();
		static CCallbackTimer* NewL(CTestStepPlayerRepeatsPause& aTest);
		void RunL();
	private:
		CTestStepPlayerRepeatsPause& iTest;
		};
private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	CCallbackTimer* iTimer;
public:
	TSchedulerStopReasons iSchedulerStopReason;
	};


#endif //(__TEST_STEP_PLAYER_MANIP__)
