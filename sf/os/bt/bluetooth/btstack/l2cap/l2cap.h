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
// Defines the l2cap protocol class
// 
//

#ifndef L2CAP_H
#define L2CAP_H

#include <es_prot.h>
#include <bt_sock.h>
#include "secman.h"
#include "notification.h"
#include "bt.h"
#include "l2capMuxController.h"
#include <es_prot_internal.h>
#include <bluetooth/lmoptions.h>

// Stream and Datagram I/f's are mutually exclusive. You can't have some sockets one
// time and some the other -- esock will always give precedence to stream.
const TUint KL2CAPSeqPacketServiceInfo = 
  KSIReliable|KSIInOrder|KSIDatagram|KSIGracefulClose|KSIBroadcast|KSIQOS|KSICanReconnect;

class CL2CAPSAPStateFactory;
class CL2CAPSignalStateFactory;

NONSHARABLE_CLASS(CL2CAPProtocol) : public CBluetoothProtocolBase
	{
public:
	// Create ourselves
	static CL2CAPProtocol* NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	
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
	// From higher protocol
	void BindL(CProtocolBase* aProtocol, TUint aId);
	void BindToL(CProtocolBase* aProtocol);	// From Protocol Manager

	// Query functions
	void Identify(TServerProtocolDesc* aDesc)const;	
	static void ProtocolIdentity(TServerProtocolDesc* aDesc);	

	// L2CAP specific functions
	CL2CAPSAPStateFactory& StateFactory() const;  
	CL2CAPSignalStateFactory& SigStateFactory() const;  

	// Get our local address
	const TBTDevAddr& LocalBTAddr() const;

	virtual TInt BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSAP);

	// Interface for the L2CAP Mux
	CL2CAPMuxController& MuxController() const;
	
	// A queue of resources has been reduced to zero so try to remove stack
	void TryToClose();

private:
    // Functions	
	CL2CAPProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	~CL2CAPProtocol();
	void ConstructL();

	void QueIdleTimerEntry();
	void RemoveIdleTimerEntry();

	static TInt TryToClose(TAny* aProtocol);
	// Start Listening
	virtual TInt StartProtocolListening();
	
	// Data members
	// Singleton objects
	CL2CAPSAPStateFactory*		iStateFactory;
	CL2CAPSignalStateFactory*	iSigStateFactory;
	
	TDeltaTimerEntry			iIdleTimerEntry;
	TBool						iIdleEntryQueued;
	
	TBool                       iClosePending;
	
	CL2CAPMuxController* 		iMuxController;
#ifdef _DEBUG
public:
	TUint	iDebugOptionMask;
#endif
	};

inline CL2CAPMuxController& CL2CAPProtocol::MuxController() const
	{
	return *iMuxController;
	}

#endif
