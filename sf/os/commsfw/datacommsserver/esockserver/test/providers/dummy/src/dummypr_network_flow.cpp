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
// Dummy implementation file for network tier flow
//
//

/**
 @file
 @internalComponent
*/

#include <ss_std.h>
#include <comms-infras/ss_log.h>
#include <ss_pman.h>
#include "dummypr_network_flow.h"
#include <comms-infras/ss_sapshim.h>
#include <comms-infras/ss_nodemessages_internal_esock.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestdmyprnt, "ESockTestdmyprnt");
#endif

using namespace ESock;
using namespace Messages;

#if defined(_DEBUG)
void ESockLog::Printf(TRefByValue<const TDesC> /*aFmt*/,...) {}
void ESockLog::Printf(TRefByValue<const TDesC8> /*aFmt*/,...) {}
void ESockLog::Printf(const TDesC8& /*aSubTag*/, TRefByValue<const TDesC8> /*aFmt*/, ...) {}
void ESockLog::Printf(const TDesC8& /*aSubTag*/, TRefByValue<const TDesC> /*aFmt*/, ...) {}
void ESockLog::Printf(const TDesC8& /*aMajorTag*/, const TDesC8& /*aSubTag*/, TRefByValue<const TDesC> /*aFmt*/, VA_LIST& /*aList*/) {}
#endif



// Construction
//-------------
CDummyNetworkFlow* CDummyNetworkFlow::NewL(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf)
	{
	CDummyNetworkFlow* self = new (ELeave) CDummyNetworkFlow(aFactory, aSubConn, aProtocolIntf);
	return self;
	}

CDummyNetworkFlow::CDummyNetworkFlow(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf)
: CSubConnectionFlowBase(aFactory, aSubConn, aProtocolIntf)
	{
    LOG_NODE_CREATE(KESockFlowTag, CDummyNetworkFlow)
	}

CDummyNetworkFlow::~CDummyNetworkFlow()
	{
	TEBase::TError err(
		TEBase::TNull::Id(),
		KErrBindersInvalid);

    SubConnectionError(err, EErrorAllOperations);
    LOG_NODE_DESTROY(KESockFlowTag, CDummyNetworkFlow)
	}

MSessionControl* CDummyNetworkFlow::GetControlL(
	TInt /*aSessionType*/,
	MSessionControlNotify& aSessionControlNotify)
    {
	// Apply binding locally
	__ASSERT_DEBUG(iSessionControlNotify == NULL, User::Panic(KSpecAssert_ESockTestdmyprnt, 1));
	iSessionControlNotify = &aSessionControlNotify;

    // We should already be bound to the transport shim flow below
    __ASSERT_DEBUG(iLowerFlow, User::Panic(KSpecAssert_ESockTestdmyprnt, 2));
	return this;
    }

MSessionData* CDummyNetworkFlow::BindL(MSessionDataNotify& aNotify)
    {
	LOG( ESockLog::Printf(_L8("CDummyNetworkFlow %08x:\tBindL(aNotify 0x%08x)"), this, &aNotify) );

	// Apply binding locally
	__ASSERT_DEBUG(iSessionDataNotify == NULL, User::Panic(KSpecAssert_ESockTestdmyprnt, 3));
	iSessionDataNotify = &aNotify;
	iSubConnectionProvider.RNodeInterface::PostMessage(
		Id(),
		TCFControlProvider::TActive().CRef());

    // We should already be bound to the transport shim flow below
	__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 4));
	return this;
    }

void CDummyNetworkFlow::Unbind()
    {
	LOG( ESockLog::Printf(_L8("CDummyNetworkFlow %08x:\tUnbind()"), this) );

    // Forward to the flow shim beneath us
	iLowerFlow->Unbind();

    // And the local unbinding
    if (iSessionDataNotify)
    	{
    	iSessionControlNotify = NULL;
		iSessionDataNotify = NULL;

		__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockTestdmyprnt, 5));
		if(iDCIdle < EIdle)
			{
			iDCIdle = EIdle;
			ProcessDCIdleState();
			}
    	}
    }

