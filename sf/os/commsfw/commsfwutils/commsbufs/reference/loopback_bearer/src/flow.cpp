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
#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <elements/nm_messages_base.h>
#include <elements/nm_messages_child.h>

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include "ss_subconnflow.h"
#else
#include <comms-infras/ss_subconnflow.h>
#endif

#include <comms-infras/ss_log.h>
#include "flow.h"
#include "binder.h"
#include "provision.h"

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;

_LIT8(KNif,"legacy_loopback_link_protocol");

CLegacyLoopbackSubConnectionFlowFactory* CLegacyLoopbackSubConnectionFlowFactory::NewL(TAny* aConstructionParameters)
/**
Constructs a Default SubConnection Flow Factory

@param aConstructionParameters construction data passed by ECOM

@returns pointer to a constructed factory
*/
	{
	CLegacyLoopbackSubConnectionFlowFactory* ptr = new (ELeave) CLegacyLoopbackSubConnectionFlowFactory(TUid::Uid(KLegacyLoopbackFlowImplementationUid), *(reinterpret_cast<CSubConnectionFlowFactoryContainer*>(aConstructionParameters)));
	return ptr;
	}


CLegacyLoopbackSubConnectionFlowFactory::CLegacyLoopbackSubConnectionFlowFactory(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer)
:	CSubConnectionFlowFactoryBase(aFactoryId, aParentContainer)
/**
Default SubConnection Flow Factory Constructor

@param aFactoryId ECOM Implementation Id
@param aParentContainer Object Owner
*/
	{
	}


CSubConnectionFlowBase* CLegacyLoopbackSubConnectionFlowFactory::DoCreateFlowL(CProtocolIntfBase* aProtocolIntf, TFactoryQueryBase& aQuery)
	{
	const TDefaultFlowFactoryQuery& query = static_cast<const TDefaultFlowFactoryQuery&>(aQuery);
	return CLegacyLoopbackSubConnectionFlow::NewL(*this, query.iSCprId, aProtocolIntf);
	}


//=======================================================================================
// CLegacyLoopbackSubConnectionFlow
//

CLegacyLoopbackSubConnectionFlow::CLegacyLoopbackSubConnectionFlow(CSubConnectionFlowFactoryBase& aFactory, const TNodeId& aSubConnId, CProtocolIntfBase* aProtocolIntf)
:	CSubConnectionFlowBase(aFactory, aSubConnId, aProtocolIntf)
	{
    __FLOG_OPEN(KCFNodeTag, KNif);
    LOG_NODE_CREATE(KNif, CLegacyLoopbackSubConnectionFlow);
    }

CLegacyLoopbackSubConnectionFlow::~CLegacyLoopbackSubConnectionFlow()
/**
Destroys 'this'
*/
    {
    ASSERT(iBinder4 == NULL);
    ASSERT(iBinder6 == NULL);

    LOG_NODE_DESTROY(KNif, CLegacyLoopbackSubConnectionFlow);
    __FLOG_CLOSE;
    }

CLegacyLoopbackSubConnectionFlow* CLegacyLoopbackSubConnectionFlow::NewL(CSubConnectionFlowFactoryBase& aFactory, const TNodeId& aSubConnId, CProtocolIntfBase* aProtocolIntf)
	{
	CLegacyLoopbackSubConnectionFlow* flow = new (ELeave) CLegacyLoopbackSubConnectionFlow(aFactory, aSubConnId, aProtocolIntf);
	return flow;
	}

// Methods called from Binders

void CLegacyLoopbackSubConnectionFlow::FlowDown(TInt aError, TInt aAction /*= MNifIfNotify::EDisconnect*/)
/**
Binder requesting a FlowDown() message be sent to SCPR
*/
    {
	PostFlowDownMessage(aError, aAction);
	iAgentProvision = NULL;
    }

void CLegacyLoopbackSubConnectionFlow::Progress(TInt aStage, TInt aError)
/**
Binder requesting a Progress() message be sent to SCPR
*/
    {
    PostProgressMessage(aStage, aError);
    }

const TLegacyLoopbackIp6Provision* CLegacyLoopbackSubConnectionFlow::Ip6Provision() const
/**
Return a pointer to the IPv6 provisioning structure
*/
	{
	ASSERT(iProvision);
	return &iProvision->iIp6Provision;
	}

