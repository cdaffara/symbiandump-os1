// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Test capture key
// Coverted from TMan test code (TMCAPKEY.CPP) October 2000
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "TCapKey.H"

#define LOGGING on

//
// CAbortHandler

CAbortHandler::CAbortHandler(RWsSession *aWs, CCKAbortConnection *aConnection) : CTEventBase(aWs, 10), iConnection(aConnection)
	{
	}

CAbortHandler::~CAbortHandler()
	{
	Cancel();
	}

void CAbortHandler::Request()
	{
	iWs->PriorityKeyReady(&iStatus);
	SetActive();
	}

void CAbortHandler::DoCancel()
	{
	iWs->PriorityKeyReadyCancel();
	}

void CAbortHandler::doRunL()
	{
	TWsPriorityKeyEvent abortEvent;
	iWs->GetPriorityKey(abortEvent);
	iConnection->ReceivedAbortEvent(abortEvent.Handle(),abortEvent.Key());
	}

//
// CCKConnection

CCKConnectionBase::CCKConnectionBase(CTCaptureKey *aTest) : iTest(aTest)
	{
	}

void CCKConnectionBase::ConstructL()
	{
	CTClient::SetScreenNumber(iTest->ScreenNumber());
	CTClient::ConstructL();
	iGroup=new(ELeave) CCKWindowGroup(this, iTest);
	iGroup->ConstructL();
	}

CCKConnection::CCKConnection(CTCaptureKey *aTest, TInt aConnIndex) : CCKConnectionBase(aTest), iConnIndex(aConnIndex)
	{
	}

CCKConnection::~CCKConnection()
	{
	for(TInt index=0;index<numCapKeys;index++)
		if (capKeys[index].connIndex==iConnIndex)
			iGroup->GroupWin()->CancelCaptureKey(capKeyHandle[index]);
	for(TInt index1=0;index1<numCapKeysWithPriority;index1++)
		if (capKeysWithPriority[index1].connIndex==iConnIndex)
			iGroup->GroupWin()->CancelCaptureKey(capKeyWithPriorityHandle[index1]);
	for(TInt index2=0;index2<numUpDownKeys;index2++)
		if (capUpAndDownKeys[index2].connIndex==iConnIndex)
			iGroup->GroupWin()->CancelCaptureKeyUpAndDowns(capUpDownKeyHandle[index2]);
	for(TInt index3=0;index3<numUpDownKeysWithPriority;index3++)
		if (capUpAndDownKeysWithPriority[index3].connIndex==iConnIndex)
			iGroup->GroupWin()->CancelCaptureKeyUpAndDowns(capUpDownKeyWithPriorityHandle[index3]);
	CTWin::Delete(iWin);
	}

void CCKConnection::SubStateChanged()
	{
	iWin->Invalidate();
	iWs.Flush();
	}

void CCKConnection::ConstructL()
	{
	CCKConnectionBase::ConstructL();
	for(TInt index=0;index<numCapKeys;index++)
		if (capKeys[index].connIndex==iConnIndex)
			capKeyHandle[index]=User::LeaveIfError(iGroup->GroupWin()->CaptureKey(capKeys[index].keyCode,capKeys[index].modifier_mask,capKeys[index].modifiers));
	for(TInt index1=0;index1<numCapKeysWithPriority;index1++)
		if (capKeysWithPriority[index1].connIndex==iConnIndex)
			capKeyWithPriorityHandle[index1]=User::LeaveIfError(iGroup->GroupWin()->CaptureKey(capKeysWithPriority[index1].keyCode,capKeysWithPriority[index1].modifier_mask,capKeysWithPriority[index1].modifiers,capKeysWithPriority[index1].priority));
	for(TInt index2=0;index2<numUpDownKeys;index2++)
		if (capUpAndDownKeys[index2].connIndex==iConnIndex)
			capUpDownKeyHandle[index2]=User::LeaveIfError(iGroup->GroupWin()->CaptureKeyUpAndDowns(capUpAndDownKeys[index2].keyCode,
								capUpAndDownKeys[index2].modifier_mask,capUpAndDownKeys[index2].modifiers));
	for(TInt index3=0;index3<numUpDownKeysWithPriority;index3++)
		if (capUpAndDownKeysWithPriority[index3].connIndex==iConnIndex)
			capUpDownKeyWithPriorityHandle[index3]=User::LeaveIfError(iGroup->GroupWin()->CaptureKeyUpAndDowns(capUpAndDownKeysWithPriority[index3].keyCode,
								capUpAndDownKeysWithPriority[index3].modifier_mask,capUpAndDownKeysWithPriority[index3].modifiers,capUpAndDownKeysWithPriority[index3].priority));
	TSize screenSize=iGroup->Size();
	TInt winWidth=screenSize.iWidth/2;
	TInt winHeight=screenSize.iHeight-10;
	iGroup->GroupWin()->AutoForeground(EFalse);		// Don't allow clicking to cause foreground, might mess up test
	CCKWindow *win=new(ELeave) CCKWindow(iTest, iConnIndex);
	win->SetUpL(TPoint(5,5),TSize(winWidth,winHeight),iGroup,*iGc);
	iWin=win;
	}

