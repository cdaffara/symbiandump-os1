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
using namespace ZeroCopyLoopbackMCprStates;

// No Bearer Activity
namespace ZeroCopyLoopbackMCPRNoBearerActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityNoBearer, ZeroCopyLoopbackMCPRNoBearer, TCFControlProvider::TNoBearer)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBearer, CoreNetStates::TAwaitingNoBearer, MeshMachine::TNoTag)	
NODEACTIVITY_END()
}


// Activity Map
namespace ZeroCopyLoopbackMCprStates
{
DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
   ACTIVITY_MAP_ENTRY(ZeroCopyLoopbackMCPRNoBearerActivity, ZeroCopyLoopbackMCPRNoBearer)
ACTIVITY_MAP_END_BASE(AgentMCprActivities, agentMCprActivities)
} // namespace ZeroCopyLoopbackMCprStates


#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
    _LIT8(KZeroCopyLoopbackMCprSubTag, "zerocopy_loopback");
#endif

//-=========================================================
//
// CZeroCopyLoopbackMetaConnectionProvider implementation
//
//-=========================================================

CZeroCopyLoopbackMetaConnectionProvider* CZeroCopyLoopbackMetaConnectionProvider::NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo)
    {
    CZeroCopyLoopbackMetaConnectionProvider* self = new (ELeave) CZeroCopyLoopbackMetaConnectionProvider(aFactory, aProviderInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CZeroCopyLoopbackMetaConnectionProvider::CZeroCopyLoopbackMetaConnectionProvider(CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo)
:	CAgentMetaConnectionProvider(aFactory, aProviderInfo, ZeroCopyLoopbackMCprStates::stateMap::Self())
	{
	LOG_NODE_CREATE(KZeroCopyLoopbackMCprSubTag, CZeroCopyLoopbackMetaConnectionProvider);
	}

CZeroCopyLoopbackMetaConnectionProvider::~CZeroCopyLoopbackMetaConnectionProvider()
	{
	// Assumption is that CZeroCopyLoopbackProvision will be cleaned up by ~CAccessPointConfig

	// Clean up Agent Notification Handler
    SetAgentNotificationHandlerL(NULL);
    delete iAgentHandler;

	LOG_NODE_DESTROY(KZeroCopyLoopbackMCprSubTag, CZeroCopyLoopbackMetaConnectionProvider);
	}


void CZeroCopyLoopbackMetaConnectionProvider::ConstructL()
    {
    CAgentMetaConnectionProvider::ConstructL();
	SetAccessPointConfigFromDbL();
	}


void CZeroCopyLoopbackMetaConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	__CFLOG_VAR((KDummyProtoMCprTag, KZeroCopyLoopbackMCprSubTag, _L8("CZeroCopyLoopbackMetaConnectionProvider [this=%08x]::ReceivedL() aMessage=%d"),
	   this, aMessage.MessageId()));

	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);

	TNodeContext<CZeroCopyLoopbackMetaConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreMetaConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

void CZeroCopyLoopbackMetaConnectionProvider::SetAccessPointConfigFromDbL()
    {
	RMetaExtensionContainer mec;
	mec.Open(AccessPointConfig());
	CleanupClosePushL(mec);

    // Add provisioning information.
   	CCommsDatIapView* iapView = OpenIapViewLC();
	CZeroCopyLoopbackProvision* provision = new (ELeave) CZeroCopyLoopbackProvision();
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
    iAgentHandler = new (ELeave) CZeroCopyLoopbackProtoAgentHandler();
    SetAgentNotificationHandlerL(iAgentHandler);	// ownership NOT transferred
    }
