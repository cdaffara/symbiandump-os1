// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the SDP Protocol object
// 
//

#include <bluetooth/logger.h>
#include <bt_sock.h>
#include <btsdp.h>
#include "sdp.h"
#include "sdpclient.h"
#include "sdpnetdb.h"
#include "sdpstackutil.h"
#include "sdpconsts.h"
#include "bt.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDP);
#endif

CSdpProtocol::CSdpProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
	:CBluetoothProtocolBase(aSecMan, aControlPlane, aCodMan),
	iClients(_FOFF(CSdpClient, iLink))
/** 
    Protocol Constructor.
**/
	{
	LOG_FUNC
	}

CSdpProtocol::~CSdpProtocol()
/** 
    Protocol Destructor.
    We only clean up the signallers as the NetDbProviders are owned by ESOCK and
    it is up to it to destroy them.
**/
	{
	LOG_FUNC
	while(!iClients.IsEmpty())
		{
		CSdpClient* c = iClients.First();
		c->iLink.Deque();
		delete c;
		}
	if(iLowerProtocol)
		iLowerProtocol->Close();  // Matches the bind
#ifdef __FLOG_ACTIVE
	CLOSE_LOGGER
#endif
	}

CSdpProtocol* CSdpProtocol::NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
	{
#ifdef __FLOG_ACTIVE
	CONNECT_LOGGER
#endif
	LOG_STATIC_FUNC
	CSdpProtocol* i=new (ELeave) CSdpProtocol(aSecMan, aControlPlane, aCodMan);
	return i;
	}

void CSdpProtocol::InitL(TDesC& /*aTag*/)
/**
	Pre-binding initialise.
	Alloc any stuff we need.

	This will only ever be called once during the lifetime of this
	protocol.
**/
	{
	LOG_FUNC
	// ...nothing required!
	}

void CSdpProtocol::StartL()
/**
	Binding complete.
**/
	{
	LOG_FUNC
	  // Should check that we're bound now.
	__ASSERT_ALWAYS(iLowerProtocol,Panic(ESdpNotBound));
	}


void CSdpProtocol::BindToL(CProtocolBase* aProtocol)
/***
    Request by Protocol Mgr to bind to the specified protocol.
    We can only be bound to one lower layer protocol, so the function panics
    if we are already bound.
    @param aProtocol The protocol we need to bind to.
**/
	{
	LOG_FUNC
	FTRACE(TBuf<255> tmp(aProtocol->Tag()); 
		   LOG1(_L("CSdpProtocol::BindToL binding to %S"),
				  &tmp));
	
	if(!iLowerProtocol)
		{
#ifdef _DEBUG
		TServerProtocolDesc prtDesc;
		aProtocol->Identify(&prtDesc);

		if(prtDesc.iAddrFamily!=KBTAddrFamily ||
		   prtDesc.iProtocol!=KL2CAP)
			{
			User::Leave(KErrBtEskError);
			}
#endif

		iLowerProtocol=static_cast<CBluetoothProtocolBase*>(aProtocol);
		iLowerProtocol->BindL(this, KSDPPSM);
		iLowerProtocol->Open();
		}
	else
		{
	    User::Leave(KErrSdpAlreadyBound);
		}
	}

// Factory functions

CNetDBProvdBase* CSdpProtocol::NewNetDatabaseL()
/** 
	Create a new NetDbProvider.
	The NetDbProvider returned is owned by the caller -- this protocol will
	not clean it up.  esock uses this function to create a new NetDbProvider,
	and esock will delete when it is finished with it.
**/
	{
	LOG_FUNC
	return CSdpNetDbProvider::NewL(*this);
	}

// Query functions

void CSdpProtocol::Identify(TServerProtocolDesc *aDesc)const
/**
	Identify request from SOCKET server
**/
	{
	LOG_FUNC
	ProtocolIdentity(aDesc);
	}