void CCKConnection::KeyTestL(TInt aBool)
	{
	if (!aBool)
		{
		CTDialog *dialog= new(ELeave) CTDialog();
		TInt ret=0;
		if (dialog)
			{
			dialog->ConstructLD(*iGroup,*iGc);
			dialog->SetTitle(_L("Invalid key press"));
			dialog->SetLine1(_L("Try again?"));
			dialog->SetNumButtons(2);
			dialog->SetButtonText(0,_L("Fail"));
			dialog->SetButtonText(1,_L("Retry"));
			ret=dialog->Display();
			}
		User::Leave(ret==1 ? ECapKeyRetry : ECapKeyFail);
		}
	}

void CCKConnection::CheckKeyL(const SCapKeyTestParams &aKeyTest, const TKeyEvent &aKey)
	{
	KeyTestL(aKeyTest.keyCode==aKey.iCode);
	KeyTestL(aKeyTest.modifiers==(aKey.iModifiers&aKeyTest.modifier_mask));
	}

void CCKConnection::CheckUpDownL(const SCapKeyTestParams &aKeyTest, const TKeyEvent &aKey)
	{
	if(iTest->Mode()==EModeKeyUpsAndDownsWithPriority)
		KeyTestL(aKeyTest.connIndex==iTest->SubState()-15);
	KeyTestL(aKeyTest.connIndex==iConnIndex);
	KeyTestL(aKeyTest.keyCode==(TUint)aKey.iScanCode);
	KeyTestL(aKeyTest.modifiers==(aKey.iModifiers&aKeyTest.modifier_mask));
	}

void CCKConnection::KeyL(const TKeyEvent &aKey,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KLog,"CCKConnection::KeyL  State=%d  SubState=%d  Mode=%d  KeyCode=%d  ScanCode=%d  Mod=0x%x");
	iTest->LOG_MESSAGE7(KLog,iTest->State(),iTest->SubState(),iTest->Mode(),aKey.iCode,aKey.iScanCode,aKey.iModifiers);
#endif
	if (iTest->Mode()==EModeNormalCapture || iTest->Mode()==EModeNormalCaptureWithPriority)
		{
		//if (aKey.iCode==EKeyEscape)
		//	iTest->Abort();
		if(iTest->Mode()==EModeNormalCaptureWithPriority)
			{
			SCapKeyTestParams tmpCapKeyTest=capKeyTests[iTest->SubState()];
			KeyTestL(tmpCapKeyTest.connIndex==iTest->SubState()-8);
			}
		TRAPD(ret,CheckKeyL(capKeyTests[iTest->SubState()],aKey));
		if (ret==ECapKeyOkay)
			iTest->IncSubState();
		else
			{
			iTest->TEST(ret==ECapKeyRetry);
			if (ret!=ECapKeyRetry)
				iTest->INFO_PRINTF3(_L("CheckKeyL(capKeyTests[iTest->SubState()],aKey) return value - Expected: %d, Actual: %d"), ECapKeyRetry, ret);		
			}
			
		}
	}

void CCKConnection::CheckUpDownL(const TKeyEvent &aKey)
	{
	CheckUpDownL(capKeyTests[iTest->SubState()],aKey);
	}

CCKAbortConnection::CCKAbortConnection(CTCaptureKey *aTest) : CCKConnectionBase(aTest)
	{
	}

