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
// Implements the CRawIPFlow class, which provides a central point for the NIF
// to communicate with NifMan and the BCA controllers. This class creates an interface
// to the IPv4 protocol when it is required.
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "RawIP2FlowTraces.h"
#endif

#include <f32file.h>
#include <nifman.h>
#include <nifmbuf.h>
#include <in_iface.h>
#include <nifvar.h>
#include "IPv4Binder.h"
#include "IPv6Binder.h"
#include "RawIP2Flow.h"
#include "BcaController.h"
#include <comms-infras/linkprovision.h>
#include <comms-infras/linkmessages.h>
#include <comms-infras/ss_metaconnprov.h>					// for SAccessPointConfig
#include <elements/nm_messages_base.h>
#include <elements/nm_messages_child.h>
#include <comms-infras/ss_nodemessages_flow.h>

using namespace Messages;
using namespace MeshMachine;


CRawIP2Flow::CRawIP2Flow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf)
/**
 * Constructor.
 *
 * @param aFactory Reference to the factory which created this object.
 * @param aSubConnId Reference to the node id for sub connection.
 * @param aProtocolIntf pointer to the CProtocolIntfBase
 */
	: CSubConnectionFlowBase(aFactory, aSubConnId, aProtocolIntf),
	  iInitError(KErrNone),
	  iStarted(EFalse),
	  iStopping(EFalse)
	{
	LOG_NODE_CREATE(KNifSubDir, CRawIP2Flow);
	}

void CRawIP2Flow::ConstructL()
/**
 * Second-phase constructor. Creates the Etel
 * control engine
 *
 * @param aName The name of the NIF (unused)
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_CONSTRUCTL_1, "CRawIP2Flow %08x:\tConstructL()", this);
	
    iBinderControl = new (ELeave) TBinderControlProxy(*this);
	iBcaController = CBcaController::NewL(*this);
	}

CRawIP2Flow::~CRawIP2Flow()
/**
 * Destructor. Cancels and deletes all owned active objects.
 */
	{
	// Note that we don't delete iBinder because it's not owned by us.
	delete iBcaController;

	ASSERT(iBinder == NULL);
	
	delete iBinderControl;	
	LOG_NODE_DESTROY(KNifSubDir, CRawIP2Flow);
	}

void CRawIP2Flow::StartFlowL()
/**
 * Starts the Flow up. This process involves connecting to the BCA.  
 *
 * Called in response to corresponding SCPR message.
 *
 * @return Allways KErrNone
 */
	{
	ASSERT(iStarting==EFalse);
	iStarting = ETrue;
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_STARTFLOWL_1, "CRawIP2Flow %08x:\tStartFlowL()", this);

	// If there were any errors during earlier processing of the ProvisionConfig message
	// then send an Error message in response to the StartFlow (because ProvisionConfig
	// does not have a response code).
	if (iProvisionError != KErrNone)
		{
		User::Leave(iProvisionError);
		}

	// Retrieve Agent provisioned information only available at StartFlow time
	DynamicProvisionConfigL();
	InitialiseL();
	
	// associate the binder object with the lowersender
	iBinder->SetSender(&(iBcaController->Bca()->GetSender()));
	//the name calculation should be done only if logging is enabled
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
	const TUint KModemNameLen = KCommsDbSvrMaxColumnNameLength + 10;  // need enough for ppp-XXX.txt - e.g. ppp-comm-1.txt, ppp-btcomm-10.txt etc
	TBuf8<KModemNameLen> modemName;

	_LIT8(KDoubleColon, "::");

	modemName.Copy(iBCAProvisionConfig->GetPortName());
	const TInt pos = modemName.FindC((const TDesC8&)KDoubleColon);
	if(pos != KErrNotFound)
		{
		modemName.Delete(pos, 2);
		}

	//append time stamp
	const TUint KTimeStampLen = 8;
	TBuf8<KTimeStampLen> logFileName;

	_LIT8(KTimeFormat, "%08X");
	TUint32 counter = User::FastCounter();
	logFileName.Format(KTimeFormat, counter);

	
	const TUint KLogTextLen = KModemNameLen+KTimeStampLen+30;
	TBuf8<KLogTextLen> logText;
	_LIT8(KLogTimeText, "TcpDump log file time stamp:");
	_LIT8(KLogModemText, " for modem:");
	logText.Append(KLogTimeText);
	logText.Append(logFileName);
	logText.Append(KLogModemText);
	logText.Append(modemName);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_STARTFLOWL_3, logText);
