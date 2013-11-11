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
#include "activehelper.h"
#include "bearermanager.h"
#include "controllersession.h"
#include "messagequeue.h"
#include "remconmessage.h"
#include "remconserver.h"
#include "server.h"
#include "utils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("ctsession");
#endif


CRemConControllerSession* CRemConControllerSession::NewL(CRemConServer& aServer,
	CBearerManager& aBearerManager,
	const TClientInfo& aClientInfo,
	TUint aId)
	{
	LOG_STATIC_FUNC;
	CRemConControllerSession* self = new(ELeave) CRemConControllerSession(aServer, aBearerManager, aId);
	CleanupStack::PushL(self);
	self->ConstructL(aClientInfo);
	CLEANUPSTACK_POP1(self);
	return self;
	}

CRemConControllerSession::~CRemConControllerSession()
	{
	LOG_FUNC;

	delete iPendingMsgProcessor;

	// we will need to tell the server which bearer this used to be connected to
	// this enables the server to not inform a bearer that is already connected
	// that its been connected
	// Tell the server we've gone away- it may start its shutdown timer.
	iServer.ControllerClientClosed(*this, iRemoteAddress.BearerUid());
	iPlayerName.Close();
	}

CRemConControllerSession::CRemConControllerSession(CRemConServer& aServer, 
	CBearerManager& aBearerManager,
	TUint aId)
	: CRemConSession(aServer, aBearerManager, aId)
	{
	LOG_FUNC;
	}

void CRemConControllerSession::ConstructL(const TClientInfo& aClientInfo)
	{
	LOG_FUNC;
	
	BaseConstructL(aClientInfo);

	iPendingMsgProcessor = new (ELeave) CActiveHelper(*this);
	
	LEAVEIFERRORL(iServer.ControllerClientOpened(*this));

	// Set our pointer into the connection history at the current/'Last' item.
	// Can't do this til we've told the server we exist
	iServer.SetConnectionHistoryPointer(Id());
	}

TBool CRemConControllerSession::SupportedMessage(const CRemConMessage& aMsg) const
	{
	LOG_FUNC;
	LOG1(_L("\taMsg.InterfaceUid() = 0x%08x"), aMsg.InterfaceUid());

	// Return true unless this is a command for an unsupported interface
	TBool result = !(aMsg.MsgType() == ERemConCommand && !FindInterfaceByUid(aMsg.InterfaceUid()));

	LOG1(_L("result = %d"), result);
	return result;
	}

void CRemConControllerSession::SetPlayerType(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Controller clients don't provide the additional parameters are optional,
	// so we just complete the message with KErrNone.
	CompleteClient(aMessage, KErrNone);
	}

void CRemConControllerSession::CompleteConnect(const TRemConAddress& aAddr, TInt aError)
	{
	LOG_FUNC;
	LOG2(_L("\taError = %d, aAddr.BearerUid = 0x%08x"), aError, aAddr.BearerUid());

	LOG1(_L("\tiRemoteAddress.BearerUid = 0x%08x"), iRemoteAddress.BearerUid());
	LOG1(_L("\tiConnectBearerMsg.Handle = %d"), iConnectBearerMsg.Handle());

	if ( iRemoteAddress == aAddr )
		{
		if ( iConnectBearerMsg.Handle() )
			{
			// We are a session that has an outstanding request on this specific 
			// connection address.
			CompleteClient(iConnectBearerMsg, aError);
			}
		else 
			{
			// Connect bearer message is not valid. 
			// Check for pending messages.
			CheckForPendingMsg();
			}
		}
	}

void CRemConControllerSession::CompleteDisconnect(const TRemConAddress& aAddr, TInt aError)
	{
	LOG_FUNC;
	LOG2(_L("\taError = %d, aAddr.BearerUid = 0x%08x"), aError, aAddr.BearerUid());

	LOG1(_L("\tiRemoteAddress.BearerUid = 0x%08x"), iRemoteAddress.BearerUid());
	LOG1(_L("\tiDisconnectBearerMsg.Handle = %d"), iDisconnectBearerMsg.Handle());

	if ( iRemoteAddress == aAddr )
		{
		if ( iDisconnectBearerMsg.Handle() )
			{
			// We are a session that has an outstanding request on this specific 
			// connection address.
			CompleteClient(iDisconnectBearerMsg, aError);
			}
		else 
			{
			// Diconnect bearer message is not valid. 
			// Check for pending messages.
			CheckForPendingMsg();
			}

		}
	}

