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

#ifndef __TREDRAW_H__
#define __TREDRAW_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TScreenModeScaling.h"
#include "TGraphicsHarness.h"

class CTRedrawTest;

/**
 * Abstract base class to draw a pattern into a window.
 * 
 * This provides a default Draw() implementation which
 * DOES NOT set the clipping region before drawing.
 */
class CRedrawWindow : public CTWin
	{
	friend class CTRedrawTest;
public:
	enum TDrawPattern
		{
		EDrawGraphPaperlHatched = 0,
		EDrawSlantingHatched,
		EDrawCenteredRectangle
		};
public:
	CRedrawWindow(CTRedrawTest* aTest);
	~CRedrawWindow();
public:
	/*
	 * Draw the window, counting the number of calls.
	 * @post DrawRequests() returns a value increased by 1.
	 */
	virtual void Reset();
	virtual void PrepareInvalidation(const TRect& aRect) = 0;
	virtual void PerformInvalidation() = 0;
	//Virtual function from CTBaseWin
	void Draw();
public:
	void ActivateAndDraw(TInt aPattern);
	void DrawPattern(TInt aPattern);
	void VisibleRegion(RRegion& aRegion);
	void ValidateAndClear();
	void ActivateAndDraw(TInt aPattern,TRegion* aRegion);
	TInt DrawRequests() const;
protected:
	void ReceivedDrawRequest();
	TPoint iOffset;
	RRegion iInvalid;
	CTRedrawTest *iTest;
private:
	TInt iDrawRequests;
	};

/**
 * Concrete class to draw a pattern into a window.
 * 
 */
class CReferenceComparisonRedrawWindow : public CRedrawWindow
	{
public:
	CReferenceComparisonRedrawWindow(CTRedrawTest* aTest);
public:
	// from CRedrawWindow
	void PrepareInvalidation(const TRect& aRect);
	void PerformInvalidation();
	};

class CRedrawWindow2 : public CRedrawWindow
	{
public:
	CRedrawWindow2(CTRedrawTest* aTest);
public:
	//Virtual function from CTBaseWin overridden in CRedrawWindow
	void Draw();
	//Virtual function from CRedrawWindow
	void Reset();
	// from CRedrawWindow
	void PrepareInvalidation(const TRect& aRect);
	void PerformInvalidation();
private:
	TBool iClipped;
	};

class CRedrawWindow3 : public CRedrawWindow
	{
public:
	CRedrawWindow3(CTRedrawTest* aTest);
public:
	//Virtual function from CTBaseWin overridden in CRedrawWindow
	void Draw();
	// from CRedrawWindow
	void PrepareInvalidation(const TRect& aRect);
	void PerformInvalidation();
public:
	void SetUp1L(TPoint pos,TSize size,CTWinBase* parent,CWindowGc& aGc);
	void Activate();
	void Redraw(const TRect& aRect);
protected:
	TBool isActive;
	};

class CTRedrawOrderWindow : public CTWin
	{
public:
	CTRedrawOrderWindow(TInt aOrder, CTRedrawTest *aTest);
	static CTRedrawOrderWindow* NewLC(TInt aOrder,CTWinBase* aParent,const TPoint& aPos,const TSize& aSize,CTRedrawTest* aTest);
	inline static TInt RedrawNumber() {return iRedrawNumber;}
	static void ResetRedrawNumber();
	void Redraw(const TRect& aRect);
	void Draw();
private:
	CTRedrawTest* iTest;
	TInt iOrder;
	static TInt iRedrawNumber;
	};

class CTCheckDefectWin : public CBasicWin
	{
public:
	static CTCheckDefectWin *NewL(TPoint aPos,TSize aWinSize);
private:
	CTCheckDefectWin(){}
	};