CCKAbortConnection::~CCKAbortConnection()
	{
	iGroup->GroupWin()->RemovePriorityKey(3,EModifierCtrl,EModifierCtrl);
	delete iAbortHandler;
	}

void CCKAbortConnection::ConstructL()
	{
	CCKConnectionBase::ConstructL();
	User::LeaveIfError(iGroup->GroupWin()->AddPriorityKey(abortKeyTests[0].keyCode,abortKeyTests[0].modifier_mask,abortKeyTests[0].modifiers));
	User::LeaveIfError(iGroup->GroupWin()->AddPriorityKey(abortKeyTests[1].keyCode,abortKeyTests[1].modifier_mask,abortKeyTests[1].modifiers));
	User::LeaveIfError(iGroup->GroupWin()->AddPriorityKey(abortKeyTests[2].keyCode,abortKeyTests[2].modifier_mask,abortKeyTests[2].modifiers));
	iAbortHandler=new(ELeave) CAbortHandler(&iWs, this);
	iAbortHandler->Construct();
	iWs.Flush();
	}

void CCKAbortConnection::ReceivedAbortEvent(TInt aHandle,TKeyEvent* aKey)
	{
#if defined(LOGGING)
	_LIT(KLog,"CCKAbortConnection::ReceivedAbortEvent  State=%d  SubState=%d  Mode=%d  KeyCode=%d  ScanCode=%d  Mod=0x%x");
	iTest->LOG_MESSAGE7(KLog,iTest->State(),iTest->SubState(),iTest->Mode(),aKey->iCode,aKey->iScanCode,aKey->iModifiers);
#endif
	TInt group=reinterpret_cast<TInt>(iGroup);
	iTest->TEST(aHandle==group);
	if (aHandle!=group)
		{
		_LIT(KLog,"Event Handle is not for the right group, expected=%d actual=%d");
		iTest->LOG_MESSAGE3(KLog,group,aHandle);
		}

	iTest->TEST(aKey->iCode==abortKeyTests[iAbortKeyIndex].keyCode);
	if (aKey->iCode!=abortKeyTests[iAbortKeyIndex].keyCode)
		{
		_LIT(KLog,"The event KeyCode is not right, expected=%d actual=%d");
		iTest->LOG_MESSAGE3(KLog,abortKeyTests[iAbortKeyIndex].keyCode,aKey->iCode);
		}

	iTest->TEST((aKey->iModifiers&abortKeyTests[iAbortKeyIndex].modifier_mask)==abortKeyTests[iAbortKeyIndex].modifiers);
	if ((aKey->iModifiers&abortKeyTests[iAbortKeyIndex].modifier_mask)!=abortKeyTests[iAbortKeyIndex].modifiers)
		{
		_LIT(KLog,"The event modifiers are not right, expected=0x%x actual=0x%x (0x%x) mask=0x%x");
		iTest->LOG_MESSAGE5(KLog,abortKeyTests[iAbortKeyIndex].modifiers,aKey->iModifiers&abortKeyTests[iAbortKeyIndex].modifier_mask
									,aKey->iModifiers,abortKeyTests[iAbortKeyIndex].modifier_mask);
		}
	iAbortKeyIndex++;
	iTest->IncSubState();
	}

void CCKAbortConnection::Foreground()
	{
	iGroup->GroupWin()->SetOrdinalPosition(0);
	iWs.Flush();
	}

//
// CCKWindowGroup class //
//

CCKWindowGroup::CCKWindowGroup(CTClient *aClient, CTCaptureKey *aTest) : CTWindowGroup(aClient), iTest(aTest)
	{}

void CCKWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &time)
	{
#if defined(LOGGING)
	_LIT(KLog,"CCKWindowGroup::KeyL  State=%d  SubState=%d  Mode=%d  KeyCode=%d  ScanCode=%d  Mod=0x%x");
	iTest->LOG_MESSAGE7(KLog,iTest->State(),iTest->SubState(),iTest->Mode(),aKey.iCode,aKey.iScanCode,aKey.iModifiers);
#endif
	if (iTest->Mode()==EModeWaitingForFirstNormalCapWithPriority)
		iTest->SetMode(EModeNormalCaptureWithPriority);
	if (iCurWin)
		iCurWin->WinKeyL(aKey,time);
	else
		((CCKConnection *)iClient)->KeyL(aKey,time);
	}

