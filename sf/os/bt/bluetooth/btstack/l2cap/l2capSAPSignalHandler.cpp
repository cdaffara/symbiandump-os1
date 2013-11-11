// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the handling of l2cap signal packets (both inbound and outbound) from SAP interface
// 
//

#include <bt_sock.h>
#include <bluetooth/logger.h>


#include "l2capSAPSignalHandler.h"
#include "l2capSigStates.h"
#include "l2capSigPacketConnection.h"
#include "l2capSigPacketConfigure.h"
#include "l2capSigPacketDisconnection.h"
#include "l2capSigPacketEcho.h"
#include "l2capSigPacketCommandReject.h"
#include "l2util.h"
#include "l2sap.h"
#include "l2cap.h"

#include "l2capEntityConfig.h"
#include "l2signalmgr.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

/*static*/ CL2CapSAPSignalHandler* CL2CapSAPSignalHandler::NewL(CL2CAPConnectionSAP& aSAP)
	{
	LOG_STATIC_FUNC
	CL2CapSAPSignalHandler* self = new(ELeave) CL2CapSAPSignalHandler(aSAP);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// Disable warning WINS 4355: 'this' : used in base member initializer list
// This will not cause any problems in this usage and is preferable to the use of a
// non-owned pointer.
#pragma warning (disable: 4355)
CL2CapSAPSignalHandler::CL2CapSAPSignalHandler(CL2CAPConnectionSAP& aSAP)
 : CL2CapSignalHandler(),
   iSAP(&aSAP),
   iLocalBTAddress(TBTDevAddr(0)),
   iDefaultRTXTimerDuration(HL2CapCommand::KDefaultRTXTimerDuration),
   iERTXTimerDuration(HL2CapCommand::KDefaultERTXTimerDuration),
   iSigState(&aSAP.Protocol().SigStateFactory().GetState(CL2CAPSignalStateFactory::EClosed)),
   iSAPSignalHandlerTimerState(ETimerIdle)
	{
	LOG_FUNC
	}
#pragma warning (default: 4355)

void CL2CapSAPSignalHandler::ConstructL()
	{
	LOG_FUNC
	iChannelConfig = CL2CapChannelConfig::NewL(*this);
	LOG2(_L("CL2CapSAPSignalHandler.iChannelConfig = %X.%X"), (TAny*)this, (TAny*)iChannelConfig)
	}

CL2CapSAPSignalHandler::~CL2CapSAPSignalHandler()
	{
	LOG_FUNC
	DetachFromMux();
	delete iChannelConfig;
	
	__ASSERT_DEBUG(iMuxer == NULL, Panic(EL2CAPSigHandlerDeletedWithResources));
	// If our ConstructL leaves we will be deleted with iSAP non-NULL.  This is ok as
	// the SAP will also be deleted as part of the cleanup process.  Because it's useful
	// to keep this assert for the normal case we test iChannelConfig (which may never be
	// NULL after ConstructL) to detect the case where ConstructL has left and iSAP may
	// validly be non-NULL
	__ASSERT_DEBUG((iSAP == NULL) || (iChannelConfig == NULL), Panic(EL2CAPSigHandlerDeletedWithResources));
	}

TInt CL2CapSAPSignalHandler::ConstructConnectionResponse(TUint8 aId, TConnectResponseResult aResult, TConnectResponseStatus aStatus/*= EConnectPendNoFurtherInfo*/)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	
	HL2CapCommand* command = HConnectionResponse::New(aId, iLocalPort, iRemotePort, aResult, aStatus);
	if(command)
		{
		AddToOutgoingQueue(command);
		}
	else
		{
		rerr = KErrNoMemory;
		}
	return rerr;
	}

TInt CL2CapSAPSignalHandler::ConstructConnectionRequest()
	{
	LOG_FUNC
	TInt rerr = KErrNone;

	HL2CapCommand* command = HConnectionRequest::New(iLocalPort, iRemotePort, CurrentRTXTimerDuration(iDefaultRTXTimerDuration), iERTXTimerDuration);
	if(command)
		{
		AddToOutgoingQueue(command);
		}
	else
		{
		rerr = KErrNoMemory;
		}
	return rerr;
	}


