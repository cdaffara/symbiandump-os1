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
// Test window server events
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __INIPARSER_H__
#define __INIPARSER_H__
#endif // __INIPARSER_H__

#include "PARSEINIDATA.H"
#include "TEVENT.H"
#include "W32CLICK.H"
#include <w32debug.h>
#include <hal.h>

#define FAIL_LOGGING on
//#define DETAILED
#define LOGGING 1	//Uncomment this when there is a fail so that you get more logging

_LIT(KKeyClickPluginDll, "click.dll");

const TInt KLongKeyCaptureLowPriority = 1;
const TInt KLongKeyCaptureHighPriority = 2;

CTQueueClient::CTQueueClient(CTEventTest *aTest) : iTest(aTest)
	{
	}

CTQueueClient::~CTQueueClient()
	{
	CloseEventTestGroupOnSecondScreen();
	iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	delete iChildWin;
	delete iWin;

	}

void CTQueueClient::ConstructL(TBool aCreateChild)
	{
	CTClient::ConstructL();
	iGroup=new(ELeave) CTQueueWindowGroup(this);
	iGroup->ConstructL();
	iWin=new(ELeave) CTBackedUpWin(EGray4);
	iWin->ConstructExtLD(*iGroup,TPoint(EWinPositionX,EWinPositionY),TSize(EWinWidth,EWinHeight));
	iWin->Activate();
	if (aCreateChild)
		{
		iChildWin=new(ELeave) CTBackedUpWin(EGray4);
		iChildWin->ConstructExtLD(*iWin,ChildTopLeft(),ChildSize());
		iChildWin->Activate();
		}
	iWs.Flush();
	}

void CTQueueClient::ConstructEventHandlerL()
	{
	iEventHandler=new(ELeave) CTQueueTestEvent(&iWs, iTest);
	((CTQueueTestEvent *)iEventHandler)->ConstructL();
	}

TInt CTQueueClient::WindowGroupIdentifier()
	{
	return(iGroup->GroupWin()->Identifier());
	}

CTWinBase* CTQueueClient::GetHandle(TPoint& aPos)
	{
	if (iCapturing)
		return iChildWin;
	if (iChildWin && TRect(ChildTopLeft(),ChildSize()).Contains(aPos))
	//if (iChildWin && TRect(TPoint(),ChildSize()).Contains(aPos))
		{
		aPos-=ChildTopLeft();
		if (iChildCapturing)
			iCapturing=ETrue;
		return iChildWin;
		}
	return iWin;
	}

void CTQueueClient::AddExpectedEvent(TWsEvent &aEvent)
	{
#if defined(DETAILED)
	_LIT(KLog,"   Adding Event Type=%d");
	TLogMessageText buf;
	buf.Format(KLog,aEvent.Type());
	LogMessage(buf);
#endif
	((CTQueueTestEvent *)iEventHandler)->AddExpectedEvent(aEvent);
	}

void CTQueueClient::SetChildCapture(TInt aCaptureFlags)
	{
	if (!iChildWin)
		return;
	iChildWin->BaseWin()->SetPointerCapture(aCaptureFlags);
	if (aCaptureFlags|RWindowBase::TCaptureFlagEnabled)
		iChildCapturing=ETrue;
	}
//

CTQueueTestEvent::CTQueueTestEvent(RWsSession *aWs, CTEventTest *aTest) : CTEvent(aWs)
	{
	iTest = aTest;
	iCheckTimeStamp = EFalse;
	iLastEventError = EFalse;
	}

void CTQueueTestEvent::ConstructL()
	{
	CTEventBase::Construct();
	iEventBuffer.SetLengthL(EEventBufferSize);
	iRedrawType=iTest->TestBase()->RedrawStoreTypeL();
	}

void CTQueueTestEvent::TestL(TInt aTest)
	{
	if (!aTest)
		{
		iTest->Failed();
		User::Leave(ETestFailed);
		}
	}

//#define FAIL_LOGGING yes
void CTQueueTestEvent::doRunL()
	{
	TWsEvent wsEvent;
	iWs->GetEvent(wsEvent);
	TWsEvent expectedEvent;

#if defined(FAIL_LOGGING)
	TInt eType,wsHan,eHan,wsKeyCode,eKeyCode,wsKeyRepeat,eKeyRepeat,wsKeyScan,eKeyScan;
	TLogMessageText buf;
	_LIT(KStartString,"AUTO  EventTestFail ");
#endif
	TInt wsType=wsEvent.Type();
	if (wsEvent.Type()==EEventNull)
		TestL(wsEvent.Handle()==0);
	else
		{
		TInt count=iEventBuffer.Count();
		if (count==0 && wsType==EEventFocusGained)
			goto End;
		if (iLastEventError
			&& (iRedrawType==EPartialRedraw_FullRedrawSupport || iRedrawType==EPartialRedraw_PreserveStoredCmds)
			&& wsEvent.Type()==EEventErrorMessage /*&& expectedEvent.Type()!=EEventErrorMessage*/)		//Should really check if the next expected event is an error but there is no way to do this without removing it from the buffer
			{
		#if defined(DETAILED)
			_LIT(KLog,"Skip Duplicate Error Message");
			iTest->LOG_MESSAGE(KLog);
		#endif
			return;		//Ignore repeated error events
			}
		do	{
			TestL(iEventBuffer.Remove(&expectedEvent));	//Loop needed for REL Build with DEB WSERV and W32
		#if defined(DETAILED)
			_LIT(KLog,"Skip event while waiting for error message");
			if (expectedEvent.Type()==EEventErrorMessage && wsEvent.Type()!=expectedEvent.Type())
				iTest->LOG_MESSAGE(KLog);
		#endif
			} while (expectedEvent.Type()==EEventErrorMessage && wsEvent.Type()!=expectedEvent.Type());
		++iEventCount;		//iEventSet
	#if defined(FAIL_LOGGING)
		wsType=wsEvent.Type(),eType=expectedEvent.Type(),wsHan=wsEvent.Handle(),eHan=expectedEvent.Handle();
		if (wsType!=eType)
			{
			_LIT(KEventDiff,"Count=%d  Events Differ Exp=%d  Act=%d");
			buf.Copy(KStartString);
			buf.AppendFormat(KEventDiff,iEventCount,eType,wsType);
			iTest->LOG_MESSAGE(buf);
			}
		else
			{
		#if defined(DETAILED)
			_LIT(KLog," Count=%d  Event Type=%d");
			iTest->LOG_MESSAGE3(KLog,iEventCount,wsType);
		#endif
			}
		if (wsHan!=eHan)
			{
			_LIT(KHandlesDiff,"Count=%d  Handles Differ");
			buf.Copy(KStartString);
			buf.AppendFormat(KHandlesDiff,iEventCount);
			iTest->LOG_MESSAGE(buf);
			}
	#endif
		TestL(wsEvent.Type()==expectedEvent.Type() && wsEvent.Handle()==expectedEvent.Handle());
		}		
	iLastEventError=(wsEvent.Type()==EEventErrorMessage);

	switch(wsEvent.Type())
		{
		case EEventKey:
		#if defined(FAIL_LOGGING)
			wsKeyCode=wsEvent.Key()->iCode,eKeyCode=expectedEvent.Key()->iCode;
			if (wsKeyCode!=eKeyCode)
				{
				_LIT(KKeysDiff,"Count=%d  Key Code Exp=%d (%c)  Act=%d (%c)");
				buf.Copy(KStartString);
				buf.AppendFormat(KKeysDiff,iEventCount,eKeyCode,eKeyCode,wsKeyCode,wsKeyCode);
				iTest->LOG_MESSAGE(buf);
				}
			wsKeyRepeat=wsEvent.Key()->iRepeats,eKeyRepeat=expectedEvent.Key()->iRepeats;
			if ((wsKeyRepeat>0)!=(eKeyRepeat>0) || wsKeyRepeat<eKeyRepeat)
				{
				_LIT(KKeysRepDiff,"Count=%d  Key Repeat Exp=%d  Act=%d");
				buf.Copy(KStartString);
				buf.AppendFormat(KKeysRepDiff,iEventCount,eKeyRepeat,wsKeyRepeat);
				iTest->LOG_MESSAGE(buf);
				}
		#endif
			TestL(wsEvent.Key()->iCode==expectedEvent.Key()->iCode &&
				wsEvent.Key()->iRepeats>=expectedEvent.Key()->iRepeats &&
				(wsEvent.Key()->iRepeats>0)==(expectedEvent.Key()->iRepeats>0));
			// Fall Through
		case EEventKeyDown:
		case EEventKeyUp:
			{
			TUint wsModifiers = wsEvent.Key()->iModifiers;
			TUint eModifiers = expectedEvent.Key()->iModifiers;
		#if defined(FAIL_LOGGING)
			wsKeyScan=wsEvent.Key()->iScanCode,eKeyScan=expectedEvent.Key()->iScanCode;
			if (wsKeyScan!=eKeyScan)
				{
				_LIT(KScanDiff,"Count=%d  Scan Code Exp=%d (%c)  Act=%d (%c)");
				buf.Copy(KStartString);
				buf.AppendFormat(KScanDiff,iEventCount,eKeyScan,eKeyScan,wsKeyScan,wsKeyScan);
				iTest->LOG_MESSAGE(buf);
				}

			if (eModifiers != KModifiersIgnore && wsModifiers != eModifiers)
				{
				_LIT(KModDiff,"Count=%d  Modifiers Exp=0x%x  Act=0x%x");
				buf.Copy(KStartString);
				buf.AppendFormat(KModDiff, iEventCount, eModifiers, wsModifiers);
				iTest->LOG_MESSAGE(buf);
				}
		#endif
			TestL(wsEvent.Key()->iScanCode == expectedEvent.Key()->iScanCode &&
				(eModifiers == KModifiersIgnore || wsModifiers == eModifiers));
			}
			break;
		case EEventModifiersChanged:
			{
			TBool changeModifiersOK=(wsEvent.ModifiersChanged()->iChangedModifiers
																==expectedEvent.ModifiersChanged()->iChangedModifiers);
			TBool modifiersOK=((wsEvent.ModifiersChanged()->iModifiers&wsEvent.ModifiersChanged()->iChangedModifiers)==
						(expectedEvent.ModifiersChanged()->iModifiers&wsEvent.ModifiersChanged()->iChangedModifiers));
		#if defined(FAIL_LOGGING)
			if (!changeModifiersOK || !modifiersOK)
				{
				_LIT(KModDiff,"Count=%d  Modifiers");
				buf.Copy(KStartString);
				buf.AppendFormat(KModDiff,iEventCount);
				iTest->LOG_MESSAGE(buf);
				}
		#endif
			TestL(changeModifiersOK);
			TestL(modifiersOK);
			}
			break;
		case EEventPointer:
		case EEventDragDrop:
			{
		#if defined(FAIL_LOGGING)
			TInt expectedType=expectedEvent.Pointer()->iType, actualType=wsEvent.Pointer()->iType;
			TPoint expectedPos=expectedEvent.Pointer()->iPosition, actualPos=wsEvent.Pointer()->iPosition;
			if (actualType!=expectedType)
				{
				_LIT(KPointerDiff,"Count=%d  Pointer Type Exp=%d  Act=%d");
				buf.Copy(KStartString);
				buf.AppendFormat(KPointerDiff,iEventCount,expectedType,actualType);
				iTest->LOG_MESSAGE(buf);
				}
			if (actualPos!=expectedPos)
				{
				_LIT(KPointerPosDiff,"Count=%d  Pointer Pos E=(%d,%d)  A=(%d,%d)");
				buf.Copy(KStartString);
				buf.AppendFormat(KPointerPosDiff,iEventCount,expectedPos.iX,expectedPos.iY
																,actualPos.iX,actualPos.iY);
				iTest->LOG_MESSAGE(buf);
				}
		#endif
			TestL(wsEvent.Pointer()->iType==expectedEvent.Pointer()->iType);
			TestL(wsEvent.Pointer()->iPosition==expectedEvent.Pointer()->iPosition);
			TInt count=iEventBuffer.Count();count++;			//Dummy line to help in debugging.
			}
			break;
		case EEventWindowVisibilityChanged:
			{
		#if defined(FAIL_LOGGING)
			TUint expectedFlags=expectedEvent.VisibilityChanged()->iFlags;
			TUint actualFlags=wsEvent.VisibilityChanged()->iFlags;
			if (actualFlags!=expectedFlags)
				{
				_LIT(KVisibilityDiff,"Count=%d  Visibility Flags Exp=%d  Act=%d");
				buf.Copy(KStartString);
				buf.AppendFormat(KVisibilityDiff,iEventCount,expectedFlags,actualFlags);
				iTest->LOG_MESSAGE(buf);
				}
		#endif
			TestL(wsEvent.VisibilityChanged()->iFlags==expectedEvent.VisibilityChanged()->iFlags);
			TInt count=iEventBuffer.Count();count++;			//Dummy line to help in debugging.
			}
			break;
		case EEventMessageReady:
			{
			const SEventMessageReady message = *(SEventMessageReady*)wsEvent.EventData();
			TestL(message.iMessageUid== TUid::Uid(123));
			}
			break;
		default:;
		}
	if(iCheckTimeStamp)
		{
		const TInt KOffset=100000; //0.1secs
		TInt64 offset = expectedEvent.Time().Int64() - wsEvent.Time().Int64();
		#if defined(FAIL_LOGGING)
			if(Abs(offset)<KOffset)
				{
				_LIT(KTimeStampDiff,"Count=%d  Offset=%d, ExpectedTime=%d, WServEventTime=%d");
				buf.Copy(KStartString);
				buf.AppendFormat(KTimeStampDiff,iEventCount,offset,expectedEvent.Time().Int64(),wsEvent.Time().Int64());
				iTest->LOG_MESSAGE(buf);
				}
		#endif
		TestL(Abs(offset)<KOffset);
		}
	End:
	if (iEventBuffer.Count()==0)
	    {
		TRAPD(err, iTest->NextSetOfEventsL());		//Not allowed to leave
		if(err != KErrNone)
		    {
            buf.Copy(KStartString);
            buf.AppendFormat(_L("Subtest does not follow rules set by TEvent test framework, leave code: %d"), err);
            iTest->TEST(EFalse);
            iTest->LOG_MESSAGE(buf);
            User::Leave(err);
		    }
	    }
	
	}

void CTQueueTestEvent::AddExpectedEvent(TWsEvent &aEvent)
	{
	if(iCheckTimeStamp)
		{
		static_cast<TWsQTEvent&>(aEvent).Time().UniversalTime();
		}
	iEventBuffer.Add(&aEvent);
	}

TInt CTQueueTestEvent::EventsLeft()
	{
	return iEventBuffer.Count();
	}
//

CTQueueWindowGroup::CTQueueWindowGroup(CTClient *aClient) : CTWindowGroup(aClient)
	{}

void CTQueueWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	User::LeaveIfError(iGroupWin.EnableOnEvents(EEventControlAlways));
	User::LeaveIfError(iGroupWin.EnableGroupChangeEvents());
	}

//

/**
 Closes and deletes anim DLL.
 @param aAnimDll Pointer to the anim Dll to be closed and deleted
 */
LOCAL_C void DestroyAnimDll(TAny* aAnimDll)
	{
	(static_cast<RAnimDll*>(aAnimDll))->Destroy();
	}

/**
 Destroys an REventAnimBase derived anim
 @param aAnim REventAnimBase derived client-side handle to anim
 */
LOCAL_C void DestroyAnim(TAny* aAnim)
	{
	(static_cast<REventAnimBase*>(aAnim))->Destroy();
	}

/**
 Loads the DLL containing the test anims and pushes it onto the cleanup stack.
 This function leaves if the operation was unsuccessful.
 @return A pointer to the DLL containing test anims
 */
RAnimDll* REventAnimBase::LoadL()
	{
	RAnimDll* animDll=new(ELeave) RAnimDll(TheClient->iWs);
	CleanupStack::PushL(TCleanupItem(DestroyAnimDll,animDll));
	User::LeaveIfError(animDll->Load(KAnimDLLName));
	CleanupStack::Pop(animDll);
	return animDll;
	}

/**
 Closes the client-side handle to the anim, destroys the DLL and deletes itself.
 */
void REventAnimBase::Destroy()
	{
	Close();
	delete this;
	}

/**
 Resets server-side state of anim
 */
void REventAnimBase::Reset()
	{
	CommandReply(EADllReset);
	}

/**
 @return Under non-error conditions, returns 0.
 Under error conditions, returns the number of events received
 since the first unexpected event was sent.
 */
TInt REventAnimBase::ErrorCode()
	{
	return CommandReply(EADllErrorCode);
	}

/**
 @return The number of events received by the anim since it was last reset.
 */
TInt REventAnimBase::NumberOfEventsReceived()
	{
	return CommandReply(EADllNumberOfEventsReceived);
	}

/**
 Adds an expected event to the list of events expected by the anim.
 @param aEat Indicates if event is expected to be eaten (ETrue 'indicates expected to be eaten')
 @param aEvent The expected event
 @return KErrNone if succcessful, othewise error code.
 */
TInt REventAnimBase::AddExpectedEvent(TBool aEat, const TRawEvent& aEvent)
	{
	TAnimRawEvent event;
	event.iRawEvent=aEvent;
	event.iEat=aEat;
	return AddExpectedEvent(event);
	}

/**
 Adds an expected event to the list of events expected by the anim.
 @param aEvent The expected event
 @return KErrNone if succcessful, othewise error code.
 */
TInt REventAnimBase::AddExpectedEvent(const TAnimRawEvent& aEvent)
	{
	TPckgBuf<TAnimRawEvent> param;
	param()=aEvent;
	return CommandReply(EADllAddExpectedEvent,param);
	}

//

/**
 Loads the DLL containing the event test anim and creates an anim for event tests
 @param aWin the base window to which the anim is associated (this anim is a Window anim)
 @return A pointer to the new REventAnim
 */
REventAnim* REventAnim::NewL(RWindowBase* aWin, RAnimDll* aDll, REventAnimBase::EAnimationType aType/*=EEventAnim*/)
	{
	REventAnim* self=new(ELeave) REventAnim(aDll);
	CleanupStack::PushL(TCleanupItem(DestroyAnim,self));
	User::LeaveIfError(self->Construct(*aWin,(aType == EEventAnim ? EAnimTypeEventTest : EAnimTypeEventPostingTest), TPtrC8()));
	CleanupStack::Pop(self);
	return self;
	}

/**
 Checks if the anim has received the correct events, and resets the sever-side state of the anim
 @return positive value if events did not match expected showing the first event that didn't match.
         negative value if there were the wrong number of events showing the number of events there were
         0 if correct number of events and they all match
 */
TInt REventAnim::TestFail(TInt aNoEvents)
	{
	TInt events=CommandReply(EADllErrorCode);
	if (events>0)
		return events;
	events=CommandReply(EADllNoEventsAndReset);
	if (events==aNoEvents)
		return 0;
	return -events;
	}

//
/**
 Loads the DLL containing the event test anim and creates an anim for event handler removal tests
 @param aWin the base window to which the anim is associated (this anim is a Window anim)
 @return A pointer to the new REventAnim
 */
RRemovableAnim* RRemovableAnim::NewLC(RWindowBase* aWin, RAnimDll* aDll)
	{
	RRemovableAnim* self=new(ELeave) RRemovableAnim(aDll);
	CleanupStack::PushL(TCleanupItem(DestroyAnim,self));
	User::LeaveIfError(self->Construct(*aWin,EAnimTypeRemovableAnim,TPtrC8()));
	return self;
	}

/**
 Sets the lifetime of the anim. This anim will remove itself from the
 event handler list after receiving this number of events.
 @return The lifetime of the anim
 */
TInt RRemovableAnim::SetEventHandlerLifetime(TInt aEventCount)
	{
	TPckgBuf<TInt> param;
	param()=aEventCount;
	return CommandReply(EADllSetEventHandlerLifetime, param);
	}

/**
 Retrieves the lifetime of the anim. This anim will remove itself from the
 event handler list after receiving this number of events.
 @return The lifetime of the anim
 */
TInt RRemovableAnim::EventHandlerLifetime()
	{
	return CommandReply(EADllEventHandlerLifetime);
	}
/**
 Adds the anim to the list of event handlers.
 This anim should receive any subsequent events.
 @return KErrNone on successful addition.
 */
TInt RRemovableAnim::AddToEventHandlers()
	{
	return CommandReply(EADllAddToEventHandlers);
	}

/**
 Removes the anim from the list of event handlers.
 This anim should not receive any subsequent events.
 @return KErrNone on successful removal.
 */
TInt RRemovableAnim::RemoveFromEventHandlers()
	{
	return CommandReply(EADllRemoveFromEventHandlers);
	}

/**
 Tests if the anim has received the correct number of events, and that
 the events received are all the correct events.
 @param aNoEvents The total expected number of events since it was last reset/constructed.
 @return ETrue if the anim has received the expected events.
 */
TBool RRemovableAnim::TestPassed()
	{
	return CommandReply(EADllTestPassed);
	}

//

CTEventTest::CTEventTest(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("Testing Event functions"));
	}

CTEventTest::~CTEventTest()
	{
	delete iBlankWin;
	iBackingWindow.Close();
	delete iVisWins1;
	delete iVisWins2;
	delete iVisWins3;
	delete iTransWin;
	delete iQueueClient;
	if (iAnim)
		iAnim->Destroy();
	delete iEventTestGroup2;		//Just in case it doesn't get deleted normally
	delete iEventTestGroup3;		//Just in case it doesn't get deleted normally
	delete iEventTestGroup4;        //Just in case it doesn't get deleted normally
	if (iPtrPluginLoaded)
	    {
	    iClick.Unload();
	    }
	iClick.Close();
	if (iTurnBackOnCapsLock)
		{
		TheClient->iWs.SetModifierState(EModifierCapsLock,ETurnOnModifier);
		}
	if (iRemovableAnims)
		{
		EventHandlerRemoval_DestroyAllAnims();
		iRemovableAnims->Close();
		delete iRemovableAnims;
		}
	if (iAnimDll)
		{
		iAnimDll->Destroy();
		}
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS	
#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA  	
	if (!iSurfaceId.IsNull())
	    {
	    iSurfaceManager.CloseSurface(iSurfaceId);
	    }
	iSurfaceManager.Close();
	if(iTimeOutCallback)
	    {
	    iTimeOutCallback->Cancel();
	    delete iTimeOutCallback;
	    }
#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
#endif // SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
	}

void CTEventTest::ConstructL()
	{
    _LIT(KWsIniFile, "z:\\system\\data\\wsini.ini");
    CIniData* iniData = CIniData::NewL(KWsIniFile, ' ');
    _LIT( KWsiniYShifting, "YSHIFTING");
    iniData->FindVar(KWsiniYShifting, iYOffset);
    delete iniData;
	TheClient->iScreen->SetAppScreenMode(0);
	TheClient->iScreen->SetScreenMode(0);						// in mode 0 TheClient->iScreen->SizeInPixels() 
	iPhysicalScreenSize = TheClient->iScreen->SizeInPixels();	// is equal to physical screen size
	BaseWin->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());
	TheGc->SetBrushColor(TRgb::Gray16(12));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(BaseWin->Win()->Size()));
	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
	TheGc->Activate(*TestWin->Win());
	TheGc->SetBrushColor(TRgb::Gray16(4));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(TestWin->Win()->Size()));
	TheGc->Deactivate();
	iBackingWindow=RBlankWindow(TheClient->iWs);
	iBackingWindow.Construct(*TheClient->iGroup->WinTreeNode(),ENullWsHandle);
	iBackingWindow.SetColor(TRgb(0,0xDD,0xDD));
	iBackingWindow.SetOrdinalPosition(-1,-25);
	iBackingWindow.Activate();
	iBlankWin=new(ELeave) CTBlankWindow();
	iBlankWin->ConstructL(*TheClient->iGroup);
	User::LeaveIfError(iBlankWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iBlankWin->SetExt(TPoint(),iPhysicalScreenSize);
	iBlankWin->SetVisible(EFalse);
	iBlankWin->Activate();
	CArrayFixFlat<TInt> *rotations=new(ELeave) CArrayFixFlat<TInt>(1);
	iNumScrModes=TheClient->iScreen->NumScreenModes();
	TInt modes=Min(2,iNumScrModes);
	TInt mode;
	TInt rot;
	CleanupStack::PushL(rotations);
	for (mode=0;mode<modes;++mode)
		{
		User::LeaveIfError(TheClient->iScreen->GetRotationsList(mode,rotations));
		for (rot=rotations->Count()-1;rot>=0;--rot)
			{
			iScreenModeTests|=(1<<(*rotations)[rot]);
			}
		}
	if (CheckScalingSupportedOrNot())
		iScreenModeTests|=EDoScale;

	iClick=RSoundPlugIn(TheClient->iWs);
	User::LeaveIfError(iClick.Construct(TUid::Uid(CLICK_THIRD_UID)));
    TBool ptrPluginLoadable = EFalse;
    iClick.IsLoaded(ptrPluginLoadable);
    if (ptrPluginLoadable)
        {
        User::LeaveIfError(iClick.Load(KKeyClickPluginDll));
        iPtrPluginLoaded = ETrue;
        }
	CleanupStack::PopAndDestroy();
	if (TheClient->iWs.GetModifierState()&EModifierCapsLock)
		{
		iTurnBackOnCapsLock=ETrue;
		TheClient->iWs.SetModifierState(EModifierCapsLock,ETurnOffModifier);
		}
	iAnimDll = REventAnimBase::LoadL();
	TInt mods=TheClient->iWs.GetModifierState();
	if (mods&EModifierCapsLock)
		iTest->SimulateKeyDownUp(EStdKeyCapsLock);
	if (mods&EModifierNumLock)
		iTest->SimulateKeyDownUp(EStdKeyNumLock);
	//Make sure all the keys we test are in the up state
	iTest->SimulateKeyDownUp(EStdKeyLeftShift);
	iTest->SimulateKeyDownUp(EStdKeyRightShift);
	iTest->SimulateKeyDownUp(EStdKeyLeftFunc);
	iTest->SimulateKeyDownUp(EStdKeyLeftCtrl);
	iTest->SimulateKeyDownUp(EStdKeyRightCtrl);
	mods=TheClient->iWs.GetModifierState();
	_LIT(KLog,"Initial Modifiers state 0x%x (ideally should be zero)");
	LOG_MESSAGE2(KLog,mods);
	iTest->DelayIfFullRomL();

#if !defined(TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA)
	// 2d or 3d pointer?
	if(HAL::Get(HALData::EPointer3D,iIs3DPointer)!=KErrNone)
		{
		iIs3DPointer=EFalse;
		}
#endif
	TheClient->iWs.GetKeyboardRepeatRate(iKeyBoardRepeatInitialDelay, iKeyBoardRepeatNextDelay);
	}

void CTEventTest::Failed()
	{
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS	
#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA  	
   if(iTimeOutCallback)
        {
        iTimeOutCallback->Cancel();
        iTimeOutCallback->Deque();
        delete iTimeOutCallback;
        iTimeOutCallback = NULL;
        }
    TEST(EFalse);
#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
#endif // SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
	CActiveScheduler::Stop();
	iFailed=ETrue;
	}

void CTEventTest::CheckTimeInterval(const TTimeIntervalMicroSeconds32 &aTime,TInt aCheckTime)
	{
	TEST(aTime.Int()==aCheckTime);
	if (aTime.Int()!=aCheckTime)
		INFO_PRINTF3(_L("aTime.Int()==aCheckTime - Expected: %d, Actual: %d"), aCheckTime, aTime.Int());
	}

void CTEventTest::KeyRepeatRate(TInt aInitialRepeat, TInt aRepeat)
	{
	TheClient->iWs.SetKeyboardRepeatRate(TTimeIntervalMicroSeconds32(aInitialRepeat), TTimeIntervalMicroSeconds32(aRepeat));
	TTimeIntervalMicroSeconds32 initialTime;
	TTimeIntervalMicroSeconds32 time;
	TheClient->iWs.GetKeyboardRepeatRate(initialTime,time);
	CheckTimeInterval(initialTime,aInitialRepeat);
	CheckTimeInterval(time,aRepeat);
	}

void CTEventTest::DoubleClickSettings(TInt aInterval, TInt aDistance)
	{
	TheClient->iWs.SetDoubleClick(TTimeIntervalMicroSeconds32(aInterval), aDistance);
	TTimeIntervalMicroSeconds32 interval;
	TInt distance;
	TheClient->iWs.GetDoubleClickSettings(interval, distance);
	CheckTimeInterval(interval,aInterval);
	TEST(aDistance==distance);
	if (aDistance!=distance)
		INFO_PRINTF3(_L("aDistance==distance - Expected: %d, Actual: %d"), distance, aDistance);
	}

TInt EventCancelFunc(TAny* aClient)
	{
	TWsQTEvent event;
	event.SetType(EEventModifiersChanged);
	((TModifiersChangedEvent *)event.EventData())->iChangedModifiers=0;
	((TModifiersChangedEvent *)event.EventData())->iModifiers=0;
	TheClient->iWs.SendEventToWindowGroup(TheClient->iGroup->GroupWin()->Identifier(),event);
//
	TRequestStatus status;
	((CTClient *)aClient)->iWs.EventReady(&status);
	((CTClient *)aClient)->iWs.EventReadyCancel();
	User::WaitForRequest(status);
	return(KErrNone);
	}

void CTEventTest::General()
	{
	TWsQTEvent event;
	event.SetType(EEventModifiersChanged);
	((TModifiersChangedEvent *)event.EventData())->iChangedModifiers=0;
	((TModifiersChangedEvent *)event.EventData())->iModifiers=0;
	TheClient->iWs.SendEventToWindowGroup(TheClient->iGroup->GroupWin()->Identifier(),event);
	TCallBack callBack(EventCancelFunc,TheClient);
	TheClient->SetCancelFunction(callBack);
//
	TTimeIntervalMicroSeconds32 oldInitialTime;
	TTimeIntervalMicroSeconds32 oldTime;
	TheClient->iWs.GetKeyboardRepeatRate(oldInitialTime,oldTime);
//
	KeyRepeatRate(EKeyboardInitialRepeat1, EKeyboardRepeat1);
	KeyRepeatRate(EKeyboardInitialRepeat2, EKeyboardRepeat2);
	KeyRepeatRate(EKeyboardInitialRepeat3+200, EKeyboardRepeat3); // Max initial repeat with normal keyboard repeat value
	KeyRepeatRate(EKeyboardInitialRepeat3, EKeyboardRepeat7+200); // Max key and initial repeat
	KeyRepeatRateNegTest(EKeyboardInitialRepeat4, EKeyboardRepeat4); // -ve initial repeat
	KeyRepeatRateNegTest(EKeyboardInitialRepeat2, EKeyboardRepeat6); // -ve keyboard repeat
	#if defined __WINS__ || defined __WINSCW__
	KeyRepeatRateNegTest(EKeyboardInitialRepeat3+400, EKeyboardRepeat5); // More than max initial repeat
	#endif//defined __WINS__ || defined __WINSCW__
	DoubleClickSettings(EDoubleClickTime1,EDoubleClickGap1);
	DoubleClickSettings(EDoubleClickTime2,EDoubleClickGap2);
	TheClient->iWs.SetKeyboardRepeatRate(oldInitialTime,oldTime);
//
	}

#define UNIQUE_PRIORITY 123
void CTEventTest::General2()
	{
	RWindowGroup& group=*TheClient->iGroup->GroupWin();
	TInt priority=group.OrdinalPriority();
	TInt position=group.OrdinalPosition();
	TInt priorityGain=TheClient->iWs.GetWindowGroupOrdinalPriority(group.Identifier())-priority;
	group.SetOrdinalPosition(0,UNIQUE_PRIORITY);
	TWsQTEvent event;
	event.SetType(EEventModifiersChanged);
	((TModifiersChangedEvent *)event.EventData())->iChangedModifiers=0;
	((TModifiersChangedEvent *)event.EventData())->iModifiers=0;
	TheClient->iWs.SendEventToAllWindowGroups(UNIQUE_PRIORITY+priorityGain,event);
	TCallBack callBack(EventCancelFunc,TheClient);
	TheClient->SetCancelFunction(callBack);
	group.SetOrdinalPosition(position,priority);
	}

void CTEventTest::AddExpectedKey(TInt aEventCode, TInt aScanCode, TInt aCode/*=0*/, TInt aRepeats/*=0*/, TUint aModifiers/*=KModifiersIgnore*/)
	{
	TPckgBuf<TWsEvent> evtPkg;
	TWsEvent& event=evtPkg();
	if (aEventCode==EEventKey && aCode==0)
		aCode=aScanCode;
	event.SetType(aEventCode);
	event.SetHandle((TInt)iQueueClient->iGroup);
	event.Key()->iCode=aCode;
	event.Key()->iScanCode=aScanCode;
	event.Key()->iModifiers=aModifiers;
	event.Key()->iRepeats=aRepeats;
	iQueueClient->AddExpectedEvent(event);
	if (iAddToClick)
		{
		if (aModifiers == KModifiersIgnore)
			{
			// Key click tests assume zero modifiers in click event by default
			event.Key()->iModifiers = 0;
			}
		iClick.CommandReply(EClickEventAdd,evtPkg);
		}
	}

void CTEventTest::AddExpectedKeyDownUp(TInt aScanCode, TInt aCode/*=0*/, TInt aRepeats/*=0*/, TUint aModifiers/*=KModifiersIgnore*/)
	{
	__ASSERT_DEBUG(aScanCode<'a' || aScanCode>'z',AutoPanic(EAutoPanicScanCapital));
	// EModiferAutorepeatable can be present in EEventKey but is never valid
	// in EEventKeyDown and EEventKeyUp. 
	TUint modifiersDownUp = (aModifiers == KModifiersIgnore) ?
		aModifiers : aModifiers & ~EModifierAutorepeatable;

	AddExpectedKey(EEventKeyDown,aScanCode,0,aRepeats,modifiersDownUp);
	AddExpectedKey(EEventKey,aScanCode,aCode,aRepeats,aModifiers);
	AddExpectedKey(EEventKeyUp,aScanCode,0,aRepeats,modifiersDownUp);
	}

void CTEventTest::AddExpectedEvent(TInt aEventCode, CTWinBase* aWin)
	{
	TWsQTEvent event;
	event.iType=aEventCode;
	event.iHandle=REINTERPRET_CAST(TInt,aWin);
	iQueueClient->AddExpectedEvent(event);
	}

void CTEventTest::AddExpectedEvent(TInt aEventCode, RWindowGroup* aWindow)
	{
	TWsQTEvent event;
	event.iType=aEventCode;
	event.iHandle=(TUint32)aWindow;
	iQueueClient->AddExpectedEvent(event);
	}

void CTEventTest::AddExpectedEvent(TInt aEventCode)
	{
	AddExpectedEvent(aEventCode,iQueueClient->iGroup);
	}

void CTEventTest::AddExpectedErrorMessage(TWsErrorMessage::TErrorCategory aCategory, TInt aError)
	{
	TWsQTEvent event;
	event.iType=EEventErrorMessage;
	event.iHandle=(TInt)iQueueClient->iGroup;
	event.ErrorMessage()->iErrorCategory=aCategory;
	event.ErrorMessage()->iError=aError;
	iQueueClient->AddExpectedEvent(event);
	}

void CTEventTest::CalculatePointerEvent(TWsEvent& aEvent,TPointerEvent::TType aType, TPoint aPos)
	{
#if !defined(TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA)
	TAdvancedPointerEvent& ptrEvent=*aEvent.Pointer();
	aEvent.SetType(EEventPointer);
	ptrEvent.iParentPosition=aPos;
	// SetHandle must be done after iParentPosition is set, but before iPosition is set.
	aEvent.SetHandle(reinterpret_cast<TInt>(iQueueClient->GetHandle(aPos)));	
	const TPoint3D point3D(aPos.iX, aPos.iY, 0);
	const TInt pointerEventModifiers=0;
	const TUint8 pointerNumber=0;
	aEvent.InitAdvancedPointerEvent(aType,pointerEventModifiers,point3D,pointerNumber);
#else	
	TPointerEvent& ptrEvent=*aEvent.Pointer();
	aEvent.SetType(EEventPointer);
	ptrEvent.iParentPosition=aPos;		//Must be done before next line
	aEvent.SetHandle(REINTERPRET_CAST(TInt,iQueueClient->GetHandle(aPos)));
	ptrEvent.iType=aType;
	ptrEvent.iModifiers=0;
	ptrEvent.iPosition=aPos;
#endif
	}

void CTEventTest::CalculatePointerCaptureEvent(TWsEvent& aEvent, TPointerEvent::TType aType, TPoint aPos, TInt aHandle)
	{
	TPointerEvent& ptrEvent=*aEvent.Pointer();
	aEvent.SetType(EEventPointer);
	ptrEvent.iParentPosition=aPos;		//Must be done before next line
	if(iVisWins1 && iVisWins2)
		{
		if (TRect(iQueueClient->ChildTopLeft(),iQueueClient->ChildSize()).Contains(aPos) &&
			!TRect(iQueueClient->ChildTopLeft()+TPoint(15,15),iQueueClient->ChildSize()).Contains(aPos))
			{
			aPos-=iQueueClient->ChildTopLeft();	//Pointer is on the first window
			aEvent.SetHandle((TInt)iVisWins1);
			}
		else if (TRect(iQueueClient->ChildTopLeft()+TPoint(15,15),iQueueClient->ChildSize()).Contains(aPos))
			{
			aPos-=(iQueueClient->ChildTopLeft()+TPoint(15,15));	//Pointer is on the second window
			aEvent.SetHandle((TInt)iVisWins2);
			}
		else if (TRect(TPoint(EWinPositionX,EWinPositionY),TSize(EWinWidth,EWinHeight)).Contains(aPos))
			{
			aPos-=TPoint(EWinPositionX,EWinPositionY);	//Pointer is on the parent window
			aEvent.SetHandle((TInt)iQueueClient->iWin);
			}
		else
			{
			aEvent.SetHandle((TInt)0);		//Pointer is not in the same window group
			}
		}
	ptrEvent.iType=aType;
	ptrEvent.iModifiers=0;
	if(aHandle!=0 && aHandle!=aEvent.Handle())
		{
		aEvent.SetHandle(aHandle);
		ptrEvent.iPosition=ptrEvent.iParentPosition-(iQueueClient->ChildTopLeft()+TPoint(15,15));
		}
	else
		{
		ptrEvent.iPosition=aPos;
		}
	}

