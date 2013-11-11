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
//

/**
 @file
 @internalComponent
*/
#include "ss_roles.h"

#include <e32base.h>
#include <ss_glob.h>
#include <comms-infras/ss_log.h>
#include <es_ini.h>
#include <cfthread.h>
#include <rsshared.h>	// logging tags
#include "ss_subconn.h"
#include <comms-infras/ss_tiermanagerutils.h>
#include <ss_sock.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_internal.h>
#endif
#include "SS_rslv.H"
#include "SS_conn.H"
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_sapshim.h>
#include <comms-infras/idquerynetmsg.h>

#include <comms-infras/nifprvar.h> //for KNifEntireConnectionSubConnectionId
#include <elements/responsemsg.h> //CResponseMsg
#include "ss_connectionsession.h"
#include "ss_flowrequest.h"
#include "ss_msgs.h"
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include "ss_connectionserver.h"
#include "ss_tierthreadmap.h"

#include <comms-infras/ss_nodemessages_internal_esock.h>
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
#include <comms-infras/commsbufpondop.h>
#else
#include <es_mbman.h>
#endif // SYMBIAN_ZERO_COPY_NETWORKING



#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocks_rls, "ESockSSocks_rls.");
#endif

using ESock::MProviderSelector;
using ESock::ISelectionNotify;

#define MSG_PRM(prmIndex)		(prmIndex)
using namespace NetInterfaces;

using namespace NetInterfaces;
using namespace ESock;
using namespace Messages;
using namespace Den;
using namespace CommsFW;

#include <cs_subconparams.h>



//
// CPlayer
//

CPlayer* CPlayer::NewL(CWorkerThread* aOwnerThread, TPlayerRole aPlayerRole)
	{
	CPlayer* self = new(ELeave) CPlayer(aOwnerThread, aPlayerRole);
	return self;
	}

/**
The Player destructor doesn't have much to do as a lot of the cleanup is done during the
normal shutdown routines. Here the Player merely deletes all sub-sessions it owns.
*/
CPlayer::~CPlayer()
	{
	// The object container is stored as a packed array, so working backwards through it avoids invalidating
	// the iterator when removing entries (and as a bonus is more efficient)
	LOG(ESockLog::Printf(KESockBootingTag, _L8("CPlayer::~CPlayer()")));
    iTransferBuffer.Close();
	}

CPlayer::CPlayer(CWorkerThread* aOwnerThread, TPlayerRole aPlayerRole)
:	CCommonPlayer(aOwnerThread, aPlayerRole)
	{
	LOG(ESockLog::Printf(KESockBootingTag, _L8("CPlayer::CPlayer()")));
	}

CSockSubSession* CPlayer::SubSession(const Den::TSubSessionUniqueId& aSubSessionUniqueId) const
	{
	return static_cast<CSockSubSession*>(CCommonPlayer::SubSession(aSubSessionUniqueId));
	}

CSockSession* CPlayer::CurrentSession() const
	{
	return static_cast<CSockSession*>(CCommonPlayer::Session());
	}

CWorkerThread& CPlayer::WorkerThread() const
	{
	return static_cast<CWorkerThread&>(CCommonPlayer::WorkerThread());
	}

/**
Check whether the Player is ready to shut down and if so, tells the owning Worker Thread
*/
TBool CPlayer::IsPlayerShutdownComplete()
	{
	CSockManData* globals = SockManGlobals::Get();
	// Basic reason: we have no subsessions or protocol families remaining open
	TBool shutdownComplete = globals->iNumFamilies == 0;
	LOG_STMT(TBool dataThreadShutdownPending = EFalse;)
	if(shutdownComplete)
		{
		// But it could be that we're providing management services and have a data thread still bound; if so we hold-off from shutdown
		// until the data thread completes its own shutdown. Being relatively dumb it certainly can't survive without us.
		if(!HasDataPlane() && !SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EDataPlane)).IsNull())
			{
			shutdownComplete = EFalse;
			LOG_STMT(dataThreadShutdownPending = ETrue;)
			}
		}

	LOG(ESockLog::Printf(KESockBootingTag, _L8("CPlayer::IsPlayerShutdownComplete(), shutdownComplete = %d [#subSess=%d, numFam=%d, dataPending=%d]"),
		shutdownComplete, SubSessions().Count(), globals->iNumFamilies, dataThreadShutdownPending) );

	if(globals->iNumFamilies)
		{
		// Help them figure out what (might be) blocking shutdown
#ifdef __FLOG_ACTIVE
		TProtocolManagerLogger::LogLoadedInfo();
#endif
		}

	return shutdownComplete;
	}

/**
Called directly by the PitBoss to get protocol information.
@note Index is 1-based for legacy reasons.
*/
TInt CPlayer::ProtocolInfo(TUint aIndex, TProtocolDesc& aProtocol)
	{
	// This could be optimised, eg to remember the last found position in case caller is iterating from 1..n protocols (since the
	// code here also iterates that gives O2 complexity). However, it is likely that this is a seldom-used function
	if((aIndex > SockManGlobals()->iNumProtocols) || aIndex<1)
		{
		return KErrNotSupported;
		}

	// Simply iterate along the queue aIndex times
	TSglQueIter<CProtocolRef> i(*SockManGlobals()->iProtocols);

	while(--aIndex)
		{
		i++;
		}

	aProtocol=((CProtocolRef*)i)->Info();

	return KErrNone;
	}

/**
Write a handle back to Ptr3 of the current message
*/
TInt CPlayer::WriteSubSessionHandle(TInt aHandle)
	{
	TPckgC<TInt> pH(aHandle);
	return SafeMessage().Write(MSG_PRM(3),pH);
	}

void CPlayer::CommsApiExtBindIfaceL(const RMessage2& aMessage, CSockSubSession& aSubSession)
	{
	aSubSession.CommsApiExtBindIfaceL(aMessage);
	}

void CPlayer::CommsApiExtIfaceSendReceiveL(const RMessage2& aMessage, CSockSubSession& aSubSession)
	{
	aSubSession.CommsApiExtIfaceSendReceiveL(aMessage);
	}

void CPlayer::CloseExtensionInterface(const RMessage2& aMessage, CSockSubSession& aSubSession)
	{
	aSubSession.CloseExtensionInterface(aMessage);
	}

/**
Called from a socket to delete it
Removing a socket has the side effect of Close()ing it.
*/
void CPlayer::DeleteSocket(CSocket& aSocket)
	{
	aSocket.InitiateDestruction();
	}



MShimControlClient* CPlayer::CSubConnectionProviderFromHandleL(ESock::CConnection& /*aConnection*/, TSubConnectionUniqueId /*aSubConnectionUniqueId*/)
	{//this is a very link shim specific function to send a message to the shim connection provider factory
	LOG( ESockLog::Printf(_L("CPlayer %08x:\tCSubConnectionProviderFromHandleL() KErrNotReady"), this) );
	User::Leave(KErrNotReady);
	return NULL;
	}

