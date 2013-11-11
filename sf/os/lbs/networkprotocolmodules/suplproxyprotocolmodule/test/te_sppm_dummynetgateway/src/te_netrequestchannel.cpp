// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of network request channel component.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include <e32base.h>
#include <e32debug.h>
#include <lbs/lbslocerrors.h>
#include "lbsdevloggermacros.h"
#include <lbs/lbsadmin.h>
#include "lbsprocessuiddefs.h"
#include "te_netrequestchannel.h"


//
// CNetworkRequestChannel
//

const TLbsNetSessionIdInt KInvalidSessionId(TUid::Uid(0xABBADEAD), 999);
const TLbsNetPosRequestQualityInt KInvalidQuality;
const TLbsNetPosRequestMethodInt KInvalidMethod;
const TLbsNetPosRequestPrivacyInt KInvalidRequestPrivacy;
const TLbsExternalRequestInfo KInvalidExternalRequestInfo;

CNetworkRequestChannel::CNetworkRequestChannel(MNetworkRequestObserver& aObserver) : 
	CActive(EPriorityStandard),
	iObserver(aObserver)
	{
	}
	
CNetworkRequestChannel::~CNetworkRequestChannel()
	{
	Cancel();
	iNetRequestChannel.Close();
	delete iMsgBuffer;
	}

CNetworkRequestChannel* CNetworkRequestChannel::NewL(MNetworkRequestObserver& aObserver)
	{
	CNetworkRequestChannel* self = new (ELeave) CNetworkRequestChannel(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CNetworkRequestChannel::ConstructL()
	{
	// If the privacy protocol module is being used,
	// use the privacy message buffer.
	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);
	TUint maxNumRequests;
	User::LeaveIfError(admin->Get(KLbsSettingMaximumExternalLocateRequests, maxNumRequests));
	CleanupStack::PopAndDestroy(admin);
	
	if (maxNumRequests > 1)
		{
		iMsgBuffer = CNetworkRequestPrivacyBuffer::NewL();
		}
	else
		{
		iMsgBuffer = CNetworkRequestStandardBuffer::NewL();
		}
	
	iNetRequestChannel.OpenL(RLbsNetChannel::EChannelNG2NRH, *this);
	CActiveScheduler::Add(this);
	}
	
void CNetworkRequestChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	// If there is a pending message in the buffer, pass it on to the NRH
	SendNextBufferedMessage();
	}
	
void CNetworkRequestChannel::DoCancel()
	{
	iNetRequestChannel.CancelSendMessageNotification();
	}
	
TInt CNetworkRequestChannel::RunError(TInt aError)
	{
	return aError;
	}

void CNetworkRequestChannel::SendNetRequestMessage(const TLbsNetInternalMsgBase& aMessage)
	{
	LBSLOG(ELogP1, "CNetworkRequestChannel::SendNetRequestMessage:");
	
	if (!IsActive())
		{		
		// Immediately send the new message.
		SendMessageAndNotifyForResponse(aMessage);
		}
	else
		{
		// Still waiting for acknowledgement that a previous message
		// was read, so buffer this new message.
		TInt err = iMsgBuffer->BufferMessage(aMessage);
		if (err != KErrNone)
			{
			LBSLOG(ELogP1, "BUFFERING MESSAGE FAILED!!");
			}
		}
	}
	
void CNetworkRequestChannel::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	__ASSERT_DEBUG(aChannelId == RLbsNetChannel::EChannelNG2NRH, User::Panic(KLbsNGFault, ENGUnexpectedNetChannelId));
	(void) aChannelId;
	iObserver.ProcessNetRequestMessage(aMessage);
	}

void CNetworkRequestChannel::SendMessageAndNotifyForResponse(const TLbsNetInternalMsgBase& aMessage)
	{
	LBSLOG(ELogP1, "CNetworkRequestChannel::SendMessage:");
	LBSLOG2(ELogP2, "Sending message : Type %d", aMessage.Type());
	
	iNetRequestChannel.SendMessage(aMessage, iStatus);
	SetActive();
	}
	
