// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Remote Control bulk session implementation.
//



/**
 @file
 @internalComponent
*/
#include "bulksession.h"
#include "remconserver.h"

#include "bulkserver.h"
#include "remconmessage.h"
#include "utils.h"

#include "messagequeue.h"

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("bulksess");
#endif

CRemConBulkSession* CRemConBulkSession::NewL(CRemConBulkServer& aServer,
		const RMessage2& aMessage)
	{
	LOG_STATIC_FUNC;
	CRemConBulkSession* self = new(ELeave) CRemConBulkSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL(aMessage);
	CLEANUPSTACK_POP1(self);
	return self;
	}

CRemConBulkSession::CRemConBulkSession(CRemConBulkServer& aServer)
 :	iServer(aServer),
	iId(KNullClientId)
	{
	LOG_FUNC;
	}

void CRemConBulkSession::ConstructL(const RMessage2& aMessage)
	{
	LOG_FUNC;
	
	// Get the client's process ID.
	RThread thread;
	LEAVEIFERRORL(aMessage.Client(thread));
	CleanupClosePushL(thread);
	RProcess process;
	LEAVEIFERRORL(thread.Process(process));
	iClientInfo.ProcessId() = process.Id();
	process.Close();
	iClientInfo.SecureId() = thread.SecureId();
	CleanupStack::PopAndDestroy(&thread);
	
	// Tell the server about us.
	LEAVEIFERRORL(iServer.ClientOpened(*this, iClientInfo.ProcessId()));
	}

CRemConBulkSession::~CRemConBulkSession()
	{
	LOG_FUNC;
	
	iInterestedAPIs.Close();
	
	// Tell the server we've gone away- it may start its shutdown timer.
	iServer.ClientClosed(*this);
	}

void CRemConBulkSession::ServiceL(const RMessage2& aMessage)
	{
	LOG_FUNC;
	LOG1(_L8("\taMessage.Function() = %d"), aMessage.Function());

	// Switch on the IPC number and call a 'message handler'. Message handlers 
	// complete aMessage (either with Complete or Panic), or make a note of 
	// the message for later asynchronous completion.
	// Message handlers should not leave- the server does not have an Error 
	// function. 

	switch ( aMessage.Function() )
		{
	// Heap failure testing APIs.
	case ERemConBulkDbgMarkHeap:
#ifdef _DEBUG
		LOG(_L8("\tmark heap"));
		__UHEAP_MARK;
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;
	
	case ERemConBulkDbgCheckHeap:
#ifdef _DEBUG
		LOG1(_L8("\tcheck heap (expecting %d cells)"), aMessage.Int0());
		__UHEAP_CHECK(aMessage.Int0());
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;
	
	case ERemConBulkDbgMarkEnd:
#ifdef _DEBUG
		LOG1(_L8("\tmark end (expecting %d cells)"), aMessage.Int0());
		__UHEAP_MARKENDC(aMessage.Int0());
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;
	
	case ERemConBulkDbgFailNext:
#ifdef _DEBUG
		{
		LOG1(_L8("\tfail next (simulating failure after %d allocation(s))"), aMessage.Int0());
		if ( aMessage.Int0() == 0 )
			{
			__UHEAP_RESET;
			}
		else
			{
			__UHEAP_FAILNEXT(aMessage.Int0());
			}
		}
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;
	
	case ERemConBulkSend:
		Send(aMessage);
		break;
	
	case ERemConBulkSendUnreliable:
		SendUnreliable(aMessage);
		break;
	
	case ERemConBulkSendCancel:
		SendCancel(aMessage);
		ASSERT_DEBUG(aMessage.Handle() == 0);
		break;
	
	case ERemConBulkReceive:
		Receive(aMessage);
		break;
	
	case ERemConBulkReceiveCancel:
		ReceiveCancel(aMessage);
		ASSERT_DEBUG(aMessage.Handle() == 0);
		break;
	
	default:
		// Unknown message
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicIllegalIpc);
		break;
		}
	}

void CRemConBulkSession::CompleteClient(const RMessage2& aMessage, TInt aError)
	{
	LOG1(_L("\tcompleting client message with %d"), aError);
	TBool cleanClientInfoMessage = (iClientInfo.Message().Handle() == aMessage.Handle());
	aMessage.Complete(aError);
	if(cleanClientInfoMessage)
		{
		iClientInfo.Message() = RMessage2();
		}
	}

