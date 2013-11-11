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
// Test capture key
// 
//

#include <e32std.h>
#include <e32svr.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

enum THotKeyRetValues
	{
	EHotKeyOkay,
	EHotKeyRetry,
	EHotKeyFail,
	};

struct SHotKeyParams
	{
	THotKey hotKey;
	TUint keyCode;
	TUint mod_mask;
	TUint modifiers;
	};

struct SHotKeyTestParams
	{
	TText *txt;			// Text message telling user what to do
	};

LOCAL_D SHotKeyParams HotKeys[]={
	{EHotKeyEnableLogging,'e',EModifierFunc|EModifierCtrl|EModifierShift,0},
	{EHotKeyDisableLogging,'d',EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc},
	{EHotKeyOfDeath,'q',0},
	{EHotKeyOfDeath,'w',0},
	};
	
LOCAL_D SHotKeyTestParams HotKeyTests[]={
	{(TText *)_S("Use \"e\" to enable logging")},
	{(TText *)_S("Use \"<Alt>d\" to disable logging")},
	{(TText *)_S("Use \"q\" to kill the foreground app")},
	{(TText *)_S("Use \"w\" to kill the foreground app")},
	{(TText *)_S("Use \"<Cntrl><Alt><Shift>K\" to kill the foreground app")},
	};

LOCAL_D TBool HotKeyTestIsDeathTest[]={EFalse, EFalse, ETrue, ETrue, ETrue};

struct SErrorHotKey
	{
	THotKey hotKey;
	TUint keyCode;
	TUint mod_mask;
	TUint modifiers;
	};

LOCAL_D SErrorHotKey errorKeys[]={
	{EHotKeyEnableLogging,'a',EModifierFunc|EModifierShift,EModifierFunc|EModifierCtrl},
	{EHotKeyDisableLogging,'1',0,EModifierFunc},
	{EHotKeyEnableLogging,3,EModifierCtrl,EModifierCtrl|EModifierShift},
	{EHotKeyDisableLogging,'a',EModifierFunc|EModifierShift,EModifierFunc|EModifierCtrl},
	{(THotKey)100,'1',0,EModifierFunc},
	{(THotKey)200, 3,EModifierCtrl,EModifierCtrl|EModifierShift},
	};

const TInt numHotKeys=sizeof(HotKeys)/sizeof(HotKeys[0]);
const TInt numHotKeyTests=sizeof(HotKeyTests)/sizeof(HotKeyTests[0]);
const TInt numErrorKeys=sizeof(errorKeys)/sizeof(errorKeys[0]);

class SHKWindow;
class THotKeyTest;
class SHKConnection;

class SHKDeath : public CActive
	{
public:
	SHKDeath(TInt aPriority);
	void SetConnection(SHKConnection *aConn);
	virtual void DoCancel();
	virtual void RunL();
	void Request();
private:
	SHKConnection *iConn;
	};

class SHKWindowGroup : public CTWindowGroup
	{
public:
	SHKWindowGroup(CTClient *aClient);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	};

class SHKConnection : public CTClient
	{
public:
	SHKConnection(THotKeyTest *aTest, TInt aMode);
	~SHKConnection();
	void ConstructL();
	void KeyL(const TKeyEvent &aKey);
	void SubStateChangedL();
	void CompleteL();
protected:
	TInt iMode;
	SHKDeath iDeath;
	THotKeyTest *iTest;
	CTWin *iWin;
	static TInt iMainWinId;
	};

class SHKWindow : public CTWin
	{
public:
	SHKWindow(THotKeyTest *aTest);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw();
protected:
	THotKeyTest *iTest;
	TRgb iBack;
	};

class SHKWindow2 : public CTWin
	{
public:
	SHKWindow2();
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw();
	};

