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
// Provides functionality to a layer n protocol to listen on layer n-1
// 
//

#ifndef INCOMINGCONNLISTENER_H
#define INCOMINGCONNLISTENER_H

#include <es_prot.h>

class CBluetoothProtocolBase;
class CBluetoothSAP;

NONSHARABLE_CLASS(CIncomingConnectionListener) : public CBase, public MSocketNotify
	{
public:
	//	NB. NewL takes immediate ownership of aNewSAP
	static CIncomingConnectionListener* NewL(CBluetoothProtocolBase& aProtocol, CServProviderBase* aNewSAP, TUint aPort, TUint aQueSize, TUid aUid);
	static CIncomingConnectionListener* NewL(CBluetoothProtocolBase& aProtocol, CServProviderBase* aNewSAP, const TBTSockAddr& aCompleteSockAddr, TUint aQueSize);
	~CIncomingConnectionListener();

	//  For passing SetOptions down the stack
	TInt SetOption(const TUint aLevel, TUint aName, TDesC8& aOption);
	
	//  For sending Ioctls down the stack
	void Ioctl(const CBluetoothSAP& aSAP, TUint aLevel, TUint aName, TDes8* aOption);
	void CancelIoctl(const CBluetoothSAP& aSAP);

	// for Preauthorising devices - typical use is for combined profiles in one use-case
	// eg headet connects (in unknown order) to AVCTP and AVDTP
	// each protocol uses control plane message to inform the other that a
	// device is preauthorised for remaining connections
	void SetPreauthorisation(TUint aProtocolLevel,
								const TBTDevAddr& aPreauthoriseAddress,
								TBool aSetPreauthorisation);
	TBool IsPreauthorised(TUint aProtocolLevel, const TBTDevAddr& aRemoteAddress);

	//	From MSocketNotify
	virtual void NewData(TUint aCount);
	virtual void CanSend();

	virtual void ConnectComplete();
	virtual void ConnectComplete(const TDesC8& aConnectData);
    virtual void ConnectComplete(CServProviderBase& aSSP);
	virtual void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	
	virtual void CanClose(TDelete aDelete=EDelete);
	virtual void CanClose(const TDesC8& aDisconnectData,TDelete aDelete=EDelete);

	virtual void Error(TInt aError,TUint aOperationMask=EErrorAllOperations);
	
	virtual void Disconnect(void);
	virtual void Disconnect(TDesC8& aDisconnectData);
	virtual void IoctlComplete(TDesC8 *aBuf);

	virtual void NoBearer(const TDesC8& /*aConnectionParams*/) {};
	virtual void Bearer(const TDesC8& /*aConnectionInf*/) {};

private:	
	//	Construction
	CIncomingConnectionListener(CBluetoothProtocolBase& aProtocol, CServProviderBase* aNewSAP);
	void ConstructL(const TBTSockAddr& aCompletedSockAddr, TUint aQueSize);

	void ClearIoctl();

	TUint					iIoctlLevel;
	TUint					iIoctlName;
	CBluetoothProtocolBase&	iProtocol;
	CServProviderBase*		iListeningSAP;
	CBluetoothSAP*			iRequestingSAP;	// the SAP asking for the Muxless Ioctl
	};

enum TListenerPanic
	{
	EListenerNullSAPForListening,
	EListenerSAPTwoIoctls,
	EListenerUnexpectedEvent,
	EListenerInvalidSAPIoctlCancel,
	};

void Panic(TListenerPanic aPanic);

#endif
