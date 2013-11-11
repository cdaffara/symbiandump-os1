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
// Core CPR Implementation
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/corecpr.h>
#include <comms-infras/corecprstates.h>
#include <comms-infras/corecpractivities.h>

#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <elements/sm_statetriple.h>
#include <elements/nm_signatures.h>

_LIT(KPanicCategory, "corecpr");

#if defined(__CFLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)
_LIT8(KCoreCprSubTag, "corecpr");
#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;


EXPORT_C CCoreConnectionProvider::CCoreConnectionProvider(CConnectionProviderFactoryBase& aFactory,
                                                          const MeshMachine::TNodeActivityMap& aActivityMap)
:	CConnectionProviderBase(aFactory,aActivityMap)
	{
	LOG_NODE_CREATE(KCoreCprSubTag, CCoreConnectionProvider);
	}

EXPORT_C CCoreConnectionProvider* CCoreConnectionProvider::NewL(CConnectionProviderFactoryBase& aFactory)
	{
    CCoreConnectionProvider* provider = new (ELeave) CCoreConnectionProvider(aFactory,CprActivities::coreCprActivities::Self());
    CleanupStack::PushL(provider);
    provider->ConstructL();
    CleanupStack::Pop(provider);
    return provider;
	}

EXPORT_C CCoreConnectionProvider::~CCoreConnectionProvider()
	{
	LOG_NODE_DESTROY(KCoreCprSubTag, CCoreConnectionProvider);
	}


EXPORT_C void CCoreConnectionProvider::Received(TNodeContextBase& aContext)
    {
    Messages::TNodeSignal::TMessageId noPeerIds[] = {
        TCFFactory::TPeerFoundOrCreated::Id(),
        TCFPeer::TJoinRequest::Id(),
		TEBase::TError::Id(),
        Messages::TNodeSignal::TMessageId()
        };
    MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

EXPORT_C void CCoreConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	TNodeContext<CCoreConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}


TUint CCoreConnectionProvider::Priority() const
	{
	TUint currentPriority = KMaxTUint;

	const TAccessPointPriority* apPriorityExt = static_cast<const TAccessPointPriority*>(AccessPointConfig().FindExtension(
			STypeId::CreateSTypeId(TAccessPointPriority::EUid, TAccessPointPriority::ETypeId)));
	__ASSERT_DEBUG(apPriorityExt, User::Panic(KPanicCategory, CorePanics::KPanicUnexpectedExecutionPath));

	TClientIter<TDefaultClientMatchPolicy> iter = GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl));
	RCFNodePriorityInterface* ctrlClient = static_cast<RCFNodePriorityInterface*>(iter++);
	
	while (ctrlClient)
		{
		if (ctrlClient->Priority() < currentPriority)
			{
			currentPriority = ctrlClient->Priority();
			}
		ctrlClient = static_cast<RCFNodePriorityInterface*>(iter++);
		}
	
	if (apPriorityExt && apPriorityExt->Priority() < currentPriority)
		{
		currentPriority = apPriorityExt->Priority();
		}
	
	return currentPriority;
	}