class CInvalidRegionChecker : public CBase
	{
public:
	CInvalidRegionChecker(const CTRedrawTest& aGraphicsTest);
	~CInvalidRegionChecker();
	
	TInt CheckInvalidRegions(const CTWindowGroup* aGroup);	
	TBool ExcludedWindow(const CTWinBase* aTWinBase) const;	
	TInt AddExcludedWindow(const CTWinBase* aExcludedWindow);
	void RemoveExcludedWindow(const CTWinBase* aExcludedWindow);
private:
	void ProcessWindowGroup(const CTWinBase* aTWinBase);
	void ProcessChildWindow(const CTWinBase* aTWinBase);
	void ProcessWindow(const CTWinBase* aTWinBase);
private:
	TInt iInvalidRegionCount;
	RPointerArray<const CTWinBase> iExcludedWindowArray;
	const CTRedrawTest& iGraphicsTest;
	};

class CTRedrawTest : public CTWsGraphicsBase
	{
public:
	CTRedrawTest(CTestStep* aStep);
	~CTRedrawTest();
	void doDestruct();
	void ConstructL();
	void CheckRedrawWindows();
	void ProcessBaseInvalidate();
	void SetBackground(const TRgb& aRgb);
	void DumpRegion(const TRegion& aRegion);
	inline TInt WinContent();
	void InvalidateTestWins(const TRect& aRect);
	void MoveInvalidAreaL();
	void TestGetInvalidRegionL(const CArrayFixFlat<TRect>* aRectList);
	void GetInvalidRegionTestsL();
	void CompareRegionsL(const TRegion& aRegion1,const TRegion& aRegion2);
	void Failed(TInt aOrder);
	void CheckOrderL();
	void TestRect();
	void CheckDefectINC049554L();
	void CheckDefectPDEF099892L();
	void CheckOrdinalPositionDefectL();
	void CheckDefectPDEF117784L();
	void CheckMMSDefectL(TBool aMoveBlankWindow);
	void ConstructAndSetBlankWinLC(RBlankWindow& aWindow, TSize aSize, TPoint aPoint=TPoint(),
						   TRgb aBackgroundColor=TRgb(0,0,0));
	void ConstructWindowLC(RWindow& aWindow, TSize aSize, TPoint aPoint=TPoint(),
						   TRgb aBackgroundColor=TRgb(255,255,255), TBool aTransparencyByAlpha=EFalse,
						   TDisplayMode aDisplayMode=EColor64K);
	void ActivateAndSetGc(CWindowGc& aGc, RWindow& aWindow, CGraphicsContext::TBrushStyle aBrushStyle=CGraphicsContext::ESolidBrush, TRgb aBrushColor=TRgb(0,0,0),
						  		CGraphicsContext::TPenStyle aPenStyle=CGraphicsContext::ENullPen, TRgb aPenColor=TRgb(0,0,0));
	void DrawWin(CWindowGc& aGc, RWindow& aWin, TSize winSize, TRgb aRectColor1, TRgb aRectColor2, TInt aNewOrdinalPos=0, RWindow* aWinToMove=NULL, TBool aDrawAllPixels=EFalse);
	enum TRedrawCheckType
		{
		ECheckRedrawActiveObjectOnly,
		ECheckRedrawActiveObjectAndInvalidRegions	
		};
	TInt WaitForRedrawsToFinish(TRedrawCheckType aRedrawCheckType);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void ValidateAndClear(TestWindow *aWin);
private:
	TSize iWinSize;
	TBool iDoCheck;
	TInt iRedrawNo;
	CReferenceComparisonRedrawWindow *iBaseRedrawWin;
	CRedrawWindow2 *iTestRedrawWin;
	CBlankWindow *iBaseChildWin;
	CBlankWindow *iTestChildWin;
	TInt iDrawRequestsFromTestWindow;
	TInt iWinContent;
	CInvalidRegionChecker *iInvalidRegionChecker;
	};

class CTRedrawTestStep : public CTGraphicsStep
	{
public:
	CTRedrawTestStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTRedrawTestStep,"TRedrawTest");


#endif
