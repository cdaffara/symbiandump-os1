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

/**
 @file SS_SAPSHIM.CPP
*/

#define SYMBIAN_NETWORKING_UPS
#include "ss_sapshim.h"

#include <ss_std.h>
#include <ss_glob.h>
#include <comms-infras/ss_roles.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_connprov.h>
#include <ss_protprov.h>
#include <ss_sock.h>
#include <comms-infras/ss_protflow.h>
#include <in_sock.h> // KAfInet
#include <elements/nm_messages_child.h>
#include <elements/nm_messages_peer.h>

#include <comms-infras/nifif.h>
#include <comms-infras/nifif_internal.h>

#include <addressupdate.h>

#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_flow.h>
#include <comms-infras/ss_nodemessages_ipmessages.h>
#include <comms-infras/ss_nodemessages_internal_esock.h>

#include "ss_protopt.h"

#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/ss_upsaccesspointconfigext.h>
#include <comms-infras/upsmessages.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksspshm, "ESockSSocksspshm");
#endif

_LIT_SECURITY_POLICY_C1(KPolicyNetworkServices, ECapabilityNetworkServices);
#endif

using namespace ESock;
using namespace Messages;
using namespace MeshMachine;
using namespace Factories;

//CHostResolver specific functions this is to remove once we have app layer protocols
//***************************************************************************************
//also loo for iHostResolverNotify
MLowerControl* CTransportFlowShim::GetControlL(const TDesC8& /*aProtocol*/)
    {
    return this;
    }

TInt CTransportFlowShim::Control(TUint aLevel, TUint aName, TDes8& aOption)
    {
    return iLowerControl ? iLowerControl->Control(aLevel, aName, aOption) : KErrNotReady;
    }

MLowerDataSender* CTransportFlowShim::BindL(const TDesC8& /*aProtocol*/, MUpperDataReceiver* aReceiver, MUpperControl* aControl)
    {
    if (aReceiver || iProvider || IsBoundToSession())
        {
        User::Leave(KErrInUse);
        }
    iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TActive().CRef());
    iHostResolverNotify = aControl;
    return this;
    }

void CTransportFlowShim::Unbind( MUpperDataReceiver* /*aReceiver*/, MUpperControl* aControl)
    {
    (void)aControl;
    __ASSERT_DEBUG(aControl == iHostResolverNotify, User::Panic(KSpecAssert_ESockSSocksspshm, 1));
    __ASSERT_DEBUG(!(Idle() || IdleSent()), User::Panic(KSpecAssert_ESockSSocksspshm, 2));
    SetIdle();
	
    iHostResolverNotify = NULL;
    if(iSubConnectionProvider.IsOpen())	// legacy flows have no control side
    	{
    	ProcessDCIdleState();
    	}
    }

MLowerDataSender::TSendResult CTransportFlowShim::Send(RMBufChain& /*aData*/)
    {
    __ASSERT_DEBUG(iHostResolverNotify, User::Panic(KSpecAssert_ESockSSocksspshm, 3));

    TBuf8<sizeof(TConnectionInfo)> buf;
    if (Control(KSOLProvider, static_cast<TUint>(KSoConnectionInfo), buf) == KErrNone)
        {
        iHostResolverNotify->StartSending();
        }
    else if (!iLowerControl)
        {
#ifdef SYMBIAN_NETWORKING_UPS
		TInt result = SetupForNoBearerOnSend();
		if (result != KErrNone)
			{
			return TSendResult(result);
			}
#endif
		__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockSSocksspshm, 4));	// legacy flows have no control side, so must never get here
        PostNoBearer();
        }

    return MLowerDataSender::ESendAccepted;
    }
//***************************************************************************************
//End of CHostResolver specific functions also look for iHostResolverNotify to find the rest

#ifndef SYMBIAN_NETWORKING_UPS

//
// Not used with UPS support - CUpsTransportFlowShim::NewL() used instead.
//

CTransportFlowShim* CTransportFlowShim::NewL(CSubConnectionFlowFactoryBase& aFactory, const TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf)
	{
	return new (ELeave) CTransportFlowShim(aFactory, aSubConn, aProtocolIntf);
	}

#endif

CTransportFlowShim::CTransportFlowShim(CSubConnectionFlowFactoryBase& aFactory, const TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf)
: CNetworkFlow(aFactory, aSubConn, aProtocolIntf), iFlowParams(TFlowParams(0, 0, 0, 0, NULL))
	{
    LOG_NODE_CREATE(KESockFlowTag, CTransportFlowShim)
	}


void CTransportFlowShim::SetSSP(CServProviderBase& aSSP)
	{
	__ASSERT_DEBUG(iProvider == NULL, User::Panic(KSpecAssert_ESockSSocksspshm, 5));
	iProvider = &aSSP;
	iProvider->SetNotify(this);
	}

CTransportFlowShim::~CTransportFlowShim()
	{
	// Remove ourselves from any listening control session's accept queue.
	if(iListenerControlNotify)
		{
		iListenerControlNotify->DisconnectFromListener(*this);
		}

	if(!Detaching())
		{
		delete iProvider;
		iProvider = NULL;
		}

	Messages::TEBase::TError err(TEBase::TNull::Id(), KErrBindersInvalid);
    SubConnectionError(err, EErrorAllOperations);

    LOG_NODE_DESTROY(KESockFlowTag, CTransportFlowShim)
	}

void CTransportFlowShim::SetSockType(TUint aSockType)
	{
	if (iProvider)
		{
		iProvider->SetSockType(aSockType);
		}
	}

MSessionControl* CTransportFlowShim::GetControlL(TInt aSessionType,MSessionControlNotify& aSessionControlNotify)
    {
    CNetworkFlow::GetControlL(aSessionType,aSessionControlNotify);
    SetSockType(aSessionType);
	return this;
    }

MSessionData* CTransportFlowShim::BindL(MSessionDataNotify& aNotify)
    {
	LOG( ESockLog::Printf(_L8("CTransportFlowShim %08x:\tBindL(aNotify 0x%08x)"), this, &aNotify) );

	// The flow shim now manages custody and loading of the protocol involved.
	// However in the case of host resolvers, until ported for the same behaviour, protocol loading is still managed by the socket.
	if(iFlowParams.iFlowLoadsProtocol)
		{
		CProtocolRef * protocolReference = ProtocolManager::FindProtocolL(
			iFlowParams.iAddrFamily,
			iFlowParams.iSocketType,
			iFlowParams.iProtocol);

        if (protocolReference->GetFlag() & CProtocolRef::EThreePlaneCommsBased)
            {
            // Three plane comms protos should not be seen here. If an RSocket is opened with a supported
            // protocol and it happens to be 3PC this point will be reached. In the case of RTP some
            // special processing is performed (in the SCPR) in order to sort out the mess and ensure that
            // a UDP/TCP flow is requested instead.
            User::Leave(KErrProtocolNotReady);
            }

        // Check if the protocol is supported over sockets
        if (!(protocolReference->Info().iServiceTypeInfo & ESocketSupport))
            {
            User::Leave(KErrNotSupported);
            }

        // Have the protocol loaded if not already the case and confirm it has socket support
        protocolReference->LoadAndBindL();

        __ASSERT_DEBUG(iProtocolReference == NULL, User::Panic(KSpecAssert_ESockSSocksspshm, 6));
        iProtocolReference = protocolReference;

        // If the client requested a wildcard protocol, fill in the actual protocol that was selected
        if (iFlowParams.iProtocol == KUndefinedProtocol)
            {
            iFlowParams.iProtocol = protocolReference->Info().iProtocol;
            }
        
        // Have the protocol added to the list of protocols for the session it is associated with
        CSockSessionProxy* sessionProxy = reinterpret_cast<CSockSessionProxy*>(iFlowParams.iSessionProxy);
        if(sessionProxy != NULL)
            {
            sessionProxy->AddProtocolL(iProtocolReference->Protocol());
            }

        // Fetch a new SAP from the protocol for this flow
        if (iProvider == NULL)
            {
            iProvider = iProtocolReference->Protocol()->NewSAPL(iFlowParams.iSocketType);

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
            // Set the options iterating the RArray variable
            SetProtocolOptions();
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    
            if (iProvider)
                {
                iProvider->SetNotify(this);
                }
            }
		}
    
    // Now perform the binding proper
    if (!iProvider)
        {
        User::Leave(KErrNotSupported);
        }
        
    __ASSERT_DEBUG(!iHostResolverNotify, User::Panic(KSpecAssert_ESockSSocksspshm, 8));
    CNetworkFlow::BindL(aNotify);
    iProvider->SetNotify(this);

	return this;
    }

void CTransportFlowShim::HostResolverSpecificUnbind()
	{
	// Can't have both HR & SAP
	__ASSERT_DEBUG(!iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 9));

	if(iSubConnectionProvider.IsOpen())
		{
		iSessionControlNotify = NULL;
		iSessionDataNotify = NULL;
		iHostResolverNotify = NULL;
		if (!Idle())
			{
			SetIdle();
			ProcessDCIdleState();
			}
		}
	else
		{
		DeleteThisFlow();
		}
	}

