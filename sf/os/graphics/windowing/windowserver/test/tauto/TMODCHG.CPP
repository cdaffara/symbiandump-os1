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
// Coverted from TMan test code (TMMODCHG.CPP) August 2000
// Test modifier changed message
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TMODCHG.H"

#define LOGGING on	//Uncoment this line to get extra logging

struct TModifierChangedTestsParams
	{
	TText *txt;			// Text message telling user what to do
	TUint changed;
	TUint state;
	TUint stateMask;
	};

LOCAL_D TModifierChangedTestsParams ModifierChangedTests[]={
	{(TText *)_S("Press Caps"),EModifierCapsLock,EModifierCapsLock,EModifierCapsLock},
	{(TText *)_S("Press Caps (again)"),EModifierCapsLock,0,EModifierCapsLock},
	{(TText *)_S("Press left shift"),EModifierShift|EModifierLeftShift,EModifierShift|EModifierLeftShift,EModifierShift|EModifierLeftShift},
	{(TText *)_S("Release left shift"),EModifierShift|EModifierLeftShift,0,EModifierShift|EModifierLeftShift},
	{(TText *)_S("Press right shift"),EModifierShift|EModifierRightShift,EModifierShift|EModifierRightShift,EModifierShift|EModifierRightShift},
	{(TText *)_S("Release right shift"),EModifierShift|EModifierRightShift,0,EModifierShift|EModifierRightShift},
	{(TText *)_S("Press (left) func"),EModifierFunc|EModifierLeftFunc,EModifierFunc|EModifierLeftFunc,EModifierFunc|EModifierLeftFunc},
	{(TText *)_S("Release (left) func"),EModifierFunc|EModifierLeftFunc,0,EModifierFunc|EModifierLeftFunc},
	{(TText *)_S("Press (left) control"),EModifierCtrl|EModifierLeftCtrl,EModifierCtrl|EModifierLeftCtrl,EModifierCtrl|EModifierLeftCtrl},
	{(TText *)_S("Release (left) control"),EModifierCtrl|EModifierLeftCtrl,0,EModifierCtrl|EModifierLeftCtrl},
	};

// list of ModifierEvents that should be monitored
LOCAL_D TInt ModifierChangedEvents=EModifierShift|EModifierLeftShift|EModifierRightShift|
	EModifierCapsLock|EModifierFunc|EModifierLeftFunc|EModifierCtrl|EModifierLeftCtrl;

const TInt numTests=sizeof(ModifierChangedTests)/sizeof(ModifierChangedTests[0]);


//
// CTEventWindowGroup class //
//

CTEventWindowGroup::CTEventWindowGroup(CTClient *aClient, CTModifiersChanged *aTest) : CTWindowGroup(aClient), iTest(aTest)
	{}

void CTEventWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	iGroupWin.EnableModifierChangedEvents(ModifierChangedEvents,EEventControlAlways);
	}

void CTEventWindowGroup::ModifiersChanged(const TModifiersChangedEvent &aModifiersChanged, const TTime &)
	{
	iTest->TestModifiersState(aModifiersChanged); // tests if event is correct
	}

void CTEventWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode==EKeyEscape || (aKey.iCode==' ' && iTest->iModSetTest))
		iTest->EndTest(); // not really needed, because timer cancels long running tests
	}

//
// CMCWindow, class //
//

CMCWindow::CMCWindow(CTModifiersChanged *aTest) : CTWin(), iTest(aTest)
	{
	iBack=TRgb::Gray256(230);
	}

void CMCWindow::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iWin.SetBackgroundColor(iBack);
	Activate();
	AssignGC(aGc);
	iLineHeight=iFont->HeightInPixels()*5/4;
	iFontAscent=iFont->AscentInPixels();
	iXpos1=4;
	iXpos2=iXpos1+12*iFont->TextWidthInPixels(_L("M"));
	}

