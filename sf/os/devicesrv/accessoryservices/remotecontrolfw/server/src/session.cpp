// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Remote Control session implementation.
// 
//

/**
 @file
 @internalComponent
*/
#include "e32base.h"

#include <bluetooth/logger.h>
#include <remcon/remconbearerinterface.h>
#include <remcon/remconifdetails.h>
#include <s32mem.h>
#include "utils.h"
#include "server.h"
#include "bearermanager.h"
#include "remconmessage.h"
#include "connections.h"
#include "session.h"
#include "messagequeue.h"
#include "remconserver.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("session");
#endif


CRemConSession::CRemConSession(CRemConServer& aServer,
		CBearerManager& aBearerManager,
		TUint aId)
 :	iServer(aServer),
	iBearerManager(aBearerManager),
	iId(aId)
	{
	LOG_FUNC;
	}

void CRemConSession::BaseConstructL(const TClientInfo& aClientInfo)
	{
	LOG_FUNC;

	iClientInfo = aClientInfo;

	iSendQueue = CMessageQueue::NewL();

	// The send callback is used by the base class to handle queued sends.
	iSendNextCallBack = new(ELeave) CAsyncCallBack(CActive::EPriorityStandard);
	TCallBack cb(SendNextCb, this);
	iSendNextCallBack->Set(cb);

	}

CRemConSession::~CRemConSession()
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	
	delete iSendNextCallBack;
	delete iSendQueue;
	delete iInterestedAPIs;
	}