void CTransportFlowShim::Unbind()
    {
	LOG( ESockLog::Printf(_L8("CTransportFlowShim %08x:\tUnbind()"), this) );

	// Infant mortality case in which binding never completed,
	if (!FlowParamsInitialised())
		{
		delete this;
		return;
		}

	// Legacy support for host resolvers involves a separate north bound MUpperControl interface
	if(iHostResolverNotify)
		{
		HostResolverSpecificUnbind();
		}
	else
		{
		if (iProvider)
			{
			iProvider->SetNotify(NULL);

			if (!Detaching())
				{
				delete iProvider;
				iProvider = NULL;
				}
			}

#ifdef SYMBIAN_NETWORKING_UPS
		// Hook for derived classes to do cleanup before unbind occurs
		PreUnbind();
#endif
		CNetworkFlow::Unbind();
		}
    }

ESock::CSubConnectionFlowBase& CTransportFlowShim::CloneFlowL()
    {
    __ASSERT_DEBUG(iProtocolIntf, User::Panic(KSpecAssert_ESockSSocksspshm, 10));
    TDefaultFlowFactoryQuery query (iProtocolIntf->ControlProviderId(), iSubConnectionProvider.RecipientId());
	ESock::CSubConnectionFlowBase& flow = static_cast<ESock::CSubConnectionFlowBase&>(*(Factory().CreateObjectL(query)));
	// Legacy flows lack a control provider
	if(iSubConnectionProvider.IsOpen())
		{
		TCFPeer::TJoinRequest msg(flow.Id(), TClientType(TCFClientType::EData, TCFClientType::EActive));
		iSubConnectionProvider.PostMessage(Id(), msg);
		}
	return flow;
    }

void CTransportFlowShim::Start()
	/**
	Start a service provider.
	*/
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 11));
	iProvider->Start();
	}

void CTransportFlowShim::LocalName(TSockAddr& anAddr) const
/** Gets the local name (address) of the socket service provider entity. The format
of the data in the TSockAddr object is defined by individual protocols.

The local address is the address of the local machine plus a local port number.
Generally only the port number is important, unless you have two IP interfaces,
for example.

@param anAddr The address to be filled in */
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 12));
	iProvider->LocalName(anAddr);
	}

void CTransportFlowShim::SetLocalName(TSockAddr& anAddr)
/** Sets the local name (address) of the socket service provider entity. The format
of the data in the TSockAddr object is defined by individual protocols.

@param anAddr The address */
	{
	CNetworkFlow::SetLocalName(anAddr);
	PostDataClientRouted();
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 65));
	TInt err = iProvider->SetLocalName(anAddr);
	if (err != KErrNone)
		{
		// Post TError to self, which will result in SubConnectionError to
		// be called in the TError handling. Have to fake that the message is
		// coming from iSubConnectionProvider otherwise ReceivedL will leave.
		RClientInterface::OpenPostMessageClose(iSubConnectionProvider.RecipientId(), Id(), TEBase::TError(err).CRef());
		return;
		}

	SetLocalNameProcess();
	}

void CTransportFlowShim::SetLocalNameProcess()
	{
	TBool bearerSent = EFalse;
	if(iFlowParams.iFlowRequestType != TFlowParams::EImplicit)
		{
		// For explicit sockets, TNoBearer will be sent out if it has not already
		// been done so. This was introduced for PREQ2279, as it is necessary
		// to lock this SAP to an interface for incoming sockets. Once locked,
		// this ensures that any incoming packets are only accepted over the locked down
		// interface and no other interfaces. To lock the interface down the
		// NoBearer sequence is executed. This change will also now apply to
		// outgoing explicit sockets. The change for outgoing explicit sockets is
		// that the NoBearer sequence is started here instead of when receiving
		// a NoBearer upcall from the protocol.

		// Attempt to send out TNoBearer.
		if (DoNoBearer())
		    {
		    // TNoBearer was sent out.
			bearerSent = ETrue;
		    }
		}

	if (!bearerSent)
		{
		// No Bearer was already sent out earlier, or this is an implicit socket.
		iSessionControlNotify->SetLocalNameComplete();
		}
	}


TBool CTransportFlowShim::DoNoBearer()
/**
Do the actual no Bearer call.
@return ETrue if the NoBearer post was actually done, EFalse otherwise
*/
	{
	TInt ret = EFalse;
	if(!Stopped())
		{ // Prevent sending NoBearer if DataClientStop was received
		if (LockToConnectionInfo() != KErrNone)
		    {
	        __ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockSSocksspshm, 66));
			PostNoBearer();
			ret = ETrue;
		    }
		ClearUseBearerErrors();

		ClearDataClientRoutedGuard();
		}
	return ret;
	}



void CTransportFlowShim::RemName(TSockAddr& anAddr) const
/** Gets the remote name (address) of the socket service provider entity. The format
of the data in the TSockAddr object is defined by individual protocols.

A remote address is either the address you're sending data to (non connection-oriented
sockets)* or the remote end of the connection. It is the address of the remote
machine (your peer in the network) plus a port number.

@note RemName is only meaningful if the socket server client has called Connect()
to set up a default address for SendTo(). This function will only be called
on the protocol if this is the case.

@param anAddr The address to be filled in */
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 14));
	iProvider->RemName(anAddr);
	}

TInt CTransportFlowShim::SetRemName(TSockAddr& anAddr)
/** Sets the remote name (address) of the socket service provider entity. The format
of the data in the TSockAddr object is defined by individual protocols.

@param anAddr The address
@return Returns KErrNone if the remote name is correctly set or, if this is
not the case, an informative error number. */
	{
	CNetworkFlow::SetRemName(anAddr);
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 15));
	return iProvider->SetRemName(anAddr);
	}

TInt CTransportFlowShim::GetOption(TUint aLevel, TUint aName, TDes8& anOption) const
/** Gets some protocol specific option when called by the socket server on behalf of a
client. A protocol may pass the request down a protocol	stack (to protocols it is bound
to) using the GetOption() function of CProtocolBase.

@param aLevel Option level.
@param aName Option name.
@param anOption Option data.
@return System wide error code.
*/
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 16));
	return iProvider->GetOption(aLevel, aName, anOption);
	}

void CTransportFlowShim::Ioctl(TUint aLevel, TUint aName, TDes8* anOption)
/** Performs some protocol specific IO control.

@note If this function is called erroneously, the protocol should call Error() on the
socket. If an Ioctl call is already outstanding, the client will be panicked with the
value ETwoIoctls.

@param aLevel Option level.
@param aName Option name.
@param anOption Option data.
*/
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 17));
	iProvider->Ioctl(aLevel, aName, anOption);
	}

void CTransportFlowShim::CancelIoctl(TUint aLevel, TUint aName)
/** Cancels an outstanding Ioctl call. You are guaranteed only to have one outstanding
at once.

@param aLevel IOCTL level.
@param aName IOCTL name.
*/
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 18));
	iProvider->CancelIoctl(aLevel, aName);
	}

TInt CTransportFlowShim::SetOption(TUint aLevel, TUint aName, const TDesC8 &anOption)
/** Sets some protocol specific option when called by the socket server on behalf of a
client. A protocol may pass the request down a protocol	stack (to protocols it is bound
to) using the SetOption() function	of CProtocolBase.

@param aLevel Option level.
@param aName Option name.
@param anOption Option data.
@return System wide error code.
*/
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 19));
	return iProvider->SetOption(aLevel, aName, anOption);
	}

void CTransportFlowShim::ActiveOpen()
/** Initiates a connection operation - this means that it tells the protocol to
attempt to connect to a peer. It is called by the socket server in response
to a connect request from a client.

This version of the function has user data in the connection frame.

Only ever called on connection-oriented sockets. Such a socket
should always have both the local address and the remote address specified
before this function is called. If this is not the case then the protocol
should panic.

When a connection has completed, the protocol should call ConnectComplete()
on its TNotify. If an error occurs during connection the protocol should not
call ConnectComplete() at all; instead it should call Error().
*/
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 20));
	iProvider->ActiveOpen();
	}

void CTransportFlowShim::ActiveOpen(const TDesC8& aConnectionData)
/** Initiates a connection operation - this means that it tells the protocol to
attempt to connect to a peer. It is called by the socket server in response
to a connect request from a client.

This version of the function has user data in the connection frame.

Only ever called on connection-oriented sockets. Such a socket
should always have both the local address and the remote address specified
before this function is called. If this is not the case then the protocol
should panic.

When a connection has completed, the protocol should call ConnectComplete()
on its TNotify. If an error occurs during connection the protocol should not
call ConnectComplete() at all; instead it should call Error().

@param aConnectionData If the protocol supports user specified connection
data, then it will be held in this buffer. */
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 21));
	iProvider->ActiveOpen(aConnectionData);
	}

TInt CTransportFlowShim::PassiveOpen(TUint aQueueSize)
/** Tells the protocol to start waiting for an incoming connection request on this
socket (i.e. port). It is called by the socket server in response to a listen
request from a client.

Only ever called on connection-oriented sockets. Such a socket
should always have both the local address and the remote address specified
before this function is called. If this is not the case, then the protocol
should panic.

The aQue parameter is the number of sockets which can be waiting for an outstanding
Start after calling ConnectComplete(). The protocol should keep a count of
sockets in this state - incrementing a variable in ConnectComplete(), and
decrementing it in Start().

When a connection has completed, the protocol should call ConnectComplete()
on its TNotify. If an error occurs during connection the protocol should not
call ConnectComplete() at all; instead it should call Error().

@param aQueSize Size of connect queue.
*/
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 22));
	return iProvider->PassiveOpen(aQueueSize);
	}