TInt CL2CapSAPSignalHandler::ConstructConfigRequest()
	{
	LOG_FUNC
	TInt rerr = KErrNone;

	HL2CapCommand* command = HConfigureRequest::New(RemotePort(), *iChannelConfig, CurrentRTXTimerDuration(iDefaultRTXTimerDuration), iERTXTimerDuration);
	if(command)
		{
		AddToOutgoingQueue(command);
		}
	else
		{
		rerr = KErrNoMemory;
		}
	return rerr;
	}

/**
	Used for constructing a configuration response for unknown parameters
	@internalComponent
**/
TInt CL2CapSAPSignalHandler::ConstructConfigResponse(TUint8 aId, TConfigFlags aFlags, TConfigResponseResult aResult, RMBufChain& aUnknownParameters)
	{
	LOG_FUNC
	TInt rerr = KErrNone;

	HConfigureResponse* command = HConfigureResponse::New(aId, RemotePort(), aFlags, aResult);
	if(command)
		{
		command->AddUnknownOptionsToCommand(aUnknownParameters);
		AddToOutgoingQueue(command);
		}
	else
		{
		rerr = KErrNoMemory;
		}
	return rerr;
	}


TInt CL2CapSAPSignalHandler::ConstructConfigResponse(TUint8 aId, TConfigFlags aFlags, TConfigResponseResult aResult)
	{
	LOG_FUNC
	TInt rerr = KErrNone;

	HConfigureResponse* command = HConfigureResponse::New(aId, RemotePort(), aFlags, aResult);
	if(command)
		{
		// Only add options if this is not a response to a Config Request
		// with the C-flag set.
		if(aFlags & KConfigOptionContinuationMask)
			{
			command->WriteDataLength();
			}
		else if (aResult == EConfigSuccess || aResult == EConfigUnacceptableParams)
			{
			// Add the options
			rerr = command->AddOptionsToCommand(*iChannelConfig, aResult);
			if(rerr != KErrNone)
				{
				delete command;
				command = NULL;
				}
			}
		
		if(command)
			{
			AddToOutgoingQueue(command);
			}
		}
	else
		{
		rerr = KErrNoMemory;
		}
	return rerr;
	}
	
TInt CL2CapSAPSignalHandler::ConstructDisconnectRequest()
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	
	HL2CapCommand* command = HDisconnectRequest::New(iLocalPort, iRemotePort, CurrentRTXTimerDuration(iDefaultRTXTimerDuration), iERTXTimerDuration);
	if(command)
		{
		AddToOutgoingQueue(command);
		}
	else
		{
		rerr = KErrNoMemory;
		}
	return rerr;
	}
	
TInt CL2CapSAPSignalHandler::ConstructDisconnectResponse(TUint8 aId)
	{
	LOG_FUNC
	TInt rerr = KErrNone;

	HL2CapCommand* command = HDisconnectResponse::New(aId, iLocalPort, iRemotePort);
	if(command)
		{
		AddToOutgoingQueue(command);
		}
	else
		{
		rerr = KErrNoMemory;
		}
	return rerr;
	}


TBool CL2CapSAPSignalHandler::HandleConnectionResponse(HConnectionResponse* aConnectionResponse)
	{
	LOG_FUNC
	TBool rcode = EFalse;
	
	// Check the response is for this CID.
	if(aConnectionResponse->SourceCID() == iLocalPort)
		{
		// Check that the Connection Response has been requested.
		HL2CapCommand* requestCommand = FindMatchingOutstandingRequest(EConnectionRequest, aConnectionResponse->ID());
		if(requestCommand)
			{
			// If this has a pending result.  Re-start the the Extended RTX timer
			// on the Request.
			if(aConnectionResponse->Result() == EConnectPending)
				{
				requestCommand->StartRTXTimer(HL2CapCommand::EERTXTimer, *this);
				}
			else
				{
				// Delete the request.
				delete requestCommand;
				}

			iSigState->ConnectResponse(*this, aConnectionResponse);		// the state m/c will update the Destination CID
			}
		else
			{
			LOG(_L("CL2CapSAPSignalHandler::HandleConnectionResponse, no matching req"));
			}

		// NOTE: If there was no Connection Request outstanding
		// (i.e., requestCommand == NULL) but the command IS for this
		// CID this could be a duplicate response, one that has arrived in
		// the window between RTX expiry and retransmission, or just bogus.  
		// Silently discard the packet.			
		rcode = ETrue;
		}

	return rcode;
	}