void CRemConSession::ServiceL(const RMessage2& aMessage)
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	LOG1(_L("\taMessage.Function() = %d"), aMessage.Function());
	// Switch on the IPC number and call a 'message handler'. Message handlers 
	// complete aMessage (either with Complete or Panic), or make a note of 
	// the message for later asynchronous completion.
	// Message handlers should not leave- the server does not have an Error 
	// function. 

	switch ( aMessage.Function() )
		{
	// Heap failure testing APIs.
	case ERemConDbgMarkHeap:
#ifdef _DEBUG
		LOG(_L("\tmark heap"));
		__UHEAP_MARK;
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;

	case ERemConDbgCheckHeap:
#ifdef _DEBUG
		LOG1(_L("\tcheck heap (expecting %d cells)"), aMessage.Int0());
		__UHEAP_CHECK(aMessage.Int0());
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;

	case ERemConDbgMarkEnd:
#ifdef _DEBUG
		LOG1(_L("\tmark end (expecting %d cells)"), aMessage.Int0());
		__UHEAP_MARKENDC(aMessage.Int0());
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;

	case ERemConDbgFailNext:
#ifdef _DEBUG
		{
		LOG1(_L("\tfail next (simulating failure after %d allocation(s))"), aMessage.Int0());
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

	case ERemConSetPlayerType:
		SetPlayerType(aMessage);
		// This is a sync API- check that the message has been completed.
		// (NB We don't check the converse for async APIs because the message 
		// may have been panicked synchronously.)
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConSend:
		Send(aMessage);
		break;
				
	case ERemConSendUnreliable:
		SendUnreliable(aMessage);
		break;
	
	case ERemConSendCancel:
		SendCancel(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConReceive:
		Receive(aMessage);
		break;

	case ERemConReceiveCancel:
		ReceiveCancel(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConGetConnectionCount:
		GetConnectionCount(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConGetConnections:
		GetConnections(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConNotifyConnectionsChange:
		NotifyConnectionsChange(aMessage);
		break;

	case ERemConNotifyConnectionsChangeCancel:
		NotifyConnectionsChangeCancel(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;
		
	case ERemConRegisterInterestedAPIs:
		RegisterInterestedAPIs(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;
		
	case ERemConGoConnectionOriented:
		GoConnectionOriented(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConGoConnectionless:
		GoConnectionless(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConConnectBearer:
		ConnectBearer(aMessage);
		break;

	case ERemConConnectBearerCancel:
		ConnectBearerCancel(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConDisconnectBearer:
		DisconnectBearer(aMessage);
		break;

	case ERemConDisconnectBearerCancel:
		DisconnectBearerCancel(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;
		
	case ERemConSendNotify:
		SendNotify(aMessage);
		break;
		
	default:
		// Unknown message
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicIllegalIpc);
		break;
		}
	}

void CRemConSession::CompleteClient(const RMessage2& aMessage, TInt aError)
	{
	LOG1(_L("\tcompleting client message with %d"), aError);
	TBool cleanClientInfoMessage = (iClientInfo.Message().Handle() == aMessage.Handle());
	aMessage.Complete(aError);
	if(cleanClientInfoMessage)
		{
		iClientInfo.Message() = RMessage2();
		}
	}

void CRemConSession::GetPlayerTypeAndNameL(const RMessage2& aMessage, TPlayerTypeInformation& aPlayerType, RBuf8& aPlayerName)
	{
	// check validity of descriptors
	if (aMessage.GetDesLength(1) < 0 || aMessage.GetDesLength(2) < 0)
		{
		LEAVEL(KErrBadDescriptor);
		}

	// Retrieve and validate the client type information 
	TPckg<TPlayerTypeInformation> pckg(aPlayerType);
	aMessage.ReadL(1, pckg);
	switch (aPlayerType.iPlayerType)
		{
	case ERemConAudioPlayer:
		// Valid
	case ERemConVideoPlayer:
		// Valid
	case ERemConBroadcastingAudioPlayer:
		// Valid
	case ERemConBroadcastingVideoPlayer:
		// Valid
		break;
	default:
		// Invalid
		LEAVEL(KErrArgument);
		}
	switch (aPlayerType.iPlayerSubType)
		{
	case ERemConNoSubType:
		// Valid
	case ERemConAudioBook:
		// Valid
	case ERemConPodcast:
		// Valid
		break;
	default:
		// Invalid
		LEAVEL(KErrArgument);
		}

	// Retrieve the client player name inforamtion
	aPlayerName.CreateL(aMessage.GetDesLengthL(2));
	CleanupClosePushL(aPlayerName);
	aMessage.ReadL(2, aPlayerName);	
	CleanupStack::Pop(&aPlayerName);
	}

void CRemConSession::Send(const RMessage2& aMessage)
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

	// Before we ask the server to send, we must set our ClientInfo 
	// correctly so the TSP can get information about the client. 
	iClientInfo.Message() = aMessage;

	// Prepare the message for send. If DoPrepareSendMessageL() returns
	// NULL, it panicked the client.
	CRemConMessage* msg = NULL;
	TRAPD(err, msg = DoPrepareSendMessageL(aMessage));

	if ( err != KErrNone )
		{
		CompleteClient(aMessage, err);
		}
	else if (msg)
		{
		ASSERT_DEBUG(iSendQueue);
	
		if (iSending != ENotSending || !iSendQueue->IsEmpty())
			{
			iSendQueue->Append(*msg);
			}
		else
			{
			// we know msg cannot be null here as said above.
			SendToServer(*msg);
			}
		}
	}

void CRemConSession::SendCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	// See comments in ConnectBearerCancel.
	if ( iSendMsg.Handle() )
		{
		DoSendCancel();
		}
	
	if (iSendMsg.Handle())
		{
		CRemConMessage* msg;
		TBool first = ETrue;
		ASSERT_DEBUG(iSendQueue);
		TSglQueIter<CRemConMessage>& iter = iSendQueue->SetToFirst();
		while ((msg = iter++) != NULL)
			{
			if (msg->IsReliableSend())
				{
				CompleteClient(iSendMsg, KErrCancel);
				iSendQueue->RemoveAndDestroy(*msg);
				if (first)
					{
					ASSERT_DEBUG(iSendNextCallBack);
					iSendNextCallBack->Cancel();
					}
				break;
				}
			first = EFalse;
			}
		}
	
	CompleteClient(aMessage, KErrNone);
	}

void CRemConSession::Receive(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Messages are pushed from bearers, so we 
	// (a) do some sanity checking, 
	// (b) check the queue of incoming messages in case there's anything 
	// already waiting to be given to the client.

	if ( iReceiveMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicReceiveAlreadyOutstanding);
		return;
		}

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	iReceiveMsg = aMessage;

	DoReceive();
	}

void CRemConSession::ReceiveCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	// See comments in ConnectBearerCancel.
	if ( iReceiveMsg.Handle() )
		{
		CompleteClient(iReceiveMsg, KErrCancel);
		}
	CompleteClient(aMessage, KErrNone);
	}

void CRemConSession::GetConnectionCount(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	// Get the answer to the question- the number of connections at the 
	// current point in time (i.e. the latest entry in the connection 
	// history).
	const TUint connCount = iServer.Connections().Count();
	LOG1(_L("\tconnCount = %d"), connCount);
	TPckg<TUint> count(connCount);
	TInt err = aMessage.Write(0, count);

	// If the client was told the answer with no error, then remember the 
	// current point in the connection history, so that when the client asks 
	// for the connections themselves, we give them a consistent answer.
	if ( err == KErrNone )
		{
		iServer.SetConnectionHistoryPointer(Id());
		}
	CompleteClient(aMessage, err);
	}

void CRemConSession::GetConnections(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	// Get the array of connections at the point in the history we're 
	// interested in and write it back to the client. NB This is not 
	// necessarily the Last item in the history but the item that we were 
	// pointing at when GetConnectionCount was called.
	const CConnections& conns = iServer.Connections(iId);
	const TUint count = conns.Count();
	LOG1(_L("\tcount = %d"), count);
	RBuf8 buf;
	TInt err = buf.Create(count * sizeof(TRemConAddress));
	if ( err == KErrNone )
		{
		TSglQueIter<TRemConAddress>& iter = conns.SetToFirst();
		TRemConAddress* addr;
		while ( ( addr = iter++ ) != NULL )
			{
			buf.Append((TUint8*)addr, sizeof(TRemConAddress));
			}

		// Write back to the client...
		err = aMessage.Write(0, buf);
		buf.Close();
		if ( err != KErrNone )
			{
			// We don't need to call SetConnectionHistoryPointer here because 
			// the server will do it when it cleans up the panicked client.
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
			return;
			}	   		
		}

	// Whether or not there was an error, we're no longer interested in the 
	// history item we're currently registered as being interested in, so tell 
	// the server to bump up our pointer to the current (latest) one. NB This 
	// may in fact be the same record, if no connection changes have occurred 
	// since GetConnectionCount was called, but it's still important to give 
	// the server a chance to remove obsolete history records.
	iServer.SetConnectionHistoryPointer(Id());
	CompleteClient(aMessage, err);
	}

void CRemConSession::NotifyConnectionsChange(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Messages are pushed to us from bearers, so we don't need anything more 
	// than some sanity checking here.
	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	if ( iNotifyConnectionsChangeMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicConnectionsNotificationAlreadyOutstanding);
		}
	else
		{
		iNotifyConnectionsChangeMsg = aMessage;
		// Check the connection history for any more recent items than that we 
		// currently know about. If our pointer into the connection history 
		// isn't pointing at the 'current' item, we can complete the 
		// notification immediately and move the pointer up.
		if ( !iServer.ConnectionHistoryPointerAtLatest(Id()) )
			{
			CompleteClient(iNotifyConnectionsChangeMsg, KErrNone);
			iServer.SetConnectionHistoryPointer(Id());
			}
		}
	}

void CRemConSession::NotifyConnectionsChangeCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our features set...
	if (!ClientAvailable())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientFeaturesNotSet);
		return;
		}

	// See comments in ConnectBearerCancel.
	if ( iNotifyConnectionsChangeMsg.Handle() )
		{
		CompleteClient(iNotifyConnectionsChangeMsg, KErrCancel);
		}
	CompleteClient(aMessage, KErrNone);
	}

CRemConInterfaceDetailsArray* CRemConSession::ExtractInterestedAPIsL(const RMessage2& aMessage)
	{
	LOG_FUNC;
	
	CRemConInterfaceDetailsArray* result;
	
	RBuf8 buf;
	buf.CreateL(aMessage.GetDesLengthL(0));
	CleanupClosePushL(buf);
	
	aMessage.ReadL(0, buf);
	RDesReadStream ipcStream(buf);
	
	result = CRemConInterfaceDetailsArray::InternalizeL(ipcStream);
	
	ipcStream.Close(); 
	CleanupStack::PopAndDestroy(&buf);
	
	return result;
	}

TBool CRemConSession::DoGetSendInfoLC(const RMessage2& aMessage, 
		TUid& aInterfaceUid,
		TUint& aOperationId,
		TRemConMessageSubType& aMessageSubType,
		RBuf8& aSendDes)
	{
	// Get the data the client wants to send.
	aInterfaceUid = TUid::Uid(aMessage.Int0());
	LOG1(_L("\taInterfaceUid = 0x%08x"), aInterfaceUid);

	if (aMessage.GetDesLengthL(1) != sizeof(TOperationInformation))
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		return EFalse;
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
		return EFalse;
		}	
	
	aOperationId = opInfoPckg().iOperationId;
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);
	
	aMessageSubType = opInfoPckg().iMessageSubType;
	LOG1(_L("\taMessageSubType = 0x%02x"), aMessageSubType);

	const TUint dataLength = (TUint)aMessage.GetDesLengthL(3);
	LOG1(_L("\tdataLength = %d"), dataLength);
	
	// If the client wanted to send some operation-associated data, read it 
	// from them.
	if ( dataLength != 0 )
		{
		aSendDes.CreateL(dataLength);
		TInt err = aMessage.Read(
			3, // location of the descriptor in the client's message (as we expect them to have set it up)
			aSendDes, // descriptor to write to from client memory space
			0 // offset into our descriptor to put the client's data
			);
		// NB We don't do LEAVEIFERRORL(aMessage.Read) because a bad client 
		// descriptor is a panicking offence for them, not an 'error the 
		// request' offence.
		if ( err != KErrNone )
			{
			LOG1(_L("\taMessage.Read = %d"), err);
			aSendDes.Close();
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
			return EFalse;
			}
		}
	CleanupClosePushL(aSendDes);
	return ETrue;
	}