TInt CTransportFlowShim::PassiveOpen(TUint aQueueSize, const TDesC8& aConnectionData)
/** Tells the protocol to start waiting for an incoming connection request on this
socket (i.e. port). It is called by the socket server in response to a listen
request from a client.

This version of the function has user data in the connection frame.

Only ever called on connection-oriented sockets. Such a socket
should always have both the local address and the remote address specified
before this function is called. If this is not the case then the protocol
should panic.

The aQue parameter is the number of sockets which can be waiting for an outstanding
Start after calling ConnectComplete(). The protocol should keep a count of
sockets in this state - incrementing a variable in ConnectComplete(), and
decrementing it in Start().

When a connection has completed the protocol should call ConnectComplete()
on its TNotify. If an error occurs during connection the protocol should not
call ConnectComplete() at all; instead it should call Error().

@param aQueueSize size of connect queue
@param aConnectionData if the protocol supports user specified connection data
then it will be held in this buffer. */
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 23));
	return iProvider->PassiveOpen(aQueueSize, aConnectionData);
	}

void CTransportFlowShim::Shutdown(MSessionControl::TCloseType aOption)
/** Terminates a connection (or closes a non connection-oriented socket down).

The value of the option argument specifies the type of processing which will
be required of the protocol after this function is called.

Normally, when the socket server has called Shutdown() for a socket, it will
wait for the socket to call CanClose() before destroying the CServProviderBase
object. However, if the option argument is EImmediate, the CServProviderBase
will be destroyed as soon as Shutdown() returns.

@param option The shutdown type. */
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 24));
	if (aOption == MSessionControl::EImmediate)
		{
		SetShuttingDown();
		}

	// It is possible for the provider to be null if an error occurs immediatly
	// after socket creation before the flow is bound and the provider is received
	// from the protocol.
	if(iProvider)
		{
		iProvider->Shutdown(CServProviderBase::TCloseType(aOption));
		}
	}

void CTransportFlowShim::Shutdown(MSessionControl::TCloseType aOption, const TDesC8& aDisconnectionData)
/** Terminates a connection (or closes a non connection-oriented socket down).

The value of the option argument specifies the type of processing which will
be required of the protocol after this function is called.

Normally, when the socket server has called Shutdown() for a socket, it will
wait for the socket to call CanClose() before destroying the CServProviderBase
object. However, if the option argument is EImmediate, the CServProviderBase
will be destroyed as soon as Shutdown() returns.

@param option The shutdown type.
@param aDisconnectionData If the protocol supports disconnect data, any such
data required will be held in this buffer. */
	{
	// It is possible for the provider to be null if an error occurs immediatly
	// after socket creation before the flow is bound and the provider is received
	// from the protocol.
	if(iProvider)
		{
		iProvider->Shutdown(CServProviderBase::TCloseType(aOption), aDisconnectionData);
		}
	}

void CTransportFlowShim::AutoBind()
/** Specifies that the protocol should choose a local address for the service access
point itself. */
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 25));
	iProvider->AutoBind();
	SetLocalNameProcess();

	}

TUint CTransportFlowShim::Write(const TDesC8& aDesc, TUint aOptions, TSockAddr* anAddr)
/** Sends data onto the network via the protocol.

Connection-oriented sockets must be in a connected state (that is ConnectComplete() has
been called on their MSocketNotify before Write() is called).

The socket server keeps track of how much data is waiting and then tries to send it all
until the protocol tells it to hold off by returning 0 (datagram sockets) or 'less than
all data consumed' (stream sockets) to Write(). The protocol should call CanSend() when it
is ready to send more data.

anAddr is the address to write the data to.	Connection oriented sockets always use the
default value.

@param aDesc The data to be sent.
@param aOptions Protocol specific options.
@param anAddr Address to write the data to.

@returns For stream-oriented protocols the return value is the number of bytes actually written.
If this is less than the length of the descriptor then the protocol should call CanSend()
when it is ready to send more data. For datagram-oriented protocols, the write should return
either 0 if the write cannot be completed, or the length of the descriptor if the write succeeds -
no other values are valid. If the Write() must return 0, then it should call CanSend() when it is
ready to send more data. If the Write() fails due to some error, then it should call Error() with
an informative error number.
*/
	{
	CNetworkFlow::Write( aDesc, aOptions, anAddr);
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 26));
	return iProvider->Write(aDesc, aOptions, anAddr);
	}

TInt CTransportFlowShim::Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr)
/** Sends data onto the network via the protocol.

Connection-oriented sockets must be in a connected state (that is ConnectComplete() has
been called on their MSocketNotify before Write() is called).

The socket server keeps track of how much data is waiting and then tries to send it all
until the protocol tells it to hold off by returning 0 (datagram sockets) or 'less than
all data consumed' (stream sockets) to Write(). The protocol should call CanSend() when it
is ready to send more data.

anAddr is the address to write the data to.	Connection oriented sockets always use the
default value.

@param aData The data to be sent.
@param aOptions Protocol specific options.
@param anAddr Address to write the data to.

@returns For stream-oriented protocols the return value is the number of bytes actually written.
If this is less than the length of the descriptor then the protocol should call CanSend()
when it is ready to send more data. For datagram-oriented protocols, the write should return
either 0 if the write cannot be completed, or the length of the descriptor if the write succeeds -
no other values are valid. If the Write() must return 0, then it should call CanSend() when it is
ready to send more data. If the Write() fails due to some error, then it should call Error() with
an informative error number.
*/
	{
	CNetworkFlow::Write(aData, aOptions, anAddr);
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 27));
	return iProvider->Write(aData, aOptions, anAddr);
	}

void CTransportFlowShim::GetData(TDes8& aDesc, TUint aOptions, TSockAddr* anAddr)
/** Gets data which the protocol has indicated is waiting in its buffers using the NewData
up-call on the MSocketNotify.

GetData() will only ever be called for as much data as the protocol has specified it can process
using the NewData up-call.

For stream oriented protocols GetData() should fill the descriptor with data from the stream. On
a datagram protocol GetData() should copy one datagram into the descriptor and set the length of
the descriptor. If a full datagram will not fit into the supplied descriptor, the overflow should
be discarded.

anAddr should be filled in by the protocol with the address of where the data came from.

@param aDesc  The buffer for data.
@param aOptions Protocol specific options.
@param anAddr Address where the data came from.
*/
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 28));
	iProvider->GetData(aDesc, aOptions, anAddr);
	}

TInt CTransportFlowShim::GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr)
/** Gets data which the protocol has indicated is waiting in its buffers using the NewData
up-call on the MSocketNotify.

GetData() will only ever be called for as much data as the protocol has specified it can process
using the NewData up-call.

For stream oriented protocols GetData() should fill the descriptor with data from the stream. On
a datagram protocol GetData() should copy one datagram into the descriptor and set the length of
the descriptor. If a full datagram will not fit into the supplied descriptor, the overflow should
be discarded.

anAddr should be filled in by the protocol with the address of where the data came from.

@param aDesc  The buffer for data.
@param aOptions Protocol specific options.
@param anAddr Address where the data came from.
*/
	{
	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 29));
	return iProvider->GetData(aData, aLength, aOptions, anAddr);
	}

TInt CTransportFlowShim::SecurityCheck(MProvdSecurityChecker* aChecker)
/**
Ask the SAP provider to perform a security policy check on the client process (default implementation).
*/
	{
	// ********************************************************************
	// NOTE: KErrNone for now, but this should be changed to KErrNotSupported to
	// turn on strict checking and fail PRTs that have not been secured.
	// ********************************************************************

	__ASSERT_DEBUG(iProvider, User::Panic(KSpecAssert_ESockSSocksspshm, 30));
	return iProvider->SecurityCheck(aChecker);
	}

void CTransportFlowShim::NewData(TUint aCount)
	{
	__ASSERT_DEBUG(iSessionDataNotify, User::Panic(KSpecAssert_ESockSSocksspshm, 31));
	iSessionDataNotify->NewData(aCount);
	}

void CTransportFlowShim::CanSend()
	{
	__ASSERT_DEBUG(iSessionDataNotify, User::Panic(KSpecAssert_ESockSSocksspshm, 32));
	iSessionDataNotify->CanSend();
	}

void CTransportFlowShim::ConnectComplete()
	{
	__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockSSocksspshm, 33));
	iSessionControlNotify->ConnectComplete();
	}

void CTransportFlowShim::ConnectComplete(const TDesC8& aConnectData)
	{
	__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockSSocksspshm, 34));
	iSessionControlNotify->ConnectComplete(aConnectData);
	}

CTransportFlowShim* CTransportFlowShim::CloneNSetSAP(CServProviderBase& aSSP)
    {
	//Here we can afford to create a flow directly since we know sub-connection
	//+ protocols & flow plug-is've been already loaded while opening listenning socket
	//this is the only case when flow joins without being created from control side
	CTransportFlowShim* shimFlow = NULL;
	TRAPD(ret,shimFlow = factoryobject_cast<CTransportFlowShim>(&CloneFlowL()));
	if (ret != KErrNone)
		{
		Error(ret,MSessionControlNotify::EErrorCompleteConnect);
		}
	else
	    {
        __ASSERT_DEBUG(shimFlow, User::Panic(KSpecAssert_ESockSSocksspshm, 35)); //otherwise we shouldn't be here
       	shimFlow->SetSSP(aSSP);
       	shimFlow->SetFlowParams(iFlowParams);
	    }
	return shimFlow;
    }

