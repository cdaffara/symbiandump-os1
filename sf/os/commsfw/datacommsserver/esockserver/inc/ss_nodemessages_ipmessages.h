// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll
*/

#ifndef SYMBIAN_NODEMESSAGES_IPMESSAGES_H
#define SYMBIAN_NODEMESSAGES_IPMESSAGES_H

#include <addressupdate.h>

namespace ESock
{

//any new enum added here needs to go into one of the utracedecoder definition files
//NEVER INSERT ANYTHING IN HERE ONLY ADD THE PENALTY IS SUDDEN DEATH!
class TCFIPMessages
	{
  private:
  	enum
 	{
	ECFDataClientRouted             = 1
	};
public:
    enum { ERealmId = 0x10285F47 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms
    /*==========================================================================
    <<Keys:>>
    [Name]:      <messagename>
    [Semantics]: Message semantics

    [Type]:      Request for <responsemsg>|Response to <requestmsg>|Notification/[Peerless]
                 Where:
                 Request      - request to perform an action. Sender should expect <responsemsg>
                                to ackowledge the completion of the action.
                 Response     - acknowledgement that the previous request is now completed.
                 Notification - a one-off message flagging an event.
                 Peerless     - the message is peerless, i.e.: the sender doesn't have to
                                be the recipient's client.
    [Fallible]:  Yes|No
                 Only applicable to requests and denotes whether the request can fail or not,
                 or, in other words, whether the request can be completed to with TError.

    [Structure]:
    Every message defined in this realm has at least:
    param        iSender      - the message sender
    param        iActivityId  - depending on the message type:
                 Request      - the activity id the response should be addressed to.
                                Request senders fill up this param to match the response
                                with the local activity. Activity id on the sender side
                                should uniquelly identify the activity expecting the response.
                 Response     - the activity on the recipient side expecting this response.
                                the activityid to put here has been previously conveyed
                                with the request.
                 Notification - ignored.
    ===========================================================================*/
    //[399TODO] RZ: the list of messages that follow is pretty long. I was then thinking
    //that perhaps we could chop it (with namespaces) into conceptual interfaces. Even better,
    //perhaps we could even make some runtime checks on message reception to panic if a message
    //received doesn't belong to any of the interfaces implemented locally.

//--Generic framework--
    //typedef Messages::TMessageSigVoid<ECFNull, TCFMessage::ERealmId> TNull;

    /*==========================================================================
    [Name]:      TDataClientRouted
    [Semantics]: Notification about the IP address info changing for the sender.

    [Type]:      Notification

    [Structure]:
    param: iAddrUpdate    - structure of type TAddrUpdate carrying the new IP address info
                            pertaining to the sender.
    ===========================================================================*/
    typedef TMessageSigAddrUpdate<ECFDataClientRouted, TCFIPMessages::ERealmId> TDataClientRouted;
	};

} //namespace esock

#endif
//SYMBIAN_NODEMESSAGES_IPMESSAGES_H

