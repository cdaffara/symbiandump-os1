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
// Test pointer capture
// 
//

#include <e32std.h>
#include <e32svr.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

enum TTestState
	{
	ECaptureDisabled,
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
class TPointerCaptureTest;

class CPcConnection : public CTClient
	{
public:
	CPcConnection(TPointerCaptureTest *aTest);
	~CPcConnection();
	virtual void ConstructL();
protected:
	TPointerCaptureTest *iTest;
	};

class CPcWindowBase : public CTWin
	{
public:
	CPcWindowBase(TPointerCaptureTest *aTest);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw(TDesC &aBuf);
	void PointerL(const TPointerEvent &pointer,const TTime &);
	void DragDropL(const TPointerEvent &pointer,const TTime &);
	virtual TPointerCheckRet PointerDown()=0;
	virtual TPointerCheckRet DragDrop()=0;
	virtual void SubStateChanged();
protected:
	TPointerCaptureTest *iTest;
	TRgb iBack;
	};

class CPcWindowChild : public CPcWindowBase
	{
public:
	CPcWindowChild(TPointerCaptureTest *aTest);
	void Draw();
	TPointerCheckRet PointerDown();
	TPointerCheckRet DragDrop();
	};

class CPcWindowMain : public CPcWindowBase
	{
public:
	CPcWindowMain(TPointerCaptureTest *aTest);
	void Draw();
	TPointerCheckRet PointerDown();
	TPointerCheckRet DragDrop();
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	};

class CPcWindowAltGroup : public CPcWindowBase
	{
public:
	CPcWindowAltGroup(TPointerCaptureTest *aTest);
	void Draw();
	TPointerCheckRet PointerDown();
	TPointerCheckRet DragDrop();
	};

class CPcWindowNickFocusGroup : public CTWindowGroup
	{
public:
	CPcWindowNickFocusGroup(TPointerCaptureTest *aTest, CTClient *aClient);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
private:
	TPointerCaptureTest *iTest;
	};

class CPcWindowAltConnection : public CPcWindowBase
	{
public:
	CPcWindowAltConnection(TPointerCaptureTest *aTest);
	void Draw();
	TPointerCheckRet PointerDown();
	TPointerCheckRet DragDrop();
	};

class TPointerCaptureTest : public CTestBase
	{
private:
	enum TDState {DDStateNull, DDStateGot, DDStateWaiting};
public:
	TPointerCaptureTest();
	~TPointerCaptureTest();
	TestState DoTestL();
	void ConstructL();
	void NextTest();
	void AbortTests();
	TInt State() const;
	TInt SubState() const;
	void doIncSubState();
	void IncSubState(TBool aNeedsDD=EFalse);
	void GotDD();
	void StateChanged();
	void TestFailed();
	void NickFocusL();
	void SetCapture(TInt aCaptureFlags);
	void RestartTest();
private:
	TInt doTestFailedL();
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
	TInt iState;
	TInt iSubState;
	TDState iDDState;
public:
	TBool iFailed;
	};

GLDEF_C CTestBase *CreatePointerCaptureTest()
	{
	return(new(ELeave) TPointerCaptureTest());
	}

//
// CMcConnection

CPcConnection::CPcConnection(TPointerCaptureTest *aTest) : iTest(aTest)
	{
	}

CPcConnection::~CPcConnection()
	{
	}

void CPcConnection::ConstructL()
	{
	CTClient::ConstructL();
	iGroup=new(ELeave) CTWindowGroup(this);
	iGroup->ConstructL();
	iGroup->GroupWin()->SetOrdinalPosition(0,1);
	iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	iWs.Flush();
	}

//
// CPcWindow, base class //
//

CPcWindowBase::CPcWindowBase(TPointerCaptureTest *aTest) : CTWin(), iTest(aTest)
	{
	}

void CPcWindowBase::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iWin.SetBackgroundColor(iBack);
	Activate();
	AssignGC(aGc);
	}

void CPcWindowBase::SetUpL(TPoint pos,TSize size,CTWinBase *parent)
	{
	SetUpL(pos,size,parent,*iTest->Client()->iGc);
	}

