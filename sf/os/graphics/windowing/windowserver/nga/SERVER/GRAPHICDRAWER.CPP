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
//

#include "server.h"
#include "WSGRAPHICDRAWERFACTORY.H"
#include "panics.h"
#include <s32mem.h>
#include "wstop.h"

TInt CWsGraphicDrawerObject::TransientSequence=1;
TBool CWsGraphicDrawerObject::RollOver=EFalse;

CWsGraphicDrawerObject* CWsGraphicDrawerObject::NewL(CWsClient* aClient,const TWsClCmdUnion &aParams)
	{
	if(!aClient)
		{
		WS_PANIC_DEBUG(EWsPanicWsGraphic);
		User::Leave(KErrGeneral);
		}
	TGraphicDrawerId id;
	id.iId = aParams.CreateGraphic->iId;
	id.iIsUid = !(EWsGraphicIdTransient & aParams.CreateGraphic->iFlags);
	// check for collision
	const CWsGraphicDrawer* dup = CWsTop::WindowServer()->ResolveGraphic(id);
	// request-specific checks
	if(dup)
		{
		// must be exact same session
		if(&(dup->Owner()) != aClient)
			{
			/*	Someone is squatting on someone else's uid, or trying to,
				but there is no good way to know which. */
			__DEBUG_ONLY(aClient->PPanic(EWservPanicPermissionDenied);)
			User::Leave(KErrAlreadyExists);
			}
		// dupping or otherwise must be explicitly intended by the client
		if(!(EWsGraphicReplace & aParams.CreateGraphic->iFlags))
			{
			aClient->PPanic(EWservPanicPermissionDenied);
			}
		}
	else
		{
		// dupping or otherwise must be explicitly intended by the client
		if(EWsGraphicReplace & aParams.CreateGraphic->iFlags)
			{
			aClient->PPanic(EWservPanicPermissionDenied);
			}
		if(id.iIsUid)
			{
			// police creation of artwork with UIDs
			_LIT_SECURITY_POLICY_C1(KSecurityPolicyCreateWithUid,ECapabilityProtServ);
			if(!KSecurityPolicyCreateWithUid.CheckPolicy(aClient->Client()))
				{
				__DEBUG_ONLY(aClient->PPanic(EWservPanicPermissionDenied);)
				User::Leave(KErrPermissionDenied);
				}
			}
		else
			{
			// allocate a new transient id
			// assumption: that there are more values of a TInt than there are possibly Transient objects
			do
				{
				id.iId = TransientSequence++;
				RollOver |= !TransientSequence;
				}
			while(RollOver && CWsTop::WindowServer()->ResolveGraphic(id)); // until no collision
			}
		}
	// create the drawer object
	TPtrC8 data;
	HBufC8* dataBuf = NULL;
	if(aParams.CreateGraphic->iRemoteReadData)
		{
		const TInt len = aParams.CreateGraphic->iDataLen;
		if ((len >= KMaxTInt/4) || (len < 0))
			aClient->PPanic(EWservPanicBuffer);
		dataBuf = HBufC8::NewLC(len);
		TPtr8 des = dataBuf->Des();
		aClient->RemoteRead(des,0);
		if(des.Size() != len)
			{
			aClient->PPanic(EWservPanicBuffer);
			}
		data.Set(des);
		}
	else
		{
		data.Set(CWsClient::BufferTPtr8((TText8*)(aParams.CreateGraphic+1),aParams.CreateGraphic->iDataLen));
		}

	CWsGraphicDrawerObject* drawer = new(ELeave) CWsGraphicDrawerObject(aClient);
	CleanupStack::PushL(drawer);
	drawer->ConstructL(aParams.CreateGraphic->iType,*CWsTop::WindowServer(),id,data,aParams.CreateGraphic->iClientHandle);
	if(!dup)
		{
		User::LeaveIfError(CWsTop::WindowServer()->AddGraphicDrawer(drawer->Drawer()));
		}
	else
		{
		User::LeaveIfError(CWsTop::WindowServer()->SwapGraphicDrawer(drawer->Drawer()));
		}
	// take off cleanup stack
	CleanupStack::Pop(drawer);
	if(dataBuf)
		{
		CleanupStack::PopAndDestroy(dataBuf);
		}
	// delete dup, which means resolving the object that encapsulates it
	if(dup)
		{
		CWsGraphicDrawerObject* obj = aClient->DrawerObject(dup);
		WS_ASSERT_DEBUG(obj, EWsPanicWsGraphic);
		if(obj)
			{
			obj->CloseObject();
			}
		}
	// trigger redraws if anyone is drawing this graphic before it exists
	if(id.iIsUid)
		{
		CWsTop::WindowServer()->Invalidate(id);
		}
	// done
	return drawer;
	}