class THotKeyTest : public CTestBase
	{
public:
	THotKeyTest();
	~THotKeyTest();
	TestState DoTestL();
	void ConstructL();
	void EndCaptureKeyTest();
	TInt SubState() const;
	void IncSubStateL();
	void BadParamsL();
private:
	SHKConnection *iConn1;
	TSize iWinSize;
	TInt iState;
	TInt iSubState;
	TBool iIsInActiveScheduler;
	};

TInt SHKConnection::iMainWinId;

TInt SubThread(TAny *);

GLDEF_C CTestBase *CreateHotKeyTest()
	{
	return(new(ELeave) THotKeyTest());
	}

THotKeyTest::THotKeyTest() : CTestBase(_L("Hot Key"))
	{}

THotKeyTest::~THotKeyTest()
	{
	User::SetJustInTime(ETrue);
	for(TInt index=0;index<numHotKeys;index++)
		{
		Client()->iWs.ClearHotKeys(HotKeys[index].hotKey);
		Client()->iWs.RestoreDefaultHotKey(HotKeys[index].hotKey);
		}
	delete iConn1;
	if (iIsInActiveScheduler)
		CActiveScheduler::Stop();
	}

void THotKeyTest::EndCaptureKeyTest()
	{
	Request();
	}

void THotKeyTest::ConstructL()
	{
	iConn1=new(ELeave) SHKConnection(this, EFalse);
	iConn1->ConstructL();
	for(TInt index=0;index<numHotKeys;index++)
		User::LeaveIfError(Client()->iWs.SetHotKey(HotKeys[index].hotKey, HotKeys[index].keyCode,HotKeys[index].mod_mask,HotKeys[index].modifiers));
	User::SetJustInTime(EFalse);
	}

//
// SHKDeath //
//

SHKDeath::SHKDeath(TInt aPriority) : CActive(aPriority)
	{
	CActiveScheduler::Add(this);
	}

void SHKDeath::SetConnection(SHKConnection *aConn)
	{
	iConn=aConn;
	}

void SHKDeath::DoCancel()
	{
	}

void SHKDeath::RunL()
	{
	iConn->CompleteL();
	}

void SHKDeath::Request()
	{
	SetActive();
	}

//

SHKWindowGroup::SHKWindowGroup(CTClient *aClient) : CTWindowGroup(aClient)
	{}

void SHKWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &)
	{
	((SHKConnection *)iClient)->KeyL(aKey);
	}

//
// SHKConnection

SHKConnection::SHKConnection(THotKeyTest *aTest, TInt aMode) : iMode(aMode), iDeath(100), iTest(aTest)
	{
	iDeath.SetConnection(this);
	}

SHKConnection::~SHKConnection()
	{
	iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	CTWin::Delete(iWin);
	}

void SHKConnection::KeyL(const TKeyEvent &aKey)
	{
	if (iTest)
		{
		if (aKey.iCode==EKeyEnter && !HotKeyTestIsDeathTest[iTest->SubState()])
			iTest->IncSubStateL();
		else if (aKey.iCode==EKeyEscape)
			iTest->AbortL();
		}
	}

void SHKConnection::CompleteL()
	{
	iTest->IncSubStateL();
	}

void SHKConnection::SubStateChangedL()
	{
	if (HotKeyTestIsDeathTest[iTest->SubState()])
		{
		RThread thread;
		iMainWinId=iGroup->GroupWin()->Identifier();
		TInt subState=iTest->SubState();
		User::After(100000);
		User::LeaveIfError(thread.Create(_L("SubThread"),SubThread,KDefaultStackSize,0x2000,0x2000,&subState,EOwnerThread));
		thread.Logon(iDeath.iStatus);
		iDeath.Request();
		thread.Resume();
		thread.Close();
		}
	iWin->Invalidate();
	iWs.Flush();
	}

