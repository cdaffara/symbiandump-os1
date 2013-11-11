// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for window server auto test code
// 
//


#ifndef __AUTO_H__
#define __AUTO_H__

#include <e32std.h>
#include <e32math.h>
#include "../../SERVER/w32cmd.h"
#include <w32std.h>
#include "../tlib/testbase.h"
#include <wsautotest.mbg>
#include "TGraphicsHarness.h"
#include "../../SERVER/openwfc/panics.h"

#define DISABLE_FAIL_DIALOG

#define TEST_BITMAP_NAME _L("Z:\\WSTEST\\WSAUTOTEST.MBM")

#define SCREEN_MODE_TESTING

#define LOG_MESSAGE(p) LogMessage(((TText8*)__FILE__), __LINE__,(p))
#define LOG_MESSAGE2(p1,p2) LogMessage(((TText8*)__FILE__), __LINE__,(p1),(p2))
#define LOG_MESSAGE3(p1,p2,p3) LogMessage(((TText8*)__FILE__), __LINE__,(p1),(p2),(p3))
#define LOG_MESSAGE4(p1,p2,p3,p4) LogMessage(((TText8*)__FILE__), __LINE__,(p1),(p2),(p3),(p4))
#define LOG_MESSAGE5(p1,p2,p3,p4,p5) LogMessage(((TText8*)__FILE__), __LINE__,(p1),(p2),(p3),(p4),(p5))
#define LOG_MESSAGE6(p1,p2,p3,p4,p5,p6) LogMessage(((TText8*)__FILE__), __LINE__,(p1),(p2),(p3),(p4),(p5),(p6))
#define LOG_MESSAGE7(p1,p2,p3,p4,p5,p6,p7) LogMessage(((TText8*)__FILE__), __LINE__,(p1),(p2),(p3),(p4),(p5),(p6),(p7))

// MB 23\11\04 The heap size was only 4k which was causing DSA step 7 to fail on HW.
// Increased heap size to 8k
// 23/02/2009 Again failing with no memory due to COLOR16MAP used. 
// Increased heap size to 16k
LOCAL_D const TUint KOtherProcHeapSize=0x4000;
LOCAL_D const TInt KAutoTestPriority=-20;

TBool DoCheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect);
TBool DoCheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect, TUint aFlags);
TBool DoCheckRectRWin(RWindowBase &aWin1,RWindowBase &aWin2,const TRect &aRect);
TBool DoCheckRectRWin(RWindowBase &aWin1,RWindowBase &aWin2,const TRect &aRect, TUint aFlags);
TBool DoCheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2);
TBool DoCheckRect(TPoint aPos1,TPoint aPos2,TSize aSize);

_LIT(KWSERV,"WSERV");
_LIT(KW32,"W32");
_LIT(KPlatsecBegin,"*PlatSec* ERROR - BEGIN NEGATIVE PLATSEC TESTING");
_LIT(KPlatsecEnd,"*PlatSec* ERROR - END NEGATIVE PLATSEC TESTING");
_LIT(KTestFontTypefaceName,"DejaVu Sans Condensed");

class TestClient;
class CMinWin;
class CTWsGraphicsBase;

enum TestState
	{
	EInvalid,
	ENext,
	EWait,
	EFinished
	};

enum TAutoPanics
	{
	EAutoPanicTestFailed,
	EAutoPanicPanicFailed,
	EAutoPanicTestRectsIndex,
	EAutoPanicTestRectsSection,
	EAutoPanicUnexpectedLeave,
	EAutoPanicTestBrushesIndex,
	EAutoPanicWindowType,
	EAutoPanicTooManyDownKeys,
	EAutoPanicInvalidEvent,
	EAutoFadeWindow,
	EAutoPanicWrongTest,
	EAutoPanicAnimError,
	ETManPanicPcFailed,		              //Panic use in a test copied from TMan
	EAutoPanicTestError,	              //Test returned bad return type
	EAutoPanicDirect,		              //Direct Screen Access logic state error
	EAutoPanicScanCapital,	              //Scan codes must not be lower case letters
	EAutoPanicRedrawStoring,              //Inconsistent Redraw Storing Behaviour
 	EAutoPanicRecalledCreateTestBaseL,    //Called CreateTestBaseL twice
 	EAutoPanicHeartBeatCursorRect,        //Invalid size of rectangle in ChangeTransAndCheckHeartBeat
	EAutoPanicGroupWinHasChild,           //Main test group window has child at start of test
  	EAutoPanicPanicSetTrans,              //Error setting transparency during panic tests
  	EAutoPanicNoDigitiser,                //Pointer event when the configuration has no digitiser
	};

