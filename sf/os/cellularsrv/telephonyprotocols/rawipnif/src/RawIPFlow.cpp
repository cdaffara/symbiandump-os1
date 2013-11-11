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
#include "RawIPFlowTraces.h"
#endif

#include <f32file.h>
#include <nifman.h>
#include <nifmbuf.h>
#include <in_iface.h>
#include <nifvar.h>
#include "IPv4Binder.h"
#include "IPv6Binder.h"
#include "RawIPFlow.h"
#include <comms-infras/linkprovision.h>
#include <comms-infras/linkmessages.h>
#include <comms-infras/ss_metaconnprov.h>					// for SAccessPointConfig
#include <elements/nm_messages_base.h>
#include <elements/nm_messages_child.h>
#include <comms-infras/ss_nodemessages_flow.h>

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;

#ifdef __FLOG_ACTIVE
//These variables are used only if flog is active.
_LIT8(KTcpDumpFirstTag,"TcpDump");
static const TUint16 KTcpDumpLinkType = 12;
#endif

CRawIPFlow::CRawIPFlow(CSubConnectionFlowFactoryBase& aFactory, const TNodeId& aSubConnId, CProtocolIntfBase* aProtocolIntf)
/**
 * Constructor.
 *
 * @param aFactory Reference to the factory which created this object.
 */
	: CSubConnectionFlowBase(aFactory, aSubConnId, aProtocolIntf),
	  iInitError(KErrNone),
	  iStarted(EFalse),
	  iStopping(EFalse)
	{
	LOG_NODE_CREATE(KNifSubDir, CRawIPFlow);
	}

void CRawIPFlow::ConstructL()
/**
 * Second-phase constructor. Creates the Etel
 * control engine
 *
 * @param aName The name of the NIF (unused)
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_CONSTRUCTL_1, "CRawIPFlow %08x:\tConstructL()", this);
    iBinderControl = new (ELeave) TBinderControlProxy(*this);
	iBcaController = CBcaIoController::NewL(*this);
	}

CRawIPFlow::~CRawIPFlow()
/**
 * Destructor. Cancels and deletes all owned active objects.
 */
	{
	// Note that we don't delete iBinder because it's not owned by us.
	delete iBcaController;

	ASSERT(iBinder == NULL);
	
	delete iBinderControl;	
	LOG_NODE_DESTROY(KNifSubDir, CRawIPFlow);
	// Cleanup when stop was not called - harmless as it null's the pointer
	__PACKETLOG_DELETE;
	}

void CRawIPFlow::StartFlowL()
/**
 * Starts the Flow up. This process involves connecting to the BCA.  When startup is
 * complete, LinkLayerUp() will be called.
 *
 * Called in response to corresponding SCPR message.
 *
 * @return Allways KErrNone
 */
	{
	ASSERT(iStarting==EFalse);
	iStarting = ETrue;
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_STARTFLOWL_1, "CRawIPFlow %08x:\tStartFlowL()", this);

	// If there were any errors during earlier processing of the ProvisionConfig message
	// then send an Error message in response to the StartFlow (because ProvisionConfig
	// does not have a response code).
	if (iProvisionError != KErrNone)
		{
		User::Leave(iProvisionError);
		}

	// Retrieve Agent provisioned information only available at StartFlow time
	DynamicProvisionConfigL();
	InitialiseL(MControllerObserver::EStart, KErrNone);

	//the name calculation should be done only if logging is enabled
#ifdef __FLOG_ACTIVE
	const TUint KModemNameLen = KCommsDbSvrMaxColumnNameLength + 10;  // need enough for ppp-XXX.txt - e.g. ppp-comm-1.txt, ppp-btcomm-10.txt etc
	TBuf8<KModemNameLen> modemName;

	_LIT8(KDoubleColon, "::");

	modemName.Copy(iProvision->GetPortName());
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
	
    TRAPD(err,__PACKETLOG_NEWL(KTcpDumpFirstTag, logFileName, CPacketLogger::ETcpDump, KTcpDumpLinkType));
        if (err)
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_STARTFLOWL_2, "Trapped leave from __PACKETLOG_NEWL");
            }
	
	const TUint KLogTextLen = KModemNameLen+KTimeStampLen+30;
	TBuf8<KLogTextLen> logText;
	_LIT8(KLogTimeText, "TcpDump log file time stamp:");
	_LIT8(KLogModemText, " for modem:");
	logText.Append(KLogTimeText);
	logText.Append(logFileName);
	logText.Append(KLogModemText);
	logText.Append(modemName);
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_STARTFLOWL_3, "%s", logText);
#endif
	}


