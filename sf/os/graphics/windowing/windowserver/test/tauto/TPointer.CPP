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
// Test pointer event handling
// Coverted from TMan test code (TMPOINTR.CPP) October 2000
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TPointer.H"


//#define LOGGING

#if defined(LOGGING)
	LOCAL_D TLogMessageText LogMessageText;
#endif


CPointerWindow::CPointerWindow(CTPointer *aTest) : CTWin(), iTest(aTest)
	{}

void CPointerWindow::SetState(TInt aState)
	{
#if defined(LOGGING)
	_LIT(KState,"CPointerWindow::SetState(%d) OldState=%d");
	LogMessageText.Format(KState,aState,iState);
	TheClient->LogMessage(LogMessageText);
#endif
	iState=aState;
	iWin.Invalidate();
	switch(aState)
		{
		case KStateWaitingForTest1:
		case KStateWaitingForTest2:
		case KStateWaitingForTest3:
		case KStateWaitingForTest4:
		case KStateWaitingForTest5:
		case KStateWaitingForTest6:
		case KStateWaitingForTest8:
		case KStateWaitingForTest7:
			iRepeatRect=TRect();
			break;
		case KStateTesting8:
			iWin.Close();
			Client()->iWs.Flush();
			User::After(500000);
			FinishedTests();
			break;
		}
	TheClient->WaitForRedrawsToFinish();
	SendEvent();
	}

void CPointerWindow::ResetTest(TInt aState)
	{
	TInt newState=KStateFinished;
	switch(aState)
		{
		case KStateTesting1:
			newState=KStateWaitingForTest1;
			break;
		case KStateTesting2:
			newState=KStateWaitingForTest2;
			break;
		case KStateTesting3:
			newState=KStateWaitingForTest3;
			break;
		case KStateTesting4:
			newState=KStateWaitingForTest4;
			break;
		case KStateTesting5:
			newState=KStateWaitingForTest5;
			break;
		case KStateTesting6:
			newState=KStateWaitingForTest6;
			break;
		case KStateTesting8:
			newState=KStateWaitingForTest8;
			break;
		case KStateTesting7:
			newState=KStateWaitingForTest7;
			break;
		}
	TheClient->iWs.PurgePointerEvents();
	SetState(newState);
	}

void CPointerWindow::SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iTl=pos;
	iSize=size;
	Activate();
	AssignGC(aGc);
	SetState(KStateWaitingForTest1);
	BaseWin()->EnableOnEvents();
	}

void CPointerWindow::Draw()
	{
	iGc->Clear();
	iGc->DrawRect(Size());
	iGc->DrawRect(iRepeatRect);
	TBuf<0x80> buf;
	TBuf<0x80> buf2;
	switch(iState)
		{
		case KStateWaitingForTest1:
			{
			_LIT(Draw1,"Click anywhere in the window and hold the pointer steady");
			buf.Format(Draw1);
			}
			break;
		case KStateTesting1:
			{
			_LIT(Draw2,"Hold pointer inside the box");
			buf.Format(Draw2);
			}
			break;
		case KStateWaitingForTest2:
			{
			_LIT(Draw3,"Release the pointer then click in the window and hold the pointer steady");
			buf.Format(Draw3);
			}
			break;
		case KStateTesting2:
			{
			_LIT(Draw4,"Drag the pointer outside the box");
			buf.Format(Draw4);
			}
			break;
		case KStateWaitingForTest3:
			{
			_LIT(Draw5,"Release the pointer then click in the window and hold the pointer steady");
			buf.Format(Draw5);
			}
			break;
		case KStateTesting3:
			{
			_LIT(Draw6,"Release the pointer");
			buf.Format(Draw6);
			}
			break;
		case KStateWaitingForTest4:
			{
			_LIT(Draw7,"Click anywhere in the window and move the pointer slightly");
			buf.Format(Draw7);
			}
			break;
		case KStateTesting4:
			{
			_LIT(Draw8,"Release the pointer");
			buf.Format(Draw8);
			}
			break;
		case KStateWaitingForTest5:
			{
			_LIT(Draw9,"Click anywhere in the window and move the pointer slightly");
			buf.Format(Draw9);
			}
			break;
		case KStateTesting5:
			{
			_LIT(Draw10,"Release the pointer");
			buf.Format(Draw10);
			break;
			}
		case KStateWaitingForTest6:
			{
			_LIT(Draw11,"Click anywhere in the window");
			buf.Format(Draw11);
			}
			break;
		case KStateTesting6:
			{
			_LIT(Draw12,"Move the pointer");
			buf.Format(Draw12);
			}
			break;
		case KStateWaitingForTest8:
			{
			_LIT(Draw13,"Click anywhere in the window, and keep the pointer pressed");
			buf.Format(Draw13);
			}
			break;
		case KStateTesting8:
			buf.Format(KNullDesC);
			break;
		case KStateWaitingForTest7:
			{
			_LIT(Draw14,"Switch off and touch the center of the screen");
			buf.Format(Draw14);
			}
			break;
		case KStateTesting7:
			buf.Format(KNullDesC);
			break;
		}
	switch(iState)
		{
		case KStateTesting1:
			{
			_LIT(Repeat2,"Repeat (%d/%d), interval=%d.%d");
			buf2.Format(Repeat2,iRepeatCount,KRepeatCount,iInterval.Int()/1000000,(iInterval.Int()%1000000)/100000);
			}
			break;
		case KStateTesting2:
		case KStateTesting3:
			{
			_LIT(Repeat1,"Repeat (%d), interval=%d.%d");
			buf2.Format(Repeat1,iRepeatCount,iInterval.Int()/1000000,(iInterval.Int()%1000000)/100000);
			}
			break;
		}
	iGc->DrawText(buf,TPoint(10,20));
	iGc->DrawText(buf2,TPoint(10,40));
	}