void CPcWindowBase::SubStateChanged()
	{
	iWin.Invalidate();
	Client()->iWs.Flush();
	}

void CPcWindowBase::Draw(TDesC &aBuf)
	{
	iGc->Clear();
	iGc->SetPenColor(iBack.Gray4()>1 ? TRgb(0,0,0) : TRgb(255,255,255));
	iGc->DrawText(aBuf, TPoint(10,20));
	}

void CPcWindowBase::PointerL(const TPointerEvent &pointer,const TTime &)
	{
	if (pointer.iType==TPointerEvent::EButton1Down && !iTest->iFailed)
		{
		if (iTest->SubState()==ERootWindow)	// Root window click, must not be inside this window
			{
			if (TRect(Size()).Contains(pointer.iPosition))
				{
				iTest->TestFailed();
				return;
				}
			}
		TInt ret;
		if ((ret=PointerDown())==EFailed)
			iTest->TestFailed();
		else
			iTest->IncSubState(ret==ENeedsDDEvent);
		}
	}

void CPcWindowBase::DragDropL(const TPointerEvent &pointer,const TTime &)
	{
	switch(iTest->State())
		{
		case ECaptureDisabled:
		case ENormalCapture:
		case ECaptureAllGroups:
		case ENormalCaptureWithoutFocus:
			iTest->TestFailed();
			break;
		}
	if (pointer.iType==TPointerEvent::EButton1Down && !iTest->iFailed)
		{
		if (DragDrop()==EFailed)
			iTest->TestFailed();
		else
			iTest->GotDD();
		}
	}
//

CPcWindowMain::CPcWindowMain(TPointerCaptureTest *aTest) : CPcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(236);
	}

TPointerCheckRet CPcWindowMain::PointerDown()
	{
	switch(iTest->State())
		{
		case ECaptureDisabled:
			if (iTest->SubState()==EMainWindow)
				return(EOkay);
			break;
		case ENormalCapture:
			switch(iTest->SubState())
				{
				case EMainWindow:
				case ERootWindow:
					return(EOkay);
				default:
					break;
				}
			break;
		case ECaptureAllGroups:
			switch(iTest->SubState())
				{
				case EMainWindow:
				case EOtherGroup:
				case EOtherSession:
				case ERootWindow:
					return(EOkay);
				default:
					break;
				}
			break;
		case EDragDropCapture:
			switch(iTest->SubState())
				{
				case EMainWindow:
					return(ENeedsDDEvent);
				case ERootWindow:
					return(EOkay);
				default:
					break;
				}
			break;
		case EDragDropCaptureAllGroups:
			switch(iTest->SubState())
				{
				case EMainWindow:
				case EOtherGroup:
				case EOtherSession:
					return(ENeedsDDEvent);
				case ERootWindow:
					return(EOkay);
				default:
					break;
				}
			break;
		case ENormalCaptureWithoutFocus:
			switch(iTest->SubState())
				{
				case EMainWindow:
					return(EOkay);
				default:
					break;
				}
			break;
		default:
			break;
		}
	return(EFailed);
	}

TPointerCheckRet CPcWindowMain::DragDrop()
	{
	switch(iTest->State())
		{
		case EDragDropCapture:
		case EDragDropCaptureAllGroups:
			switch(iTest->SubState())
				{
				case EMainWindow:
					return(EOkay);
				default:
					break;
				}
			break;
		default:
			break;
		}
	return(EFailed);
	}

void CPcWindowMain::Draw()
	{
	TBuf<0x40> buf;
	if (iTest->State()==ECaptureDisabled)
		{
		switch(iTest->SubState())
			{
			case EMainWindow:
				buf.Copy(_L("Click on me"));
				break;
			}
		}
	else
		{
		switch(iTest->SubState())
			{
			case EMainWindow:
				buf.Copy(_L("Click on me"));
				break;
			case EChildWindow:
			case EOtherGroup:
			case EOtherSession:
				break;
			case ERootWindow:
				if (iTest->State()==ENormalCaptureWithoutFocus)
					buf.Copy(_L("Click on the root window, then press <Escape>"));
				else
					buf.Copy(_L("Click on the root window"));
				break;
			}
		}
	CPcWindowBase::Draw(buf);
	}

