// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test Key Click Plug-In DLL
// 
//

#include <e32std.h>
#include "W32CLICK.H"
#include "CLICK.H"
#include <graphics/pointereventdata.h>
#if defined(__WINS__)
	#include <emulator.h>
	#include "LOGWIN.H"
#endif

#define bufSize 64

GLREF_D struct TSharedMemory GSharedMemory;

class MClickMaker
	{
public:
	virtual void KeyEvent(TEventCode aType,const TKeyEvent& aEvent)=0;
	virtual void PointerEvent(const TPointerEvent& aEvent)=0;
	virtual void OtherEvent(TInt aType,TAny* aParam)=0;
	};

struct TGpWinInfo
	{
	TBool iInUse;
	TInt iServerValue;
	TInt iNumGroups;
	};

struct TGpWinIdData
	{
	TInt iId;
	TInt iClient;
	};

NONSHARABLE_CLASS(TLogClicks) : public MClickMaker
	{
public:
	void StartLoggingL();
	inline TBool IsLogging() {return iLogging;}
	//Pure virtual functions from MClickMaker
	void KeyEvent(TEventCode aType,const TKeyEvent& aEvent);
	void PointerEvent(const TPointerEvent& aEvent);
    void OtherEvent(TInt aType,TAny* aParam=NULL);
private:
	TBool iLogging;
	};

NONSHARABLE_CLASS(CEventClicks) : public CBase, public MClickMaker
	{
	enum {
		EEventBufferSize=32,
		EMaxGroupWinClients=6,
		EMaxGroupWinIdData=12
		};
public:
	void ConstructL();
	TInt Add(TWsEvent* aEvent);
	inline TInt Failed() {return iFailed;}
	inline TInt Events() {return iEventsTested;}
	void Reset();
	void ExtendedPointerEvent(const TPointerEventData& aPointerEvent);
	void ExpectNewWindowGroup(TInt aClient);
	void NewWindowGroup(const TGroupWindowOpenData& aGpWinOpen);
	void CheckGpWinId(TInt aId);
	void ExpectCloseWindowGroup(TInt aId);
	void CloseWindowGroup(TInt aId);
	void ExpectCloseWindow(TWindowCloseData& aCloseWin);
	void CloseWindow(TWindowCloseData& aCloseWin);
	void PointerEventInfo(TPointerEventInfo& aPointerEventInfo);	
	//Pure virtual functions from MClickMaker
	void KeyEvent(TEventCode aType,const TKeyEvent& aEvent);
	void PointerEvent(const TPointerEvent& aEvent);
	void OtherEvent(TInt aType,TAny* aParam=NULL);
private:
	TBool GetEvent(TWsEvent& aEvent);
	void Fail();
	void AddGroupWindowId(TInt aClient,TInt aGpWinId);
	void RemoveGroupWindowId(TInt aGpWinId);
private:
	CCirBuf<TWsEvent> iEventBuffer;
	TInt iFailed;
	TInt iEventsTested;
	TInt iFailedAt;
	TGpWinInfo iGroupWins[EMaxGroupWinClients];
	TGpWinIdData iGroupWinIds[EMaxGroupWinIdData];
	TInt iExpectedEvent;
	TInt iExpectedEventData;
	TInt iExpectedHandle;
	TPointerEventInfo iExpectedPointerEventInfo;
	TInt iLastNewGpWinId;
	TAdvancedPointerEvent iLastPointerEvent;
	TBool iExpectingExtendedPointer;
	};

NONSHARABLE_CLASS(CMyClickMaker) : public CClickMaker
	{
public:
	~CMyClickMaker();
	void ConstructL();
	//Virtual function from CClickMaker
	void KeyEvent(TEventCode aType,const TKeyEvent& aEvent);
	void PointerEvent(const TPointerEvent& aEvent);
	void OtherEvent(TInt aType,TAny* aParam);
	TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
private:
	void LogToWindowL();
private:
	TClickOutputModes iMode;
	MClickMaker* iCurrentClick;
	TLogClicks iLogClicks;
	CEventClicks* iEventClicks;
	};


/*TLogClicks*/

void TLogClicks::StartLoggingL()
	{
#if defined(__WINS__)
	if (!IsLogging())
		{
		CreateLogWinThreadL();
		iLogging=ETrue;
		}
#else
	User::Leave(KErrNotSupported);
#endif
	}

