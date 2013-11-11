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
// 
//

#include <e32std.h>
#include <hal.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

class CTPointerTest;
class CGrabWindow;

class CPointerWindow : public CTWin
	{
	enum {KRepeatMargin=200000};	// 0.2 Seconds
	enum {KRepeatCount=4};
	enum {KRepeatIntervalIncrements=100000};
	enum {KRepeatRectXExtra=50};
	enum {KRepeatRectYExtra=50};
	enum {KStateWaitingForTest1,KStateFailed,KStateTesting1,
			KStateWaitingForTest2,KStateTesting2,
			KStateWaitingForTest3,KStateTesting3,
			KStateWaitingForTest4,KStateTesting4,
			KStateWaitingForTest5,KStateTesting5,
			KStateWaitingForTest6,KStateTesting6,
			KStateWaitingForTest8,KStateTesting8,
			KStateWaitingForTest7,KStateTesting7,
			KStateFinished};
public:
	CPointerWindow(CTPointerTest *aTest);
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw();
	void StartNextRepeatTest();
	void QueueNextRepeat();
	void PointerL(const TPointerEvent &aPointer,const TTime&);
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
private:
	void SwitchOn(const TTime &);
	void ResetTest(TInt aState);
	void SetState(TInt aState);
	void FinishedTests();
	void TestFailed();
private:
	CTPointerTest *iTest;
	TInt iRepeatCount;
	TInt iState;
	TTimeIntervalMicroSeconds32 iInterval;
	TTime iPrevTime;
	TRect iRepeatRect;
	};

class CGrabWindow2 : public CTWin
	{
public:
	CGrabWindow2(CGrabWindow *aWindow);
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
private:
	void Draw();
	void PointerL(const TPointerEvent &aPointer,const TTime&);
private:
	CGrabWindow *iGrabWindow;
	};

class CGrabWindow : public CTWin
	{
	enum {KGrabStateWaitingForDown,KGrabStateWaitingForDragOut,KGrabStateWaitingForUp,KGrabStateFinished,
		KGrabStateWaitingForDown2,KGrabStateWaitingForUp2a,KGrabStateWaitingForDrag2,KGrabStateWaitingForUp2b,
		KGrabStateWaitingForDown3,KGrabStateWaitingForUp3a,KGrabStateWaitingForUp3b,
		KGrabStateWaitingForDown4,KGrabStateWaitingForUp4,
		KGrabStateWaitingForDown5,KGrabStateWaitingForUp5,KGrabStateWaitingForDrag5,
		KStateFailed};
public:
	CGrabWindow(CTPointerTest *aTest);
	void Pointer2(const TPointerEvent &aPointer);
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
private:
	void Draw();
	void SetState(TInt aState);
	void ResetTest();
	void PointerL(const TPointerEvent &aPointer,const TTime&);
	void TestFailed();
	void FinishedTests();
private:
	TInt iState;
	CTPointerTest *iTest;
	};

class CTPointerTest : public CTestBase
	{
public:
	CTPointerTest();
	~CTPointerTest();
	TBool TestFailed();
	TestState DoTestL();
	void StartGrabTestL();
	void ConstructL();
	CGrabWindow2 *GrabWin2() const;
	inline TBool Digitiser() const {return !iNoDigitiser;}
private:
	TInt doTestFailedL();
private:
	CPointerWindow *iRepeatWin;
	CGrabWindow *iGrabWin;
	CGrabWindow2 *iGrabWin2;
	TSize iWinSize;
	TInt iState;
	TBool iOldPointerState;
	TBool iNoDigitiser;
	};

GLDEF_C CTestBase *CreatePointerTest()
	{
	return(new(ELeave) CTPointerTest());
	}

CPointerWindow::CPointerWindow(CTPointerTest *aTest) : CTWin(), iTest(aTest)
	{}

void CPointerWindow::SetState(TInt aState)
	{
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
	SetState(newState);
	}

