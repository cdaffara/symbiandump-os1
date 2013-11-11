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

#ifndef CHAR_VIDEOPLAY_COMMON_APR_H
#define CHAR_VIDEOPLAY_COMMON_APR_H

#include <mdaaudiosampleplayer.h>
#include <videoplayer.h>
#include <e32std.h>
#include <e32base.h>
#include "char_videoplay_common.h"

#include <testframework.h>

const TInt KSizeBuf = 256;
const TInt KMaxEvents = 2;

// Forward class declaration

class CTestAVPlayerAPRCallbackSupport;
class CVclntAviAprWillResumeTimer;

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

TAny GetDriveName(TDes& aFileName);

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
 * Base Class for APR functionality for APR video player tests
 * 
 * @class RTestBaseVideoPlayAPR
 *
 */
class RTestBaseVideoPlayAPR : public RAsyncTestStep,
							  public MTestAVPlayerAPRCallback
	{
public :
	// Constructor
	RTestBaseVideoPlayAPR(const TDesC& aTestName, const TDesC& aSectName);

	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();
	virtual void DoKickoffTestL() = 0; //-- for the derived class to implement

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
	virtual TBool ValidateEvent(TAVPlayerAPREvents aEventCode, TInt aError);
	static TInt TimerCallback(TAny*);
	virtual void DoTimerCallback();
protected :	
		// StateInit Function
	void StateInit();
	// StateChange Function
	void StateChange(TAVPlayerAPRState aState, TAVPlayerAPREvents aExpectedEvent, TInt aExpectedError=KErrNone, TInt aEventIndex = 1);
	// check if we can go to the next state
	TInt AnalyseInput(TAVPlayerAPREvents aEventCode, TInt aError);
protected :	

	// CMdaAudioPlayerUtility instances
	CMdaAudioPlayerUtility* iAudioPlayer1;
	CMdaAudioPlayerUtility* iAudioPlayer2;
	// CVideoPlayerUtility instances
	CVideoPlayerUtility* iVideoPlayer;
	// CTestAVPlayerAPRCallbackSupport instances
	CTestAVPlayerAPRCallbackSupport* iVideoPlayerCallbackSupport1;
	CTestAVPlayerAPRCallbackSupport* iAudioPlayerCallbackSupport1;
	CTestAVPlayerAPRCallbackSupport* iAudioPlayerCallbackSupport2;
	
	// window server related functions
	void InitWservL();
    void DeInitWserv();


	
	// Other class variable declarations
	TBuf<KSizeBuf> iFilename1;
	TBuf<KSizeBuf> iFilename2;
	TBuf<KSizeBuf> iFilename3;
	TTimeIntervalMicroSeconds iPosition;
	TTimeIntervalMicroSeconds iDuration;
	TInt iError;
	TAVPlayerAPRState iState;
	TRequestStatus iRequestStatus;
	TCallBack iCallBack;
	CPeriodic* iTimer;
	TAVPlayerAPREvents iExpectedEvent[KMaxEvents];
	TInt iExpectedError[KMaxEvents];
	TBool iCanStateChange;
	TInt iExpEventCount;
	TInt iActEventCount;

	// Window server related objects
    RWsSession iWs;
    CWsScreenDevice* iScreenDevice;
    RWindow* iWindow;
    RWindowGroup iRootWindow;
    TBuf<KNameBufSize> iSectName;            // Section name for retrieving filename
	};

#endif // CHAR_VIDEOPLAY_COMMON_APR_H
