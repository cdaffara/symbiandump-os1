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
// eintsock.cpp
// Implementation of forwarding methods for internal sockets
// 
//

/**
 @file
 @internalComponent
*/

#include "ss_eintsocklog.h"
#include <comms-infras/eintsock.h>
#include <comms-infras/ss_common.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_thread.h>
#include "ss_eintsockimpl.h"
#include "ss_eintsock_msgs.h"
#include <ss_glob.h>
#include <comms-infras/sockmes.h>

using namespace ESock;
using namespace CommsFW;
using namespace Messages;

EXPORT_C RInternalSocket::RInternalSocket()
: iImplementation(NULL)
	{
    }

EXPORT_C RInternalSocket::~RInternalSocket()
	{
    }

EXPORT_C TInt RInternalSocket::Open()
/**
Special Open() for this class to mimic RSocket, but without the socket server session being required
@see RSocket documentation
*/
	{
	__ASSERT_DEBUG(!iImplementation, InternalSocketPanic(ENotNullSocket));
	LOG(Log::Printf(_L("RIS::Open()")));

	TWorkerId selfWorkerId;
	TInt ret = CWorkerThread::CurrentWorkerId(selfWorkerId);
	if(ret == KErrNone)
		{
		TNodeId serverCookie;
		ret = CWorkerThread::CookieForWorkerId(selfWorkerId, serverCookie);
		if(ret == KErrNone)
			{
			MCommsTransportSender* sender = &static_cast<MCommsTransportSender&>(TlsGlobals::Get().TransportSender());
			TEIntSockOpMsgWrapper opMsg;
			TEIntSockMsgExt openMsg(ESoCreateNull, opMsg, iImplementation, sender, serverCookie);
			ret = CInternalSockSubSession::SendReceiveMsg(openMsg, sender, serverCookie);
			}
		}
	LOG(Log::Printf(_L("RIS::Open() ret=%d impl=%08x"), ret, iImplementation));
	return ret;
	}

EXPORT_C TInt RInternalSocket::Open(const TDesC& aName)
/**
Special Open() for this class to mimic RSocket, but without the socket server session being required
With the asynchronous nature of COMMS FW this API might cause deadlock scenario, hence this API is deprecated 
and asynchrnous version is provided. 

@see RInternalSocket::Open(const TDesC& aName, TRequestStatus& aStatus)

@deprecated from 9.5
@see RSocket documentation
*/
	{
	__ASSERT_DEBUG(!iImplementation, InternalSocketPanic(ENotNullSocket));
	LOG(Log::Printf(_L("RIS::Open(%S)"), &aName));
	
	TNodeId serverCookie;
	TInt ret = CWorkerThread::CookieForProtocolByName(aName, serverCookie);
	if(ret == KErrNone)
		{
		MCommsTransportSender* sender = &static_cast<MCommsTransportSender&>(TlsGlobals::Get().TransportSender());
		TEIntSockOpMsgWrapper opMsg (&aName);
		TEIntSockMsgExt openMsg((TSockMess)KSoOpenByName, opMsg, iImplementation, sender, serverCookie);
		ret = CInternalSockSubSession::SendReceiveMsgAsync(openMsg, sender, serverCookie);
		}
	LOG(Log::Printf(_L("RIS::Open(%S)=%d impl=%08x"), &aName, ret, iImplementation));
	return ret;
	}

