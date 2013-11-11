// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TDIRECTA_H__
#define __TDIRECTA_H__

#include "AUTO.H"
#include <e32msgqueue.h>
#include <time.h>
#include "TGraphicsHarness.h"

//#define LOGGING

class CColorAnimation;
class CScrollText;
class CScrollingTextDrawer;
class CDirectScreenAccessOld;


struct TDirectThreadParam
	{
	TRect iRect;
	TInt iScreenNumber;
	TBool iIsInFront;
	};

class MAnimCallBacks
	{
public:
	virtual void LogLeave(TInt aErr)=0;
	virtual void Fail()=0;
	virtual void Finished(TInt aId)=0;
	virtual TDisplayMode DisplayMode(TInt aId)=0;
	virtual TRgb BrushColorL(TInt aId,TInt& aColor,TBool& aFinished)=0;
	virtual TInt TimerInterval(TInt aId)=0;
	virtual void FailedReStart(TInt aId,TInt aReason)=0;
	virtual TInt SlowStopping(TInt aId,TInt aCount)=0;
	virtual void Log(const TText8* aFile, TInt aLine, TInt aSeverity,const TDesC& aString)=0;
public:
	CTBlankWindow* iCallBackWin;
	};

class CTDirect : public CTWsGraphicsBase, public MAbortDirectScreenAccess, public MAnimCallBacks
	{
public:
	enum
		{
		eDirectNumFrames=10
		};
	enum
		{
		eDirectControlSem=0x01,
		eDirectWindowSem=0x02,
		eFontSet=0x04,
		eMultiAnim=0x08,
		};
public:
	CTDirect(CTestStep* aStep);
	~CTDirect();
	void ConstructL();
	TBool DrawFrame();
	TBool Restart();
	void ChangeToNextScreenModeL();
	void ChangeScreenScale();
	//The Tests
	TestState AnimateWindowL();
	TestState AnimationDiesL();
	TestState PackagingClassL();
	TestState MultipleL();
	TestState FailCodesL();
	TestState ScrolingText1L();
	TestState RClassL();
	TestState ScrolingText2L();
	TestState ScrolingText3L();
	TestState ScrolingText4L();
	TestState DefectFix_KAA_5J3BLW_L();
	TestState KillAnimationL();
	TestState DSAWithScreenModeOffset1L();
	TestState DSAWithScreenModeOffset2L();
	TestState WindowPoistionRelativeToScreenL();
	TestState ScreenModeScalingTestL();
	TestState ScreenModeTestForScalingL();
	TestState MultipleDSAsOnSameWindowL();
	TestState TemporaryDeadlockL();
	TestState RegionTrackingOnlyNotificationsL(TUint aId);
	TestState MixDsaAndRegionTrackingOnlyL(TBool aWhoExitsLast);
	TestState TryDifferentSupportedModesL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void ConstrucBlankWindowL();
	void ScanTypeFacesL();
	void InitialiseAnimationL();
	void DestroyAnimation();
	void ResetScreenSizeMode();
	void StartDirect();
	void SignalWindow();
	void ModeSwitch();
	void ScrolingTextL(TInt aId,TRect aWinRect,TBool aStartThread,TInt aScreenMode=0);
	void AnimTestL(TInt aId,TRect aWinRect,CTWinBase& aParent);
	//Pure virtual function from MAbortDirectScreenAccess
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
	//Pure virtual functions from MAnimCallBacks
	void LogLeave(TInt aErr);
	void Fail();
	void Finished(TInt aId);
	void DeleteMoveWindow();
	void DeleteScroll();
	TDisplayMode DisplayMode(TInt aId);
	TRgb BrushColorL(TInt aId,TInt& aColor,TBool& aFinished);
	TInt TimerInterval(TInt aId);
	void FailedReStart(TInt aId,TInt aReason);
	TInt SlowStopping(TInt aId,TInt aCount);
	void Log(const TText8* aFile, TInt aLine, TInt aSeverity,const TDesC& aString);
	void SetScreenMode(TInt aMode,TPixelsAndRotation& aPixelsAndRotation);
	void CreateAnimForScreenModeL(TInt aAnimIndex,CTWinBase& aParent,TRect aRect,TInt aId);
	void BlankTopClientWindowL(CTBlankWindow& aBlankWindow,TRect aRect);
	void TestDifferentOriginAndScaleL(TSizeMode &aMode,TPoint aOrigin);
	void CheckForTemporaryDeadlock();
private:
	CProcess* iProcess;
	CProcess* iMoveWin;
	RSemaphore iControlSem;
	RMsgQueueBase iControlQueue;
	RSemaphore iWinSem;
	TRequestStatus iThreadStatus;
	TUint iFlags;
	CTBlankWindow* iWin;
	CTBlankWindow* iWin2;
	CFbsScreenDevice* iScrDev;
	CFbsBitGc* iGc;
	CFont* iFont;
	CPeriodic* iTimer;
	CIdle* iRestart;
	CDirectScreenAccessOld* iDirect;
	RRegion* iDrawingRegion;
	TSize iScrSize;
	TRect iDrawRect;
	TInt iDrawHeight;
	TInt iFrameNo;
	TInt iLastTextWidth;
	TInt iLastMaxTextWidth;
	TInt iFrameToSignalAfter;
	TBool iDelay;
	TBool iCancelNext;
	TBool iWindowDead;
	CColorAnimation* iAnim;
	CColorAnimation* iAnims[13];
	TInt iCount;
	CScrollText* iScroll;
	TInt iCycles;
	TScreenModeEnforcement iModeBackup;
	TThreadStartUp iFirstFunction;
	TInt iNumThreadHandles;
	CTBlankWindow* iBlankTopClientWin1;
	CTBlankWindow* iBlankTopClientWin2;
	CPeriodic* iChangeScreenModeTimer;
	TInt iCurrentMode;
	TInt iNumOfModes;
	TPoint iCurrentScreenModeOrigin;
	TSize iCurrentScreenModeScale;
	TInt iNumAnimation;
	TSizeMode iModeData;
	CPeriodic* iScreenModeTimer;
	TInt iIsScalingSupported;
	TInt iNumOfCallBack;
	TBool iTestJustFailed;
	TBool iTestJustCompleted;
	TInt iState;
	TBool iNextFrameFinished;
	TBool iPackagingFinished;
	TBool iTimerRunning;
	};

