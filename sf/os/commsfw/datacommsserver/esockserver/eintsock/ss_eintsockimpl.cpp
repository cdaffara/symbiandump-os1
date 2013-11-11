// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// eintsockimpl.cpp
// @file
// @internalTechnology
//
//

#include "ss_eintsocklog.h"
#include <comms-infras/eintsock.h>
#include "ss_eintsockimpl.h"
#include <in_sock.h>
#include <ss_pman.h>
#include <ss_glob.h>
#include <comms-infras/ss_roles.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/cfmacro.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockEIntSocksnt, "ESockEIntSocksnt");
#endif

using namespace CommsFW;
using namespace ESock;
using namespace Messages;
using namespace Den;

#define MSG_PRM(prmIndex)		(prmIndex)

//
// CInternalSocketImpl
//

//
// Replacement for RSocket::Open gubbins - need to do it slightly differently since we're in the esock thread
//

CInternalSocketImpl* CInternalSocketImpl::NewL(const CInternalSockSubSession::TParameters& aParams)
/**
Create a new (blank) internal socket for use in socket Accept() call
@note This will Open() the protocol as part of the Accept() call
*/
	{
	CInternalSocketImpl* self = new(ELeave) CInternalSocketImpl(aParams, KUndefinedSockType);
	CleanupStack::PushL(self);	
	self->ConstructL(aParams, NULL, NULL);
	CleanupStack::Pop(self);
	return(self);
	}

CInternalSocketImpl* CInternalSocketImpl::NewL ( const CInternalSockSubSession::TParameters& aParams, TServerProtocolDesc* aServiceInfo, CProtocolBase* aProt )
	{
	CInternalSocketImpl* self = new (ELeave) CInternalSocketImpl ( aParams, aServiceInfo->iSockType );
	CleanupStack::PushL(self);	
	self->ConstructL ( aParams, aServiceInfo, aProt );
	CleanupStack::Pop(self);
	return(self);	
	}
	
CInternalSocketImpl::CInternalSocketImpl(const CInternalSockSubSession::TParameters& aParams, TUint aSockType)
: CInternalSockSubSession(aParams),
  ASocket (aSockType)
/**
Just some random bits and pieces of setup
*/
	{
	LOG_NODE_CREATE(KESockFlowTag, CInternalSocketImpl)
	}


void CInternalSocketImpl::ConstructL ( const TParameters& aParams, TServerProtocolDesc* aServiceInfo, CProtocolBase* aProt)
	{
	LOG(Log::Printf(_L("CInternalSocketImpl[%x]: ConstructL() called."), this));
	CInternalSockSubSession::ConstructL(aParams);
	ASocket::Create (aServiceInfo);	
	
	// Open a reference to the protocol we using
	__ASSERT_DEBUG(iProtocol == NULL, User::Panic(KSpecAssert_ESockEIntSocksnt, 1));
	iProtocol = aProt;
	if(iProtocol)
		{
		iProtocol->Open();
		}
	
	InitUserMessageL (ESocketCurrentMessage);
	InitUserMessageL (ESocketReadMessage);
	InitUserMessageL (ESocketWriteMessage);
	InitUserMessageL (ESocketCloseMessage);
	InitUserMessageL (ESocketIoCtlMessage);				
	InitUserMessageL (ESocketConnectMessage);	
	InitUserMessageL (ESocketSetLocalNameMessage);		

	// Post a request to bind to a flow.
	if ( aServiceInfo )	
		{
		TFlowParams flowParams(
			aServiceInfo->iAddrFamily,
			aServiceInfo->iSockType,
			aServiceInfo->iProtocol,
			TFlowParams::EImplicit,
			NULL
			);

		RClientInterface::OpenPostMessageClose(
			Id(),                     //socket is the sender
			SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane)), //phoney recipient - we only care the recipient is on the control plane so that this message is dispatched on the correct thread
			TCFImplicitFlowRequest(flowParams)
			);

		SetFlowRequestPending(ETrue);
		AdoptFlowRequest(*(aParams.iRequest));
		DontCompleteCurrentRequest();
		}	
	}	
	
