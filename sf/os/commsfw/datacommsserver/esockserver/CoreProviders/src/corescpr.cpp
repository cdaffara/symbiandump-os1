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
// Core SCPR Implementation
// 
//

/**
 @file
 @internalComponent
*/

#include "corescpr.h"
#include <comms-infras/corescprstates.h>
#include <comms-infras/corescpractivities.h>

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <elements/sm_statetriple.h>
#include <elements/nm_signatures.h>

#if defined(__CFLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)
	_LIT8(KCoreSCPRSubTag, "corescpr");
#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;

EXPORT_C CCoreSubConnectionProvider::CCoreSubConnectionProvider(CSubConnectionProviderFactoryBase& aFactory,
                                                                const MeshMachine::TNodeActivityMap& aActivityMap)
:CSubConnectionProviderBase(aFactory,aActivityMap)
	{
	LOG_NODE_CREATE(KCoreSCPRSubTag, CCoreSubConnectionProvider);
	}

EXPORT_C CCoreSubConnectionProvider::CCoreSubConnectionProvider(CSubConnectionProviderFactoryBase& aFactory)
:CSubConnectionProviderBase(aFactory,SCprActivities::coreSCprActivities::Self())
	{
	LOG_NODE_CREATE(KCoreSCPRSubTag, CCoreSubConnectionProvider);
	}

EXPORT_C CCoreSubConnectionProvider* CCoreSubConnectionProvider::NewL(CSubConnectionProviderFactoryBase& aFactory)
	{
    CCoreSubConnectionProvider* provider = new (ELeave) CCoreSubConnectionProvider(aFactory);
    CleanupStack::PushL(provider);
    provider->ConstructL();
    CleanupStack::Pop();
    return provider;
	}

EXPORT_C CCoreSubConnectionProvider::~CCoreSubConnectionProvider()
	{
	LOG_NODE_DESTROY(KCoreSCPRSubTag, CCoreSubConnectionProvider);
	}

EXPORT_C void CCoreSubConnectionProvider::Received(TNodeContextBase& aContext)
    {
    Messages::TNodeSignal::TMessageId noPeerIds[] = {
        TCFFactory::TPeerFoundOrCreated::Id(),
        TCFPeer::TJoinRequest::Id(),
        Messages::TNodeSignal::TMessageId()
        };
    MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

EXPORT_C void CCoreSubConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	TNodeContext<CCoreSubConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreSubConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}