void TLogClicks::KeyEvent(TEventCode aType,const TKeyEvent& aEvent)
	{
	_LIT(KKeyDown,		"KEY DOWN   ");
	_LIT(KKeyUp,		"KEY UP     ");
	_LIT(KKeyEvent,		"KEY EVENT  ");
	_LIT(KKeyRepeat,	"KEY REPEAT ");
	_LIT(KKeyUnknown,	"KEY Unknown");
	_LIT(KKeyDataFormatChar,"'%c',");
	_LIT(KKeyDataFormatCode," Code=%u,");
	_LIT(KKeyDataFormatScan," Scan=%d,");
	_LIT(KKeyDataFormatModRep," Mod=0x%x, Rep=%d");
	TBuf<bufSize> bufPlusZero;
	switch (aType)
		{
	case EEventKey:
		bufPlusZero.Copy(KKeyEvent);
		break;
	case EEventKeyUp:
		bufPlusZero.Copy(KKeyUp);
		break;
	case EEventKeyDown:
		bufPlusZero.Copy(KKeyDown);
		break;
	case EEventKeyRepeat:
		bufPlusZero.Copy(KKeyRepeat);
		break;
	default:
		bufPlusZero.Copy(KKeyUnknown);
		break;
		}
	bufPlusZero.AppendFormat(KKeyDataFormatCode,aEvent.iCode);
	if (aEvent.iCode!=0)
		bufPlusZero.AppendFormat(KKeyDataFormatChar,aEvent.iCode);
	bufPlusZero.AppendFormat(KKeyDataFormatScan,aEvent.iScanCode);
	if (aEvent.iScanCode!=0)
		bufPlusZero.AppendFormat(KKeyDataFormatChar,aEvent.iScanCode);
	bufPlusZero.AppendFormat(KKeyDataFormatModRep,aEvent.iModifiers,aEvent.iRepeats);
	bufPlusZero.ZeroTerminate();
#if defined(__WINS__)
	Emulator::Escape();
	SendMessage(GSharedMemory.iHwnd, WM_USER+EAppendText, (bufPlusZero.Length()+1)*sizeof(TText), (TInt32)(bufPlusZero.Ptr()));
	Emulator::Reenter();
#endif
	}

void TLogClicks::PointerEvent(const TPointerEvent& aEvent)
	{
	_LIT(KButtonDown,	"POINTER DOWN ");
	_LIT(KButtonUp,		"POINTER UP   ");
	_LIT(KButton2Down,	"BUTTON 2 DOWN");
	_LIT(KButton2Up,	"BUTTON 2 UP  ");
	_LIT(KButton3Down,	"BUTTON 3 DOWN");
	_LIT(KButton3Up,	"BUTTON 3 UP  ");
	_LIT(KButtonDrag,	"POINTER DRAG ");
	_LIT(KButtonMove,	"POINTER MOVE ");
	_LIT(KButtonRepeat,	"BUTTON REPEAT");
	_LIT(KSwitchOn,		"POINTER ON   ");
	_LIT(KUnknown,		"PTR Unknown  ");
	//_LIT(KPtrDataFormat," Pos=(%d,%d), ScrPos=(%d,%d), Modifiers=%x");
	_LIT(KPtrDataFormat," Pos=(%d,%d), ScrPos=(%d,%d), Mod=%x");
	TBuf<bufSize> bufPlusZero;
	switch (aEvent.iType)
		{
	case TPointerEvent::EButton1Down:
		bufPlusZero.Copy(KButtonDown);
		break;
	case TPointerEvent::EButton1Up:
		bufPlusZero.Copy(KButtonUp);
		break;
	case TPointerEvent::EButton2Down:
		bufPlusZero.Copy(KButton2Down);
		break;
	case TPointerEvent::EButton2Up:
		bufPlusZero.Copy(KButton2Up);
		break;
	case TPointerEvent::EButton3Down:
		bufPlusZero.Copy(KButton3Down);
		break;
	case TPointerEvent::EButton3Up:
		bufPlusZero.Copy(KButton3Up);
		break;
	case TPointerEvent::EDrag:
		bufPlusZero.Copy(KButtonDrag);
		break;
	case TPointerEvent::EMove:
		bufPlusZero.Copy(KButtonMove);
		break;
	case TPointerEvent::EButtonRepeat:
		bufPlusZero.Copy(KButtonRepeat);
		break;
	case TPointerEvent::ESwitchOn:
		bufPlusZero.Copy(KSwitchOn);
		break;
	default:
		bufPlusZero.Copy(KUnknown);
		break;
		}
	bufPlusZero.AppendFormat(KPtrDataFormat,aEvent.iPosition.iX,aEvent.iPosition.iY
																,aEvent.iParentPosition.iX,aEvent.iParentPosition.iY,aEvent.iModifiers);
	bufPlusZero.ZeroTerminate();
#if defined(__WINS__)
	Emulator::Escape();
	SendMessage(GSharedMemory.iHwnd, WM_USER+EAppendText, (bufPlusZero.Length()+1)*sizeof(TText), (TInt32)(bufPlusZero.Ptr()));
	Emulator::Reenter();
#endif
	}