TInt CRemConBulkSession::WriteMessageToClient(const CRemConMessage& aMsg)
	{
	LOG_FUNC;

	ASSERT_DEBUG(iReceiveMsg.Handle());
	
	TInt err = KErrNone;
	TPckg<TUint> uid(aMsg.InterfaceUid().iUid);
	
	//check if our client is interested in this API
	//Only need to check commands because it is safe to assume that we are interested 
	//in the response if we have sent out a command.
	
	if(aMsg.MsgType() == ERemConCommand && iInterestedAPIs.Find(aMsg.InterfaceUid())==KErrNotFound)
		{
		//The server will clean up the resource allocated for this msg
		err = KErrArgument;
		}
	else 
		{
		err = iReceiveMsg.Write(0, uid);
		}
		
	if ( err == KErrNone )
		{
		TPckg<TUint> opId(aMsg.OperationId());
		err = iReceiveMsg.Write(1, opId);
		if ( err == KErrNone )
			{
			// This logging code left in for maintenance.
			//LOG1(_L8("\t\tOperationData = \"%S\""), &aMsg.OperationData());
			// Note that we do not panic the client if their descriptor is not 
			// big enough to hold the operation-specific data. If we did, then 
			// a buggy remote could take down a client of RemCon. Just error 
			// the client instead.
			err = iReceiveMsg.Write(2, aMsg.OperationData());
			}
		}

	CompleteClient(iReceiveMsg, err);
	LOG1(_L8("\terr = %d"), err);

	return err;
	}

void CRemConBulkSession::Receive(const RMessage2& aMessage)
	{
	LOG_FUNC;

	if ( iReceiveMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicReceiveAlreadyOutstanding);
		return;
		}

	iReceiveMsg = aMessage;
	// If there's anything waiting to be given to us, ReceiveRequest will call 
	// back to us with it.
	iServer.ReceiveRequest(*this);
	}

void CRemConBulkSession::ReceiveCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	if ( iReceiveMsg.Handle() )
		{
		CompleteClient(iReceiveMsg, KErrCancel);
		}
	CompleteClient(aMessage, KErrNone);
	// At no point do we make any change to the processes going on underneath
	// us- 'Cancel' APIs are just for cancelling interest in an async
	// operation.
	}

void CRemConBulkSession::Send(const RMessage2& aMessage)
	{
	LOG_FUNC;
	TRAPD(err, DoSendL(aMessage));
	CompleteClient(aMessage, err);
	}

void CRemConBulkSession::DoSendL(const RMessage2& aMessage)
	{
	LOG_FUNC;
	
	CRemConMessage* msg = DoCreateMessageL(aMessage, ETrue);
	LEAVEIFERRORL(SendToServer(*msg));
	}

void CRemConBulkSession::SendUnreliable(const RMessage2& aMessage)
	{
	LOG_FUNC;

	CRemConMessage* msg = NULL;
	TRAPD(err, msg = DoCreateMessageL(aMessage, EFalse));
	CompleteClient(aMessage, err);
	if (err == KErrNone)
		{
		static_cast<void>(SendToServer(*msg)); // unreliable so ignore error.
		}
	}

CRemConMessage* CRemConBulkSession::DoCreateMessageL(const RMessage2& aMessage, TBool aReliable)
	{
	LOG_FUNC;

	// Get the data the client wants to send.
	const TUid interfaceUid = TUid::Uid(aMessage.Int0());
	LOG1(_L8("\tinterfaceUid = 0x%08x"), interfaceUid);
	
	const TUint operationId = aMessage.Int1();
	LOG1(_L8("\toperationId = 0x%08x"), operationId);
	
	const TUint dataLength = (TUint)aMessage.GetDesLengthL(2);
	LOG1(_L8("\tdataLength = %d"), dataLength);
	
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
			LOG1(_L8("\taMessage.Read = %d"), err);
			sendDes.Close();
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
			LEAVEL(KErrBadDescriptor);
			}
		}
	CleanupClosePushL(sendDes);

	CRemConMessage* msg = CRemConMessage::NewL(
			TRemConAddress(), // we don't know which remotes it's going to yet
			ERemConResponse, // targets can only send responses
			ERemConMessageDefault,
			interfaceUid,
			operationId,
			sendDes, // msg takes ownership
			Id(), // session id to match this response against the originating command
			0, // transaction id not yet known
			aReliable);
	CLEANUPSTACK_POP1(&sendDes); // now owned by msg
	
	iClientInfo.Message() = aMessage;

	return msg;
	}

TInt CRemConBulkSession::SendToServer(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	return iServer.SendResponse(aMsg, *this);
	}

void CRemConBulkSession::SendCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;
	
	// This is left as an actual function on the server for interface
	// reasons.  In fact the sending is (unlike in the control server)
	// processed synchronously - so currently there is never be anything
	// to cancel when this arrives.
	
	CompleteClient(aMessage, KErrNone);
	}

