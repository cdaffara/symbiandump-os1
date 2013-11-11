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
#include "PDPDeftSCPRTraces.h"
#endif

#include <comms-infras/corescpractivities.h>
#include "PDPDeftSCPR.h"
#include "PDPSCPRStates.h"
#include "PDPSCPRFactory.h"
#include <comms-infras/ss_log.h>
#include <comms-infras/commsdebugutility.h>
#include <elements/nm_signatures.h>

#if defined(SYMBIAN_TRACE_ENABLE)
#define KPDPSCprTag KESockSubConnectionTag
_LIT8(KPDPSCprSubTag, "pdpscpr");
#endif

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace NetStateMachine;


//-=========================================================
//
// Activities
//
//-=========================================================

namespace PDPDeftSCprDataClientStartActivity
{
typedef MeshMachine::TAcceptErrorState<CoreNetStates::TAwaitingDataClientStarted> TAwaitingDataClientStartedOrError;

DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStartDataClient, PDPDeftDataClientStart, TCFDataClient::TStart, PDPSCprStates::CStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientStart, PDPSCprStates::TNoTagOrUserAuthenticateOrAlreadyStarted)
	LAST_NODEACTIVITY_ENTRY(CoreNetStates::KAlreadyStarted, PRStates::TSendDataClientStarted)

	NODEACTIVITY_ENTRY(PDPSCprStates::KUserAuthenticate, PDPSCprStates::TSendAuthenticate, PDPSCprStates::TAwaitingAuthenticateComplete, MeshMachine::TNoTagOrErrorTag)
	NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TCreatePrimaryPDPCtx, PDPSCprStates::TAwaitingPrimaryPDPCtxCreated, PDPSCprStates::TNoTagOrSendErrorRecoveryRequestOrError)
	NODEACTIVITY_ENTRY(PDPSCprStates::KSendErrorRecoveryRequest, PDPSCprStates::TSendErrorRecoveryRequest, MeshMachine::TAwaitingErrorRecoveryResponseOrError, PDPSCprStates::TNoTagBackwardOrErrorTag)
 	THROUGH_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TFillInGrantedParamsAndImsExtParams, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TOverrideProvisionAndStartDataClient, TAwaitingDataClientStartedOrError, MeshMachine::TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TSendDataClientStarted)

	// Cleanup if error occurred
    THROUGH_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing, CoreNetStates::TNoTagOrNoDataClientsToStop)
    NODEACTIVITY_ENTRY(KNoTag, SCprStates::TStopYourFlows, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TTag<CoreNetStates::KNoDataClientsToStop>)
    THROUGH_NODEACTIVITY_ENTRY(CoreNetStates::KNoDataClientsToStop, MeshMachine::TDoNothing, PDPSCprStates::TNoTagOrProviderStopped)
    NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TDestroyPDPContext, PDPSCprStates::TAwaitingPDPContextDestroyed, MeshMachine::TTag<CoreNetStates::KProviderStopped>)
    THROUGH_NODEACTIVITY_ENTRY(CoreNetStates::KProviderStopped, PDPSCprStates::TCleanupFSMAndDataClients, MeshMachine::TTag<CoreNetStates::KProviderStopped>)
    LAST_NODEACTIVITY_ENTRY(CoreNetStates::KProviderStopped, MeshMachine::TRaiseActivityError)
NODEACTIVITY_END()
}

namespace PDPSCprNetworkStatusEventActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityNotification, subConnStatusEvent, TPDPMessages::TPDPFSMMessage)
	FIRST_NODEACTIVITY_ENTRY(PDPSCprStates::TAwaitingNetworkStatusEvent, PDPSCprStates::TNetworkStatusEventTypeTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)

	// Assume that anything other than EStatusUnattached means the network
	// is still up, so just ignore this event.
	LAST_NODEACTIVITY_ENTRY(RPacketService::EStatusAttached, MeshMachine::TDoNothing)
	LAST_NODEACTIVITY_ENTRY(RPacketService::EStatusActive, MeshMachine::TDoNothing)
	LAST_NODEACTIVITY_ENTRY(RPacketService::EStatusSuspended, MeshMachine::TDoNothing)

	NODEACTIVITY_ENTRY(RPacketService::EStatusUnattached, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, SCprStates::TSendDataClientGoneDown)
NODEACTIVITY_END()
}

