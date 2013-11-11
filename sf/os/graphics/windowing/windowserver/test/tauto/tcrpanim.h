// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TCrpAnim.H Defines class CTCrpAnim
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TCRPANIM_H__
#define __TCRPANIM_H__

#include "AUTO.H"

_LIT(KTCrpAnimStep,"TCrpAnim");

namespace	//anonymous local scope
	{
	class CCrpAnim;
	typedef  CCrpAnim CAnonAnimWindow;
	class CActiveWait;
	typedef  CActiveWait CAnonActiveWait;
	}
	
class CTCrpAnim : public CTWsGraphicsBase
	{
public:
	CTCrpAnim(CTestStep* aStep);
	~CTCrpAnim();
	void ConstructL();
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	// TAnimFrameResult is a struct designed to track the various frame results
	// as the animation progresses. Once the animation is complete, TAnimFrameResult
	// provides an easy to read indication as to the animation's performance
	struct TAnimFrameResult
		{
		TInt iFrameOK;
		TInt iFramePartial;
		TInt iFrameIdentical;
		TInt iFrameEmpty;
		TInt iFrameTearing;
		TInt iFrameSkipped;
		};
	void InvalidatePauseAndRedraw(const TRect &aRect,TTimeIntervalMicroSeconds32 aInterval);
	void RedrawAndCheckWindows(TBool aBlankIt=EFalse);
	void TestSpriteLoopL(TBool aAnimForeground,TBool aDrawForeground);
    void CheckAnimProgressedL(CAnonAnimWindow* aAnimWin, TInt aAdditionalFrameCount, TBool aCaptureFrameResult = ETrue);
	TInt DetermineApproxFrameNum(CFbsBitmap* aBitmap, TBool aCaptureFrameResult);
	void ResetFrameCounters();
	TInt LogResults();
// The tests
    void	TestSpriteInterruptsForegroundL();
    void	TestSpriteRestartInterruptsForegroundL();
	void	BasicCRPDemo();
	void	TestOverlappingWindowsL();
	void	TestClippingWindowsL();
	void	DemoClippingWindowsL();
	void	TestInvalidateL();
private:
	CAnonAnimWindow *iTestWin;
	CAnonAnimWindow *iOverWin;
	CAnonAnimWindow *iRedrawWin;
	CAnonAnimWindow *iBaseWin;
	TInt iPreviousFrameNum;				// identifies the last successfully drawn frame number calculated
	TInt iMinimumCalcRedLine;			// the calculated minimum length of the red line in the animation
	TAnimFrameResult iFrameStatus;		// tracker for the performance of each animated frame
	CActiveWait* iWaiter;				// active object with configurable timer
	CWsGraphicBitmapAnimation* iAnim; 	// ptr to server-side animation object
	CFbsBitmap* iScreenBitmap;			// bitmap holding screen snap-shot
	HBufC8* iScanlineBuf;				// buffer holding pixel data on a single vertical scan line
	};

class CTCrpAnimStep : public CTGraphicsStep
	{
public:
	CTCrpAnimStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

#endif  //__TCRPANIM_H__
