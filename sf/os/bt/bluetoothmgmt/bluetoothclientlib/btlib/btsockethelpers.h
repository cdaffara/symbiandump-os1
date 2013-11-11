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
// BT socket active object helpers
// 
//

#ifndef BTSOCKETHELPERS_H
#define BTSOCKETHELPERS_H


#include <bluetooth/btbaseband.h>

//=============================================================================
//Active Objects
//



//CBluetoothSocket Active Objects...
NONSHARABLE_CLASS(CBTConnecter) : public CActive
	{
public:
	static CBTConnecter* NewL(CBluetoothSocket& aParent);
	void Connect(TBTSockAddr& aSockAddr);
	void Connect(TBTSockAddr& aSockAddr, const TDesC8& aConnectDataOut, TDes8& aConnectDataIn);

	~CBTConnecter();


private:
	CBTConnecter(CBluetoothSocket& aParent);
	void ConstructL();
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	TInt BasebandAccessor();

	CBluetoothSocket& iParent;
	RBTBaseband iActiveModeRequester;
	};


NONSHARABLE_CLASS(CBTAccepter) : public CActive
	{
public:
	static CBTAccepter* NewL(CBluetoothSocket& aParent, CBluetoothSocket& aBlankSocket);
	
	void Accept(); 
	void Accept(TDes8& aConnectData); 
	~CBTAccepter();
	void SetActive(){CActive::SetActive();};

private:
	CBTAccepter(CBluetoothSocket& aParent, CBluetoothSocket& aBlankSocket);
	void ConstructL();
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	void BasebandAccessor();

	CBluetoothSocket& iParent;
	CBluetoothSocket& iBlankSocket;
	};



NONSHARABLE_CLASS(CBTShutdowner) : public CActive
	{
public:
	static CBTShutdowner* NewL(CBluetoothSocket& aParent);
	void Shutdown(RSocket::TShutdown aHow,const TDesC8& aDisconnectDataOut,TDes8& aDisconnectDataIn);
	void Shutdown(RSocket::TShutdown aHow);
	void BasebandShutdown(/*TBTDevAddr aAddr*/);
	~CBTShutdowner();


private:
	CBTShutdowner(CBluetoothSocket& aParent);
	void ConstructL();
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothSocket& iParent;
	};


NONSHARABLE_CLASS(CBTSender) : public CActive
	{
public:
	static CBTSender* NewL(CBluetoothSocket& aParent);
	~CBTSender();

	void Write(const TDesC8& aDesc);
	void Send(const TDesC8& aDesc,TUint someFlags);
	void Send(const TDesC8& aDesc,TUint someFlags,TSockXfrLength& aLen);
	void SendTo(const TDesC8& aDesc,TSockAddr& aAddr,TUint flags);
	void SendTo(const TDesC8& aDesc,TSockAddr& aAddr,TUint flags,TSockXfrLength& aLen);


private:
	enum TCurrentRequest
		{
		ENone,
		EWrite,
		ESend,
		};
	CBTSender(CBluetoothSocket& aParent);
	void ConstructL();
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothSocket& iParent;
	TCurrentRequest iCurrentRequest;
	};


NONSHARABLE_CLASS(CBTReceiver) : public CActive
	{
public:
	static CBTReceiver* NewL(CBluetoothSocket& aParent);
	~CBTReceiver();

	void Read(TDes8& aDesc);
	void ReadSCOData(const TDesC8& aSCOData, TUint32 aPreferredPacket);
	void Recv(TDes8& aDesc,TUint flags);
	void Recv(TDes8& aDesc,TUint flags,TSockXfrLength& aLen);
	void RecvOneOrMore(TDes8& aDesc,TUint flags,TSockXfrLength& aLen);
	void RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint flags);
	void RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint flags,TSockXfrLength& aLen);


private:
	enum TCurrentRequest
		{
		ENone,
		ERead,
		ERecv,
		};
	CBTReceiver(CBluetoothSocket& aParent);
	void ConstructL();
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothSocket& iParent;
	TCurrentRequest iCurrentRequest;
	};


