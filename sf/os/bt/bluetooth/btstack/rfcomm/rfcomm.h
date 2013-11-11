// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef RFCOMM_H
#define RFCOMM_H

#include <es_prot.h>

#include <bttypes.h>
#include "secman.h"
#include "bt.h"
#include <es_prot_internal.h>

const TUint KRFCOMMStreamServiceInfo = 
  KSIReliable|KSIInOrder|KSIStreamBased|KSIGracefulClose|KSICanReconnect;

class CBTProtocolFamily;
class CRfcommMuxer;
class CMuxChannelStateFactory;
class CRfcommStateFactory;
class CRfcommFlowStrategyFactory;
class CRfcommPortAllocator;
class CRfcommSAP;
class TBTSockAddr;
class CRfcommIncomingConnectionListener;

/**
   The RFCOMM Protocol class.
   
   This is the singleton object created by ESOCK to provide access to all
   the RFCOMM services.
*/

NONSHARABLE_CLASS(CRfcommProtocol) : public CBluetoothProtocolBase
	{
public:
	// Create ourselves
	static CRfcommProtocol* NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	
	// From CProtocolBase
	// Factories
	CServProviderBase* NewSAPL(TUint aSockType);
	
	void Close();
	void Open();
	void CloseNow();

	// From ProtocolManager before all binding.
	void InitL(TDesC &aTag);
	// From Protocol Manager - after all binding
	void StartL();	
	// From Protocol Manager
	void BindToL(CProtocolBase* protocol);

	// Query functions
	void Identify(TServerProtocolDesc* aProtocolDesc)const;	
	static void ProtocolIdentity(TServerProtocolDesc* aDesc);	

	// Rfcomm specific functions
	CRfcommStateFactory* StateFactory() const;
	CRfcommFlowStrategyFactory* FlowStrategyFactory() const;

	// Get our local address
	const TBTDevAddr& LocalBTAddr() const;

	//	Two flavours of GetMux to provide pointers to CRfcommMuxers on demand.

	//	First function - called when attempting to create an outgoing connection
	TInt GetMux(TBTSockAddr& aRemoteAddr, CRfcommMuxer*& aMuxer);

	// From BluetoothProtocolBase
	virtual TInt BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSSP);

	void MuxDown(CRfcommMuxer& aMux);

	// Functions to manipulate and query the list of listening RFCOMM SAPs
	void AddIdleSAP(CRfcommSAP& aSAP);
	void RemoveIdleSAP(CRfcommSAP& aSAP);
	CRfcommSAP* FindIdleSAP(const TBTSockAddr& aSockAddr);
	TInt FindFreeServerChannel();

	void DisallowCBFC();
	void AllowCBFC();
	TBool CBFCDisallowed();

	void AddBoundSAP(CRfcommSAP& aSAP);
	void RemoveBoundSAP(CRfcommSAP& aSAP);
	CRfcommSAP* FindBoundSAP(const TBTSockAddr& aSockAddr);
	TBool SAPIsBound(const CRfcommSAP& aSAP);

	CRfcommSAP* FindInboundConnectedSAP(const TBTSockAddr& aSockAddr);

	void SetL2CapConfig(TL2CapConfigPkg& aConfigPkg);

private:
	// Functions
	CRfcommProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	void ConstructL();
	~CRfcommProtocol();

	CRfcommMuxer* FindMux(const TBTDevAddr& aAddr);
	TBool CheckForClose();
	static TInt TryToClose(TAny* aProtocol);
	void RemoveIdleTimerEntry();
	void QueIdleTimerEntry();
	
	//	Called when responding to an incoming connection request
	TInt GetMux(const TBTDevAddr& aRemoteAddr, CServProviderBase* aL2CAPConSAP, CRfcommMuxer*& aMuxer);
	virtual TInt StartProtocolListening();

private:
	// Data members
	CRfcommStateFactory*		iStateFactory;
	CRfcommFlowStrategyFactory*	iFlowStrategyFactory;
	CMuxChannelStateFactory*	iMuxChannelStateFactory;

	TBool   iClosePending;
	TDblQue<CRfcommMuxer> iMuxes;   // All the muxes
	TDblQue<CRfcommSAP> iListeningSAPs;	// SAPS which are listening
	TDblQue<CRfcommSAP> iBoundSAPs;		// SAPS which are bound
	TDeltaTimerEntry  iIdleTimerEntry;
	TBool             iIdleEntryQueued;
	TBool			  iCBFCDisallowed; //"Etrue" => a SetOpt has switched CBFC off
	};


#endif
