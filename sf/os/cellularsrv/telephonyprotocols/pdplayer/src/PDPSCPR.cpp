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
// PDP SubConnection Provider implementation
// 
//

/**
 @file
 @internalComponent
*/

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PDPSCPRTraces.h"
#endif

#include "PDPSCPRStates.h"
#include "PDPDeftSCPR.h"
#include "PDPSCPR.h"
#include "PDPSCPRFactory.h"
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_msgintercept.h>
#include <comms-infras/agentscprstates.h>
#include <comms-infras/agentscpractivities.h>
#include <comms-infras/ss_nodemessages_factory.h>

#if  defined SYMBIAN_TRACE_ENABLE
#define KPDPSCprTag KESockSubConnectionTag
_LIT8(KPDPSCprSubTag, "pdpscpr");
#endif

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace NetStateMachine;
using namespace Factories;

//-=========================================================
//
// CPDPSubConnectionProvider Activities
//
//-=========================================================

namespace PDPSCprProvisionActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityStoreProvision, provision, TCFDataClient::TProvisionConfig)
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TStoreProvisionAndInitSelf, CoreNetStates::TAwaitingProvision, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PDPSCprStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, PDPScprStart, TCFServiceProvider::TStart, MeshMachine::CNodeRetryActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStart, CoreNetStates::TNoTagOrBearerPresentBlockedByStop)
    NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStartSelf, CoreNetStates::TAwaitingDataClientStarted, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendStarted)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreStates::TPanic)
NODEACTIVITY_END()
}

namespace PDPSCprDataClientStartActivity
{
typedef MeshMachine::TAcceptErrorState<CoreNetStates::TAwaitingDataClientStarted> TAwaitingDataClientStartedOrError;

DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStartDataClient, pdpDataClientStart, TCFDataClient::TStart, PDPSCprStates::CStartActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientStart, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TCreateSecondaryOrMbmsPDPCtx, PDPSCprStates::TAwaitingPDPCtxCreated, PDPSCprStates::TNoTagOrError)
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TSetTFT, PDPSCprStates::TAwaitingTFTSet, PDPSCprStates::TNoTagOrError)
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TSetQoS, PDPSCprStates::TAwaitingQoSSet, PDPSCprStates::TNoTagOrError)
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TActivatePDPContext, PDPSCprStates::TAwaitingPDPContextActive, PDPSCprStates::TNoTagOrError)
    NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TOverrideProvisionAndStartDataClient, TAwaitingDataClientStartedOrError, MeshMachine::TNoTagOrErrorTag)
    //This call below is somewhat obscure, but must be performed for the final activation of a pdp context.
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TGetNegotiatedQoS, PDPSCprStates::TAwaitingNegotiatedQoSRetrieved, PDPSCprStates::TNoTagOrError)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TRaiseParamsGrantedAndSendDataClientStarted)

	// Cleanup if error occurred
    THROUGH_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing, CoreNetStates::TNoTagOrNoDataClientsToStop)
    NODEACTIVITY_ENTRY(KNoTag, SCprStates::TStopYourFlows, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TTag<CoreNetStates::KNoDataClientsToStop>)
    THROUGH_NODEACTIVITY_ENTRY(CoreNetStates::KNoDataClientsToStop, MeshMachine::TDoNothing, PDPSCprStates::TNoTagOrProviderStopped)
    NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TDestroyPDPContext, PDPSCprStates::TAwaitingPDPContextDestroyed, MeshMachine::TTag<CoreNetStates::KProviderStopped>)
    THROUGH_NODEACTIVITY_ENTRY(CoreNetStates::KProviderStopped, PDPSCprStates::TCleanupFSMAndDataClients, MeshMachine::TTag<CoreNetStates::KProviderStopped>)
    LAST_NODEACTIVITY_ENTRY(CoreNetStates::KProviderStopped, MeshMachine::TRaiseActivityError)
NODEACTIVITY_END()
}