void CTEventTest::AddExpectedPointerCapture(TPointerEvent::TType aType, TPoint aPos, TInt aHandle)
	{
	TPckgBuf<TWsQTEvent> evtPkg;
	CalculatePointerCaptureEvent(evtPkg(),aType,aPos,aHandle);
	iQueueClient->AddExpectedEvent(evtPkg());
	}

void CTEventTest::AddExpectedPointer(TPointerEvent::TType aType, TPoint aPos)
	{
	TPckgBuf<TWsQTEvent> evtPkg;
	CalculatePointerEvent(evtPkg(),aType,aPos);
	iQueueClient->AddExpectedEvent(evtPkg());
	if (iAddToClick)
		{
		evtPkg().Pointer()->iParentPosition+=TPoint(EWinPositionX,EWinPositionY);
		iClick.CommandReply(EClickEventAdd,evtPkg);
		}
	}

void CTEventTest::AddExpectedPointerClick(TPointerEvent::TType aType, TPoint aPos)
	{
	TPckgBuf<TWsQTEvent> evtPkg;
	CalculatePointerEvent(evtPkg(),aType,aPos);
	evtPkg().Pointer()->iParentPosition+=TPoint(EWinPositionX,EWinPositionY);
	iClick.CommandReply(EClickEventAdd,evtPkg);
	}

void CTEventTest::AddExpectedPointerDownUp(TPoint aPos)
	{
	AddExpectedPointer(TPointerEvent::EButton1Down,aPos);
	AddExpectedPointer(TPointerEvent::EButton1Up,aPos);
	}

void CTEventTest::AddExpectedPointerScr(TPointerEvent::TType aType, TPoint aPos)
	{
	AddExpectedPointer(aType,TPoint(aPos.iX-EWinPositionX,aPos.iY-EWinPositionY));
	}

void CTEventTest::AddExpectedPointerDownUpScr(TPoint aPos)
	{
	AddExpectedPointerDownUp(TPoint(aPos.iX-EWinPositionX,aPos.iY-EWinPositionY));
	}

void CTEventTest::AddExpectedModifiersChanged(TInt aChangedModifiers, TInt aModifiers)
	{
	TWsQTEvent event;
	event.iType=EEventModifiersChanged;
	event.iHandle=(TInt)iQueueClient->iGroup;
	event.ModifiersChanged()->iChangedModifiers=aChangedModifiers;
	event.ModifiersChanged()->iModifiers=aModifiers;
	iQueueClient->AddExpectedEvent(event);
	}

void CTEventTest::AddExpectedEnterExit(TInt aEventCode, TPoint aPos)
	{
	AddExpectedEvent(aEventCode,iQueueClient->GetHandle(aPos));
	}

void CTEventTest::AddExpectedKeyAnim(TRawEvent::TType aEventCode, TInt aScanCode, TBool aEat)
	{
	TAnimRawEvent event;
	event.iRawEvent.Set(aEventCode,aScanCode);
	event.iEat=aEat;
	TInt retVal = iAnim->AddEvent(event);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("iAnim->AddEvent(event) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	if (!aEat)
		AddExpectedKey((aEventCode==TRawEvent::EKeyDown ? EEventKeyDown:EEventKeyUp),aScanCode);
	}

void CTEventTest::AddExpectedPointerAnim(TRawEvent::TType aEventCode, TPoint aPoint, TBool aEat/*=ETrue*/)
	{
	TAnimRawEvent event;
	event.iRawEvent.Set(aEventCode,aPoint.iX,aPoint.iY);
	event.iEat=aEat;

	TInt retVal = iAnim->AddEvent(event);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("iAnim->AddEvent(event) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	if (!aEat)
		AddExpectedPointer((aEventCode==TRawEvent::EButton1Down ? TPointerEvent::EButton1Down:TPointerEvent::EButton1Up)
																			,aPoint-TPoint(EWinPositionX,EWinPositionY));
	}

void CTEventTest::AddExpectedVisibilityChange(TUint aFlags)
	{
	TWsQTEvent event;
	event.iType=EEventWindowVisibilityChanged;
	event.iHandle=(TInt)iVisWins1;
	event.VisibilityChanged()->iFlags=aFlags;
	iQueueClient->AddExpectedEvent(event);
	}

void CTEventTest::AddExpectedVisibilityChange(TUint aFlags, CTBlankWindow* aWindow)
    {
    TWsQTEvent event;
    event.iType=EEventWindowVisibilityChanged;
    event.iHandle=(TInt)aWindow;
    event.VisibilityChanged()->iFlags=aFlags;
    iQueueClient->AddExpectedEvent(event);
    }

void CTEventTest::SimulatePointerWin(TRawEvent::TType aType, TInt aX, TInt aY)
	{
	SimulatePointer(aType,EWinPositionX+aX,EWinPositionY+aY+iYOffset);
	}

void CTEventTest::SimulatePointerDownUpWin(TInt aX, TInt aY)
	{
	iTest->SimulatePointerDownUp(EWinPositionX+aX,EWinPositionY+aY+iYOffset);
	}

void CTEventTest::SimulatePointer(TRawEvent::TType aType, TInt aX, TInt aY)
    {
    iTest->SimulatePointer(aType,aX,aY+iYOffset);
    }

#if !defined(TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA)
void CTEventTest::SimulateButton1DownWithSimulatedMove(TInt aX, TInt aY)
	{
	if(iIs3DPointer)
		{
		// With advanced pointer events we can have an XYInputType of Pointer coupled with 3DPointer
		// which now makes MovesAvailable return ETrue rahter than EFalse, which in turn stops the simulated
		// Emove from being generated.
		// So...
		// If MovesAvailable() returns ETrue, then this move will be used, and the WServ simulated move wont be generated
		// If MovesAvailable() returns EFalse, then this move will be filtered out, and the WServ simulated move will be generated		
		SimulatePointer(TRawEvent::EPointerMove,aX,aY);
		}
	// then the original button down event
	SimulatePointer(TRawEvent::EButton1Down,aX,aY);	
	}
#endif

void CTEventTest::NextSetOfEventsL()
	{
	iQueueClient->ResetCount(iEventSet);
	switch (iTest->iState)
		{
	case 1:
		QueuePurge_NextSetOfEventsL();
		break;
	case 2:
		SendEventTest_NextSetOfEvents();
		break;
	case 4:
		InvisibleWindow_NextSetOfEvents();
		break;
	case 5:
		JavaAdditions_NextSetOfEvents();
		break;
	case 6:
		WinCap_NextSetOfEvents();
		break;
	case 7:
		XyInputType_NextSetOfEvents();
		break;
	case 8:
		MovingPointerCursor_NextSetOfEventsL();
		break;
	case 9:
		RotatedMode_NextSetOfEventsL();
		break;
	case 10:
		AnimEvent_NextSetOfEventsL();
		break;
	case 11:
		FocusChanged_NextSetOfEventsL();
		break;
	case 12:
		StopEvents_NextSetOfEventsL();
		break;
	case 13:
		VirtualKeyboard_NextSetOfEventsL();
		break;
	case 14:
		KeyClicks_NextSetOfEvents();
		break;
	case 15:
		CaptureLong_NextSetOfEventsL();
		break;
	case 16:
		Password_NextSetOfEvents();
		break;
	case 17:
		GroupListChanged_NextSetOfEventsL();
		break;
	case 18:
		RepeatableKeysL();
		break;
	case 19:
		ScreenScaling_NextSetOfEventsL();
		break;
	case 20:
		VisibilityChanged_NextSetOfEventsL();
		break;
	case 21:
		CheckTimeStamp_NextSetOfEventsL();
		break;
	case 22:
		PointerCapture_NextSetOfEventsL();
		break;
	case 23:
		EventHandlerRemoval_NextSetOfEventsL();
		break;	
	// Note that case 24 (which is KTest24) does not use NextSetOfEventsL()	
	case 25:
		MismatchedCapture_NextSetOfEventsL();
		break;
	case 26:
		PointerBufferPurge_NextSetOfEventsL();		
		break;	
	case 27:
	    RawEventRepeatTest_NextSetOfEventsL();
	    break;

#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA 	    
    case 28:
        KeyEventTranslation_NextSetOfEventsL();
        break;
    case 29:
        KeyEventBlocking_NextSetOfEventsL();
        break;
    case 30:
        KeyEventAppRestriction_NextSetOfEventsL();
        break;
    case 31:
        KeyEventAppPriority_NextSetOfEventsL();
        break;
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS	    
    case 32:
        SurfaceVisibilityChanged1_NextSetOfEventsL();
        break;
    case 33:
        SurfaceVisibilityChanged2_NextSetOfEventsL();
        break;
    case 34:
        SurfaceVisibilityChanged3_NextSetOfEventsL();
        break;
#endif // SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
        
	default:
		AutoPanic(EAutoPanicWrongTest);
		}
	}

void CTEventTest::TriggerOomMessage(TTimeIntervalMinutes aPeriod)
	{
	TTime time;
	if (aPeriod.Int()!=0) // adjust time to affect Oom message triggering
		{
		time.HomeTime();
		User::SetHomeTime(time+aPeriod);
		}
	RBlankWindow win(TheClient->iWs);
	win.Construct(*(TheClient->iGroup->WinTreeNode()),1);
	win.SetColor(TRgb::Gray4(1));
	win.SetExtent(TPoint(10,10),TheClient->iGroup->Size()-TSize(20,20));
	TheClient->iWs.HeapSetFail(RHeap::EDeterministic,1);
	win.Activate();
	TheClient->iWs.Flush();
	TheClient->iWs.HeapSetFail(RHeap::ENone,0);
	if (aPeriod.Int()!=0)
		{
		time.HomeTime();
		User::SetHomeTime(time-aPeriod);
		}
	win.Close();
	}

TBool CTEventTest::SetClickType(TClickOutputModes aType)
	{
	TPckgBuf<TInt> intPkg;
	intPkg()=aType;
	TInt err=iClick.CommandReply(EClickCommandSetOutput,intPkg);
	if (err!=KErrNone)
		Failed();
	return err;
	}

void CTEventTest::CreateEventTestGroup()
	{
	iEventTestGroup=RWindowGroup(TheClient->iWs);
	iEventTestGroup.Construct(123, EFalse);
	iEventTestGroup.SetName(_L("Fred"));
	}

void CTQueueClient::CreateEventTestGroupOnSecondScreenL()
	{
	const TInt KSndScreenNo=1;
	iSndScreen=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iSndScreen->Construct(KSndScreenNo));
	iWinGp1 = new(ELeave) RWindowGroup(iWs);
	User::LeaveIfError(iWinGp1->Construct((TUint32)iWinGp1,ETrue,iSndScreen));
	iWinGp2 = new (ELeave) RWindowGroup(iWs);
	User::LeaveIfError(iWinGp2->Construct((TUint32)iWinGp2,ETrue,iSndScreen));
	}

void CTQueueClient::CloseEventTestGroupOnSecondScreen()
	{
	if(iWinGp1)
		{
		iWinGp1->Close();
		delete iWinGp1;
		iWinGp1=NULL;
		}
	if(iWinGp2)
		{
		iWinGp2->Close();
		delete iWinGp2;
		iWinGp2=NULL;
		}
	if(iSndScreen)
		{
		delete iSndScreen;
		iSndScreen = NULL;
		}
	}

void CTEventTest::CloseEventTestGroup()
	{
	iEventTestGroup.Close();
	}

void CTEventTest::CreateAndCloseEventTestGroup()
//
// Create and close a window group to test sending of group changed messages
//
	{
	CreateEventTestGroup();
	CloseEventTestGroup();
	}

void CTEventTest::QueuePurge_NextSetOfEventsL()
	{
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KSet,"QueuePurge SetOfEvents: %d of 11");
	logMessageText.Format(KSet,iEventSet);
	TheClient->LogMessage(logMessageText);
#endif
	iTheClientFlush=TheClient->iWs.SetAutoFlush(ETrue);
	iQuequeClientFlush=iQueueClient->iWs.SetAutoFlush(ETrue);
	switch(iEventSet++)
		{
		case 0:
			iTest->SimulateKey(TRawEvent::EKeyDown,32);
			iTest->SimulateKey(TRawEvent::EKeyUp,32);
			CreateAndCloseEventTestGroup();
			SimulatePointer(TRawEvent::EButton1Down,10,20);
			SimulatePointer(TRawEvent::EButton1Up,20,30);
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			CreateAndCloseEventTestGroup();
		//
			AddExpectedEvent(EEventFocusGained);
			AddExpectedKey(EEventKeyDown,32);
			AddExpectedKey(EEventKeyUp,32);
			AddExpectedEvent(EEventWindowGroupsChanged);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(10-EWinPositionX,20-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(20-EWinPositionX,30-EWinPositionY));
			AddExpectedEvent(EEventSwitchOn);
			break;
		case 1:
			{
			for(TInt count=0;count<100;count++)
				{
				iTest->SimulateKey(TRawEvent::EKeyDown,'A'+(count%26));
				iTest->SimulateKey(TRawEvent::EKeyUp,'A'+(count%26));
				}
			for(TInt count2=0;count2<EMaxEventQueueSize/3;count2++)
				{
				AddExpectedKey(EEventKeyDown,'A'+(count2%26));
				AddExpectedKey(EEventKey,'A'+(count2%26),'a'+(count2%26));
				AddExpectedKey(EEventKeyUp,'A'+(count2%26));
				}
			AddExpectedKey(EEventKeyDown,'A'+(99%26));
			AddExpectedKey(EEventKeyUp,'A'+(99%26));
			}
			break;
		case 2:
			{
			User::LeaveIfError(iQueueClient->iGroup->GroupWin()->EnableModifierChangedEvents(0xFFFFFFFF, EEventControlOnlyWhenVisible));
			for(TInt count=0;count<20;count++)
				{
				iTest->SimulateEvent(TRawEvent::ESwitchOn);
				iTest->SimulateEvent(TRawEvent::ESwitchOn);
				iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftShift);
				iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyRightShift);
				iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyRightShift);
				iTest->SimulateEvent(TRawEvent::ESwitchOn);
				iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftShift);
				}
			for(TInt count2=0;count2<60;count2++)
				iTest->SimulateKey(TRawEvent::EKeyDown,'A');
			AddExpectedEvent(EEventSwitchOn);
			AddExpectedModifiersChanged(EModifierLeftShift|EModifierRightShift|EModifierShift,0);
			for(TInt count3=0;count3<EMaxEventQueueSize-2;count3++)
				AddExpectedKey(EEventKeyDown,'A');
			iQueueClient->iGroup->GroupWin()->DisableModifierChangedEvents();
			}
			break;
		case 3:
			{
			SimulatePointer(TRawEvent::EButton1Up,10,20);
			SimulatePointer(TRawEvent::EButton2Up,11,22);
			SimulatePointer(TRawEvent::EButton1Down,12,23);
			SimulatePointer(TRawEvent::EPointerMove,13,24);
			SimulatePointer(TRawEvent::EButton3Down,15,25);
			SimulatePointer(TRawEvent::EButton1Up,14,24);
			for(TInt count2=0;count2<60;count2++)
				iTest->SimulateKey(TRawEvent::EKeyDown,'A');
			SimulatePointer(TRawEvent::EButton3Up,15,25); // Should get stripped by unmatched purged down handling code
			iTest->SimulateKey(TRawEvent::EKeyUp,'A');	// To stop repeats
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(10-EWinPositionX,20-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton2Up, TPoint(11-EWinPositionX,22-EWinPositionY));
			for(TInt count3=0;count3<EMaxEventQueueSize-2;count3++)
				AddExpectedKey(EEventKeyDown,'A');
			}
			break;
		case 4:
			{
			SimulatePointer(TRawEvent::EButton1Up,10,20);
			SimulatePointer(TRawEvent::EButton2Up,11,22);
			iTest->SimulateKey(TRawEvent::EKeyDown,'B');
			SimulatePointer(TRawEvent::EButton1Down,12,23);
			SimulatePointer(TRawEvent::EPointerMove,13,24);
			iTest->SimulateKey(TRawEvent::EKeyUp,'B');
			SimulatePointer(TRawEvent::EButton3Down,15,25);
			SimulatePointer(TRawEvent::EButton1Up,14,24);
			iQueueClient->iWs.PurgePointerEvents();
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(10-EWinPositionX,20-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton2Up, TPoint(11-EWinPositionX,22-EWinPositionY));
			AddExpectedKey(EEventKeyDown,'B');
			AddExpectedKey(EEventKey,'B','b');
			AddExpectedKey(EEventKeyUp,'B');
//			AddExpectedPointer(TPointerEvent::EButton3Down, TPoint(15-EWinPositionX,25-EWinPositionY));
			AddExpectedEvent(EEventSwitchOn);	// To check pointer events purged okay
			}
			break;
		case 5:
			CreateEventTestGroup();
			AddExpectedEvent(EEventWindowGroupsChanged);
			break;
		case 6:
			CloseEventTestGroup();
			AddExpectedEvent(EEventWindowGroupsChanged);
			break;
		case 7:
			iQueueClient->iGroup->GroupWin()->DisableGroupChangeEvents();
			CreateAndCloseEventTestGroup();
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			AddExpectedEvent(EEventSwitchOn);
			break;
		case 8:
			SimulatePointer(TRawEvent::EButton1Down,12,23);
			iQueueClient->iWs.PurgePointerEvents();
			SimulatePointer(TRawEvent::EButton1Up,13,24);
			SimulatePointer(TRawEvent::EButton2Down,12,23);
			iQueueClient->iWs.PurgePointerEvents();
			SimulatePointer(TRawEvent::EButton2Up,13,24);
			SimulatePointer(TRawEvent::EButton3Down,12,23);
			iQueueClient->iWs.PurgePointerEvents();
			SimulatePointer(TRawEvent::EButton3Up,13,24);
//
			SendEvent(EEventUser);
			AddExpectedEvent(EEventUser);
			break;
		case 9:
			{
			for(TInt count5=0;count5<500;count5++)
				{
				RWindowGroup group(TheClient->iWs);
				group.Construct(ENullWsHandle);
				group.EnableReceiptOfFocus(EFalse);
				group.Close();
				SimulatePointer(TRawEvent::EButton1Up,10,20);
				}
			for(TInt count4=0;count4<EMaxEventQueueSize;count4++)
				AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(10-EWinPositionX,20-EWinPositionY));
			}
			break;
		case 10:
			{
			RWindowGroup *group=iQueueClient->iGroup->GroupWin();
			User::LeaveIfError(group->EnableOnEvents(EEventControlOnlyWithKeyboardFocus));
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			SimulatePointer(TRawEvent::EButton3Down,EWinPositionX,EWinPositionY);
			AddExpectedEvent(EEventSwitchOn);
			AddExpectedPointer(TPointerEvent::EButton3Down, TPoint(0,0));
//
			group->SetOrdinalPosition(-1);
			TheClient->iWs.Finish();
			iTest->SimulateEvent(TRawEvent::ESwitchOn);	// Shouldn't get it as in background this time
			AddExpectedEvent(EEventFocusLost);
			group->SetOrdinalPosition(0);
			TheClient->iWs.Finish();
			AddExpectedEvent(EEventFocusGained);
//
			User::LeaveIfError(group->EnableOnEvents(EEventControlOnlyWhenVisible));
			User::LeaveIfError(group->EnableModifierChangedEvents(EModifierShift, EEventControlOnlyWhenVisible));
			iTest->SimulateEvent(TRawEvent::ESwitchOn);	// Should get this as window still visible
			AddExpectedEvent(EEventSwitchOn);
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftShift);	// Should get modifier changed event as window still visible
			AddExpectedModifiersChanged(EModifierShift,EModifierShift);
			AddExpectedKey(EEventKeyDown,EStdKeyLeftShift);
			SimulatePointer(TRawEvent::EButton1Down,EWinPositionX,EWinPositionY);
			SimulatePointer(TRawEvent::EButton1Up,EWinPositionX,EWinPositionY);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(0,0));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(0,0));
			iQueueClient->iWin->SetPos(TPoint(-EWinWidth,-EWinHeight));
			TheClient->iWs.Finish();
// The following two events shouldn't cause an on or mod changed event as the window is now hidden
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftShift);
			AddExpectedKey(EEventKeyUp,EStdKeyLeftShift);
//
			iQueueClient->iWin->SetPos(TPoint(EWinPositionX,EWinPositionY));
			TheClient->iWs.Finish();
			}
			break;
		case 11:
			{
			RWindowGroup *group=iQueueClient->iGroup->GroupWin();
			SendEvent(EEventUser);
			AddExpectedEvent(EEventUser);

			if(iTest->IsFullRomL())
				break;
//
// Test focus only messages when with and without focus
//
			User::LeaveIfError(group->EnableErrorMessages(EEventControlOnlyWithKeyboardFocus));
			TriggerOomMessage(TTimeIntervalMinutes(3));
			AddExpectedErrorMessage(TWsErrorMessage::EDrawingRegion,KErrNoMemory);
			SendEvent(EEventUser);
			AddExpectedEvent(EEventUser);
			group->SetOrdinalPosition(1);
			TriggerOomMessage(TTimeIntervalMinutes(3));
			AddExpectedEvent(EEventFocusLost);
			group->SetOrdinalPosition(0);
			AddExpectedEvent(EEventFocusGained);
			SendEvent(EEventUser+1);
			AddExpectedEvent(EEventUser+1);
//
// Test focus only messages when with visible and invisible
//
			User::LeaveIfError(group->EnableErrorMessages(EEventControlOnlyWhenVisible));
			TriggerOomMessage(TTimeIntervalMinutes(3));
			AddExpectedErrorMessage(TWsErrorMessage::EDrawingRegion,KErrNoMemory);
			iQueueClient->iWin->SetPos(TPoint(-EWinWidth,-EWinHeight));
			TriggerOomMessage(TTimeIntervalMinutes(3));
			SendEvent(EEventUser+2);
			AddExpectedEvent(EEventUser+2);
			iQueueClient->iWin->SetPos(TPoint(EWinPositionX,EWinPositionY));
//
// Check always mode sends an event
//
			User::LeaveIfError(group->EnableErrorMessages(EEventControlAlways));
			TriggerOomMessage(TTimeIntervalMinutes(3));
			AddExpectedErrorMessage(TWsErrorMessage::EDrawingRegion,KErrNoMemory);
//
			iQueueClient->iWin->SetPos(TPoint(-EWinWidth,-EWinHeight));
			TriggerOomMessage(TTimeIntervalMinutes(3));
			AddExpectedErrorMessage(TWsErrorMessage::EDrawingRegion,KErrNoMemory);
			iQueueClient->iWin->SetPos(TPoint(EWinPositionX,EWinPositionY));
//
			group->SetOrdinalPosition(1);
			AddExpectedEvent(EEventFocusLost);
			TriggerOomMessage(TTimeIntervalMinutes(3));
			AddExpectedErrorMessage(TWsErrorMessage::EDrawingRegion,KErrNoMemory);
			group->SetOrdinalPosition(0);
			AddExpectedEvent(EEventFocusGained);
//
// Check timing of OOM messages
//
			TriggerOomMessage(TTimeIntervalMinutes(3));
			AddExpectedErrorMessage(TWsErrorMessage::EDrawingRegion,KErrNoMemory);
			TriggerOomMessage(TTimeIntervalMinutes(0));
			User::After(TTimeIntervalMicroSeconds32(1100000));	// 1.1 second
			TriggerOomMessage(TTimeIntervalMinutes(0));
			SendEvent(EEventUser+3);
			AddExpectedEvent(EEventUser+3);
			TriggerOomMessage(TTimeIntervalMinutes(3));	// Forward by a 2 minutes plus the 1.1 from previous test should trigger an OOM event
			AddExpectedErrorMessage(TWsErrorMessage::EDrawingRegion,KErrNoMemory);
			SendEvent(EEventUser+4);
			AddExpectedEvent(EEventUser+4);
			TriggerOomMessage(TTimeIntervalMinutes(-1)); // Clock going backwards should trigger an event
			AddExpectedErrorMessage(TWsErrorMessage::EDrawingRegion,KErrNoMemory);
			User::After(TTimeIntervalMicroSeconds32(800000));	// 8/10ths of a second
			TriggerOomMessage(TTimeIntervalMinutes(0));	// Shouldn't trigger an event
			SendEvent(EEventUser+5);
			AddExpectedEvent(EEventUser+5);
			}
			break;
		default:
			TheClient->iWs.DebugInfo(EWsDebugSetEventQueueTest,ETrue);
			TRAPD(err, EventQueueExpansionTestL());
			if (err!=KErrNone)
				{
				iFailed=ETrue;
				}
			TheClient->iWs.DebugInfo(EWsDebugSetEventQueueTest,EFalse);
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.SetAutoFlush(iTheClientFlush);
	iQueueClient->iWs.SetAutoFlush(iQuequeClientFlush);
	}

// Used for testing PDEF112243.
// Create RWsSession/RWindowGroup/RBlankWindow in a loop. In case of odd loop, 
// close the session which causes the previous window to gain focus. Also go 
// on adding events to the previous session. Along with some change in product
// code and this test code we get some space/memory before globaleventqueue, 
// which is the problem faced by licensee for this defect.
void CTEventTest::EventQueueExpansionTestL()
	{
	const TInt numClients = 5;
	TInt cliHanGrpWin = 777;
	TInt cliHanBlkWin = 888;
	TInt pushed=0;
	RWsSession wsSession[numClients];
	RWindowGroup groupWin[numClients];
	RBlankWindow blankWin[numClients];
	
	for (TInt i = 0; i < numClients; i++)
		{
		User::LeaveIfError(wsSession[i].Connect());
		CleanupClosePushL(wsSession[i]);
		
		groupWin[i] = RWindowGroup(wsSession[i]);
		User::LeaveIfError(groupWin[i].Construct(cliHanGrpWin++));
		CleanupClosePushL(groupWin[i]);
		groupWin[i].DefaultOwningWindow();
		
		blankWin[i] = RBlankWindow(wsSession[i]);
		User::LeaveIfError(blankWin[i].Construct(groupWin[i], cliHanBlkWin++));
		if (i%2 == 0)
			{
			CleanupClosePushL(blankWin[i]);
			pushed++;
			}
		blankWin[i].Activate();
		
		if (i%2 == 1)
			{
			CleanupStack::Pop(2, &wsSession[i]);
			wsSession[i].Close();
			
			TRawEvent rawEvent;
			for (TInt count=0; count < 40; count++)
				{
				rawEvent.Set(TRawEvent::EKeyDown,'A'+(i%26));
				wsSession[i-1].SimulateRawEvent(rawEvent);

				rawEvent.Set(TRawEvent::EKeyUp,'A'+(i%26));
				wsSession[i-1].SimulateRawEvent(rawEvent);
				}
			}
		}
	CleanupStack::PopAndDestroy(pushed*3, &wsSession[0]);
	}

void CTEventTest::doSendEvent(TWsEvent &aEvent)
	{
	iQueueClient->iWs.SendEventToWindowGroup(iQueueClient->WindowGroupIdentifier(),aEvent);
	}

void CTEventTest::SendKey(TInt aEventCode, TInt aScanCode, TInt aCode/*=0*/, TInt iRepeats/*=0*/)
	{
	TWsEvent event;
	if (aEventCode==EEventKey && aCode==0)
		aCode=aScanCode;
	event.SetType(aEventCode);
	event.Key()->iCode=aCode;
	event.Key()->iScanCode=aScanCode;
	event.Key()->iModifiers=0;
	event.Key()->iRepeats=iRepeats;
	doSendEvent(event);
	}

void CTEventTest::SendEvent(TInt aEventCode)
	{
	TWsQTEvent event;
	event.SetType(aEventCode);
	doSendEvent(event);
	}

void CTEventTest::SimulateAndCheck(TPoint aOrigin,TSize aScale,TInt aXOffset,TInt aYOffset,TPointerEvent::TType aEventType,TRect aPointerCursorArea)
	{
	TPoint ptCheck=PhysicalToLogical(TPoint(aOrigin.iX+aXOffset,aOrigin.iY+aYOffset)-aOrigin,aScale);
	if (ptCheck.iX < aPointerCursorArea.iTl.iX)
		{
		ptCheck.iX=aPointerCursorArea.iTl.iX;
		}
	else if (ptCheck.iX > aPointerCursorArea.iBr.iX)
		{
		ptCheck.iX=aPointerCursorArea.iBr.iX;
		}
	if (ptCheck.iY < aPointerCursorArea.iTl.iY)
		{
		ptCheck.iY=aPointerCursorArea.iTl.iY;
		}
	else if (ptCheck.iY > aPointerCursorArea.iBr.iY)
		{
		ptCheck.iY=aPointerCursorArea.iBr.iY;
		}
	AddExpectedPointer(aEventType,TPoint(ptCheck.iX-EWinPositionX,ptCheck.iY-EWinPositionY));
	}

void CTEventTest::CheckPointerCursorInDifferentScreenMode(TSizeMode aMode,TPoint aOrigin)
	{
	aMode.iOrigin=aOrigin;
	iQueueClient->iScreen->SetCurrentScreenModeAttributes(aMode);
	iQueueClient->iScreen->SetAppScreenMode(iCurrentMode);		//This makes the App pick up the new mode parameters
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);			//Ditto
	iQueueClient->iScreen->SetScreenMode(iCurrentMode);			//This causes all the windows to be updated

	//.. Tests the general pointer cursor area setting and getting
	RWsSession& ws=iQueueClient->iWs;
	TRect rect=ws.PointerCursorArea(iCurrentMode); //ws.PointerCursorArea() by default gets first screen mode's pointer cursor area so use other overloaded function
	TPoint origin=iQueueClient->iScreen->GetScreenModeOrigin(iCurrentMode);
	TSize scale=iQueueClient->iScreen->GetScreenModeScale(iCurrentMode);
	TSize fullScreenModeSize=iQueueClient->iScreen->SizeInPixels();
	TRect rect1(PhysicalToLogical(TPoint(),scale),PhysicalToLogical(fullScreenModeSize.AsPoint()-origin,scale));
	TRect rect2=rect1;
	rect2.Shrink(20,20);
	ws.SetPointerCursorArea(iCurrentMode,rect1);
	TEST(ws.PointerCursorArea(iCurrentMode)==rect1);
	ws.SetPointerCursorArea(iCurrentMode,rect2);
	TEST(ws.PointerCursorArea(iCurrentMode)==rect2);
	ws.SetPointerCursorArea(iCurrentMode,rect);
	TEST(ws.PointerCursorArea(iCurrentMode)==rect);

	//.. Test events are restricted to correct pointer cursor area
	TRect rectPointerCursorArea(10,10,51,51);
	iQueueClient->iWs.SetPointerCursorArea(iCurrentMode,rectPointerCursorArea);
	TPoint screenModeOrigin=iQueueClient->iScreen->GetDefaultScreenModeOrigin();

	SimulatePointer(TRawEvent::EButton1Down,screenModeOrigin.iX+5,screenModeOrigin.iY+5);
	SimulatePointer(TRawEvent::EButton1Up,screenModeOrigin.iX+25,screenModeOrigin.iY+25);
	SimulatePointer(TRawEvent::EButton1Down,screenModeOrigin.iX+50,screenModeOrigin.iY+50);
	SimulatePointer(TRawEvent::EButton1Up,screenModeOrigin.iX+75,screenModeOrigin.iY+75);

	SimulatePointer(TRawEvent::EButton1Down,screenModeOrigin.iX+10,screenModeOrigin.iY+15);
	SimulatePointer(TRawEvent::EButton1Up,screenModeOrigin.iX+20,screenModeOrigin.iY+5);
	SimulatePointer(TRawEvent::EButton1Down,screenModeOrigin.iX+35,screenModeOrigin.iY+40);
	SimulatePointer(TRawEvent::EButton1Up,screenModeOrigin.iX+45,screenModeOrigin.iY+70);

	SimulateAndCheck(screenModeOrigin,aMode.iScreenScale,5,5,TPointerEvent::EButton1Down,rectPointerCursorArea);
	SimulateAndCheck(screenModeOrigin,aMode.iScreenScale,25,25,TPointerEvent::EButton1Up,rectPointerCursorArea);
	SimulateAndCheck(screenModeOrigin,aMode.iScreenScale,50,50,TPointerEvent::EButton1Down,rectPointerCursorArea);
	SimulateAndCheck(screenModeOrigin,aMode.iScreenScale,75,75,TPointerEvent::EButton1Up,rectPointerCursorArea);

	SimulateAndCheck(screenModeOrigin,aMode.iScreenScale,10,15,TPointerEvent::EButton1Down,rectPointerCursorArea);
	SimulateAndCheck(screenModeOrigin,aMode.iScreenScale,20,5,TPointerEvent::EButton1Up,rectPointerCursorArea);
	SimulateAndCheck(screenModeOrigin,aMode.iScreenScale,35,40,TPointerEvent::EButton1Down,rectPointerCursorArea);
	SimulateAndCheck(screenModeOrigin,aMode.iScreenScale,45,70,TPointerEvent::EButton1Up,rectPointerCursorArea);
	}

void CTEventTest::SendEventTest_NextSetOfEvents()
	{
	TTimeIntervalMicroSeconds32 initialTime,time;
	TheClient->iWs.GetKeyboardRepeatRate(initialTime,time);
	switch(iEventSet++)
		{
		case 0:
			SendKey(EEventKeyDown,1);
			SendKey(EEventKeyDown,2);
			SendEvent(EEventUser);
			SendKey(EEventKeyUp,1);
			SendKey(EEventKeyUp,2);
			SendEvent(EEventUser+1);
			SendEvent(EEventUser+2);
//
			AddExpectedEvent(EEventFocusGained);
			AddExpectedKey(EEventKeyDown,1);
			AddExpectedKey(EEventKeyDown,2);
			AddExpectedEvent(EEventUser);
			AddExpectedKey(EEventKeyUp,1);
			AddExpectedKey(EEventKeyUp,2);
			AddExpectedEvent(EEventUser+1);
			AddExpectedEvent(EEventUser+2);
			break;
		case 1:
	#if defined(SCREEN_MODE_TESTING)
			if (iNumScrModes>1)
				{
				iQueueClient->iGroup->GroupWin()->EnableScreenChangeEvents();

				if (SetClickType(EClickCheck))
					return;
				iAddToClick=ETrue;

				iQueueClient->iScreen->SetScreenMode(1);

				iAddToClick=EFalse;
				SetClickType(EClickNone);

				AddExpectedEvent(EEventScreenDeviceChanged);
				SendEvent(EEventUser);
				AddExpectedEvent(EEventUser);
				iQueueClient->iGroup->GroupWin()->DisableScreenChangeEvents();
				if (iQueueClient->iScreen->ScreenModeEnforcement()==ESizeEnforcementPixelsAndRotation)
					AddExpectedEvent(EEventFocusLost);
				}
	#endif
			SendEvent(EEventUser+1);
			AddExpectedEvent(EEventUser+1);
			break;
		case 2:
	#if defined(SCREEN_MODE_TESTING)
			if (iNumScrModes>1)
				{
				iQueueClient->iGroup->GroupWin()->EnableScreenChangeEvents();

				if (SetClickType(EClickCheck))
					return;
				iAddToClick=ETrue;

				iQueueClient->iScreen->SetScreenMode(0);
				iQueueClient->iScreen->SetScreenMode(1);
				iQueueClient->iScreen->SetScreenMode(0);

				iAddToClick=EFalse;
				SetClickType(EClickNone);

				AddExpectedEvent(EEventScreenDeviceChanged);	// Only one event should get queued
				if (iQueueClient->iScreen->ScreenModeEnforcement()==ESizeEnforcementPixelsAndRotation)
					AddExpectedEvent(EEventFocusGained);
				SendEvent(EEventUser+2);
				AddExpectedEvent(EEventUser+2);
				iQueueClient->iGroup->GroupWin()->DisableScreenChangeEvents();
				}
	#endif
			SendEvent(EEventUser+3);
			AddExpectedEvent(EEventUser+3);
			break;
		case 3:
	#if defined(SCREEN_MODE_TESTING)
			if (iNumScrModes>1)
				{
				iQueueClient->iGroup->GroupWin()->EnableScreenChangeEvents();
				iQueueClient->iGroup->GroupWin()->DisableScreenChangeEvents();

				if (SetClickType(EClickCheck))
					return;
				iAddToClick=ETrue;

				iQueueClient->iScreen->SetScreenMode(0);

				iAddToClick=EFalse;
				SetClickType(EClickNone);

				SendEvent(EEventUser+1);
				AddExpectedEvent(EEventUser+1);
				}
	#endif
			SendEvent(EEventUser+4);
			AddExpectedEvent(EEventUser+4);
			break;
		case 4:
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeySpace);
			TheClient->iWs.Flush();
			User::After(initialTime.Int()+time.Int()/3);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKey,EStdKeySpace,' ');
			AddExpectedKey(EEventKey,EStdKeySpace,' ',1);
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			break;
		case 5:			//To test sending a key event with iRepeats!=0
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeySpace);
			TheClient->iWs.Flush();
			User::After(initialTime.Int()+time.Int()/3);
			SendKey(EEventKey,EStdKeyHome,EKeyHome,0);
			User::After(initialTime.Int()+time.Int()/3);
			SendKey(EEventKey,EStdKeyEnd,EKeyEnd,12345);
			User::After(initialTime.Int()+time.Int()/3);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKey,EStdKeySpace,' ');
			AddExpectedKey(EEventKey,EStdKeySpace,' ',1);
			AddExpectedKey(EEventKey,EStdKeyHome,EKeyHome);
			AddExpectedKey(EEventKey,EStdKeySpace,' ',1);
			AddExpectedKey(EEventKey,EStdKeyEnd,EKeyEnd,1);
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