void CRemConControllerSession::ProcessPendingMsgL()
	{
	LOG_FUNC;
	if (!iPendingMsg.Handle())
		{
		// This means that the pending connect or disconnect message,
		// has been cancelled by the time we got here.
		// (It was cancelled between two following calls:
		// iPendingMsgProcessor::Complete and iPendingMsgProcessor::RunL
		return;
		}

	ServiceL(iPendingMsg);
	if (iPendingMsg.Handle())
		{
		// This means that the pending msg has not been completed in ServiceL call.
		// It was stored either in iConnectBearerMsg or iDisconnectBearerMsg member.
		// This also means that this message is not "pending" any more 
		// (as processing of its copy has been started). 
		// However because the copy will get completed we need to 
		// clean iPendingMsg.iHandle here
		// To supress coverity error for uninitialized use of 'emptyMsg' coverity annotations
		// are used as the in-line default constructor of RMessage2 doesn't initialize all member variables.
		// coverity[var_decl]
		RMessage2 emptyMsg;
		iPendingMsg = emptyMsg;
		}
	}

void CRemConControllerSession::CheckForPendingMsg() const
	{
	LOG_FUNC;
	if (iPendingMsg.Handle())
		{
		ASSERT_DEBUG(iPendingMsgProcessor);
		iPendingMsgProcessor->Complete();
		}
	}

CRemConMessage* CRemConControllerSession::DoPrepareSendMessageL(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we don't have a disconnect outstanding- this makes no sense from 
	// a client viewpoint (they should cancel the disconnect first).
	// [The client is allowed to have a connect request outstanding- the 
	// bearer manager makes sure a bearer-level connect is not posted on the 
	// same address twice.]
	if ( iDisconnectBearerMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return NULL;
		}

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
	LOG(_L("\tCONTROLLER send"));
	if (  (messageSubType == ERemConNotifyCommandAwaitingInterim)
	   || (messageSubType == ERemConNotifyCommandAwaitingChanged)
		)
		{
		LOG(_L("\terror, not allowed to use Send() to send notify command"));
		CleanupStack::PopAndDestroy(&sendDes);
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicIllegalIpc);
		}
	else
		{
		msg = CRemConMessage::NewL(
				iRemoteAddress, // either specified (if we're connection-oriented) or null (we're connectionless- this field will be filled in by the TSP)
				ERemConCommand, 
				messageSubType,
				interfaceUid,
				operationId,
				sendDes, // msg takes ownership
				Id(), // session id for when the response comes back
				0, // we let the bearer manager invent a new transaction id when the message gets to it
				ETrue);
		CLEANUPSTACK_POP1(&sendDes); // now owned by msg
		}		

	return msg;
	}

void CRemConControllerSession::SendUnreliable(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	// Check we don't have a disconnect outstanding- this makes no sense from 
	// a client viewpoint (they should cancel the disconnect first).
	// [The client is allowed to have a connect request outstanding- the 
	// bearer manager makes sure a bearer-level connect is not posted on the 
	// same address twice.]
	if ( iDisconnectBearerMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
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

CRemConMessage* CRemConControllerSession::DoCreateUnreliableMessageL(const RMessage2& aMessage)
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
	
	LOG(_L("\tCONTROLLER send"));
	
	// A client is not allowed to send an unreliable notify command.
	if	(	(messageSubType == ERemConNotifyCommandAwaitingInterim)
		||	(messageSubType == ERemConNotifyCommandAwaitingChanged)
		)
		{
		LOG(_L8("\tNot allowed to send unreliable notify command"));
		CleanupStack::PopAndDestroy(&sendDes);
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicIllegalIpc);
		LEAVEL(KErrBadDescriptor);
		}
	
	msg = CRemConMessage::NewL(
		iRemoteAddress, // either specified (if we're connection-oriented) or null (we're connectionless- this field will be filled in by the TSP)
		ERemConCommand, // controllers can only send commands
		messageSubType,
		interfaceUid,
		operationId,
		sendDes, // msg takes ownership
		Id(), // session id for when the response comes back
		0, // we let the bearer manager invent a new transaction id when the message gets to it
		EFalse);
	CLEANUPSTACK_POP1(&sendDes); // now owned by msg

	return msg;
	}