/**
Cleanup various objects that we own and/or use.
*/
CInternalSocketImpl::~CInternalSocketImpl()
	{
	// Close the reference to the protocol we have open
	if(iProtocol)
		{
		iProtocol->Close();
		}

	LOG(Log::Printf(_L("CInternalSocketImpl[%x]: destructor called."), this));
	iLink.Deque ();
	FinalCompleteAllBlockedMessages(KErrAbort);	
	LOG_NODE_DESTROY(KESockFlowTag, CInternalSocketImpl)	
	}

void CInternalSocketImpl::FinalCompleteAllBlockedMessages(TInt aResult)
	{
	// We don't check for whether the socket is orphaned as opposed to CSocket. 
	// Our internal subsession exists. Just complete everything.
	CompleteConnect(aResult);
	CompleteClose(aResult);
	CompleteRead(aResult);
	CompleteWrite(aResult);
	CompleteIoctl(aResult);
	CompleteSetLocalName(aResult);
	}
	
/**
Cleanup the resources used by the socket.  This may close the socket immediately, or, if the
protocol supports graceful close, may wait for the protocol to call CanClose().
*/
TBool CInternalSocketImpl::CloseSocket()
	{
	LOG(Log::Printf(_L("CInternalSocketImpl[%x]: CloseSocket() called..."), this));		
	TBool immediate = ASocket::CloseSocket ();
	if ( immediate )
		{
		// We have to do the destruction when ASocket::CloseSocket has returned ETrue.
		// It returns ETrue only when you need to really delete the implementation else
		// it will return EFalse
		InitiateDestruction();
		}
	return immediate;	
	}

void CInternalSocketImpl::SetClosing()
	{
	iClosing = ETrue;	
	}

TBool CInternalSocketImpl::IsClosing()
	{
	return iClosing;	
	}

void CInternalSocketImpl::CompleteCurrentRequest ( TBool aValue )
	{
	iComplete = aValue;		
	}

void CInternalSocketImpl::DontCompleteCurrentRequest()
	{
	CompleteCurrentRequest ( EFalse );
	}

TBool CInternalSocketImpl::ShouldCompleteCurrentRequest () const
	{
	return iComplete;		
	}


ASocket* CInternalSocketImpl::InitiateAcceptingSocket()
	{
	RInternalSocket* blankSocket = reinterpret_cast < RInternalSocket* > ( iCurrentMsg->ReadInt(1) );

    CInternalSocketImpl* nullSocket = blankSocket->Implementation();	
	if(!nullSocket)
		{
		PanicSocketClient(ESockBadHandle);
		return NULL;
		}
	if (nullSocket->State() != ESStateNull)
		{
		PanicSocketClient(EAcceptTwice);
		return NULL;
		}
	//We need to ensure that the socket does not get accepted onto twice
	nullSocket->SetState(ESStateAccepting);
	return nullSocket;
	
	}

ASocket* CInternalSocketImpl::GetAcceptingSocket()
	{
	RInternalSocket* blankSocket = reinterpret_cast < RInternalSocket* > ( GetUserMessage(ESocketConnectMessage)->ReadInt(1) );	
    return blankSocket->Implementation ();		
	}
	
void CInternalSocketImpl::SetReturn(TInt aReturnValue) const
	{
	iReturn = aReturnValue;
	}

void CInternalSocketImpl::GetOwnerInfo(TProcessId& aProcId, TSoOwnerInfo& aInfo, TThreadId& aThreadId)
	{
	const RThread& thread = iFlowRequest.PeerThread ();
	aThreadId = thread.Id ();
	RProcess process;
	thread.Process ( process );
	aProcId = process.Id ();
	aInfo.iUid = process.Type ();
	}

TInt CInternalSocketImpl::SecurityCheck()
	{
	return iSSP->SecurityCheck(this);
	}

void CInternalSocketImpl::SetCurrentMessage ( TEIntSockOpMsgWrapper& aIntSockMsg )
	{
	(static_cast<CIntSocketMessage*>(iCurrentMsg))->SetMessage(aIntSockMsg);
	}

