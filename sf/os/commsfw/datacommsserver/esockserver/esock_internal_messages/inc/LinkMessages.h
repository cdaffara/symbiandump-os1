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
// Definitions for generic Link Tier messages.
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef LINKMESSAGES_H
#define LINKMESSAGES_H

#include <comms-infras/ss_nodemessages.h>


class TLinkMessage
    {
public:
    enum { ERealmId = 0x10281DE6 }; //same as agentcorepr
    
private:
    //
    // Messages are grouped according to direction in an attempt to make it easier
    // to decipher message direction whilst debugging.
    //
    enum
    /**
    Definition of generic Link Tier message ids
    */
    	{
    	// Flow -> SCPR
    	EAuthenticate                       = Messages::KNullMessageId + 1,
    	EFlowToAgentNotification            = 2,

    	// SCPR -> Flow
    	EAuthenticateResponse               = 3,
    	EAgentToFlowNotification            = 4,
    	
    	// AgentSCPr -> IPProtoSCPr
    	EAgentEventNotification				= 10
    	};
public:
    //
    // Generic Link Tier Flow -> SCPR messages
    //
    // These are simple messages with no arguments.

	typedef Messages::TMessageSigVoid<EAuthenticate, TLinkMessage::ERealmId> TAuthenticate;
    
    // Send a TCancel message to SCPR to cancel a previous TAuthenticateMsg
    // (there is no specific CancelAuthenticate)

    // Notification message to Agent
    typedef Messages::TMessageSigNumber<EFlowToAgentNotification, TLinkMessage::ERealmId> TFlowToAgentNotification;



    //
    // Generic Link Tier SCPR -> Flow messages
    //
    // These are simple messages with no arguments.

    typedef Messages::TMessageSigVoid<EAuthenticateResponse, TLinkMessage::ERealmId> TAuthenticateResponse;

    // Notification message to Flow
    typedef Messages::TMessageSigNumber<EAgentToFlowNotification, TLinkMessage::ERealmId> TAgentToFlowNotification;
    
    //
    // AgentSCPr -> IPProtoSCPr
    //
    typedef Messages::TMessageSigNumberNumber<EAgentEventNotification, TLinkMessage::ERealmId> TAgentEventNotification;
    };

#endif
// LINKMESSAGES_H

