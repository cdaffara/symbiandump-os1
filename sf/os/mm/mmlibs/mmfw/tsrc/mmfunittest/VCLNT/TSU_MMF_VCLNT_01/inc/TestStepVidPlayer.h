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
// This file contains testing steps for the Unit
// test of the MMF Video Clients.
// Tests exercise all exposed member functions.
// 
//


#ifndef TEST_STEP_VID_PLAYER
#define TEST_STEP_VID_PLAYER

//#include <eikenv.h>
//#include <eikappui.h>
//#include "VclntAppTest.h"
#include <mmf/common/mmfaudio.h>

#include <mdaaudiooutputstream.h>

#ifdef SYMBIAN_BUILD_GCE
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#include <mmf/devvideo/devvideovideosurfacecustominterface.h>
#endif // SYMBIAN_BUILD_GCE


_LIT (KTestAppFullPath, "Z:\\Sys\\bin\\VclntAppTest.exe");
 
/**
 *
 * Initialise
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerNew :	public	CTestStepUnitMMFVidClient,
								public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerNew* NewL(TBool aUseSurface = EFalse);
	CTestStepVidPlayerNew(TBool aUseSurface);
	~CTestStepVidPlayerNew();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Initialise Priority Preference
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerNewPriorPref :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerNewPriorPref* NewL(TBool aUseSurface = EFalse);
	CTestStepVidPlayerNewPriorPref(TBool aUseSurface);
	~CTestStepVidPlayerNewPriorPref();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TMMFPrioritySettings iPrioritySettings;
	};

/**
 *
 * Open File
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerOpenFile :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	enum TTest
		{
		EFileName,
		EFileHandle,
		EFileSource,
		EFileHandleSource,
		};
		
public:
	static CTestStepVidPlayerOpenFile* NewL(const TDesC& aTestName, const TTest aTest);
	CTestStepVidPlayerOpenFile(const TDesC& aTestName, const TTest aTest);
	~CTestStepVidPlayerOpenFile();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TTest iTest;
	};

/**
 *
 * Open Desc
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerOpenDesc :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerOpenDesc* NewL(TInt aUseSurface = CTestStepUnitMMFVidClient::ENotSurfaceMode);
	CTestStepVidPlayerOpenDesc(TInt aUseSurface);
	~CTestStepVidPlayerOpenDesc();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	void OpenDescAndStartSchedulerL();

private:
	TInt iError;
	TMMFEvent iEvent;
	HBufC8* iBuf;
	};

/**
 *
 * Open Url
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerOpenUrl :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerOpenUrl* NewL(TInt aUseSurface = CTestStepUnitMMFVidClient::ENotSurfaceMode);
	CTestStepVidPlayerOpenUrl(TInt aUseSurface);
	~CTestStepVidPlayerOpenUrl();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	void OpenUrlAndStartSchedulerL();

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Open Url via uri tag
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerOpenviaUriTag :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerOpenviaUriTag* NewL();
	CTestStepVidPlayerOpenviaUriTag();
	~CTestStepVidPlayerOpenviaUriTag();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	void OpenUrlAndStartSchedulerL();

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Open Url while media client video uses try next controller
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerOpenTryNextCtrl :	public	CTestStepUnitMMFVidClient,
											public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerOpenTryNextCtrl* NewL();
	CTestStepVidPlayerOpenTryNextCtrl();
	~CTestStepVidPlayerOpenTryNextCtrl();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	void OpenUrlAndStartSchedulerL();

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Open File Specific Controller Uid
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerOpenFileCtrlUid :	public	CTestStepUnitMMFVidClient,
											public	MVideoPlayerUtilityObserver
	{
public:
	enum TOpenMethod
		{
		EFileName,
		EFileHandle,
		EDescriptor,
		EUrl,
		};
	static CTestStepVidPlayerOpenFileCtrlUid* NewL(TInt aUseFileHandle, 
				TInt aSurfaceMode = CTestStepUnitMMFVidClient::ENotSurfaceMode);
	CTestStepVidPlayerOpenFileCtrlUid(TInt aUseFileHandle, TInt aSurfaceMode);
	~CTestStepVidPlayerOpenFileCtrlUid();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	void OpenFileAndStartSchedulerL();

private:
	TInt iError;
	TMMFEvent iEvent;
	TInt iUseFileHandle;
	TInt iSurfaceMode;
	RBuf8 iBuf;
	};

/**
 *
 * Close
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerClose :	public	CTestStepUnitMMFVidClient,
								public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerClose* NewL();
	CTestStepVidPlayerClose();
	~CTestStepVidPlayerClose();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Play
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerPlay :	public	CTestStepUnitMMFVidClient,
								public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerPlay* NewL();
	CTestStepVidPlayerPlay();
	~CTestStepVidPlayerPlay();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};
	
/**
 *
 * Stop
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerStop :	public	CTestStepUnitMMFVidClient,
								public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerStop* NewL();
	CTestStepVidPlayerStop();
	~CTestStepVidPlayerStop();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Pause
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerPause :	public	CTestStepUnitMMFVidClient,
								public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerPause* NewL();
	CTestStepVidPlayerPause();
	~CTestStepVidPlayerPause();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get/Set Priority
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerPriority :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
									//public	MMMFControllerEventMonitorObserver // is it  here?
	{
public:
	static CTestStepVidPlayerPriority* NewL();
	CTestStepVidPlayerPriority();
	~CTestStepVidPlayerPriority();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Set Display Window
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 */
class CTestStepVidPlayerSetDisplayWindow :	public	CTestStepUnitMMFVidClient,
											public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerSetDisplayWindow* NewL(TBool aUseSurface, 
						TInt aStatus = CTestStepUnitMMFVidClient::ENotSurfaceMode);
	CTestStepVidPlayerSetDisplayWindow(TBool aUseSurface, TInt aStatus);
	~CTestStepVidPlayerSetDisplayWindow();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Player Restart
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerRestart : public CTestStepUnitMMFVidClient,
								  public MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerRestart* NewL();
	CTestStepVidPlayerRestart();
	~CTestStepVidPlayerRestart();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	virtual void Close();

	// from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get Frame
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerGetFrame :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerGetFrame* NewL(TBool aUseIntentAPI);
	
	~CTestStepVidPlayerGetFrame();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	
