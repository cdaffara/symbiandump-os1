// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
 @released
*/

#if !defined(__SS_SAPSHIM_H__)
#define __SS_SAPSHIM_H__

#define SYMBIAN_NETWORKING_UPS

#include <comms-infras/es_sap.h>
#include <comms-infras/ss_connlegacy.h>
#include "ss_sapfactshim.h"
#include <comms-infras/nifif.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_flow.h>

#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/upsmessages.h>
#endif

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
#include <comms-infras/ss_protopt.h>
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

using NetInterfaces::TIfStaticFetcher;
using NetInterfaces::TIfStaticFetcherFirstInHierarchy;

namespace ESock
{

class CTransportFlowShim : public CNetworkFlow,
                         protected MSocketNotify,
                         protected MLowerDataSender,
                         protected MLowerControl
/** Service Access Point Shim to convert CServProviderBase objects to
CControlledServProvider objects.

Provides transport services to a single protocol. Several of the calls to
CServProviderBase have pre-conditions attached to them - for example
a connection oriented protocol must have its local address set (either by
a SetLocalName() or AutoBind()) before it is opened. If the socket server
calls the CServProviderBase in such an erroneous way, the protocol should panic.

@internalTechnology
@released Since 9.0 */
	{
    friend class ASocket;
#ifdef SYMBIAN_NETWORKING_UPS
	friend class CHostResolver;		// to allow CHostResolver to call SecurityCheck()
#endif

public:
    typedef CTransportFlowShimFactory FactoryType; //for factoryobject_cast to work

	static CTransportFlowShim* NewL(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf);
	void SetSSP(CServProviderBase& aSSP);

	inline CServProviderBase* Provider();

protected:
	virtual ~CTransportFlowShim();
	void SetSockType(TUint aSockType);
	TInt LockToConnectionInfo();
	
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
	virtual TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* anAddr);
	virtual TInt Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr);
	virtual void GetData(TDes8& aDesc, TUint aOptions, TSockAddr* anAddr);
	virtual TInt GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr);
	virtual TInt SecurityCheck(MProvdSecurityChecker* aSecurityChecker);

    //MFlowBinderControl
    //session binders for CSocket
    virtual MSessionControl* GetControlL(TInt aSessionType,MSessionControlNotify& aSessionControlNotify);
    virtual MSessionData* BindL(MSessionDataNotify& aNotify);
    virtual void Unbind();
    virtual CSubConnectionFlowBase& CloneFlowL();

	//CHostResolver specific workaround for now
    //protocol binders for CHostResolver => subject to change the moment we can stack app level protocols
    //resolvers are really app level protocols
	virtual MLowerControl* GetControlL(const TDesC8& aProtocol);
	virtual MLowerDataSender* BindL(const TDesC8& aProtocol, MUpperDataReceiver* aReceiver, MUpperControl* aControl);
	virtual void Unbind( MUpperDataReceiver* aReceiver, MUpperControl* aControl);
	//MLowerDataSender for CHostResolver only => it kicks-off sub-connection by a dummy send and waits
	//for MUpperControl::StartSending()/Error() after which it can read connection info/fail
	virtual MLowerDataSender::TSendResult Send(RMBufChain& aData);
    //MLowerControl
	virtual TInt Control(TUint aLevel, TUint aName, TDes8& aOption);
    //Messages::ANode
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	virtual const Messages::TNodeId& NodeId() const;
	//Messages From sub-connection
#ifdef SYMBIAN_NETWORKING_UPS
    	virtual void SubConnectionError(const Messages::TEBase::TError& errorMsg, TUint anOperationMask = (MSessionControlNotify::EErrorSend |
			MSessionControlNotify::EErrorConnect | MSessionControlNotify::EErrorCompleteSetLocalName));

#else
	    void SubConnectionError(
		const Messages::TEBase::TError& errorMsg,
		TUint anOperationMask = (MSessionControlNotify::EErrorSend |
				MSessionControlNotify::EErrorConnect | MSessionControlNotify::EErrorCompleteSetLocalName)
		);

