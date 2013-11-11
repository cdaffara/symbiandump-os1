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
// PDP SubConnection Provider states/transitions
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/es_config.h>
#include <comms-infras/corescpractivities.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include "PDPSCPRStates.h"
#include "PDPDeftSCPR.h"
#include "PDPParamMapper.h"
#include <comms-infras/ss_log.h>
#include <networking/ipaddrinfoparams.h>
#include <networking/mbmsparams.h>
#include <ip_subconparams.h>
#include <genericscprparams.h>
#include "pdpfsmnmspace.h"
#include "pdpdef.h"
#include <comms-infras/ss_nodemessages_flow.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_api_ext.h>
#include <elements/nm_messages_errorrecovery.h>
#include <comms-infras/sockmes.h> // for ioctl ipc
#include <nifvar_internal.h>
#include <elements/mm_activities.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include <comms-infras/connectionsettings.h>
#include <hash.h>
#include <e32math.h>

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace NetStateMachine;
using namespace PDPSCprActivities;
using namespace SpudMan;
using namespace ConnectionServ;

#ifdef _DEBUG
_LIT (KPdpSCprPanic,"PpdScprPanic");
#endif

const TUint8 KGenericNifChallengeSize = 8;


//-=========================================================
//
// States
//
//-=========================================================
namespace PDPSCprStates
{
//-=========================================================
//Util
//-=========================================================
DEFINE_SMELEMENT(TAwaitingPDPFSMMessage, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingPDPFSMMessage::Accept()
    {
    return iContext.iMessage.IsMessage<TPDPMessages::TPDPFSMMessage>();
    }

DEFINE_SMELEMENT(TNoTagOrError, NetStateMachine::MStateFork, PDPSCprStates::TContext)
TInt TNoTagOrError::TransitionTag()
    {
    ASSERT(iContext.iNodeActivity);
    TPDPMessages::TPDPFSMMessage& msg = message_cast<TPDPMessages::TPDPFSMMessage>(iContext.iMessage);
    if (msg.iValue2 != KErrNone)
        {
        iContext.iNodeActivity->SetError(msg.iValue2);
        return KErrorTag;
        }
    return KNoTag;
    }

DEFINE_SMELEMENT(TNoTagOrUserAuthenticateOrAlreadyStarted, NetStateMachine::MStateFork, PDPSCprStates::TDefContext)
TInt TNoTagOrUserAuthenticateOrAlreadyStarted::TransitionTag()
    {
    ASSERT(iContext.iMessage.IsMessage<TCFDataClient::TStart>());
    ASSERT(iContext.iNodeActivity);
    if (iContext.Node().iPDPFsmContextId != CPDPSubConnectionProvider::EInvalidContextId)
        {
        return CoreNetStates::KAlreadyStarted;
        }

    if (iContext.Node().PromptForAuth())
        {
        return PDPSCprStates::KUserAuthenticate;
        }

    return KNoTag;
    }

TBool TAwaitingPDPFSMMessage::Accept(TInt aExtensionId)
    {
    TPDPMessages::TPDPFSMMessage* pdpmsg = message_cast<TPDPMessages::TPDPFSMMessage>(&iContext.iMessage);
    if ( pdpmsg )
        {
        if (pdpmsg->iValue1 == aExtensionId)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

DEFINE_SMELEMENT(TSendDataClientIdleIfNoSubconnsAndNoClients, NetStateMachine::MStateTransition, PDPSCprStates::TDefContext)
void TSendDataClientIdleIfNoSubconnsAndNoClients::DoL()
    {
	iContext.Node().SendDataClientIdleIfNoSubconnsAndNoClientsL();
	}

DEFINE_SMELEMENT(TNoTagOrSendErrorRecoveryRequestOrError, NetStateMachine::MStateFork, PDPSCprStates::TContext)
TInt TNoTagOrSendErrorRecoveryRequestOrError::TransitionTag()
	{
	ASSERT(iContext.iNodeActivity);
	TPDPMessages::TPDPFSMMessage& msg = message_cast<TPDPMessages::TPDPFSMMessage>(iContext.iMessage);
	if (msg.iValue2 == KErrUmtsMaxNumOfContextExceededByNetwork ||
	    msg.iValue2 == KErrUmtsMaxNumOfContextExceededByPhone)
		{
		return KSendErrorRecoveryRequest;
		}
	else if (msg.iValue2 != KErrNone)
		{
		iContext.iNodeActivity->SetError(msg.iValue2);
		return KErrorTag;
		}
	return KNoTag;
	}

DEFINE_SMELEMENT(TNoTagBackwardOrErrorTag, NetStateMachine::MStateFork, PDPSCprStates::TContext)
TInt TNoTagBackwardOrErrorTag::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KPdpSCprPanic, CorePanics::KPanicNoActivity));
	iContext.Node().SetContentionRequested(EFalse);
	if (iContext.iMessage.IsMessage<TEErrorRecovery::TErrorRecoveryResponse>() &&
		message_cast<TEErrorRecovery::TErrorRecoveryResponse>(iContext.iMessage).iErrResponse.iAction == TErrResponse::ERetry)
		{
		return KNoTag | NetStateMachine::EBackward;	
		}

		
	TEErrorRecovery::TErrorRecoveryResponse* recoveryMsg = message_cast<TEErrorRecovery::TErrorRecoveryResponse>(&iContext.iMessage);
	if (recoveryMsg)
		{
		iContext.iNodeActivity->SetError(recoveryMsg->iErrResponse.iError);
		}
		
		
	TEBase::TError* errMsg = message_cast<TEBase::TError>(&iContext.iMessage);
	if (errMsg)
		{
		iContext.iNodeActivity->SetError(errMsg->iValue);
		}
	return KErrorTag;
	}

DEFINE_SMELEMENT(TNoTagOrContentionTag, NetStateMachine::MStateFork, PDPSCprStates::TContext)
TInt TNoTagOrContentionTag::TransitionTag()
	{
	if (iContext.Node().ContentionRequested())
		{
		return KContentionTag;
		}
	return KNoTag;
	}



//-=========================================================
//Provisioning
//-=========================================================
DEFINE_SMELEMENT(TSelfInit, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
// This is should be revisited in future releases, basic idea is that information
// that can be modified should have a visibility that is to that node's
// level and not further (i.e. changes here will affect this connection
// only, not every connection that uses this MCPR)
//      MCPR - AccessPointConfig() contains information that is used by
//             all connections under that MCPR and is unchanging
//       CPR - Again for all connections under that CPR and is unchanging
//      SCPR - Connection specific information (i.e. can change and does change)
//        DC - No point - just reads information.
// Here we've moved basically all of the configuration information that was in
// the MCPR [that could be moved] this is because some of it changes / is overwritten
// with information that comes out of etel/modem.  This also has the added benefit of
// freeing up memory in cases where the connection is torn down but the MCPR lies 
// around for availability purposes.  In the future, this code (or something like it)
// should be moved into the PDP.CPR when it exists.

void TSelfInit::SetupProvisionCfgL(ESock::CCommsDatIapView* aIapView)
    {
    CPDPSubConnectionProvider &tNode = static_cast<CPDPSubConnectionProvider&>(iContext.Node());
    
    RMetaExtensionContainer mec;
    mec.Open(tNode.AccessPointConfig());
    CleanupClosePushL(mec);
  
    mec.AppendExtensionL(CIPConfig::NewFromGPRSOutLC(aIapView));
    CleanupStack::Pop();

    mec.AppendExtensionL(CBCAProvision::NewLC(aIapView));
    CleanupStack::Pop();

    mec.AppendExtensionL(CImsExtProvision::NewLC(aIapView));
    CleanupStack::Pop();

    CGPRSProvision* gprsProvision = CGPRSProvision::NewLC(aIapView);
    mec.AppendExtensionL(gprsProvision);
    CleanupStack::Pop(gprsProvision);
    
    //It's not legal for the qos defaults to be absent.
    CDefaultPacketQoSProvision* defaultQoS = NULL;
    
    TRAPD(ret, defaultQoS = CDefaultPacketQoSProvision::NewL(aIapView));   
    
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
    
    CRawIpAgentConfig* rawIpAgentConfig = CRawIpAgentConfig::NewLC(aIapView, &gprsProvision->GetScratchContextAs<TPacketDataConfigBase>());
    mec.AppendExtensionL(rawIpAgentConfig);
    CleanupStack::Pop();

    tNode.AccessPointConfig().Close();
    tNode.AccessPointConfig().Open(mec);
    CleanupStack::PopAndDestroy(&mec); 
    }

void TSelfInit::DoL()
    {
    CPDPSubConnectionProvider &tNode = static_cast<CPDPSubConnectionProvider&>(iContext.Node());

    const TProviderInfoExt* providerInfoExt = static_cast<const TProviderInfoExt*>(tNode.AccessPointConfig().FindExtension(
            STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId)));

    // this should always be here, however, a bit of defensive programming
    // never hurt..
    if (providerInfoExt == NULL)
        {
        User::Leave(KErrCorrupt);
        }

    CCommsDatIapView* iapView = CCommsDatIapView::NewLC(providerInfoExt->iProviderInfo.APId());

    TRAP(tNode.iProvisionFailure,SetupProvisionCfgL(iapView));

    // Don't want any failure here to be masked by successful configuration later on.
    // A leave here will not cause a roll back of the connection but it will show an
    // Error -> message in the SVG diagram.  The provision failure error code will 
    // cause the connection rollback.
    User::LeaveIfError(tNode.iProvisionFailure);
    
    // if the FSM interface is null, this means that we're initializing a secondary context
    // as this code is common for both
    
    if (tNode.iPdpFsmInterface == NULL)
        {
        //Non-default SCPR running this code
        ASSERT(iContext.Node().ControlProvider());
        ESock::TDefaultSCPRFactoryQuery query(iContext.Node().ControlProvider()->RecipientId(), TSubConnOpen::EAttachToDefault );
        ESock::CSubConnectionProviderBase* defProvider = static_cast<ESock::CSubConnectionProviderBase*>(static_cast<ESock::CSubConnectionProviderFactoryBase&>(iContext.Node().Factory()).Find(query));
        ASSERT(defProvider);
        iContext.Node().iDefaultSCPR = static_cast<CPDPDefaultSubConnectionProvider*>(defProvider);
        ASSERT(iContext.Node().iDefaultSCPR != &iContext.Node()); //This transition is not expected to be executed for defaults.
        ASSERT(iContext.Node().iDefaultSCPR->iPdpFsmInterface != NULL);
        ASSERT(iContext.Node().iPdpFsmInterface == NULL);
        iContext.Node().iPdpFsmInterface = iContext.Node().iDefaultSCPR->iPdpFsmInterface;
        iContext.Node().iPdpFsmInterface->Open();
        }
    else
        {
        //default SCPR running this code (i.e. primary context)
        const CTSYProvision* tsyProvision =\
                static_cast<const CTSYProvision*>(tNode.AccessPointConfig().FindExtension(CTSYProvision::TypeId()));
        
        if (tsyProvision == NULL)
        	{
            // we do not have to set provision failure here
            // as it was set in the base class constructor
            // svg will show the leave error code though
        	User::Leave(KErrCorrupt);
        	}
        
        CGPRSProvision* gprsProvision = const_cast<CGPRSProvision*>(static_cast<const CGPRSProvision*>(
        	    tNode.AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CGPRSProvision::EUid,CGPRSProvision::ETypeId))));
        
