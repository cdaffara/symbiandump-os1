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
 @file SS_SAP.CPP
*/

#define SYMBIAN_NETWORKING_UPS

#include <ss_std.h>
#include <ss_glob.h>
#include <comms-infras/ss_roles.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_subconnflow.h>
#include <ss_protprov.h>
#include <comms-infras/es_sap.h>
#include <ss_sock.h>
#include <comms-infras/ss_nodemessages_dataclient.h>

#include <elements/nm_signatures.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockss_sp, "ESockSSockss_sp.");
#endif

using namespace ESock;
using namespace Messages;


EXPORT_C CNetworkFlow::CNetworkFlow(CSubConnectionFlowFactoryBase& aFactory, const TNodeId& aSubConnId, CProtocolIntfBase* aProtocolIntf)
	: CSubConnectionFlowBase(aFactory, aSubConnId, aProtocolIntf)
	{
	}

EXPORT_C CNetworkFlow::~CNetworkFlow()
	{
	if (iLowerFlow)
		{
		iLowerFlow->Unbind(NULL,NULL);
		}
	}
void
CNetworkFlow::SetLocalName(TSockAddr& anAddr)
/** Sets the local name (address) of the socket service provider entity. The format
of the data in the TSockAddr object is defined by individual protocols.

@param anAddr The address
@return Returns KErrNone if the local name is correctly set or, if this is
not the case, an informative error number. */
	{
	SetLocalAddressSet();
	iLocalAddress = anAddr;
	
	}

TInt CNetworkFlow::SetRemName(TSockAddr& anAddr)
/** Sets the remote name (address) of the socket service provider entity. The format
of the data in the TSockAddr object is defined by individual protocols.

@param anAddr The address
@return Returns KErrNone if the remote name is correctly set or, if this is
not the case, an informative error number. */
	{
	SetRemoteAddressSet();
	iRemoteAddress = anAddr;
	return KErrNone;
	}

void CNetworkFlow::UpdateDestinationAddress(const TSockAddr& aDest)
	{
	iRemoteAddress = aDest;
	SetRemoteAddressSet();
	}

TUint CNetworkFlow::Write(const TDesC8& /*aDesc*/, TUint /*aOptions*/, TSockAddr* anAddr)
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
	if (anAddr)
		{
		UpdateDestinationAddress(*anAddr);
		}
	return KErrNone;
	}

TInt CNetworkFlow::Write(RMBufChain& /*aData*/, TUint /*aOptions*/, TSockAddr* anAddr)
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
	if (anAddr)
		{
		UpdateDestinationAddress(*anAddr);
		}
	return KErrNone;
	}



void CNetworkFlow::BindToL(const TCFDataClient::TBindTo& aBindTo)
/**
Request from control side (at network layer) to indicate that the SubConnection is
up and running and that we should bind to a Flow below.

@param aLowerFlow Flow below to bind to.
*/
	{
	LOG( ESockLog::Printf(_L("CNetworkFlow %08x:\tBindTo()"), this) );

	const TNodeId& commsId = aBindTo.iNodeId;
#if defined(__GCCXML__)
    CSubConnectionFlowBase* flow = reinterpret_cast<CSubConnectionFlowBase*>(reinterpret_cast<Messages::ANode*>(commsId.Ptr()));
#else
    CSubConnectionFlowBase* flow = mcfnode_cast<CSubConnectionFlowBase>(reinterpret_cast<Messages::ANode*>(commsId.Ptr()));
#endif

    if (flow==NULL)
        {
        __ASSERT_DEBUG(!commsId.IsNull(), User::Panic(KSpecAssert_ESockSSockss_sp, 1));
        iSubConnectionProvider.PostMessage(Id(), TCFDataClient::TStopped(KErrDisconnected).CRef());
        }

    //flows can only be directly bound when running in the same thread
    //Are we in the same thread?
    __ASSERT_DEBUG(commsId.Thread() == Id().Thread(), User::Panic(KSpecAssert_ESockSSockss_sp, 2));

	if (iLowerFlow && iLowerFlow->Flow() != flow )
    	{
    	//already bound -> unbind first.
    	iLowerFlow->Unbind(NULL,NULL);
    	iLowerFlow = NULL;
    	iLowerControl = NULL;
    	}
    	
    if (iLowerFlow == NULL && flow != NULL)
        {
        iLowerFlow = flow->GetBinderControlL();
    	iLowerControl = iLowerFlow->GetControlL(KNullDesC8);
    	iLowerFlow->BindL(KNullDesC8, NULL,NULL);
        }
	}

MSessionControl* CNetworkFlow::GetControlL(TInt /*aSessionType*/,MSessionControlNotify& aSessionControlNotify)
    {
	__ASSERT_DEBUG(iSessionControlNotify == NULL, User::Panic(KSpecAssert_ESockSSockss_sp, 3));
	iSessionControlNotify = &aSessionControlNotify;
	return this;
    }

MSessionData* CNetworkFlow::BindL(MSessionDataNotify& aNotify)
    {
	__ASSERT_DEBUG(iSessionDataNotify == NULL, User::Panic(KSpecAssert_ESockSSockss_sp, 4));
	iSessionDataNotify = &aNotify;
	iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TActive().CRef());
	return this;
    }

void CNetworkFlow::Unbind()
    {
    if (iSessionDataNotify)			// if not already unbound
    	{
    	iSessionControlNotify = NULL;
		iSessionDataNotify = NULL;
		if(iSubConnectionProvider.IsOpen())
			{
			if (!Idle())
				{
				SetIdle();
		    	ProcessDCIdleState();
		    	if(!IdleSent())
		    		{
					LOG( ESockLog::Printf(_L("CNetworkFlow %08x:\tUnbind() - idle not sent"), this) );
		    		}
				}
			}
		else
			{
			// Legacy flows lack a SCPR to look after their destruction so need to suicide
			delete this;
			}
    	}
    }

CSubConnectionFlowBase* CNetworkFlow::Flow()
    {
    return this;
    }

MFlowBinderControl* CNetworkFlow::DoGetBinderControlL()
	{
	return this;
	}

void CNetworkFlow::ProcessDCIdleState()
	{
#ifdef SYMBIAN_NETWORKING_UPS
	if (Idle() && !IdleSent() && !ActivityRunning())
#else
	if (Idle() && !IdleSent() && !NoBearerGuard())
#endif
		{
		if(Started())
			{
			iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TDataClientGoneDown(KErrConnectionTerminated).CRef());
			}
		SetIdleSent();
		iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TIdle().CRef());
		}
	}