void CRemConControllerSession::RegisterInterestedAPIs(const RMessage2& aMessage)
	{
	LOG_FUNC;
	// No interfaces should have been registered yet!
	ASSERT_DEBUG(iInterestedAPIs == NULL);
	
	TRAPD(err, iInterestedAPIs = ExtractInterestedAPIsL(aMessage));
	
	iServer.ControllerClientAvailable();

	CompleteClient(aMessage, err);
	}

void CRemConControllerSession::GoConnectionOriented(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	if ( !iRemoteAddress.IsNull() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicAlreadyConnectionOriented);
		return;
		}

	if ( iConnectBearerMsg.Handle() || iDisconnectBearerMsg.Handle() || iSendMsg.Handle())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}
	if (iSending != ENotSending)
		{
		DoSendCancel();
		}
	EmptySendQueue();
	
	// Get the desired address from the message and check it.
	const TUid uid = TUid::Uid(aMessage.Int0());
	LOG1(_L("\tuid = 0x%08x"), uid);
	// Check the requested bearer exists.
	TBool bearerExists = iBearerManager.BearerExists(uid);
	if ( !bearerExists)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerPluginIncorrectInterface);
		return;
		}
	// Check the bearer-specific part of the address.
	TBuf8<TRemConAddress::KMaxAddrSize> buf;
	TInt err = aMessage.Read(1, buf);
	if ( err != KErrNone )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		return;
		}

	// Do security check- if this client won't be allowed to use the bearer 
	// then fail the request. 
	// NB This security check (repeated in debug at ConnectBearer and 
	// DisconnectBearer time) is all that stands between a connection-oriented 
	// client and the bearer, and is all the caps checking that RemCon does!
	err = KErrPermissionDenied;
	if ( iBearerManager.CheckPolicy(uid, aMessage) )
		{
		err = KErrNone;
		}
		
		
	// if alls well and we're connection oriented then set up as such
	if (KErrNone == err)
		{
		// The client has passed all our checks- set our data member.
		iRemoteAddress.BearerUid() = uid;
		iRemoteAddress.Addr() = buf;
		// tell the server
		iServer.ClientGoConnectionOriented(*this,uid);
		}
				
	CompleteClient(aMessage, err);
	}

void CRemConControllerSession::GoConnectionless(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	if ( iRemoteAddress.IsNull() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicNotConnectionOriented);
		return;
		}

	if ( iConnectBearerMsg.Handle() || iDisconnectBearerMsg.Handle() || iSendMsg.Handle())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}
	
	if (iSending != ENotSending)
		{
		DoSendCancel();
		}
	EmptySendQueue();
	
	// we will need to tell the server which bearer this used to be connected to
	// this enables the server to not inform a bearer that is already connected
	// that its been connected
	TUid oldUid = iRemoteAddress.BearerUid();
	
	iRemoteAddress.BearerUid() = KNullUid;	

	// tell the server
	iServer.ClientGoConnectionless(*this, oldUid);

	CompleteClient(aMessage, KErrNone);
	}

void CRemConControllerSession::ConnectBearer(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	if ( iConnectBearerMsg.Handle() || iDisconnectBearerMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}

	if ( iRemoteAddress.IsNull() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicNotConnectionOriented);
		return;
		}

	// Check the requested bearer exists.
	TBool bearerExists = iBearerManager.BearerExists(iRemoteAddress.BearerUid());
	// This check was done at GoConnectionOriented time.
	ASSERT_DEBUG(bearerExists);
	// So was this one.
	ASSERT_DEBUG(iBearerManager.CheckPolicy(iRemoteAddress.BearerUid(), aMessage));

	// Check the state of our given connection at the bearer level. If it is: 
	// -) disconnected request the connection to come up,
	// -) connecting or disconnecting, add message to the queue of pending 
	//		messages, and process it once connecting/disconnecting has been completed
	// -) connected, complete the client's message,

	TConnectionState conState;
	conState = iServer.ConnectionState(iRemoteAddress);

	if ( conState == EDisconnected )
		{
		// The bearer may indicate connection synchronously, so set this 
		// message _before_ we ask them
		iConnectBearerMsg = aMessage;
		TInt err = iBearerManager.Connect(iRemoteAddress);
		if ( err != KErrNone )
			{
			CompleteClient(iConnectBearerMsg, err);
			}
		}
	else if ( conState == EDisconnecting ||  conState == EConnecting )
		{
		if ( iPendingMsg.Handle() )
			{
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
			return;
			}
		// Store the message, it will get processed later.
		iPendingMsg = aMessage;
		}
	else // EConnected
		{
		CompleteClient(aMessage, KErrNone);
		}
		
	}

