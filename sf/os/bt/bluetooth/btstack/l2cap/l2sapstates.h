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
// Defines classes to implement each state a L2CAP sap can
// be in.
// Note we assume for all states that the socket will only send us
// reasonable state change requests - i.e. it won't call PassiveOpen
// on a connected socket.  We make no such assumption about events
// coming from the Mux
// 
//


#ifndef L2CAPSAPSTATES_H
#define L2CAPSAPSTATES_H

#include <e32base.h>
#include <es_prot.h>
#include <bt_sock.h>


#include "L2CapChannelConfig.h"
#include "L2types.h"

class CL2CAPConnectionSAP;
class CL2CAPSAPStateFactory;

class CL2CAPMux;

NONSHARABLE_CLASS(TL2CAPSAPState)
	{
public:
	friend class CL2CAPConnectionSAP;

	TL2CAPSAPState(CL2CAPSAPStateFactory& aFactory);

	
	// Events called from the SAP.  As we assume that the socket
	// will never send us requests which are invalid for our state,
	// the default is to do nothing.  Derived state classes can then
	// simply implement the appropriate bits.
	virtual void Start(CL2CAPConnectionSAP& aSAP) const;

	virtual TInt Ioctl(CL2CAPConnectionSAP& aSAP, TUint aLevel, TUint aName, TDes8* aOption) const;
	virtual void CancelIoctl(CL2CAPConnectionSAP& aSAP, TUint aLevel, TUint aName) const;
	virtual void IoctlComplete(CL2CAPConnectionSAP& aSAP, TInt aErr, TUint aLevel, TUint aName, TDesC8* aBuf) const;

	virtual void ActiveOpen(CL2CAPConnectionSAP& aSAP) const;
	virtual TInt PassiveOpen(CL2CAPConnectionSAP& aSAP, TUint aQueSize) const;
	virtual void Shutdown(CL2CAPConnectionSAP& aSAP) const;
	virtual void FastShutdown(CL2CAPConnectionSAP& aSAP) const;
	
	virtual TInt Send(CL2CAPConnectionSAP& aSAP, RMBufChain& aData, TUint aFlag) const;
	virtual TInt Read(CL2CAPConnectionSAP& aSAP, RMBufChain& aData) const;

	virtual void Bound(CL2CAPConnectionSAP& aSAP) const;
	
	// Events called from the SAP Signal Handler.
	virtual void ChannelConfigured(CL2CAPConnectionSAP& aSAP,
	                               CL2CapChannelConfig& aConfig,
                                   CL2CAPMux& aMuxer,
                                   TL2CAPPort aLocalPort,
                                   TL2CAPPort aRemotePort);
                                       
	virtual void ReconfiguringChannel(CL2CAPConnectionSAP& aSAP) const;

	virtual void ChannelClosed(CL2CAPConnectionSAP& aSAP) const;
	virtual void CloseOutgoingSDUQueue(CL2CAPConnectionSAP& aSAP) const;
	virtual void SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;
	virtual void LinkUp(CL2CAPConnectionSAP& aSAP) const;
	virtual void ChannelOpened(CL2CAPConnectionSAP& aSAP) const;

	// Events called from the Data Controller.
	virtual void NewData(CL2CAPConnectionSAP& aSAP) const;
	virtual TInt NewDataAsyncCallBack(CL2CAPConnectionSAP& aSAP) const;
	virtual void CanSend(CL2CAPConnectionSAP& aSAP) const;
	virtual void SDUQueueClosed(CL2CAPConnectionSAP& aSAP) const;
	virtual void DataPlaneError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;

	// Events from the security manager.
	virtual void AccessRequestComplete(CL2CAPConnectionSAP& aSignalHandler, TInt aResult) const;
	
	// State Transition Actions.
	virtual void Enter(CL2CAPConnectionSAP& aSAP) const;
	virtual void Exit(CL2CAPConnectionSAP& aSAP) const;
	
	// Helper function.
	void NotifySocketClosed(CL2CAPConnectionSAP& aSAP) const;

protected:
	void StartCalledWhileDisconnected(CL2CAPConnectionSAP& aSAP) const;
	void PanicInState(TL2CAPPanic aPanic) const;
	void DebugPanicInState(TL2CAPPanic aPanic) const;
	
protected:
	CL2CAPSAPStateFactory& iFactory;  // Used to get next state.

private:
    // Forbid copying
	TL2CAPSAPState(const TL2CAPSAPState&);
	const TL2CAPSAPState& operator=(const TL2CAPSAPState&) const;
	};




