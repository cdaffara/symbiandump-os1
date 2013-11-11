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
//

#include <bluetooth/logger.h>

#include "l2capLinkSignalHandler.h"

#include "l2capSAPSignalHandler.h"

#include "l2capSigPacketEcho.h"
#include "l2capSigPacketInformation.h"
#include "l2capSigPacketConnection.h"
#include "l2capSigPacketConfigure.h"
#include "l2capSigPacketDisconnection.h"
#include "l2capSigPacketCommandReject.h"

#include "l2capCommand.h"

#include "l2capMuxController.h"
#include "l2signalmgr.h"

#include "l2capEntityConfig.h"

#include "l2util.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

CL2CapLinkSignalHandler* CL2CapLinkSignalHandler::NewL(CL2CAPMux* aMuxer)
	{
	LOG_STATIC_FUNC
 	CL2CapLinkSignalHandler* linkSigHandler=new (ELeave) CL2CapLinkSignalHandler(aMuxer);
	CleanupStack::PushL(linkSigHandler);
	linkSigHandler->ConstructL();
	CleanupStack::Pop();
	return linkSigHandler;
 	}

CL2CapLinkSignalHandler::~CL2CapLinkSignalHandler()
	{
	LOG_FUNC
	DeleteCommands(iPendingCommands);
	DeleteCommands(iCommandsAwaitingResponse);
	}
	


void CL2CapLinkSignalHandler::ConstructL()
	{
	LOG_FUNC
	}

// Disable warning WINS 4355: 'this' : used in base member initializer list
// This will not cause any problems in this usage and is preferable to the use of a
// non-owned pointer.
#pragma warning (disable: 4355)
CL2CapLinkSignalHandler::CL2CapLinkSignalHandler(CL2CAPMux* aMuxer)
 : CL2CapSignalHandler(aMuxer),
   iPeerL2CapEntityConfig(*this),
   iSigMTU(KL2MinMTU)
	{
	LOG_FUNC
	}
#pragma warning (default: 4355)


TBool CL2CapLinkSignalHandler::HandleConnectionRequest(HConnectionRequest* aConnectionRequest)
	{
	LOG_FUNC
	const TL2CAPPort scid = aConnectionRequest->SourceCID(); 
	const TInt8 id = aConnectionRequest->ID();
	TBool scidValid = ETrue;

	if(scid < KL2CapDynamicCIDStart)	   
		{
		scidValid = EFalse;
		}
	else
		{
		CL2CapSAPSignalHandler* listeningSH = iMuxer->MuxController().FindListeningSignalHandler(aConnectionRequest->PSM());
		if(!listeningSH)
			{
			// Create a Connection Response Command
			HL2CapCommand* command = HConnectionResponse::New(id, 0, scid, EConnectPSMNotSupported);
			if(command)
				{
				AddToOutgoingQueue(command);
				}
			}
		else
			{
			// Does given CID already exist in the queue?
			CL2CapSAPSignalHandler* sh = iMuxer->GetSignalHandlerWithRemoteCID(scid);
			if (sh)
				{
				// ... it does: check whether the Connection Request is a retransmission
				// (and retransmit the response if so), or a new command (illegal - send CmdRej).
				if (!sh->IsDuplicateCommandRequest(aConnectionRequest))
					{
					LOG1(_L("Received a new Connection Request for an existing CID: 0x%04x"), scid)
					scidValid = EFalse;
					}
				}
			else
				{
				LOG1(_L("Remote CID (0x%04x) is free"),scid)
				// Usual case, remote CID is free.
				TInt err = listeningSH->PassiveConnectionRequest(iMuxer->RemoteBTAddr(), aConnectionRequest);
				if(err != KErrNone)
					{
					HL2CapCommand* command = HConnectionResponse::New(id, 0, scid, EConnectPSMNotSupported);
					if(command)
						{
						AddToOutgoingQueue(command);
						}
					}
				// If the command could not be created the connection will fail in the
				// signalling state machine.
				}
			}
		}

	if (!scidValid)
		{
		TL2CAPCommandRejectData reason;
		reason.iReason = EInvalidCID;
		reason.iLocalEndpoint = 0;
		reason.iRemoteEndpoint = scid;
		reason.iMTUExceeded = 0;
		
		HL2CapCommand* command = HCommandReject::New(reason, id);
		if(command)
			{
			AddToOutgoingQueue(command);
			}
		}

	// Always return true.  This is the only signal handler 
	// that can process this command.
	return ETrue;
	}

