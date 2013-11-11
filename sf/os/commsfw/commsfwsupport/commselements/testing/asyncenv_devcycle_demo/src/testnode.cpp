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
//

#include "testnode.h"
#include <dummynode.h>
#include <dummymessages.h>
#include <dummyruntimectxaddressbook.h>
#include <elements/nm_interfaces.h>

#include <elements/nm_messages_peer.h>
#include <elements/nm_messages_child.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemTestingtstnd, "ElemTestingtstnd");
#endif


using namespace Dummy;
using namespace Messages;

#ifdef SYMBIAN_TRACE_ENABLE
	_LIT8(KTestNodeTag, "TestNode");
#endif

//
//CTestNode
CTestNode* CTestNode::NewLC()
	{
	CTestNode* self = new (ELeave) CTestNode;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

TInt StopTest(TAny* /*aPtr*/)
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}

void CTestNode::ConstructL()
	{
	__ASSERT_DEBUG(iStopTestCb==NULL, User::Panic(KSpecAssert_ElemTestingtstnd, 1));
	iStopTestCb = new (ELeave) CAsyncCallBack(TCallBack(&StopTest), CActive::EPriorityIdle);
	}

CTestNode::CTestNode()
	{
	NM_LOG_NODE_CREATE(KTestNodeTag, CTestNode);
	}

CTestNode::~CTestNode()
	{
	NM_LOG_NODE_DESTROY(KTestNodeTag, CTestNode);
	delete iStopTestCb;
	iStopTestCb = NULL;
	}

void CTestNode::StartTest()
	{
	//Node's UID would normally would be read from a DB or some other configuration mechanism..
	RNodeInterface::OpenPostMessageClose(ANodeId::Id(), TRuntimeCtxAddressBook::ObjectBroker(), TBindProtocol::TFindOrCreateFactoryObject(TUid::Uid(0x10285A7F)).CRef());
	CActiveScheduler::Start();
	}

//Please note that this node drives a very simple protocols with just two other nodes!
//It also never receives unsolicited notifications or state change messages.
//This node also represents the user of our demo and as such is never cancelled itself.
//This is why it is possible to made it a non-MeshMachine based node.