namespace PDPSCprStopActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStopDataClient, Stop, TCFDataClient::TStop, MeshMachine::CNodeRetryActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(PDPSCprStates::TAwaitingDataClientStopOrCancel, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, PRDataClientStopActivity::TNoTagOrProviderStoppedBlockedByStart)
    
    // stopping
    NODEACTIVITY_ENTRY(KNoTag, SCprStates::TStopYourFlows, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TTag<CoreNetStates::KProviderStopped>)
	THROUGH_NODEACTIVITY_ENTRY(CoreNetStates::KProviderStopped, MeshMachine::TDoNothing, PDPSCprStates::TNoTagOrProviderStopped)
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TDestroyPDPContext, PDPSCprStates::TAwaitingPDPContextDestroyed, MeshMachine::TTag<CoreNetStates::KProviderStopped>)
	THROUGH_NODEACTIVITY_ENTRY(CoreNetStates::KProviderStopped, PDPSCprStates::TCleanupFSMAndDataClients, MeshMachine::TTag<CoreNetStates::KProviderStopped>)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KProviderStopped, PDPSCprStates::TSendDataClientStopped)
NODEACTIVITY_END()
}

namespace PDPSCprGoneDownActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityGoneDown, goneDown, TPDPMessages::TPDPFSMMessage)
	FIRST_NODEACTIVITY_ENTRY(PDPSCprStates::TAwaitingPDPContextGoneDown, MeshMachine::TActiveOrNoTag<ECFActivityStartDataClient>)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TSendGoneDown, CoreNetStates::TNoTagOrNoDataClientsToStop)
	NODEACTIVITY_ENTRY(KNoTag, SCprStates::TStopYourFlows, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TTag<CoreNetStates::KNoDataClientsToStop>)

	LAST_NODEACTIVITY_ENTRY(KActiveTag, CoreNetStates::TCancelDataClientStart) //MZTODO: This triple should wait for TError sent as a response to TCancel
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KNoDataClientsToStop, MeshMachine::TClearError)
NODEACTIVITY_END()
}

namespace PDPSCprSetParameters
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, paramRequest, TCFScpr::TSetParamsRequest)
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TStoreAndRespondWithCurrentParams, PRStates::TAwaitingParamRequest,  MeshMachine::TNoTag)
#else
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, paramRequest, TCFScpr::TParamsRequest)
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TStoreParamsAndRespond, SCprStates::TAwaitingParamRequest,  MeshMachine::TNoTag)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
NODEACTIVITY_END()
}

namespace PDPSCprApply
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityApplyChanges, pdpApplyReq, TCFScpr::TApplyRequest)
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
  	FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingApplyRequest, MeshMachine::TNoTag)
#else
	FIRST_NODEACTIVITY_ENTRY(SCprStates::TAwaitingApplyRequest, MeshMachine::TNoTag)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TSetTFT, PDPSCprStates::TAwaitingTFTSet, PDPSCprStates::TNoTagOrError)
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TSetQoS, PDPSCprStates::TAwaitingQoSSet, PDPSCprStates::TNoTagOrError)
  	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TModifyActivePDPContext, PDPSCprStates::TAwaitingActivePDPContextModified, PDPSCprStates::TNoTagOrError)
  	LAST_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TRaiseParamsGrantedAndSendApplyResponse)
  	LAST_NODEACTIVITY_ENTRY(KErrorTag, PDPSCprStates::TRaiseParamsRejectedL)
NODEACTIVITY_END()
}

namespace PDPSCprParamsChanged
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, paramsChanged, TPDPMessages::TPDPFSMMessage)
	FIRST_NODEACTIVITY_ENTRY(PDPSCprStates::TAwaitingParamsChanged, PDPSCprStates::TNoTagOrError)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TRaiseParamsChanged)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, PDPSCprStates::TRaiseParamsRejectedL)
NODEACTIVITY_END()
}

namespace PDPSCprContextBlockedUnblockedActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityParamRequest, ctxBlockedUnblocked, TPDPMessages::TPDPFSMMessage)
    FIRST_NODEACTIVITY_ENTRY(PDPSCprStates::TAwaitingContextBlockedOrUnblocked, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TForwardContextBlockedOrUnblockedToDC, PDPSCprStates::TBlockedOrUnblocked)
    LAST_NODEACTIVITY_ENTRY(PDPSCprStates::KBlocked, PDPSCprStates::TSendDataTransferTemporarilyBlocked)
    /**
    This needs to send DataTransferUnblocked progress, but this needs to be discussed with people in finland
    when they return. In old world, KLinkLayerOpen was sent in this case, but KLinkLayerOpen has changed and is misused for
    this purpose. Should be a new, functionallity specific progress .
    */
    LAST_NODEACTIVITY_ENTRY(PDPSCprStates::KUnblocked, PDPSCprStates::TSendDataTransferUnblocked)
