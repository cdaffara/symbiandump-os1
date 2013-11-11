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
// Test Wserv password features
// 
//

#include <e32std.h>
#include <e32hal.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

//#define LOG_TESTS

#define THE_PASSWORD _L("pass")

enum TPasswordState 
	{
	ENotStarted,
	EWaitForSwitchOff,
	EWaitForEnter,
	EWaitForA,
	EWaitForSwitchOff2,
	EWaitForSwitchOff3,
	EWaitForSwitchOff4,
	EWaitForSwitchOff5,
	EWaitForEnter2,
	EPasswordFinished,
	};

class CPasswordTest;

class CPasswordWindowGroup : public CTWindowGroup
	{
public:
	CPasswordWindowGroup(CTClient *aClient, CPasswordTest *aTest);
	void ConstructL();
	void PasswordL(const TTime &aTime);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void SwitchOn(const TTime &aTime);
private:
	CPasswordTest *iTest;
	};

class CPasswordWindowGroup2 : public CTWindowGroup
	{
public:
	CPasswordWindowGroup2(CTClient *aClient, CPasswordTest *aTest);
	void ConstructL();
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
private:
	CPasswordTest *iTest;
	};

class CPasswordWindow : public CTWin
	{
public:
	CPasswordWindow(CPasswordTest *aTest);
	void Draw();
private:
	CPasswordTest *iTest;
	};

class CPasswordTest : public CTestBase
	{
public:
	CPasswordTest();
	~CPasswordTest();
	TestState DoTestL();
	void ConstructL();
	void Fail(TInt aWhere);
	void EnterKeyPressed(TPasswordState aNewState);
	void SwitchOn();
	void TestComplete(TPasswordState aNewState);
	void PasswordMsgReceivedL();
	TPasswordState PasswordState() const;
	void SetPassState(TPasswordState aPassState);
	void StartMainPasswordTestL();
	void StartOnceADayPasswordTestL();
	void TurnOffAndOn();
private:
	CPasswordWindowGroup *iGroup;
	CPasswordWindowGroup2 *iGroup2;
	CPasswordWindow *iPassWin;
	CTBlankWindow *iBlankWin;
	TSize iWinSize;
	TInt iState;
	TPasswordState iPassState;
	TBool iPasswordTestFailed;
	};

GLDEF_C CTestBase *CreatePasswordTest()
	{
	return(new(ELeave) CPasswordTest());
	}

//

CPasswordWindowGroup::CPasswordWindowGroup(CTClient *aClient, CPasswordTest *aTest) :
	CTWindowGroup(aClient),
	iTest(aTest)
	{
	__DECLARE_NAME(_S("CPasswordWindowGroup"));
	}

void CPasswordWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	GroupWin()->EnableOnEvents();
	}

void CPasswordWindowGroup::SwitchOn(const TTime &)
	{
	iTest->SwitchOn();
	}

void CPasswordWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode==EKeyEscape)
		iTest->TestComplete(EPasswordFinished);
	switch(iTest->PasswordState())
		{
		case EWaitForEnter:
			if (aKey.iCode==EKeyEnter)
				iTest->EnterKeyPressed(EWaitForA);
			break;
		case EWaitForEnter2:
			if (aKey.iCode==EKeyEnter)
				iTest->TestComplete(EPasswordFinished);
			break;
		default:;
		}
	}

void CPasswordWindowGroup::PasswordL(const TTime &)
	{
	iTest->TestL(iGroupWin.OrdinalPosition()==0);
	iTest->PasswordMsgReceivedL();
	}

//

CPasswordWindowGroup2::CPasswordWindowGroup2(CTClient *aClient, CPasswordTest *aTest) :
	CTWindowGroup(aClient),
	iTest(aTest)
	{
	__DECLARE_NAME(_S("CPasswordWindowGroup"));
	}

void CPasswordWindowGroup2::ConstructL()
	{
	CTWindowGroup::ConstructL();
	iGroupWin.CaptureKey('a',0,0);
	}

