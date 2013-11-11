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
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NODEMESSAGES_FACTORY_H
#define SYMBIAN_NODEMESSAGES_FACTORY_H

#include <comms-infras/ss_common.h>
#include <comms-infras/ss_fact_internal.h>
#include <comms-infras/ss_nodemessages.h>

namespace ESock
{

/**
Message interface for factories
Nodes wishing to implement this interface must implment all message protocols in the interface.

@publishedPartner
@released
*/
class TCFFactory
	{
private:
	enum
	{
 	ECFFindOrCreatePeer			    = 1, //new realm TFactory. if tricky ignore till robs changes pulled in
 	ECFPeerFoundOrCreated         = 2, //new realm TFactory. if tricky ignore till robs changes pulled in
	ECFFactoryContainerJoinRequest  = 3
	};
protected:

public:
    enum { ERealmId = 0x10285F44 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms

    /**
    Sent to a factory container to request a factory object. The factory object is to
    be preexisting or newly created depending on the request argument and
    factory logic.

    @li Type:      Request for TFoundOrCreated.
    @li Fallible:  Yes

    @param  iFactoryObjectParams structure of type TFactoryObjectParams conveying the
                                 the request details (description of the requested factory object).
	*/
	typedef ESock::TMessageSigPlayerRoleUidFactoryQuery<ECFFindOrCreatePeer, TCFFactory::ERealmId> TFindOrCreatePeer;

	/**
	A node has been found or created that matchs the criteria specified in TFindOrCreateFactoryObject or TFindOrCreateTM

    @li Type: Response to TFindOrCreateFactoryObject/TFindOrCreateTM

    @param  iNodeId  Id of node found or created
    @param  iValue   Flags for the node
    */
	typedef Messages::TMessageSigNodeIdNumberDefault<ECFPeerFoundOrCreated, TCFFactory::ERealmId> TPeerFoundOrCreated;

    /**
	Request akin to TEPeer::TJoin but for use only by factory containers to establish a join with a central object broker
	Also carries the uid of the factory container joining so that the broker can distinguish between its clients.

    @li Type:      Request
    @li Fallible:  No

    @param  iCommsId address of the joining factory container
    @param  iClientType the relationship being established in the join
    @param  iFactoryContainerId the uid of the factory container
	*/
	typedef TMessageSigNodeIdClientTypeUid<ECFFactoryContainerJoinRequest, TCFFactory::ERealmId> TFactoryContainerJoinRequest;
	};

} //namespace esock

#endif
//SYMBIAN_NODEMESSAGES_FACTORY_H

