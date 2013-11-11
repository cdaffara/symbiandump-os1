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

#include <comms-infras/coremcpr.h>
#include <comms-infras/coremcprstates.h>
#include <comms-infras/coremcpractivities.h>

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <elements/sm_statetriple.h>
#include <comms-infras/ss_nodemessages_serviceprovider.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <elements/nm_messages_errorrecovery.h>

#if defined(__CFLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)
	_LIT8(KCoreMCprSubTag, "coremcpr");
#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;


//
//CCoreMetaConnectionProvider
EXPORT_C CCoreMetaConnectionProvider::CCoreMetaConnectionProvider(CMetaConnectionProviderFactoryBase& aFactory,
                                                                  const TProviderInfo& aProviderInfo,
                                                                  const MeshMachine::TNodeActivityMap& aActivityMap)
:	CMetaConnectionProviderBase(aFactory,aProviderInfo,aActivityMap),
	iIsSelectionCompleted(EFalse)
	{
	LOG_NODE_CREATE(KCoreMCprSubTag, CMetaConnectionProviderBase);
	}


EXPORT_C CCoreMetaConnectionProvider::~CCoreMetaConnectionProvider()
	{
	LOG_NODE_DESTROY(KCoreMCprSubTag, CMetaConnectionProviderBase);
	}

EXPORT_C void CCoreMetaConnectionProvider::Received(TNodeContextBase& aContext)
    {
    Messages::TNodeSignal::TMessageId noPeerIds[] = {
        TCFFactory::TPeerFoundOrCreated::Id(),
        TCFPeer::TJoinRequest::Id(),
		TEErrorRecovery::TErrorRecoveryRequest::Id(), //May be comming from SCpr or Cpr
												//In the first case we do not want the Cpr to forward
												//this message "as the originator" but only to forward
												//it "on behalf of" the SCpr. This allows the Error
												//Recovery Activities to have the very originator in
												//their originators list.
		TEBase::TError::Id(),
        Messages::TNodeSignal::TMessageId()
        };
    MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

EXPORT_C void CCoreMetaConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	TNodeContext<CCoreMetaConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreMetaConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

//Default implementation, usually empty on the mid-tier MCprs
EXPORT_C void CCoreMetaConnectionProvider::StartAvailabilityMonitoringL(const TNodeCtxId& /*aAvailabilityActivity*/)
	{
	}

//Default implementation, usually empty on the mid-tier MCprs
EXPORT_C void CCoreMetaConnectionProvider::CancelAvailabilityMonitoring()
	{
	}