NODEACTIVITY_END()
}

namespace PDPSCprStateChangeActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityForwardStateChange, forwardStateChange, TCFMessage::TStateChange)
    //Bin all the progresses received, PDP.SCPR will generate its own ones.
	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MeshMachine::TAwaitingMessageState<TCFMessage::TStateChange>, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PDPSCprAuthenticationActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityAuthentication, PDPSCprAuthentication, TLinkMessage::TAuthenticate)
    NODEACTIVITY_ENTRY(KNoTag, AgentSCprStates::TSendAuthenticateComplete, AgentSCprStates::TAwaitingAuthenticate, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace PDPSCprActivities
{
DEFINE_ACTIVITY_MAP(activityMap)
    ACTIVITY_MAP_ENTRY(PDPSCprProvisionActivity, provision)
	ACTIVITY_MAP_ENTRY(PDPSCprStartActivity, PDPScprStart)
	ACTIVITY_MAP_ENTRY(PDPSCprDataClientStartActivity, pdpDataClientStart)
	ACTIVITY_MAP_ENTRY(PDPSCprSetParameters, paramRequest)
	ACTIVITY_MAP_ENTRY(PDPSCprParamsChanged, paramsChanged)
	ACTIVITY_MAP_ENTRY(PDPSCprContextBlockedUnblockedActivity, ctxBlockedUnblocked)
	ACTIVITY_MAP_ENTRY(PDPSCprApply, pdpApplyReq)
	ACTIVITY_MAP_ENTRY(PDPSCprStopActivity, Stop)
	ACTIVITY_MAP_ENTRY(PDPSCprGoneDownActivity, goneDown)
	ACTIVITY_MAP_ENTRY(PDPSCprStateChangeActivity, forwardStateChange)
	ACTIVITY_MAP_ENTRY(PDPSCprAuthenticationActivity, PDPSCprAuthentication)
ACTIVITY_MAP_END_BASE(SCprActivities, coreSCprActivities)
}

//-=========================================================
//
// CPDPSubConnectionProvider methods
//
//-=========================================================
CPDPSubConnectionProvider* CPDPSubConnectionProvider::NewL(CPDPSubConnectionProviderFactory& aFactory)
/**
Construct a new PDP SubConnection Provider Object

@params aFactory factory that create this object
@param aConnProvider Connection Provider associated with this object
*/
	{
	CPDPSubConnectionProvider* ptr = new (ELeave) CPDPSubConnectionProvider(aFactory);
    CleanupStack::PushL(ptr);
    ptr->ConstructL();
    CleanupStack::Pop();
	return ptr;
	}


CPDPSubConnectionProvider::~CPDPSubConnectionProvider()
    {
    LOG_NODE_DESTROY(KPDPSCprSubTag, CPDPSubConnectionProvider)
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSUBCONNECTIONPROVIDER_DTOR_1, "Destructor");
    __FLOG_CLOSE;
    if (iPdpFsmInterface)
        {
        iPdpFsmInterface->Close();
        }
    }


CPDPSubConnectionProvider::CPDPSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory)
:CCoreSubConnectionProvider(aFactory, PDPSCprActivities::activityMap::Self()),
 iPDPFsmContextId(EInvalidContextId),
 iPdpFsmInterface(NULL),
 iActivityAwaitingResponse(KActivityNull),
 iProvisionFailure(KErrCorrupt)
    {
    LOG_NODE_CREATE1(KPDPSCprSubTag, CPDPSubConnectionProvider, " [factory=%08x]", &aFactory)
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSUBCONNECTIONPROVIDER_CTOR_1, "Constructor");
    __FLOG_OPEN(KCFNodeTag, KPDPSCprSubTag);
    }


CPDPSubConnectionProvider::CPDPSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory,
                           const MeshMachine::TNodeActivityMap& aActivityMap)
:CCoreSubConnectionProvider(aFactory, aActivityMap),
 iPDPFsmContextId(EInvalidContextId),
 iPdpFsmInterface(NULL),
 iActivityAwaitingResponse(KActivityNull),
 iParamsRelease(MPDPParamMapper::KParameterRelInvalid)
    {
    }

