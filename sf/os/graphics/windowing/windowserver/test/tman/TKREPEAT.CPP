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
// Keyboard repeat test
// 
//

#include <e32std.h>
#include <e32svr.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

class CRKWindow;

class TKRepeatTest : public CTestBase
	{
public:
	TKRepeatTest();
	~TKRepeatTest();
	TestState DoTestL();
	void ConstructL();
	void TestKeyboardRepeatRateL(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime);
	TBool CheckReportL();
public:
	TBool iAbort;
private:
	TTimeIntervalMicroSeconds32 iOldInitialTime;
	TTimeIntervalMicroSeconds32 iOldTime;
	CRKWindow *iWin;
	TSize iWinSize;
	TInt iState;
	};

class CRKWindow : public CTWin
	{
	enum TRKStates {
		EStateWaitingForKeyDown,
		EStateWaitingForKeyCode,
		EStateWaitingForFirstRepeat,
		EStateWaitingForNthRepeat,
		EStateWaitingForKeyUp,
		EStateInactive,
		EStateError,
		};
public:
	CRKWindow(TKRepeatTest *aTest);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void SetState(TRKStates aState);
	void SetKeyboardRepeatRate(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime);
	void WinKeyL(const TKeyEvent &,const TTime &);
	void KeyUpL(const TKeyEvent &aKey,const TTime &aTime);
	void KeyDownL(const TKeyEvent &aKey,const TTime &aTime);
	void Draw();
	TDesC& Report();
	TBool CheckResults();
protected:
	TInt iConnIndex;
	TKRepeatTest *iTest;
	TRgb iBack;
	TRKStates iState;
	TInt iDownCode;
	TInt iRepCount;
	TTimeIntervalMicroSeconds32 iInitialRepeatSet;
	TTimeIntervalMicroSeconds32 iRepeatSet;
	TTime iPrevTime;
	TTimeIntervalMicroSeconds32 iInitialGap;
	TTimeIntervalMicroSeconds32 iTotalGap;
	TTimeIntervalMicroSeconds32 iMinGap;
	TTimeIntervalMicroSeconds32 iMaxGap;
	TBuf<0x40> iReport;
	};

GLDEF_C CTestBase *CreateKRepeatTest()
	{
	return(new(ELeave) TKRepeatTest());
	}

//
// CRKWindow, class //
//

CRKWindow::CRKWindow(TKRepeatTest *aTest) : CTWin(), iTest(aTest)
	{
	}

void CRKWindow::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iWin.SetBackgroundColor(TRgb::Gray256(230));
	Activate();
	AssignGC(aGc);
	}

void CRKWindow::Draw()
	{
	iGc->Clear();
	switch(iState)
		{
		case EStateWaitingForKeyDown:
			iGc->DrawText(_L("Press and hold the space bar"), TPoint(10,20));
			break;
		case EStateWaitingForFirstRepeat:
		case EStateWaitingForNthRepeat:
			{
			TBuf<0x40> buf;
			buf.Format(TRefByValue<const TDesC>(_L("Keep space bar down (%d repeats so far)")),iRepCount);
			iGc->DrawText(buf, TPoint(10,20));
			}
			break;
		case EStateWaitingForKeyUp:
			iGc->DrawText(_L("Release space bar"), TPoint(10,20));
		default:
			break;
		}
	}

void CRKWindow::SetState(TRKStates aState)
	{
	iState=aState;
	DrawNow();
	}

TBool CRKWindow::CheckResults()
	{
//
// Checks repeat results, first convert everything to 10th's as that what is actually used 
// for the timer in the window server.
//
// Return ETrue if the inacuracy in the average time is greater than 1/10th either way
// Allow initial 2/10ths either
// Allow min 2/10ths below
// Allow max 2/10ths above
//
	if (iState!=EStateInactive)
		return(ETrue);
	TInt initial=iInitialGap.Int()/100000;
	TInt initialX=iInitialRepeatSet.Int()/100000;
	if (initialX==0)
		initialX=1;
	TInt average=(iTotalGap.Int()/100000)/(iRepCount-1);
	TInt repeatX=iRepeatSet.Int()/100000;
	if (repeatX==0)
		repeatX=1;
	TInt min=iMinGap.Int()/100000;
	TInt max=iMaxGap.Int()/100000;
	if (average>(repeatX+1) || average<(repeatX-1))
		return(ETrue);
	if (initial>(initialX+2) || initial<(initialX-2))
		return(ETrue);
	if (min>(repeatX+1) || min<(repeatX-2))
		return(ETrue);
	if (max>(repeatX+3) || max<repeatX)
		return(ETrue);
	return(EFalse);
	}

TDesC& CRKWindow::Report()
	{
	if (iState!=EStateInactive)
		{
		iReport.Format(_L("Error, test not completed"));
		}
	else
		{
		TInt initial=iInitialGap.Int()/10000;
		TInt initialX=iInitialRepeatSet.Int()/10000;
		TInt average=(iTotalGap.Int()/10000/(iRepCount-1));
		TInt repeatX=iRepeatSet.Int()/10000;
		TInt min=iMinGap.Int()/10000;
		TInt max=iMaxGap.Int()/10000;
		iReport.Format(TRefByValue<const TDesC>(_L("Initial=%d [%d], Av=%d [%d], Min=%d, Max=%d")),initial,initialX,average,repeatX,min,max);
		}
	return(iReport);
	}

