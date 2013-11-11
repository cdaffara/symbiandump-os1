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
// Define the L2CAP Service Access Point (SAP) class
// 
//

#ifndef L2SAP_H
#define L2SAP_H

#include <bt_sock.h>
#include "BtSap.h"

class CL2CAPProtocol;
class TL2CAPSAPState;

class CL2CapSDUQueue;
class CL2CapSAPSignalHandler;
class CL2CapChannelConfig;
class CL2CAPMux;
//class CBTCodServiceMan;

NONSHARABLE_CLASS(CL2CAPConnectionSAP) : public CBluetoothSAP
	{
public:
	friend class TL2CAPSAPStateClosed;
	friend class TL2CAPSAPStateBound;
	friend class TL2CAPSAPStateAwaitingInitialData;	
	friend class TL2CAPSAPStateOpen;
	friend class TL2CAPSAPStateAccepting;
	friend class TL2CAPSAPStateDisconnecting;

	enum TShutdownType
		{
		ESAPShutdownNone,
		ESAPShutdownImmediate,
		ESAPShutdownNormal,
		};

	static CL2CAPConnectionSAP* NewL(CL2CAPProtocol&);
	~CL2CAPConnectionSAP();

	// 'Un-hide' the other overloads
#ifndef __WINS__
#ifndef __GCC32__
	using CServProviderBase::Write;
	using CServProviderBase::GetData;
#endif
#endif

	// Virtual Methods from CServProviderBase / CBluetoothSAP
	void LocalName(TSockAddr& anAddr) const;
	TInt SetLocalName(TSockAddr& anAddr);
	void RemName(TSockAddr& anAddr) const;
	TInt SetRemName(TSockAddr& anAddr);
	const TBTDevAddr& RemoteDev() const;
	void SetRemoteDev(const TBTDevAddr& aAddr);
	
	TInt SAPSetOption(TUint aLevel,TUint aName,const TDesC8 &aOption);
	TInt GetOption(TUint aLevel,TUint aName,TDes8& aOption)const;

	void Ioctl(TUint aLevel,TUint aName,TDes8* aOption);
	void CancelIoctl(TUint aLevel,TUint aName);
	void IoctlComplete(TInt aErr, TUint aLevel,TUint aName,TDesC8* aBuf); // From CBluetoothSAP

	void Start();

	void ActiveOpen();
	void ActiveOpen(const TDesC8& aConnectionData);
	TInt PassiveOpen(TUint aQueSize);
	TInt PassiveOpen(TUint aQueSize,const TDesC8& aConnectionData);
	void Shutdown(TCloseType aOption);
	void Shutdown(TCloseType option,const TDesC8& aDisconnectionData);
	void AutoBind();

	TInt Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr = NULL);
	TInt GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr = NULL);

	// Des. versions.  These should be removed when they become
	// redundant.
	TUint Write(const TDesC8& aDesc, TUint aOptions, TSockAddr* anAddr = NULL);
	void GetData(TDes8& aDesc, TUint aOptions, TSockAddr* anAddr = NULL);

	
	// Methods from SAP Signal Handler
	void LinkUp();
	void ChannelOpened();
	void ChannelConfigured(CL2CapChannelConfig& aConfig,
                           CL2CAPMux& aMuxer,
                           TL2CAPPort aLocalPort,
                           TL2CAPPort aRemotePort);

	TInt UpdateChannelConfig(const TL2CapConfig& aAPIConfig);
	void ReconfiguringChannel();
	
	void ChannelClosed();
	
	void AccessRequestComplete(TInt aResult);

	void SignalHandlerError(TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction);
	void DataPlaneError(TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction);


	// Methods from Data Controller
	void NewData();
	void CanSend();
	void CloseOutgoingSDUQueue();
	void SDUQueueClosed();

	// State Pattern Utility Methods.
	void SetState(TL2CAPSAPState& aNewState);
	
	TInt CreateDataPlane(CL2CapChannelConfig& aConfig,
                         CL2CAPMux& aMuxer,
                         TL2CAPPort aLocalPort,
                         TL2CAPPort aRemotePort);

	void PauseDataPlane();
	void TryToCompleteConfigurationIoctl(TInt aError);	
		
	static TInt NewDataAsyncCallBack(TAny* aSAP);
	inline void StartNewDataAsyncCallBack();
	
	// Cloning Related Methods.
	TUint8 TotalOutstandingCloneCount() const;
	TBool CanAcceptConnection() const;
	CL2CAPConnectionSAP* CloneListeningSAP(const TBTDevAddr& aAddr);
	TBool DequeClone(CL2CAPConnectionSAP* aSAP);

	void DeleteAllClones();
	
	// This method is used by the SAP states if they need to delete the SAP.
	void DeleteSAP();
	
	void SocketConnectComplete();
	
	void EchoResponseReceived(const TDesC8* aData);
	void DetachFromListeningSAP();
	inline void SetListeningSAP(CL2CAPConnectionSAP& aSAP);
	inline void ClearListeningSAP();
	inline CL2CAPConnectionSAP* ListeningSAP() const;

	inline CL2CapSAPSignalHandler& SignalHandler() const;
	inline CL2CapSDUQueue& DataQueue() const;
	inline MSocketNotify* Socket() const;
	inline CL2CAPProtocol& Protocol() const;

	inline TBool IsSDUQueueClosed() const;
	inline TShutdownType ShutdownReceived() const;
	inline TUint8 ChannelPriority() const;

	inline TInt SocketErrorCode() const;
	inline TUint SocketErrorAction() const;
	inline void SetSocketErrorCode(TInt aError);
	inline void SetSocketErrorAction(MSocketNotify::TOperationBitmasks aAction);
	inline TBool IsAcceptPending() const;
private:
	CL2CAPConnectionSAP(CL2CAPProtocol&);
	void ConstructL();

	TBool EnqueClone(CL2CAPConnectionSAP* aSAP);
	
	TUint GetOptimalMTUSize(TUint aMTU, TUint aPduSize, TBool aBasicMode) const;

private:
	TL2CAPSAPState* iState;
	CL2CAPProtocol& iProtocol;

	CL2CapSDUQueue* iL2CapDataQueue;
	CL2CapSAPSignalHandler* iL2CapSAPSignalHandler;

	// Async Callback to notify New Data.
	CAsyncCallBack*	iNewDataAsyncCallBack;
	TUint16 iNewDataToNotify;

	TUint8 iMaxAcceptingQCount;

	RPointerArray<CL2CAPConnectionSAP> iClones;
	CL2CAPConnectionSAP* iListeningSAP;
	
	TShutdownType iShutdownReceived;
	TUint iOutstandingIoctlName;
	
	TUint8 iChannelPriority;

	TInt iSocketErrorCode;
	MSocketNotify::TOperationBitmasks iSocketErrorAction;
	//flag for dealing with the situation that we disconnect after connection 
	//complete and before accept. 
	TBool iAcceptPending;
	};


#include "l2sap.inl"


#endif