EXPORT_C TInt RInternalSocket::Open(TUint aAddrFamily,TUint aSockType,TUint aProtocol)
/**
Special Open() for this class to mimic RSocket, but without the socket server session being required
With the asynchronous nature of COMMS FW this API might cause deadlock scenario, hence this API is deprecated 
and asynchrnous version is provided. 

@see RInternalSocket::Open(TUint aAddrFamily,TUint aSockType,TUint aProtocol, TRequestStatus& aStatus)

@see RSocket documentation
*/
	{
	__ASSERT_DEBUG(!iImplementation, InternalSocketPanic(ENotNullSocket));
	LOG(Log::Printf(_L("RIS::Open(%d, %d, %d)"), aAddrFamily, aSockType, aProtocol));
	
	TNodeId serverCookie;
	TInt ret = CWorkerThread::CookieForProtocol(aAddrFamily, aSockType, aProtocol, serverCookie);
	if(ret == KErrNone)
		{
		MCommsTransportSender* sender = &static_cast<MCommsTransportSender&>(TlsGlobals::Get().TransportSender());
		TEIntSockOpMsgWrapper opMsg (aAddrFamily, aSockType, aProtocol);
		TEIntSockMsgExt openMsg(ESoCreate, opMsg, iImplementation, sender, serverCookie);		
		ret = CInternalSockSubSession::SendReceiveMsgAsync(openMsg, sender, serverCookie);
		}
	LOG(Log::Printf(_L("RIS::Open(%d, %d, %d)=%d impl=%08x"), aAddrFamily, aSockType, aProtocol, ret, iImplementation));
	return ret;
	}

EXPORT_C void RInternalSocket::Open( const TDesC& aName, TRequestStatus& aStatus )
	{
	__ASSERT_DEBUG(!iImplementation, InternalSocketPanic(ENotNullSocket));
	LOG(Log::Printf(_L("RIS::Open(%S)"), &aName));
	
	aStatus = KRequestPending;
	
	TNodeId serverCookie;
	TInt ret = CWorkerThread::CookieForProtocolByName(aName, serverCookie);
	if(ret == KErrNone)
		{
		MCommsTransportSender* sender = &static_cast<MCommsTransportSender&>(TlsGlobals::Get().TransportSender());
		TEIntSockOpMsgWrapper opMsg (&aName);		
		TEIntSockMsgExt openMsg((TSockMess)KSoOpenByName, opMsg, iImplementation, aStatus, sender, serverCookie);
		CInternalSockSubSession::SendMsg(openMsg, sender, serverCookie);
		}
	else
		{
		TRequestStatus* pStatus = &aStatus;
		User::RequestComplete ( pStatus, ret );			
		}
	}

EXPORT_C void RInternalSocket::Open(TUint aAddrFamily,  TUint aSockType, TUint aProtocol, TRequestStatus& aStatus )
	{
	__ASSERT_DEBUG(!iImplementation, InternalSocketPanic(ENotNullSocket));
	LOG(Log::Printf(_L("RIS::Open(%d, %d, %d, %08x)"), aAddrFamily, aSockType, aProtocol, &aStatus));

	aStatus = KRequestPending;
		
	TNodeId serverCookie;
	TInt ret = CWorkerThread::CookieForProtocol(aAddrFamily, aSockType, aProtocol, serverCookie);
	if(ret == KErrNone)
		{
		MCommsTransportSender* sender = &static_cast<MCommsTransportSender&>(TlsGlobals::Get().TransportSender());
		TEIntSockOpMsgWrapper opMsg (aAddrFamily, aSockType, aProtocol);
		TEIntSockMsgExt openMsg(ESoCreate, opMsg, iImplementation, aStatus, sender, serverCookie);
		CInternalSockSubSession::SendMsg(openMsg, sender, serverCookie);
		}
	else
		{
		TRequestStatus* pStatus = &aStatus;
		User::RequestComplete ( pStatus, ret );			
		}		
	}

