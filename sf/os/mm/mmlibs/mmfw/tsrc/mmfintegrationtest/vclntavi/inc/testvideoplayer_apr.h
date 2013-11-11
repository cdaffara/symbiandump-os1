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
// Header file: Declaration for Various Classes used for Audio Resource Notification tests for Video Client.
// 
//

/**
 @file Testvideoplayer_apr.h
*/

#ifndef TESTVIDEOPLAYER_APR_H
#define TESTVIDEOPLAYER_APR_H


#include "testvideoplayer.h"
#include "mdaaudiosampleplayer.h"
#include "u32hal.h"


const TInt KSizeBuf = 256;
const TInt KMaxEvents = 2;

//------------------------------------------------------------------------------
//********************PREQ 797 - Audio Resource Notification********************
//------------------------------------------------------------------------------

/**
 * stopActiveScheduler
 */
static TInt stopActiveScheduler(TAny*)
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}

//
//Declarations//
//

//
//Custom Error Codes & CONST Declarations.
//
const TInt KErrUser = -1000;
const TInt KErrBadResult = (KErrUser - 1);
const TInt KMicroSecsInOneSec = 1000000; 
const TUint KSampleRate8000Hz  = 8000;

//
//Forward class declarations
//
class CCallbackTimer;
class CTestAVPlayerAPRCallbackSupport;
class CVclntAviAprWillResumeTimer;
//------------------------------------------------------------------------------
//******************CR 0805 - Auto Pause Resume for video client*******************
//------------------------------------------------------------------------------

//
//ENUM DECLARATIONS//
//

// Enum to describe the current state.
enum TAVPlayerAPRState
	{
	EStateBegin = 0,
	EStateOpenAudioPlayer1,
	EStateOpenAudioPlayer2,
	EStateAudioPlaying1,
	EStateAudioPlaying2,
	EStateEndAudioPlay1,
	EStateEndAudioPlay1A,
	EStateEndAudioPlay2,
	EStateEndAudioPlay2A,
	EStateErrInUseAudioPlayer1,
	EStateErrInUseAudioPlayer2,
	EStateNotifiedAudioPlayer1,
	EStateNotifiedAudioPlayer2,
	EStateErrInUseAudioPlayer1A,
	EStateNotifiedAudioPlayer1A,
	EStateAudioPlaying2A,
	EStateAudioEndPlay2A,
	EStatePrepareVideoPlayer,
	EStateOpenVideoPlayer,
	EStateVideoPlaying,
	EStateEndVideoPlay,
	EStateErrInUseVideoPlayer,
	EStateNotifiedVideoPlayer,
	EStateErrInUseVideoPlayer1A,
	EStateNotifiedVideoPlayer1A,
	EStateVideoPlaying2A,
	EStateEndVideoPlay2A,
	EStateNoOtherPlayerNotified
	};

// Enum to decide which Event has occured.
enum TAVPlayerAPREvents
	{
	EIdlePlayer = 0,
	EMvpuoOpenComplete,
	EMvpuoPrepareComplete,
	EMvpuoFrameReady,
	EMvpuoPlayComplete,
	EMvpuoEvent,
	EMapcInitComplete1,
	EMapcInitComplete2,
	EMapcPlayComplete1,
	EMapcPlayComplete2,
	EMarncResourceAvailable1,
	EMarncResourceAvailable2,
	EMarncResourceAvailable3
	};

//
//BASE CLASS DECLARATIONS//
//

/**
 * timer support
 *
 * @class CCallBackTimer
 *
 */

class CCallBackTimer : public CTimer
	{
public:
	static CCallBackTimer* NewL(TCallBack aCallBack, TPriority aPriority = EPriorityStandard);
private:
	virtual void RunL();
private:
	CCallBackTimer(TCallBack aCallBack, TPriority aPriority);
private:
	TCallBack iCallBack;
	};


/**
 * Observer class to identify and pass events to the test step class
 * 
 * @class MTestAVPlayerAPRCallback
 *
 */
class MTestAVPlayerAPRCallback
	{
public :
	virtual void MvpuoOpenCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport) = 0;
	virtual void MvpuoPrepareCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport)= 0;
	virtual void MvpuoFrameReadyTest(CFbsBitmap &aFrame, TInt aError, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport)=0;
	virtual void MvpuoPlayCompleteTest(TInt aError,CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport)=0;
	virtual void MvpuoEventTest(const TMMFEvent &aEvent, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport)=0;
	virtual void MapcInitCompleteTest(TInt aError, const TTimeIntervalMicroSeconds &aDuration, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport)=0;
	virtual void MapcPlayCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport)=0;
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport) = 0;
	};

/**
 * Support class to pass the events to the test step class using the MTestAVPlayerAPRCallback
 * 
 * @class CTestAVPlayerAPRCallbackSupport
 *
 */