void CTEventTest::InvisibleWindow_NextSetOfEvents()
	{
	switch(iEventSet++)
		{
		case 0:
			SimulatePointer(TRawEvent::EButton1Down,45,30);
			SimulatePointer(TRawEvent::EButton1Up,45,30);
			SimulatePointer(TRawEvent::EButton1Down,20,10);
			SimulatePointer(TRawEvent::EButton1Up,20,10);
			TheClient->iWs.Flush();
			iQueueClient->iChildWin->BaseWin()->SetVisible(EFalse);
			iQueueClient->iChildWin->BaseWin()->SetVisible(ETrue);
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,46,31);
			SimulatePointer(TRawEvent::EButton1Up,46,31);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(20-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(20-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(46-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(46-EWinPositionX,31-EWinPositionY));
			break;
		case 1:
			SimulatePointer(TRawEvent::EButton1Down,21,11);
			SimulatePointer(TRawEvent::EButton1Up,21,11);
			SimulatePointer(TRawEvent::EButton1Down,44,30);
			SimulatePointer(TRawEvent::EButton1Up,44,30);
			TheClient->iWs.Flush();
			iQueueClient->iWin->BaseWin()->SetVisible(EFalse);
			iQueueClient->iWin->BaseWin()->SetVisible(ETrue);
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,19,12);
			SimulatePointer(TRawEvent::EButton1Up,19,12);
			SimulatePointer(TRawEvent::EButton1Down,47,32);
			SimulatePointer(TRawEvent::EButton1Up,47,32);
			/*AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(44-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(44-EWinPositionX,30-EWinPositionY));*/
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(19-EWinPositionX,12-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(19-EWinPositionX,12-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(47-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(47-EWinPositionX,32-EWinPositionY));
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

#define TOP16A 0xAAAA0000
#define TOP16B 0x55550000
void CTEventTest::JavaAdditions_NextSetOfEvents()
	{
	switch(iEventSet++)
		{
		case 0:
			SimulatePointer(TRawEvent::EButton1Down,45,30);
			SimulatePointer(TRawEvent::EButton1Up,45,30);
			TheClient->iWs.Flush();
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerMoveEvents,EPointerGenerateSimulatedMove);
			iQueueClient->iWs.Flush();
#if !defined(TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA)
			SimulateButton1DownWithSimulatedMove(46,31);
#else
			SimulatePointer(TRawEvent::EButton1Down,46,31);
#endif			
			SimulatePointer(TRawEvent::EButton1Up,46,31);
			TheClient->iWs.Flush();
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerMoveEvents,0);
			iQueueClient->iWs.Flush();			
			SimulatePointer(TRawEvent::EButton1Down,20,10);
			SimulatePointer(TRawEvent::EButton1Up,20,10);
			TheClient->iWs.Flush();
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerMoveEvents,EPointerGenerateSimulatedMove);
			iQueueClient->iWs.Flush();
#if !defined(TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA)
			SimulateButton1DownWithSimulatedMove(21,11);
#else	
			SimulatePointer(TRawEvent::EButton1Down,21,11);			
#endif
			SimulatePointer(TRawEvent::EButton1Up,21,11);
			TheClient->iWs.Flush();
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerMoveEvents,EPointerMoveEvents);
			iQueueClient->iWs.Flush();				
			SimulatePointer(TRawEvent::EButton1Down,22,12);
			SimulatePointer(TRawEvent::EButton1Up,22,12);
			TheClient->iWs.Flush();
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerMoveEvents,EPointerGenerateSimulatedMove);
			iQueueClient->iWs.Flush();
#if !defined(TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA)
			SimulateButton1DownWithSimulatedMove(23,13);
#else			
			SimulatePointer(TRawEvent::EButton1Down,23,13);
#endif
			SimulatePointer(TRawEvent::EButton1Up,23,13);
			TheClient->iWs.Flush();
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerMoveEvents,EPointerFilterMove);
			iQueueClient->iWs.Flush();		
			SimulatePointer(TRawEvent::EButton1Down,24,14);
			SimulatePointer(TRawEvent::EButton1Up,24,14);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(45-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(45-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(46-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(46-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(46-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(20-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(20-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(21-EWinPositionX,11-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(21-EWinPositionX,11-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(21-EWinPositionX,11-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(22-EWinPositionX,12-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(22-EWinPositionX,12-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(23-EWinPositionX,13-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(23-EWinPositionX,13-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(23-EWinPositionX,13-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(24-EWinPositionX,14-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,14-EWinPositionY));
			break;
		#if defined(__WINS__)
		case 1:
			{
			RWindowGroup *group=iQueueClient->iGroup->GroupWin();
			group->SetOrdinalPosition(0);
			iTest->SimulateKey(TRawEvent::EKeyDown,'2'|TOP16A);
			iTest->SimulateKey(TRawEvent::EKeyUp,'2');
			TheClient->iWs.Flush();
			iQueueClient->iWs.SetRemoveKeyCode(EFalse);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'3'|TOP16B);
			iTest->SimulateKey(TRawEvent::EKeyUp,'3');
			TheClient->iWs.Flush();
			iQueueClient->iWs.SetRemoveKeyCode(ETrue);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'4'|TOP16B);
			iTest->SimulateKey(TRawEvent::EKeyUp,'4');
			AddExpectedKey(EEventKeyDown,'2');
			AddExpectedKey(EEventKey,'2',TOP16A>>16);
			AddExpectedKey(EEventKeyUp,'2');
			AddExpectedKey(EEventKeyDown,'3'|TOP16B);
			AddExpectedKey(EEventKey,'3',TOP16B>>16);
			AddExpectedKey(EEventKeyUp,'3');
			AddExpectedKey(EEventKeyDown,'4');
			AddExpectedKey(EEventKey,'4',TOP16B>>16);
			AddExpectedKey(EEventKeyUp,'4');
			break;
			}
		#endif
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

void CTEventTest::WinCap_NextSetOfEvents()
	{
	switch(iEventSet++)
		{
		case 0:
			//iQueueClient->SetChildCapture(RWindowBase::TCaptureEnabled);
			SimulatePointer(TRawEvent::EButton1Down,35,27);
			SimulatePointer(TRawEvent::EButton1Up,25,12);


			AddExpectedEvent(EEventFocusGained);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(35-EWinPositionX,27-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(25-EWinPositionX,12-EWinPositionY));
			break;


		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

void CTEventTest::XyInputType_NextSetOfEvents()
	{
	switch(iEventSet++)
		{
		case 0:
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerMoveEvents|EPointerFilterDrag,EPointerGenerateSimulatedMove);
			iQueueClient->iWs.Flush();
#if !defined(TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA)
			SimulateButton1DownWithSimulatedMove(46,31);
#else			
			SimulatePointer(TRawEvent::EButton1Down,46,31);
#endif
			SimulatePointer(TRawEvent::EButton1Up,46,31);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedPointer(TPointerEvent::EMove, TPoint(46-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(46-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(46-EWinPositionX,31-EWinPositionY));
			break;		//Must do something
		case 1:
			{
			// check if RWindowGroup::SimulatePointerEvent is working (DEF131563)
			RWindowGroup& group = *iQueueClient->iGroup->GroupWin();
			TRawEvent event;
			event.Set(TRawEvent::EButton1Down, 10, 10);
			group.SimulatePointerEvent(event);
			event.Set(TRawEvent::EButton1Up, 10, 10);
			group.SimulatePointerEvent(event);
			if(!iIs3DPointer)
				{
				// simulated move will be generated
				AddExpectedPointer(TPointerEvent::EMove, TPoint(10 - EWinPositionX, 10 - EWinPositionY));
				}
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(10 - EWinPositionX, 10 - EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(10 - EWinPositionX, 10 - EWinPositionY));
			}
			break;
		#if defined(__WINS__)
		case 2:
#if !defined(TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA)
			SimulateButton1DownWithSimulatedMove(45,32);
#else		
			SimulatePointer(TRawEvent::EButton1Down,45,32);
#endif
			SimulatePointer(TRawEvent::EButton1Up,45,32);
			TheClient->iWs.SimulateXyInputType(EXYInputNone);
			SimulatePointer(TRawEvent::EButton1Down,44,32);
			SimulatePointer(TRawEvent::EPointerMove,43,32);
			SimulatePointer(TRawEvent::EButton1Up,43,31);
			SimulatePointer(TRawEvent::EButton2Down,44,33);
			SimulatePointer(TRawEvent::EButton2Up,44,33);
			SimulatePointer(TRawEvent::EButton3Down,43,32);
			SimulatePointer(TRawEvent::EButton3Up,43,32);
			SimulatePointer(TRawEvent::EPointerSwitchOn,48,32);
			TheClient->iWs.SimulateXyInputType(EXYInputPointer);
#if !defined(TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA)
			SimulateButton1DownWithSimulatedMove(47,32);
#else				
			SimulatePointer(TRawEvent::EButton1Down,47,32);
#endif
			SimulatePointer(TRawEvent::EButton1Up,47,32);
			AddExpectedPointer(TPointerEvent::EMove, TPoint(45-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(45-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(45-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(47-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(47-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(47-EWinPositionX,32-EWinPositionY));
			break;
		case 3:
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerGenerateSimulatedMove,0);
			iQueueClient->iWs.Flush();
			TheClient->iWs.SimulateXyInputType(EXYInputPointer);
			SimulatePointer(TRawEvent::EButton1Down,49,32);
			SimulatePointer(TRawEvent::EPointerMove,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EButton1Down,51,32);
			SimulatePointer(TRawEvent::EPointerMove,52,30);
			SimulatePointer(TRawEvent::EButton1Up,52,30);
			TheClient->iWs.SimulateXyInputType(EXYInputPointer);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(49-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag, TPoint(50-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(50-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(51-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag, TPoint(52-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(52-EWinPositionX,30-EWinPositionY));
			break;
		case 4:
			SimulatePointer(TRawEvent::EButton1Down,49,31);
			SimulatePointer(TRawEvent::EButton1Up,49,31);
			TheClient->iWs.SimulateXyInputType(EXYInputMouse);
			SimulatePointer(TRawEvent::EPointerMove,50,31);
			SimulatePointer(TRawEvent::EButton1Down,50,31);
			SimulatePointer(TRawEvent::EPointerMove,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EPointerMove,51,31);
			SimulatePointer(TRawEvent::EButton1Down,51,31);
			SimulatePointer(TRawEvent::EPointerMove,51,30);
			SimulatePointer(TRawEvent::EButton1Up,51,30);
			TheClient->iWs.SimulateXyInputType(EXYInputPointer);
			SimulatePointer(TRawEvent::EButton1Down,52,31);
			SimulatePointer(TRawEvent::EButton1Up,52,31);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(49-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(49-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(50-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(50-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag, TPoint(50-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(50-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(51-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(51-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag, TPoint(51-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(51-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(52-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(52-EWinPositionX,31-EWinPositionY));
			break;
		case 5:
			SimulatePointer(TRawEvent::EButton1Down,69,31);
			SimulatePointer(TRawEvent::EButton1Up,69,31);
			TheClient->iWs.SimulateXyInputType(EXYInputDeltaMouse);
			SimulatePointer(TRawEvent::EPointerMove,-5,-5);
			SimulatePointer(TRawEvent::EButton1Down,7,8);
			SimulatePointer(TRawEvent::EPointerMove,10,-3);
			SimulatePointer(TRawEvent::EButton1Up,-7,-8);
			SimulatePointer(TRawEvent::EPointerMove,-4,9);
			SimulatePointer(TRawEvent::EButton1Down,123,456);
			SimulatePointer(TRawEvent::EPointerMove,15,13);
			SimulatePointer(TRawEvent::EButton1Up,-2345,-6789);
			TheClient->iWs.SimulateXyInputType(EXYInputPointer);
			SimulatePointer(TRawEvent::EButton1Down,62,31);
			SimulatePointer(TRawEvent::EButton1Up,62,31);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(69-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(69-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(64-EWinPositionX,26-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(64-EWinPositionX,26-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag, TPoint(74-EWinPositionX,23-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(74-EWinPositionX,23-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(70-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(70-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag, TPoint(85-EWinPositionX,45-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(85-EWinPositionX,45-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(62-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(62-EWinPositionX,31-EWinPositionY));
			break;
		#endif
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

void CTEventTest::MovingPointerCursor_NextSetOfEventsL()
	{
	switch(iEventSet++)
		{
		case 0:
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerFilterMove|EPointerFilterDrag,0);
			iQueueClient->iWin->BaseWin()->SetPointerGrab(EFalse);
			iQueueClient->iChildWin->BaseWin()->PointerFilter(EPointerFilterMove|EPointerFilterDrag,0);
			iQueueClient->iChildWin->BaseWin()->SetPointerGrab(EFalse);
			iQueueClient->iWs.Flush();
			//AddExpectedEvent(EEventFocusGained);
			break;
		#if defined(__WINS__)
		case 1:
			TheClient->iWs.SimulateXyInputType(EXYInputMouse);
			iQueueClient->iChildWin->BaseWin()->PointerFilter(EPointerFilterMove|EPointerFilterDrag,0);
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerFilterMove|EPointerFilterDrag,0);
			iQueueClient->iWin->BaseWin()->SetPointerGrab(EFalse);
			iQueueClient->iChildWin->BaseWin()->SetPointerGrab(EFalse);
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EPointerMove,46,31);
			SimulatePointer(TRawEvent::EButton1Down,46,31);
			SimulatePointer(TRawEvent::EPointerMove,47,32);
			SimulatePointer(TRawEvent::EButton1Up,47,32);
			AddExpectedPointer(TPointerEvent::EMove, TPoint(46-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(46-EWinPositionX,31-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag, TPoint(47-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(47-EWinPositionX,32-EWinPositionY));
			break;
		case 2:
			TheClient->iWs.SimulateXyInputType(EXYInputDeltaMouse);
			TheClient->iWs.SetPointerCursorPosition(TPoint(62,24));
			SimulatePointer(TRawEvent::EPointerMove,-5,-5);
			TheClient->iWs.SetPointerCursorPosition(TPoint(60,20));
			SimulatePointer(TRawEvent::EPointerMove,6,7);
			SimulatePointer(TRawEvent::EButton1Down,27,38);
			SimulatePointer(TRawEvent::EPointerMove,0,0);
			TheClient->iWs.SetPointerCursorPosition(TPoint(55,25));
			SimulatePointer(TRawEvent::EPointerMove,4,-13);
			TheClient->iWs.SetPointerCursorPosition(TPoint(58,20));
			SimulatePointer(TRawEvent::EButton1Up,45,-678);
			TheClient->iWs.Flush();
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerFilterEnterExit,0);
			iQueueClient->iChildWin->BaseWin()->PointerFilter(EPointerFilterEnterExit,0);
			iQueueClient->iWs.Flush();
			TheClient->iWs.SetPointerCursorPosition(TPoint(80,45));
			TheClient->iWs.SimulateXyInputType(EXYInputPointer);
			AddExpectedPointer(TPointerEvent::EMove, TPoint(57-EWinPositionX,19-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(66-EWinPositionX,27-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(66-EWinPositionX,27-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag, TPoint(66-EWinPositionX,27-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag, TPoint(59-EWinPositionX,12-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(58-EWinPositionX,20-EWinPositionY));
			AddExpectedEnterExit(EEventPointerEnter, TPoint(58-EWinPositionX,20-EWinPositionY));
			AddExpectedEnterExit(EEventPointerExit, TPoint(58-EWinPositionX,20-EWinPositionY));
			AddExpectedEnterExit(EEventPointerEnter, TPoint(80-EWinPositionX,45-EWinPositionY));
			break;
		case 3:
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerFilterEnterExit,EPointerFilterEnterExit);
			iQueueClient->iChildWin->BaseWin()->PointerFilter(EPointerFilterEnterExit,EPointerFilterEnterExit);
			iQueueClient->iWs.Flush();
			TheClient->iWs.SetPointerCursorArea(TRect(0,0,70,240));
			TheClient->iWs.SimulateXyInputType(EXYInputDeltaMouse);
			TheClient->iWs.SetPointerCursorPosition(TPoint(65,30));
			SimulatePointer(TRawEvent::EPointerMove,20,0);
			TheClient->iWs.SetPointerCursorPosition(TPoint(75,32));
			SimulatePointer(TRawEvent::EPointerMove,-1,20);
			SimulatePointer(TRawEvent::EButton1Down,29,37);
			SimulatePointer(TRawEvent::EPointerMove,25,-13);
			SimulatePointer(TRawEvent::EButton1Up,47,-648);
			TheClient->iWs.SimulateXyInputType(EXYInputPointer);
			AddExpectedPointer(TPointerEvent::EMove, TPoint(69-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(68-EWinPositionX,52-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(68-EWinPositionX,52-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag, TPoint(69-EWinPositionX,39-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(69-EWinPositionX,39-EWinPositionY));
			break;
		case 4:
			TheClient->iWs.SetPointerCursorArea(TRect(EWinPositionX+2,EWinPositionY+2,EWinPositionX+99,EWinPositionY+49));
			TheClient->iWs.SimulateXyInputType(EXYInputDeltaMouse);
			TheClient->iWs.SetPointerCursorPosition(TPoint(56,30));
			SimulatePointer(TRawEvent::EPointerMove,0,-34);
			TheClient->iWs.SetPointerCursorPosition(TPoint(56,30));
			SimulatePointer(TRawEvent::EPointerMove,0,0);
			SimulatePointer(TRawEvent::EPointerMove,-77,-35);
			TheClient->iWs.SetPointerCursorPosition(TPoint(56,30));
			SimulatePointer(TRawEvent::EPointerMove,0,0);
			SimulatePointer(TRawEvent::EPointerMove,-78,0);
			TheClient->iWs.SetPointerCursorPosition(TPoint(56,30));
			SimulatePointer(TRawEvent::EPointerMove,0,0);
			SimulatePointer(TRawEvent::EPointerMove,-79,36);
			TheClient->iWs.SetPointerCursorPosition(TPoint(56,30));
			SimulatePointer(TRawEvent::EPointerMove,0,0);
			SimulatePointer(TRawEvent::EPointerMove,0,37);
			TheClient->iWs.SetPointerCursorPosition(TPoint(56,30));
			SimulatePointer(TRawEvent::EPointerMove,0,0);
			SimulatePointer(TRawEvent::EPointerMove,80,38);
			TheClient->iWs.SetPointerCursorPosition(TPoint(56,30));
			SimulatePointer(TRawEvent::EPointerMove,0,0);
			SimulatePointer(TRawEvent::EPointerMove,81,0);
			TheClient->iWs.SetPointerCursorPosition(TPoint(56,30));
			SimulatePointer(TRawEvent::EPointerMove,0,0);
			SimulatePointer(TRawEvent::EPointerMove,82,-39);
			TheClient->iWs.SetPointerCursorPosition(TPoint(2,2));
			SimulatePointer(TRawEvent::EPointerMove,30,30);
			TheClient->iWs.SetPointerCursorPosition(TPoint(1,30));
			SimulatePointer(TRawEvent::EPointerMove,1,0);
			TheClient->iWs.SetPointerCursorPosition(TPoint(2,88));
			SimulatePointer(TRawEvent::EPointerMove,30,-30);
			TheClient->iWs.SetPointerCursorPosition(TPoint(56,88));
			SimulatePointer(TRawEvent::EPointerMove,0,-1);
			TheClient->iWs.SetPointerCursorPosition(TPoint(123,88));
			SimulatePointer(TRawEvent::EPointerMove,-30,-30);
			TheClient->iWs.SetPointerCursorPosition(TPoint(123,30));
			SimulatePointer(TRawEvent::EPointerMove,-1,0);
			TheClient->iWs.SetPointerCursorPosition(TPoint(123,2));
			SimulatePointer(TRawEvent::EPointerMove,-30,30);
			TheClient->iWs.SetPointerCursorPosition(TPoint(56,1));
			SimulatePointer(TRawEvent::EPointerMove,0,1);
			TheClient->iWs.SimulateXyInputType(EXYInputPointer);
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,2));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(2,2));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(2,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(2,48));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,48));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(98,48));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(98,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(98,2));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(32,32));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(3,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(32,18));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,47));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(68,18));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(97,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(68,32));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,3));
			break;
		case 5:
			TheClient->iWs.SetPointerCursorArea(TRect(0,0,EWinPositionX+100,EWinPositionY+50));
			TheClient->iWs.SimulateXyInputType(EXYInputDeltaMouse);
			TheClient->iWs.SetPointerCursorPosition(TPoint(57,31));
			TheClient->iWs.SetPointerCursorArea(TRect(0,35,EWinPositionX+100,EWinPositionY+50));
			SimulatePointer(TRawEvent::EPointerMove,1,0);
			TheClient->iWs.SetPointerCursorPosition(TPoint(10,55));
			TheClient->iWs.SetPointerCursorArea(TRect(15,0,EWinPositionX+100,50));
			SimulatePointer(TRawEvent::EPointerMove,1,1);
			TheClient->iWs.SetPointerCursorPosition(TPoint(57,31));
			TheClient->iWs.SetPointerCursorArea(TRect(0,0,50,EWinPositionY+50));
			SimulatePointer(TRawEvent::EPointerMove,0,1);
			TheClient->iWs.SetPointerCursorPosition(TPoint(10,10));
			TheClient->iWs.SetPointerCursorArea(TRect(15,15,EWinPositionX+100,EWinPositionY+50));
			SimulatePointer(TRawEvent::EPointerMove,1,-1);
			TheClient->iWs.SetPointerCursorPosition(TPoint(57,31));
			TheClient->iWs.SetPointerCursorArea(TRect(0,0,EWinPositionX+100,30));
			SimulatePointer(TRawEvent::EPointerMove,-1,0);
			TheClient->iWs.SetPointerCursorPosition(TPoint(100,10));
			TheClient->iWs.SetPointerCursorArea(TRect(0,15,95,EWinPositionY+50));
			SimulatePointer(TRawEvent::EPointerMove,-1,-1);
			TheClient->iWs.SetPointerCursorPosition(TPoint(57,31));
			TheClient->iWs.SetPointerCursorArea(TRect(60,0,EWinPositionX+100,EWinPositionY+50));
			SimulatePointer(TRawEvent::EPointerMove,0,-1);
			TheClient->iWs.SetPointerCursorPosition(TPoint(100,55));
			TheClient->iWs.SetPointerCursorArea(TRect(0,0,95,50));
			SimulatePointer(TRawEvent::EPointerMove,1,-1);
			TheClient->iWs.SimulateXyInputType(EXYInputPointer);
			AddExpectedPointer(TPointerEvent::EMove, TPoint(58-EWinPositionX,35-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(16-EWinPositionX,49-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(49-EWinPositionX,32-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(16-EWinPositionX,15-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(56-EWinPositionX,29-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(93-EWinPositionX,15-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(60-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EMove, TPoint(94-EWinPositionX,48-EWinPositionY));
			break;
		#endif
		default:
			TheClient->iWs.SetPointerCursorArea(TRect(0,0,95,50));
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

void CTEventTest::RotatedMode_NextSetOfEventsL()
	{
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KSet,"RotatedMode SetOfEvents: %d  Max=4");
	logMessageText.Format(KSet,iEventSet);
	INFO_PRINTF1(logMessageText);
#endif
	switch(iEventSet++)
		{
		case 0:
			{
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerFilterMove|EPointerFilterDrag,0);
			iQueueClient->iWin->BaseWin()->SetPointerGrab(EFalse);
			iQueueClient->iWs.Flush();
			TheClient->iWs.SetPointerCursorArea(TRect(10,10,51,51));
			SimulatePointer(TRawEvent::EButton1Down,5,5);
			SimulatePointer(TRawEvent::EButton1Up,25,5);
			SimulatePointer(TRawEvent::EButton1Down,55,5);
			SimulatePointer(TRawEvent::EButton1Up,55,35);
			SimulatePointer(TRawEvent::EButton1Down,55,55);
			SimulatePointer(TRawEvent::EButton1Up,25,55);
			SimulatePointer(TRawEvent::EButton1Down,5,55);
			SimulatePointer(TRawEvent::EButton1Up,5,35);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(10-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(25-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(50-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(50-EWinPositionX,35-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(50-EWinPositionX,50-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(25-EWinPositionX,50-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(10-EWinPositionX,50-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(10-EWinPositionX,35-EWinPositionY));
			}
			break;
		case 1:
			{
			TInt enforc=TheClient->iScreen->ScreenModeEnforcement();
			iBlankWin->SetVisible(ETrue);
		#if defined(LOGGING)
			_LIT(KSet1,"AUTO  Rotating Events :1st ");
			INFO_PRINTF1(KSet1);
		#endif
			INFO_PRINTF1(_L(" Set of Events"));
			if (iScreenModeTests&EDoRot90)
				{
				TheClient->iScreen->SetScreenMode(1);
				TheClient->iWs.Flush();
				iTest->DelayIfFullRomL();
				INFO_PRINTF1(_L(" Screen mode 1."));
				TPoint screenModeOrigin=TheClient->iScreen->GetDefaultScreenModeOrigin();
			#if defined(__WINS__)
				iTest->SimulatePointer(TRawEvent::EButton1Down,20+screenModeOrigin.iX,20+screenModeOrigin.iY);
				iTest->SimulatePointer(TRawEvent::EButton1Up,20+screenModeOrigin.iX,20+screenModeOrigin.iY);
			#else
				// simulate pointer at (20,20) on rotated screen by providing physical coordinates
				iTest->SimulatePointer(TRawEvent::EButton1Down,iPhysicalScreenSize.iWidth-1-20+screenModeOrigin.iX,20+screenModeOrigin.iY);
				iTest->SimulatePointer(TRawEvent::EButton1Up,iPhysicalScreenSize.iWidth-1-20+screenModeOrigin.iX,20+screenModeOrigin.iY);
			#endif
				INFO_PRINTF1(_L(" Sent Up and Down 90"));
				}
			if (iScreenModeTests&EDoRot180)
				{
				TheClient->iScreen->SetCurrentRotations(0,CFbsBitGc::EGraphicsOrientationRotated180);
				TheClient->iScreen->SetScreenMode(0);
				INFO_PRINTF1(_L(" Screen mode 0"));
				TheClient->iWs.Flush();
				iTest->DelayIfFullRomL();
				INFO_PRINTF1(_L(" Upside Down"));
			#if defined(__WINS__)
				iTest->SimulatePointer(TRawEvent::EButton1Down,22,22);
				iTest->SimulatePointer(TRawEvent::EButton1Up,22,22);
			#else
				// simulate pointer at (22,22) on rotated screen by providing physical coordinates
				iTest->SimulatePointer(TRawEvent::EButton1Down,iPhysicalScreenSize.iWidth-1-22,iPhysicalScreenSize.iHeight-1-22);
				iTest->SimulatePointer(TRawEvent::EButton1Up,iPhysicalScreenSize.iWidth-1-22,iPhysicalScreenSize.iHeight-1-22);
			#endif
				INFO_PRINTF1(_L(" Sent Up and Down 180"));
				}
			TheClient->iScreen->SetCurrentRotations(0,CFbsBitGc::EGraphicsOrientationNormal);
			TheClient->iScreen->SetScreenMode(0);
			TheClient->iWs.Flush();
			iTest->DelayIfFullRomL();
			INFO_PRINTF1(_L(" Normal Screen mode"));
			if (iScreenModeTests&EDoRot0)
				{
				SimulatePointer(TRawEvent::EButton1Down,24,24);
				SimulatePointer(TRawEvent::EButton1Up,24,24);
				}
			if (iScreenModeTests&EDoRot90)
				{
				if (enforc>ESizeEnforcementNone)
					{
					AddExpectedEvent(EEventFocusLost);
					AddExpectedEvent(EEventFocusGained);
					}
				else
					{
                    AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(20-EWinPositionX,20-EWinPositionY-iYOffset));
                    AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(20-EWinPositionX,20-EWinPositionY-iYOffset));
                    }
				}
			if (iScreenModeTests&EDoRot180)
				{
                AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(22-EWinPositionX,22-EWinPositionY-iYOffset));
                AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(22-EWinPositionX,22-EWinPositionY-iYOffset));
				}
			if (iScreenModeTests&EDoRot0)
				{
				AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(24-EWinPositionX,24-EWinPositionY));
				AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,24-EWinPositionY));
				}
			INFO_PRINTF1(_L(" Added Expected"));
			User::After(EPointerCursorUpdateGap);
			iBlankWin->SetVisible(EFalse);
			}
			break;
	#if defined(__WINS__)
		case 2:
			{
			TInt enforc=TheClient->iScreen->ScreenModeEnforcement();
			//INFO_PRINTF1("AUTO  Rotating Events ");
			//INFO_PRINTF1(" :2nd Set of Events");
			TheClient->iWs.SimulateXyInputType(EXYInputDeltaMouse);
			TheClient->iWs.SetPointerCursorPosition(TPoint(24+EWinPositionX,24+EWinPositionY));
			if (iScreenModeTests&EDoRot90)
				{
				TheClient->iScreen->SetScreenMode(1);
				TheClient->iWs.Flush();
				iTest->DelayIfFullRomL();
				SimulatePointer(TRawEvent::EPointerMove,1,2);
				SimulatePointer(TRawEvent::EButton1Down,56,89);
				SimulatePointer(TRawEvent::EPointerMove,2,-1);
				SimulatePointer(TRawEvent::EButton1Up,234,456);
				}
			else
				TheClient->iWs.SetPointerCursorPosition(TPoint(27+EWinPositionX,25+EWinPositionY));
			if (iScreenModeTests&EDoRot180)
				{
				TheClient->iScreen->SetCurrentRotations(0,CFbsBitGc::EGraphicsOrientationRotated180);
				TheClient->iScreen->SetScreenMode(0);
				TheClient->iWs.Flush();
				iTest->DelayIfFullRomL();
				SimulatePointer(TRawEvent::EPointerMove,-3,4);
				SimulatePointer(TRawEvent::EButton1Down,156,289);
				SimulatePointer(TRawEvent::EPointerMove,-4,-3);
				SimulatePointer(TRawEvent::EButton1Up,2234,3456);
				}
			else
				TheClient->iWs.SetPointerCursorPosition(TPoint(20+EWinPositionX,26+EWinPositionY));
			TheClient->iScreen->SetCurrentRotations(0,CFbsBitGc::EGraphicsOrientationNormal);
			TheClient->iScreen->SetScreenMode(0);
			TheClient->iWs.Flush();
			iTest->DelayIfFullRomL();
			if (iScreenModeTests&EDoRot0)
				{
				SimulatePointer(TRawEvent::EButton1Down,24,24);
				SimulatePointer(TRawEvent::EButton1Up,24,24);
				}
			TheClient->iWs.SimulateXyInputType(EXYInputPointer);
			TheClient->iWs.Flush();
			if (iScreenModeTests&EDoRot90)
				{
				if (enforc>ESizeEnforcementNone)
					{
					AddExpectedEvent(EEventFocusLost);
					AddExpectedEvent(EEventFocusGained);
					}
				else
					{
					AddExpectedPointer(TPointerEvent::EMove, TPoint(25,26));
					AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(25,26));
					AddExpectedPointer(TPointerEvent::EDrag, TPoint(27,25));
					AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(27,25));
					}
				}
			if (iScreenModeTests&EDoRot180)
				{
				AddExpectedPointer(TPointerEvent::EMove, TPoint(24,29));
				AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(24,29));
				AddExpectedPointer(TPointerEvent::EDrag, TPoint(20,26));
				AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(20,26));
				}
			if (iScreenModeTests&EDoRot0)
				{
				AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(20,26));
				AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(20,26));
				}
			}
			break;
		case 3:
			{
		#if defined(LOGGING)
			_LIT(KSet2,"AUTO  Rotating Events ");
			INFO_PRINTF1(KSet2);
		#endif
			INFO_PRINTF1(_L(" :3rd Set of Events"));
			if (iScreenModeTests&EDoScale)
				{
				INFO_PRINTF1(_L(" :Screen Scalling Supported"));
				iBlankWin->SetVisible(ETrue);
				iCurrentMode=iNumScrModes;
				--iCurrentMode;
				iTheClientFlush=TheClient->iWs.SetAutoFlush(ETrue);
				iQuequeClientFlush=iQueueClient->iWs.SetAutoFlush(ETrue);
				iQueueClient->iScreen->SetScreenMode(iCurrentMode);
				TheClient->iScreen->SetAppScreenMode(iCurrentMode);
				iQueueClient->iScreen->SetAppScreenMode(iCurrentMode);
				iSizeMode=iQueueClient->iScreen->GetCurrentScreenModeAttributes();
				TSizeMode testMode=iSizeMode;
				for (TInt xScale=2;xScale<4;xScale++)
					{
				INFO_PRINTF1(_L(" :New X-Scale Value"));
					for (TInt yScale=2;yScale<4;yScale++)
						{
						INFO_PRINTF1(_L(" :New Y-Scale Value"));
						testMode.iScreenScale.SetSize(xScale,yScale);
						CheckPointerCursorInDifferentScreenMode(testMode,TPoint(20,30));
						}
					}
				User::After(EPointerCursorUpdateGap);
				iBlankWin->SetVisible(EFalse);
				break;
				}
			}
		case 4:
			{
			if (iScreenModeTests&EDoScale)
				{
				TSizeMode testMode=iSizeMode;
				iBlankWin->SetVisible(ETrue);
				for (TInt xScale=2;xScale<4;xScale++)
					{
					for (TInt yScale=2;yScale<4;yScale++)
						{
						testMode.iScreenScale.SetSize(xScale,yScale);
						CheckPointerCursorInDifferentScreenMode(testMode,TPoint(30,20));
						}
					}
				User::After(EPointerCursorUpdateGap);
				iBlankWin->SetVisible(EFalse);
				break;
				}
			}
	#endif
		default:
		#if defined(__WINS__)
			if (iScreenModeTests&EDoScale)
				{
				iQueueClient->iScreen->SetScreenMode(iCurrentMode);
				iQueueClient->iScreen->SetCurrentScreenModeAttributes(iSizeMode);
				iQueueClient->iScreen->SetAppScreenMode(0);
				TheClient->iScreen->SetAppScreenMode(0);
				iQueueClient->iScreen->SetScreenMode(0);
				iBlankWin->SetVisible(EFalse);
				TheClient->iWs.SetAutoFlush(iTheClientFlush);
				iQueueClient->iWs.SetAutoFlush(iQuequeClientFlush);
				}
		#endif
			User::After(500000);		//0.5secs, to allow other screen devices to respond to the flip events
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}


void CTEventTest::ScreenScaling_NextSetOfEventsL()
	{
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KSet,"ScreenScaling SetOfEvents: %d (Last=3)");
	logMessageText.Format(KSet,iEventSet);
	INFO_PRINTF1(logMessageText);
#endif
	if(! (iScreenModeTests & EDoScale))
		{
		User::After(500000);
		CActiveScheduler::Stop();
		return;
		}
	switch(iEventSet++)
		{
		case 0:
			{
			iTheClientFlush=TheClient->iWs.SetAutoFlush(ETrue);
			iQuequeClientFlush=iQueueClient->iWs.SetAutoFlush(ETrue);
			iBlankWin->SetVisible(ETrue);

			TInt mode = TheClient->iScreen->CurrentScreenMode();

			iSizeMode = iQueueClient->iScreen->GetCurrentScreenModeAttributes();

			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerFilterMove|EPointerFilterDrag,0);
			iQueueClient->iWin->BaseWin()->SetPointerGrab(EFalse);

			TSizeMode szMode2 = iSizeMode;

			szMode2.iScreenScale.SetSize(2,2);

			iQueueClient->iScreen->SetCurrentScreenModeAttributes(szMode2);
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);

			TPoint theScreenOrigin(iQueueClient->iScreen->GetScreenModeOrigin(mode));

			SimulatePointer(TRawEvent::EButton1Down,
				20+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				20+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			iQueueClient->iScreen->SetCurrentScreenModeAttributes(iSizeMode);
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);

			AddExpectedEvent(EEventFocusGained);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(10-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(10-EWinPositionX,10-EWinPositionY));

			break;
			}
		case 1:
			{
			TSizeMode szMode2 = iSizeMode;
			TInt mode = TheClient->iScreen->CurrentScreenMode();

			szMode2.iScreenScale.SetSize(2,2);
			szMode2.iOrigin.SetXY(20,20);

			iQueueClient->iScreen->SetCurrentScreenModeAttributes(szMode2);
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);

			TPoint theScreenOrigin(iQueueClient->iScreen->GetDefaultScreenModeOrigin());

			SimulatePointer(TRawEvent::EButton1Down,
				30+theScreenOrigin.iX, 30+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Down,
				40+theScreenOrigin.iX, 60+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);

			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(15-EWinPositionX,15-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(13-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(20-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(13-EWinPositionX,10-EWinPositionY));

			iQueueClient->iScreen->SetCurrentScreenModeAttributes(iSizeMode);
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);

			theScreenOrigin = TPoint(iQueueClient->iScreen->GetDefaultScreenModeOrigin());

			SimulatePointer(TRawEvent::EButton1Down,
				30+theScreenOrigin.iX, 30+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Down,
				40+theScreenOrigin.iX, 50+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);

			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(30-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(26-EWinPositionX,20-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(40-EWinPositionX,50-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(26-EWinPositionX,20-EWinPositionY));
			break;
			}
		case 2:
			{
			TInt mode = TheClient->iScreen->CurrentScreenMode();
			TRect	theRect = TheClient->iWs.PointerCursorArea();

			TSizeMode szMode2 = iSizeMode;

			//-----------  scaling 2X2
			szMode2.iScreenScale.SetSize(2,2);

			iQueueClient->iScreen->SetCurrentScreenModeAttributes(szMode2);
			iQueueClient->iWs.SetPointerCursorArea(mode, TRect(24,10,52,52));
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);


			TPoint theScreenOrigin(iQueueClient->iScreen->GetDefaultScreenModeOrigin());

			SimulatePointer(TRawEvent::EButton1Down,
				20+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Down,
				50+theScreenOrigin.iX, 30+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);

			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(24-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(25-EWinPositionX,15-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,10-EWinPositionY));
			//------------------------  scaling 1X2
			szMode2.iScreenScale.SetSize(1,2);

			iQueueClient->iScreen->SetCurrentScreenModeAttributes(szMode2);
			iQueueClient->iWs.SetPointerCursorArea(mode, TRect(24,10,52,52));
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);

			theScreenOrigin = TPoint(iQueueClient->iScreen->GetDefaultScreenModeOrigin());

			SimulatePointer(TRawEvent::EButton1Down,
				20+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Down,
				50+theScreenOrigin.iX, 30+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);

			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(24-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(26-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(50-EWinPositionX,15-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(26-EWinPositionX,10-EWinPositionY));
			//------------------------  scaling 2X1
			szMode2.iScreenScale.SetSize(2,1);

			iQueueClient->iScreen->SetCurrentScreenModeAttributes(szMode2);
			iQueueClient->iWs.SetPointerCursorArea(mode, TRect(24,10,52,52));
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);

			theScreenOrigin = TPoint(iQueueClient->iScreen->GetDefaultScreenModeOrigin());

			SimulatePointer(TRawEvent::EButton1Down,
				20+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Down,
				50+theScreenOrigin.iX, 30+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);

			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(24-EWinPositionX,20-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,20-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(25-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,20-EWinPositionY));
			//------------------------  scaling 3X1

			szMode2.iScreenScale.SetSize(3,1);

			iQueueClient->iScreen->SetCurrentScreenModeAttributes(szMode2);
			iQueueClient->iWs.SetPointerCursorArea(mode, TRect(24,10,52,52));
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);

			theScreenOrigin = TPoint(iQueueClient->iScreen->GetDefaultScreenModeOrigin());

			SimulatePointer(TRawEvent::EButton1Down,
				21+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				30+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Down,
				75+theScreenOrigin.iX, 30+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				30+theScreenOrigin.iX, 20+theScreenOrigin.iY);

			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(24-EWinPositionX,20-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,20-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(25-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,20-EWinPositionY));
			//------------------------  scaling 3X2

			szMode2.iScreenScale.SetSize(3,2);

			iQueueClient->iScreen->SetCurrentScreenModeAttributes(szMode2);
			iQueueClient->iWs.SetPointerCursorArea(mode, TRect(24,10,52,52));
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);

			theScreenOrigin = TPoint(iQueueClient->iScreen->GetDefaultScreenModeOrigin());

			SimulatePointer(TRawEvent::EButton1Down,
				21+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				30+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Down,
				75+theScreenOrigin.iX, 30+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				30+theScreenOrigin.iX, 20+theScreenOrigin.iY);

			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(24-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(25-EWinPositionX,15-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,10-EWinPositionY));
			//------------------------  scaling 3X3
			szMode2.iScreenScale.SetSize(3,3);

			iQueueClient->iScreen->SetCurrentScreenModeAttributes(szMode2);
			iQueueClient->iWs.SetPointerCursorArea(mode, TRect(24,10,52,52));
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);

			theScreenOrigin = TPoint(iQueueClient->iScreen->GetDefaultScreenModeOrigin());

			SimulatePointer(TRawEvent::EButton1Down,
				21+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				30+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Down,
				75+theScreenOrigin.iX, 33+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				30+theScreenOrigin.iX, 20+theScreenOrigin.iY);

			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(24-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,10-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(25-EWinPositionX,11-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(24-EWinPositionX,10-EWinPositionY));
			//------------------------
			iQueueClient->iWs.SetPointerCursorArea(mode, theRect);
			iQueueClient->iScreen->SetCurrentScreenModeAttributes(iSizeMode);
			TheClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetAppScreenMode(mode);
			iQueueClient->iScreen->SetScreenMode(mode);
			break;
			}
		case 3:
			{
			iQueueClient->iWs.SetAutoFlush(ETrue);
			TInt theCurMode = iQueueClient->iScreen->CurrentScreenMode();
			TPoint theScreenOrigin;
			if (iNumScrModes>2)
				{
				TheClient->iScreen->SetAppScreenMode(2);
				iQueueClient->iScreen->SetAppScreenMode(2);
				TheClient->iScreen->SetScreenMode(2);

				theScreenOrigin=iQueueClient->iScreen->GetDefaultScreenModeOrigin();
				SimulatePointer(TRawEvent::EButton1Down,
					30+theScreenOrigin.iX,30+theScreenOrigin.iY);
				SimulatePointer(TRawEvent::EButton1Up,
					26+theScreenOrigin.iX,20+theScreenOrigin.iY);
				SimulatePointer(TRawEvent::EButton1Down,
					40+theScreenOrigin.iX,60+theScreenOrigin.iY);
				SimulatePointer(TRawEvent::EButton1Up,
					26+theScreenOrigin.iX,20+theScreenOrigin.iY);

				AddExpectedPointer(TPointerEvent::EButton1Down,
					TPoint(15-EWinPositionX,15-EWinPositionY));
				AddExpectedPointer(TPointerEvent::EButton1Up,
					TPoint(13-EWinPositionX,10-EWinPositionY));
				AddExpectedPointer(TPointerEvent::EButton1Down,
					TPoint(20-EWinPositionX,30-EWinPositionY));
				AddExpectedPointer(TPointerEvent::EButton1Up,
					TPoint(13-EWinPositionX,10-EWinPositionY));
				}

			TheClient->iScreen->SetAppScreenMode(0);
			iQueueClient->iScreen->SetAppScreenMode(0);
			iQueueClient->iScreen->SetScreenMode(0);
			iTest->DelayIfFullRomL();

			theScreenOrigin = TPoint(iQueueClient->iScreen->GetDefaultScreenModeOrigin());

			SimulatePointer(TRawEvent::EButton1Down,
				30+theScreenOrigin.iX, 30+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Down,
				40+theScreenOrigin.iX, 50+theScreenOrigin.iY);
			SimulatePointer(TRawEvent::EButton1Up,
				26+theScreenOrigin.iX, 20+theScreenOrigin.iY);

		#if defined(__WINS__)
			iQueueClient->iWs.SimulateXyInputType(EXYInputDeltaMouse);
			iQueueClient->iWs.SetPointerCursorPosition(TPoint(24+theScreenOrigin.iX,24+theScreenOrigin.iY));
			SimulatePointer(TRawEvent::EPointerMove,1,2);
			iQueueClient->iWs.SimulateXyInputType(EXYInputPointer);
		#endif
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(30-EWinPositionX,30-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(26-EWinPositionX,20-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(40-EWinPositionX,50-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(26-EWinPositionX,20-EWinPositionY));

		#if defined(__WINS__)
			AddExpectedPointer(TPointerEvent::EMove, TPoint(25-EWinPositionX,26-EWinPositionY));
		#endif

			iQueueClient->iScreen->SetAppScreenMode(theCurMode);
			TheClient->iScreen->SetScreenMode(theCurMode);
			break;
			}
		default:
			User::After(EPointerCursorUpdateGap);
			iBlankWin->SetVisible(EFalse);
			TheClient->iWs.SetAutoFlush(iTheClientFlush);
			iQueueClient->iWs.SetAutoFlush(iQuequeClientFlush);
			iQueueClient->iScreen->SetCurrentScreenModeAttributes(iSizeMode);
			User::After(500000);		//0.5secs, to allow other screen devices to respond to the flip events
			CActiveScheduler::Stop();
			break;
		}
	}

void CTEventTest::AnimEvent_NextSetOfEventsL()
	{
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KSet,"AnimEvent SetOfEvents: %d  (Max=4)");
	logMessageText.Format(KSet,iEventSet);
	INFO_PRINTF1(logMessageText);
#endif
	switch(iEventSet++)
		{
		case 0:
			iAnim=REventAnim::NewL(iQueueClient->iWin->BaseWin(), iAnimDll);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedKeyAnim(TRawEvent::EKeyDown,32,ETrue);
			AddExpectedKeyAnim(TRawEvent::EKeyUp,32,ETrue);
			AddExpectedKeyAnim(TRawEvent::EKeyDown,33,EFalse);
			AddExpectedKeyAnim(TRawEvent::EKeyUp,33,EFalse);
			iTest->SimulateKey(TRawEvent::EKeyDown,32);
			iTest->SimulateKey(TRawEvent::EKeyUp,32);
			iTest->SimulateKey(TRawEvent::EKeyDown,33);
			iTest->SimulateKey(TRawEvent::EKeyUp,33);
			TheClient->iWs.Flush();
			if (iAnim->TestFail(4))
				Failed();
			break;
		case 1:
			iQueueClient->iWin->BaseWin()->PointerFilter(EPointerFilterMove|EPointerFilterDrag,0);
			iQueueClient->iWin->BaseWin()->SetPointerGrab(EFalse);
			iQueueClient->iWs.Flush();
			TheClient->iWs.SetPointerCursorArea(TRect(10,10,51,51));
			AddExpectedPointerAnim(TRawEvent::EButton1Down, TPoint(10,10));
			AddExpectedPointerAnim(TRawEvent::EButton1Up, TPoint(25,10));
			AddExpectedPointerAnim(TRawEvent::EButton1Down, TPoint(50,10));
			AddExpectedPointerAnim(TRawEvent::EButton1Up, TPoint(50,35));
			AddExpectedPointerAnim(TRawEvent::EButton1Down, TPoint(50,50));
			AddExpectedPointerAnim(TRawEvent::EButton1Up, TPoint(25,50));
			AddExpectedPointerAnim(TRawEvent::EButton1Down, TPoint(10,50));
			AddExpectedPointerAnim(TRawEvent::EButton1Up, TPoint(10,35));
			AddExpectedPointerAnim(TRawEvent::EButton1Down, TPoint(30,30),EFalse);
			AddExpectedPointerAnim(TRawEvent::EButton1Up, TPoint(30,30),EFalse);
			SimulatePointer(TRawEvent::EButton1Down,5,5);
			SimulatePointer(TRawEvent::EButton1Up,25,5);
			SimulatePointer(TRawEvent::EButton1Down,55,5);
			SimulatePointer(TRawEvent::EButton1Up,55,35);
			SimulatePointer(TRawEvent::EButton1Down,55,55);
			SimulatePointer(TRawEvent::EButton1Up,25,55);
			SimulatePointer(TRawEvent::EButton1Down,5,55);
			SimulatePointer(TRawEvent::EButton1Up,5,35);
			SimulatePointer(TRawEvent::EButton1Down,30,30);
			SimulatePointer(TRawEvent::EButton1Up,30,30);
			TheClient->iWs.Flush();
			if (iAnim->TestFail(10))
				Failed();
			break;
		case 2:
			{
		#if defined(LOGGING)
			_LIT(KLog," :2nd Set of Events  Modes=0x%x");
			logMessageText.Format(KLog,iScreenModeTests);
			INFO_PRINTF1(logMessageText);
		#endif
			TInt enforc=TheClient->iScreen->ScreenModeEnforcement();
			iBlankWin->SetVisible(ETrue);
			TInt expectedEvents=0;
			if (iScreenModeTests&EDoRot90 && enforc==ESizeEnforcementNone)
				{
				INFO_PRINTF1(_L(" :Rotation 90"));
				TheClient->iScreen->SetScreenMode(1);
				TheClient->iWs.Flush();
				iTest->DelayIfFullRomL();
                AddExpectedPointerAnim(TRawEvent::EButton1Down, TPoint(20,20-iYOffset));
                AddExpectedPointerAnim(TRawEvent::EButton1Up, TPoint(20,20-iYOffset));
				expectedEvents+=2;
				TPoint screenModeOrigin=TheClient->iScreen->GetDefaultScreenModeOrigin();
			#if defined(__WINS__)
				iTest->SimulatePointer(TRawEvent::EButton1Down,20+screenModeOrigin.iX,20+screenModeOrigin.iY);
				iTest->SimulatePointer(TRawEvent::EButton1Up,20+screenModeOrigin.iX,20+screenModeOrigin.iY);
			#else
				// simulate pointer at (20,20) on rotated screen by providing physical coordinates
				iTest->SimulatePointer(TRawEvent::EButton1Down,iPhysicalScreenSize.iWidth-1-20+screenModeOrigin.iX,20+screenModeOrigin.iY);
				iTest->SimulatePointer(TRawEvent::EButton1Up,iPhysicalScreenSize.iWidth-1-20+screenModeOrigin.iX,20+screenModeOrigin.iY);
			#endif
				}
			if (iScreenModeTests&EDoRot180)
				{
				INFO_PRINTF1(_L(" :Rotation 180"));
				TheClient->iScreen->SetCurrentRotations(0,CFbsBitGc::EGraphicsOrientationRotated180);
				TheClient->iScreen->SetScreenMode(0);
				TheClient->iWs.Flush();
				iTest->DelayIfFullRomL();
                AddExpectedPointerAnim(TRawEvent::EButton1Down, TPoint(22,22-iYOffset));
                AddExpectedPointerAnim(TRawEvent::EButton1Up, TPoint(22,22-iYOffset));
               
				expectedEvents+=2;
			#if defined(__WINS__)
				iTest->SimulatePointer(TRawEvent::EButton1Down,22,22);
				iTest->SimulatePointer(TRawEvent::EButton1Up,22,22);
			#else
				// simulate pointer at (22,22) on rotated screen by providing physical coordinates
				iTest->SimulatePointer(TRawEvent::EButton1Down,iPhysicalScreenSize.iWidth-1-22,iPhysicalScreenSize.iHeight-1-22);
				iTest->SimulatePointer(TRawEvent::EButton1Up,iPhysicalScreenSize.iWidth-1-22,iPhysicalScreenSize.iHeight-1-22);
			#endif
				}
			TheClient->iScreen->SetCurrentRotations(0,CFbsBitGc::EGraphicsOrientationNormal);
			TheClient->iScreen->SetScreenMode(0);
			TheClient->iWs.Flush();
			iTest->DelayIfFullRomL();
			if (iScreenModeTests&EDoRot0)
				{
				INFO_PRINTF1(_L(" :Rotation 0"));
				AddExpectedPointerAnim(TRawEvent::EButton1Down, TPoint(24,24),EFalse);
				AddExpectedPointerAnim(TRawEvent::EButton1Up, TPoint(24,24),EFalse);
				expectedEvents+=2;
				SimulatePointer(TRawEvent::EButton1Down,24,24);
				SimulatePointer(TRawEvent::EButton1Up,24,24);
				}
		#if defined(LOGGING)
			_LIT(KNumEvents," :Expected Number of Events=%d  Actual=%d");
			logMessageText.Format(KNumEvents,expectedEvents,iAnim->GetNumEvents());
			INFO_PRINTF1(logMessageText);
			_LIT(KError," :Error Value %d ('0'=OK)");
			logMessageText.Format(KError,iAnim->GetError());
			INFO_PRINTF1(logMessageText);
		#endif
			if (iAnim->TestFail(expectedEvents))
				Failed();
			User::After(EPointerCursorUpdateGap);
			iBlankWin->SetVisible(EFalse);
			INFO_PRINTF1(_L(" :Finished 2nd Set"));
			}
			break;
		case 3:
			iAnim->Destroy(); iAnim=NULL;
			// Create an EventPostingAnim, key down & key up events are simulated.
			// The event posting anim sees the key events in turn, and creates duplicates.
			// Test checks that each simulated event is received twice. 
			TRAPD(err, iAnim=REventAnim::NewL(iQueueClient->iWin->BaseWin(), iAnimDll, REventAnimBase::EEventPostingAnim));
			if (err!=KErrNone)
				{
				Failed();
				return;
				}
			AddExpectedKeyAnim(TRawEvent::EKeyDown,32,ETrue);
			AddExpectedKeyAnim(TRawEvent::EKeyDown,32,EFalse);
			AddExpectedKeyAnim(TRawEvent::EKeyUp,32,ETrue);
			AddExpectedKeyAnim(TRawEvent::EKeyUp,32,EFalse);
			
			iTest->SimulateKey(TRawEvent::EKeyDown,32);
			iTest->SimulateKey(TRawEvent::EKeyUp,32);
			break;
		case 4:
			// Simulate a key down event, which calls anim's offer raw event
			// which inturn calls post key event with old and new API
			// Test checks the repeat count for old and new API are as expected.
			AddExpectedKeyAnim(TRawEvent::EKeyDown,45,ETrue);
			
			TKeyEvent event;
			event.iCode='J';
			event.iScanCode=0;
			event.iModifiers=0;
			event.iRepeats=0;
			AddExpectedKey(1, event.iScanCode, event.iCode, event.iRepeats, event.iModifiers);
			event.iRepeats=2;
			AddExpectedKey(1, event.iScanCode, event.iCode, event.iRepeats, event.iModifiers);
			
			iTest->SimulateKey(TRawEvent::EKeyDown,45);
			break;
		default:
			iAnim->Destroy();
			iAnim=NULL;
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

void CTEventTest::FocusChanged_NextSetOfEventsL()
	{
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KSet,"FocusChanged SetOfEvents: %d ");
	logMessageText.Format(KSet,iEventSet);
	INFO_PRINTF1(logMessageText);
#endif
	switch(iEventSet++)
		{
		case 0:
			{
			_LIT(AutoFocusTest,"FocusEvents");
			iQueueClient->EnableFocusChangeEvents();
			iEventTestGroup2=new(ELeave) CTWindowGroup(TheClient);
			iEventTestGroup2->ConstructL();
			iEventTestGroup2->GroupWin()->SetName(AutoFocusTest);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedEvent(EEventFocusLost);
			AddExpectedEvent(EEventFocusGroupChanged);
			AddExpectedEvent(EEventWindowGroupsChanged);
			}
			break;
		case 1:
			iEventTestGroup2->GroupWin()->EnableReceiptOfFocus(EFalse);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedEvent(EEventFocusGroupChanged);
			break;
		case 2:
			iEventTestGroup2->GroupWin()->EnableReceiptOfFocus(ETrue);
			AddExpectedEvent(EEventFocusLost);
			AddExpectedEvent(EEventFocusGroupChanged);
			break;
		case 3:
			iEventTestGroup2->GroupWin()->SetOrdinalPosition(999);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedEvent(EEventFocusGroupChanged);
			break;
		case 4:
			iEventTestGroup2->GroupWin()->SetOrdinalPosition(0);
			AddExpectedEvent(EEventFocusLost);
			AddExpectedEvent(EEventFocusGroupChanged);
			break;
		case 5:
			delete iEventTestGroup2;
			iEventTestGroup2=NULL;
			AddExpectedEvent(EEventWindowGroupsChanged);
			AddExpectedEvent(EEventFocusGroupChanged);
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

	
void CTEventTest::StopEvents_NextSetOfEventsL()
	{
	// we are not running tests 0,1,2 as they currently fail
	if(iEventSet==0)
		{
		iEventSet=3;
		AddExpectedEvent(EEventFocusGained);
		}
	TInt retVal	= KErrNone;
				
	switch(iEventSet++)
		{
		case 0: // doesn't work if shell gets the off events - will fail on User::LeaveIfError(....
			iQueueClient->iWs.Flush();
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iGroup->WinTreeNode()));
			iTest->SimulateEvent(TRawEvent::ESwitchOff);
			TheClient->iWs.Flush();
			retVal = iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iWin->WinTreeNode());
			TEST(retVal==KErrAlreadyExists);
			if (retVal!=KErrAlreadyExists)
				INFO_PRINTF3(_L("iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iWin->WinTreeNode()) return value - Expected: %d, Actual: %d"), KErrAlreadyExists, retVal);
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(EFalse,iQueueClient->iGroup->WinTreeNode()));
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iGroup->WinTreeNode()));
			iTest->SimulateEvent(TRawEvent::ESwitchOff);
			TheClient->iWs.Flush();
			retVal = iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iGroup->WinTreeNode());
			TEST(retVal==KErrAlreadyExists);
			if (retVal!=KErrAlreadyExists)
				INFO_PRINTF3(_L("iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iGroup->WinTreeNode()) return value - Expected: %d, Actual: %d"), KErrAlreadyExists, retVal);
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(EFalse));
			AddExpectedEvent(EEventFocusGained);
			AddExpectedEvent(EEventSwitchOff);
			AddExpectedEvent(EEventSwitchOff);
			break;
			
		case 1: // doesn't work if shell gets the off events - will fail on User::LeaveIfError(....
			{
			TKeyEvent event; 
			iQueueClient->iWs.Flush();
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iGroup->WinTreeNode()));
			event.iCode=EKeyOff;
			event.iScanCode=EKeyOff;
			event.iModifiers=0;
			event.iRepeats=0;
			TheClient->iWs.SimulateKeyEvent(event);
			TheClient->iWs.Flush();
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(EFalse));
			AddExpectedEvent(EEventKeySwitchOff);
			}
			break;
		
		case 2: // doesn't work if shell gets the off events - will fail on User::LeaveIfError(....
			{
			TKeyEvent event;
			event.iCode=EKeyOff;
			event.iScanCode=EKeyOff;
			event.iModifiers=0;
			event.iRepeats=0;
			iQueueClient->iWs.Flush();
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iGroup->WinTreeNode()));
			iTest->SimulateEvent(TRawEvent::ESwitchOff);
			TheClient->iWs.SimulateKeyEvent(event);
			iTest->SimulateEvent(TRawEvent::ECaseClose);
			TheClient->iWs.Flush();
			retVal = iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iWin->WinTreeNode());
			TEST(retVal==KErrAlreadyExists);
			if (retVal!=KErrAlreadyExists)
				INFO_PRINTF3(_L("iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iWin->WinTreeNode()) return value - Expected: %d, Actual: %d"), KErrAlreadyExists, retVal);
			iTest->SimulateEvent(TRawEvent::ESwitchOff);
			iTest->SimulateEvent(TRawEvent::ECaseClose);
			TheClient->iWs.SimulateKeyEvent(event);
			TheClient->iWs.Flush();
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(EFalse,iQueueClient->iGroup->WinTreeNode()));
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iWin->WinTreeNode()));
			iTest->SimulateEvent(TRawEvent::ECaseClose);
			iTest->SimulateEvent(TRawEvent::ESwitchOff);
			TheClient->iWs.SimulateKeyEvent(event);
			TheClient->iWs.Flush();
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(EFalse));
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(ETrue,iQueueClient->iChildWin->WinTreeNode()));
			iTest->SimulateEvent(TRawEvent::ESwitchOff);
			TheClient->iWs.SimulateKeyEvent(event);
			iTest->SimulateEvent(TRawEvent::ECaseClose);
			TheClient->iWs.Flush();
			User::LeaveIfError(iQueueClient->iWs.RequestOffEvents(EFalse));
			AddExpectedEvent(EEventSwitchOff);
			AddExpectedEvent(EEventKeySwitchOff);
			AddExpectedEvent(EEventCaseClosed);
			AddExpectedEvent(EEventSwitchOff);
			AddExpectedEvent(EEventCaseClosed);
			AddExpectedEvent(EEventKeySwitchOff);
			AddExpectedEvent(EEventCaseClosed,iQueueClient->iWin);
			AddExpectedEvent(EEventSwitchOff,iQueueClient->iWin);
			AddExpectedEvent(EEventKeySwitchOff,iQueueClient->iWin);
			AddExpectedEvent(EEventSwitchOff,iQueueClient->iChildWin);
			AddExpectedEvent(EEventKeySwitchOff,iQueueClient->iChildWin);
			AddExpectedEvent(EEventCaseClosed,iQueueClient->iChildWin);
			}
			break;
			
		case 3: 
			iQueueClient->iWs.Flush();
			//User::LeaveIfError(iQueueClient->iGroup->WinTreeNode()->EnableOnEvents(EEventControlAlways));
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			iTest->SimulateEvent(TRawEvent::ECaseOpen);
			TheClient->iWs.Flush();
			User::LeaveIfError(iQueueClient->iWin->WinTreeNode()->EnableOnEvents(EEventControlAlways));
			iTest->SimulateEvent(TRawEvent::ECaseOpen);
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			TheClient->iWs.Flush();
			iQueueClient->iWin->WinTreeNode()->DisableOnEvents();
			AddExpectedEvent(EEventSwitchOn);
			//AddExpectedEvent(EEventSwitchOn);
			AddExpectedEvent(EEventCaseOpened);
			AddExpectedEvent(EEventCaseOpened,iQueueClient->iWin);
			AddExpectedEvent(EEventCaseOpened);
			AddExpectedEvent(EEventSwitchOn,iQueueClient->iWin);
			AddExpectedEvent(EEventSwitchOn);
			break;
			
		case 4:
			iQueueClient->iWs.Flush();
			// switch off the device
			iTest->SimulateEvent(TRawEvent::ESwitchOff);
			// switch it back on. 
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			TheClient->iWs.Flush();	
			// TRawEvent::ESwitchOff never actually comes back. The tests will hang if we look for one
			AddExpectedEvent(EEventSwitchOn);
			break;
			
		case 5:
			{
			TKeyEvent event; 
			iQueueClient->iWs.Flush();
			event.iCode=EKeyOff;
			event.iScanCode=EKeyOff;
			event.iModifiers=0;
			event.iRepeats=0;
			// switch off the device - this one seems to send it to standby rather than off
			TheClient->iWs.SimulateKeyEvent(event);
			// switch it back on or no further tests will complete
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			TheClient->iWs.Flush();	
			// again we would expect EEventKeySwitchOff but it doesn't come back. The tests will hang if we look for one
			AddExpectedEvent(EEventSwitchOn);
			}
			break;
		
		case 6:
			iQueueClient->iWs.Flush();
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			iTest->SimulateEvent(TRawEvent::ECaseOpen);
			TheClient->iWs.Flush();
			User::LeaveIfError(iQueueClient->iWin->WinTreeNode()->EnableOnEvents(EEventControlAlways));
			iTest->SimulateEvent(TRawEvent::ECaseOpen);
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			TheClient->iWs.Flush();
			iQueueClient->iWin->WinTreeNode()->DisableOnEvents();
			AddExpectedEvent(EEventSwitchOn);
			AddExpectedEvent(EEventCaseOpened);
			AddExpectedEvent(EEventCaseOpened,iQueueClient->iWin);
			AddExpectedEvent(EEventCaseOpened);
			AddExpectedEvent(EEventSwitchOn,iQueueClient->iWin);
			AddExpectedEvent(EEventSwitchOn);
			break;
					
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}
	
void CTEventTest::VirtualKeyboard_NextSetOfEventsL()
	{
	const TInt noVirtualKeys=10;
	const TInt keyWidth=2*EWinWidth/noVirtualKeys/3;
	const TInt keyHeight=EWinHeight/2;
	const TInt keySpacing=EWinWidth/noVirtualKeys;
	const TInt baseChar='1';
	TInt startX=1;
	TInt ii;
	switch(iEventSet++)
		{
		case 0:
			iQueueClient->iWin->BaseWin()->SetPointerGrab(EFalse);
			for (ii=0;ii<noVirtualKeys;++ii)
				{
				iQueueClient->iWin->BaseWin()->AddKeyRect(TRect(startX,1,startX+keyWidth,keyHeight),baseChar+ii,EFalse);
				startX+=keySpacing;
				}
			iQueueClient->iWs.Flush();
			iTest->SimulatePointerDownUp(EWinPositionX+1,EWinPositionY+1+iYOffset);
			//TheClient->iWs.Flush();
			AddExpectedEvent(EEventFocusGained);
			AddExpectedKeyDownUp(baseChar);
			break;
		case 1:
			startX=3;
			for (ii=0;ii<noVirtualKeys-2;++ii)		//Can't do all 10 as this will overflow the event buffer
				{
				iTest->SimulatePointerDownUp(startX+EWinPositionX+1,EWinPositionY+keyHeight-1+iYOffset);
				AddExpectedKeyDownUp(baseChar+ii);
				startX+=keySpacing;
				}
			break;
		case 2:
			SimulatePointerDownUpWin(keySpacing+1,1);
			//SimulatePointerDownUp(EWinPositionX+keySpacing+1,EWinPositionY+1);
			iTest->SimulatePointerDownUp(EWinPositionX+keyWidth,EWinPositionY+1+iYOffset);
			iTest->SimulatePointerDownUp(EWinPositionX+1,EWinPositionY+keyHeight-1+iYOffset);
			iTest->SimulatePointerDownUp(EWinPositionX+keyWidth,EWinPositionY+keyHeight-1+iYOffset);
			AddExpectedKeyDownUp(baseChar+1);
			AddExpectedKeyDownUp(baseChar);
			AddExpectedKeyDownUp(baseChar);
			AddExpectedKeyDownUp(baseChar);
			break;
		case 3:
			SimulatePointerDownUpWin(keySpacing+3,4);
			SimulatePointerDownUpWin(0,0);		//No Pointer event get through even if there are gaps
			SimulatePointerDownUpWin(1,0);
			SimulatePointerDownUpWin(0,1);
			SimulatePointerDownUpWin(keyWidth+1,0);
			SimulatePointerDownUpWin(keyWidth,0);
			SimulatePointerDownUpWin(keyWidth+1,1);
			SimulatePointerDownUpWin(0,keyHeight);
			SimulatePointerDownUpWin(1,keyHeight);
			SimulatePointerDownUpWin(0,keyHeight-1);
			SimulatePointerDownUpWin(keyWidth+1,keyHeight);
			SimulatePointerDownUpWin(keyWidth,keyHeight);
			SimulatePointerDownUpWin(keyWidth+1,keyHeight-1);
			SimulatePointerDownUpWin(2*keySpacing+3,4);
			AddExpectedKeyDownUp(baseChar+1);
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+0,EWinPositionY+0));		//No Pointer event get through even if there are gaps
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+1,EWinPositionY+0));
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+0,EWinPositionY+1));
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+keyWidth+1,EWinPositionY+0));
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+keyWidth,EWinPositionY+0));
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+keyWidth+1,EWinPositionY+1));
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+0,EWinPositionY+keyHeight));
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+1,EWinPositionY+keyHeight));
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+0,EWinPositionY+keyHeight-1));
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+keyWidth+1,EWinPositionY+keyHeight));
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+keyWidth,EWinPositionY+keyHeight));
			AddExpectedPointerDownUpScr(TPoint(EWinPositionX+keyWidth+1,EWinPositionY+keyHeight-1));
			AddExpectedKeyDownUp(baseChar+2);
			break;
		case 4:
			SimulatePointerWin(TRawEvent::EButton1Down,keySpacing+2,6);
			SimulatePointerWin(TRawEvent::EButton1Up,2*keySpacing+1,5);
			SimulatePointerWin(TRawEvent::EButton1Down,2*keySpacing+2,7);
			SimulatePointer(TRawEvent::EButton1Up,0,0);
			AddExpectedKeyDownUp(baseChar+1);
			AddExpectedKeyDownUp(baseChar+2);
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

TInt CTEventTest::ClickCommandReply(TInt aCommand,TInt aData)
	{
	TPckgBuf<TInt> intPkg;
	intPkg()=aData;
	return iClick.CommandReply(aCommand,intPkg);
	}

TInt CTEventTest::CreateGroupWindow(RWindowGroup& aWinGp,TClients aClient,TInt& aWinGpId)
	{
	ClickCommandReply(EClickCreateGroupWin,aClient);
	TInt err=aWinGp.Construct(ENullWsHandle,EFalse);
	if (!err)
		{
		aWinGpId=aWinGp.Identifier();
		ClickCommandReply(EClickCheckGpWinId,aWinGpId);
		}
	return err;
	}

void CTEventTest::CloseGroupWindow(RWindowGroup& aWinGp,TInt aWinGpId)
	{
	ClickCommandReply(EClickCloseGroupWin,aWinGpId);
	aWinGp.Close();
	}

void CTEventTest::CloseWindow(RWindowBase& aWin,TInt aHandle,TInt aWinGpId)
	{
	TPckgBuf<TWindowCloseData> winClose;
	winClose().iClientHandle=aHandle;
	winClose().iWindowGroupId=aWinGpId;
	iClick.CommandReply(EClickCloseWin,winClose);
	aWin.Close();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0447

@SYMCR				CR1164

@SYMTestCaseDesc	Tests that Key Click Plug-in gets new event Information

@SYMTestPriority	High

@SYMTestStatus		Implemented

@SYMTestActions		Do things that cause the new events to be sent into plug-in.
Creating destroying windows, pointer events.  Provide test plug-in with information it needs to check events.

@SYMTestExpectedResults When Test Plug-in is querried to see if events match what was expected it says that they were.
*/
void CTEventTest::KeyClicks_NextSetOfEvents()
	{
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KSet,"KeyClicks SetOfEvents: %d (last=13)");
	logMessageText.Format(KSet,iEventSet);
	INFO_PRINTF1(logMessageText);
#endif
	TInt events=-1;
	switch(iEventSet++)
		{
		case 0:
			if (SetClickType(EClickCheck))
				return;
			iAddToClick=ETrue;
			iQueueClient->iWin->BaseWin()->SetPointerGrab(EFalse);
			iQueueClient->iChildWin->BaseWin()->SetPointerGrab(EFalse);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedKeyDownUp(EStdKeySpace,EKeySpace);
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp(EStdKeySpace);		//3 events
			events=3;
			break;
		case 1:
			AddExpectedKeyDownUp(EStdKeyEnter,EKeyEnter);
			AddExpectedKeyDownUp('0');
			AddExpectedKeyDownUp('A','a');
			AddExpectedKeyDownUp('X','x');
			AddExpectedKey(EEventKeyDown,EStdKeyLeftShift,0,0,EModifierLeftShift|EModifierShift);
			AddExpectedKey(EEventKeyUp,EStdKeyLeftShift);
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp(EStdKeyEnter);		//3 events
			iTest->SimulateKeyDownUp('0');		//3 events
			iTest->SimulateKeyDownUp('A');		//3 events
			iTest->SimulateKeyDownUp('X');		//3 events
			iTest->SimulateKeyDownUp(EStdKeyLeftShift);		//2 events
			events=14;
			break;
		case 2:
			AddExpectedKey(EEventKeyDown,EStdKeyRightShift,0,0,EModifierRightShift|EModifierShift);
			AddExpectedKeyDownUp('B','B',0,EModifierRightShift|EModifierShift|EModifierAutorepeatable);
			AddExpectedKeyDownUp('W','W',0,EModifierRightShift|EModifierShift|EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp,EStdKeyRightShift);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyRightShift);		//1 event
			iTest->SimulateKeyDownUp('B');		//3 events
			iTest->SimulateKeyDownUp('W');		//3 events
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyRightShift);		//1 event
			events=8;
			break;
		case 3:
			AddExpectedKey(EEventKeyDown,'C');
			AddExpectedKey(EEventKey,'C','c');
			AddExpectedKey(EEventKeyDown,'V');
			AddExpectedKey(EEventKey,'V','v');
			AddExpectedKey(EEventKeyUp,'C');
			AddExpectedKey(EEventKeyUp,'V');
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'C');		//2 events
			iTest->SimulateKey(TRawEvent::EKeyDown,'V');		//2 events
			iTest->SimulateKey(TRawEvent::EKeyUp,'C');		//1 event
			iTest->SimulateKey(TRawEvent::EKeyUp,'V');		//1 event
			events=6;
			break;
		case 4:
			{
			AddExpectedKey(EEventKeyDown,'D');
			AddExpectedKey(EEventKey,'D','d');
			AddExpectedKey(EEventKey,'D','d',1);
			AddExpectedKey(EEventKeyUp,'D');
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'D');		//2 events
			TTimeIntervalMicroSeconds32 initialTime,time;
			TheClient->iWs.GetKeyboardRepeatRate(initialTime,time);
			User::After(initialTime.Int()+time.Int()/4);		//1 event
			iTest->SimulateKey(TRawEvent::EKeyUp,'D');		//1 event
			events=4;
			}
			break;
		case 5:
			AddExpectedPointerDownUpScr(TPoint(40,25));
			AddExpectedPointerDownUpScr(TPoint(95,30));
			AddExpectedPointer(TPointerEvent::EButton1Down,TPoint(20-EWinPositionX,50-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag,TPoint(45-EWinPositionX,42-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up,TPoint(45-EWinPositionX,42-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down,TPoint(98-EWinPositionX,15-EWinPositionY));
			AddExpectedPointerClick(TPointerEvent::EDrag,TPoint(68-EWinPositionX,39-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EDrag,TPoint(41-EWinPositionX,42-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up,TPoint(50-EWinPositionX,58-EWinPositionY));
			iQueueClient->iWs.Flush();
			iTest->SimulatePointerDownUp(40,25+iYOffset);		//2 events
			iTest->SimulatePointerDownUp(95,30+iYOffset);		//2 events
			SimulatePointer(TRawEvent::EButton1Down,20,50);		//1 event
			SimulatePointer(TRawEvent::EPointerMove,45,42);		//1 event
			SimulatePointer(TRawEvent::EButton1Up,45,42);		//1 event
			SimulatePointer(TRawEvent::EButton1Down,98,15);		//1 event
			SimulatePointer(TRawEvent::EPointerMove,68,39);		//1 event
			SimulatePointer(TRawEvent::EPointerMove,41,42);		//1 event
			SimulatePointer(TRawEvent::EButton1Up,50,58);		//1 event
			events=11;
			break;
		case 6:
			AddExpectedKey(EEventKeyDown,'E');
			AddExpectedKey(EEventKey,'E','e');
			AddExpectedPointerDownUpScr(TPoint(41,24));
			AddExpectedKey(EEventKeyUp,'E');
			AddExpectedPointer(TPointerEvent::EButton1Down,TPoint(23-EWinPositionX,52-EWinPositionY));
			AddExpectedKeyDownUp('1');
			AddExpectedPointer(TPointerEvent::EButton1Up,TPoint(44-EWinPositionX,45-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Down,TPoint(33-EWinPositionX,53-EWinPositionY));
			AddExpectedKey(EEventKeyDown,'F');
			AddExpectedKey(EEventKey,'F','f');
			AddExpectedPointer(TPointerEvent::EButton1Up,TPoint(34-EWinPositionX,47-EWinPositionY));
			AddExpectedKey(EEventKeyUp,'F');
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'E');		//2 events
			iTest->SimulatePointerDownUp(41,24+iYOffset);		//2 events
			iTest->SimulateKey(TRawEvent::EKeyUp,'E');		//1 event
			SimulatePointer(TRawEvent::EButton1Down,23,52);		//1 event
			iTest->SimulateKeyDownUp('1');		//3 events
			SimulatePointer(TRawEvent::EButton1Up,44,45);		//1 event
			SimulatePointer(TRawEvent::EButton1Down,33,53);		//1 event
			iTest->SimulateKey(TRawEvent::EKeyDown,'F');		//2 events
			SimulatePointer(TRawEvent::EButton1Up,34,47);		//1 event
			iTest->SimulateKey(TRawEvent::EKeyUp,'F');		//1 event
			events=15;
			break;
		case 7:
			AddExpectedKeyDownUp('2');
			iAddToClick=EFalse;
			AddExpectedKeyDownUp('G','g');
			iAddToClick=ETrue;
			AddExpectedPointerDownUpScr(TPoint(42,26));
			AddExpectedKeyDownUp('U','u');
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp('2');		//3 events
			TheClient->iWs.Flush();
			iQueueClient->iGroup->GroupWin()->DisableKeyClick(ETrue);
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp('G');
			iTest->SimulatePointerDownUp(42,26+iYOffset);		//2 events
			TheClient->iWs.Flush();
			iQueueClient->iGroup->GroupWin()->DisableKeyClick(EFalse);
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp('U');		//3 events
			events=8;
			break;
		case 8:
			{
			TPckgBuf<TPointerEventInfo> pointerEventInfo;
			pointerEventInfo().iClientHandle=reinterpret_cast<TInt>(iQueueClient->iChildWin);
			pointerEventInfo().iWinGpId=iQueueClient->iGroup->GroupWin()->Identifier();
			pointerEventInfo().iWinOrigin=iQueueClient->ChildTopLeft();
			pointerEventInfo().iParentOrigin.SetXY(EWinPositionX,EWinPositionY);
			iClick.CommandReply(EClickPointerEvent,pointerEventInfo);
			AddExpectedKeyDownUp('3');
			AddExpectedPointerDownUpScr(TPoint(43,26));
			AddExpectedKeyDownUp('H','h');
			iAddToClick=EFalse;
			AddExpectedPointerDownUpScr(TPoint(43,27));
			AddExpectedKeyDownUp('4');
			AddExpectedPointerDownUpScr(TPoint(42,27));
			AddExpectedKeyDownUp('I','i');
			iAddToClick=ETrue;
			AddExpectedPointerDownUpScr(TPoint(44,27));
			AddExpectedKeyDownUp('5');
			AddExpectedPointerDownUpScr(TPoint(44,26));
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp('3');		//3 events
			iTest->SimulatePointerDownUp(43,26+iYOffset);		//2 events
			iClick.SetPenClick(EFalse);
			iTest->SimulateKeyDownUp('H');		//3 events
			iTest->SimulatePointerDownUp(43,27+iYOffset);
			iClick.SetKeyClick(EFalse);
			iTest->SimulateKeyDownUp('4');
			iTest->SimulatePointerDownUp(42,27+iYOffset);
			iClick.SetPenClick(ETrue);
			iTest->SimulateKeyDownUp('I');
			iTest->SimulatePointerDownUp(44,27+iYOffset);		//2 events
			iClick.SetKeyClick(ETrue);
			iTest->SimulateKeyDownUp('5');		//3 events
			iTest->SimulatePointerDownUp(44,26+iYOffset);		//2 events
			pointerEventInfo().iClientHandle=0;
			pointerEventInfo().iWinGpId=0;
			iClick.CommandReply(EClickPointerEvent,pointerEventInfo);
			events=15;
			}
			break;
		case 9:
			{
			RWindowGroup winGp(iQueueClient->iWs);
			TInt winGpId;
			TInt ii;
			AddExpectedEvent(EEventWindowGroupsChanged);
			for (ii=0;ii<2;++ii)	//events x2
				{
				if (CreateGroupWindow(winGp,EQueueClient,winGpId))		//1 event
					Failed();
				CloseGroupWindow(winGp,winGpId);		//1 event
				iQueueClient->iWs.Flush();
				}
			events=4;
			}
			break;
		case 10:
			{
			RWindowGroup winGp(TheClient->iWs);
			RWindowGroup winGp1(iQueueClient->iWs);
			RWindowGroup winGp2(iQueueClient->iWs);
			TInt winGpId;
			TInt winGpId1;
			TInt winGpId2;
			AddExpectedEvent(EEventWindowGroupsChanged);
			if (CreateGroupWindow(winGp,ETheClient,winGpId))		//1 event
				Failed();
			CloseGroupWindow(winGp,winGpId);		//1 event
			TheClient->iWs.Flush();
			if (CreateGroupWindow(winGp1,EQueueClient,winGpId1))		//1 event
				Failed();
			if (CreateGroupWindow(winGp2,EQueueClient,winGpId2))		//1 event
				Failed();
			if (CreateGroupWindow(winGp,ETheClient,winGpId))		//1 event
				Failed();
			CloseGroupWindow(winGp2,winGpId2);		//1 event
			iQueueClient->iWs.Flush();
			CloseGroupWindow(winGp,winGpId);		//1 event
			TheClient->iWs.Flush();
			CloseGroupWindow(winGp1,winGpId1);		//1 event
			iQueueClient->iWs.Flush();
			events=8;
			}
			break;
		case 11:
			{
			RWindowGroup winGp(iQueueClient->iWs);
			TInt winGpId;
			AddExpectedEvent(EEventWindowGroupsChanged);
			if (CreateGroupWindow(winGp,EQueueClient,winGpId))		//1 event
				Failed();
			RWindow win(iQueueClient->iWs);
			const TInt handle=79;
			if (win.Construct(winGp,handle))
				Failed();
			win.Activate();
			CloseWindow(win,handle,winGpId);		//1 event
			iQueueClient->iWs.Flush();
			CloseGroupWindow(winGp,winGpId);		//1 event
			iQueueClient->iWs.Flush();
			events=3;
			}
			break;
		case 12:
			{
			RWindowGroup winGp1(iQueueClient->iWs);
			RWindowGroup winGp2(TheClient->iWs);
			TInt winGpId1;
			TInt winGpId2;
			AddExpectedEvent(EEventWindowGroupsChanged);
			if (CreateGroupWindow(winGp2,ETheClient,winGpId2))		//1 event
				Failed();
			if (CreateGroupWindow(winGp1,EQueueClient,winGpId1))		//1 event
				Failed();
			RWindow win1(iQueueClient->iWs);
			RWindow win2(iQueueClient->iWs);
			RWindow win(TheClient->iWs);
			const TInt handle1a=80;
			const TInt handle1b=81;
			const TInt handle2a=82;
			const TInt handle2b=83;
			const TInt handle=84;
			if (win1.Construct(winGp1,handle1a))
				Failed();
			win1.Activate();
			if (win2.Construct(win1,handle2a))
				Failed();
			win2.Activate();
			if (win.Construct(winGp2,handle))
				Failed();
			win.Activate();
			CloseWindow(win2,handle2a,winGpId1);		//1 event
			iQueueClient->iWs.Flush();
			if (win2.Construct(winGp1,handle2b))
				Failed();
			win2.Activate();
			CloseWindow(win1,handle1a,winGpId1);		//1 event
			iQueueClient->iWs.Flush();
			CloseWindow(win,handle,winGpId2);		//1 event
			TheClient->iWs.Flush();
			if (win1.Construct(win2,handle1b))
				Failed();
			win1.Activate();
			CloseWindow(win1,handle1b,winGpId1);		//1 event
			iQueueClient->iWs.Flush();
			CloseGroupWindow(winGp2,winGpId2);		//1 event
			TheClient->iWs.Flush();
			CloseWindow(win2,handle2b,winGpId1);		//1 event
			iQueueClient->iWs.Flush();
			CloseGroupWindow(winGp1,winGpId1);		//1 event
			iQueueClient->iWs.Flush();
			events=9;
			}
			break;
		case 13:
			{
			TPckgBuf<TPointerEventInfo> pointerEventInfo;
			pointerEventInfo().iClientHandle=reinterpret_cast<TInt>(iQueueClient->iWin);
			pointerEventInfo().iWinGpId=iQueueClient->iGroup->GroupWin()->Identifier();
			pointerEventInfo().iWinOrigin.SetXY(EWinPositionX,EWinPositionY);
			pointerEventInfo().iParentOrigin.SetXY(0,0);
			iClick.CommandReply(EClickPointerEvent,pointerEventInfo);
			AddExpectedPointerDownUpScr(TPoint(43,18));
			AddExpectedPointerDownUpScr(TPoint(43,17));
			AddExpectedPointerDownUpScr(TPoint(42,52));
			AddExpectedPointerDownUpScr(TPoint(24,27));
			AddExpectedPointerDownUpScr(TPoint(94,26));
			iQueueClient->iWs.Flush();
			iTest->SimulatePointerDownUp(43,18+iYOffset);		//2 events
			iTest->SimulatePointerDownUp(43,17+iYOffset);		//2 events
			iTest->SimulatePointerDownUp(42,52+iYOffset);		//2 events
			iTest->SimulatePointerDownUp(24,27+iYOffset);		//2 events
			iTest->SimulatePointerDownUp(94,26+iYOffset);		//2 events
			pointerEventInfo().iClientHandle=0;
			pointerEventInfo().iWinGpId=0;
			iClick.CommandReply(EClickPointerEvent,pointerEventInfo);
			events=10;
			}
			break;
		default:
			iAddToClick=EFalse;
			if (!SetClickType(EClickNone))
				CActiveScheduler::Stop();
			return;
		}
	TInt failedAt=iClick.CommandReply(EClickFailed,TPtrC8(NULL,0));
	TInt eventsDone=iClick.CommandReply(EClickEvents,TPtrC8(NULL,0));
	if (failedAt!=0 || eventsDone!=events)		//iEventSet (already incremented)
		{
	#if defined(LOGGING)
		_LIT(KeyClickFailed,"KeyClick Failed EventSet=%d FailedAt=%d TotalEvents=%d EventsDone=%d");
		logMessageText.Format(KeyClickFailed,iEventSet-1,failedAt,events,eventsDone);
		INFO_PRINTF1(logMessageText);
	#endif
		Failed();
		}
	iClick.CommandReply(EClickReset,TPtrC8(NULL,0));
	}

void CTEventTest::CaptureLong_NextSetOfEventsL()
	{
	#if defined(LOGGING)
		TLogMessageText logMessageText;
		_LIT(KSet,"CaptureLong SetOfEvents: %d of 23");
		logMessageText.Format(KSet,iEventSet);
		INFO_PRINTF1(logMessageText);
	#endif

	switch(iEventSet++)
		{
		case 0:
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp('A');
			AddExpectedEvent(EEventFocusGained);
			AddExpectedKeyDownUp('A','a');
			break;
		case 1:
			// Test CaptureLongKey() with modifier value EModifierShift and flag value ELongCaptureNormal
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureLongKey(' ','a',EModifierShift,EModifierShift,2,ELongCaptureNormal);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftShift);
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeySpace);
			TheClient->iWs.Flush();
			DelayForRepeatEvents(1, 3);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftShift);
			AddExpectedKey(EEventKeyDown,EStdKeyLeftShift);
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKey,EStdKeySpace,' ',0,EModifierLeftShift|EModifierShift|EModifierAutorepeatable);
			AddExpectedKey(EEventKey,EStdKeySpace,'a',1,EModifierLongKey|EModifierLeftShift|EModifierShift|EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			AddExpectedKey(EEventKeyUp,EStdKeyLeftShift);
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			break;
		case 2:	
			// Test CaptureLongKey() with modifier value EModifierFunc and flag value ELongCaptureWaitShort
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureLongKey(EKeyEnter,'a',EModifierFunc,EModifierFunc,2,ELongCaptureWaitShort);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftFunc);
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyEnter);
			TheClient->iWs.Flush();
			DelayForRepeatEvents(1, 3);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyEnter);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftFunc);
			iTest->SimulateKeyDownUp(EStdKeySpace);
			AddExpectedKey(EEventKeyDown,EStdKeyLeftFunc);
			AddExpectedKey(EEventKeyDown,EStdKeyEnter);
			AddExpectedKey(EEventKey,EStdKeyEnter,'a',1,EModifierLongKey|EModifierLeftFunc|EModifierFunc|EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp,EStdKeyEnter);
			AddExpectedKey(EEventKeyUp,EStdKeyLeftFunc);
			AddExpectedKeyDownUp(EStdKeySpace,' ');
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			break;
		case 3:
			// Test CaptureLongKey() with modifier values EModifierFunc|EModifierAlt and flag values ELongCaptureNormal|ELongCaptureRepeatEvents)
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureLongKey(EKeyEnter,'a',EModifierFunc|EModifierAlt,EModifierFunc|EModifierAlt,2,ELongCaptureNormal|ELongCaptureRepeatEvents);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftFunc);
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftAlt);
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeyEnter);
			TheClient->iWs.Flush();
			DelayForRepeatEvents(5, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyEnter);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftAlt);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftFunc);
			AddExpectedKey(EEventKeyDown,EStdKeyLeftFunc);
			AddExpectedKey(EEventKeyDown,EStdKeyLeftAlt);
			AddExpectedKey(EEventKeyDown,EStdKeyEnter);
			AddExpectedKey(EEventKey,EStdKeyEnter,EKeyEnter,0,EModifierLeftFunc|EModifierFunc|EModifierLeftAlt|EModifierAlt|EModifierAutorepeatable);
			AddExpectedKey(EEventKey,EStdKeyEnter,'a',2,EModifierLongKey|EModifierLeftFunc|EModifierFunc|EModifierLeftAlt|EModifierAlt|EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp,EStdKeyEnter);
			AddExpectedKey(EEventKeyUp,EStdKeyLeftAlt);
			AddExpectedKey(EEventKeyUp,EStdKeyLeftFunc);
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			break;
		case 4:
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'Z');
			TheClient->iWs.Flush();
			DelayForRepeatEvents(3, 1);
			iTest->SimulateKey(TRawEvent::EKeyDown,'Y');
			TheClient->iWs.Flush();
			DelayForRepeatEvents(2, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp,'Z');
			iTest->SimulateKey(TRawEvent::EKeyUp,'Y');
			AddExpectedKey(EEventKeyDown,'Z');
			AddExpectedKey(EEventKey,'Z','z');
			AddExpectedKey(EEventKey,'Z','z',1);
			AddExpectedKey(EEventKeyDown,'Y');
			AddExpectedKey(EEventKey,'Y','y');
			AddExpectedKey(EEventKey,'Y','y',1);
			AddExpectedKey(EEventKeyUp,'Z');
			AddExpectedKey(EEventKeyUp,'Y');
			break;
		case 5:
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'Z');
			TheClient->iWs.Flush();
			DelayForRepeatEvents(3, 1);
			iTest->SimulateKey(TRawEvent::EKeyDown,'Y');
			TheClient->iWs.Flush();
			DelayForRepeatEvents(5, 2);
			iTest->SimulateKey(TRawEvent::EKeyUp,'Y');
			TheClient->iWs.Flush();
			DelayForRepeatEvents(2, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp,'Z');
			AddExpectedKey(EEventKeyDown,'Z');
			AddExpectedKey(EEventKey,'Z','z');
			AddExpectedKey(EEventKey,'Z','z',1);
			AddExpectedKey(EEventKeyDown,'Y');
			AddExpectedKey(EEventKey,'Y','y');
			AddExpectedKey(EEventKey,'Y','y',1);
			AddExpectedKey(EEventKeyUp,'Y');
			AddExpectedKey(EEventKey,'Z','z',1);
			AddExpectedKey(EEventKeyUp,'Z');
			break;
		case 6:
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureLongKey(' ','b',0,0,2,ELongCaptureNormal);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'X');
			TheClient->iWs.Flush();
			DelayForRepeatEvents(3, 1);
			iTest->SimulateKeyDownUp(EStdKeySpace);
			TheClient->iWs.Flush();
			if(!iTest->IsFullRomL())
				{
				DelayForRepeatEvents(2, 1);
				}
			iTest->SimulateKey(TRawEvent::EKeyUp,'X');
			AddExpectedKey(EEventKeyDown,'X');
			AddExpectedKey(EEventKey,'X','x');
			AddExpectedKey(EEventKey,'X','x',1);
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKey,EStdKeySpace,' ');
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			if(!iTest->IsFullRomL())
				{
				AddExpectedKey(EEventKey,'X','x',1);
				}
			AddExpectedKey(EEventKeyUp,'X');
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			break;
		case 7:
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureLongKey(' ','b',0,0,2,ELongCaptureNormal|ELongCaptureRepeatEvents);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'X');
			TheClient->iWs.Flush();
			DelayForRepeatEvents(5, 1);
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeySpace);
			TheClient->iWs.Flush();
			DelayForRepeatEvents(1, 3);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
			TheClient->iWs.Flush();
			User::ResetInactivityTime();
			DelayForRepeatEvents(4, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp,'X');
			AddExpectedKey(EEventKeyDown,'X');
			AddExpectedKey(EEventKey,'X','x');
			AddExpectedKey(EEventKey,'X','x',1);
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKey,EStdKeySpace,' ');
			AddExpectedKey(EEventKey,EStdKeySpace,'b',1);
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			AddExpectedKey(EEventKey,'X','x',1);
			AddExpectedKey(EEventKeyUp,'X');
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			break;
		case 8:
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureLongKey(' ','c',0,0,2,ELongCaptureRepeatEvents);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeySpace);
			TheClient->iWs.Flush();
			DelayForRepeatEvents(5, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKey,EStdKeySpace,'c',1);
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			break;
		case 9:
			{
			TInt captureKey=iQueueClient->iGroup->GroupWin()->CaptureLongKey(EKeyEscape,'e',0,0,2,ELongCaptureNormal|ELongCaptureRepeatEvents);
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureLongKey(' ','d',0,0,2,ELongCaptureNormal|ELongCaptureRepeatEvents);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeySpace);
			TheClient->iWs.Flush();
			DelayForRepeatEvents(5, 2);
			iTest->SimulateKeyDownUp(EStdKeyEscape);
			TheClient->iWs.Flush();
			DelayForRepeatEvents(3, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKey,EStdKeySpace,' ');
			AddExpectedKey(EEventKey,EStdKeySpace,'d',1);
			AddExpectedKey(EEventKeyDown,EStdKeyEscape);
			AddExpectedKey(EEventKey,EStdKeyEscape,EKeyEscape);
			AddExpectedKey(EEventKeyUp,EStdKeyEscape);
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(captureKey);
			}
			break;
		case 10:
			//Cancel a long capture key event whilst a repeat is underway
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureLongKey(' ','a',0,0,2,ELongCaptureNormal|ELongCaptureRepeatEvents);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeySpace);
			TheClient->iWs.Flush();
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			DelayForRepeatEvents(5, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKey,EStdKeySpace,' ');
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			break;
		case 11:
			// Cancel a key up/down capture whilst a repeat is underway.
			// This should have no effect since only key press events are
			// repeated.
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureKeyUpAndDowns('X',0,0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'X');
			TheClient->iWs.Flush();
			iQueueClient->iGroup->GroupWin()->CancelCaptureKeyUpAndDowns(iCaptureKey);
			DelayForRepeatEvents(5, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp,'X');
			AddExpectedKey(EEventKeyDown,'X');
			AddExpectedKey(EEventKey,'X','x');
			AddExpectedKey(EEventKey,'X','x',2);
			AddExpectedKey(EEventKeyUp,'X');
			break;
		case 12:
			//Cancel a capture key event whilst a repeat is underway
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureKey('y',0,0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'Y');
			TheClient->iWs.Flush();
			iQueueClient->iGroup->GroupWin()->CancelCaptureKey(iCaptureKey);
			DelayForRepeatEvents(5, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp,'Y');
			AddExpectedKey(EEventKeyDown,'Y');
			AddExpectedKey(EEventKey,'Y','y');
			AddExpectedKey(EEventKeyUp,'Y');
			break;
		case 13:
			//Variation on case 12 i.e. change in the timing of the CancelCaptureKey call
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureKey('z',0,0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,'Z');
			TheClient->iWs.Flush();
			DelayForRepeatEvents(5, 2);
			iQueueClient->iGroup->GroupWin()->CancelCaptureKey(iCaptureKey);
			DelayForRepeatEvents(5, 2);
			iTest->SimulateKey(TRawEvent::EKeyUp,'Z');
			AddExpectedKey(EEventKeyDown,'Z');
			AddExpectedKey(EEventKey,'Z','z');
			AddExpectedKey(EEventKey,'Z','z',2);
			AddExpectedKey(EEventKeyUp,'Z');
			break;
		case 14:
			{
	    	// Cancel long capture key event when normal capture key is scheduled for different
	    	// window group.
			iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureLongKey(' ','a',0,0,2,ELongCaptureNormal);
			TInt shortCaptKey = TheClient->iGroup->GroupWin()->CaptureKey(' ',0,0);

			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeySpace);
			TheClient->iWs.Flush();
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			DelayForRepeatEvents(1, 3);

			TheClient->iGroup->GroupWin()->CancelCaptureKey(shortCaptKey);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			}
			break;
		case 15:
			// @SYMPREQ 417-61800: Check that long key events are marked with
			// EModifierLongKey and short events are not.
			iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureLongKey('m', 'm', 0, 0, 0, ELongCaptureNormal);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, 'M');
			TheClient->iWs.Flush();
			DelayForRepeatEvents(1, 3);
			iTest->SimulateKey(TRawEvent::EKeyUp, 'M');
			AddExpectedKey(EEventKeyDown, 'M', 0, 0, 0);
			AddExpectedKey(EEventKey, 'M', 'm', 0, EModifierAutorepeatable);
			AddExpectedKey(EEventKey, 'M', 'm', 1, EModifierLongKey | EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp, 'M', 0, 0, 0);
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			break;
		case 16:
			CheckLongkeyCaptureWithPriority(ELongkeyCaptureDiffPriority);
			break;
		case 17:
			CheckLongkeyCaptureWithPriority(ELongkeyCaptureSamePriority);
			break;
		case 18:
			// Test CaptureLongKey() with flag value ELongCaptureWaitShort
			// when key is not held down long enough to generate a long event.
			// Verify that short event is delivered when the key is released.
			iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureLongKey('p', 'p', 0, 0, 0, ELongCaptureWaitShort);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, 'P');
			TheClient->iWs.Flush();
			User::After(iKeyBoardRepeatInitialDelay.Int() / 2);
			iTest->SimulateKey(TRawEvent::EKeyUp, 'P');
			TheClient->iWs.Flush();
			iTest->SimulateKeyDownUp(EStdKeySpace);
			AddExpectedKey(EEventKeyDown, 'P');
			AddExpectedKey(EEventKey, 'P', 'p', 0, EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp, 'P');
			AddExpectedKeyDownUp(EStdKeySpace, ' ');
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			break;
		case 19:
			{
			// Cancel a capture key event whilst a repeat is underway for a
			// different capture. This should not cancel repeating.
			iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureKey('q', 0, 0);
			TInt captureKey2 = iQueueClient->iGroup->GroupWin()->CaptureKey('r', 0, 0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, 'Q');
			TheClient->iWs.Flush();
			iQueueClient->iGroup->GroupWin()->CancelCaptureKey(captureKey2);
			DelayForRepeatEvents(5, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp, 'Q');
			AddExpectedKey(EEventKeyDown, 'Q');
			AddExpectedKey(EEventKey, 'Q', 'q', 0, EModifierAutorepeatable);
			AddExpectedKey(EEventKey, 'Q', 'q', 2, EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp, 'Q');
			iQueueClient->iGroup->GroupWin()->CancelCaptureKey(iCaptureKey);
			}
			break;
		case 20:
			{
			// Cancel a long capture key event whilst a repeat is underway for
			// a different capture. This should not cancel repeating.
			iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureLongKey('s', 's', 0, 0, 2, ELongCaptureNormal|ELongCaptureRepeatEvents);
			TInt captureKey2 = iQueueClient->iGroup->GroupWin()->CaptureLongKey('t', 't', 0, 0, 2, ELongCaptureNormal|ELongCaptureRepeatEvents);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, 'S');
			TheClient->iWs.Flush();
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(captureKey2);
			DelayForRepeatEvents(5, 1);
			iTest->SimulateKey(TRawEvent::EKeyUp, 'S');
			AddExpectedKey(EEventKeyDown, 'S');
			AddExpectedKey(EEventKey, 'S', 's', 0, EModifierAutorepeatable);
			AddExpectedKey(EEventKey, 'S', 's', 2, EModifierLongKey | EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp, 'S');
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			}
			break;
		case 21:
			{
			// Test CaptureLongKey() with a user specified initial repeat time.
			// Simulate holding down the key for a period shorter than the
			// specified delay. Verify that a long key event is NOT generated.
			// (Note that the period is longer than the default initial repeat
			// time, thus proving that CaptureLongKey() has honoured the delay
			// argument.)
			TTimeIntervalMicroSeconds32 delay(iKeyBoardRepeatInitialDelay.Int() * 4);
			iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureLongKey(delay, 'u', 'u', 0, 0, 0, ELongCaptureNormal);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, 'U');
			TheClient->iWs.Flush();
			User::After(delay.Int() / 2);
			iTest->SimulateKey(TRawEvent::EKeyUp, 'U');
			TheClient->iWs.Flush();
			iTest->SimulateKeyDownUp(EStdKeySpace);
			AddExpectedKey(EEventKeyDown, 'U');
			AddExpectedKey(EEventKey, 'U', 'u', 0, EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp, 'U');
			AddExpectedKeyDownUp(EStdKeySpace, ' ');
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			}
			break;
		case 22:
			{
			// Test CaptureLongKey() with a user specified initial repeat time.
			// Simulate holding down the key for a period longer than the
			// specified delay. Verify that a long key event is generated.
			// (Note that the period is shorter than the default initial repeat
			// time, thus proving that CaptureLongKey() has honoured the delay
			// argument.)
			TTimeIntervalMicroSeconds32 delay(iKeyBoardRepeatInitialDelay.Int() / 4);
			iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureLongKey(delay, 'v', 'v', 0, 0, 0, ELongCaptureNormal);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, 'V');
			TheClient->iWs.Flush();
			User::After(delay.Int() * 2);
			iTest->SimulateKey(TRawEvent::EKeyUp, 'V');
			TheClient->iWs.Flush();
			iTest->SimulateKeyDownUp(EStdKeySpace);
			AddExpectedKey(EEventKeyDown, 'V');
			AddExpectedKey(EEventKey, 'V', 'v', 0, EModifierAutorepeatable);
			AddExpectedKey(EEventKey, 'V', 'v', 1, EModifierLongKey | EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp, 'V');
			AddExpectedKeyDownUp(EStdKeySpace, ' ');
			iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
			}
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

