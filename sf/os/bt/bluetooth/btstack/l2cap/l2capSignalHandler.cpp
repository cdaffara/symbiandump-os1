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
// Base class for implementation of signalling handlers.  This class is able to sort incoming CFramePDUs and 
// queue outgoing l2cap commands. This class can be derived by classes wanting to handle subsets of the 
// signalling commands.
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>

#include "l2capSignalHandler.h"

#include "l2capCommand.h"
#include "l2signalmgr.h"

#include "l2capSigPacketCommandReject.h"

#include "l2util.h"
#include "L2CapDebugControlInterface.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

//Constructor as used by the link signal handler
CL2CapSignalHandler::CL2CapSignalHandler(CL2CAPMux* aMuxer)
 : iMuxer(aMuxer),
   iDrainPendingCommands(EFalse),
   iPendingCommands(_FOFF(HL2CapCommand, iLink)),
   iCommandsAwaitingResponse(_FOFF(HL2CapCommand, iLink)),
   iSentResponses(_FOFF(HL2CapCommand, iLink))
	{
	LOG_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::ESigHandlers,
	                             L2capDebugInfo::EAllocated));	
	}

CL2CapSignalHandler::~CL2CapSignalHandler()
/**
	Destruct the signal handler.
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iPendingCommands.IsEmpty(), Panic(EL2CAPSigHandlerDeletedWithResources));
	__ASSERT_DEBUG(iCommandsAwaitingResponse.IsEmpty(), Panic(EL2CAPSigHandlerDeletedWithResources));

	HL2CapCommand *cmd = NULL;
	TDblQueIter<HL2CapCommand> responseIter(iSentResponses);
	responseIter.SetToFirst();
	while((cmd = responseIter++) != NULL)
		{
		delete cmd;
		}

	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::ESigHandlers,
	                             L2capDebugInfo::EDeleted));
	}

// Returns a signalling pdu awaiting transmission (might contain more than one L2CAP Command)
HL2CapPDU* CL2CapSignalHandler::GetPDU()
	{
	LOG_FUNC
	TBool pduComplete = EFalse;
	TBool pduValid = EFalse;
	TInt err = KErrNone;
	
	HCFramePDU* pdu = NULL;
	
	// 1. Check whether there are any commands to send
	if(!iPendingCommands.IsEmpty())
		{
		// 2. Create a new PDU to put the commands into
		pdu = HCFramePDU::New(iMuxer->GetACLMTU());
		
		if(pdu)
			{
			TDblQueIter<HL2CapCommand> iter(iPendingCommands);
			HL2CapCommand* cmd;
			while((cmd = iter++) != NULL && !pduComplete)
				{
				// 3. Attempts to add command onto end of pdu.
				err = pdu->AddCommand(*cmd, *iMuxer);

				if(err == KErrNone)
			 		{
			 		// The command has been added.  Remove it from the
			 		// pending list.
					pduValid = ETrue;
			 		cmd->iLink.Deque();
			 		
					if(cmd->IsRequest())
						{
						// Add this to the awaiting response queue.
						iCommandsAwaitingResponse.AddLast(*cmd);	

						// Start response timer.
						cmd->StartRTXTimer(HL2CapCommand::ERTXTimer, *this);
						}
					else 
						{
						if(cmd->StatefulResponse())
							{
							iSentResponses.AddLast(*cmd);
							}
						else
							{
							delete cmd;
							}
						}
					}
				else
					{
					if(err == KErrCompletion)
						{
						// The command could not be added.  This indicates that the
						// CFrame is full.
						pduComplete = ETrue;
						}
					else
						{
						// A different error has occurred.  Error the signal handler.
						Error(err, MSocketNotify::EErrorAllOperations);
						}
					}
			
			 	}

			// Check if the PDU is valid, i.e., contains at least one command.
			if(!pduValid)
				{
				delete pdu;
				pdu = NULL;
				}
				
			// Check if any commands are outstanding.  If so request
			// another callback.
			if(!iPendingCommands.IsEmpty() || iDrainPendingCommands)
				{
				iMuxer->PDUAvailable();
				}
			}
		else
			{
			// A C-Frame could not be created.
			Error(KErrNoMemory, MSocketNotify::EErrorAllOperations);
			}
		}
	else
		{
		if(iDrainPendingCommands && iCommandsAwaitingResponse.IsEmpty())
			{
			// Nothing more to send and nothing to wait for. 
			// Signal disconnect to the state machine.
			PendingCommandsDrained();
			}
		}
	return pdu;
	}


HL2CapCommand* CL2CapSignalHandler::FindMatchingOutstandingRequest(TUint8 aExpectedCommandCode, TUint8 aId)
	{
	LOG_FUNC
	HL2CapCommand* l2CapCommand = NULL;
	HL2CapCommand* matchingCommand = NULL;
	
	TDblQueIter<HL2CapCommand> iter(iCommandsAwaitingResponse);
	while((l2CapCommand = iter++) != NULL)
		{
		if(l2CapCommand->ID() == aId)
			{
			if(l2CapCommand->Code() == aExpectedCommandCode || 
			   (aExpectedCommandCode == EMatchAnyL2CAPRequest && l2CapCommand->IsRequest()))
				{
				matchingCommand = l2CapCommand;
				break;
				}
			}
		}
	return matchingCommand;
	}
	
// Incoming PDU handler.
TBool CL2CapSignalHandler::HandleIncomingCFrame(HCFramePDU* aCFrame)
	{
	LOG_FUNC
	// The C-Frame can contain many L2CAP commands.  Not all of
	// the commands will be for a SAP signal handler.  If a
	// command can be processed it will be removed from the
	// C-Frame.  Any remaining commands will be offered to the
	// other SAP signal handlers, then the whole process of
	// offering the commands to the SAP signal handlers is
	// repeated until no more commands can be handled by them,
	// after which the commands are offered to the link signal
	// handler.  Only one command is handled at a time because a
	// Command Reject may delete a SAP signal handler, thereby
	// rendering it ineffective at processing further commands.

	HL2CapCommand* cmd = aCFrame->FirstCommand();
	while(cmd)
		{
		// This could be a duplicate sent from the peer due to an RTx timer.
		// If the original was received and is being processed discard the
		// command.
		if(!IsDuplicateCommandRequest(cmd))
			{
			if (cmd->ProcessCommand(*this))
				{
				return ETrue;
				}
			}
		cmd = aCFrame->NextCommand();
		}
	return EFalse;
	}

TBool CL2CapSignalHandler::IsDuplicateCommandRequest(HL2CapCommand* aCommand)
	{
	LOG_FUNC
	TBool rcode = EFalse;
	
	// Check if this is a duplicate still kicking about waiting to be sent
	if(aCommand->IsRequest())
		{
		HL2CapCommand *response = NULL;
		
		TDblQueIter<HL2CapCommand> pendIter(iPendingCommands);
		while((response = pendIter++) != NULL)
			{
			// Check if this matches the last peer request.
			if(aCommand->ID() == response->ID() &&
			   (aCommand->Code() + 1) == response->Code())
				{
				delete aCommand;
				rcode = ETrue;
				
				break;
				}
			}
		}
		
	// Otherwise check if this is a response we've already sent
	if(!rcode && aCommand->IsRequest())
		{
		HL2CapCommand *response = NULL;
		
		TDblQueIter<HL2CapCommand> respIter(iSentResponses);
		while((response = respIter++) != NULL)
			{
			// Check if this matches the last peer request.
			if(aCommand->ID() == response->ID() &&
			   (aCommand->Code() + 1) == response->Code())
				{			
				// Re-send the stored response.
				response->iLink.Deque();
				AddToOutgoingQueue(response);

				delete aCommand;
				rcode = ETrue;
				
				break;
				}
			}
		}

	return rcode;
	}

/**
Store this command on the outgoing queue.  

@pre aCommand is not already on a queue.
@param aCommand The command to add to the queue.
*/
void CL2CapSignalHandler::AddToOutgoingQueue(HL2CapCommand* aCommand)
	{
	LOG_FUNC
	if(aCommand->CommandLength() <= iMuxer->SigMTU())
		{
		// 1. Add to the list of outgoing commands.
		iPendingCommands.AddLast(*aCommand);

		// 2. Signal to the PDU that data is available in the queue.
		iMuxer->PDUAvailable();
		}
	else
		{
		// At present the stack will not create a command greater in 
		// length than the minimum SigMTU.
		__ASSERT_DEBUG(EFalse, Panic(EL2CAPAttemptToCreateCommandLongerThanSigMTU));
		}
	}

