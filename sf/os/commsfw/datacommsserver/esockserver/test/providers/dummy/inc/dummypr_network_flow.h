// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Dummy network flow
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__DUMMYPR_NETWORK_FLOW_H__)
#define __DUMMYPR_NETWORK_FLOW_H__

#include "dummypr_factory.h"
#include <comms-infras/ss_protflow.h>
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/es_sap.h>
#include <comms-infras/nifif.h>

#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_flow.h>

namespace ESock
{

class CDummyNetworkFlow : public CSubConnectionFlowBase,
	public MSessionData,
	public MSessionControl,
	public MFlowBinderControl,
	public MSessionControlNotify,
	public MSessionDataNotify
/**
@internalComponent
@released since v9.6
 */
	{
    friend class CSocket;
public:
    enum {EProtocolId = 253};
    typedef CDummyFlowFactory FactoryType; //for factoryobject_cast to work

	static CDummyNetworkFlow* NewL(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf);
	void SetSSP(CServProviderBase& aSSP);

	inline CServProviderBase* Provider();

protected:
	// Lifetime
	CDummyNetworkFlow(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf);
	virtual ~CDummyNetworkFlow();
	void SetSockType(TUint aSockType);
	TInt LockToConnectionInfo();

	// MSessionData
	virtual TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* anAddr);
	virtual TInt Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr);
	virtual void GetData(TDes8& aDesc, TUint aOptions, TSockAddr* anAddr);
	virtual TInt GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr);

	// MSessionControl
	virtual void Start();
	virtual void LocalName(TSockAddr& anAddr) const;
	virtual void SetLocalName(TSockAddr& anAddr);
	virtual void RemName(TSockAddr& anAddr) const;
	virtual TInt SetRemName(TSockAddr& anAddr);
	virtual TInt GetOption(TUint aLevel,TUint aName,TDes8& anOption) const;
	virtual void Ioctl(TUint aLevel,TUint aName,TDes8* anOption);
	virtual void CancelIoctl(TUint aLevel,TUint aName);
	virtual TInt SetOption(TUint aLevel, TUint aName,const TDesC8 &anOption);
	virtual void ActiveOpen();
	virtual void ActiveOpen(const TDesC8& aConnectionData);
	virtual TInt PassiveOpen(TUint aQueueSize);
	virtual TInt PassiveOpen(TUint aQueueSize,const TDesC8& aConnectionData);
	virtual void Shutdown(MSessionControl::TCloseType aOption);
	virtual void Shutdown(MSessionControl::TCloseType aOption, const TDesC8& aDisconnectionData);
	virtual void AutoBind();
	virtual TInt SecurityCheck(MProvdSecurityChecker* aSecurityChecker);

    // MFlowBinderControl
    //session binders for CSocket
    virtual MSessionControl* GetControlL(TInt aSessionType,MSessionControlNotify& aSessionControlNotify);
    virtual MSessionData* BindL(MSessionDataNotify& aNotify);
    virtual void Unbind();
    virtual CSubConnectionFlowBase& CloneFlowL();
	virtual CSubConnectionFlowBase* Flow();

private:
	// MSessionControlNotify
    void ConnectComplete();
	void ConnectComplete(const TDesC8& aConnectData);
    void ConnectComplete(CSubConnectionFlowBase& anSSP);
	void ConnectComplete(CSubConnectionFlowBase& anSSP,const TDesC8& aConnectData);
	void CanClose(MSessionControlNotify::TDelete aDelete=MSessionControlNotify::EDelete);
    void CanClose(const TDesC8& aDisconnectData,MSessionControlNotify::TDelete aDelete=MSessionControlNotify::EDelete);
	TInt Error(TInt anError,TUint anOperationMask);
	void Disconnect( );
	void Disconnect(TDesC8& aDisconnectData);
	void IoctlComplete(TDesC8* aBuf);
	void DisconnectFromListener(CSubConnectionFlowBase& aSSP);
	void SetLocalNameComplete();


	// MSessionDataNotify
	void NewData(TUint aCount);
	void CanSend();

protected:
	// CSubConnectionFlowBase
	virtual MFlowBinderControl* DoGetBinderControlL();

	// Messages::ANode
	virtual void ReceivedL(
		const Messages::TRuntimeCtxId& aSender,
		const Messages::TNodeId& aRecipient,
		Messages::TSignatureBase& aMessage
		);

    // Local methods
    void SubConnectionError(
		const Messages::TEBase::TError& errorMsg,
		TUint anOperationMask = (MSocketNotify::EErrorSend | MSocketNotify::EErrorConnect));
	void BindToL(TCFDataClient::TBindTo& aBindTo);
	void Rejoin(const TCFFlow::TRejoin& aRejoinMessage);
	void StartFlow();
	void StopFlow(TCFDataClient::TStop& aMessage);
	void ProcessDCIdleState();
	void CompleteStart(TInt aError);
	void Destroy();
	void PostNoBearer();

	// Inline methods
	TBool NoBearerGuard() const { return iNoBearerRunning; }
	void SetNoBearerGuard() { iNoBearerRunning = ETrue; }
	void ClearNoBearerGuard() { iNoBearerRunning = EFalse; }
	void ClearUseBearerErrors() { iUseBearerErrors = EFalse; }
	TBool IsStopped() { return iIsStopped; }
	TBool IsHostResolver() const { return EFalse; }
	void SetIfInfo(const TSoIfConnectionInfo& aInfo) { iIfInfo = aInfo; }
    TBool IsBoundToSession() const {return  iSessionControlNotify || iSessionDataNotify; }
	TBool ActivityRunning()	{return NoBearerGuard();}


private:
	// Flow binders
	MFlowBinderControl* iLowerFlow;
	MLowerControl* iLowerControl; //just to keep the lower flow up

	// Lower session binding
	MSessionControl* iFlowShim;
	MSessionData* iFlowShimData;

	// Upper session binding
	MSessionControlNotify* iSessionControlNotify;
	MSessionDataNotify* iSessionDataNotify;

	Messages::RRequestOriginator iStartRequest;

	// Local properties
	TSoIfConnectionInfo iIfInfo;
	TUint iUseBearerErrors:1;		// error the socket on Error() upcalls from bearer rather than StopFlow() calls from SCPR
	TUint iIsStopped:1;				// DataClientStop has been received.
	TUint iIsStarting:1;
	TBool iNoBearerRunning:1;		// NoBearer message has been issued to SCPR
	TUint iStartActivityId:16;		// If iIsStarting is set, iStartActivityId contains activity id that opened the TCFDataClient::TStarted channel
	TBool iIsStarted:1;

	enum TDCIdleState
		{
		EClientsPresent = 0, // presumption is we're always created for a client (ie HR or socket)
		EIdle = 1,
		EIdleSent = 2
		};
	TDCIdleState iDCIdle:2;
	};

}


#endif	// __DUMMYPR_NETWORK_FLOW_H__