/**
	Process the client message forwarded from the Dealer. For subsession-Close commands
	the Dealer provides the subsession pointer explicitly as it has already been removed from
	the index.
*/
void CPlayer::DoProcessMessageL(const RSafeMessage& aMessage, Den::CWorkerSubSession* aSubSession)
	{
	LOG(
		TBuf8<64> messBuf;
		ESockLog::IPCMessName((TSockMess) aMessage.Function(), messBuf);
		ESockLog::Printf(KESockSessDetailTag, _L8("CPlayer:\tProcessMessageL: session=%08x, subsess=%08x, Message(%08x) [%S]"), Session(), aSubSession, aMessage.Handle(), &messBuf);
	);

	switch (aMessage.Function())
		{
	case ESoCreateNull:
		{
		TInt dummyHandle;
		NewSocketL(ETrue, dummyHandle);
		break;
		}

//	case ESSNumProtocols:
//		NumProtocols();
//		break;
	case ESSProtocolInfo:
		ProtocolInfo();
		break;

	case ESSProtocolInfoByName:
		ProtocolInfoByName();
		break;

	case ESSProtocolStart:
		LoadProtocolL(aMessage.Int0(),aMessage.Int1(),aMessage.Int2());
		break;

	case ESSProtocolStop:
		UnLoadProtocolL(aMessage.Int0(),aMessage.Int1(),aMessage.Int2());
		break;

// socket messages
	case ESoCreate:
		NewSocketDefaultL();
		break;

	case ESoCreateWithConnection:
		NewSocketWithConnectionL();
		break;

	case ESoCreateWithSubConnection:
		NewSocketWithSubConnectionL();
		break;

	case ESoTransfer:
		__ASSERT_DEBUG(aSubSession->Type().iType == TCFSubSessInfo::ESocket, User::Panic(KSpecAssert_ESockSSocks_rls, 2));	// Dealer does all of the validation for us
		TransferSocketL(static_cast<CSocket*>(aSubSession));
		break;

// Host resolver message types
	case EHRCreate:
		NewHostResolverDefaultL(aMessage.Int0(),aMessage.Int1());
		break;

	case EHRCreateWithConnection:
		NewHostResolverWithConnectionL(aMessage.Int0(),aMessage.Int1(), aMessage.Int2());
		break;

// Service resolver message types
	case ESRCreate:
		NewServiceResolverL(aMessage.Int0(),aMessage.Int1(),aMessage.Int2());
		break;

// Net database message types
	case ENDCreate:
		NewNetDatabaseL(aMessage.Int0(),aMessage.Int1());
		break;

	// Connection Messages
	case ECNCreate:
		{
		NewConnectionL();
		break;
		}
	case ECNCreateWithName:
		{
		NewConnectionWithNameL(static_cast<CSockSubSession*>(aSubSession));
		break;
		}
/*
	case ESCPSStop:
	case ESCPSProgressNotification:
	case ESCPSCancelProgressNotification:
	case ESCPSDataTransferred:
	case ESCPSDataTransferredCancel:
	case ESCPSDataSentNotificationRequest:
	case ESCPSDataSentNotificationCancel:
	case ESCPSDataReceivedNotificationRequest:
	case ESCPSDataReceivedNotificationCancel:
	case ESCPSIsSubConnectionActiveRequest:
	case ESCPSIsSubConnectionActiveCancel:
	case ESCPSGetSubConnectionInfo:
		{
		__ASSERT_DEBUG(aSubSession, User::Panic(KSpecAssert_ESockSSocks_rls, 3));
		ESock::CConnection* cn = static_cast<ESock::CConnection*>(aSubSession);
		TSubConnectionUniqueId subConnectionUniqueId = aMessage.Function() != ESCPSGetSubConnectionInfo ? static_cast<TSubConnectionUniqueId>(aMessage.Int0()) : KNifEntireConnectionSubConnectionId;
		MShimControlClient* sc = CSubConnectionProviderFromHandleL(*cn, subConnectionUniqueId);
		if (!sc)
			{
			LOG( ESockLog::Printf(_L("CPlayer %08x:\tProcessMessageL() ESCPS...... KErrNotReady"), this) );
			User::Leave(KErrNotReady);
			}
		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocks_rls, 4));
		//@TODO service RConnection sub-connection oriented calls
		//iComplete = CSubConnection::ServiceL(*sc,aMessage);

		break;
		}
*/
	case ESCCreate:
		{
		LOG( ESockLog::Printf(KESockSubConnectionTag, _L8("CSubConnection %08x\tESCCreate aMessage %08x"), this, &SafeMessage()) );
		ESock::CSubConnection* sc = NewSubConnectionWithConnectionL();
    	__ASSERT_DEBUG(sc, User::Panic(KSpecAssert_ESockSSocks_rls, 5));
    	//If successful, the ownership of the RMessage2 is taken by the activity, otherwise completed on leave
    	//The activity also takes ownership of the CSubConnection for the duration of the activity for cleanup purposes
    	sc->CMMSockSubSession::ReceivedL(ESCCreate, ESock::TCFInternalEsock::TSubSess(ESCCreate,SafeMessage()).CRef());
		break;
		}

	case ECommsApiExtBindIface:
		__ASSERT_DEBUG(aSubSession, User::Panic(KSpecAssert_ESockSSocks_rls, 6));
		CommsApiExtBindIfaceL(aMessage, static_cast<CSockSubSession&>(*aSubSession));
		break;

	case ECommsApiExtIfaceSend:
	case ECommsApiExtIfaceSendReceive:
		__ASSERT_DEBUG(aSubSession, User::Panic(KSpecAssert_ESockSSocks_rls, 7));
		CommsApiExtIfaceSendReceiveL(aMessage, static_cast<CSockSubSession&>(*aSubSession));
		break;

	case ECommsApiExtIfaceClose:
		{
		CloseExtensionInterface(aMessage, static_cast<CSockSubSession&>(*aSubSession));
		break;
		}

	default:
		__ASSERT_DEBUG(aSubSession, User::Panic(KSpecAssert_ESockSSocks_rls, 8));

		aSubSession->ProcessMessageL();

		break;
		}

	// Message handlers can change the state of the iMessage if they want to hold onto the message.
	// They can also write a return value to iReturn.
	if (iComplete)
		{
		LOG(ESockLog::Printf(KESockServerTag, _L8("CPlayer:\tProcessMessageL, session=%08x, RMessage2::Complete (%08x) with %d."), Session(), aMessage.Handle(), iReturn) );
		aMessage.Complete(Return());
		}

	//This is a normal return so we do no longer need the reference
	SetSession(NULL);
	}

/**
Get info for a protocol by index.
*/
void CPlayer::ProtocolInfo()
	{
	TProtocolDesc prot;
	TInt ret=0;
	TInt localIndex = static_cast<CPitBoss&>(PitBoss()).GetLocalProtocolIndex(SafeMessage().Int1());
	if((ret=ProtocolInfo(localIndex,prot))==KErrNone)
		{
		TPckgC<TProtocolDesc> p(prot);
		ret = SafeMessage().Write(MSG_PRM(0),p);
		}
	SetReturn(ret);
	}

/**
Get protocol info by name.
*/
void CPlayer::ProtocolInfoByName()
	{
	TProtocolName name;
	TServerProtocolDesc prot;
	TInt ret=0;

	ret = SafeMessage().Read(MSG_PRM(1),name);

	if((ret=ProtocolInfo(name, prot))==KErrNone)
		{
		TPckgC<TProtocolDesc> p(prot);
		ret = SafeMessage().Write(MSG_PRM(0),p);
		}
	SetReturn(ret);
	}

/**
Find a protocol by name - no wildcard support.
*/
TInt CPlayer::ProtocolInfo(const TDesC &aName, TServerProtocolDesc &aProtocol)
	{
	TSglQueIter<CProtocolRef> i(*SockManGlobals::Get()->iProtocols);

	// Run the queue looking for a match.
	do
		{
		if(((CProtocolRef *)i)->Info().iName.CompareF(aName)==0)
			{
			aProtocol=((CProtocolRef*)i)->Info();
			return KErrNone;
			}
		i++;
		}
	while((CProtocolRef *)i);

	return KErrNotFound;
	}


/**
The socket reference is the object name preceded by the Worker's id and a space. This is
intended as an opaque cookie for use with socket transfer.
*/
void CPlayer::TransferSocketL(CSocket* aSocket)
	{
	__ASSERT_DEBUG(&aSocket->Player() == this, User::Panic(KSpecAssert_ESockSSocks_rls, 9));

	TInt id;
	TInt ret;

		{
		ProtocolManager::TransferSocketL(aSocket, this);
		aSocket->CancelAll();

		CurrentSession()->SubSessions().Lock();

		ret = PitBoss().AddSubSession(aSocket, CurrentSession(), id);
		aSocket->SetUniqueId(PitBoss().NextSubSessionUniqueId());

		CurrentSession()->SubSessions().Unlock();
		}

	if(ret == KErrNone)
		{
			{
			aSocket->Session()->SubSessions().Lock();

			// Remove from original session
			CSubSessionIx& srcSubSessions = aSocket->Session()->SubSessions();
			TInt oldIndex;
			VERIFY_RESULT(srcSubSessions.Find(aSocket, oldIndex), KErrNone);
			PitBoss().RemoveSubSession(oldIndex, static_cast<CSockSession*>(aSocket->Session()));

			aSocket->Session()->SubSessions().Unlock();
			}

		aSocket->SetSessionProxy(NULL);
		aSocket->SetSession(CurrentSession());
		aSocket->SetSessionProxy(CurrentSessionProxyL());
		ret = WriteSubSessionHandle(id);
		if (ret != KErrNone)
			{
			aSocket->InitiateDestruction();	// this should leave socket (almost) dead; we're not trying to reverse the transfer
			}
		if(ret == KErrNone && aSocket->RequiresOwnerInfo())
			{
			// store information about the client that created this socket
			aSocket->StoreOwnerInfo();
			aSocket->CommunicateOwner();
			}
		}
	if(ret != KErrNone)
		{
		User::Leave(ret);
		}
	}

/**
Create a new empty socket to be used for accept.
We need to make empty sockets just to get a valid handle to accept into.
*/
CSocket* CPlayer::NewSocketL(TBool aCompleteClientRequest, TInt& aHandle)
	{
	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CPlayer::NewSocketL(null sock)")) );

	CSocket *sp = NULL;
	TInt ret;

		{

		// Create a new socket and add it our list of subsessions
		sp = CSocket::NewLC(
			NULL,
			CurrentSession(),
			this,
			0,
			PitBoss().NextSubSessionUniqueId(),
			KUndefinedSockType);

		SubSessions().AppendL(sp);
		CleanupStack::Pop();

		// Now add the new socket, itself a subsession, to the global store
		CurrentSession()->SubSessions().Lock();
		ret = PitBoss().AddSubSession(sp, CurrentSession(), aHandle);
		if(ret == KErrNone)
			{
			if(aCompleteClientRequest)
				{
				ret = WriteSubSessionHandle(aHandle);
				}
			if(ret != KErrNone)
				{
				PitBoss().RemoveSubSession(aHandle, CurrentSession());
				}
			}

		CurrentSession()->SubSessions().Unlock();
		}

	if(ret != KErrNone)
		{
		sp->InitiateDestruction();
		User::Leave(ret);
		}

	return sp;
	}