void CPointerWindow::SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	Activate();
	AssignGC(aGc);
	//SetState(KStateWaitingForTest1);
	SetState(KStateWaitingForTest7);		//Only unconverted test
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
			buf.Format(_L("Click anywhere in the window and hold the pointer steady"));
			break;
		case KStateTesting1:
			buf.Format(_L("Hold pointer inside the box"));
			break;
		case KStateWaitingForTest2:
			buf.Format(_L("Release the pointer then click in the window and hold the pointer steady"));
			break;
		case KStateTesting2:
			buf.Format(_L("Drag the pointer outside the box"));
			break;
		case KStateWaitingForTest3:
			buf.Format(_L("Release the pointer then click in the window and hold the pointer steady"));
			break;
		case KStateTesting3:
			buf.Format(_L("Release the pointer"));
			break;
		case KStateWaitingForTest4:
			buf.Format(_L("Click anywhere in the window and move the pointer slightly"));
			break;
		case KStateTesting4:
			buf.Format(_L("Release the pointer"));
			break;
		case KStateWaitingForTest5:
			buf.Format(_L("Click anywhere in the window and move the pointer slightly"));
			break;
		case KStateTesting5:
			buf.Format(_L("Release the pointer"));
			break;
		case KStateWaitingForTest6:
			buf.Format(_L("Click anywhere in the window"));
			break;
		case KStateTesting6:
			buf.Format(_L("Move the pointer"));
			break;
		case KStateWaitingForTest8:
			buf.Format(_L("Click anywhere in the window, and keep the pointer pressed"));
			break;
		case KStateTesting8:
			buf.Format(_L(""));
			break;
		case KStateWaitingForTest7:
			buf.Format(_L("Switch off and touch the center of the screen"));
			break;
		case KStateTesting7:
			buf.Format(_L(""));
			break;
		}
	switch(iState)
		{
		case KStateTesting1:
			buf2.Format(TRefByValue<const TDesC>(_L("Repeat (%d/%d), interval=%d.%d")),iRepeatCount,KRepeatCount,iInterval.Int()/1000000,(iInterval.Int()%1000000)/100000);
			break;
		case KStateTesting2:
		case KStateTesting3:
			buf2.Format(TRefByValue<const TDesC>(_L("Repeat (%d), interval=%d.%d")),iRepeatCount,iInterval.Int()/1000000,(iInterval.Int()%1000000)/100000);
			break;
		}
	iGc->DrawText(buf,TPoint(10,20));
	iGc->DrawText(buf2,TPoint(10,40));
	}

void CPointerWindow::FinishedTests()
	{
	iState=KStateFinished;
	iTest->Request();
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
	iWin.RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(iInterval),TRect(iRepeatRect));
	iPrevTime.HomeTime();
	iWin.Invalidate();
	}

void CPointerWindow::PointerL(const TPointerEvent &aPointer,const TTime &aTime)
	{
	if (aPointer.iType==TPointerEvent::EButtonRepeat)
		{
		if (iState!=KStateTesting1 && iState!=KStateTesting2 && iState!=KStateTesting3 && iState!=KStateTesting4)
			TestFailed();
		else
			{
			TTimeIntervalMicroSeconds32 interval(I64LOW(aTime.MicroSecondsFrom(iPrevTime).Int64()));
			if (interval.Int()<(iInterval.Int()-KRepeatMargin) || interval.Int()>(iInterval.Int()+KRepeatMargin))
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
			TestFailed();
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
		#if !defined(__WINS__)	// Can't emulate touching dig when switched off under WINS
				if (iTest->Digitiser())
					SetState(KStateWaitingForTest7);
				else
		#endif
					SetState(KStateWaitingForTest8);
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
				SetState(KStateTesting8);		//Jump straight to end as this is only unconverted test.
				//SetState(KStateWaitingForTest8);
			break;
		case KStateTesting8:
			break;
		}
	}

void CPointerWindow::TestFailed()
	{
	if (iState!=KStateFailed)
		{
		TInt oldState=iState;
		iState=KStateFailed;
		if (iTest->TestFailed())
			ResetTest(oldState);
		}
	}

void CPointerWindow::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode==EKeyEscape)
		FinishedTests();	// Simply skip this test if the Escape key is pressed
	}

void CPointerWindow::SwitchOn(const TTime &)
	{
	if (iState==KStateWaitingForTest7)
		SetState(KStateTesting7);
	else
		TestFailed();
	}

//
// CGrabWindow //
//