/**
   The CLOSED state.

   This is the state that newly created saps start in, and the state
   they end in after a communication has closed down or a connection
   failed.

   Basically this is a quiescent state from which the sap can go to
   different futures.
**/

NONSHARABLE_CLASS(TL2CAPSAPStateClosed) : public TL2CAPSAPState
	{
public:
	TL2CAPSAPStateClosed(CL2CAPSAPStateFactory& aFactory);

	// Events called from the SAP.
	void Start(CL2CAPConnectionSAP& aSAP) const;
	void FastShutdown(CL2CAPConnectionSAP& aSAP) const;
	void Shutdown(CL2CAPConnectionSAP& aSAP) const;
	
	void ActiveOpen(CL2CAPConnectionSAP& aSAP) const;
	void SDUQueueClosed(CL2CAPConnectionSAP& aSAP) const;

	void Bound(CL2CAPConnectionSAP& aSAP) const;
	
	// Events called from the SAP Signal Handler.
	void ChannelClosed(CL2CAPConnectionSAP& aSAP) const;
 	void SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;
	
	// Events called from the Data Controller.
	TInt NewDataAsyncCallBack(CL2CAPConnectionSAP& aSAP) const;
		
	// State Transition Actions.
	void Enter(CL2CAPConnectionSAP& aSAP) const;
	};

NONSHARABLE_CLASS(TL2CAPSAPStateChannelPendingBase) : public TL2CAPSAPState
 	{
public:
	TL2CAPSAPStateChannelPendingBase(CL2CAPSAPStateFactory& aFactory);	

	// Events called from the SAP.
	virtual void FastShutdown(CL2CAPConnectionSAP& aSAP) const;
	virtual void Shutdown(CL2CAPConnectionSAP& aSAP) const;

	// Events called from the SAP Signal Handler.
	virtual void ChannelClosed(CL2CAPConnectionSAP& aSAP) const;
 	virtual void SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;
 	};
 	
NONSHARABLE_CLASS(TL2CAPSAPStatePassiveLinkPending) : public TL2CAPSAPStateChannelPendingBase
 	{
public:
	TL2CAPSAPStatePassiveLinkPending(CL2CAPSAPStateFactory& aFactory);	

	// Events called from the SAP Signal Handler.
	void LinkUp(CL2CAPConnectionSAP& aSAP) const;
 	};

NONSHARABLE_CLASS(TL2CAPSAPStateActiveLinkPending) : public TL2CAPSAPStateChannelPendingBase
 	{
public:
	TL2CAPSAPStateActiveLinkPending(CL2CAPSAPStateFactory& aFactory);	

	// Events called from the SAP Signal Handler.
	void LinkUp(CL2CAPConnectionSAP& aSAP) const;
 	};

NONSHARABLE_CLASS(TL2CAPSAPStateActiveSecMode4AccessRequestPending) : public TL2CAPSAPStateChannelPendingBase
	{
public:
	TL2CAPSAPStateActiveSecMode4AccessRequestPending(CL2CAPSAPStateFactory& aFactory);
	
	// Events called from the SAP.
	void FastShutdown(CL2CAPConnectionSAP& aSAP) const;
	void Shutdown(CL2CAPConnectionSAP& aSAP) const;

	// Events called from the SAP Signal Handler.
	void ChannelClosed(CL2CAPConnectionSAP& aSAP) const;
 	void SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;
	
	void AccessRequestComplete(CL2CAPConnectionSAP& aSignalHandler, TInt aResult) const;
	};

