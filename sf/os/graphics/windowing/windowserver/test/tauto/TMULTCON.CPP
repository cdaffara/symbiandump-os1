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
// Test multiple connections to the window server
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TMULTCON.H"

const TInt EMaxSubState=3;

//#define LOGGING on		//Uncomment this line to get extra logging

_LIT(ClickOnMe,"Click on me");
_LIT(KError,"ERROR");


CTMultiCon::CTMultiCon(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}

CTMultiCon::~CTMultiCon()
	{
	delete iConn1;
	delete iConn2;
	delete iConn3;
	delete iTimeOut;
	}

void CTMultiCon::EndAutoForegroundTest()
	{
#if defined(LOGGING)
	_LIT(KLog,"EndAutoForegroundTest  SubState %d");
	LOG_MESSAGE2(KLog,iSubState);
#endif
	iConn1->iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	iConn2->iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	iConn3->iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	CActiveScheduler::Stop();
	}

void CTMultiCon::ConstructL()
	{
	iTimeOut=new(ELeave) CTimeOut();
	iTimeOut->ConstructL();
	TheClient->iWs.SetPointerCursorArea(iTest->iNormalPointerCursorArea);
	iScreenSize=TSize(TheClient->iScreen->SizeInPixels());
	iConn3=new(ELeave) CMcConnectionDef(this);
	iConn3->ConstructL();
	iConn2=new(ELeave) CMcConnection(this);
	iConn2->ConstructL();
	iConn1=new(ELeave) CMcConnectionAf(this);
	iConn1->ConstructL();
	iTest->SimulateKeyDownUp(EStdKeyLeftCtrl);
	iTest->SimulateKeyDownUp(EStdKeyRightCtrl);
	TInt mods=TheClient->iWs.GetModifierState();
	_LIT(KLog,"Initial Modifiers state 0x%x (ideally should be zero)");
	LOG_MESSAGE2(KLog,mods);
	}

TInt CTMultiCon::TimeOut(TAny* aTest)		//static
	{
	static_cast<CTMultiCon*>(aTest)->TimeOut();
	return(KErrNone);
	}

void CTMultiCon::TimeOut()
	{
	TLogMessageText buf;
	_LIT(KMultiConTimeOut,"TIMEOUT: Multiple Conection Test, %d, %S");
	buf.AppendFormat(KMultiConTimeOut,iTest->iState,&iTest->iSubTitle);
	TheClient->LogMessage(buf);
	++iTimeOutCount;
	EndAutoForegroundTest();
	}


//
// CMcConnection //
//

CMcConnectionBase::CMcConnectionBase(CTMultiCon *aTest) : iTest(aTest)
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
	CTClient::SetScreenNumber(iTest->ScreenNumber());
	CTClient::ConstructL();
	User::LeaveIfError(iScreen->CreateContext(iGc));
	}

CMcConnection::CMcConnection(CTMultiCon *aTest) : CMcConnectionBase(aTest)
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

CMcConnectionAf::CMcConnectionAf(CTMultiCon *aTest) : CMcConnectionBase(aTest)
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
#if defined(LOGGING)
	_LIT(KLog1,"KeyL  SS=%d (0)  GpWinOrdPos=%d (0)  Code=%d (32)");
	_LIT(KLog2," '%c'");
	TLogMessageText buf;
	buf.Format(KLog1,iTest->SubState(),iGroup->GroupWin()->OrdinalPosition(),aKey.iCode);
	if (aKey.iCode>0)
		buf.AppendFormat(KLog2,aKey.iCode);
	iTest->LOG_MESSAGE(buf);
#endif
	switch(aKey.iCode)
		{
		case ' ':
			if (iTest->SubState()==0)
				{
				iTest->TEST(iGroup->GroupWin()->OrdinalPosition()==0);
				iTest->IncSubState();
				}
			break;
		case EKeyEscape:
			iTest->EndAutoForegroundTest();
			break;
		}
	}

CMcConnectionDef::CMcConnectionDef(CTMultiCon *aTest) : CMcConnectionBase(aTest)
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

CMcWindowBase::CMcWindowBase(CTMultiCon *aTest) : CTWin(), iTest(aTest)
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

CMcWindow::CMcWindow(CTMultiCon *aTest) : CMcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(221);
	}

void CMcWindow::PointerL(const TPointerEvent &pointer,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KLog,"Pointer  SS=%d (1)  Type=%d (%d)  GpWinOrdPos=%d (1)");
	iTest->LOG_MESSAGE5(KLog,iTest->SubState(),pointer.iType,TPointerEvent::EButton1Down,Client()->iGroup->GroupWin()->OrdinalPosition());
#endif
	if (pointer.iType==TPointerEvent::EButton1Down)
		{
		switch(iTest->SubState())
			{
			case 1:
				iTest->TEST(Client()->iGroup->GroupWin()->OrdinalPosition()==1);
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
			buf.Copy(ClickOnMe);
			break;
		case 0:
		case 2:
		case 3:
			buf.Copy(KNullDesC);
			break;
		default:
			buf.Copy(KError);
		}
	iGc->DrawText(buf, TPoint(10,20));
	}