enum TPanicExitReasons
	{
	EWsExitReasonBad,
	EWsExitReasonFinished,
	};

enum TAutoLeaves
	{
	ETestPassed=0,
	ETestFailed=1,
	};

typedef TInt (*TPanicFunction)(TInt aInt, TAny *aPtr);

struct SPanicParams
	{
	TInt num;
	TPanicFunction func;
	TAny *ptr;
	};

class LogWindow : public CTWin
	{
	enum
		{
		EDummyValue=-9999
		};
public:
	LogWindow();
	void ConstructL(CTWinBase& aParent);
	void Draw();
	void DrawSubTitle();
	void DrawMessage();
	void LogTest(const TDesC &aTitle,TInt aNum);
	const TDesC& LogSubTest(const TDesC &aTitle,TInt aNum);
	const TDesC& LogMessage(TBool aLog,const TDesC& aText,TInt aNum=EDummyValue);
private:
	TBuf<0x40> iTestTitle;
	TBuf<0x40> iSubTitle;
	TBuf<0x40> iMessage;
	TInt iTitleHeight;
	};

class CTestBase;

/** 
This class is a window that is used to fake a border around the TestWindow defined below.
It simply draws an opaque black window on top of which the TestWindow will be drawn giving the illusion
there is a border.

@see TestWindow
@internalComponent
@released
*/
class CBorderWindow : public CTWin
	{
public:
	CBorderWindow();
	void ConstructL(CTWinBase& aParent);
	void Draw();
	};

class TestWindow : public CTWin
	{
public:
	TestWindow();
	~TestWindow();
	void Draw();
	void SetUpL(TPoint aPos, TSize aSize, CTWinBase* aParent, CWindowGc& aGc);
	void ClearWin();
	void SetBorderExt(TPoint aPos, TSize aSize);
	CBorderWindow* GetBorderWin();
private:
	CBorderWindow* iBorderWin;
	};

/**
This class used to run each individual test before the migration to TEF.
Now it doesn't do much and should probably be refactored.

@internalComponent
@released
*/
class CTestDriver : public CBase
	{
public:
	/** Factory function that returns a class derived from CTestDriver */
	static CTestDriver* CreateL(TInt aScreenNumber);
	virtual ~CTestDriver();
	
protected:
	CTestDriver(TInt aScreenNumber);
	
public:
	virtual void DoDisplayDialog(TDesC& timeBuf,TDesC& testReport) = 0;

	void TestComplete2();
	void DestroyTest();

	inline TInt ScreenNumber() {return iScreenNumber;}
	inline TInt NumberOfScreens() {return iNumberOfScreens;}

public:
	CTestBase *iTest;
	TInt iSubTestNum;
	static TInt iTestNum;
protected:
	TTime iStartTime;
	TInt iScreenNumber;
	TInt iTestSize;
	TInt iNumberOfScreens;
	};

/** Variant of CTestDriver used when the tests are run on the primary screen.

@internalComponent
@released
*/
class CTestDriverPrimary: public CTestDriver
	{
public:
	CTestDriverPrimary(TInt aScreenNumber);
	void DoDisplayDialog(TDesC& timeBuf,TDesC& testReport);
	};

/** Variant of CTestDriver used when the tests are run on a secondary screen.

@internalComponent
@released
*/
class CTestDriverSecondary : public CTestDriver
	{
public:
	CTestDriverSecondary(TInt aScreenNumber);
	void DoDisplayDialog(TDesC& timeBuf,TDesC& testReport);
	};

