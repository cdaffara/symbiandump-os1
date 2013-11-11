// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//#include "ss_std.h"

#define SYMBIAN_NETWORKING_UPS

#include <es_prot.h>
#include <comms-infras/ss_roles.h>
#include <comms-infras/ss_log.h>
#include "es_mbufif.h"
#include <ss_glob.h>
#include <ss_protprov.h>
#include <comms-infras/ss_sapshim.h>
#include "ss_sock.h"
#include <in_sock.h>
#include "ss_sapfactshim.h"
#include <comms-infras/ss_nodemessages_dataclient.h>

#include <comms-infras/ss_msgintercept.h>

#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/ss_platsec_apiext.h>		// MPlatSecApiExt
#endif //SYMBIAN_NETWORKING_UPS

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockS_SOCK, "ESockSSockS_SOCK");
#endif


using namespace ESock;
using namespace Messages;
using namespace Factories;
using namespace Den;

//
#define MSG_PRM(prmIndex)		(prmIndex)

CSocket::CSocket(CSockSession *aSession, CPlayer* aPlayer, const Den::TSubSessionUniqueId aSubSessionUniqueId, TInt aSocketType)
	: CSockSubSession(aSession, aPlayer, aSubSessionUniqueId), ASocket(aSocketType)
/**
Constructor - set up default options.
*/
	{
	LOG_NODE_CREATE(KESockFlowTag, CSocket)
	}

TInt CSocket::SecurityCheck()
    {
	return iSSP->SecurityCheck(Session());
    }

void CSocket::DontCompleteCurrentRequest()
    {
    CSockSubSession::DontCompleteCurrentRequest();
    }

void CSocket::SetClosing()
    {
    CSockSubSession::SetClosing();
    }

TBool CSocket::IsClosing()
    {
    return CSockSubSession::IsClosing();
    }

void CSocket::GetOwnerInfo(TProcessId& aProcId, TSoOwnerInfo& aInfo, TThreadId& aThreadId)
    {
    CSockSubSession::GetOwnerInfo(aProcId, aInfo.iUid, aThreadId);
    }

const RNodeInterface* CSocket::ServiceProvider() const
    {
    return ASocket::ServiceProvider();
    }


void CSocket::SetReturn(TInt aReturnValue) const
    {
    CSockSubSession::SetReturn(aReturnValue);
    }

ASocket* CSocket::GetAcceptingSocket()
    {
    CSocket* newCon=Session()->CSocketFromHandle(GetUserMessage(ESocketConnectMessage)->ReadInt(1));
    return newCon;
    }


CSocket* CSocket::NewLC(TServerProtocolDesc *aServiceInfo, CSockSession *aSession, CPlayer* aPlayer, CProtocolBase* aProt, const Den::TSubSessionUniqueId aSubSessionUniqueId, TInt aSocketType)
/**
Common static cocreator to create all sockets.

*/
	{
	CSocket *s = new(ELeave) CSocket(aSession, aPlayer, aSubSessionUniqueId, aSocketType);
	CleanupStack::PushL(s);
	s->ConstructL(aProt);
	s->Create(aServiceInfo);
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(ESockDebug::KSocketNodeUid, s);
	return s;
	}


CSocket::~CSocket()
/**
Socket destructor. Will ensure that the SSP (and any accept queue) is shutdown before destroying it.
Automatically completes all outstanding requests.

*/
	{
	// Toast anything we've got hanging around....
	FinalCompleteAllBlockedMessages(KErrAbort);
	LOG_NODE_DESTROY(KESockFlowTag, CSocket)
	}

void CSocket::ConstructL (CProtocolBase* aProtocol)
	{
	InitUserMessageL (ESocketCurrentMessage);
	InitUserMessageL (ESocketReadMessage);
	InitUserMessageL (ESocketWriteMessage);
	InitUserMessageL (ESocketCloseMessage);
	InitUserMessageL (ESocketIoCtlMessage);				
	InitUserMessageL (ESocketConnectMessage);	
	InitUserMessageL (ESocketSetLocalNameMessage);		
	CSockSubSession::ConstructL(aProtocol);		
	}

