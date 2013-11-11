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
// Converted from TMan test code (TMPNTKEY.CPP) May 2001
// Test Pointer move/drag buffer
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TPNTKEY.H"

#define LOGGING on	//Uncomment this line to get more logging

TInt CTPntKeyWindow::iTestScanCodes[ENumPntKeyTests]={'A','B',0,'C',EStdKeyEnter,'Y'};
TUint CTPntKeyWindow::iTestCodes[ENumPntKeyTests]={'a','B',0,'c',EKeyEnter,'y'};
TUint CTPntKeyWindow::iTestModifiers[ENumPntKeyTests]={0,EModifierShift,0,0,0,0};

CTPntKeyWindow::CTPntKeyWindow(CTPntKey *aTest) : iTest(aTest)
	{}

CTPntKeyWindow::~CTPntKeyWindow()
	{
	}

void CTPntKeyWindow::SetUpLD(TPoint pos,TSize size,CTWinBase *parent)
	{
	ConstructExtLD(*parent,pos,size);
	iWin.SetPointerGrab(ETrue);
	iKey1=TRect(size.iWidth*1/16,size.iHeight/2,size.iWidth*5/16,size.iHeight);
	iKey2=TRect(size.iWidth*6/16,size.iHeight/2,size.iWidth*10/16,size.iHeight);
	iKey3=TRect(size.iWidth*11/16,size.iHeight/2,size.iWidth*15/16,size.iHeight);
	iWin.AddKeyRect(iKey1,'A',EFalse);
	iWin.AddKeyRect(iKey2,'B',EFalse);
	AssignGC(*Client()->iGc);
	BaseWin()->EnableOnEvents();
	Activate();
	}

void CTPntKeyWindow::Error(TInt aWhere)
	{
	iTest->Failed(aWhere);
	}

void CTPntKeyWindow::NextKey()
	{
	if (++iKeyCount!=ENumPntKeyTests)
		{
	#if defined(LOGGING)
		_LIT(KLog,"Next Key  KeyCount=%d");
		iTest->LOG_MESSAGE2(KLog,iKeyCount);
	#endif
		if (iKeyCount==2)
			iWin.RemoveAllKeyRects();
		else if (iKeyCount==3)
			iWin.AddKeyRect(iKey3,'C',EFalse);
		else if (iKeyCount==4)
			{
	#if !defined(__WINS__)
			if (iTest->NoDigitiser())
				{
				return;
				}
	#endif
			iWin.RemoveAllKeyRects();
			iWin.AddKeyRect(TRect(Client()->iScreen->SizeInPixels()),'Z',EFalse);
			}
		else if (iKeyCount==5)
			{
			iWin.RemoveAllKeyRects();
			Client()->iWs.Flush();
			User::After(500000);	// Wait half a second
			iWin.AddKeyRect(TRect(Client()->iScreen->SizeInPixels()),'Y',ETrue);
			}
		Invalidate();
		}
	SendEvent();
	}

void CTPntKeyWindow::SendEvent()
	{
	TheClient->WaitForRedrawsToFinish();
#if defined(LOGGING)
	_LIT(KLog,"SendEvent  KeyCount=%d");
	iTest->LOG_MESSAGE2(KLog,iKeyCount);
#endif
	switch(iKeyCount)
		{
		case 0:
			SimulatePointerDownUp(iKey1);
			break;
		case 1:
			iTest->TestBase()->SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftShift);
			SimulatePointerDownUp(iKey2);
			iTest->TestBase()->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftShift);
			break;
		case 2:
		case 5:
			{
			TPoint pos=Position();
			TSize size=Size();
			iTest->TestBase()->SimulatePointerDownUp(pos.iX+size.iWidth/2,pos.iY+size.iHeight/4);
			}
			break;
		case 3:
			SimulatePointerDownUp(iKey3);
			break;
		case 4:
			iTest->TestBase()->SimulateKeyDownUp(EStdKeyEnter);
			break;
		}
	}

void CTPntKeyWindow::SimulatePointerDownUp(const TRect& aRect)
	{
	iTest->TestBase()->SimulatePointerDownUp((aRect.iTl.iX+aRect.iBr.iX)/2,(aRect.iTl.iY+aRect.iBr.iY)/2);
	}

void CTPntKeyWindow::Test(TInt aCheck)
	{
	if (!aCheck)
		Error(3);
	}

void CTPntKeyWindow::KeyUpL(const TKeyEvent &aKey,const TTime&)
	{
	if (aKey.iScanCode==iTestScanCodes[iKeyCount])
		NextKey();
	}