/**
Create a new socket on this session for normal Open. The Player adds the new socket to
the session owned by the dealer (uses explicitly locked access).
*/
CSocket* CPlayer::NewSocketL(TUint aAddrFamily, TUint aSocketType, TUint aProtocol)
	{
	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CPlayer:\tNewSocketL(family=%d, type=%d, protocol=%d"), aAddrFamily, aSocketType, aProtocol) );

	CSocket* sp = NULL;
	TInt ret;

		{

		// Look up the protocol for which we are creating a socket
		CProtocolRef* protocolReference = ProtocolManager::FindProtocolL(aAddrFamily, aSocketType, aProtocol);

		// Create a new socket and add it our list of subsessions
		sp = CSocket::NewLC(
			&(protocolReference->Info()),
			CurrentSession(),
			this,
			protocolReference->Protocol(),
			PitBoss().NextSubSessionUniqueId(),
			aSocketType);

		SubSessions().AppendL(sp);
		CleanupStack::Pop();

		CurrentSession()->SubSessions().Lock();
		TInt id;
		ret = PitBoss().AddSubSession(sp, CurrentSession(), id);
		if(ret == KErrNone)
			{
			ret = WriteSubSessionHandle(id);
			if(ret != KErrNone)
				{
				// V1 ESock didn't remove after failure, but did for the other overload of NewSocketL().
				// Believed to have been in error
				PitBoss().RemoveSubSession(id, CurrentSession());
				}
			}

		CurrentSession()->SubSessions().Unlock();
		}

	if(sp->RequiresOwnerInfo())
		{
		// store information about the client that created this socket, to
		// communicate to the flow once we're bound
		sp->StoreOwnerInfo();
		}

	if(ret != KErrNone)
		{
		sp->InitiateDestruction();
		User::Leave(ret);
		}

	return sp;
	}

/**
Create a new socket on this session for normal Open. The Player adds the new socket to
the session owned by the dealer (uses explicitly locked access).
*/
void CPlayer::NewSocketDefaultL()
	{
	TUint aAddrFamily = SafeMessage().Int0();
	TUint aSocketType = SafeMessage().Int1();
	TUint aProtocol = SafeMessage().Int2();

	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CPlayer:\tNewSocketDefaultL(family=%d, type=%d, protocol=%d"), aAddrFamily, aSocketType, aProtocol) );
	CSocket* s = NewSocketL(aAddrFamily, aSocketType, aProtocol);

	TFlowParams flowParams(
		aAddrFamily,
		aSocketType,
		aProtocol,
		TFlowParams::EImplicit,
		CurrentSessionProxyL());

	//requesting a lower flow by throwing a self-dispatcher (TCFImplicitFlowRequest) through the
	//fence (onto the control plane). TCFImplicitFlowRequest assumes no preexiting stack (hence implicit)
	//and it will select and start a suitable stack based on the default access point.
	//That stack will be interrogated for a suitable flow (flowParams), which will be returned back
	//to the sender (socket) with TBindTo
	RClientInterface::OpenPostMessageClose(s->Id(),                     //socket is the sender
					  SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane)), //phoney recipient - we only care the recipient is on the control plane so that this message is dispatched on the correct thread
		              TCFImplicitFlowRequest(s->UniqueId(), flowParams));

	s->SetFlowRequestPending(ETrue);
	s->AdoptFlowRequestMessage(SafeMessage());

	DontCompleteCurrentRequest();
	}

/**
Create a new socket on this session for normal Open
*/
void CPlayer::NewSocketWithConnectionL()
	{
	// If we don't host the data plane then the request was forwarded to us to determine which data thread shall create the socket
	if(!HasDataPlane())
		{
		Messages::TNodeId flowFC = SockManGlobals()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EDataPlane));
		__ASSERT_DEBUG(!flowFC.IsNull(), User::Panic(KSpecAssert_ESockSSocks_rls, 10));
		TPlayerForwardRequestMsg msg(SafeMessage(), TPlayerForwardRequestMsg::NormalCreationFlag());
		WorkerThread().PostMessage(flowFC.Thread(), msg);
		DontCompleteCurrentRequest();
		return;
		}

	TPckgBuf<TSockOpen> argPkg;
	SafeMessage().ReadL(MSG_PRM(0),argPkg);

	//!!PS remember the race conditions in case someone closes connection in control thread
	//(Lock on the sub-session queue should persist over the code accessing cn*
	CSocket* s = NewSocketL(argPkg().iAddrFamily, argPkg().iSockType, argPkg().iProtocol);

	// Send the flow request message to the connection plane
	TFlowParams flowParams(
		argPkg().iAddrFamily,
		argPkg().iSockType,
		argPkg().iProtocol,
		TFlowParams::EExplicitConnection,
		CurrentSessionProxyL());

 	//requesting a lower flow by throwing a self-dispatcher (TCFConnFlowRequest) through the
 	//fence (onto the control plane). TCFConnFlowRequest will interrogate the connection (argPkg().iHandle)
 	//for its default subconnection and the subconnection for a suitable (flowParams) flow.
 	//The flow will be returned back to the sender (socket) with TBindTo.
	RClientInterface::OpenPostMessageClose(s->Id(), //socket is the sender
		SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EConnPlane)), //phoney recipient - we only care the recipient is on the control plane, so that this message will be dispatched on a correct thread.
		TCFConnFlowRequest(s->UniqueId(), *CurrentSession(), argPkg().iHandle, flowParams));

	s->SetFlowRequestPending(ETrue);
	s->AdoptFlowRequestMessage(SafeMessage());

	DontCompleteCurrentRequest();
    }

/**
Create a new socket on this session for normal Open.
*/
void CPlayer::NewSocketWithSubConnectionL()
	{
	// If we don't host the data plane then the request was forwarded to us to determine which data thread shall create the socket
	if(!HasDataPlane())
		{
		Messages::TNodeId flowFC = SockManGlobals()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EDataPlane));
		__ASSERT_DEBUG(!flowFC.IsNull(), User::Panic(KSpecAssert_ESockSSocks_rls, 11));
		TPlayerForwardRequestMsg msg(SafeMessage(), TPlayerForwardRequestMsg::NormalCreationFlag());
		WorkerThread().PostMessage(flowFC.Thread(), msg);
		DontCompleteCurrentRequest();
		return;
		}

	TPckgBuf<TSockOpen> argPkg;
	SafeMessage().ReadL(MSG_PRM(0),argPkg);

	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CPlayer:\tNewSocketWithSubConnectionL(family=%d, type=%d, protocol=%d"), argPkg().iAddrFamily, argPkg().iSockType, argPkg().iProtocol) );
	CSocket* s = NewSocketL(argPkg().iAddrFamily, argPkg().iSockType, argPkg().iProtocol);

	// Send the flow request message to the connection plane
	TFlowParams flowParams(
		argPkg().iAddrFamily,
		argPkg().iSockType,
		argPkg().iProtocol,
		TFlowParams::EExplicitSubConnection,
		CurrentSessionProxyL());

 	//requesting a lower flow by throwing a self-dispatcher (TCFSubConnFlowRequest) through the
 	//fence (onto the control plane). TCFConnFlowRequest will interrogate the subconnection (argPkg().iHandle)
 	//for a suitable (flowParams) flow. The flow will be returned back to the sender (socket) with TBindTo.
	RClientInterface::OpenPostMessageClose(s->Id(), //socket is the sender
		SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ESubConnPlane)), //phoney recipient - we only care the recipient is on the control plane so that this message is dispatched on the correct thread
  			TCFSubConnFlowRequest(s->UniqueId(), *CurrentSession(), argPkg().iHandle, flowParams));

	s->SetFlowRequestPending(ETrue);
	s->AdoptFlowRequestMessage(SafeMessage());

	DontCompleteCurrentRequest();
	}

/**
Create a new host resolver for this session.
*/
CHostResolver* CPlayer::NewHostResolverL(TUint anAddrFamily,TUint aProtocol)
	{
	CHostResolver* h = NULL;
	TInt ret;

		{

		h=ProtocolManager::NewHostResolverL(anAddrFamily,aProtocol, this, PitBoss().NextSubSessionUniqueId());

		TInt id;
		CurrentSession()->SubSessions().Lock();
		ret = PitBoss().AddSubSession(h, CurrentSession(), id);
		if(ret == KErrNone)
			{
			ret = WriteSubSessionHandle(id);
			if(ret != KErrNone)
				{
				PitBoss().RemoveSubSession(id, CurrentSession());
				}
			}
		CurrentSession()->SubSessions().Unlock();
		}

	if(ret != KErrNone)
		{
		h->InitiateDestruction();
		User::Leave(ret);
		}

	if(h->RequiresOwnerInfo())
		{
		// store information about the client that created this host resolver
		h->StoreOwnerInfo();
		}

	return (h);
	}

