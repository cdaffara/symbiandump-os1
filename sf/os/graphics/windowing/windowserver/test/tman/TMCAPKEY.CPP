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
#include <e32keys.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

enum TTestMode
	{
	EModeNormalCapture,
	EModeWaitingForFirstDown,
	EModeKeyUpsAndDowns,
	EModeAbortKeys,
	};
enum TCapKeyRetValues
	{
	ECapKeyOkay,
	ECapKeyRetry,
	ECapKeyFail,
	};

struct SCapKeyParams
	{
	TInt connIndex;		// Connection that should capture this key
	TUint keyCode;
	TUint modifier_mask;
	TUint modifiers;
	};

struct SCapKeyTestParams
	{
	TText *txt;			// Text message telling user what to do
	TInt connIndex;		// Connection that receive the key press
	TUint keyCode;		// Expected key code
	TUint modifier_mask;// Modifiers mask for expected modifiers
	TUint modifiers;	// Expected modifiers
	};

struct SAbortKeyTests
	{
	TUint keyCode;		// Expected key code
	TUint modifier_mask;// Modifiers mask for expected modifiers
	TUint modifiers;	// Expected modifiers
	};

LOCAL_D SCapKeyParams capKeys[]={
	{1,19,EModifierFunc|EModifierCtrl,EModifierFunc|EModifierCtrl},
	{2,'b',EModifierFunc|EModifierCtrl,EModifierFunc},
	{2,'n',EModifierFunc|EModifierCtrl,EModifierFunc},
	{0,3,EModifierFunc|EModifierCtrl,EModifierFunc|EModifierCtrl},
	{2,'q',0,0},
	{2,'w',0,0},
	};
	
LOCAL_D SCapKeyParams capUpAndDownKeys[]={
	{2,EStdKeySpace,EModifierFunc|EModifierCtrl|EModifierShift,0},
	{2,EStdKeyEnter,EModifierFunc|EModifierCtrl|EModifierShift,EModifierShift},
	};
	
LOCAL_D SCapKeyTestParams capKeyTests[]={
	{(TText *)_S("Press q"),2,'q',EModifierFunc|EModifierCtrl|EModifierShift,0},
	//{(TText *)_S("Press <Alt>q"),2,'q',EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc},
	{(TText *)_S("Press <Alt>w"),2,'w',EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc},
	{(TText *)_S("Press a"),0,'a',0},
	//{(TText *)_S("Press <Ctrl><Alt>a"),1,1,EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc|EModifierCtrl},
	{(TText *)_S("Press <Ctrl><Alt>s"),1,19,EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc|EModifierCtrl},
	//{(TText *)_S("Press <Alt>b"),2,'b',EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc},
	{(TText *)_S("Press <Alt>n"),2,'n',EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc},
	{(TText *)_S("Press <Ctrl><Alt>b"),0,2,EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc|EModifierCtrl},
	//{(TText *)_S("Press <Alt>c"),0,'c',EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc},
	{(TText *)_S("Press <Alt>d"),0,'d',EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc},
	{(TText *)_S("Press <Ctrl><Alt>c"),0,3,EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc|EModifierCtrl},
//
// Capture ups and downs
//
	{(TText *)_S("Press <Space>"),2,EStdKeySpace,EModifierFunc|EModifierCtrl|EModifierShift,0},
	{(TText *)_S("Press <Enter>"),0,EStdKeyEnter,EModifierShift,0},
	{(TText *)_S("Press <Shift><Space>"),0,EStdKeySpace,EModifierFunc|EModifierCtrl|EModifierShift,EModifierShift},
	{(TText *)_S("Press <Shift><Enter>"),2,EStdKeyEnter,EModifierShift,EModifierShift},
//
// Priority keys
//
	{(TText *)_S("Press <Ctrl>c"),0,0,0,0},
	{(TText *)_S("Press <Ctrl>d"),0,0,0,0},
	{(TText *)_S("Press z"),0,0,0,0},
	};

LOCAL_D SAbortKeyTests abortKeyTests[]={
	{3,EModifierCtrl,EModifierCtrl},
	{4,EModifierCtrl,EModifierCtrl},
	{'z',EModifierFunc|EModifierCtrl|EModifierShift,0}
	};

struct SErrorCapKey
	{
	TInt keyCode;
	TUint modifier_mask;
	TUint modifiers;
	};

LOCAL_D SErrorCapKey errorKeys[]={
	{'a',EModifierFunc|EModifierShift,EModifierFunc|EModifierCtrl},
	{'1',0,EModifierFunc},
	{3,EModifierCtrl,EModifierCtrl|EModifierShift},
	};