/** Function used for checking CaptureLongKey() works fine with different priorities.

If two window groups have requested capture of the same key with different priorities, 
then the event should be delivered to the group that specified higher priority 
(even if it is in the background).

If two window groups have requested capture of the same key with same priorities,
then the event should be delivered to the group that requested capture most recently.

@param aTestType Enum value specifying test with same priority or with different priority
*/
void CTEventTest::CheckLongkeyCaptureWithPriority(TLongkeyCapPriorityTests aTestType)
	{
	TInt ordinalPriority = TheClient->iGroup->GroupWin()->OrdinalPriority();
	TInt ordinalPosition = TheClient->iGroup->GroupWin()->OrdinalPosition();
	// Bring other window group to foreground and request capture with lower/same priority
	// And request capture from iQueueClient group window with higher priority
	TheClient->iGroup->GroupWin()->SetOrdinalPosition(0, 10);
	TInt capKeyFrontWin = TheClient->iGroup->GroupWin()->CaptureLongKey('m', 'm', 0, 0, (aTestType == ELongkeyCaptureDiffPriority ? KLongKeyCaptureLowPriority : KLongKeyCaptureHighPriority), ELongCaptureNormal);
	iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureLongKey('m', 'm', 0, 0, KLongKeyCaptureHighPriority, ELongCaptureNormal);
	iQueueClient->iWs.Flush();
	iTest->SimulateKey(TRawEvent::EKeyDown, 'M');
	TheClient->iWs.Flush();
	DelayForRepeatEvents(1, 3);
	iTest->SimulateKey(TRawEvent::EKeyUp, 'M');
	AddExpectedEvent(EEventFocusLost);
	//Note: Button down/up events along with immediate key events are not delivered to iQueueCLient's window group
	//as they are sent to foreground/focused window group
	AddExpectedKey(EEventKey, 'M', 'm', 1, EModifierLongKey | EModifierAutorepeatable);
	iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
	TheClient->iGroup->GroupWin()->CancelCaptureLongKey(capKeyFrontWin);
	TheClient->iGroup->GroupWin()->SetOrdinalPosition(ordinalPosition, ordinalPriority);
	AddExpectedEvent(EEventFocusGained);
	}