#endif
	}


void CRawIP2Flow::LinkLayerUp()
/**
 * This function is called when initialisation is complete and the Flow is
 * ready to send/receive data. It notifies NifMan and the IP interface that it's
 * ready to go.
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_LINKLAYERUP_1, "CRawIP2Flow %08x:\tLinkLayerUp()", this);

	iLastRequestOriginator.ReplyTo(Id(), ESock::TCFDataClient::TStarted().CRef());
	
	iStarted = ETrue;
	iStarting = EFalse;
	}

void CRawIP2Flow::LinkLayerDown(TInt aError)
/**
 * This function is called when the context has been deactivated and the NIF
 * is ready to be deleted, or if there is an error on startup. Its job is to
 * notify NifMan that the link has gone down, so it can delete the NIF.
 *
 * @param aError An error code to propagate to NifMan
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_LINKLAYERDOWN_1, "CRawIP2Flow %08x:\tLinkLayerDown(aError %d)", (TUint)this, aError);


	if (iStopping)
		{
		iLastRequestOriginator.ReplyTo(Id(), ESock::TCFDataClient::TStopped(aError).CRef());
		}
    else if (iStarting)
		{
		iSubConnectionProvider.PostMessage(Id(), TEBase::TError(ESock::TCFServiceProvider::TStart::Id(), aError).CRef());
		}
	else    
		{
		iSubConnectionProvider.PostMessage(Id(), ESock::TCFControlProvider::TDataClientGoneDown(aError, MNifIfNotify::EDisconnect).CRef());
		}

    iStarted = EFalse;
    iStarting = EFalse;
    iStopping = EFalse;
	}

void CRawIP2Flow::StopFlow(TInt aError)
/**
 * This function is called by NifMan to bring down the link. It starts
 * the asynchronous context deactivation process; LinkLayerDown() will be
 * called when the context has been deactivated.
 *
 * @param aError A code specifying why the link is to be brought down
 * @param aAction The action to take: disconnect or reconnect
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_STOPFLOW_1, "CRawIP2Flow %08x:\tStopFlow(aError %d)",(TUint) this, aError);
	iStopping = ETrue;
	ShutDown(aError);
	}


void CRawIP2Flow::InitialiseL()
/**
 * This method controls the initialisation sequence of the different modules
 * in the Flow. When a module completes all the initialisation tasks it will
 * call again this method with the new state. If an error occurs then the
 * ShutDown() method will be called.
 *
 * @param aState The current initialisation step
 * @param aError A possible error
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_INITIALISEL_1, "CRawIPFlow %08x:\tInitialiseL",this);

	// Update local state from TSY information originally from Agent.
	GetBinder()->UpdateContextConfigL(*iAgentProvision->iGprsConfig);
	GetBinder()->UpdateConnectionSpeed(iAgentProvision->iConnectionSpeed);

	UpdateContextState(RPacketContext::EStatusActive, KErrNone);

	iBcaController->StartLoadL(iBCAProvisionConfig,iBinder,iBinder);
	}

void CRawIP2Flow::ShutDown(TInt aError)
/**
 * This method controls the shutdown sequence of the different modules
 * in the Flow. When a module completes all the shutdown tasks it will
 * call again this method with the new state.
 *
 * @param aState The current state
 * @param aError A possible error (only during initialisation)
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_SHUTDOWN_1, "CRawIPFlow %08x:\tShutDown(aError %d)",(TUint)this, aError);
			
	if (aError != KErrNone)
		{
		// Flow is shutting down due to an error.
		// The error code must be stored in order to call linklayer down
		// passing that value
		iInitError = aError;
		}

	iBcaController->Stop(aError);
	LinkLayerDown(iInitError);
	}

CBinderBase* CRawIP2Flow::GetBinder()
/**
 * Returns a pointer to the Binder class
 *
 * @return a pointer to a CBinderBase derived class
 */
	{
	return iBinder;
	}