/** List of rectangles use by TGDI.CPP and TRECTLIST.CPP.

Since only 2 tests make use of this shouldn't it be moved outside of the framework?

@internalComponent
@released
*/
class TTestRects
	{
private:
	enum TRectListLimits
		{
		EMaxRectFromList=64,
		EMaxRectZeroSize=73,
		EMaxRectWide=83,
		EMaxRectHigh=93
		};
		
public:
	void Construct(const RWindow &aWindow);
	TInt Count1() const;
	TInt Count2() const;
	TInt Count3() const;
	TRect operator[](TInt aIndex);
	static void ResetSeed();
private:
	TInt Rnd(TInt aSize);
	TInt RndMax(TInt aSize);
	TInt RectCoord(TInt aSection,TInt aSize);
	void RectCoordPair(TInt &aTl, TInt &aBr, TInt aSection, TInt aSize);
	
private:
	TSize iSize;
	static TRect iList[64];
	static TInt64 iSeed;
	};

/**
@internalComponent
@released
*/
class CBitmap : public CBase
	{
public:
	static CBitmap* NewLC(const TSize& aSizeInPixels,TDisplayMode aDispMode);
	static CBitmap* NewL(const TSize& aSizeInPixels,TDisplayMode aDispMode);
	static CBitmap* NewL(TInt aHandle);
	static CBitmap* NewL(TDesC& aFileName);
	~CBitmap();
	inline CFbsBitmap& Bitmap() const {return *iBitmap;}
	inline CFbsBitmapDevice& Device() const {return *iDevice;}
	inline CFbsBitGc& Gc() const {return *iGc;}
private:
	static CBitmap* NewLC(TInt aHandle,const TSize& aSizeInPixels,TDisplayMode aDispMode);
	void ConstructL(TInt aHandle,const TSize& aSizeInPixels,TDisplayMode aDispMode);
private:
	CFbsBitmap* iBitmap;
	CFbsBitmapDevice* iDevice;
	CFbsBitGc* iGc;
	};

/*TPartialRedrawType*/
enum TPartialRedrawType
	{
	//redraw store type is not yet determined.
	EPartialRedraw_Unknown,
	//redraw store is discarded on invalidation and begin-redraw on a smaller rect.
	EPartialRedraw_None,
	// redraw store is preserved on invalidation and begin-redraw on a smaller rect.
	EPartialRedraw_PreserveStoredCmds,
	// redraw store is appended with partial redraw commands, and old redraw store is suitably modified.
	EPartialRedraw_FullRedrawSupport
	};