void CNetworkRequestChannel::SendNextBufferedMessage()
	{
	LBSLOG(ELogP1, "CNetworkRequestChannel::SendNextBufferedMessage:");
	
	const TLbsNetInternalMsgBase* msg = iMsgBuffer->PeekNextMessage();
	if (msg != NULL)
		{
		// Send the oldest buffered message. This will always
		// be the one at position zero.
		LBSLOG2(ELogP2, "Sending buffered message. Type: %d", msg->Type());
		SendMessageAndNotifyForResponse(*msg);
		iMsgBuffer->RemoveMessage(msg);
		}
	}

//
// CNetworkRequestMessageBuffer
//

/* Comparison function used to find two buffered 
   messages with the same type.
   
   See RPointerArray<T>::Find() and TIdentityRelation for
   more information about how this works.
*/
TBool CNetworkRequestMessageBuffer::IsMsgTypeEqual(
		const TLbsNetInternalMsgBase::TLbsNetInternalMsgType* aType, 
		const TLbsNetInternalMsgBase& aData)
	{
	return (*aType == aData.Type());
	}

/* Comparison function used to find two buffered 
   messages with the same contents.
   
   Uses a direct memory comparison to check contents.
   
   See RPointerArray<T>::Find() and TIdentityRelation for
   more information about how this works.
*/
TBool CNetworkRequestMessageBuffer::IsMsgEqual(
		const TLbsNetInternalMsgBase& aData1, 
		const TLbsNetInternalMsgBase& aData2)
	{
	return (Mem::Compare(
			(const TUint8*)&aData1, aData1.Size(), 
			(const TUint8*)&aData2, aData2.Size()) == 0);
	}

//
// CNetworkRequestStandardBuffer
//

/* The maximum number of messages that may be buffered at any one time.
*/
const TInt KBufferedMessageMaxCount = 3;

CNetworkRequestStandardBuffer::CNetworkRequestStandardBuffer() :
	iBufferedLocationRequestMsg(KInvalidSessionId, EFalse, 
								TLbsNetworkEnumInt::EServiceNone, 
								KInvalidQuality, KInvalidMethod),
	iBufferedPrivacyRequestMsg(KInvalidSessionId, EFalse,
							   KInvalidRequestPrivacy,
							   KInvalidExternalRequestInfo),
	iBufferedSessionCompleteMsg(KInvalidSessionId, KErrUnknown)
	{
	
	}

CNetworkRequestStandardBuffer::~CNetworkRequestStandardBuffer()
	{
	iBuffer.Reset();
	}