CWsGraphicDrawerObject::CWsGraphicDrawerObject(CWsClient* aOwner):
	CWsObject(aOwner,WS_HANDLE_GRAPHIC_DRAWER)
	{
	}

CWsGraphicDrawerObject::~CWsGraphicDrawerObject()
	{
	delete iDrawer;
	}

void CWsGraphicDrawerObject::ConstructL(TUid aType,MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,const TDesC8& aData,TInt aClientHandle)
	{
	CWsGraphicDrawer* drawer = WsGraphicDrawer::CreateLC(aType,aEnv,aId,*WsOwner(),aData);
	NewObjL();
	iClientHandle = aClientHandle;
	iDrawer=drawer;
	CleanupStack::Pop(iDrawer);
	}

void CWsGraphicDrawerObject::CommandL(TInt aOpcode, const TAny *aCmdData)
	{
	WS_ASSERT_DEBUG(iDrawer, EWsPanicWsGraphic);
	
	if(iDrawer)
		{
		TWsClCmdUnion pData;
		pData.any = aCmdData;
		switch(aOpcode)
			{
			case EWsGdOpFree:
				// trigger redraws if anyone is drawing this graphic before it exists
				CWsTop::WindowServer()->RemoveGraphicDrawer(iDrawer->Id());
				CloseObject();
				return;
			case EWsGdOpSendMsg:
			case EWsGdOpSendSynchronMsg:
				{
				const TWsClCmdGdSendMessage& cmd = *pData.GraphicSendMessage;
				__ASSERT_DEBUG(cmd.iDataLen,WsOwner()->PPanic(EWservPanicPermissionDenied));
				if(cmd.iDataLen)
					{
					if(cmd.iRemoteReadData)
						{
						HBufC8* wsAlloc = NULL;
						TUint8* drawerAlloc = NULL;
						TPtr8 des(NULL,0);
						// try to get the drawer to allocate for us
						MWsGraphicDrawerMessageAlloc* drawerAllocator = iDrawer->ObjectInterface<MWsGraphicDrawerMessageAlloc>();
						if(drawerAllocator)
							{
							drawerAlloc = reinterpret_cast<TUint8*>(drawerAllocator->Alloc(cmd.iDataLen));
							if(drawerAlloc)
								des.Set(drawerAlloc,0,cmd.iDataLen);
							}
						// else use the normal WSERV default heap
						if(!drawerAlloc)
							{
							wsAlloc = HBufC8::NewLC(cmd.iDataLen);
							des.Set(wsAlloc->Des());
							}
						// fetch
   						WsOwner()->RemoteRead(des,0);
						TBool receivedOk = (des.Size() == cmd.iDataLen);
						// dispatch
						if(receivedOk)
							{
							if(aOpcode == EWsGdOpSendMsg)
								{
								iDrawer->HandleMessage(des);
								}
							else
								{
								MWsGraphicHandleSynchronMessage* obj = iDrawer->ObjectInterface<MWsGraphicHandleSynchronMessage>();

								if(obj)
									SetReply(obj->HandleSynchronMessage(des));
								else
									SetReply(KErrNotSupported);
								}
							}

						// dealloc
						if(drawerAlloc)
							{
							drawerAllocator->Free(drawerAlloc);
							}
						else
							{
							CleanupStack::PopAndDestroy(wsAlloc);
							}
						// defer panic until after dealloc
						if(!receivedOk)
							{
							WsOwner()->PPanic(EWservPanicBuffer);
							}
						}
					else
						{
						const TPtrC8 data = CWsClient::BufferTPtr8((TText8*)(pData.GraphicSendMessage+1),cmd.iDataLen);
						if(aOpcode == EWsGdOpSendMsg)
							{
							iDrawer->HandleMessage(data);
							}
						else
							{
							MWsGraphicHandleSynchronMessage* obj = iDrawer->ObjectInterface<MWsGraphicHandleSynchronMessage>();
							if(obj)
								SetReply(obj->HandleSynchronMessage(data));
							else
								SetReply(KErrNotSupported);
							}
						}
					}
				}
				break;
			case EWsGdOpGetGraphicId:
				{
				__ASSERT_COMPILE(sizeof(TWsClCmdGdGetId) == sizeof(TGraphicDrawerId));
				const TGraphicDrawerId id = iDrawer->Id();
				CWsClient::ReplyBuf(&id,sizeof(id));
				}
				break;
			case EWsGdOpShareGlobally:
				SetReply(iDrawer->ShareGlobally());
				break;
			case EWsGdOpUnShareGlobally:
				SetReply(iDrawer->UnShareGlobally());
				break;
			case EWsGdOpShare:
				{
				const TSecureId& secid = *reinterpret_cast<const TSecureId*>(aCmdData);
				SetReply(iDrawer->Share(secid));
				break;
				}
			case EWsGdOpUnShare:
				{
				const TSecureId& secid = *reinterpret_cast<const TSecureId*>(aCmdData);
				SetReply(iDrawer->UnShare(secid));
				break;
				}
			default:
				WsOwner()->PPanic(EWservPanicOpcode);
			}
		}
	}