void CTPntKeyWindow::KeyDownL(const TKeyEvent &aKey,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KLog,"KeyDownL  ScanCode=%d '%c' (%d)  KeyCount=%d");
	iTest->LOG_MESSAGE5(KLog,aKey.iScanCode,aKey.iScanCode,iTestScanCodes[iKeyCount],iKeyCount);
#endif
	if (aKey.iScanCode!=EStdKeyLeftFunc && aKey.iScanCode!=EStdKeyRightFunc && 
		 aKey.iScanCode!=EStdKeyLeftAlt && aKey.iScanCode!=EStdKeyRightAlt &&
		 aKey.iScanCode!=EStdKeyLeftCtrl && aKey.iScanCode!=EStdKeyRightCtrl &&
		 aKey.iScanCode!=EStdKeyLeftShift && aKey.iScanCode!=EStdKeyRightShift && 
		 aKey.iScanCode!=EStdKeyOff &&
		 aKey.iScanCode!=EStdKeyEscape)
		Test(aKey.iScanCode==iTestScanCodes[iKeyCount]);
	}

void CTPntKeyWindow::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode!=EKeyEscape)
		{
#if defined(LOGGING)
		_LIT(KLog1,"WinKeyL1  ScanCode=%d (%d)  Code=%d '%c' (%d)");
		_LIT(KLog2,"WinKeyL2  ScanCode=%d  Modifiers=0x%x (0x%x) KeyCount=%d");
		iTest->LOG_MESSAGE6(KLog1,aKey.iScanCode,iTestScanCodes[iKeyCount],aKey.iCode,aKey.iCode,iTestCodes[iKeyCount]);
		iTest->LOG_MESSAGE5(KLog2,aKey.iScanCode,aKey.iModifiers&EModifierMask,iTestModifiers[iKeyCount]&EModifierMask,iKeyCount);
#endif
		Test(aKey.iScanCode==iTestScanCodes[iKeyCount]);
		Test(aKey.iCode==iTestCodes[iKeyCount]);
		Test((aKey.iModifiers&EModifierMask)==(iTestModifiers[iKeyCount]&EModifierMask));
		}
	}

void CTPntKeyWindow::SwitchOn(const TTime &)
	{
#if defined(LOGGING)
	_LIT(KLog,"SwitchOn  KeyCount=%d");
	iTest->LOG_MESSAGE2(KLog,iKeyCount);
#endif
	if (iKeyCount==4)
		NextKey();
 	else if (iKeyCount!=5)
		Error(2);
	}

void CTPntKeyWindow::PointerL(const TPointerEvent &aPointer,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KLog,"Pointer Event  Type=%d  Pos=(%d,%d)  PPos=(%d,%d)  KeyCount=%d");
	iTest->LOG_MESSAGE7(KLog,aPointer.iType,aPointer.iPosition.iX,aPointer.iPosition.iY
						,aPointer.iParentPosition.iX,aPointer.iParentPosition.iY,iKeyCount);
#endif
	if (aPointer.iType==TPointerEvent::EButton1Down)
		{
		if (iKeyCount!=2)
			Error(1);
		else
			NextKey();
		}
	}

void CTPntKeyWindow::DrawButton(const TRect &aRect, const TDesC &aText)
	{
	iGc->DrawRect(aRect);
	iGc->DrawText(aText, TPoint((aRect.iBr.iX+aRect.iTl.iX)/2,(aRect.iBr.iY+aRect.iTl.iY)/2));
	}

void CTPntKeyWindow::Draw()
	{
	iGc->SetBrushColor(TRgb::Gray4(0));
	iGc->SetPenColor(TRgb::Gray4(3));
	iGc->Clear();
	DrawButton(iKey1,_L("A"));
	DrawButton(iKey2,_L("B"));
	DrawButton(iKey3,_L("C"));
	switch(iKeyCount)
		{
		case 0:
			iGc->DrawText(_L("Click on 'A'"), TPoint(10,20));
			break;
		case 1:
			iGc->DrawText(_L("Shift-Click on 'B'"), TPoint(10,20));
			break;
		case 2:
			iGc->DrawText(_L("Click anywhere in this window"), TPoint(10,20));
			break;
		case 3:
			iGc->DrawText(_L("Click on 'C'"), TPoint(10,20));
			break;
		case 4:
#if defined(__WINS__)	// Can't emulate touching dig when switched off under WINS
			iGc->DrawText(_L("Switch off and on (or press Enter)"), TPoint(10,20));
#else
			iGc->DrawText(_L("Switch off, then touch the screen to switch on"), TPoint(10,20));
#endif
			break;
		case 5:
#if defined(__WINS__)	// Can't emulate touching dig when switched off under WINS
			iGc->DrawText(_L("Touch anywhere in the window"), TPoint(10,20));
#else
			iGc->DrawText(_L("Switch off and touch the screen to switch on again"), TPoint(10,20));
#endif
			break;
		}
	}