//
// CMcWindowAf, Auto foreground version of CMcWindow //
//

CMcWindowAf::CMcWindowAf(CTMultiCon *aTest) : CMcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(150);
	}

void CMcWindowAf::PointerL(const TPointerEvent &pointer,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KLog,"PointerL  SS=%d (2)  Type=%d (%d)  GpWinOrdPos=%d (0)");
	iTest->LOG_MESSAGE5(KLog,iTest->SubState(),pointer.iType,TPointerEvent::EButton1Down,Client()->iGroup->GroupWin()->OrdinalPosition());
#endif
	if (pointer.iType==TPointerEvent::EButton1Down)
		{
		switch(iTest->SubState())
			{
			case 2:
				iTest->TEST(Client()->iGroup->GroupWin()->OrdinalPosition()==0);
				iTest->IncSubState();
				break;
			}
		}
	}

void CMcWindowAf::Draw()
	{
	_LIT(PressSpace,"Press <Space>");
	iGc->Clear();
	TBuf<0x40> buf;
	switch(iTest->SubState())
		{
		case 1:
		case 3:
			break;
		case 0:
			buf.Copy(PressSpace);
			break;
		case 2:
			buf.Copy(ClickOnMe);
			break;
		default:
			buf.Copy(KError);
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

CMcWindowDef::CMcWindowDef(CTMultiCon *aTest) : CMcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(236);
	}

void CMcWindowDef::PointerL(const TPointerEvent &pointer,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KLog,"PointerL  SS=%d (3)  Type=%d (%d)  GpWinOrdPos=%d (0)");
	iTest->LOG_MESSAGE5(KLog,iTest->SubState(),pointer.iType,TPointerEvent::EButton1Down,Client()->iGroup->GroupWin()->OrdinalPosition());
#endif
	if (pointer.iType==TPointerEvent::EButton1Down)
		{
		switch(iTest->SubState())
			{
			case 3:
				iTest->TEST(Client()->iGroup->GroupWin()->OrdinalPosition()==0);
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
			buf.Copy(ClickOnMe);
			break;
		default:
			buf.Copy(KError);
		}
	iGc->DrawText(buf, TPoint(10,20));
	}

//

TInt CTMultiCon::SubState() const
	{
	return(iSubState);
	}

void CTMultiCon::IncSubState()
	{
	if (iSubState==EMaxSubState)
		EndAutoForegroundTest();
	else
		{
		iSubState++;
	#if defined(LOGGING)
		_LIT(KLog,"New SubState %d");
		LOG_MESSAGE2(KLog,iSubState);
	#endif
		iConn1->SubStateChanged();
		iConn2->SubStateChanged();
		iConn3->SubStateChanged();
		TheClient->WaitForRedrawsToFinish();
		SendEvents();
		}
	}

void CTMultiCon::SendEvents()
	{
#if defined(LOGGING)
	_LIT(KLog,"Sending event for substate %d");
	LOG_MESSAGE2(KLog,iSubState);
#endif

	switch (iSubState)
		{
	case 0:
		iTest->SimulateKeyDownUp(EStdKeySpace);
		break;
	case 1:
		iTest->SimulatePointerDownUp(iScreenSize.iWidth/6+5,iScreenSize.iHeight/4);
		break;
	case 2:
		iTest->SimulatePointerDownUp(iScreenSize.iWidth/2,iScreenSize.iHeight/4);
		break;
	case 3:
		iTest->SimulatePointerDownUp(iScreenSize.iWidth/3,3*iScreenSize.iHeight/4-5);
		break;
	default:
		TEST(EFalse);
		}
	TheClient->iWs.Flush();
	}

void CTMultiCon::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTestMultiCon1,"MultiCon 1");
	_LIT(KTimeOut,"Test Timed Out after %dsecs.  SubState=%d");
	((CTMultiConStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	
	if (!TestBase()->ConfigurationSupportsPointerEventTesting())
	    {
	    INFO_PRINTF1(_L("Test skipped because config does not support pointer event testing"));
	    TestComplete();
	    return;
	    }
    
	switch(++iTest->iState)
		{
/**

  @SYMTestCaseID GRAPHICS-WSERV-0289
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test multiple connections to the window server
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Make mutiple connections to the window server and check
  				  that the connections are made corectly
  
  @SYMTestExpectedResults The connections are made correctly
 
*/		
		case 1:
			((CTMultiConStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0289"));
			iTest->LogSubTest(KTestMultiCon1);
			iTimeOut->Start(KTimeOutAfter,TCallBack(CTMultiCon::TimeOut,this));
			SendEvents();
			CActiveScheduler::Start();
			iTimeOut->Cancel();
			TEST(iTimeOutCount==0);
			if (iTimeOutCount!=0)
				LOG_MESSAGE3(KTimeOut,KTimeOutAfter/1000000,iSubState);
			break;
		default:
			((CTMultiConStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTMultiConStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTMultiConStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(MultiCon)
