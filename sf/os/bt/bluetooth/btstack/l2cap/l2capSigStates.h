// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the signalling state machine for an L2Cap Channel
// 
//

#ifndef L2CAPSIGSTATES_H
#define L2CAPSIGSTATES_H


#include <e32base.h>
#include <bt_sock.h>
#include <es_prot.h>

#include "L2CapChannelConfig.h"

class CL2CapSAPSignalHandler;
class CL2CAPSignalStateFactory;
class CL2CAPConnectionSAP;
class TL2CAPSigState;

class HConnectionResponse;
class HConfigureRequest;
class HConfigureResponse;


// Factory class for the L2CAP state machine.
NONSHARABLE_CLASS(CL2CAPSignalStateFactory) : public CBase
	{
public:
	enum TSigStates
		{
		EClosed,
		EWaitConnectRsp,
		EWaitConnect,
		EWaitConfig,
		EWaitSendConfig,
		EWaitConfigReqRsp,
		EWaitConfigRsp,
		EWaitConfigReq,
		EOpen,
		EWaitDisconnect,
		EMaxState,
		};

	TL2CAPSigState& GetState(const TSigStates aState) const;
	TInt StateIndex(const TL2CAPSigState* aState) const;
	
	static CL2CAPSignalStateFactory* NewL();
	~CL2CAPSignalStateFactory();

private:
	CL2CAPSignalStateFactory();
	void ConstructL();

private:	
	TFixedArray<TL2CAPSigState*, EMaxState> iSigStates;
	};


// Base class for the L2CAP state machine.
NONSHARABLE_CLASS(TL2CAPSigState)
	{
public:
	friend class CL2CapSAPSignalHandler;

	TL2CAPSigState(const CL2CAPSignalStateFactory& aFactory);
	
	// Events from the SAP
	virtual void CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const;
	virtual void OpenChannel(CL2CapSAPSignalHandler& aSignalHandler) const;
	virtual void ConnectRequestReceived(CL2CapSAPSignalHandler& aSignalHandler) const;
	virtual void ConfigureChannel(CL2CapSAPSignalHandler& aSignalHandler) const;
	virtual TInt UpdateChannelConfig(CL2CapSAPSignalHandler& aSignalHandler, const TL2CapConfig& aAPIConfig) const;

	virtual TInt GetNegotiatedChannelMode(const CL2CapSAPSignalHandler& aSignalHandler, TL2CapChannelMode& aMode) const;

	// L2CAP commands received from the peer.
	virtual void ConnectResponse(CL2CapSAPSignalHandler& aSignalHandler,
								 HConnectionResponse* aConnectionResponse) const;
	
	virtual void ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
							   HConfigureRequest* aConfigRequest) const;
	virtual void ConfigResponse(CL2CapSAPSignalHandler& aSignalHandler,
 		                        HConfigureResponse* aConfigResponse) const;
	virtual void DisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const;
	virtual void DisconnectResponse(CL2CapSAPSignalHandler& aSignalHandler) const;

	// Events from the Mux
	virtual void Error(CL2CapSAPSignalHandler& aSignalHandler,
	                   TInt aErrorCode, 
	                   MSocketNotify::TOperationBitmasks aErrorAction) const;

	// Change of state events
	virtual void Enter(CL2CapSAPSignalHandler& aSignalHandler) const;
	virtual void Exit(CL2CapSAPSignalHandler& aSignalHandler) const;
	
	// Signal command queue event
	virtual void PendingCommandsDrained(CL2CapSAPSignalHandler& aSignalHandler) const;

	// Timers
	virtual void ConfigurationTimerExpiry(CL2CapSAPSignalHandler& aSignalHandler) const;

	// Helpers
	virtual TBool IsChannelClosed() const;
	virtual TBool IsChannelOpen() const;
	void HandleDisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const;
	void HandleSAPDisconnect(CL2CapSAPSignalHandler& aSignalHandler) const;
	TInt SendConfigRequest(CL2CapSAPSignalHandler& aSignalHandler) const;
	TInt SendConfigResponse(CL2CapSAPSignalHandler& aSignalHandler, TConfigResponseResult aResult) const;
	void DisconnectWithError(CL2CapSAPSignalHandler& aSignalHandler, TInt aError) const;

protected:
	void PanicInState(TL2CAPPanic aPanic) const;

protected:
	const CL2CAPSignalStateFactory& iFactory;  // Get states
	};