TBool CL2CapSAPSignalHandler::HandleConfigureRequest(HConfigureRequest* aConfigRequest)
	{
	LOG_FUNC
	TBool rcode = EFalse;
	
	// Check the response is for this CID.
	if(aConfigRequest->DestinationCID() == iLocalPort)
		{
		iSigState->ConfigRequest(*this, aConfigRequest);
		rcode = ETrue;
		}

	return rcode;
	}	
	

TBool CL2CapSAPSignalHandler::HandleConfigureResponse(HConfigureResponse* aConfigResponse)	
	{
	LOG_FUNC
	TBool rcode = EFalse;
	
	// Check the response is for this CID.
	if(aConfigResponse->SourceCID() == iLocalPort)
		{
		// Check that the Config Response has been requested.
		HL2CapCommand* requestCommand = FindMatchingOutstandingRequest(EConfigureRequest, aConfigResponse->ID());
		if(requestCommand)
			{
			// Delete the corresponding request.
			delete requestCommand;

			iSigState->ConfigResponse(*this, aConfigResponse);
			}
		// NOTE: If there was no Configure Request outstanding
		// (i.e., requestCommand == NULL) this could be a duplicate 
		// response, or just bogus.  Silently discard the packet.			
		rcode = ETrue;			
		}
		
	return rcode;
	}

TBool CL2CapSAPSignalHandler::HandleDisconnectRequest(HDisconnectRequest* aDisconnectRequest)
	{
	LOG_FUNC
	TBool rcode = EFalse;
	
	// Check the response is for this CID.
	if(aDisconnectRequest->DestinationCID() == iLocalPort &&
	   aDisconnectRequest->SourceCID() == iRemotePort)
		{	
		iSigState->DisconnectRequest(*this, aDisconnectRequest->ID());
		rcode = ETrue;
		}
		
	return rcode;
	}
	
TBool CL2CapSAPSignalHandler::HandleDisconnectResponse(HDisconnectResponse* aDisconnectResponse)
	{
	LOG_FUNC
	TBool rcode = EFalse;
	
	// Check the response is for this CID.
	if(aDisconnectResponse->DestinationCID() == iRemotePort &&
	   aDisconnectResponse->SourceCID() == iLocalPort)
		{	
		// Check that the Disconnect Response has been requested.
		HL2CapCommand* requestCommand = FindMatchingOutstandingRequest(EDisconnectionRequest, aDisconnectResponse->ID());
		if(requestCommand)
			{
			// Delete the corresponding request.
			delete requestCommand;

			iSigState->DisconnectResponse(*this);			
			}
		// NOTE: If there was no Disconnect Request outstanding
		// (i.e., requestCommand == NULL) this could be a duplicate 
		// response, or just bogus.  Silently discard the packet.			
		rcode = ETrue;			
		}
		
	return rcode;
	}
	

TBool CL2CapSAPSignalHandler::HandleCommandReject(HCommandReject* aCommandReject)
	{
	LOG_FUNC
	TBool rcode = EFalse;
	
	// Process the incoming command reject data
	TL2CAPCommandRejectData rejectData;
	
	if (aCommandReject->RejectData(rejectData) != KErrNone) return rcode;

	switch(rejectData.iReason)
		{
		case EInvalidCID:
			{
			TBool forThisChannel = EFalse;
			
			// To be considered as 'for this channel', either both pairs should match
			// OR only one of the endpoints should be present and match.
			if(rejectData.iLocalEndpoint != 0 &&
			   rejectData.iLocalEndpoint == iRemotePort)
				{
				forThisChannel = ETrue;
				}
				
			if(rejectData.iRemoteEndpoint != 0)
				{
				if(rejectData.iRemoteEndpoint == iLocalPort)
			   		{
			   		forThisChannel = ETrue;
			   		}
				else
					{
			   		forThisChannel = EFalse;
			   		}
				}

			if(forThisChannel)
				{
				LOG1(_L("CL2CapSAPSignalHandler::HandleCommandReject(): forThisChannel = ETrue for commandID = 0x%04x"),aCommandReject->ID())
				LOG2(_L("rejectData.iLocalEndpoint = 0x%04x, iRemotePort = 0x%04x"),rejectData.iLocalEndpoint,iRemotePort)
				LOG2(_L("rejectData.iRemoteEndpoint = 0x%04x, iLocalPort = 0x%04x"),rejectData.iRemoteEndpoint,iLocalPort)
				
				// If there is no matching command then this rejection is not for this handler
				if (FindMatchingOutstandingRequest(EMatchAnyL2CAPRequest,aCommandReject->ID()))
					{
					// The channel has been disconnected or is not
					// recognised by the peer.  Error the channel.
					iSigState->Error(*this, KErrL2PeerRejectedCommand, MSocketNotify::EErrorAllOperations);
					rcode = ETrue;
					}
				else
					{
					LOG(_L("CL2CapSAPSignalHandler::HandleCommandReject(): No matching command is found, skipping it"))
					}
				}
			break;
			}
		default:
			break;
		};
					
	return rcode;
	}
	