void CTransportFlowShim::ConnectComplete(CServProviderBase& aSSP)
	{
	CTransportFlowShim* shimFlow;
	if ((shimFlow = CloneNSetSAP(aSSP)) != NULL)
		{
		__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockSSocksspshm, 36));

		// Store a pointer to the listening control session in case we need to unbind before
		// the passively opened socket is accepted.
		shimFlow->iListenerControlNotify = iSessionControlNotify;

		iSessionControlNotify->ConnectComplete(*shimFlow);
		}
	}

void CTransportFlowShim::ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData)
	{
//	CTransportFlowShim* shim = NULL;
//	TRAPD(ret, shim = CTransportFlowShim::NewL(aSSP, NULL, iProtocolId));

	CTransportFlowShim* shimFlow;
    if ((shimFlow = CloneNSetSAP(aSSP)) != NULL)
		{
		__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockSSocksspshm, 37));
		iSessionControlNotify->ConnectComplete(*shimFlow, aConnectData);
		}
	}

void CTransportFlowShim::CanClose(MSocketNotify::TDelete aDelete)
	{
	LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tCanClose() aDelete %d"), this, aDelete) );

	if(iSessionControlNotify)
		{
		aDelete == MSocketNotify::EDetach ? SetDetaching() : ClearDetaching();
		
		iSessionControlNotify->CanClose(MSessionControlNotify::TDelete(aDelete));
        if(aDelete==MSocketNotify::EDetach)
            {
            iProvider = NULL;
            }
		}
	else
		{
		LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tCanClose() no control above us to notify (open was likely passive and has not been accepted yet) so simply unbinding"), this) );

		// No control above us - likely cause is that we're the result of a passive open that
		// hasn't yet been accepted.
		Unbind();
		}
	}

void CTransportFlowShim::CanClose(const TDesC8& aDisconnectData,MSocketNotify::TDelete aDelete)
	{
	LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tCanClose() aDisconnectData %08x, aDelete %d"), this, aDisconnectData.Ptr(), aDelete) );

	if(iSessionControlNotify)
		{
		aDelete == MSocketNotify::EDetach ? SetDetaching() : ClearDetaching();
		iSessionControlNotify->CanClose(aDisconnectData, MSessionControlNotify::TDelete(aDelete));
		if(aDelete==MSocketNotify::EDetach)
			{
			iProvider = NULL;
			}
		}
	else
		{
		LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tCanClose() no control above us to notify (open was likely passive and has not been accepted yet) so simply unbinding"), this) );

		// No control above us - likely cause is that we're the result of a passive open that
		// hasn't yet been accepted.
		Unbind();
		}
	}

void CTransportFlowShim::Error(TInt anError, TUint anOperationMask)
	{
	LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tError() anError %d, anOperationMask %d"), this, anError, anOperationMask) );

	NM_LOG((KESockServerTag, _L8("CTransportFlowShim %08x:\tSynchronous call: From=%08x To=%08x Func=Error(%d)"),
			this, static_cast<Messages::ANode*>(this), static_cast<Messages::ANode*>(this), anError) )

	if(iSessionControlNotify)
		{
		if (iSessionControlNotify->Error(anError, anOperationMask) != KErrNone)
	    	{
	    	if (iLowerFlow)
	    		{
	    		iLowerFlow->Unbind(NULL,NULL);
	    		iLowerFlow = NULL;
	    		}
	        iLowerControl = NULL;
	    	}
		}
	else
		{
		LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tError() no control above us to notify (open was likely passive and has not been accepted yet) so simply unbinding"), this) );

		// No control above us - likely cause is that we're the result of a passive open that
		// hasn't yet been accepted.
		SetDetaching();
		Unbind();
		}
	}

void CTransportFlowShim::Disconnect(void)
	{
	LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tDisconnect()"), this) );

	if(iSessionControlNotify)
		{
		iSessionControlNotify->Disconnect();
		}
	else
		{
		LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tDisconnect() no control above us to notify (open was likely passive and has not been accepted yet) so simply unbinding"), this) );

		// No control above us - likely cause is that we're the result of a passive open that
		// hasn't yet been accepted.
		SetDetaching();
		Unbind();
		}
	}

void CTransportFlowShim::Disconnect(TDesC8& aDisconnectData)
	{
	LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tDisconnect() aDisconnectData %08x"), this, aDisconnectData.Ptr()) );

	if(iSessionControlNotify)
		{
		iSessionControlNotify->Disconnect(aDisconnectData);
		}
	else
		{
		LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tDisconnect() no control above us to notify (open was likely passive and has not been accepted yet) so simply unbinding"), this) );

		// No control above us - likely cause is that we're the result of a passive open that
		// hasn't yet been accepted.
		SetDetaching();
		Unbind();
		}
	}

void CTransportFlowShim::IoctlComplete(TDesC8 *aBuf)
	{
	__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockSSocksspshm, 38));
	iSessionControlNotify->IoctlComplete(aBuf);
	}

void CTransportFlowShim::NoBearer(const TDesC8& aConnectionParams)
/**
Upcall from protocol indicating no underlying bearer for the socket.
@param aConnectionParams Additional connection parameters from the stack, in the form of a
human readable string.  Initially, the required protocols (e.g. "protocol=ip").
*/
	{
	// *** NOTE ***
	// overridden by CUpsTransportFlowShim::NoBearer() for UPS support

	(void)aConnectionParams;
	LOG( ESockLog::ConnectionInfoPrintf(aConnectionParams, _L("CTransportFlowShim %08x:\tNoBearer()"), this) );

	DoNoBearer(); // Do not care if actually posted, so no need to check return value

	}

void CTransportFlowShim::Bearer(const TDesC8& aConnectionInfo)
	{
	//we might need to translate the aConnectionInfo into "our" conn info format
	LOG( ESockLog::ConnectionInfoPrintf(aConnectionInfo, _L("CTransportFlowShim %08x:\tBearer()"), this) );

#if defined(SYMBIAN_TRACE_ENABLE)
	const TSoIfConnectionInfo *info = REINTERPRET_CAST(const TSoIfConnectionInfo*, aConnectionInfo.Ptr());
	NM_LOG((KESockServerTag, _L8("CTransportFlowShim %08x:\tSynchronous call: From=%08x To=%08x Func=Bearer(Iap %d, Network %d)"),
			this, static_cast<Messages::ANode*>(this), static_cast<Messages::ANode*>(this), info->iIAPId, info->iNetworkId) )
#endif

	if (TConnectionInfo::IsLocalBearer(aConnectionInfo))
		{
		return;
		}

	SetUseBearerErrors();
	LocalName(iLocalAddress);
	SetLocalAddressSet();
	RemName(iRemoteAddress);
	SetRemoteAddressSet();
	__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockSSocksspshm, 41));	// legacy flows have no control side; should never get here

	PostDataClientRouted();
	}

TInt CTransportFlowShim::LockToConnectionInfo()
	{
	//bind provider with the selected connection
	// Note that the following SetOption() can result in an upcall into
	// CTransportFlowShim::Bearer() in the same stack frame.
    TPckgBuf<TConnectionInfo> info;
    if (Control(KSOLProvider, static_cast<TUint>(KSoConnectionInfo), info) == KErrNone)
		{
		LOG(ESockLog::Printf(_L("CTransportFlowShim %08x:\tLockToConnectionInfo() Iap %d, Network %d"), this, info().iIapId, info().iNetId));
		NM_LOG((KESockServerTag, _L8("CTransportFlowShim %08x:\tSynchronous call: From=%08x To=%08x Func=LockToConnectionInfo(Iap %d, Network %d)"),
				this, static_cast<Messages::ANode*>(this), static_cast<Messages::ANode*>(this), info().iIapId, info().iNetId) )

		TPckg<TSoIfConnectionInfo> ifInfo(iIfInfo);
	    ifInfo().iIAPId = info().iIapId;
    	ifInfo().iNetworkId = info().iNetId;
		iProvider->SetOption(KSOLProvider, static_cast<TUint>(KSoConnectionInfo), ifInfo);
		return KErrNone;
		}
    else
    	{
    	LOG(ESockLog::Printf(_L("CTransportFlowShim %08x:\tLockToConnectionInfo()"), this));
    	return KErrNotFound;
    	}
	}

void CTransportFlowShim::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    if (aMessage.IsMessage<TCFDataClient::TBindTo>() && !NoBearerGuard())
    	{
    	//TODO PROD RZ
    	//This needs investigating and fixing. There are two cases for why is this needed here:
    	//(1) Bearer mobility is the only scenario for unsolicited TBindTo
    	//(hence checking the guard above). CTransportFlowShim cannot just migrate to the new, as the
    	//socket above it needs to be rebound. mobility_761220:
    	//  (1) opens and starts a connection
    	//  (2) opens a socket against
    	//  (3) sendreceive UDP data and expects (packet colouring) that data to go via IAP4.
    	//  (4) migrates to another bearer (IAP5)
    	//  (5) uses the same socket (without rebinding) to sendreceive data and expects
    	//    the data to go via IAP4 again (despite migrating).
    	//Talk to Ivan Kelly to find out if this is a requirement on the socket (stays connected
    	//over the old until rebound) or is this a wrong test case.
    	//If the former, ipscpr will need changing not to propagate TBindTo to its flows if they
    	//don't ask.
    	//(2) Rejoing this flow. During rejoin scenario, it's the new owner obtains its new
    	//dataclient ('this') before ('this') dataclient knowing. The rejoin procedure is make
    	//before break, hence it tries to apply the new owner, during which time the new
    	//owner starts and hence attempts to TBindTo his new child. The child hates it
    	//as it arrives from an unknown node. The rejoin protocol needs rethinking.
		RClientInterface::OpenPostMessageClose(Id(), aSender, TCFDataClient::TBindToComplete().CRef());
    	return;
    	}
    CNetworkFlow::ReceivedL(aSender, aRecipient, aMessage);
