// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ss_mmnode_subsess.h"
#include "ss_apiext_messages.h"
#include <comms-infras/ss_nodemessages_internal_esock.h>

using namespace ESock;
using namespace Messages;
using namespace MeshMachine;

void CMMSockSubSession::HandleMessageReturnValue(TNodeContextBase& aContext)
    {
	TCFInternalEsock::TSubSess* msg = message_cast<TCFInternalEsock::TSubSess>(&aContext.iMessage);
	if (msg && !msg->iMessage.IsNull())
		{
		msg->iMessage.Complete(aContext.iReturn);
		DontCompleteCurrentRequest();
		}
	else
    	{
    	MeshMachine::AMMNodeBase::HandleMessageReturnValue(aContext);
    	}
    }

void CMMSockSubSession::ConstructL()
    {
    CSockSubSession::ConstructL(NULL);
    ACFMMNodeIdBase::ConstructL();
    }


//Very useful function not to have to construct a message on the stack
void CMMSockSubSession::ReceivedL(TUint16 aIpcId, const Messages::TSignatureBase& aCFMessage)
	{
	static_cast<Messages::ANode*>(this)->ReceivedL(TNodeCtxId(aIpcId, Id()), TNodeCtxId(aIpcId, Id()), const_cast<Messages::TSignatureBase&>(aCFMessage));
	}

//This function could be specialised by any of the subsessions
void CMMSockSubSession::CommsApiExtBindIfaceL(const RMessage2& aMessage)
	{
	const RNodeInterface* sp = ServiceProvider();
	if (sp)
    	{
		TSupportedCommsApiExt interfaceId = static_cast<TSupportedCommsApiExt>(aMessage.Int0());
		TOpenExtensionInterface msg(UniqueId(), interfaceId, aMessage);
    	sp->PostMessage(Id(), msg);
    	DontCompleteCurrentRequest();
    	}
    else
        {
        User::Leave(KErrNotReady);
        }
	}

//This function could be specialised by any of the subsessions
void CMMSockSubSession::CommsApiExtIfaceSendReceiveL(const RMessage2& aMessage)
	{
	const RNodeInterface* sp = ServiceProvider();
	if (sp)
    	{
		Elements::RResponseMsg responseMsg(aMessage, aMessage.Int0(), 1, 2);
		TApiExtMsgDispatcher msgDispatcher(UniqueId(), responseMsg);
    	sp->PostMessage(Id(), msgDispatcher);
    	DontCompleteCurrentRequest();
    	}
    else
        {
        User::Leave(KErrNotReady);
        }

	}

//This function could be specialised by any of the subsessions
void CMMSockSubSession::CloseExtensionInterface(const RMessage2& aMessage)
	{
	const RNodeInterface* sp = ServiceProvider();
	if (sp)
    	{
		TSupportedCommsApiExt interfaceId = static_cast<TSupportedCommsApiExt>(aMessage.Int0());
		TCloseExtensionInterface msg(UniqueId(), interfaceId, aMessage);
    	sp->PostMessage(Id(), msg);
    	DontCompleteCurrentRequest();
    	}
	}

//This function could be specialised by any of the subsessions
void CMMSockSubSession::CancelAndCloseAllClientExtIfaces()
	{
	const RNodeInterface* sp = ServiceProvider();
	if(sp)
		{
		TCancelAndCloseAllClientExtItf msg(UniqueId());
		sp->PostMessage(Id(), msg);
		}
	}