/** Function used for delaying current thread so that repeat events can be generated.

It uses iKeyBoardRepeatInitialDelay and iKeyBoardRepeatNextDelay variables to get the desired delay.
The idea is to wait for initial delay + some extra value so that first repeat is generated.
For more than one repeat events, wait will be initial delay + next delay + some extra value, and so on.

The extra value is taken as fraction of iKeyBoardRepeatNextDelay.

@param aNumeratorFracVal Numerator value of the fraction.
@param aDenominatorFracVal Denominator value of the fraction.
*/
void CTEventTest::DelayForRepeatEvents(TInt aNumeratorFracVal, TInt aDenominatorFracVal)
	{
	if (!aNumeratorFracVal || !aDenominatorFracVal)
		return;
	User::After(iKeyBoardRepeatInitialDelay.Int() + (iKeyBoardRepeatNextDelay.Int()*aNumeratorFracVal/aDenominatorFracVal));
	}

#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA  	
void CTEventTest::KeyEventTranslation_NextSetOfEventsL()
	{
	TTimeIntervalMicroSeconds32 initialTime;
	TTimeIntervalMicroSeconds32 time;
	TheClient->iWs.GetKeyboardRepeatRate(initialTime, time);

	RWindowGroup* wg = iQueueClient->iGroup->GroupWin();

	#if defined(LOGGING)
		TLogMessageText logMessageText;
		_LIT(KSet,"KeyEventTranslation SetOfEvents: %d of 5");
		logMessageText.Format(KSet,iEventSet);
		INFO_PRINTF1(logMessageText);
	#endif

	switch (iEventSet++)
		{
		case 0:
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp('A');
			AddExpectedEvent(EEventFocusGained);
			AddExpectedKeyDownUp('A','a');
			break;
		case 1:
			// Test key event translation using CaptureKey()
			iCaptureKey = wg->CaptureKey(EKeyDevice0, 0, 0);
			TEST(iCaptureKey >= 0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp(EStdKeyDevice1);
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKeyDown, EStdKeyDevice1, 0, 0);
			AddExpectedKey(EEventKey, EStdKeyDevice1, EKeyDevice0, 0);
			AddExpectedKey(EEventKeyUp, EStdKeyDevice1, 0, 0);
			wg->CancelCaptureKey(iCaptureKey);
			break;
		case 2:
			// Test key event translation using CaptureKeyUpAndDowns()
			iCaptureKey = wg->CaptureKeyUpAndDowns(EStdKeyDevice0, 0, 0);
			TEST(iCaptureKey >= 0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp(EStdKeyDevice1);
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKeyDown, EStdKeyDevice0, 0, 0);
			AddExpectedKey(EEventKey, EStdKeyDevice1, EKeyDevice1, 0);
			AddExpectedKey(EEventKeyUp, EStdKeyDevice0, 0, 0);
			wg->CancelCaptureKeyUpAndDowns(iCaptureKey);
			break;
		case 3:
			// Test key event translation using CaptureLongKey()
			iCaptureKey = wg->CaptureLongKey(EKeyDevice0, EKeyDevice0, 0, 0, 0, ELongCaptureNormal);
			TEST(iCaptureKey >= 0);
			iQueueClient->iWs.Flush();

			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyDevice1);
			TheClient->iWs.Flush();
			User::After(initialTime.Int() + time.Int() / 3);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyDevice1);
			TheClient->iWs.Flush();

			AddExpectedKey(EEventKeyDown, EStdKeyDevice1, 0, 0);
			AddExpectedKey(EEventKey, EStdKeyDevice1, EKeyDevice1, 0, EModifierAutorepeatable);
			AddExpectedKey(EEventKey, EStdKeyDevice1, EKeyDevice0, 1, EModifierLongKey | EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp, EStdKeyDevice1, 0, 0);

			wg->CancelCaptureLongKey(iCaptureKey);
			break;
		case 4:
			// Negative test: check that the capture key code listed in the
			// translation table of the Key Routing Plug-in is not translated
			// in the absence of any capture request for the mapped key.
			iTest->SimulateKeyDownUp(EStdKeyDevice1);
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKeyDown, EStdKeyDevice1, 0, 0);
			AddExpectedKey(EEventKey, EStdKeyDevice1, EKeyDevice1, 0);
			AddExpectedKey(EEventKeyUp, EStdKeyDevice1, 0, 0);
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}

	TheClient->iWs.Flush();
	}

void CTEventTest::KeyEventBlocking_NextSetOfEventsL()
	{
	TTimeIntervalMicroSeconds32 initialTime;
	TTimeIntervalMicroSeconds32 time;
	TheClient->iWs.GetKeyboardRepeatRate(initialTime, time);

	RWindowGroup* wg = iQueueClient->iGroup->GroupWin();

	#if defined(LOGGING)
		TLogMessageText logMessageText;
		_LIT(KSet,"KeyEventBlocking SetOfEvents: %d of 5");
		logMessageText.Format(KSet,iEventSet);
		INFO_PRINTF1(logMessageText);
	#endif

	switch (iEventSet++)
		{
		case 0:
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp('A');
			AddExpectedEvent(EEventFocusGained);
			AddExpectedKeyDownUp('A','a');
			break;
		case 1:
			// Simulate pressing a blocked key (Device3) while no
			// key capture is in effect.
			iTest->SimulateKeyDownUp(EStdKeyDevice3);
			iTest->SimulateKeyDownUp('B');
			TheClient->iWs.Flush();
			AddExpectedKeyDownUp('B','b');
			break;
		case 2:
			// Use RWindowGroup::CaptureKey() to capture EKeyDevice3
			// and simulate pressing the blocked key again.
			iCaptureKey = wg->CaptureKey(EKeyDevice3, 0, 0);
			TEST(iCaptureKey >= 0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp(EStdKeyDevice3);
			iTest->SimulateKeyDownUp('B');
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKey, EStdKeyDevice3, EKeyDevice3, 0);
			AddExpectedKeyDownUp('B','b');
			wg->CancelCaptureKey(iCaptureKey);
			break;
		case 3:
			// Repeat using CaptureKeyUpAndDowns()
			iCaptureKey = wg->CaptureKeyUpAndDowns(EStdKeyDevice3, 0, 0);
			TEST(iCaptureKey >= 0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKeyDownUp(EStdKeyDevice3);
			iTest->SimulateKeyDownUp('B');
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKeyDown, EStdKeyDevice3, 0, 0);
			AddExpectedKey(EEventKeyUp, EStdKeyDevice3, 0, 0);
			AddExpectedKeyDownUp('B','b');
			wg->CancelCaptureKeyUpAndDowns(iCaptureKey);
			break;
		case 4:
			// Repeat using CaptureLongKey()
			iCaptureKey = wg->CaptureLongKey(EKeyDevice3, EKeyDevice3, 0, 0, 0, ELongCaptureNormal);
			TEST(iCaptureKey >= 0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyDevice3);
			TheClient->iWs.Flush();
			User::After(initialTime.Int() + time.Int() / 3);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyDevice3);
			iTest->SimulateKeyDownUp('B');
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKey, EStdKeyDevice3, EKeyDevice3, 1, EModifierLongKey | EModifierAutorepeatable);
			AddExpectedKeyDownUp('B','b');
			wg->CancelCaptureLongKey(iCaptureKey);
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}

	TheClient->iWs.Flush();
	}

