// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Automatically test the window server capabilities.
// 
//

#include <e32std.h>
#include "W32STD.H"
#include "../../SERVER/w32cmd.h"
#include <e32svr.h>
#include <bacline.h>
#include <hal.h>

typedef TInt (*TPanicFunction)(TInt aInt,TDes& capability,TInt aExternalGroupId);
LOCAL_D const TUint KPanicThreadHeapSize=0x2000;
#define EWindowGroupHandle 1234
#define EWindowGroupHandle2 4567
_LIT(KCAPABILITY_ALL,"CAPABILITY_ALL");
_LIT(KCAPABILITY_NONE,"CAPABILITY_NONE");
_LIT(KWRITEDATA_SWEVENT,"WRITEDATA+SWEVENT");
_LIT(KWRITEDATA_POWERMGMT,"WRITEDATA+POWERMGMT");
_LIT(KPOWERMGMT_SWEVENT,"POWERMGMT+SWEVENT");
_LIT(KSetOrdinalPositionPri,"EWsWinOpSetOrdinalPositionPri Capability Check");
_LIT(KSetOrdinalPositionErr,"EWsWinOpSetOrdinalPositionErr Capability Check");
TLogMessageText LogMessageText;

//Enum for indicating which capability is checked.
enum TTestCapability
	{
	EWriteDeviceData,
	ESwEvent,
	EPowerMgmt,
	EDoNotTest,
	ENoCapReq,
	EEikSrvSID,
	ESwEventPri,
	ENoSwEventPri,
	ENoSwEventPriSmallOrdinal,
	ESwEventErr,
	ENoSwEventErr,
	ENoSwEventErrSmallOrdinal,
	};

//Enum for test exit reasons
enum TTestState
	{
	EWsExitReasonBad,
	EWsTestNext,
	EWsTestFinished,
	};
	
	
TTestCapability gTheTestCapability;
TInt gTestState=KErrNone;


//Panics if EWsExitReasonBad
enum TAutoPanics
	{
	EAutoPanicPanicFailed,
	};

//Thread function structure
struct SPanicParams
	{
	TInt num;
	TPanicFunction func;
	TBuf<256> capability;
	TInt externalGroupId;
	};

class CTestBase
	{
public:
	CTestBase();
	~CTestBase();
	void ConstructL(const TDes* aCapabilityTest);
	TInt TestWsPanicL(TPanicFunction aFunction, TInt aInt, const TDes& aCommandLine);
	void UpdateLogsL();
private :
	TInt TestPanicL(SPanicParams* aPtr);
	TInt LaunchPanicThread(RThread& aThread, SPanicParams* aPtr);
	void Test(TInt aCondition);
private :
	TInt iThreadNumber;
	RWsSession iWs;
	TBuf<256> iCapabilityTest;
	TInt iTestCount;
	TInt iTestPass;
	};

//RWsSession derived class to test the EWservMessShutdown message
class RShellWsSession : public RWsSession
	{
public:
	void ShutDown();
	};
	
void RShellWsSession::ShutDown()
	{
	SendReceive(EWservMessShutdown,TIpcArgs(EWservShutdownCheck));
	}
	
LOCAL_C TInt PanicThreadFunc(TAny* aPtr)
	{
	CTrapCleanup* CleanUpStack=CTrapCleanup::New();
	SPanicParams* ptr=(SPanicParams*)aPtr;
	TInt ret;
	TRAP(ret,ret=(*ptr->func)(ptr->num,ptr->capability,ptr->externalGroupId));
	delete CleanUpStack;
	if (ret==EWsExitReasonBad)
		{
		User::Panic(_L("Auto"),EAutoPanicPanicFailed);
		}
	return(ret);
	}

CTestBase::CTestBase() 
	{
	}
	
CTestBase::~CTestBase() 
	{
	iWs.Close();
	}
	
void CTestBase::ConstructL(const TDes* aCapabilityTest)
	{
	iTestCount=iTestPass=0;
	iWs.Connect();
	TLex lex(*aCapabilityTest);
 	TPtrC capability = lex.NextToken();
 	iCapabilityTest.Append(capability);
	}
	