void CPcWindowMain::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	switch(aKey.iCode)
		{
		case EKeyEscape:
			iTest->AbortTests();
			break;
		}
	}

//

CPcWindowChild::CPcWindowChild(TPointerCaptureTest *aTest) : CPcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(85);
	}

TPointerCheckRet CPcWindowChild::PointerDown()
	{
	switch(iTest->State())
		{
		case ECaptureDisabled:
			break;
		default:
			switch(iTest->SubState())
				{
				case EChildWindow:
					return(EOkay);
				default:
					break;
				}
			break;
		}
	return(EFailed);
	}

TPointerCheckRet CPcWindowChild::DragDrop()
	{
	switch(iTest->State())
		{
		case EDragDropCapture:
		case EDragDropCaptureAllGroups:
			switch(iTest->SubState())
				{
				case EChildWindow:
					return(EOkay);
				default:
					break;
				}
			break;
		default:
			break;
		}
	return(EFailed);
	}

void CPcWindowChild::Draw()
	{
	TBuf<0x40> buf;
	if (iTest->State()!=ECaptureDisabled)
		{
		switch(iTest->SubState())
			{
			case EChildWindow:
				buf.Copy(_L("Click on me"));
				break;
			default:
				break;
			}
		}
	CPcWindowBase::Draw(buf);
	}

//

CPcWindowNickFocusGroup::CPcWindowNickFocusGroup(TPointerCaptureTest *aTest, CTClient *aClient) : CTWindowGroup(aClient), iTest(aTest)
	{
	}

void CPcWindowNickFocusGroup::KeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode==EKeyEscape)
		iTest->IncSubState(EFalse);
	}

//

CPcWindowAltGroup::CPcWindowAltGroup(TPointerCaptureTest *aTest) : CPcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(236);
	}

TPointerCheckRet CPcWindowAltGroup::PointerDown()
	{
	switch(iTest->State())
		{
		case ECaptureDisabled:
			if (iTest->SubState()==EChildWindow)
				return(EOkay);
			break;
		case ENormalCapture:
		case EDragDropCapture:
		case ENormalCaptureWithoutFocus:
			switch(iTest->SubState())
				{
				case EOtherGroup:
					return(EOkay);
				default:
					break;
				}
			break;
		default:
			break;
		}
	return(EFailed);
	}

TPointerCheckRet CPcWindowAltGroup::DragDrop()
	{
	switch(iTest->State())
		{
		case EDragDropCapture:
		case EDragDropCaptureAllGroups:
			switch(iTest->SubState())
				{
				case EOtherGroup:
					return(EOkay);
				default:
					break;
				}
			break;
		default:
			break;
		}
	return(EFailed);
	}

void CPcWindowAltGroup::Draw()
	{
	TBuf<0x40> buf;
	if (iTest->State()==ECaptureDisabled)
		{
		switch(iTest->SubState())
			{
			case EMainWindow:
				break;
			case EChildWindow:
				buf.Copy(_L("Click on me"));
				break;
			}
		}
	else
		{
		switch(iTest->SubState())
			{
			case EOtherGroup:
				buf.Copy(_L("Click on me"));
				break;
			default:
				break;
			}
		}
	CPcWindowBase::Draw(buf);
	}

//

CPcWindowAltConnection::CPcWindowAltConnection(TPointerCaptureTest *aTest) : CPcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(236);
	}

TPointerCheckRet CPcWindowAltConnection::PointerDown()
	{
	switch(iTest->State())
		{
		case ECaptureDisabled:
			if (iTest->SubState()==EChildWindow)
				return(EOkay);
			break;
		case ENormalCapture:
		case EDragDropCapture:
		case ENormalCaptureWithoutFocus:
			switch(iTest->SubState())
				{
				case EOtherSession:
					return(EOkay);
				default:
					break;
				}
			break;
		default:
			break;
		}
	return(EFailed);
	}