void CPointerWindow::FinishedTests()
	{
	iState=KStateFinished;
	}

void CPointerWindow::StartNextRepeatTest()
	{
	iInterval=0;
	iRepeatCount=0;
	if (iState==KStateWaitingForTest4)
		User::After(TTimeIntervalMicroSeconds32(1000000));
	else if (iState==KStateWaitingForTest5)
		User::After(TTimeIntervalMicroSeconds32(1000000));
	QueueNextRepeat();
	switch(iState)
		{
		case KStateWaitingForTest1:
			SetState(KStateTesting1);
			break;
		case KStateWaitingForTest2:
			SetState(KStateTesting2);
			break;
		case KStateWaitingForTest3:
			SetState(KStateTesting3);
			break;
		case KStateWaitingForTest4:
			SetState(KStateTesting4);
			break;
		case KStateWaitingForTest5:
			SetState(KStateTesting5);
			break;
		case KStateWaitingForTest6:
			SetState(KStateTesting6);
			break;
		case KStateWaitingForTest8:
			SetState(KStateTesting8);
			break;
		case KStateWaitingForTest7:
			SetState(KStateTesting7);
			break;
		}
	}

void CPointerWindow::QueueNextRepeat()
	{
	iInterval=iInterval.Int()+KRepeatIntervalIncrements;
#if defined(LOGGING)
	/*_LIT(KRequestRepeat,"Request Repeat, State=%d, Interval=%d");
	TLogMessageText buf;
	buf.Format(KRequestRepeat,iState,iInterval.Int());
	TheClient->LogMessage(buf);*/
#endif
	iWin.RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(iInterval),TRect(iRepeatRect));
	iPrevTime.HomeTime();
	iWin.Invalidate();
	}