EXPORT_C void RInternalSocket::Send(const TDesC8& aDesc,TUint aFlags,TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Send(desc, flags=%d, status=%08x) impl=%08x"), aFlags, &aStatus, iImplementation));

	aStatus = KRequestPending;	
	TEIntSockOpMsgWrapper opMsg( &aFlags, 0, &aDesc, EFalse);	
	TEIntSockMsg msg(ESoSendNoLength, opMsg, iImplementation, aStatus);
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::Send(RMBufChain& aData,TUint aFlags,TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Send(mbuf, flags=%d, status=%08x) impl=%08x"), aFlags, &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(aFlags, 0, &aData, ETrue);
	TEIntSockMsg msg(ESoSendNoLength, opMsg, iImplementation, aStatus);
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::Send(const TDesC8& aDesc,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Send(desc+xferLen, flags=%d, status=%08x) impl=%08x"), aFlags, &aStatus, iImplementation));
	
	aStatus = KRequestPending;	
	aLen = aFlags;
	TEIntSockOpMsgWrapper opMsg(&aLen, 0, &aDesc, EFalse);	
	TEIntSockMsg msg(ESoSend, opMsg, iImplementation, aStatus);	
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::Send(RMBufChain& aData,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Send(mbuf+xferLen, flags=%d, status=%08x) impl=%08x"), aFlags, &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	aLen = aFlags;
	TEIntSockOpMsgWrapper opMsg(&aLen, 0, &aData, ETrue);	
	TEIntSockMsg msg(ESoSend, opMsg, iImplementation, aStatus);	
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::SendTo(const TDesC8& aDesc,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(aFlags, &aAddr, &aDesc, EFalse);	
	TEIntSockMsg msg(ESoSendToNoLength, opMsg, iImplementation, aStatus);
	iImplementation->SendMsg(msg);	
	}

EXPORT_C void RInternalSocket::SendTo(RMBufChain& aData,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(aFlags, &aAddr, &aData, ETrue);	
	TEIntSockMsg msg(ESoSendToNoLength, opMsg, iImplementation, aStatus);
	iImplementation->SendMsg(msg);			
	}

EXPORT_C void RInternalSocket::SendTo(const TDesC8& aDesc,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	
	aLen = aFlags;
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(&aLen, &aAddr, &aDesc, EFalse);	
	TEIntSockMsg msg(ESoSendTo, opMsg, iImplementation, aStatus);		
	iImplementation->SendMsg(msg);			
	}

EXPORT_C void RInternalSocket::SendTo(RMBufChain& aData, TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	
	aLen = aFlags;
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(&aLen, &aAddr, &aData, ETrue);	
	TEIntSockMsg msg(ESoSendTo, opMsg, iImplementation, aStatus);		
	iImplementation->SendMsg(msg);					
	}

EXPORT_C void RInternalSocket::CancelSend()
/**
@see RSocket documentation
*/
	{
	LOG(Log::Printf(_L("RIS::CancelSend() impl=%08x"), iImplementation));
	if(iImplementation)
		{
		TEIntSockOpMsgWrapper opMsg;
		TEIntSockMsg msg(ESoCancelSend, opMsg, iImplementation);
		iImplementation->SendReceiveMsg(msg);
		}
	}

EXPORT_C void RInternalSocket::Recv(TDes8& aDesc,TUint aFlags,TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Recv(desc, flags=%d, status=%08x) impl=%08x"), aFlags, &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(aFlags, 0, &aDesc, EFalse);
	TEIntSockMsg msg(ESoRecvNoLength, opMsg, iImplementation, aStatus);
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::Recv(RMBufChain& aData,TUint aFlags,TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Recv(mbuf, flags=%d, status=%08x) impl=%08x"), aFlags, &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(aFlags, 0, &aData, ETrue);
	TEIntSockMsg msg(ESoRecvNoLength, opMsg, iImplementation, aStatus);
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::Recv(TDes8& aDesc, TUint aFlags, TRequestStatus& aStatus, TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	
	aLen = aFlags;
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(&aLen, 0, &aDesc, EFalse);	
	TEIntSockMsg msg(ESoRecv, opMsg, iImplementation, aStatus);	
	iImplementation->SendMsg(msg);	
	}

EXPORT_C void RInternalSocket::Recv(RMBufChain& aData, TUint aFlags, TRequestStatus& aStatus, TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Recv(mbuf, flags=%d, status=%08x) impl=%08x"), aFlags, &aStatus, iImplementation));
	
	aLen = aFlags;		
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(&aLen, 0, &aData, ETrue);	
	TEIntSockMsg msg(ESoRecv, opMsg, iImplementation, aStatus);	
	iImplementation->SendMsg(msg);		
	}
	
/**
@see RSocket documentation.

Note: This API is not having a RMBufChain variant as a RIS::Recv with RMBufChain will give the exact behaviour 
*/
EXPORT_C void RInternalSocket::RecvOneOrMore(TDes8& aDesc, TUint aFlags, TRequestStatus& aStatus, TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::RecvOneOrMore(flags=%d, status=%08x) impl=%08x"), aFlags, &aStatus, iImplementation));
	
	aLen = aFlags;
	aStatus = KRequestPending;
	
	TEIntSockOpMsgWrapper opMsg(&aLen, 0, &aDesc, EFalse);
	TEIntSockMsg msg(ESoRecvOneOrMore, opMsg, iImplementation, aStatus );
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::CancelRecv()
/**
@see RSocket documentation
*/
	{
	LOG(Log::Printf(_L("RIS::CancelRecv() impl=%08x"), iImplementation));
	if(iImplementation)
		{
		TEIntSockOpMsgWrapper opMsg;
		TEIntSockMsg msg (ESoCancelRecv, opMsg, iImplementation);
		iImplementation->SendReceiveMsg(msg);
		}
	}

EXPORT_C void RInternalSocket::RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(aFlags, &aAddr, &aDesc, EFalse);	
	TEIntSockMsg msg(ESoRecvFromNoLength, opMsg, iImplementation, aStatus);
	iImplementation->SendMsg(msg);		
	}

EXPORT_C void RInternalSocket::RecvFrom(RMBufChain& aData,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(aFlags, &aAddr, &aData, ETrue);	
	TEIntSockMsg msg(ESoRecvFromNoLength, opMsg, iImplementation, aStatus);
	iImplementation->SendMsg(msg);		
	}

EXPORT_C void RInternalSocket::RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	
	aStatus = KRequestPending;
	aLen = aFlags;
	TEIntSockOpMsgWrapper opMsg(&aLen, &aAddr, &aDesc, EFalse);	
	TEIntSockMsg msg(ESoRecvFrom, opMsg, iImplementation, aStatus);	
	iImplementation->SendMsg(msg);				
	}

EXPORT_C void RInternalSocket::RecvFrom(RMBufChain& aData,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	
	aLen = aFlags;
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(&aLen, &aAddr, &aData, ETrue);	
	TEIntSockMsg msg(ESoRecvFrom, opMsg, iImplementation, aStatus);	
	iImplementation->SendMsg(msg);				
	}


EXPORT_C void RInternalSocket::Read(TDes8& aDesc,TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Read(desc, status=%08x) impl=%08x"), &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(0, 0, &aDesc, EFalse);	
	TEIntSockMsg msg(ESoRead, opMsg, iImplementation, aStatus);	
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::Read(RMBufChain& aData,TRequestStatus& aStatus)
/**
@see RSocket documentation
@note Pass in an empty (ie. no mbufs attached) RMBuf chain here - the mbufs will be attached by the protocol
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Read(mbuf, status=%08x) impl=%08x"), &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(0, 0, &aData, ETrue);	
	TEIntSockMsg msg(ESoRead, opMsg, iImplementation, aStatus);	
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::CancelRead()
/**
@see RSocket documentation
*/
	{
	LOG(Log::Printf(_L("RIS::CancelRead() impl=%08x"), iImplementation));
	if(iImplementation)
		{
		TEIntSockOpMsgWrapper opMsg;
		TEIntSockMsg msg (ESoCancelRecv, opMsg, iImplementation);
		iImplementation->SendReceiveMsg(msg);
		}
	}

EXPORT_C void RInternalSocket::Write(const TDesC8& aDesc, TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Write(desc, status=%08x) impl=%08x"), &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(0, 0, &aDesc, EFalse);		
	TEIntSockMsg msg (ESoWrite, opMsg, iImplementation, aStatus);	
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::Write(RMBufChain& aData, TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Write(mbuf, status=%08x) impl=%08x"), &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(0, 0, &aData, ETrue);
	TEIntSockMsg msg (ESoWrite, opMsg, iImplementation, aStatus);	
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::CancelWrite()
/**
@see RSocket documentation
*/
	{
	LOG(Log::Printf(_L("RIS::CancelWrite() impl=%08x"), iImplementation));
	if(iImplementation)
		{
		TEIntSockOpMsgWrapper opMsg;
		TEIntSockMsg msg (ESoCancelSend, opMsg, iImplementation);
		iImplementation->SendReceiveMsg(msg);
		}
	}

EXPORT_C void RInternalSocket::Connect(TSockAddr& aAddr,TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Connect(status=%08x) impl=%08x"), &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(&aAddr, NULL, NULL);
	TEIntSockMsg msg (ESoConnect, opMsg, iImplementation, aStatus);
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::Connect(TSockAddr& aAddr,const TDesC8& aConnectDataOut,TDes8& aConnectDataIn,TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Connect(withdata, status=%08x) impl=%08x"), &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(&aAddr, &aConnectDataOut, &aConnectDataIn);
	TEIntSockMsg msg (ESoConnect, opMsg, iImplementation, aStatus);
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::CancelConnect()
/**
@see RSocket documentation
*/
	{
	LOG(Log::Printf(_L("RIS::CancelConnect() impl=%08x"), iImplementation));
	if(iImplementation)
		{
		TEIntSockOpMsgWrapper opMsg;		
		TEIntSockMsg msg (ESoCancelConnect, opMsg, iImplementation);
		iImplementation->SendReceiveMsg(msg);
		}
	}

EXPORT_C TInt RInternalSocket::Bind(TSockAddr& aAddr)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Bind() impl=%08x"), iImplementation));
	TEIntSockOpMsgWrapper opMsg(&aAddr);	
	TEIntSockMsg msg (ESoBind, opMsg, iImplementation);
	return iImplementation->SendReceiveMsg(msg);
	}

EXPORT_C TInt RInternalSocket::SetLocalPort(TInt aPort)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::SetLocalPort(%d) impl=%08x"), aPort, iImplementation));
	TSockAddr addr;
	addr.SetPort ( aPort );
	return Bind ( addr );
	}

EXPORT_C void RInternalSocket::Accept(RInternalSocket& aBlankSocket,TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Accept(blankImpl=%08x, status=%08x) impl=%08x"), aBlankSocket.iImplementation, &aStatus, iImplementation));

	aStatus = KRequestPending;
	TInt ret = AdoptBlankSocketToCurrentThread (aBlankSocket );
	if(ret == KErrNone)
		{
		// Finally can process the accept
		TEIntSockOpMsgWrapper opMsg(NULL, &aBlankSocket, NULL);
		TEIntSockMsg msg (ESoAccept, opMsg, iImplementation, aStatus);
		iImplementation->SendMsg(msg);
		}
	else
		{
		// Complete the accept with the error that arose trying to do it
		LOG(Log::Printf(_L("RIS::Accept - error %d when substituting blanks"), ret));
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, ret);
		}
	}

EXPORT_C void RInternalSocket::Accept(RInternalSocket& aBlankSocket,TDes8& aConnectData,TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Accept(blankImpl=%08x, status=%08x) impl=%08x"), aBlankSocket.iImplementation, &aStatus, iImplementation));

	aStatus = KRequestPending;
	TInt ret = AdoptBlankSocketToCurrentThread (aBlankSocket );
	if(ret == KErrNone)
		{
		// Finally can process the accept
		TEIntSockOpMsgWrapper opMsg(NULL, &aBlankSocket, &aConnectData);		
		TEIntSockMsg msg (ESoAccept, opMsg, iImplementation, aStatus);
		iImplementation->SendMsg(msg);
		}
	else
		{
		// Complete the accept with the error that arose trying to do it
		LOG(Log::Printf(_L("RIS::Accept - error %d when substituting blanks"), ret));
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, ret);
		}		
	}

EXPORT_C void RInternalSocket::CancelAccept()
/**
@see RSocket documentation
*/
	{
	LOG(Log::Printf(_L("RIS::CancelAccept() impl=%08x"), iImplementation));
	if(iImplementation)
		{
		TEIntSockOpMsgWrapper opMsg;
		TEIntSockMsg msg (ESoCancelAccept, opMsg, iImplementation);
		iImplementation->SendReceiveMsg(msg);
		}
	}

EXPORT_C TInt RInternalSocket::Listen(TUint aQSize)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Listen(%d) impl=%08x"), aQSize, iImplementation));
	
	TEIntSockOpMsgWrapper opMsg(aQSize, NULL);
	TEIntSockMsg msg (ESoListen, opMsg, iImplementation);
	return iImplementation->SendReceiveMsg(msg);
	}

EXPORT_C TInt RInternalSocket::Listen(TUint aQSize,const TDesC8& aConnectData)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Listen(withData, %d) impl=%08x"), aQSize, iImplementation));
	
	TEIntSockOpMsgWrapper opMsg(aQSize, &aConnectData);
	TEIntSockMsg msg (ESoListen, opMsg, iImplementation);
	return iImplementation->SendReceiveMsg(msg);
	}

EXPORT_C TInt RInternalSocket::SetOpt(TUint aOptionName,TUint aOptionLevel,const TDesC8& aOption)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::SetOpt(%d, %d) impl=%08x"), aOptionName, aOptionLevel, iImplementation));
	
	TEIntSockOpMsgWrapper opMsg(aOptionName,&aOption,aOptionLevel);
	TEIntSockMsg msg (ESoSetOpt, opMsg, iImplementation);  	
	return iImplementation->SendReceiveMsg(msg);
	}

EXPORT_C TInt RInternalSocket::SetOpt(TUint aOptionName,TUint aOptionLevel,TInt aOption)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::SetOpt(%d, %d, %d) impl=%08x"), aOptionName, aOptionLevel, aOption, iImplementation));
	
	TPtr8 optionDes( (TUint8*)&aOption, sizeof(TInt), sizeof(TInt) );
	return SetOpt ( aOptionName, aOptionLevel, optionDes );
	}

EXPORT_C TInt RInternalSocket::GetOpt(TUint aOptionName,TUint aOptionLevel,TDes8& aOption)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::GetOpt(%d, %d) impl=%08x"), aOptionName, aOptionLevel, iImplementation));

	TEIntSockOpMsgWrapper opMsg(aOptionName,&aOption,aOptionLevel);
	TEIntSockMsg msg (ESoGetOpt, opMsg, iImplementation);  	
	return iImplementation->SendReceiveMsg(msg);
	}

EXPORT_C TInt RInternalSocket::GetOpt(TUint aOptionName,TUint aOptionLevel,TInt& aOption)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::GetOpt(%d, %d) impl=%08x"), aOptionName, aOptionLevel, iImplementation));
	
	TPtr8 optionDes( (TUint8*)&aOption, sizeof(TInt), sizeof(TInt) );
	return GetOpt ( aOptionName, aOptionLevel, optionDes );
	}

