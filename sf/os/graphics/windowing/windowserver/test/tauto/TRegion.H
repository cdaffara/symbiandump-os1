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

#ifndef __TREGION_H__
#define __TREGION_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include <bitstd.h>
#include "TGraphicsHarness.h"

class CTRegion : public CTWsGraphicsBase
	{
public:
	CTRegion(CTestStep* aStep);
	~CTRegion();
	void ConstructL();

protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
	
private:
	void DrawNonClippedSample1(CTWin *aDrawable, const TPoint &aOrigin);
	void DrawNonClippedSample1(CTWin *aDrawable, const TRegion &aRegion, const TPoint &aOrigin);
	void DrawClippedSample1(TestWindow *aDrawable, const TRegion &aRegion, const TRect &aClippingRect, const TPoint &aOrigin);
	void DrawNonClippedSample2(CTWin *aDrawable, const TPoint &aOrigin);
	void DrawNonClippedSample2(CTWin *aDrawable, const TRegion &aRegion, const TPoint &aOrigin);
	void DrawClippedSample2(TestWindow *aDrawable, const TRegion &aRegion, const TRect &aClippingRect, const TPoint &aOrigin);
	void DrawClippedSample3(CBitmapContext &aGc);
	void DrawNonClippedSample1(CBitmapContext &aGc);
	void DrawClippedSample1(CBitmapContext &aGc);
	void DrawNonClippedSample2(CBitmapContext &aGc, CFont *aFont);
	void DrawClippedSample2(CBitmapContext &aGc, CFont *aFont);
	void TestRegionL(const TRegion &aClippingRegion, const TRect &aClippingRect, const TPoint &aOrigin);
	void ScrollBugL();
	void ExposeTestL();
	void ExposeTest2L();
	void CoverTestL();
	void OffsetTestL();
	void ClipTestL();
	void CreateChildWindowsL();
	void DeleteChildWindows();
	void ClearBitmapAndWindows();
	void CompareRectsL(TPoint aTl1,TPoint aTl2,TSize aSize);

private:
	CFbsFont *iFont;
	CFbsFont *iBitFont;
	CFbsBitmap iPicture;
	CFbsBitmapDevice *iBitmapDevice;
	CWsBitmap *iBitmap;
	CFbsBitGc *iBitGc;
	TSize iDrawableSize;
	CBlankWindow *iBaseChildWin;
	CBlankWindow *iTestChildWin;
	};

class CTRegionStep : public CTGraphicsStep
	{
public:
	CTRegionStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTRegionStep,"TRegion");


#endif