TInt CL2CapLinkSignalHandler::ConstructEchoRequest(const TDes8* aData, MEchoResponseHandler& aEchoResponseHandler)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	HEchoRequest* command = NULL;
	
	if(aData)
		{
		RMBufChain data;
		TRAP(rerr, data.CreateL(*aData));
		if(rerr == KErrNone)
			{
			command = HEchoRequest::New(data, CurrentRTXTimerDuration(HL2CapCommand::KDefaultRTXTimerDuration));
			}
		}
	else
		{
		command = HEchoRequest::New();
		}
	
	if(rerr == KErrNone)
		{
		if(command)
			{
			command->SetEchoResponseHandler(aEchoResponseHandler);
			AddToOutgoingQueue(command);
			}
		else
			{
			rerr = KErrNoMemory;
			}
		}
	return rerr;
	}

void CL2CapLinkSignalHandler::DeregisterOutstandingEchoRequests(MEchoResponseHandler& aEchoResponseHandler)
	{
	LOG_FUNC
	// Find any Echo Request and if the calling reference matches set
	// it to NULL.
	HL2CapCommand* l2CapCommand;

	TDblQueIter<HL2CapCommand> iter(iCommandsAwaitingResponse);
	while((l2CapCommand = iter++) != NULL)
		{
		if(l2CapCommand->Code() == EEchoRequest)
			{
			HEchoRequest* request = static_cast<HEchoRequest*>(l2CapCommand);
			if(request->EchoResponseHandler() == &aEchoResponseHandler)
				{
				// Found a matching request - there can only be one so stop searching.
				request->RemoveEchoResponseHandler();
				break;
				}
			}
		}
	}

TBool CL2CapLinkSignalHandler::HandleEchoResponse(HEchoResponse* aEchoResponse)
	{
	LOG_FUNC
	// Check that the Echo Response has been requested.
	HL2CapCommand* requestCommand = FindMatchingOutstandingRequest(EEchoRequest, aEchoResponse->ID());
	if(requestCommand)
		{
		HEchoRequest* request = static_cast<HEchoRequest*>(requestCommand);
		if(request->EchoResponseHandler())
			{
			RMBufChain data;
			HBufC8* echoDataBuf = NULL;
			if(aEchoResponse->GetData(data) == KErrNone)
				{
				echoDataBuf = HBufC8::New(data.Length());	
				if(echoDataBuf)
					{		
					const TDes8& bufc = echoDataBuf->Des();
					TDes8& echoData = const_cast<TDes8&>(bufc);
					echoData.SetMax();
					data.CopyOut(echoData);
					}
				data.Free();
				}
			
			iMuxer->EchoResponseReceived(echoDataBuf, *request->EchoResponseHandler());
			}
		// Delete the request.
		delete requestCommand;
		}
	// If the response does not have an outstanding request drop
	// the command silently.  Always return true, this command
	// can only be for the link signal handler.
	return ETrue;
	}

TBool CL2CapLinkSignalHandler::HandleEchoRequest(HEchoRequest* aEchoRequest) //Incoming
	{
	LOG_FUNC
	//1. Create a Echo Response Command
	HL2CapCommand* command = HEchoResponse::New(aEchoRequest->ID());

	//2. Add to the list of outgoing commands
	if(command)
		{
		AddToOutgoingQueue(command);
		}
	return ETrue;
	}	
	
