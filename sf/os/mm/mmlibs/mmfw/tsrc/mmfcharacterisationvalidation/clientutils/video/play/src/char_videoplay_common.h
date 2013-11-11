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

#ifndef CHAR_VIDEOPLAY_COMMON_H
#define CHAR_VIDEOPLAY_COMMON_H

#include <mdaaudiosampleplayer.h>
#include <videoplayer.h>
#include <e32std.h>

#include <testframework.h>

const TInt KNameBufSize            = 25;

// Video Controller UIDs
const TUid KMmfTestAviPlayControllerUid = { 0x102737A5 };
const TUid KMmfTestAviRecordControllerUid = { 0x102737E1 };

enum TVideoPlayState
    {
    EVPIdle = 0,
    EVPOpening,
    EVPOpenComplete,
    EVPOpening1,
    EVPOpenComplete1,
    EVPPreparing,
    EVPPrepareComplete,
    EVPPreparing1,
    EVPPrepareComplete1,
    EVPPlaying,
    EVPPlaying1,
    EVPPlaying2,
    EVPPlaying3,
    EVPPlaying4,
    EVPPlayResumed,
    EVPPlayComplete,
    EVPPlayComplete1,
    EVPPaused,
    EVPStopped,
    EVPStopped1,
    EVPFrameRequested
    };

// Array to display which state change event occurred.
const TText* const KVideoPlayState[] =
    {
    _S("EVPIdle"),
    _S("EVPOpening"),
    _S("EVPOpenComplete"),
    _S("EVPOpening1"),
    _S("EVPOpenComplete1"),
    _S("EVPPreparing"),
    _S("EVPPrepareComplete"),
    _S("EVPPreparing1"),
    _S("EVPPrepareComplete1"),
    _S("EVPPlaying"),
    _S("EVPPlaying1"),
    _S("EVPPlaying2"),
    _S("EVPPlaying3"),
    _S("EVPPlaying4"),
    _S("EVPPlayResumed"),
    _S("EVPPlayComplete"),
    _S("EVPPlayComplete1"),
    _S("EVPPaused"),
    _S("EVPStopped"),
    _S("EVPStopped1"),
    _S("EVPStopped2"),
    _S("EVPFrameRequested")
    };

TAny GetDriveName(TDes& aFileName);

class RTestBaseVideoPlay : public RAsyncTestStep,
					       public MVideoPlayerUtilityObserver
	{
public:
	RTestBaseVideoPlay(const TDesC& aStepName, const TDesC& aSectName);
	
	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();
	virtual void DoKickoffTestL() = 0; //-- for the derived class to implement

	// from MVideoPlayerUtilityObserver
	void MvpuoOpenComplete(TInt aError);
	void MvpuoPrepareComplete(TInt aError);
	void MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
	void MvpuoPlayComplete(TInt aError);
	void MvpuoEvent(const TMMFEvent &aEvent);
	
	// State machine functions
	virtual void Fsm() = 0;
	void SetExpectedState(TVideoPlayState aExpState, TInt aExpError = KErrNone);
	TBool ValidateState();
	
public:
	static TInt TimerCallback(TAny*);
	virtual void DoTimerCallback();
	
protected:
    void InitWservL();
    void DeInitWserv();

public:
	TFileName iFilename;
	CVideoPlayerUtility* iUtility;
	TCallBack iCallBack;
	CPeriodic* iTimer;
	TVideoPlayState iState;
	TInt iError;
	TVideoPlayState iExpState;
	TInt iExpError;

protected:
    // Window server related objects
    RWsSession iWs;
    CWsScreenDevice* iScreenDevice;
    RWindow* iWindow;
    RWindowGroup iRootWindow;
    TBuf<KNameBufSize> iSectName;            // Section name for retrieving filename
   	};

#endif // CHAR_VIDEOPLAY_COMMON_H