void CL2CapSAPSignalHandler::CloneChannelConfig(CL2CapChannelConfig& aL2CAPChannelConfig)
	{
	LOG_FUNC
	iChannelConfig->CloneChannelConfig(aL2CAPChannelConfig);
	}


TInt CL2CapSAPSignalHandler::UpdateChannelConfig(const TL2CapConfig& aAPIConfig)
	{
	LOG_FUNC
	return iSigState->UpdateChannelConfig(*this, aAPIConfig);
	}

void CL2CapSAPSignalHandler::SetState(TL2CAPSigState& aState)
	{
	LOG_FUNC
	LOG2(_L("CL2CapSAPSignalHandler::SetState 0x%08x -> 0x%.8x"), iSigState, &aState);
	
	iSigState->Exit(*this);
	iSigState = &aState;
	iSigState->Enter(*this);
	}

void CL2CapSAPSignalHandler::SetLocalPort(TL2CAPPort aPort)
	{
	LOG_FUNC
	LOG1(_L("Setting local port to %d"), aPort);
	iLocalPort = aPort;
	}

void CL2CapSAPSignalHandler::SetRemotePort(TL2CAPPort aPort)
	{
	LOG_FUNC
	LOG1(_L("Setting remote port to %d"), aPort);
	iRemotePort = aPort;
	}

void CL2CapSAPSignalHandler::SetRemName(TL2CAPSockAddr& aAddr)
	{
	LOG_FUNC
	SetRemotePort(aAddr.Port());
	}

void CL2CapSAPSignalHandler::GetRemName(TL2CAPSockAddr& aAddr) const
	{
	LOG_FUNC
	aAddr.SetBTAddr(RemoteBTAddress());
	aAddr.SetPort(iRemotePort);
	}

void CL2CapSAPSignalHandler::SetLocalParameters(TL2CAPSockAddr& aAddr)
	{
	LOG_FUNC
	iLocalBTAddress = aAddr.BTAddr();
	LOGBTDEVADDR(iLocalBTAddress);	
	SetLocalPort(aAddr.Port());
	}

void CL2CapSAPSignalHandler::GetLocalParameters(TL2CAPSockAddr& aAddr) const
	{
	LOG_FUNC
	aAddr.SetBTAddr(iLocalBTAddress);
	aAddr.SetPort(iLocalPort);
	}
	
TInt CL2CapSAPSignalHandler::SetOption(TUint aLevel, TUint aName, const TDesC8& aOption)
	{
	LOG_FUNC
	// iMuxer may be NULL
	if (iMuxer != NULL)
		{
		return iMuxer->SetOption(aLevel, aName, aOption);
		}
	else
		{
		return KErrNotReady;
		}
	}
	
	
TInt CL2CapSAPSignalHandler::GetOption(TUint aLevel, TUint aName, TDes8& aOption) const
	{
	LOG_FUNC
	// iMuxer may be NULL
	if (iMuxer != NULL)
		{
		return iMuxer->GetOption(aLevel, aName, aOption);
		}
	else
		{
		return KErrNotReady;
		}
	}

void CL2CapSAPSignalHandler::SAPSignalHandlerRegistered(CL2CAPMux& aMuxer, TL2CAPEntityConfig& aEntityConfig)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iMuxer == NULL, Panic(EL2CAPSAPSignalHandlerRegisteredTwice));
	iMuxer = &aMuxer;
	iSAP->SetRemoteDev(iMuxer->RemoteBTAddr());
	iChannelConfig->RegisterL2CapEntityConfig(aEntityConfig);
	}