void CMCWindow::DrawModSetLine(const TDesC &aDes,TInt aModifier,TInt iSettableModifiers)
	{
	iYpos+=iLineHeight;
	iGc->DrawText(aDes, TPoint(iXpos1,iYpos));
	iGc->DrawText(aModifier&iSettableModifiers ? _L("Yes"):_L("No"), TPoint(iXpos2,iYpos));
	}

void CMCWindow::Draw()
	{
	iGc->Clear();
	if (iTest->iModSetTest)
		{
		iYpos=iFontAscent+3;
		TBuf<32> buf;
		buf.Format(TRefByValue<const TDesC>(_L("Settable? [0x%4x]")), iTest->iSettable);
		iGc->DrawText(_L("Modifier"), TPoint(iXpos1,iYpos));
		iGc->DrawText(buf, TPoint(iXpos2,iYpos));
		DrawModSetLine(_L("Shift"),EModifierShift,iTest->iSettable);
		DrawModSetLine(_L("Left Shift"),EModifierLeftShift,iTest->iSettable);
		DrawModSetLine(_L("Func"),EModifierFunc,iTest->iSettable);
		DrawModSetLine(_L("Caps"),EModifierCapsLock,iTest->iSettable);
		DrawModSetLine(_L("NumLock"),EModifierNumLock,iTest->iSettable);
		DrawModSetLine(_L("Double Click"),EModifierDoubleClick,iTest->iSettable);
		}
	else
		iGc->DrawText(TPtrC(ModifierChangedTests[iTest->SubState()].txt), TPoint(10,20));
	}

//

CTModifiersChanged::CTModifiersChanged(CTestStep* aStep): CTWsGraphicsBase(aStep), 
	iSkipFirstModifierEvents(EFalse), iSubSchedulerRunning(EFalse)
	{
	}

CTModifiersChanged::~CTModifiersChanged()
	{
	delete iTimeOut;
	delete iWin;
	delete iEventGroup;
	}

void CTModifiersChanged::ConstructL()
	{
	iTimeOut=new(ELeave) CTimeOut();
	iTimeOut->ConstructL();
	iWin=new(ELeave) CMCWindow(this);
	iWin->SetUpL(TPoint(10,10),TSize(240,200),Client()->iGroup, *Client()->iGc);
	}

TInt CTModifiersChanged::TimeOut(TAny* aTest) // static
	{
	static_cast<CTModifiersChanged*>(aTest)->TimeOut();
	return(KErrNone);
	}

void CTModifiersChanged::TimeOut()
	{
	#if defined(LOGGING)
		_LIT(KLogTimeOut,"Test timed out after %d secs.");
		LOG_MESSAGE2(KLogTimeOut,KTimeOutAfter/1000000);
	#endif
	EndTest();
	}

void CTModifiersChanged::EndTest() 
	{
	TEST(EFalse); // test failed (timeout or manually cancelled)
	if (iModSetTest)
		{
		iModSetTest=EFalse;
		iWin->Invalidate();
		}
	_LIT(KLogEndTest,"EndTest - State %d - SubState %d");
	LOG_MESSAGE3(KLogEndTest,iTest->iState,iSubState);
	if (iSubSchedulerRunning) 
		{
		iSubSchedulerRunning = EFalse;
		CActiveScheduler::Stop(); // stop the sub-scheduler, so test-framework can execute the next test
		}		
	}

TInt CTModifiersChanged::SubState() const
	{
	return(iSubState);
	}

void CTModifiersChanged::IncSubState()
	{
	if (iSubState<numTests-1)
		{
		iSubState++;
		iWin->Invalidate();
		TheClient->WaitForRedrawsToFinish();
		SendEvents();
		}
	else if (iSubState>=numTests-1) // test finished successfully
		{
		if (iSubSchedulerRunning)
			{
			iSubSchedulerRunning = EFalse;
			CActiveScheduler::Stop(); // stop the sub-scheduler, so test-framework can execute the next test
			}
		}
	}