void CTEventTest::KeyEventAppRestriction_NextSetOfEventsL()
	{
	TTimeIntervalMicroSeconds32 initialTime;
	TTimeIntervalMicroSeconds32 time;
	TheClient->iWs.GetKeyboardRepeatRate(initialTime, time);

	RWindowGroup* wg = iQueueClient->iGroup->GroupWin();

	#if defined(LOGGING)
		TLogMessageText logMessageText;
		_LIT(KSet,"KeyEventAppRestriction SetOfEvents: %d of 4");
		logMessageText.Format(KSet,iEventSet);
		INFO_PRINTF1(logMessageText);
	#endif

	switch (iEventSet++)
		{
		case 0:
			// Make window group non-focusable so that it receives only
			// keys that are captured and not those that are default-routed.
			wg->EnableReceiptOfFocus(EFalse);
			iQueueClient->iWs.Flush();
			AddExpectedEvent(EEventFocusGained);
			AddExpectedEvent(EEventFocusLost);
			break;
		case 1:
			// Using RWindowGroup::CaptureKey(), attempt to capture a key
			// that is restricted by UID to another application.
			iCaptureKey = wg->CaptureKey(EKeyF20, 0, 0);
			TEST(iCaptureKey == KErrPermissionDenied);
			// Now capture a key that is restricted to the current
			// application's UID and simulate pressing the restricted key.
			iCaptureKey = wg->CaptureKey(EKeyF21, 0, 0);
			TEST(iCaptureKey >= 0);
			iQueueClient->iWs.Flush();
			// Can't use SimulateKeyDownUp() here due to its check that scan
			// codes are not in the range corresponding to lower case letters
			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF21);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF21);
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKey, EStdKeyF21, EKeyF21, 0);
			wg->CancelCaptureKey(iCaptureKey);
			break;
		case 2:
			// Repeat using CaptureKeyUpAndDowns()
			iCaptureKey = wg->CaptureKeyUpAndDowns(EStdKeyF20, 0, 0);
			TEST(iCaptureKey == KErrPermissionDenied);
			iCaptureKey = wg->CaptureKeyUpAndDowns(EStdKeyF21, 0, 0);
			TEST(iCaptureKey >= 0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF21);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF21);
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKeyDown, EStdKeyF21, 0, 0);
			AddExpectedKey(EEventKeyUp, EStdKeyF21, 0, 0);
			wg->CancelCaptureKeyUpAndDowns(iCaptureKey);
			break;
		case 3:
			// Repeat using CaptureLongKey()
			iCaptureKey = wg->CaptureLongKey(EKeyF20, EKeyF20, 0, 0, 0, ELongCaptureNormal);
			TEST(iCaptureKey == KErrPermissionDenied);
			iCaptureKey = wg->CaptureLongKey(EKeyF21, EKeyF21, 0, 0, 0, ELongCaptureNormal);
			TEST(iCaptureKey >= 0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF21);
			TheClient->iWs.Flush();
			User::After(initialTime.Int() + time.Int() / 3);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF21);
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKey, EStdKeyF21, EKeyF21, 1, EModifierLongKey | EModifierAutorepeatable);
			wg->CancelCaptureLongKey(iCaptureKey);
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}

	TheClient->iWs.Flush();
	}

void CTEventTest::KeyEventAppPriority_NextSetOfEventsL()
	{
	TTimeIntervalMicroSeconds32 initialTime;
	TTimeIntervalMicroSeconds32 time;
	TheClient->iWs.GetKeyboardRepeatRate(initialTime, time);

	RWindowGroup* wg = iQueueClient->iGroup->GroupWin();
	TInt handle1 = 0;
	TInt handle2 = 0;
	TInt handle3 = 0;
	TInt handle4 = 0;
	TInt handle5 = 0;
	TInt handle6 = 0;

	#if defined(LOGGING)
		TLogMessageText logMessageText;
		_LIT(KSet,"KeyEventAppPriority SetOfEvents: %d of 5");
		logMessageText.Format(KSet,iEventSet);
		INFO_PRINTF1(logMessageText);
	#endif

	switch (iEventSet++)
		{
		case 0:
			SpawnCaptureAppL(iCaptureApp1);
			iQueueClient->iWs.Flush();
			AddExpectedEvent(EEventFocusGained);
			AddExpectedEvent(EEventFocusLost);
			break;
		case 1:
			// Use RWindowGroup::CaptureKey() to capture EKeyF22
			// and EKeyF23 with priority 0. Simulate pressing both
			// keys. Since the current application has precedence by UID
			// for capture of EKeyF22, it should receive that key
			// but should not receive EKeyF23.
			handle1 = wg->CaptureKey(EKeyF22, 0, 0);
			handle2 = wg->CaptureKey(EKeyF23, 0, 0);
			TEST(handle1 >= 0);
			TEST(handle2 >= 0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF22);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF22);
			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF23);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF23);
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKey, EStdKeyF22, EKeyF22, 0);
			wg->CancelCaptureKey(handle1);
			wg->CancelCaptureKey(handle2);
			break;
		case 2:
			// Repeat using CaptureKeyUpAndDowns()
			handle1 = wg->CaptureKeyUpAndDowns(EStdKeyF22, 0, 0);
			handle2 = wg->CaptureKeyUpAndDowns(EStdKeyF23, 0, 0);
			TEST(handle1 >= 0);
			TEST(handle2 >= 0);
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF22);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF22);
			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF23);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF23);
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKeyDown, EStdKeyF22, 0, 0);
			AddExpectedKey(EEventKeyUp, EStdKeyF22, 0, 0);
			wg->CancelCaptureKeyUpAndDowns(handle1);
			wg->CancelCaptureKeyUpAndDowns(handle2);
			break;
		case 3:
			// Repeat using CaptureLongKey()
			handle1 = wg->CaptureLongKey(EKeyF22, EKeyF22, 0, 0, 0, ELongCaptureNormal);
			handle2 = wg->CaptureLongKey(EKeyF23, EKeyF23, 0, 0, 0, ELongCaptureNormal);
			TEST(handle1 >= 0);
			TEST(handle2 >= 0);
			iQueueClient->iWs.Flush();

			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF22);
			TheClient->iWs.Flush();
			User::After(initialTime.Int() + time.Int() / 3);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF22);

			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF23);
			TheClient->iWs.Flush();
			User::After(initialTime.Int() + time.Int() / 3);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF23);
			TheClient->iWs.Flush();

			AddExpectedKey(EEventKey, EStdKeyF22, EKeyF22, 1, EModifierLongKey | EModifierAutorepeatable);
			wg->CancelCaptureLongKey(handle1);
			wg->CancelCaptureLongKey(handle2);
			break;
		case 4:
			// Repeat with additional capture requests for improved code
			// coverage. (Also verifies that UID-based precedence works even
			// when another app makes the most recent capture request.)
			// Spawn a second capture app so that requests occur in the
			// following order:
			// 1. CaptureApp1, priority 1 (outstanding)
			// 2. Current App, priority 0, precedence by UID
			// 3. CaptureApp2, priority 1
			// Note that all three capture types are tested together here.
			handle1 = wg->CaptureKey(EKeyF22, 0, 0);
			handle2 = wg->CaptureKey(EKeyF23, 0, 0);
			handle3 = wg->CaptureKeyUpAndDowns(EStdKeyF22, 0, 0);
			handle4 = wg->CaptureKeyUpAndDowns(EStdKeyF23, 0, 0);
			handle5 = wg->CaptureLongKey(EKeyF22, EKeyF22, 0, 0, 0, ELongCaptureNormal);
			handle6 = wg->CaptureLongKey(EKeyF23, EKeyF23, 0, 0, 0, ELongCaptureNormal);
			TEST(handle1 >= 0);
			TEST(handle2 >= 0);
			TEST(handle3 >= 0);
			TEST(handle4 >= 0);
			TEST(handle5 >= 0);
			TEST(handle6 >= 0);
			iQueueClient->iWs.Flush();

			SpawnCaptureAppL(iCaptureApp2);

			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF22);
			TheClient->iWs.Flush();
			User::After(initialTime.Int() + time.Int() / 3);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF22);

			iTest->SimulateKey(TRawEvent::EKeyDown, EStdKeyF23);
			TheClient->iWs.Flush();
			User::After(initialTime.Int() + time.Int() / 3);
			iTest->SimulateKey(TRawEvent::EKeyUp, EStdKeyF23);
			TheClient->iWs.Flush();

			AddExpectedKey(EEventKeyDown, EStdKeyF22, 0, 0);
			AddExpectedKey(EEventKey, EStdKeyF22, EKeyF22, 0);
			AddExpectedKey(EEventKey, EStdKeyF22, EKeyF22, 1, EModifierLongKey | EModifierAutorepeatable);
			AddExpectedKey(EEventKeyUp, EStdKeyF22, 0, 0);
			wg->CancelCaptureKey(handle1);
			wg->CancelCaptureKey(handle2);
			wg->CancelCaptureKeyUpAndDowns(handle3);
			wg->CancelCaptureKeyUpAndDowns(handle4);
			wg->CancelCaptureLongKey(handle5);
			wg->CancelCaptureLongKey(handle6);
			break;
		default:
			iCaptureApp1.Kill(KErrNone);
			iCaptureApp2.Kill(KErrNone);
			iCaptureApp1.Close();
			iCaptureApp2.Close();
			CActiveScheduler::Stop();
			break;
		}

	TheClient->iWs.Flush();
	}

/**
Spawn another application that will capture the keys EKeyF22 and
EKeyF23 with high priority.

@param aProcess		Process to create
*/
void CTEventTest::SpawnCaptureAppL(RProcess& aProcess)
	{
	_LIT(KCaptureApp, "tevent_captureapp.exe");
	TInt error = aProcess.Create(KCaptureApp, KNullDesC);
	if (error != KErrNone)
		{
#if defined(LOGGING)
		TLogMessageText logMessageText;
		_LIT(KLog, "Cannot create capture app - error %d");
		logMessageText.Format(KLog, error);
		INFO_PRINTF1(logMessageText);
#endif
		User::Leave(error);
		}

	// Make rendezvous request to capture app
	TRequestStatus status;
	aProcess.Rendezvous(status);
	aProcess.Resume();

	// Wait for capture app to make its capture requests
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	}
#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA  	

#define PASSWORD_START_OF_DAY 4
void CTEventTest::Password_NextSetOfEvents()
	{
	switch(iEventSet++)
		{
		case 0:
			iQueueClient->iWin->BaseWin()->PasswordWindow(EPasswordAlwaysTriggerNow);
			iQueueClient->iWs.PasswordEntered();
			iTest->SimulateKeyDownUp('2');
			iQueueClient->iWs.Flush();
			AddExpectedEvent(EEventFocusGained);
			AddExpectedEvent(EEventPassword);
			AddExpectedKeyDownUp('2');
			break;
		case 1:
			iQueueClient->iWin->BaseWin()->PasswordWindow(EPasswordOnceADayTriggerNow);
			iQueueClient->iWs.PasswordEntered();
			iTest->SimulateKeyDownUp('3');
			iQueueClient->iWs.Flush();
			AddExpectedEvent(EEventPassword);
			AddExpectedKeyDownUp('3');
			break;
		case 2:
			{
			TTime time;
			time.HomeTime();
			TInt day=time.DayNoInMonth();
			if (day==0)
				day=5;
			time-=TTimeIntervalDays(day);
			time+=TTimeIntervalHours(PASSWORD_START_OF_DAY);
			User::SetHomeTime(time);
			iQueueClient->iWin->BaseWin()->PasswordWindow(EPasswordOnceADayTriggerNow);
			iQueueClient->iWs.PasswordEntered();
			iTest->SimulateKeyDownUp('3');
			iQueueClient->iWs.Flush();
			AddExpectedEvent(EEventPassword);
			AddExpectedKeyDownUp('3');
			time+=TTimeIntervalDays(day);
			time-=TTimeIntervalHours(PASSWORD_START_OF_DAY);
			User::SetHomeTime(time);
			}
			break;
		case 3:
			{
			TTime time;
			time.HomeTime();
			TInt day=time.DayNoInMonth();
			time-=TTimeIntervalDays(day);
			time+=TTimeIntervalHours(PASSWORD_START_OF_DAY);
			User::SetHomeTime(time);
			iQueueClient->iWin->BaseWin()->PasswordWindow(EPasswordOnceADay);
			iQueueClient->iWs.PasswordEntered();
			iTest->SimulateEvent(TRawEvent::ESwitchOn);
			iTest->SimulateKeyDownUp('3');
			iQueueClient->iWs.Flush();
			AddExpectedEvent(EEventPassword);
			AddExpectedEvent(EEventSwitchOn);
			AddExpectedKeyDownUp('3');
			time+=TTimeIntervalDays(day);
			time-=TTimeIntervalHours(PASSWORD_START_OF_DAY);
			User::SetHomeTime(time);
			}
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

void CTEventTest::GroupListChanged_NextSetOfEventsL()
	{
	_LIT(WindowGroup1,"WindowGroupName1");
	_LIT(WindowGroup2,"WindowGroupName2");
	switch(iEventSet++)
		{
		case 0:
			{
			_LIT(TestGroup3,"GroupListEvents3");
			iQueueClient->EnableGroupListChangeEvents();
			iEventTestGroup3=new(ELeave) CTWindowGroup(TheClient);
			iEventTestGroup3->ConstructL();
			iEventTestGroup3->GroupWin()->SetName(TestGroup3);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedEvent(EEventFocusLost);
			AddExpectedEvent(EEventWindowGroupListChanged);
			AddExpectedEvent(EEventWindowGroupsChanged);
			}
			break;
		case 1:
			{
			_LIT(TestGroup4,"GroupListEvents4");
			iEventTestGroup4=new(ELeave) CTWindowGroup(TheClient);
			iEventTestGroup4->ConstructL();
			iEventTestGroup4->GroupWin()->SetName(TestGroup4);
			AddExpectedEvent(EEventWindowGroupListChanged);
			AddExpectedEvent(EEventWindowGroupsChanged);
			}
			break;
		case 2:
			iEventTestGroup3->GroupWin()->SetOrdinalPosition(0);
			AddExpectedEvent(EEventWindowGroupListChanged);
			break;
		case 3:
			iEventTestGroup3->GroupWin()->SetOrdinalPosition(999);
			AddExpectedEvent(EEventWindowGroupListChanged);
			break;
		case 4:
			iEventTestGroup4->GroupWin()->SetOrdinalPosition(999);
			AddExpectedEvent(EEventFocusGained);
			AddExpectedEvent(EEventWindowGroupListChanged);
            // Testcase for defect INC135635
			// Since iWin has OrdinalPosition Zero (the front) clicking on it won't generate an 
			// EEventWindowGroupListChanged event as there is no change in the order of the windows
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(20-EWinPositionX,11-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(20-EWinPositionX,11-EWinPositionY));
			SimulatePointer(TRawEvent::EButton1Down,20,11);
			SimulatePointer(TRawEvent::EButton1Up,20,11);
			break;
		case 5:
			iEventTestGroup3->GroupWin()->SetOrdinalPosition(999);
			AddExpectedEvent(EEventWindowGroupListChanged);
			break;
		case 6:
			iEventTestGroup3->GroupWin()->SetOrdinalPosition(99);
			SimulatePointer(TRawEvent::EButton1Down,20,11);
			iEventTestGroup3->GroupWin()->SetName(WindowGroup1);
			AddExpectedPointer(TPointerEvent::EButton1Down, TPoint(20-EWinPositionX,11-EWinPositionY));
			AddExpectedEvent(EEventWindowGroupsChanged);
			break;
		case 7:
			iEventTestGroup3->GroupWin()->SetName(WindowGroup2);
			SimulatePointer(TRawEvent::EButton1Up,10,20);
			AddExpectedEvent(EEventWindowGroupsChanged);
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(10-EWinPositionX,20-EWinPositionY));
			break;
		case 8:
			iEventTestGroup3->GroupWin()->SetName(WindowGroup1);
			SimulatePointer(TRawEvent::EButton2Down,20,30);
			AddExpectedEvent(EEventWindowGroupsChanged);
			AddExpectedPointer(TPointerEvent::EButton2Down, TPoint(20-EWinPositionX,30-EWinPositionY));
			break;
		case 9:
			SimulatePointer(TRawEvent::EButton1Up,15,15);
			iEventTestGroup3->GroupWin()->SetName(WindowGroup2);
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(15-EWinPositionX,15-EWinPositionY));
			AddExpectedEvent(EEventWindowGroupsChanged);
			break;
		case 10:
			SimulatePointer(TRawEvent::EButton2Down,25,20);
			iEventTestGroup3->GroupWin()->SetName(WindowGroup2);
			SimulatePointer(TRawEvent::EButton1Up,15,20);
			iEventTestGroup3->GroupWin()->SetName(WindowGroup1);
			AddExpectedPointer(TPointerEvent::EButton2Down, TPoint(25-EWinPositionX,20-EWinPositionY));
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(15-EWinPositionX,20-EWinPositionY));
			AddExpectedEvent(EEventWindowGroupsChanged);
			break;
		case 11:
			iEventTestGroup3->GroupWin()->SetName(WindowGroup1);
			SimulatePointer(TRawEvent::EButton2Down,12,12);
			iEventTestGroup3->GroupWin()->SetName(WindowGroup2);
			SimulatePointer(TRawEvent::EButton1Up,12,12);
			AddExpectedPointer(TPointerEvent::EButton2Down, TPoint(12-EWinPositionX,12-EWinPositionY));
			AddExpectedEvent(EEventWindowGroupsChanged);
			AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(12-EWinPositionX,12-EWinPositionY));
			break;
		case 12:
			delete iEventTestGroup3;
			iEventTestGroup3=NULL;
			AddExpectedEvent(EEventWindowGroupsChanged);
			AddExpectedEvent(EEventWindowGroupListChanged);
			break;
		case 13:
			delete iEventTestGroup4;
			iEventTestGroup4=NULL;
			AddExpectedEvent(EEventWindowGroupsChanged);
			AddExpectedEvent(EEventWindowGroupListChanged);
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

TInt CTEventTest::KeyRepeatTime() const
	{
	TTimeIntervalMicroSeconds32 initialTime;
	TTimeIntervalMicroSeconds32 time;
	TheClient->iWs.GetKeyboardRepeatRate(initialTime,time);
	return(initialTime.Int());
	}

void CTEventTest::RepeatableKeysL()
	{
	// repeatable key event
	TKeyEvent repKeyEvent; 
	repKeyEvent.iCode='J';
	repKeyEvent.iScanCode=0;
	repKeyEvent.iModifiers=EModifierAutorepeatable;
	repKeyEvent.iRepeats=0;
	
	// non-repeatable key event
	TKeyEvent keyEvent; 
	keyEvent.iCode='J';
	keyEvent.iScanCode=0;
	keyEvent.iModifiers=0;
	keyEvent.iRepeats=0;
	
	switch(iEventSet++)
		{
		case 0:
			{
			TheClient->iWs.SimulateKeyEvent(repKeyEvent);	// Create a repeatable key event
			TheClient->iWs.SimulateKeyEvent(repKeyEvent);	// Create a repeatable key event
			TheClient->iWs.SimulateKeyEvent(keyEvent);		// Create a non-repeatable key event
			TheClient->iWs.Flush();							// Send events
			AddExpectedEvent(EEventFocusGained);			// Always get a focus gain at start
			AddExpectedKey(1, repKeyEvent.iScanCode, repKeyEvent.iCode, repKeyEvent.iRepeats, repKeyEvent.iModifiers);// Expect the event that was created above
			AddExpectedKey(1, repKeyEvent.iScanCode, repKeyEvent.iCode, repKeyEvent.iRepeats, repKeyEvent.iModifiers);// Expect the event that was created above
			AddExpectedKey(1, keyEvent.iScanCode, keyEvent.iCode, keyEvent.iRepeats, keyEvent.iModifiers);// Expect the event that was created above
			break;
			}
		case 1:
			{
			TheClient->iWs.SimulateKeyEvent(repKeyEvent);	// Create a repeatable key event
			TheClient->iWs.SimulateKeyEvent(keyEvent);		// Create a non-repeatable key event
			TheClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown, 'A');	// Create a raw event
			TheClient->iWs.Flush();
			User::After(KeyRepeatTime()*3/2);
			iTest->SimulateKey(TRawEvent::EKeyUp, 'A');		// Create a raw event
			TheClient->iWs.Flush();
			AddExpectedKey(1, repKeyEvent.iScanCode, repKeyEvent.iCode, repKeyEvent.iRepeats, repKeyEvent.iModifiers);// Expect the event that was created above
			AddExpectedKey(1, keyEvent.iScanCode, keyEvent.iCode, keyEvent.iRepeats, keyEvent.iModifiers);// Expect the event that was created above
			AddExpectedKey(EEventKeyDown, 'A');
			AddExpectedKey(EEventKey,'A','a');
			AddExpectedKey(EEventKey,'A','a',1);
			AddExpectedKey(EEventKeyUp, 'A');
			break;
			}
		case 2:
			{
			iTest->SimulateKey(TRawEvent::EKeyDown, 'A');	// Create a raw event
			TheClient->iWs.Flush();
			User::After(KeyRepeatTime()*3/2);
			TheClient->iWs.SimulateKeyEvent(repKeyEvent);	// Create a repeatable key event
			TheClient->iWs.SimulateKeyEvent(keyEvent);		// Create a non-repeatable key event
			TheClient->iWs.Flush();							// Send event
			iTest->SimulateKey(TRawEvent::EKeyUp, 'A');		// Create a raw event
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKeyDown, 'A');
			AddExpectedKey(EEventKey,'A','a');
			AddExpectedKey(EEventKey,'A','a',1);
			AddExpectedKey(1, repKeyEvent.iScanCode, repKeyEvent.iCode, repKeyEvent.iRepeats, repKeyEvent.iModifiers);// Expect the event that was created above
			AddExpectedKey(1, keyEvent.iScanCode, keyEvent.iCode, keyEvent.iRepeats, keyEvent.iModifiers);// Expect the event that was created above
			AddExpectedKey(EEventKeyUp, 'A');
			break;
			}
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

CTBlankWindow* CTEventTest::NewBlueForgroundWindowL()
	{
	CTBlankWindow* window;
	window=new (ELeave) CTBlankWindow;
	CleanupStack::PushL(window);
	window->ConstructL(*iQueueClient->iGroup);
	User::LeaveIfError(window->BaseWin()->SetRequiredDisplayMode(EColor256));
	CleanupStack::Pop(window);

	window->SetColor(KRgbBlue);
	window->SetExt(TPoint(40, 40), TSize(40,40));
	window->BaseWin()->SetOrdinalPosition(0);
	return window;
	}

CTBlankWindow* CTEventTest::NewBlueForgroundWindow()
	{
	CTBlankWindow* window = NULL;
	TRAPD(err, window=NewBlueForgroundWindowL());
	if (err != KErrNone)
		Failed();
	return window;
	}

#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
static void CreateSurfaceAttributes(RSurfaceManager::TSurfaceCreationAttributesBuf& aBuffer , const TSize& aSize)
    {
    RSurfaceManager::TSurfaceCreationAttributes& b = aBuffer();
                
    b.iSize.iWidth = aSize.iWidth;
    b.iSize.iHeight = aSize.iHeight;
    b.iBuffers = 1;              // number of buffers in the surface
    b.iPixelFormat = EUidPixelFormatARGB_8888;
    b.iStride = aSize.iWidth * 4;        // Number of bytes between start of one line and start of next
    b.iOffsetToFirstBuffer = 0; // way of reserving space before the surface pixel data
    b.iAlignment = 4;           // alignment, 1,2,4,8 byte aligned
    b.iContiguous = EFalse;
    b.iMappable = ETrue;
    }

static const TInt KFullyVisible = TWsVisibilityChangedEvent::EPartiallyVisible | TWsVisibilityChangedEvent::EFullyVisible;

void CTEventTest::SurfaceVisibilityChanged1_NextSetOfEventsL()
    {    
    switch(iEventSet++)
        {
        case 0:
            {
            //Setup
            LOG_MESSAGE(_L("case 0"));
            TInt err = iSurfaceManager.Open();
            if (err != KErrNone)
                {
                RDebug::Printf("RSurfaceManager::Open() failed", err);
                User::Leave(err);
                }
            
            iVisWins1=NewBlueForgroundWindowL();
            iVisWins1->Activate();
            iVisWins1->BaseWin()->EnableVisibilityChangeEvents();

            AddExpectedEvent(EEventFocusGained);
            AddExpectedVisibilityChange(KFullyVisible, iVisWins1);
            break;
            }
        case 1:
            {
            LOG_MESSAGE(_L("case 1"));
            iVisWins2=NewBlueForgroundWindowL(); // will cover iVisWins1
            iVisWins2->BlankWin()->SetColor();  //remove colour to ensure window does not draw to UI layer
            iVisWins2->Activate();
            
            AddExpectedVisibilityChange(TWsVisibilityChangedEvent::ENotVisible, iVisWins1);
            break;
            }
        case 2:
            {
            // This case contains the actual test GRAPHICS-WSERV-2669-0015
            LOG_MESSAGE(_L("case 2"));
            RSurfaceManager::TSurfaceCreationAttributesBuf surfaceBuf;
            CreateSurfaceAttributes(surfaceBuf, iVisWins2->BaseWin()->Size());
            
            iSurfaceId = TSurfaceId::CreateNullId();
            TInt err = iSurfaceManager.CreateSurface(surfaceBuf, iSurfaceId);
            if (err != KErrNone)
                {
                RDebug::Printf("RSurfaceManager::CreateSurface() error: %d", err);
                TEST(EFalse);
                }
         
            err = iVisWins2->BaseWin()->SetBackgroundSurface(iSurfaceId);
            if (err != KErrNone)
                {
                RDebug::Printf("RWindow::SetBackgroundSurface() error: %d", err);
                TEST(EFalse);
                }
            
            iVisWins2->BaseWin()->SetSurfaceTransparency(ETrue); //iVisWins1 will become visible
            
            AddExpectedVisibilityChange(KFullyVisible, iVisWins1);
            break;
            }
        default:
            LOG_MESSAGE(_L("default"));
            if (!iSurfaceId.IsNull())
                {
                iSurfaceManager.CloseSurface(iSurfaceId);
                iSurfaceId = TSurfaceId::CreateNullId();
                }
            iSurfaceManager.Close();
            delete iVisWins1;
            iVisWins1 = NULL;
            delete iVisWins2;
            iVisWins2 = NULL;
            CActiveScheduler::Stop();
            break;
        }
    TheClient->iWs.Flush();
    }

void CTEventTest::SurfaceVisibilityChanged2_NextSetOfEventsL()
    {
    switch (iEventSet++)
        {
        case 0:
            {
            //Win1 - bottom window
            //Win2 - middle window with semi-transparent background surface, which totally obscures win1
            LOG_MESSAGE(_L("case 0"));
            TInt err = iSurfaceManager.Open();
            if (err != KErrNone)
                {
                RDebug::Printf("RSurfaceManager::Open() failed", err);
                User::Leave(err);
                }
            
            iVisWins1 = NewBlueForgroundWindowL();
            iVisWins1->BaseWin()->EnableVisibilityChangeEvents();

            iVisWins2 = NewBlueForgroundWindowL();
            iVisWins2->BlankWin()->SetColor(); //remove colour to ensure window does not draw to UI layer
            iVisWins2->BaseWin()->EnableVisibilityChangeEvents();
            
            iVisWins1->Activate(); //obscured
            iVisWins2->Activate(); //visible
            
            // Create surface for background window    
            RSurfaceManager::TSurfaceCreationAttributesBuf surfaceBuf;
            CreateSurfaceAttributes(surfaceBuf, iVisWins2->BaseWin()->Size());

            iSurfaceId = TSurfaceId::CreateNullId();
            err = iSurfaceManager.CreateSurface(surfaceBuf, iSurfaceId);
            if (err != KErrNone)
                {
                RDebug::Printf("RSurfaceManager::CreateSurface() error: %d", err);
                TEST(EFalse);
                }

            err = iVisWins2->BaseWin()->SetBackgroundSurface(iSurfaceId);
            if (err != KErrNone)
                {
                RDebug::Printf("RWindow::SetBackgroundSurface() error: %d", err);
                TEST(EFalse);
                }

            iVisWins2->BaseWin()->SetSurfaceTransparency(ETrue); //win1 and win2 both visible

            AddExpectedEvent(EEventFocusGained);
            AddExpectedVisibilityChange(KFullyVisible, iVisWins2);
            AddExpectedVisibilityChange(KFullyVisible, iVisWins1);
            break;
            }
        case 1:
            {
            //Win3 - opaque top window which obscures win2 and win1
            LOG_MESSAGE(_L("case 1"));
            iVisWins3 = NewBlueForgroundWindowL();
            iVisWins3->SetColor(KRgbRed);
            iVisWins3->Activate();

            AddExpectedVisibilityChange(TWsVisibilityChangedEvent::ENotVisible, iVisWins2);
            AddExpectedVisibilityChange(TWsVisibilityChangedEvent::ENotVisible, iVisWins1);
            break;
            }
        case 2:
            {
            //This case contains the actual test GRAPHICS-WSERV-2669-0016
            LOG_MESSAGE(_L("case 2"));
            delete iVisWins3;
            iVisWins3 = NULL;

            AddExpectedVisibilityChange(KFullyVisible, iVisWins2);
            AddExpectedVisibilityChange(KFullyVisible, iVisWins1);
            break;
            }
        default:
            LOG_MESSAGE(_L("default"));
            if (!iSurfaceId.IsNull())
                {
                iSurfaceManager.CloseSurface(iSurfaceId);
                iSurfaceId = TSurfaceId::CreateNullId();
                }
            iSurfaceManager.Close();
            delete iVisWins1;
            iVisWins1 = NULL;
            delete iVisWins2;
            iVisWins2 = NULL;
            delete iVisWins3;
            iVisWins3 = NULL;
            CActiveScheduler::Stop();
            return;
        }
    TheClient->iWs.Flush();
    }

TInt CTEventTest::GenerateAnEvent(TAny* aEventTest)
    {
    CTEventTest* self = static_cast<CTEventTest*>(aEventTest);
    self->LogMessage(((TText8*)__FILE__), __LINE__,_L("Negative test passed OK - no event was generated."));
    //Now we must generate an event to get CTQueueTestEvent out of its wait loop 
    self->SimulateRepeatEvent(32);
    self->AddExpectedKey(EEventKey, 32);
    return KErrNone;
    }

void CTEventTest::SurfaceVisibilityChanged3_NextSetOfEventsL()
    {
    switch (iEventSet++)
        {
        case 0:
            {
            //Win1 - Bottom window
            //Win2 - Top window with semi-transparent background surface. Does not overlap win1.
            LOG_MESSAGE(_L("case 0"));
            TInt err = iSurfaceManager.Open();
            if (err != KErrNone)
                {
                RDebug::Printf("RSurfaceManager::Open() failed", err);
                User::Leave(err);
                }
            iTimeOutCallback = CPeriodic::NewL(CActive::EPriorityIdle);
            
            iVisWins1 = NewBlueForgroundWindowL();
            iVisWins1->BaseWin()->EnableVisibilityChangeEvents();

            iVisWins2 = NewBlueForgroundWindowL();
            iVisWins2->BlankWin()->SetColor(); //remove colour to ensure window does not draw to UI layer
            TPoint newPos(iVisWins1->Position() + iVisWins1->Size());
            iVisWins2->SetPos(newPos); //to not overlap win1 
            
            iVisWins1->Activate(); //upper left
            iVisWins2->Activate(); //lower right
            
            // Create surface for background window    
            RSurfaceManager::TSurfaceCreationAttributesBuf surfaceBuf;
            CreateSurfaceAttributes(surfaceBuf, iVisWins2->BaseWin()->Size());

            iSurfaceId = TSurfaceId::CreateNullId();
            err = iSurfaceManager.CreateSurface(surfaceBuf, iSurfaceId);
            if (err != KErrNone)
                {
                RDebug::Printf("RSurfaceManager::CreateSurface() error: %d", err);
                TEST(EFalse);
                }

            err = iVisWins2->BaseWin()->SetBackgroundSurface(iSurfaceId);
            if (err != KErrNone)
                {
                RDebug::Printf("RWindow::SetBackgroundSurface() error: %d", err);
                TEST(EFalse);
                }

            iVisWins2->BaseWin()->SetSurfaceTransparency(ETrue);

            AddExpectedEvent(EEventFocusGained);
            AddExpectedVisibilityChange(KFullyVisible, iVisWins1);            
            break;
            }
        case 1:
            {
            //This case contains the actual test GRAPHICS-WSERV-2669-0017
            LOG_MESSAGE(_L("case 1"));
            iVisWins2->SetPos(iVisWins1->Position()); //Win2 now overlaps Win1, but because Win2 has 
                                                      //a transparent surface Win1 is still fully visible

            //Because we don't expect any events we need to generate one to avoid waiting forever.
            TCallBack timeout(GenerateAnEvent, this);
            iTimeOutCallback->Start(1000000, 10000000, timeout);
            break;
            }
        default:            
            LOG_MESSAGE(_L("default"));
            iTimeOutCallback->Cancel();
            delete iTimeOutCallback;
            iTimeOutCallback = NULL;
            if (!iSurfaceId.IsNull())
                {
                iSurfaceManager.CloseSurface(iSurfaceId);
                iSurfaceId = TSurfaceId::CreateNullId();
                }
            iSurfaceManager.Close();
            delete iVisWins1;
            iVisWins1 = NULL;
            delete iVisWins2;
            iVisWins2 = NULL;
            CActiveScheduler::Stop();
            return;
        }
    TheClient->iWs.Flush();
    }
#endif //TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
#endif //SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS

void CTEventTest::VisibilityChanged_NextSetOfEventsL()
	{
	const TInt fullyVisible = TWsVisibilityChangedEvent::EPartiallyVisible | TWsVisibilityChangedEvent::EFullyVisible;

	switch(iEventSet++)
		{
		case 0: //only this case can leave
			{
			iQueueClient->iWin->BackedUpWin()->MaintainBackup();
			iVisWins1=NewBlueForgroundWindowL();
			iVisWins2=NewBlueForgroundWindowL();

			iVisWins1->BaseWin()->EnableVisibilityChangeEvents();
			iVisWins1->Activate();
			iVisWins2->SetColor(KRgbGreen);
			iVisWins2->SetExt(TPoint(80, 80), TSize(120,120));
			iVisWins2->Activate();

			AddExpectedEvent(EEventFocusGained);
			AddExpectedVisibilityChange(fullyVisible);
			}
			break;
		case 1:
			iVisWins1->BaseWin()->SetVisible(EFalse);
			AddExpectedVisibilityChange(TWsVisibilityChangedEvent::ENotVisible);
			break;
		case 2:
			iVisWins1->BaseWin()->SetVisible(ETrue);
			AddExpectedVisibilityChange(fullyVisible);
			break;
		case 3:
			iVisWins2->SetExt(TPoint(0,0), TSize(120,120));
			AddExpectedVisibilityChange(TWsVisibilityChangedEvent::ENotVisible);
			break;
		case 4:
			iVisWins2->BaseWin()->SetVisible(EFalse);
			AddExpectedVisibilityChange(fullyVisible);
			break;
		case 5:
			iVisWins2->BaseWin()->SetVisible(ETrue);
			AddExpectedVisibilityChange(TWsVisibilityChangedEvent::ENotVisible);
			break;
		case 6:
			iVisWins2->SetExt(TPoint(60,60), TSize(120,120));
			AddExpectedVisibilityChange(TWsVisibilityChangedEvent::EPartiallyVisible);
			break;
		case 7:
			iVisWins1->SetExt(TPoint(80,80), TSize(40,40));
			AddExpectedVisibilityChange(TWsVisibilityChangedEvent::ENotVisible);
			break;
		case 8:
			iVisWins1->BaseWin()->SetOrdinalPosition(0);
			AddExpectedVisibilityChange(fullyVisible);

			if(TransparencySupportedL() != KErrNone)
				{
				iEventSet += 2; //we shall skip set 9,10 as they work with transparency
				}
			break;
		case 9:
			{
			TDisplayMode mode = EColor256;
			iTransWin=CTransWindow::NewL(iQueueClient->iGroup, TRgb(255, 0, 0, 128), TRect(0,0,200,200), &mode);
			iTransWin->CTWin::DrawNow();
			// it gets put at ordinal position 0
			iVisWins2->BaseWin()->SetOrdinalPosition(1);
			AddExpectedVisibilityChange(TWsVisibilityChangedEvent::ENotVisible);
			}
			break;
		case 10:
			iVisWins1->BaseWin()->SetOrdinalPosition(1);
			AddExpectedVisibilityChange(fullyVisible);
			break;
		case 11:
			{
			delete iVisWins1;
			iVisWins1=NULL;
			iVisWins1=NewBlueForgroundWindow();

			iVisWins1->Activate();
			iVisWins1->BaseWin()->EnableVisibilityChangeEvents();

			AddExpectedVisibilityChange(fullyVisible);
			}
			break;
		case 12:
			{
			delete iVisWins1;
			iVisWins1=NULL;
			iVisWins1=NewBlueForgroundWindow();

			iVisWins1->Activate();
			iVisWins1->BaseWin()->EnableVisibilityChangeEvents();
			iVisWins1->BaseWin()->SetVisible(EFalse);

			AddExpectedVisibilityChange(TWsVisibilityChangedEvent::ENotVisible);
			}
			break;
		case 13:
			{
			delete iVisWins1;
			iVisWins1=NULL;
			iVisWins1=NewBlueForgroundWindow();

			iVisWins1->BaseWin()->EnableVisibilityChangeEvents();
			iVisWins1->Activate();

			AddExpectedVisibilityChange(fullyVisible);
			}
			break;
		case 14:
			{
			delete iVisWins1;
			iVisWins1=NULL;
			iVisWins1=NewBlueForgroundWindow();

			iVisWins1->Activate();
			iVisWins1->BaseWin()->SetVisible(EFalse);
			iVisWins1->BaseWin()->EnableVisibilityChangeEvents();

			AddExpectedVisibilityChange(TWsVisibilityChangedEvent::ENotVisible);
			}
			break;
		default:
			delete iVisWins1;
			delete iVisWins2;
			iVisWins1 = NULL;
			iVisWins2 = NULL;
			delete iTransWin;
			iTransWin = NULL;
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

void CTEventTest::CheckTimeStamp_NextSetOfEventsL()
	{
	iQueueClient->EventQueue()->SetCheckTimeStamp(ETrue);
	switch(iEventSet++)
		{
	case 0:
		iTest->SimulateKey(TRawEvent::EKeyDown, 'A');	// Create a raw event
		iTest->SimulateKey(TRawEvent::EKeyUp, 'A');	// Create a raw event
		AddExpectedEvent(EEventFocusGained);
		AddExpectedKey(EEventKeyDown,'A');
		AddExpectedKey(EEventKey,'A','a');
		AddExpectedKey(EEventKeyUp,'A','a');
		break;
	case 1:
		{
		_LIT8(KMsgParam,"CheckTimeStamp");
		TBuf8<0x20> params(KMsgParam);
		TInt err = TheClient->iWs.SendMessageToWindowGroup(iQueueClient->WindowGroupIdentifier(),TUid::Uid(123),params);
		AddExpectedEvent(EEventMessageReady);
		}
		break;
	default:
		CActiveScheduler::Stop();
		break;
		}
	TheClient->iWs.Flush();
	}

/**
 * @SYMTestCaseID GRAPHICS-WSERV-0001
 *
 * @SYMPREQ PREQ525
 *
 * @SYMTestCaseDesc Test Capture priority of different top most windows in a Group.
 *
 * @SYMTestPriority Critical
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions Create two different top client windows under one parent window
 *	in a same window group. Add pointer event by clicking various position on the
 *	window and Simulate the pointer event. Change the pointer capture flag, capture
 *	priority and simulate the pointer event.
 *
 * @SYMTestExpectedResults In general the expected event and the window server event should
 *	match same event handle, type and pointer position for following cases.
 *	When pointer capture is disabled and capture priorities are 0, the pointer position on window1
 *	window2 or parent window are same as expected.
 *	When pointer capture is enabled and capture priorities are 0, the pointer position on window2(which is top most now)
 *	is same as expected.
 *	When pointer capture is enabled and capture priorities of win1 is greater than win2
 *	the pointer position on win1 and win2 are same as expected.
 *	The combination of above cases should match the general expexted result.
 *
 */
void CTEventTest::PointerCapture_NextSetOfEventsL()
	{
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KSet,"Pointer Capture SetOfEvents: %d (last=9)");
	logMessageText.Format(KSet,iEventSet);
	INFO_PRINTF1(logMessageText);
#endif
	switch(iEventSet++)
		{
		case 0:			//Create two top client windows of same group
			iVisWins1=new (ELeave) CTBlankWindow;
			iVisWins2=new (ELeave) CTBlankWindow;
			iVisWins1->ConstructL(*iQueueClient->iGroup);
			iVisWins2->ConstructL(*iQueueClient->iGroup);
			iVisWins1->SetColor(KRgbBlue);
			iVisWins1->SetExt(iQueueClient->ChildTopLeft(),iQueueClient->ChildSize());
			iVisWins1->Activate();
			iVisWins2->SetColor(KRgbGreen);
			iVisWins2->SetExt(iQueueClient->ChildTopLeft()+TPoint(15,15),iQueueClient->ChildSize());
			iVisWins2->Activate();
			iQueueClient->iWs.Flush();
			AddExpectedEvent(EEventFocusGained);
			break;
		case 1:			//Pointer capture are disabled and priorities are 0
			iVisWins1->BaseWin()->SetPointerCapture(RWindowBase::TCaptureDisabled);
			iVisWins1->BaseWin()->SetPointerCapturePriority(0);
			iVisWins2->BaseWin()->SetPointerCapture(RWindowBase::TCaptureDisabled);
			iVisWins2->BaseWin()->SetPointerCapturePriority(0);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(35,25));	//Pointer on the first window
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(35,25));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(50,30));	//Pointer on the second window(overlaping on first window)
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(60,50));	//Pointer on the second window
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(75,25));	//Pointer on the parent window
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(75,25));
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,35,25);
			SimulatePointer(TRawEvent::EButton1Up,35,25);
			SimulatePointer(TRawEvent::EButton1Down,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EButton1Down,60,50);
			SimulatePointer(TRawEvent::EButton1Up,60,50);
			SimulatePointer(TRawEvent::EButton1Down,75,25);
			SimulatePointer(TRawEvent::EButton1Up,75,25);
			break;
		case 2:			//Pointer capture are enabled and priorities are 0
			iVisWins1->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins2->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins1->BaseWin()->SetPointerCapturePriority(0);
			iVisWins2->BaseWin()->SetPointerCapturePriority(0);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(35,25),(TInt)iVisWins2);	//Pointer on the first window
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(35,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(50,30));	//Pointer on the second window(overlaping on first window)
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(60,50));	//Pointer on the second window
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(75,25),(TInt)iVisWins2);	//Pointer on the parent window
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(75,25),(TInt)iVisWins2);
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,35,25);
			SimulatePointer(TRawEvent::EButton1Up,35,25);
			SimulatePointer(TRawEvent::EButton1Down,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EButton1Down,60,50);
			SimulatePointer(TRawEvent::EButton1Up,60,50);
			SimulatePointer(TRawEvent::EButton1Down,75,25);
			SimulatePointer(TRawEvent::EButton1Up,75,25);
			break;
		case 3:			//Pointer capture are enabled and priorities of win1 is higher than win2
			iVisWins1->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins2->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins1->BaseWin()->SetPointerCapturePriority(1);
			iVisWins2->BaseWin()->SetPointerCapturePriority(0);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(35,25));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(35,25));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(75,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(75,25),(TInt)iVisWins2);
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,35,25);
			SimulatePointer(TRawEvent::EButton1Up,35,25);
			SimulatePointer(TRawEvent::EButton1Down,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EButton1Down,60,50);
			SimulatePointer(TRawEvent::EButton1Up,60,50);
			SimulatePointer(TRawEvent::EButton1Down,75,25);
			SimulatePointer(TRawEvent::EButton1Up,75,25);
			break;
		case 4:			//Pointer capture are enabled and priorities of win1 is equal to win2
			iVisWins1->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins2->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins1->BaseWin()->SetPointerCapturePriority(1);
			iVisWins2->BaseWin()->SetPointerCapturePriority(1);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(35,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(35,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(75,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(75,25),(TInt)iVisWins2);
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,35,25);
			SimulatePointer(TRawEvent::EButton1Up,35,25);
			SimulatePointer(TRawEvent::EButton1Down,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EButton1Down,60,50);
			SimulatePointer(TRawEvent::EButton1Up,60,50);
			SimulatePointer(TRawEvent::EButton1Down,75,25);
			SimulatePointer(TRawEvent::EButton1Up,75,25);
			break;
		case 5:			//Pointer capture are enabled and priorities of win2 is higher than win1
			iVisWins1->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins2->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins1->BaseWin()->SetPointerCapturePriority(0);
			iVisWins2->BaseWin()->SetPointerCapturePriority(1);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(35,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(35,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(75,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(75,25),(TInt)iVisWins2);
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,35,25);
			SimulatePointer(TRawEvent::EButton1Up,35,25);
			SimulatePointer(TRawEvent::EButton1Down,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EButton1Down,60,50);
			SimulatePointer(TRawEvent::EButton1Up,60,50);
			SimulatePointer(TRawEvent::EButton1Down,75,25);
			SimulatePointer(TRawEvent::EButton1Up,75,25);
			break;
		case 6:			//Pointer capture are enabled for all groups and priorities of win1 is higher than win2
			iVisWins1->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled|RWindowBase::TCaptureFlagAllGroups);
			iVisWins2->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled|RWindowBase::TCaptureFlagAllGroups);
			iVisWins1->BaseWin()->SetPointerCapturePriority(1);
			iVisWins2->BaseWin()->SetPointerCapturePriority(0);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(35,25));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(35,25));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(75,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(75,25),(TInt)iVisWins2);
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,35,25);
			SimulatePointer(TRawEvent::EButton1Up,35,25);
			SimulatePointer(TRawEvent::EButton1Down,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EButton1Down,60,50);
			SimulatePointer(TRawEvent::EButton1Up,60,50);
			SimulatePointer(TRawEvent::EButton1Down,75,25);
			SimulatePointer(TRawEvent::EButton1Up,75,25);
			break;
		case 7:			//Pointer capture are enabled and priorities of iWin is higher than the other windows
			iVisWins1->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins2->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins1->BaseWin()->SetPointerCapturePriority(0);
			iVisWins2->BaseWin()->SetPointerCapturePriority(0);
			iQueueClient->iWin->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iQueueClient->iWin->BaseWin()->SetPointerCapturePriority(1);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(80,20));	//Pointer on the parent window
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(80,20));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(35,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(35,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(75,25));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(75,25));
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,80,20);
			SimulatePointer(TRawEvent::EButton1Up,80,20);
			SimulatePointer(TRawEvent::EButton1Down,35,25);
			SimulatePointer(TRawEvent::EButton1Up,35,25);
			SimulatePointer(TRawEvent::EButton1Down,60,50);
			SimulatePointer(TRawEvent::EButton1Up,60,50);
			SimulatePointer(TRawEvent::EButton1Down,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EButton1Down,75,25);
			SimulatePointer(TRawEvent::EButton1Up,75,25);
			break;
		case 8:			//Pointer capture are enabled and priorities of iWin is 0
			iVisWins1->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins2->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iVisWins1->BaseWin()->SetPointerCapturePriority(1);
			iVisWins2->BaseWin()->SetPointerCapturePriority(0);
			iQueueClient->iWin->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled);
			iQueueClient->iWin->BaseWin()->SetPointerCapturePriority(0);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(80,20),(TInt)iVisWins2);	//Pointer on the parent window
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(80,20),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(35,25));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(35,25));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(75,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(75,25),(TInt)iVisWins2);
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,80,20);
			SimulatePointer(TRawEvent::EButton1Up,80,20);
			SimulatePointer(TRawEvent::EButton1Down,35,25);
			SimulatePointer(TRawEvent::EButton1Up,35,25);
			SimulatePointer(TRawEvent::EButton1Down,60,50);
			SimulatePointer(TRawEvent::EButton1Up,60,50);
			SimulatePointer(TRawEvent::EButton1Down,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EButton1Down,75,25);
			SimulatePointer(TRawEvent::EButton1Up,75,25);
			break;
		case 9:			//Pointer capture are enabled and with different groups
			iVisWins1->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled|RWindowBase::TCaptureFlagAllGroups);
			iVisWins2->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled|RWindowBase::TCaptureFlagAllGroups);
			iVisWins1->BaseWin()->SetPointerCapturePriority(0);
			iVisWins2->BaseWin()->SetPointerCapturePriority(0);
			iQueueClient->iWin->BaseWin()->SetPointerCapture(RWindowBase::TCaptureEnabled|RWindowBase::TCaptureFlagAllGroups);
			iQueueClient->iWin->BaseWin()->SetPointerCapturePriority(1);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(25,150),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(25,150),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(150,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(150,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(80,20));	//Pointer on the parent window
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(80,20));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(35,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(35,25),(TInt)iVisWins2);
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(60,50));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(50,30));
			AddExpectedPointerCapture(TPointerEvent::EButton1Down,TPoint(75,25));
			AddExpectedPointerCapture(TPointerEvent::EButton1Up,TPoint(75,25));
			iQueueClient->iWs.Flush();
			SimulatePointer(TRawEvent::EButton1Down,25,150);
			SimulatePointer(TRawEvent::EButton1Up,25,150);
			SimulatePointer(TRawEvent::EButton1Down,150,25);
			SimulatePointer(TRawEvent::EButton1Up,150,25);
			SimulatePointer(TRawEvent::EButton1Down,80,20);
			SimulatePointer(TRawEvent::EButton1Up,80,20);
			SimulatePointer(TRawEvent::EButton1Down,35,25);
			SimulatePointer(TRawEvent::EButton1Up,35,25);
			SimulatePointer(TRawEvent::EButton1Down,60,50);
			SimulatePointer(TRawEvent::EButton1Up,60,50);
			SimulatePointer(TRawEvent::EButton1Down,50,30);
			SimulatePointer(TRawEvent::EButton1Up,50,30);
			SimulatePointer(TRawEvent::EButton1Down,75,25);
			SimulatePointer(TRawEvent::EButton1Up,75,25);
			break;
		default:
			delete iVisWins1;
			delete iVisWins2;
			iVisWins1=NULL;
			iVisWins2=NULL;
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

/**
PDEF110849
Allocate a pointer buffer, send it some events and then disconnect.
Now fill the event queue, causing a purge.
The purge will call void CWsPointerBuffer::DiscardPointerMoveBuffer(TUint aHandle)
in which an access violation occurred because iCurrentBuffer had not been set to NULL.
*/
void CTEventTest::PointerBufferPurge_NextSetOfEventsL()
	{
#if defined(LOGGING)
	TLogMessageText logMessageText;
	_LIT(KSet,"PointerBufferPurge SetOfEvents: %d of 1");
	logMessageText.Format(KSet,iEventSet);
	TheClient->LogMessage(logMessageText);
#endif
	iTheClientFlush=TheClient->iWs.SetAutoFlush(ETrue);
	iQuequeClientFlush=iQueueClient->iWs.SetAutoFlush(ETrue);
	switch(iEventSet++)
		{
		case 0:
			{
			RWindowBase& win = *iQueueClient->iWin->BaseWin();
			
			// Cause a connect to happen
			RDebug::Print(_L("PointerBufferPurge - AllocPointerMoveBuffer()"));
			win.AllocPointerMoveBuffer(10, 0);
			
			// Allow pen events to be received
			RDebug::Print(_L("PointerBufferPurge - EnablePointerMoveBuffer()"));
			win.EnablePointerMoveBuffer();
			
			// Simulate some pen events
			RDebug::Print(_L("PointerBufferPurge - Events"));
			SimulatePointer(TRawEvent::EPointerMove,10,20);
			SimulatePointer(TRawEvent::EButton1Down,10,20);
			SimulatePointer(TRawEvent::EPointerMove,11,21);
			SimulatePointer(TRawEvent::EButton1Up,11,21);
	
			// Disconnect
			RDebug::Print(_L("PointerBufferPurge - FreePointerMoveBuffer()"));
			win.FreePointerMoveBuffer();
			
			// Send sufficient events to cause a purge to happen
			for (TInt i=0; i<EMaxEventQueueSize; i++)
				{
				RDebug::Print(_L("PointerBufferPurge - Purge Loop %d"), i);
				SimulatePointer(TRawEvent::EButton1Up,15,25);
				}
	
			// Expected events
			// - We only expect the focus and button up events because the other events have been purged
			// - button up events which are not paired with button down take precedence 
			// - but even some of our button up events will have been purged - hence the lower count
			
			AddExpectedEvent(EEventFocusGained);
			
			for (TInt i=0; i<EMaxEventQueueSize-1; i++)
				{
				AddExpectedPointer(TPointerEvent::EButton1Up, TPoint(15-EWinPositionX,25-EWinPositionY));
				}
				
			break;
			}
		default:
			CActiveScheduler::Stop();
		break;
		}
	TheClient->iWs.SetAutoFlush(iTheClientFlush);
	iQueueClient->iWs.SetAutoFlush(iQuequeClientFlush);
	}

/**
 * @SYMTestCaseID GRAPHICS-WSERV-0025
 *
 * @SYMDEF DEF085009
 *
 * @SYMTestCaseDesc Test removal of event handlers mid-way through processing an event.
 *
 * @SYMTestPriority Critical
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions The following tests use RRemovable anim to replicate the removal of
 * event handlers mid-way through processing an event (where an event is offered to each of the
 * listed event handlers).
 *
 * RRemovableAnim is an anim which removes itself from the event handler list
 * after receiving n events, where n is the 'lifetime' of the anim measured in events received.
 * By setting the lifetimes of a number of these anims, we replicate the removal of event handlers
 * mid-way through processing an event.
 *
 * Event set 0:
 *  Sends 2 events e1,e2 and expects them be received by wserv event processing.
 *
 * Event set 1: (Simple scenario)
 *  Adds 3 removable anims to event handler list
 *  Sets lifetimes as {1000,2,5}
 *  Sends 6 events
 *  Expected events to be received by anims: {6,2,5}
 *
 * Event set 2: (Manual removal and reset state)
 *  Manually remove all anims from event handler list
 *  Sends 2 events
 *  No events expected to be received by anims
 *  Resets all anims ready for next test
 *
 * Event set 3: (Removal of multiple anims on receipt of same event)
 *  Adds 10 removable anims in event handler list
 *  Set lifetimes: {1,5,9,3,8,8,8,10,5,2}
 *  Sends 12 events:
 *  Expected number of received events for anims: {1,5,9,3,8,8,8,10,5,2}
 *
 * Event set 4: (Manual removal and reset state)
 *  Manually remove all anims from event handler list
 *  Sends 2 events
 *  No events expected to be received by anims
 *  Resets all anims ready for next test
 *
 * Event set 5: (Addition of anims to non-empty event handler list, part 1)
 *  Adds 6 removable anims in the event handler list
 *  Set lifetimes: {1,1,1,5,5,5}
 *  Sends 2 events:
 *  Expected number of received events for anims: {1,1,1,2,2,2}
 *
 * Event set 6: (Addition of anims to non-empty event handler list, part 2)
 *  Readds first 3 anims to event handler list
 *  Sets lifetimes of these three anims {13,24,6}
 *  Sends 5 events:
 *  Expected number of received events for anims: {5,5,5,3,3,3}
 *
 * Event set 7: (Addition of anims to non-empty event handler list, part 3)
 *  Sends 16 events:
 *  Expected number of received events for anims: {8,16,1,0,0,0}
 *
 * Event set 8: (Manual Removal and reset state)
 *  Manually remove all anims from event handler list
 *  Sends 2 events
 *  No events expected to be received by anims
 *  Resets all anims ready for next test
 *
 * Event set 9: (Creating anims which again generate events.)
 *  Create 3 anims. One anims generates events in its OfferrawEvent function.
 *	Check the funtionality of ProcessRawEvent in Wserv when it is called
 *	recursively. Also check the fix for defect INC095892.
 *
 * @SYMTestExpectedResults
 *  All events should be received by wserv event processing
 *  Each anim should only receive events when (number of events received by anim) <= (lifetime of anim)
 */
void CTEventTest::EventHandlerRemoval_NextSetOfEventsL()
	{
#if defined(LOGGING)
	INFO_PRINTF1(_L("EventHandlerRemoval SetOfEvents: (Max=8)"));
#endif
	switch(iEventSet++)
		{
		case 0:
		#if defined(LOGGING)
			INFO_PRINTF1(_L("AUTO  Event Handler Removal Anim Events"));
		#endif
		#if defined(DETAILED)
			INFO_PRINTF1(_L("No anims in event handler list"));
			INFO_PRINTF1(_L(" Send 2 events"));
			INFO_PRINTF1(_L(" - Expected to be received by wserv event processing"));
		#endif
			// Always get a focus gain at start
			AddExpectedEvent(EEventFocusGained);
			// All expected to be received by wserv event processing
			AddExpectedKey(EEventKeyDown,32,ETrue);
			AddExpectedKey(EEventKeyUp,32,ETrue);
			// Send 2 events - none expected to be received by anims,
			iTest->SimulateKey(TRawEvent::EKeyDown,32);
			iTest->SimulateKey(TRawEvent::EKeyUp,32);
			// Flush events to wserv
			TheClient->iWs.Flush();
			// Create all anims for all tests in first event set because
			// first event set is only place where leaving functions can be called
			EventHandlerRemoval_CreateAnimsL(10);
			break;
		case 1:
			{
		#if defined(DETAILED)
			INFO_PRINTF1(_L(" Add 3 removable anims to event handler list"));
			INFO_PRINTF1(_L(" Set lifetimes for anims: {1000,2,5}"));
			INFO_PRINTF1(_L(" Send 6 events"));
			INFO_PRINTF1(_L(" - Expected number of received events for anims: {6,2,5}"));
			INFO_PRINTF1(_L(" All events expected to be received by wserv event processing"));
		#endif
			const TInt KLifetimes [] = {1000,2,5};
			EventHandlerRemoval_PrepareAnims(KLifetimes, 3);
			EventHandlerRemoval_CreateExpectedEvents(6,3);
			// Flush events to wserv
			TheClient->iWs.Flush();
			// Check each anim has received the expected events
			EventHandlerRemoval_CheckExpectedEventsReceivedByAnims();
			}
			break;
		case 2:
			{
			// Remove all anims, check removal was successful
			// and reset all anims ready for next test
			EventHandlerRemoval_RemoveAndResetAllAnims();
			}
			break;
		case 3:
			{
		#if defined(DETAILED)
			INFO_PRINTF1(_L(" Add 10 removable anims in event handler list"));
			INFO_PRINTF1(_L(" Set lifetimes: {1,5,9,3,8,8,8,10,5,2}"));
			INFO_PRINTF1(_L(" Send 12 events"));
			INFO_PRINTF1(_L(" - Expected number of received events for anims:{1,5,9,3,8,8,8,10,5,2}"));
			INFO_PRINTF1(_L(" All expected to be received by wserv event processing"));
		#endif
			const TInt KLifetimes [] = {1,5,9,3,8,8,8,10,5,2};

			// Create anims, set lifetimes, add to event handlers and add to iRemovableAnims array
			EventHandlerRemoval_PrepareAnims(KLifetimes, 10);
			EventHandlerRemoval_CreateExpectedEvents(12,10);
			// Flush events to wserv
			TheClient->iWs.Flush();
			// Check each anim has received the expected events
			EventHandlerRemoval_CheckExpectedEventsReceivedByAnims();
			}
			break;
		case 4:
			{
			// Remove all anims, check removal was successful
			// and reset all anims ready for next test
			EventHandlerRemoval_RemoveAndResetAllAnims();
			}
			break;
		case 5:
			{
		#if defined(DETAILED)
			INFO_PRINTF1(_L(" Add 6 removable anims in event handler list"));
			INFO_PRINTF1(_L(" Set lifetimes: {1,1,1,5,5,5}"));
			INFO_PRINTF1(_L(" Send 2 events"));
			INFO_PRINTF1(_L(" - Expected number of received events for anims:{1,1,1,2,2,2}"));
			INFO_PRINTF1(_L(" All expected to be received by wserv event processing"));
		#endif
			const TInt KLifetimes [] = {1,1,1,5,5,5};

			// Create anims, set lifetimes, add to event handlers and add to iRemovableAnims array
			EventHandlerRemoval_PrepareAnims(KLifetimes, 6);
			EventHandlerRemoval_CreateExpectedEvents(2, 6);
			// Flush events to wserv
			TheClient->iWs.Flush();
			// Check each anim has received the expected events
			EventHandlerRemoval_CheckExpectedEventsReceivedByAnims();
			}
			break;
		case 6:
			{
		#if defined(DETAILED)
			INFO_PRINTF1(_L(" Readds first 3 removable anims to event handler list"));
			INFO_PRINTF1(_L(" Set lifetimes of these three anims {13,24,6}"));
			INFO_PRINTF1(_L(" Send 5 events"));
			INFO_PRINTF1(_L(" - Expected number of received events for anims: {5,5,5,3,3,3}"));
			INFO_PRINTF1(_L(" All expected to be received by wserv event processing"));
		#endif
			const TInt KLifetimes [] = {13,24,6};

			// Create anims, set lifetimes, add to event handlers and add to iRemovableAnims array
			EventHandlerRemoval_PrepareAnims(KLifetimes, 3);
			EventHandlerRemoval_CreateExpectedEvents(5, 6);
			// Flush events to wserv
			TheClient->iWs.Flush();
			// Check each anim has received the expected events
			EventHandlerRemoval_CheckExpectedEventsReceivedByAnims();
			}
			break;
		case 7:
			{
		#if defined(DETAILED)
			INFO_PRINTF1(_L(" Send 16 events"));
			INFO_PRINTF1(_L(" - Expected number of received events for anims:{8,16,1,0,0,0}"));
			INFO_PRINTF1(_L(" All expected to be received by wserv event processing"));
		#endif
			EventHandlerRemoval_CreateExpectedEvents(16, 3);
			// Flush events to wserv
			TheClient->iWs.Flush();
			// Check each anim has received the expected events
			EventHandlerRemoval_CheckExpectedEventsReceivedByAnims();
			}
			break;
		case 8:
			{
			// Remove all anims, check removal was successful
			// and reset all anims ready for next test
			EventHandlerRemoval_RemoveAndResetAllAnims();
			}
			break;
		case 9:
			EventHandlerRemoval_CheckRecursionOfProcessRawEvent();
			break;
		case 10:
			EventHandlerRemoval_AddEventHandlerMultipleTimes();
			break;
		default:
		#if defined(DETAILED)
			INFO_PRINTF1(_L(" Test postamble"));
			INFO_PRINTF1(_L(" Destroy anims"));
		#endif
			// Destroy anims
			EventHandlerRemoval_DestroyAllAnims();
			iRemovableAnims->Close();
			delete iRemovableAnims;
			iRemovableAnims = NULL;
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}

/**
 * Creates an array of aAnimCount anims on heap
 * Assigns array to iRemovableAnims for use in rest of tests
 */
void CTEventTest::EventHandlerRemoval_CreateAnimsL(TInt aAnimCount)
	{
	RPointerArray<RRemovableAnim>* removableAnims = new (ELeave) RPointerArray<RRemovableAnim>();
	CleanupStack::PushL(removableAnims);
	for (TInt animIndex = 0; animIndex < aAnimCount; animIndex++)
		{
		RRemovableAnim* anim = RRemovableAnim::NewLC(iQueueClient->iWin->BaseWin(), iAnimDll);
		removableAnims->AppendL(anim);
		}
	CleanupStack::Pop(aAnimCount, (*removableAnims)[0]);
	CleanupStack::Pop(removableAnims);
	iRemovableAnims = removableAnims;
	}

/**
 * Sets lifetimes of anims as specified in aLifeTimes
 * Adds anims to event handler list
 */
void CTEventTest::EventHandlerRemoval_PrepareAnims(const TInt* aLifetimes, TInt aAnimCount)
	{
	for (TInt animIndex = 0; animIndex < aAnimCount; animIndex++)
		{
		RRemovableAnim* anim = (*iRemovableAnims)[animIndex];
		anim->SetEventHandlerLifetime(aLifetimes[animIndex]);
		anim->AddToEventHandlers();
		}
	}

/**
 * Creates aEventCount key events to be used in test
 * Adds the expected event aEvent to the first aLiveAnimCount anims
 * Adds expected events observed by the rest of wserv
 * Simulates events on wserv
 */
void CTEventTest::EventHandlerRemoval_CreateExpectedEvents(TInt aEventCount, TInt aLiveAnimCount)
	{
	RArray<TRawEvent> events;

	// Add expected events to anims
	for (TInt eventCount = 0; eventCount < aEventCount; eventCount++)
		{
		TRawEvent rawEvent;
		TInt scanCode = 33 + eventCount;
		// Alternate key down, key up events.
		if (eventCount%2 == 0)
			{
			rawEvent.Set(TRawEvent::EKeyDown, scanCode);
			}
		else
			{
			rawEvent.Set(TRawEvent::EKeyUp, scanCode);
			}
 		EventHandlerRemoval_AddExpectedEvent(rawEvent, aLiveAnimCount);
		events.Append(rawEvent);
		}

	// Simulates expected events
	for (TInt eventCount = 0; eventCount < aEventCount; eventCount++)
		{
		TRawEvent event = events[eventCount];
		iTest->SimulateKey(event.Type(), event.ScanCode());
		}

	events.Close();
	}

/**
 * Adds the expected event aEvent to the first aLiveAnimCount anims
 * Adds expected event observed by the rest of wserv
 */
void CTEventTest::EventHandlerRemoval_AddExpectedEvent(TRawEvent aEvent, TInt aLiveAnimCount)
	{
	// Add event to each of the anims
	for (TInt animIndex = 0; animIndex < aLiveAnimCount; animIndex++)
		{
		TInt err = (*iRemovableAnims)[animIndex]->AddExpectedEvent(aEvent);
		}
	TInt eventType = (aEvent.Type() == TRawEvent::EKeyDown ? EEventKeyDown : EEventKeyUp);
	AddExpectedKey(eventType, aEvent.ScanCode(), ETrue);
	}

/**
 * Checks each anim has received the expected events
 */
void CTEventTest::EventHandlerRemoval_CheckExpectedEventsReceivedByAnims()
	{
	for (TInt animIndex = iRemovableAnims->Count() - 1; animIndex >= 0; animIndex--)
		{
		if (!(*iRemovableAnims)[animIndex]->TestPassed())
			{
			Failed();
			break;
			}
		}
	}


/**
 * Create three anims. In the first anim's Offer raw event generate two more events
 * which will call recursively the process raw event at the server side.
 * The second and third anims are just removable anims. To simulate the scenario 
 * for the defect INC095892. If teh fix for this is defect is not supplied then
 * wserv would panic for this test case.
 */
void CTEventTest::EventHandlerRemoval_CheckRecursionOfProcessRawEvent()
	{
	iTheClientFlush=TheClient->iWs.SetAutoFlush(ETrue);
	iQuequeClientFlush=iQueueClient->iWs.SetAutoFlush(ETrue);
	
	// Create 3 anims	
	RRemovableAnim* eventRecurAnim = RRemovableAnim::NewLC(iQueueClient->iWin->BaseWin(), iAnimDll);
	RRemovableAnim* removableAnim = RRemovableAnim::NewLC(iQueueClient->iWin->BaseWin(), iAnimDll);
	RRemovableAnim* eventAnim = RRemovableAnim::NewLC(iQueueClient->iWin->BaseWin(), iAnimDll);

	// Add these anim as event handlers and set thier respective life time
	eventRecurAnim->AddToEventHandlers();
	eventRecurAnim->SetEventHandlerLifetime(4);
	
	removableAnim->AddToEventHandlers();
	removableAnim->SetEventHandlerLifetime(1);
	
	eventAnim->AddToEventHandlers();
	eventAnim->SetEventHandlerLifetime(2);

	TRawEvent rawEvent1;
	TRawEvent rawEvent2;
	TRawEvent rawEvent3;
	rawEvent1.Set(TRawEvent::EKeyDown, 60);
	rawEvent2.Set(TRawEvent::EKeyUp, 34);
	rawEvent3.Set(TRawEvent::EKeyUp, 35);
	
	eventRecurAnim->AddExpectedEvent(rawEvent2);
	eventRecurAnim->AddExpectedEvent(rawEvent3);
	
	removableAnim->AddExpectedEvent(rawEvent2);
	
	eventAnim->AddExpectedEvent(rawEvent2);
	eventAnim->AddExpectedEvent(rawEvent3);

	// Add these events to iQueueClient for testing	
	AddExpectedKey(EEventKeyUp, 34, ETrue);
	AddExpectedKey(EEventKeyUp, 35, ETrue);

	iTest->SimulateKey(rawEvent1.Type(), rawEvent1.ScanCode());
	
	CleanupStack::PopAndDestroy(3, eventRecurAnim);
	
	TheClient->iWs.SetAutoFlush(iTheClientFlush);
	iQueueClient->iWs.SetAutoFlush(iQuequeClientFlush);
	}

/**
 * Manually removes all anims from event handlers array
 */
void CTEventTest::EventHandlerRemoval_ManuallyRemoveAllAnims()
	{
	// Manually remove all anims from event handler list
	for (TInt animIndex = iRemovableAnims->Count() - 1; animIndex >= 0; animIndex--)
		{
		(*iRemovableAnims)[animIndex]->RemoveFromEventHandlers();
		}
	}

/**
 * Manually removes all anims from event handlers array
 */
void CTEventTest::EventHandlerRemoval_ResetAllAnims()
	{
	// Manually remove all anims from event handler list
	for (TInt animIndex = iRemovableAnims->Count() - 1; animIndex >= 0; animIndex--)
		{
		(*iRemovableAnims)[animIndex]->Reset();
		(*iRemovableAnims)[animIndex]->SetEventHandlerLifetime(0);
		}
	}

/**
 *  Manually removes all anims from event handler list
 *  Sends 2 events
 *  Checks that no events have been received by anims
 *  Resets all anims ready for next test
 */
void CTEventTest::EventHandlerRemoval_RemoveAndResetAllAnims()
	{
#if defined(DETAILED)
	INFO_PRINTF1(_L(" Manually remove all anims from event handler list"));
	INFO_PRINTF1(_L(" Send 2 events: none expected to be received by anims"));
	INFO_PRINTF1(_L(" All expected to be received by wserv event processing"));
#endif
	// Manually remove all anims from event handler list
	EventHandlerRemoval_ManuallyRemoveAllAnims();
	// All expected to be received by wserv event processing
	AddExpectedKey(EEventKeyDown,32,ETrue);
	AddExpectedKey(EEventKeyUp,32,ETrue);
	// Send 2 events - none expected to be received by anims,
	iTest->SimulateKey(TRawEvent::EKeyDown,32);
	iTest->SimulateKey(TRawEvent::EKeyUp,32);
	// Flush events to wserv
	TheClient->iWs.Flush();
	// Check the anims have not received any events
	EventHandlerRemoval_CheckExpectedEventsReceivedByAnims();
#if defined(DETAILED)
	INFO_PRINTF1(_L(" Cleanup before next step"));
#endif
	// Reset anims
	EventHandlerRemoval_ResetAllAnims();
	}

/**
 * Destroys all anims in iRemovableAnims
 * Resets iRemovableAnims
 */
void CTEventTest::EventHandlerRemoval_DestroyAllAnims()
	{
	if (iRemovableAnims)
		{
		// Destroy anims
		for (TInt animIndex = iRemovableAnims->Count() - 1; animIndex >= 0; animIndex--)
			{
			RRemovableAnim* anim = (*iRemovableAnims)[animIndex];
			if (anim)
				{
				anim->Destroy();
				}
			}
		iRemovableAnims->Reset();
		}
	}

/**
SYMTestCaseID			GRAPHICS-WSERV-0496

@SYMDEF					DEF133776

@SYMTestCaseDesc		Cancels key captures using all combinations of 
 RWindowGroup::Capture...() and RWindowGroup::Cancel...() APIs.

@SYMTestPriority 		High

@SYMTestStatus 			Implemented

@SYMTestActions 		Call each of the RWindowGroup::Capture***() APIs followed by
 each of the RWindowGroup::CancelCapture***() APIs.
 RWindowGroup::Capture***() APIs:
 	CaptureKey(TUint, TUint, TUint)
 	CaptureKey(TUint, TUint, TUint, TInt)
 	CaptureKeyUpAndDowns(TUint, TUint, TUint)
 	CaptureKeyUpAndDowns(TUint, TUint, TUint, TInt)
 	CaptureLongKey(TUint, TUint, TUint, TUint, TInt, TUint)
  	CaptureLongKey(TTimeIntervalMicroSeconds32, TUint, TUint, TUint, TUint, TInt, TUint)
 RWindowGroup::CancelCapture***() APIs:
	CancelCaptureKey()
	CancelCaptureKeyUpAndDowns()
	CancelCaptureLongKey()

@SYMTestExpectedResults WServ should handle matched and mismatched Cancel 
 and Capture calls without panicking.
 */
void CTEventTest::MismatchedCapture_NextSetOfEventsL()
	{
	if (iEventSet == 0)
		AddExpectedEvent(EEventFocusGained);
	
	if (iEventSet < (EMaxCaptureKeyApis*EMaxCancelCaptureKeyApis))
		{
		TestCaptureAndCancelCapturePair(static_cast<TCaptureKeyApis>(iEventSet%EMaxCaptureKeyApis), static_cast<TCancelCaptureKeyApis>(iEventSet/EMaxCaptureKeyApis));
		iEventSet++;
		}	
	else
		{
		CActiveScheduler::Stop();				
		}
	}

/** Helper function that applies a key capture and then cancels it using the
passed capture and cancel RWindowGroup APIs.

@param aCaptureApi Enum value specifying the capture API to use
@param aCancelCaptureApi Enum value specifying the cancel capture API to use
 */
void CTEventTest::TestCaptureAndCancelCapturePair(TCaptureKeyApis aCaptureApi, TCancelCaptureKeyApis aCancelCaptureApi)
	{		
#if defined(LOGGING)
	_LIT(KCaptureKey, "CaptureKey");
	_LIT(KCaptureKeyKeyUpAndDowns, "CaptureKeyKeyUpAndDowns");
	_LIT(KCaptureLongKey, "CaptureLongKey");	
	_LIT(KCancelCaptureKey, "CancelCaptureKey");
	_LIT(KCancelCaptureKeyKeyUpAndDowns, "CancelCaptureKeyKeyUpAndDowns");
	_LIT(KCancelCaptureLongKey, "CancelCaptureLongKey");
	_LIT(KLog,"TestCaptureAndCancelCapturePair: %S, %S");
	TPtrC captures[EMaxCaptureKeyApis] = {KCaptureKey(), KCaptureKey(), KCaptureKeyKeyUpAndDowns(), KCaptureKeyKeyUpAndDowns(), KCaptureLongKey(), KCaptureLongKey()};
	TPtrC cancelCaptures[EMaxCancelCaptureKeyApis] = {KCancelCaptureKey(), KCancelCaptureKeyKeyUpAndDowns(), KCancelCaptureLongKey()};
	TLogMessageText logMessageText;	
	logMessageText.Format(KLog, &captures[aCaptureApi], &cancelCaptures[aCancelCaptureApi]);
	INFO_PRINTF1(logMessageText);
#endif // LOGGING	
	
	// Start a key capture using one of the six RWindowGroup Capture***() APIs
	switch (aCaptureApi)
		{
		case ECaptureKey1:
		case ECaptureKey2:
			// Test RWindowGroup::CaptureKey()
			if (aCaptureApi == ECaptureKey1) 				
				iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureKey('A',0,0);
			else
				iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureKey('A',0,0,1); // priority 1 (default priority is 0)
			iTest->SimulateKey(TRawEvent::EKeyDown, 'A');
			TheClient->iWs.Flush();
			User::After(KeyRepeatTime()*3/2);
			iTest->SimulateKey(TRawEvent::EKeyUp, 'A');
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKeyDown, 'A');
			AddExpectedKey(EEventKey,'A','a');
			AddExpectedKey(EEventKey,'A','a',1);			
			AddExpectedKey(EEventKeyUp, 'A');
			break;				
			
		case ECaptureKeyUpAndDowns1:
		case ECaptureKeyUpAndDowns2:
			// Test RWindowGroup::CaptureKeyUpAndDowns()
			if (aCaptureApi == ECaptureKeyUpAndDowns1)			
				iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureKeyUpAndDowns(EStdKeySpace,0,0);
			else
				iCaptureKey=iQueueClient->iGroup->GroupWin()->CaptureKeyUpAndDowns(EStdKeySpace,0,0,1); // priority 1 (default priority is 0)
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeySpace);
			TheClient->iWs.Flush();			
			User::After(KeyRepeatTime()*3/2);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKey,EStdKeySpace,' ');
			AddExpectedKey(EEventKey,EStdKeySpace,' ',1);
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			break;	
			
		case ECaptureLongKey1:
		case ECaptureLongKey2:
			{
			// Test RWindowGroup::CaptureLongKey()
			TTimeIntervalMicroSeconds32 repeat = KeyRepeatTime();
			if (aCaptureApi == ECaptureLongKey1)
				{
				iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureLongKey(' ', 'a', 0, 0, 2, ELongCaptureNormal);	
				}
			else
				{
				repeat = KeyRepeatTime()/2;
				iCaptureKey = iQueueClient->iGroup->GroupWin()->CaptureLongKey(repeat, ' ', 'a', 0, 0, 2, ELongCaptureNormal);
				}					
			iQueueClient->iWs.Flush();
			iTest->SimulateKey(TRawEvent::EKeyDown,EStdKeySpace);
			TheClient->iWs.Flush();
			User::After(KeyRepeatTime()*3/2);
			iTest->SimulateKey(TRawEvent::EKeyUp,EStdKeySpace);
			TheClient->iWs.Flush();
			AddExpectedKey(EEventKeyDown,EStdKeySpace);
			AddExpectedKey(EEventKey,EStdKeySpace,' ');
			AddExpectedKey(EEventKey,EStdKeySpace,'a',1);
			AddExpectedKey(EEventKeyUp,EStdKeySpace);
			}
			break;
				
		default:
			INFO_PRINTF1(_L("Unexpected TCaptureKeyApis value passed to CTEventTest::TestCaptureAndCancelCapturePair()"));
			TEST(EFalse);
			break;
		}	
	
	if (iCaptureKey)
		{
		// End a capture using one of the three RWindowGroup CancelCapture***() APIs,
		// for each Capture***() API each one of these will be called, therefore there will
		// be one correct CancelCapture***() APIs called and two incorrect CancelCapture***() 
		// APIs called for each Capture***() API.
		switch (aCancelCaptureApi)
			{
			case ECancelCaptureKey:
				iQueueClient->iGroup->GroupWin()->CancelCaptureKey(iCaptureKey);
				break;
			case ECancelCaptureKeyUpAndDowns:
				iQueueClient->iGroup->GroupWin()->CancelCaptureKeyUpAndDowns(iCaptureKey);
				break;
			case ECancelCaptureLongKey:
				iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
				break;
			default:
				INFO_PRINTF1(_L("Unexpected TCancelCaptureKeyApis value passed to CTEventTest::TestCaptureAndCancelCapturePair()"));
				TEST(EFalse);
				break;
			}
		
		// If an mistmatched CancelCapture***() API was called, call the correct one now
		// to make sure the Capture/CancelCapture state is clean before the next test starts
		if ((aCaptureApi/2) != aCancelCaptureApi)
			{
			switch (aCaptureApi)				
				{
				case ECaptureKey1:
				case ECaptureKey2:
					iQueueClient->iGroup->GroupWin()->CancelCaptureKey(iCaptureKey);
					break;
				case ECaptureKeyUpAndDowns1:
				case ECaptureKeyUpAndDowns2:
					iQueueClient->iGroup->GroupWin()->CancelCaptureKeyUpAndDowns(iCaptureKey);
					break;
				case ECaptureLongKey1:
				case ECaptureLongKey2:
					iQueueClient->iGroup->GroupWin()->CancelCaptureLongKey(iCaptureKey);
					break;
				}
			}
		}
		
	TheClient->iWs.Flush();
	}	