class CTestBase : public CBase
	{
public:
	enum TPriorites
		{
		EAutoTestPriority=KAutoTestPriority
		};
	enum TTestLevel
		{
		ELevelQuick,		// Quick check on main cases for each function
		ELevelNormal,		// Suitable test level for internal release
		ELevelRelease,		// Thorough test suitable for official releases
		ELevelMega,			// Really OTT, takes forever to run, leave it going over the weekend sort of tests
		};
public:
	CTestBase(const TDesC &aTitle,CTWsGraphicsBase* aTestBase);
	~CTestBase();
	void StartTestL(TInt aNum, CTestDriver* aDriver, TTestLevel aLevel);
	void LogSubTest(const TDesC &aTitle);
	void LogMessage(TInt aValue);
	void LogSubState(TInt aSubState);
	TInt LaunchPanicThread(RThread &aThread, SPanicParams *aPtr);
	TInt TestPanicL(SPanicParams *aPtr, TInt aExitReason, const TDesC &aCategory, TBool* aTestFinished=NULL);
	TInt TestPanicL(TPanicFunction aFunction, TInt aExitReason, TInt aInt, TAny *aPtr, const TDesC &aCategory, TBool* aTestFinished=NULL);
	TInt TestWsPanicL(TPanicFunction aFunction, TClientPanic aExitReason, TInt aInt, TAny *aPtr=NULL, TBool* aTestFinished=NULL);
	TInt TestW32PanicL(TPanicFunction aFunction, TW32Panic aExitReason, TInt aInt, TAny *aPtr, TBool* aTestFinished=NULL);
	TInt TestWservPanicL(TPanicFunction aFunction, TWservPanic aExitReason, TInt aInt, TAny *aPtr);
	void TriggerFail();
	static void DrawTestBackground(TBool aInvertColours, const TSize &aSize, TInt aGrays=16);
	inline CTestDriver* Driver();
	void SimulatePointer(TRawEvent::TType aType, TInt aX, TInt aY);
	TBool ConfigurationSupportsPointerEventTesting() const;
	void SimulateEvent(TRawEvent::TType aType);
	void LogColors(const CBitmapDevice& aDevice,TPoint aBasePoint, TPoint aStartPoint, TPoint aEndPoint);
	void LogColors4(const CBitmapDevice& aDevice,TPoint aStartPoint,TInt aLen);
	inline TInt ScreenNumber() {return iScreenNumber;}
	TPartialRedrawType RedrawStoreTypeL();
	TBool IsFullRomL();
	void SimulateKeyDownUpWithModifiers(TInt aScanCode,TUint aModifiers);
	void SimulatePointerDownUp(TInt aX, TInt aY);
	void SimulateKeyDownUp(TInt aScanCode);
	void SimulateKey(TRawEvent::TType aType, TInt aScanCode);
	void DelayIfFullRomL();
	void SetUpMember(TSpriteMember &aMember);
	void AbortL();
	void LogLeave(TInt aErr);
	void CloseAllPanicWindows();
	const TSize& StdTestWindowSize();
	TInt MaxGrays() const;
	TInt MaxColors() const;
	TInt SaveScreen(const TDesC& aFileName);
	TInt SaveScreen(const TDesC& aFileName,const TSize& aScreenSize,TDisplayMode aColorDepth);
protected:
	void UpdateTestResults(TInt aNoOfTest, TInt aNoOfTestPass);
	void SaveScreenL(const TDesC& aFileName,const TSize& aScreenSize,TDisplayMode aColorDepth);
public:
	static TInt iNumberTestsPass;
	static TInt iNumberTests;
	TInt iState;
	TInt iScreenNumber;
	static TInt iScreenNo;
	TTestRects iTestRects;
	static TRect iNormalPointerCursorArea;
	TBuf<0x40> iSubTitle;
	TTestLevel iTestLevel;
	static TInt iNumberOfGrpWndsOnPrimaryScreenWithZeroPriority; // Will be used in TGWHANDLE test.
protected:
	CTestDriver* iDriver;
private:
	TBuf<0x40> iTitle;
	CTWsGraphicsBase* iTestBase;
	/** 
	Several tests use 3 windows : one is a log window, one is a reference window 
	and one is the actual output of the test. All these windows have the same width which is roughly
	1/3 of the screen. They also have the same height which is roughly equal to the screen height.
	*/
	TSize iStdTestWindowSize;
	CMinWin* iMinWin;
	TInt iTestNum;
	TBool iFail;	// Used to trigger failures to test test code.
	TInt iThreadNumber;
	/** 
	The type of redraw store in use. Static members so that	RedrawStoreTypeL computes
	this value only once and then returns the cached result as this remains the same for
	the entire test run.
	*/
	static TPartialRedrawType iRedrawType;
	/** The number of greys available in the richest grey mode. */
	static TInt iMaxGrays;
	/** The number of colours available in the richest supported colour mode. */
	static TInt iMaxColors;
	};