void CCKWindowGroup::KeyDownL(const TKeyEvent &aKey,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KLog,"CCKWindowGroup::KeyDownL  State=%d  SubState=%d  Mode=%d  KeyCode=%d  ScanCode=%d  Mod=0x%x");
	iTest->LOG_MESSAGE7(KLog,iTest->State(),iTest->SubState(),iTest->Mode(),aKey.iCode,aKey.iScanCode,aKey.iModifiers);
#endif
	if (aKey.iScanCode!=EStdKeyLeftShift && aKey.iScanCode!=EStdKeyRightShift)
		{
		if (iTest->Mode()==EModeWaitingForFirstDown)
			iTest->SetMode(EModeKeyUpsAndDowns);
		if (iTest->Mode()==EModeWaitingForFirstDownWithPriority)
			iTest->SetMode(EModeKeyUpsAndDownsWithPriority);
		if (iTest->Mode()==EModeKeyUpsAndDowns || iTest->Mode()==EModeKeyUpsAndDownsWithPriority)
			((CCKConnection *)iClient)->CheckUpDownL(aKey);
		}
	}

void CCKWindowGroup::KeyUpL(const TKeyEvent &aKey,const TTime &)
	{
#if defined(LOGGING)
	_LIT(KLog,"CCKWindowGroup::KeyUpL  State=%d  SubState=%d  Mode=%d  KeyCode=%d  ScanCode=%d  Mod=0x%x");
	iTest->LOG_MESSAGE7(KLog,iTest->State(),iTest->SubState(),iTest->Mode(),aKey.iCode,aKey.iScanCode,aKey.iModifiers);
#endif
	if (aKey.iScanCode!=EStdKeyLeftShift && aKey.iScanCode!=EStdKeyRightShift)
		{
		if (iTest->Mode()==EModeKeyUpsAndDowns || iTest->Mode()==EModeKeyUpsAndDownsWithPriority)
			{
			((CCKConnection *)iClient)->CheckUpDownL(aKey);
			iTest->IncSubState();
			}
		}
	}

//
// CCKWindow, class //
//

CCKWindow::CCKWindow(CTCaptureKey *aTest, TInt aConnIndex) : CTWin(), iConnIndex(aConnIndex), iTest(aTest)
	{
	iBack=TRgb::Gray256(230);
	}

void CCKWindow::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iWin.SetBackgroundColor(iBack);
	Activate();
	AssignGC(aGc);
	}

void CCKWindow::Draw()
	{
	iGc->Clear();
	if (iConnIndex==0)
		{
		TInt test=iTest->SubState();
		if (test<numTests)
			iGc->DrawText(TPtrC(capKeyTests[test].txt), TPoint(10,20));
		}
	}

//

CTCaptureKey::CTCaptureKey(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}
	
CTCaptureKey::~CTCaptureKey()
	{
	((CTCaptureKeyStep*)iStep)->CloseTMSGraphicsStep();
	delete iConn1;
	delete iConn2;
	delete iConn3;
	delete iAbortConn;
	}

void CTCaptureKey::ConstructL()
	{
	iAbortConn=new(ELeave) CCKAbortConnection(this);
	iAbortConn->ConstructL();
	iConn3=new(ELeave) CCKConnection(this,2);
	iConn3->ConstructL();
	iConn2=new(ELeave) CCKConnection(this,1);
	iConn2->ConstructL();
	iConn1=new(ELeave) CCKConnection(this,0);
	iConn1->ConstructL();
	iMode=EModeNormalCapture;
	SetSelfDrive(ETrue);
	TUint mods=TheClient->iWs.GetModifierState();
	_LIT(KLog,"Initial Modifiers state 0x%x (ideally should be zero)");
	LOG_MESSAGE2(KLog,mods);
	}

TInt CTCaptureKey::SubState() const
	{
	return(iSubState);
	}

