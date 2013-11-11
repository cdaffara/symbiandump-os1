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

#include <elements/mm_context_internal.h>
#include <elements/mm_activities.h>
#include <elements/mm_node.h>
#include <elements/nm_address_internal.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemMeshMachCtxC, "ElemMeshMachCtxC");
#endif

using namespace MeshMachine;
using namespace Messages;

EXPORT_C TNodeContextBase::TNodeContextBase(AMMNodeBase& aNode, TSignatureBase& aMessage, const TRuntimeCtxId& aSender, const TNodeId& aRecipient, CNodeActivityBase* aNodeActivity)
:   iNodeActivity(aNodeActivity),
	iMessage(aMessage),
	iNode(aNode),
	iReturn(KErrNone),
	iPeer(NULL),
	iSender(aSender),
	iRecipient(aRecipient)
	{
	const TNodeCtxId* recipient = address_cast<const TNodeCtxId>(&aRecipient);
	__ASSERT_DEBUG(iNodeActivity==NULL || recipient==NULL || recipient->IsNull() || iNodeActivity->ActivityId() == recipient->NodeCtx(), User::Panic(KSpecAssert_ElemMeshMachCtxC, 1));

	//We only allow TNodeNullContext or the aSender must be valid
	__ASSERT_DEBUG((aSender.IsNull() && aRecipient.IsNull()) || (!aSender.IsNull() && !aRecipient.IsNull()), User::Panic(KSpecAssert_ElemMeshMachCtxC, 2));
	if (!iSender.IsNull())
		{
		if (iSender == aNode.Id())
			{
			iPeer = &aNode.SelfInterface();
			}
		else
			{
			iPeer = aNode.FindClient(iSender);
			}
		}
	}

EXPORT_C TNodeContextBase::~TNodeContextBase()
	{
	iPeer = NULL;
	}

EXPORT_C TUint16 TNodeContextBase::ActivityId() const
	{
	return iNodeActivity? iNodeActivity->ActivityId() : KActivityNull;
	}

EXPORT_C const TNodeId& TNodeContextBase::NodeId() const
	{
	return iNode.Id();
	}

EXPORT_C void TNodeContextBase::PostToSender(const TSignalBase& aMessage) const
	{
	if (iNodeActivity)
		{
		RClientInterface::OpenPostMessageClose(TNodeCtxId(iNodeActivity->ActivityId(), NodeId()), iSender, aMessage);
		}
	else
		{
		//TODO[PROD] - try to use iPeer (can be used for majority of cases)
		RClientInterface::OpenPostMessageClose(iRecipient, iSender, aMessage);
		}
	}

EXPORT_C TBool TNodeContextBase::IsPeer() const
	{
	return iPeer && iPeer != &iNode.SelfInterface();
	}

EXPORT_C TBool TNodeContextBase::IsSelf() const
	{
	return iPeer == &iNode.SelfInterface();
	}


