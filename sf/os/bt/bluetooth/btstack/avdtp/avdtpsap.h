// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of AVDTP SAP
// The SAP is really only a conduit to the socket
// The sessions implement the meaningful protocol stuff
// 
//

/**
 @file
 @internalComponent
*/


#ifndef AVDTPSAP_H
#define AVDTPSAP_H

#include <bluetoothav.h>
#include "BtSap.h"
#include "avdtpTransportSession.h"

// could have v1 v2 versions of this for MT ESOCK?

NONSHARABLE_CLASS(HQueuedAvdtpIndication)
	{
public:
	static HQueuedAvdtpIndication* New(const TDesC8& aIndicationData);
	inline TDesC8* Indication() const;
	~HQueuedAvdtpIndication();
public:
	TSglQueLink	iLink;
private:
    void ConstructL(const TDesC8& aIndicationData);
private:
    HBufC8* iBuf;
	};

inline TDesC8* HQueuedAvdtpIndication::Indication() const
    {
    return iBuf;
    }
    

NONSHARABLE_CLASS(CAvdtpSAP) : public CBluetoothSAP
/**

*/
	{
friend class CSignallingChannel; // for quing
friend class CAvdtpProtocol; // for quing
public:
	//SAP stuff
	static CAvdtpSAP* NewL(CAvdtpProtocol& aProtocol);
	~CAvdtpSAP();

	virtual TInt SetLocalName(TSockAddr& aAddr); // defines which TransportSession will be created
	virtual void LocalName(TSockAddr& anAddr) const;
	virtual void RemName(TSockAddr& anAddr) const;
	virtual TInt SetRemName(TSockAddr& aAddr);
	virtual TInt GetOption(TUint aLevel,TUint aName,TDes8& aOption) const;
	virtual void Ioctl(TUint aLevel,TUint aName, TDes8* aOption);
	virtual void CancelIoctl(TUint aLevel,TUint aName);
	virtual TInt SAPSetOption(TUint aLevel,TUint aName,const TDesC8 &aOption);
	virtual void ActiveOpen();
	virtual void ActiveOpen(const TDesC8& aConnectionData);
	virtual TInt PassiveOpen(TUint aQueSize);
	virtual TInt PassiveOpen(TUint aQueSize,const TDesC8& aConnectionData);
	virtual void Shutdown(TCloseType aOption);
	virtual void Shutdown(TCloseType aOption, const TDesC8& aDisconnectionData);
	virtual void AutoBind();
	virtual void Start();

	virtual TInt Write(RMBufChain& aData, TUint aOptions, TSockAddr* aAddr);
	virtual TInt GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* aAddr);
	
	const TAvdtpSockAddr& SocketAddress() const;

	// interface for sessions
	void Ready();
	void SessionDisconnect();
	void Error(TInt aError);
	void SendError(TInt aError);
	void PacketReceived();
	void CanSend();
	void ServiceComplete(const TDesC8* aBuf);
	void Indication(const TDesC8& aIndicationData);
	void NewData(TUint aCount);
	void CanClose();
	inline void DetachSession();
private:
	CAvdtpSAP(CAvdtpProtocol& aProtocol);

	void ConstructL();
	void UpdateSessionL(const TAvdtpSockAddr& aAddr);
	void DoIndication(const TDesC8& aIndicationData);

	inline void ClearIoctl();
private:
	CTransportSession*	iSession;
	CAvdtpProtocol&		iProtocol;
	TBool				iSocketBlocked;
	TSglQueLink			iLink; // a couple of classes use this depending on its session 'type'
	CAvdtpSAP*			iSecondarySAP; // non-owned (SAPs owned by socket)
	TSglQue<HQueuedAvdtpIndication>	iIndicationQueue;
	TBool				iIsSecondary; // don'twant secondaries creating secondaries!
#ifdef _DEBUG
	TBool				iCanSignalDisconnect; // don't want to tell esock disconnect if !connected. copes in urel.
#endif
	};

inline void CAvdtpSAP::DetachSession()
	{
	iSession = NULL;
	}

#endif //AVDTPSAP_H