ESock::CSubConnectionFlowBase& CDummyNetworkFlow::CloneFlowL()
    {
    TDefaultFlowFactoryQuery query (iProtocolIntf->ControlProviderId(), iSubConnectionProvider.RecipientId());

	ESock::CSubConnectionFlowBase& flow = static_cast<ESock::CSubConnectionFlowBase&>(*(Factory().CreateObjectL(query)));

	// Have the new flow become a data client of our subconnection too
	__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockTestdmyprnt, 6));
	TCFPeer::TJoinRequest msg(
		flow.Id(),
		TCFClientType(TCFClientType::EData, TCFClientType::EActive)
		);

	iSubConnectionProvider.PostMessage(Id(), msg);

	return flow;
    }

CSubConnectionFlowBase* CDummyNetworkFlow::Flow()
	{
	return this;
	}


// MSessionControl
//----------------
void CDummyNetworkFlow::Start()
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 7)); return iFlowShim->Start();}

void CDummyNetworkFlow::LocalName(TSockAddr& anAddr) const
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 8)); return iFlowShim->LocalName(anAddr);}
void CDummyNetworkFlow::SetLocalName(TSockAddr& anAddr)
	{
	__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 54));
	iFlowShim->SetLocalName(anAddr);
	}

void CDummyNetworkFlow::RemName(TSockAddr& anAddr) const
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 10)); iFlowShim->RemName(anAddr);}

TInt CDummyNetworkFlow::SetRemName(TSockAddr& anAddr)
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 11)); return iFlowShim->SetRemName(anAddr);}

const TInt KDummyNetworkFlowOptionLevel = CDummyFlowFactory::iUid;
const TInt KFlowProtocolName = 1;

TInt CDummyNetworkFlow::GetOption(TUint aLevel, TUint aName, TDes8& anOption) const
	{
	__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 12));

	if(aLevel == KDummyNetworkFlowOptionLevel && aName == KFlowProtocolName)
		{
		anOption.Copy(KDummyProtocolName);
		return KErrNone;
		}
	else
		{
		return iFlowShim->GetOption(aLevel, aName, anOption);
		}
	}

void CDummyNetworkFlow::Ioctl(TUint aLevel, TUint aName, TDes8* anOption)
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 13)); iFlowShim->Ioctl(aLevel, aName, anOption);}

void CDummyNetworkFlow::CancelIoctl(TUint aLevel, TUint aName)
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 14)); iFlowShim->CancelIoctl(aLevel, aName);}

TInt CDummyNetworkFlow::SetOption(TUint aLevel, TUint aName, const TDesC8 &anOption)
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 15)); return iFlowShim->SetOption(aLevel, aName, anOption);}

void CDummyNetworkFlow::ActiveOpen()
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 16)); iFlowShim->ActiveOpen();}

void CDummyNetworkFlow::ActiveOpen(const TDesC8& aConnectionData)
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 17)); iFlowShim->ActiveOpen(aConnectionData);}

TInt CDummyNetworkFlow::PassiveOpen(TUint aQueueSize)
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 18)); return iFlowShim->PassiveOpen(aQueueSize);}

TInt CDummyNetworkFlow::PassiveOpen(TUint aQueueSize, const TDesC8& aConnectionData)
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 19)); return iFlowShim->PassiveOpen(aQueueSize, aConnectionData);}

void CDummyNetworkFlow::Shutdown(MSessionControl::TCloseType aOption)
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 20)); iFlowShim->Shutdown(aOption);}

void CDummyNetworkFlow::Shutdown(MSessionControl::TCloseType aOption, const TDesC8& aDisconnectionData)
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 21)); iFlowShim->Shutdown(aOption, aDisconnectionData);}

void CDummyNetworkFlow::AutoBind()
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 22));	iFlowShim->AutoBind();}

TInt CDummyNetworkFlow::SecurityCheck(MProvdSecurityChecker* aChecker)
	{__ASSERT_DEBUG(iFlowShim, User::Panic(KSpecAssert_ESockTestdmyprnt, 23));	return iFlowShim->SecurityCheck(aChecker);}