class CDirectScreenAccessOld : public CActive
	{
public:
	static CDirectScreenAccessOld* NewL(RWsSession& aWs,MAbortDirectScreenAccess& aAborter);
	~CDirectScreenAccessOld();
	TInt Request(RRegion*& aRegion, RWindowBase& aWindow);
private:
	inline CDirectScreenAccessOld(RWsSession& aWs,MAbortDirectScreenAccess& aAborter)
		: CActive(RDirectScreenAccess::EPriorityVeryHigh), iDirectAccess(aWs), iAborter(aAborter) {}
	void ConstructL();
	//Pure virtual functions from CActive
	void DoCancel();
	void RunL();
private:
	RDirectScreenAccess iDirectAccess;
	MAbortDirectScreenAccess& iAborter;
	TBool iAborting;
	};

class CColorAnimation : public CBase, public MDirectScreenAccess
	{
public:
	enum
		{
		eStopNow=EFalse,
		eStopDelayed=ETrue,
		eAbort=2,
		eAbortAll,
		};
public:
	static TInt DrawColorL(TAny* aAnimation);
	static TInt IdlingL(TAny* aAnimation);
	static CColorAnimation* NewL(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack,CTWinBase& aParent,TRect aExtent,TBool aStart,TBool aRegionTrackingOnly = EFalse);
	inline CColorAnimation(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack) :iId(aId), iCallBack(aCallBack), iScreenNumber(aScreenNumber) {}
	~CColorAnimation();
	void ConstructL(CTWinBase& aParent,TRect aExtent,TBool aRegionTrackingOnly,TInt aTypeWindow=0,TInt aSingleWinForMultipleDSA=0);
	void StartL(TBool aChildWindow=EFalse);
	void StartOrPanic();
	void Stop();
	void BringWindowToFront();
	void ChangeModeL(TDisplayMode aMode);
	TBool inline IsTimerActive() {return iTimer->IsActive();}
	inline MAnimCallBacks& CallBack() { return iCallBack; }
	TPoint AbsoluteWindowPosition(TInt aWindowId=0);
	inline CDirectScreenAccess* GetDrawer();
	void FinishTest();
	TBool TestGcAndScreenDeviceValues();
	//inline TBool IsReadyToAbort(){return (iDrawer->iStatus!=KRequestPending);}
protected:
	void DrawColorL();
	void IdlingL();
	/*pure virtual function from MAbortDirectScreenAccess*/
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
	/*pure virtual function from MDirectScreenAccess*/
	void Restart(RDirectScreenAccess::TTerminationReasons aReason);
private:
	TInt iId;
	CTBlankWindow* iWindow;
	CTBlankWindow* iWindow2;
	TSize iWinSize;
	CDirectScreenAccess* iDrawer;
	CPeriodic* iTimer;
	MAnimCallBacks& iCallBack;
	TInt iColor;
	TInt iCount;
	TUint iIdling;
	TInt iScreenNumber;
	TInt iSingleWinForMultipleDSA;
	TBool iRegionTrackingOnly;
	};