void CPasswordWindowGroup2::KeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (iTest->PasswordState()!=EPasswordFinished)
		{
		if (iTest->PasswordState()!=EWaitForA)
			{
		#if defined(LOG_TESTS)
			TLogMessageText buf;
			_LIT(KBadKey,"Bad Key  Code=%d(%c) Scan=%d(%c) Rep=%d");
			buf.AppendFormat(KBadKey,aKey.iCode,aKey.iCode,aKey.iScanCode,aKey.iScanCode,aKey.iRepeats);
			Client()->LogMessage(buf);
		#endif
			if (aKey.iRepeats==0)
				iTest->Fail(1);
			}
		else
			{
			if (aKey.iCode!='a')
				iTest->Fail(2);
			iTest->TestComplete(EWaitForSwitchOff2);
			}
		}
	}

//

CPasswordWindow::CPasswordWindow(CPasswordTest *aTest) :
	iTest(aTest)
	{
	}

void CPasswordWindow::Draw()
	{
	iGc->Clear();
	switch(iTest->PasswordState())
		{
		case EWaitForSwitchOff:
			iGc->DrawText(_L("Please wait, turning off & on [1]"),TPoint(10,20));
			break;
		case EWaitForSwitchOff2:
			iGc->DrawText(_L("Please wait, turning off & on [2]"),TPoint(10,20));
			break;
		case EWaitForSwitchOff3:
			iGc->DrawText(_L("Please wait, turning off & on [3]"),TPoint(10,20));
			break;
		case EWaitForSwitchOff4:
			iGc->DrawText(_L("Please wait, turning off & on [4]"),TPoint(10,20));
			break;
		case EWaitForSwitchOff5:
			iGc->DrawText(_L("Please wait, turning off & on [5]"),TPoint(10,20));
			break;
		case EWaitForEnter:
			iGc->DrawText(_L("Try the key of death, then..."),TPoint(10,20));
			iGc->DrawText(_L("Press 'a', then..."),TPoint(10,40));
			iGc->DrawText(_L("Press Enter"),TPoint(10,60));
			break;
		case EWaitForEnter2:
			iGc->DrawText(_L("Press Enter"),TPoint(10,20));
			break;
		case EWaitForA:
			iGc->DrawText(_L("Press 'a'"),TPoint(10,20));
		case EPasswordFinished:
			break;
		default:;
		}
	}

//

CPasswordTest::CPasswordTest() : CTestBase(_L("Password"))
	{}

CPasswordTest::~CPasswordTest()
	{
	delete iBlankWin;
	delete iPassWin;
	delete iGroup;
	delete iGroup2;
	}

#if defined(LOG_TESTS)
void CPasswordTest::Fail(TInt aWhere)
#else
void CPasswordTest::Fail(TInt /*aWhere*/)
#endif
	{
#if defined(LOG_TESTS)
	TLogMessageText buf;
	_LIT(KFailed,"Password Failed at %d (%d,%d)");
	buf.AppendFormat(KFailed,aWhere,iState,iPassState);
	Client()->LogMessage(buf);
#endif
	iPasswordTestFailed=ETrue;
	Request();
	}

void CPasswordTest::TurnOffAndOn()
	{
/*#if defined(LOG_TESTS)
	TLogMessageText buf;
	_LIT(KSettingTime,"Setting Off Timer");
	buf.Append(KSettingTime);
	Client()->LogMessage(buf);
#endif*/
	RTimer timer;
	timer.CreateLocal();
	TTime time;
	time.HomeTime();
	time+=TTimeIntervalSeconds(7);	// For some reason the O/S won't switch off for less than 6 seconds
	TRequestStatus status;
	timer.At(status,time);
	UserHal::SwitchOff();
	User::WaitForRequest(status);
#if !defined(__WINS__)
	TRawEvent event;
	event.Set(TRawEvent::ESwitchOn);
	UserSvr::AddEvent(event);
#endif
/*#if defined(LOG_TESTS)
	TLogMessageText buf;
	_LIT(KTimerOff,"Timer Gone Off (P=%d,S=%d)");
	buf.AppendFormat(KTimerOff,iState,iPassState);
	Client()->LogMessage(buf);
#endif*/
	}

TPasswordState CPasswordTest::PasswordState() const
	{
	return(iPassState);
	}