const TInt numCapKeys=sizeof(capKeys)/sizeof(capKeys[0]);
const TInt numTests=sizeof(capKeyTests)/sizeof(capKeyTests[0]);
const TInt numErrorKeys=sizeof(errorKeys)/sizeof(errorKeys[0]);
const TInt numAbortKeyTests=sizeof(abortKeyTests)/sizeof(abortKeyTests[0]);
const TInt numUpDownKeys=sizeof(capUpAndDownKeys)/sizeof(capUpAndDownKeys[0]);
const TInt numUpDownKeyTests=4;
const TInt numCapKeyTests=numTests-numAbortKeyTests-numUpDownKeyTests;

LOCAL_D TInt capKeyHandle[numCapKeys];
LOCAL_D TInt capUpDownKeyHandle[numUpDownKeys];

class CCKWindow;
class TCaptureKeyTest;
class CCKAbortConnection;

class CCKConnectionBase : public CTClient
	{
public:
	CCKConnectionBase(TCaptureKeyTest *aTest);
	void ConstructL();
protected:
	TCaptureKeyTest *iTest;
	};

class CCKConnection : public CCKConnectionBase
	{
public:
	CCKConnection(TCaptureKeyTest *aTest, TInt aConnIndex);
	~CCKConnection();
	void ConstructL();
	void CheckKeyL(const SCapKeyTestParams &aKeyTest, const TKeyEvent &aKey);
	void SubStateChanged();
	void KeyTestL(TInt aBool);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void CheckUpDownL(const SCapKeyTestParams &aKeyTest, const TKeyEvent &aKey);
	void CheckUpDownL(const TKeyEvent &aKey);
	inline TInt Index() {return iConnIndex;}
protected:
	TInt iConnIndex;
	CCKWindow *iWin;
	};

class CAbortHandler : public CTEventBase
	{
public:
	CAbortHandler(RWsSession *aWs, CCKAbortConnection *aConnection);
	~CAbortHandler();
	void Request();
protected:
	void DoCancel();
	void doRunL();
protected:
	CCKAbortConnection *iConnection;
	};

class CCKAbortConnection : public CCKConnectionBase
	{
public:
	CCKAbortConnection(TCaptureKeyTest *aTest);
	~CCKAbortConnection();
	void ConstructL();
	void ReceivedAbortEventL(TInt aHandle, TKeyEvent *aKey);
	void Foreground();
protected:
	CAbortHandler *iAbortHandler;
	TInt iAbortKeyIndex;
	};

class CCKWindow : public CTWin
	{
public:
	CCKWindow(TCaptureKeyTest *aTest, TInt aConnIndex);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw();
protected:
	TInt iConnIndex;
	TCaptureKeyTest *iTest;
	TRgb iBack;
	};

class CCKWindowGroup : public CTWindowGroup
	{
public:
	CCKWindowGroup(CTClient *aClient, TCaptureKeyTest *aTest);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void KeyDownL(const TKeyEvent &aKey,const TTime &);
	void KeyUpL(const TKeyEvent &aKey,const TTime &);
	void CheckUpDown(const SCapKeyTestParams &aKeyTest, const TKeyEvent &aKey);
private:
	TCaptureKeyTest *iTest;
	};

class TCaptureKeyTest : public CTestBase
	{
public:
	TCaptureKeyTest();
	~TCaptureKeyTest();
	TestState DoTestL();
	void ConstructL();
	void EndCaptureKeyTest();
	TInt SubState() const;
	void IncSubState();
	void BadParamsL();
	void CapKeyPurgingL();
	void SetUpPriorityKeyTest();
	void TestComplete();
	void CaptureUpsAndDownsTest();
	TTestMode Mode();
	void SetMode(TTestMode aMode);
private:
	CCKConnection *iConn1;
	CCKConnection *iConn2;
	CCKConnection *iConn3;
	CCKAbortConnection *iAbortConn;
	TSize iWinSize;
	TInt iState;
	TInt iSubState;
	TTestMode iMode;
	};

GLDEF_C CTestBase *CreateMultiCaptureKeyTest()
	{
	return(new(ELeave) TCaptureKeyTest());
	}

//
// CAbortHandler

CAbortHandler::CAbortHandler(RWsSession *aWs, CCKAbortConnection *aConnection) : CTEventBase(aWs, 10), iConnection(aConnection)
	{
	}