        if (gprsProvision == NULL)
            {
            tNode.iProvisionFailure = KErrCorrupt;
            User::Leave(KErrCorrupt);
            }
        
        TInt configType = TPacketDataConfigBase::KConfigGPRS;
        
        switch (gprsProvision->UmtsGprsRelease())
        	{
        	case TPacketDataConfigBase::KConfigGPRS:
        	    configType = TPacketDataConfigBase::KConfigGPRS;
        		break;
        	case TPacketDataConfigBase::KConfigRel99Rel4:
        	    configType = TPacketDataConfigBase::KConfigRel99Rel4;
        		break;
        	case TPacketDataConfigBase::KConfigRel5:
        	    configType = TPacketDataConfigBase::KConfigRel5;
        		break;
        	default:
        	    // we do not have to set provision failure here
        	    // as it was set in the base class constructor
        	    // svg will show the leave error code though
        		User::Leave(KErrNotSupported);
        		break;
        	}
        
        // a provisioning failure would be unrecoverable as this only happens
        // we cannot create memory, access etel or there is a configuration
        // problem - however, leaving with an error here does not stop the
        // sequence of events because the handler for provision config is not
        // expecting a response. if there is a failure then it will be errored in 
        // the next activity in the sequence, i.e. DataClientStart
        TRAP(tNode.iProvisionFailure,tNode.iPdpFsmInterface->NewL(tsyProvision->iTsyName,configType));
        
        tNode.iDefaultSCPR = static_cast<CPDPDefaultSubConnectionProvider*>(&tNode);

        TBool promptForAuth = EFalse;
        iapView->GetBool(KCDTIdWCDMAIfPromptForAuth | KCDTIdOutgoingGprsRecord, promptForAuth);

        //Default SCPR executing this code, so safe to cast.
        CPDPDefaultSubConnectionProvider &tempNode = static_cast<CPDPDefaultSubConnectionProvider&>(iContext.Node());
        tempNode.SetPromptForAuth(promptForAuth);        
        }
    CleanupStack::PopAndDestroy();          // CloseIapView();
    }


//-=========================================================
//Creating context
//-=========================================================
MeshMachine::CNodeActivityBase* CStartActivity::NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
    {
    return new (ELeave) CStartActivity(aActivitySig, aNode);
    };

CStartActivity::~CStartActivity()
    {
    RNodeInterface* startedDataClient = iNode.GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EStarted));
    ASSERT(iNode.GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData))[1] == NULL);
    CPDPSubConnectionProvider& pdpscpr = static_cast<CPDPSubConnectionProvider&>(iNode);
    if (pdpscpr.iPDPFsmContextId == CPDPSubConnectionProvider::EInvalidContextId ||
        startedDataClient == NULL)
        {
        pdpscpr.LinkDown(Error());
        }
    }