void CRawIPFlow::LinkLayerUp()
/**
 * This function is called when initialisation is complete and the Flow is
 * ready to send/receive data. It notifies NifMan and the IP interface that it's
 * ready to go.
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_LINKLAYERUP_1, "CRawIPFlow %08x:\tLinkLayerUp()", this);

	iLastRequestOriginator.ReplyTo(Id(), TCFDataClient::TStarted().CRef());


	iBinder->StartSending();
	iStarted = ETrue;
	iStarting = EFalse;
	}

void CRawIPFlow::LinkLayerDown(TInt aError)
/**
 * This function is called when the context has been deactivated and the NIF
 * is ready to be deleted, or if there is an error on startup. Its job is to
 * notify NifMan that the link has gone down, so it can delete the NIF.
 *
 * @param aError An error code to propagate to NifMan
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_LINKLAYERDOWN_1, "CRawIPFlow %08x:\tLinkLayerDown(aError %d)", (TUint)this, aError);
	
	__PACKETLOG_DELETE;
	
	if (iStopping)
		{
		iLastRequestOriginator.ReplyTo(Id(), TCFDataClient::TStopped(aError).CRef());
		}
    else if (iStarting)
		{
		iSubConnectionProvider.PostMessage(Id(), TEBase::TError(TCFServiceProvider::TStart::Id(), aError).CRef());
		}
	else
		{
		iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TDataClientGoneDown(aError, MNifIfNotify::EDisconnect).CRef());
		}

    iStarted = EFalse;
    iStarting = EFalse;
    iStopping = EFalse;
	}

void CRawIPFlow::StopFlow(TInt aError)
/**
 * This function is called by NifMan to bring down the link. It starts
 * the asynchronous context deactivation process; LinkLayerDown() will be
 * called when the context has been deactivated.
 *
 * @param aError A code specifying why the link is to be brought down
 * @param aAction The action to take: disconnect or reconnect
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_STOPFLOW_1, "CRawIPFlow %08x:\tStopFlow(aError %d)", (TUint)this, aError);
	__PACKETLOG_DELETE;
	iStopping = ETrue;
	ShutDown(MControllerObserver::EInitialised, aError);
	}

MLowerDataSender::TSendResult CRawIPFlow::SendPacket(RMBufChain& aPdu, TAny* /*aProtocol*/,
							   TUint16 /*aType*/)
/**
 * Sends a packet, via the BCA controller. This method is protocol-agnostic,
 * and is called by the IPv4 and IPv6 binder to actually send packets.
 *
 * @param aPdu The packet to send
 * @return MLowerDataSender::TSendResult
 */
	{
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_SENDPACKET_1, "CRawIPFlow %08x:\tSendPacket(): length=%d, blocked=%d", (TUint)this, aPdu.Length() - aPdu.First()->Length(),iBlocked);
    __PACKETLOG_WRITE_PACKET(aPdu, 0);
    return iBcaController->Send(aPdu);
	}