CAbortHandler::~CAbortHandler()
	{
	Cancel();
	}

void CAbortHandler::Request()
	{
	iWs->PriorityKeyReady(&iStatus);
	SetActive();
	}

void CAbortHandler::DoCancel()
	{
	iWs->PriorityKeyReadyCancel();
	}

void CAbortHandler::doRunL()
	{
	TWsPriorityKeyEvent abortEvent;
	iWs->GetPriorityKey(abortEvent);
	iConnection->ReceivedAbortEventL(abortEvent.Handle(),abortEvent.Key());
	}

//
// CCKConnection

CCKConnectionBase::CCKConnectionBase(TCaptureKeyTest *aTest) : iTest(aTest)
	{
	}

void CCKConnectionBase::ConstructL()
	{
	CTClient::ConstructL();
	iGroup=new(ELeave) CCKWindowGroup(this, iTest);
	iGroup->ConstructL();
	}

CCKConnection::CCKConnection(TCaptureKeyTest *aTest, TInt aConnIndex) : CCKConnectionBase(aTest), iConnIndex(aConnIndex)
	{
	}

CCKConnection::~CCKConnection()
	{
	for(TInt index=0;index<numCapKeys;index++)
		if (capKeys[index].connIndex==iConnIndex)
			iGroup->GroupWin()->CancelCaptureKey(capKeyHandle[index]);
	for(TInt index2=0;index2<numUpDownKeys;index2++)
		if (capUpAndDownKeys[index2].connIndex==iConnIndex)
			iGroup->GroupWin()->CancelCaptureKeyUpAndDowns(capUpDownKeyHandle[index2]);
	CTWin::Delete(iWin);
	}

void CCKConnection::SubStateChanged()
	{
	iWin->Invalidate();
	iWs.Flush();
	}

void CCKConnection::ConstructL()
	{
	CCKConnectionBase::ConstructL();
	for(TInt index=0;index<numCapKeys;index++)
		if (capKeys[index].connIndex==iConnIndex)
			capKeyHandle[index]=User::LeaveIfError(iGroup->GroupWin()->CaptureKey(capKeys[index].keyCode,capKeys[index].modifier_mask,capKeys[index].modifiers));
	for(TInt index2=0;index2<numUpDownKeys;index2++)
		if (capUpAndDownKeys[index2].connIndex==iConnIndex)
			capUpDownKeyHandle[index2]=User::LeaveIfError(iGroup->GroupWin()->CaptureKeyUpAndDowns(capUpAndDownKeys[index2].keyCode,
								capUpAndDownKeys[index2].modifier_mask,capUpAndDownKeys[index2].modifiers));
	TSize screenSize=iGroup->Size();
	TInt winWidth=screenSize.iWidth/2;
	TInt winHeight=screenSize.iHeight-10;
	iGroup->GroupWin()->AutoForeground(EFalse);		// Don't allow clicking to cause foreground, might mess up test
	CCKWindow *win=new(ELeave) CCKWindow(iTest, iConnIndex);
	win->SetUpL(TPoint(5,5),TSize(winWidth,winHeight),iGroup,*iGc);
	iWin=win;
	}

void CCKConnection::KeyTestL(TInt aBool)
	{
	if (!aBool)
		{
		CTDialog *dialog=new(ELeave) CTDialog();
		TInt ret=0;
		dialog->ConstructLD(*iGroup,*iGc);
		dialog->SetTitle(_L("Invalid key press"));
		dialog->SetLine1(_L("Try again?"));
		dialog->SetNumButtons(2);
		dialog->SetButtonText(0,_L("Fail"));
		dialog->SetButtonText(1,_L("Retry"));
		ret=dialog->Display();
		User::Leave(ret==1 ? ECapKeyRetry : ECapKeyFail);
		}
	}

void CCKConnection::CheckKeyL(const SCapKeyTestParams &aKeyTest, const TKeyEvent &aKey)
	{
	KeyTestL(aKeyTest.connIndex==iConnIndex);
	KeyTestL(aKeyTest.keyCode==aKey.iCode);
	KeyTestL(aKeyTest.modifiers==(aKey.iModifiers&aKeyTest.modifier_mask));
	}

void CCKConnection::CheckUpDownL(const SCapKeyTestParams &aKeyTest, const TKeyEvent &aKey)
	{
	KeyTestL(aKeyTest.connIndex==iConnIndex);
	KeyTestL(aKeyTest.keyCode==(TUint)aKey.iScanCode);
	KeyTestL(aKeyTest.modifiers==(aKey.iModifiers&aKeyTest.modifier_mask));
	}