/**
Create a new host resolver for this session.
*/
void CPlayer::NewHostResolverDefaultL(TUint aAddrFamily,TUint aProtocol)
	{
	CHostResolver* h = NewHostResolverL(aAddrFamily, aProtocol);

	// Send the flow parameters to the tier manager plane
	TFlowParams flowParams(
		aAddrFamily,
		KUndefinedSockType,
		aProtocol,
		TFlowParams::EImplicit,
		CurrentSessionProxyL(),
		EFalse);

	//requesting a lower flow by throwing a self-dispatcher (TCFImplicitFlowRequest) through the
	//fence (onto the control plane). TCFImplicitFlowRequest assumes no preexiting stack (hence implicit)
	//and it will select and start a suitable stack based on the default access point.
	//That stack will be interrogated for a suitable flow (flowParams), which will be returned back
	//to the sender (host resolver) with TBindTo
	RClientInterface::OpenPostMessageClose(h->Id(),                     //socket is the sender
					  SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane)), //phoney recipient - we only care the recipient is on the control plane so that this message is dispatched on the correct thread
		              TCFImplicitFlowRequest(h->UniqueId(), flowParams));

	h->SetFlowRequestPending(ETrue);
	h->AdoptFlowRequestMessage(SafeMessage());

	DontCompleteCurrentRequest();
	}

/**
Create a new host resolver for this session.
*/
void CPlayer::NewHostResolverWithConnectionL(TUint aAddrFamily, TUint aProtocol, TInt aHandle)
	{
	CHostResolver* h = NewHostResolverL(aAddrFamily, aProtocol);

	// Send the flow parameters to the tier manager plane
	TFlowParams flowParams(
		aAddrFamily,
		KUndefinedSockType,
		aProtocol,
		TFlowParams::EExplicitConnection,
		CurrentSessionProxyL(),
		EFalse);

 	//requesting a lower flow by throwing a self-dispatcher (TCFConnFlowRequest) through the
 	//fence (onto the control plane). TCFConnFlowRequest will interrogate the connection (aHandle)
 	//for its default subconnection and the subconnection for a suitable (flowParams) flow.
 	//The flow will be returned back to the sender (socket) with TBindTo.
	RClientInterface::OpenPostMessageClose(h->Id(), //socket is the sender
		SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EConnPlane)), //phoney recipient - we only care the recipient is on the control plane, so that this message will be dispatched on a correct thread.
		TCFConnFlowRequest(h->UniqueId(), *CurrentSession(), aHandle, flowParams));

	h->SetFlowRequestPending(ETrue);
	h->AdoptFlowRequestMessage(SafeMessage());

	DontCompleteCurrentRequest();
	}


/**
Create a new service resolver for this session.
*/
void CPlayer::NewServiceResolverL(TUint anAddrFamily,TUint aSocketType,TUint aProtocol)
	{
	CServiceResolver* r = NULL;
	TInt ret;

		{

		r=ProtocolManager::NewServiceResolverL(anAddrFamily, aSocketType, aProtocol, this, PitBoss().NextSubSessionUniqueId());

		TInt id;
		CurrentSession()->SubSessions().Lock();
		ret = PitBoss().AddSubSession(r, CurrentSession(), id);
		if(ret == KErrNone)
			{
			ret = WriteSubSessionHandle(id);
			if(ret != KErrNone)
				{
				PitBoss().RemoveSubSession(id, CurrentSession());
				}
			}
		CurrentSession()->SubSessions().Unlock();
		}

	if (ret != KErrNone)
		{
		delete r;
		User::Leave(ret);
		}
	}

/**
Create a new net database for this session.
*/
void CPlayer::NewNetDatabaseL(TUint anAddrFamily,TUint aProtocol)
	{
	CNetDatabase* n = NULL;
	TInt ret;

		{

		n=ProtocolManager::NewNetDatabaseL(anAddrFamily, aProtocol, this, PitBoss().NextSubSessionUniqueId());

		TInt id;
		CurrentSession()->SubSessions().Lock();
		ret = PitBoss().AddSubSession(n, CurrentSession(), id);
		if(ret == KErrNone)
			{
			ret = WriteSubSessionHandle(id);
			if(ret != KErrNone)
				{
				PitBoss().RemoveSubSession(id, CurrentSession());
				}
			}

		CurrentSession()->SubSessions().Unlock();
		}

	if (ret!=KErrNone)
		{
		delete n;
		User::Leave(ret);
		}
	}

/**
Create a new socket on this session for normal Open.
*/
CSubConnection* CPlayer::NewSubConnectionWithConnectionL()
	{
	TPckgBuf<TSubConnOpen> argPkg;
	SafeMessage().ReadL(MSG_PRM(0),argPkg);

	ESock::CConnection* cn=static_cast<CSockSession*>(CurrentSession())->CConnectionFromHandle(argPkg().iHandle);
	if (!cn)
		{
		PanicClient(KESockClientPanic, ESockBadHandle);
		return NULL;
		}

	if (!cn->ServiceProvider())
	    {
		LOG( ESockLog::Printf(_L("CPlayer %08x:\tNewSubConnectionWithConnectionL() KErrNotReady"), this) );
		User::Leave(KErrNotReady);
	    }

	return NewSubConnectionL(*cn);
    }

/**
Create a new sub-connection for this session.
*/
CSubConnection* CPlayer::NewSubConnectionL(ESock::CConnection& aConnection)
	{
	ESock::CSubConnection* subconn = NULL;
	TInt ret;

		{

		subconn = ESock::CSubConnection::NewL(aConnection, this, PitBoss().NextSubSessionUniqueId());
		ret = SubSessions().Append(subconn);
		if(ret == KErrNone)
			{
			CurrentSession()->SubSessions().Lock();
			TInt id;
			TInt ret = PitBoss().AddSubSession(subconn, CurrentSession(), id);
			if(ret == KErrNone)
				{
				ret = WriteSubSessionHandle(id);
				if(ret != KErrNone)
					{
					PitBoss().RemoveSubSession(id, CurrentSession());
					}
				}
			CurrentSession()->SubSessions().Unlock();
			}

		}

	if (ret!=KErrNone)
		{
		delete subconn;
		User::Leave(ret);
		}

    return subconn;
	}

void CPlayer::NewConnectionL()
	{
	TUint family = SafeMessage().Int0();
	TUid tierId = TierManagerUtils::MapTierIdsL(TUid::Uid(family), 0);
	ESock::CConnection* conn = ESock::CConnection::NewLC(static_cast<CSockSession*>(CurrentSession()), this, tierId, PitBoss().NextSubSessionUniqueId());
	SetupNewConnectionL(conn);
	CleanupStack::Pop(conn);
	}

void CPlayer::NewConnectionWithNameL(CSockSubSession* aSubSession)
	{
	// The passed subsession is the original to base the creation off
	__ASSERT_DEBUG(aSubSession->Type().iType == TCFSubSessInfo::EConnection, User::Panic(KSpecAssert_ESockSSocks_rls, 12));	// Dealer does all of the validation for us

	CConnection& origConn = static_cast<CConnection&>(*aSubSession);
	// Police the clone open request against the security policy stored applied to the original RConnection
	User::LeaveIfError(origConn.CheckCloneOpenPolicy(SafeMessage()));
	ESock::CConnection* conn = ESock::CConnection::NewLC(static_cast<CSockSession*>(CurrentSession()), this, origConn, PitBoss().NextSubSessionUniqueId());
	SetupNewConnectionL(conn);
	CleanupStack::Pop(conn);
	}

/**
Handle creation of a new connection for this session
*/
void CPlayer::SetupNewConnectionL(CConnection* aConn)
	{
	TInt ret;

		{

		SubSessions().AppendL(aConn);
		TInt id;
		CurrentSession()->SubSessions().Lock();
		ret = PitBoss().AddSubSession(aConn, CurrentSession(), id);
		if(ret == KErrNone)
			{
			ret = WriteSubSessionHandle(id);
			if(ret != KErrNone)
				{
				PitBoss().RemoveSubSession(id, CurrentSession());
				}
			}

		CurrentSession()->SubSessions().Unlock();
		}

	if(ret!=KErrNone)
		{
		User::Leave(ret);
		}

	// store information about the client that created this connection
	// note that CConnections should always store their owner info
	aConn->StoreOwnerInfo();
	}

CCommonSessionProxy* CPlayer::DoCreateSessionProxyL(CWorkerSession* aSession)
	{
	return CSockSessionProxy::NewL(aSession, *this);
	}

