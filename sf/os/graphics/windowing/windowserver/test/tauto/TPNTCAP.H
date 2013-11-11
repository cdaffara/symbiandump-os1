
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

#ifndef __TPNTCAP_H__
#define __TPNTCAP_H__

#include <e32std.h>
#include <e32svr.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

enum TTestState
	{
	ECaptureDisabled=0,
	ENormalCapture,
	ECaptureAllGroups,
	EDragDropCapture,
	EDragDropCaptureAllGroups,
	ENormalCaptureWithoutFocus,
	};

enum TTestSubState
	{
	EMainWindow,
	EChildWindow,
	EOtherGroup,
	EOtherSession,
	ERootWindow,
	};

enum TPointerCheckRet
	{
	EFailed,
	EOkay,
	ENeedsDDEvent,
	};

const TInt ESubStates1=2;
const TInt ESubStates2=5;
const TInt ESubStates3=5;
const TInt ESubStates4=5;
const TInt ESubStates5=5;
const TInt ESubStates6=5;

const TInt EWinBorderSize=10;

class CPcWindowBase;
class CTPointerCapture;

class CPcConnection : public CTClient
	{
public:
	CPcConnection(CTPointerCapture *aTest);
	~CPcConnection();
	virtual void ConstructL();
protected:
	CTPointerCapture *iTest;
	};

class CPcWindowBase : public CTWin
	{
public:
	CPcWindowBase(CTPointerCapture *aTest);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw(TDesC &aBuf);
	void PointerL(const TPointerEvent &pointer,const TTime &);
	void DragDropL(const TPointerEvent &pointer,const TTime &);
	virtual TPointerCheckRet PointerDown()=0;
	virtual TPointerCheckRet DragDrop()=0;
	virtual void SubStateChanged();
protected:
	CTPointerCapture *iTest;
	TRgb iBack;
	};

class CPcWindowChild : public CPcWindowBase
	{
public:
	CPcWindowChild(CTPointerCapture *aTest);
	void Draw();
	TPointerCheckRet PointerDown();
	TPointerCheckRet DragDrop();
	};

class CPcWindowMain : public CPcWindowBase
	{
public:
	CPcWindowMain(CTPointerCapture *aTest);
	void Draw();
	TPointerCheckRet PointerDown();
	TPointerCheckRet DragDrop();
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	};

class CPcWindowAltGroup : public CPcWindowBase
	{
public:
	CPcWindowAltGroup(CTPointerCapture *aTest);
	void Draw();
	TPointerCheckRet PointerDown();
	TPointerCheckRet DragDrop();
	};

class CPcWindowNickFocusGroup : public CTWindowGroup
	{
public:
	CPcWindowNickFocusGroup(CTPointerCapture *aTest, CTClient *aClient);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
private:
	CTPointerCapture *iTest;
	};

class CPcWindowAltConnection : public CPcWindowBase
	{
public:
	CPcWindowAltConnection(CTPointerCapture *aTest);
	void Draw();
	TPointerCheckRet PointerDown();
	TPointerCheckRet DragDrop();
	};

class CTPointerCapture : public CTWsGraphicsBase
	{
private:
	enum TDState {DDStateNull, DDStateGot, DDStateWaiting};
public:
	CTPointerCapture(CTestStep* aStep);
	~CTPointerCapture();
	void ConstructL();
	void NextTest();
	void AbortTests();
	TInt State() const;
	TInt SubState() const;
	void doIncSubStateL();
	void IncSubStateL(TBool aNeedsDD=EFalse);
	void GotDDL();
	void StateChanged();
	void TestFailed();
	void NickFocusL();
	void SetCapture(TInt aCaptureFlags);
	inline TestClient* Client() {return TheClient;}
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TInt doTestFailedL();
	void SendEventsL();
private:
	CPcConnection *iAltConnection;
	CTWindowGroup *iMainGroup;
	CTWindowGroup *iAltGroup;
	CPcWindowChild *iChildWin;
	CPcWindowMain *iMainWin;
	CPcWindowAltGroup *iAltGroupWin;
	CPcWindowAltConnection *iAltConnectionWin;
	CPcWindowNickFocusGroup *iNickFocusGroup;
	CTBlankWindow *iNickFocusBlankWin;
	//TInt iState;
	TInt iSubState;
	TDState iDDState;
	TSize iScreenSize;
public:
	TBool iFailed;
	};

class CTPointerCaptureStep : public CTGraphicsStep
	{
public:
	CTPointerCaptureStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTPointerCaptureStep,"TPointerCapture");


#endif