void CInternalSocketImpl::AdoptFlowRequest ( TRequestWrapper& aRequest )
	{
	iFlowRequest = aRequest;		
	}

void CInternalSocketImpl::CompleteFlowRequestMessage(TInt aErr )
	{
	// We are erroring. So we have to set the RInternalSocket iImplementation pointer
	// as NULL. Otherwise it will hold the pointer even after the deletion.
	if ( aErr != KErrNone && !IsClosing() )
		*iImpl = NULL;
	iFlowRequest.RequestComplete ( aErr );
	}


void CInternalSocketImpl::InitiateDestruction()
	{
  	LOG(Log::Printf(_L("CInternalSocketImpl[%x]: InitiateDestruction() called..."), this));
    ASocket::InitiateDestruction();

	if (!FlowRequestPending())
		{
		delete this;
		}		
	}

void CInternalSocketImpl::InitUserMessageL ( TSocketMessage aMessage )
	{
	CIntSocketMessage* msg = new (ELeave)CIntSocketMessage;
	SetUserMessage ( aMessage, msg );
	}


void CInternalSocketImpl::PanicSocketClient(TESockPanic aPanic)
	{
    InternalSocketPanic( aPanic );		
	}

void CInternalSocketImpl::SendL ( TSockMess aWriteFunction, TEIntSockOpMsgWrapper& aMsg )
	{
	TInt xferLenIdx = ASocket::KNoXferLen;
	TInt addrIdx = ASocket::KNoAddrArg;
	TInt sendFlags = 0;
	switch(aWriteFunction)
		{
	case ESoSendTo:
		addrIdx = MSG_PRM(1);
		// fall-through
	case ESoSend:
		{
		xferLenIdx = 0; 
		TSockXfrLength xferLen;
	    User::LeaveIfError(aMsg.ReadDes(MSG_PRM(0), xferLen, 0));
		sendFlags = xferLen();
		}
		break;
	case ESoSendToNoLength:
		addrIdx = MSG_PRM(1);
		// fall-through
	case ESoSendNoLength:
		sendFlags = aMsg.ReadInt(0);
		break;
	case ESoWrite:
	    addrIdx = ASocket::KWriteNoAddrArg;
		break;
	default:
		ASSERT(0);
		}
	TBool sendMBuf = aMsg.ReadInt( 3 );
    TInt sendByteCount = ( sendMBuf ? aMsg.GetMBufChainLength( 2 ) : aMsg.GetDesLength ( 2 ) );
    ASocket::SendL( xferLenIdx, addrIdx, sendByteCount, sendFlags, sendMBuf );		
	}

void CInternalSocketImpl::RecvL ( TSockMess aReadFunction, TEIntSockOpMsgWrapper& aMsg )
	{
	TInt xferLenIdx = ASocket::KNoXferLen;
	TInt addrIdx = ASocket::KNoAddrArg;
	TInt recvFlags = 0;
    TBool recvMBuf = aMsg.ReadInt( 3 );
	TBool recvOneOrMore = (aReadFunction == ESoRecvOneOrMore) || (aReadFunction == ESoRecvOneOrMoreNoLength) || (IsStream() && recvMBuf);

	switch(aReadFunction)
		{
	case ESoRecvFrom:
		addrIdx = MSG_PRM(1);
		// fall-through
	case ESoRecv:
	case ESoRecvOneOrMore:
		{
		xferLenIdx = 0; 
		TSockXfrLength xferLen;
	    User::LeaveIfError(aMsg.ReadDes(MSG_PRM(0), xferLen, 0));
		recvFlags = xferLen();
		break;
		}
	case ESoRecvFromNoLength:
		addrIdx = MSG_PRM(1);
		// fall-through
	case ESoRecvNoLength:
	case ESoRecvOneOrMoreNoLength:
		recvFlags = aMsg.ReadInt(0);
		break;
	case ESoRead:
		break;
	default:
		ASSERT(0);
		}
    TInt readByteCount = ( recvMBuf ? KSocketDefaultBufferSize : aMsg.GetDesMaxLength ( 2 ) );
    ASocket::RecvL(xferLenIdx, addrIdx, readByteCount, recvFlags, recvMBuf, recvOneOrMore );		
	}