EXPORT_C void RInternalSocket::Ioctl(TUint aCommand,TRequestStatus& aStatus,TDes8* aDesc,TUint aLevel)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Ioctl(cmd=%d, level=%d, aStatus=%08x) impl=%08x"), aCommand, aLevel, &aStatus, iImplementation));

	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(aCommand,aDesc,aLevel);	
	TEIntSockMsg msg (ESoIoctl, opMsg, iImplementation, aStatus);  	
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::CancelIoctl()
/**
@see RSocket documentation
*/
	{
	LOG(Log::Printf(_L("RIS::CancelIoctl() impl=%08x"), iImplementation));
	if(iImplementation)
		{
		TEIntSockOpMsgWrapper opMsg;
		TEIntSockMsg msg (ESoCancelIoctl, opMsg, iImplementation);
		iImplementation->SendReceiveMsg(msg);
		}
	}

EXPORT_C TInt RInternalSocket::GetDisconnectData(TDes8& aDesc)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::GetDisconnectData() impl=%08x"), iImplementation));
	TEIntSockOpMsgWrapper opMsg(&aDesc);
	TEIntSockMsg msg (ESoGetDiscData, opMsg, iImplementation);	 
	return iImplementation->SendReceiveMsg(msg);
	}

EXPORT_C void RInternalSocket::LocalName(TSockAddr& aAddr)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::LocalName() impl=%08x"), iImplementation));

	TEIntSockOpMsgWrapper opMsg(&aAddr);
	TEIntSockMsg msg (ESoGetLocalName, opMsg, iImplementation);	 
	iImplementation->SendReceiveMsg(msg);
	}