namespace PDPDeftSCprGoneDownActivity
{
//TODO perhaps get this from a header file since its used in a number of places - see ss_subconn.cpp
typedef MeshMachine::TAcceptErrorState<CoreNetStates::TAwaitingApplyResponse> TAwaitingApplyResponseOrError;

//mandated by 3GPP TS 23.107 Annex C, the default going down needs to select a new default
//among the non-defaults and swap the datapath. Swapping datapath is now done
//transparently, i.e.: without the upper layer knowing (binders are kept, flows are swapped).
//This arguably isn't pretty and could be changed to something more explicit, i.e.:
//sending TBindToRequest to the upper layer SCPR so that it does an explit rebind.
//[401TODO]: the swap only works for rawIp, doesn't for PPP.
//[401TODO]: if we stick to the transpartent swap, we need to change the message
//           this scpr sends to the flow. The flow responds with a full swap
//           to TRejoin, whereas TRejoin doesn't mean swap - suggesting to
//           introduce PDP level msg: TRejoinAndSwap.
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityGoneDown, PDPDeftSCprGoneDown, TPDPMessages::TPDPFSMMessage, PDPSCprStates::CPrimaryPDPGoneDownActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(PDPSCprStates::TAwaitingPDPContextGoneDown, MeshMachine::TActiveOrNoTag<ECFActivityStartDataClient>)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TCleanupFSM, PDPSCprStates::CPrimaryPDPGoneDownActivity::TNoTagOrProviderStopped)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::CPrimaryPDPGoneDownActivity::TStoreOriginalDataClient, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::CPrimaryPDPGoneDownActivity::TRejoinDataClient, CoreNetStates::TAwaitingRejoinDataClientComplete, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::CPrimaryPDPGoneDownActivity::TApplyNewDefault, TAwaitingApplyResponseOrError, MeshMachine::TNoTag)
    THROUGH_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::CPrimaryPDPGoneDownActivity::TSwitchToNewDefault, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::CPrimaryPDPGoneDownActivity::TStopOriginalDataClient, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TNoTag)
    
    LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TClearError)
    THROUGH_NODEACTIVITY_ENTRY(KActiveTag, MeshMachine::TDoNothing, PDPSCprStates::TNoTagOrContentionTag)
    //Awaiting for contention result, do not stop the start activity.
    LAST_NODEACTIVITY_ENTRY(PDPSCprStates::KContentionTag, MeshMachine::TDoNothing)
    
    LAST_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TCancelDataClientStartInPDP)
    
    NODEACTIVITY_ENTRY(CoreNetStates::KProviderStopped, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, MeshMachine::TTag<CoreNetStates::KProviderStopped>)
    LAST_NODEACTIVITY_ENTRY(CoreNetStates::KProviderStopped, PDPSCprStates::TSendGoneDown)
NODEACTIVITY_END()
}

namespace PDPDeftSCprClientLeaveActivity
{ //This activity will wait for ECFActivityBinderRequest to complete
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityClientLeave, PDPDeftSCprClientLeave, TNodeSignal::TNullMessageId, MeshMachine::CNodeRetryActivity::NewL) //May be waiting for both messages
	NODEACTIVITY_ENTRY(KNoTag, PRStates::TRemoveClientAndSendLeaveCompleteIfRequest, CoreStates::TAwaitingClientLeave, CoreNetStates::TNoTagBlockedByBindToAndNoClients)
	LAST_NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TSendDataClientIdleIfNoSubconnsAndNoClients)
NODEACTIVITY_END()
}

namespace PDPDeftSCprIoctlActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityIoctl, PDPDeftSCprIoctl, TNodeSignal::TNullMessageId, MeshMachine::CNodeParallelMessageStoreActivityBase::NewL)
    FIRST_NODEACTIVITY_ENTRY(PDPSCprStates::TAwaitingIoctlMessage, MeshMachine::TNoTag)
    NODEACTIVITY_ENTRY(KNoTag, PDPSCprStates::TRetrieveSipAddr, CoreNetStates::TAwaitingRMessage2Processed, MeshMachine::TNoTag)
    LAST_NODEACTIVITY_ENTRY(KNoTag, CoreStates::TPostToOriginators)
NODEACTIVITY_END()
}

namespace PDPDefSCprActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(activityMap)
	ACTIVITY_MAP_ENTRY(PDPDeftSCprDataClientStartActivity, PDPDeftDataClientStart)
	ACTIVITY_MAP_ENTRY(PDPSCprNetworkStatusEventActivity, subConnStatusEvent)
	ACTIVITY_MAP_ENTRY(PDPDeftSCprGoneDownActivity, PDPDeftSCprGoneDown)
    ACTIVITY_MAP_ENTRY(PDPDeftSCprClientLeaveActivity, PDPDeftSCprClientLeave)
    ACTIVITY_MAP_ENTRY(PDPDeftSCprIoctlActivity, PDPDeftSCprIoctl)
