// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for window server auto test code
// 
//

#include <e32std.h>
#include <e32math.h>
#include "W32STD.H"
#include "../tlib/testbase.h"

class TestClient;

enum TestState
	{
	EInvalid,
	ENext,
	EContinue,
	EFinished
	};

enum TAutoPanics
	{
	ETManPanicPcFailed,
	};

enum TAutoLeaves
	{
	ETestFailed=1,
	};

class LogWindow : public CTWin
	{
public:
	LogWindow();
	void ConstructL(CTWinBase &parent);
	void Draw();
	void LogTest(TDesC &aTitle,TInt aNum);
private:
	TBuf<0x40> iTestTitle;
	TInt iTitleHeight;
	};

class CTestBase;

class BorderWindow : public CTWin
	{
public:
	BorderWindow();
	void ConstructL(CTWinBase &parent);
	void Draw();
	};

class TestWindow : public CTWin
	{
public:
	TestWindow();
	void Draw();
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
private:
	BorderWindow *iBorderWin;
	friend class CTestBase;
	};

class TestDriver : public CActive
	{
public:
	TestDriver(TestClient *aClient);
	~TestDriver();
	void DoCancel();
	void RunL();
	void Request();
	void TestComplete();
	void DestroyTest();
	TestClient *Client();
public:
	CTestBase *iTest;
private:
	TInt iTestNum;
	TestClient *iClient;
	};

class CTestBase : public CActive
	{
public:
	CTestBase(const TDesC &aTitle);
	~CTestBase();
	void DoCancel();
	void RunL();
	void Request();
	void TestL(TInt aCondition);
	void StartTest(TInt aNum, TestDriver *aDriver);
	void LogSubTest(const TDesC &aTitle,TInt aNum);
	void TestPanic(TThreadFunction aFunction, TAny *aPtr, TInt aExitReason, const TDesC &aCategory);
	void TestWsPanic(TThreadFunction aFunction, TAny *aPtr, TInt aExitReason);
	void TestWsPanic(TThreadFunction aFunction, TInt aInt, TInt aExitReason);
	void TriggerFail();
	void LogLeave(TInt aErr);
	void CheckRect(CTWin *aWin1,CTWin *aWin2,const TRect &aRect);
	void CompareWindows(TRect &aRect);
	void CompareWindows();
	void CheckBorder();
	void DrawTestBackground(TBool aInvertColours);
	inline TestDriver *Driver();
	virtual TestState DoTestL()=0;
	virtual void ConstructL()=0;
	TestClient *Client();
	void AbortL();
protected:
	TestDriver *iDriver;
	TAny *iBuf1;
	TAny *iBuf2;
private:
	TInt iTestNum;
	TInt iSubTestNum;
	TBuf<0x40> iTitle;
	TBuf<0x40> iSubTitle;
	TBool iFail;	// Used to trigger failures to test test code.
	};

class TestClient : public CTClient
	{
public:
	TestClient();
	~TestClient();
	void ConstructL();
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void Shutdown();
	TestDriver *Driver();
private:
	TestDriver *iDriver;
	};

GLREF_D CWindowGc *TheGc;
GLREF_C void TManPanic(TInt aPanic);

inline TestDriver *CTestBase::Driver()
	{return(iDriver);}
