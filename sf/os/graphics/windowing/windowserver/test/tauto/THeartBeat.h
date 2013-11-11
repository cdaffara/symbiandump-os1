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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __THEARTBEAT_H__
#define __THEARTBEAT_H__

#include <e32std.h>
#include <w32std.h>
#include "AUTO.H"
#include "TAUTODLL.H"

class CTHeartBeatTest : public CTWsGraphicsBase
	{
public:
	CTHeartBeatTest(CTestStep* aStep);
	~CTHeartBeatTest();
protected:
//from MTestCases
	void RunTestCaseL(TInt aCurTestCase);
private:
	enum TCursorTransWinTestMode
		{
		ECursorTransWinTestNA=-1,
		ECursorTransWinTestModeFullTrans,
		ECursorTransWinTestNoTrans,
		ECursorTransWinTestPartialTransExposingCursor,
		ECursorTransWinTestPartialTransCoveringCursor,
		ENumCursorTransWinTestModes,
		};
private:
	void SetCursor(const TPoint& aPos,const TSize& aSize, TUint aFlags=0);
	void CancelTextCursor();
	void ConstructL();
	void TestTextCursorTimerL();
	void TestTextCursorTimerTransL();
	void RunTransWinTestsL(const TRect& aGraphicRect);
	void LoopThroughTransWinTestModes(RWindow& aTransWin, const TRect& aGraphicRect, TInt aExtraWinTestMode);
	TBool ChangeTransAndCheckHeartBeat(TInt aTestMode, RWindow& aTransWin, const TRect& aGraphicRect, TInt aExtraWinTestMode, const TDesC* aErrorMsg, TBool aSetVisibleAfterTransUpdate);
	void FailCursorHbTest(const TDesC& aErrorBase, const TDesC* aExtraText, TBool aHbState, TBool aHbCheckState);
	void TestHeartBeatStopStart(TBool aHeartbeatStateOff,TBool aHeartbeatStateOn,const TDesC& aOffText,const TDesC& aOnText);
	TBool GetHeartBeatTimerState() const;
	TBool SetAutoHeartBeatPauseState(TBool aState) const;
	void TestActiveInactiveEvents();
	void TestAnimDllL(TBool aSpriteMode, TUint aSpriteFlags);
	void TestAnimDllTransL();
	void doTestActiveInactiveEvents(TBool aCursorOn, TBool aAutoPauseOn);
	static TInt DoPanicTest(TInt aInt, TAny* aScreenNumber);
private:
	RAnimDll iAnimDll;
	CAnimWindow* iHbTestWin;
	};

class CTHeartBeatTestStep : public CTGraphicsStep
	{
public:
	CTHeartBeatTestStep();
protected:	
	//from CTGraphicsStep
	CTGraphicsBase* CreateTestL();
	};

_LIT(KTHeartBeatTestStep,"THeartBeatTest");

#endif