CStartActivity::CStartActivity( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
:MeshMachine::CNodeRetryActivity(aActivitySig, aNode)
    {
    }

DEFINE_SMELEMENT(TCreateSecondaryOrMbmsPDPCtx, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TCreateSecondaryOrMbmsPDPCtx::DoL()
    {
    ASSERT(iContext.Node().iPdpFsmInterface);

  	RParameterFamily mbmsInfoFamily = iContext.Node().iParameterBundle.FindFamily(KSubConChannelParamsType);
  	STypeId typeId = STypeId::CreateSTypeId(KSubConChannelParamsImplUid, KSubConChannelParamsType);
    CSubConChannelParamSet *mbmsSubConParamSet = mbmsInfoFamily.IsNull() ? NULL :
    		(CSubConChannelParamSet *)(mbmsInfoFamily.FindParameterSet(typeId, RParameterFamily::ERequested));

    if (mbmsSubConParamSet)
		{
		RPacketMbmsContext::TContextConfigMbmsV1 mbmsConfig;
		ConnectionServ::TMBMSChannelInfoV1* channelPtr = reinterpret_cast<ConnectionServ::TMBMSChannelInfoV1*>(mbmsSubConParamSet->GetChannelInfo());
   		mbmsConfig.iTmgi		 		= channelPtr->GetTmgi();
   	 	mbmsConfig.iMbmsServicePriority = channelPtr->GetServicePriority();
   		mbmsConfig.iMbmsAccessBearer    = channelPtr->GetScope();

   		TSessionOperatioInfo sessionInfo;
		CSubConMBMSExtensionParamSet *mbmsSessionSet=static_cast<CSubConMBMSExtensionParamSet *>(mbmsInfoFamily.FindParameterSet(
			STypeId::CreateSTypeId(KSubConMBMSExtParamsImplUid,KSubConMBMSSessionExtParamsType),RParameterFamily::ERequested));
		if (mbmsSessionSet)
			{
			sessionInfo.iSessionIds.Reset();
			for(TInt i=0;i<mbmsSessionSet->GetSessionCount();i++)
				{
				sessionInfo.iSessionIds.Append(mbmsSessionSet->GetSessionId(i));
				}
			sessionInfo.iOperation = mbmsSessionSet->GetOperationType();
			mbmsConfig.iMbmsServiceMode  = mbmsSessionSet->GetServiceMode();
			}
		else
			{
			User::Leave(KErrCorrupt);
			}

		iContext.Node().iPDPFsmContextId = iContext.Node().iPdpFsmInterface->NewFsmContextL(iContext.Node(),SpudMan::EMbms);

		iContext.Node().iContextType=SpudMan::EMbms;

       	iContext.Node().iPdpFsmInterface->Set(iContext.Node().iPDPFsmContextId,mbmsConfig);

		TTFTInfo tft;
    	User::LeaveIfError(iContext.Node().iPdpFsmInterface->Set(iContext.Node().iPDPFsmContextId, tft));
    	User::LeaveIfError(iContext.Node().iPdpFsmInterface->Set(iContext.Node().iPDPFsmContextId, sessionInfo));
    	User::LeaveIfError(iContext.Node().iPdpFsmInterface->Input(iContext.Node().iPDPFsmContextId, SpudMan::ECreateMbmsPDPContext));
    	iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
    	sessionInfo.iSessionIds.Close();
    	}
	else
		{
		// Reset the default QoS and TFT here. KContextQoSSet should arrive soon
		// with the proper values.
#ifdef SYMBIAN_NETWORKING_UMTSR5
		RPacketQoS::TQoSR5Requested qos;
#else
		RPacketQoS::TQoSR99_R4Requested qos;
#endif
		// SYMBIAN_NETWORKING_UMTSR5

		iContext.Node().iPDPFsmContextId = iContext.Node().iPdpFsmInterface->NewFsmContextL(iContext.Node(),SpudMan::ESecondary);

		iContext.Node().iContextType = SpudMan::ESecondary;

		User::LeaveIfError(iContext.Node().iPdpFsmInterface->Set(iContext.Node().iPDPFsmContextId,  qos));

		TTFTInfo tft;
		User::LeaveIfError(iContext.Node().iPdpFsmInterface->Set(iContext.Node().iPDPFsmContextId, tft));
		User::LeaveIfError(iContext.Node().iPdpFsmInterface->Input(iContext.Node().iPDPFsmContextId, SpudMan::ECreateSecondaryPDPContext));
		iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
		}
    }

DEFINE_SMELEMENT(TAwaitingPDPCtxCreated, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingPDPCtxCreated::Accept()
    {
    if (iContext.Node().iContextType != SpudMan::EMbms)
    	{
    	return TAwaitingPDPFSMMessage::Accept(KSecondaryContextCreated);
    	}
    else
    	{
       	return TAwaitingPDPFSMMessage::Accept(KMbmsContextCreated); //
		}
    }

DEFINE_SMELEMENT(TAwaitingPrimaryPDPCtxCreated, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingPrimaryPDPCtxCreated::Accept()
    {
    return TAwaitingPDPFSMMessage::Accept(KPrimaryContextCreated);
    }

DEFINE_SMELEMENT(TCreatePrimaryPDPCtx, NetStateMachine::MStateTransition, PDPSCprStates::TDefContext)
void TCreatePrimaryPDPCtx::SetupSipServerAddrRetrievalL(RPacketContext::TProtocolConfigOptionV2& aPco)
	{
	TPtr8 pcoPtr(const_cast<TUint8*>(aPco.iMiscBuffer.Ptr()),aPco.iMiscBuffer.Length(),aPco.iMiscBuffer.MaxLength());

	// attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> tlv(pcoPtr,0);
	tlv.AppendItemL(RPacketContext::TPcoId(RPacketContext::EEtelPcktPCSCFAddressRequest),
		TPtr8(static_cast<TUint8*>(NULL), 0, 0));
	aPco.iMiscBuffer.SetLength(pcoPtr.Length());
	}

void TCreatePrimaryPDPCtx::SetImsSignallingFlagL(RPacketContext::TProtocolConfigOptionV2& aPco, TBool aImcn)
	{
	if (aImcn)
		{
		TPtr8 pcoPtr(const_cast<TUint8*>(aPco.iMiscBuffer.Ptr()),aPco.iMiscBuffer.Length(),aPco.iMiscBuffer.MaxLength());
		TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> tlv(pcoPtr,0);
		tlv.AppendItemL(RPacketContext::TPcoId(RPacketContext::EEtelPcktIMCNMSSubsystemSignallingFlag ),
						TPtr8(static_cast<TUint8*>(NULL), 0, 0));
		aPco.iMiscBuffer.SetLength(pcoPtr.Length());
		}
	}

TBool TCreatePrimaryPDPCtx::IsModeGsmL() const
	{
    const CTSYProvision* tsyProvision = static_cast<const CTSYProvision*>(
    	iContext.Node().AccessPointConfig().FindExtension(CTSYProvision::TypeId()));
	if (tsyProvision == NULL)
		{
		User::Leave(KErrCorrupt);
		}

	RTelServer telServer;
	User::LeaveIfError(telServer.Connect());
	CleanupClosePushL(telServer);

	User::LeaveIfError(telServer.LoadPhoneModule(tsyProvision->iTsyName));

	TInt phones = 0;
	User::LeaveIfError(telServer.EnumeratePhones(phones));

	RTelServer::TPhoneInfo phoneInfo;
	TBuf<KCommsDbSvrMaxFieldLength> currentTsyName;
	TBool found = EFalse;
	_LIT(KTsyNameExtension, ".tsy");
	for (TInt i = 0; i < phones; ++i)
		{
		User::LeaveIfError(telServer.GetTsyName(i, currentTsyName));
		// remove .TSY extension if necessary
		if (currentTsyName.Right(4).CompareF(KTsyNameExtension()) == 0)
			{
			currentTsyName = currentTsyName.Left(currentTsyName.Length() - 4);
			}
		// compare current TSY name with the one we're looking for
		if (currentTsyName.CompareF(tsyProvision->iTsyName) == 0)
			{
			// get phone info from the TSY
			User::LeaveIfError(telServer.GetPhoneInfo(i, phoneInfo));
			found = ETrue;
			break;
			}
		}
	if (!found)
		{
		User::Leave(KErrNotFound);
		}

	RMobilePhone phone;
	User::LeaveIfError(phone.Open(telServer, phoneInfo.iName));
	CleanupClosePushL(phone);

	RMobilePhone::TMobilePhoneNetworkMode networkMode = RMobilePhone::ENetworkModeUnknown;
	User::LeaveIfError(phone.GetCurrentMode(networkMode));
	CleanupStack::PopAndDestroy(2); // phone, telServer
	return (networkMode == RMobilePhone::ENetworkModeGsm);
	}

void TCreatePrimaryPDPCtx::SetChapInformationL(RPacketContext::TProtocolConfigOptionV2& aPco)
/*
 * This function basically checks if secure authentication is required or not.
 * If it is needed then it sets all secure authentication related information.
 */
    {
    const TProviderInfoExt* providerInfoExt = static_cast<const TProviderInfoExt*>(iContext.Node().AccessPointConfig().FindExtension(
            STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId)));

    if (providerInfoExt == NULL)
        {
        User::Leave(KErrCorrupt);
        }

    CCommsDatIapView* iapView = CCommsDatIapView::NewLC(providerInfoExt->iProviderInfo.APId());

    TBool isDisablePlainTextAuth = EFalse;
    iapView->GetBool(KCDTIdWCDMADisablePlainTextAuth | KCDTIdOutgoingGprsRecord, isDisablePlainTextAuth);

    //If user name is not available then no authentication Required.
    if (aPco.iAuthInfo.iUsername.Length() == 0)
        {
        aPco.iAuthInfo.iProtocol = RPacketContext::EProtocolNone;
        }
    else if (isDisablePlainTextAuth)  //If Disable, CHAP will be used.
        {
        if (aPco.iAuthInfo.iPassword.Length() == 0)
            {
            User::Leave(KErrArgument);      //CHAP without password?
            }
        else
            {
            aPco.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
            CreateChallengeAndResponseForChapL(aPco);
            }
        }
    else
        {
        aPco.iAuthInfo.iProtocol = RPacketContext::EProtocolPAP;
        }

    CleanupStack::PopAndDestroy();          // CloseIapView();
    }

void TCreatePrimaryPDPCtx::CreateChallengeAndResponseForChapL(RPacketContext::TProtocolConfigOptionV2& aProtocolConfigOption)
/*
 * This function basically sets the CHAP protocol authentication such as (challenge) and (response) 
 */
    {
    //Challenge
    TTime currentTime;
    currentTime.UniversalTime();
    
    TInt64 seedValue = currentTime.Int64();

    TUint8 challenge[KGenericNifChallengeSize] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    TUint8 i=0;
    while((i < KGenericNifChallengeSize) && (aProtocolConfigOption.iChallenge.Length() < aProtocolConfigOption.iChallenge.MaxLength()))
        {
        challenge[i] = (TUint8)(Math::Rand(seedValue)%256);
        aProtocolConfigOption.iChallenge.Append(challenge[i++]);
        }

    //Response
    TInt length = 1 /*iId length */ + aProtocolConfigOption.iAuthInfo.iPassword.Length() + aProtocolConfigOption.iChallenge.Length();
    HBufC8* buf = HBufC8::NewL(length);
    CleanupStack::PushL(buf);
     
    TPtr8 ptr((TUint8*)buf->Des().Ptr(),length);
    ptr.Append(aProtocolConfigOption.iId);
    ptr.Append(aProtocolConfigOption.iAuthInfo.iPassword);
    ptr.Append(aProtocolConfigOption.iChallenge);
    
    CMD5* md5=0;
    md5 = CMD5::NewL();
    CleanupStack::PushL(md5);
    TPtrC8 Response = md5->Hash(ptr);
   
    if (Response.Length() < aProtocolConfigOption.iResponse.MaxLength())
        {
        aProtocolConfigOption.iResponse.Copy(Response);
        }
    else
        {
        // Just a defensive measure - this should not happen.
        User::Leave(KErrCorrupt);
        }

    CleanupStack::PopAndDestroy(2);     //buf, md5
    }

void TCreatePrimaryPDPCtx::DoL()
    {
    // if the provisionconfig failed, there is no way to inform the CPR of the failure
    // as the framework doesn't expect a response from provisionconfig, so error here
    // if there was a problem so that the appropriate clean up can happen.
    
    CPDPDefaultSubConnectionProvider &tNode = static_cast<CPDPDefaultSubConnectionProvider&>(iContext.Node());
    
    User::LeaveIfError(tNode.iProvisionFailure);
    
    ASSERT(tNode.iPdpFsmInterface);
    
    tNode.PostToClients<TDefaultClientMatchPolicy>(
            iContext.NodeId(),
            TCFMessage::TStateChange(
                    Elements::TStateChange(KPsdStartingConfiguration, KErrNone)).CRef(),
            TClientType(TCFClientType::ECtrlProvider));
    
    CGPRSProvision* gprsProvision = const_cast<CGPRSProvision*>(static_cast<const CGPRSProvision*>(
        tNode.AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CGPRSProvision::EUid,CGPRSProvision::ETypeId))));
    
    if (gprsProvision == NULL)
        {
        User::Leave(KErrCorrupt);
        }
    
    //retrieve QoS (should be provisioned, but can also be overriden with SetParams).
    RPacketQoS::TQoSR5Requested qosOverridenParams;
    
    const RPacketQoS::TQoSR5Requested* qosParams = NULL;
    
    if ((!tNode.iParameterBundle.IsNull()) &&\
            (!tNode.iParameterBundle.FindFamily(KSubConQoSFamily).IsNull()))
        {
        MPDPParamMapper::MapQosParamBundleToEtelL(tNode.iParameterBundle, qosOverridenParams);
        qosParams = &qosOverridenParams;
        }
    else
        {
        const CDefaultPacketQoSProvision* defaultQoSProvision = static_cast<const CDefaultPacketQoSProvision*>(
            tNode.AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CDefaultPacketQoSProvision::EUid,CDefaultPacketQoSProvision::ETypeId)));
        
        if (defaultQoSProvision)
            {
            qosParams = &defaultQoSProvision->iParams;
            }
        else
            {
            User::Leave(KErrCorrupt);
            }
    
        //Here we're taking the qos defaults from the provision info, hence skipping the iParameterBundle.
        //The lack of iParameterBundle however and the respective ERequested params is badly tolerated by the rest
        //of the code (e.g.: when subsequently raising granted params it is assumed something has been requested).
 
        // These parameter bundles are also used in a situation when a context has failed to create because of a lack
        // of modem / network resources, as the SCPR isn't torn down, the parameter bundles need to hold the 
        // information of the default QoS otherwise bad things happen [bad QoS].
        
        RCFParameterFamilyBundle newBundle;
        newBundle.CreateL();
        tNode.iParameterBundle.Open(newBundle);
        RParameterFamily family = newBundle.CreateFamilyL(KSubConQoSFamily);
       
        TInt tRelease = gprsProvision->UmtsGprsRelease();
     
        CSubConQosR99ParamSet* extRequestedR99  = NULL;
        CSubConQosR99ParamSet* extAcceptableR99 = NULL;
        CSubConQosR5ParamSet* extRequestedR5    = NULL;
        CSubConQosR5ParamSet* extAcceptableR5   = NULL;
        
        if (tRelease == TPacketDataConfigBase::KConfigRel5)
            {
            // R5 - basically R4 + a few parameters
        
            extRequestedR5 = CSubConQosR5ParamSet::NewL(family,RParameterFamily::ERequested);        
            extAcceptableR5 = CSubConQosR5ParamSet::NewL(family,RParameterFamily::EAcceptable);

            // Requested QoS Parameters
            extRequestedR5->SetSourceStatisticsDescriptor(qosParams->iSourceStatisticsDescriptor);
            extRequestedR5->SetSignallingIndicator(qosParams->iSignallingIndication);
            
            extRequestedR99 = dynamic_cast<CSubConQosR99ParamSet*>(extRequestedR5);
                 
            // Acceptable QoS Parameters
            
            extAcceptableR5->SetSourceStatisticsDescriptor(qosParams->iSourceStatisticsDescriptor);
            extAcceptableR5->SetSignallingIndicator(qosParams->iSignallingIndication);
            
            extAcceptableR99 = dynamic_cast<CSubConQosR99ParamSet*>(extAcceptableR5);
                      
            }
        else
            {
            // R4 and R99
            extRequestedR99  = CSubConQosR99ParamSet::NewL(family,RParameterFamily::ERequested);
            extAcceptableR99 = CSubConQosR99ParamSet::NewL(family,RParameterFamily::EAcceptable);
            }
        
        // Requested QoS Parameters
        extRequestedR99->SetTrafficClass(qosParams->iReqTrafficClass);
        extRequestedR99->SetDeliveryOrder(qosParams->iReqDeliveryOrderReqd);
        extRequestedR99->SetErroneousSDUDelivery(qosParams->iReqDeliverErroneousSDU);
        extRequestedR99->SetResidualBitErrorRatio(qosParams->iReqBER);
        extRequestedR99->SetSDUErrorRatio(qosParams->iReqSDUErrorRatio);
        extRequestedR99->SetTrafficHandlingPriority(qosParams->iReqTrafficHandlingPriority);
        extRequestedR99->SetTransferDelay(qosParams->iReqTransferDelay);
        extRequestedR99->SetMaxSduSize(qosParams->iReqMaxSDUSize);
        extRequestedR99->SetMaxBitrateUplink(qosParams->iReqMaxRate.iUplinkRate);
        extRequestedR99->SetMaxBitrateDownlink(qosParams->iReqMaxRate.iDownlinkRate);
        extRequestedR99->SetGuaBitrateUplink(qosParams->iReqGuaranteedRate.iUplinkRate);
        extRequestedR99->SetGuaBitrateDownlink(qosParams->iReqGuaranteedRate.iDownlinkRate);    
        
        // Acceptable QoS Parameters
        extAcceptableR99->SetTrafficClass(qosParams->iMinTrafficClass);
        extAcceptableR99->SetDeliveryOrder(qosParams->iMinDeliveryOrderReqd);
        extAcceptableR99->SetErroneousSDUDelivery(qosParams->iMinDeliverErroneousSDU);
        extAcceptableR99->SetResidualBitErrorRatio(qosParams->iMaxBER);
        extAcceptableR99->SetSDUErrorRatio(qosParams->iMaxSDUErrorRatio);
        extAcceptableR99->SetTrafficHandlingPriority(qosParams->iMinTrafficHandlingPriority);
        extAcceptableR99->SetTransferDelay(qosParams->iMaxTransferDelay);
        extAcceptableR99->SetMaxSduSize(qosParams->iMinAcceptableMaxSDUSize);
        extAcceptableR99->SetMaxBitrateUplink(qosParams->iMinAcceptableMaxRate.iUplinkRate);
        extAcceptableR99->SetMaxBitrateDownlink(qosParams->iMinAcceptableMaxRate.iDownlinkRate);
        extAcceptableR99->SetGuaBitrateUplink(qosParams->iMinGuaranteedRate.iUplinkRate);
        extAcceptableR99->SetGuaBitrateDownlink(qosParams->iMinGuaranteedRate.iDownlinkRate);               
        
        }
        
    TTFTInfo tft; //We'll use empty/thus default TFT
    
    const CImsExtProvision* imsprov = static_cast<const CImsExtProvision*>(
        tNode.AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CImsExtProvision::EUid, CImsExtProvision::ETypeId)));
    
    TRAP_IGNORE(tNode.iIsModeGsm = IsModeGsmL());
    
    switch (gprsProvision->UmtsGprsRelease())
        {
        case TPacketDataConfigBase::KConfigGPRS:
            {
            SetImsSignallingFlagL(gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigGPRS>().iProtocolConfigOption, imsprov->iImsSignalIndicator);
        
            // Only request SIP server address retrieval when network not in GSM/GPRS mode
            // e.g. UMTS/WCDMA
            if (!tNode.iIsModeGsm)
                {
                SetupSipServerAddrRetrievalL(gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigGPRS>().iProtocolConfigOption);
                }
            
            SetChapInformationL(gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigGPRS>().iProtocolConfigOption);
        
            }
            break;
        
        case TPacketDataConfigBase::KConfigRel5:
        case TPacketDataConfigBase::KConfigRel99Rel4:
            {
            SetImsSignallingFlagL(gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigR99_R4>().iProtocolConfigOption, imsprov->iImsSignalIndicator);
        
            // Only request SIP server address retrieval when network not in GSM/GPRS mode
            // e.g. UMTS/WCDMA
            if (!tNode.iIsModeGsm)
                {
                SetupSipServerAddrRetrievalL(gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigR99_R4>().iProtocolConfigOption);
                }
            
            SetChapInformationL(gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigR99_R4>().iProtocolConfigOption);
            
            }
            break;				
        }
    
    tNode.iPDPFsmContextId = tNode.iPdpFsmInterface->NewFsmContextL(tNode,SpudMan::EPrimary);
    
    tNode.PostToClients<TDefaultClientMatchPolicy>(
            iContext.NodeId(),
            TCFMessage::TStateChange(
                    Elements::TStateChange(KPsdFinishedConfiguration, KErrNone)).CRef(),
            TClientType(TCFClientType::ECtrlProvider));
    
    tNode.PostToClients<TDefaultClientMatchPolicy>(
            iContext.NodeId(),
            TCFMessage::TStateChange(
                    Elements::TStateChange(KPsdStartingActivation, KErrNone)).CRef(),
            TClientType(TCFClientType::ECtrlProvider));
    
    ASSERT(tNode.iPDPFsmContextId == KPrimaryContextId);
    tNode.iContextType=SpudMan::EPrimary;
    tNode.iPdpFsmInterface->Set(KPrimaryContextId, gprsProvision->GetScratchContextAs<TPacketDataConfigBase>());
    //Set default QoS
    tNode.iPdpFsmInterface->Set(KPrimaryContextId, *qosParams);
    //Set default TFTs
    tNode.iPdpFsmInterface->Set(KPrimaryContextId, tft); // ignore any error
    //Start the primary.
    User::LeaveIfError(tNode.iPdpFsmInterface->Input(KPrimaryContextId, SpudMan::ECreatePrimaryPDPContext));
    iContext.iNodeActivity->ClearPostedTo();
    tNode.iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
    }

