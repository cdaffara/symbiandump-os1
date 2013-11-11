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
// Test Pointer move/drag buffer
// 
//

#include <e32std.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

class CTPntBufTest;

class CPntBufWindow : public CTBackedUpWin
	{
private:
	enum {KPointerMoveBufferSize=32};
public:
	CPntBufWindow(CTPntBufTest *aTest);
	~CPntBufWindow();
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent);
	void PointerBufferReady(const TTime &aTime);
	void PointerL(const TPointerEvent &pointer,const TTime &aTime);
	void SetUpState();
	void NextTest();
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	void ErrorL();
private:
	//Virtual functions from CTWinBase
	void PointerEnter(const TTime&);
	void PointerExit(const TTime&);
private:
	CTPntBufTest *iTest;
	TBool iDragging;
	TBool iErrorNest;
	TBool iDisabled;
	TBool iDiscard;
	TInt iMode;
	TBool iIgnorNextPoint;
	};

class CTPntBufTest : public CTestBase
	{
public:
	CTPntBufTest();
	~CTPntBufTest();
	TestState DoTestL();
	void FinishedTests();
	void ConstructL();
private:
	CPntBufWindow *iWin;
	TSize iWinSize;
	TInt iState;
	};

GLDEF_C CTestBase *CreatePointerBufferTest()
	{
	return(new(ELeave) CTPntBufTest());
	}

CPntBufWindow::CPntBufWindow(CTPntBufTest *aTest) : CTBackedUpWin(EGray4), iTest(aTest)
	{}

CPntBufWindow::~CPntBufWindow()
	{
	delete iGc;
	}

void CPntBufWindow::SetUpLD(TPoint pos,TSize size,CTWinBase *parent)
	{
	ConstructExtLD(*parent,pos,size);
	User::LeaveIfError(iWin.AllocPointerMoveBuffer(KPointerMoveBufferSize,0));
	iWin.SetPointerGrab(ETrue);
	iWin.PointerFilter(EPointerFilterEnterExit,0);
	Activate();
	User::LeaveIfError(Client()->iScreen->CreateContext(iGc));
	iGc->Activate(iWin);
	SetUpState();
	}

void CPntBufWindow::PointerBufferReady(const TTime &)
	{
	if (iDiscard)
		iWin.DiscardPointerMoveBuffer();
	iGc->SetPenColor(iDragging ? TRgb::Gray4(0) : TRgb::Gray4(1));
	iGc->SetPenSize(iDragging ? TSize(2,2) : TSize(1,1));
	TPoint pnts[KPointerMoveBufferSize];
	TPtr8 ptr((TUint8 *)&pnts,sizeof(pnts));
	TInt numPnts=iWin.RetrievePointerMoveBuffer(ptr);
	TInt index=0;
	if (iIgnorNextPoint)
		{
		iGc->MoveTo(pnts[index]);
		index=1;
		iIgnorNextPoint=EFalse;
		}
	for(;index<numPnts;index++)
		iGc->DrawLineTo(pnts[index]);
	}

void CPntBufWindow::ErrorL()
	{
	if (!iErrorNest)
		{
		iErrorNest=ETrue;
		iTest->TestL(EFalse);
		}
	}

void CPntBufWindow::PointerL(const TPointerEvent &pointer,const TTime &)
	{
	switch(pointer.iType)
		{
		case TPointerEvent::EButton1Down:
			iDragging=ETrue;
			iGc->MoveTo(pointer.iPosition);
			if (iMode==2)
				{
				if (pointer.iModifiers&EModifierShift)
					{
					iDisabled=ETrue;
					iWin.DisablePointerMoveBuffer();
					}
				else if (pointer.iModifiers&EModifierCtrl)
					iDiscard=ETrue;
				}
			break;
		case TPointerEvent::EButton1Up:
			if (iDisabled)
				{
				iDisabled=EFalse;
				iWin.EnablePointerMoveBuffer();
				}
			iDiscard=EFalse;
			iGc->MoveTo(pointer.iPosition);
			iDragging=EFalse;
			break;
		case TPointerEvent::EDrag:
			if (iDragging && !iDisabled)
				ErrorL();
			break;
		case TPointerEvent::EMove:
			if (iDragging && !iDisabled)
				ErrorL();
			break;
		default:;
		}
	}

