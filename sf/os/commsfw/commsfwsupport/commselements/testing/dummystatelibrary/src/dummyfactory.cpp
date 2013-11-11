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

#include "dummyfactory.h"
#include "dummymessages.h"
#include "dummynode.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemTestingdmyfc, "ElemTestingdmyfc");
#endif

using namespace Dummy;
using namespace Factories;
using namespace Messages;

#ifdef SYMBIAN_TRACE_ENABLE
	_LIT8(KTestFactoryTag, "TestFactory");
#endif

//
//XFactoryQuery
class XFactoryQuery : public MFactoryQuery
	{
	friend class CTestFactoryContainer;

public:

	virtual TMatchResult Match(TFactoryObjectInfo& aFactoryObjectInfo);
	};

MFactoryQuery::TMatchResult XFactoryQuery::Match(TFactoryObjectInfo& /*aFactoryObjectInfo*/)
	{
	return MFactoryQuery::EMatch; //Return the first.
	}


//
//CTestFactoryContainer
EXPORT_C void CTestFactoryContainer::InstallL()
	{
	__ASSERT_DEBUG(Dll::Tls()==NULL, User::Panic(KSpecAssert_ElemTestingdmyfc, 1));
	Dll::SetTls(new (ELeave) CTestFactoryContainer);
	}

EXPORT_C void CTestFactoryContainer::UnInstall()
	{
	CFactoryContainerNode* fc = reinterpret_cast<CFactoryContainerNode*>(Dll::Tls());
	Dll::SetTls(NULL);
	__ASSERT_DEBUG(fc!=NULL, User::Panic(KSpecAssert_ElemTestingdmyfc, 2));
	delete fc;
	}

CTestFactoryBase* CTestFactoryContainer::FindFactory(TUid aFactoryUid) const
	{
	return static_cast<CTestFactoryBase*>((CFactoryContainerNode::FindFactory(aFactoryUid)));
	}

CTestFactoryBase& CTestFactoryContainer::FindOrCreateFactoryL(TUid aFactoryUid)
    {
	CFactoryBase* factory = FindFactory(aFactoryUid);
	if (NULL==factory)
		{
		factory = CTestFactoryBase::NewL(aFactoryUid,*this);
		}
	return static_cast<CTestFactoryBase&>(*factory);
    }

CTestFactoryBase* CTestFactoryContainer::Factory(TInt aIndex) const
	{
	return static_cast<CTestFactoryBase*>(CFactoryContainerNode::Factory(aIndex));
	}

CTestFactoryContainer::~CTestFactoryContainer()
	{
	NM_LOG_NODE_DESTROY(KTestFactoryTag, CTestFactoryContainer);
	}

CTestFactoryContainer::CTestFactoryContainer()
:	CFactoryContainerNode()
	{
	NM_LOG_NODE_CREATE(KTestFactoryTag, CTestFactoryContainer);
	}

void CTestFactoryContainer::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage)
	{
	if (aMessage.IsMessage<TBindProtocol::TFindOrCreateFactoryObject>())
		{
		//RClientInterface::OpenPostMessageClose(Id(), aSender, TEBase::TError(aMessage.MessageId(), KErrNoMemory).CRef());
		TBindProtocol::TFindOrCreateFactoryObject& msg = message_cast<TBindProtocol::TFindOrCreateFactoryObject>(aMessage);
		CTestFactoryBase& factory = FindOrCreateFactoryL(msg.iUid);
		XFactoryQuery query; //Always match query!
		CTestNodeBase& node = factory.FindOrCreateL(query);
		RClientInterface::OpenPostMessageClose(Id(), aSender, TBindProtocol::TFactoryObjectCreated(node.Id(), &node).CRef());
		aMessage.ClearMessageId();
		}
	else
		{
		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ElemTestingdmyfc, 3)); //For debug configurations
		User::Leave(KErrNotSupported); //For release configurations
		}
	}


//
//CTestFactoryBase
EXPORT_C CTestFactoryBase::CTestFactoryBase(TUid aFactoryId, CTestFactoryContainer& aParentContainer)
:	CFactoryBase(aFactoryId, aParentContainer)
	{
	}

EXPORT_C CTestFactoryBase::~CTestFactoryBase()
	{
	}

EXPORT_C CTestNodeBase& CTestFactoryBase::FindOrCreateL(MFactoryQuery& aQuery)
	{
	CTestNodeBase* node = Find(aQuery);
	if (node == NULL)
		node = &CreateL();
	return *node;
	}

EXPORT_C CTestNodeBase* CTestFactoryBase::Find(MFactoryQuery& aQuery)
	{
	CTestNodeBase* node = static_cast<CTestNodeBase*>(FindObject(aQuery));
	if (node)
		{
		++node->iOutstandingJoinCount;
		}
    return node;
	}

EXPORT_C CTestNodeBase& CTestFactoryBase::CreateL()
	{
	CTestNodeBase& node = DoCreateL();
	CleanupStack::PushL(&node);
	AddManagedObjectL(node);
	CleanupStack::Pop(&node);
	++node.iOutstandingJoinCount; //From 0 to 1
	return node;
	}



