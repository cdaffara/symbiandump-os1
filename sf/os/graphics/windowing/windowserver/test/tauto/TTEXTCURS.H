
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

#ifndef __TTEXTCURS_H__
#define __TTEXTCURS_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

/*
 * Wrapper class for a custom text cursor.
 */
class CCustomTextCursor : public CBase
	{
public:
	static CCustomTextCursor* CreateCustomTextCursorL(TInt aScreenNumber,TInt aBmpIndex,CTestBase* aTest);
	~CCustomTextCursor();
	
private:
	CCustomTextCursor(CTestBase* aTest);
	void ConstructL(TInt aScreenNumber,TInt aBmpIndex);
	
public:
	TInt iIdentifier;
	RArray<TSpriteMember> iSpriteMemberArray;
	TUint iSpriteFlags;
	RWsSession::TCustomTextCursorAlignment iAlignment;
	CTestBase* iTest;
	};
	
class CTCursorTest : public CTWsGraphicsBase
	{
public:
	CTCursorTest(CTestStep* aStep);
	~CTCursorTest();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void SetCursor(const TPoint &aPos,const TSize &aSize,TRgb aColor, TUint aFlags=0);
	void SetCursor(const TPoint &aPos,const TSize &aSize,TRgb aColor, const TRect &aRect, TUint aFlags=0);
	void SetCursorPlusBox(const TPoint &aPos,const TSize &aSize,TRgb aColor, const TRect *aRect, TUint aFlags=0);
	void CancelTextCursor();
	void ConstructL();
	void doMoveWindowTestL();
	TBool MoveWindow();
	void TestPanicsL();
	void ScrollTest();
	void GeneralTestsL();
	void MoveWindowTest1L();
	void MoveWindowTest2L();
	void INC041278L();
	void INC040489L();
	void CursorUpdatedBeforeWindowRenderedL();
	void INC097774();
	void INC117232();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	void TextCursorNoFlashTestL();
	void TextCursorFlashTestL();
	void CheckCursorDoesNotFlash(const TSize& aSize);
	void UpdateCountersOnCursorTransition(
			const TBool	aTransitionedToOn,
			TTime& 		aNow,
			TInt64&		aDeltaTime,
			TTime&		aLastDeltaTime,
			TInt&		aWarmUpIterations,
			const TInt&	aFlashChangeTime,
			const TInt&	aToleranceMargin,
			TInt&		aSampleNumber,
			TInt&		aToleranceViolations
			);
	void CheckCursorDoesFlash(const TPoint& aPos, const TTextCursor& aTextCursor, TRgb aBackgroundColor);
	void DrawTextCursorSimilarToRenderStage(CWindowGc& aGc, RWindow& aWin, const TPoint& aPos, const TTextCursor& aTextCursor);
#endif
	TBool IncrementCursorType();
	CTSprite* CreateTestSpriteLC(RWindowTreeNode &aWindow, const TPoint &aPos, TInt aCount);
	void TestForArtifacts(CBlankWindow* aWin1, TTextCursor& aCursor);
	void CheckNoDoubleCursorTest1L(CBlankWindow* aWin1, CBlankWindow* win2, TTextCursor& aTextCursor,TTextCursor& aCustomCursor, CWindowGc* winGc);
	void CheckNoDoubleCursorTest2L(CBlankWindow* aWin1, CBlankWindow* win2, TTextCursor& aTextCursor,TTextCursor& aCustomCursor, CWindowGc* winGc);
	void CheckNoDoubleCursorTest3L(CBlankWindow* aWin1, CBlankWindow* win2, TTextCursor& aTextCursor,TTextCursor& aCustomCursor, CWindowGc* winGc);
	void MakeCursors(TTextCursor& aTextCursor, TTextCursor& aCustomCursor);										  
	void StartDoubleCursorTestL(TInt aTestNumber);
	void doCheckNoDoubleCursor(CBlankWindow* aWin1,
								CBlankWindow* aWin2,
								const TPoint& aWin1Tl,
								const TPoint& aWin1T2,
								const TTextCursor& aCursor,
								const TSize& aCursorSize,
								CWsScreenDevice::TSpriteInCompare aFlags);
	
	void ResetWindows(CWindowGc* aWinGc,CBlankWindow* aWin1,CBlankWindow* aWin2);
	void ValidateWin(TestWindow *aWin, TRgb aColor);
	void RedrawWin(RWindow &aWin, TRgb aColor);
	void DeleteMoveWindows();
	void ResetMoveWindowsL();
	static TInt DoPanicTest(TInt aInt, TAny *aScreenNumber);
	void TextCursorSetLCoverageTests();
	
private:
	TSize iWinSize;
	TPoint iWinPos;
	TInt iWinState;
	CBlankWindow *iMoveWin;
	CBlankWindow *iCheckWin;
	CBlankWindow *iWorkInProgress;
	CBlankWindow *iComparisonWindow;
	TTextCursor::EType iCursorType;
	static CTestBase* iStaticTest;	// static because it is used by DoPanicTest
	};


class CTCursorTestStep : public CTGraphicsStep
	{
public:
	CTCursorTestStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTCursorTestStep,"TCursorTest");


#endif