void CTCaptureKey::IncSubState()
	{
	iSubState++;
#if defined(LOGGING)
	_LIT(KLog,"IncSubState  State=%d  SubState=%d  Mode=%d");
	LOG_MESSAGE4(KLog,iState,iSubState,iMode);
#endif
	iConn1->SubStateChanged();
	iConn2->SubStateChanged();
	iConn3->SubStateChanged();
	switch(iMode)
		{
		case EModeNormalCapture:
			if (iSubState==numCapKeyTests)
				{
				CaseComplete();
				return;
				}
			break;
		case EModeNormalCaptureWithPriority:
			if (iSubState==numCapKeyTests+numCapKeyWithPriorityTests)
				{
				CaseComplete();
				return;
				}
			break;
		case EModeKeyUpsAndDowns:
			if (iSubState==(numCapKeyTests+numCapKeyWithPriorityTests+numUpDownKeyTests))
				{
				CaseComplete();
				return;
				}
			break;
		case EModeKeyUpsAndDownsWithPriority:
			if (iSubState==(numCapKeyTests+numCapKeyWithPriorityTests+numUpDownKeyTests+numUpDownKeyWithPriorityTests))
				{
				SetSelfDrive(EFalse);
				return;
				}
			break;
		case EModeAbortKeys:
			if (iSubState==(numCapKeyTests+numCapKeyWithPriorityTests+numUpDownKeyTests+numUpDownKeyWithPriorityTests+numAbortKeyTests))
				++iState;
			TheClient->WaitForRedrawsToFinish();
			return;
		default:
			break;
		}
	TheClient->WaitForRedrawsToFinish();
	SendEvents();
	}

void CTCaptureKey::CapKeyPurgingL()
	{
	TInt base=Client()->iWs.ResourceCount();
	RWindowGroup groupWin(Client()->iWs);
	User::LeaveIfError(groupWin.Construct(ENullWsHandle));
	TInt capHandle=User::LeaveIfError(groupWin.CaptureKey('a',EModifierFunc,EModifierFunc));
	User::LeaveIfError(groupWin.CaptureKey('b',EModifierFunc,EModifierFunc));
	TInt retVal = Client()->iWs.ResourceCount();
	TEST(retVal==(base+3));
	if (retVal!=(base+3))
		INFO_PRINTF3(_L("Client()->iWs.ResourceCount() return value - Expected: %d, Actual: %d"), retVal, (base+3));		

	groupWin.CancelCaptureKey(capHandle);
	retVal = Client()->iWs.ResourceCount();
	TEST(retVal==(base+2));
	if (retVal!=(base+2))
		INFO_PRINTF3(_L("Client()->iWs.ResourceCount() return value - Expected: %d, Actual: %d"), retVal, (base+2));		

	groupWin.Close();
	// Check it also freed the extra capture key
	retVal = Client()->iWs.ResourceCount();
	TEST(retVal==base);
	if (retVal!=base)
		INFO_PRINTF3(_L("Client()->iWs.ResourceCount() return value - Expected: %d, Actual: %d"), retVal, base);		

	}

void CTCaptureKey::BadParams()
	{
	TInt retVal;
	TInt resCount=Client()->iWs.ResourceCount();
	for(TInt index=0;index<numErrorKeys;index++)
		{
		retVal = Client()->iGroup->GroupWin()->CaptureKey(errorKeys[index].keyCode,errorKeys[index].modifier_mask,errorKeys[index].modifiers, errorKeys[index].priority);
		TEST(retVal==KErrArgument);
		if (retVal!=KErrArgument)
			{
			_LIT(KMsg, "CaptureKey() with bad parameter set %d returned %d - expected KErrArgument (%d)");
			LOG_MESSAGE4(KMsg, index, retVal, KErrArgument);	
			}

		retVal = Client()->iGroup->GroupWin()->CaptureKeyUpAndDowns(errorKeys[index].keyCode, errorKeys[index].modifier_mask, errorKeys[index].modifiers, errorKeys[index].priority);
		TEST(retVal == KErrArgument);
		if (retVal != KErrArgument)
			{
			_LIT(KMsg, "CaptureKeyUpAndDowns() with bad parameter set %d returned %d - expected KErrArgument (%d)");
			LOG_MESSAGE4(KMsg, index, retVal, KErrArgument);	
			}

		retVal = Client()->iGroup->GroupWin()->CaptureLongKey(errorKeys[index].keyCode, errorKeys[index].keyCode, errorKeys[index].modifier_mask, errorKeys[index].modifiers, errorKeys[index].priority, ELongCaptureNormal);
		TEST(retVal == KErrArgument);
		if (retVal != KErrArgument)
			{
			_LIT(KMsg, "CaptureLongKey() with bad parameter set %d returned %d - expected KErrArgument (%d)");
			LOG_MESSAGE4(KMsg, index, retVal, KErrArgument);	
			}
		}

	retVal = Client()->iWs.ResourceCount();
	TEST(retVal==resCount);
	if (retVal!=resCount)
		{
		_LIT(KMsg, "Resource count %d - expected %d");
		LOG_MESSAGE3(KMsg, retVal, resCount);	
		}
	}