const TLegacyLoopBackAgentProvision* CLegacyLoopbackSubConnectionFlow::AgentProvision() const
/**
Return a pointer to the Agent provisioning structure
*/
	{
	ASSERT(iAgentProvision);
	return iAgentProvision;
	}

//-=========================================================
// Messages::ANode methods
//-=========================================================

void CLegacyLoopbackSubConnectionFlow::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
/**
Method called on incoming SCPR messages

@param aCFMessage message base
*/
    {
    CSubConnectionFlowBase::ReceivedL(aSender, aRecipient, aMessage);
	if (aMessage.IsMessage<TEBase::TError>())
		{
		SubConnectionError(static_cast<TEBase::TError&>(aMessage).iValue);
		}
	else if (TEChild::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TEChild::TDestroy::EId :
			Destroy();
			break;
		default:
//TODO - logging
			ASSERT(EFalse);
			}
		}
	else if (TCFDataClient::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TCFDataClient::TStart::EId :
			StartFlowL();
			break;
		case TCFDataClient::TStop::EId :
			StopFlow(static_cast<TCFDataClient::TStop&>(aMessage).iValue);
			break;
		case TCFDataClient::TProvisionConfig::EId:
			ProvisionConfig(static_cast<TCFDataClient::TProvisionConfig&>(aMessage).iConfig);
			break;
		case TCFDataClient::TBindTo::EId:
            {
			TCFDataClient::TBindTo& bindToReq = message_cast<TCFDataClient::TBindTo>(aMessage);
			if (!bindToReq.iNodeId.IsNull())
				{
				User::Leave(KErrNotSupported);
				}
			RClientInterface::OpenPostMessageClose(Id(), aSender, TCFDataClient::TBindToComplete().CRef());
            }
			break;
		default:
//TODO - logging
			ASSERT(EFalse);
			}
		}
	else		// realm != TCFMessage::ERealmId
		{
		Panic(EUnexpectedMessage);
		}
    }

//
// Methods for handling received SCPR messages
//

void CLegacyLoopbackSubConnectionFlow::StartFlowL()
	{
    __FLOG(_L8("CLegacyLoopbackSubConnectionFlow:\tStartFlowL()"));
    if (iDisableStart)
    	{
    	User::Leave(KErrCouldNotConnect);
    	}

    if (iFlowStarted)
    	{
    	//Flow can be started multiple times.
    	//For example a start may fail on an upper layer in which case the upper layer will not be able
    	//to stop our layer and we need to accept start again.
    	PostProgressMessage(KLinkLayerOpen, KErrNone);
    	PostDataClientStartedMessage();
    	return;
    	}

    iFlowStarted = ETrue;

	// If the processing of the ProvisionConfig message failed earlier, send the error response
	// here to the StartFlow message as there is no response to ProvisionConfig.

	User::LeaveIfError(iSavedError);

	AgentProvisionConfigL();		// retrieve and validate the Agent provisioning information

	PostProgressMessage(KLinkLayerOpen, KErrNone);
	PostDataClientStartedMessage();

	if (iBinder4)
   	    {
		iBinder4->BinderReady();
       	}
	if (iBinder6)
		{
		iBinder6->BinderReady();
		}
	}

void CLegacyLoopbackSubConnectionFlow::StopFlow(TInt aError)
	{
    __FLOG_1(_L8("CLegacyLoopbackSubConnectionFlow:\tStopFlow(%d)"), aError);

	PostProgressMessage(KLinkLayerClosed, aError);
    iFlowStarted = EFalse;
	PostFlowDownMessage(aError);
	iAgentProvision = NULL;
	}


/*
Provisioning description for Dummy CFProtocol Flow:

- on receipt of the ProvisionConfig message, the pointer contained within it is stored
  in iAccessPointConfig and the information contained within the iAccessPointConfig
  array is validated:
	- CDummyProtoProvision must be present.  It is added by the DummyProtoMCPr and populated from CommsDat.
	  It is a 'C' class to take advantage of zero initialisation.
  Any errors are saved in iSavedError - there is no response to ProvisionConfig, so an error
  response is sent later to StartFlow message.

- on receipt of TCFDataClient::TStart:
  - iSavedError is checked and, if non-zero, an Error message is sent to the SCPr
  - TLegacyLoopBackAgentProvision must be present.  It is added by the DummyAgentHandler and populated via calls
	  to the Agent.  It is a 'T' class because it requires no zero initialisation.  If missing,
	  an Error message is signalled to the SCPr.
*/