void CPDPSubConnectionProvider::ConstructL()
/**
IP SubConnection Provider Second Phase Constructor
*/
	{
	CCoreSubConnectionProvider::ConstructL();
	}

void CPDPSubConnectionProvider::Received(TNodeContextBase& aContext)
    {
    Messages::TNodeSignal::TMessageId noPeerIds[] = {
		TCFFactory::TPeerFoundOrCreated::Id(),
        TCFPeer::TJoinRequest::Id(),
        TCFMessage::TStateChange::Id(),
        Messages::TNodeSignal::TNullMessageId::Id()
        };
    MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

void CPDPSubConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSUBCONNECTIONPROVIDER_RECEIVEDL_1, "Realm Id [%u] Message Id [%u]", aMessage.MessageId().Realm(), aMessage.MessageId().MessageId());
	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);

	PDPSCprStates::TContext ctx(*this, aMessage, aSender, aRecipient);
    CPDPSubConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

void CPDPSubConnectionProvider::Event(TInt aEvent, TInt aParam)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSUBCONNECTIONPROVIDER_EVENT_1, "Event [%d], Param [%d]", aEvent, aParam);
    TPDPMessages::TPDPFSMMessage msg(aEvent, aParam);

	RClientInterface::OpenPostMessageClose(TNodeCtxId(iActivityAwaitingResponse, Id()), Id(), msg);
    iActivityAwaitingResponse = KActivityNull;
    }

void CPDPSubConnectionProvider::LinkUp()
    {
    if (iLinkUps++ == 0)
        {
        iDefaultSCPR->LinkUp();
        }
    }

void CPDPSubConnectionProvider::LinkDown(TInt aCause)
    {
    if (--iLinkUps == 0)
        {
        iDefaultSCPR->LinkDown(aCause);
        }
    }

CSubConQosR99ParamSet* CPDPSubConnectionProvider::GrantedQoS()
    {
	if(static_cast<CSubConnectionProviderBase*>(this)->GetParameterBundle().IsNull())
        {
        return NULL;
        }
    RParameterFamily qosFamily=static_cast<CSubConnectionProviderBase*>(this)->GetParameterBundle().FindFamily(KSubConQoSFamily);
    if (qosFamily.IsNull())
        {
        return NULL;
        }
    CSubConQosR99ParamSet* qosParams = static_cast<CSubConQosR99ParamSet*>(qosFamily.FindParameterSet(
        STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid,KSubConQosR99ParamsType), RParameterFamily::EGranted));
    if (!qosParams)
        {
        qosParams = static_cast<CSubConQosR99ParamSet*>(qosFamily.FindParameterSet(
        STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid,KSubConQosR5ParamsType), RParameterFamily::EGranted));
        }
    ASSERT(qosParams);
    return qosParams;
    }

TInt CPDPSubConnectionProvider::QoSRank()
/**
This method calculates the qos ranking as specified in 3GPP TS 23.107 Annex C.
Magic number usage is deemed justified based on the content of 3GPP TS 23.107.
*/
    {
    CSubConQosR99ParamSet* qosParams = GrantedQoS();
    ASSERT(qosParams);

    switch (qosParams->GetTrafficClass())
        {
        case RPacketQoS::ETrafficClassConversational:
            return 2;
        case RPacketQoS::ETrafficClassStreaming:
            return 3;
        case RPacketQoS::ETrafficClassInteractive:
            {
            switch (qosParams->GetTrafficHandlingPriority())
                {
                case RPacketQoS::ETrafficPriority1:
                    return 1;
                case RPacketQoS::ETrafficPriority2:
                    return 4;
                case RPacketQoS::ETrafficPriority3:
                    return 5;
                default:
                ;
                }
            break;
            }
        case RPacketQoS::ETrafficClassBackground:
            return 6;
        default:
        ;
        }
    return 7;
    }


