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
#include "lbsprocessuiddefs.h"
#include "netrequestchannel.h"
#include "lbsnrhngmsgs.h"
#include "LbsPsyNgMsgs.h"

const TLbsNetSessionIdInt KInvalidSessionId(TUid::Uid(0xDC0DED), 999);
const TLbsNetPosRequestQualityInt KInvalidQuality;
const TLbsNetPosRequestMethodInt KInvalidMethod;
const TLbsNetPosRequestPrivacyInt KInvalidRequestPrivacy;
const TLbsExternalRequestInfo KInvalidExternalRequestInfo;


//
// CRequestMessageBuffer
//

/* Comparison function used to find two buffered 
   messages with the same type.
   
   See RPointerArray<T>::Find() and TIdentityRelation for
   more information about how this works.
*/
TBool CRequestMessageBuffer::IsMsgTypeEqual(
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
TBool CRequestMessageBuffer::IsMsgEqual(
		const TLbsNetInternalMsgBase& aData1, 
		const TLbsNetInternalMsgBase& aData2)
	{
	return (Mem::Compare(
			(const TUint8*)&aData1, aData1.Size(), 
			(const TUint8*)&aData2, aData2.Size()) == 0);
	}

CRequestMessageBuffer::~CRequestMessageBuffer()
	{
	iBuffer.ResetAndDestroy();
	}

CRequestMessageBuffer* CRequestMessageBuffer::NewL()
	{
	CRequestMessageBuffer* self = new (ELeave) CRequestMessageBuffer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

void CRequestMessageBuffer::ConstructL()
	{
	}

TInt CRequestMessageBuffer::BufferMessage(const TLbsNetInternalMsgBase& aMessage)
	{
	LBSLOG(ELogP1, "CRequestMessageBuffer::BufferMessage:");
	LBSLOG2(ELogP2, "Buffering message : Type %d", aMessage.Type());

	TInt err(KErrNoMemory);
	TLbsNetInternalMsgBase* msg(NULL);
	
	// Network Request Handler has not read the message, so buffer it until it has finished
	switch (aMessage.Type())
		{
		case TLbsNetInternalMsgBase::ELocationRequest:
			{
			msg = new TLbsNetLocationRequestMsg(
					KInvalidSessionId, EFalse,
					TLbsNetworkEnumInt::EServiceNone,
					KInvalidQuality, KInvalidMethod);
					
			if (msg != NULL)
				{
				*(static_cast<TLbsNetLocationRequestMsg*>(msg)) = 
					static_cast<const TLbsNetLocationRequestMsg&>(aMessage);
				err = KErrNone;
				}
			break;
			}

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
			
		case TLbsNetInternalMsgBase::ENetworkLocationUpdate:
			{
			TPositionInfo info;
			TTime time;
			msg = new TLbsNetLocationUpdateMsg(
					KInvalidSessionId, KErrUnknown,
					info, time);
			
	        if (msg != NULL)
	             {
	             *(static_cast<TLbsNetLocationUpdateMsg*>(msg)) = 
	                 static_cast<const TLbsNetLocationUpdateMsg&>(aMessage);
	             err = KErrNone;
	             }
	        break;
	        }

        case TLbsNetInternalMsgBase::ECellLocationResponse:
            {
            TPositionInfo info;
            TTime time;
            msg = new TLbsCellLocationResponseMsg(
                    KInvalidSessionId, KErrUnknown,
                    info);
            
			if (msg != NULL)
				{
				*(static_cast<TLbsCellLocationResponseMsg*>(msg)) = 
					static_cast<const TLbsCellLocationResponseMsg&>(aMessage);
				err = KErrNone;
				}
			break;
			}
			
		default:
			{
			err = KErrAccessDenied;
			break;
			}
		}

	// Only buffer the message if it was created successfully.
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

const TLbsNetInternalMsgBase* CRequestMessageBuffer::PeekNextMessage()
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

void CRequestMessageBuffer::RemoveMessage(
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

