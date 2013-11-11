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
// Definition of PSY request channel component functions.
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
#include "cpsyrequestchannel.h"


CPsyRequestChannel::CPsyRequestChannel(MPsyRequestObserver& aObserver) : 
    CActive(EPriorityStandard),
    iObserver(aObserver)
    {
    }
    
CPsyRequestChannel::~CPsyRequestChannel()
    {
    Cancel();
    iPsyResponseChannel.Close();
    delete iMsgBuffer;
    }

CPsyRequestChannel* CPsyRequestChannel::NewL(MPsyRequestObserver& aObserver, TUint aChannel)
    {
    CPsyRequestChannel* self = new (ELeave) CPsyRequestChannel(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aChannel);
    CleanupStack::Pop(self);
    return self;
    }
    
/**
Opens the channel specified
 
@param aChannel, Net Internal API channel to use for this object
*/
void CPsyRequestChannel::ConstructL(TUint aChannel)
    {
    iPsyResponseChannel.OpenL(static_cast<RLbsNetChannel::TLbsNetChannelId> (aChannel), *this);
    iMsgBuffer = CRequestMessageBuffer::NewL();
    CActiveScheduler::Add(this);
    }
    
/**
Handle the ACK from the SendMessage call.
*/
void CPsyRequestChannel::RunL()
    {
    LBS_RDEBUG_ARGINT("PSY Req Chan", "LBS", "RunL", iStatus.Int());

    User::LeaveIfError(iStatus.Int());
    SendNextBufferedMessage();
    }
    
void CPsyRequestChannel::DoCancel()
    {
    iPsyResponseChannel.CancelSendMessageNotification();
    }
    
TInt CPsyRequestChannel::RunError(TInt aError)
    {
    return aError;
    }

/**
Sends a message to a Network PSY from the Network Gateway
 
@param aChannel, Net Internal API channel to use for this object
*/
void CPsyRequestChannel::SendNetworkResponse(const TLbsNetInternalMsgBase& aMessage)
    {
    LBSLOG(ELogP1, "CPsyRequestChannel::SendNetworkResponse:");
    if (!IsActive())
        {       
        // Immediately send the new message.
        SendMessage(aMessage);
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


void CPsyRequestChannel::SendMessage(const TLbsNetInternalMsgBase& aMessage)
    {
    LBSLOG(ELogP1, "CPsyRequestChannel::SendMessage:");
    LBSLOG2(ELogP2, "Sending message : Type %d", aMessage.Type());
    
    iPsyResponseChannel.SendMessage(aMessage, iStatus);
    SetActive();
    }


void CPsyRequestChannel::SendNextBufferedMessage()
    {
    LBSLOG(ELogP1, "CPsyRequestChannel::SendNextBufferedMessage:");
    
    const TLbsNetInternalMsgBase* msg = iMsgBuffer->PeekNextMessage();
    if (msg != NULL)
        {
        // Send the oldest buffered message. This will always
        // be the one at position zero.
        LBSLOG2(ELogP2, "Sending buffered message. Type: %d", msg->Type());
        SendMessage(*msg);
        iMsgBuffer->RemoveMessage(msg);
        }
    }


/**
Passes a message from a Network PSY to the Network Gateway
 
@param aChannel, Net Internal API channel to use for this object
*/
void CPsyRequestChannel::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
    {
    LBSLOG(ELogP1, "CPsyRequestChannel::ProcessNetChannelMessage");
    __ASSERT_DEBUG(aChannelId == RLbsNetChannel::EChannelNG2SUPLPSY, User::Panic(KLbsNGFault, ENGUnexpectedNetChannelId));
    (void) aChannelId;
    iObserver.ProcessPsyMessage(aMessage);
    }



