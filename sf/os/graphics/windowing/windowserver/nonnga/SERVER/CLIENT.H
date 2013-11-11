// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <graphics/wsgraphicdrawerinterface.h>
#include "WSGRAPHICDRAWERARRAY.H"

#include <e32std.h>
#include <e32base.h>
#include <w32std.h>
#include "w32cmd.h"
#include "WSOBJIX.H"
#include "EVQUEUE.H"
#include "PRIKEY.H"
#include "wstypes.h"

class CWsWindowBase;
class CWsClientWindow;
class CWsPointerCursor;
class CWsCustomTextCursor;
class CWsSpriteBase;
class DWsScreenDevice;
class CScreen;
class RHandleBase;
class CWindowServer;
class CWsGraphicDrawerObject;
class CWsGraphicDrawer;

class TWsCursorArrayItem
	{
public:
	CWsSpriteBase *iCursor;
	TInt iIndex;
	};

/** Server-side object for a client session.

@internalComponent
@released
*/
class CWsClient : public CSession2, public MWsClient
	{
private:
	enum{EObjectGranularity=5};
	enum{ENoDefaultSystemPointerCursor=-1};
public:
	enum{EPanicLeave=1};
public:
	CWsClient(RThread aClient);
	~CWsClient();
	inline const RThread& Client() const;
	inline CWindowServer& WindowServer();
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	void StartInitializationL(TUint aConnectionHandle);
	static const TUint8 *EndOfCommandBuffer();
	static const TPtrC BufferTPtr(TText *aStart,TInt aLen);
	static TBool BufferTPtrGc(TText* aStart,TInt aLen,TPtrC& aPtr);
	static const TPtrC8 BufferTPtr8(TUint8* aStart,TInt aLen);
	void CommandBufL();
	static void ReplyBuf(const TAny *aSource, TInt aLength);
	static void ReplyBuf(const TDesC8 &aDes);
	static void ReplyBuf(const TDesC16 &aDes);
	static void ReplySize(const TSize &aSize);
	static void ReplyPoint(const TPoint &aPoint);
	static void ReplyRect(const TRect &aRect);
	static void SetReply(TInt reply);
	static void PanicCurrentClient(TClientPanic aPanic);
	void PPanic(TClientPanic aPanic) const;
	void HandleToWindow(TInt handle,CWsWindowBase **pWin);
	void HandleToClientWindow(TInt handle,CWsClientWindow **pWin);
	inline CEventQueue *EventQueue() const;
	inline CRedrawQueue *RedrawQueue() const;
	inline CWsObjectIx *ObjectIndex();
	CWsObject *HandleToObjUntyped(TInt aHandle);
	CWsObject *HandleToObj(TInt aHandle, WH_HANDLES aType);
	void RequestComplete(TRequestStatus * &aStatus, TInt aErr);
//
	inline void EventReady(const RMessagePtr2& aEventMsg);
	inline void GetEventData();
	inline void CancelEvent();
	inline void PurgePointerEvents();

	inline void RedrawEventReady(const RMessagePtr2& aEventMsg);
	inline void CancelRedrawEvent();
	inline void GetRedrawData();

	inline void PriorityKeyEventReady(const RMessagePtr2& aEventMsg);
	inline void CancelPriorityKeyEvent();
	inline void GetPriorityKeyData();
	inline void PriorityKeyPressed(TInt aHandle, const TKeyData &aKey, TInt aScanCode);

	void TriggerRedraw();
	void SessionPanic(TClientPanic aReason) const;
	void SessionTerminate();
	void UpdateWindowOrdinalPrioritys();
	void CreateNewWindowGroupL(const TWsClCmdCreateWindowGroup &aCmd);
	void CreateNewWindowL(const TWsClCmdCreateWindow &aCmd);
	void CreateNewAnimDllL(const TWsClCmdUnion &aParams);
//
	void CreateNewSpriteL(const TWsClCmdCreateSprite &aCmd);
	void CreateNewPointerCursorL(const TWsClCmdCreatePointerCursor &aCmd);
	void StartSetCustomTextCursorL(const TWsClCmdCustomTextCursorData& aCmd);
	void CompleteSetCustomTextCursorL(TInt aErr);
	static CWsCustomTextCursor* FindCustomTextCursor(TInt aIdentifier);

	void CreateNewScreenDeviceL( TInt aDefaultScreenNumber, TUint aClientScreenDevicePointer);
	void CreateNewBitmapL(const TWsClCmdCreateBitmap &aCmd);
	void CreateNewClickL(const TUid& aUid);
//
	void ReplyGroupName(HBufC *aName, TInt aMaxLength) const;
	inline TInt ObjectHandle(const CWsObject *aThis);
	void SetSystemPointerCursorL(TInt aIndex, CWsPointerCursor *aCursor);
	void ClearSystemPointerCursor(TInt aIndex);
	void ClaimSystemPointerCursorListL();
	void FreeSystemPointerCursorList();
	static CWsPointerCursor *SystemPointerCursor(TInt aIndex);
	static inline CWsPointerCursor *DefaultSystemPointerCursor();
	void SetDefaultSystemPointerCursor(TInt aIndex);
	void SetComputeMode(RWsSession::TComputeMode aComputeMode);
	void SetClientPriority();
	inline TInt ConnectionHandle() const;
	inline DWsScreenDevice *PrimaryScreenDevice() const;
 	inline void NotifyScreenDeviceDeleted(DWsScreenDevice* aDeletedScreenDevice);
	inline CScreen* Screen() const;
	inline TBool NotClosing() const;
	inline const RMessage2& ClientMessage() const;
	void RemoteRead(TDes16& aDes, TInt aOffset);
	void RemoteRead(TDes8& aDes, TInt aOffset);
	void RemoteReadL(TDes16& aDes, TInt aOffset);
	void RemoteReadL(TDes8& aDes, TInt aOffset);
	static void DeleteStatics();
	void SetResponseHandle(RHandleBase* aHandle);

	// from MWsClient
	TBool HasCapability(TCapability aCapability) const;
	TSecureId SecureId() const;
	TVendorId VendorId() const;
	TInt SendMessage(const CWsGraphicDrawer* aOnBehalfOf,const TDesC8& aData);
	TInt SendMessage(const CWsGraphicDrawer* aOnBehalfOf,CWsMessageData& aData);

	CWsGraphicDrawerObject* DrawerObject(const CWsGraphicDrawer* aDrawer);
	const CWsGraphicDrawerObject* DrawerObject(const CWsGraphicDrawer* aDrawer) const;
	static inline CWsClient* CurrentClient();
	static inline TBool CheckBuffer(TInt aLength, TInt aMaxLength);
 	static TBool DebugEnforceRedrawCallingConvention();
private: // from CSession2
	void ServiceL(const RMessage2 &aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
private:
	void CompleteInitializationL();
	static void DeleteSystemPointerListEntry(TInt aIndex);
	static TBool FindCursorArrayItem (CArrayFixFlat<TWsCursorArrayItem>* aCursorArray,
									  TInt aIndex, TInt& aPosition);
	static inline CWsPointerCursor*& PointerCursor (TInt aPosition);
	static inline CWsCustomTextCursor*& TextCursor (TInt aPosition);
	void CommandL(TInt aOpcode, const RMessage2& aMessage);
	void DoServiceL(const RMessage2& aMessage, TBool& aCompleteRequest);
	void CompleteMessage(const RMessage2& aMessage,TInt aReason);
	void InitialiseScreenDevices();
	inline TBool IsInitialised();
	void DebugInfoL(TInt aFunction, TInt aParam, TBool aHasReplyBuf) const;
public:
	static TWsCmdHeaderBase iCurrentCommand;
private:
	enum TInternalFlags
		{
		EPanicClientAsSoonAsPossible	=0x01,		//The client should be panicked, but we have no way to panic him now
		EClientIsClosing		=0x02,		//The client is closing down, so the screen device may no longer be valid
		};
private:	// Private data
	RThread iClient;
	TUint iConnectionHandle; // Connection ID, only needed for logging, could bin this
	CEventQueue *iEventQueue;
	CRedrawQueue *iRedrawQueue;
	CPriorityKey *iPriorityKeyEvent;
	CWsGraphicMessageQueue iGraphicMessageQueue;
	RWsSession::TComputeMode iComputeMode;
	CWsObjectIx *iObjectIndex;
	DWsScreenDevice *iPrimaryScreenDevice;
	TWsCursorArrayItem iTempCustomTextCursor;
	CScreen* iScreen;		//## This needs updating
	mutable TInt iPanicReason;
	mutable TInt iInternalFlags;
	RMessage2 iClientMessage;
	RHandleBase* iResponseHandle;
	TInt iMessageIdSeq;
	TBool iIsInitialised;
#if defined(_DEBUG)
	TBool iLastCommand;
#endif
	static CWsClient *iCurrentClient;	// Client who's buffer is currently being processed
	static TInt iReply;					// Value to reply
	static TInt iReplyOffset;			// Offset into reply to write next block of data
	static TInt iDefaultSystemPointerCursorIndex;		//Negative when there isn't one
	static CWsPointerCursor *iDefaultSystemPointerCursor;
	static CArrayFixFlat<TWsCursorArrayItem> *iSystemPointerCursors;
	static CWsClient *iSystemPointerCursorListOwner;
	static CArrayFixFlat<TWsCursorArrayItem> *iTextCursorArray;
	static TBuf8<EClientBufferMaxSize> iCmdBuf;
	static TUint iConnectionId;
 	static TBool iDebug_EnforceRedrawCallingConvention;
#if defined(__WINS__)
	TBool iRemoveKeyCode;
public:
	inline TBool RemoveKeyCode();
#endif
	};


//
// inlines			//
//

//
inline const RThread& CWsClient::Client() const
	{return iClient;}
inline CWindowServer& CWsClient::WindowServer()
	{return *reinterpret_cast<CWindowServer*>(const_cast<CServer2*>(Server()));}
inline void CWsClient::EventReady(const RMessagePtr2& aEventMsg)
	{iEventQueue->EventReady(aEventMsg);}
inline void CWsClient::GetEventData()
    {iEventQueue->GetData();}
inline void CWsClient::PurgePointerEvents()
	{iEventQueue->PurgePointerEvents();}
inline void CWsClient::CancelEvent()
	{iEventQueue->CancelRead();}
inline void CWsClient::RedrawEventReady(const RMessagePtr2& aEventMsg)
    {iRedrawQueue->EventReady(aEventMsg);}
inline void CWsClient::GetRedrawData()
    {iRedrawQueue->GetData();}
inline void CWsClient::CancelRedrawEvent()
	{iRedrawQueue->CancelRead();}
inline void CWsClient::PriorityKeyEventReady(const RMessagePtr2& aEventMsg)
    {iPriorityKeyEvent->EventReady(aEventMsg);}
inline void CWsClient::CancelPriorityKeyEvent()
	{iPriorityKeyEvent->CancelRead();}
inline void CWsClient::GetPriorityKeyData()
	{iPriorityKeyEvent->GetData();}
inline void CWsClient::PriorityKeyPressed(TInt aHandle, const TKeyData &aKey, TInt aScanCode)
	{iPriorityKeyEvent->PriorityKey(aHandle,aKey,aScanCode);}
inline TInt CWsClient::ObjectHandle(const CWsObject *aThis)
	{return(iObjectIndex->At(aThis));}
#if defined(__WINS__)
inline TBool CWsClient::RemoveKeyCode()
	{return iRemoveKeyCode;}
#endif
//
inline CEventQueue *CWsClient::EventQueue() const
	{return(iEventQueue);}
inline CRedrawQueue *CWsClient::RedrawQueue() const
	{return(iRedrawQueue);}
inline CWsObjectIx *CWsClient::ObjectIndex()
	{return(iObjectIndex);}
inline TInt CWsClient::ConnectionHandle() const
	{return(iConnectionHandle);}
inline DWsScreenDevice *CWsClient::PrimaryScreenDevice() const
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
inline CScreen* CWsClient::Screen() const
	{return iScreen;}
inline TBool CWsClient::NotClosing() const
	{return !(iInternalFlags&EClientIsClosing);}
inline CWsClient* CWsClient::CurrentClient()
	{return iCurrentClient;}
inline TBool CWsClient::CheckBuffer(TInt aLength, TInt aMaxLength)
	{return TBool((aLength>=0) && (aLength<=aMaxLength));}
inline TBool CWsClient::IsInitialised()
	{return iIsInitialised;}
#endif