void TLogClicks::OtherEvent(TInt aType,TAny* aParam)
	{
	_LIT(KPointer,"POINTER EVENT Ver=%d, ScrPos=(%d,%d), WinClientHandle=0x%x, WinOrigin=%d, WinGpId=%d");
	_LIT(KScreenDeviceChanged,"SCREEN DEVICE CHANGED EVENT, Mode=%d");
	_LIT(KGroupWindowOpen,"GROUP WINDOW OPEN EVENT WinGpId=%d, Client=%d, NumWinGps=%d");
	_LIT(KGroupWindowClose,"GROUP WINDOW CLOSE EVENT WinGpId=%d");
	_LIT(KWindowClose,"WINDOW CLOSE EVENT Client=%d, WinGpId=%d");
	_LIT(KEventUnknown,	"EVENT Unknown");
	TBuf<bufSize> bufPlusZero;
	switch (aType)
		{
	case EEventPointer:
		{
		TPointerEventData& data=*static_cast<TPointerEventData*>(aParam);
		bufPlusZero.Format(KPointer,data.iVersion,data.iCurrentPos.iX,data.iCurrentPos.iY,data.iClientHandle
										,data.iWindowOrigin.iX,data.iWindowOrigin.iY,data.iWindowGroupId);
		}
		break;
	case EEventScreenDeviceChanged:
		{
		TClickMakerData& data=*static_cast<TClickMakerData*>(aParam);
		bufPlusZero.Format(KScreenDeviceChanged,data.screenDeviceMode);
		}
		break;
	case EEventGroupWindowOpen:
		{
		TGroupWindowOpenData& data=*static_cast<TGroupWindowOpenData*>(aParam);
		bufPlusZero.Format(KGroupWindowOpen,data.iIdentifier,data.iClient,data.iNumClientWindowGroups);
		}
		break;
	case EEventGroupWindowClose:
		bufPlusZero.Format(KGroupWindowClose,reinterpret_cast<TInt&>(aParam));
		break;
	case EEventWindowClose:
		{
		TWindowCloseData& data=*static_cast<TWindowCloseData*>(aParam);
		bufPlusZero.Format(KWindowClose,data.iClientHandle,data.iWindowGroupId);
		}
		break;
	default:
		bufPlusZero.Copy(KEventUnknown);
		break;
		}
	bufPlusZero.ZeroTerminate();
#if defined(__WINS__)
	Emulator::Escape();
	SendMessage(GSharedMemory.iHwnd, WM_USER+EAppendText, (bufPlusZero.Length()+1)*sizeof(TText), (TInt32)(bufPlusZero.Ptr()));
	Emulator::Reenter();
#endif
	}

/*CEventClicks*/

void CEventClicks::ConstructL()
	{
	iEventBuffer.SetLengthL(EEventBufferSize);
	}

TInt CEventClicks::Add(TWsEvent* aEvent)
	{
	return (iEventBuffer.Add(aEvent) ? KErrNone:KErrOverflow);
	}

void CEventClicks::Reset()
	{
	iFailed=EFalse;
	iEventsTested=0;
	iExpectedEvent=0;
	iLastNewGpWinId=0;
	}

