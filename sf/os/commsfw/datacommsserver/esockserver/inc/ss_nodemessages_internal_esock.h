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

#ifndef SYMBIAN_NODEMESSAGES_INTERNAL_ESOCK_H
#define SYMBIAN_NODEMESSAGES_INTERNAL_ESOCK_H

#include <comms-infras/ss_nodemessages_internal.h>
#include <comms-infras/ss_nodemessages.h>

namespace ESock
{

//any new enum added here needs to go into one of the utracedecoder definition files
//NEVER INSERT ANYTHING IN HERE ONLY ADD THE PENALTY IS SUDDEN DEATH!
class TCFInternalEsock
	{
private:
	enum
	{
	ECFCreateCSR                    = 1,
	ECFCSRCreated                   = 2,
	ECFFlowRequest                  = 3,
	ECFSubSess                      = 4,
	ECFSubConnectionOpenedEvent		= 5,
	ECFSubConnectionClosedEvent		= 6,
	ECFLegacyConnectionEnumResponse	= 7,
	ECFBuildStackRequest            = 8,
	ECFBuildStackResponse           = 9,
 	ECFFlowProvision                = 10
	};
protected:

public:
    enum { ERealmId = 0x10285F46 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms
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
    [Name]:      TCreateCSR
    [Semantics]: Sent to a factory container to request a CSR object.

    [Type]:      Request
    [Fallible]:  Yes

    [Structure]:
    param        iUid - UID of the top-most tier (used to initialise the new CSR)
    ===========================================================================*/
	typedef Messages::TMessageSigUid<ECFCreateCSR, TCFInternalEsock::ERealmId> TCreateCSR;

    /*==========================================================================
    [Name]:      TCSRCreated
    [Semantics]: Response to TCreateCSR.
                 Carries the actual object's address.

    [Type]:      Response
    [Fallible]:  No

    [Structure]: [399TODO]
    ===========================================================================*/
	typedef Messages::TMessageSigNodeId<ECFCSRCreated, TCFInternalEsock::ERealmId> TCSRCreated;

    /*==========================================================================
    [Name]:      TFlowRequest
    [Semantics]: Request sent from a data plane subsession to a factory container
                 (in the relevant plane - could be the SubConnection FC, the Connection
                 FC or the TierManager FC for the implicit flow requests).
                 Sent always indirectly, wrapped in (carried by) a self-dispatching message
                 such as TImplicitFlowRequest/TConnFlowRequest/TSubConnFlowRequest which,
                 upon arrival to the relevant destination node, creates a suitable FlowRequest
                 worker node on which activities run to perform the flow creation processes.

    [Type]:      Request
    [Fallible]:  Yes (at many stages)

    [Structure]:
    param        iValue - id of the IPC function such as ESoCreate etc
    ===========================================================================*/
 	typedef TMessageSigFlowParams<ECFFlowRequest, TCFInternalEsock::ERealmId> TFlowRequest;

    /*==========================================================================
    [Name]:      TSubSess
    [Semantics]: Reserved and private to EsockSvr

    [Type]:      N/A

    [Structure]:
                 N/A
    ===========================================================================*/
	typedef Messages::TMessageExtensionRMessage2<ECFSubSess, TCFInternalEsock::ERealmId> TSubSess;

    /*==========================================================================
    [Name]:      TSubConnectionOpenedEvent
    [Semantics]: [399TODO] RZ: Simon please finish.

    [Type]:
    [Fallible]:

    [Structure]:
    ===========================================================================*/
	typedef TMessageSigSubConnOpenedEvent<ECFSubConnectionOpenedEvent, TCFInternalEsock::ERealmId> TSubConnectionOpenedEvent;

    /*==========================================================================
    [Name]:      TSubConnectionClosedEvent
    [Semantics]: [399TODO] RZ: Simon please finish.

    [Type]:
    [Fallible]:

    [Structure]:
    ===========================================================================*/
	typedef TMessageSigSubConnClosedEvent<ECFSubConnectionClosedEvent, TCFInternalEsock::ERealmId> TSubConnectionClosedEvent;

//--Legacy Connection queries; to deprecate and ultimately remove
	/*==========================================================================
	[Name]:      TLegacyConnEnumResponse
	[Semantics]: Sent as a response to TLegacyConnSocketsRequest.

	[Type]:      Response from data thread to TLegacyConnSocketsResponse
	[Fallible]:  No.
	[Structure]:
	param: iValue1 - the number of sockets described in the buffer
		   iValue2 - pointer to HBufC8 containing the C-array TConnectionSocketInfo. Created
					 by data thread and destroyed by control thread
		   iValue3 - error code
	===========================================================================*/
	typedef Messages::TMessageSigNumberNumberNumber<ECFLegacyConnectionEnumResponse, TCFInternalEsock::ERealmId> TLegacyConnectionEnumResponse;

	/*==========================================================================
    [Name]:      TBuildStackRequest
    [Semantics]: Sent by a subconnection to itself to prompt the building of the stack beneath it.

    [Type]:      Request from subconnection to itself
    [Fallible]:  No.
    [Structure]:
    ===========================================================================*/
	typedef TMessageSigFamilyBundle<ECFBuildStackRequest, TCFInternalEsock::ERealmId> TBuildStackRequest;

    /*==========================================================================
    [Name]:      TBuildStackResponse
    [Semantics]: Sent as a response to TBuildStackRequest upon completion.

    [Type]:      Response to TBuildStackRequest
    [Fallible]:  No.
    [Structure]:
    ===========================================================================*/
	typedef Messages::TMessageSigVoid<ECFBuildStackResponse, TCFInternalEsock::ERealmId> TBuildStackResponse;

    /*==========================================================================
    [Name]:      TFlowProvision
    [Semantics]: This message carries a TFlowParams payload that provisions the CTransportFlowShim with
                 the protocol argument triple (addressfamily, protocol, socket type) that would previously
                 have been applied with ConstructL from the CSocket above. So the introduction of this message
                 effectively shifts responsiblity for initialising the flow to the SCPR and away from the socket
                 in fitting with three plane comms expected practice. In addition flows can then be stacked
                 upon CTransportFlowShim without the difficulty of the synchronous ConstructL during binding.

                 This message should follow TProvisionConfig and must be sent before the flow is bound to from above.
                 That is before the binder request is completed.

    [Type]:      Notification
                 IMPORTANT: There is no response to TFlowProvision. As with TProvisionConfig if the
                 recipient finds out that the configuration received is in any way inadequate, it cannot flag this
                 fact until it is requested to do something (e.g.: TCFDataClient::TStart) it can't
                 (due to inadequate configuration).

    [Fallible]:  N/A - Notification

    [Structure]:
    param        iFlowParams - TFlowParams struct with the given flow params for the flow.

    ===========================================================================*/
	typedef TMessageSigFlowParams<ECFFlowProvision, TCFInternalEsock::ERealmId> TFlowProvision;
	};

} //namespace esock

#endif
//SYMBIAN_NODEMESSAGES_CONTROLPROVIDER_H