/**
   The closed state.

   This is the state that newly created signal handlers start in, and the state
   they end in after a communication has closed down or a connection
   failed.
*/
NONSHARABLE_CLASS(TL2CAPSigStateClosed) : public TL2CAPSigState
	{
public:
	TL2CAPSigStateClosed(const CL2CAPSignalStateFactory& aFactory);

	// Events from the SAP
	void OpenChannel(CL2CapSAPSignalHandler& aSignalHandler) const;
	void ConnectRequestReceived(CL2CapSAPSignalHandler& aSignalHandler) const;
	void CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const;

	void ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
					   HConfigureRequest* aConfigRequest) const;

	// Change of state state events
	void Enter(CL2CapSAPSignalHandler& aSignalHandler) const;

	// Helpers
	TBool IsChannelClosed() const;
	};

NONSHARABLE_CLASS(TL2CAPSigStateWaitConnectRsp) : public TL2CAPSigState
/*
   Waiting for a response to our ConnectRequest.
*/
	{
public:
	TL2CAPSigStateWaitConnectRsp(const CL2CAPSignalStateFactory& aFactory);

	// Events from the SAP
	void CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const;

	// L2CAP commands received from the peer.
	void ConnectResponse(CL2CapSAPSignalHandler& aSignalHandler,
						 HConnectionResponse* aConnectionResponse) const;
	void ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
						 HConfigureRequest* aConfigRequest) const;				 
	
	// Change of state state events
	void Enter(CL2CapSAPSignalHandler& aSignalHandler) const;
	};

NONSHARABLE_CLASS(TL2CAPSigStateWaitConnect) : public TL2CAPSigState
 	{
public:
	TL2CAPSigStateWaitConnect(const CL2CAPSignalStateFactory& aFactory);	

	// Events from the SAP
	void OpenChannel(CL2CapSAPSignalHandler& aSignalHandler) const;
	void CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const;
	
	// L2CAP commands received from the peer.
	void DisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const;

	// Change of state state events
	void PendingCommandsDrained(CL2CapSAPSignalHandler& aSignalHandler) const;

	void Enter(CL2CapSAPSignalHandler& aSignalHandler) const;
	};
	
NONSHARABLE_CLASS(TL2CAPSigStateConfigBase) : public TL2CAPSigState
/*
	This is the base class for the config states. It will NEVER be instantiated itself.
*/
	{
public:
	using TL2CAPSigState::ConfigRequest;
	using TL2CAPSigState::ConfigResponse;
	TL2CAPSigStateConfigBase(const CL2CAPSignalStateFactory& aFactory);	

	// L2CAP commands received from the peer.
	void DisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const;

	void Error(CL2CapSAPSignalHandler& aSignalHandler,
	           TInt aErrorCode, 
	           MSocketNotify::TOperationBitmasks aErrorAction) const;

	// Events from the SAP
	void CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const;
	TInt UpdateChannelConfig(CL2CapSAPSignalHandler& aSignalHandler, const TL2CapConfig& aAPIConfig) const;

	// Timer Events
	void ConfigurationTimerExpiry(CL2CapSAPSignalHandler& aSignalHandler) const;

	// Common handling of Config Request and Response commands.
	virtual void ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
					   HConfigureRequest* aConfigRequest,
					   CL2CAPSignalStateFactory::TSigStates aConfigSuccessState) const;
	virtual void ConfigResponse(CL2CapSAPSignalHandler& aSignalHandler,
 		                HConfigureResponse* aConfigResponse,
 		                CL2CAPSignalStateFactory::TSigStates aConfigSuccessState) const;
	};

	
NONSHARABLE_CLASS(TL2CAPSigStateWaitConfig) : public TL2CAPSigStateConfigBase
/*
	No config requests/responses sent or received.
	Waiting to receive peer L2Cap supported features.
*/
	{
public:
	TL2CAPSigStateWaitConfig(const CL2CAPSignalStateFactory& aFactory);	

	// Events from the SAP
	void ConfigureChannel(CL2CapSAPSignalHandler& aSignalHandler) const;

	// L2CAP commands received from the peer.
	void ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
					   HConfigureRequest* aConfigRequest) const;

	// Change of state state events
	void Enter(CL2CapSAPSignalHandler& aSignalHandler) const;
	};