void CTModifiersChanged::TestModifiersState(const TModifiersChangedEvent &aModifiersChanged)
	{
	if (iTest->iState==1) // only if currently in test 1 (add another if for further tests)
		{
		TInt getMods=Client()->iWs.GetModifierState();	// double check the modifiers
		#if defined(LOGGING)
			if (iSkipFirstModifierEvents)
				{
				_LIT(KLogIgnored,"### This Event is part of the initial setup and is ignored for the test. ###");
				LOG_MESSAGE(KLogIgnored);
				}			
			TLogMessageText buf;
			_LIT(KLog1,"##MC1  SS=%x  Test-Modifiers=0x%x  Test-Changed=0x%x  Test-Mask=0x%x");
			buf.Format(KLog1,iSubState,ModifierChangedTests[iSubState].state,ModifierChangedTests[iSubState].changed,ModifierChangedTests[iSubState].stateMask);
			LOG_MESSAGE(buf);
			_LIT(KLog2,"##MC2  Event-Modifiers=0x%x  Event-Changed=0x%x  Get-Modifiers=0x%x");
			buf.Format(KLog2,aModifiersChanged.iModifiers,aModifiersChanged.iChangedModifiers,getMods);
			LOG_MESSAGE(buf);
			_LIT(KLog3,"##MC3  Changed:              (Event) 0x%x==0x%x (Test)");
			buf.Format(KLog3,aModifiersChanged.iChangedModifiers,ModifierChangedTests[iSubState].changed);
			LOG_MESSAGE(buf);
			_LIT(KLog4,"##MC4  Modifier (with mask): (Event) 0x%x==0x%x (Test)");
			buf.Format(KLog4,aModifiersChanged.iModifiers&ModifierChangedTests[iSubState].stateMask,ModifierChangedTests[iSubState].state);
			LOG_MESSAGE(buf);
			_LIT(KLog5,"##MC5  Modifier (with mask):   (Get) 0x%x==0x%x (Test)");
			buf.Format(KLog5,getMods&ModifierChangedTests[iSubState].stateMask,ModifierChangedTests[iSubState].state);
			LOG_MESSAGE(buf);
		#endif
		if (iSkipFirstModifierEvents) // skip the events caused by the initialization
			{
			if(--iSkipCounter<=0)
				{
				iSkipFirstModifierEvents = EFalse;
				}
			}
		else
			{
			if (aModifiersChanged.iChangedModifiers==ModifierChangedTests[iSubState].changed && 
					(aModifiersChanged.iModifiers&ModifierChangedTests[iSubState].stateMask)==ModifierChangedTests[iSubState].state &&
					(getMods&ModifierChangedTests[iSubState].stateMask)==ModifierChangedTests[iSubState].state)
				{
				IncSubState(); // subtest is successfull
				}
			else
				{				
				TEST(EFalse); // subtest failed
				_LIT(KLogTestFailed,"Modifier Change test failed.");
				LOG_MESSAGE(KLogTestFailed);
				}
			}
		} // if test 1
	}

void CTModifiersChanged::BadParams()
	{
	}

/**
 * Resets all the modifiers after the test is finished. Make sure that all keys used in the
 * test are in the up-state.
 */