void CRemConControllerSession::ConnectBearerCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	if ( iConnectBearerMsg.Handle() )
		{
		CompleteClient(iConnectBearerMsg, KErrCancel);
		}
	else if ( iPendingMsg.Handle() && ( iPendingMsg.Function() == ERemConConnectBearer ))
		{
		CompleteClient(iPendingMsg, KErrCancel);
		}
		
	CompleteClient(aMessage, KErrNone);
	// At no point do we make any change to the processes going on underneath
	// us- 'Cancel' APIs are just for cancelling interest in an async
	// operation.
	}

void CRemConControllerSession::DisconnectBearer(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	if ( iDisconnectBearerMsg.Handle() || iConnectBearerMsg.Handle() || iSendMsg.Handle())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}

	if ( iRemoteAddress.IsNull() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicNotConnectionOriented);
		return;
		}

	if (iSending != ENotSending)
		{
		DoSendCancel();
		}
	EmptySendQueue();
	
	// Check the requested bearer exists.
	TBool bearerExists = iBearerManager.BearerExists(iRemoteAddress.BearerUid());
	// This check was done at GoConnectionOriented time.
	ASSERT_DEBUG(bearerExists);
	// So was this one.
	ASSERT_DEBUG(iBearerManager.CheckPolicy(iRemoteAddress.BearerUid(), aMessage));

	// Check the state of the given connection. If it is:
	// -) connected, request connection to go away,
	// -) disconnected, compete the client's message,
	// -) connecting or disconnecting, add message to the queue of pending 
	//		messages, and process it once connecting/disconnecting has been completed

	TInt err;
	TConnectionState conState;
	conState = iServer.ConnectionState(iRemoteAddress);

	if ( conState == EConnected )
		{
		// The bearer may indicate disconnection synchronously, so set this 
		// message _before_ we ask them
		iDisconnectBearerMsg = aMessage;
		err = iBearerManager.Disconnect(iRemoteAddress);
		if ( err != KErrNone )
			{
			CompleteClient(iDisconnectBearerMsg, err);
			}
		}
	else if ( conState == EDisconnecting ||  conState == EConnecting )
		{
		if ( iPendingMsg.Handle() )
			{
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
			return;
			}
		// Store the message, it will get processed later.
		iPendingMsg = aMessage;
		}
	else //disconnected
		{
		CompleteClient(aMessage, KErrNone);	
		}
	}

void CRemConControllerSession::DisconnectBearerCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	if ( iDisconnectBearerMsg.Handle() )
		{
		CompleteClient(iDisconnectBearerMsg, KErrCancel);
		}
	else if ( iPendingMsg.Handle() && (iPendingMsg.Function() == ERemConDisconnectBearer ))
		{
		CompleteClient(iPendingMsg, KErrCancel);
		}
		
	CompleteClient(aMessage, KErrNone);
	}

/**
Sends a notify message to the remote device.

This function is intended for the RemCon controller client to send a notify
command to the remote device.
*/
void CRemConControllerSession::SendNotify(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we're not already sending...
	if ( iSendMsg.Handle())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicSendAlreadyOutstanding);
		return;
		}
	
	iSendMsg = aMessage;
	
	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	// Check we don't have a disconnect outstanding- this makes no sense from 
	// a client viewpoint (they should cancel the disconnect first).
	// [The client is allowed to have a connect request outstanding- the 
	// bearer manager makes sure a bearer-level connect is not posted on the 
	// same address twice.]
	if ( iDisconnectBearerMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}

	TRAPD(err, DoSendNotifyL(aMessage));
	if ( err != KErrNone )
		{
		CompleteClient(aMessage, err);
		}
	}