class CScrollingTextDrawer : public CBase
	{
public:
	static CScrollingTextDrawer* NewL(TInt aScreenNumber,CFbsScreenDevice*& aDevice,CFbsBitGc& aGc);
	~CScrollingTextDrawer();
	inline void SetScrollJump(TInt aJump) {iJump=aJump;}
	void SetBottomOfTest(TInt aBottom);
	void Scroll();
private:
	inline CScrollingTextDrawer(CFbsScreenDevice*& aDevice,CFbsBitGc& aGc) : iDevice(aDevice), iGc(&aGc) {}
	void ConstructL(TInt aScreenNumber);
	void CreateFontL();
private:
	CFbsScreenDevice*& iDevice;
	CFbsScreenDevice* iFontDevice;
	CFbsBitGc* iGc;
	CFont* iFont;
	TBuf<32> iText;
	TRect iDrawRect;
	TInt iDrawHeight;
	TInt iCharWidth;
	TInt iFirstChar;
	TInt iJump;
#if defined(LOGGING)
	RWsSession iWs;
#endif
	};

class CScrollText : public CBase, public MDirectScreenAccess
	{
public:
	static TInt DrawText(TAny* aAnimation);
	static CScrollText* NewL(TInt aScreenNumber,TInt aId,CTWindowGroup& aParent,TInt aScrollJump,TBool aStart=EFalse);
	inline CScrollText(TInt aId,TInt aScrollJump,TInt aScreenNumber) :iId(aId), iScrollJump(aScrollJump), iScreenNumber(aScreenNumber) {}
	~CScrollText();
	void ConstructL(CTWindowGroup& aParent);
	void StartL();
	void Stop();
	void ContinueL();
	inline TBool IsRunning() {return iTimer->IsActive();}
	inline void SetCountDown(TInt aCountDown) {iAbortCountDown=aCountDown; iCounting=ETrue;}
	inline void SetBottomOfTest(TInt aBottom) {iTextDraw->SetBottomOfTest(aBottom);}
private:
	void ScrollText();
	void DoContinueL();
	/*pure virtual function from MAbortDirectScreenAccess*/
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
	/*pure virtual function from MDirectScreenAccess*/
	void Restart(RDirectScreenAccess::TTerminationReasons aReason);
private:
	TInt iId;
	CTBlankWindow* iWindow;
	TSize iWinSize;
	CDirectScreenAccess* iDrawer;
	CScrollingTextDrawer* iTextDraw;
	CPeriodic* iTimer;
	TInt iScrollJump;
	TBool iCounting;
	TInt iAbortCountDown;
	TInt iScreenNumber;
	};

	
/**
 A simple class based on CScrollText constructs a blank window that has
 a blank window child, used for functional coverage testing of CWsClientWindow
 */
class CWindowWithChild : public CBase, public MDirectScreenAccess
	{
public:
	static CWindowWithChild* NewL(TInt aScreenNumber, CTWindowGroup& aParent,TBool aStart=EFalse);
	inline CWindowWithChild(TInt aScreenNumber) : iScreenNumber(aScreenNumber) {}
	~CWindowWithChild();
	void ConstructL(CTWindowGroup& aParent);
	void StartL();
	void Stop();
	void ContinueL();
	inline TBool IsRunning() {return iRunning;}
	void PerformCoverageCalls();
	RWindow *ChildWindow()    {return iChildWindow->Win();}
private:
	void DoContinueL();
	/*pure virtual function from MAbortDirectScreenAccess*/
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
	/*pure virtual function from MDirectScreenAccess*/
	void Restart(RDirectScreenAccess::TTerminationReasons aReason);
private:
	CTBlankWindow* iWindow;
	CTWin * iChildWindow;
	TSize iWinSize;
	CDirectScreenAccess* iDrawer;
	TInt iScreenNumber;
	TBool iRunning;
	};
	
	
class CWsBase : public CBase
	{
public:
protected:
	~CWsBase();
	void ConstructL(TInt aScreenNumber,TInt aHandle);
	void CreateBlankWindowL(RBlankWindow& iWin,TInt aHandle);
protected:
	RWsSession iWs;
	CWsScreenDevice *iScrDev;
	RWindowGroup iGroup;
	};