void CRemConSession::GoConnectionOriented(const RMessage2& aMessage)
	{
	PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
	}

void CRemConSession::GoConnectionless(const RMessage2& aMessage)
	{
	PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
	}

void CRemConSession::ConnectBearer(const RMessage2& aMessage)
	{
	PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
	}

void CRemConSession::ConnectBearerCancel(const RMessage2& aMessage)
	{
	PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
	}

void CRemConSession::DisconnectBearer(const RMessage2& aMessage)
	{
	PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
	}

void CRemConSession::DisconnectBearerCancel(const RMessage2& aMessage)
	{
	PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
	}

void CRemConSession::SendNotify(const RMessage2& aMessage)
	{
	PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
	}

void CRemConSession::ConnectionsChanged()
	{
	LOG_FUNC;

	LOG1(_L("\tiNotifyConnectionsChangeMsg.Handle = %d"), iNotifyConnectionsChangeMsg.Handle());
	if ( iNotifyConnectionsChangeMsg.Handle() )
		{
		// Set the connection history pointer to point to the latest item and then complete the 
		// NotifyConnectionChange request of the client.
		iServer.SetConnectionHistoryPointer(Id());
		CompleteClient(iNotifyConnectionsChangeMsg, KErrNone);
		}
	}

void CRemConSession::CompleteSend()
	{
	LOG_FUNC;
	LOG2(_L("\tiNumRemotes = %d, iSendError = %d"), iNumRemotes, iSendError);

	ASSERT_DEBUG(NumRemotesToTry() == 0);
	
	if (iSending == ESendingReliable)
		{
		if ( iSendError == KErrNone )
			{
			TPckg<TUint> count(iNumRemotes);
			// 2 is the slot in the client's message for the number of remotes the 
			// message got sent to.
			iSendError = iSendMsg.Write(2, count);
			}
		CompleteClient(iSendMsg, iSendError);
		}
	
	ASSERT_DEBUG(iSendQueue);
	if (!iSendQueue->IsEmpty())
		{
		ASSERT_DEBUG(iSendNextCallBack);
		iSendNextCallBack->CallBack();
		}
	iSending = ENotSending;
	}