void CInternalSocketImpl::CancelOpen ()
	{
	if ( FlowRequestPending() )	
		{
		SetClosing ();
		}
	}

void CInternalSocketImpl::SetImpl ( CInternalSocketImpl*& aImpl )
	{
	iImpl = &aImpl;		
	}

TDes8* CInternalSocketImpl::BorrowTemporaryBuffer(TInt aSize)
	{
	return static_cast<CPlayer*>(iOwnerThread->Player())->BorrowTemporaryBuffer(aSize);
	}

TDes8* CInternalSocketImpl::BorrowTemporaryBufferL(TInt aSize)
    {
    return static_cast<TDes8*>(User::LeaveIfNull(static_cast<CPlayer*>(iOwnerThread->Player())->BorrowTemporaryBuffer(aSize)));
    }

//
// CInternalSockSubSession
//

CInternalSockSubSession::TGlobals::TGlobals()
: iSubSessions(CInternalSockSubSession::LinkOffset())
	{
	}

CInternalSockSubSession::TParameters::TParameters(TWorkerId aClientWorkerId, MCommsTransportSender* aSender, TTransportUserStorage& aStorage, const TNodeId& aServerCookie, TRequestWrapper* aRequest)
: iStorage(aStorage),
  iClientWorkerId(aClientWorkerId),
  iServerCookie(aServerCookie),
  iSender(aSender),
  iRequest(aRequest)
  	{
	}

CInternalSockSubSession::CInternalSockSubSession(const TParameters& aParams)
: iAccessCount(1),
  iServerCookie(aParams.iServerCookie),
  iSender(aParams.iSender)
	{
	}

void CInternalSockSubSession::ConstructL(const TParameters& aParams)
	{
	if(aParams.iStorage.Ptr() == NULL)
		{
		aParams.iStorage.RefPtr() = new(ELeave) TGlobals;
		}
	AddSubsession(aParams.iStorage, this);
	iOwnerThread = SockManGlobals::Get()->SelfWorker();	// using globals is ugly, but reduced coupling 
	}

CInternalSockSubSession::TSubSessions& CInternalSockSubSession::SubSessions(TTransportUserStorage& aStorage)
	{
	TGlobals* globals = Globals(aStorage);
	__ASSERT_DEBUG(globals, User::Panic(KSpecAssert_ESockEIntSocksnt, 2));
	return globals->iSubSessions;
	}

void CInternalSockSubSession::AddSubsession(TTransportUserStorage& aStorage, CInternalSockSubSession* aSubSession)
	{
	__ASSERT_DEBUG(aSubSession, User::Panic(KSpecAssert_ESockEIntSocksnt, 3));
	SubSessions(aStorage).AddFirst(*aSubSession);
	}