ACTIVITY_MAP_END_BASE(PDPSCprActivities, activityMap)
}

//-=========================================================
//
// CPDPDefaultSubConnectionProvider methods
//
//-=========================================================

CPDPDefaultSubConnectionProvider::CPDPDefaultSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory)
	: CPDPSubConnectionProvider(aFactory, PDPDefSCprActivities::activityMap::Self()),
	iAuthDialog(NULL)
    {
    LOG_NODE_CREATE1(KPDPSCprSubTag, CPDPDefaultSubConnectionProvider, " [factory=%08x]", &aFactory)
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_CTOR_1, "Constructor");
    __FLOG_OPEN(KCFNodeTag, KPDPSCprSubTag);
    }

CPDPDefaultSubConnectionProvider::~CPDPDefaultSubConnectionProvider()
    {
	LOG_NODE_DESTROY(KPDPSCprSubTag, CPDPDefaultSubConnectionProvider)
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_DTOR_1, "Destructor");
	__FLOG_CLOSE;
	delete iAuthDialog;
    }

CPDPDefaultSubConnectionProvider* CPDPDefaultSubConnectionProvider::NewL(CPDPSubConnectionProviderFactory& aFactory)
/**
Construct a new PDP SubConnection Provider Object

@params aFactory factory that create this object
@param aConnProvider Connection Provider associated with this object
*/
	{
	CPDPDefaultSubConnectionProvider* ptr = new (ELeave) CPDPDefaultSubConnectionProvider(aFactory);
    CleanupStack::PushL(ptr);
    ptr->ConstructL();
    CleanupStack::Pop();
	return ptr;
	}

void CPDPDefaultSubConnectionProvider::ConstructL()
    {
    CPDPSubConnectionProvider::ConstructL();
    iPdpFsmInterface = new (ELeave) CPdpFsmInterface(*this);
    }

void CPDPDefaultSubConnectionProvider::PdpFsmAllContextEvent(TInt aNotification, TInt /*aParam*/)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_PDPFSMALLCONTEXTEVENT_1, "Notification Id [%d]", aNotification);
    if (aNotification == KNetworkStatusEvent)
        {
        TPDPMessages::TPDPFSMMessage statusMsg(KNetworkStatusEvent, KErrNone);
        TRAP_IGNORE(ReceivedL(TNodeId(Id()), TNodeCtxId(0, Id()), statusMsg)); //TODO - use trap assert?
        }
    }

void CPDPDefaultSubConnectionProvider::LinkUp()
    {
    if (iLinkUps++ == 0)
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_LINKUP_1, "Sending KLinkLayerOpen message to control client/provider");
		TCFMessage::TStateChange msg(Elements::TStateChange(KLinkLayerOpen, KErrNone));
		//Forward to control clients if there are any
		TInt ctrlClientCount = PostToClients<TDefaultClientMatchPolicy>(Id(), msg, TClientType(TCFClientType::ECtrl));
		if (0==ctrlClientCount)
			{ //If there are no control clients any more, forward to the control provider
			PostToClients<TDefaultClientMatchPolicy>(Id(), msg, TClientType(TCFClientType::ECtrlProvider));
			}
        }
    }

void CPDPDefaultSubConnectionProvider::LinkDown(TInt aCause)
    {
    if (++iLinkDowns-iLinkUps == 0 || iLinkUps == 0)
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_LINKDOWN_1, "Sending KLinkLayerClosed message to control provider");
        ControlProvider()->PostMessage(Id(),
            TCFMessage::TStateChange(Elements::TStateChange(KLinkLayerClosed, aCause)).CRef());
        }
	SendDataClientIdleIfNoSubconnsAndNoClientsL();
    }

void CPDPDefaultSubConnectionProvider::SendDataClientIdleIfNoSubconnsAndNoClientsL()
	{
	// linkDowns is called more places than linkup, so it may in fact be higher, would be nice if something was
	// done in the future to make iLinkDowns and iLinkUps are symettrical
	if (iLinkDowns-iLinkUps >= 0 || iLinkUps == 0)
		{
		if (GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl|TCFClientType::EAux))==NULL
			&& GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EActive)) == NULL)
			{
			RNodeInterface* cp = ControlProvider();
			if (cp)
				{ //If there is no Control Provider we probably are an MCPR/Tier Manager/etc
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_SENDDATACLIENTIDLE_1, "Sending Idle message to control provider");
				cp->PostMessage(Id(), TCFControlProvider::TIdle().CRef());
				}
			return;
			}

		RNodeInterface* cc = GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl));
		if (cc==NULL)
			{
			TClientIter<TDefaultClientMatchPolicy> dciter = GetClientIter<TDefaultClientMatchPolicy>(
				TClientType(
					TCFClientType::EData,
						TCFClientType::EActive
						|TCFClientType::EActivating
						|TCFClientType::EStarted
						|TCFClientType::EStarting
						|TCFClientType::ELeaving
					)
				);

			TInt i = 0;
			RNodeInterface* dc;
			while ((dc = dciter[i++]) != NULL)
				{
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_SENDDATACLIENTIDLE_2, "Sending Idle message");
				dc->PostMessage(Id(), TCFControlProvider::TIdle().CRef());
				}
			}
		}
	}