TInt CTestBase::LaunchPanicThread(RThread& aThread, SPanicParams* aPtr)
	{
	TBuf<32> threadName;
	_LIT(KPanicThread, "AutoPanicThread%d");
	threadName.AppendFormat(KPanicThread,iThreadNumber++);
	return(aThread.Create(threadName,PanicThreadFunc,KDefaultStackSize,KPanicThreadHeapSize,KPanicThreadHeapSize,aPtr,EOwnerThread));
	}
	
TInt CTestBase::TestPanicL(SPanicParams* aPtr)
	{
	RThread thread;
	TRequestStatus stat;
	TInt err=LaunchPanicThread(thread, aPtr);
	if (err==KErrAlreadyExists)
		{
		// wait for kernel to clear up old threads
		// and have several attempts at starting the thread
		// if unsuccessful the first time
		for (TInt i=0;i<3;i++)
			{
			User::After(TTimeIntervalMicroSeconds32(100000));		//0.1 secs
			err=LaunchPanicThread(thread, aPtr);
			if (err!=KErrAlreadyExists)
				{
				break;
				}
			}
		}
	User::LeaveIfError(err);
	thread.Logon(stat);
	User::SetJustInTime(EFalse);
	thread.Resume();
	User::WaitForRequest(stat);
	User::SetJustInTime(ETrue);
	TInt threadExit=thread.ExitReason();
	if (threadExit!=EWsTestFinished)
		{
		if (gTheTestCapability==ENoCapReq)
			{
			Test(threadExit==KErrNone);
			}
		else if(gTheTestCapability==EEikSrvSID)
			{
			Test(threadExit==KErrPermissionDenied);
			}
		else if(!iCapabilityTest.Compare(KCAPABILITY_ALL))
			{
			if(gTheTestCapability!=EDoNotTest)
				{
				iTestCount++;
				if(gTheTestCapability==ESwEventPri)
					{
					Test(threadExit==KPasswordWindowGroupPriority);
					}
				else
					{
					Test(threadExit==KErrNone);
					}
				}
			}
		else if(!iCapabilityTest.Compare(KCAPABILITY_NONE))
			{
			if(gTheTestCapability!=EDoNotTest)
				{
				iTestCount++;
				if((gTheTestCapability==ENoSwEventPriSmallOrdinal)||(gTheTestCapability==ENoSwEventPri))
					{
					Test(threadExit==KPasswordWindowGroupPriority-1);
					}
				else if(gTheTestCapability==ENoSwEventErrSmallOrdinal)
					{
					Test(threadExit==KErrNone);
					}
				else if(gTheTestCapability==ENoSwEventErr)
					{
					Test(threadExit==KErrPermissionDenied);
					}
				else
					{
					Test((threadExit==EWservPanicPermissionDenied)||(threadExit==KErrPermissionDenied));
					}
				}
			}
		else if(!iCapabilityTest.Compare(KWRITEDATA_SWEVENT))
			{
			if(gTheTestCapability==EWriteDeviceData || gTheTestCapability==ESwEvent)
				{
				iTestCount++;
				Test(threadExit==KErrNone);
				}
			if(gTheTestCapability==EPowerMgmt)
				{
				iTestCount++;
				Test((threadExit==EWservPanicPermissionDenied)||(threadExit==KErrPermissionDenied));
				}
			}
		else if(!iCapabilityTest.Compare(KWRITEDATA_POWERMGMT))
			{
			if(gTheTestCapability==EWriteDeviceData || gTheTestCapability==EPowerMgmt)
				{
				iTestCount++;
				Test(threadExit==KErrNone);
				}
			if(gTheTestCapability==ESwEvent) 	
				{
				iTestCount++;
				Test((threadExit==EWservPanicPermissionDenied)||(threadExit==KErrPermissionDenied));
				}
			}
		else if(!iCapabilityTest.Compare(KPOWERMGMT_SWEVENT))
			{
			if(gTheTestCapability==EPowerMgmt || gTheTestCapability==ESwEvent)
				{
				iTestCount++;
				Test(threadExit==KErrNone);
				}
			if(gTheTestCapability==EWriteDeviceData) 
				{
				iTestCount++;
				Test((threadExit==EWservPanicPermissionDenied)||(threadExit==KErrPermissionDenied));
				}
			}
		}
	thread.Close();
	return(threadExit);
	}