#ifdef SYMBIAN_NETWORKING_UPS
	// Allow derived classes to process received messages.
	//
	// Rationale for ProcessReceivedL() virtual:
	// The existing idiom whereby we call CNetworkFlow::ReceivedL() up front is awkward.
	// If we move that call into the derived class, the derived class can do its
	// specialised processing and then decide to call this base class method for any
	// remaining more general processing.  In this case, the ProcessReceivedL() would
	// not be required.  However, CTransportFlowShim will then become unusable if
	// instantiated on its own because the call to CNetworkFlow::ReceivedL() is not present.
    TInt ret = ProcessReceivedL(aMessage);
    if (ret == KErrNone || ret != KErrNotSupported)
    	{
    	return;	// PREQ1116 Productisation: RE-WORK ProcessReceivedL().  Perhaps return TBool indicating whether message processed.  Leave if a "real" error.  Extra arguments?
    	}
#endif

   	if ( aMessage.IsMessage<TEBase::TError>() )
   		{
		SubConnectionError(static_cast<TEBase::TError&>(aMessage));
		}
   	else if (aMessage.IsMessage<TEBase::TCancel>())
   		{
   		// TCancel can be received as a result of a Connect() being cancelled on the socket and a
   		// TBindToComplete() with an error being sent to the PRBindTo activity on our control provider.
   		}
   	else if (TEChild::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TEChild::TDestroy::EId :
			Destroy();
			break;
		default:
			//TODO - logging
			//LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("ERROR: CTransportFlowShim %08x:\tReceivedL(%s) - KErrNotSupported"), this, aMessage.Printable()));
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksspshm, 42)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else if (TCFDataClient::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TCFDataClient::TStart::EId :
			StartFlowL(aSender);
			break;
		case TCFDataClient::TStop::EId :
			StopFlow(static_cast<TCFDataClient::TStop&>(aMessage));
			break;
		case TCFDataClient::TProvisionConfig::EId :

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
			//Store the provision config message,
			//to store the Provision config message
            StoreProvision(static_cast<TCFDataClient::TProvisionConfig&>(aMessage));
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

			break;
		case TCFDataClient::TCommitBindTo::EId :
			/*TODO RZ PROD- mustn't just ignore TCommitBindTo - must implement*/
			break;
		case TCFDataClient::TBindTo::EId :
			{
			TCFDataClient::TBindTo& bindToMsg(static_cast<TCFDataClient::TBindTo&>(aMessage));
			TRAPD(err,BindToL(bindToMsg));
			// Ensure that TBindToComplete message gets sent before TIdle so that it gets to the destination
			// before destroy processing.
			if(err == KErrNone)
			    {
			    RClientInterface::OpenPostMessageClose(Id(), aSender, TCFDataClient::TBindToComplete().CRef());
			    }
			else
			    {
			    RClientInterface::OpenPostMessageClose(Id(), aSender, TEBase::TError(aMessage.MessageId(), err).CRef());
			    }
			
			
			ProcessDCIdleState();	// in case we were waiting to send idle
			//If we have received TDataClientStart before (when we did not yet have a bearer),
			//we complete the start here as well
			if (err != KErrNone)
				{
				//If we have received TDataClientStart before (when we did not yet have a bearer),
				//we complete the start here as well
				if (iStartRequest.IsOpen())
					{
					CompleteStart(err);
					}
				SetBearerExpected();
				}
			else
		    	{
				// If we get a TBindTo message then the TNoBearer request has succeeded
				// and we can inform the client.
		    	if (iSessionControlNotify)
		    		{
		    		iSessionControlNotify->SetLocalNameComplete();
		    		}
		        }
		    break;
			}
		default:
			//TODO - logging
			//LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("ERROR: CTransportFlowShim %08x:\tReceivedL(%s) - KErrNotSupported"), this, aMessage.Printable()));
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksspshm, 43)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else if (TCFFlow::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TCFFlow::TRejoin::EId :
			Rejoin(static_cast<TCFFlow::TRejoin&>(aMessage));
			break;
		default:
			//TODO - logging
			//LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("ERROR: CTransportFlowShim %08x:\tReceivedL(%s) - KErrNotSupported"), this, aMessage.Printable()));
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksspshm, 44)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else if (TCFInternalEsock::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
	    case TCFInternalEsock::TFlowProvision::EId :
			SetFlowParams((static_cast<TCFInternalEsock::TFlowProvision&>(aMessage)).iFlowParams);
			break;
		default:
			//TODO - logging
			//LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("ERROR: CTransportFlowShim %08x:\tReceivedL(%s) - KErrNotSupported"), this, aMessage.Printable()));
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksspshm, 45)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}

//This is the case of Modulation change for a bearer.
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	else if (TCFMessage::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TCFMessage::TTransportNotification::EId :
          	//In case of modulation change,
			//extract the value from this message,
			//pass it to the shim to set the options for socket.
	   		SetProtocolOptions();
			break;
        default:
        	//TODO : Logging for error message
        	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksspshm, 46)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

	else if ( aMessage.IsMessage<TCFPeer::TJoinComplete>() )
		{;}
	else if ( aMessage.IsMessage<TCFControlProvider::TBearer>() )
		{
		//If we have received TDataClientStart before (when we did not yet have a bearer),
		//we complete the start here as well
		if (iStartRequest.IsOpen())
			{
			CompleteStart(KErrNone);
			}
		LOG( ESockLog::Printf(_L8("CTransportFlowShim %08x:\tReceivedL(): TBearer: iDeleteUponBearerReception %d"), this, DeleteUponBearerReception()));

		ClearNoBearerGuard();
		ProcessDCIdleState();
		NoBearerCompletion();		// may delete this !
		}
	else
		{
		//TODO - logging
		//LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("ERROR: CTransportFlowShim %08x:\tReceivedL(%s) - KErrNotSupported"), this, aMessage.Printable()));
    	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksspshm, 47)); //For debug configurations
    	User::Leave(KErrNotSupported); //For release configurations
		}
    }

void CTransportFlowShim::NoBearerCompletion()
	{
	ClearBearerExpected();

	if (DeleteUponBearerReception())
		{
		delete this;
		}
	}

const TNodeId& CTransportFlowShim::NodeId() const
	{
	return Id();
	}

void CTransportFlowShim::CompleteStart(TInt aError)
	{
	if (aError==KErrNone)
		{
		iStartRequest.ReplyTo(Id(), TCFDataClient::TStarted().CRef());
		SetStarted();
		ClearStopped();
		}
	else
		{
		iStartRequest.ReplyTo(Id(), TEBase::TError(TCFDataClient::TStart::Id(),aError).CRef());
		}
	iStartRequest.Close();
	}

void CTransportFlowShim::SubConnectionError(const TEBase::TError& errorMsg, TUint anOperationMask)
	{
	LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tSubConnectionError() errorMsg.iValue %d, anOperationMask %d"), this, errorMsg.iValue, anOperationMask) );

	if (errorMsg.iMsgId == TCFControlProvider::TNoBearer::Id())
		{
		LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tSubConnectionError() - clearing no-bearer guard"), this) );
		ClearNoBearerGuard();
#ifdef SYMBIAN_NETWORKING_UPS
		ProcessDCIdleState();
#endif
		NoBearerCompletion();
		}

	if (IsBoundToSession())
	    {
		LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tSubConnectionError() - calling Error() function"), this) );
    	Error(errorMsg.iValue, anOperationMask);
	    }
	else if (iHostResolverNotify)
	    {
		LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tSubConnectionError() - passing to host resolver's Error() function"), this) );
	    iHostResolverNotify->Error(errorMsg.iValue);
	    }
	}

void CTransportFlowShim::BindToL(TCFDataClient::TBindTo& aBindTo)
/**
Request from control side (at network layer) to indicate that the SubConnection is
up and running and that we should bind to a Flow below.

@param aLowerFlow Flow below to bind to.
*/
	{
	//provisioning message must come before bindto in case we didn't get it after we've joined
	//the sub-connection

	NM_LOG((KESockServerTag, _L8("CTransportFlowShim %08x:\tSynchronous call: From=%08x To=%08x Func=BindToL"),
			this, static_cast<Messages::ANode*>(this), &aBindTo.iNodeId.Node()) )

	if (ShuttingDown())
		{
		User::Leave(KErrCancel);
		return;
		}

	CNetworkFlow::BindToL(aBindTo);
	if (iLowerFlow && IsBoundToSession())
		{
		LockToConnectionInfo();
		LocalName(iLocalAddress);
		SetLocalAddressSet();
		RemName(iRemoteAddress);
		SetRemoteAddressSet();
		
		__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockSSocksspshm, 48));	// legacy flows have no control side; should never get here
		}
	else if (iHostResolverNotify)
	    {//workaroud to indicate to CHostResolver we've got connection info
	    if (!aBindTo.iNodeId.Ptr())
	        {
            iHostResolverNotify->Error(KErrDisconnected);
	        }
	    else
	    if (iFlowParams.iFlowRequestType == TFlowParams::EExplicitConnection)
	        {
            // Re-issue explicit host resolver requests here rather than later on in StartFlowL().  This is
            // to accomodate the HotSpot server and Internet Connectivity Test (ICT).  The ICT hooks into the
            // NetCfgExtensionBase mechanism, blocks the interface startup at 8400 and performs a host resolver
            // request.  However, the TransportFlowShim will only receive StartFlowL() after the interface has
            // fully come up, so chicken and egg.  Implicit host resolver requests must still be re-issued in
            // StartFlowL().
            iHostResolverNotify->StartSending();
	        }
	    }
	}