void CSocket::InitiateDestruction()
    {
	// Remove the subsession from the session's subsession list.
    if(iSession)
		{
		iSession->SubSessions().Lock();

		CSubSessionIx::TSubSessionHandle handle;
		if(iSession->SubSessions().Find(this, handle) == KErrNone)
			{
			iSession->PitBoss().RemoveSubSession(handle, iSession);
			}
		
		iSession->SubSessions().Unlock();
		}

    ASocket::InitiateDestruction();
	
	if (!FlowRequestPending())
		{
		delete this;
		}
    }

void CSocket::InitUserMessageL ( TSocketMessage aMessage )
	{
	CSocketMessage* msg = new (ELeave)CSocketMessage;
	SetUserMessage ( aMessage, msg );
	}


void CSocket::FinalCompleteAllBlockedMessages(TInt aResult)
	{
	CompleteConnect(aResult);
	CompleteClose(aResult);
	CompleteRead(aResult);
	CompleteWrite(aResult);
	CompleteIoctl(aResult);
	CompleteSetLocalName(aResult);
	}

void CSocket::ShutdownL()
/**
Terminate the protocol
*/
	{
    ASocket::ShutdownL(RSocket::TShutdown(Message().Int0())/*type*/, Message().Ptr1() != NULL);
	}

TBool CSocket::CloseSocket()
/**
A Client has closed our RSocket - or we're being closed by the session because our client has exited
*/
	{
	TBool immediate = ASocket::CloseSocket();
	if (immediate)
		{
		static_cast<CPlayer&>(Player()).DeleteSocket(*this);
		}
	return immediate;
	}

void CSocket::ConnectL()
/**
Active open the socket - from a client request.
*/
	{
	ASocket::ConnectL(Message().Ptr1() != NULL);
	}

//indexes of the data elements in RMessage for Get/SetOpt
#define  OPT_NAME_INDEX		    0
#define  OPT_OPTION_INDEX		1 //for the option value
#define  OPT_OPT_LENGTH_INDEX	2
#define  OPT_OPT_LEVEL_INDEX	3

/**
    Set a socket option from a client request
*/
void CSocket::SetSockOptionL()
	{
	TInt optionName=Message().Int0();
	TInt optionLength=Message().GetDesLengthL(1);
	TInt optionLevel=Message().Int2();

    ASocket::SetSockOptionL(optionName, optionLength, optionLevel);
	}

void CSocket::GetSockOptionL()
/**
Read a socket option from us (or attempt the protocol if we don't support the option)

*/
	{
	TInt optionName=Message().Int0();
	TInt optionLength=SafeMessage().GetDesMaxLengthL(1);
	TInt optionLevel=Message().Int2();
    ASocket::GetSockOptionL(optionName, optionLength, optionLevel, Message().Ptr1() != NULL);
	}

void CSocket::IoctlL()
/**
Perform an Ioctl from a user request
*/
	{
	TInt optionName = Message().Int0();
	TInt optionLength = (Message().Ptr1() != NULL) ? SafeMessage().GetDesMaxLengthL(1) : 0;
	TInt optionLevel =Message().Int2();
    ASocket::IoctlL(optionName, optionLength, optionLevel, Message().Ptr1() != NULL);
	}


void CSocket::ListenL()
/**
Listen request service
Listen is a dead and, clients can't do anything with the socket other than Accept after a listen.
*/
	{
   	TInt qlen = Message().Int0()>0 ? Message().Int0() : 1;
    ASocket::ListenL(qlen, Message().Ptr1() != NULL);
	}

