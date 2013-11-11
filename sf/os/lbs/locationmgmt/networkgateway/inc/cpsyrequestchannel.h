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
// Definition of the PSY request channel component.
// 
//

#ifndef CPSYREQUESTCHANNEL_H_
#define CPSYREQUESTCHANNEL_H_

#include "lbsnetinternalapi.h"
#include "LbsPsyNgMsgs.h"
#include "requestmessagebuffer.h"

/**
Observer class to be used by the Network Gateway to inform it
when messages are deivered from a Network PSY.
*/
class MPsyRequestObserver
    {
public:
    virtual void ProcessPsyMessage(const TLbsNetInternalMsgBase& aMessage) = 0;
    };

/**
Provides an API for use by Network PSYs to the Network Gateway.

The creator of a CPsyRequestChannel should pass in the channel it
wishes the object to write to and read from.
*/
class CPsyRequestChannel : public CActive, public MLbsNetChannelObserver
    {
public:
    static CPsyRequestChannel* NewL(MPsyRequestObserver& aObserver, TUint aChannel);
    ~CPsyRequestChannel();
    
    // from CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    
    // Send a message to a PSY
    void SendNetworkResponse(const TLbsNetInternalMsgBase& aMessage);
    
    // from MLbsNetChannelObserver : process a message from a PSY
    void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
    
private:
    CPsyRequestChannel(MPsyRequestObserver& aObserver);
    void ConstructL(TUint aChannel);
    void SendMessage(const TLbsNetInternalMsgBase& aMessage);
    void SendNextBufferedMessage();
    
private:
    CRequestMessageBuffer* iMsgBuffer;
    MPsyRequestObserver& iObserver;
    RLbsNetChannel iPsyResponseChannel;
    };

#endif /* CPSYREQUESTCHANNEL_H_ */
