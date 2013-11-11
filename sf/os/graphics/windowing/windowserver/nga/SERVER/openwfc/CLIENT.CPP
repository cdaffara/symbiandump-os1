// Copyright (c) 1994-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Client handling
// 
//

#include "CLIENT.H"

#include "ANIM.H"
#include "Direct.H"
#include "EVENT.H"
#include "KEYCLICK.H"
#include "server.h"
#include "gc.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "wstop.h"
#include "panics.h"
#include "../CLIENT/w32comm.h"
#include "password.h"
#include "pointer.h"
#include <u32hal.h> // EHalGroupEmulator
#include "WsMemMgr.h"
#include <e32hashtab.h>  // for RHashMap
#include "registeredsurfacemap.h"
#include <graphics/wselement.h>

#include "windowelementset.h"
#include "drawresource.h"

extern CDebugLogBase* wsDebugLog;
_LIT(KWSERVSessionPanicCategory,"WSERV");

GLREF_D TPtr nullDescriptor;

TWsCmdHeaderBase CWsClient::iCurrentCommand;
TBuf8<EClientBufferMaxSize> CWsClient::iCmdBuf;
TInt CWsClient::iReply;
TInt CWsClient::iReplyOffset;
CWsClient* CWsClient::iCurrentClient;
CWsObject* CWsClient::iDestObj;
const TUint8* CWsClient::iNextCmd;

TUint CWsClient::iConnectionId = CDebugLogBase::EDummyConnectionId+1;
CArrayFixFlat<CWsClient::TWsCursorArrayItem>* CWsClient::iSystemPointerCursors = NULL;
TInt CWsClient::iDefaultSystemPointerCursorIndex = 0;		//Negative when there isn't one
CWsPointerCursor* CWsClient::iDefaultSystemPointerCursor;
CWsClient* CWsClient::iSystemPointerCursorListOwner = NULL;
CArrayFixFlat<CWsClient::TWsCursorArrayItem>* CWsClient::iTextCursorArray = NULL;

TKeyArrayFix CWsClient::iCursorKey(_FOFF(CWsClient::TWsCursorArrayItem,iIndex), ECmpTInt);

/**
Used for enforcing the redraw calling convention in emulator builds.
When enabled this will panic any client calling a CWindowGc draw operation outside a 
RWindow::BeginRedraw() / RWindow::EndRedraw() pair (known as non-redraw drawing).

Enable by adding "debug_wserv_exe_EnforceRedrawCallingConvention X" to epoc.ini 
where X is either 0 (zero) for "off" or 1 (one) for "on". 

Then enable globally in WServ AutoFlush by defining __AUTO_FLUSH in ../client/client.h 
or locally by calling RWsSession::SetAutoFlush(ETrue) for a specific client programatically, 
or locally pressing Ctrl-Alt-Shift-F in the emulator.
*/
TBool CWsClient::iDebug_EnforceRedrawCallingConvention = EFalse;

// Security policy stings
static _LIT_SECURITY_POLICY_C1(KSecurityPolicy_WriteDeviceData,ECapabilityWriteDeviceData);
static _LIT_SECURITY_POLICY_C1(KSecurityPolicy_SwEvent,ECapabilitySwEvent);
static _LIT_SECURITY_POLICY_C1(KSecurityPolicy_PowerMgmt,ECapabilityPowerMgmt);

//
// class CWsClient
// 

CWsClient::CWsClient(RThread aClient) : iClient(aClient), iGraphicMessageQueue(this), iInternalFlags(ERemoveKeyCode|EFinishedProcessingCommands)
	{
	iScreen = CWsTop::Screen();
	}

CWsClient::~CWsClient()
	{
	CWsTop::WindowServer()->RemoveAllGraphicDrawers(*this); // deindexes all graphic drawers owned by this client

	delete iTempCustomTextCursor.iCursor;
	FreeSystemPointerCursorList();
	CWsTop::ClientDestroyed(this);
	if (wsDebugLog)
		{
		_LIT(ClientDestuct,"Client %d destructing");
		wsDebugLog->MiscMessage(CDebugLogBase::ELogIntermediate,ClientDestuct, iConnectionHandle);
		}
		
	iInternalFlags |= EClientIsClosing;
	delete iObjectIndex;
	delete iEventQueue;
	delete iRedrawQueue;
	delete iPriorityKeyEvent;
	CWsTop::ClearSurfaceMap(this);

	CWsTop::SessionExited(this);
	iClient.Close();
	}

void CWsClient::CompleteInitializationL()
	{
	iObjectIndex = new(ELeave) CWsObjectIx();
	iObjectIndex->ConstructL();
	
    iEventQueue = new(ELeave) CEventQueue(this);
	iEventQueue->ConstructL();
    
    iRedrawQueue = new(ELeave) CRedrawQueue(this);
	iRedrawQueue->ConstructL();
    
    iPriorityKeyEvent = new(ELeave) CPriorityKey(this);
	
	CWsCliObj::NewL(this);
	
	iComputeMode = RWsSession::EPriorityControlComputeOff;
	
	CWsTop::NewSession(this);
	
#ifdef __WINS__
	TBool halValue = EFalse;
	if (UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty, 
		(TAny*)"debug_wserv_exe_EnforceRedrawCallingConvention", &halValue) == KErrNone)
		{
		iDebug_EnforceRedrawCallingConvention = halValue;
		}
#endif
	
	iInternalFlags |= EIsInitialised;
	}

TBool CWsClient::DebugEnforceRedrawCallingConvention()
	{
	return iDebug_EnforceRedrawCallingConvention;
	}

void CWsClient::StartInitializationL(TUint aConnectionHandle)
	{
	if (wsDebugLog)
		wsDebugLog->NewClient(aConnectionHandle);
	
	if (iObjectIndex)
		PPanic(EWservPanicReInitialise);
	else
		{
		iConnectionHandle = aConnectionHandle;
		CompleteInitializationL();
		}
	}

//
// Convert a handle to object checking it is of the correct type.
//
void CWsClient::HandleToWindow(TInt aHandle,CWsWindowBase** pWin)
	{
	if ((*pWin=(CWsWindowBase* )HandleToObjUntyped(aHandle))==NULL ||
		((*pWin)->Type()!=WS_HANDLE_WINDOW && (*pWin)->Type()!=WS_HANDLE_GROUP_WINDOW))
		PPanic(EWservPanicWindow);
	}

//
// Convert a handle to object checking it is of the correct type.
//
void CWsClient::HandleToClientWindow(TInt aHandle,CWsClientWindow** pWin)
	{
	if ((*pWin=(CWsClientWindow*)HandleToObj(aHandle, WS_HANDLE_WINDOW))==NULL)
		PPanic(EWservPanicWindow);
	}

void CWsClient::CreateNewPointerCursorL(const TWsClCmdCreatePointerCursor &aCmd)
	{
	CWsPointerCursor* pc=new(ELeave) CWsPointerCursor(this);
	CleanupStack::PushL(pc);
	pc->ConstructL(aCmd);
	CleanupStack::Pop();
	}

// Create a new custom text cursor
void CWsClient::StartSetCustomTextCursorL(const TWsClCmdCustomTextCursorData& aCmd)
	{
	if (!iTextCursorArray)
		{
		const TInt textCursorArrayGranularity = 4;
		iTextCursorArray = new(ELeave) CArrayFixFlat<TWsCursorArrayItem>(textCursorArrayGranularity);
		}
		
	TInt arrayIndex = KErrNotFound;
	if (FindCursorArrayItem(iTextCursorArray, aCmd.identifier, arrayIndex))
		User::Leave(KErrAlreadyExists);
	
	delete iTempCustomTextCursor.iCursor;
	iTempCustomTextCursor.iCursor = NULL;
	iTempCustomTextCursor.iCursor = new(ELeave) CWsCustomTextCursor(this, aCmd.alignment);
	
	static_cast<CWsCustomTextCursor*>(iTempCustomTextCursor.iCursor)->ConstructL(aCmd.flags);
	iTempCustomTextCursor.iIndex = aCmd.identifier;
	}

// Add new custom text cursor to global list
void CWsClient::CompleteSetCustomTextCursorL(TInt aError)
	{
	if (aError)
		{
		delete iTempCustomTextCursor.iCursor;
		iTempCustomTextCursor.iCursor = NULL;
		User::Leave(aError);
		}

	TWsCursorArrayItem entry = iTempCustomTextCursor;
	iTempCustomTextCursor.iCursor = NULL;
	CleanupStack::PushL(entry.iCursor);

	TInt arrayIndex;
	if (FindCursorArrayItem(iTextCursorArray, entry.iIndex, arrayIndex))
		User::Leave(KErrAlreadyExists);
	else
		iTextCursorArray->InsertIsqL(entry, iCursorKey);

	CleanupStack::Pop(entry.iCursor);
	}

CWsCustomTextCursor* CWsClient::FindCustomTextCursor(TInt aIdentifier)
	{
	TInt arrayIndex;
	if (!FindCursorArrayItem(iTextCursorArray, aIdentifier, arrayIndex))
		return NULL;

	return TextCursor(arrayIndex);
	}

void CWsClient::CreateNewSpriteL(const TWsClCmdCreateSprite &aCmd)
	{
	CWsSprite* sprite = new(ELeave) CWsSprite(this);
	CleanupStack::PushL(sprite);
	sprite->ConstructL(aCmd);
	CleanupStack::Pop();
	}

void CWsClient::CreateNewBitmapL(const TWsClCmdCreateBitmap &aCmd)
	{
	DWsBitmap* bitmap = new(ELeave) DWsBitmap(this);
	CleanupStack::PushL(bitmap);
	bitmap->ConstructL(aCmd);
	CleanupStack::Pop();
	}

/** Creates a new window.

If the parent is a window group then a new CWsTopClientWindow instance is created. If the parent is
a window then a new CWsClientWindow instance is created.

@param aCmd The command received from the client
@internalComponent
@released
*/
void CWsClient::CreateNewWindowL(const TWsClCmdCreateWindow &aCmd)
	{
	CWsWindowBase* parent;
	HandleToWindow(aCmd.parent,&parent);
	CWsClientWindow* win = NULL;
	TBool deviceIsInvalid = EFalse;
	CScreen* screen = parent->Screen();
	if (parent->WinType()==EWinTypeGroup)
		{
		__ASSERT_DEBUG(!((CWsWindowGroup*)parent)->ScreenDeviceDeleted(),PPanic(EWservPanicGroupWinScreenDeviceDeleted));
		win=new(ELeave) CWsTopClientWindow(this, screen);
		deviceIsInvalid=!((CWsWindowGroup*)parent)->ScreenDeviceValid();
		}
	else
		win=new(ELeave) CWsClientWindow(this, screen);

	CleanupStack::PushL(win);
	win->ConstructL(aCmd,parent,deviceIsInvalid);
	CleanupStack::Pop(win);
	}