ASocket* CSocket::InitiateAcceptingSocket()
    {
    // Create the accepting socket - we have to do this outside the subsessions lock
    // because the lock does not support recursion.
	CSocket* acceptingSocket = NULL;
	TInt acceptingHandle = 0;
	TRAPD(ret, acceptingSocket = static_cast<CPlayer&>(Player()).NewSocketL(EFalse, acceptingHandle));
	if(ret != KErrNone)
		{
		SetReturn(ret);
		return NULL;
		}
	
	CSocket* nullSocket = NULL;	
	TBool found = EFalse;

	// Now that we have the new socket created on the Player we replace the null one with it.
		{
		CSubSessionIx& subSessions = iSession->SubSessions();
		subSessions.Lock();

		// Find the null socket - we do this inside the subsessions lock to ensure it cannot
		// be orphaned until we are finished.
		const TInt nullSocketHandle = Message().Int1();
		nullSocket = static_cast<CSocket*>(subSessions.At(nullSocketHandle, TCFSubSessInfo::ESocket));
		if(nullSocket)
			{
			if (nullSocket->State()!=ESStateNull)
				{
				PanicSocketClient(EAcceptTwice);
		
				subSessions.Unlock();

				return NULL;
				}
			
			found = subSessions.At(acceptingHandle, TCFSubSessInfo::ESocket) != NULL;
			if(found)
				{
				LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSocket::InitiateAcceptingSocket() replacing null socket %08x with accepting socket %08x"), nullSocket, acceptingSocket) );
				subSessions.Remove(acceptingHandle);
				subSessions.Replace(nullSocketHandle, acceptingSocket);
				//We need to ensure that the socket does not get accepted onto twice
				acceptingSocket->SetState(ESStateAccepting);
				}
			else
				{
				LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSocket::InitiateAcceptingSocket() accepting socket %08x not found - the session is probably being destroyed"), acceptingSocket, nullSocket) );
				}
			}
		else
			{
			PanicSocketClient(ESockBadHandle);
		
			subSessions.Unlock();

			return NULL;
			}
		
		subSessions.Unlock();
		}
	
	// Destroy the null socket if the replacement was successful.
	if(found)
		{
		RAllocator* prevAllocator = Player().PitBoss().MaybeSwitchHeap(nullSocket->Player().WorkerId());
		LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSocket::InitiateAcceptingSocket() destroying null socket %08x"), nullSocket) );
		nullSocket->InitiateDestruction();
		if(prevAllocator)
			{
			User::SwitchAllocator(prevAllocator);
			}
		}
	else
		{
		// Accepting socket will be deleted by session cleanup.
		acceptingSocket = NULL;
		SetReturn(KErrCancel);
		}
		
	return acceptingSocket;
    }
    
void CSocket::PanicSocketClient(TESockPanic aPanic)
	{
	CWorkerSubSession::PanicClient(KESockClientPanic, (TInt) aPanic);
	}


/** Provides a sizeable buffer for strictly temporary use, eg within current stack frame: there's only one buffer
 * and no protocol for returning so shared use can't be detected: the intended use is for transferring data to/from 
 * a client. 
 * @return pointer to a buffer of at least the requested size, or NULL if this proves impossible
 */ 
TDes8* CSocket::BorrowTemporaryBuffer(TInt aSize)
    {
    return static_cast<CPlayer&>(Player()).BorrowTemporaryBuffer(aSize);
    }

/** Provides a sizeable buffer for strictly temporary use, eg within current stack frame: there's only one buffer
 * and no protocol for returning so shared use can't be detected: the intended use is for transferring data to/from 
 * a client. 
 * @return Pointer to a buffer of at least the requested size
 * @leave KErrNoMemory if the request cannot be satisfied
 */ 
TDes8* CSocket::BorrowTemporaryBufferL(TInt aSize)
    {
    return static_cast<TDes8*>(User::LeaveIfNull(static_cast<CPlayer&>(Player()).BorrowTemporaryBuffer(aSize)));
    }

void CSocket::ReferenceL()
/**
Get a global reference to this socket.
*/
	{
	TName name;
	ComposeSubSessionName(this, name);
	Message().WriteL(MSG_PRM(0), name);
	SetReturn(KErrNone);
	}