TInt CL2CapLinkSignalHandler::ConstructInformationRequest(TInfoType aInfoType)
	{
	LOG_FUNC
	TInt rerr = KErrNone;

	//1. Create a ConnectionRequest Command
	HL2CapCommand* command = HInformationRequest::New(aInfoType, CurrentRTXTimerDuration(HL2CapCommand::KDefaultRTXTimerDuration));
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
	
TBool CL2CapLinkSignalHandler::HandleInformationRequest(HInformationRequest* aInformationRequest)
	{
	LOG_FUNC
	// The peer is requesting the L2CAP entities capabilities.
	switch(aInformationRequest->InfoType())
		{
		case EExtendedFeaturesSupported:
			{
			TUint32 extendedFeatures = KL2CAPExtendedFeaturesSupported;

			HL2CapCommand* command = HInformationResponse::New(EExtendedFeaturesSupported, ESuccess, aInformationRequest->ID(), extendedFeatures);
			if(command)
				{
				// Add to the list of outgoing commands
				AddToOutgoingQueue(command);	
				}
			// If the command could not be created the connection will fail in the
			// signalling state machine.
			break;
			}

		// If the info requested is Connectionless MTU or a info type that
		// is not recognised, return a response with result code 'not supported'.
		case EConnectionlessMTU:
		default:
			{
			//Return InfoType and Not supported
			HL2CapCommand* command = HInformationResponse::New(aInformationRequest->InfoType(), 
			                                                   ENotsupported, 
			                                                   aInformationRequest->ID());
			if(command)
				{
				// Add to the list of outgoing commands
				AddToOutgoingQueue(command);	
				}
			// If the command could not be created the connection will fail in the
			// signalling state machine.
			break;
			}
		};

	//This should not occur as it should be handled by derived classes. 
	return(ETrue);
	}

TBool CL2CapLinkSignalHandler::HandleInformationResponse(HInformationResponse* aInformationResponse)
	{
	LOG_FUNC
	// Check that the Information Response has been requested.
	HL2CapCommand* requestCommand = FindMatchingOutstandingRequest(EInformationRequest, aInformationResponse->ID());
	if(requestCommand)
		{
		HInformationRequest* req = static_cast<HInformationRequest*>(requestCommand);
		// The Info Type in the response should match that in the 
		// request.
		if(req->InfoType() == aInformationResponse->InfoType())
			{
			switch(aInformationResponse->InfoType())
				{
				case EExtendedFeaturesSupported:
					if(aInformationResponse->Result() == ESuccess)
						{
						iPeerL2CapEntityConfig.UpdatePeerL2CapSupportedFeatures(aInformationResponse->RemoteExtendedFeatureMask());
						}
					else
						{
						// The result was a failure.  Set the supported
						// features to the default of not supported.
						iPeerL2CapEntityConfig.UpdatePeerL2CapSupportedFeatures(TL2CapEntityInfo());
						}
					iMuxer->L2CapEntityConfigUpdated();
					break;
					
				default:
					break;
				};
			}
		else
			{
			// The request and response info types don't match.  If the request
			// was for extended features, [for the sake of interop] assume that
			// no extended features are supported by the peer device.
			if(req->InfoType() == EExtendedFeaturesSupported)
				{
				iPeerL2CapEntityConfig.UpdatePeerL2CapSupportedFeatures(TL2CapEntityInfo());
				iMuxer->L2CapEntityConfigUpdated();
				}
			}
					
		// Delete the request.
		delete requestCommand;
		}
	// If the response does not have an outstanding request drop
	// the command silently.  Always return true, this command
	// can only be for the link signal handler.
	return ETrue;
	}

TBool CL2CapLinkSignalHandler::HandleCommandReject(HCommandReject* aCommandReject)
	{
	LOG_FUNC
	// Process the incoming command reject data
	TL2CAPCommandRejectData rejectData;
	
	if (aCommandReject->RejectData(rejectData) != KErrNone) 
		{
		LOG(_L("CL2CapLinkSignalHandler::HandleCommandReject - reject data could not be parsed"));
		// Couldn't parse the reject data, but we've got no-one to
		// pass handling this on to, so we have to say we've processed
		// it.
		return ETrue;
		}

	LOG1(_L("CL2CapLinkSignalHandler::HandleCommandReject - reject data: reason %d"), rejectData.iReason);
		
	switch(rejectData.iReason)
		{
		case ECommandNotUnderstood:
			{
			// The peer does not recognise a command we have sent.
			// Check if the outstanding request is causing the issue.
			HL2CapCommand* requestCommand = FindMatchingOutstandingRequest(EMatchAnyL2CAPRequest, aCommandReject->ID());
			if(requestCommand)
				{
				// This is a work around to allow the new stack to interop with
				// stacks that send Command Reject in response to Information Request.
				if(requestCommand->Code() == EInformationRequest)
					{
					// Set the supported features to the default of not supported.
					iPeerL2CapEntityConfig.UpdatePeerL2CapSupportedFeatures(TL2CapEntityInfo());
					iMuxer->L2CapEntityConfigUpdated();
					}
				delete requestCommand;
				}
			}
			break;
			
		case EMTUExceeded:
			iSigMTU = Max(rejectData.iMTUExceeded, KL2MinMTU);
			break;
		
		case EInvalidCID:
			// This handles a corner case where a Disconnect Request command 
			// retransmission crosses over with the Response for the initial
			// Disconnect Request.  The peer will correctly send a Command Reject
			// for the second Disconnect Request.  This should be ignored. 
			break;
			
		default:
			break;
		};
					
	return ETrue;
	}

TBool CL2CapLinkSignalHandler::HandleInvalidCommand(HInvalidCommand* aInvalidCommand)
	{
	LOG_FUNC
	TL2CAPCommandRejectData reason;
	reason.iReason = ECommandNotUnderstood;
	// Other reject data fields are not used for Command Not Understood
	reason.iMTUExceeded = 0;
	reason.iLocalEndpoint = 0;
	reason.iRemoteEndpoint = 0;
	
	HL2CapCommand* command = HCommandReject::New(reason, aInvalidCommand->ID());
	if(command)
		{
		AddToOutgoingQueue(command);
		}
	
	// The link signal handler always services invalid commands.
	return ETrue;
	}


TBool CL2CapLinkSignalHandler::HandleConnectionResponse(HConnectionResponse* /*aConnectionResponse*/)
	{
	LOG_FUNC
	// This command has not been handled by any of the SAP 
	// signal handlers. It's a response so just drop it.
	return ETrue;
	}

TBool CL2CapLinkSignalHandler::HandleConfigureRequest(HConfigureRequest* aConfigRequest)
	{
	LOG_FUNC
	// This command has not been handled by any of the SAP 
	// signal handlers.  Send a Command Reject.
	SendInvalidCIDCommandReject(aConfigRequest->ID(),
	                            0,
                                aConfigRequest->DestinationCID());

	return ETrue;
	}

TBool CL2CapLinkSignalHandler::HandleConfigureResponse(HConfigureResponse* /*aConfigResponse*/)	
	{
	LOG_FUNC
	// This command has not been handled by any of the SAP 
	// signal handlers. It's a response so just drop it.
	return ETrue;
	}

TBool CL2CapLinkSignalHandler::HandleDisconnectRequest(HDisconnectRequest* aDisconnectRequest)	
	{
	LOG_FUNC
	// This command has not been handled by any of the SAP 
	// signal handlers.  Send a Command Reject.
	SendInvalidCIDCommandReject(aDisconnectRequest->ID(),
	                            aDisconnectRequest->SourceCID(),
                                aDisconnectRequest->DestinationCID());

	return ETrue;
	}

TBool CL2CapLinkSignalHandler::HandleDisconnectResponse(HDisconnectResponse* /*aDisconnectResponse*/)	
	{
	LOG_FUNC
	// This command has not been handled by any of the SAP 
	// signal handlers. It's a response so just drop it.
	return ETrue;
	}

void CL2CapLinkSignalHandler::PendingCommandsDrained()
	{
	LOG_FUNC
	// No action is required here.
	}
	
// HIncomingSAPSigPDUHandler Interface Definition.
void CL2CapLinkSignalHandler::LinkUp()
	{
	LOG_FUNC
	// No action required from the LinkSignalHandler when the link is established.
	}

void CL2CapLinkSignalHandler::Error(TInt /*aErrorCode*/, MSocketNotify::TOperationBitmasks /*aErrorAction*/)
	{
	LOG_FUNC
	HandleLinkError();
	}
	
void CL2CapLinkSignalHandler::CommandResponseFailure(HL2CapCommand* aCommand)
	{
	LOG_FUNC
	switch(aCommand->Code())
		{
		case EInformationRequest:
			//For IOP We set extended feature to basic if there is no reply for info request.
			iPeerL2CapEntityConfig.UpdatePeerL2CapSupportedFeatures(TL2CapEntityInfo());
			iMuxer->L2CapEntityConfigUpdated();
			break;
			
		default:
			iMuxer->ErrorAllSignalHandlers(KErrL2CAPRequestTimeout);
			break;
		}
	
	delete aCommand;
	}
	
void CL2CapSignalHandler::PendingCommandsDrained()
	{
	LOG_FUNC
	// Currently no action is required by the link signal handler.
	}




