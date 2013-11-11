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
// RawIP MCPR
// 
//

/**
 @file
 @internalComponent
*/

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "rawipmcprTraces.h"
#endif

#include <comms-infras/ss_log.h>
#include "rawipmcpr.h"
#include <comms-infras/linkprovision.h>
#include "RawIpAgentHandler.h"
#include <comms-infras/agentmcpractivities.h>
#include <comms-infras/coremcpractivities.h>
#include <comms-infras/ss_msgintercept.h>

using namespace ESock;
using namespace NetStateMachine;
using namespace MCprActivities;
using namespace Messages;
using namespace MeshMachine;


// No Bearer Activity
namespace RawIPMCPRNoBearerActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityNoBearer, activity, TCFControlProvider::TNoBearer)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingNoBearer, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBearer)
NODEACTIVITY_END()
}

// Activity Map
namespace RawIPMCprStates
{
DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
   ACTIVITY_MAP_ENTRY(RawIPMCPRNoBearerActivity, activity)
ACTIVITY_MAP_END_BASE(AgentMCprActivities, agentMCprActivities)
} // namespace RawIPMCprStates

//-=========================================================
//
//CRawIpMetaConnectionProvider implementation
//
//-=========================================================

CRawIpMetaConnectionProvider* CRawIpMetaConnectionProvider::NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const TProviderInfo& aProviderInfo)
    {
    // coverity[alloc_fn] coverity[alias] coverity[assign]
    CRawIpMetaConnectionProvider* self = new (ELeave) CRawIpMetaConnectionProvider(aFactory, aProviderInfo);
    // coverity[push]
    CleanupStack::PushL(self);
    // coverity[alias] coverity[double_push]
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CRawIpMetaConnectionProvider::ConstructL()
	{
	CAgentMetaConnectionProvider::ConstructL();
	SetAccessPointConfigFromDbL();
	}

CRawIpMetaConnectionProvider::CRawIpMetaConnectionProvider(CMetaConnectionProviderFactoryBase& aFactory, const TProviderInfo& aProviderInfo)
:	CAgentMetaConnectionProvider(aFactory, aProviderInfo, RawIPMCprStates::stateMap::Self())
	{
	LOG_NODE_CREATE(KRawIPMCprTag, CRawIpMetaConnectionProvider);
	}

CRawIpMetaConnectionProvider::~CRawIpMetaConnectionProvider()
	{
	// Cleanup up Agent Notification Handler
	SetAgentNotificationHandlerL(NULL);
	delete iAgentHandler;

	LOG_NODE_DESTROY(KRawIPMCprTag, CRawIpMetaConnectionProvider);
	}

void CRawIpMetaConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPMETACONNECTIONPROVIDER_RECEIVEDL_1, "CRawIpMetaConnectionProvider [this=%08x]::ReceivedL() aCFMessage=%u",(TUint)this, aMessage.MessageId().MessageId());

	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);

	TNodeContext<CRawIpMetaConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreMetaConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

void CRawIpMetaConnectionProvider::SetAccessPointConfigFromDbL()
/**
Setup provisioning information
*/
    {
    RMetaExtensionContainer mec;
    mec.Open(AccessPointConfig());
    CleanupClosePushL(mec);
    
	CCommsDatIapView* iapView = OpenIapViewLC();

	// Presumptions:
	// - none of the extensions can already exist in the AccessPointConfig array.  AppendExtensionL()
	//   is presumed to panic if adding the same extension a second time.
	// - if we have added several extensions to the AccessPointConfig array before getting a failure
	//   and leaving, it is presumed that the MCPr will be destroyed and AccessPointConfig destructor
	//   will clean up the extensions immediately afterwards.

	mec.AppendExtensionL(CIPConfig::NewFromGPRSOutLC(iapView));
	CleanupStack::Pop(); //Ownership with the list

	mec.AppendExtensionL(CBCAProvision::NewLC(iapView));
	CleanupStack::Pop(); //Ownership with the list

	CleanupStack::PopAndDestroy();			// CloseIapView();

	iAccessPointConfig.Close();
	iAccessPointConfig.Open(mec);
	CleanupStack::PopAndDestroy(&mec);

	// Setup RawIp Agent Notification handler in order to retrieve TSY and connection speed
	// information from the Agent once ConnectComplete() occurs.
	ASSERT(iAgentHandler == NULL);
	iAgentHandler = new (ELeave) CRawIpAgentHandler;
	SetAgentNotificationHandlerL(iAgentHandler);	// ownership NOT transferred
	}

