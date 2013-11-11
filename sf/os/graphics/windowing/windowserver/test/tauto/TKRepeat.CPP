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
// Converted from TMan test code May 2001
// Keyboard repeat test
// 
//

#include "TKRepeat.H"

#define LOGGING 1

//
// CRKWindow, class //
//

CRKWindow::CRKWindow(CTKRepeat *aTest) : CTWin(), iTest(aTest)
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
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KStateChange,"State Change  Old=%d  New=%d");
	logMessageText.Format(KStateChange,iState,aState);
	iTest->LOG_MESSAGE(logMessageText);
#endif
	iState=aState;
	DrawNow();
	TheClient->WaitForRedrawsToFinish();
	SendEvent();
	}

void CRKWindow::SendEvent()
	{
	switch (iState)
		{
	case EStateWaitingForKeyDown:
	case EStateWaitingForKeyUp:
	#if defined(LOGGING)
		_LIT(KKeyUp,"Send key up event  substate 0");
		_LIT(KKeyDown,"Send key down event  substate 4");
		iTest->LOG_MESSAGE(iState==EStateWaitingForKeyDown ? KKeyDown() : KKeyUp());
	#endif
		iTest->TestBase()->SimulateKey((iState==EStateWaitingForKeyDown ? TRawEvent::EKeyDown : TRawEvent::EKeyUp),EStdKeySpace);
		break;
	case EStateWaitingForKeyCode:
	case EStateWaitingForFirstRepeat:
	case EStateWaitingForNthRepeat:
	case EStateInactive:
	case EStateError:
	#if defined(LOGGING)
		_LIT(KLog,"SendEvent:  substate %d so no event to send");
		iTest->LOG_MESSAGE2(KLog,iState);
	#endif
		break;
		}
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
#if defined(__MARM_ARM4__)
	if (min>(repeatX+1) || min<(repeatX-2) || max>(repeatX+3) || max<repeatX)
		{
		if (max<min || (max+min)<2*(repeatX-2) || (max+min)>2*(repeatX+2))
			return(ETrue);
		}
#else
	if (min>(repeatX+1) || min<(repeatX-2))
		return(ETrue);
	if (max>(repeatX+3) || max<repeatX)
		return(ETrue);
#endif
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
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KKeyDown,"CRKWindow::KeyDownL  Code=%d '%c'  State=%d (0)");
	logMessageText.Format(KKeyDown,aKey.iScanCode,aKey.iScanCode,iState);
	iTest->LOG_MESSAGE(logMessageText);
#endif
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
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KKeyUp,"CRKWindow::KeyUpL  Code=%d (%c)  State=%d  Down=%d");
	logMessageText.Format(KKeyUp,aKey.iScanCode,aKey.iScanCode,iState,iDownCode);
	iTest->LOG_MESSAGE(logMessageText);
#endif
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
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KKey,"CRKWindow::WinKeyL  Code=%d (%c)  State=%d  RepeatCount=%d");
	logMessageText.Format(KKey,aKey.iScanCode,aKey.iScanCode,iState,iRepCount);
	iTest->LOG_MESSAGE(logMessageText);
#endif
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
				#if defined(LOGGING)
					TLogMessageText logMessageText;
					_LIT(KRepeatGap,"Repeat after %d");
					logMessageText.AppendFormat(KRepeatGap,gap.Int());
					iTest->LOG_MESSAGE(logMessageText);
					TheClient->Flush();
				#endif
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
				//iTest->Test(EFalse);
				iTest->TestBase()->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
				CActiveScheduler::Stop();
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

CTKRepeat::CTKRepeat(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}

CTKRepeat::~CTKRepeat()
	{
	CTWin::Delete(iWin);
	Client()->iWs.SetKeyboardRepeatRate(iOldInitialTime, iOldTime);
	}

void CTKRepeat::ConstructL()
	{
	TheClient->iWs.SetFocusScreen(iTest->iScreenNumber);
	iWin=new(ELeave) CRKWindow(this);
	TSize screenSize=Client()->iGroup->Size();
	iWin->SetUpL(TPoint(5,5),TSize(Min(Max(screenSize.iWidth/2,250),screenSize.iWidth-10),screenSize.iHeight-10),Client()->iGroup,*Client()->iGc);
	Client()->iGroup->WinTreeNode()->SetOrdinalPosition(0);
	Client()->iGroup->SetCurrentWindow(iWin);
	Client()->iWs.GetKeyboardRepeatRate(iOldInitialTime, iOldTime);
	iTest->SimulateKeyDownUp(EStdKeyLeftCtrl);
	iTest->SimulateKeyDownUp(EStdKeyRightCtrl);
	TInt mods=Client()->iWs.GetModifierState();
	TheClient->WaitForRedrawsToFinish();		//Let all pending events be processed before test begins
	_LIT(KLog,"Initial Modifiers state 0x%x (ideally should be zero)");
	LOG_MESSAGE2(KLog,mods);
	}

