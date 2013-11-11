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

#ifndef __TWINDOW_H__
#define __TWINDOW_H__

#include <e32std.h>
#include <w32std.h>
#include "AUTO.H"

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
#include "../nonnga/CLIENT/w32comm.h"
#endif

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
#include "../nga/CLIENT/w32comm.h"
#endif

#include "../tlib/testbase.h"
#include "TGraphicsHarness.h"

struct TWindowThreadParam
	{
	TInt iCommand;
	TInt iScreenNumber;
	};

class CWinTestWindow : public CBlankWindow
	{
public:
	CWinTestWindow(TRgb aCol);
	~CWinTestWindow();
	void ConstructL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc, TInt aDepth);
public:
	CWinTestWindow *iChild;
	};

class CEvWindowGroup : public CTWindowGroup
	{
public:
	static CEvWindowGroup* NewLC(CTClient* aClient,CTWsGraphicsBase* aTest);
	CEvWindowGroup(CTClient* aClient,CTWsGraphicsBase* aTest);
	void SetExpectedEvent(TInt aType);
	void SendEvent(TInt aType);
	//Virtual functions from CTWindowGroup
	void ConstructL();
	void UserEvent(TInt aEventType);
private:
	CTWsGraphicsBase* iTest;
	TInt iExpectedEvent;
	TBool iExpectingEvent;
	};

class CTWindowTest : public CTWsGraphicsBase
	{
public:
	enum {EWinTypeFactor=1000};
	enum
		{
		EIdMask=0x0000FFFF,
		EPosMask=0xFFFF0000,
		EPosShift=16
		};
private:
	enum TCorner {ECornerTL,ECornerTR,ECornerBL,ECornerBR};
public:
	CTWindowTest(CTestStep* aStep);
	~CTWindowTest();
	TestState DoTestL();
	void ConstructL();
	void CreateTestWindowL();
	void CheckAndDestroyWindows();
	void DestroyWindowWithActiveGc();
	void DestroyWindowWithActiveGc2L();
	void DrawWindows(CWinTestWindow *aWin);
	void ClearRedraw(RWindow &aWindow, TRgb aRgb);
	void TestInvalidFunctionsL();
	void ShadowAutoClearTest();
	void CreateMegaTree();
	void TiledWindowTestL();
	void TiledWindowTest2L();
	void CornerTests();
	void ColorTestL();
	void TestInvalidFunctions2L();
	void Bug1L();
	void Bug2L();
	void Bug3L();
	void TestDeletedParentPanics1L();
	void TestDeletedParentPanics2L();
	void BackColorBugL();
	void FocusChangedL();
	void FadeRegionsL();
	void FadeRegionsWithRedrawStoringL();
	void EventsToAllL();
	void ErrorCodesL();
	void GroupIdL();
	void doCornerTestsL();
	void doCornerTest(TCornerType aCornerType, TInt aFlags);
	void CheckCorner(TCorner aCorner, const TInt *aInsetList);
	void ShadowTestL();
	void TestWindowDelete();
	void SaturateSendEvent();
	void TestNotSupportedFunctionality();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	static TInt MoveGroup(TAny* aParam);
	void DoFadeRegionsL(TBool aRedrawStoring);
private:
	CWinTestWindow *iWin;
	CTBlankWindow *iBlankWin1;
	CTBlankWindow *iBlankWin2;
	CWsScreenDevice *iScreenDev;
	TThreadStartUp iFirstFunction;
	TWindowThreadParam iThreadParam;
	};

class CTWindowTestStep : public CTGraphicsStep
	{
public:
	CTWindowTestStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTWindowTestStep,"TWindowTest");


#endif
