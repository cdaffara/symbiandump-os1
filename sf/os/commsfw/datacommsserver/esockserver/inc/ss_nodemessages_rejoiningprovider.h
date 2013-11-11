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
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_NODEMESSAGES_REJOININGPROVIDER_H
#define SYMBIAN_NODEMESSAGES_REJOININGPROVIDER_H

namespace ESock
{

//any new enum added here needs to go into one of the utracedecoder definition files
//NEVER INSERT ANYTHING IN HERE ONLY ADD THE PENALTY IS SUDDEN DEATH!
class TCFRejoiningProvider
	{
  private:
  	enum
 	{
	ECFRejoinDataClientRequest      = 1,
	ECFRejoinComplete               = 2,
	};
protected:

public:
    enum { ERealmId = 0x10285F4C }; //UID allocated on 4/6/08 from KUidNodeMessageRealms
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

    /*==========================================================================
    [Name]:      TRejoinDataClientRequest
    [Semantics]: Sent to a provider requesting that the provider will move
                 one of its dataclient to another provider.

    [Type]:      Request for RReJoinComplete.
    [Fallible]:  Yes.

    [Structure]:
    param: iCommsId     - the dataclient to be migrated.
    param: iCommsId     - the new owner.
    ===========================================================================*/
    typedef Messages::TMessageSigNodeIdNodeId<ECFRejoinDataClientRequest, TCFRejoiningProvider::ERealmId> TRejoinDataClientRequest;
    typedef Messages::TMessageSigVoid<ECFRejoinComplete,                    TCFRejoiningProvider::ERealmId> TRejoinComplete;

	};

} //namespace esock

#endif
//SYMBIAN_NODEMESSAGES_REJOININGPROVIDER_H

