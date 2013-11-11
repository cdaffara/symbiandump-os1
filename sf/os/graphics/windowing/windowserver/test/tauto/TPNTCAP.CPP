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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TPNTCAP.H"

#define LOGGING on

_LIT(ClickOnMe,"Click on me");

//
// CMcConnection

CPcConnection::CPcConnection(CTPointerCapture *aTest) : iTest(aTest)
	{
	}

CPcConnection::~CPcConnection()
	{
	}

void CPcConnection::ConstructL()
	{
	CTClient::SetScreenNumber(iTest->TestBase()->ScreenNumber());
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

CPcWindowBase::CPcWindowBase(CTPointerCapture *aTest) : CTWin(), iTest(aTest)
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
#if defined(LOGGING)
	_LIT(KLog1,"Pointer Event Recieved  Type=%S  State=%d  SubState=%d");
	_LIT(KDown,"Down");
	_LIT(KUp,"Up");
	iTest->LOG_MESSAGE4(KLog1,&(pointer.iType?KUp():KDown()),iTest->State(),iTest->SubState());
#endif
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
			iTest->IncSubStateL(ret==ENeedsDDEvent);
		}
#if defined(LOGGING)
	_LIT(KLog2,"End of processing Pointer Event");
	iTest->LOG_MESSAGE(KLog2);
#endif
	}

void CPcWindowBase::DragDropL(const TPointerEvent &pointer,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KLog,"CPcWindowBase::DragDropL  Type=%d  State=%d  SubState=%d");
	iTest->LOG_MESSAGE4(KLog,pointer.iType,iTest->State(),iTest->SubState());
#endif
	switch(iTest->State())
		{
		case ENormalCaptureWithoutFocus:
			if (iTest->SubState()==0)
				break;
		case ECaptureDisabled:
		case ENormalCapture:
		case ECaptureAllGroups:
			iTest->TestFailed();
			break;
		}
	if (pointer.iType==TPointerEvent::EButton1Down && !iTest->iFailed)
		{
		if (DragDrop()==EFailed)
			iTest->TestFailed();
		else
			iTest->GotDDL();
		}
	}
//

CPcWindowMain::CPcWindowMain(CTPointerCapture *aTest) : CPcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(236);
	}