class CTestAVPlayerAPRCallbackSupport : public CBase, public MVideoPlayerUtilityObserver, public MMMFAudioResourceNotificationCallback, public MMdaAudioPlayerCallback
	{
public :
	static CTestAVPlayerAPRCallbackSupport* NewL(MTestAVPlayerAPRCallback& aCallback);
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent &aEvent);
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration);
	virtual void MapcPlayComplete(TInt aError);
	virtual void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
	
protected:
	CTestAVPlayerAPRCallbackSupport(MTestAVPlayerAPRCallback& aCallback);
	void ConstructL();
private :
	MTestAVPlayerAPRCallback& iCallback;
	};


/**
 * Base Class for APR functionality for most APR video player tests
 * 
 * @class RTestMmfVclntVideoPlayerAPR
 *
 */
class RTestMmfVclntVideoPlayerAPR : public RTestMmfVclntAviStep, public MTestAVPlayerAPRCallback
	{
public :
	// Constructor
	RTestMmfVclntVideoPlayerAPR(const TDesC& aTestName, const TDesC& aSectName);

	// Virtual methods for Preamble and Postamble.
	virtual TVerdict DoTestStepCreateL();
	virtual TVerdict DoTestStepDeleteL();
	// Virtual function to access test specific methods
	virtual TVerdict DoTestStepL();
	// Pure function to access test specific methods
	virtual TVerdict DoTestL()=0;
	// from MTestAVPlayerAPRCallback
	virtual void MvpuoOpenCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport);
	virtual void MvpuoPrepareCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport);
	virtual void MvpuoFrameReadyTest(CFbsBitmap &aFrame, TInt aError, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport);
	virtual void MvpuoPlayCompleteTest(TInt aError,CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport);
	virtual void MvpuoEventTest(const TMMFEvent &aEvent, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport);
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport);
	virtual void MapcInitCompleteTest(TInt aError, const TTimeIntervalMicroSeconds &aDuration, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport);
	virtual void MapcPlayCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport);
	
	// Centralised State Handler
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError)=0;
	virtual TBool FsmCheck(TAVPlayerAPREvents aEventCode, TInt aError);
	virtual TInt SetCacheSize();
	virtual TInt CheckCacheError(TInt aError);
		
protected :	
		// StateInit Function
	void StateInit();
	// StateChange Function
	void StateChange(TAVPlayerAPRState aState, TAVPlayerAPREvents aExpectedEvent, TInt aExpectedError=KErrNone, TInt aEventIndex = 1);
	// check if we can go to the next state
	TInt AnalyseInput(TAVPlayerAPREvents aEventCode, TInt aError);
protected :	

	// CMdaAudioPlayerUtility instances
	CMdaAudioPlayerUtility* iTestAudioPlayer1;
	CMdaAudioPlayerUtility* iTestAudioPlayer2;
	// CVideoPlayerUtility instances
	CVideoPlayerUtility* iTestVideoPlayer;
	// CTestAVPlayerAPRCallbackSupport instances
	CTestAVPlayerAPRCallbackSupport* iVideoPlayerCallbackSupport1;
	CTestAVPlayerAPRCallbackSupport* iAudioPlayerCallbackSupport1;
	CTestAVPlayerAPRCallbackSupport* iAudioPlayerCallbackSupport2;

	
	// Other class variable declarations
	TBuf<KSizeBuf> iFilename1;
	TBuf<KSizeBuf> iFilename2;
	TBuf<KSizeBuf> iFilename3;
	TTimeIntervalMicroSeconds iPosition;
	TTimeIntervalMicroSeconds iDuration;
	TInt iError;
	TAVPlayerAPRState iState;
	TRequestStatus iRequestStatus;
	CCallBackTimer* iCallBackTimer;
	TAVPlayerAPREvents iExpectedEvent[KMaxEvents];
	TInt iExpectedError[KMaxEvents];
	TBool iCanStateChange;
	TInt iExpEventCount;
	TInt iActEventCount;
	};

//
//POSITIVE TESTS//
//

/** 
 * One-time registration for same event by a player.
 *
 * RTestVclntAprRegOnce4SameEvent
 * MM-MMF-VCLNTAVIAPR-I-0001-HP
 * 
 */