class CAnimating : public CWsBase, public MAbortDirectScreenAccess
	{
public:
	static TInt StartLC(TAny* aScreenNumber);
	//static CAnimating* NewLC();
	void DrawFrame();
	/*pure virtual function from MAbortDirectScreenAccess*/
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
private:
	void ConstructL(TInt aScreenNumber);
	~CAnimating();
	void StartAnimatingL();
private:
	RSemaphore iControlSem;
	RMsgQueueBase iControlQueue;
	TBool iSemCreated;
	RBlankWindow iWindow;
	CFbsScreenDevice* iScrDev;
	CFbsBitGc* iGc;
	TSize iScrSize;
	CDirectScreenAccessOld* iDirect;
	RRegion* iDrawingRegion;
	CPeriodic* iTimer;
	TInt iColor;
	};

class CMoveWindow : public CWsBase
	{
public:
	static TInt StartLC(TAny* aScreenNumber);
	static TInt StateChange(TAny* aMoveWin);
	static TInt FlipChange(TAny*);
	//Call Backs
	void MoveWindow();
private:
	inline static CMoveWindow* Cast(TAny* aMoveWin) {return STATIC_CAST(CMoveWindow*,aMoveWin);}
	~CMoveWindow();
	void ConstructL(TInt aScreenNumber);
	void CreateTimerL();
	void StateChange();
	void ToggleDisplayMode();
	void FlipChange();
private:
	CPeriodic* iFlipTimer;
	CPeriodic* iStateTimer;
	CPeriodic* iTimer;
	CWsScreenDevice* iDevice;
	RBlankWindow iWindow;
	TDisplayMode iDisplayMode;
	RBackedUpWindow iBackUpWin;
	CWindowGc* iGc;
	TRect iBounceArea;
	TSize iScrSize;
	TPoint iTl;
	TSize iSize;
	TSize iDelta;
	TInt iStateCountDown;
	TInt iNumOfModes;
	TInt iCurrentMode;
	};

class CBugFixColorAnimation : public CColorAnimation
	{
public:
	static CBugFixColorAnimation* NewL(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack,CTWindowGroup& aParent,TRect aExtent,TBool aStart=EFalse);
	~CBugFixColorAnimation();
	inline void Started() { iSem.Signal(); }
private:
	CBugFixColorAnimation(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack);
	void ConstructL(CTWindowGroup& aParent,TRect aExtent);
private:
	/*pure virtual function from MAbortDirectScreenAccess*/
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
	/*pure virtual function from MDirectScreenAccess*/
	void Restart(RDirectScreenAccess::TTerminationReasons aReason);
private:
	CProcess* iThread;
	CIdle* iTestFailed;
	TRect iAnimRect;
	TDirectThreadParam iThreadParam;
	RSemaphore iSem;
	};

class CRegionTrackingOnly : public CColorAnimation
	{
public:
	static CRegionTrackingOnly* NewL(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack,CTWindowGroup& aParent,TRect aExtent,TBool aStart,TBool aOpenWindowInFrontDsa);
	void ConstructL(CTWindowGroup& aParent,TRect aExtent,TBool aOpenWindowInFrontDsa);
	~CRegionTrackingOnly();
	inline void Started() { iSem.Signal(); }
private:
	CRegionTrackingOnly(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack);
	/*Overriding of bases class virtual functions*/
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
	void Restart(RDirectScreenAccess::TTerminationReasons aReason);
private:
	CProcess* iThread;
	TRect iAnimRect;
	TBool iExpectedToAbort;	
	TDirectThreadParam iThreadParam;
	RSemaphore iSem;
	};

/*CPanicDirect*/
 class CPanicDirect : public CBase
 	{
 public:
	static TInt DoTestOnNewScheduler(TInt aInt, TAny* aPtr);
	static TInt DoTestOnNewSchedulerL(TInt aInt, TAny* aPtr);
 private:
 	inline CPanicDirect() {}
 	~CPanicDirect();
 	void ConstructL(TInt aScreenNumber, TInt aInt);
	static void DoTestNowL(TInt aInt, TAny* aPtr);
 	void TestL();
 	void DoDrawingL();
private:
	TInt iTestNo;
	RWsSession iWs;
	CWsScreenDevice* iWsScrDev;
	RWindowGroup iGroup;
	RBlankWindow iBlankWin;
	RDirectScreenAccess iDirect;
	TRequestStatus iDirectStatus;
	RRegion* iRegion;
	RTimer iTimer;
	TRequestStatus iTimerStatus;
	TBool iDrawingAllowed;
	CFbsScreenDevice* iScreenDevice;
	CFbsBitGc* iGc;
	TDisplayMode iDisplayMode;
	CScrollingTextDrawer* iTextDraw;
	TInt iCount;
	TInt iScreenNumber;
	};

class CTDirectStep : public CTGraphicsStep
	{
public:
	CTDirectStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTDirectStep,"TDirect");


#endif