// Indicates that the baseband is up and going and that all the objects are tied together ok.
void CL2CapSAPSignalHandler::LinkUp()
	{
	LOG_FUNC
	iSAP->LinkUp();
	}

void CL2CapSAPSignalHandler::Error(TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction)
	{
	LOG_FUNC
	iSigState->Error(*this, aErrorCode, aErrorAction);
	}
	
//Called by the state machine when we go into channel reconfiguration
void CL2CapSAPSignalHandler::ReconfiguringChannel()
	{
	LOG_FUNC
	iSAP->ReconfiguringChannel();
	}

void CL2CapSAPSignalHandler::SignalHandlerDisconnectedCanClose()
	{
	LOG_FUNC
	// If the SAP exists signal the disconnect.  Otherwise delete this
	// signal handler.
	if(iSAP)
		{
		iSAP->ChannelClosed();
		}
	else
		{
		delete this;
		}
	}

void CL2CapSAPSignalHandler::SignalHandlerErrorD(TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction)
	{
	LOG_FUNC
	// If the SAP exists signal the error.  Otherwise delete this
	// signal handler.
	if(iSAP)
		{
		iSAP->SignalHandlerError(aErrorCode, aErrorAction);
		}
	else
		{
		delete this;
		}
	}

//Called by the link signal handler 	
TInt CL2CapSAPSignalHandler::PassiveConnectionRequest(const TBTDevAddr& aAddr,HConnectionRequest* aConnectionRequest)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	CL2CAPConnectionSAP *clonedSAP = NULL;

	TUint8 id = aConnectionRequest->ID();
	TL2CAPPort scid = aConnectionRequest->SourceCID(); 

	// Check if the SAP is ready to accept a connection.
	// Note.  If the SAP is not in the listening state this
	// will return with the not ready error code.
	if(!iSAP->CanAcceptConnection())
		{
		rerr = KErrNotReady;
		}
	else
		{
		if((clonedSAP = iSAP->CloneListeningSAP(aAddr)) == NULL)
			{
			rerr = KErrNoMemory;
			}
		else
			{
			CL2CapSAPSignalHandler& clonedSH = clonedSAP->SignalHandler();

			clonedSH.iRemotePort = scid;
			clonedSH.iOutstandingRequestID = id;

			clonedSH.CloneChannelConfig(*iChannelConfig);
			rerr = iSAP->Protocol().MuxController().AttachPassiveSignalHandler(clonedSH, aAddr);
			}
		}
	return rerr;
	}

void CL2CapSAPSignalHandler::ActiveConnectionRequest()
	{
	LOG_FUNC
	iMuxer->CompleteACLConnect(NULL);
	}

void CL2CapSAPSignalHandler::L2CapEntityConfigUpdated()
 	{
 	LOG_FUNC
 	if (iOpenChannelRequestAwaitingPeerEntityConfig)
 		{
 		// SAP state machine requested that we open the channel before we could do it.
 		// We can do it now.
 		iOpenChannelRequestAwaitingPeerEntityConfig = EFalse;
 		iSigState->OpenChannel(*this);
 		}
 	}

TBool CL2CapSAPSignalHandler::DelayConfigRequest()
	{
	LOG_FUNC
	// The whole delaying of sending of ConfigRequest is basically a workaround for
	// certain broken carkits from a certain Swedish manufacturer at a certain stretch
	// of time.
	// The defect was: the carkits advertised their capability to do RTM in Extended
	// Feature Mask, but when sent a ConfigRequest with RTM they would just panic
	// (ie. they really only did Basic mode). 
	// So the workaround is to delay the sending of our Config Request such that
	// we receive the carkit's request first - we have an optimization which will
	// downgrade our requested channel mode to Basic if the remote requests it before
	// we've sent out our ConfigReq. This way we send Basic mode in our ConfigReq
	// and the carkit is happy.
	return iSAPSignalHandlerTimerState==EConfigRequestDelayTimer;
	}

void CL2CapSAPSignalHandler::ConfigRequestDelayTimerExpiry()	
 	{
 	LOG_FUNC
 	// Now that ConfigReq delay timer have expired we can start the proper configuration timer.
 	StartConfigurationTimer();

 	if (!iAwaitConfigureChannelRequest)
 		{
		ConfigureChannel();
 		}
 	// [else]: we're in an active open scenario and SAP hasn't yet issued a ConfigureChannelRequest.
 	}