TPointerCheckRet CPcWindowMain::PointerDown()
	{
#if defined(LOGGING)
	_LIT(KLog1,"CPcWindowMain::PointerDown  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog1,iTest->State(),iTest->SubState());
#endif
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
#if defined(LOGGING)
	_LIT(KLog2,"CPcWindowMain::PointerDown  FAILED  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog2,iTest->State(),iTest->SubState());
#endif
	return(EFailed);
	}

TPointerCheckRet CPcWindowMain::DragDrop()
	{
#if defined(LOGGING)
	_LIT(KLog1,"CPcWindowMain::DragDrop  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog1,iTest->State(),iTest->SubState());
#endif
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
#if defined(LOGGING)
	_LIT(KLog2,"CPcWindowMain::DragDrop  FAILED  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog2,iTest->State(),iTest->SubState());
#endif
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
				buf.Copy(ClickOnMe);
				break;
			}
		}
	else
		{
		switch(iTest->SubState())
			{
			case EMainWindow:
				buf.Copy(ClickOnMe);
				break;
			case EChildWindow:
			case EOtherGroup:
			case EOtherSession:
				break;
			case ERootWindow:
				{
				_LIT(ClickOnRoot,"Click on the root window");
				if (iTest->State()<ENormalCaptureWithoutFocus)
					buf.Copy(ClickOnRoot);
				else
					{
					_LIT(AndEscape,", then press <Escape>");
					buf.Copy(ClickOnRoot);
					buf.Append(AndEscape);
					}
				}
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
		case EKeyEnter:
			if (iTest->SubState()==ERootWindow)
				iTest->IncSubStateL(EFalse);
			break;
		}
	}

//

CPcWindowChild::CPcWindowChild(CTPointerCapture *aTest) : CPcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(85);
	}

TPointerCheckRet CPcWindowChild::PointerDown()
	{
#if defined(LOGGING)
	_LIT(KLog1,"CPcWindowChild::PointerDown  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog1,iTest->State(),iTest->SubState());
#endif
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
#if defined(LOGGING)
	_LIT(KLog2,"CPcWindowChild::PointerDown  FAILED  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog2,iTest->State(),iTest->SubState());
#endif
	return(EFailed);
	}

TPointerCheckRet CPcWindowChild::DragDrop()
	{
#if defined(LOGGING)
	_LIT(KLog1,"CPcWindowChild::PointerDown  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog1,iTest->State(),iTest->SubState());
#endif
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
#if defined(LOGGING)
	_LIT(KLog2,"CPcWindowChild::PointerDown  FAILED  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog2,iTest->State(),iTest->SubState());
#endif
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
				buf.Copy(ClickOnMe);
				break;
			default:
				break;
			}
		}
	CPcWindowBase::Draw(buf);
	}

//

CPcWindowNickFocusGroup::CPcWindowNickFocusGroup(CTPointerCapture *aTest, CTClient *aClient) : CTWindowGroup(aClient), iTest(aTest)
	{
	}

void CPcWindowNickFocusGroup::KeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode==EKeyEscape)
		iTest->IncSubStateL(EFalse);
	}

//

CPcWindowAltGroup::CPcWindowAltGroup(CTPointerCapture *aTest) : CPcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(236);
	}

TPointerCheckRet CPcWindowAltGroup::PointerDown()
	{
#if defined(LOGGING)
	_LIT(KLog1,"CPcWindowAltGroup::PointerDown  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog1,iTest->State(),iTest->SubState());
#endif
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
#if defined(LOGGING)
	_LIT(KLog2,"CPcWindowAltGroup::PointerDown  FAILED  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog2,iTest->State(),iTest->SubState());
#endif
	return(EFailed);
	}

TPointerCheckRet CPcWindowAltGroup::DragDrop()
	{
#if defined(LOGGING)
	_LIT(KLog1,"CPcWindowAltGroup::DragDrop  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog1,iTest->State(),iTest->SubState());
#endif
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
#if defined(LOGGING)
	_LIT(KLog2,"CPcWindowAltGroup::DragDrop  FAILED  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog2,iTest->State(),iTest->SubState());
#endif
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
				buf.Copy(ClickOnMe);
				break;
			}
		}
	else
		{
		switch(iTest->SubState())
			{
			case EOtherGroup:
				buf.Copy(ClickOnMe);
				break;
			default:
				break;
			}
		}
	CPcWindowBase::Draw(buf);
	}

//

CPcWindowAltConnection::CPcWindowAltConnection(CTPointerCapture *aTest) : CPcWindowBase(aTest)
	{
	iBack=TRgb::Gray256(236);
	}

TPointerCheckRet CPcWindowAltConnection::PointerDown()
	{
#if defined(LOGGING)
	_LIT(KLog1,"CPcWindowAltConnection::PointerDown  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog1,iTest->State(),iTest->SubState());
#endif
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
#if defined(LOGGING)
	_LIT(KLog2,"CPcWindowAltConnection::PointerDown  FAILED  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog2,iTest->State(),iTest->SubState());
#endif
	return(EFailed);
	}

TPointerCheckRet CPcWindowAltConnection::DragDrop()
	{
#if defined(LOGGING)
	_LIT(KLog1,"CPcWindowAltConnection::DragDrop  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog1,iTest->State(),iTest->SubState());
#endif
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
#if defined(LOGGING)
	_LIT(KLog2,"CPcWindowAltConnection::DragDrop  FAILED  State=%d  SubState=%d");
	iTest->LOG_MESSAGE3(KLog2,iTest->State(),iTest->SubState());
#endif
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
				buf.Copy(ClickOnMe);
				break;
			default:
				break;
			}
		}
	CPcWindowBase::Draw(buf);
	}

//

CTPointerCapture::CTPointerCapture(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	iSubState=0;
	iFailed=EFalse;
	}

CTPointerCapture::~CTPointerCapture()
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

void CTPointerCapture::TestFailed()
	{
	if (iFailed)
		{
		_LIT(KLog,"Test Failed  State=%d  SubState=%d");
		LOG_MESSAGE3(KLog,iTest->iState,iSubState);
		}
	__ASSERT_DEBUG(iFailed==EFalse,AutoPanic(ETManPanicPcFailed));
	TInt dRet=1;
#if !defined(DISABLE_FAIL_DIALOG)
	Client()->iGroup->GroupWin()->SetOrdinalPosition(0,10);	// Put error dialog on top of test windows
	TRAPD(err,dRet=doTestFailedL());
	Client()->iGroup->GroupWin()->SetOrdinalPosition(0,0);
#endif
	switch(dRet)
		{
		case 0:
			break;
		case 1:
			iFailed=ETrue;
			NextTest();
			break;
		}
	}

TInt CTPointerCapture::doTestFailedL()
	{
	CTDialog *dialog=new(ELeave) CTDialog();
	dialog->SetTitle(_L("Pointer capture test failed"));
	dialog->SetNumButtons(2);
	dialog->SetButtonText(0,_L("Retest"));
	dialog->SetButtonText(1,_L("Fail"));
	dialog->ConstructLD(*Client()->iGroup,*Client()->iGc);
	return dialog->Display();
	}

TInt CTPointerCapture::State() const
	{
	return(iTest->iState);
	}

TInt CTPointerCapture::SubState() const
	{
	return(iSubState);
	}

void CTPointerCapture::doIncSubStateL()
	{
	iSubState++;
	TInt max=0;
	switch(iTest->iState)
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
	//SubState 4 has been broken by something in the framework
	if (iSubState==max || iSubState==4)
		{
	#if defined(LOGGING)
		_LIT(KLog,"IncState  OldState=%d  SubState=%d");
		LOG_MESSAGE3(KLog,iTest->iState,iSubState);
	#endif
		NextTest();
		}
	StateChanged();
	TheClient->WaitForRedrawsToFinish();
	if (iSubState>0)
		SendEventsL();
	}

void CTPointerCapture::GotDDL()
	{
	if (iDDState==DDStateWaiting)
		doIncSubStateL();
	else
		iDDState=DDStateGot;
	}

void CTPointerCapture::IncSubStateL(TBool aNeedsDD)
	{
	if (!aNeedsDD)
		{
		if (iDDState!=DDStateNull)
			TestFailed();
		else
			doIncSubStateL();
		}
	else if (iDDState==DDStateGot)
		doIncSubStateL();
	else
		iDDState=DDStateWaiting;
	}

void CTPointerCapture::StateChanged()
	{
	iDDState=DDStateNull;
	iChildWin->SubStateChanged();
	iMainWin->SubStateChanged();
	iAltGroupWin->SubStateChanged();
	iAltConnectionWin->SubStateChanged();
	}

void CTPointerCapture::AbortTests()
	{
	iTest->iState=99;
	}

void CTPointerCapture::NextTest()
	{
	TEST(!iFailed);
	if (iFailed)
		{
		_LIT(KLog,"SubTest %d failed");
		LOG_MESSAGE2(KLog,iTest->iState);
		}
	++iTest->iState;
	iSubState=0;
	iFailed=EFalse;
	CaseComplete();
	_LIT(KLog,"Done NextTest  NewSubTest %d");
	LOG_MESSAGE2(KLog,iTest->iState);
	}

void CTPointerCapture::ConstructL()
	{
	TheClient->iWs.SetPointerCursorArea(iTest->iNormalPointerCursorArea);
	iScreenSize=TSize(TheClient->iScreen->SizeInPixels());
	TInt winWidth2=iScreenSize.iWidth/2-EWinBorderSize*2;
	TInt winWidth4=iScreenSize.iWidth/4-EWinBorderSize*2;
	TInt winHeight=iScreenSize.iHeight/2-EWinBorderSize*2;
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
	iAltGroupWin->SetUpL(TPoint(iScreenSize.iWidth/2+EWinBorderSize,EWinBorderSize) ,TSize(winWidth4,winHeight) ,iAltGroup);
//
	iAltConnection=new(ELeave) CPcConnection(this);
	iAltConnection->ConstructL();
	iAltConnectionWin=new(ELeave) CPcWindowAltConnection(this);
	iAltConnectionWin->SetUpL(TPoint(iScreenSize.iWidth/4*3+EWinBorderSize,EWinBorderSize) ,TSize(winWidth4,winHeight),iAltConnection->iGroup,*iAltConnection->iGc);
	iTest->DelayIfFullRomL();
	SetSelfDrive(ETrue);
	}

void CTPointerCapture::NickFocusL()
	{
	iNickFocusGroup=new(ELeave) CPcWindowNickFocusGroup(this,Client());
	iNickFocusGroup->ConstructL();
	iNickFocusGroup->GroupWin()->SetOrdinalPosition(0,2);
	iNickFocusBlankWin=new(ELeave) CTBlankWindow();
	iNickFocusBlankWin->ConstructL(*iNickFocusGroup);
	iNickFocusBlankWin->SetSize(TSize(1,1));
	iNickFocusBlankWin->Activate();
	}

void CTPointerCapture::SetCapture(TInt aCaptureFlags)
	{
#if defined(LOGGING)
	_LIT(KLog,"Change Capture  State=%d  CaptureFlags=0x%x");
	LOG_MESSAGE3(KLog,iTest->iState,aCaptureFlags);
#endif
	iMainWin->Win()->SetPointerCapture(aCaptureFlags);
	}

void CTPointerCapture::SendEventsL()
	{
#if defined(LOGGING)
	_LIT(KLog,"SendEvent  State=%d  SubState=%d");
	LOG_MESSAGE3(KLog,iTest->iState,iSubState);
#endif
	switch (iSubState)
		{
	case 0:
		iTest->SimulatePointerDownUp(iScreenSize.iWidth/4,iScreenSize.iHeight/8+EWinBorderSize/2);
		break;
	case 1:
		if (iTest->iState>ECaptureDisabled)
			{
			iTest->SimulatePointerDownUp(iScreenSize.iWidth/4,3*iScreenSize.iHeight/8-EWinBorderSize/2);
			break;
			}
	case 2:
		iTest->SimulatePointerDownUp(5*iScreenSize.iWidth/8,iScreenSize.iHeight/4);
		break;
	case 3:
		iTest->SimulatePointerDownUp(7*iScreenSize.iWidth/8,iScreenSize.iHeight/4);
		break;
	case 4:
		if (iTest->IsFullRomL())
			iTest->SimulateKeyDownUp(EStdKeyEnter);
		else
			iTest->SimulatePointerDownUp(0,0);
		if (iTest->iState==ENormalCaptureWithoutFocus)
			{
			iTest->SimulateKeyDownUp(EStdKeyEscape);
			break;
			}
		break;
	default:
		TEST(EFalse);
		}
	TheClient->iWs.Flush();
	}

void CTPointerCapture::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(TestNoCapture,"No capture");
	_LIT(TestNormalCapture,"Normal capture");
	_LIT(TestAllGroups,"All groups");
	_LIT(TestDragDrop,"Drag & Drop");
	_LIT(TestDragDropAllGroups,"Drag & Drop All groups");
	_LIT(TestWithoutFocus,"Without focus");
	StateChanged();
	
	if (!TestBase()->ConfigurationSupportsPointerEventTesting())
	    {
	    INFO_PRINTF1(_L("Test skipped because config does not support pointer event testing"));
	    TestComplete();
	    return;
	    }
	
	((CTPointerCaptureStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(iTest->iState)
		{
/**

  @SYMTestCaseID GRAPHICS-WSERV-0290
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test pointer capture can be disabled
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Disable the pointer capture to the screen and simulate
  					capture with the pointer
  
  @SYMTestExpectedResults The pointer capture has been disabled
 
*/
		case ECaptureDisabled:
			((CTPointerCaptureStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0290"));
			if (TheClient->iScreen->GetScreenNumber()>0)
				{
				_LIT(KLog,"Cannot run this test on the 2nd screen - all pointer events goto the 1st screen");
				LOG_MESSAGE(KLog);
				TestComplete();
				}
			iTest->LogSubTest(TestNoCapture);
			SetCapture(RWindowBase::TCaptureDisabled);
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0291
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test pointer capture can be enabled
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Enable the pointer capture to the screen and simulate
  					capture with the pointer
  
  @SYMTestExpectedResults The pointer capture has been enabled and works
  							correctly
 
*/
		case ENormalCapture:
			((CTPointerCaptureStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0291"));
			iTest->LogSubTest(TestNormalCapture);
			SetCapture(RWindowBase::TCaptureEnabled);
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0292
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test pointer capture can be enabled for 
  					windows belonging to any group
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Enable the pointer capture to all windows and simulate
  					capture with the pointer
  
  @SYMTestExpectedResults The pointer capture has been enabled and works
  							correctly
 
*/
		case ECaptureAllGroups:
			((CTPointerCaptureStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0292"));
			iTest->LogSubTest(TestAllGroups);
			SetCapture(RWindowBase::TCaptureEnabled|RWindowBase::TCaptureFlagAllGroups);
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0293
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test drag and drop pointer capture can be enabled for 
  					windows belonging to any group
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Enable drag and drop  pointer capture to all windows and simulate
  					capture with the pointer
  
  @SYMTestExpectedResults The pointer capture has been enabled and works
  							correctly
 
*/
		case EDragDropCapture:
			((CTPointerCaptureStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0293"));
			iTest->LogSubTest(TestDragDrop);
			SetCapture(RWindowBase::TCaptureDragDrop&~RWindowBase::TCaptureFlagAllGroups);
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0294
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test drag and drop pointer capture can be enabled
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Enable drag and drop  pointer capture and simulate
  					capture with the pointer
  
  @SYMTestExpectedResults The pointer capture has been enabled and works
  							correctly
 
*/
		case EDragDropCaptureAllGroups:
			((CTPointerCaptureStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0294"));
			iTest->LogSubTest(TestDragDropAllGroups);
			SetCapture(RWindowBase::TCaptureDragDrop);
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0295
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test pointer capture can be enabled
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Enable the pointer capture to the screen and simulate capture
  					with the pointer when the screen does not have the focus
  
  @SYMTestExpectedResults The pointer capture has been enabled and works
  							correctly
 
*/
		case ENormalCaptureWithoutFocus:
			((CTPointerCaptureStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0295"));
			iTest->LogSubTest(TestWithoutFocus);
			NickFocusL();
			SetCapture(RWindowBase::TCaptureEnabled);
			break;
		default:
			((CTPointerCaptureStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTPointerCaptureStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			return;
		}
	((CTPointerCaptureStep*)iStep)->RecordTestResultL();
	SendEventsL();
	}

__WS_CONSTRUCT_STEP__(PointerCapture)