void CCKConnection::KeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (iTest->Mode()==EModeNormalCapture)
		{
		if (aKey.iCode==EKeyEscape)
			iTest->AbortL();
		TRAPD(ret,CheckKeyL(capKeyTests[iTest->SubState()],aKey));
		if (ret==ECapKeyOkay)
			iTest->IncSubState();
		else
			iTest->TestL(ret==ECapKeyRetry);
		}
	}

void CCKConnection::CheckUpDownL(const TKeyEvent &aKey)
	{
	CheckUpDownL(capKeyTests[iTest->SubState()],aKey);
	}

CCKAbortConnection::CCKAbortConnection(TCaptureKeyTest *aTest) : CCKConnectionBase(aTest)
	{
	}

CCKAbortConnection::~CCKAbortConnection()
	{
	iGroup->GroupWin()->RemovePriorityKey(3,EModifierCtrl,EModifierCtrl);
	delete iAbortHandler;
	}

void CCKAbortConnection::ConstructL()
	{
	CCKConnectionBase::ConstructL();
	User::LeaveIfError(iGroup->GroupWin()->AddPriorityKey(abortKeyTests[0].keyCode,abortKeyTests[0].modifier_mask,abortKeyTests[0].modifiers));
	User::LeaveIfError(iGroup->GroupWin()->AddPriorityKey(abortKeyTests[1].keyCode,abortKeyTests[1].modifier_mask,abortKeyTests[1].modifiers));
	User::LeaveIfError(iGroup->GroupWin()->AddPriorityKey(abortKeyTests[2].keyCode,abortKeyTests[2].modifier_mask,abortKeyTests[2].modifiers));
	iAbortHandler=new(ELeave) CAbortHandler(&iWs, this);
	iAbortHandler->Construct();
	iWs.Flush();
	}

void CCKAbortConnection::ReceivedAbortEventL(TInt aHandle, TKeyEvent *aKey)
	{
	iTest->TestL(aHandle==(TInt)iGroup);
	iTest->TestL(aKey->iCode==abortKeyTests[iAbortKeyIndex].keyCode);
	iTest->TestL((aKey->iModifiers&abortKeyTests[iAbortKeyIndex].modifier_mask)==abortKeyTests[iAbortKeyIndex].modifiers);
	iAbortKeyIndex++;
	iTest->IncSubState();
	}

void CCKAbortConnection::Foreground()
	{
	iGroup->GroupWin()->SetOrdinalPosition(0);
	iWs.Flush();
	}

//
// CCKWindowGroup class //
//

CCKWindowGroup::CCKWindowGroup(CTClient *aClient, TCaptureKeyTest *aTest) : CTWindowGroup(aClient), iTest(aTest)
	{}

void CCKWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &time)
	{
	/*TLogMessageText buf(_S("##Recieved Key: "));
	buf.Append(aKey.iScanCode);
	buf.Append('(');
	buf.AppendNum(aKey.iScanCode);
	buf.Append(')');
	if (!iCurWin)
		{
		buf.Append('(');
		buf.AppendNum(((CCKConnection *)iClient)->Index());
		buf.Append(')');
		}
	iClient->iWs.LogMessage(buf);*/
	if (iCurWin)
		iCurWin->WinKeyL(aKey,time);
	else
		((CCKConnection *)iClient)->KeyL(aKey,time);
	}

void CCKWindowGroup::KeyDownL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iScanCode!=EStdKeyLeftShift && aKey.iScanCode!=EStdKeyRightShift)
		{
		if (iTest->Mode()==EModeWaitingForFirstDown)
			iTest->SetMode(EModeKeyUpsAndDowns);
		if (iTest->Mode()==EModeKeyUpsAndDowns)
			((CCKConnection *)iClient)->CheckUpDownL(aKey);
		}
	}

void CCKWindowGroup::KeyUpL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iScanCode!=EStdKeyLeftShift && aKey.iScanCode!=EStdKeyRightShift)
		{
		if (iTest->Mode()==EModeKeyUpsAndDowns)
			{
			((CCKConnection *)iClient)->CheckUpDownL(aKey);
			iTest->IncSubState();
			}
		}
	}

//
// CCKWindow, class //
//

CCKWindow::CCKWindow(TCaptureKeyTest *aTest, TInt aConnIndex) : CTWin(), iConnIndex(aConnIndex), iTest(aTest)
	{
	iBack=TRgb::Gray256(230);
	}

