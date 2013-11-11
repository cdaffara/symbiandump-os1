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

#ifndef __TKREPEAT_H__
#define __TKREPEAT_H__

#include <e32std.h>
#include <e32svr.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CRKWindow;

class CTKRepeat : public CTWsGraphicsBase
	{
	friend class CRKWindow;
public:
	CTKRepeat(CTestStep* aStep);
	~CTKRepeat();
	void ConstructL();
	void TestKeyboardRepeatRateL(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime);
	TBool CheckReportL();
public:
	TBool iAbort;
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	inline TestClient *Client() {return TheClient;}
private:
	TTimeIntervalMicroSeconds32 iOldInitialTime;
	TTimeIntervalMicroSeconds32 iOldTime;
	CRKWindow *iWin;
	TSize iWinSize;
	TInt iState;
	};

class CRKWindow : public CTWin
	{
	enum TRKStates {
		EStateWaitingForKeyDown,
		EStateWaitingForKeyCode,
		EStateWaitingForFirstRepeat,
		EStateWaitingForNthRepeat,
		EStateWaitingForKeyUp,
		EStateInactive,
		EStateError,
		};
public:
	CRKWindow(CTKRepeat *aTest);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void SetState(TRKStates aState);
	void SetKeyboardRepeatRate(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime);
	void WinKeyL(const TKeyEvent &,const TTime &);
	void KeyUpL(const TKeyEvent &aKey,const TTime &aTime);
	void KeyDownL(const TKeyEvent &aKey,const TTime &aTime);
	void Draw();
	TDesC& Report();
	TBool CheckResults();
	void SendEvent();
protected:
	TInt iConnIndex;
	CTKRepeat *iTest;
	TRgb iBack;
	TRKStates iState;
	TInt iDownCode;
	TInt iRepCount;
	TTimeIntervalMicroSeconds32 iInitialRepeatSet;
	TTimeIntervalMicroSeconds32 iRepeatSet;
	TTime iPrevTime;
	TTimeIntervalMicroSeconds32 iInitialGap;
	TTimeIntervalMicroSeconds32 iTotalGap;
	TTimeIntervalMicroSeconds32 iMinGap;
	TTimeIntervalMicroSeconds32 iMaxGap;
	TBuf<0x40> iReport;
	};

class CTKRepeatStep : public CTGraphicsStep
	{
public:
	CTKRepeatStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTKRepeatStep,"TKRepeat");


#endif