NONSHARABLE_CLASS(TL2CAPSAPStateActiveChannelRequestPending) : public TL2CAPSAPStateChannelPendingBase
	{
public:
	TL2CAPSAPStateActiveChannelRequestPending(CL2CAPSAPStateFactory& aFactory);
	
	// Events called from the SAP Signal Handler.
	void ChannelOpened(CL2CAPConnectionSAP& aSAP) const;
	};

NONSHARABLE_CLASS(TL2CAPSAPStatePassiveAccessRequestPending) : public TL2CAPSAPStateChannelPendingBase
 	{
public:
	TL2CAPSAPStatePassiveAccessRequestPending(CL2CAPSAPStateFactory& aFactory);	

	// Events called from the SAP.
	void FastShutdown(CL2CAPConnectionSAP& aSAP) const;
	void Shutdown(CL2CAPConnectionSAP& aSAP) const;

	// Events called from the SAP Signal Handler.
	void ChannelClosed(CL2CAPConnectionSAP& aSAP) const;
 	void SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;

	// Events from the security manager.
	void AccessRequestComplete(CL2CAPConnectionSAP& aSignalHandler, TInt aResult) const;
 	};

NONSHARABLE_CLASS(TL2CAPSAPStateActiveSecMode2AccessRequestPending) : public TL2CAPSAPStateChannelPendingBase
 	{
public:
	TL2CAPSAPStateActiveSecMode2AccessRequestPending(CL2CAPSAPStateFactory& aFactory);	

	// Events called from the SAP.
	void FastShutdown(CL2CAPConnectionSAP& aSAP) const;
	void Shutdown(CL2CAPConnectionSAP& aSAP) const;

	// Events called from the SAP Signal Handler.
	void ChannelClosed(CL2CAPConnectionSAP& aSAP) const;
 	void SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;

	// Events from the security manager.
	void AccessRequestComplete(CL2CAPConnectionSAP& aSignalHandler, TInt aResult) const;
 	};


NONSHARABLE_CLASS(TL2CAPSAPStatePendingOpen) : public TL2CAPSAPStateChannelPendingBase
	{
public:
	TL2CAPSAPStatePendingOpen(CL2CAPSAPStateFactory& aFactory);

	// Events called from the SAP Signal Handler.
	void ChannelConfigured(CL2CAPConnectionSAP& aSAP,
	                       CL2CapChannelConfig& aConfig,
                           CL2CAPMux& aMuxer,
                           TL2CAPPort aLocalPort,
                           TL2CAPPort aRemotePort);

	// Events called from the Data Controller.
	void DataPlaneError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;	

	// State Transition Actions.
	};


NONSHARABLE_CLASS(TL2CAPSAPStateListening) : public TL2CAPSAPState
	{
public:
	TL2CAPSAPStateListening(CL2CAPSAPStateFactory& aFactory);

	// Events called from the SAP.
	void Shutdown(CL2CAPConnectionSAP& aSAP) const;
	void FastShutdown(CL2CAPConnectionSAP& aSAP) const;
	
	// Events called from the SAP Signal Handler.
	void ChannelClosed(CL2CAPConnectionSAP& aSAP) const;

	// Events called from the Data Controller.
	
	// State Transition Actions.
	void Enter(CL2CAPConnectionSAP& aSAP) const;
	void Exit(CL2CAPConnectionSAP& aSAP) const;	
	};