protected:
	CTestStepVidPlayerGetFrame(TBool aUseIntentAPI);

private:
	TBool iUseIntentAPI;
	TInt iError;
	TInt iError2;
	TMMFEvent iEvent;
	CFbsBitmap iFrame;
	};

/**
 *
 * Get Frame Rate
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerGetFrameRate :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerGetFrameRate* NewL();
	CTestStepVidPlayerGetFrameRate();
	~CTestStepVidPlayerGetFrameRate();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get Video frame size
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerGetVFrameSize :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerGetVFrameSize* NewL();
	CTestStepVidPlayerGetVFrameSize();
	~CTestStepVidPlayerGetVFrameSize();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get Video bit rate
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerGetVBitRate :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerGetVBitRate* NewL();
	CTestStepVidPlayerGetVBitRate();
	~CTestStepVidPlayerGetVBitRate();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get Audio bit rate
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerGetABitRate :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerGetABitRate* NewL();
	CTestStepVidPlayerGetABitRate();
	~CTestStepVidPlayerGetABitRate();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get Audio enabled
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerAudEnabled :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerAudEnabled* NewL();
	CTestStepVidPlayerAudEnabled();
	~CTestStepVidPlayerAudEnabled();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get position in the clip
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerClipPosition :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerClipPosition* NewL();
	CTestStepVidPlayerClipPosition();
	~CTestStepVidPlayerClipPosition();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get duration of the clip
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerDuration :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerDuration* NewL();
	CTestStepVidPlayerDuration();
	~CTestStepVidPlayerDuration();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get video type
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerVideoType :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerVideoType* NewL();
	CTestStepVidPlayerVideoType();
	~CTestStepVidPlayerVideoType();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get audio type
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerAudioType :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerAudioType* NewL();
	CTestStepVidPlayerAudioType();
	~CTestStepVidPlayerAudioType();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Volume functions
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerVol :	public	CTestStepUnitMMFVidClient,
								public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerVol* NewL();
	CTestStepVidPlayerVol();
	~CTestStepVidPlayerVol();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Set/Get balance functions
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerBalance :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerBalance* NewL();
	CTestStepVidPlayerBalance();
	~CTestStepVidPlayerBalance();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Metadata functions
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerMeta :	public	CTestStepUnitMMFVidClient,
								public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerMeta* NewL();
	CTestStepVidPlayerMeta();
	~CTestStepVidPlayerMeta();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};
	



/**
 *
 * PlayWindow
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerPlayWindow : 	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerPlayWindow* NewL();
	CTestStepVidPlayerPlayWindow();
	~CTestStepVidPlayerPlayWindow();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};
	
/**
 *
 * GetImplementationInfo
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerGetImplementationInfo : public	CTestStepUnitMMFVidClient,
												public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerGetImplementationInfo* NewL();
	CTestStepVidPlayerGetImplementationInfo();
	~CTestStepVidPlayerGetImplementationInfo();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};

/**
 *
 * RefreshFrame
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerRefreshFrame : 	public CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerRefreshFrame* NewL();
	CTestStepVidPlayerRefreshFrame();
	~CTestStepVidPlayerRefreshFrame();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};

/**
 *
 * SendCustomCommandSync
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerSendCustomCommandSync : public CTestStepUnitMMFVidClient,
												public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerSendCustomCommandSync* NewL();
	CTestStepVidPlayerSendCustomCommandSync();
	~CTestStepVidPlayerSendCustomCommandSync();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};

/**
 *
 * SendCustomCommandAsync
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerSendCustomCommandAsync : 	public CTestStepUnitMMFVidClient,
													public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerSendCustomCommandAsync* NewL();
	CTestStepVidPlayerSendCustomCommandAsync();
	~CTestStepVidPlayerSendCustomCommandAsync();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	
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
	TMMFEvent iEvent;
	TBool iHasPlayed;
	CAsyncObserver* iAsyncObserver;
	TMMFMessageDestinationPckg iDestinationPckg;
	};
	
/**
 *
 * CTestStepVidPlayerGeneralEvent
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerGeneralEvent : 	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerGeneralEvent* NewL();
	CTestStepVidPlayerGeneralEvent();
	~CTestStepVidPlayerGeneralEvent();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TBool iCorrectEventReceived;
	TBool iEventReceived;
	TInt iError;
	};
	
/**
 *
 * CTestStepVidPlayerReloadingTest
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerReloadingTest : public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver,
										public MVideoLoadingObserver
	{
public:
	static CTestStepVidPlayerReloadingTest* NewL();
	CTestStepVidPlayerReloadingTest();
	~CTestStepVidPlayerReloadingTest();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	
	//from MVideoLoadingObserver
	virtual void MvloLoadingStarted();
	virtual void MvloLoadingComplete();

private:
	TBool iCorrectEventReceived;
	TBool iEventReceived;
	TBool iLoadingComplete;
	TBool iLoadingStarted;
	TInt iError;
	};

/**
 *
 * Test Get/SetRotation methods
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerRotation : 	public CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerRotation* NewL();
	CTestStepVidPlayerRotation();
	~CTestStepVidPlayerRotation();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};


/**
 *
 * Test Get/SetScaleFactor methods
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerScale : public CTestStepUnitMMFVidClient,
								public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerScale* NewL();
	CTestStepVidPlayerScale();
	~CTestStepVidPlayerScale();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};
	
/**
 *
 * Test Get/SetCropRegion methods
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerCropRegion : 	public CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerCropRegion* NewL();
	CTestStepVidPlayerCropRegion();
	~CTestStepVidPlayerCropRegion();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};

/**
 *
 * PlayPause
 *
 * INC041303  - PSS - PlaybackComplete problem in MMF Framework 
 * This checks to see whether we get the MvpuoPlayComplete() callback
 * in the paused state: It relies on the test controller
 * doing nothing much when it gets a Pause command.
 *
 */
