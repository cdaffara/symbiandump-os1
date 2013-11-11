// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of classes for the class that deals with client communication
// 
//

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "WSGRAPHICDRAWERARRAY.H"

#include <e32std.h>
#include <e32base.h>
#include "W32STD.H"
#include "w32cmd.h"
#include "WSOBJIX.H"
#include "EVQUEUE.H"
#include "PRIKEY.H"
#include "wstypes.h"
#include <e32hashtab.h>  //RHashMap

class CWsWindowBase;
class CWsClientWindow;
class CWsPointerCursor;
class CWsCustomTextCursor;
class CWsSpriteBase;
class DWsScreenDevice;
class CScreen;
class RHandleBase;
class CWsGraphicDrawerObject;
class CWsGraphicDrawer;
class MWsElement;
class TSurfaceConfiguration;
class CWsRedrawMsgWindow;

/** Server-side object for a client session.

@internalComponent
@released
*/
class CWsClient : public CSession2, public MWsClient
	{
private:
	friend class CWsRedrawMsgWindow;
private:
	class TWsCursorArrayItem;
	enum{EObjectGranularity=5};
	enum{ENoDefaultSystemPointerCursor=-1};
public:
	enum{EPanicLeave=1};
public: // from MWsClient
	TBool HasCapability(TCapability aCapability) const;
	TSecureId SecureId() const;
	TVendorId VendorId() const;
public:
	CWsClient(RThread aClient);
	~CWsClient();
	static void InitStaticsL();
	static void DeleteStatics();
	inline TBool NotClosing() const;
	inline CWsObjectIx* ObjectIndex();

	inline const RMessage2& ClientMessage() const;
	
	inline const RThread& Client() const;
	static inline CWsClient* CurrentClient();
	inline TInt ConnectionHandle() const;
	inline DWsScreenDevice* PrimaryScreenDevice();
	inline const DWsScreenDevice* PrimaryScreenDevice() const;
	inline CScreen* Screen();
	inline const CScreen* Screen() const;
	static CWsPointerCursor* SystemPointerCursor(TInt aIndex);
	static inline CWsPointerCursor* DefaultSystemPointerCursor();
	static CWsCustomTextCursor* FindCustomTextCursor(TInt aIdentifier);
	void SessionPanic(TClientPanic aReason) const;
	
	// Window object access from handle
	void HandleToWindow(TInt aHandle, CWsWindowBase** pWin);
	void HandleToClientWindow(TInt aHandle, CWsClientWindow** pWin);
	
	// Generic WsObject access from handle
	CWsObject* HandleToObj(TInt aHandle, WH_HANDLES aType);
	const CWsObject* HandleToObj(TInt aHandle, WH_HANDLES aType) const;
	CWsObject* HandleToObjUntyped(TInt aHandle);
	const CWsObject* HandleToObjUntyped(TInt aHandle) const;
	
	CWsGraphicDrawerObject* DrawerObject(const CWsGraphicDrawer* aDrawer);
	const CWsGraphicDrawerObject* DrawerObject(const CWsGraphicDrawer* aDrawer) const;
	
	// Handle from object
	inline TInt ObjectHandle(const CWsObject* aThis) const;
	
	// Queue access
	inline CEventQueue* EventQueue();
	inline const CEventQueue* EventQueue() const;
	inline CRedrawQueue* RedrawQueue();
	inline const CRedrawQueue* RedrawQueue() const;	
	
	// Execute client command from buffer
	void ExecuteCommandL(TInt aOpcode, const TAny* aCmdData);
	static const TUint8* EndOfCommandBuffer();
	static const TPtrC BufferTPtr(TText* aStart,TInt aLen);
	static TBool BufferTPtrGc(TText* aStart,TInt aLen,TPtrC& aPtr);
	static const TPtrC8 BufferTPtr8(TUint8* aStart,TInt aLen);
	
	// Remote read data from client
	void RemoteRead(TDes16& aDes, TInt aOffset);
	void RemoteRead(TDes8& aDes, TInt aOffset);
	void RemoteReadL(TDes16& aDes, TInt aOffset);
	void RemoteReadL(TDes8& aDes, TInt aOffset);

	// Send the reply to the client's request
	void SetResponseHandle(RHandleBase* aHandle);
	static void ReplyBuf(const TAny* aSource, TInt aLength);
	static void ReplyBuf(const TDesC8 &aDes);
	static void ReplyBuf(const TDesC16 &aDes);
	static void ReplySize(const TSize &aSize);
	static void ReplyPoint(const TPoint &aPoint);
	static void ReplyRect(const TRect &aRect);
	static void ReplyGroupName(HBufC* aName, TInt aMaxLength);
	static void SetReply(TInt reply);
	void RequestComplete(TRequestStatus*& aStatus, TInt aErr);
	
	// Panic or terminate the client
	static void PanicCurrentClient(TClientPanic aPanic);
	static TInt ReplyBufSpace()  ;
	void PPanic(TClientPanic aPanic) const;
	void SessionTerminate();
	
	// All events
	inline void EventReady(const RMessagePtr2& aEventMsg);
	
	// Pointer events
	inline void PurgePointerEvents();

	// Key events
	inline void PriorityKeyPressed(TInt aHandle, const TKeyEvent &aKeyEvent);

	// Notification of misc events
	void AddNotificationL(TInt aKey, const RMessage2& aClientMsg);
	void CompleteNotification(TInt aKey, TInt aReason);
	inline void NotifyScreenDeviceDeleted(DWsScreenDevice* aDeletedScreenDevice);
	
	// Redraw
	void TriggerRedraw();

	// Misc
	void UpdateWindowOrdinalPrioritys();
	void SetClientPriority();	// boost when in foreground

	void CreateDrawableSourceL(const TWsClCmdCreateDrawableSource& aDrawableSourceData);

	//retry putting events on event queue
	void SetRetryFlag(TEventCode aEventCode);
	void RemoveRetryFlag(TEventCode aEventCode);
	TBool RetryEvent(TEventCode aEventCode);

    inline void WgMsgQueueOverflow();// Set flag  window group message queue is overflow and has pending messages
    
    //Get orientation value that app indicated
    TInt GetIndicatedAppOrientation();

private: // from MWsClient
	TInt SendMessage(const CWsGraphicDrawer* aOnBehalfOf, const TDesC8& aData);
	TInt SendMessage(const CWsGraphicDrawer* aOnBehalfOf, CWsMessageData& aData);

private: // from CSession2
	void ServiceL(const RMessage2 &aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
private:
	// Construction and destruction
	inline TBool IsInitialised();
	void StartInitializationL(TUint aConnectionHandle);
	void CompleteInitializationL();
	void InitialiseScreenDevices();
	
	// Service client commands by dispatching them to the object associated with the op code
	void DoServiceL(const RMessage2& aMessage, TBool& aCompleteRequest);
	void DoServiceCommandBuf();
	void DispatchCommandsInBufL();
	void CompleteMessage(const RMessage2& aMessage,TInt aReason);
	
	// Execute async client commands
	void ExecuteAsyncClientCommandL(TInt aOpcode, const RMessage2& aMessage);
	static inline TBool CheckBuffer(TInt aLength, TInt aMaxLength);
	static TBool DebugEnforceRedrawCallingConvention();
	
	// Create new objects
	void CreateNewWindowGroupL(const TWsClCmdCreateWindowGroup &aCmd);
	void CreateNewWindowL(const TWsClCmdCreateWindow &aCmd);
	void CreateNewAnimDllL(const TWsClCmdUnion &aParams);
	void CreateNewSpriteL(const TWsClCmdCreateSprite &aCmd);
	void CreateNewPointerCursorL(const TWsClCmdCreatePointerCursor &aCmd);
	void CreateNewBitmapL(const TWsClCmdCreateBitmap &aCmd);
	void CreateNewClickHandlerL(const TUid& aUid);
	void CreateNewScreenDeviceL( TInt aDefaultScreenNumber, TUint aClientScreenDevicePointer);
	
	// Surface management
	TInt RegisterSurface(const TWsClCmdUnion& pData);
	void UnregisterSurface(const TWsClCmdUnion& pData);
	
	// Text cursor
	static inline CWsCustomTextCursor*& TextCursor (TInt aPosition);
	void StartSetCustomTextCursorL(const TWsClCmdCustomTextCursorData& aCmd);
	void CompleteSetCustomTextCursorL(TInt aErr);
	
	// Pointer cursor
	static inline CWsPointerCursor*& PointerCursor (TInt aPosition);
	void SetSystemPointerCursorL(TInt aIndex, CWsPointerCursor* aCursor);
	void ClearSystemPointerCursor(TInt aIndex);
	void ClaimSystemPointerCursorListL();
	void FreeSystemPointerCursorList();
	void SetDefaultSystemPointerCursor(TInt aIndex);
	static void DeleteSystemPointerListEntry(TInt aIndex);
	
	static TBool FindCursorArrayItem(CArrayFixFlat<TWsCursorArrayItem>* aCursorArray,
									  TInt aIndex, TInt& aPosition);
	
	// Key events
	inline void CancelClientRequestForPriorityKeyEvent();
	inline void HandleClientRequestForPriorityKeyData();
	inline void PriorityKeyEventReady(const RMessagePtr2& aEventMsg);
	
	// Redraw events
	inline void CancelClientRequestForRedrawEvent();
	inline void HandleClientRequestForRedrawData();
	inline void RedrawEventReady(const RMessagePtr2& aEventMsg);
	inline TBool ClientProcessingRedrawEvent();
	
	// Client event requests
	inline void HandleClientRequestForEventData();
	inline void CancelClientRequestForEventData();

	// Debugging
	void DebugInfoL(TInt aFunction, TInt aParam, TBool aHasReplyBuf) const;
	void DebugInfoUnclassifiedL(TInt aFunction, TInt aParam, TBool aHasReplyBuf) const;
	TInt DebugInfoClassifiedL(TInt aFunction, TInt aParam, TBool aHasReplyBuf) const;
	TInt DebugInfoScreenUiL(TInt aFunction, TInt aParam, TInt aReplyBufSize, CScreen& aScreen) const;
	TInt DebugInfoScreenElementSetL(TInt aFunction, TInt aParam, TInt aReplyBufSize,const CScreen& aScreen) const;
	TInt DebugInfoElementSetWindowL(TInt aFunction, TInt aParam, TInt aReplyBufSize,const CScreen& aScreen) const;
	TInt DebugInfoElementSetElementL(TInt aFunction, TInt aParam, TInt aReplyBufSize,const CScreen& aScreen) const;
	TInt DebugReturnConfig(TInt aReplyBufSize, MWsElement* aElement, TInt aFlags=0)const;
	TInt DebugReturnBase(TInt aReplyBufSize, const MWsElement* aElement) const;
	TInt DebugReturnFlags(TInt aReplyBufSize, const MWsElement* aElement, TInt aFlags=0) const;
	TInt DebugReturnRegion(TInt aReplyBufSize, const TRegion* aRegion, TInt aErrCodeIfEmpty) const;	
	
	// Misc
	void SetComputeMode(RWsSession::TComputeMode aComputeMode);
	
	//Set orientation that application indicated
	void IndicateAppOrientation(TRenderOrientation aOrientation);
	
public:
	static TWsCmdHeaderBase iCurrentCommand;
private:
	class TWsCursorArrayItem
		{
	public:
		CWsSpriteBase* iCursor;
		TInt iIndex;
		};

	enum TInternalFlags
		{
		EIsInitialised = 0x01,
		EPanicClientAsSoonAsPossible = 0x02,		//The client should be panicked, but we have no way to panic him now
		EClientIsClosing = 0x04,		//The client is closing down, so the screen device may no longer be valid
		EFinishedProcessingCommands=0x08,	//Reached last command in command buffer or otherwised finihsed processing it
		ERemoveKeyCode=0x10,		//Remove the Win32 keycode emulator only
		ERetryDisplayEvent=0x20,
		EIsPerformingRedrawEvent=0x40,         //The client is performing a RedrawEvent
		EWgMsgQueueOverflow =0x80,
		};
private:
	RThread iClient;
	CScreen* iScreen;		//## This needs updating
	TUint iConnectionHandle; // Connection ID, only needed for logging, could bin this
	CEventQueue* iEventQueue;
	CRedrawQueue* iRedrawQueue;
	CPriorityKey* iPriorityKeyEvent;
	CWsGraphicMessageQueue iGraphicMessageQueue;
	RWsSession::TComputeMode iComputeMode;
	CWsObjectIx* iObjectIndex;
	DWsScreenDevice* iPrimaryScreenDevice;
	TWsCursorArrayItem iTempCustomTextCursor;
	mutable TInt iPanicReason;
	mutable TUint iInternalFlags;
	RMessage2 iClientMessage;
	RHandleBase* iResponseHandle;
	TInt iMessageIdSeq;
	TInt iIndicatedAppOrientation;

	//Members for procerssing command buffer
	static CWsClient* iCurrentClient;	// Client who's buffer is currently being processed
	static TInt iReply;					// Value to reply
	static TInt iReplyOffset;			// Offset into reply to write next block of data
	static TBuf8<EClientBufferMaxSize> iCmdBuf; // Buffer contain a block of client commands
	static CWsObject* iDestObj;			// Current object client command is for
	static const TUint8* iNextCmd;		// Pointer in buffer to the next command to be processed

	static TInt iDefaultSystemPointerCursorIndex;		// Negative when there isn't one
	static CWsPointerCursor* iDefaultSystemPointerCursor;
	static CArrayFixFlat<TWsCursorArrayItem>* iSystemPointerCursors;
	static CWsClient* iSystemPointerCursorListOwner;
	static CArrayFixFlat<TWsCursorArrayItem>* iTextCursorArray;
	static TUint iConnectionId;
	static TKeyArrayFix iCursorKey;
 	static TBool iDebug_EnforceRedrawCallingConvention;
#if defined(__WINS__)
public:
	inline TBool RemoveKeyCode();
#endif
	};


//
// inlines			//
//

inline const RThread& CWsClient::Client() const
	{return iClient;}

inline void CWsClient::EventReady(const RMessagePtr2& aEventMsg)
	{iEventQueue->EventReady(aEventMsg);}

inline void CWsClient::HandleClientRequestForEventData()
    {iEventQueue->GetData();}

inline void CWsClient::CancelClientRequestForEventData()
	{iEventQueue->CancelRead();}

inline void CWsClient::PurgePointerEvents()
	{iEventQueue->PurgePointerEvents();}

inline void CWsClient::RedrawEventReady(const RMessagePtr2& aEventMsg)
    {
    iInternalFlags&=~EIsPerformingRedrawEvent;
    iRedrawQueue->EventReady(aEventMsg);
    }

inline TBool CWsClient::ClientProcessingRedrawEvent()
	{return !!(iInternalFlags&EIsPerformingRedrawEvent);}

inline void CWsClient::HandleClientRequestForRedrawData()
    {
    iInternalFlags|=EIsPerformingRedrawEvent;
    iRedrawQueue->GetData();
    }

inline void CWsClient::CancelClientRequestForRedrawEvent()
	{iRedrawQueue->CancelRead();}

inline void CWsClient::PriorityKeyEventReady(const RMessagePtr2& aEventMsg)
    {iPriorityKeyEvent->EventReady(aEventMsg);}

inline void CWsClient::CancelClientRequestForPriorityKeyEvent()
	{iPriorityKeyEvent->CancelRead();}

inline void CWsClient::HandleClientRequestForPriorityKeyData()
	{iPriorityKeyEvent->GetData();}

inline void CWsClient::PriorityKeyPressed(TInt aHandle, const TKeyEvent &aKeyEvent)
	{iPriorityKeyEvent->PriorityKey(aHandle, aKeyEvent);}

inline TInt CWsClient::ObjectHandle(const CWsObject* aThis) const
	{return(iObjectIndex->At(aThis));}

#if defined(__WINS__)
inline TBool CWsClient::RemoveKeyCode()
	{return iInternalFlags&ERemoveKeyCode;}
#endif

inline CEventQueue* CWsClient::EventQueue()
	{return(iEventQueue);}

inline const CEventQueue* CWsClient::EventQueue() const
	{return(iEventQueue);}

inline CRedrawQueue* CWsClient::RedrawQueue()
	{return(iRedrawQueue);}

inline const CRedrawQueue* CWsClient::RedrawQueue() const
	{return(iRedrawQueue);}

inline CWsObjectIx* CWsClient::ObjectIndex()
	{return(iObjectIndex);}

inline TInt CWsClient::ConnectionHandle() const
	{return(iConnectionHandle);}

inline DWsScreenDevice* CWsClient::PrimaryScreenDevice()
	{return(iPrimaryScreenDevice);}

inline const DWsScreenDevice* CWsClient::PrimaryScreenDevice() const
	{return(iPrimaryScreenDevice);}

inline void CWsClient::NotifyScreenDeviceDeleted(DWsScreenDevice* aDeletedScreenDevice)
	{
	if (iPrimaryScreenDevice == aDeletedScreenDevice)
		iPrimaryScreenDevice = NULL;
	}

inline CWsPointerCursor*& CWsClient::PointerCursor(TInt aIndex)
	{return (CWsPointerCursor*&)(*iSystemPointerCursors)[aIndex].iCursor;}

inline CWsCustomTextCursor*& CWsClient::TextCursor(TInt aIndex)
	{return (CWsCustomTextCursor*&)(*iTextCursorArray)[aIndex].iCursor;}

inline const RMessage2& CWsClient::ClientMessage() const
	{return iClientMessage;}

inline CScreen* CWsClient::Screen()
	{return iScreen;}

inline const CScreen* CWsClient::Screen() const
	{return iScreen;}

inline TBool CWsClient::NotClosing() const
	{return !(iInternalFlags&EClientIsClosing);}

inline CWsClient* CWsClient::CurrentClient()
	{return iCurrentClient;}

inline TBool CWsClient::CheckBuffer(TInt aLength, TInt aMaxLength)
	{return TBool((aLength>=0) && (aLength<=aMaxLength));}

inline TBool CWsClient::IsInitialised()
	{return iInternalFlags&EIsInitialised; }

inline void CWsClient::WgMsgQueueOverflow()
    {iInternalFlags |= EWgMsgQueueOverflow;}

#endif