EXPORT_C TUint RInternalSocket::LocalPort()
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::LocalPort() impl=%08x"), iImplementation));
	
	TSockAddr addr;
	LocalName ( addr );
	return addr.Port();
	}

EXPORT_C void RInternalSocket::RemoteName(TSockAddr& aAddr)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::RemoteName() impl=%08x"), iImplementation));
	
	TEIntSockOpMsgWrapper opMsg(&aAddr);
	TEIntSockMsg msg (ESoGetRemoteName, opMsg, iImplementation);	 
	iImplementation->SendReceiveMsg(msg);
	}

EXPORT_C void RInternalSocket::Close()
/**
@see RSocket documentation
*/
	{
	LOG(Log::Printf(_L("RIS::Close() impl=%08x"), iImplementation));
	if(iImplementation)
		{
		CInternalSocketImpl* impl = iImplementation;
		iImplementation = NULL; // this will leave the implementation "floating", but it's a CObject
								// and will delete itself once the protocol calls CanClose()
		TEIntSockOpMsgWrapper opMsg;
		TEIntSockMsg msg (ESoClose, opMsg, impl);	 
		impl->SendReceiveMsg(msg);
		}
	}

EXPORT_C void RInternalSocket::Shutdown(RSocket::TShutdown aHow,TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Shutdown(how=%d, status=%08x) impl=%08x"), aHow, &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(aHow);
	TEIntSockMsg msg (ESoShutdown, opMsg, iImplementation, aStatus);	 
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::Shutdown(RSocket::TShutdown aHow,const TDesC8& aDisconnectDataOut,TDes8& aDisconnectDataIn,TRequestStatus& aStatus)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Shutdown(withData, how=%d, status=%08x), impl=%08x"), aHow, &aStatus, iImplementation));
	
	aStatus = KRequestPending;
	TEIntSockOpMsgWrapper opMsg(aHow, &aDisconnectDataOut, &aDisconnectDataIn);	
	TEIntSockMsg msg (ESoShutdown, opMsg, iImplementation, aStatus);	 
	iImplementation->SendMsg(msg);
	}