class CTestStepVidPlayerPlayPause : public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerPlayPause* NewL();
	CTestStepVidPlayerPlayPause();
	~CTestStepVidPlayerPlayPause();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};
	
/**
 *
 * Open and play a protected video clip from file using PLAY intent.
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerDrm :	public	CTestStepUnitMMFVidClient,
								public	MVideoPlayerUtilityObserver
	{
public:
	enum TState
		{
		EOpen,
		EPrepare,
		EPlay,
		EStop
		};
		
	enum TTest
		{
		EProtectedPlayIntent,
		EProtectedUnknownIntent,
		EProtectedPlayNoRights
		};
		
	enum TSource
		{
		EFileSource,
		EFileHandleSource
		};
		
public:
	static CTestStepVidPlayerDrm* NewL(const TDesC& aTestName, TSource aSource, TTest aTest);
	static CTestStepVidPlayerDrm* NewAllocTestL(const TDesC& aTestName, TSource aSource, TTest aTest);
	~CTestStepVidPlayerDrm();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	CTestStepVidPlayerDrm(const TDesC& aTestName, TSource aSource, TTest aTest, TBool aDoAllocTest);
	TVerdict GetConfig();
	TInt OpenFile();
	void CheckVerdict(const TDesC& aName);
	void RunStepL();
	void NextState();
	
	TVerdict DoTestStepAllocL();
	
private:
	TInt iError;
	TState iState;
	TTest iTest;
	TSource iSource;
	RFile iFile;
	RFs iFs;
	
	TBool iDoAllocTest;
	};

/**
 *
 * Player stop and start Direct Screen Access
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerStopStartDSA : public CTestStepUnitMMFVidClient,
								  		public MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerStopStartDSA* NewL(TInt aSurfaceMode = CTestStepUnitMMFVidClient::ENotSurfaceMode);
	CTestStepVidPlayerStopStartDSA(TInt aSurfaceMode);
	~CTestStepVidPlayerStopStartDSA();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	virtual void Close();

	// from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	};
	
/**
 *
 * Test case for DEF081389,DEF082497,DEF085513 
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerOpenDurationCustomCommand :	public	CTestStepUnitMMFVidClient,
													public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerOpenDurationCustomCommand* NewL();
	CTestStepVidPlayerOpenDurationCustomCommand();
	~CTestStepVidPlayerOpenDurationCustomCommand ();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	
	//from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
private:
	void OpenUrlAndStartSchedulerL();
private:
	TInt iError;
	TMMFEvent iEvent;
	};


/**
 *
 * CheckImplementationInfo
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidPlayerCheckImplementationInfo : public	CTestStepUnitMMFVidClient,
												public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerCheckImplementationInfo* NewL();
	CTestStepVidPlayerCheckImplementationInfo();
	~CTestStepVidPlayerCheckImplementationInfo();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};

/**
 *
 * Test case for DEF092672
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 *
 */