/**
@see CRemConControllerSession::SendNotify
*/
void CRemConControllerSession::DoSendNotifyL(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Get the data the client wants to send.
	const TUid interfaceUid = TUid::Uid(aMessage.Int0());
	LOG1(_L("\tinterfaceUid = 0x%08x"), interfaceUid);

	if (aMessage.GetDesLengthL(1) != sizeof(TOperationInformation))
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		return;
		}

	TPckgBuf<TOperationInformation> opInfoPckg;	
	TInt err= aMessage.Read(
			1, // location of the descriptor in the client's message (as we expect them to have set it up)
			opInfoPckg, // descriptor to write to from client memory space
			0 // offset into our descriptor to put the client's data
			);
	
	if ( err != KErrNone )
		{
		LOG1(_L("\taMessage.Read = %d"), err);
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		return;
		}	
	
	const TUint operationId = opInfoPckg().iOperationId;
	LOG1(_L("\toperationId = 0x%02x"), operationId);
	
	const TRemConMessageSubType messageSubType = opInfoPckg().iMessageSubType;
	LOG1(_L("\tmessageSubType = 0x%02x"), messageSubType);
	
	const TUint dataLength = (TUint)aMessage.GetDesLengthL(2);
	LOG1(_L("\tdataLength = %d"), dataLength);
	
	// If the client wanted to send some operation-associated data, read it 
	// from them.
	RBuf8 sendDes;
	if ( dataLength != 0 )
		{
		sendDes.CreateL(dataLength);
		TInt err = aMessage.Read(
			2, // location of the descriptor in the client's message (as we expect them to have set it up)
			sendDes, // descriptor to write to from client memory space
			0 // offset into our descriptor to put the client's data
			);
		// NB We don't do LEAVEIFERRORL(aMessage.Read) because a bad client 
		// descriptor is a panicking offence for them, not an 'error the 
		// request' offence.
		if ( err != KErrNone )
			{
			LOG1(_L("\taMessage.Read = %d"), err);
			sendDes.Close();
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
			return;
			}
		}

	// Before we ask the server to send, we must set our ClientInfo 
	// correctly so the TSP can get information about the client. 
	iClientInfo.Message() = aMessage;

	CRemConMessage* msg = NULL;
	
	if (messageSubType != ERemConNotifyCommandAwaitingInterim)
		{
		sendDes.Close();
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicIllegalIpc);
		return;
		}
	
	CleanupClosePushL(sendDes);
	msg = CRemConMessage::NewL(
			iRemoteAddress, // either specified (if we're connection-oriented) or null (we're connectionless- this field will be filled in by the TSP)
			ERemConNotifyCommand, 
			messageSubType,
			interfaceUid,
			operationId,
			sendDes, // msg takes ownership
			Id(), // session id for when the response comes back
			0, // we let the bearer manager invent a new transaction id when the message gets to it
			ETrue);	
	CLEANUPSTACK_POP1(&sendDes); // now owned by msg
	
	LOG(_L("\tCONTROLLER send"));
	ASSERT_DEBUG(iSendQueue);
	if (iSending != ENotSending || !iSendQueue->IsEmpty())
		{
		iSendQueue->Append(*msg);
		}
	else
		{
		SendToServer(*msg);
		}
	}

void CRemConControllerSession::SendToServer(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	
	// Set our completion members.
	NumRemotes() = 0;
	NumRemotesToTry() = 0;
	SendError() = KErrNone;

	
	iSending = (aMsg.IsReliableSend()) ? ESendingReliable: ESendingUnreliable;
	
	iServer.SendCommand(aMsg);
	}

void CRemConControllerSession::EmptySendQueue()
	{
	LOG_FUNC;

	ASSERT_DEBUG(!iSendMsg.Handle())
	ASSERT_DEBUG(iSendNextCallBack);
	iSendNextCallBack->Cancel();
	CRemConMessage* msg;
	ASSERT_DEBUG(iSendQueue);
	TSglQueIter<CRemConMessage>& iter = iSendQueue->SetToFirst();
	while ((msg = iter++) != NULL)
		{
		iSendQueue->RemoveAndDestroy(*msg);
		}
	}