void CLegacyLoopbackSubConnectionFlow::ProvisionConfig(const ESock::RMetaExtensionContainerC& aConfigData)
/**
Save the provisioning structure pointer passed in the TProvisionConfig message

@param aData pointer to provisioning structure
*/
	{
	__FLOG_0(_L8("CLegacyLoopbackSubConnectionFlow:\tProvisionConfig message received"));

	iAccessPointConfig.Close();
	iAccessPointConfig.Open(aConfigData);

	const CLegacyLoopbackProvision* provision = static_cast<const CLegacyLoopbackProvision*>(AccessPointConfig().FindExtension(
		STypeId::CreateSTypeId(CLegacyLoopbackProvision::EUid, CLegacyLoopbackProvision::ETypeId)));
	if (provision == NULL)
		{
		__FLOG_0(_L8("CLegacyLoopbackSubConnectionFlow:\tProvisionConfigL() - config not found"));
		iSavedError = KErrCorrupt;
		}

	// Save pointer to provisioning information
	iProvision = provision;
	}


void CLegacyLoopbackSubConnectionFlow::AgentProvisionConfigL()
/**
Validate and process the Agent provisioning information.

This information is not valid when TProvisionConfig message is received, but only
becomes valid at TCFDataClient::TStart.
*/
	{
    const TLegacyLoopBackAgentProvision* agentProvision = static_cast<const TLegacyLoopBackAgentProvision*>(
		AccessPointConfig().FindExtension(
			STypeId::CreateSTypeId(TLegacyLoopBackAgentProvision::EUid, TLegacyLoopBackAgentProvision::ETypeId)));

    if (agentProvision == NULL)
        {
        __FLOG_0(_L8("CLegacyLoopbackSubConnectionFlow:\tProvisionConfigL() - DummyProto Agent config not found"));
		User::Leave(KErrCorrupt);
        }

	ASSERT(iAgentProvision == NULL);
	iAgentProvision = agentProvision;
	}

void CLegacyLoopbackSubConnectionFlow::Destroy()
/**
Request from SCPR to destroy
*/
	{
	// No-one should be bound to us from above if we are about to disappear.
	ASSERT(iBinder4 == NULL && iBinder6 == NULL);
	DeleteThisFlow();
	}

void CLegacyLoopbackSubConnectionFlow::SubConnectionGoingDown()
	{
    __FLOG(_L8("CLegacyLoopbackSubConnectionFlow:\tSubConnectionGoingDown"));
	}

void CLegacyLoopbackSubConnectionFlow::SubConnectionError(TInt /*aError*/)
	{
    __FLOG(_L8("CLegacyLoopbackSubConnectionFlow:\tSubConnectionError"));
	}

MFlowBinderControl* CLegacyLoopbackSubConnectionFlow::DoGetBinderControlL()
/**
Return MFlowBinderControl instance.

Called by upper layer for binding

@return MFlowBinderControl instance
*/
	{
    __FLOG(_L8("CLegacyLoopbackSubConnectionFlow::DoGetBinderControlL"));
	return this;
	}

//-=========================================================
// MFlowBinderControl methods
//

MLowerControl* CLegacyLoopbackSubConnectionFlow::GetControlL(const TDesC8& aProtocol)
/**
Create and return an MLowerControl instance of the specified binder type.

Called from upper layer during binding procedure.

@param aProtocol Protocol type of the binder
@return MLowerControl instance of the protocol type
*/
	{

	MLowerControl* lowerControl = NULL;

	if (aProtocol.CompareF(KProtocol4()) == 0)
		{
        __FLOG(_L8("CLegacyLoopbackSubConnectionFlow:\tGetLowerControlL(KProtocol4)"));
		iBinder4 = CLegacyLoopbackBinder4::NewL(*this);
		lowerControl = iBinder4;
		}
	else
	if (aProtocol.CompareF(KProtocol6()) == 0)
		{
        __FLOG(_L8("CLegacyLoopbackSubConnectionFlow::GetLowerControlL(KProtocol6)"));

        iBinder6 = CLegacyLoopbackBinder6::NewL(*this);
		lowerControl = iBinder6;
		}

	ASSERT(lowerControl);
	return lowerControl;
	}

