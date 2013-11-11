// btincoming.cpp
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

#include <e32base.h>
#include "btincomingserial.h"
#include "btservice.h"
#include "btdebug.h"

_LIT(KBtSerialPanic, "BTSerial");
void Panic(TBtSerialPanic aReason)
	{
	TRACE2("BtIncoming panic %d", aReason);
	User::Panic(KBtSerialPanic, aReason);
	};
	
	
NONSHARABLE_CLASS(CBtRegQuery) : public CActive
	{
public:
	CBtRegQuery(CBluetoothIncomingSerialConnection& aOwner);
	void Start(CBTRegistryResponse& aResponse);
private:
	virtual void RunL();
	virtual void DoCancel();
private:
	CBluetoothIncomingSerialConnection& iOwner;
	CBTRegistryResponse* iResponse;
	};
	

EXPORT_C CBluetoothIncomingSerialConnection* CBluetoothIncomingSerialConnection::NewL(MBtIncomingConnectionObserver& aObserver)
	{
	TRACE1("CBluetoothIncomingSerialConnection::NewL");
	CBluetoothIncomingSerialConnection* self = new(ELeave)CBluetoothIncomingSerialConnection(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CBluetoothIncomingSerialConnection::~CBluetoothIncomingSerialConnection()
	{
	TRACE1("CBluetoothIncomingSerialConnection::~CBluetoothIncomingSerialConnection");
	delete iService;
	delete iSocket;
	iSockServ.Close();
	iSdpSession.Close();
	}
	
CBluetoothIncomingSerialConnection::CBluetoothIncomingSerialConnection(MBtIncomingConnectionObserver& aObserver)
	: iObserver(aObserver)
	{
	}
	
void CBluetoothIncomingSerialConnection::ConstructL()
	{
	TRACE1("+CBluetoothIncomingSerialConnection::ConstructL");
	User::LeaveIfError(iSdpSession.Connect());
	User::LeaveIfError(iSockServ.Connect());
	TUUID serialPortUUID = TUUID(0x1101);
	iService = CBtService::NewL(serialPortUUID, iSdpSession, iSockServ, *this, KRFCOMM); 
	
	// create a blank socket for the incoming connection
	iSocket = CBluetoothSocket::NewL(*this, iSockServ);
	TRACE1("-CBluetoothIncomingSerialConnection::ConstructL");
	}
	
	
EXPORT_C void CBluetoothIncomingSerialConnection::WaitForConnection()
	{
	TRACE1("CBluetoothIncomingSerialConnection::WaitForConnection");
	iService->AcceptConnection(*iSocket);
	}
	
EXPORT_C TBool CBluetoothIncomingSerialConnection::Connected()
	{
	TRACE2("CBluetoothIncomingSerialConnection::Connected %d", iSocketReady);
	return iSocketReady;
	}
	
EXPORT_C TBTDevAddr CBluetoothIncomingSerialConnection::ConnectedDeviceAddrL()
	{
	if (!iSocketReady) User::Leave(KErrNotReady);
	TBTSockAddr remote;
	iSocket->RemoteName(remote);
	return remote.BTAddr();
	}
	
EXPORT_C void CBluetoothIncomingSerialConnection::GetConnectedDeviceNameL()
	{
	if (iConnectedDevice) return;
	if (!iSocketReady) User::Leave(KErrNotReady);
	// lazy construction....
	if (!iRegQuery) iRegQuery = new(ELeave)CBtRegQuery(*this);
	if (iRegQuery->IsActive())
		{
		return;
		}
	if (reinterpret_cast<RHandleBase&>(iBtRegServ).Handle() == KNullHandle)
		{
		User::LeaveIfError(iBtRegServ.Connect());
		}
	if (iBtReg.SubSessionHandle() == KNullHandle)
		{
		User::LeaveIfError(iBtReg.Open(iBtRegServ));
		}
	if (!iRegReponse) iRegReponse = CBTRegistryResponse::NewL(iBtReg);
	
	
	TBTSockAddr remote;
	iSocket->RemoteName(remote);

	TBTRegistrySearch findConnected;
	findConnected.FindAddress(remote.BTAddr());
	TRequestStatus stat;
	iBtReg.CreateView(findConnected, stat);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	
	iRegQuery->Start(*iRegReponse);
	}
	
void CBluetoothIncomingSerialConnection::QueryComplete(TInt aError)
	{
	iBtReg.CloseView();
	if (aError == KErrNone)
		{
		RBTDeviceArray& results = iRegReponse->Results();
		if (results.Count() != 1)
			// we queried by BD address, so should get 1 (or 0) results
			{
			aError = KErrNotFound;
			}
		if (aError == KErrNone)
			{
			TRAP(aError, iConnectedDevice = results[0]->CopyL());
			}
		if (aError == KErrNone)
			{
			if (iConnectedDevice->IsValidFriendlyName())
				{
				aError = iDeviceName.Create(iConnectedDevice->FriendlyName());
				}
			else
				{
				aError = iDeviceName.Create(iConnectedDevice->DeviceName().Length());
				if (aError == KErrNone)
					{
					iDeviceName.Copy(iConnectedDevice->DeviceName());
					}
				}
			if (aError!=KErrNone)
				{
				delete iConnectedDevice;
				iConnectedDevice = NULL;
				}
				
			}
		}
	iObserver.BtNameReceived(aError, iDeviceName);
	}

EXPORT_C void CBluetoothIncomingSerialConnection::Write(const TDesC8& aData)
	{
	TRACE1("CBluetoothIncomingSerialConnection::Write");
	__ASSERT_ALWAYS(iSocketReady, Panic(EBtSerialNotConnected));
	__ASSERT_ALWAYS(!iSending, Panic(EBtSerialWritePending));
	TInt err = iSocket->Send(aData, 0);
	if (err==KErrNone)
		{
		iSending = ETrue;
		}
	else
		{
		iObserver.BtWriteComplete(err);
		}
	}
	
EXPORT_C void CBluetoothIncomingSerialConnection::CancelWrite()
	{
	TRACE1("CBluetoothIncomingSerialConnection::CancelWrite");
	iSocket->CancelSend();
	iSending = EFalse;
	}
	
EXPORT_C void CBluetoothIncomingSerialConnection::Read(TDes8& aData)
	{
	TRACE1("CBluetoothIncomingSerialConnection::Read");
	__ASSERT_ALWAYS(iSocketReady, Panic(EBtSerialNotConnected));
	__ASSERT_ALWAYS(!iReceiving, Panic(EBtSerialReadPending));
	aData.Zero();	
	TInt err = iSocket->RecvOneOrMore(aData, 0, iXferLen);
	if (err==KErrNone)
		{
		iReceiving = ETrue;
		}
	else
		{
		iObserver.BtReadComplete(err);
		}
	}
	
EXPORT_C void CBluetoothIncomingSerialConnection::CancelRead()
	{
	TRACE1("CBluetoothIncomingSerialConnection::CancelRead");
	iSocket->CancelRead();
	iReceiving = EFalse;
	}
	
EXPORT_C void CBluetoothIncomingSerialConnection::ShutDown()
	{
	iSocket->Shutdown(RSocket::EImmediate);
	}

void CBluetoothIncomingSerialConnection::HandleConnectCompleteL(TInt _BTDEB(aErr))
	{
	TRACE2("CBluetoothIncomingSerialConnection::HandleConnectCompleteL(%d)", aErr);
	}
	
void CBluetoothIncomingSerialConnection::HandleAcceptCompleteL(TInt _BTDEB(aErr))
	{
	TRACE2("CBluetoothIncomingSerialConnection::HandleAcceptCompleteL(%d)", aErr);
	}
	
void CBluetoothIncomingSerialConnection::HandleShutdownCompleteL(TInt aErr)
	{
	TRACE2("CBluetoothIncomingSerialConnection::HandleShutdownCompleteL(%d)", aErr);
	iObserver.BtShutdownComplete(aErr);
	}
	
void CBluetoothIncomingSerialConnection::HandleSendCompleteL(TInt aErr)
	{
	TRACE1("CBluetoothIncomingSerialConnection::HandleSendCompleteL");
	__ASSERT_ALWAYS(iSending, PANIC());
	iSending = EFalse;
	iObserver.BtWriteComplete(aErr);
	}
	
void CBluetoothIncomingSerialConnection::HandleReceiveCompleteL(TInt aErr)
	{
	TRACE1("CBluetoothIncomingSerialConnection::HandleReceiveCompleteL");
	__ASSERT_ALWAYS(iReceiving, PANIC());
	iReceiving = EFalse;
	iObserver.BtReadComplete(aErr);
	}

void CBluetoothIncomingSerialConnection::HandleIoctlCompleteL(TInt _BTDEB(aErr))
	{
	TRACE2("CBluetoothIncomingSerialConnection::HandleIoctlCompleteL(%d)", aErr);
	}
	
void CBluetoothIncomingSerialConnection::HandleActivateBasebandEventNotifierCompleteL(TInt _BTDEB(aErr), TBTBasebandEventNotification& /*aEventNotification*/)
	{
	TRACE2("CBluetoothIncomingSerialConnection::HandleActivateBasebandEventNotifierCompleteL(%d)", aErr);
	}
	
void CBluetoothIncomingSerialConnection::HandleNewConnection(CBluetoothSocket* aConnectedSocket)
	{
	TRACE1("CBluetoothIncomingSerialConnection::HandleNewConnection");
	__ASSERT_ALWAYS(iSocket == aConnectedSocket, PANIC());
	iSocketReady = ETrue;
	iObserver.BtSerialConnected();
	}
	
void CBluetoothIncomingSerialConnection::HandleConnectFailed(TInt aError)
	{
	TRACE2("CBluetoothIncomingSerialConnection::HandleConnectFailed", aError);
	iSocketReady = EFalse;
	iObserver.BtSerialError(aError);
	}
	
CBtRegQuery::CBtRegQuery(CBluetoothIncomingSerialConnection& aOwner)
	: CActive(CActive::EPriorityStandard), iOwner(aOwner)
	{
	CActiveScheduler::Add(this);
	}
	
void CBtRegQuery::Start(CBTRegistryResponse& aBtResponse)
	{
	iResponse = &aBtResponse;
	iResponse->Start(iStatus);
	SetActive();
	}
		
void CBtRegQuery::RunL()
	{
	iOwner.QueryComplete(iStatus.Int());
	}
	
void CBtRegQuery::DoCancel()
	{
	iResponse->Cancel();
	// nasty race condition inside CBTRegistryResponse which means the cancal call may not
	// request out TRequestStatus (if DoCancel doesn't need to be called)
	if (iStatus == KRequestPending)
		{
		TRequestStatus* stat = &iStatus;
		User::RequestComplete(stat, KErrCancel);
		}
	}