DEFINE_SMELEMENT(TOverrideProvision, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TOverrideProvision::DoL()
    {
	const CBCAProvision& bcaExtension = static_cast<const CBCAProvision&>(iContext.Node().AccessPointConfig().FindExtensionL(CBCAProvision::TypeId()));
   		
    // This cannot return NULL - checked in an earlier transition
	const CTSYProvision& tsyProvision = static_cast<const CTSYProvision&>(
        iContext.Node().AccessPointConfig().FindExtensionL(CTSYProvision::TypeId()));
	
	RPacketContext::TDataChannelV2& dataChannel = iContext.Node().iTempDataChannelV2;
	iContext.Node().iPdpFsmInterface->Get(iContext.Node().iPDPFsmContextId, dataChannel);

	// The telephony etel documentation is confusing/misleading.
	// In source it states that both iChannel and iPort have been deprecated, and that iPort is undeprecated
	// and that iPort should be used.
	// Other etel documentation states that iPort is deprecated and that iChannel should be used.
	// Current CBS TA advise suggests iChannelId should be used rather than iPort
	// Legacy SPUDs and this PDP SCpr use iPort and will do this when common TSY is not used maintaining compatibility with existing licensee TSYs
	
	_LIT(KCommonTsy, "phonetsy");
	if (!tsyProvision.iTsyName.Left(KCommonTsy().Length()).CompareF(KCommonTsy))
		{
		// CTSY has been changed to return the context name + channel Id in iChannelId,
		const_cast<CBCAProvision&>(bcaExtension).SetPortName(dataChannel.iChannelId);
		}
	else
		{
		// Co-incidentally SIMTSY will return the same value in both iChannelId and iPort meaning that
		// a change here would not show a regression in our tests - YOU HAVE BEEN CAUTIONED!
		const_cast<CBCAProvision&>(bcaExtension).SetPortName(dataChannel.iPort);
    	}
	
	if (iContext.Node().iContextType != SpudMan::EMbms)
    	{
		CGPRSProvision& gprsProvision = const_cast<CGPRSProvision&>(static_cast<const CGPRSProvision&>(
	    	iContext.Node().AccessPointConfig().FindExtensionL(STypeId::CreateSTypeId(CGPRSProvision::EUid,CGPRSProvision::ETypeId))));
		iContext.Node().iPdpFsmInterface->Get(iContext.Node().iPDPFsmContextId, gprsProvision.GetScratchContextAs<TPacketDataConfigBase>());
    	}
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(
	        iContext.NodeId(),
	        TCFMessage::TStateChange(
	                Elements::TStateChange(KPsdFinishedActivation, KErrNone)).CRef(),
	                TClientType(TCFClientType::ECtrlProvider));
    }

DEFINE_SMELEMENT(TSendErrorRecoveryRequest, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TSendErrorRecoveryRequest::DoL()
	{
	TPDPMessages::TPDPFSMMessage* msg = message_cast<TPDPMessages::TPDPFSMMessage>(&iContext.iMessage);

	__ASSERT_DEBUG(msg, User::Panic(KPdpSCprPanic,  CorePanics::KPanicIncorrectMessage));
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KPdpSCprPanic, CorePanics::KPanicNoActivity));

	const RNodeInterface* controlProvider = iContext.Node().ControlProvider();
	__ASSERT_DEBUG(controlProvider, User::Panic(KPdpSCprPanic, CorePanics::KPanicNoControlProvider));

	
	TErrContext ctx(controlProvider->RecipientId(), msg->MessageId(), iContext.iNodeActivity->ActivitySigId(), Elements::TStateChange(0, msg->iValue2));

	TEErrorRecovery::TErrorRecoveryRequest rawReq(ctx);
	
	
	iContext.iNodeActivity->PostRequestTo(
		*controlProvider,
		TCFSafeMessage::TRequestCarrierEast<TEErrorRecovery::TErrorRecoveryRequest>(rawReq).CRef()
		);
	iContext.iNodeActivity->ClearPostedTo();

	iContext.Node().SetContentionRequested(ETrue);
	}



//-=========================================================
//Activiating context
//-=========================================================

