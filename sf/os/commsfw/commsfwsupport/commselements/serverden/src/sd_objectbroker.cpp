// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sd_objectbroker.h"

using namespace Messages;
using namespace Den;

#ifdef SYMBIAN_TRACE_ENABLE
	_LIT8(KObjectBrokerTag, "ObjectBroker");
#endif

//-=========================================================
//
//Panics
//
//-=========================================================
_LIT(KObjectBrokerPanic,"ObjectBroker");

enum
	{
	EPanicFactoryContainerNotRegistered = 1
	};

//
//CCommonObjectBroker
EXPORT_C CCommonObjectBroker::~CCommonObjectBroker()
	{
	NM_LOG_NODE_DESTROY(KObjectBrokerTag, CCommonObjectBroker);
	iClients.ResetAndDestroy();
	}

EXPORT_C CCommonObjectBroker::CCommonObjectBroker()
	{
	NM_LOG_NODE_CREATE(KObjectBrokerTag, CCommonObjectBroker);
	}

EXPORT_C void CCommonObjectBroker::ForwardMesasge(const TRuntimeCtxId& aSender, const TSignalBase& aMessage, const TClientType& aClientType)
	{
	this->PostToClients<TDefaultClientMatchPolicy>(aSender, aMessage, aClientType);

	__ASSERT_ALWAYS(1 == PostToClients<TDefaultClientMatchPolicy>(aSender, aMessage, aClientType),
		User::Panic(KObjectBrokerPanic, EPanicFactoryContainerNotRegistered));
	}

EXPORT_C void CCommonObjectBroker::UpdateWorkerInfo(CommsFW::TWorkerId aPlayerId,
									 const TClientType& aClientType,
									 const TNodeId& aFactoryId)
	{

	//TODO: This is never supposed to leave.
	//This binding is happening during the boot sequence hence it is save to assume the
	//allocation of RNodeInterface will succeed.
	//What we need to do to make sure of that is to use preallocation.

	//const TNodeId& aClientId, const TClientType& aClientType, TAny* aClientInfo = NULL
	__ASSERT_ALWAYS_NO_LEAVE(AddClientL(aFactoryId, aClientType, &aPlayerId));
	//return iFactoryContainerMap.Insert(aPlayerId, aFactoryId);
	}