/*void CTEventTest::_NextSetOfEvents()		//Template for these functions
	{
	switch(iEventSet++)
		{
		case 0:
			iQueueClient->iWin->WinTreeNode()->;		//Set up window
			iQueueClient->iWin->BaseWin()->;
			iQueueClient->iGroup->GroupWin()->;
			iQueueClient->iWs.Flush();					//Make take effect
			SimulateEvent(TRawEvent::);					//Create an event
			TheClient->iWs.Flush();						//Send event
			AddExpectedEvent(EEventFocusGained);		//Always get a focus gain at start
			AddExpectedEvent();							//Expect the event that was created above
			break;
		case 1:
			iQueueClient->iWs.Flush();
			SimulateEvent(TRawEvent::);
			TheClient->iWs.Flush();
			AddExpectedEvent();
			break;
		default:
			CActiveScheduler::Stop();
			break;
		}
	TheClient->iWs.Flush();
	}*/

/**
	@SYMTestCaseID GRAPHICS-WSERV-CODEBASE-WSERV-0051
  
	@SYMPREQ PGM027
  
	@SYMTestCaseDesc Tests SetKeyboardRepeatRate API. 
   
	@SYMTestPriority 1 
  
	@SYMTestStatus Implemented
   
	@SYMTestActions This test tryes to call SetKeyboardRepeatRate with invalid time values\n
			
	@SYMTestExpectedResults KeyRepeatRateNegTest should tests the API SetKeyboardRepeatRate return value\n
	The test should pass and API should return KErrArgument.

 */
void CTEventTest::KeyRepeatRateNegTest(TInt aInitialRepeat, TInt aRepeat)
	{
	((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-CODEBASE-WSERV-0051"));
	TInt ret=TheClient->iWs.SetKeyboardRepeatRate(TTimeIntervalMicroSeconds32(aInitialRepeat), TTimeIntervalMicroSeconds32(aRepeat));
	TEST(ret==KErrArgument);
	TRAPD(err,((CTEventTestStep*)iStep)->RecordTestResultL());
   	if (err!=KErrNone)
    		INFO_PRINTF1(_L("Failed to record test result"));
	}

void CTEventTest::RunTestsL(TBool aNeedChildWindows/*=EFalse*/)
	{
	iQueueClient=new(ELeave) CTQueueClient(this);
	iQueueClient->SetScreenNumber(iTest->iScreenNumber);
	iQueueClient->ConstructL(aNeedChildWindows);
	iEventSet=0;
	TRAPD(err,NextSetOfEventsL());
	if (err!=KErrNone)
		{
		iFailed=ETrue;
		return;
		}
//
	CActiveScheduler::Start();
//
	TInt eventsLeft=iQueueClient->EventQueue()->EventsLeft();
	if (eventsLeft>0)
		iFailed=ETrue;
	delete iVisWins1;
	iVisWins1 = NULL;
	delete iVisWins2;
	iVisWins2 = NULL;
	delete iQueueClient;
	iQueueClient=NULL;
	}

void CTEventTest::RunTestsRestoreAreaL(TBool aNeedChildWindows)
	{
	TRect area(TheClient->iWs.PointerCursorArea());
	RunTestsL(aNeedChildWindows);
	TheClient->iWs.SetPointerCursorArea(area);
	}

/*void CTEventTest::QueuePurgingL()
	{
	RunTestsL(EFalse);
	}

void CTEventTest::SendEventL()
	{
	RunTestsL(EFalse);
	}

void CTEventTest::InvisibleWindowL()
	{
	RunTestsL(ETrue);
	}

void CTEventTest::JavaAdditionsL()
	{
	RunTestsL(EFalse);
	}*/

void CTEventTest::WindowCapBugL()
	{
	RunTestsL(ETrue);
	}

/*void CTEventTest::XyInputTypeL()
	{
	RunTestsL(EFalse);
	}

void CTEventTest::MovingPointerCursorL()
	{
	RunTestsRestoreAreaL(ETrue);
	}

void CTEventTest::RotatedModeL()
	{
	RunTestsRestoreAreaL(EFalse);
	}*/

/**
* This test creates one anim and calls AddToEventHandlers() twenty times.
* it shouldn't add the same anim to event handler if anim is already in handler.
*/
void CTEventTest::EventHandlerRemoval_AddEventHandlerMultipleTimes()
	{
	iTheClientFlush=TheClient->iWs.SetAutoFlush(ETrue);
	iQuequeClientFlush=iQueueClient->iWs.SetAutoFlush(ETrue);

	RRemovableAnim* eventTestAnim = RRemovableAnim::NewLC(iQueueClient->iWin->BaseWin(), iAnimDll);

	// Call AddToEventHandlers() on same anim 20 times, it should add this anim in event handler only once
	for (TInt count = 0; count < 20; count++)
		eventTestAnim->AddToEventHandlers();
	eventTestAnim->SetEventHandlerLifetime(1);

	TRawEvent rawEvent1;
	rawEvent1.Set(TRawEvent::EKeyUp, 34);

	eventTestAnim->AddExpectedEvent(rawEvent1);

	// Add these events to iQueueClient for testing
	AddExpectedKey(EEventKeyUp, 34, ETrue);

	iTest->SimulateKey(TRawEvent::EKeyUp, 34);

	CleanupStack::PopAndDestroy(eventTestAnim);
	TheClient->iWs.SetAutoFlush(iTheClientFlush);
	iQueueClient->iWs.SetAutoFlush(iQuequeClientFlush);
	}

void CTEventTest::EventQueuePtrCrashTest()
// Code designed to hit the defect reported in INC109199 & INC105430, needs a very specific sequence of
// events and client disconnections to hit the exact problem state.
	{
	const TInt KNumTestEvents=6;
	const TInt KMaxNumTestEvents=16;
	// Number of iterations needs to be as high as the queue granularity to make sure at least
	// once around we actually hit the code to change the queue size
	const TInt KNumEvTestIterations=4;
	const TInt KNumBaseTestSessions=KNumEvTestIterations+1;
	const TInt KNumMainTestSessions=13;
	const TInt KMaxNumSessions=KNumBaseTestSessions+KNumMainTestSessions+KNumEvTestIterations;
	RWsSession wsSessions[KMaxNumSessions];
	RWindowGroup wGroups[KMaxNumSessions];
	for(TInt iterationLoop=0;iterationLoop<KNumEvTestIterations;iterationLoop++)
		{
		const TInt numTestSessions=KNumBaseTestSessions+KNumMainTestSessions+iterationLoop;
	#if defined(LOGGING)
		_LIT(KLog1,"EventQueuePtrCrashTest: Test Creating %d Clients");
		LOG_MESSAGE2(KLog1,numTestSessions);
	#endif
		for(TInt testSessionIndex=0;testSessionIndex<numTestSessions;testSessionIndex++)
			{
			RWsSession& ws=wsSessions[testSessionIndex];
			User::LeaveIfError(ws.Connect());
			wGroups[testSessionIndex]=RWindowGroup(ws);
			RWindowGroup& group=wGroups[testSessionIndex];
			User::LeaveIfError(group.Construct(testSessionIndex+1,EFalse)); 
			if (testSessionIndex>=KNumBaseTestSessions)
				{
				TWsEvent event;
				for(TInt evLoop=0;evLoop<KNumTestEvents;evLoop++)
					{
					event.SetType(1001+evLoop);
					ws.SendEventToWindowGroup(group.Identifier(),event);
					}
				if (testSessionIndex==(numTestSessions-1))
					{
					for(TInt eventIndex=0;eventIndex<KMaxNumTestEvents;eventIndex++)
						{
						event.SetType(1001+eventIndex);
						ws.SendEventToAllWindowGroups(event);
						}
					}
				}
			}
	#if defined(LOGGING)
		_LIT(KLog2,"EventQueuePtrCrashTest: Closing Clients");
		LOG_MESSAGE(KLog2);
	#endif
		TInt closeLoop=0;
		while(closeLoop<KNumBaseTestSessions)
			{
			wsSessions[closeLoop++].Close();
			}
		RWsSession extraSession1;
		User::LeaveIfError(extraSession1.Connect());
		extraSession1.Close();
		while(closeLoop<numTestSessions)
			{
			wsSessions[closeLoop++].Close();
			}
		}
	}

/**
Test queue size of the new queue can be initialized to be at least EMinQueueSize.
@param aEventReadyCancel whether there is inactive client. This parameter can be specified
		to test different code path for expanding spaces for new queue. If the value is false, 
		the space for the new queue is gained from current global queue or by growing global queue size. 
		Otherwise, there are inactive clients which	cancel their listening to event, in addition, 
		memory allocation is simulated to be failed when growing global queue size, 
		so that space is gained by purging events from inactive clients. 
*/
void CTEventTest::InitializeQueueSizeTestL(TBool aEventReadyCancel)
	{
	const TInt numEvents = 50;
	const TInt maxClients = 3; 
	const TInt oneSecond = 1000000;
	RWsSession wsSession[maxClients];
	RWindowGroup groupWin[maxClients];
	TRequestStatus status;
	TInt cliHanGrpWin = 10000;
	RTimer timer;
	timer.CreateLocal();
	CleanupClosePushL(timer);
	// To have the test creating different number of clients is to make sure
	// there is at least one of the tests meets the below requirement:
	// the number of the connections when the new queue created for test is not multiple
	// of queue granularity, so that the queue size initialization is not done by growing
	// global queue size due to the increase of the connections.
	for (TInt numClients = maxClients - 1; numClients <= maxClients; numClients++)
		{
		TInt clientIndex;
		//Create test clients
		for (clientIndex = 0; clientIndex < numClients; clientIndex++)
			{
			User::LeaveIfError(wsSession[clientIndex].Connect());
			CleanupClosePushL(wsSession[clientIndex]);
			groupWin[clientIndex] = RWindowGroup(wsSession[clientIndex]);
			User::LeaveIfError(groupWin[clientIndex].Construct(cliHanGrpWin++));
			CleanupClosePushL(groupWin[clientIndex]);
			}
		
		// Cancel listening to event for the first client. 
		// This is for testing PurgeInactiveEvents. 
		if (aEventReadyCancel)
			{
			wsSession[0].EventReady(&status);
			wsSession[0].EventReadyCancel();
			User::WaitForRequest(status);
			}

		//send events to window group to make the queue full.
		TWsEvent event;
		TInt evLoop;
		for(evLoop = 0; evLoop < numEvents; evLoop++)
			{
			event.SetType(2001 + evLoop);
			TheClient->iWs.SendEventToWindowGroup(groupWin[0].Identifier(),event);
			TheClient->iWs.SendEventToWindowGroup(groupWin[1].Identifier(),event);
			}
		
		//To create a new client, although the queue is full, wserv will guarantee to initialize 
		//the queue size of the new queue to at least EMinQueueSize. So that it can response to 
		//the event.
		RWsSession testWsSession;
		RWindowGroup testGroupWin;
		TRequestStatus testStatus;
		if (aEventReadyCancel)
			{
			//Simulate the heap allocation failure to test expanding spaces by purge 
			//events from inactive clients when initialize queue size.
			TInt failAt = 1;
			TInt err;
			do	{
				TheClient->iWs.HeapSetFail(RHeap::EFailNext, failAt++);
				TheClient->iWs.Flush();
				err = testWsSession.Connect();
				TheClient->iWs.HeapSetFail(RHeap::ENone, 1);
				} while (err == KErrNoMemory);
			User::LeaveIfError(err);
			}
		else
			{
			User::LeaveIfError(testWsSession.Connect());
			}
		CleanupClosePushL(testWsSession);
		testGroupWin = RWindowGroup(testWsSession);
		User::LeaveIfError(testGroupWin.Construct(cliHanGrpWin++));
		CleanupClosePushL(testGroupWin);
	
		//Send events to the newly created client.
		// The Debug version of WServ puts a EEventFocusGained into the event 
		// before this test's custom event, so leave one slot in the queue 
		// for it.
#if (defined _DEBUG)
		TInt testEventCount = EMinQueueSize - 1; 
#else
		TInt testEventCount = EMinQueueSize; 
#endif
		for(TInt evLoop = 0; evLoop < testEventCount; evLoop++)
			{
			event.SetType(3001 + evLoop);
			TheClient->iWs.SendEventToWindowGroup(testGroupWin.Identifier(),event);
			}
		
		//Check the event queue.
		//Having a timer here to avoid infinitely wait for event.
		TInt expectedEvent = 3001;
		for(TInt evLoop = 0; evLoop < EMinQueueSize; evLoop++)
            {
			testWsSession.EventReady(&testStatus);
			TRequestStatus timerStatus;
			timer.After(timerStatus, oneSecond);
			User::WaitForRequest(testStatus, timerStatus);
			TEST(testStatus == 0);
			if (testStatus == 0) 
				{
				// Tests the event
				testWsSession.GetEvent(event);
				// WServ sometimes puts a EEventFocusGained event into the queue 
				// before the test's custom event, so ignore it.
				if (event.Type() != EEventFocusGained)
					{
					TEST(event.Type() == expectedEvent);
					expectedEvent++;
					}
				// testStatus has been completed. Cancel the timer.
				timer.Cancel();
				User::WaitForRequest(timerStatus);
				}
			else
				{
				// Times out, cancel the event notification
				testWsSession.EventReadyCancel();
				User::WaitForRequest(testStatus);
				}
			}
		CleanupStack::PopAndDestroy(2*numClients + 2, &wsSession[0]);
		}
	CleanupStack::PopAndDestroy(&timer);
	}

void CTEventTest::SimulateRepeatEvent(TInt aScanCode, TInt aRepeats/*=0*/)
    {
    TRawEvent rawEvent;
    if (aRepeats)
        rawEvent.Set(TRawEvent::EKeyRepeat, aScanCode, aRepeats);
    else
        rawEvent.Set(TRawEvent::EKeyRepeat, aScanCode);
    UserSvr::AddEvent(rawEvent);
    }

void CTEventTest::RawEventRepeatTest_NextSetOfEventsL()
    {
    switch(iEventSet++)
        {
        case 0:
            // Tests EKeyRepeat without repeat value set.
            SimulateRepeatEvent(32);
            
            // Tests EKeyRepeat with repeat value set to 2.
            SimulateRepeatEvent(33,2);
          
            AddExpectedEvent(EEventFocusGained);
            AddExpectedKey(EEventKey, 32);
            AddExpectedKey(EEventKey, 33, 0, 2);
            break;
        default:
             CActiveScheduler::Stop();
             break;
        }
    TheClient->iWs.Flush();
    }

void CTEventTest::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest0,"General 1");
	_LIT(KTest1,"Event queue purging");
	_LIT(KTest2,"SendEvent");
	_LIT(KTest3,"SendEventToAll");
	_LIT(KTest4,"InvisibleWindow");
	_LIT(KTest5,"JavaAdditions");
	_LIT(KTest6,"WindowCaptureBug");
	_LIT(KTest7,"XYInput Types");
	_LIT(KTest8,"MovingPointerCursor");
	_LIT(KTest9,"Rotated Modes");
	_LIT(KTest10,"Anim Event");
	_LIT(KTest11,"Focus Changed");
	_LIT(KTest12,"On/Off Events");
	_LIT(KTest13,"Virtual Keyboard");
	_LIT(KTest14,"Key Clicks");
	_LIT(KTest15,"Capture Long");
	_LIT(KTest16,"Password Window");
	_LIT(KTest17,"Group List Changed");
	_LIT(KTest18,"Repeatable Key Events");
	_LIT(KTest19,"Screen Scaling");
	_LIT(KTest20,"Visibility Changed Events");
	_LIT(KTest21,"Check Time Stamp Of Events");
	_LIT(KTest22,"Pointer Capture Priority Events");
	_LIT(KTest23,"Event Handler Removal");
	_LIT(KTest24,"Event Queue ptr crash test");
	_LIT(KTest25,"Mismatched Pointer Events");
	_LIT(KTest26,"Pointer Buffer Purge");
	_LIT(KTest27,"TRawEvent test for Repeats");
#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA 
	_LIT(KTest28,"Translation of key events by routing plug-in");
	_LIT(KTest29,"Blocking of key events by routing plug-in");
	_LIT(KTest30,"App UID based restriction of key capture");
	_LIT(KTest31,"App UID based routing of key events");
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS            
	_LIT(KTest32,"Transparent Surface Visibility Changed Events 1");
	_LIT(KTest33,"Transparent Surface Visibility Changed Events 2");
	_LIT(KTest34,"Transparent Surface Visibility Changed Events 3");
#endif // SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	_LIT(KTest35A,"Initialize Queue Size without inactive queue");
	_LIT(KTest35B,"Initialize Queue Size with inactive queue");	
	
	if (!TestBase()->ConfigurationSupportsPointerEventTesting())
	    {
	    INFO_PRINTF1(_L("Test skipped because config does not support pointer event testing"));
	    TestComplete();
	    return;
	    }
	
	//if (iTest->iState==1) iTest->iState=14;	//Use this line to skip to a particular test
	((CTEventTestStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0053

@SYMDEF             DEF081259

@SYMTestCaseDesc    Carry out general event tests on WServ

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     General events are sent via WServ

@SYMTestExpectedResults The events are sent without error
*/
		case 0:
			iTest->LogSubTest(KTest0);
			General();
			((CTEventTestStep*)iStep)->SetOverallTestStepID(_L("GRAPHICS-WSERV-0053"));
			//iState=8;
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0054

@SYMDEF             DEF081259

@SYMTestCaseDesc    Carry out event queue purging

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Purge the event queue

@SYMTestExpectedResults The events queue is purged without error
*/
		case 1:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0054"));
			iTest->LogSubTest(KTest1);
			RunTestsL();		//QueuePurgingL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0055

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test that events can be sent

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events in WServ

@SYMTestExpectedResults The events are sent without error
*/
		case 2:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0055"));
			iTest->LogSubTest(KTest2);
			RunTestsL();		//SendEventL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0056

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send an event to all window groups

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send an event to all window groups

@SYMTestExpectedResults The events are sent to all window groups
						without error
*/
		case 3:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0056"));
			iTest->LogSubTest(KTest3);
			General2();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0057

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events to an invisible window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events to an invisible window

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 4:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0057"));
			iTest->LogSubTest(KTest4);
			RunTestsL(ETrue);		//InvisibleWindowL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0058

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events for java additions

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events for java additions

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 5:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0058"));
			iTest->LogSubTest(KTest5);
			RunTestsL();		//JavaAdditionsL();
			break;

		case 6:
			((CTEventTestStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			iTest->LogSubTest(KTest6);
			//WindowCapBugL();		//Not finished
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0059

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events for x and y inputs

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events for x and y inputs

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 7:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0059"));
			iTest->LogSubTest(KTest7);
			RunTestsL();		//XyInputTypeL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0060

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events for moving pointer cursor

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events for moving pointer cursor

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 8:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0060"));
			iTest->LogSubTest(KTest8);
			RunTestsRestoreAreaL(ETrue);		//MovingPointerCursorL();
		    break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0061

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events for rotate mode

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events for rotate mode

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 9:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0061"));
			iTest->LogSubTest(KTest9);
			RunTestsRestoreAreaL(EFalse);		//RotatedModeL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0062

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events for an anim event

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events for an anim event

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 10:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0062"));
			iTest->LogSubTest(KTest10);
			RunTestsRestoreAreaL(EFalse);		//AnimEvent();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0063

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events for focus changed

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events for focus changed

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 11:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0063"));
			iTest->LogSubTest(KTest11);
			RunTestsL();		//FocusChanged();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0064

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send stop events

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send stop events

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 12:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0064"));
			iTest->LogSubTest(KTest12);
			RunTestsL(ETrue);		//StopEvents();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0065

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events for the virtual keyboard

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events for the virtual keyboard

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 13:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0065"));
			iTest->LogSubTest(KTest13);
			RunTestsL();		//VirtualKeyboard();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0066

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events for key clicks

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send stop events

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 14:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0066"));
			iTest->LogSubTest(KTest14);
			{
			TBool changeable;
			if (iClick.IsLoaded(changeable) && !iTest->IsFullRomL())
				RunTestsL(ETrue);		//KeyClicks();
			}
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0067

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test RWindowGroup::CaptureLongKey()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Simulate long key presses and check that long key capture
					and key repeat work as expected.

@SYMTestExpectedResults The correct key events are sent to the window without
						error
*/
		case 15:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0067"));
			iTest->LogSubTest(KTest15);
			RunTestsL(ETrue);		//CaptureLong();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0068

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events for activating password

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events for activating password

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 16:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0068"));
			iTest->LogSubTest(KTest16);
			if (!iTest->IsFullRomL())
				RunTestsL();		//Password();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0069

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send events for activating password

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events for activating password

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 17:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0069"));
			iTest->LogSubTest(KTest17);
			RunTestsL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0070

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send repeatable key events

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send repeatable key events

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 18:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0070"));
			iTest->LogSubTest(KTest18);
			RunTestsL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0071

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send Screen Scaling events

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send Screen Scaling events

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 19:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0071"));
			iTest->LogSubTest(KTest19);
			if (iScreenModeTests&EDoScale)
				{
				RunTestsL();
				}
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0072

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send Visibility Changed events

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send Visibility Changed events

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 20:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0072"));
			iTest->LogSubTest(KTest20);
			RunTestsL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0073

@SYMDEF             DEF081259

@SYMTestCaseDesc    Check Time Stamp Of Sent Events

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check Time Stamp Of Sent Events

@SYMTestExpectedResults The time stamps for the events are correct
*/
		case 21:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0073"));
			iTest->LogSubTest(KTest21);
			RunTestsL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0074

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send Pointer Capture Priority Events

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send Pointer Capture Priority Events

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 22:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0074"));
			iTest->LogSubTest(KTest22);
			RunTestsL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0343

@SYMDEF             DEF081259

@SYMTestCaseDesc    Send Event Handler Removal Events

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send Event Handler Removal Events

@SYMTestExpectedResults The events are sent to the window without
						error
*/
		case 23:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0343"));
			iTest->LogSubTest(KTest23);
			RunTestsL();                   //EventHandlerRemoval();
			break;
		case 24:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0565
*/
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0565"));
			iTest->LogSubTest(KTest24);
			EventQueuePtrCrashTest();
			break;
/**
@SYMTestCaseID 		GRAPHICS-WSERV-0496

@SYMDEF 			DEF133776

@SYMTestCaseDesc 	Cancels key captures using all combinations of 
  RWindowGroup::Capture...() and RWindowGroup::Cancel...() APIs.

@SYMTestPriority 	High

@SYMTestStatus 		Implemented

@SYMTestActions 	Call each of the RWindowGroup::Capture***() APIs followed by
 each of the RWindowGroup::CancelCapture***() APIs.
 RWindowGroup::Capture***() APIs:
 	CaptureKey(TUint, TUint, TUint)
 	CaptureKey(TUint, TUint, TUint, TInt)
 	CaptureKeyUpAndDowns(TUint, TUint, TUint)
 	CaptureKeyUpAndDowns(TUint, TUint, TUint, TInt)
 	CaptureLongKey(TUint, TUint, TUint, TUint, TInt, TUint)
 	CaptureLongKey(TTimeIntervalMicroSeconds32, TUint, TUint, TUint, TUint, TInt, TUint)
 RWindowGroup::CancelCapture***() APIs:
	CancelCaptureKey()
	CancelCaptureKeyUpAndDowns()
	CancelCaptureLongKey()
 
@SYMTestExpectedResults WServ should handle matched and mismatched Cancel 
 and Capture calls without panicking.
*/			
		case 25:
// Skip this test in debug on the emulator, run it always on hardware as debug ROMs include
// a release version of the production code that doesn't panic
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0496"));
#if defined(_DEBUG) && !defined (__MARM__)
			INFO_PRINTF1(_L("Skipping this test in _DEBUG on emulator to avoid debug only panics, see GRAPHICS-WSERV-0497 for the matching panic test"));				
#else						
			iTest->LogSubTest(KTest25);
			RunTestsL();
#endif
			break;	
/**
@SYMTestCaseID		GRAPHICS-WSERV-0444

@SYMDEF             PDEF110849

@SYMTestCaseDesc    Pointer Buffer Exception After Event Queue Purge

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Connect to pointer buffer, send it some pointer events and disconnect.
					Now send enough events to cause an event queue purge.  The purge will
					attempt to empty the pointer buffer, but will fail because we have disconnected.

@SYMTestExpectedResults The purge will realise it does not need to empty the pointer buffer and 
						so there will not be an access violation - exception.
						
*/			
		case 26:
			((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0444"));
			iTest->LogSubTest(KTest26);
			RunTestsL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0484
@SYMDEF             PDEF120721
@SYMTestCaseDesc    Window server guarantees to initialize the queue size of the new queue to 
					at least EMinQueueSize.
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Create multiple clients to connect to window server.
					Send enough events to make the event queue full.
					Create a new client to connect to window server.
					Send events to the new client and test that the new client 
					can get the events.
@SYMTestExpectedResults Window server guarantees to initialize the size of the new event 
						queue to EMinQueueSize, so that window server will not lock up
						due to the full of global event queue.
						
*/			
		case 27:
		    iTest->LogSubTest(KTest27);
		    RunTestsL();
		    break;
		    
#ifndef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA            
/**
@SYMTestCaseID      GRAPHICS-WSERV-0751, GRAPHICS-WSERV-0752, GRAPHICS-WSERV-0753
@SYMPREQ			417-61800
@SYMTestCaseDesc    Test translation of key events by Key Event Routing plug-in
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Use RWindowGroup::CaptureKey() to capture pseudo key
					Device0 (key code EKeyDevice0, scan code EStdKeyDevice0).
					Simulate pressing key Device1.
					Repeat using CaptureKeyUpAndDowns().
					Repeat using CaptureLongKey().
					Repeat without any capture requests.
@SYMTestExpectedResults
					The key event delivered contains the key or scan code for
					Device0, if captured. When using CaptureKey() and
					CaptureLongKey(), only the key code is translated. Using
					CaptureKeyUpAndDowns(), only the scan code is translated.

Note: this test requires an entry in the translation table of the Key Routing
Plug-in to map Device0 to Device1 at capture request time.
*/
		case 28:
            ((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0751"));
            iTest->LogSubTest(KTest28);
            RunTestsL();
            break;

/**
@SYMTestCaseID      GRAPHICS-WSERV-0760, GRAPHICS-WSERV-0761, GRAPHICS-WSERV-0762
@SYMPREQ			417-61800
@SYMTestCaseDesc    Test blocking of key events by Key Event Routing plug-in
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Simulate pressing a blocked key (Device3) while no
					key capture is in effect.
					Use RWindowGroup::CaptureKey() to capture EKeyDevice3
					and simulate pressing the blocked key again.
					Repeat using CaptureKeyUpAndDowns().
					Repeat using CaptureLongKey().
@SYMTestExpectedResults
					The blocked key event is only delivered when captured and
					is not routed to the focussed window group by default.

Note: this test requires entries in the Blocked Key Table of the Reference
Key Routing Plug-in for EKeyDevice3 and EStdKeyDevice3.
*/
		case 29:
            ((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0760"));
            iTest->LogSubTest(KTest29);
            RunTestsL();
            break;

/**
@SYMTestCaseID      GRAPHICS-WSERV-0754, GRAPHICS-WSERV-0755, GRAPHICS-WSERV-0756
@SYMPREQ			417-61800
@SYMTestCaseDesc    Test application UID-based restriction of key capture by
					Key Event Routing plug-in.
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Attempt to capture a key that is restricted by UID
					to another application.
					Use RWindowGroup::CaptureKey() to capture a key that is
					restricted to the current application's UID.
					Simulate pressing the second key.
					Repeat using CaptureKeyUpAndDowns().
					Repeat using CaptureLongKey().
@SYMTestExpectedResults
					Capture of the first key fails with KErrPermissionDenied.
					Capture of the second key succeeds and the key event is
					delivered.

Note: this test requires entries in the Restricted Key Table of the Reference
Key Routing Plug-in for the chosen keys.
*/
		case 30:
            ((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0754"));
            iTest->LogSubTest(KTest30);
            RunTestsL();
            break;

/**
@SYMTestCaseID      GRAPHICS-WSERV-0757, GRAPHICS-WSERV-0758, GRAPHICS-WSERV-0759
@SYMPREQ			417-61800
@SYMTestCaseDesc    Test application UID-based routing of key events by
					Key Event Routing plug-in.
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Invoke another application to capture two different keys
					with high priority using each of RWindowGroup::CaptureKey(),
					CaptureKeyUpAndDowns() and CaptureLongKey(). One key of
					each pair has precedence for capture by the current
					application's UID (i.e. this test).
					In the current application, use RWindowGroup::CaptureKey()
					to capture both keys with lower priority.
					Simulate pressing the keys.
					Repeat using CaptureKeyUpAndDowns().
					Repeat using CaptureLongKey().
@SYMTestExpectedResults
					The key event with UID-based precedence is delivered to
					the current application but the other key is not.

Note: this test requires entries in the Priority Application Table of the
Reference Key Routing Plug-in for the chosen key.
*/
		case 31:
            ((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0757"));
            iTest->LogSubTest(KTest31);
            RunTestsL();
            break;

#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS            
/**
@SYMTestCaseID      GRAPHICS-WSERV-2669-0015
@SYMREQ             REQ13202: Possibility for external layers to appear above UI layer
@SYMTestCaseDesc    Window obscured by transparent surface window receives 
                    (EPartiallyVisible | EFullyVisible) when surface is made semi-transparent.
@SYMTestPriority    2
@SYMPrerequisites   Win1 – bottom window
                    Win2 – middle window with background surface, which totally obscures win1
@SYMTestActions     Call SetSurfaceTransparency(ETrue) on win2
@SYMTestExpectedResults Win1 receives visibility event
*/            
        case 32:
            ((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-2669-0015"));
            iTest->LogSubTest(KTest32);
            RunTestsL();
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-2669-0016
@SYMREQ             REQ13202: Possibility for external layers to appear above UI layer 
@SYMTestCaseDesc    Window obscured by semi-transparent surface window receives   
                    (EPartiallyVisible | EFullyVisible) when obscuring window is deleted
@SYMTestPriority    2
@SYMPrerequisites   Win1 – bottom window
                    Win2 – middle window with semi-transparent background surface, which totally obscures win1
                    Win3 – top window which totally obscures win2, and is opaque
@SYMTestActions     Delete win3
@SYMTestExpectedResults Both win1 and win2 receive visibility events
*/            
        case 33:
            ((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-2669-0016"));
            iTest->LogSubTest(KTest33);
            RunTestsL();
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-2669-0017
@SYMREQ             REQ13202: Possibility for external layers to appear above UI layer 
@SYMTestCaseDesc    Window does not receive any visibiliy event when a semi-transparent
                    surface is moved onto/above it.
@SYMTestPriority    2
@SYMPrerequisites   Win1 – bottom window
                    Win2 – top window which does not overlap win1
@SYMTestActions     Move win2 to overlap win1
@SYMTestExpectedResults No visibility event is received (win1 is still fully visible)
*/            
        case 34:
            ((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-2669-0017"));
            iTest->LogSubTest(KTest34);
            RunTestsL();
            break;
#endif // SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS  
#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA

/**
@SYMTestCaseID      GRAPHICS-WSERV-0559
@SYMDEF             INC140850
@SYMTestCaseDesc    To check Wserv passes correct repeat value for TRawEvent of type EKeyRepeat.
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Simulate TRawEvent of type EKeyRepeat without Repeat value set.
                    Simulate the above with repeat value set to 2.
@SYMTestExpectedResults Simulated events should match expected events added to the array. 
*/

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		case 28:
#else
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
		case 35:
#else
		case 32:
#endif
#endif
		    // This test was moved to be the last test in the test suite, because it sometimes leaves events in the event queue, 
		    //it can affect the results of other tests. This test case should be the last test in the test suite. 
		    ((CTEventTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0484"));
		    iTest->LogSubTest(KTest35A);
		    InitializeQueueSizeTestL(EFalse);
		    iTest->LogSubTest(KTest35B);
		    InitializeQueueSizeTestL(ETrue);
		    break;    
			
		default:
            ((CTEventTestStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTEventTestStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTEventTestStep*)iStep)->RecordTestResultL();
	if (iFailed)
		{
		TEST(EFalse);
		iFailed=EFalse;
		}
	++iTest->iState;
	}

__WS_CONSTRUCT_STEP__(EventTest)