DEFINE_SMELEMENT(TActivatePDPContext, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TActivatePDPContext::DoL()
    {
    ASSERT(iContext.Node().iPdpFsmInterface);
    User::LeaveIfError(iContext.Node().iPdpFsmInterface->Input(iContext.Node().iPDPFsmContextId, SpudMan::EContextActivate));

    //Expect Response
    iContext.iNodeActivity->ClearPostedTo();
    iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
    }


DEFINE_SMELEMENT(TAwaitingPDPContextActive, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingPDPContextActive::Accept()
    {
    return TAwaitingPDPFSMMessage::Accept(KContextActivateEvent);
    }

DEFINE_SMELEMENT(TModifyActivePDPContext, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TModifyActivePDPContext::DoL()
    {
    ASSERT(iContext.Node().iPdpFsmInterface);

    TBool QoSChanged=MPDPParamMapper::QoSRequested(iContext.Node().iParameterBundle);

    const RParameterFamily &IPAddressInfoFamily = iContext.Node().iParameterBundle.FindFamily(KSubConIPAddressInfoFamily);
    const CSubConIPAddressInfoParamSet* IPAddressInfoSet = IPAddressInfoFamily.IsNull() ? NULL : static_cast<const CSubConIPAddressInfoParamSet*>(
               IPAddressInfoFamily.FindParameterSet(STypeId::CreateSTypeId(CSubConIPAddressInfoParamSet::EUid,CSubConIPAddressInfoParamSet::ETypeId),RParameterFamily::ERequested));

   	TBool TFTChanged = IPAddressInfoSet?ETrue:EFalse;

   	if(QoSChanged || TFTChanged)
   	    {
        User::LeaveIfError(iContext.Node().iPdpFsmInterface->Input(iContext.Node().iPDPFsmContextId, SpudMan::EContextModifyActive));
   	    }
   	else
   	    {
        RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.NodeId(),
                TPDPMessages::TPDPFSMMessage(KContextModifyActiveEvent, KErrNone).CRef());
   	    }

    //Expect Response
    iContext.iNodeActivity->ClearPostedTo();
    iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
    }

DEFINE_SMELEMENT(TAwaitingActivePDPContextModified, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingActivePDPContextModified::Accept()
    {
    return TAwaitingPDPFSMMessage::Accept(KContextModifyActiveEvent);
    }

DEFINE_SMELEMENT(TAwaitingNegotiatedQoSRetrieved, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingNegotiatedQoSRetrieved::Accept()
    {

    return TAwaitingPDPFSMMessage::Accept(KGetNegQoSEvent);
    }

DEFINE_SMELEMENT(TGetNegotiatedQoS, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TGetNegotiatedQoS::DoL()
    {
    if (iContext.Node().iContextType == SpudMan::EMbms)
    	{
    	//just move to next step
    	RClientInterface::OpenPostMessageClose(
    			TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
    			iContext.NodeId(),
    			TPDPMessages::TPDPFSMMessage(KGetNegQoSEvent, KErrNone).CRef()
    			);
    	}
    else
    	{
    	ASSERT(iContext.Node().iPdpFsmInterface);
    	User::LeaveIfError(iContext.Node().iPdpFsmInterface->Input(iContext.Node().iPDPFsmContextId, SpudMan::EGetNegQoS));
    	}
    //Expect Response
    iContext.iNodeActivity->ClearPostedTo();
    iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
    }


DEFINE_SMELEMENT(TSendDataClientStarted, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TSendDataClientStarted::DoL()
    {
    PRStates::TSendDataClientStarted coreDCStarted(iContext);
    coreDCStarted.DoL();
    iContext.Node().LinkUp();
    }

DEFINE_SMELEMENT(TSendDataClientStopped, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TSendDataClientStopped::DoL()
    {
    PRStates::TSendDataClientStopped coreDCStopped(iContext);
    coreDCStopped.DoL();
    iContext.Node().LinkDown(coreDCStopped.iStopCode);
    }



//-=========================================================
//Setting Params
//-=========================================================
DEFINE_SMELEMENT(TSetQoS, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TSetQoS::DoL()
    {
    ASSERT(iContext.Node().iPdpFsmInterface);
    ASSERT(!iContext.Node().GetParameterBundle().IsNull());

    if (MPDPParamMapper::QoSRequested(iContext.Node().GetParameterBundle()))
        {
        RPacketQoS::TQoSR5Requested requestedParams;
        iContext.Node().iParamsRelease = MPDPParamMapper::MapQosParamBundleToEtelL(iContext.Node().GetParameterBundle(), requestedParams);
        iContext.Node().iPdpFsmInterface->Set(iContext.Node().iPDPFsmContextId, requestedParams);
        User::LeaveIfError(iContext.Node().iPdpFsmInterface->Input(iContext.Node().iPDPFsmContextId, SpudMan::EContextQoSSet));
        }
    else
        {
        //No QoS Requested. Complete this message locally to push yourself forward
        RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.NodeId(),
                TPDPMessages::TPDPFSMMessage(KContextQoSSetEvent, KErrNone).CRef());
        }

    //Expect Response
    iContext.iNodeActivity->ClearPostedTo();
    iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
    }

DEFINE_SMELEMENT(TAwaitingQoSSet, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingQoSSet::Accept()
    {
    return TAwaitingPDPFSMMessage::Accept(KContextQoSSetEvent);
    }

DEFINE_SMELEMENT(TSetTFT, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TSetTFT::DoL()
    {
    ASSERT(iContext.Node().iPdpFsmInterface);
    ASSERT(!iContext.Node().GetParameterBundle().IsNull());

	RParameterFamily ipAddressInfoFamily =
		iContext.Node().GetParameterBundle().FindFamily(KSubConIPAddressInfoFamily);
	CSubConIPAddressInfoParamSet* ipAddressInfoSet = NULL;
	if( ! ipAddressInfoFamily.IsNull())
		{
		ipAddressInfoSet = static_cast<CSubConIPAddressInfoParamSet*>(
			ipAddressInfoFamily.FindParameterSet(
				STypeId::CreateSTypeId(CSubConIPAddressInfoParamSet::EUid,CSubConIPAddressInfoParamSet::ETypeId),
					RParameterFamily::ERequested));
		}

    if (ipAddressInfoSet)
        {
        iContext.Node().iPdpFsmInterface->Set(iContext.Node().iPDPFsmContextId, iContext.Node().GetTftInfoL(ipAddressInfoSet));
        iContext.Node().iPdpFsmInterface->Set(iContext.Node().iPDPFsmContextId, iContext.Node().GetOperationCodeL(ipAddressInfoSet));
        User::LeaveIfError(iContext.Node().iPdpFsmInterface->Input(iContext.Node().iPDPFsmContextId, SpudMan::EContextTFTModify));
        }
    else
        {
        //No QoS Requested. Complete this message locally to push yourself forward
        RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.NodeId(),
                TPDPMessages::TPDPFSMMessage(KContextTFTModifiedEvent, KErrNone).CRef());
        }

    //Expect Response
    iContext.iNodeActivity->ClearPostedTo();
    iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
    }

DEFINE_SMELEMENT(TAwaitingTFTSet, NetStateMachine::MState, PDPSCprStates::TDefContext)
TBool TAwaitingTFTSet::Accept()
    {
    return TAwaitingPDPFSMMessage::Accept(KContextTFTModifiedEvent);
    }



//-=========================================================
//Setting MBMS Params (session ids)
//-=========================================================
DEFINE_SMELEMENT(TSetMbmsParameters, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TSetMbmsParameters::DoL()
    {
    ASSERT(iContext.Node().iPdpFsmInterface);
    if (iContext.Node().iContextType == SpudMan::EMbms)
		{
		/* extract session ids from MBMS Extension Set*/
		RParameterFamily mbmsInfoFamily = iContext.Node().iParameterBundle.FindFamily(KSubConChannelParamsType);
   		CSubConMBMSExtensionParamSet *mbmsExtensionSet = mbmsInfoFamily.IsNull() ? 	NULL :
   		   static_cast<CSubConMBMSExtensionParamSet *>(mbmsInfoFamily.FindParameterSet(
   		   		STypeId::CreateSTypeId(KSubConMBMSExtParamsImplUid,KSubConMBMSSessionExtParamsType), RParameterFamily::ERequested));
		if (mbmsExtensionSet)
			{
			TSessionOperatioInfo sessionInfo;
			sessionInfo.iSessionIds.Reset();
			for(int i=0 ;i < mbmsExtensionSet->GetSessionCount(); i++)
				{
				sessionInfo.iSessionIds.Append(mbmsExtensionSet->GetSessionId(i));
				}
			sessionInfo.iOperation=mbmsExtensionSet->GetOperationType();
			User::LeaveIfError(iContext.Node().iPdpFsmInterface->Set(iContext.Node().iPDPFsmContextId, sessionInfo));
			User::LeaveIfError(iContext.Node().iPdpFsmInterface->Input(iContext.Node().iPDPFsmContextId, SpudMan::EMbmsSessionUpdate));
			sessionInfo.iSessionIds.Close();
			}
		}
	}

//-=========================================================
//Events
//-=========================================================
DEFINE_SMELEMENT(TBlockedOrUnblocked, NetStateMachine::MStateFork, PDPSCprStates::TContext)
TInt TBlockedOrUnblocked::TransitionTag()
	{
	TPDPMessages::TPDPFSMMessage& pdpmsg = message_cast<TPDPMessages::TPDPFSMMessage>(iContext.iMessage);
	if (pdpmsg.iValue1 == KContextBlockedEvent)
		return KBlocked;
	else
		return KUnblocked;
	}



DEFINE_SMELEMENT(TAwaitingParamsChanged, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingParamsChanged::Accept()
    {
    return TAwaitingPDPFSMMessage::Accept(KContextParametersChangeEvent);
    }

DEFINE_SMELEMENT(TAwaitingContextBlockedOrUnblocked, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingContextBlockedOrUnblocked::Accept()
    {

    if (TAwaitingPDPFSMMessage::Accept(KContextBlockedEvent) || TAwaitingPDPFSMMessage::Accept(KContextUnblockedEvent))
        {
        //check if there is a data flow as well. Incase we are getting the notification and the Data Client has already
        //been destroyed, we will just ignore the notification. (ou1cimx1#466386)
        RNodeInterface* theOnlyDataClient = iContext.iNode.GetFirstClient<TDefaultClientMatchPolicy>(TCFClientType::EData);
        if (theOnlyDataClient == NULL)
            {
            // clear the message and return EFalse.
            iContext.iMessage.ClearMessageId();
            return EFalse;
            }
        else
            {
            return ETrue;
            }
        }
    else
        {
        return EFalse;
        }
    }

DEFINE_SMELEMENT(TForwardContextBlockedOrUnblockedToDC, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TForwardContextBlockedOrUnblockedToDC::DoL()
    {
    TPDPMessages::TPDPFSMMessage& pdpmsg = message_cast<TPDPMessages::TPDPFSMMessage>(iContext.iMessage);

	RNodeInterface* theOnlyDataClient = iContext.iNode.GetFirstClient<TDefaultClientMatchPolicy>(TCFClientType::EData);
	ASSERT(iContext.iNode.GetClientIter<TDefaultClientMatchPolicy>(TCFClientType::EData)[1] == NULL);
    ASSERT(theOnlyDataClient);

    switch (pdpmsg.iValue1)
        {
        case KContextBlockedEvent:
        theOnlyDataClient->PostMessage(iContext.NodeId(), ESock::TCFFlow::TBlock().CRef());
        break;

        case KContextUnblockedEvent:
        theOnlyDataClient->PostMessage(iContext.NodeId(), ESock::TCFFlow::TUnBlock().CRef());
        break;

        default:
        ASSERT(EFalse);
        }
    }

DEFINE_SMELEMENT(TSendDataTransferTemporarilyBlocked, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TSendDataTransferTemporarilyBlocked::DoL()
    {
    if (iContext.Node().iPDPFsmContextId == KPrimaryContextId)
        {
        iContext.Node().PostToClients<TDefaultClientMatchPolicy>(
                iContext.NodeId(),
                TCFMessage::TStateChange(
                    Elements::TStateChange(KDataTransferTemporarilyBlocked, KErrNone)).CRef(),
                    TClientType(TCFClientType::ECtrlProvider));
        }
	}

DEFINE_SMELEMENT(TSendDataTransferUnblocked, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TSendDataTransferUnblocked::DoL()
    {
    if (iContext.Node().iPDPFsmContextId == KPrimaryContextId)
        {
        iContext.Node().PostToClients<TDefaultClientMatchPolicy>(
                iContext.NodeId(),
                TCFMessage::TStateChange(
                    Elements::TStateChange(KDataTransferUnblocked, KErrNone)).CRef(),
                    TClientType(TCFClientType::ECtrlProvider));
        }
    }
	
DEFINE_SMELEMENT(TFillInGrantedParams, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TFillInGrantedParams::DoL()
    {
    ASSERT(!iContext.Node().GetParameterBundle().IsNull());
    RCFParameterFamilyBundleC& bundle = iContext.Node().GetParameterBundle();
    TBool notifySetFlag=EFalse;

	CSubConGenEventParamsGranted* event;
	RParameterFamily parFamily;

    //-=============================================
    //Qos Params
    //-=============================================
    //Fish out the qos family
    if (MPDPParamMapper::QoSRequested(bundle))
        {
        //Quite awkward CSubConnectionParameterBundle API doesn't allow browsing
        //the extensions. CSubConGenEventParamsGranted does. Hence the
        //MPDPParamMapper fills in events (and not bundles), as bundles
        //can be populated from the events and not vice versa.
        //It's unfortunate, as we're not even raising an event here.
        //CSubConGenEventParamsGranted & CSubConnectionParameterBundle
        //would benefit greatly from having a common ancestor.
		parFamily = bundle.FindFamily(KSubConQoSFamily);
		if( parFamily.IsNull() )
			{
			// really should have been found!
			User::Leave(KErrUnknown);
			}
		event = CSubConGenEventParamsGranted::NewL();
        CleanupStack::PushL(event);
        event->SetFamily (KSubConQoSFamily);
        FillInEventL(*event); //fill in the params.
        notifySetFlag = ETrue;
        }
	else if (!(parFamily = iContext.Node().iParameterBundle.FindFamily(KSubConAuthorisationFamily)).IsNull()) // check out sblp session ids granted
        {  // check out sblp familytype (KSubConAuthorisationFamily==5)         	    	   			
    			event = CSubConGenEventParamsGranted::NewL();
    			CleanupStack::PushL(event);
    			event->SetFamily (KSubConAuthorisationFamily);
    			FillInEventL(*event); //fill in the params.
    			notifySetFlag = ETrue;
        }
	else //check out mbms session ids granted
        {
        parFamily = iContext.Node().iParameterBundle.FindFamily(KSubConChannelParamsType);
        STypeId typeId = STypeId::CreateSTypeId(KSubConChannelParamsImplUid, KSubConChannelParamsType);
		if (!parFamily.IsNull() && parFamily.FindParameterSet(typeId, RParameterFamily::ERequested))
			{
			event = CSubConGenEventParamsGranted::NewL();
	        CleanupStack::PushL(event);
			event->SetFamily (KSubConChannelParamsType);
			FillInMbmsSessionIdsL(*event); //Fill Session Ids
			notifySetFlag = ETrue;
			}
		}

	if (notifySetFlag)
		{
        parFamily.ClearAllParameters(RParameterFamily::ERequested);
        parFamily.ClearAllParameters(RParameterFamily::EAcceptable);
        parFamily.ClearAllParameters(RParameterFamily::EGranted);

   		//Populate EGranted params from the event.
   		FillInGrantedL(parFamily, *event);

   		iContext.Node().NotifyClientsL(*event);
   		CleanupStack::Pop(event);
		}


    //-=============================================
    //TFT Params
    //-=============================================
	// Should use KSubConIPAddressInfoFamily in FindFamily as argument instead of EUid
	// DEF123513 FindFamily uses EUid as argument raised to fix regressions.
	RParameterFamily ipAddressInfoFamily=bundle.FindFamily(CSubConIPAddressInfoParamSet::EUid);
    if ( ! ipAddressInfoFamily.IsNull())
        {
        CSubConIPAddressInfoParamSet* requestedIPAddressInfo = CSubConIPAddressInfoParamSet::NewL(ipAddressInfoFamily, RParameterFamily::ERequested);
		TInt count = requestedIPAddressInfo->GetParamNum();
		if (count > 0)
			{
		CSubConIPAddressInfoParamSet::TSubConIPAddressInfo paramInfo(requestedIPAddressInfo->GetParamInfoL(0));
		TUint id = iContext.Node().FindPacketFilterIdL(paramInfo);

		switch(paramInfo.iState)
			{
			case CSubConIPAddressInfoParamSet::TSubConIPAddressInfo::EAdd :
					iContext.Node().NewPacketFilterAddedL(paramInfo, id);
					break;
			case CSubConIPAddressInfoParamSet::TSubConIPAddressInfo::ERemove :
					iContext.Node().PacketFilterRemovedL(id);
					break;
			case CSubConIPAddressInfoParamSet::TSubConIPAddressInfo::ENone :
					break;
			default: ;
				/** TODO: What to do with an error */
				}
			}

		ipAddressInfoFamily.ClearAllParameters(RParameterFamily::ERequested);

        }
    //something must have had been granted.
    /* [401TODO]: We have a case where applyrequest after rejoin on transfloshim will cause this bit to execute
    Since we don't have neither qos nor addrInfo parameters set it will kick this assert*/
    }

void TParamsEvent::FillInEventL(CSubConGenEventParamsGranted& aEvent)
    {
    RPacketQoS::TQoSR5Negotiated negotiatedQoS;
    iContext.Node().iPdpFsmInterface->Get(iContext.Node().iPDPFsmContextId,  negotiatedQoS);
    MPDPParamMapper::MapQosEtelToGrantedParamsL(&negotiatedQoS, aEvent, iContext.Node().iParamsRelease);
    }


void TParamsEvent::FillInMbmsSessionIdsL(CSubConGenEventParamsGranted& aEvent)
	{

	CSubConChannelParamSet* genericMbms = CSubConChannelParamSet::NewL ();
	aEvent.SetGenericSet(genericMbms);

	//Fill  List of Session Ids received in Extension Set
	TSessionOperatioInfo sessionInfo;
	User::LeaveIfError(iContext.Node().iPdpFsmInterface->Get(iContext.Node().iPDPFsmContextId, sessionInfo));
	CSubConMBMSExtensionParamSet *mbmsSessionExtn= CSubConMBMSExtensionParamSet::NewL();
	CleanupStack::PushL(mbmsSessionExtn);
	for (TInt i=0;i<sessionInfo.iSessionIds.Count();i++)
		{
		mbmsSessionExtn->SetSessionId(sessionInfo.iSessionIds[i]);
		}
	aEvent.AddExtensionSetL(mbmsSessionExtn);
   	sessionInfo.iSessionIds.Close();
	CleanupStack::Pop(mbmsSessionExtn);
	}

void TParamsEvent::FillInGrantedL(RParameterFamily& aFamily, const CSubConGenEventParamsGranted& aEvent)
    {
    //Unfortunatelly no easy way to copy a content of an event into
    //a bundle family. Using ECOM Load/Store to do that.
    //Unfortunatelly CSubConGenEventParamsGranted::GetNumExtensionSets is a non-const
    //method.
    CSubConGenEventParamsGranted& noncEvent = const_cast<CSubConGenEventParamsGranted&>(aEvent);

    //clear granted params in the bundle
    aFamily.ClearAllParameters(RParameterFamily::EGranted);
    ASSERT(aEvent.GetGenericSet());
    const CSubConParameterSet* genSetSource = aEvent.GetGenericSet();

    TInt i = 0;

    //dry run - find out the max length for the buffer.
    TInt bufLen = genSetSource->Length();
    for (i = 0; i < noncEvent.GetNumExtensionSets(); i++)
        {
        TInt extLen = aEvent.GetExtensionSet(i)->Length();
        bufLen = extLen > bufLen ? extLen : bufLen;
        }


	RBuf8 buffer;
	buffer.CreateL(bufLen);
	buffer.CleanupClosePushL();

	genSetSource->Store(buffer);
    TPtrC8 whyDoWeNeedPointerToRbufHere(buffer);
	CSubConGenericParameterSet* genSetTarget = static_cast<CSubConGenericParameterSet*>(
	                    CSubConGenericParameterSet::LoadL(whyDoWeNeedPointerToRbufHere));
	CleanupStack::PushL(genSetTarget);
    aFamily.AddParameterSetL(genSetTarget, RParameterFamily::EGranted);
    CleanupStack::Pop(genSetTarget);

    //loop thru the extensions
    for (i = 0; i < noncEvent.GetNumExtensionSets(); i++)
        {
        const CSubConExtensionParameterSet* extSetSource = aEvent.GetExtensionSet(i);
        buffer.Zero();
        extSetSource->Store(buffer);
        whyDoWeNeedPointerToRbufHere.Set(buffer);
        CSubConExtensionParameterSet* extSetTarget = static_cast<CSubConExtensionParameterSet*>(CSubConParameterSet::LoadL(whyDoWeNeedPointerToRbufHere));
        CleanupStack::PushL(extSetTarget);
        aFamily.AddParameterSetL(extSetTarget, RParameterFamily::EGranted);
        CleanupStack::Pop(extSetTarget);
        }
    CleanupStack::PopAndDestroy(&buffer);
    }


DEFINE_SMELEMENT(TRaiseParamsRejectedL, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TRaiseParamsRejectedL::DoL()
    {
    ASSERT(iContext.iNodeActivity && iContext.iNodeActivity->Error() != KErrNone);
    CSubConGenEventParamsRejected* event= CSubConGenEventParamsRejected::NewL();
    CleanupStack::PushL(event);

	if (iContext.Node().iContextType == SpudMan::EMbms)
		{
		TPDPMessages::TPDPFSMMessage& message = message_cast<TPDPMessages::TPDPFSMMessage>(iContext.iMessage);
		TInt prevOperationValue = message.iValue2;
		if ((prevOperationValue == KErrNotFound) || (prevOperationValue == KErrNotSupported ) ||
			 (prevOperationValue == KErrMbmsImpreciseServiceEntries ) ||(prevOperationValue == KErrMbmsServicePreempted) )
			{
    		event->SetFamilyId (KSubConChannelParamsType);
			}
		}
	else
		{
		event->SetFamilyId (KSubConQoSFamily);
		}
    event->SetError(iContext.iNodeActivity->Error());
    iContext.Node().NotifyClientsL(*event);
    CleanupStack::Pop(event);

	TInt err = iContext.iNodeActivity->Error();
	iContext.iNodeActivity->SetError(KErrNone);
	User::Leave(err);
	}

DEFINE_SMELEMENT(TRaiseParamsChanged, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TRaiseParamsChanged::DoL()
    {
    TPDPMessages::TPDPFSMMessage& message = message_cast<TPDPMessages::TPDPFSMMessage>(iContext.iMessage);
	TInt err=KErrNone;
    ASSERT(message.iValue2 == KErrNone);
    CSubConGenEventParamsChanged* event = NULL;
    TRAP_IGNORE(event = CSubConGenEventParamsChanged::NewL());
    if (event)
       	{
       	RParameterFamily parFamily = iContext.Node().iParameterBundle.FindFamily(KSubConChannelParamsType);
    	if (!parFamily.IsNull()) //check the changed mbms session ids
			{
			STypeId typeId = STypeId::CreateSTypeId(KSubConChannelParamsImplUid, KSubConChannelParamsType);
			CSubConChannelParamSet* mbmsSubConParamSet = parFamily.IsNull() ? NULL :
    			static_cast<CSubConChannelParamSet *>(parFamily.FindParameterSet(typeId, RParameterFamily::ERequested));
			if (mbmsSubConParamSet)
				{
				event->SetFamily (KSubConChannelParamsType);
        		event->SetError(message.iValue2);
				TRAP(err,FillInMbmsSessionIdsL(*event)); //Fill Session Ids
				}
			}
		else
			{
			parFamily = iContext.Node().iParameterBundle.FindFamily(KSubConQoSFamily);
			event->SetFamily (KSubConQoSFamily);
        	event->SetError(message.iValue2);
        	TRAP(err, FillInEventL(*event));
			}
		if (err == KErrNone)
          	{
			FillInGrantedL(parFamily, *event);
            iContext.Node().NotifyClientsL(*event);
			}
       	}
   	else
        {
        delete event;
        }
    }


DEFINE_SMELEMENT(TAwaitingNetworkStatusEvent, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingNetworkStatusEvent::Accept()
    {
    return TAwaitingPDPFSMMessage::Accept(KNetworkStatusEvent);
    }

DEFINE_SMELEMENT(TNetworkStatusEventTypeTag, NetStateMachine::MStateFork, PDPSCprStates::TContext)
TInt TNetworkStatusEventTypeTag::TransitionTag()
    {
	RPacketService::TStatus status;
	iContext.Node().iPdpFsmInterface->Get(status);
    switch (status)
        {
        case RPacketService::EStatusUnattached:
            // If status change is to unattached, but stop has been requested,
            // or we haven't been started, do nothing
            if (iContext.Node().CountActivities(ECFActivityStopDataClient) > 0
               || iContext.Node().CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData, TCFClientType::EStarted)) == 0)
                {
                return KNoTag;
                }
			return status;	
        case RPacketService::EStatusAttached:
        case RPacketService::EStatusActive:
        case RPacketService::EStatusSuspended:
        	return status;
        }
    return KNoTag;
    }

DEFINE_SMELEMENT(TSendGoneDown, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TSendGoneDown::DoL()
    {
    ASSERT(iContext.iNodeActivity);

    const TProviderInfo& providerInfo = static_cast<const TProviderInfoExt&>(iContext.Node().AccessPointConfig().FindExtensionL(
            STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId))).iProviderInfo; 
	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(
	iContext.NodeId(),
	TCFControlClient::TGoneDown(KErrDisconnected,providerInfo.APId()).CRef(),
		TClientType(TCFClientType::ECtrl));

	iContext.Node().ControlProvider()->PostMessage(
 	        iContext.NodeId(),
 	        TCFControlProvider::TDataClientGoneDown(KErrDisconnected,providerInfo.APId()).CRef());

    }


//-=========================================================
//Destroying context
//-=========================================================
DEFINE_SMELEMENT(TDestroyPDPContext, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TDestroyPDPContext::DoL()
    {
    ASSERT(iContext.Node().iPdpFsmInterface);
    if (iContext.Node().iPDPFsmContextId == KPrimaryContextId)
        {
        iContext.Node().PostToClients<TDefaultClientMatchPolicy>(
                iContext.NodeId(),
                TCFMessage::TStateChange(
                        Elements::TStateChange(KPsdStartingDeactivation, KErrNone)).CRef(),
                        TClientType(TCFClientType::ECtrlProvider));
        }

    iContext.Node().iPdpFsmInterface->Input(iContext.Node().iPDPFsmContextId, SpudMan::EContextDelete);

    //Expect Response
    iContext.iNodeActivity->ClearPostedTo();
    iContext.Node().iActivityAwaitingResponse = iContext.iNodeActivity->ActivityId();
    }

DEFINE_SMELEMENT(TCleanupFSM, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TCleanupFSM::DoL()
    {
    CPDPSubConnectionProvider &tNode = static_cast<CPDPSubConnectionProvider &>(iContext.Node());
    
    if (tNode.iPDPFsmContextId != CPDPSubConnectionProvider::EInvalidContextId)
        {
    
        // removing the memory associated with the context - this should hopefully
        // ensure that the memory allocations that happen afterwards can be
        // accomplished without OOM errors
    
        tNode.iPdpFsmInterface->DeleteFsmContext(tNode.iPDPFsmContextId);

        if (tNode.ContentionRequested() == EFalse)
            {
            CSubConGenEventSubConDown* event = CSubConGenEventSubConDown::NewL();
            CleanupStack::PushL(event);
            tNode.NotifyClientsL(*event);
            CleanupStack::Pop(event);
            }

        if (tNode.iPDPFsmContextId == KPrimaryContextId)
            {
            tNode.PostToClients<TDefaultClientMatchPolicy>(
                    iContext.NodeId(),
                    TCFMessage::TStateChange(
                            Elements::TStateChange(KPsdFinishedDeactivation, KErrNone)).CRef(),
                            TClientType(TCFClientType::ECtrlProvider));
            }   

        tNode.iPDPFsmContextId = CPDPSubConnectionProvider::EInvalidContextId;
        
        }
    }


DEFINE_SMELEMENT(TAwaitingPDPContextDestroyed, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingPDPContextDestroyed::Accept()
    {
    if (TAwaitingPDPFSMMessage::Accept(KContextDeleteEvent))
        {
        return ETrue;
        }
    return EFalse;
    }

DEFINE_SMELEMENT(TAwaitingPDPContextGoneDown, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingPDPContextGoneDown::Accept()
    {
    if (TAwaitingPDPContextDestroyed::Accept())
        {
		if (!iContext.Node().ContentionRequested())
			{
			//Following SPUD, we're ignoring the error raised by the FSM and override it with KErrDisconnected
			iContext.Node().LinkDown(KErrDisconnected);
			}
        return ETrue;
        }
    return EFalse;
    }


DEFINE_SMELEMENT(TNoTagOrProviderStopped, NetStateMachine::MStateFork, PDPSCprStates::TContext)
TInt TNoTagOrProviderStopped::TransitionTag()
    {
    if (iContext.Node().iPDPFsmContextId != CPDPSubConnectionProvider::EInvalidContextId &&
        iContext.Node().iPdpFsmInterface->FsmContextExists(iContext.Node().iPDPFsmContextId))
        {
        //context is alive
        return KNoTag;
        }
    return CoreNetStates::KProviderStopped;
    }



//===========================================================
//   Sip Address retrieval
//===========================================================
DEFINE_SMELEMENT(TAwaitingIoctlMessage, NetStateMachine::MState, PDPSCprStates::TContext)
TBool TAwaitingIoctlMessage::Accept()
	{
	if (iContext.iMessage.IsTypeOf(Meta::STypeId::CreateSTypeId(TCFSigLegacyRMessage2Ext::EUid, TCFSigLegacyRMessage2Ext::ETypeId)))
		{
		TCFSigLegacyRMessage2Ext& msg = static_cast<TCFSigLegacyRMessage2Ext&>(iContext.iMessage);
		if (msg.iMessage.Function() == ECNIoctl)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

DEFINE_SMELEMENT(TRetrieveSipAddr, NetStateMachine::MStateTransition, PDPSCprStates::TDefContext)
void TRetrieveSipAddr::DoL()
	{
	MeshMachine::CNodeParallelMessageStoreActivityBase* act = static_cast<MeshMachine::CNodeParallelMessageStoreActivityBase*>(iContext.Activity());

	ASSERT(act->Message().IsTypeOf(Meta::STypeId::CreateSTypeId(TCFSigLegacyRMessage2Ext::EUid, TCFSigLegacyRMessage2Ext::ETypeId)));
	RLegacyResponseMsg ioctl(iContext, static_cast<TCFSigLegacyRMessage2Ext&>(act->Message()).iMessage, static_cast<TCFSigLegacyRMessage2Ext&>(act->Message()).iMessage.Int0());
	
	TInt err = KErrNotFound;

	if (iContext.Node().iIsModeGsm)
		{
		err = KErrNotSupported;
		}
	else
		{
		CGPRSProvision* gprsProvision = const_cast<CGPRSProvision*>(
			static_cast<const CGPRSProvision*>(iContext.Node().AccessPointConfig().FindExtension(
												   STypeId::CreateSTypeId(CGPRSProvision::EUid,CGPRSProvision::ETypeId))));

		RPacketContext::TProtocolConfigOptionV2* protConfigOpt = NULL;
		
		if(gprsProvision != NULL)
		   {
		   switch (gprsProvision->UmtsGprsRelease())
		   	  {
	    	  case TPacketDataConfigBase::KConfigGPRS:
			     {
				 protConfigOpt = &(gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigGPRS>().iProtocolConfigOption);
				 }
			  break;
			  
	    	  case TPacketDataConfigBase::KConfigRel5:
			  case TPacketDataConfigBase::KConfigRel99Rel4:
			     {
				 protConfigOpt = &(gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigR99_R4>().iProtocolConfigOption);
				 }
			  break;
			  }
		   }
		
		if(protConfigOpt != NULL)
			{
			if (protConfigOpt->iMiscBuffer.Length() > 0)
				{
				TSipServerAddrBuf sipbuf;
				ioctl.ReadL(2, sipbuf);

				TPtr8 pcoPtr(const_cast<TUint8*>(protConfigOpt->iMiscBuffer.Ptr()),
						protConfigOpt->iMiscBuffer.Length(),
						protConfigOpt->iMiscBuffer.MaxLength());
				TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> tlv(pcoPtr,0);
				tlv.ResetCursorPos();

				TIp6Addr addr;
				TPtr8 addrPtr(NULL, 0);
				TPckg<TIp6Addr> addrPckg(addr);

				for (TInt i = 0; tlv.NextItemL(RPacketContext::EEtelPcktPCSCFAddress,addrPtr) == KErrNone; i++)
					{
					if (i == sipbuf().index)
						{
						addrPckg.Copy(addrPtr);
						sipbuf().address.SetAddress(addr);
						if (!sipbuf().address.IsUnspecified())
							err = KErrNone;
						break;
						}
					}

				ioctl.WriteL(2, sipbuf);
				}
			else
				{
				err = KErrNotFound;
				}
			}
		}

	ioctl.Complete(err);
	}


//===========================================================
// Rejoin
//===========================================================

MeshMachine::CNodeActivityBase* CPrimaryPDPGoneDownActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    {
    return new (ELeave) CPrimaryPDPGoneDownActivity(aActivitySig, aNode);
    }

CPrimaryPDPGoneDownActivity::~CPrimaryPDPGoneDownActivity()
    {
    }

CPDPSubConnectionProvider* CPrimaryPDPGoneDownActivity::NewDefault()
    {
    if (iNewDefault == NULL)
        {
    	THighestQoSQuery query(static_cast<CPDPSubConnectionProvider&>(iNode).ControlProvider()->RecipientId());
    	static_cast<CSubConnectionProviderFactoryBase&>(static_cast<CPDPSubConnectionProvider&>(iNode).Factory()).Find(query);
    	iNewDefault = query.HighestQoSProvider();
        }
    return iNewDefault;
    }

CPrimaryPDPGoneDownActivity::CPrimaryPDPGoneDownActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
:MeshMachine::CNodeActivityBase(aActivitySig, aNode)
    {
	iOriginalDataClient.SetNull();
    }


DEFINE_SMELEMENT( CPrimaryPDPGoneDownActivity::TApplyNewDefault, NetStateMachine::MStateTransition, CPrimaryPDPGoneDownActivity::TContext)
void CPrimaryPDPGoneDownActivity::TApplyNewDefault::DoL()
	{
    ASSERT(iContext.iNodeActivity);
	CPrimaryPDPGoneDownActivity* primaryGoneDownActivity = static_cast<CPrimaryPDPGoneDownActivity*>(iContext.iNodeActivity);

    RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), primaryGoneDownActivity->NewDefault()->Id(),
    	TCFScpr::TApplyRequest().CRef());
	}

DEFINE_SMELEMENT( CPrimaryPDPGoneDownActivity::TStoreOriginalDataClient, NetStateMachine::MStateTransition, CPrimaryPDPGoneDownActivity::TContext)
void CPrimaryPDPGoneDownActivity::TStoreOriginalDataClient::DoL()
	{
	ASSERT(iContext.iNodeActivity);
	CPrimaryPDPGoneDownActivity* act = static_cast<CPrimaryPDPGoneDownActivity*>(iContext.iNodeActivity);
	act->iOriginalDataClient
		= iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData))->RecipientId();
	}

DEFINE_SMELEMENT( CPrimaryPDPGoneDownActivity::TStopOriginalDataClient, NetStateMachine::MStateTransition, CPrimaryPDPGoneDownActivity::TContext)
void CPrimaryPDPGoneDownActivity::TStopOriginalDataClient::DoL()
	{
	ASSERT(iContext.iNodeActivity);
	CPrimaryPDPGoneDownActivity* act = static_cast<CPrimaryPDPGoneDownActivity*>(iContext.iNodeActivity);
	ASSERT(!act->iOriginalDataClient.IsNull());

	iContext.iNodeActivity->PostRequestTo(act->iOriginalDataClient,
		TCFDataClient::TStop(KErrCancel).CRef());
	}


DEFINE_SMELEMENT( CPrimaryPDPGoneDownActivity::TSwitchToNewDefault, NetStateMachine::MStateTransition, CPrimaryPDPGoneDownActivity::TContext)
void CPrimaryPDPGoneDownActivity::TSwitchToNewDefault::DoL()
	{
  	ASSERT(&iContext.Node() == iContext.Node().iDefaultSCPR); //only to be excecuted by the current default
	ASSERT(iContext.iNodeActivity);
	CPrimaryPDPGoneDownActivity* primaryGoneDownActivity = static_cast<CPrimaryPDPGoneDownActivity*>(iContext.iNodeActivity);
	CPDPSubConnectionProvider* newDefault = primaryGoneDownActivity->NewDefault();
	ASSERT( newDefault && newDefault != &iContext.Node());
    newDefault->RemoveClient(iContext.NodeId());

	//switch the context owner - the current owner will go down.
	iContext.Node().iPDPFsmContextId = newDefault->iPDPFsmContextId;
	newDefault->iPDPFsmContextId = CPDPSubConnectionProvider::EInvalidContextId;
	iContext.Node().iPdpFsmInterface->Set(iContext.Node().iPDPFsmContextId, iContext.Node());
	}

DEFINE_SMELEMENT( CPrimaryPDPGoneDownActivity::TRejoinDataClient, NetStateMachine::MStateTransition, CPrimaryPDPGoneDownActivity::TContext)
void CPrimaryPDPGoneDownActivity::TRejoinDataClient::DoL()
	{
  	ASSERT(&iContext.Node() == iContext.Node().iDefaultSCPR); //only to be excecuted by the current default
	ASSERT(iContext.iNodeActivity);
	CPrimaryPDPGoneDownActivity* primaryGoneDownActivity = static_cast<CPrimaryPDPGoneDownActivity*>(iContext.iNodeActivity);
	CPDPSubConnectionProvider* newDefault = primaryGoneDownActivity->NewDefault();
	ASSERT( newDefault && newDefault != &iContext.Node());
	if(!newDefault)
	{
		User::Leave(KErrNotFound);
	}
	newDefault->AddClientL(iContext.NodeId(), TClientType(TCFClientType::ECtrl));

	//migrate the flow - will need TApply
    TCFRejoiningProvider::TRejoinDataClientRequest msg(newDefault->GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData))->RecipientId(), iContext.NodeId());
    RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), newDefault->Id(), msg);
	}


DEFINE_SMELEMENT( CPrimaryPDPGoneDownActivity::TNoTagOrProviderStopped, NetStateMachine::MStateFork, CPrimaryPDPGoneDownActivity::TContext)
TInt CPrimaryPDPGoneDownActivity::TNoTagOrProviderStopped::TransitionTag()
	{
	ASSERT(iContext.iNodeActivity);
	CPrimaryPDPGoneDownActivity* primaryGoneDownActivity = static_cast<CPrimaryPDPGoneDownActivity*>(iContext.iNodeActivity);
	if (primaryGoneDownActivity->NewDefault())
    	{
    	return KNoTag;
    	}
    return CoreNetStates::KProviderStopped;
	}

DEFINE_SMELEMENT(TFillInImsExtParams, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TFillInImsExtParams::DoL()
	{
	CGPRSProvision* gprsProvision = const_cast<CGPRSProvision*>(
			static_cast<const CGPRSProvision*>(iContext.Node().AccessPointConfig().FindExtension(
					STypeId::CreateSTypeId(CGPRSProvision::EUid,CGPRSProvision::ETypeId))));
	ASSERT(gprsProvision);

	iContext.Node().iPdpFsmInterface->Get(KPrimaryContextId, gprsProvision->GetScratchContextAs<TPacketDataConfigBase>());
	// the bundle is assumed to be valid, since its created in createPdpContext
	//RParameterFamily imcnFamily = iContext.Node().GetOrCreateParameterBundleL().CreateFamilyL(KSubConnContextDescrParamsFamily); //PJLEFT

	RCFParameterFamilyBundle newBundle;
	newBundle.CreateL();
	newBundle.Open(iContext.Node().iParameterBundle);
    CleanupClosePushL(newBundle);

	RParameterFamily imcnFamily = newBundle.CreateFamilyL(KSubConnContextDescrParamsFamily);

	CSubConImsExtParamSet *imcnFlag = CSubConImsExtParamSet::NewL(imcnFamily,RParameterFamily::EGranted);

	CleanupStack::PopAndDestroy(&newBundle);

	RPacketContext::TProtocolConfigOptionV2* pco = NULL;
	switch (gprsProvision->UmtsGprsRelease())
		{
    		case TPacketDataConfigBase::KConfigGPRS:
			{
			pco = &(gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigGPRS>().iProtocolConfigOption);
			}
			break;
	    	case TPacketDataConfigBase::KConfigRel5:
	    	case TPacketDataConfigBase::KConfigRel99Rel4:
			{
			pco = &(gprsProvision->GetScratchContextAs<RPacketContext::TContextConfigR99_R4>().iProtocolConfigOption);
			}
			break;
		}

	if(pco)
		{
		TPtr8 pcoPtr(const_cast<TUint8*>(pco->iMiscBuffer.Ptr()),pco->iMiscBuffer.Length(),pco->iMiscBuffer.MaxLength());
		TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> tlv(pcoPtr,0);
		tlv.ResetCursorPos();

		TPtr8 imsFlagPtr(NULL, 0);
		TInt err = tlv.NextItemL(RPacketContext::EEtelPcktIMCNNetworkSubsystemSignallingFlag, imsFlagPtr);
		imcnFlag->SetImsSignallingIndicator(err == KErrNone);
		}

	}

//===========================================================
// Cancel Start or Stop
//===========================================================

DEFINE_SMELEMENT(TAwaitingDataClientStopOrCancel, NetStateMachine::MState, TContext)
TBool TAwaitingDataClientStopOrCancel::Accept()
    {
    CNodeActivityBase* startActivity = iContext.Node().FindActivityById(ECFActivityStartDataClient);
    if (startActivity)
        {
        //cancel start
        startActivity->Cancel(iContext);
        return EFalse;
        }

    CoreNetStates::TAwaitingDataClientStop state(iContext);
    return state.Accept();
    }

DEFINE_SMELEMENT(TCancelDataClientStartInPDP, NetStateMachine::MStateTransition, PDPSCprStates::TContext)
void TCancelDataClientStartInPDP::DoL()
    {
    CNodeActivityBase* startActivity = iContext.Node().FindActivityById(ECFActivityStartDataClient);
    if (startActivity)
        {
        //cancel start with KErrDisconnected
        startActivity->SetError(KErrDisconnected);
        startActivity->Cancel(iContext);
        }
    else
        {
        RClientInterface::OpenPostMessageClose(TNodeCtxId(ECFActivityStart, iContext.NodeId()), iContext.NodeId(), TEBase::TCancel().CRef());
        }
    }

//===========================================================
//   User Authentication
//===========================================================

DEFINE_SMELEMENT(TSendAuthenticate, NetStateMachine::MStateTransition, PDPSCprStates::TDefContext)
void TSendAuthenticate::DoL()
    {
    iContext.Node().AuthenticateL();
    }

DEFINE_SMELEMENT(TAwaitingAuthenticateComplete, NetStateMachine::MState, PDPSCprStates::TDefContext)
TBool TAwaitingAuthenticateComplete::Accept()
    {
    if (iContext.iMessage.IsMessage<TPDPMessages::TAuthenticateComplete>())
        {
        TPDPMessages::TAuthenticateComplete& msg = message_cast<TPDPMessages::TAuthenticateComplete>(iContext.iMessage);
        if (msg.iValue != KErrNone)
            {
            iContext.iNodeActivity->SetError(msg.iValue);
            }
        return ETrue;
        }
    return EFalse;
    }

} //namespace end
