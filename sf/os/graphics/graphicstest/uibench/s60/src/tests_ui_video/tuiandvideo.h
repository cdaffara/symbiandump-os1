/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TUIANDVIDEO_H__
#define __TUIANDVIDEO_H__

#include <coecntrl.h> 
#include <coeaui.h>
#include <eikenv.h>

#include "te_conestepbase.h"

_LIT(KTUiAndVideo,"tuiandvideo");

#include <coecntrl.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
//#include <eikmenup.h>
//#include <eikon.hrh>
//#include <eikon.rsg>
#include <apgcli.h>
#include <videoplayer2.h>

class CTestAppView;
class CVideoPane;
class CTUiAndVideoAppUi;

/**
Helper class to manage commands between test framework and test thread.
*/
class CCommandSink : public CActive
{
public:
	CCommandSink(CTestAppView* aTestView);
	~CCommandSink();
	void Init(RThread* aTargetThread, TRequestStatus* aTargetStatus);
	
protected:
	void DoCancel();
	
private:
	void RunL();
	
	CTestAppView* 	iTestView;
	TRequestStatus* iTargetStatus; 
	RThread* 		iTargetThread;
	TUint32 		iState;
};


class CTUiAndVideo : public CTe_ConeStepBase
	{
public:
	CTUiAndVideo();

private:
	// From CTestStep
	virtual TVerdict doTestStepL();
	virtual void InitUIL(CCoeEnv* aCoeEnv);
	void IssueCommand (TUint aCommand);
	void StepState (TUint aNextState, TUint aPause);

private:
	CTUiAndVideoAppUi* iAppUi;
	TRequestStatus iTestState;
	};

//************************************************************************************************************
//** Application UI
//************************************************************************************************************

class CTUiAndVideoAppUi : public CEikAppUi, public MVideoPlayerUtilityObserver
	{
public:
	void ConstructL(TRequestStatus iCommandSource);
	~CTUiAndVideoAppUi();

public: // from MVIdeoPlayerUtilityObserver
	void MvpuoOpenComplete(TInt aError);
	void MvpuoPrepareComplete(TInt aError);
	void MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
	void MvpuoPlayComplete(TInt aError);
	void MvpuoEvent(const TMMFEvent &aEvent);

private:
	// from CEikAppUi
	void HandleCommandL(TInt aCommand);	
	void HandleForegroundEventL(TBool aForeground);
	static TInt BackgroundCallBack(TAny* aSelf);
	static TInt UIMoverCallBack(TAny* aSelf);
	void SetPos();
	void BringToForeground();
	static TInt PlayTimerCallBack(TAny* aSelf);

public:
	void Bounce (TBool aBounce);
	void Jump   (TBool aJump);
	
	TInt PopulateDirFilesL(const TDesC& aDir);
	void OpenSelectedFileL(const TDesC& aFile);
	TInt PlayNextFileL();
	TInt PlayPreviousFileL();
	void LogError(const TDesC& aFileName, const TDesC& aErrorMsg);
	TRequestStatus* CommandSink ();
	TBool iInitialised;
	
private:
	CCommandSink* iCommandSink;
	CTestAppView* iAppView;
	CVideoPlayerUtility2* iLoadingPlayer;
	CPeriodic* iIdleTimer;
	CPeriodic* iUIMover;
	CPeriodic* iPlayTimer;
	CDir* iDirList;
	TInt iCurrentIndex;
	TFileName iCurrentFile;
	};



//************************************************************************************************************
//** View class
//************************************************************************************************************

class CTestAppView : public CCoeControl, public MCoeControlObserver
	{
public:
	static CTestAppView* NewL(const TRect& aWindowedRect);
	~CTestAppView();
	RWindow& TheWindow();
	const RWindow& TheWindow() const { return const_cast<CTestAppView*>(this)->TheWindow(); }
	void EnableVideo(CVideoPlayerUtility2*& aPlayer);
	void DisableVideo();
	TRect VideoPaneRect() const;
	
	void SetRunMode (TUint32 aMode);
	
	void PlayVideo();
	void PauseVideo();
	void StopVideo();
	void PlayComplete(TInt aError);
	void SetPos(TPoint* aPos);
private:
	CTestAppView();
	void ConstructL(const TRect& aRect);
	void SetVideoSize(CVideoPlayerUtility2& aVideo) const;
	void UpdateButtonState();
	void PlayPrevious();
	void PlayNext();
private: // from MCoeControlObserver
	void HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType);

private: // from CCoeControl
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);    
	void SizeChanged(); 
	void Draw(const TRect& aRect) const;

private:	
	TPoint iVidPos;
	TPoint iIncrement;
	TPoint iDirection;
	TInt32 iRunMode;
	
	TRect iWindowedViewRect;
	TSize iNativeVideoSize;
	TPoint iPointerDownPos;
	TTime iPointerDownTime;
	CVideoPlayerUtility2* iCurrentPlayer;
	TBool iIsPlaying;
private:
	CFbsBitmap* iBkgrdImage;
	CFbsBitmap* iBkgrdMask;
private:
	CVideoPane* iVideoPane;
	
	enum
		{
		KFourButtonControlID,
		KThreeButtonControlID,
		KVideoControlID
		};
	};

enum TButtonPaneAction
	{
	KButtonEventNotInit = -2,
	KButtonEventDoNothing = -1,
	KButtonEventDoPlayPause = 10,
	KButtonEventDoBackward,
	KButtonEventDoForward,
	KButtonEventDoOpenFile
	};


//************************************************************************************************************
//** CVideoPane class - place 'video' window
//************************************************************************************************************

class CVideoPane : public CCoeControl
	{
public:
	~CVideoPane();
	CVideoPane();
	void ConstructL(CCoeControl& aParent);
	RWindow& TheWindow() { return Window(); }
	const RWindow& TheWindow() const { return const_cast<CVideoPane*>(this)->TheWindow(); }
	TBool IsVideoPanePointerEvent();
	void BlackoutPane();
	void SetVideoUseRect(const TRect& aRect);
	void EnableSubtitle(CVideoPlayerUtility2* aSubtitlePlayer);
private: // from CCoeControl
	void SizeChanged();
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void Draw(const TRect& aRect) const;
private:
	TBool iIsVideoPanePointerEvent;
	TRect iVideoUseRect;
private:
	CVideoPlayerUtility2* iSubtitlePlayer;
	};

#endif