TInt CTestBase::TestWsPanicL(TPanicFunction aFunction,TInt aTestNo, const TDes& aCommandLine)
	{
	TLex lex(aCommandLine);
 	TPtrC capability = lex.NextToken();
 	TPtrC idstr = lex.NextToken();
 	lex = idstr;
 	TInt id = 0;
 	lex.Val(id);

	SPanicParams params;
	params.num=aTestNo;
	params.func=aFunction;
	params.capability.Copy(capability);
	params.externalGroupId = id;
	return TestPanicL(&params);
	}

void CTestBase::Test(TInt aCondition)
	{
	if(!aCondition)
		{
		TLogMessageText buf;
		_LIT(Fail,"AUTO Failed in Capability Test : ");
		buf.Append(Fail);
		buf.Append(iCapabilityTest);
		iWs.LogMessage(buf);
		iWs.Flush();
		}
	else
		{
		iTestPass++;
		}
	}
	
void CTestBase::UpdateLogsL()
	{
	TBuf<256> testResult;
	RFs fileSession;
	RFile resultFile;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	resultFile.Replace(fileSession,_L("C:\\DATA\\TestResult.Dat"),EFileWrite);
	CleanupClosePushL(resultFile);
	TFileText fileText;
	fileText.Set(resultFile);
	testResult.Num(iTestCount);
	fileText.Write(testResult);
	testResult.Num(iTestPass);
	fileText.Write(testResult);
	resultFile.Close();
	fileSession.Close();
	CleanupStack::PopAndDestroy(&resultFile);
	CleanupStack::PopAndDestroy(&fileSession);
	}
	