void CPlayer::LoadProtocolL(TUint anAddrFamily,TUint aSocketType,TUint aProtocol)
	{
	ProtocolManager::LoadProtocolL(anAddrFamily,aSocketType,aProtocol,this);
	}

/**
	Find the protocol, check it is not referenced and delete it
*/
void CPlayer::UnLoadProtocolL(TUint anAddrFamily,TUint aSocketType,TUint aProtocol)
    {
    ProtocolManager::UnLoadProtocolL(anAddrFamily, aSocketType, aProtocol,this);
    }

/** Provides a sizeable buffer for strictly temporary use, eg within current stack frame: there's only one buffer
 * and no protocol for returning so shared use can't be detected: the intended use is for transferring data to/from 
 * a client. 
 * @return pointer to a buffer of at least the requested size, or NULL if this proves impossible
 */ 
TDes8* CPlayer::BorrowTemporaryBuffer(TInt aSize)
	{
	if(iTransferBuffer.Size() < aSize)
		{
		if(iTransferBuffer.ReAlloc(aSize) != KErrNone)
		    {
		    return NULL;
		    }
		}
	iTransferBuffer.SetMax();	// indicate whole of buffer available when used for MBuf CopyOut() - in general overwrite not append is expected since it's a temporary buffer
	return &iTransferBuffer;
	}


#ifdef _DEBUG
TBool CPlayer::RunPostBootChecks()
	{
	CWorkerThread& owner = WorkerThread();
	if(owner.DefaultOptimalDealer())
		{
		// Now that booting is completed verify that all protocols can be created via this dealer, ie that
		// our worker thread is bound to all of the necessary players
		TWorkerId host;
		for(TInt idx = 1; owner.PitBoss().GetWorkerForProtocol(idx, host); ++idx)
			{
			if(!owner.PeerReachable(host))
				{
				RDebug::Printf("ERROR worker %d is DefaultOptimalDealer but can't reach worker #%d for protocol #%d", owner.WorkerId(), host, idx);
				LOG(ESockLog::Printf(KESockBootingTag, _L("ERROR worker %d is DefaultOptimalDealer but can't reach worker #%d for protocol #%d"), owner.WorkerId(), host, idx));
				Panic(EMisconfigured);
				}
			}
		}
	return ETrue;
	}
#endif