/**
This is called when we know the remote has finished configuring this
channel, ie when we've received data from it.  At this point we can
flush the queued responses.
*/
void CL2CapSignalHandler::ChannelConfigured()
	{
	LOG_FUNC
	HL2CapCommand* cmd = NULL;
	TDblQueIter<HL2CapCommand> responseIter(iSentResponses);
	responseIter.SetToFirst();
	while((cmd = responseIter++) != NULL)
		{
		cmd->iLink.Deque();
		delete cmd;
		}
	}

void CL2CapSignalHandler::DrainPendingCommands()
	{
	LOG_FUNC
	iDrainPendingCommands = ETrue;
	iMuxer->PDUAvailable();
	}

void CL2CapSignalHandler::FlushPendingCommands()
	{
	LOG_FUNC
	// Remove all pending commands from the outgoing queue.
	HL2CapCommand* cmd = NULL;
	TDblQueIter<HL2CapCommand> pendIter(iPendingCommands);
	while((cmd = pendIter++) != NULL)
		{
		LOG1(_L(" -- Removing 0x%02x command"), cmd->Code());
		delete cmd;
		}
	LOG(_L(" - queue flushed"));
	}

void CL2CapSignalHandler::FlushAwaitingResponseCommands()
	{
	LOG_FUNC
	DeleteCommands(iCommandsAwaitingResponse);
	}