void CPointerWindow::PointerL(const TPointerEvent &aPointer,const TTime &aTime)
	{
#if defined(LOGGING)
	_LIT(KPointer,"CPointerWindow::PointerL(Type=%d, Pos=(%d,%d), ParPos=(%d,%d), Mod=0x%x) State=%d");
	LogMessageText.Format(KPointer,aPointer.iType,aPointer.iPosition.iX,aPointer.iPosition.iY,aPointer.iParentPosition.iX,aPointer.iParentPosition.iY,aPointer.iModifiers,iState);
	TheClient->LogMessage(LogMessageText);
#endif
	if (aPointer.iType==TPointerEvent::EButtonRepeat)
		{
		if (iState!=KStateTesting1 && iState!=KStateTesting2 && iState!=KStateTesting3 && iState!=KStateTesting4)
			TestFailed();
		else
			{
			TTimeIntervalMicroSeconds32 interval(I64LOW(aTime.MicroSecondsFrom(iPrevTime).Int64()));
			TBool fail1=(interval.Int()<(iInterval.Int()-KRepeatMargin) || interval.Int()>(iInterval.Int()+KRepeatMargin));
			if (fail1)
				{
				_LIT(KPointerRepeat1,"Pointer Repeat Interval, Exp=%d, Act=%d, F1=%d, F2=%d");
				TLogMessageText buf;
				TBool fail2=(interval.Int()<(iInterval.Int()-2*KRepeatMargin) || interval.Int()>(iInterval.Int()+3*KRepeatMargin));
				buf.Format(KPointerRepeat1,iInterval.Int(),interval.Int(),fail1,fail2);
				TheClient->LogMessage(buf);
				}
			if (interval.Int()<(iInterval.Int()-
		#if defined(__MARM_ARM4__)
												2*
		#endif
												  KRepeatMargin) || interval.Int()>(iInterval.Int()+
		#if defined(__MARM_ARM4__)
																								    3*
		#endif
																									  KRepeatMargin))
				TestFailed();
			else
				{
				if (!iRepeatRect.Contains(aPointer.iPosition) ||
					aPointer.iParentPosition!=(aPointer.iPosition+iWin.InquireOffset(*Parent()->WinTreeNode())))
					TestFailed();
				else
					{
					iRepeatCount++;
					if (iState==KStateTesting1 && iRepeatCount==KRepeatCount)
						{
						QueueNextRepeat();
						Client()->iWs.Flush();
						User::After(TTimeIntervalMicroSeconds32(iRepeatCount*KRepeatIntervalIncrements));
						iWin.CancelPointerRepeatEventRequest();
						SetState(KStateWaitingForTest2);
						}
					else
						QueueNextRepeat();
					}
				}
			}
		}
	else switch(iState)
		{
		case KStateWaitingForTest1:
		case KStateWaitingForTest2:
		case KStateWaitingForTest3:
		case KStateWaitingForTest4:
		case KStateWaitingForTest8:
			if (aPointer.iType==TPointerEvent::EButton1Down)
				{
				iRepeatRect.iTl=aPointer.iPosition-TPoint(KRepeatRectXExtra,KRepeatRectYExtra);
				iRepeatRect.iBr=aPointer.iPosition+TPoint(KRepeatRectXExtra,KRepeatRectYExtra);
				StartNextRepeatTest();
				}
			break;
		case KStateWaitingForTest5:
			if (aPointer.iType==TPointerEvent::EButton1Down)
				{
				iRepeatRect.iTl=aPointer.iPosition;
				iRepeatRect.iBr=aPointer.iPosition+TPoint(1,1);
				StartNextRepeatTest();
				}
			break;
		case KStateWaitingForTest6:
			if (aPointer.iType==TPointerEvent::EButton1Down)
				{
				iRepeatRect.iTl=aPointer.iPosition+TPoint(KRepeatRectXExtra,KRepeatRectYExtra);
				iRepeatRect.iBr=aPointer.iPosition+TPoint(2*KRepeatRectXExtra,2*KRepeatRectYExtra);
				StartNextRepeatTest();
				}
			break;
		case KStateTesting1:
			{
			TBool isDrag=(aPointer.iType==TPointerEvent::EDrag);
			TestFailed(isDrag);
			if (isDrag)
				{
				iTest->SimulatePointer(TRawEvent::EButton1Down,aPointer.iPosition.iX,aPointer.iPosition.iY);
				iTest->SimulatePointer(TRawEvent::EButton1Up,aPointer.iPosition.iX,aPointer.iPosition.iY);
				}
			}
			break;
		case KStateTesting2:
			if (aPointer.iType==TPointerEvent::EDrag)
				SetState(KStateWaitingForTest3);
			else
				TestFailed();
			break;
		case KStateTesting3:
			if (aPointer.iType==TPointerEvent::EButton1Up)
				SetState(KStateWaitingForTest4);
			else
				TestFailed();
			break;
		case KStateTesting4:
			if (aPointer.iType==TPointerEvent::EButton1Up)
				SetState(KStateWaitingForTest5);
			else
				TestFailed();
			break;
		case KStateTesting5:
			if (aPointer.iType==TPointerEvent::EButton1Up)
				SetState(KStateWaitingForTest6);
			else if (aPointer.iType!=TPointerEvent::EDrag)
				TestFailed();
			break;
		case KStateTesting6:
			if (aPointer.iType==TPointerEvent::EDrag)
				{
		//#if !defined(__WINS__)	// Can't emulate touching dig when switched off under WINS
		//		if (iTest->Digitiser())
					SetState(KStateWaitingForTest7);
		/*		else
		#endif
					SetState(KStateWaitingForTest8);*/
				}
			else
				TestFailed();
			break;
		case KStateWaitingForTest7:
			if (aPointer.iType==TPointerEvent::EButton1Down || aPointer.iType==TPointerEvent::ESwitchOn)
				TestFailed();
			break;
		case KStateTesting7:
			if (aPointer.iType!=TPointerEvent::ESwitchOn)
				TestFailed();
			else
				SetState(KStateWaitingForTest8);
			break;
		case KStateTesting8:
			break;
		}
	}