void CSdpProtocol::ProtocolIdentity(TServerProtocolDesc* aDesc)
	{
	LOG_STATIC_FUNC
	_LIT(name,"SDP");
	aDesc->iProtocol=KSDP;

	aDesc->iName=name;
	aDesc->iAddrFamily=KBTAddrFamily;
	aDesc->iSockType=1;	// Bogus... but (debug) esock demands non-zero here.
	
	aDesc->iVersion=TVersion(KBTMajor,KBTMinor,KBTBuild);
	aDesc->iByteOrder=EBigEndian;
	aDesc->iServiceInfo=0;
	aDesc->iNamingServices=KNSInfoDatabase;
	aDesc->iSecurity=0;
	aDesc->iMessageSize=KSocketMessageSizeUndefined;
	aDesc->iServiceTypeInfo=ECantProcessMBufChains;
	aDesc->iNumSockets=1;// Bogus... but (debug) esock demands non-zero here.
	}

void CSdpProtocol::CloseNow()
/**
   Our reference is now zero, so start to close.
   When SDP server is in place, this'll be far more involved, as we'll
   want to hang around to service SDP queries, for some (???) length
   of time.
   In the mean time, we can just close, as there are now no netdbs left, so
   even if any SDP clients are still hanging around, they may as well die.
**/
	{
	LOG_FUNC
	iClosePending = ETrue;
	TryToClose();
	}

void CSdpProtocol::Open()
/**
   Request to open the protocol.
   The protocol may be repeatedly opened and closed.  The order of calls is
   InitL, [Open *n , Close * n,  CloseNow] * m, CanClose(upcall) etc.
**/
     {
	LOG_FUNC
	 iClosePending = EFalse;
	 CProtocolBase::Open();
     }
     
void CSdpProtocol::Close()
/**
   This is one session closing.
   Just call default, which decs the ref count.
   CloseNow will be called when this hits 0.
**/
	{
	LOG_FUNC
	CProtocolBase::Close();
	}

void CSdpProtocol::TryToClose()
 	{
	LOG_FUNC
 	if (iClosePending && iClients.IsEmpty())
 		CanClose();	// delete's us!
 	}

void CSdpProtocol::GetClient(CSdpNetDbProvider& aNetDbProvider)
/**
    Find a Client for this NetDbProvider.

    When a Client is found, the NetDbProvider is notified via the ClientUp() call.
    Note that if there is no existing Client then it will be created
    here and the link brought up before the ClientUp() call is made.  This implies
    that the ClientUp() call on the NetDbProvider can be synchronous or Asynchronous.
    
    @param aNetDbProvider  The NetDbProvider that needs a Client.
**/
	{
	LOG_FUNC
	CSdpClient* client;
	TBool needsL2CAPChannel = EFalse;

	client=FindClient(aNetDbProvider.iRemoteDev);
	if(!client)
		{// Need to create one
		needsL2CAPChannel = ETrue;
		TRAPD(err, client=CSdpClient::NewL(*this, *iLowerProtocol));
		if(err != KErrNone)
			{
			// Can't create a client, so error the NetDbProvider
			aNetDbProvider.Error(err);
			return;
			}
		// Add client to the Q
		iClients.AddFirst(*client);
		}	
	// Take NetDb off the idle Q

	// and add it to the client
	aNetDbProvider.iClient=client;
	client->AddNetDbProvider(aNetDbProvider);

	// Bring up the client if needed
	if (needsL2CAPChannel)
	client->Open(aNetDbProvider.iRemoteDev);  // Will call back through ClientUp()
	}

CSdpClient* CSdpProtocol::FindClient(const TBTDevAddr& aAddr)
/**
	Find the existing client for this address.
		@return   Pointer to a client for this address, or NULL if it doesn't exist.
**/	
	{
	LOG_FUNC
	TDblQueIter<CSdpClient> iter(iClients);
	CSdpClient* client;

	while(iter)
		{
		client=iter++;
		if(client->iRemoteAddr == aAddr)
			{
			// We have a client, so return it
			return client;
			}
		}
	// No client
	return 0;
	}

void CSdpProtocol::ClientDown(CSdpClient& aClient)
	{
	LOG_FUNC
	aClient.iLink.Deque();
	delete &aClient;
	TryToClose();
	}

TInt CSdpProtocol::BearerConnectComplete(const TBTDevAddr& /*aAddr*/, CServProviderBase* /*aSSP*/)
	{
	LOG_FUNC
	// SDP Protocol doesn't do incoming stuff!
	Panic(ESdpClientPassiveConnect);
	return KErrNotSupported;
	}