void CL2CapSignalHandler::FlushAllQueues()
	{
	LOG_FUNC
	DeleteCommands(iPendingCommands);
	DeleteCommands(iCommandsAwaitingResponse);
	DeleteCommands(iSentResponses);
	}

void CL2CapSignalHandler::HandleLinkError()
	{
	LOG_FUNC
	// An error condition has occurred on the link.  
	// Any commands awaiting transmission,
	// or awaiting responses can be deleted as no more PDU can
	// be sent or received on this link.
	HL2CapCommand* cmd = NULL;
	FlushPendingCommands();
	
	TDblQueIter<HL2CapCommand> waitIter(iCommandsAwaitingResponse);
	waitIter.SetToFirst();
	while((cmd = waitIter++) != NULL)
		{
		delete cmd;
		}
		
	TDblQueIter<HL2CapCommand> responseIter(iSentResponses);
	responseIter.SetToFirst();
	while((cmd = responseIter++) != NULL)
		{
		cmd->iLink.Deque();
		delete cmd;
		}
	}
	
	
TUint8 CL2CapSignalHandler::CurrentRTXTimerDuration(TUint8 aBaseRTXTimerDuration) const
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iMuxer,Panic(EL2CAPMuxerDeletedUnexpectedly));
	
	return iMuxer->AdjustRTXTimerForSniffMode(aBaseRTXTimerDuration);
	}
	

// This base class does not handle any Frame types or commands.  These
// default implementations are here to allow derived class to only specify
// commands and frames that are handled.
TBool CL2CapSignalHandler::HandleConnectionResponse(HConnectionResponse* /*aConnectionResponse*/)
	{
	LOG_FUNC
	return EFalse;
	}

TBool CL2CapSignalHandler::HandleConnectionRequest(HConnectionRequest* /*aConnectionRequest*/)
	{
	LOG_FUNC
	return EFalse;
	}
TBool CL2CapSignalHandler::HandleConfigureRequest(HConfigureRequest* /*aConfigRequest*/)
	{
	LOG_FUNC
	return EFalse;
	}

TBool CL2CapSignalHandler::HandleConfigureResponse(HConfigureResponse* /*aConfigResponse*/)
	{
	LOG_FUNC
	return EFalse;
	}

TBool CL2CapSignalHandler::HandleEchoResponse(HEchoResponse* /*aEchoResponse*/)
	{
	LOG_FUNC
	return EFalse;
	}

TBool CL2CapSignalHandler::HandleEchoRequest(HEchoRequest* /*aEchoRequest*/) // Take incoming EchoRequest extract any data to form a valid Echo Response from CEchoResponse	
	{
	LOG_FUNC
	return EFalse;	
	}
TBool CL2CapSignalHandler::HandleInformationRequest(HInformationRequest* /*aInformationRequest*/)
	{
	LOG_FUNC
	return EFalse;	
	}
	
TBool CL2CapSignalHandler::HandleDisconnectRequest(HDisconnectRequest* /*aDisconnectRequest*/)
	{
	LOG_FUNC
	return EFalse;	
	}
	
TBool CL2CapSignalHandler::HandleDisconnectResponse(HDisconnectResponse* /*aDisconnectResponse*/)
	{
	LOG_FUNC
	return EFalse;	
	}

TBool CL2CapSignalHandler::HandleInformationResponse(HInformationResponse* /*aInformationResponse*/)
	{
	LOG_FUNC
	return EFalse;
	}
	
TBool CL2CapSignalHandler::HandleCommandReject(HCommandReject* /*aCommandReject*/)
	{
	LOG_FUNC
	return EFalse;
	}

TBool CL2CapSignalHandler::HandleInvalidCommand(HInvalidCommand* /*aInvalidCommand*/)
	{
	LOG_FUNC
	return EFalse;
	}


void CL2CapSignalHandler::SendInvalidCIDCommandReject(TUint8 aId, TL2CAPPort aSourceCID, TL2CAPPort aDestinationCID)
	{
	LOG_FUNC
	TL2CAPCommandRejectData reason;
	reason.iReason = EInvalidCID;
	reason.iMTUExceeded = 0;
	reason.iLocalEndpoint = aDestinationCID;
	reason.iRemoteEndpoint = aSourceCID;
	
	HL2CapCommand* command = HCommandReject::New(reason, aId);
	if(command)
		{
		AddToOutgoingQueue(command);
		}
	}

void CL2CapSignalHandler::DeleteCommands(TDblQue<HL2CapCommand>& aCommandsToDelete)
	{
	LOG_FUNC
	HL2CapCommand* cmd = NULL;
	TDblQueIter<HL2CapCommand> aDelCmdIter(aCommandsToDelete);
	while((cmd = aDelCmdIter++) != NULL)
		{
		delete cmd;
		}
	}
