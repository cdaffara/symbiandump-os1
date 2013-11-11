
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

#ifndef __TSCROLL_H__
#define __TSCROLL_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include <bitstd.h>
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTScroll;
class CScrollTestBase;

class CScrollWindowNorm : public CTWin
	{
public:
	CScrollWindowNorm();
	void Draw();
	void SetScrollTest(CScrollTestBase *aScrollTest);
private:
	CScrollTestBase *iScrollTest;
	};

class CScrollWindowBackedUp : public CTBackedUpWin
	{
public:
	CScrollWindowBackedUp(TDisplayMode aDisplayMode);
	void Draw();
	void SetScrollTest(CScrollTestBase *aScrollTest);
private:
	CScrollTestBase *iScrollTest;
	};

class CScrollTestBase : public CBase
	{
public:
	~CScrollTestBase();
	virtual void ConstructL(const TPoint &aPos, const TSize &aSize)=0;
	virtual void DoTestScroll(const TPoint &aOffset)=0;
	virtual void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset)=0;
	virtual void DoTestScroll(const TPoint &aOffset, const TRect &aRect)=0;
	virtual void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect)=0;
	virtual void Reset();
	TPoint Offset() const;
	virtual CTDrawableWin *ScrollWin() const=0;
	virtual void DoInvalidate();
	virtual void Redraw()=0;
protected:
	TSize iSize;
	TPoint iOffset;
	friend class CTScroll;
	RRegion iInvalid;
	CTBlankWindow *iBlankWin;
	};

class CScrollTestBaseNorm : public CScrollTestBase
	{
public:
	void ConstructL(const TPoint &aPos, const TSize &aSize);
	~CScrollTestBaseNorm();
	virtual CTDrawableWin *ScrollWin() const;
	virtual void Reset();
	void VisibleRegion(RRegion &aRegion);
	void AdjustTestScrollRegion(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect);
	void DrawScrollWin(const TPoint &aOffset, const TSize &aSize);
	void Redraw();
protected:
	CScrollWindowNorm *iScrollWin;
	};

class CScrollWindow1 : public CScrollTestBaseNorm
//
// Used as control window for a normal redraw window
//
	{
public:
	void DoTestScroll(const TPoint &aOffset);
	void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset);
	void DoTestScroll(const TPoint &aOffset, const TRect &aRect);
	void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect);
	void DoInvalidate();
	};

class CScrollWindow2 : public CScrollTestBaseNorm
//
// Used to test a normal redraw window
//
	{
public:
	CScrollWindow2(CTScroll *aTest);
	virtual void Draw();
	virtual void Reset();
	void DoTestScroll(const TPoint &aOffset);
	void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset);
	void DoTestScroll(const TPoint &aOffset, const TRect &aRect);
	void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect);
private:
	TBool iClipped;
	CTScroll *iTest;
	};

class CScrollWindow3 : public CScrollTestBaseNorm
//
// Used as control window for testing backed up windows
//
	{
public:
	void ConstructL(const TPoint &aPos, const TSize &aSize);
	~CScrollWindow3();
	virtual void Reset();
	void Draw();
	void DoTestScroll(const TPoint &aOffset);
	void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset);
	void DoTestScroll(const TPoint &aOffset, const TRect &aRect);
	void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect);
	void DoInvalidate();
	void DrawScrollBit();
private:
	CFbsBitmap *iBitmap;
	CFbsDevice *iBitmapDevice;
	CFbsBitGc *iBitmapGc;
	};

class CScrollWindow4 : public CScrollTestBase
//
// Tests a backed up window
//
	{
public:
	~CScrollWindow4();
	void ConstructL(const TPoint &aPos, const TSize &aSize);
	void DoTestScroll(const TPoint &aOffset);
	void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset);
	void DoTestScroll(const TPoint &aOffset, const TRect &aRect);
	void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect);
	virtual CTDrawableWin *ScrollWin() const;
	virtual void Reset();
	void Redraw();
protected:
	CScrollWindowBackedUp *iScrollWin;
	};

class CTScroll : public CTWsGraphicsBase
	{
public:
	CTScroll(CTestStep* aStep);
	~CTScroll();
	void ConstructL();
	void CheckScrollWindows();
	void DoTestScroll(const TPoint &aOffset);
	void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset);
	void DoTestScroll(const TPoint &aOffset, const TRect &aRect);
	void DoTestScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect);
	void FinishedRectScrolls();
	void CheckOomScroll();
	void OomScrolling();
	void DemoWindowScrollL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TSize iWinSize;
	TInt iMode;
	TBool iDoCheck;
	CScrollTestBase *iBaseScrollWin;
	CScrollTestBase *iTestScrollWin;
	};

class CTScrollStep : public CTGraphicsStep
	{
public:
	CTScrollStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTScrollStep,"TScroll");


#endif