MFactoryQuery::TMatchResult THighestQoSQuery::Match(TFactoryObjectInfo& aSubConnectionInfo)
	{
	CPDPSubConnectionProvider* sc = static_cast<CPDPSubConnectionProvider*>(aSubConnectionInfo.iInfo.iFactoryObject);
	ASSERT(sc && sc->ControlProvider());
    if (sc->ControlProvider()->RecipientId() == iCtrlProvider &&
        sc->iPDPFsmContextId != CPDPSubConnectionProvider::EInvalidContextId)
        {
        if (NULL == iHighestQoSProvider)
            {
            iHighestQoSProvider = sc;
            }
        else if (sc->QoSRank() > iHighestQoSProvider->QoSRank())
            {
            iHighestQoSProvider = sc;
            }
        }
    return MFactoryQuery::EContinue;
	}

TTFTInfo CPDPSubConnectionProvider::GetTftInfoL(CSubConIPAddressInfoParamSet* aParamSet)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSUBCONNECTIONPROVIDER_GETTFTINFOL_1, "GetTftInfoL");
	/** TODO: What to do if there will be second request before ggsn responce back and first one will go to the granted list
	Assigning of id's is not correct in that case*/

	TTFTInfo tft;
	RPacketContext::TPacketFilterV2 pf;

	TUint aParamSetNum = aParamSet->GetParamNum();
	/** TODO: It should be only one in the queue, so remove the loop */
	for (TUint i=0; i<aParamSetNum; ++i)
		{
	//	TUint8* pfAddr(pf.iSrcAddr);

		CSubConIPAddressInfoParamSet::TSubConIPAddressInfo paramInfo(aParamSet->GetParamInfoL(i));

		TInetAddr addr(paramInfo.iCliDstAddr);
		TUint32 ipv4Addr = addr.Address();

		if(ipv4Addr)
			{
			addr.ConvertToV4Mapped();
			}

		TIp6Addr ipv6 = addr.Ip6Address();
		Mem::Copy(&pf.iSrcAddr,&ipv6,RPacketContext::KIPAddressSize);

		pf.iId = FindPacketFilterIdL(paramInfo);
		Mem::Fill(pf.iSrcAddrSubnetMask, sizeof(pf.iSrcAddrSubnetMask), 0xFF);
		pf.iSrcPortMin = pf.iSrcPortMax = paramInfo.iCliDstAddr.Port();
		pf.iDestPortMin = pf.iDestPortMax = paramInfo.iCliSrcAddr.Port();
		pf.iProtocolNumberOrNextHeader = paramInfo.iProtocolId;

		tft.AddPacketFilter(pf);
		}

	return tft;
	}

TTFTOperationCode CPDPSubConnectionProvider::GetOperationCodeL(CSubConIPAddressInfoParamSet* aParamSet)
	{
	TTFTOperationCode result(0);
	TInt count = aParamSet->GetParamNum();
	if(count > 0)
		{
		CSubConIPAddressInfoParamSet::TSubConIPAddressInfo paramInfo(aParamSet->GetParamInfoL(0));
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSUBCONNECTIONPROVIDER_GETOPERATIONCODEL_1, "Param Info State [%d]", paramInfo.iState);
		switch(paramInfo.iState)
			{
			case CSubConIPAddressInfoParamSet::TSubConIPAddressInfo::EAdd:
					result = KAddFilters;
					break;
			case CSubConIPAddressInfoParamSet::TSubConIPAddressInfo::ERemove:
					result = KRemoveFilters;
					break;
			case CSubConIPAddressInfoParamSet::TSubConIPAddressInfo::ENone:
					if(aParamSet->GetOperationCode() == CSubConIPAddressInfoParamSet::EDelete)
						{
						result = KDeleteTFT;
						}
					break;
			default: ;
				/** TODO: What to do with an error */
			}
		}
	else
		{
		/** TODO: Panic? */
		}

	return result;
	}

TUint CPDPSubConnectionProvider::FindPacketFilterIdL(CSubConIPAddressInfoParamSet::TSubConIPAddressInfo aParamInfo)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSUBCONNECTIONPROVIDER_FINDPACKETFILTERIDL_1, "Param Info State [%d]", aParamInfo.iState);
	TUint id = 0;

	switch(aParamInfo.iState)
		{
		case CSubConIPAddressInfoParamSet::TSubConIPAddressInfo::EAdd :
				{
				TUint i = 0;
				TUint mask = 1;

				// search bitwise for an empty slot
				while(!(mask & iPacketFilterMaskId) && i<8)
					{
					mask <<= 1;
					++i;
					}

				// if not found assigne the first slot
				id = (i>=8) ? 1 : i;
				}
				break;
		case CSubConIPAddressInfoParamSet::TSubConIPAddressInfo::ERemove :
				id = FindIdOfMatchingParamSetL(aParamInfo);
				break;
		case CSubConIPAddressInfoParamSet::TSubConIPAddressInfo::ENone :
				break;
		default: ;
			/** TODO: What to do with an error */
		}

	// increase by one since id is starting from 1
	return id;
	}

