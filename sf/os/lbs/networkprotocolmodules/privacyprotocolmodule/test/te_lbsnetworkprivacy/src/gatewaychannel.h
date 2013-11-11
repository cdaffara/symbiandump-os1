// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Gateway channel class handles sending messages 
// to the dummy NG over the RLbsNetChannel 
// 
//


#ifndef __CGATEWAYCHANNEL_H__
#define __CGATEWAYCHANNEL_H__

#include <e32cmn.h>
#include <ecom/ecom.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include "lbsnetinternalapi.h"

// Forward declaration
class MLbsNetChannelObserver;

/* Class that handles sending messages to the Dummy Network Gateway
 * over the RLbsNetChannel interface 
 * It is an active object because it needs to wait for the
 * acknowledgement signal to ensure the message has been
 * read by the Network Gateway.
 */
class CGatewayChannel: public CActive
                                   
	{

public:
        static CGatewayChannel* NewL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId);
		~CGatewayChannel();
		
		void RunL();
		void DoCancel();
		
		void SendMessageAndWaitForResponse(const TLbsNetInternalMsgBase& aMessage);
		
private:
		CGatewayChannel();
		void ConstructL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId);
	
private:
		RLbsNetChannel iNetGatewayChannel;
	};
#endif //gatewaychannel.h