void CRemConSession::CompleteSendNotify()
	{
	LOG_FUNC;
	LOG1(_L("\tiSendError = %d"), iSendError);

	if (iSending == ESendingReliable)
		{
		CompleteClient(iSendMsg, iSendError);
		}

	ASSERT_DEBUG(iSendQueue);
	if (!iSendQueue->IsEmpty())
		{
		ASSERT_DEBUG(iSendNextCallBack);
		iSendNextCallBack->CallBack();
		}
	iSending = ENotSending;
	}

void CRemConSession::PanicSend(TRemConClientPanic aCode)
	{
	LOG_FUNC;
	LOG1(_L("\taCode = %d"), aCode);

	PANIC_MSG(iSendMsg, KRemConClientPanicCat, aCode);
	}

TInt CRemConSession::WriteMessageToClient(const CRemConMessage& aMsg)
	{
	LOG_FUNC;

	ASSERT_DEBUG(SupportedMessage(aMsg));
	ASSERT_DEBUG(iReceiveMsg.Handle());
	TRAPD(err, WriteMessageToClientL(aMsg));
	CompleteClient(iReceiveMsg, err);

	LOG1(_L("\terr = %d"), err);
	return err;
	}
	
void CRemConSession::WriteMessageToClientL(const CRemConMessage& aMsg)
	{
	LOG_FUNC;
	
	//check if our client is interested in this API
	//Only need to check commands because it is safe to assume that we are interested 
	//in the response if we have sent out a command.
	if(aMsg.MsgType() == ERemConCommand && !FindInterfaceByUid(aMsg.InterfaceUid()))
		{
		//The server will clean up the resource allocated for this msg
		LEAVEL(KErrArgument);
		}

	// This logging code left in for maintenance.
	//LOG1(_L("\t\tOperationData = \"%S\""), &aMsg.OperationData());
	
	TRemConClientReceivePackage receivePackage;
	receivePackage.iInterfaceUid = aMsg.InterfaceUid();
	receivePackage.iOperationId = aMsg.OperationId();
	receivePackage.iMessageSubType = aMsg.MsgSubType();
	receivePackage.iRemoteAddress = aMsg.Addr();
	
	TPckgC<TRemConClientReceivePackage> recPckg(receivePackage);
	LEAVEIFERRORL(iReceiveMsg.Write(0, recPckg));
	
	// Note that we do not panic the client if their descriptor is not 
	// big enough to hold the operation-specific data. If we did, then 
	// a buggy remote could take down a client of RemCon. Just error 
	// the client instead.
	LEAVEIFERRORL(iReceiveMsg.Write(1, aMsg.OperationData()));
	}