class CTestStepVideoInterruptionByAudio : public CTestStepUnitMMFVidClient,
											public MMdaAudioOutputStreamCallback,
											public MVideoPlayerUtilityObserver
	{
	enum TEvents
		{
		EAudioOpenComplete,
		EAudioPlayComplete,
		EVideoOpenComplete,
		EVideoPrepareComplete,
		EVideoFrameReady,
		EVideoPlayComplete			
		};	
public:
	static CTestStepVideoInterruptionByAudio* NewL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	TVerdict DoTestStepL();
	void Tester(TEvents aEvent, TInt aError);
	
	//MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);
	
	//MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	void OnTimer();
	void FailTest();
protected:
	CTestStepVideoInterruptionByAudio();
protected:
	CMdaAudioOutputStream*	iAudStreamA;
	CVideoPlayerUtility* iPlayer;
	RBuf8 iBuffer;
	TBool iVidGotKErrInUse; //if video got KErrInUse in MvpuoPlayComplete()
	TTime iPlayStartTime;
	TTime iPlayEndTime;
	TTimeIntervalMicroSeconds iPlayTime;
	class CTestTimer : public CTimer
		{
		public:
			static CTestTimer* NewL(CTestStepVideoInterruptionByAudio& aTestStep)
				{
				CTestTimer* self = new(ELeave)CTestTimer(aTestStep);
				CleanupStack::PushL(self);
				self->ConstructL();
				CleanupStack::Pop();
				return self;
				}
			CTestTimer(CTestStepVideoInterruptionByAudio& aTestStep):CTimer(EPriorityLow),iContainer(aTestStep)
				{
				CActiveScheduler::Add(this);	
				}
		protected:
			void RunL()
				{
				iContainer.OnTimer();
				}
		private:
			CTestStepVideoInterruptionByAudio& iContainer;
		};
	CTestTimer* iTestTimer;
	};
/**
 * Observer class to identify and pass events to the test step class
 * 
 * @class MTestArnCallback
 *
 */
class CTestGenericArnCallback;
class MTestArnCallback
	{
public:
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestGenericArnCallback & aGenericArnCallback) = 0;
	};

/**
 * Support class to identify, which client has obtained ARN - when multiple clients have registered for ARN
 * 
 * @class CTestGenericArnCallback
 *
 */