void CWsClient::CreateNewWindowGroupL(const TWsClCmdCreateWindowGroup &aCmd)
	{
	CWsWindowGroup::NewL(this, NULL, aCmd); //screen is initialised inside the constructL
	}

void CWsClient::CreateNewAnimDllL(const TWsClCmdUnion &aParams)
	{
	CWsAnimDll* animDll = new(ELeave) CWsAnimDll(this);
	CleanupStack::PushL(animDll);
	animDll->LoadL(BufferTPtr((TText*)(aParams.LoadAnimDll+1),aParams.LoadAnimDll->length));
	CleanupStack::Pop();
	}	

void CWsClient::CreateNewScreenDeviceL(TInt aDefaultScreenNumber, TUint aClientScreenDevicePointer)
	{
	DWsScreenDevice* screenDevice = new(ELeave) DWsScreenDevice(this, aDefaultScreenNumber,aClientScreenDevicePointer);
	CleanupStack::PushL(screenDevice);
	screenDevice->ConstructL();
	CleanupStack::Pop(screenDevice);
	if (iPrimaryScreenDevice==NULL)
		{
		iPrimaryScreenDevice=screenDevice;
		// When client create screen device, change default screen to the one specified.
		// Client should do this immediately after establishing session
		iScreen = iPrimaryScreenDevice->Screen();
		InitialiseScreenDevices();
		}
	}

void CWsClient::InitialiseScreenDevices()
	{
	const TWsObject* ptr = iObjectIndex->FirstObject();
	const TWsObject* end = ptr+iObjectIndex->Length();
	WS_ASSERT_DEBUG(ptr->iObject==NULL, EWsPanicObjectIndexError);
	while(++ptr<end)
		{
		if (ptr->iObject && ptr->iObject->Type()==WS_HANDLE_GROUP_WINDOW)
			{
			CWsWindowGroup* gw = static_cast<CWsWindowGroup*>(ptr->iObject);
			if(!gw->Device())
				gw->SetScreenDevice(iPrimaryScreenDevice);
			}
		}
	}

void CWsClient::CreateNewClickHandlerL(const TUid& aUid)
	{
	CClick* click = new(ELeave) CClick(this);
	CleanupStack::PushL(click);
	click->ConstructL(aUid);
	CleanupStack::Pop(click);
	}

void CWsClient::RequestComplete(TRequestStatus*  &aStatus, TInt aErr)
	{
	Client().RequestComplete(aStatus,aErr);
	}

void CWsClient::PanicCurrentClient(TClientPanic aPanic)
	{
	iCurrentClient->PPanic(aPanic);
	}

void CWsClient::PPanic(TClientPanic aPanic) const
//This function is allowed to leave with out the 'L' convention for special reasons
	{
	SessionPanic(aPanic);
	User::Leave(EPanicLeave);
	}

void CWsClient::SessionPanic(TClientPanic aReason) const
	{
	if (wsDebugLog)
		wsDebugLog->Panic(iConnectionHandle, aReason);
	
	if (!(iInternalFlags&EPanicClientAsSoonAsPossible)) // keep the first error code
		{
		iInternalFlags |= EPanicClientAsSoonAsPossible;
		iPanicReason = aReason;
		}
	}

void CWsClient::SessionTerminate()
	{
	if (wsDebugLog)
		wsDebugLog->Panic(iConnectionHandle, 0);

	const RThread thread=Client();
	RProcess process;
	if (thread.Process(process)==KErrNone)
		{
		process.Terminate(0);
		process.Close();
		}
	}

/**	
Returns the remaining space in the descriptor
*/
TInt CWsClient::ReplyBufSpace() 
	{
	const TInt retVal = iCurrentClient->ClientMessage().GetDesLength(KReplyBufferMessageSlot);
	
	if (iReplyOffset>=0 && retVal>=iReplyOffset)
		return retVal-iReplyOffset;
	else
		return (retVal<0 ? retVal : KErrBadDescriptor);
	}

void CWsClient::ReplyBuf(const TDesC16 &aDes)	// static
	{
	WS_ASSERT_DEBUG(!iCurrentClient->ClientMessage().IsNull(), EWsPanicInvalidMessageHandle);
	
	if(iCurrentClient->ClientMessage().Write(KReplyBufferMessageSlot,aDes,iReplyOffset) != KErrNone)
		PanicCurrentClient(EWservPanicDescriptor);
	
	iReplyOffset += aDes.Length();
	if (wsDebugLog)
		wsDebugLog->ReplyBuf(aDes);
	}

void CWsClient::ReplyBuf(const TDesC8 &aDes)	// static
	{
	WS_ASSERT_DEBUG(!iCurrentClient->ClientMessage().IsNull(), EWsPanicInvalidMessageHandle);
	
	if(iCurrentClient->ClientMessage().Write(KReplyBufferMessageSlot, aDes, iReplyOffset) != KErrNone)
		PanicCurrentClient(EWservPanicDescriptor);
	
	iReplyOffset += aDes.Length();
	if (wsDebugLog)
		wsDebugLog->ReplyBuf(aDes);
	}

void CWsClient::ReplyBuf(const TAny* aSource, TInt aLength)	// static
//
// Send a buffer to the client process.
//
	{
	TPtrC8 src(reinterpret_cast<const TUint8*>(aSource),aLength);
	ReplyBuf(src);
	}

void CWsClient::ReplySize(const TSize &aSize)	// static
	{
	ReplyBuf(&aSize, sizeof(aSize));
	}

void CWsClient::ReplyPoint(const TPoint &aPoint)	// static
	{
	ReplyBuf(&aPoint, sizeof(aPoint));
	}

void CWsClient::ReplyRect(const TRect &aRect)	// static
	{
	ReplyBuf(&aRect, sizeof(aRect));
	}

void CWsClient::SetReply(TInt reply)
	{
	iReply = reply;
	if (wsDebugLog)
		wsDebugLog->Reply(reply);
	}

const TUint8* CWsClient::EndOfCommandBuffer()
	{
	return(iCmdBuf.Ptr()+iCmdBuf.Size());
	}

const TPtrC CWsClient::BufferTPtr(TText *aStart,TInt aLen)
	{
	TPtrC ptr;
	if (!BufferTPtrGc(aStart,aLen,ptr))
		PanicCurrentClient(EWservPanicBufferPtr);
	return(ptr);
	}

TBool CWsClient::BufferTPtrGc(TText* aStart,TInt aLen, TPtrC& aPtr)
	{
	if (iCurrentCommand.iOpcode>0)
		{
		if ((REINTERPRET_CAST(TUint8*,aStart)<iCmdBuf.Ptr()
										|| REINTERPRET_CAST(TUint8*,aStart+aLen)>(iCmdBuf.Ptr()+iCmdBuf.Size())))
			return(EFalse);
		}
	else
		{
		if (aLen>=iCurrentCommand.iCmdLength)
			return(EFalse);
		}
	aPtr.Set(aStart,aLen);
	return(ETrue);
	}

const TPtrC8 CWsClient::BufferTPtr8(TUint8* aStart,TInt aLen)
	{
	if (iCurrentCommand.iOpcode>0 && (REINTERPRET_CAST(TUint8*,aStart)<iCmdBuf.Ptr()
										|| REINTERPRET_CAST(TUint8*,aStart+aLen)>(iCmdBuf.Ptr()+iCmdBuf.Size())))
		PanicCurrentClient(EWservPanicBufferPtr);
		
	return(TPtrC8(aStart,aLen));
	}

/**
Process a command buffer

@internalComponent
@released
*/
void CWsClient::DispatchCommandsInBufL()	// (step #4)
	{		
	if (wsDebugLog)
		{
 		wsDebugLog->CommandBuf(iConnectionHandle);
 		RThread client = Client(); 
 		wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything, client.FullName());
		}
	const TUint8* endCmd=iCmdBuf.Ptr()+iCmdBuf.Length();
	do
		{
		const TWsCmdHeader* pCmd=
				reinterpret_cast<const TWsCmdHeader*>(iNextCmd);
		TUint opcode = pCmd->iBase.iOpcode;
		TInt headerLen = sizeof(pCmd->iBase);
		iCurrentCommand = pCmd->iBase;
		
		// For performance reasons the handle is only included
		// if it is different from the previous command. The EWsOpcodeHandle
		// flag indicates whether a new handle has been included in the
		// current command. If not we use the same handle as the previous
		// command.
		if (opcode&EWsOpcodeHandle)
			{
			// Find the WServ object associated with this op code
			opcode &= ~EWsOpcodeHandle;
			iCurrentCommand.iOpcode = reinterpret_cast<TUint16&>(opcode);
			iDestObj=HandleToObjUntyped(pCmd->iDestHandle);
			headerLen = sizeof(*pCmd);
			}
			
		iNextCmd += headerLen;
		const TAny* cmdParams = iNextCmd;
		iNextCmd += pCmd->iBase.iCmdLength;
		if (!iDestObj || iNextCmd>endCmd)		// Invalid handle or Corrupt buffer
			{
			SessionPanic(iDestObj==NULL ? EWservPanicHandle : EWservPanicBuffer);
			iInternalFlags|=EFinishedProcessingCommands;
			break;
			}

		if (iNextCmd==endCmd)
			iInternalFlags|=EFinishedProcessingCommands;
			
		if (wsDebugLog)
			wsDebugLog->Command(iDestObj->Type(), opcode, cmdParams, iDestObj->LogHandle());
		
		// Dispatch the command to the WServ object that will process it
		iDestObj->CommandL(opcode, cmdParams);	// (call #5)
		} 
	while(iNextCmd<endCmd);
	
	}

void CWsClient::DoServiceCommandBuf() // (step #3.1)
	{
	iCurrentClient=this;
	iInternalFlags&=~EFinishedProcessingCommands;
	TRAPD(err, DispatchCommandsInBufL());	// (call #4)
	
#if defined(_DEBUG)
	if (err!=KErrNone && !(iInternalFlags&(EFinishedProcessingCommands|EPanicClientAsSoonAsPossible)))
		SessionPanic(EWservPanicFunctionLeave);
#endif

	if (err<KErrNone)
		SetReply(err);

	if (iInternalFlags&(EFinishedProcessingCommands|EPanicClientAsSoonAsPossible))
		CompleteMessage(iClientMessage,iReply);	// (finish)
	
	iCurrentClient=NULL;
#if defined(_DEBUG)
	User::Heap().Check();
#endif
	}