CBcaController* CRawIP2Flow::GetBcaController()
/**
 * Returns a pointer to the CBcaIoController class
 *
 * @return a pointer to CBcaIoController
 */
	{
	return iBcaController;
	}

void CRawIP2Flow::UpdateContextState(
	RPacketContext::TContextStatus aState, TInt /*aError*/)
/**
 * Updates the state of the iContextState variable
 *
 * @param aState The new state
 * @param aError A possible error
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_UPDATECONTEXTSTATE_1, "CRawIP2Flow %08x:\tUpdateContextState(aState %d)", (TUint)this, aState);

	// Note that we do not need to close the Flow down if there's been an
	// error, as the context state machine will do this for us.

	iContextStatus = aState;
	}

// =====================================================================================
//
// CSubConnectionFlowBase
//

ESock::MFlowBinderControl* CRawIP2Flow::DoGetBinderControlL()
	{
	return iBinderControl;
	}

// =====================================================================================
//
// MFlowBinderControl methods
//

ESock::MLowerControl* CRawIP2Flow::GetControlL(const TDesC8& aProtocol)
	{

	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_GETCONTROLL_1, "CRawIP2Flow %08x:\tGetControlL(aProtocol %s)", (TUint) this, aProtocol);

	// IPv4 and ICMP Protocols
	if (aProtocol.CompareF(KDescIp()) == 0 || aProtocol.CompareF(KDescIcmp()) == 0)
		{
		if (!iBinder)
			{
			iBinder = new (ELeave) CIPv4Binder(*this);
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS			
 			iBinder->SetType(KIp4FrameType);
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS			
			}
		else
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_GETCONTROLL_2, "CRawIP2Flow %08x:\tGetControlL(): IPv4 binder already exists", this);
			}
		}
	// IPv6 Protocol
	else if (aProtocol.CompareF(KDescIp6()) == 0)
		{
		if (!iBinder)
			{
			iBinder = new (ELeave) CIPv6Binder(*this);
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS			
 			iBinder->SetType(KIp6FrameType);
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS						
			}
		else
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_GETCONTROLL_3, "CRawIP2Flow %08x:\tGetControlL(): IPv6 binder already exists", this);
			}
		}		
	else
		{
		User::Leave(KErrNotSupported);
		}
	
    return iBinder;
	}

ESock::MLowerDataSender* CRawIP2Flow::BindL(const TDesC8& aProtocol, ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl)
/**
 * Binds upper CFProtocol to this CFProtocol
 *
 * @param aUpperReceiver A pointer to Upper layer Receive class
 * @param aUpperControl A pointer to Upper layer control class
 */
	{
#ifndef _DEBUG
	(void) aProtocol;
#endif

	ASSERT (aProtocol.CompareF(KDescIp()) == 0 || aProtocol.CompareF(KDescIcmp()) == 0
			|| aProtocol.CompareF(KDescIp6()) == 0);
	ASSERT(iBinder);
	
    iSubConnectionProvider.PostMessage(Id(), ESock::TCFControlProvider::TActive().CRef());
	return iBinder->Bind(aReceiver, aControl);
	}

void CRawIP2Flow::Unbind(ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_UNBIND_1, "CRawIP2Flow %08x:\tUnbind()", this);

	ASSERT(iBinder);
	iBinder->Unbind(aUpperReceiver, aUpperControl);

    delete iBinder;
    iBinder = NULL;

    SendDataClientIdleIfNoClients();
    }


void CRawIP2Flow::SendDataClientIdleIfNoClients()
    {
    if (iBinder == NULL)
        {
        iSubConnectionProvider.PostMessage(Id(), ESock::TCFControlProvider::TIdle().CRef());
        }
    }

ESock::CSubConnectionFlowBase* CRawIP2Flow::Flow()
/**
Return the Flow corresponding to the MFlowBinderControl
*/
	{
	return this;
	}

// =====================================================================================
//
// MCFNode
//