void CCKWindow::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iWin.SetBackgroundColor(iBack);
	Activate();
	AssignGC(aGc);
	}

void CCKWindow::Draw()
	{
	iGc->Clear();
	if (iConnIndex==0)
		iGc->DrawText(TPtrC(capKeyTests[iTest->SubState()].txt), TPoint(10,20));
	}

//

TCaptureKeyTest::TCaptureKeyTest() : CTestBase(_L("Capture Key"))
	{}

TCaptureKeyTest::~TCaptureKeyTest()
	{
	delete iConn1;
	delete iConn2;
	delete iConn3;
	delete iAbortConn;
	}

void TCaptureKeyTest::ConstructL()
	{
	iAbortConn=new(ELeave) CCKAbortConnection(this);
	iAbortConn->ConstructL();
	iConn3=new(ELeave) CCKConnection(this,2);
	iConn3->ConstructL();
	iConn2=new(ELeave) CCKConnection(this,1);
	iConn2->ConstructL();
	iConn1=new(ELeave) CCKConnection(this,0);
	iConn1->ConstructL();
	iMode=EModeNormalCapture;
	}

TInt TCaptureKeyTest::SubState() const
	{
	return(iSubState);
	}

void TCaptureKeyTest::IncSubState()
	{
	iSubState++;
	iConn1->SubStateChanged();
	iConn2->SubStateChanged();
	iConn3->SubStateChanged();
	switch(iMode)
		{
		case EModeNormalCapture:
			if (iSubState==numCapKeyTests)
				TestComplete();
			break;
		case EModeKeyUpsAndDowns:
			if (iSubState==(numCapKeyTests+numUpDownKeyTests))
				TestComplete();
			break;
		case EModeAbortKeys:
			if (iSubState==(numCapKeyTests+numUpDownKeyTests+numAbortKeyTests))
				TestComplete();
			break;
		default:
			break;
		}
	}

void TCaptureKeyTest::CapKeyPurgingL()
	{
	TInt base=Client()->iWs.ResourceCount();
	RWindowGroup groupWin(Client()->iWs);
	User::LeaveIfError(groupWin.Construct(ENullWsHandle));
	TInt capHandle=User::LeaveIfError(groupWin.CaptureKey('a',EModifierFunc,EModifierFunc));
	User::LeaveIfError(groupWin.CaptureKey('b',EModifierFunc,EModifierFunc));
	TestL(Client()->iWs.ResourceCount()==(base+3));
	groupWin.CancelCaptureKey(capHandle);
	TestL(Client()->iWs.ResourceCount()==(base+2));
	groupWin.Close();
	TestL(Client()->iWs.ResourceCount()==base);		// Check it also freed the extra capture key
	}

void TCaptureKeyTest::BadParamsL()
	{
	TInt resCount=Client()->iWs.ResourceCount();
	for(TInt index=0;index<numErrorKeys;index++)
		TestL(Client()->iGroup->GroupWin()->CaptureKey(errorKeys[index].keyCode,errorKeys[index].modifier_mask,errorKeys[index].modifiers)==KErrArgument);
	TestL(Client()->iWs.ResourceCount()==resCount);
	}

void TCaptureKeyTest::TestComplete()
	{
	Request();
	}

void TCaptureKeyTest::SetMode(TTestMode aMode)
	{
	iMode=aMode;
	}

TTestMode TCaptureKeyTest::Mode()
	{
	return(iMode);
	}

void TCaptureKeyTest::SetUpPriorityKeyTest()
	{
	iMode=EModeAbortKeys;
	iAbortConn->Foreground();
	}

void TCaptureKeyTest::CaptureUpsAndDownsTest()
	{
	iMode=EModeWaitingForFirstDown;
	}

TestState TCaptureKeyTest::DoTestL()
	{
	switch(iState)
		{
		case 0:	// Dummy one to let capture key tests run
			iState++;
			return(EContinue);
		case 1:
			LogSubTest(_L("Capture ups and downs"),1);
			CaptureUpsAndDownsTest();
			iState++;
			return(EContinue);
		case 2:
			LogSubTest(_L("Abort key"),1);
			SetUpPriorityKeyTest();
			iState++;
			return(EContinue);
		case 3:
			LogSubTest(_L("Errors"),1);
			BadParamsL();
			CapKeyPurgingL();
			LogSubTest(_L("CaptureKey"),1);
			iState++;
			return(ENext);
		default:
			return(EFinished);
		}
//	return(ENext);
 	}