void CTransportFlowShim::Rejoin(const TCFFlow::TRejoin& aRejoinMessage)
    {
	LOG( ESockLog::Printf(_L("CTransportFlowShim %08x:\tRejoin()"), this ));
	iSubConnectionProvider.Close();
	iSubConnectionProvider.Open(TNodeCtxId(0, aRejoinMessage.iNodeId));
    //If already bound there's a potential need to rebind.
    //As the new control provider for the flow - if different than
    //the current, rebind.
    if (iLowerFlow)
        {
        PostNoBearer();
        }
    }

void CTransportFlowShim::StartFlowL(const TRuntimeCtxId& aSender)
	{
	__ASSERT_DEBUG(!Started(), User::Panic(KSpecAssert_ESockSSocksspshm, 49));
	__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockSSocksspshm, 50));	// legacy flows have no control side; should never get here

	//We will wait for it and complete the start after we have received it
	User::LeaveIfError(iStartRequest.Open(iSubConnectionProvider, aSender));

	if (Idle())
		{
		iStartRequest.ReplyTo(Id(), TEBase::TError(TCFDataClient::TStart::Id(), KErrNotReady).CRef());
		iStartRequest.Close();
		return;
		}

	if (iLowerFlow)
		{
		iStartRequest.ReplyTo(Id(), TCFDataClient::TStarted().CRef());
		iStartRequest.Close();
		SetStarted();
		ClearStopped();

        // A held-over implicit resolution request will now work (if it ever will).
		// Explicit host resolver requests have already been re-issued in BindToL().
        if (iHostResolverNotify && (iFlowParams.iFlowRequestType != TFlowParams::EExplicitConnection))
	        {
    	    iHostResolverNotify->StartSending();
	        }

		return;
		}

	//We need a bearer
		PostNoBearer(); //Ask for bearer if not requested already

	}

void CTransportFlowShim::StopFlow(TCFDataClient::TStop& aMessage)
	{
	__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockSSocksspshm, 52));	// legacy flows have no control side; should never get here

	// We need to error the socket if the lower protocol stack is not going to do this.  Once a
	// Bearer() upcall has been received, the lower protocol will call Error() if the connection
	// goes down.  Before this point, the lower protocol will not call Error(), so StopFlow() calls
	// from the SCPR are used to error the socket (if the interface start fails).
	//
	// The main scenario is the TCP/IP stack as lower protocol which will only call Error() once it
	// has attached a flow to a route (and hence an interface) which, in turn, only occurs once the
	// interface is up and data has been sent over the socket.  Note that opening an RSocket on an
	// RConnection but not transferring any data will not cause the TCP/IP stack to attach the flow
	// to the route and hence not call Error() if the interface comes down.

	if (IsBoundToSession() && aMessage.iValue == KErrForceDisconnected)
		{
		Error(KErrDisconnected, EErrorAllOperations);
		}
	else if (IsBoundToSession() && !UseBearerErrors())
	    {
    	Error(aMessage.iValue, EErrorAllOperations);
	    }

	if (iLowerFlow)
		{
		iLowerFlow->Unbind(NULL,NULL);
		iLowerFlow = NULL;
		}
	iLowerControl = NULL;

	iSubConnectionProvider.PostMessage(Id(), TCFDataClient::TStopped(aMessage.iValue).CRef());
	ClearStarted();
	SetStopped();
	}

void CTransportFlowShim::InitDestroy()
	{
	__ASSERT_DEBUG(!(Idle() || IdleSent()), User::Panic(KSpecAssert_ESockSSocksspshm, 53));
    SetIdle();

    if(iSubConnectionProvider.IsOpen())	// legacy flows have no control side
    	{
    	ProcessDCIdleState();
    	}
	else
		{
		DeleteThisFlow();
		}
	}

void CTransportFlowShim::Destroy()
	{
	DeleteThisFlow();
	}

void CTransportFlowShim::PostNoBearer()
	{
	if (!NoBearerGuard())
		{
		iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TNoBearer().CRef());
		SetNoBearerGuard();
		}
	}

void CTransportFlowShim::PostDataClientRouted()
	{
	if (LocalAddressSet() && RemoteAddressSet()
		&& iRemoteAddress.Family() != KAFUnspec && !DataClientRoutedGuard())
		{
		iSubConnectionProvider.PostMessage(
			Id(),
			TCFIPMessages::TDataClientRouted(
				TAddrUpdate(
					iLocalAddress,
					iRemoteAddress,
					iFlowParams.iProtocol,
					iIfInfo.iIAPId)
				).CRef()
			);
		SetDataClientRoutedGuard();
		}
	}

/*
Store the provision information passed from the Control side.
@param TProvisionConfig Message recd.
*/
void CTransportFlowShim::StoreProvision(TCFDataClient::TProvisionConfig& aMessage)
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	//find access point inforamtion from the message
	iAccessPointConfig.Close();
	iAccessPointConfig.Open(aMessage.iConfig);
	//Find extension appended at NetMcpr and store it
	//Presently will extract CSAPSetOpt window extensions
	ExtractSetOptExtensions();
#else
	(void)aMessage;
#endif
	}
/*
Extract extensions appended to TProvisionConfig specific to IPTransport.
*/
void CTransportFlowShim::ExtractSetOptExtensions()
	{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	//Extract TCP receive window specific provisionconfig extension.
	iProtocolOptions = static_cast< const CSAPSetOpt*>(AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CSAPSetOpt::EUid,CSAPSetOpt::ETypeId)));
#endif
	}

#ifdef SYMBIAN_NETWORKING_UPS

TBool CTransportFlowShim::ActivityRunning()
/**
Determine whether we are in the middle of a NoBearer activity.

In other words, we are waiting for a response from a previously transmitted TNoBearer.

@return ETrue if in the middle of one of this activity, else EFalse.
*/
	{
	return NoBearerGuard();
	}

//
// Default implementations of CTransportFlowShim virtuals.
//

TInt CTransportFlowShim::SetupForNoBearerOnSend()
	{
	return KErrNotSupported;
	}

void CTransportFlowShim::PreUnbind()
	{
	}

TInt CTransportFlowShim::ProcessReceivedL(TSignatureBase& /*aCFMessage*/)
	{
	return KErrNotSupported;
	}

//
// CUpsTransportFlowShim methods
//
// This class contains User Prompt Service (UPS) specific functionality.
//

CUpsTransportFlowShim* CUpsTransportFlowShim::NewL(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf)
/**
Create a new CUpsTransportFlowShim instanace.

Note: this method replaces CTransportFlowShim::NewL() in UPS builds.
*/
	{
	return new (ELeave) CUpsTransportFlowShim(aFactory, aSubConn, aProtocolIntf);
	}

TInt CUpsTransportFlowShim::SetupForNoBearerOnSend()
/**
Called just before a TNoBearer message is posted for Host Resolver processing.

Perform the Platform Security capability check and populate the UPS Access Point Config (APC) extension.

@return KErrNone or a system wide error code.
*/
	{
	// For UPS, the capability check has been moved from the provider SAP to here so that
	// the capability check result is available at the time of the NoBearer message being sent.
	__ASSERT_DEBUG(iSecurityChecker, User::Panic(KSpecAssert_ESockSSocksspshm, 54));
	TInt result = iSecurityChecker->CheckPolicy(KPolicyNetworkServices, "UpsTransportFlowShim (SetupForNoBearerOnSend)");

	// At this point, result can contain:
	//
	// KErrNone					Capability check passed.
	// KErrPermissionDenied		Capability check failed.
	// KErrCompletion			Special error code that instructs us not to perform any UPS check.
	//							This is a means for upper layers to disable UPS checking in circumstances
	//							where the UPS check will be catered for by a higher layer.
	if (result != KErrCompletion)
		{
		if (UpsEnabled())
			{
			// UPS enabled - fill in the APC structure associated with TNoBearer message
			PopulateUpsExtension(result);
			}
		else
			{
			// UPS disabled - fall back to the Platform Security check
			if (result != KErrNone)
				{
				return TSendResult(result);
				}
			}
		}
	return KErrNone;
	}

void CUpsTransportFlowShim::PreUnbind()
/**
Called just before an unbind happens.

Ensure that we cancel any running activities.

(there are no activities on the data side in the formal sense - the term is used
to describe NoBearer processing happening locally).
*/
	{
	if (ActivityRunning())
		{
		// @TODO PREQ1116 - should we be able to have a NoBearer and a PolicyCheckRequest pending
		// at the same time ?  Should we prevent it?

		// Cancel any pending NoBearer activity.  This has become necessary because the UPS authorisation
		// can cause the NoBearer activity to be pending for an indefinate period of time.  When the
		// cancel reaches the MCpr, the UPS authorisation will be cancelled and a TError will eventually
		// be received and processed in SubConnectionError().
		iSubConnectionProvider.PostMessage(Id(), Messages::TEBase::TCancel().CRef());
		}
	}

