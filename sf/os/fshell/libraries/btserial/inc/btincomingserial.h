// btincomingserial.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __BTINCOMING_H__
#define __BTINCOMING_H__

#include <es_sock.h>
#include <bt_sock.h>
#include <btsdp.h>


enum TBtSerialPanic
	{
	EBtSerialNotConnected,
	EBtSerialWritePending,
	EBtSerialReadPending,	
	};

class CBtService;
class CBtRegQuery;

class MBtIncomingConnectionObserver
	{
public:
	virtual void BtSerialConnected() = 0;
	virtual void BtWriteComplete(TInt aError) = 0;
	virtual void BtReadComplete(TInt aError) = 0;
	virtual void BtSerialError(TInt aError) = 0;
	virtual void BtShutdownComplete(TInt /*aError*/) {};
	virtual void BtNameReceived(TInt /*aError*/, const TDesC& /*aName*/) {};
	};
	
class CBluetoothIncomingSerialConnection	: public CBase
											, public MBluetoothSocketNotifier
	{
public:
	IMPORT_C static CBluetoothIncomingSerialConnection* NewL(MBtIncomingConnectionObserver& aObserver);
	IMPORT_C virtual ~CBluetoothIncomingSerialConnection();
	
	IMPORT_C void WaitForConnection();
	IMPORT_C TBool Connected();
	
	IMPORT_C TBTDevAddr ConnectedDeviceAddrL();
	IMPORT_C void GetConnectedDeviceNameL();
	
	/**
	Write some data. Must already be connected, as signalled via a callback to BtSerialConnected();
	
	Will callback to BtWriteComplete() when completed, no more write must be sent until this event
	occurs.
	*/
	IMPORT_C void Write(const TDesC8& aData);
	IMPORT_C void CancelWrite();
	/**
	Read some data. Must already be connected, as signalled via a callback to BtSerialConnected();
	
	Will callback to BtReadComplete() when completed, no more write must be sent until this event
	occurs.
	*/
	IMPORT_C void Read(TDes8& aData);
	IMPORT_C void CancelRead();
	
	IMPORT_C void ShutDown();
	
private:
friend class CBtService;
friend class CBtRegQuery;
	CBluetoothIncomingSerialConnection(MBtIncomingConnectionObserver& aObserver);
	void ConstructL();

	virtual void HandleConnectCompleteL(TInt aErr);
	virtual void HandleAcceptCompleteL(TInt aErr);
	virtual void HandleShutdownCompleteL(TInt aErr);
	virtual void HandleSendCompleteL(TInt aErr);
	virtual void HandleReceiveCompleteL(TInt aErr);
	virtual void HandleIoctlCompleteL(TInt aErr);
	virtual void HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification);

	void HandleNewConnection(CBluetoothSocket* aConnectedSocket);
	void HandleConnectFailed(TInt aError);
	
	void QueryComplete(TInt aError);
private:
	MBtIncomingConnectionObserver& iObserver;
	RSdp iSdpSession;
	RSocketServ iSockServ;
	CBtService* iService;

	CBluetoothSocket* iSocket;
	TBool iSocketReady;
	TBool iSending;
	TBool iReceiving;
	TSockXfrLength iXferLen;
	
	RBTRegServ iBtRegServ;
	RBTRegistry iBtReg;
	CBTRegistryResponse* iRegReponse;
	CBtRegQuery* iRegQuery;
	
	CBTDevice* iConnectedDevice;
	RBuf iDeviceName;
	};

#endif //__BTINCOMING_H__