//We have not "Meshed" this node to show how complex the asynchronous interactions become for very trivial cases.
//Imagine how this function would look like if the CTestNode needed to support interactions with just onle more
//other node, able to request something from CTestNode as well as cancel its requests.
//We do not see this easily achievable using functions, switch & if statements.
void CTestNode::ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& /*aRecipient*/, Messages::TSignatureBase& aMessage)
	{
    if (aMessage.IsMessage<TBindProtocol::TFactoryObjectCreated>())
    	{
    	TBindProtocol::TFactoryObjectCreated& msg = message_cast<TBindProtocol::TFactoryObjectCreated>(aMessage);
    	__ASSERT_DEBUG(!msg.iNodeId.IsNull(), User::Panic(KSpecAssert_ElemTestingtstnd, 2));
    	RNodeInterface::OpenPostMessageClose(ANodeId::Id(), msg.iNodeId, TBindProtocol::TClientJoiningRequest().CRef());
    	}
    else if (aMessage.IsMessage<TBindProtocol::TJoinComplete>())
    	{
    	RNodeInterface* sp = NULL;
    	TRAP_IGNORE(sp = AddClientL(address_cast<TNodeId>(aSender), TClientType(TDummyClientType::EServProvider)));
    	if (sp==NULL)
    		{
    		RClientInterface::OpenPostMessageClose(ANodeId::Id(), aSender, TEPeer::TLeaveRequest().CRef());
    		iStopTestCb->CallBack(); //Finish the test, but only after all message traffic has seased
    		}
    	else
    		{
    		sp->PostMessage(ANodeId::Id(), TStartProtocol::TStart(32).CRef());  //Takes a dummy param for demo purposes only
    		sp->SetFlags(TDummyClientType::EStarting);
    		}
    	}
    else if (aMessage.IsMessage<TEBase::TError>())
    	{
    	TEBase::TError& msg = message_cast<TEBase::TError>(aMessage);
	    if (msg.iMsgId == TNodeSignal::TMessageId(TBindProtocol::TFindOrCreateFactoryObject::EId, TBindProtocol::ERealmId))
	    	{
	    	iStopTestCb->CallBack(); //Finish the test, but only after all message traffic has seased
	    	}
	    else if (msg.iMsgId == TNodeSignal::TMessageId(TBindProtocol::TClientJoiningRequest::EId, TBindProtocol::ERealmId))
	    	{
			iStopTestCb->CallBack(); //Finish the test, but only after all message traffic has seased
	    	}
	    else if (msg.iMsgId == TNodeSignal::TMessageId(TStartProtocol::TStart::EId, TStartProtocol::ERealmId))
	    	{
			RNodeInterface* sp = FindClient(address_cast<TNodeId>(aSender));
			__ASSERT_DEBUG(sp && sp->Type() & TDummyClientType::EServProvider, User::Panic(KSpecAssert_ElemTestingtstnd, 3));
	        __ASSERT_DEBUG(sp->Flags() & TDummyClientType::EStarting, User::Panic(KSpecAssert_ElemTestingtstnd, 4));
            __ASSERT_DEBUG(!(sp->Flags() & TDummyClientType::EStarted), User::Panic(KSpecAssert_ElemTestingtstnd, 5));
            __ASSERT_DEBUG(!(sp->Flags() & TDummyClientType::EStopping), User::Panic(KSpecAssert_ElemTestingtstnd, 6));
            __ASSERT_DEBUG(!(sp->Flags() & TClientType::ELeaving), User::Panic(KSpecAssert_ElemTestingtstnd, 7));
	        sp->ClearFlags(TDummyClientType::EStarting);
			sp->PostMessage(ANodeId::Id(), TEPeer::TLeaveRequest().CRef());

			//This is the final good-bye message. No further communication with sp is safe.
			//We are now awaiting the channel-clear confirmation. From now on any attempt to post a message to sp would panic (UDEB)
			sp->SetFlags(TClientType::ELeaving);

			//The Service Provider will be removed and the test finished after the leave completes
			}
		else
			{
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemTestingtstnd, 8)); //Unexpected error message
			}
    	}
    else
    	{
		RNodeInterface* sp = FindClient(address_cast<TNodeId>(aSender));
		__ASSERT_DEBUG(sp && sp->Type() & TDummyClientType::EServProvider, User::Panic(KSpecAssert_ElemTestingtstnd, 9));
	    if (aMessage.IsMessage<TStartProtocol::TStarted>())
	    	{
	    	__ASSERT_DEBUG(sp->Flags() & TDummyClientType::EStarting, User::Panic(KSpecAssert_ElemTestingtstnd, 10));
	    	__ASSERT_DEBUG(!(sp->Flags() & TDummyClientType::EStarted), User::Panic(KSpecAssert_ElemTestingtstnd, 11));
	    	sp->ClearFlags(TDummyClientType::EStarting);
	    	sp->SetFlags(TDummyClientType::EStarted);

		//Start the second part of the test i.e. stopping of the whole chain

	    	sp->PostMessage(ANodeId::Id(), TStopProtocol::TStop().CRef());
	    	sp->SetFlags(TDummyClientType::EStopping);
	    	}
	    else if (aMessage.IsMessage<TStopProtocol::TStopped>())
	    	{
	    	__ASSERT_DEBUG(sp->Flags() & TDummyClientType::EStarted, User::Panic(KSpecAssert_ElemTestingtstnd, 12));
	    	__ASSERT_DEBUG(sp->Flags() & TDummyClientType::EStopping, User::Panic(KSpecAssert_ElemTestingtstnd, 13));
	    	sp->ClearFlags(TDummyClientType::EStarted | TDummyClientType::EStopping);
			sp->PostMessage(ANodeId::Id(), TEPeer::TLeaveRequest().CRef());

			//This is the final good-bye message. No further communication with sp is safe.
			//We are now awaiting the channel-clear confirmation. From now on any attempt to post a message to sp would panic (UDEB)
			sp->SetFlags(TClientType::ELeaving);
	    	}
	    else if (aMessage.IsMessage<TEPeer::TLeaveComplete>())
	    	{
	    	__ASSERT_DEBUG(sp->Flags() & TDummyClientType::ELeaving, User::Panic(KSpecAssert_ElemTestingtstnd, 14));
	    	__ASSERT_DEBUG(!(sp->Flags() & TDummyClientType::EStarted), User::Panic(KSpecAssert_ElemTestingtstnd, 15));
	    	__ASSERT_DEBUG(!(sp->Flags() & TDummyClientType::EStarting), User::Panic(KSpecAssert_ElemTestingtstnd, 16));
	    	__ASSERT_DEBUG(!(sp->Flags() & TDummyClientType::EStopping), User::Panic(KSpecAssert_ElemTestingtstnd, 17));

	    	RemoveClient(*sp);
	    	iStopTestCb->CallBack(); //Finish the test, but only after all message traffic has seased
	    	}
	    else
	    	{
	    	__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemTestingtstnd, 18)); //Unexpected message
	    	}
	    }
	}