NONSHARABLE_CLASS(TL2CAPSigStateWaitSendConfig) : public TL2CAPSigStateConfigBase
/*
	Remote's config request received and responded to with 'success'.
	Waiting to send outgoing config request...
	...waiting to receive peer L2Cap supported features.
*/
	{
public:
	TL2CAPSigStateWaitSendConfig(const CL2CAPSignalStateFactory& aFactory);	

	// Events from the SAP
	void ConfigureChannel(CL2CapSAPSignalHandler& aSignalHandler) const;
	};

NONSHARABLE_CLASS(TL2CAPSigStateWaitConfigReqRsp) : public TL2CAPSigStateConfigBase
/*
	Outgoing config request sent.
	Waiting for remote's config request, and remote's config response.
*/
	{
public:
	TL2CAPSigStateWaitConfigReqRsp(const CL2CAPSignalStateFactory& aFactory);	

	// L2CAP commands received from the peer.
	void ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
					   HConfigureRequest* aConfigRequest) const;
	void ConfigResponse(CL2CapSAPSignalHandler& aSignalHandler,
 	                    HConfigureResponse* aConfigResponse) const;
	};

NONSHARABLE_CLASS(TL2CAPSigStateWaitConfigRsp) : public TL2CAPSigStateConfigBase
/*
	Remote's config request received and responded to with 'success'.
	Outgoing config request sent.
	Waiting for remote's config response.
*/
	{
public:
	TL2CAPSigStateWaitConfigRsp(const CL2CAPSignalStateFactory& aFactory);	

	// L2CAP commands received from the peer.
	void ConfigResponse(CL2CapSAPSignalHandler& aSignalHandler,
 	                    HConfigureResponse* aConfigResponse) const;
	};

NONSHARABLE_CLASS(TL2CAPSigStateWaitConfigReq) : public TL2CAPSigStateConfigBase
/*
	Outgoing config request sent, and responded to with 'success'.
	Waiting for remote's config request.
*/
	{
public:
	TL2CAPSigStateWaitConfigReq(const CL2CAPSignalStateFactory& aFactory);	

	// L2CAP commands received from the peer.
	void ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
					   HConfigureRequest* aConfigRequest) const;
	};
	

NONSHARABLE_CLASS(TL2CAPSigStateOpen) : public TL2CAPSigState
/*
	L2Cap logical link open. Data can be sent and received.
*/
	{
public:
	TL2CAPSigStateOpen(const CL2CAPSignalStateFactory& aFactory);	

	// Events from the SAP
	void CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const;

	// L2CAP commands received from the peer.
	void DisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const;
	void ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,HConfigureRequest* aConfigRequest) const; 

	// Config state events
	TInt UpdateChannelConfig(CL2CapSAPSignalHandler& aSignalHandler, const TL2CapConfig& aAPIConfig) const;

	// Change of state state events
	void Enter(CL2CapSAPSignalHandler& aSignalHandler) const;

	TBool IsChannelOpen() const;
	TInt GetNegotiatedChannelMode(const CL2CapSAPSignalHandler& aSignalHandler, TL2CapChannelMode& aMode) const;
	};

NONSHARABLE_CLASS(TL2CAPSigStateWaitDisconnect) : public TL2CAPSigState
/*
	Logical link still exists but is waiting to be disconnected. 
	NB Signalling can still be received, and should be handled.
*/
	{
public:
	TL2CAPSigStateWaitDisconnect(const CL2CAPSignalStateFactory& aFactory);	

	// Events from the SAP
	void CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const;
	TInt UpdateChannelConfig(CL2CapSAPSignalHandler& aSignalHandler, const TL2CapConfig& aAPIConfig) const;

	// State changes from the Mux
	void DisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const;
	void DisconnectResponse(CL2CapSAPSignalHandler& aSignalHandler) const;

	// Change of state state events
	void Enter(CL2CapSAPSignalHandler& aSignalHandler) const;
	void PendingCommandsDrained(CL2CapSAPSignalHandler& aSignalHandler) const;
	void ConnectResponse(CL2CapSAPSignalHandler& aSignalHandler,
					   HConnectionResponse* aConnectionResponse) const;
	void ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
					   HConfigureRequest* aConfigRequest) const;

	void ConfigResponse(CL2CapSAPSignalHandler& /*aSignalHandler*/,
 	                	HConfigureResponse* /*aConfigResponse*/) const;
	void ConfigureChannel(CL2CapSAPSignalHandler& /*aSignalHandler*/) const;
	};

#endif