class CTestGenericArnCallback : public CBase, public MMMFAudioResourceNotificationCallback
	{
public:
	static CTestGenericArnCallback* NewL(MTestArnCallback& aCallback);
	//MMMFAudioResourceNotificationCallback
	virtual void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
protected:
	CTestGenericArnCallback(MTestArnCallback& aCallback);
private :
	MTestArnCallback& iCallback;
	};

class CTestGenericAosCallback;
class MTestAosCallback
	{
public:
	virtual void MaoscOpenCompleteTest(TInt aError,CTestGenericAosCallback& aGenericAosCallback) = 0;
	virtual void MaoscBufferCopiedTest(TInt aError, const TDesC8& aBuffer,CTestGenericAosCallback& aGenericAosCallback) = 0;
	virtual void MaoscPlayCompleteTest(TInt aError,CTestGenericAosCallback& aGenericAosCallback) = 0;	 	
	};

class CTestGenericAosCallback : public MMdaAudioOutputStreamCallback
	{
public:
	static CTestGenericAosCallback* NewL(MTestAosCallback& aCallback);
	//MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);
protected:
	CTestGenericAosCallback(MTestAosCallback& aCallback);
private:
	MTestAosCallback& iCallback;
	};

/**
 * Check audio resource notification ARN functionality
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 * 
 */
class CTestStepVidPlayerARN : public CTestStep,
							  public MVideoPlayerUtilityObserver,
							  public MTestArnCallback,
							  public MTestAosCallback
	{
public:
	enum TClient //enumeration of clients
		{
		ETestStep,//represents test step 
		EAudioStreamA, //audio stream A (low priority)
		EAudioStreamB, //audio stream B (high priority)
		EVideo	//video player		
		};
		
	enum TTestEvent //enumeration of events
		{
		ETestStart,
		EAudioOpenComplete,
		EAudioBufferCopied,
		EAudioPlayComplete,
		EArnAvailable,		//audio resource available
		EVideoOpenComplete,
		EVideoPrepareComplete,
		EVideoFrameReady,
		EVideoPlayComplete,
		};
	/*
	 * Represents a transition set. This contains Client, event and error.
	 */	
	class CTransitionInput: public CBase
		{
	public:
		CTransitionInput():iMatched(EFalse),iIgnoreError(EFalse)
			{			
			}
		CTransitionInput(TClient aClient, TTestEvent aEvent, TInt aError):
			iClient(aClient), iEvent(aEvent), iError(aError)
			{	
			}
		void SetData(TClient aClient, TTestEvent aEvent, TInt aError)
			{
			iClient = aClient;
			iEvent = aEvent;
			iError = aError;	
			}
			
		TClient iClient;	
		TTestEvent iEvent;
		TInt iError;

		TBool iMatched; //whether match is donne for client, event, error.
		TBool iIgnoreError;
		};
			
public:
	~CTestStepVidPlayerARN();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
		
	//MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	
	//MTestArnCallback
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestGenericArnCallback & aGenericArnCallback) = 0;
	
	//MTestAosCallback
	virtual void MaoscOpenCompleteTest(TInt aError,CTestGenericAosCallback& aGenericAosCallback);
	virtual void MaoscBufferCopiedTest(TInt aError, const TDesC8& aBuffer,CTestGenericAosCallback& aGenericAosCallback);
	virtual void MaoscPlayCompleteTest(TInt aError,CTestGenericAosCallback& aGenericAosCallback);
	
	virtual void EndTest() = 0;
	void FailTest();
	
	virtual TBool JumpStateIfRequired(TClient aClient, TTestEvent aEvent, TInt aError,TInt& aState) = 0;
	virtual TBool NextState(TInt& aState) = 0;
	virtual TInt AnalyseInput(TClient aClient, TTestEvent aEvent, TInt aError); //-1 endtest; 0 continue in current state; 1 new state
	virtual void OnStateEntry() = 0; //on entering new state
	virtual void Fsm(TClient aClient, TTestEvent aEvent, TInt aError);
protected:
	CTestStepVidPlayerARN();
	void InitWservL();
	void DeInitWserv();
	
