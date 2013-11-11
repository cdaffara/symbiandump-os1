
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

#ifndef __TWSGRAPHS_H__
#define __TWSGRAPHS_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "W32STDGRAPHIC.H"
#include "wsredir.h"
#include "wslisten.h"
#include "TGraphicsHarness.h"
#include "wsframerate.h"
#include "wscontaindrawer.h"
#include "wssimpledrawer.h"
#include "listener.h"
#include "wsgdcov.h"

//
class CCrWin: public CBase
	{
public:
	static CCrWin* NewL(TInt aScreenId,TBool aDraw=ETrue);
	~CCrWin();
	void DrawFirstHalf();
	void DrawSecondHalf();
	void DrawGraphic(TInt aWsId=0);
	void SetTextCursor(const TPoint &aPos, const TTextCursor &aCursor);
	void CancelTextCursor();
private:
	void ConstructL(TInt aScreenId,TBool aDraw);
	void Draw();
private:
	RWsSession iWs;
	CWsScreenDevice* iScr;
	RWindowGroup iGroup;
	RWindow iWin;
	CWindowGc* iGc;
	};

class CCrAlphaWin: public CBase
	{
public:
	static CCrAlphaWin* NewL(TInt aScreenId);
	~CCrAlphaWin();
private:
	void ConstructL(TInt aScreenId);
private:
	RWsSession iWs;
	CWsScreenDevice* iScr;
	RWindowGroup iGroup;
	RWindow iWin;
	};

class CTWsGraphs : public CTWsGraphicsBase
	{
public:
	enum TTestCursorType
		{
		ETestStandardTextCursor,
		ETestCustomTextCursor
		};
public:
	CTWsGraphs(CTestStep* aStep);
	~CTWsGraphs();
	void ConstructL();
public: // list of test cases
	void LaunchNewProcess(const TDesC& aExecutable);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	TestState TestInterfaceExtensionL();
	TestState TestScreenRedirectionL();
	TestState TestTextCursorUnderRedirectionL(TTestCursorType aCursorType);
	TestState TestFlickerRedirectionL();
	TestState TestRedirectionUsingWsBackBufferL();
	void TestScreenModeChangeL();
#endif
	TestState TestEventNotificationL();
	void TestAddSwapGDArrayL();
	TestState TestSuccessiveMessageL();
	void ReportNegativeResultfail(TInt aLine,TInt aResult,TInt aExpectedResult);
	void DoTestLeakInServiceL();
	void TestNestedDrawerCRP();
	void CheckResult();
	void TestNotifyRemoval();
	TestState TestWindowGroupChangeL();
	TestState TestFrameRateL();
	static TInt PluginCallBack(TAny* aArg);
	void CreateWindowL(TBool aDraw=ETrue);
	void DestroyWindowL();
	void CreateAlphaWindowL();
	void DestroyAlphaWindowL();
	TBool CompareBitmapArea16Bpp(CFbsBitmap* aBmp1, const TPoint& aPos1, CFbsBitmap* aBmp2, const TPoint& aPos2, const TSize& aSize);

	TBool PosTestAddSwapGDArrayL(TInt testcase);
	struct WrapTestCall;
	TBool NegTestAddSwapGDArrayL(TInt failcase,WrapTestCall*aWrappedParams=NULL);
	static TInt	DoNegTestCall(TInt /*aInt*/, TAny *aPtr);
	TBool	LaunchNegTestCall(TUint		aTestCount,TUint aPanicCode,const TDesC &aPanicCategory);
	TestState TestGraphicDrawerCoverage();
	// For WsBuffer CRP tests
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	void TestMWsUiBufferL();
	TBool IsWhiteLine(TInt aWhiteLinePos);
#endif
	
private:
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	CWsRedir* iRedir;
	CFbsBitmap* iFrontCopy;
	CFbsBitmap* iBackCopy;
	TRedirectorInfo iRedirInfo;
	CFbsBitmap* iBefore;
	CFbsBitmap* iAfter;
	TTextCursor iTextCursor;
	RArray<TSpriteMember> iSpriteMemberArray;
#endif
	CWsListen* iListen;
	CWsNotify* iNotify1;
	CWsNotify* iNotify2;
	TListenerInfo iListenInfo;
	TInt iSubState;
	CCrWin* iWin;
	CCrAlphaWin* iAlpha;
	TInt iOriginalWindowGroupId;
	CCrWin* iNewWin;
	CGraphicTestFrameRate* iTestframerate;
	TAnimRate iAnimCount;
	CWsGdCoverage* iGdCoverage;
	};

class CTWsGraphsStep : public CTGraphicsStep
	{
public:
	CTWsGraphsStep();

protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};



_LIT(KTWsGraphsStep,"TWsGraphs");


#endif