void CTCaptureKey::SetMode(TTestMode aMode)
	{
#if defined(LOGGING)
	_LIT(KLog,"SetMode  State=%d  SubState=%d  OldMode=%d  NewMode=%d");
	LOG_MESSAGE5(KLog,State(),SubState(),iMode,aMode);
#endif
	iMode=aMode;
	}

TTestMode CTCaptureKey::Mode()
	{
	return(iMode);
	}

void CTCaptureKey::SetUpPriorityKeyTest()
	{
	iMode=EModeAbortKeys;
	iAbortConn->Foreground();
	}

void CTCaptureKey::CaptureWithPriorityTest() //normal key capture but with priority
	{
	iMode=EModeWaitingForFirstNormalCapWithPriority;
	}

void CTCaptureKey::CaptureUpsAndDownsTest()
	{
	iMode=EModeWaitingForFirstDown;
	}

void CTCaptureKey::CaptureUpsAndDownsWithPriorityTest()
	{
	iMode=EModeWaitingForFirstDownWithPriority;
	}

void CTCaptureKey::SendEvents()
	{
#if defined(LOGGING)
	_LIT(KLog,"SendEvents  State=%d  SubState=%d");
	LOG_MESSAGE3(KLog,iState,iSubState);
#endif
	switch (iSubState)
		{
	case 0:
		iTest->SimulateKeyDownUp('Q');
		break;
	case 1:
		iTest->SimulateKeyDownUpWithModifiers('W',EModifierAlt);
		break;
	case 2:
		iTest->SimulateKeyDownUp('A');
		break;
	case 3:
		iTest->SimulateKeyDownUpWithModifiers('S',EModifierAlt|EModifierCtrl);
		break;
	case 4:
		iTest->SimulateKeyDownUpWithModifiers('N',EModifierAlt);
		break;
	case 5:
		iTest->SimulateKeyDownUpWithModifiers('B',EModifierAlt|EModifierCtrl);
		break;
	case 6:
		iTest->SimulateKeyDownUpWithModifiers('D',EModifierAlt);
		break;
	case 7:
		iTest->SimulateKeyDownUpWithModifiers('C',EModifierAlt|EModifierCtrl);
		break;
	case 8:
		iTest->SimulateKeyDownUp('H');//state EModeNormalCaptureWithPriority
		break;
	case 9:
		iTest->SimulateKeyDownUp('J');
		break;
	case 10:
		iTest->SimulateKeyDownUp('K');
		break;
	case 11:
		iTest->SimulateKeyDownUp(EStdKeySpace);
		break;
	case 12:
		iTest->SimulateKeyDownUp(EStdKeyEnter);
		break;
	case 13:
		iTest->SimulateKeyDownUpWithModifiers(EStdKeySpace,EModifierShift);
		break;
	case 14:
		iTest->SimulateKeyDownUpWithModifiers(EStdKeyEnter,EModifierShift);
		break;
	case 15:
		iTest->SimulateKeyDownUp('H');//state EModeKeyUpsAndDownsWithPriority
		break;
	case 16:
		iTest->SimulateKeyDownUp('J');
		break;
	case 17:
		iTest->SimulateKeyDownUp('K');
		break;
	case 18:
		iTest->SimulateKeyDownUpWithModifiers('C',EModifierCtrl);
		break;
	case 19:
		iTest->SimulateKeyDownUpWithModifiers('D',EModifierCtrl);
		break;
	case 20:
		iTest->SimulateKeyDownUp('Z');
		break;
	default:
		TEST(EFalse);
		}
	TheClient->iWs.Flush();
	}