TPointerCheckRet CPcWindowAltConnection::DragDrop()
	{
	switch(iTest->State())
		{
		case EDragDropCapture:
		case EDragDropCaptureAllGroups:
			switch(iTest->SubState())
				{
				case EOtherSession:
					return(EOkay);
				default:
					break;
				}
			break;
		default:
			break;
		}
	return(EFailed);
	}

void CPcWindowAltConnection::Draw()
	{
	TBuf<0x40> buf;
	if (iTest->State()!=ECaptureDisabled)
		{
		switch(iTest->SubState())
			{
			case EOtherSession:
				buf.Copy(_L("Click on me"));
				break;
			default:
				break;
			}
		}
	CPcWindowBase::Draw(buf);
	}

//

TPointerCaptureTest::TPointerCaptureTest() : CTestBase(_L("Pointer Capture"))
	{}

TPointerCaptureTest::~TPointerCaptureTest()
	{
	delete iNickFocusBlankWin;
	delete iNickFocusGroup;
	delete iAltConnectionWin;
	delete iAltConnection;
	delete iChildWin;
	delete iMainWin;
	delete iMainGroup;
	delete iAltGroupWin;
	delete iAltGroup;
	}

void TPointerCaptureTest::TestFailed()
	{
	__ASSERT_DEBUG(iFailed==EFalse,TManPanic(ETManPanicPcFailed));
	iFailed=ETrue;
	Client()->iGroup->GroupWin()->SetOrdinalPosition(0,10);	// Put error dialog on top of test windows
	TInt dRet=1;
	TRAPD(err,dRet=doTestFailedL());
	Client()->iGroup->GroupWin()->SetOrdinalPosition(0,0);
	switch(dRet)
		{
		case 0:
			RestartTest();
			break;
		case 1:
			TRAP(err,TestL(EFalse));
			break;
		}
	}

TInt TPointerCaptureTest::doTestFailedL()
	{
	CTDialog *dialog=new(ELeave) CTDialog();
	dialog->SetTitle(_L("Pointer capture test failed"));
	dialog->SetNumButtons(2);
	dialog->SetButtonText(0,_L("Retest"));
	dialog->SetButtonText(1,_L("Fail"));
	dialog->ConstructLD(*Client()->iGroup,*Client()->iGc);
	return dialog->Display();
	}

TInt TPointerCaptureTest::State() const
	{
	return(iState);
	}

TInt TPointerCaptureTest::SubState() const
	{
	return(iSubState);
	}

void TPointerCaptureTest::doIncSubState()
	{
	iSubState++;
	TInt max=0;
	switch(iState)
		{
		case ECaptureDisabled:
			max=ESubStates1;
			break;
		case ENormalCapture:
			max=ESubStates2;
			break;
		case ECaptureAllGroups:
			max=ESubStates3;
			break;
		case EDragDropCapture:
			max=ESubStates4;
			break;
		case EDragDropCaptureAllGroups:
			max=ESubStates5;
			break;
		case ENormalCaptureWithoutFocus:
			max=ESubStates6;
			break;
		}
	if (iSubState==max)
		NextTest();
	StateChanged();
	}

void TPointerCaptureTest::GotDD()
	{
	if (iDDState==DDStateWaiting)
		doIncSubState();
	else
		iDDState=DDStateGot;
	}

void TPointerCaptureTest::IncSubState(TBool aNeedsDD)
	{
	if (!aNeedsDD)
		{
		if (iDDState!=DDStateNull)
			TestFailed();
		else
			doIncSubState();
		}
	else if (iDDState==DDStateGot)
		doIncSubState();
	else
		iDDState=DDStateWaiting;
	}

void TPointerCaptureTest::StateChanged()
	{
	iDDState=DDStateNull;
	iChildWin->SubStateChanged();
	iMainWin->SubStateChanged();
	iAltGroupWin->SubStateChanged();
	iAltConnectionWin->SubStateChanged();
	}

void TPointerCaptureTest::AbortTests()
	{
	iState=99;
	Request();
	}

void TPointerCaptureTest::NextTest()
	{
	iState++;
	Request();
	}