TInt CInternalSockSubSession::PerformOperationL(const TEIntSockMsg& aMsg, TRequestWrapper& aRequest)
	{
	CInternalSocketImpl* sock = aMsg.ImplPtr ();
	__ASSERT_DEBUG (sock, User::Invariant());
	
	TInt ret = KRequestPending;
	sock->CompleteCurrentRequest (ETrue); // Mark the current request as completed
	TEIntSockOpMsgWrapper opMsg = aMsg.OpMsg ();	
	opMsg.SetRequest ( aRequest ); // Set the current request status
	sock->SetCurrentMessage ( opMsg ); // Make it as the current request
	sock->SetReturn(KErrNone);
	
	switch(aMsg.Operation())
		{
		case ESoClose:
			{
			if ( sock->CloseSocket () )  // Immediate close. so we have to signal it.
				{
				// Need to return now to skip over code that calls into socket at
				// end of function - the socket has been deleted so we must not do
				// this.
				return KErrNone;
				}
			// Protocol expects a graceful close
			break;
			}
		case ESoCancelAll:
			{		
			sock->CancelAll ();
			break;
			}
		case ESoSendNoLength:
		case ESoSend:
		case ESoWrite:
		case ESoSendTo:
		case ESoSendToNoLength:
			{
			sock->SendL ( aMsg.Operation(), opMsg );
			break;
			}
			
		case ESoCancelSend:
			{
			sock->CancelSend();
			break;
			}
		case ESoRecvNoLength:
		case ESoRecv:	
		case ESoRead:
		case ESoRecvFrom:
		case ESoRecvFromNoLength:
		case ESoRecvOneOrMore:
			{
			sock->RecvL ( aMsg.Operation(), opMsg );
			break;				
			}
		case ESoCancelRecv:
			{
			sock->CancelRecv();
			break;
			}
		case ESoConnect:
			{
			sock->ConnectL (opMsg.ReadInt ( 1 ) != NULL);
			break;
			}
		case ESoCancelConnect:
			{
			sock->CancelConnect();
			break;
			}

		case ESoBind:
			{
			sock->BindL ();
			break;			
			}
			
		case ESoListen:
			{
   			TInt qlen = opMsg.ReadInt ( 0 ) > 0 ? opMsg.ReadInt ( 0 ) : 1;			
			sock->ListenL (qlen, opMsg.ReadInt ( 1 ) != NULL );
			break;
			}
		case ESoAccept:
			{
			sock->Accept ();
			break;
			}
		case ESoCancelAccept:
			{
			sock->CancelAccept();
			break;
			}

		case ESoSetOpt:
			{		
			sock->SetSockOptionL (opMsg.ReadInt(0), opMsg.GetDesLength(1), opMsg.ReadInt(2));
			break;
			}

		case ESoGetOpt:
			{
			TInt optionLength = opMsg.GetDesMaxLength ( 1 );
    		sock->GetSockOptionL(opMsg.ReadInt (0), optionLength, opMsg.ReadInt (2), optionLength != 0 );
			break;
			}
		case ESoIoctl:
			{
			TInt optionLength = ( opMsg.ReadInt (1) != NULL ) ? opMsg.GetDesMaxLength ( 1 ) : 0;
    		sock->IoctlL(opMsg.ReadInt (0), optionLength, opMsg.ReadInt (2), (opMsg.ReadInt (1) != NULL));		
			break;
			}
		case ESoCancelIoctl:
			{
			sock->CancelIoctl();
			break;
			}
		case ESoGetDiscData:
			{
			sock->GetDisconnectDataL ();
			break;
			}
		case ESoGetLocalName:
			{
			sock->LocalNameL();
			break;
			}
		case ESoGetRemoteName:
			{
			sock->RemoteNameL();
			break;
			}

		case ESoShutdown:
			{
			sock->ShutdownL(RSocket::TShutdown(opMsg.ReadInt(0)), (opMsg.ReadInt(1) != NULL));
			break;
			}
		case ESoSocketInfo:
			{
			sock->GetInfoL ();
			break;
			}
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockEIntSocksnt, 4));			
		}

	if ( ret == KRequestPending && sock->ShouldCompleteCurrentRequest() )
		ret = sock->Return ();
	return ret;
	}