void CPointerWindow::TestFailed(TBool aRetest/*=EFalse*/)
	{
	if (iState!=KStateFailed)
		{
		TInt oldState=iState;
		iState=KStateFailed;
		if (iTest->TestFailed(oldState,aRetest) || aRetest)
			ResetTest(oldState);
		}
	}

void CPointerWindow::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KKey,"CPointerWindow::WinKeyL(Code=%d, ScanCode=%d) State=%d");
	LogMessageText.Format(KKey,aKey.iCode,aKey.iScanCode,iState);
	TheClient->LogMessage(LogMessageText);
#endif
	if (aKey.iCode==EKeyEscape)
		FinishedTests();	// Simply skip this test if the Escape key is pressed
	}

void CPointerWindow::SwitchOn(const TTime &)
	{
#if defined(LOGGING)
	_LIT(KOn,"CPointerWindow::SwitchOn() State=%d");
	LogMessageText.Format(KOn,iState);
	TheClient->LogMessage(LogMessageText);
#endif
	if (iState==KStateWaitingForTest7)
		SetState(KStateTesting7);
	else
		TestFailed();
	}

void CPointerWindow::SendEvent()
	{	
#if defined(LOGGING)
	_LIT(KSend,"CPointerWindow::SendEvent() State=%d");
	LogMessageText.Format(KSend,iState);
	TheClient->LogMessage(LogMessageText);
#endif
	switch (iState)
		{
	case KStateWaitingForTest1:
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+iSize.iWidth/4,iTl.iY+iSize.iHeight/4);
		break;
	//case KStateTesting1:			//Do Nothing
	//	break;
	case KStateWaitingForTest2:
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX+iSize.iWidth/4,iTl.iY+iSize.iHeight/4);
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+iSize.iWidth/2,iTl.iY+iSize.iHeight/4);
		break;
	case KStateTesting2:
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+5*iSize.iWidth/8,iTl.iY+iSize.iHeight/4+2);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+3*iSize.iWidth/4,iTl.iY+iSize.iHeight/4+4);
		break;
	case KStateWaitingForTest3:
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX+3*iSize.iWidth/4,iTl.iY+iSize.iHeight/4+4);
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+iSize.iWidth/4,iTl.iY+iSize.iHeight/2);
		break;
	case KStateTesting3:
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX+iSize.iWidth/4,iTl.iY+iSize.iHeight/2);
		break;
	case KStateWaitingForTest4:
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+iSize.iWidth/2,iTl.iY+iSize.iHeight/2);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+iSize.iWidth/2+3,iTl.iY+iSize.iHeight/2+1);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+iSize.iWidth/2+6,iTl.iY+iSize.iHeight/2+2);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+iSize.iWidth/2+9,iTl.iY+iSize.iHeight/2+3);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+iSize.iWidth/2+12,iTl.iY+iSize.iHeight/2+4);
		break;
	case KStateTesting4:
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX+iSize.iWidth/2+12,iTl.iY+iSize.iHeight/2+4);
		break;
	case KStateWaitingForTest5:
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+iSize.iWidth/2,iTl.iY+3*iSize.iHeight/4);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+iSize.iWidth/2+3,iTl.iY+3*iSize.iHeight/4+1);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+iSize.iWidth/2+6,iTl.iY+3*iSize.iHeight/4+2);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+iSize.iWidth/2+9,iTl.iY+3*iSize.iHeight/4+3);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+iSize.iWidth/2+12,iTl.iY+3*iSize.iHeight/4+4);
		break;
	case KStateTesting5:
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX+iSize.iWidth/2+12,iTl.iY+3*iSize.iHeight/4+4);
		break;
	case KStateWaitingForTest6:
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+3*iSize.iWidth/4,iTl.iY+3*iSize.iHeight/4);
		break;
	case KStateTesting6:
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+3*iSize.iWidth/4+4,iTl.iY+3*iSize.iHeight/4+2);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+3*iSize.iWidth/4+8,iTl.iY+3*iSize.iHeight/4+4);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+3*iSize.iWidth/4+12,iTl.iY+3*iSize.iHeight/4+6);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+3*iSize.iWidth/4+16,iTl.iY+3*iSize.iHeight/4+8);
		break;
	case KStateWaitingForTest8:
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+iSize.iWidth/2,iTl.iY+3*iSize.iHeight/4);
		break;
	case KStateTesting8:
		break;
	case KStateWaitingForTest7:
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX+3*iSize.iWidth/4+16,iTl.iY+3*iSize.iHeight/4+8);
		iTest->SimulateEvent(TRawEvent::ESwitchOn);
		iTest->SimulatePointer(TRawEvent::EPointerSwitchOn,iTl.iX+iSize.iWidth/2-1,iTl.iY+iSize.iHeight/2-1);
		break;
	//case KStateTesting7:		//Do Nothing
	//	break;
	/*case KStateFailed:
		break;
	case KStateFinished:
		break;*/
		}
	}