TInt TestCapability(TInt aTest, TDes& aCapability, TInt aExternalGroupId)
	{
	_LIT(KDllName,"CLICK");
	RWsSession ws;
	ws.Connect();
	CleanupClosePushL(ws);
	RWindowGroup gr1(ws);
	RWindowGroup gr2(ws);
	gr1.Construct(EWindowGroupHandle,EFalse);
	gr2.Construct(EWindowGroupHandle2,EFalse);
	CleanupClosePushL(gr1);
	CleanupClosePushL(gr2);
	TWsEvent event;
	RSoundPlugIn click1(ws);
	CWsScreenDevice *screenDevice;
	TRawEvent rawEvent;
	CPalette* defPalette=CPalette::NewDefaultL(EColor256);
	TInt ret=KErrNone;
	switch(aTest)
		{
		case 0:
	//		ws.LogMessage(_L("EWsClOpSetKeyboardRepeatRate Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			ret=ws.SetKeyboardRepeatRate(TTimeIntervalMicroSeconds32(1000000), TTimeIntervalMicroSeconds32(500000));
			ws.Flush();
			break;
		case 1:
	//		ws.LogMessage(_L("EWsClOpSetDoubleClick Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			ret=ws.SetDoubleClick(TTimeIntervalMicroSeconds32(900000),10);	
			ws.Flush();
			break;
		case 2:
	//		ws.LogMessage(_L("EWsClOpSendEventToWindowGroup (external group) Capability Check"));
			gTheTestCapability=ESwEvent;
			event.SetType(EEventModifiersChanged);
			ret=ws.SendEventToWindowGroup(aExternalGroupId,event);
			break;
		case 3:
	//		ws.LogMessage(_L("EWsClOpSendEventToAllWindowGroup Capability Check"));
			gTheTestCapability=ESwEvent;
			event.SetType(EEventModifiersChanged);
			ret=ws.SendEventToAllWindowGroups(event);
			break;
		case 4:
	//		ws.LogMessage(_L("EWsClOpSendEventToAllWindowGroupPriority Capability Check"));
			gTheTestCapability=ESwEvent;
			event.SetType(EEventModifiersChanged);
			gr1.SetOrdinalPosition(0,1);
			ret=ws.SendEventToAllWindowGroups(gr1.OrdinalPriority(),event);
			break;
		case 5:
	//		ws.LogMessage(_L("EWsClOpSendEventToOneWindowGroupPerClient Capability Check"));
			gTheTestCapability=ESwEvent;
			event.SetType(EEventModifiersChanged);
			ret=ws.SendEventToOneWindowGroupsPerClient(event);
			break;
		case 6:
	//		ws.LogMessage(_L("EWsClOpSendMessageToWindowGroup (external group) Capability Check"));
			gTheTestCapability=ESwEvent;
			ret=ws.SendMessageToWindowGroup(aExternalGroupId,TUid::Uid(123),_L8("SomeParams"));
			break;
		case 7:
	//		ws.LogMessage(_L("EWsClOpClaimSystemPointerCursorList Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			ret=ws.ClaimSystemPointerCursorList();
			//This may return KErrInUse if succeeds.
			if(ret==KErrInUse)
				{
				ret=KErrNone;
				}
			break;
		case 8:
	//		ws.LogMessage(_L("EWsClOpSetClientCursorMode Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			ret=ws.SetClientCursorMode(EPointerCursorNone);
			ws.Flush();
			break;
		case 9:
			//Check only if capability is not defined,since the windowgroup is not focused.
			if(aCapability.Compare(KCAPABILITY_ALL)&&aCapability.Compare(KWRITEDATA_POWERMGMT)&&aCapability.Compare(KWRITEDATA_SWEVENT))
				{
	//			ws.LogMessage(_L("EWsClOpSetPointerCursorPosition Capability Check"));
				gTheTestCapability=EWriteDeviceData;
				ret=ws.SetPointerCursorPosition(TPoint(60,20));
				ws.Flush();
				}
			else
				{
				gTheTestCapability=EDoNotTest;	
				}
			break;
		case 10:
	//		ws.LogMessage(_L("EWsClOpSetModifierState Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			ret=ws.SetModifierState(EModifierCapsLock,ETurnOnModifier);
			ret=ws.SetModifierState(EModifierCapsLock,ETurnOffModifier); 
			ws.Flush();
			break;
		case 11:
			{
	//		ws.LogMessage(_L("EWsClOpRawEvent Capability Check"));
			gTheTestCapability=ESwEvent;
			rawEvent.Set(TRawEvent::EActive);
			ws.SimulateRawEvent(rawEvent);
			ws.Flush();
			}
			break;
		case 12:
			{
	//		ws.LogMessage(_L("EWsClOpKeyEvent Capability Check"));
			gTheTestCapability=ESwEvent;
			TKeyEvent keyEvent;
			keyEvent.iCode='J';
			keyEvent.iScanCode=0;
			keyEvent.iModifiers=EModifierAutorepeatable;
			keyEvent.iRepeats=0;
			ws.SimulateKeyEvent(keyEvent);
			ws.Flush();
			}
			break;
		case 13:
	//		ws.LogMessage(_L("EWsClOpSendOffEventsToShell Capability Check"));
			gTheTestCapability=EPowerMgmt;
			ret=ws.RequestOffEvents(EFalse);
			break;
		case 14:
	//		ws.LogMessage(_L("EWsClOpSetFaded Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			ret=ws.SetSystemFaded(EFalse);
			break;
		case 15:
			//Since there is some problem in running the  EWsClOpNoFlickerFree
			//code this test is not run if WriteDeviceData capability is defined.
	//		ws.LogMessage(_L("EWsClOpNoFlickerFree Capability Check"));
			if(aCapability.Compare(KCAPABILITY_ALL)&&aCapability.Compare(KWRITEDATA_POWERMGMT)&&aCapability.Compare(KWRITEDATA_SWEVENT))
				{
				CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
				gTheTestCapability=EWriteDeviceData;
				TInt err;
				if ((err=screen->Construct(0))!=KErrNone)
					{
					delete screen;
					User::Leave(err);
					}
				ws.TestWrite(ws.WsHandle(), EWsClOpNoFlickerFree, NULL, 0);
				ws.Flush();
				delete screen;
				}
			else
				{
				gTheTestCapability=EDoNotTest;
				}
			break;
		case 16:
	//		ws.LogMessage(_L("EWsClOpSetFocusScreen Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			ret=ws.SetFocusScreen(0);
			break;
		case 17:
			{
			//Check only if capability is not defined. Otherwise it will shut down the shell.
			if(aCapability.Compare(KCAPABILITY_ALL)&&aCapability.Compare(KWRITEDATA_POWERMGMT)&&aCapability.Compare(KPOWERMGMT_SWEVENT))
				{
	//			ws.LogMessage(_L("EWservMessShutdown Capability Check"));
				gTheTestCapability=EPowerMgmt;
				RShellWsSession wsShell;
				wsShell.Connect();
				wsShell.ShutDown();
				}
			else
				{
				gTheTestCapability=EDoNotTest;	
				}
			break;
			}
		case 18:
	//		ws.LogMessage(_L("EWsWinOpCaptureKey Capability Check"));
			gTheTestCapability=ESwEvent;
			ret=gr2.CaptureKey('a',EModifierFunc,EModifierFunc);
			//If the function succeeds capability check,the ret value is handle identifying the capture key.
			if(ret>0)
				{
				ret=KErrNone;
				}
			break;
		case 19:
	//		ws.LogMessage(_L("EWsWinOpCaptureKeyUpsAndDowns Capability Check"));
			gTheTestCapability=ESwEvent;
			ret=gr2.CaptureKeyUpAndDowns('a',0,0);
			//If the function succeeds capability check,the ret value is handle identifying the capture key.
			if(ret>0)
				{
				ret=KErrNone;
				}
			break;
		case 20:
	//		ws.LogMessage(_L("EWsWinOpCaptureLongKey Capability Check"));
			gTheTestCapability=ESwEvent;
			ret=gr2.CaptureLongKey(EKeyEscape,'e',0,0,2,ELongCaptureNormal|ELongCaptureRepeatEvents);
			//If the function succeeds capability check,the ret value is handle identifying the capture key.
			if(ret>0)
				{
				ret=KErrNone;
				}
			break;
		case 21:
	//		ws.LogMessage(_L("EWsClickOpLoad Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			click1.Construct();
			CleanupClosePushL(click1);
			ret=click1.Load(KDllName);
			CleanupStack::PopAndDestroy(&click1);
			break;
		case 22:
	//		ws.LogMessage(_L("EWsClickOpUnLoad Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			click1.Construct();
			ret=click1.Unload();
			break;
		case 23:
	//		ws.LogMessage(_L("EWsSdOpSetScreenMode Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			screenDevice=new(ELeave) CWsScreenDevice(ws);
			CleanupStack::PushL(screenDevice);
			screenDevice->Construct(0);
			screenDevice->SetScreenMode(0);
			CleanupStack::PopAndDestroy(screenDevice);
			break;
		case 24:
	//		ws.LogMessage(_L("EWsSdOpSetScreenModeEnforcement Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			screenDevice=new(ELeave) CWsScreenDevice(ws);
			CleanupStack::PushL(screenDevice);
			screenDevice->Construct(0);
			screenDevice->SetScreenModeEnforcement(ESizeEnforcementNone);
			ws.Flush();
			CleanupStack::PopAndDestroy(screenDevice);
			break;
		case 25:
			{
	//		ws.LogMessage(_L("EWsSdOpSetPalette Capability Check"));
			gTheTestCapability=EWriteDeviceData;
			screenDevice=new(ELeave) CWsScreenDevice(ws);
			CleanupStack::PushL(screenDevice);
			screenDevice->Construct(0);
			ret=screenDevice->SetCustomPalette(defPalette);
			//Returns KErrNotSupported if succeeds capability check.
			if(ret==KErrNotSupported)
				{
				ret=KErrNone;
				}
			CleanupStack::PopAndDestroy(screenDevice);
			}
			break;
		case 26:
			{
			//Checking the capability for SetHotKeys
	//		ws.LogMessage(_L("EWsClOpSetHotKey Capability Check"));
			gTheTestCapability=ESwEvent;
			ret=ws.SetHotKey(EHotKeyEnableLogging,'e',EModifierFunc|EModifierCtrl|EModifierShift,0);
			ws.Flush();
			}
			break;
		case 27:
			{
			//Checking the capability for ClearHotKeys
	//		ws.LogMessage(_L("EWsClOpClearHotKeys Capability Check"));
			gTheTestCapability=ESwEvent;
			ret=ws.ClearHotKeys(EHotKeyEnableLogging);
			ws.Flush();
			}
			break;
		case 28:
	//		ws.LogMessage(_L("EWsClOpSendEventToWindowGroup (own group) Capability Check"));
			gTheTestCapability=ENoCapReq;
			event.SetType(EEventModifiersChanged);
			ret=ws.SendEventToWindowGroup(gr1.Identifier(),event);
			break;
		case 29:
	//		ws.LogMessage(_L("EWsClOpSendMessageToWindowGroup (own group) Capability Check"));
			gTheTestCapability=ENoCapReq;
			ret=ws.SendMessageToWindowGroup(gr1.Identifier(),TUid::Uid(123),_L8("SomeParams"));
			break;
		case 30:
	//		ws.LogMessage(_L("EWsClOpSetBackLight Capability Check"));
			gTheTestCapability=EEikSrvSID;
			screenDevice=new(ELeave) CWsScreenDevice(ws);
			CleanupStack::PushL(screenDevice);
			screenDevice->Construct(0);
			ret=screenDevice->SetBackLight(ETrue);	//Always returns KErrPermissionDenied.
			CleanupStack::PopAndDestroy(screenDevice);
			break;
		case 31:
			{
			TBool test=EFalse;
			if(!aCapability.Compare(KCAPABILITY_ALL))
				{
				gTheTestCapability=ESwEventPri;
				test=ETrue;
				}
			else if(!aCapability.Compare(KCAPABILITY_NONE))
				{
				gTheTestCapability=ENoSwEventPri;
				test=ETrue;
				}
			else
				{
				gTheTestCapability=EDoNotTest;	
				}
			if(test)
				{
				LogMessageText.Format(KSetOrdinalPositionPri);
				ws.LogMessage(LogMessageText);
				gr1.SetOrdinalPosition(0,KPasswordWindowGroupPriority);
				ret=ws.GetWindowGroupOrdinalPriority(gr1.Identifier());
				}
			}
			break;
		case 32:
			if(!aCapability.Compare(KCAPABILITY_NONE))
				{
				LogMessageText.Format(KSetOrdinalPositionPri);
				ws.LogMessage(LogMessageText);
				gTheTestCapability=ENoSwEventPriSmallOrdinal;
				gr1.SetOrdinalPosition(0,KPasswordWindowGroupPriority-1);
				ret=ws.GetWindowGroupOrdinalPriority(gr1.Identifier());
				}
			else
				{
				gTheTestCapability=EDoNotTest;	
				}
			break;
		case 33:
			{
			TBool test=EFalse;
			if(!aCapability.Compare(KCAPABILITY_ALL))
				{
				gTheTestCapability=ESwEventErr;
				test=ETrue;
				}
			else if(!aCapability.Compare(KCAPABILITY_NONE))
				{
				gTheTestCapability=ENoSwEventErr;
				test=ETrue;
				}
			else
				{
				gTheTestCapability=EDoNotTest;	
				}
			if(test)
				{
				LogMessageText.Format(KSetOrdinalPositionErr);
				ws.LogMessage(LogMessageText);
				ret=gr1.SetOrdinalPositionErr(0,KPasswordWindowGroupPriority);
				}
			}
			break;
		case 34:
			if(!aCapability.Compare(KCAPABILITY_NONE))
				{
				LogMessageText.Format(KSetOrdinalPositionErr);
				ws.LogMessage(LogMessageText);
				gTheTestCapability=ENoSwEventErrSmallOrdinal;
				ret=gr1.SetOrdinalPositionErr(0,KPasswordWindowGroupPriority-1);
				}
			else
				{
				gTheTestCapability=EDoNotTest;	
				}
			break;
		case 35:
			// EWsClOpSetCloseProximityThresholds Capability Check			
			gTheTestCapability=EWriteDeviceData;
			ret=ws.SetCloseProximityThresholds(-20, -50);
			ws.SetCloseProximityThresholds(KMaxTInt, KMinTInt);
			break;
		case 36:
			// EWsClOpSetHighPressureThresholds Capability Check			
			gTheTestCapability=EWriteDeviceData;
			ret=ws.SetHighPressureThresholds(4000, 2000);
			ws.SetHighPressureThresholds(KMaxTInt, KMinTInt);
			break;
		default:
			ret=gTestState=EWsTestFinished;
		}
	CleanupStack::PopAndDestroy(&gr2);	
	CleanupStack::PopAndDestroy(&gr1);
	CleanupStack::PopAndDestroy(&ws);
	return ret;
	}

void MainL()
	{
    TBuf<256> commandLine;
    User::CommandLine(commandLine);
	CTestBase testBase;
	testBase.ConstructL(&commandLine);
	TInt ii=0;
	while(gTestState!=EWsTestFinished)
		testBase.TestWsPanicL(&TestCapability,ii++,commandLine) ;
//	testBase.UpdateLogsL();	
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanUpStack=CTrapCleanup::New();
	if(cleanUpStack==NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL())
	delete cleanUpStack;
	__UHEAP_MARKEND;
	return(KErrNone);
	}
