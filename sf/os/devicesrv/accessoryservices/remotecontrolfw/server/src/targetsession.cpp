// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Symbian Foundation License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <bluetooth/logger.h>
#include <remcon/remconifdetails.h>
#include "messagequeue.h"
#include "remconmessage.h"
#include "remconserver.h"
#include "server.h"
#include "targetsession.h"
#include "utils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("tgsession");
#endif

CRemConTargetSession* CRemConTargetSession::NewL(CRemConTargetClientProcess& aClientProcess,
	CRemConServer& aServer,
	CBearerManager& aBearerManager,
	TUint aId)
	{
	LOG_STATIC_FUNC;
	CRemConTargetSession* self = new(ELeave) CRemConTargetSession(aClientProcess, aServer, aBearerManager, aId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CLEANUPSTACK_POP1(self);
	return self;
	}

CRemConTargetSession::~CRemConTargetSession()
	{
	LOG_FUNC;

	// Tell the client process representation we've gone away- it may start its shutdown timer.
	iClientProcess.TargetSessionClosed(*this);
	}

CRemConTargetSession::CRemConTargetSession(CRemConTargetClientProcess& aClientProcess,
	CRemConServer& aServer, 
	CBearerManager& aBearerManager,
	TUint aId)
	: CRemConSession(aServer, aBearerManager, aId),
	iClientProcess(aClientProcess)
	{
	LOG_FUNC;
	}

void CRemConTargetSession::ConstructL()
	{
	LOG_FUNC;

	BaseConstructL(iClientProcess.ClientInfo());

	// Tell the client process representation about us (this will in turn inform the server).
	LEAVEIFERRORL(iClientProcess.TargetSessionOpened(*this));
	
	// Set our pointer into the connection history at the current/'Last' item.
	// Can't do this til we've told the server we exist
	iServer.SetConnectionHistoryPointer(Id());
	}

TBool CRemConTargetSession::SupportedMessage(const CRemConMessage& aMsg) const
	{
	LOG_FUNC;

	// Return true if the message is for an interface supported by this session
	return InterfaceSupported(aMsg.InterfaceUid());
	}

TBool CRemConTargetSession::InterfaceSupported(TUid aInterfaceUid) const
	{
	LOG_FUNC;
	LOG2(_L("\taInterfaceUid = 0x%08x, iInterestedAPIs = %d"), aInterfaceUid, iInterestedAPIs);

	TBool result = iInterestedAPIs ? (FindInterfaceByUid(aInterfaceUid) != NULL) : EFalse;

	LOG1(_L("result = %d"), result);
	return result;
	}

void CRemConTargetSession::SetPlayerType(const RMessage2& aMessage)
	{
	LOG_FUNC;
	
	TInt err = aMessage.GetDesLength(1);
	if (err >= 0)
		{
		TPlayerTypeInformation playerType;
		RBuf8 playerName;
	
		TRAP(err, GetPlayerTypeAndNameL(aMessage, playerType, playerName));

		if (err == KErrBadDescriptor)
			{
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
			playerName.Close();
			return;
			}
		else if (err == KErrArgument)
			{
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicIllegalIpc);
			playerName.Close();
			return;			
			}
		else if (err == KErrNone)
			{
			// If player type is not set, then update iClientProcess with information.
			// If player type is already set, then dont allow this to be changed for the client (panic the client if the information is different).
			if (!iClientProcess.HasPlayerInformation())
				{
				TRAP(err, iClientProcess.SetPlayerInformationL(playerType, playerName));
				}
			else if (!iClientProcess.PlayerInformationMatches(playerType, playerName))
				{
				PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicPlayerInfoAlreadySet);
				playerName.Close();
				return;
				}
			}

		playerName.Close();
		}
	else if (err == KErrBadDescriptor)
		{	
		// The additional parameters are optional (i.e. old target clients won't provide them).
		err = KErrNone;
		}
	
	CompleteClient(aMessage, err);
	}