void CRKWindow::KeyDownL(const TKeyEvent &aKey,const TTime &)
	{
	switch(iState)
		{
		case EStateWaitingForKeyDown:
			SetState(EStateWaitingForKeyCode);
			iDownCode=aKey.iScanCode;
			break;
		default:;
		}
	}

void CRKWindow::KeyUpL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iScanCode==iDownCode)
		{
		switch(iState)
			{
			case EStateWaitingForKeyUp:
				SetState(EStateInactive);
				break;
			default:
				SetState(EStateError);
				break;
			}
		CActiveScheduler::Stop();
		}
	}

void CRKWindow::WinKeyL(const TKeyEvent &aKey,const TTime &aTime)
	{
	if (aKey.iCode==EKeyEscape)
		{
		CActiveScheduler::Stop();
		iTest->iAbort=ETrue;
		}
	if (aKey.iCode==32)
		{
		switch(iState)
			{
			case EStateWaitingForKeyCode:
				SetState(EStateWaitingForFirstRepeat);
				iPrevTime=aTime;
				break;
			case EStateWaitingForFirstRepeat:
				iRepCount=1;
				iInitialGap = I64LOW(aTime.MicroSecondsFrom(iPrevTime).Int64());
				SetState(EStateWaitingForNthRepeat);
				break;
			case EStateWaitingForNthRepeat:
				if (iRepCount==5)
					SetState(EStateWaitingForKeyUp);
				else
					{
					TTimeIntervalMicroSeconds32 gap(I64LOW(aTime.MicroSecondsFrom(iPrevTime).Int64()));
					if (gap<iMinGap)
						iMinGap=gap;
					if (gap>iMaxGap)
						iMaxGap=gap;
					iTotalGap=iTotalGap.Int()+gap.Int();	// Horrible way to do a +=
					iRepCount++;
					SetState(EStateWaitingForNthRepeat);
					}
			case EStateWaitingForKeyUp: 	// Do nothing here
				break;
			default:
				iTest->TestL(EFalse);
			}
		iPrevTime=aTime;
		}
	}

void CRKWindow::SetKeyboardRepeatRate(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime)
	{
	iInitialRepeatSet=aInitialTime;
	iRepeatSet=aTime;
	iMinGap=TTimeIntervalMicroSeconds32(100000000);	// Any very big number will do
	iMaxGap=TTimeIntervalMicroSeconds32(0);
	iTotalGap=TTimeIntervalMicroSeconds32(0);
	SetState(EStateWaitingForKeyDown);
	Client()->iWs.Flush();
	}

//

TKRepeatTest::TKRepeatTest() : CTestBase(_L("KRepeat"))
	{}

TKRepeatTest::~TKRepeatTest()
	{
	CTWin::Delete(iWin);
	Client()->iWs.SetKeyboardRepeatRate(iOldInitialTime, iOldTime);
	}

void TKRepeatTest::ConstructL()
	{
	iWin=new(ELeave) CRKWindow(this);
	TSize screenSize=Client()->iGroup->Size();
	iWin->SetUpL(TPoint(5,5),TSize(screenSize.iWidth/2,screenSize.iHeight-10),Client()->iGroup,*Client()->iGc);
	Client()->iGroup->SetCurrentWindow(iWin);
	Client()->iWs.GetKeyboardRepeatRate(iOldInitialTime, iOldTime);
	}

TInt TKRepeatTest::CheckReportL()
	{
	if (iWin->CheckResults())
		{
		CTDialog *dialog=new(ELeave) CTDialog();
		dialog->SetTitle(_L("Keyboard repeat innacuracies"));
		dialog->SetLine1(iWin->Report());
		dialog->SetNumButtons(2);
		dialog->SetButtonText(0,_L("Okay"));
		dialog->SetButtonText(1,_L("Retest"));
		dialog->SetButtonText(2,_L("Fail"));
		dialog->ConstructLD(*Client()->iGroup,*Client()->iGc);
		switch(dialog->Display())
			{
			case 0:
				break;
			case 1:
				return(ETrue);	// Redo test
			case 2:
				TestL(EFalse);
				break;
			}
		}
	return(EFalse);
	}

void TKRepeatTest::TestKeyboardRepeatRateL(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime)
	{
	do
		{
		Client()->iWs.SetKeyboardRepeatRate(aInitialTime, aTime);
		iWin->SetKeyboardRepeatRate(aInitialTime, aTime);
		CActiveScheduler::Start();
		if (iAbort)
			AbortL();
		} while(CheckReportL());
	}

TestState TKRepeatTest::DoTestL()
	{
	switch(iState)
		{
		case 0:
			LogSubTest(_L("Keyboard Repeat"),1);
			TestKeyboardRepeatRateL(TTimeIntervalMicroSeconds32(1000000), TTimeIntervalMicroSeconds32(500000));
			LogSubTest(_L("Keyboard Repeat"),2);
			TestKeyboardRepeatRateL(TTimeIntervalMicroSeconds32(200000), TTimeIntervalMicroSeconds32(100000));
			LogSubTest(_L("Keyboard Repeat"),3);
			TestKeyboardRepeatRateL(TTimeIntervalMicroSeconds32(0), TTimeIntervalMicroSeconds32(100000));
			LogSubTest(_L("Keyboard Repeat"),4);
			TestKeyboardRepeatRateL(TTimeIntervalMicroSeconds32(100000), TTimeIntervalMicroSeconds32(100000));
			iState++;
			break;
		default:
			return(EFinished);
		}
	return(ENext);
	}