void CInternalSockSubSession::ProcessMessageL(const TWorkerTransportMsg& aMsg, TRequestWrapper& aRequest, TTransportUserStorage& aStorage, TWorkerId aPeerWorkerId)
	{
	const TEIntSockMsgExt& eisMsgExt = static_cast<const TEIntSockMsgExt&>(aMsg);
	LOG(Log::Printf(_L("CISSS::ProcessMessage() msgId=%d, status=%08x, storage=%x, peerId=%d"), eisMsgExt.Operation(), eisMsgExt.GetStatusPtr(), aStorage.Ptr(), aPeerWorkerId));

	CInternalSocketImpl* sock = NULL;				
	TInt ret = KRequestPending;

	switch (eisMsgExt.Operation())
		{
		case KSoOpenByName:
			{
			TWorkerId selfWorkerId;
			VERIFY_RESULT(CWorkerThread::CurrentWorkerId(selfWorkerId), KErrNone);
			CInternalSockSubSession::TParameters params (aPeerWorkerId, eisMsgExt.Sender(), aStorage, eisMsgExt.ImplCookie(), &aRequest);	
			eisMsgExt.ImplPtrRef() = sock = ProtocolManager::NewInternalSocketL(&params, eisMsgExt.OpMsg().GetDesc(0)); // Name is on 0th index	
			sock->SetImpl (eisMsgExt.ImplPtrRef());
			break;
			}
		case ESoCreate:
			{
			TWorkerId selfWorkerId;
			VERIFY_RESULT(CWorkerThread::CurrentWorkerId(selfWorkerId), KErrNone);
			CInternalSockSubSession::TParameters params (aPeerWorkerId, eisMsgExt.Sender(), aStorage, eisMsgExt.ImplCookie(), &aRequest);
			eisMsgExt.ImplPtrRef() = sock = ProtocolManager::NewInternalSocketL(&params, eisMsgExt.OpMsg().ReadInt(0), eisMsgExt.OpMsg().ReadInt(1), eisMsgExt.OpMsg().ReadInt(2));
			sock->SetImpl (eisMsgExt.ImplPtrRef());			
			break;
			}
		case ESoCreateNull:
			{
			TWorkerId selfWorkerId;
			VERIFY_RESULT(CWorkerThread::CurrentWorkerId(selfWorkerId), KErrNone);			
			eisMsgExt.ImplPtrRef() = CInternalSocketImpl::NewL(CInternalSockSubSession::TParameters(aPeerWorkerId, eisMsgExt.Sender(), aStorage, eisMsgExt.ImplCookie(), NULL));
			ret = KErrNone;
			break;
			}
		default:
			ret = CInternalSockSubSession::PerformOperationL (eisMsgExt, aRequest);
			break;
		}

	if(ret != KRequestPending)
		{
		aRequest.RequestComplete(ret);
		}
	}

EXPORT_C void CInternalSockSubSession::ProcessMessage(const TWorkerTransportMsg& aMsg, TRequestWrapper& aRequest, TTransportUserStorage& aStorage, TWorkerId aPeerWorkerId)
	{
	__ASSERT_DEBUG(aMsg.Code() == KEIntSockTransportPluginImplementationUid, User::Panic(KSpecAssert_ESockEIntSocksnt, 5));
	TInt ret = KErrNone;

	TRAP ( ret, CInternalSockSubSession::ProcessMessageL ( aMsg, aRequest, aStorage, aPeerWorkerId ) );
	if(ret != KErrNone)
		{
		aRequest.RequestComplete(ret);
		}
	
	}

EXPORT_C void CInternalSockSubSession::OnPeerDeath(TWorkerId aPeer, TTransportUserStorage& aStorage)
	{
	// Close any subsessions belonging to the dead peer, ie mimic their missing closes
	TDblQueIter<CInternalSockSubSession> iter(SubSessions(aStorage));
	CInternalSockSubSession* subSess;
	while((subSess = iter++) != NULL)
		{
		if(subSess->ClientWorkerId() == aPeer)
			{
			delete subSess;
			}
		}
	}

EXPORT_C void CInternalSockSubSession::Shutdown(TBool aAlreadyDead, TTransportUserStorage& aStorage)
	{
	// Complete all blocked messages
	TDblQueIter<CInternalSockSubSession> iter(SubSessions(aStorage));
	CInternalSockSubSession* subSess;
	while((subSess = iter++) != NULL)
		{
		subSess->FinalCompleteAllBlockedMessages(KErrServerTerminated);
		if(!aAlreadyDead)
			{
			delete subSess;				
			}
		}
	if(!aAlreadyDead)
		{
		TGlobals* globals = Globals(aStorage);
		delete globals;
		}
	}