void CSocket::ProcessMessageL()
	{
	RSafeMessage& msg = const_cast<RSafeMessage&>(SafeMessage());
	static_cast<CSocketMessage*>(iCurrentMsg)->SetMessage(msg);
	// local flag to indicate processed messages
	// cleared in the case of ESoClose - socket deletion
	TBool processed =ETrue;
	switch(msg.Function())
		{
		case ESoShutdown:
			ShutdownL();
			break;

		case ESoClose:
            CloseSocket();
            processed = EFalse; // no need to reset iMessage handle
			break;

		case ESoSendToNoLength:		// flags,addr,buf
			{
		    TInt sendByteCount = msg.GetDesLengthL(2);
		    TInt sendFlags = msg.Int0();   
		    ASocket::SendL(ASocket::KNoXferLen, MSG_PRM(1), sendByteCount, sendFlags, EFalse);
			break;
			}
		case ESoSendNoLength:		// flags,buflen,buf
			{
		    TInt sendByteCount = msg.Int1();	// sanity-checked by SendL() 
		    TInt sendFlags = msg.Int0();   
		    ASocket::SendL(ASocket::KNoXferLen, ASocket::KNoAddrArg, sendByteCount, sendFlags, EFalse);
			break;
			}
		case ESoSendTo:				//xferlen(flags),addr,buf
			{
		    TInt sendByteCount = msg.GetDesLengthL(2);
			ReadParamL(ESocketCurrentMessage,MSG_PRM(0),iXferLength);
			TInt sendFlags = iXferLength();
		    ASocket::SendL(MSG_PRM(0), MSG_PRM(1), sendByteCount, sendFlags, EFalse);
			break;
			}
		case ESoSend:				//flags,xferlen,buf
			{
		    TInt sendByteCount = msg.GetDesLengthL(2);
		    TInt sendFlags = msg.Int0();   
		    ASocket::SendL(MSG_PRM(1), ASocket::KNoAddrArg, sendByteCount, sendFlags, EFalse);
			break;
			}
		case ESoWrite:				//0,buflen,buf
			{
		    TInt sendByteCount = msg.Int1();	// sanity-checked by SendL() 
		    TInt sendFlags = 0;   
		    ASocket::SendL(ASocket::KNoXferLen, ASocket::KWriteNoAddrArg, sendByteCount, sendFlags, EFalse);
			break;
			}
		case ESoRecvFromNoLength:	//flags,addr,buf
			{
		    TInt readByteCount = msg.GetDesMaxLengthL(2);
		    TInt readFlags = msg.Int0();
		    ASocket::RecvL(KNoXferLen, MSG_PRM(1), readByteCount, readFlags, EFalse, EFalse);
			break;
			}
		case ESoRecvNoLength:		//flags,bufmaxlen,buf
			{
		    TInt readByteCount = Message().Int1();
		    TInt readFlags = Message().Int0();
		    ASocket::RecvL(KNoXferLen, KNoAddrArg, readByteCount, readFlags, EFalse, EFalse);
			}
			break;
		case ESoRecvOneOrMoreNoLength:	//flags,bufmaxlen,buf
			{
		    TInt readByteCount = Message().Int1();
		    TInt readFlags = Message().Int0();
		    ASocket::RecvL(KNoXferLen, KNoAddrArg, readByteCount, readFlags, EFalse, ETrue);
			}
			break;
		case ESoRecvOneOrMore:		//flags,xferlen,buf
			{
		    TInt readByteCount = msg.GetDesMaxLengthL(2);
		    TInt readFlags = Message().Int0();
		    ASocket::RecvL(MSG_PRM(1), KNoAddrArg, readByteCount, readFlags, EFalse, ETrue);
			}
			break;
		case ESoRecvFrom:			//xferlen(flags),addr,buf
			{
		    TInt readByteCount = msg.GetDesMaxLengthL(2);
			ReadParamL(ESocketCurrentMessage,MSG_PRM(0),iXferLength);
			TInt readFlags = iXferLength();
		    ASocket::RecvL(MSG_PRM(0), MSG_PRM(1), readByteCount, readFlags, EFalse, EFalse);
			}
			break;
		case ESoRecv:				//flags,xferlen,buf
			{
		    TInt readByteCount = msg.GetDesMaxLengthL(2);
		    TInt readFlags = Message().Int0();
		    ASocket::RecvL(MSG_PRM(1), KNoAddrArg, readByteCount, readFlags, EFalse, EFalse);
			}
			break;
		case ESoRead:				//0,bufmaxlen,buf
			{
		    TInt readByteCount = Message().Int1();
		    ASocket::RecvL(KNoXferLen, KNoAddrArg, readByteCount, 0, EFalse, EFalse);
			}
			break;

		case ESoConnect:
			ConnectL();
			break;

		case ESoBind:
			BindL();
			break;

		case ESoAccept:
			Accept();
			break;

		case ESoListen:
			ListenL();
			break;

		case ESoSetOpt:
			SetSockOptionL();
			break;

		case ESoGetOpt:
			GetSockOptionL();
			break;

		case ESoIoctl:
			IoctlL();
			break;

		case ESoGetDiscData:
			GetDisconnectDataL();
			break;

		case ESoGetLocalName:
			LocalNameL();
			break;

		case ESoGetRemoteName:
			RemoteNameL();
			break;

		case ESoCancelRecv:
			CancelRecv();
			break;

		case ESoCancelSend:
			CancelSend();
			break;

		case ESoCancelIoctl:
			CancelIoctl();
			break;

		case ESoCancelConnect:
			CancelConnect();
			break;

		case ESoCancelAccept:
			CancelAccept();
			break;

		case ESoCancelAll:
			CancelAll();
			break;

		case ESoSocketInfo:
		   GetInfoL();
			break;

		case ESoReference:
			ReferenceL();
			break;

		default:
			SetReturn(KErrNotSupported);
		}
	// message for this CSocket has been processed - can reset iMessage handle
	if (processed)
	    {
	    static_cast<CSocketMessage*>(iCurrentMsg)->ProcessedMessage();
	    }
	}