//
// CGrabWindow //
//

CGrabWindow::CGrabWindow(CTPointer *aTest) : iTest(aTest)
	{
	}

void CGrabWindow::SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iTl=pos;
	iSize=size;
	Activate();
	AssignGC(aGc);
	SetState(KGrabStateWaitingForDown);
	}

void CGrabWindow::Draw()
	{
	iGc->Clear();
	iGc->DrawRect(Size());
	TBuf<0x80> buf;
	switch(iState)
		{
		case KGrabStateWaitingForDown:
		case KGrabStateWaitingForDown2:
		case KGrabStateWaitingForDown5:
			{
			_LIT(Draw1,"Press the pointer inside the window");
			buf.Copy(Draw1);
			}
			break;
		case KGrabStateWaitingForDown3:
		case KGrabStateWaitingForUp3a:
			{
			_LIT(Draw2,"Press and release the pointer inside the window");
			buf.Copy(Draw2);
			}
			break;
		case KGrabStateWaitingForDragOut:
			{
			_LIT(Draw3,"Drag the pointer outside into the outside window");
			buf.Copy(Draw3);
			}
			break;
		case KGrabStateWaitingForUp:
		case KGrabStateWaitingForUp2b:
		case KGrabStateWaitingForUp5:
			{
			_LIT(Draw4,"Release the pointer");
			buf.Copy(Draw4);
			}
			break;
		case KGrabStateWaitingForDown4:
			{
			_LIT(Draw5,"Press then release the pointer");
			buf.Copy(Draw5);
			}
			break;
		default:;
		}
	iGc->DrawText(buf,TPoint(10,20));
	}

void CGrabWindow::PointerL(const TPointerEvent &aPointer,const TTime&)
	{
#if defined(LOGGING)
	_LIT(KPointer,"CGrabWindow::PointerL(Type=%d, Pos=(%d,%d), ParPos=(%d,%d), Mod=0x%x) State=%d");
	LogMessageText.Format(KPointer,aPointer.iType,aPointer.iPosition.iX,aPointer.iPosition.iY,aPointer.iParentPosition.iX,aPointer.iParentPosition.iY,aPointer.iModifiers,iState);
	TheClient->LogMessage(LogMessageText);
#endif
	switch(iState)
		{
		case KGrabStateWaitingForDown:
			if (aPointer.iType==TPointerEvent::EButton1Down)
				SetState(KGrabStateWaitingForDragOut);
			break;
		case KGrabStateWaitingForDragOut:
			if (aPointer.iType!=TPointerEvent::EDrag)
				TestFailed();
			else
				{
				if (!TRect(Size()).Contains(aPointer.iPosition))
					SetState(KGrabStateWaitingForUp);
				}
			break;
		case KGrabStateWaitingForUp:
			if (aPointer.iType==TPointerEvent::EDrag)
				break;
			if (aPointer.iType==TPointerEvent::EButton1Up)
				SetState(KGrabStateWaitingForDown2);
			else
				TestFailed();
			break;
		case KGrabStateWaitingForDown2:
			if (aPointer.iType!=TPointerEvent::EButton1Down)
				TestFailed();
			else
				{
				iTest->GrabWin2()->BaseWin()->ClaimPointerGrab();
				iTest->GrabWin2()->BaseWin()->ClaimPointerGrab();	// Call twice to check it's harmless
				SetState(KGrabStateWaitingForUp2a);
				}
			break;
		case KGrabStateWaitingForUp2a:
			SetState(KGrabStateWaitingForDrag2);
			break;
		case KGrabStateWaitingForDrag2:
			break;
		case KGrabStateWaitingForUp2b:
			TestFailed();
			break;
		case KGrabStateWaitingForDown3:
			if (aPointer.iType!=TPointerEvent::EButton1Down)
				TestFailed();
			else
				SetState(KGrabStateWaitingForUp3a);
			break;
		case KGrabStateWaitingForUp3a:
			if (aPointer.iType==TPointerEvent::EButton1Up)
				{
				iTest->GrabWin2()->BaseWin()->ClaimPointerGrab();
				SetState(KGrabStateWaitingForUp3b);
				}
			break;
		case KGrabStateWaitingForUp3b:
			TestFailed();
			break;
		case KGrabStateWaitingForDown5:
			if (aPointer.iType!=TPointerEvent::EButton1Down)
				TestFailed();
			else
				{
				iTest->GrabWin2()->BaseWin()->ClaimPointerGrab(EFalse);
				SetState(KGrabStateWaitingForDrag5);
				}
			break;
		case KGrabStateWaitingForDrag5:
			if (aPointer.iType==TPointerEvent::EButton1Up)
				TestFailed();
			break;
		case KGrabStateWaitingForUp5:
			TestFailed();
			break;
		case KGrabStateWaitingForDown4:
			iWin.Close();	// Close the window with the grab captured in it
			SetState(KGrabStateWaitingForUp4);
			break;
		case KGrabStateFinished:
			break;
		}
	}