class CTWsGraphicsBase : public CTGraphicsBase
	{
public:
	CTWsGraphicsBase(CTestStep* aStep);
	~CTWsGraphicsBase();
	void CreateTestBaseL(CTTMSGraphicsStep* aTmsStep);
	inline CTestBase *TestBase() const {return(iTest);};
	void TestComplete();
	void LogMessage(const TText8* aFile,TInt aLine,TRefByValue<const TDesC> aFmt,...);
	TInt GetScreenFromIni() const;
protected:
	void CheckRect(TPoint aPos1,TPoint aPos2,TSize aSize, const TDesC *aErrorMsg=NULL);
	void CheckRect(TPoint aPos1,TPoint aPos2,TSize aSize, const TDesC &aErrorMsg);
	void CheckRectNoMatch(TPoint aPos1,TPoint aPos2,TSize aSize, const TDesC *aErrorMsg=NULL);
	void CheckRectNoMatch(TPoint aPos1,TPoint aPos2,TSize aSize, const TDesC &aErrorMsg);
	void CheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2, const TDesC *aErrorMsg=NULL);
	void CheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2, const TDesC &aErrorMsg);
	void CheckRectNoMatch(CTBaseWin *aWin1,CTBaseWin *aWin2, const TDesC *aErrorMsg=NULL);
	void CheckRectNoMatch(CTBaseWin *aWin1,CTBaseWin *aWin2, const TDesC &aErrorMsg);
	TBool CheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect, const TDesC *aErrorMsg=NULL);
	TBool CheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect, const TDesC &aErrorMsg);
	void CheckRectNoMatch(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect, const TDesC *aErrorMsg=NULL);
	void CheckRectNoMatch(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect, const TDesC &aErrorMsg);
	TBool CompareWindows(const TDesC *aErrorMsg=NULL);
	TBool CompareWindows(const TDesC &aErrorMsg);
	void CompareWindows(const TRect &aRect, const TDesC *aErrorMsg=NULL);
	void CompareWindows(const TRect &aRect, const TDesC &aErrorMsg);
	void CompareWindowsSoftFailWinscw(const TText8* aFile, TInt aLine);
	const TSize& StdTestWindowSize();
	TInt MaxGrays() const;
	TInt MaxColors() const;
private:
	TBool CheckRetValue(TBool aPass,const TDesC* aErrorMsg,const TDesC& aErrorFunction);
protected:
	CTestBase* iTest;
	CTTMSGraphicsStep* iTmsStep;
	};

class TestWindowGroup : public CTWindowGroup
	{
public:
	TestWindowGroup(CTClient *aClient);
	void BecomeOwning();
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void KeyDownL(const TKeyEvent &aKey,const TTime &aTime);
	void KeyUpL(const TKeyEvent &aKey,const TTime &aTime);
	};

class TestClient : public CTClient
	{
public:
	TestClient();
	~TestClient();
	void ConstructL();	// virtual from CTClient
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void Shutdown();
	inline CTestDriver* Driver();
	TBool WaitForEvent();
	TBool IsEventWaiting();
	void LogAvailableScreenModeL();
	void TestWsSetBufferSizeL();
	void TestWsSetMaxBufferSizeL();
	inline LogWindow& StdLogWindow() {return *iStdLogWindow;}
	void SetTestClientScreenMode(TInt aMode);
	void UpdateTestClientScreenMode();
public:
	RArray<TInt> iScreenModes;
	
private:
	/**
	Several tests use 3 windows : one is a log window, one is a reference window 
	and one is the actual output of the test. This member is the log window.
	*/
	LogWindow* iStdLogWindow;
	CTestDriver* iDriver;
	};

class TThreadStartUp
	{
public:
	TThreadStartUp() :iInitFunction(0), iParam(0) {}
	TThreadStartUp(TThreadFunction aInitFunction,TAny* aParam) :iInitFunction(aInitFunction), iParam(aParam) {}
public:
	TThreadFunction iInitFunction;	   //The call to this fn will prepare a cleanup stack, but this function should not leave.
	TAny* iParam;
	};

#if defined(__WINS__)
	const TOwnerType KOwnerType=EOwnerThread;
#else
	const TOwnerType KOwnerType=EOwnerProcess;
#endif

typedef TBuf<32> TBufArg;