// MSessionData
//-------------
TUint CDummyNetworkFlow::Write(const TDesC8& aDesc, TUint aOptions, TSockAddr* anAddr)
	{__ASSERT_DEBUG(iFlowShimData, User::Panic(KSpecAssert_ESockTestdmyprnt, 24));	return iFlowShimData->Write(aDesc, aOptions, anAddr);}

TInt CDummyNetworkFlow::Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr)
	{__ASSERT_DEBUG(iFlowShimData, User::Panic(KSpecAssert_ESockTestdmyprnt, 25));	return iFlowShimData->Write(aData, aOptions, anAddr);}

void CDummyNetworkFlow::GetData(TDes8& aDesc, TUint aOptions, TSockAddr* anAddr)
	{__ASSERT_DEBUG(iFlowShimData, User::Panic(KSpecAssert_ESockTestdmyprnt, 26));	iFlowShimData->GetData(aDesc, aOptions, anAddr);}

TInt CDummyNetworkFlow::GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr)
	{__ASSERT_DEBUG(iFlowShimData, User::Panic(KSpecAssert_ESockTestdmyprnt, 27));	return iFlowShimData->GetData(aData, aLength, aOptions, anAddr);}


// MSessionControlNotify
//----------------------
void CDummyNetworkFlow::ConnectComplete()
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 28));	iSessionControlNotify->ConnectComplete();}

void CDummyNetworkFlow::ConnectComplete(const TDesC8& aConnectData)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 29));	iSessionControlNotify->ConnectComplete(aConnectData);}

void CDummyNetworkFlow::ConnectComplete(CSubConnectionFlowBase& anSSP)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 30));	iSessionControlNotify->ConnectComplete(anSSP);}

void CDummyNetworkFlow::ConnectComplete(CSubConnectionFlowBase& anSSP,const TDesC8& aConnectData)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 31));	iSessionControlNotify->ConnectComplete(anSSP, aConnectData);}

void CDummyNetworkFlow::CanClose(MSessionControlNotify::TDelete aDelete)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 32));	iSessionControlNotify->CanClose(aDelete);}

void CDummyNetworkFlow::CanClose(
	const TDesC8& aDisconnectData,
	MSessionControlNotify::TDelete aDelete)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 33));	iSessionControlNotify->CanClose(aDisconnectData, aDelete);}

TInt CDummyNetworkFlow::Error(TInt anError,TUint anOperationMask)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 34));	return iSessionControlNotify->Error(anError, anOperationMask);}

void CDummyNetworkFlow::Disconnect()
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 35));	iSessionControlNotify->Disconnect();}

void CDummyNetworkFlow::Disconnect(TDesC8& aDisconnectData)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 36));	iSessionControlNotify->Disconnect(aDisconnectData);}

void CDummyNetworkFlow::IoctlComplete(TDesC8* aBuf)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 37));	iSessionControlNotify->IoctlComplete(aBuf);}

void CDummyNetworkFlow::DisconnectFromListener(CSubConnectionFlowBase& aSSP)
	{
		__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 53));
        iSessionControlNotify->DisconnectFromListener(aSSP);
    }
void CDummyNetworkFlow::SetLocalNameComplete()
	{
	__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 52));
	iSessionControlNotify->SetLocalNameComplete();
	}

// MSessionDataNotify
//-------------------
void CDummyNetworkFlow::NewData(TUint aCount)
	{__ASSERT_DEBUG(iSessionDataNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 39)); iSessionDataNotify->NewData(aCount);}

void CDummyNetworkFlow::CanSend()
	{__ASSERT_DEBUG(iSessionDataNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 40)); iSessionDataNotify->CanSend();}

MFlowBinderControl* CDummyNetworkFlow::DoGetBinderControlL()
	{
	return this;
	}