void CRawIPFlow::Process(RMBufChain& aPdu, TUint16 aProtocolCode)
/**
 * Handles incoming packets which have been passed up from the BCA controller.
 * If the IPv4 binder wants to process the packet, it hands it on; otherwise,
 * the packet is binned.
 *
 * @param aPdu The incoming packet
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_PROCESS_1, "CRawIPFlow %08x:\tProcess() [aPdu length=%d]",(TUint)this, aPdu.Length() - aPdu.First()->Length());

	// If the packet has zero or negative length, bin it.
	if ((aPdu.Length() - aPdu.First()->Length()) <= 0)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_PROCESS_2, "Dumped packet: illegal length");
		aPdu.Free();
		return;
		}

	// See if the protocol interface wants the packet.
	if (iBinder && iBinder->WantsProtocol(aProtocolCode))
		{
		__PACKETLOG_WRITE_PACKET(aPdu, 0);
		iBinder->Process(aPdu);
		}
	else
		{
		// The protocol didn't want the packet, so bin it.
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_PROCESS_3, "Packet was not processed: freeing (protocol code: %X)",aProtocolCode);
		if (iBinder == NULL)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_PROCESS_4, "CRawIPFlow %08x:\tProcess(): NOTE: Binder not present", this);
			}
		aPdu.Free();
		}
	}

void CRawIPFlow::ResumeSending()
/**
 * Notifies the protocol interface that it can resume sending packets.
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_RESUMESENDING_1, "CRawIPFlow %08x:\tResumeSending()", this);

	if (iBinder)
		{
		iBinder->StartSending();
		}
	else
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_RESUMESENDING_2, "CRawIPFlow %08x:\tResumeSending(): NOTE: Binder not present", this);
		}
	}

void CRawIPFlow::InitialiseL(TInitialisationState aState, TInt aError)
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
	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_INITIALISEL_1, "CRawIPFlow %08x:\tInitialiseL(aState %d, aError %d)",(TUint)this, aState, aError);

	if (aError != KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_INITIALISEL_2, "  *** FAILED to initialise NIF *** Error =%d",aError);

		// Initialise shutdown sequence
		switch (aState)
		{
		case MControllerObserver::EBcaController:
			ShutDown(MControllerObserver::EInitialised, aError);
			break;
		default:
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_INITIALISEL_3, "ERROR CRawIPFlow: Unknown state:%d in NIF: %S", aState, KNifName);
			User::Leave(KErrUnknown);
			break;
			}
		return;
		}

	switch (aState)
		{
	case MControllerObserver::EStart:
		{
		// Update local state from TSY information originally from Agent.
		GetBinder()->UpdateContextConfigL(*iAgentProvision->iGprsConfig);
		GetBinder()->UpdateConnectionSpeed(iAgentProvision->iConnectionSpeed);

		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_INITIALISEL_4, "Port details %S", iProvision->GetPortName());
		iBcaController->SetPort(iProvision->GetPortName());

		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_INITIALISEL_5, "bcaName details %S,%S",iProvision->GetBCAStack(), iProvision->GetBCAName());
		iBcaController->SetBcaStackAndName(iProvision->GetBCAStack(), iProvision->GetBCAName());

		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_INITIALISEL_6, "IAP ID details %u", iProvision->GetIAPid());
		iBcaController->SetIapId(iProvision->GetIAPid());

		UpdateContextState(RPacketContext::EStatusActive, KErrNone);

		iBcaController->StartL();
		}
		break;
	case MControllerObserver::EBcaController:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_INITIALISEL_7, "  ***** NIF INITIALISED *****");
		LinkLayerUp();
		break;
	default:
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_INITIALISEL_8, "ERROR CRawIPFlow: Unknown state:%d in NIF: %S", aState, KNifName);
		User::Leave(KErrUnknown);
		break;
		}
	}

void CRawIPFlow::ShutDown(TInitialisationState aState, TInt aError)
/**
 * This method controls the shutdown sequence of the different modules
 * in the Flow. When a module completes all the shutdown tasks it will
 * call again this method with the new state.
 *
 * @param aState The current state
 * @param aError A possible error (only during initialisation)
 */
	{
	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_SHUTDOWN_1, "CRawIPFlow %08x:\tShutDown(aState %d, aError %d)",(TUint)this, aState, aError);
			
	if (aError != KErrNone)
		{
		// Flow is shutting down due to an error.
		// The error code must be stored in order to call linklayer down
		// passing that value
		iInitError = aError;
		}

	switch (aState)
		{
	case MControllerObserver::EInitialised:
		iBcaController->Stop(aError);
		break;
	case MControllerObserver::EBcaController:
		LinkLayerDown(iInitError);
		break;
	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_SHUTDOWN_2, "ERROR CRawIPFlow: Unknown Shutdown step");
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_SHUTDOWN_3, "PANIC: %S %d", KNifName, KNifUnknownShutDownState);
          User::Panic(KNifName,KNifUnknownShutDownState);
		break;
		}
	}

CBinderBase* CRawIPFlow::GetBinder()
/**
 * Returns a pointer to the Binder class
 *
 * @return a pointer to a CBinderBase derived class
 */
	{
	return iBinder;
	}

CBcaIoController* CRawIPFlow::GetBcaController()
/**
 * Returns a pointer to the CBcaIoController class
 *
 * @return a pointer to CBcaIoController
 */
	{
	return iBcaController;
	}

void CRawIPFlow::UpdateContextState(
	RPacketContext::TContextStatus aState, TInt /*aError*/)