void CL2CapSAPSignalHandler::CloseChannelRequest()
	{
	LOG_FUNC
	iSigState->CloseChannel(*this);
	}

void CL2CapSAPSignalHandler::OpenChannelRequest()
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iOpenChannelRequestAwaitingPeerEntityConfig,
				   Panic(EL2CAPOpenChannelRequestCalledTwice));

	if (IsPeerInfoDefined())
		{
		iSigState->OpenChannel(*this);
		}
	else
		{
		iOpenChannelRequestAwaitingPeerEntityConfig = ETrue;
		// We'll realize the OpenChannelRequest as soon as we receive
		// the Information Response.
		}
	}

void CL2CapSAPSignalHandler::ConnectRequestReceived()
	{
	LOG_FUNC
	iSigState->ConnectRequestReceived(*this);
	}

void CL2CapSAPSignalHandler::ConfigureChannelRequest()
	{
	LOG_FUNC
	iAwaitConfigureChannelRequest = EFalse;

	if (iSAPSignalHandlerTimerState != EConfigRequestDelayTimer)
		{
		ConfigureChannel();
		}
	}

void CL2CapSAPSignalHandler::PendingCommandsDrained()
	{
	LOG_FUNC
	iSigState->PendingCommandsDrained(*this);
	}

// SAP signal handler timer methods.	
/*static*/ TInt CL2CapSAPSignalHandler::TimerExpired(TAny* aSAPSignalHandler)
	{
	LOG_STATIC_FUNC
	CL2CapSAPSignalHandler* sh = reinterpret_cast<CL2CapSAPSignalHandler*>(aSAPSignalHandler);
	sh->HandleTimerExpired();
	return EFalse;
	}

void CL2CapSAPSignalHandler::StartConfigurationTimer()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iSAPSignalHandlerTimerState == ETimerIdle,
				   Panic(EL2CapAttemptToStartConfigTimerWhenItIsRunning));

	TCallBack cb(TimerExpired, this);
	iSAPSignalHandlerTimerEntry.Set(cb);
	BTSocketTimer::Queue(KL2ConfigWatchdogTimeout * KL2ProtocolSecondTimerMultiplier, 
	                     iSAPSignalHandlerTimerEntry);
	iSAPSignalHandlerTimerState = EConfigurationTimer;
	}

void CL2CapSAPSignalHandler::StartConfigRequestDelayTimer()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iSAPSignalHandlerTimerState == ETimerIdle,
				   Panic(EL2CapAttemptToStartConfigTimerWhenItIsRunning));

	TCallBack cb(TimerExpired, this);
	iSAPSignalHandlerTimerEntry.Set(cb);
	BTSocketTimer::Queue(KL2ConfigRequestDelayTimout, 
						 iSAPSignalHandlerTimerEntry);
	iSAPSignalHandlerTimerState = EConfigRequestDelayTimer;
	}

void CL2CapSAPSignalHandler::CancelTimer()
	{
	LOG_FUNC
	if(iSAPSignalHandlerTimerState != ETimerIdle)
		{
		BTSocketTimer::Remove(iSAPSignalHandlerTimerEntry);
		iSAPSignalHandlerTimerState = ETimerIdle;
		}
	}

void CL2CapSAPSignalHandler::HandleTimerExpired()
	{
	LOG_FUNC
	switch(iSAPSignalHandlerTimerState)
		{
		case ETimerIdle:
			Panic(EL2CAPSSHTimerExpiredWhileInIdleState);
			break;
	
		case EConfigRequestDelayTimer:
			{
			iSAPSignalHandlerTimerState = ETimerIdle;
			ConfigRequestDelayTimerExpiry();
			}
			break;

		case EConfigurationTimer:
			iSAPSignalHandlerTimerState = ETimerIdle;
			iSigState->ConfigurationTimerExpiry(*this);
			break;

		default:
			Panic(EL2CAPInvalidSAPSHTimerState);
			break;
		};		
	}

