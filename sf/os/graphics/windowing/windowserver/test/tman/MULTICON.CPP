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
// Test multiple connections to the window server
// 
//

#include <e32std.h>
#include <e32svr.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

const TInt EMaxSubState=3;

class CMcWindowBase;
class TMultiConTest;

class CMcConnectionBase : public CTClient
	{
public:
	CMcConnectionBase(TMultiConTest *aTest);
	~CMcConnectionBase();
	virtual void ConstructL();
	void SubStateChanged();
protected:
	TMultiConTest *iTest;
	CMcWindowBase *iWin;
	CWindowGc *iGc;
	};

class CMcConnection : public CMcConnectionBase	// Sets AutoForeground off
	{
public:
	CMcConnection(TMultiConTest *aTest);
	void ConstructL();
	};

class CMcWindowGroupAf : public CTWindowGroup
	{
public:
	CMcWindowGroupAf(CTClient *aClient);
	void KeyL(const TKeyEvent &aKey, const TTime &aTime);
	};

class CMcConnectionAf : public CMcConnectionBase	// Sets AutoForeground on
	{
public:
	CMcConnectionAf(TMultiConTest *aTest);
	void ConstructL();
	void KeyL(const TKeyEvent &aKey);
	};

class CMcConnectionDef : public CMcConnectionBase	// Leaves AutoForeground as the default value
	{
public:
	CMcConnectionDef(TMultiConTest *aTest);
	void ConstructL();
	};

class CMcWindowBase : public CTWin
	{
public:
	CMcWindowBase(TMultiConTest *aTest);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	virtual void Draw()=0;
	virtual void PointerL(const TPointerEvent &pointer,const TTime &)=0;
protected:
	TMultiConTest *iTest;
	TRgb iBack;
	};

class CMcWindow : public CMcWindowBase
	{
public:
	CMcWindow(TMultiConTest *aTest);
	virtual void Draw();
	virtual void PointerL(const TPointerEvent &pointer,const TTime &);
	};

class CMcWindowAf : public CMcWindowBase
	{
public:
	CMcWindowAf(TMultiConTest *aTest);
	virtual void Draw();
	virtual void PointerL(const TPointerEvent &pointer,const TTime &);
	void ConstructL();
	};

class CMcWindowDef : public CMcWindowBase
	{
public:
	CMcWindowDef(TMultiConTest *aTest);
	virtual void Draw();
	virtual void PointerL(const TPointerEvent &pointer,const TTime &);
	};

class TMultiConTest : public CTestBase
	{
public:
	TMultiConTest();
	~TMultiConTest();
	TestState DoTestL();
	void ConstructL();
	void EndAutoForegroundTest();
	TInt SubState() const;
	void IncSubState();
private:
	CMcConnectionAf *iConn1;
	CMcConnection *iConn2;
	CMcConnectionDef *iConn3;
	TSize iWinSize;
	TInt iState;
	TInt iSubState;
	};

GLDEF_C CTestBase *CreateMultiConTest()
	{
	return(new(ELeave) TMultiConTest());
	}

TMultiConTest::TMultiConTest() : CTestBase(_L("MultiCon"))
	{}

TMultiConTest::~TMultiConTest()
	{
	delete iConn1;
	delete iConn2;
	delete iConn3;
	}

void TMultiConTest::EndAutoForegroundTest()
	{
	iConn1->iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	iConn2->iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	iConn3->iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	Request();
	}

void TMultiConTest::ConstructL()
	{
	iConn3=new(ELeave) CMcConnectionDef(this);
	iConn3->ConstructL();
	iConn2=new(ELeave) CMcConnection(this);
	iConn2->ConstructL();
	iConn1=new(ELeave) CMcConnectionAf(this);
	iConn1->ConstructL();
	}

//
// CMcConnection

CMcConnectionBase::CMcConnectionBase(TMultiConTest *aTest) : iTest(aTest)
	{
	}

CMcConnectionBase::~CMcConnectionBase()
	{
	CTWin::Delete(iWin);
	delete iGc;
	}

void CMcConnectionBase::SubStateChanged()
	{
	iWin->Invalidate();
	iWs.Flush();
	}

void CMcConnectionBase::ConstructL()
	{
	CTClient::ConstructL();
	User::LeaveIfError(iScreen->CreateContext(iGc));
	}

CMcConnection::CMcConnection(TMultiConTest *aTest) : CMcConnectionBase(aTest)
	{
	}

void CMcConnection::ConstructL()
	{
	CMcConnectionBase::ConstructL();
	iGroup=new(ELeave) CTWindowGroup(this);
	iGroup->ConstructL();
	TSize screenSize=iGroup->Size();
	TInt winWidth=screenSize.iWidth/3;
	TInt winHeight=screenSize.iHeight/2-10;
	iGroup->GroupWin()->AutoForeground(EFalse);
	CMcWindow *win=new(ELeave) CMcWindow(iTest);
	win->SetUpL(TPoint(5,5),TSize(winWidth,winHeight),iGroup,*iGc);
	iWin=win;
	iWs.Flush();
	}

CMcConnectionAf::CMcConnectionAf(TMultiConTest *aTest) : CMcConnectionBase(aTest)
	{
	}

void CMcConnectionAf::ConstructL()
	{
	CMcConnectionBase::ConstructL();
	iGroup=new(ELeave) CMcWindowGroupAf(this);
	iGroup->ConstructL();
	TSize screenSize=iGroup->Size();
	TInt winWidth=screenSize.iWidth/3;
	TInt winHeight=screenSize.iHeight/2-10;
	iGroup->GroupWin()->AutoForeground(ETrue);
	CMcWindowAf *win=new(ELeave) CMcWindowAf(iTest);
	win->SetUpL(TPoint(winWidth,5),TSize(winWidth,winHeight),iGroup,*iGc);
	iWin=win;
	iWs.Flush();
	}

