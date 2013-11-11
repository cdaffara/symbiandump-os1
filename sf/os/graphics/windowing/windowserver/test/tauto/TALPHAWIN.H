
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

#ifndef __TALPHAWIN_H__
#define __TALPHAWIN_H__

#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTDrawOpWin;
class CTAlphaWindow;
class CTAlphaRefWin;

class CTAlphaWin : public CTWsGraphicsBase
	{
public:
	CTAlphaWin(CTestStep* aStep);
	~CTAlphaWin();
	void ConstructL();
	void TestCondition();
	void TestConditionL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void ConfigureDisplayModes(TDisplayMode aRequiredMode);
	// The blending test works as follows:
	// On the left, we have a window which draws using an opaque half-red colour
	// On the right, we have a window which blends using a semi-transparent full-red colour
	// The test compares left and right
	void TestSemiTransparentDrawingL();
	void TestTransparentDrawingL();

	// The transparent window tests work as follows:
	// On the left, we have transparent windows, on which we perform various various operations
	// On the right, we have a reference window, in which we draw what we think the windows on the left should look like
	// The test compares the right and the left
	void TestInitialConfiguration();
	void TestMove();
	void TestRedraw();
	void TestInvisible();
	void TestChildrenL();
	void TestAntiAliasedTextTransparentL();
	void CheckRectL(const TRect& rect1, const TRect& rect2, TSize aSize, TDisplayMode aRequiredMode, TInt aTolerance, const TDesC& aErrorMsg);

private:
	TFixedArray<CTAlphaWindow*,5> iTestWin;
	CTAlphaRefWin* iRefWin;
	};

class CTDrawOpWin : public CTWin
	{
public:
	static CTDrawOpWin* NewL(CTAlphaWin* aTest, CTWinBase* aParent, TPoint aPos, TSize aSize, TRgb aDrawColour);
	CTDrawOpWin(CTAlphaWin* aTest, TRgb aDrawColour);

	//Virtual Function from CTBaseWin
	void Draw();

	void SetDrawOp(TInt aDrawOp);
private:
	CTAlphaWin* iTest;
	TRgb iDrawColour;
	TInt iDrawOp;
	};


class CTAlphaWindow : public CTWin
	{
	friend class CTAlphaRefWin;
public:
	inline CTAlphaWindow(CTAlphaWin* aTest) : iTest(aTest) {}
	~CTAlphaWindow();
	static CTAlphaWindow* NewL(CTAlphaWin* aTest, CTWinBase* aParent, TPoint aPos, TSize aSize, TInt aDrawState);

	//Virtual Function from CTBaseWin
	void Draw();

	void SetDrawState(TInt aDrawState);
	TInt DrawState();

	void SetVisible(TBool aVisible);
	void CreateChildrenL(TInt aDepth);
	void DestroyChildren();
private:
	TInt iDrawState;
	CTAlphaWin* iTest;
	CTAlphaWindow* iChild1;
	CTAlphaWindow* iChild2;
	};

class CTAlphaRefWin : public CTWin
	{
	// This class is used to draw what we think things should look like, for comparison with what they actually look like
public:
	inline CTAlphaRefWin(TFixedArray<CTAlphaWindow*,5>& aAlphaWin);
	static CTAlphaRefWin* NewL(CTWinBase* aParent, TPoint aPos, TSize aSize, TFixedArray<CTAlphaWindow*,5>& aAlphaWin);
	void Draw();
private:
	void DrawWindow(CTAlphaWindow* aWindow, TPoint aOffset);
private:
	TFixedArray<CTAlphaWindow*,5>& iAlphaWin;
	};

class CTAlphaWinStep : public CTGraphicsStep
	{
public:
	CTAlphaWinStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTAlphaWinStep,"TAlphaWin");


#endif