void CTCaptureKey::CapKeyPowerOffL()
	{
	const TUint protKey=0xF844;		//This value needs to be defined in the WSINI.INI file as a key only capturable by a certain application
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	CleanupClosePushL(ws);
	
	// assign to the correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(iTest->iScreenNumber));
	
	RWindowGroup group(ws);
	group.Construct(ENullWsHandle);
	CleanupClosePushL(group);
	group.EnableReceiptOfFocus(EFalse);
	group.SetOrdinalPosition(0,5);
	RBlankWindow blank(ws);
	blank.Construct(group,ENullWsHandle);
	CleanupClosePushL(blank);
	blank.EnableBackup();
	blank.Activate();
	_LIT(kGroupName1,"Hello");
	group.SetName(kGroupName1);
	TInt capHandle = group.CaptureKey(EKeyDevice2,EModifierFunc,EModifierFunc);
	TEST(capHandle==KErrPermissionDenied);
	if (capHandle!=KErrPermissionDenied)
		{
		_LIT(KLog1,"Capture Key suceeded when it should of failed, Error/Handle=%d (expected: %d)");
		LOG_MESSAGE3(KLog1,capHandle,KErrPermissionDenied);
		_LIT(KLog2,"This is indicative of a bad WSINI file");
		LOG_MESSAGE(KLog2);
		}

	capHandle=group.CaptureKeyUpAndDowns(protKey,0,0);
	TEST(capHandle==KErrPermissionDenied);
	if (capHandle!=KErrPermissionDenied)
		{
		_LIT(KLog,"Capture Key Up & Down suceeded when it should of failed, Error/Handle=%d (expected: %d)");
		LOG_MESSAGE3(KLog,capHandle,KErrPermissionDenied);
		}
	
	_LIT(kGroupName2,"SysApp");
	group.SetName(kGroupName2);
	capHandle = group.CaptureKey(EKeyDevice2,EModifierFunc,EModifierFunc);
	TEST(capHandle > 0);
	if (capHandle<=0)
		{
		_LIT(KLog,"Capture Key failed when it should of succeeded, Error=%d");
		LOG_MESSAGE2(KLog,capHandle);
		}

	group.CancelCaptureKey(capHandle);
	capHandle=group.CaptureKeyUpAndDowns(protKey,0,0);
	TEST(capHandle > 0);
	if (capHandle<=0)
		{
		_LIT(KLog,"Capture Key Up & Down failed when it should of succeeded, Error=%d");
		LOG_MESSAGE2(KLog,capHandle);
		}

	group.CancelCaptureKeyUpAndDowns(capHandle);
	CleanupStack::PopAndDestroy(4,&ws);
	}