MLowerDataSender* CLegacyLoopbackSubConnectionFlow::BindL(const TDesC8& aProtocol, MUpperDataReceiver* aReceiver, MUpperControl* aControl)
/**
Create and return an MLowerDataSender instance of the specified protocol type.
This is bound to the specified upper layer objects.

Called from upper layer to bind to this layer.

@param aProtocol Protocol type of the binder (same as in GetControlL())
@param aReceiver upper layer's MUpperDataReceiver instance for this binder to associate with
@param aControl upper layer's MUpperControl instance for this binder to associate with
@return MLowerDataSender instance
*/
	{
    __FLOG(_L8("CLegacyLoopbackSubConnectionFlow:\tBindL()"));

	MLowerDataSender* lowerDataSender = NULL;

	if (aProtocol.CompareF(KProtocol4()) == 0)
		{
		lowerDataSender = iBinder4->Bind(*aReceiver, *aControl);
		}
	else
	if (aProtocol.CompareF(KProtocol6()) == 0)
		{
		lowerDataSender = iBinder6->Bind(*aReceiver, *aControl);
		}

    iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TActive().CRef());
	ASSERT(lowerDataSender);
	return lowerDataSender;
	}

void CLegacyLoopbackSubConnectionFlow::Unbind(MUpperDataReceiver* aReceiver, MUpperControl* aControl)
/**
Unbind from the upper layer.

Called from the upper layer during unbinding.

@param aReceiver
@param aControl

*/
	{
    __FLOG(_L8("CLegacyLoopbackSubConnectionFlow:\tUnbind()"));

	if (iBinder4 && iBinder4->MatchesUpperControl(aControl))
		{
		iBinder4->Unbind(*aReceiver, *aControl);
		delete iBinder4;
		iBinder4 = NULL;
		}
	else if (iBinder6 && iBinder6->MatchesUpperControl(aControl))
		{
		iBinder6->Unbind(*aReceiver, *aControl);
		delete iBinder6;
		iBinder6 = NULL;
		}


    MaybePostDataClientIdle();
	}

ESock::CSubConnectionFlowBase* CLegacyLoopbackSubConnectionFlow::Flow()
/**
Return the flow object corresponding to the MFlowBinderControl
*/
	{
	return this;
	}

//
// Utilities for posting SCPR messages
//

void CLegacyLoopbackSubConnectionFlow::PostProgressMessage(TInt aStage, TInt aError)
	{
	iSubConnectionProvider.PostMessage(Id(), TCFMessage::TStateChange(Elements::TStateChange(aStage, aError)).CRef());
	}

void CLegacyLoopbackSubConnectionFlow::PostDataClientStartedMessage()
	{
	iLastRequestOriginator.ReplyTo(Id(), TCFDataClient::TStarted().CRef());
	}

void CLegacyLoopbackSubConnectionFlow::PostFlowDownMessage(TInt aError, TInt aAction /*= MNifIfNotify::EDisconnect*/)
	{
	if (iFlowStarted)
		{
		iFlowStarted = EFalse;
		iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TDataClientGoneDown(aError, aAction).CRef());
		}
	else
		{
		iLastRequestOriginator.ReplyTo(Id(), TCFDataClient::TStopped(aError).CRef());
		}
	iAgentProvision = NULL;
	}

void CLegacyLoopbackSubConnectionFlow::MaybePostDataClientIdle()
    {
    // Can only send DataClientIdle when the upper layer has unbound and the flow is stopped
	if (iBinder4 == NULL && iBinder6 == NULL)
		{
   		iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TIdle().CRef());
		}
    }

//
// Other utilities
//

void Panic(TLegacyLoopbackPanicNum aNum)
	{
	_LIT(KLegacyLoopbackPanicString, "legacy_loopback");
	User::Panic(KLegacyLoopbackPanicString, aNum);
	}