void CEventClicks::ExtendedPointerEvent(const TPointerEventData& aPointerEvent)
	{
	if (!iExpectingExtendedPointer)
		{
		Fail();
		return;
		}
	iExpectingExtendedPointer=EFalse;
	TBool match=ETrue;
	if (0!=aPointerEvent.iVersion)
		match=EFalse;
	if (iLastPointerEvent.iType!=aPointerEvent.iPointerEvent.iType)
		match=EFalse;
	if (iLastPointerEvent.iModifiers!=aPointerEvent.iPointerEvent.iModifiers)
		match=EFalse;
	if (iLastPointerEvent.iPosition!=aPointerEvent.iPointerEvent.iPosition)
		match=EFalse;
	if (iLastPointerEvent.iParentPosition!=aPointerEvent.iCurrentPos)
		match=EFalse;
	if (TPointerEventData::EUnspecified!=aPointerEvent.iSource)
		match=EFalse;
	if (iExpectedPointerEventInfo.iClientHandle && iExpectedPointerEventInfo.iWinGpId>0)
		{
		if (iLastPointerEvent.iParentPosition-iExpectedPointerEventInfo.iParentOrigin
										!=aPointerEvent.iPointerEvent.iParentPosition)
			match=EFalse;
		if (iExpectedPointerEventInfo.iParentOrigin+iExpectedPointerEventInfo.iWinOrigin
															!=aPointerEvent.iWindowOrigin)
			match=EFalse;
		if (iExpectedPointerEventInfo.iClientHandle!=aPointerEvent.iClientHandle)
			match=EFalse;
		if (iExpectedPointerEventInfo.iWinGpId!=aPointerEvent.iWindowGroupId)
			match=EFalse;
		}
	if (!match)
		Fail();
	}

void CEventClicks::ExpectNewWindowGroup(TInt aClient)
	{
	if (iExpectedEvent>0)
		Fail();
	iExpectedEvent=EEventGroupWindowOpen;
	iExpectedEventData=aClient;
	}

void CEventClicks::NewWindowGroup(const TGroupWindowOpenData& aGpWinOpen)
	{
	iLastNewGpWinId=aGpWinOpen.iIdentifier;
	if (iExpectedEvent!=EEventGroupWindowOpen)
		{
		Fail();
		return;
		}
	iExpectedEvent=0;
	if (iExpectedEventData>=EMaxGroupWinClients)
		return;
	AddGroupWindowId(iExpectedEventData,iLastNewGpWinId);
	TGpWinInfo& gpWinInfo=iGroupWins[iExpectedEventData];
	if (gpWinInfo.iInUse)
		{
		if (aGpWinOpen.iClient!=gpWinInfo.iServerValue)
			Fail();
		else 
			{
			if (aGpWinOpen.iNumClientWindowGroups!=gpWinInfo.iNumGroups)
				Fail();
			++gpWinInfo.iNumGroups;
			}
		}
	else
		{
		gpWinInfo.iInUse=ETrue;
		gpWinInfo.iServerValue=aGpWinOpen.iClient;
		gpWinInfo.iNumGroups=aGpWinOpen.iNumClientWindowGroups+1;
		}
	}

void CEventClicks::CheckGpWinId(TInt aId)
	{
	if (iLastNewGpWinId!=aId)
		Fail();
	}

void CEventClicks::ExpectCloseWindowGroup(TInt aId)
	{
	if (iExpectedEvent>0)
		Fail();
	iExpectedEvent=EEventGroupWindowClose;
	iExpectedEventData=aId;
	}

void CEventClicks::CloseWindowGroup(TInt aId)
	{
	if (iExpectedEvent!=EEventGroupWindowClose || iExpectedEventData!=aId)
		Fail();
	if (iExpectedEvent==EEventGroupWindowClose)
		iExpectedEvent=0;
	RemoveGroupWindowId(aId);
	}

void CEventClicks::ExpectCloseWindow(TWindowCloseData& aCloseWin)
	{
	if (iExpectedEvent>0)
		Fail();
	iExpectedEvent=EEventWindowClose;
	iExpectedEventData=aCloseWin.iWindowGroupId;
	iExpectedHandle=aCloseWin.iClientHandle;
	}

void CEventClicks::CloseWindow(TWindowCloseData& aCloseWin)
	{
	if (iExpectedEvent!=EEventWindowClose || iExpectedEventData!=aCloseWin.iWindowGroupId || iExpectedHandle!=aCloseWin.iClientHandle)
		Fail();
	if (iExpectedEvent==EEventWindowClose)
		iExpectedEvent=0;
	}