void CSocket::CompleteFlowRequestMessage(TInt err)
	{
	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSocket::CompleteFlowRequestMessage() socket (%08X) completing flow request message (%08X) with %d"), this, iFlowRequestMessage.Handle(), err) );
	iFlowRequestMessage.Complete(err);
	}

// ------------------------------------------------
TInt CSocketMessage::ReadDes(TInt aSrcParamIndex,TDes8 &aDes,TInt anOffset)
	{
	return iMessage.Read(aSrcParamIndex, aDes, anOffset);
	}

TInt CSocketMessage::ReadInt(TInt aSrcParamIndex)
	{
	TInt param = 0;
	switch (aSrcParamIndex)
		{
	case 0:
		param = iMessage.Int0();
		break;
		
	case 1:
		param = iMessage.Int1();
		break;
		
	case 2:
		param = iMessage.Int2();
		break;
	
	case 3:
		param = iMessage.Int3();
		break;
		
	default:
		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockS_SOCK, 1));
		}
	return param;
	}

TInt CSocketMessage::ReadMBuf(TInt /* aSrcParamIndex */, RMBufChain& /* aBufChain */)
	{
	// Currently we don't support writing into RMBufChains
	User::Invariant();
	return KErrNotSupported;
	}

TInt CSocketMessage::WriteDes(TInt aDstParamIndex,const TDesC8& aDes,TInt anOffset)
	{
	return iMessage.Write(aDstParamIndex, aDes, anOffset);
	}

TInt CSocketMessage::WriteMBuf(TInt /* aDstParamIndex */,RMBufChain& /* aBufChain */)
	{
	// Currently we don't support writing into RMBufChains
	User::Invariant();
	return KErrNotSupported;
	}
	
void CSocketMessage::InitMBuf(TInt /* aParamIndex */)
	{
	// Currently we don't support RMBufChains
	User::Invariant();		
	}

void CSocketMessage::CompleteMessage(TInt anError)
	{
	iMessage.Complete(anError);
	}

void CSocketMessage::AcquireMessage(AMessage* aMessage)
	{
	CSocketMessage* msg = (static_cast<CSocketMessage*>(this));
	CSocketMessage* msg2 = (static_cast<CSocketMessage*>(aMessage));	
	msg->SetMessage(msg2->Message());
	}

TBool CSocketMessage::IsNull (TInt aParamIndex)
	{
	switch (aParamIndex)	
		{
		case 0:
		return (iMessage.Ptr0() == NULL);

		case 1:
		return (iMessage.Ptr1() == NULL);		
		
		case 2:
		return (iMessage.Ptr2() == NULL);				
		
		case 3:
		return (iMessage.Ptr3() == NULL);						
		
		default:
		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockS_SOCK, 2));
		}
	return EFalse;
	}

// --------------------------------------------------------------

#ifdef SYMBIAN_NETWORKING_UPS
TInt CSocketMessage::GetProcessAndThreadId(TProcessId& aProcessId, TThreadId& aThreadId) const
	{
	return ASockSubSessionPlatsecApiExt::GetProcessAndThreadIdFromRMessage(Message(), aProcessId, aThreadId);
	}
#endif