void CTModifiersChanged::ResetModifiers()
	{
	_LIT(KLogResetModifiersError,"Error %d occured while turning off modifier 0x%x");
	TInt err;
	if ((err = Client()->iWs.SetModifierState(EModifierCapsLock,ETurnOffModifier)) != KErrNone)
		LOG_MESSAGE3(KLogResetModifiersError,err, EModifierCapsLock);
	if ((err = Client()->iWs.SetModifierState(EModifierShift,ETurnOffModifier)) != KErrNone)
		LOG_MESSAGE3(KLogResetModifiersError,err, EModifierShift);
	if ((err = Client()->iWs.SetModifierState(EModifierLeftShift,ETurnOffModifier)) != KErrNone)
		LOG_MESSAGE3(KLogResetModifiersError,err, EModifierLeftShift);
	if ((err = Client()->iWs.SetModifierState(EModifierRightShift,ETurnOffModifier)) != KErrNone)
		LOG_MESSAGE3(KLogResetModifiersError,err, EModifierRightShift);
	if ((err = Client()->iWs.SetModifierState(EModifierFunc,ETurnOffModifier)) != KErrNone)
		LOG_MESSAGE3(KLogResetModifiersError,err, EModifierFunc);
	if ((err = Client()->iWs.SetModifierState(EModifierLeftFunc,ETurnOffModifier)) != KErrNone)
		LOG_MESSAGE3(KLogResetModifiersError,err, EModifierLeftFunc);
	if ((err = Client()->iWs.SetModifierState(EModifierCtrl,ETurnOffModifier)) != KErrNone)
		LOG_MESSAGE3(KLogResetModifiersError,err, EModifierCtrl);
	if ((err = Client()->iWs.SetModifierState(EModifierLeftCtrl,ETurnOffModifier)) != KErrNone)
		LOG_MESSAGE3(KLogResetModifiersError,err, EModifierLeftCtrl);
	}

/**
 * Initialises the Modifier state. All tested modifiers must be in the up-state before the
 * test starts. The number of sent key simulations needed for initialization is recorded.
 * Each key simulation during initialization causes a ModifierChanged event which should be 
 * ignored because it's not part of the actual test.
 */
void CTModifiersChanged::ModifierChangedEventsL()
	{
	iEventGroup=new(ELeave) CTEventWindowGroup(Client(), this);
	iEventGroup->ConstructL();
	TInt modifiers=Client()->iWs.GetModifierState();
	
	if ((ModifierChangedEvents&modifiers)>0) // reset modifiers if they are in the down-state
		{
		iSkipFirstModifierEvents = ETrue;
		if ((modifiers&EModifierCapsLock)>0)
			{
			iTest->SimulateKeyDownUp(EStdKeyCapsLock);
			++iSkipCounter;
			modifiers=Client()->iWs.GetModifierState();
			}
		if ((modifiers&EModifierLeftShift)>0)
			{
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftShift);
			++iSkipCounter;
			modifiers=Client()->iWs.GetModifierState();
			}
		if ((modifiers&EModifierRightShift)>0)
			{
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyRightShift);
			++iSkipCounter;
			modifiers=Client()->iWs.GetModifierState();
			}
		if ((modifiers&EModifierLeftFunc)>0)
			{
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftFunc);
			++iSkipCounter;
			modifiers=Client()->iWs.GetModifierState();
			}
		if ((modifiers&EModifierLeftCtrl)>0)
			{
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftCtrl);
			++iSkipCounter;
			}
		}
	modifiers=Client()->iWs.GetModifierState();
	_LIT(KLog,"Initial Modifiers state 0x%x (ideally should be zero)");
	LOG_MESSAGE2(KLog,modifiers);
	}