protected:	
	RBuf8 iBuffer;
	CMdaAudioOutputStream*	iAudStreamA;
	CMdaAudioOutputStream*	iAudStreamB;
	CVideoPlayerUtility* iPlayer;
	TPtrC iFileName;
	CTestGenericArnCallback* iAudClientAArnCallback; //ARN callback for audio clientA
	CTestGenericArnCallback* iAudClientBArnCallback; //ARN callback for audio clientB
	CTestGenericArnCallback* iVidClientArnCallback; //ARN callback for video client
	
	CTestGenericAosCallback* iAudStreamACallback; //Audio output stream callback for - audio stream A
	CTestGenericAosCallback* iAudStreamBCallback; //Audio output stream callback for - audio stream B
	
	class CTestTimer : public CTimer
		{
		public:
			static CTestTimer* NewL(CTestStepVidPlayerARN& aTestStepVidPlayer)
				{
				CTestTimer* self = new(ELeave)CTestTimer(aTestStepVidPlayer);
				CleanupStack::PushL(self);
				self->ConstructL();
				CleanupStack::Pop();
				return self;
				}
			CTestTimer(CTestStepVidPlayerARN& aTestStepVidPlayer):CTimer(EPriorityStandard),iContainer(aTestStepVidPlayer)
				{
				CActiveScheduler::Add(this);	
				}
		protected:
			void RunL()
				{
				iContainer.EndTest();
				}
		private:
			CTestStepVidPlayerARN& iContainer;
		};
	CTestTimer* iTestTimer;
	CActiveScheduler* iActiveScheduler;
	CWsScreenDevice* iScreen;
	RWindow* iWindow;
	RWsSession iWs;
	RWindowGroup iRootWindow;
	
	TFixedArray<CTransitionInput*, 15> ipTransitionSet; //for maximum 15 states
	TFixedArray<TInt, 15> iExpInputCnt; //expected input events count
	TFixedArray<TInt, 15> iRxInputCnt; //received input events count
	TInt iCurrentState;
	TBool iVideoOpen;
	};
	

class CTestStepVidPlayerARN0700 : public CTestStepVidPlayerARN
	{
	enum TState
		{
		EPreStart,
		EStart, //starting state; on-state-entry, open the video
		EVideoOpen,	//video opened; on-state-entry, prepare video
		EVideoPrepare,//video prepared; 
			// on-state-entry, register video for Arn and play; then open stream-B 
		EVideoPlayingAudBOpen,//video registered and playing and audio stream-B opened;
			// on-state-entry write to audio stream-B
		EVideoInterrupted, //audio component of video interrupted on starting audio stream-B;
			// on-state-entry do nothing.	
		ETestEnd //on-state-entry end the test with pass.
		};
public:
	static CTestStepVidPlayerARN0700* NewL();
	~CTestStepVidPlayerARN0700();
	void InitTransitionSetL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	
	//MTestArnCallback
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestGenericArnCallback & aGenericArnCallback);

	virtual void EndTest();
	TBool JumpStateIfRequired(TClient aClient, TTestEvent aEvent, TInt aError,TInt& aState);
	TBool NextState(TInt& aState);
	void OnStateEntry(); //on entering new state
protected:
	CTestStepVidPlayerARN0700();
	};

class CTestStepVidPlayerARN0701 : public CTestStepVidPlayerARN
	{
	//states
	enum TState
		{
		EPreStart,
		EStart, //starting state; on-state-entry, open the video
		EVideoOpen,	//video opened; on-state-entry, prepare video
		EVideoPrepare,//video prepared; on-state-entry, register video for Arn and open audio stream-B 
		EVideoRegisterAudBOpen,//video registered and audio stream-B opened; 
							//on-state-entry play video;cancel Audio registration on Video; and write to audio stream-B
							//video play interrupted on starting audio stream-B
		EVideoInterrupted,  //video play interrupted state; on-state-entry don't do anything
		EFailureOnNotification //failure as video gets notification
		};
public:
	static CTestStepVidPlayerARN0701* NewL();
	~CTestStepVidPlayerARN0701();
	void InitTransitionSetL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	
	//MTestArnCallback
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestGenericArnCallback & aGenericArnCallback);
		
	virtual void EndTest();
	
	TBool JumpStateIfRequired(TClient aClient, TTestEvent aEvent, TInt aError,TInt& aState);
	TBool NextState(TInt& aState);
	void OnStateEntry(); //on entering new state
protected:
	CTestStepVidPlayerARN0701();
private:
	TBool iTestSimulationComplete;
	};
	