void CRawIP2Flow::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
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
	else if (ESock::TCFDataClient::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case ESock::TCFDataClient::TStart::EId :
			StartFlowL();
			break;
		case ESock::TCFDataClient::TStop::EId :
			StopFlow(static_cast<ESock::TCFDataClient::TStop&>(aMessage).iValue);
			break;
		case ESock::TCFDataClient::TProvisionConfig::EId:
			ProvisionConfig(static_cast<ESock::TCFDataClient::TProvisionConfig&>(aMessage).iConfig);
			break;
		case ESock::TCFDataClient::TBindTo::EId:
            {
            ESock::TCFDataClient::TBindTo& bindToReq = message_cast<ESock::TCFDataClient::TBindTo>(aMessage);
			if (!bindToReq.iNodeId.IsNull())
				{
				User::Leave(KErrNotSupported);
				}
			RClientInterface::OpenPostMessageClose(Id(), aSender, ESock::TCFDataClient::TBindToComplete().CRef());
            }
			break;
		default:
//TODO - logging
			ASSERT(EFalse);
			}
		}
	else if (ESock::TCFFlow::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case ESock::TCFFlow::TBlock::EId :
			iBlocked = ETrue;
			break;
		case ESock::TCFFlow::TUnBlock::EId :
			iBlocked = EFalse;
			if (iBinder)
				{
				iBinder->StartSending();
				}
			break;
		case ESock::TCFFlow::TRejoin::EId:
			{
			ESock::TCFFlow::TRejoin& rejoinMsg = message_cast<ESock::TCFFlow::TRejoin>(aMessage);
			ESock::TDefaultFlowFactoryQuery query(rejoinMsg.iNodeId);
			CRawIP2Flow* otherFlow = static_cast<CRawIP2Flow*>(Factory().Find(query));
			ASSERT(otherFlow);

			//This rather ugly looking snipped of code implements
			//the scenario whereat the network tears down the default
			//scpr and the default scpr selects a new default amongst
			//the non-default and subsequently performs a transparant
			//swap - i.e.: the upper layer perceives this as a
			//non-default going down. Since the datapath is pre-bound
			//to the flow representing the default, the datapath needs
			//to perform a transparent swap too, so that the upper layer
			//doesn't notice anything. It does that by swapping the
			//flows below the binders.
			CBinderBase* localBinder = iBinder;

			TBinderControlProxy* localBinderControl = iBinderControl;
			iBinder = otherFlow->iBinder;
			otherFlow->iBinder->ChangeFlow(*this);
			localBinder->ChangeFlow(*otherFlow);
			otherFlow->iBinder = localBinder;
			iBinderControl = otherFlow->iBinderControl;
			otherFlow->iBinderControl = localBinderControl;
			otherFlow->iBinderControl->iFlow = otherFlow;
			iBinderControl->iFlow = this;


			iSubConnectionProvider.Close();
			iSubConnectionProvider.Open(address_cast<TNodeId>(rejoinMsg.iNodeId));
			}
			break;
		default:
//TODO - logging
			ASSERT(EFalse);
			}
		}
	else	// not a recognised realm or message
		{
		ASSERT(EFalse);
		}
    }


// =====================================================================================
//
// Methods for handling incoming SCPR messages
//

void CRawIP2Flow::SubConnectionGoingDown()
	{
	}

void CRawIP2Flow::SubConnectionError(TInt /*aError*/)
	{
	}

/*
Provisioning description for RawIp CFProtocol Flow:

- on receipt of the ProvisionConfig message, the pointer contained within is stored
  in iAccessPointConfig and the provisioning information contained within it is validated:
	- CBCAProvision must be present.  It is added by the RawIpMCPr and populated from CommsDat.  A pointer to it
	  is stored in iProvision.
  if any are missing, an error is stored in iProvisionError and sent in response to the StartFlow
  message later on (ProvisionConfig has no response).

- on receipt of the StartFlow message, further provisioning information is retrieved:
	- CIPConfig must be present.  It is added by the RawIpMCPr and populated from CommsDat.  The binder retrieves
	  individual fields from it.
	- CRawIpAgentConfig must be present.  It is added by the RawIpAgentHandler and populated via calls
	  to the Agent.  A pointer to it is stored in iAgentProvision.  Note that the iGprsConfig field
	  is a pointer to a (large) structure in the Agent (see CRawIpAgentHandler).
  if any of the above are missing, an Error message is signalled back to the SCPr in response
  to the StartFlow.
*/