class CProcess : public CBase
	{
public:
	enum
		{
		eProcessPriortyTest,
		eProcessKeyTest,
		eProcessDirectTest,
		eProcessMultiDisplayTest,
		eNumProcessCalls		//Must be last one
		};
	struct TInitialFunction
		{
		inline TInitialFunction(const TDesC& aParam,TThreadFunction aFunction) :iParam(aParam), iFunction(aFunction) {}
		TBuf<16> iParam;
		TThreadFunction iFunction;
		};
public:
	static TInt Start(const TWinCommand& aParam);
	static CProcess* NewL(TInt aFunctionNo,TInt aScreenNumber=KDefaultScreen);
	static CProcess* NewTL(TInt aFunctionNo,TInt aScreenNumber=KDefaultScreen,TRequestStatus* aClientRequestStatus = NULL);
	static CProcess* NewThreadL(const TDesC& aName,TThreadFunction aFunction,TThreadStartUp* aPtr
																							,TRequestStatus* aStatus);
	static CProcess* NewThreadL(const TDesC& aName,TThreadStartUp* aPtr);
	static CProcess* NewThreadRendezvousL(const TDesC& aName,TThreadFunction aFunction,TThreadStartUp* aPtr,TRequestStatus* aLogonStatus,TRequestStatus& aRendezvousStatus);
	static CProcess* NewThreadRendezvousL(const TDesC& aName,TThreadStartUp* aPtr,TRequestStatus& aRendezvousStatus);

	static CProcess* NewSimpleThreadL(const TDesC& aName,TThreadStartUp* aPtr,TRequestStatus* aStatus=NULL);
	void ConstructL(TInt aFunctionNo,TInt aScreenNumber=KDefaultScreen);
	void ConstructTL(TInt aFunctionNo,TInt aScreenNumber=KDefaultScreen,TRequestStatus* aStatus=NULL);
	void ConstructL(const TDesC& aName,TThreadFunction aFunction,TThreadStartUp* aPtr,TRequestStatus* aStatus);
	void ConstructRendezvousL(const TDesC& aName,TThreadFunction aFunction,TThreadStartUp* aPtr,TRequestStatus* aLogonStatus,TRequestStatus& aRendezvousStatus);

	~CProcess();
	void Logon(TRequestStatus& aStatus) const;
	void Terminate(TInt aReason);
	TBool StillAlive();
	void LeaveIfDied();
	static TBool ProcessDied(TInt aScreenNo=KDefaultScreen);
private:
	enum
		{
		eOtherCreated=0x0001,
		eThreadCreated=0x0002,
		};
private:
	static TInt ThreadInit(TAny *aPtr);
	static void InitialiseL(TThreadStartUp* aPtr);
	static TInt SimpleThreadInit(TAny *aPtr);
	void Close();
	static void GetProcArg(const TWinCommand& aParam,TBufArg& aProcArg);
	static void GetScreenArg(const TWinCommand& aParam, TInt& aScreenArg);

private:
	TUint iCreated;
	RProcess iOther;
	RThread iThread;
	static TInitialFunction iFunctions[eNumProcessCalls];
	};

GLREF_D TestWindow* BaseWin;
GLREF_D TestWindow* TestWin;
GLREF_D TestClient* TheClient;
GLREF_D CWindowGc* TheGc;

GLREF_C void AutoPanic(TInt aPanic);
GLREF_C void CleanUpWindow(TAny *aWindow);
GLREF_C void PushWindowL(RWindowTreeNode *aWindow);
GLREF_C TBool OpacityAndAlphaSupportedL();
GLREF_C TInt TransparencySupportedL();
GLREF_C TInt CheckScalingSupportedOrNot();
GLREF_C TBool CheckNonZeroOriginsSupportedOrNot();
GLREF_C TPoint PhysicalToLogical(TPoint aPhysicalPtMinusOrigin,TSize aScale);
GLREF_C TPtrC DisplayModeAsString(TDisplayMode aMode);
GLREF_C TBool LossyCheckBlankBitmap(const CFbsBitmap& aBitmap, const TRect aArea, const TRgb aCheckColor, TBool aLossyCompare=ETrue);
GLREF_C TBool LossyCompareBitmap(const CFbsBitmap& aBitmap1, const CFbsBitmap& aBitmap2, const TRect aCompareRect, TBool aLossyCompare=ETrue);
GLDEF_C TBool LossyCompareBitmapRecord(CFbsBitmap& aBitmap1, CFbsBitmap& aBitmap2, const TRect aCompareRect, TBool aLossyCompare, TInt& aPixelsDifferent, CTestExecuteLogger& aLogger);
GLREF_C TBool LossyCheckBlankWindow(const CWsScreenDevice& aScreen, CFbsBitmap& aBitmap, const TRect aArea, const TRgb aCheckColor);
GLREF_C TBool LossyCompareWindow(const CWsScreenDevice& aScreen, CFbsBitmap& aBitmap1, CFbsBitmap& aBitmap2, const TRect aCompareRect);