CNetworkRequestStandardBuffer* CNetworkRequestStandardBuffer::NewL()
	{
	CNetworkRequestStandardBuffer* self = new (ELeave) CNetworkRequestStandardBuffer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CNetworkRequestStandardBuffer::ConstructL()
	{
	iBuffer.ReserveL(KBufferedMessageMaxCount);
	}
	
TInt CNetworkRequestStandardBuffer::BufferMessage(
		const TLbsNetInternalMsgBase& aMessage)
	{
	LBSLOG(ELogP1, "CNetworkRequestStandardBuffer::BufferMessage:");
	LBSLOG2(ELogP2, "Buffering message : Type %d", aMessage.Type());
	__ASSERT_ALWAYS(iBuffer.Count() < KBufferedMessageMaxCount, User::Invariant());
	
	TInt err(KErrNone);
	
	// Network Request Handler has not read the message, so buffer it until it has finished
	switch (aMessage.Type())
		{
		case TLbsNetInternalMsgBase::ELocationRequest:
			{
			// If there is a privacy request already in the buffer, check to see if
			// it has the same SessionId as the incoming location request. If it has,
			// then it's ok. if not the we have to assume it is a new request that 
			// implicitly cancels the existing privacy request, so remove it from
			// the buffer.
			TInt index = iBuffer.Find(TLbsNetInternalMsgBase::EPrivacyRequest, 
									  IsMsgTypeEqual);
			if (index != KErrNotFound)
				{
				if (aMessage.SessionId() != iBuffer[index]->SessionId())
					{
					iBuffer.Remove(index);
					}
				}
			
			// Add the new request to the buffer.
			// (remove any existing message.)
			index = iBuffer.Find(TLbsNetInternalMsgBase::ELocationRequest, 
								 IsMsgTypeEqual);
			if (index != KErrNotFound)
				{
				iBuffer.Remove(index);
				}
			iBufferedLocationRequestMsg = static_cast<const TLbsNetLocationRequestMsg&>(aMessage);
			iBuffer.Append(&iBufferedLocationRequestMsg);
			break;			
			}
		case TLbsNetInternalMsgBase::EPrivacyRequest:
			{
			// If there is a location request already in the buffer, then
			// we have to assume that the new request overrides it 
			// (i.e. an implicit cancel), so remove it from the buffer.
			TInt index = iBuffer.Find(TLbsNetInternalMsgBase::ELocationRequest, 
										 IsMsgTypeEqual);
			if (index != KErrNotFound)
				{
				iBuffer.Remove(index);
				}
			
			// Add the new request to the buffer
			// Note: Overwrite any existing request.
			index = iBuffer.Find(TLbsNetInternalMsgBase::EPrivacyRequest, 
									IsMsgTypeEqual);
			if (index != KErrNotFound)
				{
				iBuffer.Remove(index);
				}
			iBufferedPrivacyRequestMsg = static_cast<const TLbsNetMtLrRequestMsg&>(aMessage);
			iBuffer.Append(&iBufferedPrivacyRequestMsg);
			break;			
			}
		case TLbsNetInternalMsgBase::ESessionComplete:
			{
			// If the new 'session complete' message matches any
			// pending location or privacy requests in the buffer,
			// then we know that those request(s) will be cancelled
			// in the NRH anyway, so we can remove them from
			// the buffer.
			TBool requestRemoved(EFalse);
			
			// Remove the pending privacy request if it has 
			// the same SessionId as the new session complete.
			TInt index = iBuffer.Find(TLbsNetInternalMsgBase::EPrivacyRequest, 
										 IsMsgTypeEqual);
			if (index != KErrNotFound)
				{
				if (aMessage.SessionId() == iBuffer[index]->SessionId())
					{
					iBuffer.Remove(index);
					requestRemoved = ETrue;
					}
				}

			// Remove the pending location request if it has 
			// the same SessionId as the new session complete.				
			index = iBuffer.Find(TLbsNetInternalMsgBase::ELocationRequest, 
									IsMsgTypeEqual);
			if (index != KErrNotFound)
				{
				// If the location request is part of a MT-LR request then
				// it is likely that the privacy request has already 
				// been sent to the NRH, so to avoid losing the cancel, don't
				// remove the location request from the buffer.				
				TLbsNetLocationRequestMsg& locReq = static_cast<TLbsNetLocationRequestMsg&>(*iBuffer[index]);
				if (aMessage.SessionId() == locReq.SessionId()
					&& (locReq.Service() != TLbsNetworkEnumInt::EServiceMobileTerminated))
					{
					iBuffer.Remove(index);
					requestRemoved = ETrue;
					}
				}
							
			// Add the new session complete to the buffer
			// only if it didn't cancel a request that was
			// pending in the buffer.
			if (EFalse == requestRemoved)
				{
				index = iBuffer.Find(TLbsNetInternalMsgBase::ESessionComplete, 
										IsMsgTypeEqual);
				if (index != KErrNotFound)
					{
					iBuffer.Remove(index);
					}
				iBufferedSessionCompleteMsg = static_cast<const TLbsNetSessionCompleteMsg&>(aMessage);
				iBuffer.Append(&iBufferedSessionCompleteMsg);
				}
			break;
			}
		default:
			{
			User::Invariant();
			break;
			}
		}
		LBSLOG2(ELogP2, "Number of messages in buffer is : %d", iBuffer.Count());
		
		return (err);
	}

const TLbsNetInternalMsgBase* CNetworkRequestStandardBuffer::PeekNextMessage()
	{
	if (iBuffer.Count() > 0)
		{
		return iBuffer[0];
		}
	else
		{
		return NULL;
		}
	}

void CNetworkRequestStandardBuffer::RemoveMessage(
		const TLbsNetInternalMsgBase* aMessage)
	{
	TInt index = iBuffer.Find(aMessage, IsMsgEqual);
	if (index != KErrNotFound)
		{
		iBuffer.Remove(index);
		}
	}

//
// CNetworkRequestStandardBuffer
//

CNetworkRequestPrivacyBuffer::CNetworkRequestPrivacyBuffer()
	{
	}

CNetworkRequestPrivacyBuffer::~CNetworkRequestPrivacyBuffer()
	{
	iBuffer.ResetAndDestroy();
	}

CNetworkRequestPrivacyBuffer* CNetworkRequestPrivacyBuffer::NewL()
	{
	CNetworkRequestPrivacyBuffer* self = new (ELeave) CNetworkRequestPrivacyBuffer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

void CNetworkRequestPrivacyBuffer::ConstructL()
	{
	}

TInt CNetworkRequestPrivacyBuffer::BufferMessage(const TLbsNetInternalMsgBase& aMessage)
	{
	LBSLOG(ELogP1, "CNetworkRequestPrivacyBuffer::BufferMessage:");
	LBSLOG2(ELogP2, "Buffering message : Type %d", aMessage.Type());
	
	TInt err(KErrNoMemory);
	TLbsNetInternalMsgBase* msg(NULL);
	
	// Network Request Handler has not read the message, so buffer it until it has finished
	switch (aMessage.Type())
		{
		case TLbsNetInternalMsgBase::EPrivacyRequest:
			{
			msg = new TLbsNetMtLrRequestMsg(
					KInvalidSessionId, EFalse,
					KInvalidRequestPrivacy,
					KInvalidExternalRequestInfo);
			if (msg != NULL)
				{
				*(static_cast<TLbsNetMtLrRequestMsg*>(msg)) = 
					static_cast<const TLbsNetMtLrRequestMsg&>(aMessage);
				err = KErrNone;
				}
			break;			
			}
		case TLbsNetInternalMsgBase::ESessionComplete:
			{
			msg = new TLbsNetSessionCompleteMsg(
					KInvalidSessionId, KErrUnknown);
			if (msg != NULL)
				{
				*(static_cast<TLbsNetSessionCompleteMsg*>(msg)) = 
					static_cast<const TLbsNetSessionCompleteMsg&>(aMessage);
				err = KErrNone;
				}
			break;
			}
		case TLbsNetInternalMsgBase::ELocationRequest: // Should never get this in privacy mode
		default:
			{
			err = KErrAccessDenied;
			break;
			}
		}

	// Only buffer the message if it was created ok.
	if (err == KErrNone)
		{
		err = iBuffer.Append(msg);
		if (err != KErrNone)
			{
			delete msg;
			}
		}

	LBSLOG2(ELogP2, "Number of messages in buffer is : %d", iBuffer.Count());	
	return err;
	}

const TLbsNetInternalMsgBase* CNetworkRequestPrivacyBuffer::PeekNextMessage()
	{
	if (iBuffer.Count() > 0)
		{
		return iBuffer[0];
		}
	else
		{
		return NULL;
		}	
	}

void CNetworkRequestPrivacyBuffer::RemoveMessage(
		const TLbsNetInternalMsgBase* aMessage)
	{
	TInt index = iBuffer.Find(aMessage, IsMsgEqual);
	if (index != KErrNotFound)
		{
		TLbsNetInternalMsgBase* msg = iBuffer[index];
		iBuffer.Remove(index);
		delete msg;
		}
	}