void CGrabWindow::ResetTest()
	{
	TheClient->iWs.PurgePointerEvents();
	SetState(KGrabStateWaitingForDown);
	}

void CGrabWindow::Pointer2(const TPointerEvent &aPointer)
	{
	switch(iState)
		{
		case KGrabStateWaitingForDrag2:
			{
			if (aPointer.iType==TPointerEvent::EDrag)
				{
				SetState(KGrabStateWaitingForUp2b);
				}
			else
				{
				if (iTest->TestFailed(iState))
					ResetTest();
				}
			break;
			}
		case KGrabStateWaitingForUp2b:
			if (aPointer.iType==TPointerEvent::EDrag)	// Harmless
				break;
			if (aPointer.iType==TPointerEvent::EButton1Up)
				SetState(KGrabStateWaitingForDown3);
			else
				TestFailed();
			break;
		case KGrabStateWaitingForUp3b:
			if (aPointer.iType==TPointerEvent::EButton1Up)
				SetState(KGrabStateWaitingForDown5);
			else
				TestFailed();
			break;
		case KGrabStateWaitingForUp4:
			FinishedTests();
			break;
		case KGrabStateWaitingForDrag5:
			{
			if (aPointer.iType==TPointerEvent::EDrag)
				SetState(KGrabStateWaitingForUp5);
			else if (iTest->TestFailed(iState))
				ResetTest();
			break;
			}
		case KGrabStateWaitingForUp5:
			if (aPointer.iType==TPointerEvent::EDrag)	// Harmless
				break;
			if (aPointer.iType==TPointerEvent::EButton1Up)
				SetState(KGrabStateWaitingForDown4);
			else
				TestFailed();
			break;
		default:;
		}
	}

void CGrabWindow::SetState(TInt aState)
	{
	iState=aState;
	if (aState!=KGrabStateWaitingForUp4)
		iWin.Invalidate();
	TheClient->WaitForRedrawsToFinish();	
	SendEvent();
	}

void CGrabWindow::TestFailed()
	{
#if defined(LOGGING)
	_LIT(KFail,"CGrabWindow::TestFailed() State=%d");
	LogMessageText.Format(KFail,iState);
	TheClient->LogMessage(LogMessageText);
#endif
	if (iState!=KStateFailed)
		{
		TInt oldState=iState;
		iState=KStateFailed;
		if (iTest->TestFailed(oldState))
			ResetTest();
		}
	}

void CGrabWindow::FinishedTests()
	{
	iState=KGrabStateFinished;
	}

void CGrabWindow::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KKey,"CGrabWindow::WinKeyL(Code=%d, ScanCode=%d) State=%d");
	LogMessageText.Format(KKey,aKey.iCode,aKey.iScanCode,iState);
	TheClient->LogMessage(LogMessageText);
#endif
	if (aKey.iCode==EKeyEscape)
		FinishedTests();	// Simply skip this test if the Escape key is pressed
	}