NONSHARABLE_CLASS(CBTIoctler) : public CActive
	{
public:
	static CBTIoctler* NewL(CBluetoothSocket& aParent);
	~CBTIoctler();

	void Ioctl(TUint aCommand, TDes8* aDesc=NULL, TUint aLevel=KLevelUnspecified);

private:
	CBTIoctler(CBluetoothSocket& aParent);
	void ConstructL();
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothSocket& iParent;
	};


NONSHARABLE_CLASS(MBTBasebandHandler)
	{
public:
	virtual RBTBaseband& MbbhBTBaseband() = 0;
	virtual void MbbhHandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification) = 0;
	virtual TInt MbbhOpen(RSocketServ& aSocketServ, RSocket& aSocket) = 0;
	virtual void MbbhClose() = 0;
	};

NONSHARABLE_CLASS(CBTBasebandChangeEventNotifier) : public CActive
	{
public:
	static CBTBasebandChangeEventNotifier* NewL(MBTBasebandHandler& aUser);
	~CBTBasebandChangeEventNotifier();

	void SetNotifyNextModeChange();
	void SetNotify(TUint32 aNotifications);


private:
	CBTBasebandChangeEventNotifier(MBTBasebandHandler& aUser);
	void ConstructL();
	//void SetNotify(TUint32 aNotifications);
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	MBTBasebandHandler& iUser;
	TUint iCurrentRequest;
	TBTBasebandEvent iEvent;
	};


NONSHARABLE_CLASS(CAutoSniffDelayTimer) : 	public CTimer, 
											public MBTBasebandHandler
/**
	This class provides a timer which when it expires requests Sniff Mode.
**/
	{
public:
	static CAutoSniffDelayTimer* NewL(CBluetoothSocket& aParent, TInt aIdleSecondsBeforeSniffRequest);
	~CAutoSniffDelayTimer();

	void Start();
	void StartActive();

	virtual RBTBaseband& MbbhBTBaseband();
	virtual void MbbhHandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification);
	virtual TInt MbbhOpen(RSocketServ& aSocketServ, RSocket& aSocket);
	virtual void MbbhClose();


private:
	CAutoSniffDelayTimer(CBluetoothSocket& aParent, TInt aIdleSecondsBeforeSniffRequest);
	void ConstructL();
	void RunL();
	TInt MakeSniffRequest();

private:
	CBluetoothSocket& 				iParent;
	RBTBaseband 					iBTAutoSniffBaseband;
	CBTBasebandChangeEventNotifier* iBTAutoSniffBasebandChangeEventNotifier;
	TTimeIntervalMicroSeconds32		iAutoSniffDelay;
	TBool							iSniffModeRequested;
	};


NONSHARABLE_CLASS(CBTBasebandManager) : public CBase
/**
	This class manages all basebands in order to keep them in step.
**/
	{
public:
	static CBTBasebandManager* NewL();
	~CBTBasebandManager();
	
	TInt RegisterBTBaseband(MBTBasebandHandler& aBasebandHandler);
	TInt UnRegisterBTBaseband(MBTBasebandHandler& aBasebandHandler);
	TInt HandleConnectionUp(RSocketServ& aSocketServ, RSocket& aSocket);
	void HandleConnectionDown();
	TBool Connected() const;

private:
	void ConstructL();
	CBTBasebandManager();
	void CloseAllBasebands();
	
private:
	TBool		 					iConnected;
	RSocketServ*					iSocketServ;
	RSocket*						iSocket;
	
	// unowned
	RPointerArray<MBTBasebandHandler>	iBasebandHandlers;
	};
	
NONSHARABLE_CLASS(CBTBasebandChangeEventDelegate) : public CBase, 
													public MBTBasebandHandler
/**
	This class provides CBluetoothSocket a proxy to access virtual functions in MBTBasebandHandler
**/
	{
public:
	static CBTBasebandChangeEventDelegate* NewL(CBluetoothSocket& Parent);
	
	virtual RBTBaseband& MbbhBTBaseband();
	virtual void MbbhHandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification);
	virtual TInt MbbhOpen(RSocketServ& aSocketServ, RSocket& aSocket);
	virtual void MbbhClose();
	
private:
	CBTBasebandChangeEventDelegate(CBluetoothSocket& Parent);
	void ConstructL();
	
private:
	CBluetoothSocket& iParent;
	};

#endif