NONSHARABLE_CLASS(TL2CAPSAPStateOpen) : public TL2CAPSAPState
	{
public:
	TL2CAPSAPStateOpen(CL2CAPSAPStateFactory& aFactory);

	// Events called from the SAP.
	virtual TInt Send(CL2CAPConnectionSAP& aSAP, RMBufChain& aData, TUint aFlag) const;
	virtual TInt Read(CL2CAPConnectionSAP& aSAP, RMBufChain& aData) const;

	virtual void Shutdown(CL2CAPConnectionSAP& aSAP) const;
	virtual void FastShutdown(CL2CAPConnectionSAP& aSAP) const;
	
	// Events called from the SAP Signal Handler.
	virtual void ReconfiguringChannel(CL2CAPConnectionSAP& aSAP) const;
	virtual void ChannelConfigured(CL2CAPConnectionSAP& aSAP,
	                       		   CL2CapChannelConfig& aConfig,
                                   CL2CAPMux& aMuxer,
                                   TL2CAPPort aLocalPort,
                                   TL2CAPPort aRemotePort);
	virtual void SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;
	virtual void ChannelClosed(CL2CAPConnectionSAP& aSAP) const;
	virtual void CloseOutgoingSDUQueue(CL2CAPConnectionSAP& aSAP) const;

	// Events called from the Data Controller.
	virtual void NewData(CL2CAPConnectionSAP& aSAP) const;
	virtual TInt NewDataAsyncCallBack(CL2CAPConnectionSAP& aSAP) const;
	virtual void CanSend(CL2CAPConnectionSAP& aSAP) const;
	virtual void DataPlaneError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;	
	
	// State Transition Actions.
	};

NONSHARABLE_CLASS(TL2CAPSAPStateAccepting) : public TL2CAPSAPStateOpen
	{
public:
	TL2CAPSAPStateAccepting(CL2CAPSAPStateFactory& aFactory);

	// Events called from the SAP.
	void Start(CL2CAPConnectionSAP& aSAP) const;
	void NewData(CL2CAPConnectionSAP& aSAP) const;
	
	// Events called from the SAP Signal Handler.
	void ReconfiguringChannel(CL2CAPConnectionSAP& aSAP) const;
	
	// Events called from the Data Controller.
	TInt NewDataAsyncCallBack(CL2CAPConnectionSAP& aSAP) const;
	void CanSend(CL2CAPConnectionSAP& aSAP) const;
	
	// State Transition Actions.
	void Exit(CL2CAPConnectionSAP& aSAP) const;	
	};

/**
This state exists to determine if the other side believes the link is opened.
It won't send us data until it has finished configuring the channel.  On
reception of data the Signal Handler will be informed before moving the state
machine on to fully open.
*/
NONSHARABLE_CLASS(TL2CAPSAPStateAwaitingInitialData) : public TL2CAPSAPStateOpen
	{
public:
	TL2CAPSAPStateAwaitingInitialData(CL2CAPSAPStateFactory& aFactory);
	
	// Events called from the SAP Signal Handler.
	void ReconfiguringChannel(CL2CAPConnectionSAP& aSAP) const;

	// Events called from the Data Controller.
	void NewData(CL2CAPConnectionSAP& aSAP) const;
	};

NONSHARABLE_CLASS(TL2CAPSAPStateDisconnecting) : public TL2CAPSAPState
	{
public:
	TL2CAPSAPStateDisconnecting(CL2CAPSAPStateFactory& aFactory);

	// Events called from the SAP.
	void Start(CL2CAPConnectionSAP& aSAP) const;
	TInt Send(CL2CAPConnectionSAP& aSAP, RMBufChain& aData, TUint aFlag) const;
	TInt Read(CL2CAPConnectionSAP& aSAP, RMBufChain& aData) const;

	void Shutdown(CL2CAPConnectionSAP& aSAP) const;
	void FastShutdown(CL2CAPConnectionSAP& aSAP) const;

	
	// Events called from the SAP Signal Handler.
	void ChannelConfigured(CL2CAPConnectionSAP& aSAP,
						   CL2CapChannelConfig& aConfig,
						   CL2CAPMux& aMuxer,
						   TL2CAPPort aLocalPort,
						   TL2CAPPort aRemotePort);

	void ReconfiguringChannel(CL2CAPConnectionSAP& aSAP) const;

	void SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;
	void ChannelClosed(CL2CAPConnectionSAP& aSAP) const;
	void CloseOutgoingSDUQueue(CL2CAPConnectionSAP& aSAP) const;
	
	// Events called from the Data Controller.
	void SDUQueueClosed(CL2CAPConnectionSAP& aSAP) const;
	void NewData(CL2CAPConnectionSAP& aSAP) const;
	TInt NewDataAsyncCallBack(CL2CAPConnectionSAP& aSAP) const;
	void CanSend(CL2CAPConnectionSAP& aSAP) const;
	void DataPlaneError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const;	
	
	// State Transition Actions.
	};