void CEventClicks::PointerEventInfo(TPointerEventInfo& aPointerEventInfo)
	{
	iExpectedPointerEventInfo=aPointerEventInfo;
	}

TBool CEventClicks::GetEvent(TWsEvent& aEvent)
	{
	++iEventsTested;
	if (iEventBuffer.Remove(&aEvent)<1)
		{
		Fail();
		return ETrue;
		}
	return EFalse;
	}

void CEventClicks::Fail()
	{
	if (!iFailed)
		{
		iFailed=iEventsTested;
		}
	}

void CEventClicks::AddGroupWindowId(TInt aClient,TInt aGpWinId)
	{
	TInt ii=0;
	while (ii<EMaxGroupWinIdData && iGroupWinIds[ii].iId>0)
		++ii;
	if (ii<EMaxGroupWinIdData)
		{
		iGroupWinIds[ii].iId=aGpWinId;
		iGroupWinIds[ii].iClient=aClient;
		}
	}

void CEventClicks::RemoveGroupWindowId(TInt aGpWinId)
	{
	TInt ii=0;
	while (ii<EMaxGroupWinIdData && iGroupWinIds[ii].iId!=aGpWinId)
		++ii;
	if (ii<EMaxGroupWinIdData)
		{
		--iGroupWins[iGroupWinIds[ii].iClient].iNumGroups;
		iGroupWinIds[ii].iId=0;
		}
	}

#define MODIFIER_FLAGS_TO_IGNOR EModifierNumLock
#pragma warning(disable : 4245)		//'initializing' : conversion from 'int' to 'unsigned int', signed/unsigned mismatch
void CEventClicks::KeyEvent(TEventCode aType,const TKeyEvent& aEvent)
	{
	TBool pass;
	TEventCode eType=aType;
	switch (aType)
		{
	case EEventKey:
		pass=(aEvent.iRepeats==0);
		break;
	case EEventKeyUp:
	case EEventKeyDown:
		pass=(aEvent.iCode==0) && (aEvent.iRepeats==0);
		break;
	case EEventKeyRepeat:
		pass=(aEvent.iRepeats>0);
		eType=EEventKey;
		break;
	default:
		pass=EFalse;
		}
	if (!pass)
		{
		Fail();
		return;
		}
	TWsEvent eEvent;
	if (GetEvent(eEvent))
		return;
	if (eEvent.Type()!=eType)
		{
		Fail();
		return;
		}
	TKeyEvent keyEvent=*eEvent.Key();
	TUint mask=~(EModifierAutorepeatable|MODIFIER_FLAGS_TO_IGNOR);
	if (keyEvent.iCode!=aEvent.iCode || (keyEvent.iModifiers&mask)!=(aEvent.iModifiers&mask) || keyEvent.iScanCode!=aEvent.iScanCode
																						|| (keyEvent.iRepeats==0)!=(aEvent.iRepeats==0))
		{
		Fail();
		return;
		}
	}

void CEventClicks::PointerEvent(const TPointerEvent& aEvent)
	{
	// Click events are now all advanced events so in order to test the modifier bits
	// appropriately we need to copy them as TAdvancedPointerEvent not TPointerEvent
	if(!aEvent.IsAdvancedPointerEvent())
		{
		Fail();
		return;
		}
	iLastPointerEvent=*aEvent.AdvancedPointerEvent();
		
	if (iExpectingExtendedPointer)
		Fail();
	else
		iExpectingExtendedPointer=ETrue;
	TWsEvent eEvent;
	if (GetEvent(eEvent))
		return;
	TAdvancedPointerEvent pEvent=*eEvent.Pointer();
	TUint mask=~(MODIFIER_FLAGS_TO_IGNOR);
	if (pEvent.iType!=aEvent.iType || (pEvent.iModifiers&mask)!=(aEvent.iModifiers&mask)
												|| pEvent.iPosition!=aEvent.iPosition || pEvent.iParentPosition!=aEvent.iParentPosition)
		{
		Fail();
		return;
		}
	}