void TPointerCaptureTest::RestartTest()
	{
	Request();
	}

void TPointerCaptureTest::ConstructL()
	{
	TSize size(Client()->iGroup->Size());
	TInt winWidth2=size.iWidth/2-EWinBorderSize*2;
	TInt winWidth4=size.iWidth/4-EWinBorderSize*2;
	TInt winHeight=size.iHeight/2-EWinBorderSize*2;
//
	iMainGroup=new(ELeave) CTWindowGroup(Client());
	iMainGroup->ConstructL();
	iMainGroup->GroupWin()->SetOrdinalPosition(0,1);
	iMainWin=new(ELeave) CPcWindowMain(this);
	iMainWin->SetUpL(TPoint(EWinBorderSize,EWinBorderSize) ,TSize(winWidth2,winHeight) ,iMainGroup);
	iMainGroup->SetCurrentWindow(iMainWin);
	iChildWin=new(ELeave) CPcWindowChild(this);
	iChildWin->SetUpL(TPoint(0,winHeight/2) ,TSize(winWidth2,winHeight/2) ,iMainWin);
//
	iAltGroup=new(ELeave) CTWindowGroup(Client());
	iAltGroup->ConstructL();
	iAltGroup->GroupWin()->SetOrdinalPosition(0,1);
	iAltGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	iAltGroupWin=new(ELeave) CPcWindowAltGroup(this);
	iAltGroupWin->SetUpL(TPoint(size.iWidth/2+EWinBorderSize,EWinBorderSize) ,TSize(winWidth4,winHeight) ,iAltGroup);
//
	iAltConnection=new(ELeave) CPcConnection(this);
	iAltConnection->ConstructL();
	iAltConnectionWin=new(ELeave) CPcWindowAltConnection(this);
	iAltConnectionWin->SetUpL(TPoint(size.iWidth/4*3+EWinBorderSize,EWinBorderSize) ,TSize(winWidth4,winHeight),iAltConnection->iGroup,*iAltConnection->iGc);
	}

void TPointerCaptureTest::NickFocusL()
	{
	iNickFocusGroup=new(ELeave) CPcWindowNickFocusGroup(this,Client());
	iNickFocusGroup->ConstructL();
	iNickFocusGroup->GroupWin()->SetOrdinalPosition(0,2);
	iNickFocusBlankWin=new(ELeave) CTBlankWindow();
	iNickFocusBlankWin->ConstructL(*iNickFocusGroup);
	iNickFocusBlankWin->SetSize(TSize(1,1));
	iNickFocusBlankWin->Activate();
	}

void TPointerCaptureTest::SetCapture(TInt aCaptureFlags)
	{
	iMainWin->Win()->SetPointerCapture(aCaptureFlags);
	}

TestState TPointerCaptureTest::DoTestL()
	{
	iSubState=0;
	iFailed=EFalse;
	StateChanged();
	switch(iState)
		{
		case ECaptureDisabled:
			LogSubTest(_L("No capture"),1);
			SetCapture(RWindowBase::TCaptureDisabled);
			break;
		case ENormalCapture:
			LogSubTest(_L("Normal capture"),1);
			SetCapture(RWindowBase::TCaptureEnabled);
			break;
		case ECaptureAllGroups:
			LogSubTest(_L("All groups"),1);
			SetCapture(RWindowBase::TCaptureEnabled|RWindowBase::TCaptureFlagAllGroups);
			break;
		case EDragDropCapture:
			LogSubTest(_L("Drag & Drop"),1);
			SetCapture(RWindowBase::TCaptureDragDrop&~RWindowBase::TCaptureFlagAllGroups);
			break;
		case EDragDropCaptureAllGroups:
			LogSubTest(_L("Drag & Drop All groups"),1);
			SetCapture(RWindowBase::TCaptureDragDrop);
			break;
		case ENormalCaptureWithoutFocus:
			LogSubTest(_L("Without focus"),1);
			NickFocusL();
			SetCapture(RWindowBase::TCaptureEnabled);
			break;
		default:
			return(EFinished);
		}
	return(EContinue);
 	}