// Node stuff
//-----------
void CDummyNetworkFlow::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    CSubConnectionFlowBase::ReceivedL(aSender, aRecipient, aMessage);

	if ( aMessage.IsMessage<TEBase::TError>() )
		{
		SubConnectionError(static_cast<TEBase::TError&>(aMessage));
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
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockTestdmyprnt, 41)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else if (TCFDataClient::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TCFDataClient::TStart::EId :
			StartFlow();
			break;
		case TCFDataClient::TStop::EId :
			StopFlow(static_cast<TCFDataClient::TStop&>(aMessage));
			break;
		case TCFDataClient::TProvisionConfig::EId:
			break;
		case TCFDataClient::TBindTo::EId:
		{
			TCFDataClient::TBindTo& bindToMsg(static_cast<TCFDataClient::TBindTo&>(aMessage));
			TRAPD(err,BindToL(bindToMsg));
			ProcessDCIdleState();	// in case we were waiting to send idle

	        if(err == KErrNone)
	            {
	            RClientInterface::OpenPostMessageClose(Id(), aSender, TCFDataClient::TBindToComplete().CRef());
	            }
	        else
	            {
	            RClientInterface::OpenPostMessageClose(Id(), aSender, TEBase::TError(aMessage.MessageId(), err).CRef());
	            }
	        
			//If we have received TCDDataClient::TStart before (when we did not yet have a bearer),
			//we complete the start here as well
			if (iIsStarting)
				{
				CompleteStart(err);
				}
		}
			break;
		default:
			//TODO - logging
			//LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("ERROR: CTransportFlowShim %08x:\tReceivedL(%s) - KErrNotSupported"), this, aMessage.Printable()));
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockTestdmyprnt, 42)); //For debug configurations
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
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockTestdmyprnt, 43)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else if (TCFInternalEsock::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TCFInternalEsock::TFlowProvision::EId :
			break;
		default:
			//TODO - logging
			//LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("ERROR: CTransportFlowShim %08x:\tReceivedL(%s) - KErrNotSupported"), this, aMessage.Printable()));
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockTestdmyprnt, 44)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else if ( aMessage.IsMessage<TCFPeer::TJoinComplete>() )
		{;}
	else
		{
		//TODO - logging
		//LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("ERROR: CTransportFlowShim %08x:\tReceivedL(%s) - KErrNotSupported"), this, aMessage.Printable()));
    	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockTestdmyprnt, 45)); //For debug configurations
    	User::Leave(KErrNotSupported); //For release configurations
		}
    }

void CDummyNetworkFlow::SubConnectionError(
	const TEBase::TError& errorMsg,
	TUint anOperationMask)
	{
	if (errorMsg.iMsgId == TCFControlProvider::TNoBearer::Id())
		{
		ClearNoBearerGuard();
		}

	if (IsBoundToSession())
	    {
    	Error(errorMsg.iValue, anOperationMask);
	    }
	}

void CDummyNetworkFlow::BindToL(TCFDataClient::TBindTo& aBindTo)
/**
Request from control side (at network layer) to indicate that the SubConnection is
up and running and that we should bind to a Flow below.

@param aLowerFlow Flow below to bind to.
*/
	{
	//provisioning message must come before bindto in case we didn't get it after we've joined
	//the sub-connection
	NM_LOG((KESockServerTag, _L8("CDummyNetworkFlow %08x:\tSynchronous call: From=%08x To=%08x Func=BindToL"),
			this, static_cast<Messages::ANode*>(this), &aBindTo.iNodeId.Node()) )

	ClearNoBearerGuard();

	const TNodeId& commsId = aBindTo.iNodeId;

#if defined(__GCCXML__)
    CSubConnectionFlowBase* flow = reinterpret_cast<CSubConnectionFlowBase*>(&commsId.Node());
#else
	CSubConnectionFlowBase* flow = Messages::mnode_cast<CSubConnectionFlowBase>(&commsId.Node());
#endif

	if (flow == NULL)
		{
		__ASSERT_DEBUG(!commsId.IsNull(), User::Panic(KSpecAssert_ESockTestdmyprnt, 46));
		iSubConnectionProvider.PostMessage(
			Id(),
			TCFDataClient::TStopped(KErrDisconnected).CRef()
			);
		}

	// Flows can only be directly bound when running in the same thread
	__ASSERT_DEBUG(commsId.Thread() == Id().Thread(), User::Panic(KSpecAssert_ESockTestdmyprnt, 47));

	if (iLowerFlow && iLowerFlow->Flow() != flow )
		{
		// Already bound to something else so unbind first
		iLowerFlow->Unbind(NULL,NULL);
		iLowerFlow = NULL;
		iLowerControl = NULL;

		iLowerFlow->Unbind();
		iFlowShim = NULL;
		iFlowShimData = NULL;
		}

	if (iLowerFlow == NULL)
		{
		// Protocol binding
		iLowerFlow = flow->GetBinderControlL();
		iFlowShim = iLowerFlow->GetControlL(KSockDatagram, *this);
		iFlowShimData = iLowerFlow->BindL(*this);
		}
	}