void CGrabWindow::SendEvent()
	{
	switch (iState)
		{
	case KGrabStateWaitingForDown:
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+iSize.iWidth/3,iTl.iY+iSize.iHeight/3);
		break;
	case KGrabStateWaitingForDragOut:
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+iSize.iWidth/6,iTl.iY+iSize.iHeight/6);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+1,iTl.iY+1);
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX-10,iTl.iY-10);
		break;
	case KGrabStateWaitingForUp:
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX-8,iTl.iY-8);
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX-5,iTl.iY-5);
		break;
	case KGrabStateWaitingForDown2:
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+2*iSize.iWidth/3,iTl.iY+iSize.iHeight/3);
		break;
	/*case KGrabStateWaitingForUp2a:		//Don't need to do anything for these cases
		break;
	case KGrabStateWaitingForDrag2:
		break;*/
	case KGrabStateWaitingForUp2b:
		iTest->SimulatePointer(TRawEvent::EPointerMove,iTl.iX+2*iSize.iWidth/3-3,iTl.iY+iSize.iHeight/3+3);
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX+2*iSize.iWidth/3,iTl.iY+iSize.iHeight/3);
		break;
	case KGrabStateWaitingForDown3:
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+2*iSize.iWidth/3,2*iTl.iY+iSize.iHeight/3);
		break;
	case KGrabStateWaitingForUp3a:
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX+2*iSize.iWidth/3,2*iTl.iY+iSize.iHeight/3);
		break;
	case KGrabStateWaitingForUp3b:
		break;
	case KGrabStateWaitingForDown4:
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+2*iSize.iWidth/5,iTl.iY+iSize.iHeight/3);
		break;
	case KGrabStateWaitingForUp4:
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX+2*iSize.iWidth/5,iTl.iY+iSize.iHeight/3);
		break;
	case KGrabStateWaitingForDown5:
		iTest->SimulatePointer(TRawEvent::EButton1Down,iTl.iX+iSize.iWidth/3,2*iTl.iY+iSize.iHeight/3);
		break;
	case KGrabStateWaitingForUp5:
		iTest->SimulatePointer(TRawEvent::EButton1Up,iTl.iX+iSize.iWidth/3,2*iTl.iY+iSize.iHeight/3);
		break;
	//case KGrabStateWaitingForDrag5:		//Don't need to do anything for these cases
	//	break;
	/*case KStateFailed:
		break;
	case KGrabStateFinished:
		break;*/
	default:;
		}
	TheClient->iWs.Flush();
	}


//
// CGrabWindow2, used as part of grab tests //
//

CGrabWindow2::CGrabWindow2(CGrabWindow *aWindow) : iGrabWindow(aWindow)
	{
	}

void CGrabWindow2::Draw()
	{
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(TRgb::Gray4(2));
	iGc->DrawRect(Size());
	}

void CGrabWindow2::PointerL(const TPointerEvent &aPointer,const TTime&)
	{
#if defined(LOGGING)
	_LIT(KPointer,"CGrabWindow2::PointerL(Type=%d, Pos=(%d,%d), ParPos=(%d,%d), Mod=0x%x)");
	LogMessageText.Format(KPointer,aPointer.iType,aPointer.iPosition.iX,aPointer.iPosition.iY,aPointer.iParentPosition.iX,aPointer.iParentPosition.iY,aPointer.iModifiers);
	TheClient->LogMessage(LogMessageText);
#endif
	iGrabWindow->Pointer2(aPointer);
	}

//
// CTPointTest //
//

CTPointer::CTPointer(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	iState = 0;
	}


CTPointer::~CTPointer()
	{
	HAL::Set(HALData::EPenDisplayOn,iOldPointerState);
	delete iRepeatWin;
	delete iGrabWin;
	delete iGrabWin2;
	Client()->ResetFocus();
	delete iTimeOut;
	}

void CTPointer::ConstructL()
	{
	iTimeOut=new(ELeave) CTimeOut();
	iTimeOut->ConstructL();
	iTimeOut->Start(KTimeOutAfter,TCallBack(CTPointer::TimeOut,this));
	
	TheClient->iWs.SetPointerCursorArea(iTest->iNormalPointerCursorArea);
	iRepeatWin=new(ELeave) CPointerWindow(this);
	TSize screenSize(Client()->iScreen->SizeInPixels());
	if (TestBase()->ConfigurationSupportsPointerEventTesting())
	    {
	    iRepeatWin->SetUpLD(TPoint(screenSize.iWidth/8,screenSize.iHeight/8),TSize(screenSize.iWidth*3/4,screenSize.iHeight*3/4),Client()->iGroup,*Client()->iGc);
	    }
	Client()->iGroup->SetCurrentWindow(iRepeatWin);
	iNoDigitiser=EFalse;
	TInt err=HAL::Get(HALData::EPenDisplayOn,iOldPointerState);
	if (err==KErrNotSupported)
		iNoDigitiser=ETrue;
	else if (err==KErrNone)
		err=HAL::Set(HALData::EPenDisplayOn,ETrue);
	if (err==KErrNotSupported)
		iNoDigitiser=(!iOldPointerState);
	else
		{
		TEST(err==KErrNone);
		if (err!=KErrNone)
		INFO_PRINTF3(_L("HAL::Set(HALData::EPenDisplayOn,ETrue) return value - Expected: %d, Actual: %d"), KErrNone, err);			
		}
	
	}