void CPntBufWindow::SetUpState()
	{

	iGc->Reset();
	iGc->UseFont((CFont *)iFont);
	iGc->Clear();
	switch(iMode)
		{
		case 0:
			iWin.PointerFilter(EPointerFilterMove,EPointerFilterMove);
			iGc->DrawText(_L("Drag the pointer around the window and check"), TPoint(10,20));
			iGc->DrawText(_L("a line is drawn following the pointer when"), TPoint(10,40));
			iGc->DrawText(_L("dragging but not when moving"), TPoint(10,60));
			iGc->DrawText(_L("Press <Space> when checked"), TPoint(10,90));
			break;
		case 1:
			iWin.PointerFilter(EPointerFilterMove|EPointerFilterDrag,EPointerFilterDrag);
			iGc->DrawText(_L("If pointer moves are supported move the pointer around the "), TPoint(10,20));
			iGc->DrawText(_L("window and check a line is drawn following the pointer"), TPoint(10,40));
			iGc->DrawText(_L("when it is up, and no lines are drawn when dragging"), TPoint(10,60));
			iGc->DrawText(_L("Press <Esc> if moves not supported or <Space> when checked"), TPoint(10,90));
			break;
		case 2:
			iWin.PointerFilter(EPointerFilterMove|EPointerFilterDrag,0);
			iGc->DrawText(_L("Drag and move the pointer around the window and check a"), TPoint(10,20));
			iGc->DrawText(_L("line is drawn following the pointer during both dragging and"), TPoint(10,40));
			iGc->DrawText(_L("moving, also check drag with the shift or control key down doesn't draw"), TPoint(10,60));
			iGc->DrawText(_L("Press <Space> when checked"), TPoint(10,90));
			break;
		}
	}

void CPntBufWindow::NextTest()
	{
	if (iMode++==2)
		iTest->FinishedTests();
	else
		SetUpState();
	}

void CPntBufWindow::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	switch(aKey.iCode)
		{
		case ' ':
			NextTest();
			break;
		case EKeyEscape:
			iTest->FinishedTests();
			break;
		case '0':
			SetUpState();
			break;
		case '1':
		case '2':
		case '3':
			{
			TInt mode=aKey.iCode-'1';
			if (mode!=iMode)
				{
				iMode=mode;
				SetUpState();
				}
			}
			break;
		}
	}

void CPntBufWindow::PointerEnter(const TTime&)
	{
	if (iMode==0)
		iDragging=ETrue;
	else if (iMode==1)
		iDragging=EFalse;
	iIgnorNextPoint=ETrue;
	}

void CPntBufWindow::PointerExit(const TTime&)
	{}


/*CTPntBufTest*/

CTPntBufTest::CTPntBufTest() : CTestBase(_L("Scale"))
	{}

CTPntBufTest::~CTPntBufTest()
	{
#if defined(__WINS__)
	Client()->iWs.SimulateXyInputType(EXYInputPointer);
#endif
	CTWin::Delete(iWin);
	}

void CTPntBufTest::FinishedTests()
	{
	Request();
	}

void CTPntBufTest::ConstructL()
	{
	CPntBufWindow *win=new(ELeave) CPntBufWindow(this);
	win->SetUpLD(TPoint(0,0),Client()->iScreen->SizeInPixels(),Client()->iGroup);
	iWin=win;
	Client()->iGroup->SetCurrentWindow(iWin);
#if defined(__WINS__)
	Client()->iWs.SimulateXyInputType(EXYInputMouse);
#endif
	}

TestState CTPntBufTest::DoTestL()
	{
	switch(iState)
		{
		case 0:
			LogSubTest(_L("Scale 1"),1);
			iState++;
			return(EContinue);
		default:
			return(EFinished);
		}
//	return(ENext);
	}