/**
 * Updates the state of the iContextState variable
 *
 * @param aState The new state
 * @param aError A possible error
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_UPDATECONTEXTSTATE_1, "CRawIPFlow %08x:\tUpdateContextState(aState %d)",(TUint) this, aState);

	// Note that we do not need to close the Flow down if there's been an
	// error, as the context state machine will do this for us.

	iContextStatus = aState;
	}

// =====================================================================================
//
// CSubConnectionFlowBase
//

MFlowBinderControl* CRawIPFlow::DoGetBinderControlL()
	{
	return iBinderControl;
	}

// =====================================================================================
//
// MFlowBinderControl methods
//

MLowerControl* CRawIPFlow::GetControlL(const TDesC8& aProtocol)
	{

	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_GETCONTROLL_1, "CRawIPFlow %08x:\tGetControlL(aProtocol %s)", (TUint)this, aProtocol);


	// IPv4 and ICMP Protocols
	if (aProtocol.CompareF(KDescIp()) == 0 || aProtocol.CompareF(KDescIcmp()) == 0)
		{
		if (!iBinder)
			{
			iBinder = new (ELeave) CIPv4Binder(*this);
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
 			iBcaController->SetType(KIp4FrameType);
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
			}
		else
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_GETCONTROLL_2, "CRawIPFlow %08x:\tGetControlL(): IPv4 binder already exists", this);
			}
		}
	// IPv6 Protocol
	else if (aProtocol.CompareF(KDescIp6()) == 0)
		{
		if (!iBinder)
			{
			iBinder = new (ELeave) CIPv6Binder(*this);
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
 			iBcaController->SetType(KIp6FrameType);
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
			}
		else
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_GETCONTROLL_3, "CRawIPFlow %08x:\tGetControlL(): IPv6 binder already exists", this);
			}
		}		
	else
		{
		User::Leave(KErrNotSupported);
		}

    return iBinder;
	}

MLowerDataSender* CRawIPFlow::BindL(const TDesC8& aProtocol, MUpperDataReceiver* aReceiver, MUpperControl* aControl)
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

    iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TActive().CRef());
	return iBinder->Bind(aReceiver, aControl);
	}

void CRawIPFlow::Unbind(MUpperDataReceiver* aUpperReceiver, MUpperControl* aUpperControl)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_UNBIND_1, "CRawIPFlow %08x:\tUnbind()", this);

	if (iBinder)
	    {
        iBinder->Unbind(aUpperReceiver, aUpperControl);

        delete iBinder;
        iBinder = NULL;
	    }

    SendDataClientIdleIfNoClients();
    }


void CRawIPFlow::SendDataClientIdleIfNoClients()
    {
    if (iBinder == NULL)
        {
        iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TIdle().CRef());
        }
    }

CSubConnectionFlowBase* CRawIPFlow::Flow()
/**
Return the Flow corresponding to the MFlowBinderControl
*/
	{
	return this;
	}

// =====================================================================================
//
// Messages::ANode
//

