// btserialsession.h
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

#ifndef __BTSERIALSESSION_H__
#define __BTSERIALSESSION_H__

#include <e32base.h>
#include <fshell/btincomingserial.h>


class CBtSerialServer;

enum TBtSerialServerPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	EPanicAlreadyReceiving,
	EPanicInvalidInputMode,
	EPanicWrongKeypressDesLength,
	};

void PanicClient(const RMessagePtr2& aMessage,TBtSerialServerPanic aPanic);

class CBtSerialSession	: public CSession2
						, public MBtIncomingConnectionObserver
	{
public:
	CBtSerialSession();
	void CreateL();
	void Send(const TDesC& aMessage);
	
	// from MBtIncomingConnectionObserver:
	virtual void BtSerialConnected();
	virtual void BtWriteComplete(TInt aError);
	virtual void BtReadComplete(TInt aError);
	virtual void BtSerialError(TInt aError);
	virtual void BtShutdownComplete(TInt aError);
	virtual void BtNameReceived(TInt aError, const TDesC& aName);
	
	virtual void Disconnect(const RMessage2& aMessage);
private:
	~CBtSerialSession();
	inline CBtSerialServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);

	void WaitForConnectionL(const RMessage2& aMessage);
	void CancelWaitForConnection(const RMessage2& aMessage);
	void IsConnectedL(const RMessage2& aMessage);
	void WriteL(const RMessage2& aMessage);
	void CancelWrite(const RMessage2& aMessage);
	void ReadL(const RMessage2& aMessage);
	void CancelRead(const RMessage2& aMessage);
	void GetConnectedDeviceAddrL(const RMessage2& aMessage);
	void GetConnectedDeviceNameL(const RMessage2& aMessage);
private:
	CBluetoothIncomingSerialConnection* iConnection;
	TBool iWaitingForConnection;
	RMessagePtr2 iConnectedMessage;
	RMessagePtr2 iWriteMessage;
	RMessagePtr2 iReadMessage;
	RMessage2 iDisconnectMessage;
	RMessagePtr2 iGetNameMessage;
	
	RBuf8 iReadBuf;
	RBuf8 iWriteBuf;
	};

#endif //__BTSERIALSESSION_H__