void CEventClicks::OtherEvent(TInt aType,TAny* aParam)
	{
	TBool pass=ETrue;
	if (aType!=EEventPointer)
		++iEventsTested;
	switch (aType)
		{
	case EEventPointer:
		ExtendedPointerEvent(*static_cast<TPointerEventData*>(aParam));
		break;
	case EEventScreenDeviceChanged:
		break;
	case EEventGroupWindowOpen:
		NewWindowGroup(*static_cast<TGroupWindowOpenData*>(aParam));
		break;
	case EEventGroupWindowClose:
		CloseWindowGroup(reinterpret_cast<TInt>(aParam));
		break;
	case EEventWindowClose:
		CloseWindow(*static_cast<TWindowCloseData*>(aParam));
		break;
	default:
		pass=EFalse;
		}
	if (!pass)
		Fail();
	//GetEvent() is not call here because CWsGroupWindow::EnableScreenChangeEvents() could not be 
	//been called.This mean that no EEventScreenDeviceChanged will be put in the client queue.
	//Instead this event will be always passed to the click plugin if this is present.
	}
#pragma warning(default : 4245)


/*CMyClickMaker*/

CMyClickMaker::~CMyClickMaker()
	{
	delete iEventClicks;
	}

void CMyClickMaker::ConstructL()
	{
	iMode=EClickNone;
	iEventClicks=new(ELeave) CEventClicks();
	iEventClicks->ConstructL();
	}

void CMyClickMaker::KeyEvent(TEventCode aType,const TKeyEvent& aEvent)
	{
	if (iCurrentClick)
		iCurrentClick->KeyEvent(aType,aEvent);
	}

void CMyClickMaker::PointerEvent(const TPointerEvent& aEvent)
	{
	if (iCurrentClick)
		iCurrentClick->PointerEvent(aEvent);
	}

void CMyClickMaker::OtherEvent(TInt aType,TAny* aParam)
	{
	if (iCurrentClick)
		iCurrentClick->OtherEvent(aType,aParam);
	}

TInt CMyClickMaker::CommandReplyL(TInt aOpcode,TAny* aArgs)
	{
	switch (aOpcode)
		{
	case EClickCommandToggleOutput:
		switch (iMode)
			{
		case EClickNone:
			LogToWindowL();
			break;
		case EClickCheck:
		case EClickToWindow:
			iMode=EClickNone;
			iCurrentClick=NULL;
			break;
			}
		break;
	case EClickCommandSetOutput:
		iMode=*STATIC_CAST(TClickOutputModes*,aArgs);
		switch (iMode)
			{
		case EClickNone:
			iCurrentClick=NULL;
			break;
		case EClickCheck:
			iCurrentClick=iEventClicks;
			iEventClicks->Reset();
			break;
		case EClickToWindow:
			LogToWindowL();
			break;
			}
		break;
	case EClickEventAdd:
		return iEventClicks->Add(STATIC_CAST(TWsEvent*,aArgs));
	case EClickFailed:
		return iEventClicks->Failed();
	case EClickEvents:
		return iEventClicks->Events();
	case EClickReset:
		iEventClicks->Reset();
		break;
	case EClickCreateGroupWin:
		iEventClicks->ExpectNewWindowGroup(*static_cast<TInt*>(aArgs));
		break;
	case EClickCheckGpWinId:
		iEventClicks->CheckGpWinId(*static_cast<TInt*>(aArgs));
		break;
	case EClickCloseGroupWin:
		iEventClicks->ExpectCloseWindowGroup(*static_cast<TInt*>(aArgs));
		break;
	case EClickCloseWin:
		iEventClicks->ExpectCloseWindow(*static_cast<TWindowCloseData*>(aArgs));
		break;
	case EClickPointerEvent:
		iEventClicks->PointerEventInfo(*static_cast<TPointerEventInfo*>(aArgs));
		break;
	default:;
		}
	return KErrNone;
	}

void CMyClickMaker::LogToWindowL()
	{
	iMode=EClickNone;
	iCurrentClick=NULL;
	iLogClicks.StartLoggingL();
	iMode=EClickToWindow;
	iCurrentClick=&iLogClicks;
	}


EXPORT_C CClickMaker* CreateClickMakerL()
	{
	CMyClickMaker* plugIn=new(ELeave) CMyClickMaker;
	CleanupStack::PushL(plugIn);
	plugIn->ConstructL();
	CleanupStack::Pop(plugIn);
	return plugIn;
	}
