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

#ifndef __TREDRSTR_H__
#define __TREDRSTR_H__

#include <e32std.h>
#include <w32std.h>
#include "../../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"
#include "TScreenModeScaling.h"


class CRedrawStoreWin;
class CNoDrawWin;
class CBitmapMaskedWin;
class CResetRedrawStoreWin;


class CTRedrawStoring : public CTWsGraphicsBase
	{
public:
	CTRedrawStoring(CTestStep* aStep);
	~CTRedrawStoring();
	void ConstructL();

	void CheckWindowsMatch();
	void CheckWindowsMatchLossy();
	void CheckWindowsNotMatch();
	void HideRevealTest();
	void MultipleHideReveal(TInt aX,TInt aY);
	void RedrawWindows();
	void DoDrawingL(CWindowGc* aWinGc);
	void DoDrawingL(TInt aDraw,CBitmapContext* aGc,TBool aWinGc);
	void ScrollTest();
	void FadeWindowTest();
	void FadeWindowTest2L();
	void DoDrawTest();
	void DoScrollTest();
	void SetScrolling(TPoint aScrollSource, TRect aScrollTarget);
	//Pure Virtual Function from CTestBase
	void DoNothingInRedrawTest();
	void DoDisableRedrawStoreTest();
	void DoBitBltAndMaskedNegTestsL();
	void DoResizeTest();
	void DoTestDrawBitmapMaskedL(TBool aWsBitmap=EFalse);
	void DrawBitmapAndCheckL(const TSize aSize,TDisplayMode aDisplayMode,CFbsBitmap* aSrceBitmap,CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	void DoFontCacheOverflowTestL();
	void FontCacheOverflowDrawingTestL(TBool aDiffGc, RWindow& aWindow);
	void DoInvisibleRedrawStoreTestL( TBool aUseTransparency );
	void DoBrushDrawTestL();
	void DoPartialDrawNowTestL( TBool aUseTransparency );
	void DoBeginEndRedraw();
	void DoRedrawStoreAlphaChannelTransTest();

	void DoExposeTestL(TInt aIteration);
	void DoExposeTest2L(TInt aIteration);
	void DoEmptyDrawTestL(TInt aTestMode);
	void DoPolygonRedrawTestSetL();
	void DoPolygonRedrawTestL(TInt aWindowMode, TInt aTestMode);
	void DoRedrawOOMTestL();
	void RedrawStoreWithBadRectL();
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
	void DoAutoResetRedrawStoreTestL(TBool aTwoWins, TBool aAnimateBothWins, TBool aKeepGcActive, TBool aUpdateInRedraw);
	void AutoResetRedrawStoreTestsL();
	void RedrawStoreWithSetExtentL();
	void PartialRedrawWithEmptyRedrawStoreL();
	void DoPartialRedrawWithEmptyRedrawStoreL(TInt aNumWins, TBool aDoWinOnTop, TBool aRedrawWindow, TBool aChildWindows);
	CResetRedrawStoreWin* CreatePartialRedrawWinLC(const TPoint& aPos, const TSize& aSize, CTWin* aParent=NULL);
	CNoDrawWin* CreateNoDrawWinLC(const TPoint &aPos, const TSize &aSize);
	void GetTestWinSizeAndPos(TInt aWinIndex, TPoint &aPos, TSize &aSize) const;
	void ScrollWinTest();
public:
	TInt iDrawOrder;
	TBool iQueueTest;

private:
	enum TDrawingMode
		{
		EServerRedraw,
		EClientRedrawsNormal,
		EClientRedrawsScrolled,
		};
	TPoint ComputeTextPosition(TPoint aPoint);
	CTBackedUpWin* iCheckWin;
	CFbsBitmap* iCheckBitmap;
	CFbsBitmapDevice* iCheckDevice;
	CFbsBitGc* iCheckGc;
	CRedrawStoreWin* iTestWin;
	TSize iWinSize;
	TPoint iWinPos;
	TDrawingMode iDrawMode;
	RBlankWindow iBlankWin;
	RRegion iRegion;
	RWindow iWinTestGc;
	TBool iWindowsFaded;
	TRect iScrollTarget;
	TPoint iScrollSource;
	TBool iDoScrollTest;
	CNoDrawWin* iNoDrawWin;
	TBool iClientDidDraw;
	CWsBitmap* iAlphaBitmap[3];
	TBool iAlphaSupported;
	CBitmapMaskedWin* iBitmapMaskedWin;
	CBitmap* iTestBitmap;
	CBitMapWin* iTestBitmapWin;
	TRect iWinRect;
	TPoint iTestWinPoint;
	TBool iXPlus;
	TBool iYPlus;
	TInt iState;
	TInt iYPoz;
	TDisplayMode iTestDisplayMode;
	CFbsBitmap *iTestWinCopy;
	CFbsBitmap *iCheckWinCopy;
	};

class CRedrawStoreWin : public CTWin
	{
public:
	inline CRedrawStoreWin(CTRedrawStoring* aTest) : iDrawOrder(0), iTest(aTest){}
	//Virtual Function from CTBaseWin
	void Draw();
public:
	TInt iDrawOrder;
private:
	CTRedrawStoring *iTest;
	};

class CNoDrawWin : public CTWin
	{
public:
	void Draw();
	};

class CBrushDrawWin : public CTWin
	{
public:
	CBrushDrawWin();
	void Draw();
	void Redraw();

private:
	};

class CBitmapMaskedWin : public CTWin
	{
public:
	static CBitmapMaskedWin* NewL(CFbsBitmap* aFbsBitmap,CFbsBitmap* aFbsMaskBitmap,
								  CWsBitmap* aWsBitmap,CWsBitmap* aWsMaskBitmap,
								  TRgb aBackground,TRect aRect,TBool aInvertMask,TBool aWsFbs);
	~CBitmapMaskedWin();
	//Virtual Function from CTBaseWin
	void Draw();
	void SetDestRectSize(const TSize aSize);
	inline void SetInvertMask(TBool aInvertMask){iInvertMask=aInvertMask;}
private:
	inline CBitmapMaskedWin(CFbsBitmap* aFbsBitmap,CFbsBitmap* aFbsMaskBitmap,CWsBitmap* aWsBitmap,CWsBitmap* aWsMaskBitmap,
							TRect aRect,TBool aInvertMask,TBool aWsFbs):
							iFbsBitmap(aFbsBitmap),iFbsMaskBitmap(aFbsMaskBitmap),
							iWsBitmap(aWsBitmap),iWsMaskBitmap(aWsMaskBitmap),
							iRect(aRect),iInvertMask(aInvertMask),iWsFbs(aWsFbs) {}
private:
	CFbsBitmap* iFbsBitmap;
	CFbsBitmap* iFbsMaskBitmap;
	CWsBitmap* iWsBitmap;
	CWsBitmap* iWsMaskBitmap;
	TRect iRect;
	TBool iInvertMask;
	TBool iWsFbs;
	};

class CPartialRedrawWin : public CTWin
	{
public:
	void Init();
	virtual void Draw();
	void DrawToBmp(CGraphicsContext& aGc);
	void DrawPartial(TPartialRedrawType aPartialRedrawType);
	static void DrawRects(CGraphicsContext& aGc, TSize aSize, TPoint aPosition,
		TBool aIsFullRedraw, TPartialRedrawType aPartialRedrawType);
	void RedrawSubRectWithBitmapL(TRgb aBitmapColour);
public:
	TBool iClientDrawn;
	TBool iClientCanDraw;
private:
	void DoDraw(CGraphicsContext& aGc);
private:
	};

class CPartialRedrawBottomWin : public CTWin
	{
public:
	void Init();
	virtual void Draw();
	void DrawPartial(TRect aRect);
	void DrawFullWindowRect();
private:
	void DoDraw();
	};
	
class CPartialRedrawTopWin : public CTWin
	{
public:
	void Init();
	virtual void Draw();
	void DrawPartial(TRect aRect);
	void DrawFullWindowRect();
private:
	void DoDraw();
	};
	
class CPartialRedrawTiledWin : public CTWin
	{
public:
	void Init(TRgb aColour,TBool aTransparent);
	virtual void Draw();
	void DrawPartial(TRect aRect);
	void DrawFullWindowRect();
private:
	void DoDraw();
private:
	TRgb iColour;
	};
	
class CInvisibleRedrawWin: public CTWin
	{
public:
	CInvisibleRedrawWin();
	TInt MakeTransparent();
	void MakeVisible( TBool aVisible );
	virtual void Redraw();
	virtual void Redraw( const TRect &aRect );

private:
	void DrawIfVisible();

private:
	TBool iVisible;
	TBool iTransparent;
	};

class CPartialDrawNowWin: public CTWin
	{
public:
	CPartialDrawNowWin();
	TInt MakeTransparent();
	void SetLodger( const TRect &aLodger );
	virtual void Redraw();
	virtual void Redraw( const TRect &aRect );

private:
	void DrawWindowAndLodger();

private:
	TRect iLodger;
	TBool iTransparent;
	};

class CPartialRedrawEmptyWin : public CTWin
	{
public:
	void Init(TRgb aColor);
	virtual void Draw();
	void DrawPartial(TRect aRect);
	void DrawFullWindowRect();
	inline TInt ReturnCount();
private:
	void DoDraw();
	TInt iCount;
	};

class CPartialRedrawPolygonWin : public CTWin
	{
public:
	void Init(TInt aWindowMode, TRgb aColor);
	virtual void Draw();
	void DrawPartial();
	void DrawFullWindowPolygonL();
	inline TInt ReturnCount();
private:
	void DoDraw();
	TInt iCount;
	};

class CRedrawRectWin : public CTWin
	{
public:
	void Init();
	void DrawNow();
	void DrawFullWindowRect();
	inline TInt RedrawCount();
	void ResetWindow(TRgb aColour);
	inline void SetLogging(CTWsGraphicsBase* aTest);
	//Virtual function from CTBaseWin
	void Draw();
	//Virtual function from CTWin
	void Redraw(const TRect& aRect);
private:
	void DoDraw();
private:
	TInt iRedrawCount;
	TRgb iRectColour;
	CTWsGraphicsBase* iLog;
	};

class CTRedrawStoringStep : public CTGraphicsStep
	{
public:
	CTRedrawStoringStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

class CResetRedrawStoreWin : public CPartialRedrawWin
	{
public:
	~CResetRedrawStoreWin();
	void SetUpdateInRedraw(TBool aUpdateInRedraw);
	void SetKeepGcActive(TBool aState);
	void UpdateAnim(TInt aSteps);
	void PreSetSize(const TSize &aSize);
	TBool Failed() const;
private:
	virtual void Draw();
	void DoDraw(CGraphicsContext& aGc) const;
	void DoDrawAnim(CGraphicsContext& aGc) const;
	TRect AnimRect() const;
private:
	TInt iAnimState;
	TBool iKeepGcActive;
	TBool iUpdateInRedraw;
	TBool iFailed;
	CWindowGc* iExtraGc;
	};

const TInt KEmptyLoopThreshold = 3;

_LIT(KTRedrawStoringStep,"TRedrawStoring");


#endif