void CRawIPFlow::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_RECEIVEDL_1, "Realm Id [%u] Message Id [%u]", aMessage.MessageId().Realm(), aMessage.MessageId().MessageId());

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
	else if (TCFFlow::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TCFFlow::TBlock::EId :
		    {
		    // if there is a packet being sent and it completes
		    // flow control will stop sending any further packets
		    // while the flow is blocked.
		    
		    iBcaController->BlockSending();
		    }
			break;
        case TCFFlow::TUnBlock::EId :
		    {          
            // ResumeSending is specific for the unblocking the flow - it
            // cancels the flow control and restarts the flow appropriately
            // (i.e. if there is a packet outstanding, messages in the queue
            // or idle and no queue).
            iBcaController->ResumeSending();
		    }
			break;
		case TCFFlow::TRejoin::EId:
			{
			TCFFlow::TRejoin& rejoinMsg = message_cast<TCFFlow::TRejoin>(aMessage);
			TDefaultFlowFactoryQuery query(rejoinMsg.iNodeId);
			CRawIPFlow* otherFlow = static_cast<CRawIPFlow*>(Factory().Find(query));
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
	else if (TLinkMessage::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TLinkMessage::TAgentToFlowNotification::EId:
			{
			TLinkMessage::TAgentToFlowNotification& msg = static_cast<TLinkMessage::TAgentToFlowNotification&>(aMessage);
			Notification(msg.iValue);
			break;
			}
		default:
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

void CRawIPFlow::SubConnectionGoingDown()
	{
	}

void CRawIPFlow::SubConnectionError(TInt /*aError*/)
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

void CRawIPFlow::ProvisionConfig(const ESock::RMetaExtensionContainerC& aConfigData)
/**
Handle ProvisionConfig message from SCPR.

Just save the pointer for now - validate it later on StartFlow.

@param aData provisioning pointer from message
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_PROVISIONCONFIG_1, "CRawIPFlow %08x:\tProvisionConfig()", this);

	AccessPointConfig().Close();
	AccessPointConfig().Open(aConfigData);

    iProvision = static_cast<const CBCAProvision*>(AccessPointConfig().FindExtension(
        STypeId::CreateSTypeId(CBCAProvision::EUid, CBCAProvision::ETypeId)));

	}

void CRawIPFlow::DynamicProvisionConfigL()
/**
Retrieve provisioning information available only at StartFlow time.
*/
	{
    if (iProvision == NULL)
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_DYNAMICPROVISIONCONFIGL_1, "CRawIPFlow:\tProvisionConfigL() - CBCAProvision config incomplete");
		iProvisionError = KErrCorrupt;
		return;
        }

    ASSERT(iAgentProvision == NULL);
    iAgentProvision = static_cast<const CRawIpAgentConfig*>(AccessPointConfig().FindExtension(
            STypeId::CreateSTypeId(CRawIpAgentConfig::EUid, CRawIpAgentConfig::ETypeId)));

    if (iAgentProvision == NULL)
    	{
    	User::Leave(KErrCorrupt);
    	}

    const CIPConfig* wcdmaIpProvision = static_cast<const CIPConfig*>(AccessPointConfig().FindExtension(
            STypeId::CreateSTypeId(CIPConfig::EUid, CIPConfig::ETypeId)));
    if (wcdmaIpProvision == NULL)
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_DYNAMICPROVISIONCONFIGL_2, "CRawIPFlow %08x:\tDynamicProvisionConfigL() - WCDMA config incomplete", this);
		iProvisionError = KErrCorrupt;
		return;
        }
   	// Pass pointer to binder specific provisioning information to binder
	if (iBinder)
		{
		iBinder->SetProvision(*wcdmaIpProvision);
		}
	}


void CRawIPFlow::Destroy()
/**
Handle Destroy message from SCPR.
*/
	{
	DeleteThisFlow();
	}

void CRawIPFlow::Notification(TInt aEvent)
/**
Handle Generic Link Tier messages from SCPR.

These used to be implemented as Agent <-> NIF Notifications, hence the method name to promote
some historical understanding.
*/
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_NOTIFICATION_1, "CRawIPFlow %08x:\tNotification(aEvent %d)", (TUint)this, aEvent);

	switch (aEvent)
	{
	case EAgentToNifEventTypeDisableTimers: //GPRS suspension
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_NOTIFICATION_2, "CRawIPFlow::Received Suspend from Agent...");

        // Let the BCA controller know that data can no longer be sent over
        // the PDP context.
        iBcaController->BlockSending();

        break;
        }

    case EAgentToNifEventTypeEnableTimers: //GPRS resumption
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_NOTIFICATION_3, "CRawIPFlow::Received Resume from Agent...");
        iBcaController->ResumeSending();

        break;
        }
	case (EAgentToNifEventTypeDisableConnection) :
		{
		// This is handled by NIFMAN and passed to Flow as a Stop() call
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_NOTIFICATION_4, "CRawIPFlow::Received Disable connection from Agent...");
		break;
		}

	default :
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRAWIPFLOW_NOTIFICATION_5, "CRawIPFlow::Received Notification [%d] from Agent...", aEvent);
		break;
		}
	}//endswitch
	}


ESock::MLowerControl* CRawIPFlow::TBinderControlProxy::GetControlL(const TDesC8& aProtocol)
    {
    return RawIPFlow().GetControlL(aProtocol);
    }

ESock::MLowerDataSender* CRawIPFlow::TBinderControlProxy::BindL(const TDesC8& aProtocol, ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl)
    {
    return RawIPFlow().BindL(aProtocol, aReceiver, aControl);
    }

void CRawIPFlow::TBinderControlProxy::Unbind(ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl)
    {
    return RawIPFlow().Unbind(aReceiver, aControl);
    }

CSubConnectionFlowBase* CRawIPFlow::TBinderControlProxy::Flow()
    {
    return iFlow;
    }

// =====================================================================================
//
// Utility functions
//

void CRawIPFlow::PostProgressMessage(TInt aStage, TInt aError)
	{
	iSubConnectionProvider.PostMessage(Id(), TCFMessage::TStateChange(Elements::TStateChange( aStage, aError)).CRef());
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