void CL2CapSAPSignalHandler::DetachFromMux()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iPendingCommands.IsEmpty(), Panic(EL2CAPSigHandlerDeletedWithResources));
	__ASSERT_DEBUG(iCommandsAwaitingResponse.IsEmpty(), Panic(EL2CAPSigHandlerDeletedWithResources));

	// If this is a listening Socket then it will not have
	// an associated muxer to detach.
	if(iMuxer)
		{
		iLink.Deque();
		
		iMuxer->DetachFromMux(*this);

		// Check to see if the stack needs to be brought down
		iMuxer->MuxController().Protocol().TryToClose();
		
		iMuxer = NULL;

		// If the SAP's Config object has registered with the Entity
		// Config object then remove this association.
		iChannelConfig->DetachChannelConfig();
		}
	}
	
void CL2CapSAPSignalHandler::SAPClosed()
	{
	LOG_FUNC
	// The SAP is about to be detached from this signal handler.
	// Ensure no overrides are active.
	UndoOverrideParkMode();
	UndoOverrideLPM();
	
	// Check if this SH has any unsent commands
	// outstanding.
	iSAP = NULL;
	if(iMuxer == NULL)	
		{
		delete this;
		}
	}
	
void CL2CapSAPSignalHandler::CommandResponseFailure(HL2CapCommand* aCommand)
	{
	LOG_FUNC
	// Pass on this error to the state and delete the command.
	iSigState->Error(*this, KErrL2CAPRequestTimeout, MSocketNotify::EErrorAllOperations);
	delete aCommand;
	}
	
TInt CL2CapSAPSignalHandler::SendEchoRequest(const TDes8* aData)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	if(iMuxer)
		{
		// Check to ensure that the echo payload isn't too big to send
		if ((!aData) || (aData->Size() <= iMuxer->SigMTU() - HEchoRequest::KEchoRequestEmptyLength))
			{
			rerr = iMuxer->SendEchoRequest(aData, *this);
			}
		else
			{
		    rerr = KErrTooBig;
			}
		}
	else
		{
		rerr = KErrDisconnected;
		}
	return rerr;
	}

void CL2CapSAPSignalHandler::EchoResponseReceived(const TDesC8* aData)
	{
	LOG_FUNC
	if(iSAP)
		{
		iSAP->EchoResponseReceived(aData);
		}
	}

TInt CL2CapSAPSignalHandler::SetRTXTimerDuration(TUint8 aDuration)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	if(aDuration > 0 && aDuration < HL2CapCommand::KMaxRTXTimerDuration)
		{
		iDefaultRTXTimerDuration = aDuration;
		}
	else
		{
		rerr = KErrArgument;	
		}
	return rerr;
	}
	
TInt CL2CapSAPSignalHandler::SetERTXTimerDuration(TUint16 aDuration)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	if(aDuration > 0 && aDuration < HL2CapCommand::KMaxERTXTimerDuration)
		{
		iERTXTimerDuration = aDuration;
		}
	else
		{
		rerr = KErrArgument;	
		}
	return rerr;	
	}
	
void CL2CapSAPSignalHandler::OverrideParkMode()
	{
	LOG_FUNC
	// Override Park mode.  A reference to the SAP is required
	// to get the remote device address.
	if(iSAP)
		{
		iSAP->Protocol().ControlPlane().ModifyPhysicalLink(EOverridePark, iSAP->RemoteDev());
		}
	}
	
void CL2CapSAPSignalHandler::UndoOverrideParkMode()
	{
	LOG_FUNC
	// Remove Park mode override.  A reference to the SAP is required
	// to get the remote device address.
	if(iSAP)
		{
		iSAP->Protocol().ControlPlane().ModifyPhysicalLink(EUndoOverridePark, iSAP->RemoteDev());
		}
	}

void CL2CapSAPSignalHandler::OverrideLPM()
	{
	LOG_FUNC
	// Temporarily override all low power modes.  A reference to the SAP is required
	// to get the remote device address.
	if(iSAP)
		{
		iSAP->Protocol().ControlPlane().ModifyPhysicalLink(EOverrideLPM, iSAP->RemoteDev());
		}
	}

void CL2CapSAPSignalHandler::UndoOverrideLPM()
	{
	LOG_FUNC
	// Remove the temporary override of all low power modes.  A reference to the SAP is 
	// required to get the remote device address.
	if(iSAP)
		{
		iSAP->Protocol().ControlPlane().ModifyPhysicalLink(EUndoOverrideLPM, iSAP->RemoteDev());
		}
	}