#endif
    void BindToL(TCFDataClient::TBindTo& aBindTo);
    void Rejoin(const TCFFlow::TRejoin& aRejoinMessage);
	void StartFlowL(const Messages::TRuntimeCtxId& aSender);
    void StopFlow(TCFDataClient::TStop& aMessage);
    void Destroy();
	
	//Stores the contents of the provision config message.
	//which is the window size in this case.
	//The value will be used when sockets are opened to set the TCP receive window size.
	void StoreProvision(TCFDataClient::TProvisionConfig& aMessage);
	//Extracts the extension from TProvisionConfig
	//Presently extracts pointer to CTCPReceiveWindow
	void ExtractSetOptExtensions();
	void SetProtocolOptions();

protected:
	CTransportFlowShim(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf);

#ifdef SYMBIAN_NETWORKING_UPS
	virtual TInt SetupForNoBearerOnSend();
	virtual void PreUnbind();
	virtual TInt ProcessReceivedL(Messages::TSignatureBase& aMessage);

	// from CNetworkFlow
	TBool ActivityRunning();
	
	// Utility functions
	inline TBool IsHostResolver() const;
	inline TBool IsStopped();
	inline void SetIfInfo(const TSoIfConnectionInfo& aInfo);
	    
    void PostNoBearer();
    void PostDataClientRouted();
	// From MSocketNotify
	virtual void Error(TInt anError,TUint anOperationMask=MSocketNotify::EErrorAllOperations);
#endif

private:
	// From MSocketNotify to convert up-calls to MSessionControlNotify & MSessionDataNotify
	virtual void NewData(TUint aCount);
	virtual void CanSend();
	virtual void ConnectComplete();
	virtual void ConnectComplete(const TDesC8& aConnectData);
    virtual void ConnectComplete(CServProviderBase& aSSP);
	virtual void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	virtual void CanClose(MSocketNotify::TDelete aDelete=MSocketNotify::EDelete);
	virtual void CanClose(const TDesC8& aDisconnectData,MSocketNotify::TDelete aDelete=MSocketNotify::EDelete);
#ifndef SYMBIAN_NETWORKING_UPS
	// Needs to be protected rather than private for UPS support
	virtual void Error(TInt anError,TUint anOperationMask=MSocketNotify::EErrorAllOperations);
#endif
	virtual void Disconnect(void);
	virtual void Disconnect(TDesC8& aDisconnectData);
	virtual void IoctlComplete(TDesC8 *aBuf);
	virtual void NoBearer(const TDesC8& aConnectionParams);
	virtual void Bearer(const TDesC8& aConnectionInfo);
	TBool DoNoBearer();
	void SetLocalNameProcess();


    CTransportFlowShim* CloneNSetSAP(CServProviderBase& aSSP);
	void InitDestroy();
#ifndef SYMBIAN_NETWORKING_UPS
	// Needs to be protected rather than private for UPS support
    void PostNoBearer();
    void PostDataClientRouted();
#endif
    void CompleteStart(TInt aError);

	void NoBearerCompletion();
	inline void SetFlowParams(const TFlowParams& aFlowParams);

protected:
	// Flags to set on iStateFlags inherited from CNetworkFlow
	enum TStateFlag
		{
		EDetaching = 0x0001000,
		EDataClientRoutedGuard = 0x00020000,
		EStopped = 0x00040000,
		EShuttingDown = 0x00080000,
		EFlowParamsInitialised = 0x00100000,
		EBearerExpected = 0x00200000,
		EDeleteUponBearerReception = 0x00400000,
		EUseBearerErrors = 0x00800000
		};
	SAP_FLAG_FUNCTIONS(Detaching, EDetaching)
	SAP_FLAG_FUNCTIONS(DataClientRoutedGuard, EDataClientRoutedGuard)
	SAP_FLAG_FUNCTIONS(Stopped, EStopped)
	SAP_FLAG_FUNCTIONS(ShuttingDown, EShuttingDown)
	SAP_FLAG_FUNCTIONS(FlowParamsInitialised, EFlowParamsInitialised)
	SAP_FLAG_FUNCTIONS(BearerExpected, EBearerExpected)
	SAP_FLAG_FUNCTIONS(DeleteUponBearerReception, EDeleteUponBearerReception)
	SAP_FLAG_FUNCTIONS(UseBearerErrors, EUseBearerErrors)

	void HostResolverSpecificUnbind();