class CTestStepVidPlayerARN0702 : public CTestStepVidPlayerARN
	{
	enum TState
		{
		EPreStart,
		EStart, //starting state; on-state-entry, open the video
		EVideoOpen,	//video opened; on-state-entry, prepare video
		EVideoPrepare,//video prepared; 
					 //on-state-entry,open and register audio streams A and B
		EAudABOpenRegister,//audio streams A and B opened and registered.
						  //on-state-entry play video and start streams A and B	
		EVideoAudAThrownoff, //video and stream A were thrownoff by stream B;
						     //on-state-entry register for notification on Video.
		EVideoNotified,	//video got audio resource notification; on-state-entry Video->WillResumePlay()
		EFailureOnAudioANotification
		};
public:
	static CTestStepVidPlayerARN0702* NewL();
	void InitTransitionSetL();
	~CTestStepVidPlayerARN0702();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	
	//MTestArnCallback
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestGenericArnCallback & aGenericArnCallback);
		
	virtual void EndTest();
	
	TBool JumpStateIfRequired(TClient aClient, TTestEvent aEvent, TInt aError,TInt& aState);
	TBool NextState(TInt& aState);
	void OnStateEntry(); //on entering new state
protected:
	CTestStepVidPlayerARN0702();
private:
	TBool iAudStreamANotified;
	};

class CTestStepVidPlayerARN0703 : public CTestStepVidPlayerARN
	{
	//states
	enum TState
		{
		EPreStart,
		EStart, //starting state; on-state-entry, open the video
		EVideoOpen,	//video opened; on-state-entry, prepare video
		EVideoPrepare,//video prepared; on-state-entry, register video for audio resource
		};
public:
	static CTestStepVidPlayerARN0703* NewL();
	void InitTransitionSetL();
	~CTestStepVidPlayerARN0703();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
				
	//MTestArnCallback
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestGenericArnCallback & aGenericArnCallback);
		
	virtual void EndTest();
	
	TBool JumpStateIfRequired(TClient aClient, TTestEvent aEvent, TInt aError,TInt& aState);
	TBool NextState(TInt& aState);
	void OnStateEntry(); //on entering new state
protected:
	CTestStepVidPlayerARN0703();
	};
	
/**
 *
 * Try to getting the audio type which is not there in the provided file.
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 */
class CTestStepVidOnlyPlayerNoAudioType : public CTestStepUnitMMFVidClient,
									      public MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidOnlyPlayerNoAudioType* NewL();
	CTestStepVidOnlyPlayerNoAudioType();
	~CTestStepVidOnlyPlayerNoAudioType();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

NONSHARABLE_CLASS(CTestStepINC122181) : public CTestStepUnitMMFVidClient,
							public MVideoPlayerUtilityObserver
	{
public:		
	static CTestStepINC122181* NewL();
	
	~CTestStepINC122181();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	CTestStepINC122181();

private:
	TInt iError;
	TInt iError2;

	};

#ifdef SYMBIAN_BUILD_GCE	
//
// graphics surface use test cases
//
/**
 *
 * AddDisplayWindowL 
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 */
class CTestStepVidPlayerAddDisplayWindow :	public	CTestStepUnitMMFVidClient,
											public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerAddDisplayWindow* NewL(TBool aSimplifiedVer, TInt aError, TBool aSameWindow = ETrue);
	CTestStepVidPlayerAddDisplayWindow(TBool aSimplifiedVer, TInt aError, TBool aSameWindow);
	~CTestStepVidPlayerAddDisplayWindow();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	void OnTimer();
private:
	class CTestTimer : public CTimer
		{
		public:
			static CTestTimer* NewL(CTestStepVidPlayerAddDisplayWindow& aTestStepVidPlayer);
			CTestTimer(CTestStepVidPlayerAddDisplayWindow& aTestStepVidPlayer):CTimer(EPriorityStandard),iContainer(aTestStepVidPlayer)
				{
				CActiveScheduler::Add(this);	
				}
		protected:
			void RunL()
				{
				iContainer.OnTimer();
				}
		private:
			CTestStepVidPlayerAddDisplayWindow& iContainer;
		};
	
	TBool TestSetVideoExtentL();
	TBool TestSetGetRotationL();
	TBool TestSetGetWindowClipRectL();
	TBool TestSetGetScaleFactorL();
	TBool TestSetGetCropRegionL();
	TInt SendCustomCommandSync(TMMFMessageDestinationPckg aDestPckg, TInt aFunction);
