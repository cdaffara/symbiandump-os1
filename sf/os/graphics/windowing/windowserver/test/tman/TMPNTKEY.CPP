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
// Test Pointer move/drag buffer
// 
//

#include <e32std.h>
#include <hal.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

const TInt ENumPntKeyTests=6;
const TUint EModifierMask=EModifierCtrl|EModifierShift|EModifierFunc;

class CTPntKeyTest;

class CTPntKeyWindow : public CTWin
	{
private:
	enum {KPointerMoveBufferSize=32};
public:
	CTPntKeyWindow(CTPntKeyTest *aTest);
	~CTPntKeyWindow();
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent);
	void PointerL(const TPointerEvent &pointer,const TTime &aTime);
	void KeyUpL(const TKeyEvent &aKey,const TTime &);
	void KeyDownL(const TKeyEvent &aKey,const TTime &);
	void WinKeyL(const TKeyEvent &aKey,const TTime &);
	void SwitchOn(const TTime &aTime);
	void SetState(TInt iState);
	void NextKey();
	void Test(TInt aCheck);
	void Error();
	void DrawButton(const TRect &aRect, const TDesC &aText);
	void Draw();
private:
	CTPntKeyTest *iTest;
	TInt iKeyCount;
	TRect iKey1;
	TRect iKey2;
	TRect iKey3;
	static TInt iTestScanCodes[ENumPntKeyTests];
	static TUint iTestCodes[ENumPntKeyTests];
	static TUint iTestModifiers[ENumPntKeyTests];
	};

class CTPntKeyTest : public CTestBase
	{
public:
	CTPntKeyTest();
	~CTPntKeyTest();
	TestState DoTestL();
	void FinishedTests();
	void ConstructL();
	void Failed();
	inline TBool NoDigitiser() const {return iNoDigitiser;}
private:
	CTPntKeyWindow *iWin;
	TSize iWinSize;
	TInt iState;
	TBool iFailed;
	TBool iOldPointerState;
	TBool iNoDigitiser;
	};

TInt CTPntKeyWindow::iTestScanCodes[ENumPntKeyTests]={'A','B',0,'C',EStdKeyEnter,'Y'};
TUint CTPntKeyWindow::iTestCodes[ENumPntKeyTests]={'a','B',0,'c',EKeyEnter,'y'};
TUint CTPntKeyWindow::iTestModifiers[ENumPntKeyTests]={0,EModifierShift,0,0,0,0};

GLDEF_C CTestBase *CreatePointerKeyTest()
	{
	return(new(ELeave) CTPntKeyTest());
	}

CTPntKeyWindow::CTPntKeyWindow(CTPntKeyTest *aTest) : iTest(aTest)
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

void CTPntKeyWindow::Error()
	{
	iTest->Failed();
	}

void CTPntKeyWindow::SetState(TInt iState)
	{
	iKeyCount=iState-1;
	NextKey();
	iTest->Client()->Flush();
	}

void CTPntKeyWindow::NextKey()
	{
	if (++iKeyCount==ENumPntKeyTests || iKeyCount>4)
		iTest->FinishedTests();
	else
		{
		if (iKeyCount==2)
			iWin.RemoveAllKeyRects();
		else if (iKeyCount==3)
			iWin.AddKeyRect(iKey3,'C',EFalse);
		else if (iKeyCount==4)
			{
	#if !defined(__WINS__)
			if (iTest->NoDigitiser())
				iTest->FinishedTests();
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
	}

void CTPntKeyWindow::Test(TInt aCheck)
	{
	if (!aCheck)
		Error();
	}

void CTPntKeyWindow::KeyUpL(const TKeyEvent &aKey,const TTime&)
	{
	if (aKey.iScanCode==iTestScanCodes[iKeyCount])
		NextKey();
	}

void CTPntKeyWindow::KeyDownL(const TKeyEvent &aKey,const TTime &)
	{
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
	if (aKey.iCode==EKeyEscape)
		iTest->FinishedTests();
	else
		{
		Test(aKey.iScanCode==iTestScanCodes[iKeyCount]);
		Test(aKey.iCode==iTestCodes[iKeyCount]);
		Test((aKey.iModifiers&EModifierMask)==(iTestModifiers[iKeyCount]&EModifierMask));
		}
	}

void CTPntKeyWindow::SwitchOn(const TTime &)
	{
	if (iKeyCount==4)
		NextKey();
	else if (iKeyCount!=5)
		Error();
	}

void CTPntKeyWindow::PointerL(const TPointerEvent &aPointer,const TTime &)
	{
	if (aPointer.iType==TPointerEvent::EButton1Down)
		{
		if (iKeyCount!=2)
			Error();
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
			iGc->DrawText(_L("1st Switch off, then touch the screen to switch on"), TPoint(10,20));
#endif
			break;
		case 5:
#if defined(__WINS__)	// Can't emulate touching dig when switched off under WINS
			iGc->DrawText(_L("Touch anywhere in the window"), TPoint(10,20));
#else
			iGc->DrawText(_L("2nd Switch off and touch the screen to switch on"), TPoint(10,20));
#endif
			break;
		}
	}

CTPntKeyTest::CTPntKeyTest() : CTestBase(_L("Pointer Key Test"))
	{}

CTPntKeyTest::~CTPntKeyTest()
	{
	HAL::Set(HALData::EPenDisplayOn,iOldPointerState);
	CTWin::Delete(iWin);
	Client()->ResetFocus();
	}

void CTPntKeyTest::Failed()
	{
	if (!iFailed)
		{
		iFailed=ETrue;
		FinishedTests();
		}
	}

void CTPntKeyTest::FinishedTests()
	{
	Request();
	}

void CTPntKeyTest::ConstructL()
	{
	CTPntKeyWindow *win=new(ELeave) CTPntKeyWindow(this);
	win->SetUpLD(TPoint(20,20),Client()->iScreen->SizeInPixels()-TSize(40,40),Client()->iGroup);
	iWin=win;
	Client()->iGroup->SetCurrentWindow(iWin);
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

TestState CTPntKeyTest::DoTestL()
	{
	TestL(!iFailed);
	switch(iState)
		{
		case 0:
			LogSubTest(_L("Key set 1"),1);
			iWin->SetState(4);
			iState++;
			return(EContinue);
		default:
			return(EFinished);
		}
//	return(ENext);
	}