void SHKConnection::ConstructL()
	{
	CTClient::ConstructL();
	iGroup=new(ELeave) SHKWindowGroup(this);
	iGroup->ConstructL();
	TSize screenSize=iGroup->Size();
	iGroup->GroupWin()->AutoForeground(EFalse);		// Don't allow clicking to cause foreground, might mess up test
	TInt winWidth;
	TInt winHeight;
	if (iMode==0)
		{
		winWidth=screenSize.iWidth/2;
		winHeight=screenSize.iHeight-10;
		SHKWindow *win=new(ELeave) SHKWindow(iTest);
		win->SetUpL(TPoint(5,5),TSize(winWidth,winHeight),iGroup,*iGc);
		iWin=win;
		}
	else
		{
		winWidth=150;
		winHeight=50;
		iGroup->GroupWin()->SetOwningWindowGroup(iMainWinId);
		SHKWindow2 *win=new(ELeave) SHKWindow2();
		win->SetUpL(TPoint((screenSize.iWidth-winWidth)/2,(screenSize.iHeight-winHeight)/2),TSize(winWidth,winHeight),iGroup,*iGc);
		iWin=win;
		}
	iWs.Flush();
	}

//
// SHKWindow, class //
//

SHKWindow::SHKWindow(THotKeyTest *aTest) : CTWin(), iTest(aTest)
	{
	iBack=TRgb::Gray256(230);
	}

void SHKWindow::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iWin.SetBackgroundColor(iBack);
	Activate();
	AssignGC(aGc);
	}

void SHKWindow::Draw()
	{
	iGc->Clear();
	iGc->DrawText(TPtrC(HotKeyTests[iTest->SubState()].txt), TPoint(10,20));
	iGc->DrawText(TPtrC(_L("Press <Enter> when tested okay")), TPoint(10,35));
	iGc->DrawText(TPtrC(_L("or escape to abort tests")), TPoint(10,50));
	}

//
// SHKWindow2, class //
//

SHKWindow2::SHKWindow2() : CTWin()
	{
	}

void SHKWindow2::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iWin.SetBackgroundColor(TRgb(0,0,0));
	Activate();
	AssignGC(aGc);
	}

void SHKWindow2::Draw()
	{
	iGc->SetBrushColor(TRgb::Gray4(1));
	iGc->Clear();
	iGc->SetPenColor(TRgb::Gray4(3));
	iGc->DrawText(TPtrC(_L("Kill me!!!")), TPoint(10,15));
	}

//

TInt THotKeyTest::SubState() const
	{
	return(iSubState);
	}

void THotKeyTest::IncSubStateL()
	{
	if (iSubState==(numHotKeyTests-1))
		EndCaptureKeyTest();
	else
		{
		iSubState++;
		iConn1->SubStateChangedL();
		}
	}

void THotKeyTest::BadParamsL()
	{
	TInt resCount=Client()->iWs.ResourceCount();
	for(TInt index=0;index<numErrorKeys;index++)
		TestL(Client()->iWs.SetHotKey(errorKeys[index].hotKey, errorKeys[index].keyCode,errorKeys[index].mod_mask,errorKeys[index].modifiers)==KErrArgument);
	TestL(Client()->iWs.ResourceCount()==resCount);
	}

TestState THotKeyTest::DoTestL()
	{
	switch(iState)
		{
		case 0:
			LogSubTest(_L("Errors"),1);
			BadParamsL();
			LogSubTest(_L("CaptureKey"),2);
			iState++;
			return(EContinue);
		default:
			return(EFinished);
		}
 	}

//======================================================//
// Sub thread to do tests and get shot by window server //
//======================================================//

void SubThreadMain()
	{
	CActiveScheduler *TheActiveScheduler=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(TheActiveScheduler);
	SHKConnection *conn=new(ELeave) SHKConnection(NULL, ETrue);
	conn->ConstructL();
	CActiveScheduler::Start();
	delete TheActiveScheduler;
	}

TInt SubThread(TAny *)
	{
	CTrapCleanup* CleanUpStack=CTrapCleanup::New();
	TRAPD(err,SubThreadMain());
	delete CleanUpStack;
	return(err);
	}