TInt CInternalSockSubSession::SendReceiveMsg(TWorkerTransportMsg& aMsg, MCommsTransportSender* aSender, const TNodeId& aServerCookie)
	{
	TRequestStatus status(KRequestPending);
	aMsg.SetStatusPtr(&status);
	TCFLegacyMessagePacker::PackForPostage(aServerCookie.Thread(), aMsg);
	aSender->SendMessageSync(aMsg);
	User::WaitForRequest(status);
	return status.Int();
	}

void CInternalSockSubSession::SendMsg(TWorkerTransportMsg& aMsg, MCommsTransportSender* aSender, const TNodeId& aServerCookie)
	{
	TCFLegacyMessagePacker::PackForPostage(aServerCookie.Thread(), aMsg);
	aSender->SendMessageSync(aMsg);
	}

TInt CInternalSockSubSession::SendReceiveMsgAsync( TWorkerTransportMsg& aMsg, MCommsTransportSender* aSender, const TNodeId& aServerCookie )
	{
	CAsyncWaiter* waiter = CAsyncWaiter::New ();
	if ( !waiter )
		return KErrNoMemory;
	waiter->iStatus = KRequestPending;
	aMsg.SetStatusPtr(&(waiter->iStatus));
	TCFLegacyMessagePacker::PackForPostage(aServerCookie.Thread(), aMsg);
	aSender->SendMessageSync(aMsg);
	TInt err = waiter->StartAndWait();
	delete waiter;	
	return err;	
	}
// ------------------------

CAsyncWaiter* CAsyncWaiter::New ()
	{
	CAsyncWaiter* self = new CAsyncWaiter;
	return self;
	}

CAsyncWaiter::~CAsyncWaiter ()
	{
	Cancel();
	}

TInt CAsyncWaiter::StartAndWait ()
	{
	SetActive ();
	CActiveScheduler::Start ();	
	return iStatus.Int();
	}

CAsyncWaiter::CAsyncWaiter ()
: CActive ( EPriorityStandard )
	{
	CActiveScheduler::Add(this);	
	}

void CAsyncWaiter::RunL ()
	{
	CActiveScheduler::Stop ();	
	}

void CAsyncWaiter::DoCancel ()
	{
	CActiveScheduler::Stop ();			
	}

// ------------------------------

void CIntSocketMessage::AcquireMessage ( AMessage* aMessage )
	{
	CIntSocketMessage* msg = (static_cast<CIntSocketMessage*>(this));
	CIntSocketMessage* msg2 = (static_cast<CIntSocketMessage*>(aMessage));	
	msg->SetMessage(msg2->Message());
	}

TInt CIntSocketMessage::ReadDes(TInt aSrcParamIndex,TDes8 &aDes,TInt aOffset)
	{
	return iMessage.ReadDes(aSrcParamIndex, aDes, aOffset);		
	}

TInt CIntSocketMessage::ReadInt(TInt aSrcParamIndex)
	{
	return iMessage.ReadInt(aSrcParamIndex);				
	}

TInt CIntSocketMessage::ReadMBuf(TInt aSrcParamIndex, RMBufChain& aBufChain)
	{
	return iMessage.ReadBufChain(aSrcParamIndex, aBufChain);		
	}

void CIntSocketMessage::InitMBuf(TInt aParamIndex)
	{
	iMessage.InitBufChain ( aParamIndex );		
	}

TInt CIntSocketMessage::WriteDes(TInt aDstParamIndex,const TDesC8& aDes,TInt anOffset)
	{
	return iMessage.WriteDesc ( aDstParamIndex, aDes, anOffset );		
	}

TInt CIntSocketMessage::WriteMBuf(TInt aDstParamIndex,RMBufChain& aBufChain)
	{
	return iMessage.WriteBufChain(aDstParamIndex, aBufChain);		
	}

void CIntSocketMessage::CompleteMessage(TInt anError)
	{
	iMessage.CompleteMessage(anError);
	}

TBool CIntSocketMessage::IsNull (TInt aParamIndex)
	{
	return (iMessage.ReadInt(aParamIndex) == NULL);
	}