void CPasswordTest::SetPassState(TPasswordState aPassState)
	{
	iPassState=aPassState;
	iPassWin->DrawNow();
	Client()->iWs.Flush();
/*#if defined(LOG_TESTS)
	TLogMessageText buf;
	_LIT(PassTestState,"Password Test(%d), State=%d");
	buf.AppendFormat(PassTestState,iState,aPassState);
	Client()->LogMessage(buf);
#endif*/
	switch(aPassState)
		{
		case EWaitForSwitchOff:
		case EWaitForSwitchOff2:
		case EWaitForSwitchOff3:
		case EWaitForSwitchOff4:
		case EWaitForSwitchOff5:
			TurnOffAndOn();
			break;
		default:;
		}
	}

void CPasswordTest::SwitchOn()
	{
/*#if defined(LOG_TESTS)
	TLogMessageText buf;
	_LIT(KTimerOff,"Switch On (P=%d,S=%d)");
	buf.AppendFormat(KTimerOff,iState,iPassState);
	Client()->LogMessage(buf);
#endif*/
	switch (iPassState)
		{
		case EWaitForSwitchOff:
			SetPassState(EWaitForEnter);
			break;
		case EWaitForSwitchOff2:
			SetPassState(EWaitForSwitchOff3);
			break;
		case EWaitForSwitchOff3:
			SetPassState(EWaitForSwitchOff4);
			break;
		case EWaitForSwitchOff4:
			{
			SetPassState(EWaitForSwitchOff5);
			TTime time;
			time.HomeTime();
			time+=TTimeIntervalHours(24);
			User::SetHomeTime(time);
			}
			break;
		case EWaitForSwitchOff5:
			SetPassState(EWaitForEnter2);
			break;
		default:;
		}
	}

void CPasswordTest::PasswordMsgReceivedL()
	{
	TestL(iPassWin->BaseWin()->OrdinalPosition()==0);
	if (iPassState==EWaitForSwitchOff3 || iPassState==EWaitForSwitchOff4)
		Fail(3);
	}

void CPasswordTest::EnterKeyPressed(TPasswordState aNewState)
	{
	iPassWin->BaseWin()->SetOrdinalPosition(-1);
	SetPassState(aNewState);
	Client()->iWs.PasswordEntered();
	}

void CPasswordTest::TestComplete(TPasswordState aNewState)
	{
	Request();
	SetPassState(aNewState);
	iPassState=aNewState;
	}

void CPasswordTest::ConstructL()
	{
	iGroup2=new(ELeave) CPasswordWindowGroup2(Client(),this);
	iGroup2->ConstructL();
	iGroup=new(ELeave) CPasswordWindowGroup(Client(),this);
	iGroup->ConstructL();
	iPassWin=new(ELeave) CPasswordWindow(this);
	iPassWin->ConstructL(*iGroup);
	iPassWin->AssignGC(*Client()->iGc);
	iPassWin->Activate();
	iBlankWin=new(ELeave) CTBlankWindow();
	iBlankWin->ConstructL(*iGroup);
	iBlankWin->SetVisible(EFalse);
	iBlankWin->Activate();
	iState=ENotStarted;
	}

void CPasswordTest::StartMainPasswordTestL()
	{
	if (iPassWin->BaseWin()->PasswordWindow(EPasswordAlways)!=KErrNone)
		{
		DisplayDialog(_L("Can't do password tests"),_L("Password window"),_L("already exists"), Client()->iGroup->GroupWin());
		AbortL();
		}
	SetPassState(EWaitForSwitchOff);
	}

void CPasswordTest::StartOnceADayPasswordTestL()
	{
	TestL(iPassWin->BaseWin()->PasswordWindow(EPasswordOnceADay)==KErrNone);
	}

TestState CPasswordTest::DoTestL()
	{
	if (iPasswordTestFailed)
		TestL(ETrue);
	switch(iState)
		{
		case 0:
			LogSubTest(_L("Password 1"),1);
			StartMainPasswordTestL();
			iState++;
			return(EContinue);
		case 1:
			LogSubTest(_L("Password 2"),2);
			StartOnceADayPasswordTestL();
			iState++;
			return(EContinue);
		default:
			return(EFinished);
		}
	}