CRemConMessage* CRemConTargetSession::DoPrepareSendMessageL(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Get the data the client wants to send.
	TUid interfaceUid;
	TUint operationId;

	TRemConMessageSubType messageSubType;
	RBuf8 sendDes;
	if (!DoGetSendInfoLC(aMessage, interfaceUid, operationId, messageSubType, sendDes))
		{
		// DoGetSendInfoLC() panicked the message
		return NULL;	
		}

	CRemConMessage* msg = NULL;
	
	LOG(_L("\tTARGET send"));

	msg = CRemConMessage::NewL(
		TRemConAddress(), // we don't know which remotes it's going to yet
		ERemConResponse, // targets can only send responses
		messageSubType,
		interfaceUid,
		operationId,
		sendDes, // msg takes ownership
		iClientProcess.Id(), // session id to match this response against the originating command
		0, // transaction id not yet known
		ETrue);
	CLEANUPSTACK_POP1(&sendDes); // now owned by msg
	
	return msg;
	}

void CRemConTargetSession::SendUnreliable(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	CRemConMessage* msg = NULL;
	TRAPD(err, msg = DoCreateUnreliableMessageL(aMessage));
	CompleteClient(aMessage, err);
	if (err == KErrNone)
		{
		ASSERT_DEBUG(iSendQueue);
		if (iSending || !iSendQueue->IsEmpty())
			{
			iSendQueue->Append(*msg);
			}
		else
			{
			SendToServer(*msg);
			}
		}
	}

CRemConMessage* CRemConTargetSession::DoCreateUnreliableMessageL(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Get the data the client wants to send.
	TUid interfaceUid;
	TUint operationId;
	TRemConMessageSubType messageSubType;
	RBuf8 sendDes;
	DoGetSendInfoLC(aMessage, interfaceUid, operationId, messageSubType, sendDes);

	// Before we ask the server to send, we must set our ClientInfo 
	// correctly so the TSP can get information about the client. 
	iClientInfo.Message() = aMessage;

	CRemConMessage* msg = NULL;
	
	LOG(_L("\tTARGET send"));

	msg = CRemConMessage::NewL(
		TRemConAddress(), // we don't know which remotes it's going to yet
		ERemConResponse, // targets can only send responses
		messageSubType,
		interfaceUid,
		operationId,
		sendDes, // msg takes ownership
		iClientProcess.Id(), // session id to match this response against the originating command
		0, // transaction id not yet known
		EFalse);
	CLEANUPSTACK_POP1(&sendDes); // now owned by msg

	return msg;
	}

void CRemConTargetSession::DoSendCancel()
	{
	LOG_FUNC;

	// We do not cancel any pending response messages, so don't need
	// to notify the server here.

	NumRemotesToTry() = 0;
	iSendError = KErrCancel;
	CompleteSend();
	}

void CRemConTargetSession::RegisterInterestedAPIs(const RMessage2& aMessage)
	{
	LOG_FUNC;
	
	// Check we haven't had our features set yet
	if (ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	CRemConInterfaceDetailsArray* interfaces = NULL;
	TRAPD(err, interfaces = ExtractInterestedAPIsL(aMessage));
	
	if(err == KErrNone)
		{
		// Ensure that none of these interfaces have already been registered by the client.
		// Also note if bulk server is required.
		TInt count = interfaces->Array().Count();
		TBool bulkServerRequired = EFalse;
		for (TInt ix=0; ix < count; ++ix)
			{
			CRemConInterfaceDetails* details = interfaces->Array()[ix];
			ASSERT_DEBUG(details);
			if (iClientProcess.IsInterfaceTypeRegisteredByAnotherSession(*this, details->Uid()))
				{
				// Destroy new interfaces and panic client.
				delete interfaces;
				PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientInterfaceAlreadyRegistered);
				return;
				}
			else if (details->IsBulk())
				{
				bulkServerRequired = ETrue;
				}
			}
	
		// Interfaces are OK, notify server if bulk server is required.
		iInterestedAPIs = interfaces;
		iClientProcess.InterfacesRegistered();
		if (bulkServerRequired)
			{
			iServer.BulkServerRequired();
			}
		}
	else
		{
		ASSERT_DEBUG(interfaces == NULL);
		}
	
	CompleteClient(aMessage, err);
	}

void CRemConTargetSession::SendToServer(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	
	// Set our completion members.
	NumRemotes() = 0;
	NumRemotesToTry() = 0;
	SendError() = KErrNone;

	
	iSending = (aMsg.IsReliableSend()) ? ESendingReliable: ESendingUnreliable;

	iServer.SendResponse(aMsg, iClientProcess);
	}

void CRemConTargetSession::DoReceive()
	{
	// Request messages from the server on behalf of the client.
	// If there's anything waiting to be given to us, iClientProcess will call 
	// back to us with it.
	iServer.ReceiveRequest(iClientProcess);
	}
