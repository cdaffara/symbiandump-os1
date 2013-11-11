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

#ifndef __TPOINTER_H__
#define __TPOINTER_H__

#include <e32std.h>
#include <w32std.h>
#include <hal.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTPointer;
class CGrabWindow;

class CPointerWindow : public CTWin
	{
	enum {KRepeatMargin=200000};	// 0.2 Seconds
	enum {KRepeatCount=4};
	enum {KRepeatIntervalIncrements=100000};
	enum {KRepeatRectXExtra=50};
	enum {KRepeatRectYExtra=50};
	enum {KStateWaitingForTest1,KStateFailed,KStateTesting1,
			KStateWaitingForTest2,KStateTesting2,
			KStateWaitingForTest3,KStateTesting3,
			KStateWaitingForTest4,KStateTesting4,
			KStateWaitingForTest5,KStateTesting5,
			KStateWaitingForTest6,KStateTesting6,
			KStateWaitingForTest8,KStateTesting8,
			KStateWaitingForTest7,KStateTesting7,
			KStateFinished};
public:
	CPointerWindow(CTPointer *aTest);
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw();
	void StartNextRepeatTest();
	void QueueNextRepeat();
	void PointerL(const TPointerEvent &aPointer,const TTime&);
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
private:
	void SwitchOn(const TTime &);
	void ResetTest(TInt aState);
	void SetState(TInt aState);
	void FinishedTests();
	void TestFailed(TBool aRetest=EFalse);
	void SendEvent();
private:
	CTPointer *iTest;
	TInt iRepeatCount;
	TInt iState;
	TTimeIntervalMicroSeconds32 iInterval;
	TTime iPrevTime;
	TRect iRepeatRect;
	TPoint iTl;
	TSize iSize;
	};

class CGrabWindow2 : public CTWin
	{
public:
	CGrabWindow2(CGrabWindow *aWindow);
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
private:
	void Draw();
	void PointerL(const TPointerEvent &aPointer,const TTime&);
private:
	CGrabWindow *iGrabWindow;
	};

class CGrabWindow : public CTWin
	{
	enum {KGrabStateWaitingForDown,KGrabStateWaitingForDragOut,KGrabStateWaitingForUp,KGrabStateFinished,
		KGrabStateWaitingForDown2,KGrabStateWaitingForUp2a,KGrabStateWaitingForDrag2,KGrabStateWaitingForUp2b,
		KGrabStateWaitingForDown3,KGrabStateWaitingForUp3a,KGrabStateWaitingForUp3b,
		KGrabStateWaitingForDown4,KGrabStateWaitingForUp4,
		KGrabStateWaitingForDown5,KGrabStateWaitingForUp5,KGrabStateWaitingForDrag5,
		KStateFailed};
public:
	CGrabWindow(CTPointer *aTest);
	void Pointer2(const TPointerEvent &aPointer);
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
private:
	void Draw();
	void SetState(TInt aState);
	void ResetTest();
	void PointerL(const TPointerEvent &aPointer,const TTime&);
	void TestFailed();
	void FinishedTests();
	void SendEvent();
private:
	TInt iState;
	CTPointer *iTest;
	TPoint iTl;
	TSize iSize;
	};


class CTPointer : public CTWsGraphicsBase
	{
public:
	enum {KTimeOutAfter=20000000};	//20secs
public:
	CTPointer(CTestStep* aStep);
	~CTPointer();
	void ConstructL();

	TBool TestFailed(TInt aCase,TBool aRetry=EFalse);
	void StartGrabTestL();
	static TInt TimeOut(TAny* aTest);
	void TimeOut();
	CGrabWindow2 *GrabWin2() const;
	inline TBool Digitiser() const {return !iNoDigitiser;}
	inline void SimulatePointer(TRawEvent::TType aType, TInt aX, TInt aY){iTest->SimulatePointer(aType, aX, aY);}
	inline void SimulateEvent(TRawEvent::TType aType){iTest->SimulateEvent(aType);}
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TInt doTestFailedL();
	inline TestClient* Client() {return TheClient;}
	TInt iState;

private:
	CTimeOut* iTimeOut;
	TInt iTimeOutCount;
	CPointerWindow *iRepeatWin;
	CGrabWindow *iGrabWin;
	CGrabWindow2 *iGrabWin2;
	TSize iWinSize;
	//TInt iState;
	TBool iOldPointerState;
	TBool iNoDigitiser;
	};


class CTPointerStep : public CTGraphicsStep
	{
public:
	CTPointerStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTPointerStep,"TPointer");


#endif