private:
	CServProviderBase* iProvider;
	//CHostResolver specific workaround for now
	MUpperControl* iHostResolverNotify; //since CHostResolver specific
	MSessionControlNotify* iListenerControlNotify; // to notify listening sockets that a passively opened socket in the accept queue has been unbound
	// TODO
	// MSocketControlledNotify* iSocket;
	Messages::RRequestOriginator iStartRequest;

    TSoIfConnectionInfo iIfInfo;
	

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    const CSAPSetOpt *iProtocolOptions;
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	
	// Reference to the protocol as managed by the ProtocolManager
	CProtocolRef* iProtocolReference;
	TFlowParams iFlowParams;
	};

inline CServProviderBase* CTransportFlowShim::Provider()
	{
	return iProvider;
	}

inline void CTransportFlowShim::SetFlowParams(const TFlowParams& aFlowParams)
    {
    iFlowParams = aFlowParams;
    SetFlowParamsInitialised();
    }

#ifdef SYMBIAN_NETWORKING_UPS

//
// CTransportFlowShim inline methods
//
inline TBool CTransportFlowShim::IsHostResolver() const
	{ return iHostResolverNotify != NULL; }

inline void CTransportFlowShim::SetIfInfo(const TSoIfConnectionInfo& aInfo)
	{ iIfInfo = aInfo; }

//
// CUpsTransportFlowShim
//

NONSHARABLE_CLASS(CUpsTransportFlowShim) : public CTransportFlowShim
/**
CUpsTransportFlowShim - UPS specific functionality over CTransportFlowShim
*/
	{
public:
	static CUpsTransportFlowShim* NewL(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf);
	inline CUpsTransportFlowShim(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf);

	inline TBool UpsEnabled() const;
	inline void SetPolicyCheckRequestPending(TBool aGuard);
	inline TBool PolicyCheckRequestPending() const;

	void ParseNoBearerParams(const TDesC8& aConnectionParams);
	void GetProcessAndThreadId(TProcessId& aProcessId, TThreadId& aThreadId) const;
	void PopulateUpsExtension(TInt aPolicyCheckResult) const;
	void PostPolicyCheckRequest(TInt aPolicyCheckResult);
	void ProcessPolicyCheckResponse(const UpsMessage::TPolicyCheckResponse& aMessage);

protected:
	// from CTransportFlowShim
	TInt SetupForNoBearerOnSend();
	void PreUnbind();
	TInt ProcessReceivedL(Messages::TSignatureBase& aMessage);
    void SubConnectionError(const Messages::TEBase::TError& aErrorMsg, TUint anOperationMask = (MSocketNotify::EErrorSend | MSocketNotify::EErrorConnect));
    void IssueScopedNoBearerResponse();
    
	// from CTransportFlowShim (CServProviderBase base class)
	TInt SetOption(TUint aLevel, TUint aName, const TDesC8& anOption);
	TInt SecurityCheck(MProvdSecurityChecker* aChecker);
	
	// from CTransportFlowShim (MSocketNotify base class)
	void NoBearer(const TDesC8& aConnectionParams);

	// from CTransportFlowShim (CNetworkFlow base class)
	TBool ActivityRunning();

private:
	MProvdSecurityChecker* iSecurityChecker;	// class used to perform PlatSec check
	TProcessId  iProcessId; 					// Client Process Id
	TThreadId   iThreadId;  					// Client Thread  Id	
	CUPSAccessPointConfigExt* iUpsExtension;	// UPS Access Point Config structure.  Remains NULL if UPS is disabled.
	const ESock::MPlatsecApiExt* iPlatsecIf;	// upcall interface to allow dynamic retrieval of thread/process id

	TUint iIsScoped:1;					// NoBearer() has indicated that the socket address has a scope id.
	TUint iPolicyCheckRequestPending:1;	// TPolicyCheckRequest message has been sent, awaiting response
	};

//
// CUpsTransportFlowShim Inline functions
//

inline CUpsTransportFlowShim::CUpsTransportFlowShim(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf)
		: CTransportFlowShim(aFactory, aSubConn, aProtocolIntf)
			{
		 	}

inline TBool CUpsTransportFlowShim::UpsEnabled() const
	{ return iUpsExtension != NULL;	}
	
inline void CUpsTransportFlowShim::SetPolicyCheckRequestPending(TBool aPending)
	{ iPolicyCheckRequestPending = aPending; }
	
inline TBool CUpsTransportFlowShim::PolicyCheckRequestPending() const
	{ return iPolicyCheckRequestPending; }

#endif

} // namespace ESock

#endif //__SS_SAPSHIM_H__