CTPntKey::CTPntKey(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

CTPntKey::~CTPntKey()
	{
	HAL::Set(HALData::EPenDisplayOn,iOldPointerState);
	CTWin::Delete(iWin);
	Client()->ResetFocus();
	delete iTimeOut;
	}

TInt CTPntKey::TimeOut(TAny* aTest)		//static
	{
	static_cast<CTPntKey*>(aTest)->TimeOut();
	return(KErrNone);
	}

void CTPntKey::TimeOut()
	{
	TLogMessageText buf;
	_LIT(KPntKeyTimeOut,"TIMEOUT: Pointer Key, %d, %S");
	buf.AppendFormat(KPntKeyTimeOut,iState,&TestBase()->iSubTitle);
	TheClient->LogMessage(buf);
	Failed(4);
	}

void CTPntKey::Failed(TInt aWhere)
	{
	_LIT(KLog,"Failed at %d");
	LOG_MESSAGE2(KLog,aWhere);
	if (!iFailed)
		{
		iFailed=ETrue;
		Client()->iGroup->ClearCurrentWindow();
		}
	}

void CTPntKey::ConstructL()
	{
	TInt mods=TheClient->iWs.GetModifierState();
	if (mods&EModifierCapsLock)
		iTest->SimulateKeyDownUp(EStdKeyCapsLock);
	//Make sure all the keys we test are in the up state
	iTest->SimulateKeyDownUp(EStdKeyLeftShift);
	iTest->SimulateKeyDownUp(EStdKeyRightShift);
	iTest->SimulateKeyDownUp(EStdKeyLeftFunc);
	iTest->SimulateKeyDownUp(EStdKeyLeftCtrl);
	iTest->SimulateKeyDownUp(EStdKeyRightCtrl);
	mods=TheClient->iWs.GetModifierState();
	_LIT(KLog,"Initial Modifiers state 0x%x (ideally should be zero)");
	LOG_MESSAGE2(KLog,mods);
	TheClient->iScreen->SetScreenMode(0);		//May sure we are in the right screen size mode
	TheClient->iScreen->SetAppScreenMode(0);
	TheClient->iWs.SetPointerCursorArea(TestBase()->iNormalPointerCursorArea);
	CTPntKeyWindow *win=new(ELeave) CTPntKeyWindow(this);
	win->SetUpLD(TPoint(20,20),Client()->iScreen->SizeInPixels()-TSize(40,40),Client()->iGroup);
	iWin=win;
	Client()->iGroup->SetCurrentWindow(iWin);
	iNoDigitiser=EFalse;
	TInt err=HAL::Get(HALData::EPenDisplayOn,iOldPointerState);
	if (err==KErrNotSupported)
		{
		iNoDigitiser=ETrue;
		}
	else if (err==KErrNone)
		{
		err=HAL::Set(HALData::EPenDisplayOn,ETrue);
		if (err==KErrNotSupported)
			iNoDigitiser=(!iOldPointerState);
		}
	else
		{
		TEST(EFalse);
		}
	TheClient->WaitForRedrawsToFinish();	//Make sure all pending events have been delt with (redraw events are lowest priority)
	iTimeOut=new(ELeave) CTimeOut();
	iTimeOut->ConstructL();
	iTimeOut->Start(KTimeOutAfter,TCallBack(CTPntKey::TimeOut,this));
	}

void CTPntKey::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTestName,"Key set 1");
	TEST(!iFailed);
	if (iFailed)
		{
		_LIT(KLog,"Test Failed  Substate=%d  KeyCount=%d");
		LOG_MESSAGE3(KLog,iTest->iState,iWin->KeyCount());
		}
	((CTPntKeyStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0200

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test Pointer move/drag buffer

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Exercise the pointer move/drag buffer and check
					that it functions correctly

@SYMTestExpectedResults The buffer functions correctly
*/
		case 1:
			((CTPntKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0200"));
			iTest->LogSubTest(KTestName);
			if (TestBase()->ConfigurationSupportsPointerEventTesting())
			    {
			    iWin->SendEvent();
			    TheClient->Flush();
			    }
			else
			    {
			    INFO_PRINTF1(_L("Test skipped because config does not support pointer event testing"));
			    }
			break;
		case 2:
			((CTPntKeyStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTPntKeyStep*)iStep)->CloseTMSGraphicsStep();
			if (TestBase()->ConfigurationSupportsPointerEventTesting())
			    {
                iTimeOut->Cancel();
			    }
			TestComplete();
			break;
		}
	((CTPntKeyStep*)iStep)->RecordTestResultL();
	}
	
__WS_CONSTRUCT_STEP__(PntKey)