//
// CSockSessionProxy
//
CSockSessionProxy* CSockSessionProxy::NewL(CWorkerSession* aSockSession, CPlayer& aPlayer)
	{
	CSockSessionProxy* self = new(ELeave) CSockSessionProxy(aSockSession, aPlayer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSockSessionProxy::CSockSessionProxy(CWorkerSession* aSockSession, CPlayer& aPlayer)
: 	CCommonSessionProxy(aSockSession, aPlayer)
	{
	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSockSessionProxy %08x:\tCSockSessionProxy(), iSockSession %08x"), this, Session()) );
	}

void CSockSessionProxy::ConstructL()
	{
	iProtocols=new(ELeave) CArrayFixFlat<CProtocolBase *>(16);
	}

CSockSessionProxy::~CSockSessionProxy()
	{
	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSockSessionProxy %08x:\t~CSockSessionProxy(), iSockSession %08x"), this, Session()) );
	if(iProtocols)
		{
		for (TInt i=0;i<iProtocols->Count();i++)
			{
			LOG(
				CProtocolBase* p = iProtocols->operator[](i); 
				const TDesC& tag(p->Tag());
				ESockLog::Printf(KESockSessDetailTag, _L("CSockSessionProxy %08x:\t~CSockSessionProxy(): closing protocol %08x '%S'"), this, p, &tag)
				);

			iProtocols->operator[](i)->Close();
			}
		iProtocols->Delete(0,iProtocols->Count());
		delete iProtocols;
		}
	}

void CSockSessionProxy::AddProtocolL(CProtocolBase* aProtocol)
	{
	LOG(
		const TDesC& tag(aProtocol->Tag());
		ESockLog::Printf(KESockSessDetailTag, _L("CSockSessionProxy %08x:\tAddProtocolL(aProtocol %08x '%S'), iSockSession %08x"), this, aProtocol, &tag, Session());
		);
	for(TInt i=0;i<iProtocols->Count();i++)
		{
		if (iProtocols->operator[](i)==aProtocol)
			{
			return;
			}
		}

	iProtocols->AppendL(aProtocol);
	aProtocol->Open();
	}

void CSockSessionProxy::RemoveProtocolL(CProtocolBase* aProtocol)
	{
	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSockSessionProxy %08x:\tRemoveProtocolL(), iSockSession %08x"), this, Session()) );

    CProtocolBase* p=0;
    TInt j;
    for(j=0;j<iProtocols->Count();j++)
		{
		if (iProtocols->operator[](j)==aProtocol)
            {
            p=iProtocols->operator[](j);
            break;
            }
		}
    if(!p)
    	{
        User::Leave(KErrNotFound);
    	}

    // in the absence of checking this protocol is not specifically referenced
    // check no resources are referenced
    if(Session()->SubSessions().ActiveCount())
    	{
        User::Leave(KErrInUse);
    	}

    p->Close();
    iProtocols->Delete(j);
    iProtocols->Compress();
	}

//
// CPitBoss
//

CPitBoss* CPitBoss::NewL(CWorkerThread* aOwnerThread)
	{
	CPitBoss* self = new(ELeave) CPitBoss(aOwnerThread);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPitBoss::~CPitBoss()
	{
	// Delete the list of removed protocols
	TProtocolPairing* pair = iDeadPairHead;
	while(pair)
		{
		TProtocolPairing* nextPair = pair->iNextDead;
		delete pair;
		pair = nextPair;
		}
	pair = iProtocolPairHead;
	while(pair)
		{
		TProtocolPairing* nextPair = pair->iNextPair;
		delete pair;
		pair = nextPair;
		}
	delete iCompleteEskList;
	}


CPitBoss::CPitBoss(CWorkerThread* aOwnerThread)
:	CCommonPitBoss(aOwnerThread)
	{
	}

void CPitBoss::ConstructL()
	{
	iCompleteEskList = new(ELeave) CommsFW::COwnEntryList(6);
	iCompleteEskList->UniqueWildScanAcrossDrivesL(KEsockIniFileDir, KEsockWildCard);
	CommsFW::COwnEntryList *noBackupEskList = new(ELeave) CommsFW::COwnEntryList(32);
	CleanupStack::PushL(noBackupEskList);
 	noBackupEskList->UniqueWildScanAcrossDrivesL(KEsockNoBackupDir, KEsockWildCard);
 	iCompleteEskList->AddL(*noBackupEskList);
 	iPropertyKey=RootServer::KUidC32StartPropertyKey;
 	CleanupStack::PopAndDestroy(noBackupEskList);
 	CCommonPitBoss::ConstructL();
	}

TBool CPitBoss::GetWorkerForProtocol(TUint aAddrFamily, TUint aSockType, TUint aProtocol, TWorkerId& aWorker) const
	{
#ifdef _DEBUG
	aWorker = TWorkerThreadPublicInfo::ENullWorkerId;	// ensure arg is polluted, lest caller ignore return value
#endif
	TProtocolPairing* pair = FindProtocolPairing(aAddrFamily, aSockType, aProtocol);
	if(pair)
		{
		aWorker = pair->iWorkerId;
		return aWorker != TWorkerThreadPublicInfo::ENullWorkerId;
		}
	return EFalse;
	}

void CPitBoss::DoFreeWorkerReferences(TWorkerId aWorkerId)
{
	LOG(ESockLog::Printf(_L("CPitBoss::DoFreeWorkerReferences(%d)"), aWorkerId) );
#if defined _DEBUG && !defined(ESOCKV3_TEMPORARY_PAIN_RELIEF)
	// The RootServer normally checks the heap for leaks when the module unloads
	// but for ESOCK modules this is commonly too early, since the PitBoss holds
	// its reference open until the cleanup completes. Hence here we check for
	// remaining allocations if we hold the last reference and if no thread
	// which used it died involuntarily
	RCFSharedHeap& heap = static_cast<RCFSharedHeap&>(*WorkerDataGlobals().GetWorkerGlobals(aWorkerId)->iHeap);
	TInt leakCount = heap.Count();	// make it accessible for conditional breakpoints
	LOG(ESockLog::Printf(_L8("~~~CPitBoss::FreeWorkerReferences heap(%08x).AccessCount()==%d, cell count=%d"), (TUint) &heap, heap.AccessCount(), leakCount));
	if(!iForsakenHeapList.IsForsaken(&heap))
		{
		if(heap.AccessCount() <= 2)	// Oddity: where does the other count come from, ie why not "1" when RS has already closed? And who does close it later?
			{
			if(leakCount > 0)
				{
				LOG(ESockLog::Printf(_L8("(log recorded under tags \"%S\" \"%S\" - you may need to enable these)"), &RootServer::KLogSubSysRSModule, &RootServer::KLogRSLeakTag));
				heap.LogAllocatedCells(RootServer::KLogSubSysRSModule, RootServer::KLogRSLeakTag);

				RProperty pubsub;
				TInt res = pubsub.Attach(RootServer::KUidCommsProcess, RootServer::KUidCommsModuleLeakCounter);
				//No nead for cleanup stack, cannot leave before Close
				if (res == KErrNone)
					{
					TInt count;
					res =pubsub.Get(count);
					if (res == KErrNone)
						{
						count += heap.Count();
						res =pubsub.Set(count);
						}
					}
				pubsub.Close();
				if (res != KErrNone)
					{
					__CFLOG_1(RootServer::KLogSubSysRSModule, RootServer::KLogRSLeakTag, _L8("Unable to report leaks. Error: %d"), res);
					}
				// As much as anything this log line is here to make it apparent that the breakpoint above was hit
				LOG(ESockLog::Printf(_L8("--- end of leaked cell log")));
				}
			}
		}

#else
	// Preventing unused variable warnings.
	(void)aWorkerId;
#endif
}

TBool CPitBoss::GetWorkerForProtocol(TUint aIndex, TWorkerId& aWorker) const
	{
#ifdef _DEBUG
	aWorker = TWorkerThreadPublicInfo::ENullWorkerId;	// ensure arg is polluted, lest caller ignore return value
#endif
	if (aIndex == 0)
		{
		return EFalse;	// protocol indices are 1-based
		}

	/* Move pointer to protocol@index or end of list */
	TProtocolPairing* pair = iProtocolPairHead;
	TUint i = 1;
	while(pair)
		{
		// This function only exists to support the client ability to retrieve a protocol by index, and they're only
		// interested in externally accessible protocols. So we skip magic ones. See also CPitBoss::GetNumProtocols()
		if(pair->iSockType < KReservedSockTypesBase)
			{
			if(i == aIndex)
				{
				break;
				}
			++i;
			}
		pair = pair->iNextPair;
		}

	if(pair)
		{
		aWorker = pair->iWorkerId;
		return aWorker != TWorkerThreadPublicInfo::ENullWorkerId;
		}
	return EFalse;
	}

TBool CPitBoss::GetWorkerForProtocolByName(const TProtocolName& aName, TWorkerId& aWorker) const
	{
#ifdef _DEBUG
	aWorker = TWorkerThreadPublicInfo::ENullWorkerId;	// ensure arg is polluted, lest caller ignore return value
#endif
	for(TProtocolPairing* pair = iProtocolPairHead; pair != NULL; pair = pair->iNextPair)
		{
		// Match the name (case insensitive)
		if(aName.CompareF(pair->iName) == 0)
			{
			aWorker = pair->iWorkerId;
			return aWorker != TWorkerThreadPublicInfo::ENullWorkerId;
			}
		}
	return EFalse;
	}

/**
Return worker which can accept a NULL Socket. Default favours a specific worker, but
if it is not installed return the first real worker in the list. This algorithmn could change
or default selection be made configurable.
@param aWorker Id of the worker which can accept a NULL socket. Unchanged if none found.
*/
TBool CPitBoss::GetWorkerForNullSocket(TWorkerId& aWorker) const
	{
	TBool found=EFalse;
	//First check if SMS WAP Worker is available by looking up heap pointer
	if(WorkerDataGlobals().WorkerPresent(TCFWorkerThreadPublicInfo::ESmsWapPlayerThread))
		{
		aWorker=TCFWorkerThreadPublicInfo::ESmsWapPlayerThread;
		found=ETrue;
		}
	else if(iProtocolPairHead && iProtocolPairHead->iWorkerId>0) // Take first worker in list
		{
		// Scan for the first valid worker (not all entries are for real protocols and some have null worker ids)
		for(TProtocolPairing* pair = iProtocolPairHead; !found && pair != NULL; pair = pair->iNextPair)
			{
			if(pair->iWorkerId != TWorkerThreadPublicInfo::ENullWorkerId)
				{
				aWorker = pair->iWorkerId;
				found = ETrue;
				}
			}
		}
	return found;
	}

/**
Return the number of known protocols. Counts by walking through the short list, so
not a super fast operation but very rarely called. Implement threadsafe (if needed) counters
on a rainy day.
*/
TUint CPitBoss::GetNumProtocols()
	{
	TUint num=0;
	// Count pairings that represent real protocols. The top range of sock types are reserved for internal trickery
	for(TProtocolPairing* pair = iProtocolPairHead; pair!=NULL; pair=pair->iNextPair)
		{
		if(pair->iSockType < KReservedSockTypesBase)
			{
			num++;
			}
		}
	return num;
	}


TBool CPitBoss::GetWorkerForTier(TInt aTierId, TWorkerId& aWorker) const
	{
	return GetWorkerForProtocol(KTierEntryProxyAddrFam, KTierEntryProxySockType, aTierId, aWorker);
	}


/**
Lookup a entry with the specified characteristics in the list of protocol pairings.
@see CPitBoss::TProtocolPairing
*/
CPitBoss::TProtocolPairing* CPitBoss::FindProtocolPairing(TUint aAddrFamily, TUint aSockType, TUint aProtocol) const
	{
	TProtocolPairing* pair = iProtocolPairHead;
	while(pair)
		{
		if(pair->iAddrFamily == aAddrFamily &&
			(pair->iSockType == KUndefinedSockType || aSockType == KUndefinedSockType || pair->iSockType == aSockType) &&
			(pair->iProtocol == KUndefinedProtocol || aProtocol == KUndefinedProtocol || pair->iProtocol == aProtocol))
			{
			LOG( ESockLog::Printf(KESockSessDetailTag, _L("CPitBoss::FindProtocolPairing() - [aAddrFamily=%08x] [aSockType=%08x] [aProtocol=%08x] found '%S' W%d"),
			   aAddrFamily, aSockType, aProtocol, &pair->iName, pair->iWorkerId));
			return pair;
			}
		pair = pair->iNextPair;
		}

	LOG( ESockLog::Printf(KESockSessDetailTag, _L("CPitBoss::FindProtocolPairing() - [aAddrFamily=%08x] [aSockType=%08x] [aProtocol=%08x] - no match"),
	   aAddrFamily, aSockType, aProtocol));
	return NULL;
	}

/**
Given session preferences, see if the Player supporting those protocol
characteristics has its own Dealer.
*/
TBool CPitBoss::FindOptimalDealer(const TSessionPref& aPref, CCommonWorkerDealer*& aDealer)
	{
	TBool found=EFalse;
	if(aPref.iAddrFamily == KUndefinedAddressFamily && aPref.iProtocol == KUndefinedProtocol)
		{
		// Client is asking for the default optimal dealer - if this isn't (yet) known then we quickly fail the request rather than possibly
		// waiting for boot to complete, which might disrupt carefully tuned boot orders - we're better off foregoing the mild optimisation
		if(iDefaultOptimalDealer != Den::TWorkerThreadPublicInfo::EMainThread) 
			{
			found = CCommonPitBoss::FindOptimalDealer(iDefaultOptimalDealer, aDealer);
			}
		}
	else
		{
		const TProtocolPairing* pair=FindProtocolPairing(aPref.iAddrFamily, KUndefinedSockType, aPref.iProtocol);
		if(pair)
			{
			found = CCommonPitBoss::FindOptimalDealer(pair->iWorkerId, aDealer);
			}
		}
	return found;
	}

CPitBoss::TProtocolPairingOwner::TProtocolPairingOwner()
: iHead(NULL)
	{
	}

void CPitBoss::TProtocolPairingOwner::Append(TProtocolPairing* aNode)
	{
	if(iHead == NULL)
		{
		iHead = aNode;
		}
	else
		{
		TProtocolPairing* curr = iHead;
		TProtocolPairing* prev;
		do
			{
			prev = curr;
			curr = curr->iNextPair;
			}
		while(curr != NULL);
		prev->iNextPair = aNode;
		}
	}

void CPitBoss::TProtocolPairingOwner::Release()
	{
	TProtocolPairing* curr = iHead;
	while(curr != NULL)
		{
		TProtocolPairing* next = curr->iNextPair;
		delete curr;
		curr = next;
		}
	iHead = NULL;
	}


void CPitBoss::AddProtocolToListL(TUint aAddrFamily, TUint aSockType, TUint aProtocol, const TProtocolName& aName, TWorkerId aWorker, TProtocolPairingOwner& aList)
	{
	TProtocolPairing* pair = new(ELeave) TProtocolPairing;
	pair->iAddrFamily = aAddrFamily;
	pair->iSockType = aSockType;
	pair->iProtocol = aProtocol;
	pair->iName		= aName;
	pair->iWorkerId = aWorker;
	pair->iNextDead = NULL;
	pair->iNextPair = NULL;
	aList.Append(pair);
	}

void CPitBoss::IncorporateProtocolListL(TProtocolPairingOwner& aList)
	{
	LOG(ESockLog::Printf(KESockBootingTag, _L("CPitBoss::IncorporateProtocolList(%08x)"), &aList) );
	// Only safe from the main thread; thread race below
	__ASSERT_DEBUG(iOwnerThread->IsMainThread(), User::Panic(KSpecAssert_ESockSSocks_rls, 14));

#ifdef _DEBUG
	// In UDEB check whether any pair entry already exists - if so that's a serious config mistake and we panic the miscreant diagnostically
	// In UREL we just ignore this possibility and let things limp on as best they can
	TProtocolPairing* curr = aList.iHead;
	while(curr != NULL)
		{
		LOG(ESockLog::Printf(KESockBootingTag, _L("CPitBoss: Adding protocol %S [fam,sock,prot]=[%x,%x,%x] for W%d"), &curr->iName, curr->iAddrFamily, curr->iSockType, curr->iProtocol, curr->iWorkerId) );
		TProtocolPairing* existing = FindProtocolPairing(curr->iAddrFamily, curr->iSockType, curr->iProtocol);
		if(existing)
			{
			LOG(ESockLog::Printf(KESockBootingTag, _L("CPitBoss: already present as protocol %S for W%d"), &existing->iName, existing->iWorkerId) );
			LOG(ESockLog::Printf(KESockBootingTag, _L("CPitBoss: Killing misconfigured W%d. To fix this look at the lists of .ESK files logged earlier for the two workers and ensure only one loads the protocol"), curr->iWorkerId) );
			WorkerDataGlobals().PanicWorker(curr->iWorkerId, KESockProtocolPanic, ECorruptIniData);
			User::Leave(KErrAlreadyExists);
			}
		curr = curr->iNextPair;
		}
#endif
	aList.Append(iProtocolPairHead);
	NETWORKING_ATOMIC(iProtocolPairHead = aList.iHead);	// atomic write of new ptr is guaranteed; it's ok to link it in before the worker ids are set
														// as any competing protocol lookups will politely fail
	}


/**
When a worker thread dies (e.g. during shutdown) the PitBoss will discover and use this method to
remove the protocol pairings for that particular worker.
*/
void CPitBoss::RemoveProtocolPairingsForWorker(TWorkerId aWorkerId)
	{
	LOG(ESockLog::Printf(KESockBootingTag, _L("CPitBoss::RemoveProtocolPairingsForWorker(%d)"), aWorkerId) );
	TProtocolPairing* pair = iProtocolPairHead;
 	TProtocolPairing* prevLive = NULL;
	while(pair)
		{
		if(pair->iWorkerId == aWorkerId)
			{
			LOG(ESockLog::Printf(KESockBootingTag, _L("CPitBoss: Removing protocol %S [fam,sock,prot]=[%x,%x,%x] for W%d"), &pair->iName, pair->iAddrFamily, pair->iSockType, pair->iProtocol, pair->iWorkerId) );
			if(prevLive)
				{
				NETWORKING_ATOMIC(prevLive->iNextPair = pair->iNextPair);		// atomic write of new ptr is guaranteed
				}
			else
				{
				NETWORKING_ATOMIC(iProtocolPairHead = pair->iNextPair);	// atomic write of new ptr is guaranteed
				}
			pair->iNextDead = iDeadPairHead;
			NETWORKING_ATOMIC(iDeadPairHead = pair);						// atomic write of new ptr is guaranteed
			}
		else
			{
			prevLive = pair;
			}
		pair = pair->iNextPair;
		}
	}

void CPitBoss::AddTierPairingToListL(TInt aTierUid, const TDesC& aTierName, TWorkerId aWorker, TProtocolPairingOwner& aList)
	{
	AddProtocolToListL(KTierEntryProxyAddrFam, KTierEntryProxySockType, aTierUid, aTierName, aWorker, aList);
	}

const CommsFW::COwnEntryList* CPitBoss::GetCompleteList()
	{
	return iCompleteEskList;
	}

/**
Used during binding when the PitBoss receives a introduction response message from a worker.
The PitBoss will set-up housekeeping datastructures for the worker and add the supported
protocols to its list of protocol pairings.
@see TWorkerMsg::EMainIntroductionResp
*/
void CPitBoss::DoProcessWorkerIntroductionL(const TWorkerIntroductionMsg& aMsg)
	{
 	// Now populate the protocol pairing list
	TProtocolPairingOwner pairList;
	CleanupReleasePushL(pairList);
	const TWorkerThreadPublicInfo& msgInfo = aMsg.WorkerInfo();
	CPlayer* player=static_cast<CPlayer*>(GetPlayer(aMsg));

	if(player)
		{
		TProtocolDesc prot;
		for(TUint protNum = 1; player->ProtocolInfo(protNum, prot) == KErrNone; ++protNum)
			{
			AddProtocolToListL(prot.iAddrFamily, prot.iSockType, prot.iProtocol, prot.iName, msgInfo.iWorkerId, pairList);
			}

		// Add proxy protocol pairings for any special roles
		if(player->HasTierResolver())
			{
#ifdef _DEBUG
			TWorkerId alternateTierResolverWorker;
			__ASSERT_DEBUG(!GetWorkerForPlayerRole(KPlayerRoleTierResolver, alternateTierResolverWorker), User::Panic(KSpecAssert_ESockSSocks_rls, 15));
#endif
			_LIT(KTierResolverDesc, "TierResolver");
			AddPlayerRolePairingL(KPlayerRoleTierResolver, KTierResolverDesc, msgInfo.iWorkerId, pairList);
			}
		}
	IncorporateProtocolListL(pairList);
	CleanupStack::Pop(&pairList);

	TBuf8<TWorkerIntroductionMsg::KMaxIntroductionInfoSize> introInfo;
	aMsg.IntroductionInfo(introInfo);
	TPckgBuf<TBool> defaultOptDealer;
	defaultOptDealer.Copy(introInfo.LeftTPtr(defaultOptDealer.MaxSize()));
	if(defaultOptDealer())
		{
		if(iDefaultOptimalDealer == Den::TWorkerThreadPublicInfo::EMainThread)
			{
			iDefaultOptimalDealer = msgInfo.iWorkerId; 
			}
		else
			{
			LOG(ESockLog::Printf(KESockBootingTag, _L("ERROR worker %d claiming DefaultOptimalDealer after worker %d already did so"), msgInfo.iWorkerId, iDefaultOptimalDealer));
#ifdef _DEBUG
			RDebug::Printf("ERROR worker %d claiming DefaultOptimalDealer after worker %d already did so", msgInfo.iWorkerId, iDefaultOptimalDealer);
			Panic(EMisconfigured);
#endif
			}
		}
	}

/**
The PitBoss monitors the Comms Configurator sequence level and when the core components
have been configured (this includes ESock) this method is called to delete any data structures
used only during startup of ESock.
@see CConfigurationLevelMonitor
@see CPitBoss::iPendingIntroResponses
*/
void CPitBoss::DoOnCPMsConfigured()
	{
	// We can now delete the shared ESK data and enable simulated failure for the main thread (if configured)
	if(iPendingIntroResponses == 0)
		{
		delete iCompleteEskList;
		iCompleteEskList = NULL;
		}
	if(iLoadTierMappingPhase == EDealerRequest)
		{
		// Was awaiting boot completion to find the tier resolver
		SendLoadTierMappingRequest();
		}
	}

/**
If a worker dies the PitBoss will call this method. It will clean up the housekeeping datastructures
related to the worker and it will spawn the RedShirt thread which will try to delete the workers own
data structures. It is a best effort attempt that doesn't guarantee to free up all the dead workers memory
and the RedShirt could be PANICed by the kernel, which is why a short lived seperate thread is doing it.
*/
void CPitBoss::DoOnPeerDeath(TWorkerId aWorkerId)
	{
	// If worker ran a Player all its protocol pairings are now dead. We can't know whether the thread actually
	// did run a Player as (presuming it exited cleanly) it cleaned up, but it's adequately cheap & a rare case
	RemoveProtocolPairingsForWorker(aWorkerId);
	}

struct ESockThreadStartupInfo
	{
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
	RCommsBufPond iCommsBufPond;
#else
	CMBufManager* iMBufManager;
#endif // SYMBIAN_ZERO_COPY_NETWORKING

	TAny* iModuleArgs;
	__CFLOG_STMT(CCFLogIf* iCFLogIf;)
	};

TInt RESockCleanupThreadFunction(TAny* aStartupInfo)
/**
Intermediate function which masquerades as the main thread function in order to
perform some specific actions for the new thread in the correct context before
calling the new thread's actual main thread function.
The thread must be resumed after being created or the startup info structure
cannot be deleted by the thread and will be leaked
@param aStartupInfo structure containing pointers to MBufMger and CFlog.
@see RCFThread::ThreadStartupInfo
@internalComponent
*/
	{
	ESockThreadStartupInfo* startInfo = reinterpret_cast<ESockThreadStartupInfo*>(aStartupInfo);
#ifdef 	SYMBIAN_ZERO_COPY_NETWORKING
	TCommsBufPondTLSOp tls(startInfo->iCommsBufPond);
	tls.Set();
#else
	startInfo->iMBufManager->SetContext();
#endif // SYMBIAN_ZERO_COPY_NETWORKING


   	__CFLOG_STMT( startInfo->iCFLogIf->SetContext(); )
	__CFLOG_OPEN;

   	TInt result = CCommonWorkerThread::PostMortemCleanupThreadEntry(startInfo->iModuleArgs);

	__CFLOG_CLOSE;
   	delete startInfo;

   	return result;
   	};

TInt CPitBoss::DoCreateRedShirt(RThread& aRedShirt, CommsFW::TWorkerId aWorkerId, Den::CCommonWorkerThread& aDeadWorker)
	{
	// Get the heap to assign to the red shirt thread and switch to it so we
	// can allocate the thread startup info structure.
	RAllocator* heap = WorkerDataGlobals().GetWorkerGlobals(aWorkerId)->iHeap;
	RHeap* prevHeap = User::SwitchHeap(heap);

	// We must not pass the thread a structure allocated on the stack because it
	// will go out of scope before the caller calls RThread::Resume so we create
	// it dynamically in the thread's heap.
	// The thread is responsible for deleting the structure when it no longer
	// needs it.  For this reason, if the thread is successfully created the
	// caller MUST resume the thread and allow it to run or else it will be
	// leaked.
   	ESockThreadStartupInfo* startupInfo = new ESockThreadStartupInfo;
 	User::SwitchHeap(prevHeap);

	TInt err = KErrNone;

   	if( startupInfo )
   		{
	   	startupInfo->iModuleArgs = &aDeadWorker;
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
	   	startupInfo->iCommsBufPond = TCommsBufPondTLSOp::Get();
		if(startupInfo->iCommsBufPond.IsNull())
			{
			err = KErrNotFound;
			}
#else
        startupInfo->iMBufManager = CMBufManager::Context();
        if(startupInfo->iMBufManager == NULL)
            {
            err = KErrNotFound;
            }
#endif // SYMBIAN_ZERO_COPY_NETWORKING

        // Check to make sure the logger is available.
		#ifdef __CFLOG_ACTIVE
        else
            {
            __CFLOG_STMT(startupInfo->iCFLogIf = CCFLogIf::Context());
            if(!startupInfo->iCFLogIf)
                {
                RDebug::Print( _L( "RCFThread::Create - the log interface was not found.  This normally means that the logging version of commsfw.dll has been mixed with a stub version of cflog.dll.  See CommsDebugUtility How-To Document FAQ section for details on enabling logging in a release build." ));

                err = KErrNotFound;
                }
            }
		#endif

		if(err == KErrNone)
			{
            // Attempt to create the RedShirt with a useful diagnostic name, to reduce the
            // likelihood that time is wasted debugging a problem in it rather than in the original
            // crashed worker
            TBuf<KMaxKernelName> threadName;
            _LIT(KNameFmt, "IgnoreMe_PostCrashCleanupHelper_%x");
            TWorkerThreadRegister* deadInfo = aDeadWorker.WorkerProperties(aDeadWorker.WorkerId()); 
            TUint deadThreadId = deadInfo? (TUint) deadInfo->iThreadId: User::TickCount();
            threadName.Format(KNameFmt, deadThreadId);
		   	err = aRedShirt.Create(threadName, RESockCleanupThreadFunction, 8192, static_cast<RHeap*>(heap), startupInfo);
			}
	   	// If any error occured, delete the startup info structure.
	   	if(err != KErrNone)
	   		{
	 		prevHeap = User::SwitchHeap(heap);
	  		delete startupInfo;
		  	User::SwitchHeap(prevHeap);
	  		}
  		}
	else
		{
    	err = KErrNoMemory;
   		}

   	return err;
	}

/**
Converts pit boss's protocol index to the local index
The pit boss maintains a global list of protocols while each worker thread
maintains its own list. Obviously the local list is shorter than the global list
This function takes the Pitboss list index and comes back with one that is
applicable to the local list
@param aPitBossIndex the index to the pit boss's protocol list NB starts at 1
@returns the index to the worker thread's list of protocols, -1 if not found
*/
TInt CPitBoss::GetLocalProtocolIndex(TInt aPitBossIndex) const
	{
	__ASSERT_DEBUG(aPitBossIndex > 0, User::Panic(KSpecAssert_ESockSSocks_rls, 16));
	TInt workers[TWorkerThreadPublicInfo::EMaxWorkerThreadId];
	Mem::FillZ(workers,sizeof(workers));

	TInt lastBox = 0;
	TProtocolPairing* pair = iProtocolPairHead;

	TInt i = 0;
	while(i < aPitBossIndex)
		{
		__ASSERT_DEBUG(pair, User::Panic(KSpecAssert_ESockSSocks_rls, 17));
		if(pair->iSockType < KReservedSockTypesBase)
			{
			lastBox = pair->iWorkerId - 1;
			__ASSERT_DEBUG(lastBox < TWorkerThreadPublicInfo::EMaxWorkerThreadId, User::Panic(KSpecAssert_ESockSSocks_rls, 18));
			++(workers[lastBox]);
			++i;
			}
		pair = pair->iNextPair;
		}

	TInt ret = workers[lastBox];
	return ret > 0 ? ret : -1;
	}

void CPitBoss::RequestLoadTierMapping()
	{
	if(iLoadTierMappingPhase == EStart)
		{
		if(ModuleConfigurationComplete())
			{
			SendLoadTierMappingRequest();
			}
		else
			{
			iLoadTierMappingPhase = EDealerRequest;
			}
		}
	}

void CPitBoss::SendLoadTierMappingRequest()
	{
    TWorkerId worker;
	if(GetWorkerForPlayerRole(KPlayerRoleTierResolver, worker))
		{
		TWorkerLoadTierMappings msg;
		WorkerThread().PostMessage(worker, msg);
		iLoadTierMappingPhase = EResolverRequested;
		}
	else
		{
		LOG(ESockLog::Printf(_L8("CPitBoss::SendLoadTierMappingRequest() *** NO TIER RESOLVER CONFIGURED *** ")));
		__ASSERT_DEBUG(0, Panic(EMisconfigured));	// configuration is so broken that leaving user in ignorance is probably unhelpful
		TRAP_IGNORE(OnTierMappingLoadedL(NULL, TWorkerThreadPublicInfo::ENullWorkerId));	// without the resolver's help all we can do is fail the tier requests
		}
	}

void CPitBoss::PopulateAndAddProtocolPairListL(TProtocolName& tierDesc, RTierThreadMap* map)
	{
	TProtocolPairingOwner pairList;
	CleanupReleasePushL(pairList);

	for(TInt i = map->Count() - 1; i >=0; --i)
		{
		const TTierMapEntry& entry = (*map)[i];

#ifdef _DEBUG
		tierDesc.Format(_L("#!#! Tier %08x"), entry.iUid);	// make protocol pairing record self-descriptive for debugging
#endif
		AddTierPairingToListL(entry.iUid, tierDesc, entry.iWorker, pairList);
		}
	IncorporateProtocolListL(pairList);

	CleanupStack::Pop(&pairList);
	}

void CPitBoss::OnTierMappingLoadedL(const TWorkerTierMappingsLoaded* aMappingMsg, TWorkerId aSenderId)
	{
	__ASSERT_DEBUG(iLoadTierMappingPhase == EResolverRequested, User::Panic(KSpecAssert_ESockSSocks_rls, 19));
	TProtocolName tierDesc;

	LOG(TInt numAdded = (aMappingMsg && aMappingMsg->TierMap())? aMappingMsg->TierMap()->Count(): 0);
	if (aMappingMsg && aMappingMsg->TierMap())
		{
		RTierThreadMap* map = const_cast<RTierThreadMap*>(aMappingMsg->TierMap());

		TRAPD(err, PopulateAndAddProtocolPairListL(tierDesc, map));

		// THeapSwitcher is not leave-safe and must be destroyed by going out of scope only.
		// Our leave implementation does not guarantee that stack objects will be destroyed
		// during a leave and although the current ARM implementation does do this because it
		// uses the C++ exception handling mechanism internally, x86gcc does not support this.
			{
			THeapSwitcher switcher(*this, aSenderId);

			map->Close();
			delete map;
			}

		if (err)
			{
			User::Leave(err);
			}
		}

	iLoadTierMappingPhase = EComplete;

	LOG(ESockLog::Printf(_L8("CPitBoss::OnTierMappingLoadedL() - %d tier entries added"), numAdded));
	// And tell all of the workers that tier mappings are ready for digestion
	BroadcastConfigurationComplete(ETierMapping);
	}

