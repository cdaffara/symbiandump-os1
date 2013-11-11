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

/**
 @file
*/

#include "nm_node.h"
#include "nm_messages_base.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessNodC, "ElemNodeMessNodC");
#endif

using namespace Messages;


EXPORT_C NetInterfaces::TInterfaceControl* ANode::FetchNodeInterfaceControlL(TInt /*aInterfaceId*/)
    {
    User::Leave(KErrInterfaceNotSupported);
    return NULL;
    }

EXPORT_C TAny* ANode::FetchNodeInterfaceL(TInt aInterfaceId)
    {
    NetInterfaces::TInterfaceControl* ic = FetchNodeInterfaceControlL(aInterfaceId);
    return ic->FetchInterfaceL(aInterfaceId);
    }

EXPORT_C const TNodeId& ASimpleNodeIdBase::NodeId() const
    {
    return ANodeId::Id();
    }

EXPORT_C const TNodeId& ANodeIdBase::NodeId() const
    {
    return ANodeId::Id();
    }

EXPORT_C ANodeBase::ANodeBase(const TNodeId& aNodeId)
:	TIfStaticFetcherNearestInHierarchy(this)
	{
	iSelfInterface.Open(aNodeId);
	}

EXPORT_C void ANodeBase::ReturnInterfacePtrL(ANodeBase*& aInterface)
    {
    aInterface = this;
    }

EXPORT_C ANodeBase::~ANodeBase()
	{
	//There can be no clients left in the array.
	//No Data Clients
	//No Control Clients
	//No Service Providers
	//No Control Provider

	TClientIter<TDefaultClientMatchPolicy> iter(GetClientIter<TDefaultClientMatchPolicy>(TClientType(TClientType::EAll)));
	while(*iter)
		{
	    NM_LOG((KNodeMessagesSubTag, _L8("ERROR: ~ANodeBase(%08x) - Client %08x Type %04x"),
	        Id().Ptr(), &((*iter)->RecipientId()), (*iter)->Type()));
		++iter;
		}

	__ASSERT_DEBUG(iClients.Count()==0, User::Panic(KSpecAssert_ElemNodeMessNodC, 1)); //Please fix your node.
    iClients.ResetAndDestroy();
    iSelfInterface.Close();
	}

EXPORT_C RNodeInterface* ANodeBase::NewClientInterfaceL(const TClientType& /*aClientType*/, TAny* /*aClientInfo*/)
    {
    return new(ELeave)RNodeInterface();
    }

/**
 * Find specified client identified by id specified
 * @return A pointer to RNodeInterface, or NULL, if not found
 */
EXPORT_C RNodeInterface* ANodeBase::FindClient(const TRuntimeCtxId& aId) const
    {
	TInt dummy = 0;
	return DoFindClient(aId, dummy);
    }

//As FindClient but must not return NULL
EXPORT_C RNodeInterface* ANodeBase::FindClientL(const TRuntimeCtxId& aId) const
    {
	TInt dummy = 0;
	RNodeInterface* c = DoFindClient(aId, dummy);
	User::LeaveIfError(c? KErrNone : KErrNotFound);
	return c;
    }

EXPORT_C RNodeInterface* ANodeBase::DoFindClient(const TRuntimeCtxId& aId, TInt& aFoundAtIndex) const
	{
	aFoundAtIndex = KErrNotFound;
	for (TInt i = iClients.Count() - 1; i>=0; i--)
        {
        if (*iClients[i] == aId)
            {
	        aFoundAtIndex = i;
            return iClients[i];
            }
        }
    return NULL;
	}

EXPORT_C TUint ANodeBase::CountClients(TClientIterBase& aClientIterator) const
	{
	TUint count = 0;
	while ((aClientIterator++) != NULL)
		{
		count++;
		}
	return count;
	}

EXPORT_C TInt ANodeBase::PostToClients(TClientIterBase& aClientIterator, const TRuntimeCtxId& aPostFrom, const TSignalBase& aMessage, TUint32 aFlagsToSet, TUint32 aFlagsToClear) const
	{
	TInt count = 0;

    RNodeInterface* ctl;
	while ((ctl = aClientIterator++) != NULL)
		{
		if (!(ctl->Flags() & TClientType::ELeaving))
    		{
    		count++;
    		ctl->PostMessage(aPostFrom, aMessage);
    		ctl->ClearFlags(aFlagsToClear);
    		ctl->SetFlags(aFlagsToSet);
    		}
		}
	return count;
	}

EXPORT_C RNodeInterface* ANodeBase::AddClientL(const TNodeId& aClientId, const TClientType& aClientType, TAny* aClientInfo)
	{
	NM_LOG((KNodeMessagesSubTag, _L8("ANodeBase %08x:\tAddClientL(%08x, flags=%04x, type=%04x)"), Id().Ptr(), aClientId.Ptr(), aClientType.Flags(), aClientType.Type()));
	RNodeInterface* client = FindClient(aClientId);
	if (NULL == client)
		{
		RNodeInterface* cc = NewClientInterfaceL(aClientType, aClientInfo);
		CleanupStack::PushL(cc);
		iClients.AppendL(cc); //don't change it some states may rely on the fact that clients
		//are appended
		CleanupStack::Pop(cc);
		cc->Open(aClientId, aClientType);
		client = cc;
		}
	else
		{
		client->SetFlags(aClientType.Flags());
		}

	return client;
	}

EXPORT_C void ANodeBase::RemoveClient(RNodeInterface& aClient)
	{
	TInt foundAt = iClients.Find(&aClient);
	__ASSERT_DEBUG(KErrNotFound!=foundAt, User::Panic(KSpecAssert_ElemNodeMessNodC, 2));
	RemoveClient(foundAt);
	}

EXPORT_C void ANodeBase::RemoveClient(TInt aIndex)
	{
	__ASSERT_ALWAYS(aIndex >= 0 && aIndex < iClients.Count(), User::Panic(Messages::KMessagesPanic,Messages::EClientNotValidPanic));
	RNodeInterface* client = iClients[aIndex];
	NM_LOG((KNodeMessagesSubTag, _L8("ANodeBase %08x:\tRemoveClient(%08x, flags=%04x, type=%04x)"), Id().Ptr(), client->RecipientId().Ptr(), client->Flags(), client->Type()));
	client->Close();
	delete client;
	iClients.Remove(aIndex);
	}