TInt CTPointer::TimeOut(TAny* aTest)		//static
	{
	static_cast<CTPointer*>(aTest)->TimeOut();
	return(KErrNone);
	}

void CTPointer::TimeOut()
	{
	TLogMessageText buf;
	_LIT(KPointerTimeOut,"TIMEOUT: Pointer Test, %d, %S");
	buf.AppendFormat(KPointerTimeOut,iState,&(iTest->iSubTitle));
	TheClient->LogMessage(buf);
	++iTimeOutCount;
	if (!TestFailed(-1) && iState<2)
		{
		iTimeOut->Start(KTimeOutAfter,TCallBack(CTPointer::TimeOut,this));
		}
	}

TBool CTPointer::TestFailed(TInt aCase,TBool aRetry/*=EFalse*/)
	{
	_LIT(KPointerTest,": Pointer Test, %d, %S, Case %d");
	_LIT(KRetry,"Retry");
	_LIT(KFail,"FAIL");
	TLogMessageText buf;
	TInt ret=0;
	if (aRetry)
		buf.Append(KRetry);
	else
		{
		ret=1;
		buf.Append(KFail);
		}
	buf.AppendFormat(KPointerTest,iState,&(iTest->iSubTitle),aCase);
	TheClient->LogMessage(buf);
#if !defined(DISABLE_FAIL_DIALOG)
	if (!aRetry)
		{
		__DEBUGGER();
		TRAPD(err,ret=doTestFailedL());
		}
#endif
	switch(ret)
		{
		case 0:
			return(ETrue);	// Re-try test
		case 1:
			break;
		}
	return(EFalse);
	}

TInt CTPointer::doTestFailedL()
	{
	//_LIT(Failed,"Pointer repeat test failed");
	_LIT(Retest,"Retest");
	_LIT(Fail,"Fail");
	CTDialog *dialog=new(ELeave) CTDialog();
	dialog->SetNumButtons(2);
	dialog->SetButtonText(0,Retest);
	dialog->SetButtonText(1,Fail);
	dialog->ConstructLD(*Client()->iGroup,*Client()->iGc);
	dialog->SetTitle(_L("Pointer repeat test failed"));
	dialog->SetFlags(CTDialog::EDialogWaitForButtonUp);
	return dialog->Display();
	}

CGrabWindow2 *CTPointer::GrabWin2() const
	{
	return(iGrabWin2);
	}

void CTPointer::StartGrabTestL()
	{
	iGrabWin=new(ELeave) CGrabWindow(this);
	iGrabWin2=new(ELeave) CGrabWindow2(iGrabWin);
//
	TSize screenSize(Client()->iScreen->SizeInPixels());
	iGrabWin2->SetUpL(TPoint(screenSize.iWidth/8,screenSize.iHeight/8),TSize(screenSize.iWidth*3/4,screenSize.iHeight*3/4),Client()->iGroup,*Client()->iGc);
	Client()->iGroup->SetCurrentWindow(iGrabWin2);
//
	iGrabWin->SetUpLD(TPoint(screenSize.iWidth/4,screenSize.iHeight/4),TSize(screenSize.iWidth/2,screenSize.iHeight/2),Client()->iGroup,*Client()->iGc);
	Client()->iGroup->SetCurrentWindow(iGrabWin);
//
	}


void CTPointer::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(Repeat,"Repeat tests");
	_LIT(Grab,"Grab tests");

	iTest->iState=iState;
	
	if (!TestBase()->ConfigurationSupportsPointerEventTesting())
	    {
	    INFO_PRINTF1(_L("Test skipped because config does not support pointer event testing"));
	    TestComplete();
	    return;
	    }
	
	((CTPointerStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(iState)
		{
		case 0:
			((CTPointerStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			iTest->LogSubTest(Repeat);
			iState++;
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0310
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test pointer event handling
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Simulate pointer events and check the events are then handled correctly
  
  @SYMTestExpectedResults Pointer events are handled correctly
 
*/
		case 1:
			((CTPointerStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0310"));
			iTest->LogSubTest(Grab);
			
			StartGrabTestL();
			iState++;
			break;
		default:
			((CTPointerStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTPointerStep*)iStep)->CloseTMSGraphicsStep();
			iTimeOut->Cancel();
			if (iTimeOutCount>0) TEST(EFalse);
			if (iTest->IsFullRomL()) User::After(5000000);
			TestComplete();
		}
	((CTPointerStep*)iStep)->RecordTestResultL();
	}


__WS_CONSTRUCT_STEP__(Pointer)