class RTestVclntAprRegOnce4SameEvent : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprRegOnce4SameEvent* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
protected:
	RTestVclntAprRegOnce4SameEvent( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Register before opening Video player and receive notification of a resource available event 
 *
 * RTestVclntAprRegB4Open
 * MM-MMF-VCLNTAVIAPR-I-0002-HP
 * 
 */
class RTestVclntAprRegB4Open : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprRegB4Open* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
protected:
	RTestVclntAprRegB4Open( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Register (after Open and before Play of video) , receive notification of a resource available event.
 *
 * RTestVclntAprRegAftrOpenAndB4Play
 * MM-MMF-VCLNTAVIAPR-I-0003-HP
 * 
 */
class RTestVclntAprRegAftrOpenAndB4Play : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprRegAftrOpenAndB4Play* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	TTimeIntervalMicroSeconds iVidPosition;
	
protected:
	RTestVclntAprRegAftrOpenAndB4Play( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Register after the highest priority client (Audio) completes playing and receive the notification of a resource availalble event.
 *
 * RTestVclntAprRegAftrHpClntCompltPlay
 * MM-MMF-VCLNTAVIAPR-I-0004-HP
 * 
 */
class RTestVclntAprRegAftrHpClntCompltPlay : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprRegAftrHpClntCompltPlay* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
protected:
	RTestVclntAprRegAftrHpClntCompltPlay( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};
	
/** 
 * Receive notification by same priority players in the order of registration
 *
 * RTestVclntAprRegSamePriorityPlayers
 * MM-MMF-VCLNTAVIAPR-I-0005-HP
 * 
 */
class RTestVclntAprRegSamePriorityPlayers : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprRegSamePriorityPlayers* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
protected:
	RTestVclntAprRegSamePriorityPlayers( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};


/** 
 * Register (at the time of Play) and Cancellation of the notification request
 *
 * RTestVclntAprRegAtPlayAndCancelNfn
 * MM-MMF-VCLNTAVIAPR-I-0006-HP
 * 
 */
class RTestVclntAprRegAtPlayAndCancelNfn : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprRegAtPlayAndCancelNfn* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
protected:
	RTestVclntAprRegAtPlayAndCancelNfn( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Test for WillResumePlay() using 3 clients.
 *
 * RTestVclntAprTestWillResumePlay
 * MM-MMF-VCLNTAVIAPR-I-0007-HP
 * 
 */
class RTestVclntAprTestWillResumePlay : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprTestWillResumePlay* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	virtual void FinishedL(); // for CVclntAviAprWillResumeTimer	

protected:
	RTestVclntAprTestWillResumePlay( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	CVclntAviAprWillResumeTimer* iWillResumePlayTimer;
	};

/**
 * 
 * CVclntAviAprWillResumeTimer
 * MM-MMF-VCLNTAVIAPR-I-0007-HP
 * - Timer for checking if no notification occurs in 5 seconds
 *   after calling WillResumePlay()
 *
 */
class CVclntAviAprWillResumeTimer : public CTimer
    {
public:
    static CVclntAviAprWillResumeTimer* NewL(RTestVclntAprTestWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    void Start();
protected:
    TInt RunError(TInt aError);
    void RunL();
private:
    CVclntAviAprWillResumeTimer(RTestVclntAprTestWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    RTestVclntAprTestWillResumePlay* iParent;
    TTimeIntervalMicroSeconds32 iWaitTime;
    };

/** 
 * APR Alloc tests (using shared heap) for Video Player utility.
 *
 * RTestVclntAprAllocUsingSharedHeap
 * MM-MMF-VCLNTAVIAPR-I-0008-HP
 * 
 */
class RTestVclntAprAllocUsingSharedHeap : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprAllocUsingSharedHeap* NewL( const TDesC& aTestName, const TDesC& aSectName );
	void VideoPlayAllocL();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
		
protected:
	RTestVclntAprAllocUsingSharedHeap( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};
	

//
//NEGATIVE TESTS//
//

/** 
 * Multiple Registration (at the time of Play) and receive notification of a resource available event
 *
 * RTestVclntAprMultRegAtPlay
 * MM-MMF-VCLNTAVIAPR-I-0101-HP
 * 
 */
class RTestVclntAprMultRegAtPlay : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprMultRegAtPlay* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
protected:
	RTestVclntAprMultRegAtPlay( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};
	
/** 
 * Cancel when no resource Notification is requested.
 *
 * RTestVclntAprCancelNoResourceNfn
 * MM-MMF-VCLNTAVIAPR-I-0102-HP
 * 
 */
 class RTestVclntAprCancelNoResourceNfn : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprCancelNoResourceNfn* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
protected:
	RTestVclntAprCancelNoResourceNfn( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};
	
/** 
 * Register for notification with an unsupported event type.
 *
 * RTestVclntAprRegUnsupportedEvent
 * MM-MMF-VCLNTAVIAPR-I-0103-HP
 * 
 */
class RTestVclntAprRegUnsupportedEvent : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprRegUnsupportedEvent* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
protected:
	RTestVclntAprRegUnsupportedEvent( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};
	
/** 
 * Cancellation of  notification with an unsupported event type.
 *
 * RTestVclntAprCancelUnsupportedEvent
 * MM-MMF-VCLNTAVIAPR-I-0104-HP
 * 
 */
class RTestVclntAprCancelUnsupportedEvent : public RTestMmfVclntVideoPlayerAPR
	{
public:
	static RTestVclntAprCancelUnsupportedEvent* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void Fsm(TAVPlayerAPREvents aEventCode, TInt aError);
	
protected:
	RTestVclntAprCancelUnsupportedEvent( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};
	
#endif