void CRawIP2Flow::ProvisionConfig(const ESock::RMetaExtensionContainerC& aConfigData)
/**
Handle ProvisionConfig message from SCPR.

Just save the pointer for now - validate it later on StartFlow.

@param aData provisioning pointer from message
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_PROVISIONCONFIG_1, "CRawIP2Flow %08x:\tProvisionConfig()", this);
	iProvisionError = KErrNone;

	AccessPointConfig().Close();
	AccessPointConfig().Open(aConfigData);

	ASSERT(iBCAProvisionConfig == NULL);
    iBCAProvisionConfig = static_cast<const CBCAProvision*>(AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CBCAProvision::EUid, CBCAProvision::ETypeId)));

    if (iBCAProvisionConfig == NULL)
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_PROVISIONCONFIG_2, "CRawIP2Flow:\tProvisionConfigL() - CBCAProvision config incomplete");
		iProvisionError = KErrCorrupt;
		return;
        }
	}

void CRawIP2Flow::DynamicProvisionConfigL()
/**
Retrieve provisioning information available only at StartFlow time.
*/
	{
	ASSERT(iAgentProvision == NULL);
    iAgentProvision = static_cast<const CRawIpAgentConfig*>(AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CRawIpAgentConfig::EUid, CRawIpAgentConfig::ETypeId)));

    if (iAgentProvision == NULL)
    	{
    	User::Leave(KErrCorrupt);
    	}

    const CIPConfig* wcdmaIpProvision = static_cast<const CIPConfig*>(AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CIPConfig::EUid, CIPConfig::ETypeId)));
    if (wcdmaIpProvision == NULL)
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIP2FLOW_DYNAMICPROVISIONCONFIGL_1, "CRawIP2Flow %08x:\tDynamicProvisionConfigL() - WCDMA config incomplete", this);
		iProvisionError = KErrCorrupt;
		return;
        }
   	// Pass pointer to binder specific provisioning information to binder
	if (iBinder)
		{
		iBinder->SetProvision(*wcdmaIpProvision);
		}
	}


void CRawIP2Flow::Destroy()
/**
Handle Destroy message from SCPR.
*/
	{
	DeleteThisFlow();
	}

ESock::MLowerControl* CRawIP2Flow::TBinderControlProxy::GetControlL(const TDesC8& aProtocol)
    {
    return RawIP2Flow().GetControlL(aProtocol);
    }
    
ESock::MLowerDataSender* CRawIP2Flow::TBinderControlProxy::BindL(const TDesC8& aProtocol, ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl)
    {
    return RawIP2Flow().BindL(aProtocol, aReceiver, aControl);
    }
    
void CRawIP2Flow::TBinderControlProxy::Unbind(ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl)
    {
    return RawIP2Flow().Unbind(aReceiver, aControl);
    }

ESock::CSubConnectionFlowBase* CRawIP2Flow::TBinderControlProxy::Flow()
    {
    return iFlow;
    }
    	
// =====================================================================================
//
// Utility functions
//

void CRawIP2Flow::PostProgressMessage(TInt aStage, TInt aError)
	{
	iSubConnectionProvider.PostMessage(Id(), ESock::TCFMessage::TStateChange(Elements::TStateChange( aStage, aError)).CRef());
	}

//
// NOTE:
// Attribute table should be in the control side module that instantiates CRawIpProvision.
// RawIP CFProtocol does not instantiate the CRawIpProvision object, so does not need to
// define the attribute tables.  The attribute table can therefore be placed directly into
// the control side module.
//
// The reason for this is to save splitting RawIP CFProtocol into two DLL's - ECOM and Main -
// just for control side module to link against Main DLL to retrieve the attribute table.
// In fact, it can be placed into control side module as "START_ATTRIBUTE_TABLE" rather
// than "EXPORT_START_ATTRIBUTE_TABLE".  In fact, do we just need "TYPEID_TABLE" ?
//
// EXPORT_START_ATTRIBUTE_TABLE(CRawIpProvision, KRawIpConfigUid, KRawIpConfigType)
// END_ATTRIBUTE_TABLE()