EXPORT_C void RInternalSocket::CancelAll()
/**
@see RSocket documentation
*/
	{
	LOG(Log::Printf(_L("RIS::CancelAll() impl=%08x"), iImplementation));
	if(iImplementation)
		{
		TEIntSockOpMsgWrapper opMsg;
		TEIntSockMsg msg (ESoCancelAll, opMsg, iImplementation);	 
		iImplementation->SendReceiveMsg(msg);
		}
	}

EXPORT_C TInt RInternalSocket::Info(TProtocolDesc& aProtocol)
/**
@see RSocket documentation
*/
	{
	__ASSERT_ALWAYS(iImplementation, InternalSocketPanic(ENullSocket));
	LOG(Log::Printf(_L("RIS::Info() impl=%08x"), iImplementation));

	TPckg<TProtocolDesc> protocolDesc(aProtocol);
	TEIntSockOpMsgWrapper opMsg(&protocolDesc);
	TEIntSockMsg msg (ESoSocketInfo, opMsg, iImplementation);	 
	return iImplementation->SendReceiveMsg(msg);
	}

// don't support socket ::Name() with internal sockets	- IMPORT_C TInt Name(TName& aName);

EXPORT_C TInt RInternalSocket::Transfer(RInternalSocket& aSource)
/**
@see RSocket documentation
@note Rather than a two-stage ::Name(), ::Transfer(), this can be done in a single operation
*/
	{
	__ASSERT_ALWAYS(!iImplementation, InternalSocketPanic(ENullSocket)); // check that we're not about to overwrite another socket
	LOG(Log::Printf(_L("RIS::Transfer(srcImp=%08x) impl=%08x"), aSource.iImplementation, iImplementation));

	// Should we ASSERT no cross thread problem here?
	iImplementation = aSource.Implementation();
	aSource.SetImplementation(NULL);

    // Get a new transport sender incase the transfer was cross thread
	iImplementation->SetSender(&static_cast<MCommsTransportSender&>(TlsGlobals::Get().TransportSender()));
	return(KErrNone);
	}