void CMcConnectionAf::KeyL(const TKeyEvent &aKey)
	{
	switch(aKey.iCode)
		{
		case ' ':
			if (iTest->SubState()==0)
				{
				iTest->TestL(iGroup->GroupWin()->OrdinalPosition()==0);
				iTest->IncSubState();
				}
			break;
		case EKeyEscape:
			iTest->EndAutoForegroundTest();
			break;
		}
	}

CMcConnectionDef::CMcConnectionDef(TMultiConTest *aTest) : CMcConnectionBase(aTest)
	{
	}

void CMcConnectionDef::ConstructL()
	{
	CMcConnectionBase::ConstructL();
	iGroup=new(ELeave) CTWindowGroup(this);
	iGroup->ConstructL();
	iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	TSize screenSize=iGroup->Size();
	TInt winWidth=screenSize.iWidth/3-10;
	TInt winHeight=(screenSize.iHeight/2)-10;
	CMcWindowDef *win=new(ELeave) CMcWindowDef(iTest);
	win->SetUpL(TPoint(5+winWidth/2,screenSize.iHeight/2),TSize(winWidth,winHeight),iGroup,*iGc);
	iWin=win;
	iWs.Flush();
	}

//
// CMcWindow, base class //
//

CMcWindowBase::CMcWindowBase(TMultiConTest *aTest) : CTWin(), iTest(aTest)
	{
	}

void CMcWindowBase::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iWin.SetBackgroundColor(iBack);
	Activate();
	AssignGC(aGc);
	}

//
// CMcWindow, window used to test multiple connections //
//

CMcWindow::CMcWindow(TMultiConTest *aTest) : CMcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(221);
	}

void CMcWindow::PointerL(const TPointerEvent &pointer,const TTime &)
	{
	if (pointer.iType==TPointerEvent::EButton1Down)
		{
		switch(iTest->SubState())
			{
			case 1:
				iTest->TestL(Client()->iGroup->GroupWin()->OrdinalPosition()==1);
				iTest->IncSubState();
				break;
			}
		}
	}

void CMcWindow::Draw()
	{
	iGc->Clear();
	TBuf<0x40> buf;
	switch(iTest->SubState())
		{
		case 1:
			buf.Copy(_L("Click on me"));
			break;
		case 0:
		case 2:
		case 3:
			buf.Copy(_L(""));
			break;
		default:
			buf.Copy(_L("ERROR"));
		}
	iGc->DrawText(buf, TPoint(10,20));
	}

//
// CMcWindowAf, Auto foreground version of CMcWindow //
//

CMcWindowAf::CMcWindowAf(TMultiConTest *aTest) : CMcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(150);
	}

void CMcWindowAf::PointerL(const TPointerEvent &pointer,const TTime &)
	{
	if (pointer.iType==TPointerEvent::EButton1Down)
		{
		switch(iTest->SubState())
			{
			case 2:
				iTest->TestL(Client()->iGroup->GroupWin()->OrdinalPosition()==0);
				iTest->IncSubState();
				break;
			}
		}
	}

void CMcWindowAf::Draw()
	{
	iGc->Clear();
	TBuf<0x40> buf;
	switch(iTest->SubState())
		{
		case 1:
		case 3:
			break;
		case 0:
			buf.Copy(_L("Press <Space>"));
			break;
		case 2:
			buf.Copy(_L("Click on me"));
			break;
		default:
			buf.Copy(_L("ERROR"));
		}
	iGc->DrawText(buf, TPoint(10,20));
	}

//

CMcWindowGroupAf::CMcWindowGroupAf(CTClient *aClient) : CTWindowGroup(aClient)
	{}

void CMcWindowGroupAf::KeyL(const TKeyEvent &aKey, const TTime &)
	{
	((CMcConnectionAf *)iClient)->KeyL(aKey);
	}

//
// CMcWindowDef, Default auto foreground version of CMcWindow //
//

CMcWindowDef::CMcWindowDef(TMultiConTest *aTest) : CMcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(236);
	}

void CMcWindowDef::PointerL(const TPointerEvent &pointer,const TTime &)
	{
	if (pointer.iType==TPointerEvent::EButton1Down)
		{
		switch(iTest->SubState())
			{
			case 3:
				iTest->TestL(Client()->iGroup->GroupWin()->OrdinalPosition()==0);
				iTest->IncSubState();
				break;
			}
		}
	}

void CMcWindowDef::Draw()
	{
	iGc->Clear();
	TBuf<0x40> buf;
	switch(iTest->SubState())
		{
		case 0:
		case 1:
		case 2:
			break;
		case 3:
			buf.Copy(_L("Click on me"));
			break;
		default:
			buf.Copy(_L("ERROR"));
		}
	iGc->DrawText(buf, TPoint(10,20));
	}

//

TInt TMultiConTest::SubState() const
	{
	return(iSubState);
	}

void TMultiConTest::IncSubState()
	{
	if (iSubState==EMaxSubState)
		EndAutoForegroundTest();
	else
		{
		iSubState++;
		iConn1->SubStateChanged();
		iConn2->SubStateChanged();
		iConn3->SubStateChanged();
		}
	}

TestState TMultiConTest::DoTestL()
	{
	switch(iState)
		{
		case 0:
			LogSubTest(_L("MultiCon 1"),1);
			iState++;
			return(EContinue);
		default:
			return(EFinished);
		}
//	return(ENext);
 	}
