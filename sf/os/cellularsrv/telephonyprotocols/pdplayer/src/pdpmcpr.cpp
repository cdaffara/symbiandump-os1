// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Pdp MCPR
// 
//

/**
 @file
 @internalComponent
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "pdpmcprTraces.h"
#endif


#include <etelqos.h>
#include <comms-infras/agentmcpractivities.h>
#include <comms-infras/coremcpractivities.h>
#include <comms-infras/ss_msgintercept.h>
#include <elements/nm_messages_errorrecovery.h>
#include <comms-infras/ss_log.h>
#include "pdpmcpr.h"
#include "pdpmcprstates.h"
#include "PDPProvision.h"
#include "psdavailabilitylistener.h"
#include "pdp.hrh"

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace NetStateMachine;
using namespace MCprActivities;


// No Bearer Activity
namespace PdpMCPRNoBearerActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityNoBearer, activity, TCFControlProvider::TNoBearer)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingNoBearer, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendBearer)
NODEACTIVITY_END()
}

namespace PdpMCprErrorRecoveryActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityErrorRecovery, PdpMCprErrorRecovery, TEErrorRecovery::TErrorRecoveryRequest, CPdpErrorRecoveryActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(MCprStates::TAwaitingErrorRecoveryRequest, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CPdpErrorRecoveryActivity::TProcessErrorRecoveryReq, CPdpErrorRecoveryActivity::TPdpContentionNoTagOrError)
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, CPdpErrorRecoveryActivity::TAwaitingContentionResult, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CPdpErrorRecoveryActivity::TProcessContentionResult)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

// Activity Map
namespace PdpMCprStates
{
DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
	ACTIVITY_MAP_ENTRY(PdpMCPRNoBearerActivity, activity)
	ACTIVITY_MAP_ENTRY(PdpMCprErrorRecoveryActivity, PdpMCprErrorRecovery)
ACTIVITY_MAP_END_BASE(AgentMCprActivities, agentMCprActivities)
} // namespace PdpMCprStates

namespace PdpMCprErrorRecoveryActivity
{
CNodeActivityBase* CPdpErrorRecoveryActivity::NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
	{
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
	return new(ELeave)CPdpErrorRecoveryActivity(aActivitySig, aNode, c);
	}

CPdpErrorRecoveryActivity::CPdpErrorRecoveryActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount)
:	MeshMachine::CNodeParallelActivityBase(aActivitySig, aNode, aActivitiesCount)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPERRORRECOVERYACTIVITY_CONSTRUCTOR_1, "Constructor");
	}
	
CPdpErrorRecoveryActivity::~CPdpErrorRecoveryActivity()
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPERRORRECOVERYACTIVITY_DESTRUCTOR_1, "Destructor");
	}
}

//-=========================================================
//
//CPdpMetaConnectionProvider implementation
//
//-=========================================================

CPdpMetaConnectionProvider* CPdpMetaConnectionProvider::NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const TProviderInfo& aProviderInfo)
    {
    // coverity[alloc_fn] coverity[alias] coverity[assign]
    CPdpMetaConnectionProvider* provider = new (ELeave) CPdpMetaConnectionProvider(aFactory, aProviderInfo);
    // coverity[push]
    CleanupStack::PushL(provider);
    // coverity[alias] coverity[double_push]
    provider->ConstructL();
    CleanupStack::Pop(provider);
    return provider;
    }

void CPdpMetaConnectionProvider::ConstructL()
    {
    CAgentMetaConnectionProvider::ConstructL();
    
    const TLayerConfig* layerCfg = static_cast<const TLayerConfig*>(AccessPointConfig().FindExtension(
        STypeId::CreateSTypeId(TLayerConfig::EUid,TLayerConfig::ETypeId)));

    // defensive programming.  If the layer cfg isn't here
    // then this is really corrupt.
    
    if (layerCfg == NULL)
        {
        User::Leave(KErrCorrupt);
        }
    
    // these constants and all of the implementation ids should be in a header file
    // and included in the .rss / ecom files.
    
    // Any layer that uses an outbound PDP.SCPR will have information that is overwritten by
    // the network (for example, negotiated parameters), this configuration information is 
    // local to the connection and should not be created here.  So this check is to ensure 
    // that legacy behaviour is maintained for any layer that uses this MCPR but not a 
    // PDP.SCPR.  But the PDP.SCPR behaviour has been modified to create the information
    // that is local to that connection.
    
    if (layerCfg->SCprUid().iUid == PDPScprImplUid)
        {
        // the PDP layer MCPR is being used in conjunction with a PDP.SCPR
        // in this case we can rightly assume that the PDP.SCPR will create
        // the access point config for that connection.
        // code from the PDP.SCPR will need to be moved into the PDP.CPR when
        // that component becomes available for use.
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPMETACONNECTIONPROVIDER_CONSTRUCTL_1, "Configuring PDP SCPR specific information");
        ConfigurePDPLayerL();
        }
    else
        {
        // non-PDP.SCPR specific behaviour
        // this is legacy for any layers that are constructed expecting the previous behaviour
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPMETACONNECTIONPROVIDER_CONSTRUCTL_2, "Non-PDP SCPR used...Configuring Non-PDP SCPR specific information");
        SetAccessPointConfigFromDbL();
        }
    }

CPdpMetaConnectionProvider::CPdpMetaConnectionProvider(CMetaConnectionProviderFactoryBase& aFactory, const TProviderInfo& aProviderInfo)
:	CAgentMetaConnectionProvider(aFactory, aProviderInfo, PdpMCprStates::stateMap::Self())
	{
	LOG_NODE_CREATE(KPdpMCprSubTag, CPdpMetaConnectionProvider);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPMETACONNECTIONPROVIDER_CONSTRUCTOR_1, "Constructor");
	}

CPdpMetaConnectionProvider::~CPdpMetaConnectionProvider()
	{
	CPdpMetaConnectionProvider::CancelAvailabilityMonitoring(); //Don't call virtual for obvious reasons!
	LOG_NODE_DESTROY(KPdpMCprSubTag, CPdpMetaConnectionProvider);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPMETACONNECTIONPROVIDER_DESTRUCTOR_1, "Destructor");
	}

void CPdpMetaConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPMETACONNECTIONPROVIDER_RECEIVEDL_1, "Realm Id [%u] Message Id [%u]", aMessage.MessageId().Realm(), aMessage.MessageId().MessageId());

	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);

	TNodeContext<CPdpMetaConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreMetaConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}


void CPdpMetaConnectionProvider::ConfigurePDPLayerL()
    {
    RMetaExtensionContainer mec;
    mec.Open(AccessPointConfig());
    CleanupClosePushL(mec);
    
    CCommsDatIapView* iapView = OpenIapViewLC();


    mec.AppendExtensionL(CTSYProvision::NewLC(iapView));
    CleanupStack::Pop();

    mec.AppendExtensionL(CPppLcpConfig::NewLC(iapView));
    CleanupStack::Pop();
    mec.AppendExtensionL(CPppAuthConfig::NewLC(iapView));
    CleanupStack::Pop();

    CPppProvisionInfo* agentProvision = new (ELeave) CPppProvisionInfo;
    CleanupStack::PushL(agentProvision);
    agentProvision->SetIsDialIn(KErrNotSupported);
    agentProvision->SetNotificationData(NULL);
    mec.AppendExtensionL(agentProvision);
    CleanupStack::Pop(agentProvision);

    CleanupStack::PopAndDestroy();          // CloseIapView();

    iAccessPointConfig.Close();
    iAccessPointConfig.Open(mec);
    CleanupStack::PopAndDestroy(&mec);
    }

void CPdpMetaConnectionProvider::SetAccessPointConfigFromDbL()
    {
    RMetaExtensionContainer mec;
    mec.Open(AccessPointConfig());
    CleanupClosePushL(mec);
    
	CCommsDatIapView* iapView = OpenIapViewLC();

	mec.AppendExtensionL(CIPConfig::NewFromGPRSOutLC(iapView));
	CleanupStack::Pop();
	mec.AppendExtensionL(CBCAProvision::NewLC(iapView));
	CleanupStack::Pop();
   	mec.AppendExtensionL(CTSYProvision::NewLC(iapView));
   	CleanupStack::Pop();
	mec.AppendExtensionL(CImsExtProvision::NewLC(iapView));
	CleanupStack::Pop();
	
	CGPRSProvision* gprsProvision = CGPRSProvision::NewLC(iapView);
	mec.AppendExtensionL(gprsProvision);
	CleanupStack::Pop(gprsProvision);
	
	//It's not legal for the qos defaults to be absent.
	CDefaultPacketQoSProvision* defaultQoS = NULL;
	TRAPD(ret, defaultQoS = CDefaultPacketQoSProvision::NewL(iapView));	  
    if ((KErrNone == ret) && defaultQoS)           
        {
        CleanupStack::PushL(defaultQoS);
        mec.AppendExtensionL(defaultQoS);
        CleanupStack::Pop(defaultQoS);
        }
    else
        {
        if (KErrNoMemory == ret)
            {
            User::Leave(KErrNoMemory);
            }
        else
            {
            User::Leave(KErrCorrupt);
            }          
        }    
	
	CRawIpAgentConfig* rawIpAgentConfig = CRawIpAgentConfig::NewLC(iapView, &gprsProvision->GetScratchContextAs<TPacketDataConfigBase>());
	mec.AppendExtensionL(rawIpAgentConfig);
	CleanupStack::Pop();

    mec.AppendExtensionL(CPppLcpConfig::NewLC(iapView));
	CleanupStack::Pop();
	mec.AppendExtensionL(CPppAuthConfig::NewLC(iapView));
	CleanupStack::Pop();

    CPppProvisionInfo* agentProvision = new (ELeave) CPppProvisionInfo;
    CleanupStack::PushL(agentProvision);
    agentProvision->SetIsDialIn(KErrNotSupported);
    agentProvision->SetNotificationData(NULL);
    mec.AppendExtensionL(agentProvision);
    CleanupStack::Pop(agentProvision);

	CleanupStack::PopAndDestroy();			// CloseIapView();

	iAccessPointConfig.Close();
	iAccessPointConfig.Open(mec);
	CleanupStack::PopAndDestroy(&mec);
	}

void CPdpMetaConnectionProvider::StartAvailabilityMonitoringL(const TNodeCtxId& aAvailabilityActivity)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPMETACONNECTIONPROVIDER_STARTAVAILABILITYMONITORINGL_1, "Start Monitoring PSD Availability");
	ASSERT(iAvailabilityListener==NULL); //Only one start allowed from the av activity!
	const CTSYProvision& config = static_cast<const CTSYProvision&>(AccessPointConfig().FindExtensionL(STypeId::CreateSTypeId(CTSYProvision::EUid, CTSYProvision::ETypeId)));
	iAvailabilityListener = CPsdAvailabilityListener::NewL(aAvailabilityActivity, config, ProviderInfo().APId(), GetTierManager()->IsUnavailableDueToContention(this));
	AddClientL(iAvailabilityListener->Id(), TClientType(TCFClientType::ERegistrar, TCFClientType::EAvailabilityProvider));
	}

void CPdpMetaConnectionProvider::CancelAvailabilityMonitoring()
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPMETACONNECTIONPROVIDER_CANCELAVAILABILITYMONITORINGL_1, "Cancel Monitoring PSD Availability");
	if (iAvailabilityListener)
		{
		RemoveClient(iAvailabilityListener->Id());
		iAvailabilityListener = NULL; //iAvailabilityListener will delete itself when cancelled from the availability activity
		}
	}
	
#ifdef SYMBIAN_NETWORKING_CONTENTION_MANAGEMENT
void CPdpMetaConnectionProvider::ContentionOccured()
	{
	// Send a stop request to the stop activity.
	TInt stopCode = KErrConnectionContention;
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPMETACONNECTIONPROVIDER_CONTENTIONOCCURED_1, "Contention Occured: Error Code [%d]", stopCode);
	RClientInterface::OpenPostMessageClose(NodeId(), NodeId(), TCFDataClient::TStop(stopCode).CRef());
	}

void CPdpMetaConnectionProvider::ContentionResolved(const Messages::TNodeId& aPendingCprId, TBool aResult)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPMETACONNECTIONPROVIDER_CONTENTIONRESOLVED_1, "Contention Resolved: Result [%d]", aResult);
	TPDPMessages::TPdpContentionResultMessage msg(aPendingCprId, aResult);
	TRAP_IGNORE(ReceivedL(TNodeId(Id()), TNodeId(Id()), msg));
	}


void CPdpMetaConnectionProvider::ReportContentionAvailabilityStatus(const TAvailabilityStatus& aAvailabilityStatus) const
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPMETACONNECTIONPROVIDER_CONTENTIONAVAILABILITYSTATUS_1, "Contention Availability Status [%d]", aAvailabilityStatus.Score());
    if (iAvailabilityListener)
		{
		iAvailabilityListener->ReportContentionAvailabilityStatusIfRequired(aAvailabilityStatus);
		}
	}
#endif