TInt CRemConSession::SupportedInterfaces(RArray<TUid>& aUids)
	{
	LOG_FUNC;

	aUids.Reset();
	return AppendSupportedInterfaces(aUids);
	}

TInt CRemConSession::AppendSupportedInterfaces(RArray<TUid>& aUids)
	{
	LOG_FUNC;
	ASSERT_DEBUG(iInterestedAPIs);
	TInt err = KErrNone;

	TInt count = iInterestedAPIs->Array().Count();
	for(TInt i=0; (i<count) && (err == KErrNone); i++)
		{
		CRemConInterfaceDetails* details = iInterestedAPIs->Array()[i];
		ASSERT_DEBUG(details);
		err = aUids.Append(details->Uid());
		}
	
	return err;
	}

TInt CRemConSession::SupportedBulkInterfaces(RArray<TUid>& aUids)
	{
	LOG_FUNC;

    	aUids.Reset();
	return AppendSupportedBulkInterfaces(aUids);
	}

TInt CRemConSession::AppendSupportedBulkInterfaces(RArray<TUid>& aUids)
	{
	LOG_FUNC;
	ASSERT_DEBUG(iInterestedAPIs);
	TInt err = KErrNone;

	TInt count = iInterestedAPIs->Array().Count();
	for(TInt i=0; (i<count) && (err == KErrNone); i++)
		{
		CRemConInterfaceDetails* details = iInterestedAPIs->Array()[i];
		ASSERT_DEBUG(details);
		if(details->IsBulk())
			{
			err = aUids.Append(details->Uid());
			}
		}

	return err;
	}

TInt CRemConSession::SupportedOperations(TUid aInterfaceUid, RArray<TUint>& aOperations)
	{
	LOG_FUNC;
	TInt err = KErrNotSupported;
	CRemConInterfaceDetails* details = FindInterfaceByUid(aInterfaceUid);
	
	if(details)
		{
		TRAP(err, details->GetRemConInterfaceFeaturesL(aOperations));
		}
	return err;
	}

CRemConInterfaceDetails* CRemConSession::FindInterfaceByUid(TUid aUid) const
	{
	LOG_FUNC;
	ASSERT_DEBUG(iInterestedAPIs);
	TInt count = iInterestedAPIs->Array().Count();
	for(TInt ix=0; ix<count; ++ix)
		{
		CRemConInterfaceDetails* details = iInterestedAPIs->Array()[ix];
		ASSERT_DEBUG(details);
		if(details->Uid() == aUid)
			{
			return details;
			}
		}
	return NULL;
	}

void CRemConSession::DoSendNext()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iSendQueue);
	CRemConMessage& msg = iSendQueue->First();
	iSendQueue->Remove(msg);
	SendToServer(msg);
	}

TInt CRemConSession::SendNextCb(TAny *aThis)
	{
	LOG_STATIC_FUNC;

	static_cast<CRemConSession*>(aThis)->DoSendNext();
	return KErrNone;
	}