TUint CPDPSubConnectionProvider::FindIdOfMatchingParamSetL(CSubConIPAddressInfoParamSet::TSubConIPAddressInfo aParamInfo)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSUBCONNECTIONPROVIDER_FINDIDOFMATCHINGPARAMSETL_1, "Returns Id of Matching Param Set");
	// check the list of the granted params for an index
	if(GetParameterBundle().IsNull())
		{
		return 0;
		}
    RParameterFamily ipAddressInfoFamily=GetParameterBundle().FindFamily(KSubConIPAddressInfoFamily);
	if( ipAddressInfoFamily.IsNull() )
		{
		return 0;
		}

	CSubConIPAddressInfoParamSet* ipAddressInfoSet = 
		static_cast<CSubConIPAddressInfoParamSet*>(
            ipAddressInfoFamily.FindParameterSet(
				STypeId::CreateSTypeId(CSubConIPAddressInfoParamSet::EUid,CSubConIPAddressInfoParamSet::ETypeId),
					RParameterFamily::EGranted));
	if(	ipAddressInfoSet == NULL )
		{
		return 0;
		}
	TUint count = ipAddressInfoSet->GetParamNum();

	TBool found(EFalse);
	TUint i=0;

	for(i=0; i<count && !found; ++i)
		{
		CSubConIPAddressInfoParamSet::TSubConIPAddressInfo paramInfo(ipAddressInfoSet->GetParamInfoL(i));

		found = paramInfo.Compare(aParamInfo);
		}
	// use the index against corresponding array of id's
	return found ? iPacketFilterId[i] : 0;
	}

void CPDPSubConnectionProvider::NewPacketFilterAddedL(CSubConIPAddressInfoParamSet::TSubConIPAddressInfo aParamInfo, TUint aId)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSUBCONNECTIONPROVIDER_NEWPACKETFILTERADDEDL_1, "Adding New Packet Filter Id [%d]", aId);
    RParameterFamily ipAddressInfoFamily = GetParameterBundle().FindFamily(KSubConIPAddressInfoFamily);
    if ( ! ipAddressInfoFamily.IsNull())
        {
        CSubConIPAddressInfoParamSet* grantedIPAddressInfo = CSubConIPAddressInfoParamSet::NewL(ipAddressInfoFamily, RParameterFamily::EGranted);

		if(grantedIPAddressInfo)
			{
			grantedIPAddressInfo->AddParamInfo(aParamInfo);

			User::LeaveIfError(iPacketFilterId.Append(aId));
			iPacketFilterMaskId |= 1 << aId;
			}
		else
			{
			User::Leave(KErrNotFound);
			}
		}
	}

void CPDPSubConnectionProvider::PacketFilterRemovedL(TUint aId)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSUBCONNECTIONPROVIDER_PACKETFILTERREMOVEDL_1, "Removing Packet Filter Id [%d]", aId);
    RParameterFamily ipAddressInfoFamily = GetParameterBundle().FindFamily(KSubConIPAddressInfoFamily);

    if ( ! ipAddressInfoFamily.IsNull())
        {
		// find an index from given id value
		//TUint count = iPacketFilterId.Count();
		TInt index = iPacketFilterId.Find(aId);
		//for (index = 0; (index < count) && (iPacketFilterId[index] != aId); ++index){}

		if(index >= 0)
			{
			CSubConIPAddressInfoParamSet* grantedIPAddressInfo = CSubConIPAddressInfoParamSet::NewL(ipAddressInfoFamily, RParameterFamily::EGranted);

			if(grantedIPAddressInfo)
				{
				grantedIPAddressInfo->RemoveParamInfo(index);

				iPacketFilterId.Remove(index);
				iPacketFilterMaskId &= !(1 << aId);
				}
			else
				{
				User::Leave(KErrNotFound);
				}
			}
		else
			{
			User::Leave(KErrNotFound);
			}
		}
	}