#define LogText(S) {_LIT(String__LINE__,S); Logger.Log(String__LINE__);}
#define LogFormat(S) {Logger.Log(S);}

inline CTestDriver* CTestBase::Driver()
	{
	return iDriver;
	}

class CTransWindow : public CBlankWindow
	{
public:
	static CTransWindow* NewL(RPointerArray<CTransWindow>& aWindows,TRgb aColor,TRect aPos);
	static CTransWindow* NewL(CTWinBase* aParent,TRgb aColor,TRect aPos,TDisplayMode* aDisplayMode);
	static CTransWindow* NewL(CTWinBase* aParent,TRgb aColor,TDisplayMode* aDisplayMode);
	void ToggleVisibility();
	static void SetOrdinal(RPointerArray<CTransWindow>& aWindows,TInt aOldPos,TInt aNewPos);
	inline TRgb Color() {return iCol;}
	inline TBool IsVisible() {return iVisible;}
	void SetShadowDisabled(TBool aState);
	inline TBool IsShadowing() {return iShadowing;}
	inline TBool ShadowHeight() {return iShadowHight;}
	inline void SetPosOffset(TPoint aOffset) {iPosOffset=aOffset;}
	virtual void Draw();
	void SetDrawOpaque(TBool aDrawOpaque) { iDrawOpaque = aDrawOpaque; }
	TBool DrawOpaque() const { return iDrawOpaque; }
	//Virtual functions from CTWinBase overridden in CTBaseWin
	void AdjustShadow(TInt aAdjust);
	TPoint Position() const;
	inline TInt TransparentError() {return iError;}
private:
	CTransWindow(TRgb aColor) :CBlankWindow(aColor), iVisible(ETrue) {}
	//Virtual function from CTBaseWin
	void InitWin();
private:
	TBool iVisible;
	TBool iShadowing;
	TBool iDrawOpaque;
	TInt iShadowHight;
	TPoint iPosOffset;
	TInt iError;
	};

class CMinWin : public CBase
	{
public:
	CMinWin(TInt aScreenNo);
	~CMinWin();
	void ConstructL();
	void Draw(const TRect& aRect);
private:
	RWsSession iWs;
	RWindow iWin;
	RWindowGroup iGroup;
	CWsScreenDevice* iScr;
	TInt iScreenNo;
	TRect iRect;
	CWindowGc* iGc;
	};

class CTimeOut : public CTimer
	{
public:
	inline CTimeOut(): CTimer((EPriorityLow+EPriorityIdle)/2) {}
	void ConstructL();
	void Start(TTimeIntervalMicroSeconds32 aInterval,TCallBack aCallBack);
	//Pure virtual from CActive
	void RunL();
private:
	TCallBack iCallBack;
	};

#define COMPARE_WINDOWS_SOFTFAIL_WINSCW	CompareWindowsSoftFailWinscw((TText8*)__FILE__, __LINE__)

#define __WS_CONSTRUCT_STEP__(a) \
	CT##a##Step::CT##a##Step() \
	{ \
	SetTestStepName(KT##a##Step); \
	SetLogHeapInfo(ETrue); \
	} \
	 \
CTGraphicsBase* CT##a##Step::CreateTestL() \
	{ \
	CTWsGraphicsBase* test=new (ELeave) CT##a (this); \
 	test->CreateTestBaseL(this);\
	return test;\
	} 

#endif