CWsGraphicDrawer* CWsGraphicDrawerObject::Drawer()
	{
	return iDrawer;
	}

const CWsGraphicDrawer* CWsGraphicDrawerObject::Drawer() const
	{
	return iDrawer;
	}

// CWsGraphicMessageQueue \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

CWsGraphicMessageQueue::CMessage* CWsGraphicMessageQueue::CMessage::New(const TDesC8& aData)
   	{
	return new(aData.Size()) CMessage(aData);
   	}

CWsGraphicMessageQueue::CMessage::CMessage(const TDesC8& aData):
	iData((reinterpret_cast<TUint8*>(this)+sizeof(*this)),aData.Size())
   	{
	iData = aData;
   	}

TPtrC8 CWsGraphicMessageQueue::CMessage::Data() const
	{
	return iData;
   	}

void CWsGraphicMessageQueue::CMessage::Release()
	{
	delete this;
   	}

CWsGraphicMessageQueue::CMessage::~CMessage()
   	{
   	}

CWsGraphicMessageQueue::CWsGraphicMessageQueue(CWsClient *aOwner):
	CEventBase(aOwner)
	{
	}

CWsGraphicMessageQueue::~CWsGraphicMessageQueue()
	{
	while(iHead)
		{
		CWsMessageData* msg = iHead;
		iHead = iHead->iNext;
		msg->Release();
		}
	}

TInt CWsGraphicMessageQueue::TopClientHandle() const
	{
	if(iHead)
		return iHead->iClientHandle;
	else
		return KErrNotFound;
	}

/** 
same functionality as CEventBase::GetData, but this one also inserts the integer header in the reply.
*/
void CWsGraphicMessageQueue::GetDataWithHeader(TUint aHeader, const TDesC8& aData, TInt aDataLen)
	{
	TPckgBuf<TUint> hdr = aHeader | (EWsGraphMessageTypeUser & 0x03);
	CWsClient::ReplyBuf(hdr.Ptr(), sizeof(TUint));
	GetData((void*)aData.Ptr(), aDataLen);
	}

void CWsGraphicMessageQueue::GetGraphicMessage()
	{
	CWsMessageData* top = Pop();
	WS_ASSERT_DEBUG(top && top->Data().Length(), EWsPanicWsGraphic);
	if(top)
		{
		GetDataWithHeader(top->iClientHandle, top->Data(), top->Data().Size());
		CWsGraphicDrawerObject* obj = iWsOwner->DrawerObject(top->iDrawer);
		if(obj)
			{
			MWsGraphicMessageCallback* messageCallback = obj->Drawer()->ObjectInterface<MWsGraphicMessageCallback>();
			if(messageCallback)
				{
				messageCallback->HandleMessageDelivery(top->iId, KErrNone);
				}
			}

		top->Release();
		}
	}

void CWsGraphicMessageQueue::AbortMessage(TInt aError)
	{
	CWsMessageData* top = Pop();
	WS_ASSERT_DEBUG(top, EWsPanicWsGraphic);
	if(top)
		{
		CWsGraphicDrawerObject* obj = iWsOwner->DrawerObject(top->iDrawer);
		if(obj)
			{
			MWsGraphicMessageCallback* messageCallback = obj->Drawer()->ObjectInterface<MWsGraphicMessageCallback>();
			if(messageCallback)
				{
				messageCallback->HandleMessageDelivery(top->iId, aError);
				}
			}
		top->Release();
		}
	}

void CWsGraphicMessageQueue::EventReady(const RMessagePtr2& aEventMsg)
	{
	CEventBase::EventReady(aEventMsg);
	if(iHead)
		{
		SignalEvent(sizeof(TInt) + iHead->Data().Size());
		}
	}


void CWsGraphicMessageQueue::Queue(CWsMessageData* aMessage)
	{
	WS_ASSERT_DEBUG(aMessage && !aMessage->iNext, EWsPanicWsGraphic);
	if(aMessage)
		{
		if(iHead)
			{
			iTail->iNext = aMessage;
			iTail = aMessage;
			}
		else
			{
			iHead = iTail = aMessage;
			if(!iEventMsg.IsNull())
				SignalEvent(sizeof(TInt) + iHead->Data().Size());
			}
		}
	}

CWsMessageData* CWsGraphicMessageQueue::Pop()
	{
	CWsMessageData* ret = NULL;
	if(iHead)
		{
		ret = iHead;
		iHead = iHead->iNext;
		}
	return ret;
	}