CGrabWindow::CGrabWindow(CTPointerTest *aTest) : iTest(aTest)
	{
	}

void CGrabWindow::SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
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
			buf.Copy(_L("Press the pointer inside the window"));
			break;
		case KGrabStateWaitingForDown3:
		case KGrabStateWaitingForUp3a:
			buf.Copy(_L("Press and release the pointer inside the window"));
			break;
		case KGrabStateWaitingForDragOut:
			buf.Copy(_L("Drag the pointer outside into the outside window"));
			break;
		case KGrabStateWaitingForDrag2:
			buf.Copy(_L("Drag the pointer outside outside both windows"));
			break;
		case KGrabStateWaitingForUp:
		case KGrabStateWaitingForUp2b:
		case KGrabStateWaitingForUp5:
			buf.Copy(_L("Release the pointer"));
			break;
		case KGrabStateWaitingForDown4:
			buf.Copy(_L("Press then release the pointer"));
			break;
		default:;
		}
	iGc->DrawText(buf,TPoint(10,20));
	}

void CGrabWindow::PointerL(const TPointerEvent &aPointer,const TTime&)
	{
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
			SetState(KGrabStateWaitingForUp4);
			iWin.Close();	// Close the window with the grab captured in it
			break;
		case KGrabStateFinished:
			break;
		}
	}

void CGrabWindow::ResetTest()
	{
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
				if (iTest->TestFailed())
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
			else if (iTest->TestFailed())
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
	iWin.Invalidate();
	}

void CGrabWindow::TestFailed()
	{
	if (iState!=KStateFailed)
		{
		iState=KStateFailed;
		if (iTest->TestFailed())
			ResetTest();
		}
	}

void CGrabWindow::FinishedTests()
	{
	iState=KGrabStateFinished;
	iTest->Request();
	}

void CGrabWindow::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode==EKeyEscape)
		FinishedTests();	// Simply skip this test if the Escape key is pressed
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
	iGrabWindow->Pointer2(aPointer);
	}

//
// CTPointTest //
//

CTPointerTest::CTPointerTest() : CTestBase(_L("CTPointerTest"))
	{}

CTPointerTest::~CTPointerTest()
	{
	HAL::Set(HALData::EPenDisplayOn,iOldPointerState);
	delete iRepeatWin;
	delete iGrabWin;
	delete iGrabWin2;
	Client()->ResetFocus();
	}

void CTPointerTest::ConstructL()
	{
	iRepeatWin=new(ELeave) CPointerWindow(this);
	TSize screenSize(Client()->iScreen->SizeInPixels());
	iRepeatWin->SetUpLD(TPoint(screenSize.iWidth/8,screenSize.iHeight/8),TSize(screenSize.iWidth*3/4,screenSize.iHeight*3/4),Client()->iGroup,*Client()->iGc);
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
		TestL(err==KErrNone);
	}

TBool CTPointerTest::TestFailed()
	{
	TInt ret=1;
	TRAP_IGNORE(ret=doTestFailedL());
	switch(ret)
		{
		case 0:
			return(ETrue);	// Re-try test
		case 1:
			Request();
			break;
		}
	return(EFalse);
	}

TInt CTPointerTest::doTestFailedL()
	{
	CTDialog *dialog=new(ELeave) CTDialog();
	dialog->SetTitle(_L("Pointer repeat test failed"));
	dialog->SetNumButtons(2);
	dialog->SetButtonText(0,_L("Retest"));
	dialog->SetButtonText(1,_L("Fail"));
	dialog->ConstructLD(*Client()->iGroup,*Client()->iGc);
	return dialog->Display();
	}

CGrabWindow2 *CTPointerTest::GrabWin2() const
	{
	return(iGrabWin2);
	}

void CTPointerTest::StartGrabTestL()
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

#pragma warning( disable : 4065 )	//switch statement contains 'default' but no 'case' labels
TestState CTPointerTest::DoTestL()
	{
	switch(iState)
		{
	#if !defined(__WINS__)		//Only unconverted test is MARM only
		case 0:
			LogSubTest(_L("Repeat tests"),1);
			iState++;
			return(EContinue);
	#endif
		
		default:
			return(EFinished);
		}
	}
#pragma warning( default : 4065 )