void CDummyNetworkFlow::Rejoin(const TCFFlow::TRejoin& aRejoinMessage)
    {
	LOG( ESockLog::Printf(_L("CDummyNetworkFlow %08x:\tRejoin()"), this ));
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

void CDummyNetworkFlow::StartFlow()
	{
	__ASSERT_DEBUG(!iIsStarted, User::Panic(KSpecAssert_ESockTestdmyprnt, 48));
	__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockTestdmyprnt, 49));	// legacy flows have no control side; should never get here

	if (iDCIdle != EClientsPresent)
		{
		iSubConnectionProvider.PostMessage(Id(), TEBase::TError(TCFDataClient::TStart::Id(), KErrNotReady).CRef());
		return;
		}

	if (iLowerFlow)
		{
		iSubConnectionProvider.PostMessage(Id(), TCFDataClient::TStarted().CRef());
		iIsStarted = ETrue;
		iIsStopped = EFalse;
		return;
		}

	//We need a bearer
	if (!NoBearerGuard())
		{
		PostNoBearer(); //Ask for bearer if not requested already
		}

	//We will wait for it and complete the start after we have received it
	iIsStarting = ETrue;
	}

void CDummyNetworkFlow::StopFlow(TCFDataClient::TStop& aMessage)
	{
	__ASSERT_DEBUG(iIsStarted, User::Panic(KSpecAssert_ESockTestdmyprnt, 50)); //Must be started now
	__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockTestdmyprnt, 51));	// legacy flows have no control side; should never get here

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
	//
	// Note that it is possible for the SCPR to send DataClientStop messages in all circumstances, and
	// the CTransportFlowShim will simply ignore them.  However, this is considered very
	// inefficient as the common case scenario will mean that these StopFlow/FlowDown exchanges
	// sent to every socket on a connection by the SCPR will most of the time be ignored by the
	// CTransportFlowShim and hence obsolete.

	if (IsBoundToSession() && !iUseBearerErrors)
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
	iIsStarted = EFalse;
	iIsStopped = ETrue;
	}

void CDummyNetworkFlow::ProcessDCIdleState()
	{
	if(iDCIdle == EIdle && !iNoBearerRunning)
		{
		iDCIdle = EIdleSent;
		iSubConnectionProvider.RNodeInterface::PostMessage(
			Id(),
			TCFControlProvider::TIdle().CRef()
			);
		}
	}

void CDummyNetworkFlow::CompleteStart(TInt aError)
	{
	if (aError==KErrNone)
		{
		iStartRequest.ReplyTo(Id(), TCFDataClient::TStarted().CRef());
		iIsStarted = ETrue;
		iIsStopped = EFalse;
		}
	else
		{
		iStartRequest.ReplyTo(Id(), TEBase::TError(TCFDataClient::TStart::Id(),aError).CRef());
		}
	iIsStarting = EFalse;
	}

void CDummyNetworkFlow::Destroy()
	{
	DeleteThisFlow();
	}

void CDummyNetworkFlow::PostNoBearer()
	{
	if (!NoBearerGuard())
		{
		iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TNoBearer().CRef());
		SetNoBearerGuard();
		}
	}