private:
	CTestTimer* iTestTimer;
	TInt iError;
	TMMFEvent iEvent;
	TBool iSimplifiedVer;
	TBool iSameWindow;
	TInt iExpectedError;
	TInt iDisplayId;
	TBool iSurfaceCreated;
	TBool iDisplayUpdated;
	};
	
/**
 *
 * Set Init Screen Number
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 */
class CTestStepVidPlayerSetInitScreenNumber :	public	CTestStepUnitMMFVidClient,
												public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerSetInitScreenNumber* NewL(TBool aUseSurface);
	CTestStepVidPlayerSetInitScreenNumber(TBool aUseSurface);
	~CTestStepVidPlayerSetInitScreenNumber();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};	

/**
 *
 * RemoveDisplayWindowL 
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 */
class CTestStepVidPlayerRemoveDisplayWindow :	public	CTestStepUnitMMFVidClient,
											public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerRemoveDisplayWindow* NewL(TBool aPositive);
	CTestStepVidPlayerRemoveDisplayWindow(TBool aPositive);
	~CTestStepVidPlayerRemoveDisplayWindow();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iPositive;
	};

class CTestStepVidPlayerSetWindowClipRect :	public	CTestStepUnitMMFVidClient,
											public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerSetWindowClipRect* NewL(TInt aExpected);
	CTestStepVidPlayerSetWindowClipRect(TInt aExpected);
	~CTestStepVidPlayerSetWindowClipRect();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TInt iExpected;
	};
	
class CTestStepVidPlayerSetVideoExtent :	public	CTestStepUnitMMFVidClient,
											public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerSetVideoExtent* NewL(TInt aExpected);
	CTestStepVidPlayerSetVideoExtent(TInt aExpected);
	~CTestStepVidPlayerSetVideoExtent();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TInt iExpected;
	};

/**
 *
 * Send events for replace sequence 1 - Create Surface, Parameter Changed, Remove Surface, Create Surface
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 */
class CTestStepVidPlayerReplaceSurfaceSequence1 :	public	CTestStepUnitMMFVidClient,
											public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerReplaceSurfaceSequence1* NewL();
	CTestStepVidPlayerReplaceSurfaceSequence1();
	~CTestStepVidPlayerReplaceSurfaceSequence1();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	void OnTimer();
private:
	class CTestTimer : public CTimer
		{
		public:
			static CTestTimer* NewL(CTestStepVidPlayerReplaceSurfaceSequence1& aTestStepVidPlayer);
			CTestTimer(CTestStepVidPlayerReplaceSurfaceSequence1& aTestStepVidPlayer):CTimer(EPriorityStandard),iContainer(aTestStepVidPlayer)
				{
				CActiveScheduler::Add(this);	
				}
		protected:
			void RunL()
				{
				iContainer.OnTimer();
				}
		private:
			CTestStepVidPlayerReplaceSurfaceSequence1& iContainer;
		};
	CTestTimer* iTestTimer;
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Send events for replace sequence 2 - Create Surface, Parameter Changed, Create Surface
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 */
class CTestStepVidPlayerReplaceSurfaceSequence2 :	public	CTestStepUnitMMFVidClient,
											public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerReplaceSurfaceSequence2* NewL();
	CTestStepVidPlayerReplaceSurfaceSequence2();
	~CTestStepVidPlayerReplaceSurfaceSequence2();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	void OnTimer();
private:
	class CTestTimer : public CTimer
		{
		public:
			static CTestTimer* NewL(CTestStepVidPlayerReplaceSurfaceSequence2& aTestStepVidPlayer);
			CTestTimer(CTestStepVidPlayerReplaceSurfaceSequence2& aTestStepVidPlayer):CTimer(EPriorityStandard),iContainer(aTestStepVidPlayer)
				{
				CActiveScheduler::Add(this);	
				}
		protected:
			void RunL()
				{
				iContainer.OnTimer();
				}
		private:
			CTestStepVidPlayerReplaceSurfaceSequence2& iContainer;
		};
	CTestTimer* iTestTimer;
	TInt iError;
	TMMFEvent iEvent;
	};

class CTestStepVidPlayerAutoScale :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepVidPlayerAutoScale* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestStepL();

	//from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	CTestStepVidPlayerAutoScale(const TDesC& aTestName);
	
private:
	TInt iError;
	TMMFEvent iEvent;
	};
		
#endif // SYMBIAN_BUILD_GCE
	
#endif // TEST_STEP_VID_PLAYER