void CRemConControllerSession::DoSendCancel()
	{
	LOG_FUNC;
	// We must tell the server, and pull the CRemConMessage from the 
	// 'outgoing pending TSP' queue if it's on it. If the TSP is currently 
	// processing the CRemConMessage, we must tell it to stop before we 
	// can complete the RMessage2 iSendMsg- the TSP might still be 
	// dereferencing bits of it. (The TSP is given iSendMsg so it can 
	// access the client's secure ID and do a capability check.)
	// NB This only matters for commands- responses don't go through the 
	// TSP.
	// Not also that this processing *stops* this 
	// CRemConSession::SendCancel method from being the very simple 'I'm 
	// no longer interested in the completion of the asynchronous request' 
	// type of API it (and all cancels) should be. It actually does work 
	// as well. As long as this work is implemented _synchronously_, we 
	// should be OK.
	iServer.SendCancel(*this);

	NumRemotesToTry() = 0;
	iSendError = KErrCancel;
	CompleteSend();
	}

void CRemConControllerSession::CompleteMessage(const CRemConMessage& aMessage)
	{
	LOG_FUNC;

	switch (aMessage.MsgType())
		{
	case ERemConCommand:
	case ERemConResponse:
	case ERemConReject:
		{
		CompleteSend();
		break;
		}
	case ERemConNotifyCommand:
		{
		CompleteSendNotify();
		break;
		}
	default:
		ASSERT_DEBUG(EFalse);
		break;
		}

	}

void CRemConControllerSession::DoReceive()
	{
	// Request messages from the server for this controller session.
	// If there's anything waiting to be given to us, ReceiveRequest will call 
	// back to us with it.
	iServer.ReceiveRequest(*this);	
	}

void CRemConControllerSession::MrcmsoMessageSendResult(const CRemConMessage& aMessage, TInt aError)
	{
	LOG_FUNC;

	// We should not already be sending a message to n remotes
	ASSERT_DEBUG(NumRemotesToTry() == 0);

	SendError() = aError;
	CompleteMessage(aMessage);
	}

void CRemConControllerSession::MrcmsoMessageSendOneOrMoreAttempt(const CRemConMessage& /*aMessage*/, TUint aNumRemotes)
	{
	LOG_FUNC;

	// We should not already be sending a message
	ASSERT_DEBUG(NumRemotesToTry() == 0);

	NumRemotes() = 0;
	NumRemotesToTry() = aNumRemotes;
	SendError() = KErrNone;
	}

void CRemConControllerSession::MrcmsoMessageSendOneOrMoreIncremental(const CRemConMessage& /*aMessage*/, TUint /*aNumRemotes*/)
	{
	LOG_FUNC;

	// This method should never be called, as it is not required to support controller sessions.
	ASSERT_DEBUG(EFalse);
	}

void CRemConControllerSession::MrcmsoMessageSendOneOrMoreAttemptFailed(const CRemConMessage& aMessage, TInt aError)
	{
	LOG_FUNC;

	// We should not already be sending a message
	ASSERT_DEBUG(NumRemotesToTry() == 0);

	NumRemotes() = 0;
	SendError() = aError;
	CompleteMessage(aMessage);
	}

void CRemConControllerSession::MrcmsoMessageSendOneOrMoreResult(const CRemConMessage& aMessage, TInt aError)
	{
	LOG_FUNC;

	// Ignore notification if client has been completed
	if (NumRemotesToTry() > 0)
		{
		// Only set error if different from KErrNone
		if (aError == KErrNone)
			{
			++NumRemotes();
			}
		else
			{
			SendError() = aError;
			}

		--NumRemotesToTry();
		if (NumRemotesToTry() == 0)
			{
			CompleteMessage(aMessage);
			}
		}
	}

void CRemConControllerSession::MrcmsoMessageSendOneOrMoreAbandoned(const CRemConMessage& /*aMessage*/)
	{
	LOG_FUNC;

	// This method should never be called, as it is not required to support controller sessions.
	ASSERT_DEBUG(EFalse);
	}