void CWsClient::ExecuteAsyncClientCommandL(TInt aOpcode, const RMessage2& aMessage)	// (step #3.2)
	{
	switch(aOpcode)
		{
		case EWsClOpEventReady:
			EventReady(aMessage);
			break;
		case EWsClOpPriorityKeyReady:
			PriorityKeyEventReady(aMessage);
			break;
		case EWsClOpRedrawReady:
			RedrawEventReady(aMessage);
			break;
		case EWsClOpGraphicMessageReady:
			iGraphicMessageQueue.EventReady(aMessage);
			break;
		default:
			{
			PPanic(EWservPanicOpcode);
			break;
			}
		}
	}



void CWsClient::ExecuteCommandL(TInt aOpcode, const TAny* aCmdData)	// (step #6)
	{
	TWsClCmdUnion pData;
	pData.any=aCmdData;
	switch(aOpcode)
		{
		case EWsClOpCreateWindowGroup:
			CreateNewWindowGroupL(*pData.CreateWindowGroup);
			break;
		case EWsClOpCreateWindow:
			CreateNewWindowL(*pData.CreateWindow);
			break;
		case EWsClOpCreateGc:
			CWsGc::NewL(this);
			break;
		case EWsClOpCreateAnimDll:
			if (!CheckBuffer(pData.LoadAnimDll->length, KMaxFileName))
				PanicCurrentClient(EWservPanicBufferPtr);
			CreateNewAnimDllL(pData);
			break;
		case EWsClOpCreateGraphic:
			CWsGraphicDrawerObject::NewL(this,pData);
			break;
		case EWsClOpCreateScreenDevice:
			{
			const TInt screenNumber = pData.CreateScreenDevice->screenNumber;
			const TUint clientScreenDevicePointer = pData.CreateScreenDevice->clientScreenDevicePointer;
			if (screenNumber<0 || screenNumber>=CWsTop::NumberOfScreens())
				{
				PPanic(EWservPanicScreenNumber);
				}
			else
				{
				CreateNewScreenDeviceL(screenNumber,clientScreenDevicePointer);
				}
			}
			break;
		case EWsClOpCreateSprite:
			CreateNewSpriteL(*pData.CreateSprite);
			break;
		case EWsClOpCreatePointerCursor:
			CreateNewPointerCursorL(*pData.CreatePointerCursor);
			break;
		case EWsClOpStartSetCustomTextCursor:
			StartSetCustomTextCursorL(*pData.CustomTextCursorData);
			break;
		case EWsClOpCompleteSetCustomTextCursor:
			CompleteSetCustomTextCursorL(*pData.Int);
			break;
		case EWsClOpCreateBitmap:
			CreateNewBitmapL(*pData.CreateBitmap);
			break;
		case EWsClOpCreateDirectScreenAccess:
			CWsDirectScreenAccess::NewL(this,EFalse);
			break;
		case EWsClOpCreateDirectScreenAccessRegionTrackingOnly:
			CWsDirectScreenAccess::NewL(this,ETrue);	//creates a DSA object that will not draw to the screen, but will use just the region tracking functionality
			break;
		case EWsClOpCreateClick:
			CreateNewClickHandlerL(*pData.Uid);
			break;
		case EWsClOpSetHotKey:
			{
			if(!KSecurityPolicy_SwEvent().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetHotKey API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			TWindowServerEvent::SetHotKeyL(*pData.SetHotKey);
			}
			break;
		case EWsClOpClearHotKeys:
			{
			if(!KSecurityPolicy_SwEvent().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::ClearHotKeys API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			TWindowServerEvent::ClearHotKeysL(*pData.UInt);
			}
			break;
		case EWsClOpRestoreDefaultHotKey:
			TWindowServerEvent::ResetDefaultHotKeyL(*pData.UInt);
			break;
		case EWsClOpSetShadowVector:
			PPanic(EWservPanicOpcode); //this op code is deprecated, should never be generated by the client
			break;
		case EWsClOpShadowVector:
			PPanic(EWservPanicOpcode); //this op code is deprecated, should never be generated by the client
			break;
		case EWsClOpSetKeyboardRepeatRate:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetKeyboardRepeatRate API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			if ((pData.SetKeyboardRepeatRate->initial.Int()<0) || (pData.SetKeyboardRepeatRate->time.Int()<0))
				{
				User::Leave(KErrArgument);
				}
			CKeyboardRepeat::SetRepeatTime(pData.SetKeyboardRepeatRate->initial,pData.SetKeyboardRepeatRate->time);
			}
			break;
		case EWsClOpGetKeyboardRepeatRate:
			{
			SKeyRepeatSettings settings;
			CKeyboardRepeat::GetRepeatTime(settings.iInitialTime,settings.iTime);
			ReplyBuf(&settings,sizeof(settings));
			}
			break;
		case EWsClOpSetDoubleClick:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetDoubleClick API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			TWsPointer::SetDoubleClick(pData.SetDoubleClick->interval,pData.SetDoubleClick->distance);
			}
			break;
		case EWsClOpGetDoubleClickSettings:
			{
			SDoubleClickSettings settings;
			TWsPointer::GetDoubleClickSettings(settings.iInterval,settings.iDistance);
			ReplyBuf(&settings,sizeof(settings));
			}
			break;
		case EWsClOpEventReady:
			// No need to do anything
			break;
		case EWsClOpGetEvent:
			HandleClientRequestForEventData();
			// Check flag if the group message queue is overflow and has pended messages  
			if (iInternalFlags & EWgMsgQueueOverflow)
				{
				iInternalFlags &= ~EWgMsgQueueOverflow;
				CWsWindowGroup::ReleasePendedMessagesToAllGroups(this);
				}
			break;
		case EWsClOpPurgePointerEvents:
			PurgePointerEvents();
			break;
		case EWsClOpEventReadyCancel:
			CancelClientRequestForEventData();
			break;
		case EWsClOpRedrawReady:
			iInternalFlags&=~EIsPerformingRedrawEvent;
			break;
		case EWsClOpRedrawReadyCancel:
			CancelClientRequestForRedrawEvent();
			break;
		case EWsClOpGetRedraw:
			HandleClientRequestForRedrawData();
			break;
		case EWsClOpPriorityKeyReady:
			// No need to do anything
			break;
		case EWsClOpPriorityKeyReadyCancel:
			CancelClientRequestForPriorityKeyEvent();
			break;
		case EWsClOpGetPriorityKey:
			HandleClientRequestForPriorityKeyData();
			break;
		case EWsClOpNumWindowGroups:
			SetReply(CWsWindowGroup::NumWindowGroups(EFalse,* pData.Int));
			break;
		case EWsClOpNumWindowGroupsAllPriorities:
			SetReply(CWsWindowGroup::NumWindowGroups(ETrue, 0));
			break;
		case EWsClOpNumWindowGroupsOnScreen:
			{
			const TInt screenNumber=pData.NumWinGroups->screenNumber;
			if (screenNumber<0 || screenNumber>=CWsTop::NumberOfScreens())
				PPanic(EWservPanicScreenNumber);
			else
				SetReply(CWsWindowGroup::NumWindowGroupsOnScreen(CWsTop::Screen(screenNumber)->RootWindow()->Child(),(pData.NumWinGroups->priority==EAllPriorities),pData.NumWinGroups->priority));
			}
			break;
		case EWsClOpWindowGroupList:
			{
			const TInt screenNumber=pData.WindowGroupList->screenNumber;
			if (screenNumber<KDummyScreenNumber || screenNumber>=CWsTop::NumberOfScreens())
				PPanic(EWservPanicScreenNumber);
			else
				SetReply(CWsWindowGroup::SendWindowGroupListL(screenNumber,(pData.WindowGroupList->priority==EAllPriorities), pData.WindowGroupList->priority, pData.WindowGroupList->count));
			}
			break;
		case EWsClOpWindowGroupListAllPriorities:
			{
			const TInt screenNumber=pData.WindowGroupList->screenNumber;
			if (screenNumber<KDummyScreenNumber || screenNumber>=CWsTop::NumberOfScreens())
				PPanic(EWservPanicScreenNumber);
			else
				SetReply(CWsWindowGroup::SendWindowGroupListL(screenNumber,ETrue, 0, pData.WindowGroupList->count));
			}
			break;
		case EWsClOpWindowGroupListAndChain:
			SetReply(CWsWindowGroup::SendWindowGroupListAndChainL(EFalse, pData.WindowGroupList->priority, pData.WindowGroupList->count));
			break;
		case EWsClOpWindowGroupListAndChainAllPriorities:
			SetReply(CWsWindowGroup::SendWindowGroupListAndChainL(ETrue, 0, pData.WindowGroupList->count));
			break;
		case EWsClOpGetDefaultOwningWindow:
			{
			const TInt screenNumber = *pData.Int;
			if (screenNumber<KDummyScreenNumber || screenNumber>=CWsTop::NumberOfScreens())
				PPanic(EWservPanicScreenNumber);
			else
				{
				CScreen* screen = (screenNumber ==KDummyScreenNumber) ? iScreen : CWsTop::Screen(screenNumber);
				SetReply(screen->DefaultOwningWindowGroup() ? screen->DefaultOwningWindowGroup()->Identifier():0);
				}
			}
			break;
		case EWsClOpGetFocusWindowGroup:
			{
			const TInt screenNumber = *pData.Int;
			if (screenNumber<KDummyScreenNumber || screenNumber>=CWsTop::NumberOfScreens())
				PPanic(EWservPanicScreenNumber);
			else
				CWsWindowGroup::GetFocusWindowGroupL(screenNumber);
			}
			break;
		case EWsClOpSetWindowGroupOrdinalPosition:
			CWsWindowGroup::WindowGroupFromIdentifierL(pData.SetWindowGroupOrdinalPosition->identifier)->SetOrdinalPosition(pData.SetWindowGroupOrdinalPosition->position);
			break;
		case EWsClOpGetWindowGroupHandle:
			SetReply(CWsWindowGroup::WindowGroupFromIdentifierL(*pData.Int)->ClientHandle());
			break;
		case EWsClOpGetWindowGroupOrdinalPriority:
			SetReply(CWsWindowGroup::WindowGroupFromIdentifierL(*pData.Int)->OrdinalPriority());
			break;
		case EWsClOpGetWindowGroupClientThreadId:
			{
			TThreadId id=CWsWindowGroup::WindowGroupFromIdentifierL(*pData.Int)->WsOwner()->Client().Id();
			ReplyBuf(&id,sizeof(id));
			}
			break;
		case EWsClOpSendEventToWindowGroup:
			{
			CWsWindowGroup* group=CWsWindowGroup::WindowGroupFromIdentifierL(pData.SendEventToWindowGroup->parameter);
			TWsEvent event=pData.SendEventToWindowGroup->event;
			event.SetHandle(group->ClientHandle());
			// Events in enum TEventCode is protected by capabilities
			if (group->WsOwner()!=this && event.Type()>=EEventNull && event.Type()<EEventUser)
				{
				if (event.Type()<EEventPowerMgmt || event.Type()>=EEventReserved)
					{
					if(!KSecurityPolicy_SwEvent().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SendEventToWindowGroup API")))
						User::Leave(KErrPermissionDenied);
					}
				else
					{
					if (!KSecurityPolicy_PowerMgmt().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SendEventToWindowGroup API")))
						User::Leave(KErrPermissionDenied);
					}
				}
			if (event.Type()==EEventKey && event.Key()->iRepeats!=0)
				CKeyboardRepeat::CancelRepeat(NULL);		//Otherwise we will trip an invarient
			if (!group->EventQueue()->QueueEvent(event))
				User::Leave(KErrNoMemory);
			}
			break;
		case EWsClOpSendEventToAllWindowGroup:
		case EWsClOpSendEventToAllWindowGroupPriority:
		case EWsClOpSendEventToOneWindowGroupPerClient:
			{
			TWsEvent event=pData.SendEventToWindowGroup->event;
			if (event.Type()<0)
				User::Leave(KErrArgument);
			if(event.Type()<EEventUser)
				{
				if (event.Type()<EEventPowerMgmt || event.Type()>=EEventReserved)
					{
					if(!KSecurityPolicy_SwEvent().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SendEventToAllWindowGroup API")))
						User::Leave(KErrPermissionDenied);
					}
				else 
					{
					if (!KSecurityPolicy_PowerMgmt().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SendEventToAllWindowGroup API")))
						User::Leave(KErrPermissionDenied);
					}
				}
			if (!CWsWindowGroup::SendEventToAllGroups(aOpcode!=EWsClOpSendEventToAllWindowGroupPriority
													,aOpcode==EWsClOpSendEventToOneWindowGroupPerClient,*pData.SendEventToWindowGroup))
				User::Leave(KErrNoMemory);
			}
			break;
		case EWsClOpSendMessageToWindowGroup:
			{
			CWsWindowGroup* group=CWsWindowGroup::WindowGroupFromIdentifierL(pData.SendMessageToWindowGroup->identifierOrPriority);
			if (group->WsOwner()!=this)
				{
				if (!KSecurityPolicy_SwEvent().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SendMessageToWindowGroup API")))
					{
					User::Leave(KErrPermissionDenied);
					}
				}
			group->QueueMessageL(pData.SendMessageToWindowGroup->uid, pData.SendMessageToWindowGroup->dataLength,* this);
			}
			break;
		case EWsClOpSendMessageToAllWindowGroups:
		case EWsClOpSendMessageToAllWindowGroupsPriority:
			{
			if ((pData.SendMessageToWindowGroup->dataLength<0) || (pData.SendMessageToWindowGroup->dataLength>=(KMaxTInt/2)))
				{
				User::Leave(KErrArgument);
				}
			CWsWindowGroup::SendMessageToAllGroupsL(*this,aOpcode==EWsClOpSendMessageToAllWindowGroups,*pData.SendMessageToWindowGroup);
			}
			break;
		case EWsClOpFetchMessage:
			CWsWindowGroup::WindowGroupFromIdentifierL(pData.FetchMessage->windowGroupIdentifier)->FetchMessageL();
			break;
		case EWsClOpGetWindowGroupNameFromIdentifier:
			ReplyGroupName(CWsWindowGroup::WindowGroupFromIdentifierL(pData.GetWindowGroupNameFromIdentifier->identifier)->GroupName(),pData.GetWindowGroupNameFromIdentifier->maxLength);
			break;
		case EWsClOpFindWindowGroupIdentifier:
			{
			if (pData.FindWindowGroupIdentifier->length<0)
				User::Leave(KErrArgument);
			TPtrC ptr(BufferTPtr((TText*)(pData.FindWindowGroupIdentifier+1),pData.FindWindowGroupIdentifier->length));
			SetReply(CWsWindowGroup::FindWindowGroupL(this, pData.FindWindowGroupIdentifier->identifier,
							pData.FindWindowGroupIdentifier->offset,&ptr,NULL)->Identifier());
			}
			break;
		case EWsClOpFindWindowGroupIdentifierThread:
			SetReply(CWsWindowGroup::FindWindowGroupL(this, pData.FindWindowGroupIdentifierThread->identifier,0,NULL,
											&pData.FindWindowGroupIdentifierThread->threadId)->Identifier());
			break;
		case EWsClOpSetBackgroundColor:
			for(TInt i=0;i<CWsTop::NumberOfScreens();i++)
				{
				CWsTop::Screen(i)->RootWindow()->SetColor(*pData.rgb);
				}
			break;
		case EWsClOpGetBackgroundColor:
			SetReply(iScreen->RootWindow()->BackColor().Internal());
			break;
		case EWsClOpClaimSystemPointerCursorList:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::ClaimSystemPointerCursorList API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			ClaimSystemPointerCursorListL();
			}
			break;
		case EWsClOpFreeSystemPointerCursorList:
			FreeSystemPointerCursorList();
			break;
		case EWsClOpSetSystemPointerCursor:
			{
			CWsObject* pointercursor = NULL;
			if ((pointercursor=HandleToObj(pData.SetSystemPointerCursor->handle, WS_HANDLE_POINTER_CURSOR))==NULL)
				PPanic(EWservPanicSprite);
			
			SetSystemPointerCursorL(pData.SetSystemPointerCursor->number, (CWsPointerCursor* )pointercursor);
			}
			break;
		case EWsClOpClearSystemPointerCursor:
			ClearSystemPointerCursor(*pData.Int);
			break;
		case EWsClOpSetPointerCursorArea:
			{
			if(KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetPointerCursorArea API")))
				{
				if (!iScreen->IsValidScreenSizeMode(*pData.Int))
					PPanic(EWservPanicScreenModeNumber);
				iScreen->SetPointerCursorArea(pData.SetPointerCursorArea->mode,pData.SetPointerCursorArea->area);
				}
			}
			break;
		case EWsClOpPointerCursorArea:
			if (!iScreen->IsValidScreenSizeMode(*pData.Int))
				PPanic(EWservPanicScreenModeNumber);
			
			ReplyRect(iScreen->GetPointerCursorArea(*pData.Int));
			break;
		case EWsClOpSetPointerCursorMode:
			{
			CWsWindowGroup* focusWinGp=CWsTop::FocusWindowGroup();
			if (focusWinGp && focusWinGp->WsOwner()==this)
				{
				TWsPointer::SetPointerCursorMode(*pData.Mode);
				TWsPointer::UpdatePointerCursor();
				}
			}
			break;
		case EWsClOpSetClientCursorMode :
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetPointerCursorModeIfFocused API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			TWsPointer::SetPointerCursorMode(*pData.Mode);
			TWsPointer::UpdatePointerCursor();
			}
			break;
		case EWsClOpPointerCursorMode:
			SetReply(TWsPointer::PointerCursorMode());
			break;
		case EWsClOpSetDefaultSystemPointerCursor:
			SetDefaultSystemPointerCursor(*pData.Int);
			break;
		case EWsClOpClearDefaultSystemPointerCursor:
			SetDefaultSystemPointerCursor(ENoDefaultSystemPointerCursor);
			break;
		case EWsClOpSetPointerCursorPosition:
			{
			CWsWindowGroup* focusWinGp=CWsTop::FocusWindowGroup();
			if ((!focusWinGp || focusWinGp->WsOwner()!=this)&&
				(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetPointerCursorPosition API"))))
				{
				User::Leave(KErrPermissionDenied);
				}
			TWsPointer::SetPointerCursorPos(*pData.Point);
			}
			break;
		case EWsClOpPointerCursorPosition:
			ReplyPoint(TWsPointer::PointerCursorPos());
			break;
		case EWsClOpSetModifierState:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetModifierState API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			TWindowServerEvent::SetModifierState(pData.SetModifierState->modifier,pData.SetModifierState->state);
			}
			break;
		case EWsClOpGetModifierState:
			SetReply(TWindowServerEvent::GetModifierState());
			break;
		case EWsClOpHeapCount:
			SetReply(CWsMemoryManager::Static()->Count());
			break;
 		case EWsClOpDebugInfo:
			DebugInfoL(pData.DebugInfo->iFunction,pData.DebugInfo->iParam,EFalse);
 			break;
 		case EWsClOpDebugInfoReplyBuf:
			DebugInfoL(pData.DebugInfo->iFunction,pData.DebugInfo->iParam,ETrue);
 			break;
		case EWsClOpResourceCount:
			SetReply(iObjectIndex->Count());
			break;
		case EWsClOpHeapSetFail:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::HeapSetFail API")))
				{
				PPanic(EWservPanicPermissionDenied);
				}
#if !defined(_DEBUG)
			if (pData.HeapSetFail->type!=RHeap::ENone)
				TWindowServerEvent::NotifyOom();
#endif
			// if there is a memory manager and we are making the allocator fail next or 
			// deteministic we want to make sure we test both when the memory manager
			// succeeds and fails in allocating after freeing up memory. we do that by
			// remapping the rate and explicitly telling the memory manager to fail:
			// requested rate  | fail on retry, actual rate
			//        1        |    true      ,    1
			//        2        |    false     ,    1
			//        3        |    true      ,    2
			CWsMemoryManager* memoryManager = NULL;
			TInt rate = pData.HeapSetFail->value;
			TBool memoryManagerRetryFail = EFalse;
			if((pData.HeapSetFail->type == RAllocator::EFailNext || pData.HeapSetFail->type == RAllocator::EDeterministic))
				{
				memoryManager = CWsMemoryManager::Static();
				if(memoryManager)
					{
					memoryManagerRetryFail = (rate % 2 == 1);
					rate = rate / 2 + (memoryManagerRetryFail ? 1 : 0);
					}
				}

			switch (pData.HeapSetFail->type)
				{	//This log message means you can safely ignore the allocation failures in between
					//see CWindowServer::ReleaseMemory()
				case RAllocator::ENone:
					RDebug::Printf("WSERV Heap: __DbgSetAllocFail() <<<ENone");
					break;
				case RAllocator::EReset:
					RDebug::Printf("WSERV Heap: __DbgSetAllocFail() <<<EReset");
					break;
				default:	
					if (memoryManagerRetryFail)
						RDebug::Printf("WSERV Heap: Memory Manager set to fail on retry");
					RDebug::Printf("WSERV Heap: __DbgSetAllocFail(EUser,%i,%i)>>>", pData.HeapSetFail->type, rate);
					break;
				}

			if (memoryManager && memoryManagerRetryFail)
				memoryManager->SetFailNextRetry();
			
			if (pData.HeapSetFail->burst >= 0)
				User::__DbgSetBurstAllocFail(RHeap::EUser, pData.HeapSetFail->type, rate, pData.HeapSetFail->burst);
			else
				User::__DbgSetAllocFail(RHeap::EUser, pData.HeapSetFail->type, rate);
			break;
			}
		case EWsClOpRawEvent:
			{
			if(!KSecurityPolicy_SwEvent().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SimulateRawEvent API")))
				{
				PPanic(EWservPanicPermissionDenied);
				}
			TRawEvent event(*pData.RawEvent);
			if (TWsPointer::PreProcessDriverEvent(event))
				TWindowServerEvent::ProcessRawEvent(event);
			}
			break;
		case EWsClOpKeyEvent:
			{
			if(!KSecurityPolicy_SwEvent().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SimulateKeyEvent API")))
				{
				PPanic(EWservPanicPermissionDenied);
				}
			TWindowServerEvent::ProcessKeyEvent(*pData.KeyEvent,0);
			}
			break;
		case EWsClOpLogMessage:
			if (wsDebugLog)
				{
				if (CheckBuffer(*pData.Int, KLogMessageLength))
					wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,BufferTPtr((TText* )(pData.Int+1),*pData.Int),0);
				}
			break;
		case EWsClOpPasswordEntered:
			CWsPassword::PasswordEntered(this);
			break;
		case EWsClOpComputeMode:
			SetComputeMode(*pData.ComputeMode);
			break;
		case EWsClOpSendOffEventsToShell:
			{
			if(!KSecurityPolicy_PowerMgmt().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::RequestOffEvents API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			SetReply(CWsTop::SetSendOffEventsToShell(this,*pData.OffEventsToShell));
			}
			break;
		case EWsClOpGetDefModeMaxNumColors:
			{
			SDefModeMaxNumColors colors;
			const TInt screenNumber = *pData.Int;
			if (screenNumber<KDummyScreenNumber || screenNumber>=CWsTop::NumberOfScreens())
				PPanic(EWservPanicScreenNumber);
			else
				{
				CScreen* screen = (screenNumber==KDummyScreenNumber)?iScreen:CWsTop::Screen(screenNumber);
				screen->MaxNumColors(colors.iColors,colors.iGrays);
				colors.iDisplayMode=screen->FirstDefaultDisplayMode();
				}
			ReplyBuf(&colors,sizeof(colors));
			}
			break;
		case EWsClOpGetColorModeList:
			{
			const TInt screenNumber = *pData.Int;
			if (screenNumber<KDummyScreenNumber || screenNumber>=CWsTop::NumberOfScreens())
				PPanic(EWservPanicScreenNumber);
			else
				SetReply((screenNumber==KDummyScreenNumber) ? iScreen->ColorModesFlag() : CWsTop::Screen(screenNumber)->ColorModesFlag());
			}
			break;
		case EWsClOpSetDefaultFadingParams:
			{
			if(KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetDefaultFadingParameters API")))
				{
				iScreen->SetFadingParams(WservEncoding::ExtractFirst8BitValue(*pData.UInt),WservEncoding::ExtractSecond8BitValue(*pData.UInt));
				}
			}
			break;
		case EWsClOpPrepareForSwitchOff:
			{
			if(KSecurityPolicy_PowerMgmt().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::PrepareForSwitchOff API")))
				{
				}
			}
			break;
		case EWsClOpSetFaded:
			{
			// Deprecated - retained for BC with applications that retrieve the fade count
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetSystemFaded API")))
				User::Leave(KErrPermissionDenied);

			TUint8 blackMap;
			TUint8 whiteMap;
			if (pData.SetSystemFaded->UseDefaultMap())
				iScreen->GetFadingParams(blackMap,whiteMap);
			else
				pData.SetSystemFaded->GetFadingParams(blackMap,whiteMap);
			
			iScreen->RootWindow()->SetSystemFaded(pData.SetSystemFaded->Faded(),blackMap,whiteMap);
			}
		break;
		case EWsClOpLogCommand:
			CWsTop::LogCommand(*pData.LogCommand);
			break;
#if defined(__WINS__)
		case EWsClOpRemoveKeyCode:
			iInternalFlags&=~ERemoveKeyCode;
			if (*pData.Bool)
				iInternalFlags|=ERemoveKeyCode;
			break;
		case EWsClOpSimulateXyInput:
			TWsPointer::SetXyInputType(static_cast<TXYInputType>(*pData.XyInput));
			break;
#endif
		case EWsClOpNoFlickerFree:
			PPanic(EWservPanicOpcode); //not supported anymore
			break;
		case EWsClOpSetFocusScreen:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetFocusScreen API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			TInt focusScreen=*pData.Int;
			if (focusScreen>=0 && focusScreen<CWsTop::NumberOfScreens())
				SetReply(CWsTop::SetCurrentFocusScreen(focusScreen));
			else
				SessionPanic(EWservPanicScreenNumber);
			break;
			}
		case EWsClOpGetFocusScreen:
			SetReply(CWsTop::CurrentFocusScreen()->ScreenNumber());
			break;
		case EWsClOpGetNumberOfScreens:
			SetReply(CWsTop::NumberOfScreens());
			break;
		case EWsClOpClearAllRedrawStores:
			CWsTop::ClearAllRedrawStores();
			break;
		case EWsClOpGetGraphicMessage:
			iGraphicMessageQueue.GetGraphicMessage();
			break;
		case EWsClOpGraphicMessageCancel:
			iGraphicMessageQueue.CancelRead();
			break;
		case EWsClOpGraphicAbortMessage:
			iGraphicMessageQueue.AbortMessage(*pData.Int);
			break;
		case EWsClOpGraphicFetchHeaderMessage:
			SetReply(iGraphicMessageQueue.TopClientHandle());
			break;
		case EWsClOpRegisterSurface:
			SetReply(RegisterSurface(pData));
			break;
		case EWsClOpUnregisterSurface:
			UnregisterSurface(pData);
			break;
		case EWsClOpSetCloseProximityThresholds:
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),
					__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetCloseProximityThresholds")))
				{
				User::Leave(KErrPermissionDenied);
				}
			SetReply(TWsPointer::SetCloseProximityThresholds(pData.ZThresholdPair->enterThreshold,
															 pData.ZThresholdPair->exitThreshold));
			break;
		case EWsClOpSetHighPressureThresholds:
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(ClientMessage(),
					__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::SetHighPressureThresholds")))
				{
				User::Leave(KErrPermissionDenied);
				}
			SetReply(TWsPointer::SetHighPressureThresholds(pData.ZThresholdPair->enterThreshold,
					 									   pData.ZThresholdPair->exitThreshold));
			break;
		case EWsClOpGetEnterCloseProximityThreshold:
			SetReply(TWsPointer::GetEnterCloseProximityThreshold());
			break;
		case EWsClOpGetExitCloseProximityThreshold:
			SetReply(TWsPointer::GetExitCloseProximityThreshold());
			break;
		case EWsClOpGetEnterHighPressureThreshold:
			SetReply(TWsPointer::GetEnterHighPressureThreshold());
			break;
		case EWsClOpGetExitHighPressureThreshold:
			SetReply(TWsPointer::GetExitHighPressureThreshold());
			break;
		case EWsClOpCreateDrawableSource:
			CreateDrawableSourceL(*pData.CreateDrawableSource);
			break;
		case EWsClOpIndicateAppOrientation:
			IndicateAppOrientation(*pData.Orientation);
			break;
		case EWsClOpUnregisterAllTFXEffect:
			RDebug::Printf("[Bug 3344] OpCode EWsClOpUnregisterAllTFXEffect not supported.");
			break;
		default:
			PPanic(EWservPanicOpcode);
			break;
		}
	}
/**	Debug information accessor.
 * 	
 * 
 **/
void CWsClient::DebugInfoL(TInt aFunction, TInt aParam, TBool aHasReplyBuf) const
	{
	if (aFunction & EWsDebugClassMask)
		SetReply(DebugInfoClassifiedL(aFunction,aParam,aHasReplyBuf));
	else
		DebugInfoUnclassifiedL(aFunction,aParam,aHasReplyBuf);
	}

/**	A wide variety of generally unconnected debug enquiries.
 * 
 * 
 **/
void CWsClient::DebugInfoUnclassifiedL(TInt aFunction, TInt aParam, TBool aHasReplyBuf) const
	{
	switch(aFunction)
		{
		case EWsDebugInfoHeap:
			if (aHasReplyBuf)
				{
				TWsDebugHeapInfo heapInfo;
				RHeap& heap=User::Heap();
				heapInfo.iCount=heap.AllocSize(heapInfo.iTotal);
				heapInfo.iAvailable=heap.Available(heapInfo.iLargestAvailable);
				ReplyBuf(&heapInfo,sizeof(heapInfo));
				}
			SetReply(KErrArgument);
			break;
		case EWsDebugSetCheckHeapOnDisconnectClient:
			if (!KSecurityPolicy_PowerMgmt().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::DebugInfo API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			CWsTop::SetCheckHeapOnDisconnectClient(this);
			break;
		case EWsDebugSetCheckHeapOnDisconnectMode:
			if (!KSecurityPolicy_PowerMgmt().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RWsSession::DebugInfo API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			CWsTop::SetCheckHeapOnDisconnectMode(STATIC_CAST(TWsCheckHeapOnDisconnectMode,aParam));
			break;
		case EWsDebugFetchCheckHeapResult:
			SetReply(CWsTop::FetchCheckHeapResult());
			break;
		case EWsDebugSetEventQueueTest:
			CWsWindowGroup::SetEventQueueTestState(aParam);
			break;
		default:
			SetReply(KErrNotSupported);
			break;
		}
	}
	
/**	Selects a debug function based on the class of debug query.
 * 
 * 
 **/
TInt CWsClient::DebugInfoClassifiedL(TInt aFunction, TInt aParam, TBool aHasReplyBuf) const
	{//Duplicating the meanings of Classified... this would be a good place for a security check
	//first part of param is always screen number
	TInt buffSpace=aHasReplyBuf?ReplyBufSpace():0;
	if (buffSpace<0)
		return (buffSpace);
		
	const TInt screenNumber = (aParam&EWsDebugArgScreenMask)>>EWsDebugArgScreenShift;
	const TInt functionClass = (aFunction&EWsDebugClassMask);
	CScreen* screen = NULL;
	if (functionClass<EWsDebugClassNonScreen)
		{
		if (screenNumber<0 || screenNumber>=CWsTop::NumberOfScreens())
			return (KErrArgument);

		screen = CWsTop::Screen(screenNumber);
		}
	WS_ASSERT_DEBUG(screen, EWsPanicNoScreen);
	switch (aFunction&EWsDebugClassMask)
		{
		case EWsDebugClassScreenUiElement:
			return DebugInfoScreenUiL(aFunction,aParam,buffSpace,*screen);
		case EWsDebugClassScreenElementSet:
			return DebugInfoScreenElementSetL(aFunction, aParam, buffSpace, *screen);
		case EWsDebugClassElementSetWindow:
			return DebugInfoElementSetWindowL(aFunction, aParam, buffSpace, *screen);
		case EWsDebugClassElementSetElement:
			return DebugInfoElementSetElementL(aFunction, aParam, buffSpace, *screen);

		case EWsDebugClassClientWindow:
		default:
			return (KErrNotSupported);
		}
	}

/**	Returns debug info about the UIElement entries for a screen.
 * 	This describes the general state or size of the element set.
 * 	It is indexed via screen num, and optionally element index.
 * 	Element index MUST BE 0 when not required.
 * 	@return size of buffer required or error code.
 **/
TInt CWsClient::DebugInfoScreenUiL(TInt aFunction, TInt /* aParam */, TInt aReplyBufSize, CScreen& aScreen) const
	{
	switch(aFunction)
		{
		case EWsDebugGetFastpathMode:
			{
			// obsolete through preq2669
            WS_ASSERT_DEBUG(EFalse, EWsPanicDrawCommandsInvalidState);
			return KErrNotSupported;
			}
			
		case EWsDebugSetFastpathMode:
			{
            // obsolete through preq2669
            WS_ASSERT_DEBUG(EFalse, EWsPanicDrawCommandsInvalidState);
            return KErrNotSupported;
			}
			
		case EWsDebugGetUIElementInfoList:
			{
            // obsolete through preq2669
            WS_ASSERT_DEBUG(EFalse, EWsPanicDrawCommandsInvalidState);
            return KErrNotSupported;
			}
			
		case EWsDebugGetUIElementBase:
			{
            // obsolete through preq2669
            WS_ASSERT_DEBUG(EFalse, EWsPanicDrawCommandsInvalidState);
            return KErrNotSupported;
			}

		case EWsDebugGetUIElementIds:
			{
            // obsolete through preq2669
            WS_ASSERT_DEBUG(EFalse, EWsPanicDrawCommandsInvalidState);
            return KErrNotSupported;
			}

		case EWsDebugGetSceneElementIdOrder:
			{
            // obsolete through preq2669
            WS_ASSERT_DEBUG(EFalse, EWsPanicDrawCommandsInvalidState);
            return KErrNotSupported;
			}
		case EWsDebugSetFastpathTestMode:
			{
            // obsolete through preq2669
            WS_ASSERT_DEBUG(EFalse, EWsPanicDrawCommandsInvalidState);
            return KErrNotSupported;
			}
			
		case EWsDebugSetFastpathOomMode:
			{
            // obsolete through preq2669
            WS_ASSERT_DEBUG(EFalse, EWsPanicDrawCommandsInvalidState);
            return KErrNotSupported;
			}
		case EWsDebugGetUIElementConfig:
			{
			return DebugReturnConfig(aReplyBufSize,&aScreen.UiElement(),0);

			}
		default:
			return (KErrNotSupported);
		}
	}

/**	Returns debug info about the CWindowElementSet entry for a screen.
 * 	This describes the general state or size of the element set
 * 	It is indexed via screen num.
 * 	@return size of buffer required or error code.
 **/
TInt CWsClient::DebugInfoScreenElementSetL(TInt aFunction, TInt /*aParam*/, TInt aReplyBufSize, const CScreen& aScreen) const
	{
	const CWindowElementSet& elementSet = aScreen.WindowElements();
	switch (aFunction)
		{
		case EWsDebugSerialSurfacesUpdated:
			return 0;

		case EWsDebugSurfaceWindowList:
			{
			TInt outSize = elementSet.Count() * sizeof(TWsDebugWindowId);
			if (outSize<=aReplyBufSize)
				{//can stream, so I shall!
				for (TInt index = 0; index < elementSet.Count(); index++)
					{
					const CWsClientWindow& win = *elementSet.DebugWindowAt(index);
					TWsDebugWindowId id=
						{
						win.ClientHandle(), 0
						};
					if (win.WsOwner()!=this)
						id.iOtherGroupId=win.WinGroup()->Identifier();
					ReplyBuf(&id, sizeof(id));
					}
				}
			return outSize;
			}
		default:
			return (KErrNotSupported);

		}
	}

/**	Returns debug info about a CWindowElement entry.
 * 	This describes the window or the background element(s)
 * 	It is indexed via screen num, and index in elementset.
 * 	@return size of buffer required or error code.
 **/
TInt CWsClient::DebugInfoElementSetWindowL(TInt aFunction, TInt aParam,
		TInt aReplyBufSize, const CScreen& aScreen) const
	{
	const CWindowElementSet& elementSet = aScreen.WindowElements();
	TUint winIndex = (aParam & EWsDebugArgWindowMask) >> EWsDebugArgWindowShift;
	const TAttributes* winElement = elementSet.DebugBackgroundAt(winIndex);
	if (winElement == NULL)
		{
		return KErrArgument;
		}

	MWsElement* backElement = winElement->iElement;
	TInt placedCount = elementSet.DebugPlacedCountAt(winIndex);
	switch (aFunction)
		{
		case EWsDebugElementIdList:
			{
			TInt retVal = (placedCount + 1) * sizeof(MWsElement*);
			if (retVal<aReplyBufSize)
				{
				ReplyBuf(&backElement, sizeof(MWsElement*));
				for (TInt index=0; index<placedCount; index++)
					ReplyBuf(&elementSet.DebugPlacedAt(winIndex, index)->iElement, sizeof(MWsElement*));
				}
			return retVal;
			}
		case EWsDebugBackgroundConfig:
			if (backElement == NULL)
				return KErrNotFound;
			else
				return DebugReturnConfig(aReplyBufSize, backElement,
						winElement->DebugFlags());
		case EWsDebugBackgroundBase:
			if (backElement == NULL)
				return KErrNotFound;
			else
				return DebugReturnBase(aReplyBufSize, backElement);
		case EWsDebugBackgroundFlags:
			return DebugReturnFlags(aReplyBufSize, backElement,
					winElement->DebugFlags());
		default:
			return KErrNotSupported;
		//This method can also be extended to return region and state information from the associated window
		}
	}

/**	Returns debug info about a placed element.
 * 	It is indexed via screen num, index in elementset, and index in placed element array.
 * 	@return size of buffer required or error code.
 **/
TInt CWsClient::DebugInfoElementSetElementL(TInt aFunction, TInt aParam,
		TInt aReplyBufSize, const CScreen& aScreen) const
	{
	const CWindowElementSet& elementSet = aScreen.WindowElements();
	TUint winIndex = (aParam & EWsDebugArgWindowMask) >> EWsDebugArgWindowShift;
	TUint placeIndex = (aParam & EWsDebugArgElementMask) >> EWsDebugArgElementShift;
	const TAttributes* placedElement = elementSet.DebugPlacedAt(winIndex, placeIndex);
	if (placedElement == NULL)
		{
		return KErrArgument;
		}

	MWsElement* element = placedElement->iElement;
	if (element == NULL)
		{
		return KErrNotFound;
		}

	switch (aFunction)
		{
		case EWsDebugPlacedConfig:
			return DebugReturnConfig(aReplyBufSize, element,
					placedElement->DebugFlags());
		case EWsDebugPlacedBase:
			return DebugReturnBase(aReplyBufSize, element);
		case EWsDebugPlacedFlags:
			return DebugReturnFlags(aReplyBufSize, element,
					placedElement->DebugFlags());
		default:
			return KErrNotSupported;
		}
	}

/**	Returns a filled in TSurfaceConfiguration from an MWsElement.
 * 	Data is usually copied if the buffer is big enough
 * 	@return the size of buffer required, or zero if the buffer is acceptable
 **/
TInt CWsClient::DebugReturnConfig(TInt aReplyBufSize, MWsElement* aElement, TInt /*aFlags*/) const
	{
	if (aElement == NULL)
		{
		return KErrNotReady;
		}

	TSurfaceConfiguration config(aReplyBufSize);
	TInt retVal=config.Size();
	if (aReplyBufSize)
		{
		retVal = CWindowElementSet::GetConfiguration(config, *aElement);
		if (retVal==KErrNone)
			{
			ReplyBuf(&config, config.Size()); //return code is 0 = "just right"
			}
		}
	return retVal;
	}

/**	Returns the base region of the element.
 * 	This region is element relative. There are a number of ways that this does not match the input region:
 * 	First, if no region is specified then the extent rectangle is returned
 * 	Any region returned has all negative ordinate values clipped. 
 * 	Positive values may exceed the extent, but negative values are never returned. 
 *  Internally, a region which is only negative is "remembered illegally", but an empty region is returned.
 **/
TInt CWsClient::DebugReturnBase(TInt /*aReplyBufSize*/, const MWsElement* /*aElement*/)const
	{
	return KErrNotSupported;
	}

/**	Returns the flags associated with the given element.
 * 	2 words are inserted.
 * 	One represents the MWsElement flags, the other represents CWindowElement or UIElement flags
 * 	@return length of two words
 **/
TInt CWsClient::DebugReturnFlags(TInt aReplyBufSize, const MWsElement* /*aElement*/, TInt aFlags)const
	{
	const TInt KArraySize=2;
	if (aReplyBufSize>KArraySize*sizeof(TInt))
		{
		// First field is for flags from scene element if any
		TInt returns[KArraySize]=
			{
			0, aFlags
			};
		ReplyBuf(returns,KArraySize*sizeof(TInt));
		}
	return KArraySize*sizeof(TInt);
	}

/**	Packages a region for return ans an array of rectangles.
 * 	If the buffer is big enough the data is transferred
 * 	@return the buffer size required, or an error code if an empty or null pointer is passed in
 **/
TInt CWsClient::DebugReturnRegion(TInt aReplyBufSize, const TRegion* aRegion, TInt aErrCodeIfEmpty)const
	{
	if (aRegion==NULL)
		return KErrNotReady;
	
	const TInt returnSize=aRegion->Count()*sizeof(TRect);
	if (returnSize==0)
		return aErrCodeIfEmpty;
	
	if (returnSize<=aReplyBufSize)
		ReplyBuf(aRegion->RectangleList(),returnSize);
	
	return returnSize;
	}

void CWsClient::ReplyGroupName(HBufC* aName, TInt aMaxLength)	// static
	{
	if (aName)
		{
		if (aName->Length()>aMaxLength)
			{
			ReplyBuf(aName->Left(aMaxLength));
			SetReply(KErrOverflow);
			}
		else
			ReplyBuf(*aName);
		}
	else
		ReplyBuf(KNullDesC);
	}

void CWsClient::TriggerRedraw()
	{
	RedrawQueue()->TriggerRedraw();
	}

void CWsClient::UpdateWindowOrdinalPrioritys()
	{
	for(CWsWindowGroup* win=iScreen->RootWindow()->Child();win;win=win->NextSibling())
		{
		if (win->WsOwner()==this)
			win->UpdateOrdinalPriority(ETrue);
		}
	}

void CWsClient::DeleteSystemPointerListEntry(TInt aIndex)
	{
	PointerCursor (aIndex)->Close();
	iSystemPointerCursors->Delete(aIndex);
	}

CWsPointerCursor* CWsClient::SystemPointerCursor(TInt aIndex)
	{
	TInt arrayIndex;
	if (iSystemPointerCursors)
		{
		if (FindCursorArrayItem(iSystemPointerCursors, aIndex, arrayIndex))
			return PointerCursor(arrayIndex);

		// Cursor not defined so try for default cursor
		if (FindCursorArrayItem(iSystemPointerCursors, 0, arrayIndex))
			return PointerCursor(arrayIndex);
		}

	// If that fails simply return NULL for no cursor
	return NULL;
	}

void CWsClient::SetSystemPointerCursorL(TInt aIndex, CWsPointerCursor* aCursor)
	{
	if (iSystemPointerCursorListOwner!=this)
		PPanic(EWservPanicNotSystemPointerCursorListOwner);
	
	TInt arrayIndex = KErrNotFound;
	if (FindCursorArrayItem(iSystemPointerCursors, aIndex, arrayIndex))
		{
		PointerCursor(arrayIndex)->Close();
		PointerCursor(arrayIndex) = aCursor;
		}
	else
		{
		TWsCursorArrayItem entry;
		entry.iIndex=aIndex;
		entry.iCursor=aCursor;
		iSystemPointerCursors->InsertIsqL(entry, iCursorKey);
		}
	
	aCursor->Open();
	if (aIndex==iDefaultSystemPointerCursorIndex)
		iDefaultSystemPointerCursor=aCursor;
	TWsPointer::UpdatePointerCursor();
	}

void CWsClient::ClearSystemPointerCursor(TInt aIndex)
	{
	if (iSystemPointerCursorListOwner!=this)
		PPanic(EWservPanicNotSystemPointerCursorListOwner);
	
	TInt arrayIndex;
	if (FindCursorArrayItem(iSystemPointerCursors, aIndex, arrayIndex))
		{
		DeleteSystemPointerListEntry(arrayIndex);
		if (aIndex==iDefaultSystemPointerCursorIndex)
			iDefaultSystemPointerCursor=NULL;
		}
	}

void CWsClient::ClaimSystemPointerCursorListL()
	{
	if (iSystemPointerCursorListOwner)
		User::Leave(KErrInUse);
	
	const TInt systemPointerCursorGranularity = 4;
	iSystemPointerCursors = new(ELeave) CArrayFixFlat<TWsCursorArrayItem> (systemPointerCursorGranularity);
	iSystemPointerCursorListOwner=this;
	}

void CWsClient::FreeSystemPointerCursorList()
	{
	if(iSystemPointerCursorListOwner == this)
		{
		iSystemPointerCursorListOwner = NULL;
		
		while(iSystemPointerCursors->Count()>0)
			DeleteSystemPointerListEntry(0);
		
		iDefaultSystemPointerCursor = NULL;
		iDefaultSystemPointerCursorIndex = 0;
		
		delete iSystemPointerCursors;
		iSystemPointerCursors = NULL;
		}
	}

void CWsClient::SetDefaultSystemPointerCursor(TInt aIndex)
	{
	TInt arrayIndex;
	if (iSystemPointerCursorListOwner != this)
		PPanic(EWservPanicNotSystemPointerCursorListOwner);
	
	iDefaultSystemPointerCursorIndex = aIndex;
	iDefaultSystemPointerCursor = NULL;
	
	if (aIndex != ENoDefaultSystemPointerCursor &&
		FindCursorArrayItem(iSystemPointerCursors, aIndex, arrayIndex))
		iDefaultSystemPointerCursor = PointerCursor (arrayIndex);
	else
		iDefaultSystemPointerCursor = NULL;
	
	TWsPointer::UpdatePointerCursor();
	}

TBool CWsClient::FindCursorArrayItem(CArrayFixFlat<TWsCursorArrayItem>* aCursorArray,
									 TInt aIndex,TInt& aPosition)
	{
	if (!aCursorArray)
		return EFalse; // No hit if the array isn't even allocated

	TWsCursorArrayItem entry;
	entry.iIndex = aIndex;
	return aCursorArray->FindIsq(entry, iCursorKey, aPosition)==KErrNone;
	}

void CWsClient::SetClientPriority()
	{
	if (iComputeMode!=RWsSession::EPriorityControlDisabled)
		{
		Client().SetProcessPriority(
				iComputeMode==RWsSession::EPriorityControlComputeOn 
			||	CWsTop::FocusWindowGroupOwner()!=this
				?	EPriorityBackground
			  	:	EPriorityForeground);
		}
	}

void CWsClient::SetComputeMode(RWsSession::TComputeMode aComputeMode)
	{
	if (aComputeMode!=RWsSession::EPriorityControlDisabled 
		&& aComputeMode	!=RWsSession::EPriorityControlComputeOn 
		&& aComputeMode	!=RWsSession::EPriorityControlComputeOff)
		PPanic(EWservPanicSetComputeMode);
	iComputeMode=aComputeMode;
	SetClientPriority();
	}

void CWsClient::CompleteMessage(const RMessage2& aMessage,TInt aReason)
	{
	WS_ASSERT_DEBUG(!aMessage.IsNull(),EWsPanicPanicFlagError);
	if (iInternalFlags&EPanicClientAsSoonAsPossible)
		{
		aMessage.Panic(KWSERVSessionPanicCategory,iPanicReason);
		iInternalFlags&=~EPanicClientAsSoonAsPossible;
		}
	else
		{
		if(!iResponseHandle)
			aMessage.Complete(aReason);
		else
			{
			aMessage.Complete(*iResponseHandle);
			iResponseHandle=NULL;
			}
		}
	}

void CWsClient::ServiceError(const RMessage2& /*aMessage*/,TInt aError)
	{
	CompleteMessage(iClientMessage,aError);	// (finish)
	}

void CWsClient::ServiceL(const RMessage2 &aMessage)	// (step ##1)
//
// Handle messages for the window server server.
//
	{
	iClientMessage=aMessage; // from now on use always the message stored in the session
	if (iInternalFlags&EPanicClientAsSoonAsPossible)
		{
		iClientMessage.Panic(KWSERVSessionPanicCategory,iPanicReason);
		}
	else
		{
		iPanicReason=KErrNone;
		iReply=KErrNone;
		TBool completeRequest=ETrue;
		DoServiceL(iClientMessage, completeRequest);	// (call #2)
		if (completeRequest)
			CompleteMessage(iClientMessage,iReply);	// (finish)
		}
	}

void CWsClient::SetResponseHandle(RHandleBase* aHandle)
	{
	iResponseHandle = aHandle;
	}

void CWsClient::DoServiceL(const RMessage2& aMessage, TBool& aCompleteRequest)	// (step #2)
	{
	if (aMessage.IsNull())
		PPanic(EWservPanicNullMessageFromClient);
	
	WS_ASSERT_DEBUG(iInternalFlags&EFinishedProcessingCommands,EWsPanicCommandBufferStillBeingProcessed);

	const TInt function = aMessage.Function();
	switch (function)
		{
	case EWservMessInit:
		StartInitializationL(iConnectionId++);
		break;
	case EWservMessSyncMsgBuf:
	case EWservMessCommandBuffer:	// Process command buffer containing draw ops
		{
		if (!IsInitialised())			
			PPanic(EWservPanicUninitialisedClient);
			
		const TInt err = aMessage.Read(KBufferMessageSlot, iCmdBuf);
		if (!err)
			{
			iReplyOffset=0;
			iDestObj=NULL;
			iNextCmd=iCmdBuf.Ptr();
			DoServiceCommandBuf();	// (call #3.1)
			}
		else if (err!=KErrDied)
			PPanic(EWservPanicDescriptor);
		
		if(function == EWservMessCommandBuffer && CWsTop::FinishEveryFlush())
			Screen()->DoRedrawNow();
		else
			aCompleteRequest=EFalse;
		}
		break;
	case EWservMessShutdown:
		{
		if(!KSecurityPolicy_PowerMgmt().CheckPolicy(ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for EWservMessShutdown message ")))
			PPanic(EWservPanicPermissionDenied);

		if (aMessage.Int0() == EWservShutdownCheck)
			CWsTop::Exit();
		else
			PPanic(EWservPanicHandle);
		}
		break;
	case EWservMessFinish:
		Screen()->DoRedrawNow();
		break;
	default:
		if (function&EWservMessAsynchronousService)
			{
			TRAPD(err, ExecuteAsyncClientCommandL((function&~EWservMessAsynchronousService), aMessage));	// (call #3.2)
			aCompleteRequest = (err!=KErrNone);
			WS_ASSERT_DEBUG((!(iInternalFlags&EPanicClientAsSoonAsPossible))==(!aCompleteRequest), EWsPanicPanicFlagError);
			}
		else if (function&EWservMessAnimDllAsyncCommand)
			{
			CWsAnimDll* const animDll = static_cast<CWsAnimDll*>(HandleToObj(aMessage.Int0(), WS_HANDLE_ANIM_DLL));
			if (!animDll)
				{
				SessionPanic(EWservPanicHandle);
				break;
				}
				
			// it looks wrong to call CommandReply here (not AsyncCommandReply, or something, which doesn't exist), but basically, we can't add a virtual AsyncCommandReplyL to CAnim to correspond to RAnim::AsyncCommandReply as that would break binary and source compatibility for Anim plug-ins; instead asynchronous commands are done by the plug-in having to complete the RMessagePtr2 returned by iFunctions->Message() (or a copy of of that object) for asynchronous commands only
			TRAPD(err, animDll->AnimObjectL(aMessage.Int1())->CommandReply(function&~EWservMessAnimDllAsyncCommand,NULL));
			aCompleteRequest=(err!=KErrNone);
			WS_ASSERT_DEBUG((!(iInternalFlags&EPanicClientAsSoonAsPossible))==(!aCompleteRequest), EWsPanicPanicFlagError);
			}
		else
			SetReply(KErrNotSupported);
		}
	}
void CWsClient::RemoteRead(TDes16& aDes, TInt aOffset)
	{
	if (iClientMessage.Read(KRemoteBufferMessageSlot,aDes,aOffset)!=KErrNone)
		SessionPanic(EWservPanicDescriptor);
	}

void CWsClient::RemoteRead(TDes8& aDes, TInt aOffset)
	{
	if (iClientMessage.Read(KRemoteBufferMessageSlot,aDes,aOffset)!=KErrNone)
		SessionPanic(EWservPanicDescriptor);
	}

void CWsClient::RemoteReadL(TDes16& aDes, TInt aOffset)
	{
	iClientMessage.ReadL(KRemoteBufferMessageSlot,aDes,aOffset);
	}

void CWsClient::RemoteReadL(TDes8& aDes, TInt aOffset)
	{
	iClientMessage.ReadL(KRemoteBufferMessageSlot,aDes,aOffset);
	}

void CWsClient::InitStaticsL()
	{
	}

void CWsClient::DeleteStatics()
	{
	if (iTextCursorArray)
		{
		const TInt count = iTextCursorArray->Count();
		for (TInt index=0;index<count;index++)
			delete iTextCursorArray->At(index).iCursor;

		delete iTextCursorArray;
		iTextCursorArray = NULL;
		}

	// coverity[extend_simple_error] 
	}
		
/* CWsClient implementing MWsClient */

TBool CWsClient::HasCapability(TCapability aCapability) const
	{
	return iClient.HasCapability(aCapability);
	}

TSecureId CWsClient::SecureId() const
	{
	return iClient.SecureId();
	}

TVendorId CWsClient::VendorId() const
	{
	return iClient.VendorId();
	}

/**
Makes a new copy of the aData. so it could be deleted after this call.
*/
TInt CWsClient::SendMessage(const CWsGraphicDrawer* aOnBehalfOf,const TDesC8& aData)
	{
	CWsGraphicMessageQueue::CMessage* msg = CWsGraphicMessageQueue::CMessage::New(aData);
	if(msg)
		return SendMessage(aOnBehalfOf, *msg);
	
	return KErrGeneral;
	}

/** adds a message to the message queue
@return a postive number to uniquely identify the message
*/
TInt CWsClient::SendMessage(const CWsGraphicDrawer* aOnBehalfOf,CWsMessageData& aData)
	{
	WS_ASSERT_DEBUG(aData.Data().Size(), EWsPanicWsGraphic);
	const CWsGraphicDrawerObject* obj = DrawerObject(aOnBehalfOf);
	WS_ASSERT_DEBUG(obj, EWsPanicWsGraphic);
	if(obj)
		{
		// assign message id
		if(iMessageIdSeq == KMaxTInt) // Wrap if iMessageIdSeq has reached KMaxTInt
			iMessageIdSeq = 0;
		
		iMessageIdSeq++;
		// correct other handles
		aData.iClientHandle = (obj->ClientHandle() | (EWsGraphMessageTypeUser & 0x03));
		aData.iDrawer = obj->Drawer();
		aData.iId = iMessageIdSeq;
		iGraphicMessageQueue.Queue(&aData);
		return iMessageIdSeq;
		}

	return KErrGeneral;
	}


CWsGraphicDrawerObject* CWsClient::DrawerObject(const CWsGraphicDrawer* aDrawer)
	{
	const TInt count = ObjectIndex()->Length();
	for(TInt i=0; i<count; i++)
		{
		CWsObject* obj = const_cast<CWsObject*>(ObjectIndex()->At(i));
		if(obj && (WS_HANDLE_GRAPHIC_DRAWER == obj->Type()))
			{
			CWsGraphicDrawerObject* candidate = static_cast<CWsGraphicDrawerObject*>(obj);
			if(candidate->Drawer() == aDrawer)
				return candidate;
			}
		}

	return NULL;
	}

const CWsGraphicDrawerObject* CWsClient::DrawerObject(const CWsGraphicDrawer* aDrawer) const
	{
	CWsObjectIx* objectIndex = const_cast<CWsClient*>(this)->ObjectIndex();
	const TInt count = objectIndex->Length();
	for(TInt i=0; i<count; i++)
		{
		CWsObject* obj = const_cast<CWsObject*>(objectIndex->At(i));
		if(obj && (WS_HANDLE_GRAPHIC_DRAWER == obj->Type()))
			{
			const CWsGraphicDrawerObject* candidate = static_cast<CWsGraphicDrawerObject*>(obj);
			if(candidate->Drawer() == aDrawer)
				return candidate;
			}
		}

	return NULL;
	}

TInt CWsClient::RegisterSurface(const TWsClCmdUnion& pData)
	{
	TInt screenNumber = pData.SurfaceRegister->screenNumber;
	if (screenNumber<0 || screenNumber>=CWsTop::NumberOfScreens())
		{
		PPanic(EWservPanicScreenNumber);
		}
	if (pData.SurfaceRegister->surfaceId.Type() == TSurfaceId::EScreenSurface
			|| pData.SurfaceRegister->surfaceId.IsNull())
		{
		PPanic(EWservPanicInvalidSurface);
		}

	CRegisteredSurfaceMap* surfaceMap = CWsTop::Screen(screenNumber)->SurfaceMap();
	const TSurfaceId& surfaceId = pData.SurfaceRegister->surfaceId;
	TInt err = surfaceMap->Add(*this,surfaceId);

	switch(err)
		{
	case KErrNone:
	case KErrNoMemory:
	case KErrInUse:
	case KErrArgument:
		break;
	default:
		PPanic(EWservPanicInvalidSurface);
		}
	return err;
	}

void CWsClient::UnregisterSurface(const TWsClCmdUnion& pData)
	{
	TInt screenNumber = pData.SurfaceRegister->screenNumber;
	if (screenNumber<0 || screenNumber>=CWsTop::NumberOfScreens())
		{
		PPanic(EWservPanicScreenNumber);
		}
	TInt err = CWsTop::Screen(screenNumber)->SurfaceMap()->Remove(*this,pData.SurfaceRegister->surfaceId);
	WS_ASSERT_DEBUG((err==KErrNone||err==KErrNotFound), EWsPanicSurfaceMapError);
	}

void CWsClient::CreateDrawableSourceL(const TWsClCmdCreateDrawableSource& aDrawableSourceData)
	{
	CWsDrawableSource* drawableSource = new(ELeave) CWsDrawableSource(this);
	CleanupStack::PushL(drawableSource);
	drawableSource->ConstructL(aDrawableSourceData);
	CleanupStack::Pop();
	}
	
void CWsClient::IndicateAppOrientation(TRenderOrientation aOrientation)
	{
	iIndicatedAppOrientation = aOrientation;
	CWsTop::CheckRenderOrientation();
	}

TInt CWsClient::GetIndicatedAppOrientation()
	{
	return iIndicatedAppOrientation;
	}

//
// class CWsCliObj
//

CWsCliObj* CWsCliObj::NewL(CWsClient* aOwner)
	{
	CWsCliObj* self = new(ELeave) CWsCliObj(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CWsCliObj::CWsCliObj(CWsClient *aOwner) :
	CWsObject(aOwner, WS_HANDLE_CLIENT)
	{
	}
	
void CWsCliObj::ConstructL()
	{
	NewObjL();	
	}

void CWsCliObj::CommandL(TInt aOpcode, const TAny* aCmdData)	// (step #5)
	{
	iWsOwner->ExecuteCommandL(aOpcode,aCmdData);	// (call #6)
	}

CWsObject* CWsClient::HandleToObjUntyped(TInt aHandle)
	{
	return iObjectIndex->HandleToObject(aHandle);
	}

const CWsObject* CWsClient::HandleToObjUntyped(TInt aHandle) const
	{
	return const_cast<CWsClient*>(this)->HandleToObjUntyped(aHandle);
	}

CWsObject* CWsClient::HandleToObj(TInt aHandle, WH_HANDLES aType)
	{
	CWsObject* object = HandleToObjUntyped(aHandle);
	return (object && object->Type() == aType) ? object : NULL;
	}


const CWsObject* CWsClient::HandleToObj(TInt aHandle, WH_HANDLES aType) const
	{
	return const_cast<CWsClient*>(this)->HandleToObj(aHandle, aType);
	}

void CWsClient::SetRetryFlag(TEventCode aEventCode)
	{
	switch(aEventCode)
		{
		//To be expanded
		case EEventDisplayChanged:
			{
			iInternalFlags |= ERetryDisplayEvent;
			}
		break;
		
		}
	}
TBool CWsClient::RetryEvent(TEventCode aEventCode)
	{
	switch(aEventCode)
		{//To be expanded
		case EEventDisplayChanged:
			{
			return (iInternalFlags & ERetryDisplayEvent);
			}
		}
	return EFalse;
	}

void CWsClient::RemoveRetryFlag(TEventCode aEventCode)
	{
	switch(aEventCode)
		{//To be expanded
		case EEventDisplayChanged:
			{
			iInternalFlags &= ~ERetryDisplayEvent;
			}
		break;
		}
	}