void CTCaptureKey::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(Test0,"Normal capture with priority");
	_LIT(TestNormalCaptureWithPriority,"Normal capture with priority");
	_LIT(TestUpDown,"Capture ups and downs");
	_LIT(TestUpDownWithPriority,"Capture ups and downs with priority");
	_LIT(TestAbort,"Abort key");
	_LIT(TestAbort2,"Abort key extended");
	_LIT(TestErrors,"Errors");
	_LIT(TestCaptureKey,"Capture Key");
	_LIT(TestCapturePowerOffKey,"Capture PowerOff Key");

	iTest->iState=iState;
	((CTCaptureKeyStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0140


@SYMDEF             DEF081259

@SYMTestCaseDesc    Normal key capture test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test uses 3 wserv clients with their own window groups. The 
					clients are registered for different keys to capture. The test 
					simulates key presses.

@SYMTestExpectedResults Checks that the key presses are captured correctly .
*/
		case 0:	// Dummy one to let capture key tests run
			((CTCaptureKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0140"));
			iTest->LogSubTest(Test0);
			iState++;
			SendEvents();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0141

@SYMDEF             DEF081259

@SYMTestCaseDesc    Normal key capture test with priorities

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test uses 3 wserv clients with their own window groups. The 
					clients are registered for different keys to capture and with
					different priorities. The test simulates key presses.

@SYMTestExpectedResults Checks that the key presses are captured correctly .
*/
		case 1:
			((CTCaptureKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0141"));
			iTest->LogSubTest(TestNormalCaptureWithPriority);
			CaptureWithPriorityTest();
			iState++;
			SendEvents();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0142

@SYMDEF             DEF081259

@SYMTestCaseDesc    Up-down key capture test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test uses 3 wserv clients with their own window groups. The 
					clients are registered for different keys to capture their
					up and down events. The test simulates key presses and releases.

@SYMTestExpectedResults Checks that the key presses and releases are captured correctly .
*/
		case 2:
			((CTCaptureKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0142"));
			iTest->LogSubTest(TestUpDown);
			CaptureUpsAndDownsTest();
			iState++;
			SendEvents();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0143

@SYMDEF             DEF081259

@SYMTestCaseDesc    Up-down key capture test with priorities

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test uses 3 wserv clients with their own window groups. The 
					clients are registered with different priorities for different keys to capture their
					up and down events. The test simulates key presses and releases.

@SYMTestExpectedResults Checks that the key presses and releases are captured correctly .
*/
		case 3:
			((CTCaptureKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0143"));
			iTest->LogSubTest(TestUpDownWithPriority);
			CaptureUpsAndDownsWithPriorityTest();
			iState++;
			SendEvents();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0144

@SYMDEF             DEF081259

@SYMTestCaseDesc    Priority key events test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test simulates key events and make sure that the events are 
					recieved by the registered wserv client.

@SYMTestExpectedResults Makes sure that key code is the same as a key simulated, event modifiers comply
					with registered priority key's modifirs and mask, and that event handle 
					is a window group of the session.
					
*/
		case 4:
			((CTCaptureKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0144"));
			if (iSubState==numCapKeyTests+numUpDownKeyWithPriorityTests+numUpDownKeyTests+numUpDownKeyWithPriorityTests)
				{
				iTest->LogSubTest(TestAbort);
				SetUpPriorityKeyTest();
				}
			else
				iTest->LogSubTest(TestAbort2);
			//iState++;
			SendEvents();
			TheClient->WaitUntilEventPending();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0145

@SYMDEF             DEF081259

@SYMTestCaseDesc    Invalid capture keys test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Memorizes amount of objects that the server has allocated for that client.
					Tries to register capture keys with invalid modifiers and
					modifier mask pairs, using each of the RWindowGroup capture
					functions CaptureKey(), CaptureKeyUpAndDowns() and
					CaptureLongKey().

@SYMTestExpectedResults Makes sure that the number of server-side objects for the 
					session didn't change.
*/
		case 5:
			((CTCaptureKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0145"));
			iTest->LogSubTest(TestErrors);
			BadParams();
			((CTCaptureKeyStep*)iStep)->RecordTestResultL();
/**
@SYMTestCaseID		GRAPHICS-WSERV-0146

@SYMDEF             DEF081259

@SYMTestCaseDesc    Capture keys clean up test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Memorizes amount of objects that the server has allocated for that client.
					Creates a window group and requests two keys to be captured.
					Cancels one key capture request and closes the window group.

@SYMTestExpectedResults Checks that creation of the group or registering a capture key
					increased this amount by 1 each time.
					Makes sure that the number of server-side objects for the 
					session in the end of the test is the same as in the beginning.
*/
			((CTCaptureKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0146"));
			iTest->LogSubTest(TestCaptureKey);
			CapKeyPurgingL();
			iState++;
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0147

@SYMDEF             DEF081259

@SYMTestCaseDesc    Capture power off key test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test creates a new session, new window group and a new window
					in the group. After that the test gives "Hello" name to the group
					and tries to register for key capture of FN+EKeyDevice2 and 0xF844
					buttons. Makes sure that registration fails due to KErrPermissionDenied
					error.
					Then the test changes the name of the window group to "SysApp"
					and tries the registration again. Makes sure that operation 
					completed successfully.

@SYMTestExpectedResults See actions
*/
		case 6:
			((CTCaptureKeyStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0147"));
			iTest->LogSubTest(TestCapturePowerOffKey);
			CapKeyPowerOffL();
			iState++;
			break;
		default:
			((CTCaptureKeyStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			TestComplete();
		}
	((CTCaptureKeyStep*)iStep)->RecordTestResultL();

	}


__WS_CONSTRUCT_STEP__(CaptureKey)