TInt CTKRepeat::CheckReportL()
	{
	if (iWin->CheckResults())
		{
		/*CTDialog *dialog=new(ELeave) CTDialog();
		dialog->SetTitle(_L("Keyboard repeat innacuracies"));
		dialog->SetLine1(iWin->Report());
		dialog->SetNumButtons(3);
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
				Client()->iGroup->ClearCurrentWindow();
				Test(EFalse);
				break;
			}
		*/}
	return(EFalse);
	}

void CTKRepeat::TestKeyboardRepeatRateL(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime)
	{
	do
		{
	#if defined(LOGGING)
		const TInt KOneSec=1000000;
		const TInt KOneHundrethSec=KOneSec/100;
		TLogMessageText logMessageText;
		_LIT(KRepeatRate,"Repeat Rate Initial=%d.%02dsecs, Subsequent=%d.%02dsecs");
		logMessageText.Format(KRepeatRate,aInitialTime.Int()/KOneSec,(aInitialTime.Int()%KOneSec)/KOneHundrethSec
																	,aTime.Int()/KOneSec,(aTime.Int()%KOneSec)/KOneHundrethSec);
		LOG_MESSAGE(logMessageText);
	#endif
		TheClient->iWs.SetKeyboardRepeatRate(aInitialTime, aTime);
		iWin->SetKeyboardRepeatRate(aInitialTime, aTime);
		CActiveScheduler::Start();
		if (iAbort)
			{
			iTest->AbortL();
			}
		} while(CheckReportL());
	}
	
void CTKRepeat::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTestName1,"Keyboard Repeat 1");
	_LIT(KTestName2,"Keyboard Repeat 2");
	_LIT(KTestName3,"Keyboard Repeat 3");
	_LIT(KTestName4,"Keyboard Repeat 4");
	((CTKRepeatStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**

  @SYMTestCaseID GRAPHICS-WSERV-0311
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test that a key can be repeatedly struck after a second, then every half second
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Simulate a key being struck first after a second and then repeatedly every
  					half second and check the response time to the key strike is correct 
  
  @SYMTestExpectedResults Response time each time the key is struck is correct
 
*/
		case 1:
			((CTKRepeatStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0311"));
			iTest->LogSubTest(KTestName1);
			TestKeyboardRepeatRateL(TTimeIntervalMicroSeconds32(1000000), TTimeIntervalMicroSeconds32(500000));
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0312
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test that a key can be repeatedly struck after a 5th of a second, then every 10th of a second
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Simulate a key being struck first after a 5th of a second and then repeatedly every
  					10th of a second and check the response time to the key strike is correct 
  
  @SYMTestExpectedResults Response time each time the key is struck is correct
 
*/
		case 2:
			((CTKRepeatStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0312"));
			iTest->LogSubTest(KTestName2);
			TestKeyboardRepeatRateL(TTimeIntervalMicroSeconds32(200000), TTimeIntervalMicroSeconds32(100000));
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0313
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test that a key can be repeatedly struck immediately, then every 10th of a second
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Simulate a key being struck first immediately and then repeatedly every
  					10th of a second and check the response time to the key strike is correct 
  
  @SYMTestExpectedResults Response time each time the key is struck is correct
 
*/
		case 3:
			((CTKRepeatStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0313"));
			iTest->LogSubTest(KTestName3);
			TestKeyboardRepeatRateL(TTimeIntervalMicroSeconds32(0), TTimeIntervalMicroSeconds32(100000));
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0314
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test that a key can be repeatedly struck after a 10 of a second, then every 10th of a second
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Simulate a key being struck first after a 10th of a second and then repeatedly every
  					10th of a second and check the response time to the key strike is correct 
  
  @SYMTestExpectedResults Response time each time the key is struck is correct
 
*/

		case 4:
			((CTKRepeatStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0314"));
			iTest->LogSubTest(KTestName4);
			TestKeyboardRepeatRateL(TTimeIntervalMicroSeconds32(100000), TTimeIntervalMicroSeconds32(100000));
			break;
		case 5:
			((CTKRepeatStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTKRepeatStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTKRepeatStep*)iStep)->RecordTestResultL();
	}
	
__WS_CONSTRUCT_STEP__(KRepeat)

