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
//

#include <dummystates.h>
#include <dummyactivities.h>
#include "Node2Factory.h"
#include "Node2.h"

using namespace Messages;
using namespace MeshMachine;
using namespace Dummy;

#ifdef SYMBIAN_TRACE_ENABLE
	_LIT8(KNode2Tag, "Node2");
#endif

namespace Node2StartActivity
{

DECLARE_SMELEMENT_HEADER( TMarkSenderActive, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL()
		{
		iContext.iPeer->SetFlags(TDummyClientType::EActive);
		}
DECLARE_SMELEMENT_FOOTER( TMarkSenderActive )

DECLARE_AGGREGATED_TRANSITION2(
   TPostStartedAndMarkActive,
   MeshMachine::TPostMessageToSender<TStartProtocol::TStarted>,
   Node2StartActivity::TMarkSenderActive
	)
}

namespace Node2StartActivity
{
DECLARE_DEFINE_NODEACTIVITY(EActivityStart, Node2Start, TStartProtocol::TStart)
    NODEACTIVITY_ENTRY(KNoTag, TPostStartedAndMarkActive, DummyStates::TAwaitingStart, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace DummyActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(node2Activities)
	ACTIVITY_MAP_ENTRY(Node2StartActivity, Node2Start)
ACTIVITY_MAP_END_BASE(DummyActivities, dummyActivities)
}

//-=========================================================
//
// CNode2 methods
//
//-=========================================================
CNode2* CNode2::NewL(CNode2Factory& aFactory)
    {
    CNode2* provider = new (ELeave) CNode2(aFactory);
    CleanupStack::PushL(provider);
    provider->ConstructL();
    CleanupStack::Pop(provider);
	return provider;
    }

CNode2::CNode2(CNode2Factory& aFactory)
:	CTestNodeBase(aFactory, DummyActivities::node2Activities::Self())
    {
    NM_LOG_NODE_CREATE(KNode2Tag, CNode2);
    }

CNode2::~CNode2()
    {
    NM_LOG_NODE_DESTROY(KNode2Tag, CNode2);
    }

void CNode2::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
	{
	//Uncomment below if you want to see the error handling
    //if (aMessage.IsMessage<TDummyMessage::TStart>())
    //	{
    //	User::Leave(KErrNoMemory);
    //	}
	CTestNodeBase::ReceivedL(aSender, aRecipient, aMessage);
	}

TUid CNode2::ServiceProviderUid() const
	{
	User::Invariant(); //Never supposed to be called!
	return TUid::Uid(0x00000000);
	}