// ---
TInt RInternalSocket::AdoptBlankSocketToCurrentThread ( RInternalSocket& aBlankSocket )
	{
	// Now that we know just which thread they are using the blank socket we can destroy the original
	// and create it where it should have been all along. Note that this code imposes the minor restriction
	// that the blank RInternalSocket must not have been aliased, because we're destroying the original
	// implementation and setting aBlankSocket.iImpl to the new one. This limitation compared to RSocket
	// (where the IPC handle mechanism hides the pointer) seems acceptable - it's small and the audience
	// must be knowledgeable and careful anyway. Unfortunately we can't ASSERT that they haven't aliased - 
	// the best we could do is log whenever they do, but the first time we see it could be after they've
	// aliased the blank, ie it wouldn't necessarily catch this case. The alternative to avoid changing the
	// implementation pointer would be providing a forwarding mechanism: non-trivial

	TInt ret = KErrNone;
	if(iImplementation->ServerWorkerId() != aBlankSocket.iImplementation->ServerWorkerId())
		{
		// Blank was created in wrong thread. Perhaps v1.1 of the Precognition module will work better!
		LOG(Log::Printf(_L("RIS::Accept - creating new blank in listener's thread")));
		TNodeId newBlankCookie;
		ret = CWorkerThread::CookieForWorkerId(iImplementation->ServerWorkerId(), newBlankCookie);
		if(ret == KErrNone)
			{
			CInternalSocketImpl* oldBlankImplementation = aBlankSocket.iImplementation;
			aBlankSocket.iImplementation = NULL;
			TEIntSockOpMsgWrapper opMsg;
			TEIntSockMsgExt msg(ESoCreateNull, opMsg, aBlankSocket.iImplementation, iImplementation->Sender(), newBlankCookie);
			ret = CInternalSockSubSession::SendReceiveMsg(msg, iImplementation->Sender(), newBlankCookie);
			if(ret == KErrNone)
				{
				// Tell the original blank socket owner to delete the implementation
				LOG(Log::Printf(_L("RIS::Accept - closing original blank")));
				RInternalSocket oldBlankSocket;
				oldBlankSocket.iImplementation = oldBlankImplementation;
				oldBlankSocket.Close();
				}
			else
				{
				aBlankSocket.iImplementation = oldBlankImplementation;	// rollback
				}
			}
		}
	return ret;		
	}


