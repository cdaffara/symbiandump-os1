/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include <comms-infras/ss_log.h>
#include <in_sock.h>
#include <comms-infras/metadata.h>
#include <comms-infras/coremcpractivities.h>
#include <comms-infras/agentmcpractivities.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include "mcpr.h"
#include "provision.h"
#include "agent_handler.h"
#include <comms-infras/ss_msgintercept.h>

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace MCprActivities;
using namespace LegacyLoopbackMCprStates;

// No Bearer Activity
namespace LegacyLoopbackMCPRNoBearerActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityNoBearer, LegacyLoopbackMCPRNoBearer, TCFControlProvider::TNoBearer)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBearer, CoreNetStates::TAwaitingNoBearer, MeshMachine::TNoTag)	
NODEACTIVITY_END()
}


// Activity Map
namespace LegacyLoopbackMCprStates
{
DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
   ACTIVITY_MAP_ENTRY(LegacyLoopbackMCPRNoBearerActivity, LegacyLoopbackMCPRNoBearer)
ACTIVITY_MAP_END_BASE(AgentMCprActivities, agentMCprActivities)
} // namespace LegacyLoopbackMCprStates


#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
    _LIT8(KLegacyLoopbackMCprSubTag, "legacy_loopback");
#endif

//-=========================================================
//
// CLegacyLoopbackMetaConnectionProvider implementation
//
//-=========================================================

CLegacyLoopbackMetaConnectionProvider* CLegacyLoopbackMetaConnectionProvider::NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo)
    {
    CLegacyLoopbackMetaConnectionProvider* self = new (ELeave) CLegacyLoopbackMetaConnectionProvider(aFactory, aProviderInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CLegacyLoopbackMetaConnectionProvider::CLegacyLoopbackMetaConnectionProvider(CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo)
:	CAgentMetaConnectionProvider(aFactory, aProviderInfo, LegacyLoopbackMCprStates::stateMap::Self())
	{
	LOG_NODE_CREATE(KLegacyLoopbackMCprSubTag, CLegacyLoopbackMetaConnectionProvider);
	}

CLegacyLoopbackMetaConnectionProvider::~CLegacyLoopbackMetaConnectionProvider()
	{
	// Assumption is that CLegacyLoopbackProvision will be cleaned up by ~CAccessPointConfig

	// Clean up Agent Notification Handler
    SetAgentNotificationHandlerL(NULL);
    delete iAgentHandler;

	LOG_NODE_DESTROY(KLegacyLoopbackMCprSubTag, CLegacyLoopbackMetaConnectionProvider);
	}


void CLegacyLoopbackMetaConnectionProvider::ConstructL()
    {
    CAgentMetaConnectionProvider::ConstructL();
	SetAccessPointConfigFromDbL();
	}


void CLegacyLoopbackMetaConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	__CFLOG_VAR((KDummyProtoMCprTag, KLegacyLoopbackMCprSubTag, _L8("CLegacyLoopbackMetaConnectionProvider [this=%08x]::ReceivedL() aMessage=%d"),
	   this, aMessage.MessageId()));

	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);

	TNodeContext<CLegacyLoopbackMetaConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreMetaConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

void CLegacyLoopbackMetaConnectionProvider::SetAccessPointConfigFromDbL()
    {
	RMetaExtensionContainer mec;
	mec.Open(AccessPointConfig());
	CleanupClosePushL(mec);

    // Add provisioning information.
   	CCommsDatIapView* iapView = OpenIapViewLC();
	CLegacyLoopbackProvision* provision = new (ELeave) CLegacyLoopbackProvision();
	CleanupStack::PushL(provision);
	provision->InitialiseConfigL(iapView);

	// Presumptions:
	// - none of the extensions can already exist in the AccessPointConfig array.  AppendExtensionL()
	//   is presumed to panic if adding the same extension a second time.
	// - if we have added several extensions to the AccessPointConfig array before getting a failure
	//   and leaving, it is presumed that the MCPr will be destroyed and AccessPointConfig destructor
	//   will clean up the extensions immediately afterwards.

	mec.AppendExtensionL(provision);
	CleanupStack::Pop(provision);			// ownership (including cleanup) transferred to AccessPointConfig()
	CleanupStack::PopAndDestroy();			// CloseIapView()

	AccessPointConfig().Close();
	AccessPointConfig().Open(mec);
	CleanupStack::PopAndDestroy(&mec);

	ASSERT(iAgentHandler == NULL);
	// Register the agent notification handler
    iAgentHandler = new (ELeave) CLegacyLoopbackProtoAgentHandler();
    SetAgentNotificationHandlerL(iAgentHandler);	// ownership NOT transferred
    }