void CPDPDefaultSubConnectionProvider::AuthenticateL()
    {
    __ASSERT_DEBUG(iAuthDialog == NULL, User::Invariant());

    //Update & send user name, password from commsdat; it is upto dialog server to decide which information to show to the user
    RPacketContext::TProtocolConfigOptionV2* configOption = ProtocolConfigOptionL();

    iUsername.Copy(configOption->iAuthInfo.iUsername);
    iPassword.Copy(configOption->iAuthInfo.iPassword);

    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_AUTHENTICATE_1, "CPDPSubConnectionProvider [this=%08x]::AuthenticationCompleteL() KCDTIdWCDMAIfAuthName [%S] ", (TUint)this, iUsername);
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_AUTHENTICATE_2, "CPDPSubConnectionProvider [this=%08x]::AuthenticationCompleteL() KCDTIdWCDMAIfAuthName [%S] ", (TUint)this, iPassword);
    
    iAuthDialog = CAuthenticationDialog::NewL();
    iAuthDialog->Authenticate(*this, iUsername, iPassword);
    }

void CPDPDefaultSubConnectionProvider::AuthenticationCompleteL(TInt aError)
    {
    //Update user name & password
    if (aError == KErrNone)
        {
        RPacketContext::TProtocolConfigOptionV2* configOption = ProtocolConfigOptionL();

        configOption->iAuthInfo.iUsername.Copy(iUsername);
        configOption->iAuthInfo.iPassword.Copy(iPassword);
       
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_AUTHENTICATIONCOMPLETE_1, "CPDPSubConnectionProvider [this=%08x]::AuthenticationCompleteL() KCDTIdWCDMAIfAuthName [%s] ", (TUint)this, configOption->iAuthInfo.iUsername);
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_AUTHENTICATIONCOMPLETE_2, "CPDPSubConnectionProvider [this=%08x]::AuthenticationCompleteL() KCDTIdWCDMAIfAuthName [%s] ", (TUint)this, configOption->iAuthInfo.iPassword);
        }

    //Send AuthenticateComplete message
    RClientInterface::OpenPostMessageClose(Id(), Id(),
            TPDPMessages::TAuthenticateComplete(aError).CRef());

    delete iAuthDialog;
    iAuthDialog = NULL;
    }

RPacketContext::TProtocolConfigOptionV2* CPDPDefaultSubConnectionProvider::ProtocolConfigOptionL()
/**
 * This function returns the pointer to TProtocolConfigOptionV2 parameter of ContextConfig.
 */
    {
    CGPRSProvision* gprsProvision = const_cast<CGPRSProvision*>(static_cast<const CGPRSProvision*>(
            AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CGPRSProvision::EUid,CGPRSProvision::ETypeId))));
    
    if (gprsProvision == NULL)
        {
        User::Leave(KErrCorrupt);
        }

    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPDEFAULTSUBCONNECTIONPROVIDER_PROTOCOLCONFIGOPTIONL_1, "Return pointer to TProtocolConfigOptionV2 of UMTS GPRS release [%d]", gprsProvision->UmtsGprsRelease());

    RPacketContext::TProtocolConfigOptionV2* configOption=NULL;

    switch (gprsProvision->UmtsGprsRelease())
        {
        case TPacketDataConfigBase::KConfigGPRS:
            {
            RPacketContext::TContextConfigGPRS& contextGPRS = gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigGPRS>();
            configOption = &contextGPRS.iProtocolConfigOption;
            }
            break;
        case TPacketDataConfigBase::KConfigRel99Rel4:
            {
            RPacketContext::TContextConfigR99_R4& contextUMTS = gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigR99_R4>();
            configOption = &contextUMTS.iProtocolConfigOption;
            }
            break;
        case TPacketDataConfigBase::KConfigRel5:
            {
            RPacketContext::TContextConfig_R5& contextR5 = gprsProvision->GetScratchContextAs<RPacketContext::TContextConfig_R5>();
            configOption = &contextR5.iProtocolConfigOption;
            }
            break;
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    return configOption;
    }
