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
//

#ifndef SDP_H
#define SDP_H

#include <es_prot.h>
#include <bttypes.h>

#include "secman.h"
#include "btsockettimer.h"
#include <es_prot_internal.h>

class CBTProtocolFamily;
class CSdpClient;
class CSdpNetDbProvider;
class CServProviderBase;

/**
	The SDP Protocol class.

	This is the singleton object created by ESOCK to provide access to all
	the SDP NetDb service.
	Currently supports only client mode -- i.e. query but not register.
**/

NONSHARABLE_CLASS(CSdpProtocol) : public CBluetoothProtocolBase
	{
public:
	// Create ourselves
	static CSdpProtocol* NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	
	// From CProtocolBase
	// Factories
	CNetDBProvdBase* NewNetDatabaseL();
	
	void Close();
	void Open();
	void CloseNow();

	// From ProtocolManager before all binding.
	void InitL(TDesC &aTag);
	// From Protocol Manager - after all binding
	void StartL();	
	// From Protocol Manager
	void BindToL(CProtocolBase* aProtocol);

	virtual TInt BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSSP);

	// Query functions
	void Identify(TServerProtocolDesc* aProtocolDesc)const;	
	static void ProtocolIdentity(TServerProtocolDesc* aDesc);	

	// Sdp specific functions
	void GetClient(CSdpNetDbProvider& aNetDbProvider);
	void ClientDown(CSdpClient& aClient);

private:
	// Functions
	CSdpProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	~CSdpProtocol();

	CSdpClient* FindClient(const TBTDevAddr& aAddr);
	void TryToClose();

private:
	// Data members
	TBool				iClosePending;
	TDblQue<CSdpClient> iClients;   // All the Clients
	};


#endif
