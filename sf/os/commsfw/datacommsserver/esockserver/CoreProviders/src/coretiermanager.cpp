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

#include "coretiermanager.h"
#include <comms-infras/coretiermanagerstates.h>

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include <elements/nm_messages_peer.h>
#include <comms-infras/ss_nodemessages_serviceprovider.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_metaconnprov.h>

#if defined(__CFLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)
	_LIT8(KCoreTMgrSubTag, "coretiermgr");
#endif

using namespace MeshMachine;
using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;

//
//CCoreTierManager
EXPORT_C CCoreTierManager::CCoreTierManager(CTierManagerFactoryBase& aFactory,
                                            const MeshMachine::TNodeActivityMap& aActivityMap)
:	CTierManagerBase(aFactory,aActivityMap)
	{
	LOG_NODE_CREATE(KCoreTMgrSubTag, CCoreTierManager);
	}

EXPORT_C CCoreTierManager::~CCoreTierManager()
	{
    LOG_NODE_DESTROY(KCoreTMgrSubTag, CCoreTierManager);
	}

EXPORT_C void CCoreTierManager::Received(TNodeContextBase& aContext)
    {
	Messages::TNodeSignal::TMessageId noPeerIds[] = {
	    TCFSelector::TSelect::Id(),
	    TCFFactory::TPeerFoundOrCreated::Id(),
		TCFPeer::TJoinRequest::Id(),
	    Messages::TNodeSignal::TMessageId()
	    };
	MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

EXPORT_C TBool CCoreTierManager::HandleContentionL(CMetaConnectionProviderBase* /*aMcpr*/, TNodeId& /*aPendingCprId*/, TUint /*aPriority*/)
	{
	// Should be overridden for TM that supports Contention Resolving
	return EFalse;
	}

EXPORT_C TBool CCoreTierManager::IsUnavailableDueToContention(const ESock::CMetaConnectionProviderBase* /*aMetaConnectionProvider*/) const
	{
	// Should be overridden for TM that supports Contention Resolving
	return EFalse;
	}