void CTModifiersChanged::CheckModifier(TEventModifier aModifier)
	{
	TBool retVal;
	TInt oldMods=Client()->iWs.GetModifierState();
	Client()->iWs.SetModifierState(aModifier,EToggleModifier);
	TInt getMods=Client()->iWs.GetModifierState();
	if (oldMods!=getMods)
		{
		_LIT(KLogOff,"Attempted to turn off modifiers 0x%x, 0x%x still on");
		_LIT(KLogOn,"Attempted to turn on modifiers 0x%x, 0x%x still off");
		iSettable|=aModifier;
		Client()->iWs.SetModifierState(aModifier,ETurnOffModifier);
		getMods=Client()->iWs.GetModifierState();
		retVal=!(getMods&aModifier);
		TEST(retVal);
		if (!retVal)
			LOG_MESSAGE3(KLogOff,aModifier,getMods&aModifier);
		Client()->iWs.SetModifierState(aModifier,ETurnOnModifier);
		getMods=Client()->iWs.GetModifierState();
		retVal=getMods&aModifier;
		TEST(retVal);
		if (!retVal)
			LOG_MESSAGE3(KLogOn,aModifier,getMods&aModifier);
		Client()->iWs.SetModifierState(aModifier,ETurnOffModifier);
		getMods=Client()->iWs.GetModifierState();
		retVal=!(getMods&aModifier);
		TEST(retVal);
		if (!retVal)
			LOG_MESSAGE3(KLogOff,aModifier,getMods&aModifier);
		if (oldMods&aModifier)
			Client()->iWs.SetModifierState(aModifier,ETurnOnModifier);
		}
	else
		{
		Client()->iWs.SetModifierState(aModifier,ETurnOffModifier);
		retVal=oldMods==Client()->iWs.GetModifierState();
		TEST(retVal);
		if (!retVal)
			{
			_LIT(KLog,"Attempted to turn off modifiers 0x%x, suceeded when it should have failed");
			LOG_MESSAGE2(KLog,aModifier);
			}
		Client()->iWs.SetModifierState(aModifier,ETurnOnModifier);
		retVal=oldMods==Client()->iWs.GetModifierState();
		TEST(retVal);
		if (!retVal)
			{
			_LIT(KLog,"Attempted to turn on modifiers 0x%x, suceeded when it should have failed");
			LOG_MESSAGE2(KLog,aModifier);
			}
		}
	}

void CTModifiersChanged::SetModifiers()
	{
	iModSetTest=ETrue;
	for(TInt mod=1;mod!=0;mod<<=1)
		CheckModifier((TEventModifier)mod);
	iWin->Invalidate();
	Client()->iWs.Flush();
	}

void CTModifiersChanged::SendEvents()
	{
	iTest->LogSubState(iSubState);
	switch (iSubState)
		{
	case 0:
		iTest->SimulateKeyDownUp(EStdKeyCapsLock);
		break;
	case 1:
		iTest->SimulateKeyDownUp(EStdKeyCapsLock);
		break;
	case 2:
		iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftShift);
		break;
	case 3:
		iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftShift);
		break;
	case 4:
		iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyRightShift);
		break;
	case 5:
		iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyRightShift);
		break;
	case 6:
		iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftFunc);
		break;
	case 7:
		iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftFunc);
		break;
	case 8:
		iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftCtrl);
		break;
	case 9:
		iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftCtrl);
		break;
	default:
		TEST(EFalse);
		return;
		}
	TheClient->iWs.Flush();
	}

void CTModifiersChanged::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KModChange,"Modifier Change");
	_LIT(KTestErrors,"Bad Parameter");
	((CTModifiersChangedStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0239

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test modifier changed message

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test that modifier changed message functions correctly

@SYMTestExpectedResults The message functions correctly
*/
		case 1:
			((CTModifiersChangedStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0239"));
			iTest->LogSubTest(KModChange);
			ModifierChangedEventsL();
			// start a timer to cancel the sub-scheduler
			iTimeOut->Start(KTimeOutAfter,TCallBack(CTModifiersChanged::TimeOut,this));
			SendEvents();
			iSubSchedulerRunning = ETrue;
			CActiveScheduler::Start(); // sub-scheduler blocks this AO (test-framework) until completion
			iTimeOut->Cancel();
			((CTModifiersChangedStep*)iStep)->RecordTestResultL();
			((CTModifiersChangedStep*)iStep)->CloseTMSGraphicsStep();
			break;	
/**
@SYMTestCaseID		GRAPHICS-WSERV-0240

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test bad parameters for modifier changed message

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test using bad parameters for a modifier changed message

@SYMTestExpectedResults Responds correctly when bad parameters are used
*/
		case 2:
			((CTModifiersChangedStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0240"));
			iTest->LogSubTest(KTestErrors);
			BadParams();
			((CTModifiersChangedStep*)iStep)->RecordTestResultL();			
			break;
		default:
			ResetModifiers(); // reset modifiers, so further tests are not influenced
			((CTModifiersChangedStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTModifiersChangedStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
		}
	
	
	}

__WS_CONSTRUCT_STEP__(ModifiersChanged)
