// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SS_NODEMESSAGES_PEER_H
#define SS_NODEMESSAGES_PEER_H

#include <elements/nm_messages_peer.h>
#include <comms-infras/ss_nodemessages.h>

namespace ESock
{

class TCFPeer : public Messages::TEPeer
	{
private:
  	enum
 	{
	ECFJoinRequest                  = 1,
	ECFJoinComplete                 = 2,
	};

public:
    enum { ERealmId = 0x102864BB };

	/*==========================================================================
	[Name]:      TJoinRequest
	[Semantics]: A request to add a new control or data client to the recipient's clients' list.
		 
	[Type]:      Request for TJoinComplete/Peerless
	[Fallible]:  Yes.

	[Structure]: 
	param 		 iNodeId - the identity of the control client (may be different from the sender).
	param 		 iClientType - the type of client (control or data) and flags describing the client.
	param		 iValue - current priority of sender's node.
	===========================================================================*/        
	typedef ESock::TCFMessageSigNodeIdClientTypeNumberDefault<ECFJoinRequest, TCFPeer::ERealmId> TJoinRequest;
	typedef Messages::TMessageSigVoid<ECFJoinComplete, TCFPeer::ERealmId> TJoinComplete;
	};
} //namespace ESock

#endif

// SS_NODEMESSAGES_PEER_H