TInt CUpsTransportFlowShim::SetOption(TUint aLevel, TUint aName, const TDesC8 &anOption)
/**
Process SetOption() requests from upper layer
*/
	{
	if (aLevel == KSOLProvider)
		{
		if (aName == KSoOwnerInfo)
			{
			// Upper layer communicating process and thread id of client opening subsession.
			__ASSERT_DEBUG(anOption.Size() == sizeof(TSoOwnerInfo), User::Panic(KSpecAssert_ESockSSocksspshm, 55));

			const TSoOwnerInfo* soOwnerInfoPtr = reinterpret_cast <const TSoOwnerInfo*> (anOption.Ptr());
			__ASSERT_DEBUG(soOwnerInfoPtr, User::Panic(KSpecAssert_ESockSSocksspshm, 56));

			iThreadId  = soOwnerInfoPtr->iThreadId;
			iProcessId = soOwnerInfoPtr->iProcessId;
			}
		else
		if (aName == KSoSetPlatSecApi)
			{
			// Upper layer communicating an 'M' class instance that can be used for retrieving
			// process and thread id of client performing current operation.
			__ASSERT_DEBUG(anOption.Size() == sizeof(const MPlatsecApiExt*), User::Panic(KSpecAssert_ESockSSocksspshm, 57));
			iPlatsecIf = *reinterpret_cast <const MPlatsecApiExt* const *> (anOption.Ptr());
			}

		if (IsHostResolver())
			{
			// For Host Resolvers, the SetOption() is only for us to store the thread and process id,
			// so we are complete.  There is no iProvider in this case to propagate the call to.
			__ASSERT_DEBUG(Provider() == NULL, User::Panic(KSpecAssert_ESockSSocksspshm, 58));
			return KErrNone;
			}
		}

	return CTransportFlowShim::SetOption(aLevel, aName, anOption);
	}

TInt CUpsTransportFlowShim::SecurityCheck(MProvdSecurityChecker* aChecker)
/**
Called from upper layer to ask the SAP provider to perform a security policy check
on the client process.

@param aChecker Security checker class instance
@return KErrNone or a system wide error code
*/
	{
	// Save away the security checker class so that we can perform capability checking
	// here instead of in the TCP/IP stack.
	//
	// Note: iSecurityChecker can get overwritten as a result of ProtocolManager::TransferSocketL(),
	// so don't ASSERT(iSecurityChecker == NULL).
	iSecurityChecker = aChecker;

	if (Provider())
		{
		// Sockets
		__ASSERT_DEBUG(!IsHostResolver(), User::Panic(KSpecAssert_ESockSSocksspshm, 59));
		return Provider()->SecurityCheck(aChecker);
		}
	else
		{
		// Host Resolvers
		__ASSERT_DEBUG(IsHostResolver(), User::Panic(KSpecAssert_ESockSSocksspshm, 60));
		return KErrNone;
		}
	}

void CUpsTransportFlowShim::GetProcessAndThreadId(TProcessId& aProcessId, TThreadId& aThreadId) const
/**
Retrieve the process and thread id corresponding to the current Socket/Host Resolver request.

The iProcessId and iThreadId variables contain the ids at the time of subsession open, which
is not necessarily the same as the ids of the client performing the actual request.  The
call to iPlatsecIf->GetProcessAndThreadId() will actually go to the layer above for processing
and retrieve the ids associated with the current pending request.

@param aProcessId variable receiving process id (out)
@param aThreadId variable receiveing thread id (out)
@return KErrNone, else a system wide error code.
*/
	{
	TInt err = KErrGeneral;
	if (iPlatsecIf)
		{
		err = iPlatsecIf->GetProcessAndThreadId(aProcessId, aThreadId);
		}

	if (err != KErrNone)
		{
		aProcessId = iProcessId;
		aThreadId = iThreadId;
		}
	}

void CUpsTransportFlowShim::NoBearer(const TDesC8& aConnectionParams)
/**
Upcall from protocol indicating no underlying bearer for the socket.
@param aConnectionParams Additional connection parameters from the stack, in the form of a
human readable string.  Initially, the required protocols (e.g. "protocol=ip").

This method overrides CTransportFlowShim::NoBearer() for UPS specific handling.
*/
	{
	(void)aConnectionParams;
	LOG( ESockLog::Printf(_L8("CUpsTransportFlowShim %08x:\tNoBearer(%S)"), this, &aConnectionParams) );

	// Perform the Platform Security check (moved from the Provider SAP to here in order to
	// obtain the platsec result as part of UPS processing).

	TInt result = iSecurityChecker->CheckPolicy(KPolicyNetworkServices, "TransportFlowShim (NoBearer)");

	NM_LOG((KESockServerTag, _L8("CTransportFlowShim %08x:\tSynchronous call: From=%08x To=%08x Func=NoBearer(%S)"),
			this, static_cast<Messages::ANode*>(this), static_cast<Messages::ANode*>(this), &aConnectionParams) )

	// At this point, result can contain:
	//
	// KErrNone					Capability check passed.
	// KErrPermissionDenied		Capability check failed.
	// KErrCompletion			Special error code that instructs us not to perform any UPS check.
	//							This is a means for upper layers to disable UPS checking in circumstances
	//							where the UPS check will be catered for by a higher layer.

	TBool disableUpsCheck = (result == KErrCompletion);
	if (!disableUpsCheck && !UpsEnabled())
		{
		// UPS disabled - fall back to the Platform Security check
		if (result != KErrNone)
			{
			Error(result, MSocketNotify::EErrorSend | MSocketNotify::EErrorConnect);
			return;
			}
		}

	if(!Stopped())
		{ // Prevent sending NoBearer if DataClientStop was received
		ParseNoBearerParams(aConnectionParams);
		if (iIsScoped)
		    {
		    if (!disableUpsCheck && UpsEnabled())
		        {
	            // Do not perform standard NoBearer() processing for "gratuitous" NoBearer() calls.  The
	            // TCP/IP stack already has enough scope information to select an interface, but this
	            // probably will be different from the "default" implicit interface that the control side will
	            // select.  The consequence is that the TransportFlowShim could end up being associated with the
	            // wrong SCpr.  Note that this is not a problem introduced by UPS functionality - it exists in
	            // the Networking Subsystem anyway.  The "Bearer()-only" cases in pre-UPS code do not result in
	            // the TCP/IP stack generating a NoBearer() (just a Bearer()), so the TransportFlowShim never
	            // gets associated with any SCpr.  We are just replicating this behaviour for UPS functionality,
	            // by performing a TPolicyCheckRequest sequence for the sake of UPS checking, but otherwise the
	            // behaviour mirrors what is already present (i.e. does nothing else).
	            PostPolicyCheckRequest(result);
		        }
            else
                {
                // If no UPS handling is required, tell the TCP/IP stack that it can go ahead and route the flow.
				IssueScopedNoBearerResponse();
                }
		    }
		else
		if (LockToConnectionInfo() != KErrNone)
		    {
			__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockSSocksspshm, 61));	// legacy flows have no control side; should never get here
			// If UPS is enabled, fill in the APC structure to accompany the TNoBearer
			// Do this even if disableUpsCheck is set so that the APC will contain KErrCompletion.
			if (UpsEnabled())
				{
				PopulateUpsExtension(result);
				}
	        PostNoBearer();
		    }
		ClearUseBearerErrors();
		ClearDataClientRoutedGuard();
		}
	else
	    {
	    // Return an error on an attempt to issue a SendTo() on an explicit socket (datagram) whose corresponding
	    // connection has gone down (BR2639).  Note that the use of KErrDisconnected should NOT be documented -
	    // in future, we may wish to return the same error as the connection itself experienced.
	    Error(KErrDisconnected, MSessionControlNotify::EErrorSend);
	    }
	}

void CUpsTransportFlowShim::PopulateUpsExtension(TInt aPolicyCheckResult) const
/**
Populate the UPS Access Point Config structure with the required information in preparation for
a TNoBearer message to be transmitted.

@param aPolicyCheckResult result of Platform Security check
*/
	{
	__ASSERT_DEBUG(iUpsExtension, User::Panic(KSpecAssert_ESockSSocksspshm, 62));
	iUpsExtension->SetPolicyCheckResult(aPolicyCheckResult);

	TProcessId processId;
	TThreadId threadId;
	GetProcessAndThreadId(processId, threadId);

	iUpsExtension->SetProcessId(processId);
	iUpsExtension->SetThreadId(threadId);

	if (IsHostResolver())
		{
		// Null destination name for Host Resolvers.
		iUpsExtension->ResetDestinationAddr();
		iUpsExtension->SetDestinationAddrType(ETNone);
		}
	else
		{
		// @TODO PREQ1116 - something better than this cast?  For example, argument to
		// SetDestinationAddr() could become a "const TDesC8&" perhaps (without worrying about
		// error checking) ?
		iUpsExtension->SetDestinationAddr(static_cast<TUpsDestinationAddr>(iRemoteAddress));
		iUpsExtension->SetDestinationAddrType(ETSockAddress);
		}
	}


