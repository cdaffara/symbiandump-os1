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


//
// CNetworkRequestChannel
//

const TLbsNetSessionIdInt KInvalidSessionId(TUid::Uid(0xDC0DED), 999);
const TLbsNetPosRequestQualityInt KInvalidQuality;
const TLbsNetPosRequestMethodInt KInvalidMethod;
const TLbsNetPosRequestPrivacyInt KInvalidRequestPrivacy;
const TLbsExternalRequestInfo KInvalidExternalRequestInfo;
const TInt KEmergencyBufMaxCount = 2;

CNetworkRequestChannel::CNetworkRequestChannel(MNetworkRequestObserver& aObserver) : 
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iEmergencyLocationRequestMsg(KInvalidSessionId, EFalse, 
									TLbsNetworkEnumInt::EServiceNone, 
									KInvalidQuality, KInvalidMethod),
	iEmergencyPrivacyRequestMsg(KInvalidSessionId, EFalse,
								   KInvalidRequestPrivacy,
								   KInvalidExternalRequestInfo)
	{
	}
	
CNetworkRequestChannel::~CNetworkRequestChannel()
	{
	Cancel();
	iNetRequestChannel.Close();
	iEmergencyBuffer.Reset();
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
	iMsgBuffer = CRequestMessageBuffer::NewL();
	iEmergencyBuffer.ReserveL(KEmergencyBufMaxCount);
	
	iNetRequestChannel.OpenL(RLbsNetChannel::EChannelNG2NRH, *this);
	CActiveScheduler::Add(this);
	}
	
void CNetworkRequestChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	if (iEmergencyBuffer.Count() > 0) // If there is a pending emergency request send this first
		{
		SendMessageAndNotifyForResponse(*(iEmergencyBuffer[0]));
		RemoveEmergencyMessage(iEmergencyBuffer[0]);
		}
	else
		{
		// If there is a pending message in the buffer, pass it on to the NRH
		SendNextBufferedMessage();
		}
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
	TBool emergency = EFalse;

	if (!IsActive())
		{		
		// Immediately send the new message.
		SendMessageAndNotifyForResponse(aMessage);
		}
	else // First see if this is an emergency request
		{
		if(aMessage.Type() == TLbsNetInternalMsgBase::ELocationRequest)
			{
			if((static_cast<const TLbsNetLocationRequestMsg*>(&aMessage))->IsEmergency())
				{
				iEmergencyLocationRequestMsg = static_cast<const TLbsNetLocationRequestMsg&>(aMessage);
				TInt err = iEmergencyBuffer.Append(&iEmergencyLocationRequestMsg);
				if ( KErrNone != err)
					{
					LBSLOG(ELogP1, "LOCATION BUFFER APPEND FAILED!!");
					}
				emergency = ETrue;
				}
			}
		else if(aMessage.Type() == TLbsNetInternalMsgBase::EPrivacyRequest)
			{
			if( (static_cast<const TLbsNetMtLrRequestMsg*>(&aMessage))->IsEmergency() )
				{
				iEmergencyPrivacyRequestMsg = static_cast<const TLbsNetMtLrRequestMsg&>(aMessage);
				TInt err = iEmergencyBuffer.Append(&iEmergencyPrivacyRequestMsg);
				if ( KErrNone != err)
					{
					LBSLOG(ELogP1, "PRIVACY BUFFER APPEND FAILED!!");
					}
				emergency = ETrue;
				}
			}
		
		if(!emergency) // If it is an emergency then handle in RunL
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
	}
	
void CNetworkRequestChannel::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	LBSLOG(ELogP1, "CNetworkRequestChannel::ProcessNetChannelMessage");
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

void CNetworkRequestChannel::RemoveEmergencyMessage(
		const TLbsNetInternalMsgBase* aMessage)
	{
	TInt index = iEmergencyBuffer.Find(aMessage, iMsgBuffer->IsMsgEqual);
	if (index != KErrNotFound)
		{
		iEmergencyBuffer.Remove(index);
		}
	}

