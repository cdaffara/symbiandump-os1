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

#ifndef __TAUTODLL_H__
#define __TAUTODLL_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"


#include <w32adll.h>
#include "AUTODLL.H"

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
#include "../nonnga/CLIENT/w32comm.h"
#endif

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
#include "../nga/CLIENT/w32comm.h"
#endif

#include "tdirecta.h"


class CAnimWindow;

class CAnimRedrawWindow : public CTWin
	{
public:
	CAnimRedrawWindow(CAnimWindow *aAnimWindow, TBool aIsBase);
	~CAnimRedrawWindow();
	void Draw();
private:
	CAnimWindow *iAnimWindow;
	TBool iIsBase;
	};

class CAnimWindow : public CBase
	{
	friend class CTAnimDll;
	friend class CAnimRedrawWindow;
public:
	enum TWinType
		{
		ERedraw,
		EBlank,
		EBackedUp,
		};
public:
	CAnimWindow(TBool aIsBase, TWinType aWinType);
	~CAnimWindow();
	void ConstructL(const TPoint &aPos, const TSize &aSize);
	void SetRect(const TRect &aRect);
	static void Draw(CBitmapContext *aGc, const TSize &aSize, TBool aIsBase,const TRect &aRect, TBool aBlankIt);
	static void DrawEllipse(CBitmapContext *aGc, const TRect &aRect);
	void DoDraw(TBool aBlankIt=EFalse);
	void DoDrawEllipse();
	void DoDrawCoverage();
	TSize Size();
	RWindowBase *BaseWin() const;
	CTBaseWin *CtBaseWin();
	void Invalidate();
	void Invalidate(const TRect &aRect);
	static void SetEllipseDrawMode(CGraphicsContext::TDrawMode aEllipseDrawMode);
	void DrawTestScreen(CFbsBitmap *aBitmap, CFbsBitmap *aMaskBitmap, CFbsFont *aFont);
protected:
	CTBaseWin *iCtWin;
	TWinType iWinType;
	TBool iIsBase;
	TRect iRect;
	static CGraphicsContext::TDrawMode iEllipseDrawMode;
	};



class CTAnimDll : public CTWsGraphicsBase
	{
public:
	CTAnimDll(CTestStep* aStep);
	~CTAnimDll();
	void ConstructL();

	void TestPanicsL();
	void RemoteBuffer();
	void Misc();
	void ExtraCopyTest();
	void SyncMode1();
	void TestSyncModes(MAnimGeneralFunctions::TAnimSync aSyncMode);
	void TestTimeChange();
	void TestLoadApiL();
	void KeyClickPriorityL();
protected:	//Pure virtual from MTestCases
	void RunTestCaseL(TInt aCurTestCase);
private:
	void InvalidatePauseAndRedraw(const TRect &aRect,TTimeIntervalMicroSeconds32 aInterval);	
	void RedrawAndCheckWindows(TBool aBlankIt=EFalse);
	void SetUpMember(TSpriteMember &aMember,TInt aType);
	void DrawingTestL();
	void ShadowDrawingTestL();
	void GeneralDrawingTestL();
	void CheckWindows();
	void DestroyWindowTestL();
	void SpriteAnimL();
	void FreeTimerL();
	void DisableTimerL();
	void MultipleAnimsL();
	void ClickPlugInL();
	void WinFunctionsL();
	void WinFunctions2L();
	void TestNotificationsL();
	void TestCoverageL();
	void TestPanicCoverageL();
	void TestWindowInfo(RTestAnim& aAnim,TInt aScreen,TInt aPos,TInt aId);
	void TestEventHandlerRemovalL();
	void ParameterValueTestL();
	void TestSetIntervalL();
	TInt CheckError(TInt aError,TInt aExpected);
	TRgb PixelColour(const TPoint& aPoint) const;
private:
	RAnimDll iAnimDll;
	TSize iWinSize;
	CAnimWindow *iTestWin;
	CAnimWindow *iRedrawWin;
	CAnimWindow *iBaseWin;
	CAnimWindow *iBackedUpTestWin;
	CAnimWindow *iBlankTestWin;
	};

class CTAnimDllStep : public CTGraphicsStep
	{
public:
	CTAnimDllStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTAnimDllStep,"TAnimDll");


#endif