void CUpsTransportFlowShim::PostPolicyCheckRequest(TInt aPolicyCheckResult)
/**
Post a TPolicyCheckRequest message to the SCpr

@param aPolicyCheckResult result of Platform Security check
*/
	{
	if (!PolicyCheckRequestPending())
		{
		__ASSERT_DEBUG(UpsEnabled(), User::Panic(KSpecAssert_ESockSSocksspshm, 63));	// UPS should not be "short circuited" off

		TProcessId processId;
		TThreadId threadId;
		GetProcessAndThreadId(processId, threadId);

		const TPolicyCheckRequestParams params(processId, threadId, aPolicyCheckResult, static_cast<TUpsDestinationAddr>(iRemoteAddress), ETSockAddress);
		iSubConnectionProvider.PostMessage(Id(), UpsMessage::TPolicyCheckRequest(params).CRef());
		SetPolicyCheckRequestPending(ETrue);
		}
	}

void CUpsTransportFlowShim::ProcessPolicyCheckResponse(const UpsMessage::TPolicyCheckResponse& aResponse)
/**
Process a TPolicyCheckResponse from the SCpr.

If UPS authorisation was granted, prod the TCP/IP stack into re-attaching the flow (based on the original
scope id in the socket address).

@param aResponse TPolicyCheckResponse message
*/
	{
	SetPolicyCheckRequestPending(EFalse);

	if (Idle() && !IdleSent())
		{
		ProcessDCIdleState();
		}
	else
		{
		if (aResponse.iValue == KErrNone)
			{
			if (iIsScoped)
				{
				// If socket address already has a scope id, do not tell the stack to use our
				// idea of NetworkId.  If we do, our idea of the NetworkId could be a mismatch
				// with the existing scope id, and we could end up with an SCPR which represents a
				// different interface from the one that the TCP/IP stack has chosen based on the scope
				// id.  Use the special KNetworkIdFromAddress value for NetworkId which the TCP/IP stack
				// recognises as meaning "attempt to attach the flow to a route, but determine the
				// NetworkId from the socket address, as you've already indicated in the NoBearer()
				// that you have enough information to do so".
				//
				// This code has become necessary due to the UPS support, which forces the
				// TCP/IP stack to issue NoBearer() calls in circumstances where it never used to
				// before, to give an opportunity to perform UPS authorisation.
				IssueScopedNoBearerResponse();
				}
			}
		else
			{
			Error(aResponse.iValue, MSocketNotify::EErrorSend | MSocketNotify::EErrorConnect);
			}
		}
	}

void CUpsTransportFlowShim::IssueScopedNoBearerResponse()
/**
After a gratuitous NoBearer() upcall has been issued to us (to allow a chance for UPS checking to occur before
a flow is routed), issue the response to the TCP/IP stack indicating that it can now route the flow and continue.
 */
    {
    TSoIfConnectionInfo info;
    TPckg<TSoIfConnectionInfo> ifInfo(info);
    info.iIAPId = 0;
    info.iNetworkId = KNetworkIdFromAddress;
    __ASSERT_DEBUG(Provider(), User::Panic(KSpecAssert_ESockSSocksspshm, 64));
    SetIfInfo(info);
    Provider()->SetOption(KSOLProvider, static_cast<TUint>(KSoConnectionInfo), ifInfo);
    }

void CUpsTransportFlowShim::ParseNoBearerParams(const TDesC8& aConnectionParams)
/**
Parse the descriptor passed in NoBearer() upcall.

Main raison d'etre for this routine is to check whether the "scoped" string was
part of the descriptor contents (indicating a "gratuitous" NoBearer()).

@param aConnectionParams Argument passed in NoBearer() upcall from lower layer.
*/
	{
	_LIT8(KScoped, "scoped");
	if (aConnectionParams.Find(KScoped()) != KErrNotFound)
		{
		// "scoped" string signifies a "gratuitous" NoBearer().  The flow could have been routed
		// without a NoBearer() (i.e. it was scoped enough to route it), but the NoBearer() was
		// generated anyway so that ESock could perform UPS checking before routing it.
		iIsScoped = ETrue;
		}
	else
		{
		iIsScoped = EFalse;
		}
	}

TInt CUpsTransportFlowShim::ProcessReceivedL(TSignatureBase& aCFMessage)
/*
Process any SCpr messages specific to UPS functionality.

Called from base class (CTransportFlowShim) before it attempts to process the incoming message.

@param aCFMessage Message from SCpr
@return KErrNone if we have handled the incoming message and no more processing is necessary,
KErrNotSupported if have not handled the incoming message, else a system wide error code.
*/
    {
	// Check for special UPS messages.
	if (UpsMessage::ERealmId == aCFMessage.MessageId().Realm())
		{
        switch (aCFMessage.MessageId().MessageId())
		  {
		case UpsMessage::TPolicyCheckResponse::EId :
			{
			// Support for UPS
			const UpsMessage::TPolicyCheckResponse& policyCheckResponseMsg = static_cast<UpsMessage::TPolicyCheckResponse&>(aCFMessage);
			ProcessPolicyCheckResponse(policyCheckResponseMsg);
			return KErrNone;
			}
		  }
		}
	else
	if (TCFDataClient::ERealmId == aCFMessage.MessageId().Realm())
		{
		switch (aCFMessage.MessageId().MessageId())
		  {
		case TCFDataClient::TProvisionConfig::EId :
			{
           	// Get a pointer to the UPS Access Point Config structure.  If not present, then UPS is
           	// disabled ("short circuited") and iUpsExtension remains set to NULL.
           	const  TCFDataClient::TProvisionConfig& provisionMsg = static_cast<TCFDataClient::TProvisionConfig&>(aCFMessage);
           	
           	iAccessPointConfig.Close();
           	iAccessPointConfig.Open(provisionMsg.iConfig);
           	
			const Meta::SMetaData* const extension = AccessPointConfig().FindExtension(
			        STypeId::CreateSTypeId(CUPSAccessPointConfigExt::EUPSAccessPointConfigUid, CUPSAccessPointConfigExt::ETypeId));
			if (extension)
				{
				iUpsExtension = const_cast<CUPSAccessPointConfigExt*> (static_cast<const CUPSAccessPointConfigExt*>(extension));
				}
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
			//
			//Store the provision config message,
			//to store the Provision config message
			//which will have TCP Receive window sizes.
            StoreProvision(static_cast<TCFDataClient::TProvisionConfig&>(aCFMessage));
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

			return KErrNone;
	       	}
		  }
		}
	return KErrNotSupported;
	}

void CUpsTransportFlowShim::SubConnectionError(const Messages::TEBase::TError& aErrorMsg, TUint anOperationMask)
/**
Handle errors from SCpr

@param aErrorMsg The incoming TError message
@param anOperationMask Pending operations to be error'd.
*/
	{
	if (aErrorMsg.iMsgId == UpsMessage::TPolicyCheckRequest::Id())
		{
		// Previously sent TPolicyCheckRequest failed.
		SetPolicyCheckRequestPending(EFalse);
		ProcessDCIdleState();
		}

	CTransportFlowShim::SubConnectionError(aErrorMsg, anOperationMask);
	}

TBool CUpsTransportFlowShim::ActivityRunning()
/**
Determine whether we are in the middle of NoBearer or PolicyCheckRequest processing.

In other words, we are waiting for a response from a previously transmitted TNoBearer
or TPolicyCheckRequest.

@return ETrue if in the middle of one of this activity, else EFalse.
*/
	{
	return PolicyCheckRequestPending() || CTransportFlowShim::ActivityRunning();
	}

#endif //f_NETWORKING_UPS



//Register the class variable
START_ATTRIBUTE_TABLE(CSAPSetOpt,CSAPSetOpt::EUid, CSAPSetOpt::ETypeId)
END_ATTRIBUTE_TABLE()

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
//Set the protocol options
void CTransportFlowShim::SetProtocolOptions()
	{
	if(iProvider && iProtocolOptions)
		{
		for(TInt iter = 0; iter < iProtocolOptions->iOption.Count(); iter++)
			{
   	    	TPtr8 optionDesProtOptVal( (TUint8*)&(iProtocolOptions->iOption[iter].iOptionValue), sizeof(TUint), sizeof(TUint) );
			iProvider->SetOption(iProtocolOptions->iOption[iter].iOptionName,iProtocolOptions->iOption[iter].iOptionLevel, optionDesProtOptVal);
			}
		}
	}
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

EXPORT_C CSAPSetOpt::CSAPSetOpt()
	{
	}

CSAPSetOpt::~CSAPSetOpt()
	{
	//Free the RArray resources.
	iOption.Close();
	}

//Add the option to the RArray variable
EXPORT_C void CSAPSetOpt::AddProtocolOptionL(TUint aOptionName, TUint aOptionLevel, TUint aOptionValue)
	{
	TProtocolOption opt;
	opt.iOptionName = aOptionName;
	opt.iOptionLevel = aOptionLevel;
	opt.iOptionValue = aOptionValue;
	iOption.AppendL(opt);
	}

//Update the value for an option
EXPORT_C void CSAPSetOpt::UpdateProtocolOption(TUint aOptionName, TUint aOptionLevel, TUint aOptionValue)
	{
	for(TInt i = 0; i < iOption.Count(); i++)
		if((iOption[i].iOptionName == aOptionName) && (iOption[i].iOptionLevel == aOptionLevel))
			iOption[i].iOptionValue = aOptionValue;
	}