NONSHARABLE_CLASS(TL2CAPSAPStateError) : public TL2CAPSAPState
	{
public:
	TL2CAPSAPStateError(CL2CAPSAPStateFactory& aFactory);

	// Events called from the SAP.
	void Start(CL2CAPConnectionSAP& aSAP) const;

	TInt Ioctl(CL2CAPConnectionSAP& aSAP, TUint aLevel, TUint aName, TDes8* aOption) const;
	void CancelIoctl(CL2CAPConnectionSAP& aSAP, TUint aLevel, TUint aName) const;
	void IoctlComplete(CL2CAPConnectionSAP& aSAP, TInt aErr, TUint aLevel, TUint aName, TDesC8* aBuf) const;

	void ActiveOpen(CL2CAPConnectionSAP& aSAP) const;
	TInt PassiveOpen(CL2CAPConnectionSAP& aSAP, TUint aQueSize) const;
	void Shutdown(CL2CAPConnectionSAP& aSAP) const;
	void FastShutdown(CL2CAPConnectionSAP& aSAP) const;
	
	TInt Send(CL2CAPConnectionSAP& aSAP, RMBufChain& aData, TUint aFlag) const;
	TInt Read(CL2CAPConnectionSAP& aSAP, RMBufChain& aData) const;

	// Events called from the SAP Signal Handler.
	void ChannelClosed(CL2CAPConnectionSAP& aSAP) const;
	void SignalHandlerError(CL2CAPConnectionSAP& /*aSAP*/, TInt /*aErrorCode*/, MSocketNotify::TOperationBitmasks /*aErrorAction*/) const;

	// State Transition Actions.
	void Enter(CL2CAPConnectionSAP& aSAP) const;	
	};

/**
   The BOUND state.

   This is the state that freshly bound saps are in. This state should 
   only be reached once for each sap since ESOCK doesn't allow rebinding 
   or unbinding of saps. 

   A sap can be bound from an ESOCK point of view but not be in this state.
   Then the sap will either be on the listening path or Closed.
**/

NONSHARABLE_CLASS(TL2CAPSAPStateBound) : public TL2CAPSAPState
	{
public:
	TL2CAPSAPStateBound(CL2CAPSAPStateFactory& aFactory);

	// Events called from the SAP.
	TInt PassiveOpen(CL2CAPConnectionSAP& aSAP, TUint aQueSize) const;	
	void Shutdown(CL2CAPConnectionSAP& aSAP) const;
	void FastShutdown(CL2CAPConnectionSAP& aSAP) const;

	// State Transition Actions.
	void Enter(CL2CAPConnectionSAP& aSAP) const;
	void Exit(CL2CAPConnectionSAP& aSAP) const;	
	};
	

NONSHARABLE_CLASS(CL2CAPSAPStateFactory) : public CBase
	{
friend class CL2CAPProtocol;	
public:
	enum TStates
		{
		EClosed = 0,
		EPassiveLinkPending,
		EActiveLinkPending,
		EPassiveAccessRequestPending,
		EActiveSecMode2AccessRequestPending,
		EPendingOpen,
		EListening,
		EAccepting,
		EAwaitingInitialData,
		EOpen,
		EDisconnecting,
		EError,
		EBound,
		EActiveSecMode4AccessRequestPending,
		EActiveChannelRequestPending,
		EMaxState,
		};
	
	TL2CAPSAPState& GetState(const TStates aState) const;
	TInt StateIndex(const TL2CAPSAPState* aState) const;

	~CL2CAPSAPStateFactory();

private:
	static CL2CAPSAPStateFactory* NewL();
	CL2CAPSAPStateFactory();

	TFixedArray<TL2CAPSAPState*, EMaxState> iStates;
	};

	
#endif
