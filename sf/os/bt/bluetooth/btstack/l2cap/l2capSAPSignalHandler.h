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
//

#ifndef L2CAPSAPSIGNALHANDLER_H
#define L2CAPSAPSIGNALHANDLER_H

#include <e32std.h>
#include <e32base.h>

#include "L2CapChannelConfig.h"
#include "l2capSigStates.h"
#include "bttypes.h"
#include "l2capSignalHandler.h"
#include "l2capSigPacketEcho.h"
#include "l2sap.h"

class HConnectionResponse;
class HConnectionRequest;
class HConfigureRequest;
class HConfigureResponse;
class HDisconnectRequest;
class HDisconnectResponse;
class HCommandReject;

class TL2CAPEntityConfig;

class CL2CAPConnectionSAP;
class CL2CAPSignalStateFactory;

NONSHARABLE_CLASS(CL2CapSAPSignalHandler) : public CL2CapSignalHandler,
                               public MEchoResponseHandler
	{
public:
	static CL2CapSAPSignalHandler* NewL(CL2CAPConnectionSAP& aSAP);

	virtual ~CL2CapSAPSignalHandler();

	// Prepare outgoing commands
	TInt UpdateChannelConfig(const TL2CapConfig& aAPIConfig);
	void CloneChannelConfig(CL2CapChannelConfig& aL2CAPChannelConfig);

	TInt ConstructConnectionResponse(TUint8 aId, TConnectResponseResult aResult, TConnectResponseStatus aStatus = EConnectPendNoFurtherInfo);
	TInt ConstructConnectionRequest();
	TInt ConstructConfigRequest();
	TInt ConstructConfigResponse(TUint8 aId, TConfigFlags aFlags, TConfigResponseResult aResult, RMBufChain& aUnknownParameters);
	TInt ConstructConfigResponse(TUint8 aId, TConfigFlags aFlags, TConfigResponseResult aResult);
    TInt ConstructDisconnectRequest();
	TInt ConstructDisconnectResponse(TUint8 aId);

	void L2CapEntityConfigUpdated();
	inline TBool IsPeerInfoDefined();
	void ConfigRequestDelayTimerExpiry();	
	TBool DelayConfigRequest();

	TBool HandleConnectionResponse(HConnectionResponse* aConnectionResponse);
	TBool HandleConfigureRequest(HConfigureRequest* aConfigRequest);
	TBool HandleConfigureResponse(HConfigureResponse* aConfigResponse);	
	TBool HandleDisconnectRequest(HDisconnectRequest* aDisconnectRequest);	
	TBool HandleDisconnectResponse(HDisconnectResponse* aDisconnectResponse);	
	TBool HandleCommandReject(HCommandReject* aCommandReject);

	void CommandResponseFailure(HL2CapCommand* aCommand);

	TInt PassiveConnectionRequest(const TBTDevAddr& aAddr, HConnectionRequest* aConnectionRequest);
	void ActiveConnectionRequest();

	void ConnectRequestReceived();
	void OpenChannelRequest();
	void ConfigureChannelRequest();
	void CloseChannelRequest();

	void SAPSignalHandlerRegistered(CL2CAPMux& aMuxer, TL2CAPEntityConfig& aEntityConfig);
	void ReconfiguringChannel();

	void SignalHandlerDisconnectedCanClose();
	void SignalHandlerErrorD(TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction);

	void Error(TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction);
	
	void LinkUp();

	inline TBool IsChannelClosed() const;
	inline TBool IsChannelOpen() const;
	inline TInt GetNegotiatedChannelMode(TL2CapChannelMode& aMode) const;
	
	void SetLocalParameters(TL2CAPSockAddr& aAddr);
	void GetLocalParameters(TL2CAPSockAddr& aAddr) const;
	void SetRemName(TL2CAPSockAddr& aAddr);
	void GetRemName(TL2CAPSockAddr& aAddr) const;

	inline TUint8 RTXTimerDuration() const;
	inline TUint16 ERTXTimerDuration() const;
	TInt SetRTXTimerDuration(TUint8 aDuration);
	TInt SetERTXTimerDuration(TUint16 aDuration);

	void SetState(TL2CAPSigState& aState);
	
	TInt SetOption(TUint aLevel,TUint aName,const TDesC8 &aOption);
	TInt GetOption(TUint aLevel,TUint aName,TDes8& aOption)const;		

	inline CL2CAPConnectionSAP* SAP();
	inline const TBTDevAddr& RemoteBTAddress() const;

	void SetLocalPort(TL2CAPPort aPort);
	inline TL2CAPPort LocalPort() const;
	void SetRemotePort(TL2CAPPort aPort);
	inline TL2CAPPort RemotePort() const;

	inline CL2CapChannelConfig& ChannelConfig();
	inline const CL2CapChannelConfig& ChannelConfig() const;
		
	TUint8 GetOutstandingRequestID();
	void SetOutstandingRequestID(TUint8 aOutstandingRequestID);
	
	// SAP signal handler timer methods.	
	static TInt TimerExpired(TAny* aSAPSignalHandler);

	void StartConfigurationTimer();
	void StartConfigRequestDelayTimer();
	void CancelTimer();

	void DetachFromMux();
	void SAPClosed();

	TInt SendEchoRequest(const TDes8* aData);
	void EchoResponseReceived(const TDesC8* aData);

	inline void ConfigureChannel();

	void OverrideParkMode();
	void UndoOverrideParkMode();
	void OverrideLPM();
	void UndoOverrideLPM();

	inline TInt SignalHandlerErrorCode() const;
	inline MSocketNotify::TOperationBitmasks SignalHandlerErrorAction() const;
	inline void SetSignalHandlerErrorCode(TInt aError);
	inline void SetSignalHandlerErrorAction(MSocketNotify::TOperationBitmasks aAction);
private:
	CL2CapSAPSignalHandler(CL2CAPConnectionSAP& aSAP);
	void ConstructL();
	
	void PendingCommandsDrained();
	void HandleTimerExpired();

public:
	TDblQueLink iLink;	
private:
	enum TSAPSignalHandlerTimerState
		{
		ETimerIdle,
		EConfigRequestDelayTimer,
		EConfigurationTimer,
		};
		
	CL2CAPConnectionSAP* iSAP;

	TBTDevAddr iLocalBTAddress;
	TL2CAPPort iLocalPort;

	TL2CAPPort iRemotePort;

	TUint8 iDefaultRTXTimerDuration;
	TUint16 iERTXTimerDuration;
	
	CL2CapChannelConfig* iChannelConfig;
	TL2CAPSigState* iSigState;  				// Our signalling base state object

	TUint8 iOutstandingRequestID;

	TSAPSignalHandlerTimerState iSAPSignalHandlerTimerState;
	TDeltaTimerEntry iSAPSignalHandlerTimerEntry;
	
	TInt iSignalHandlerErrorCode;
	MSocketNotify::TOperationBitmasks iSignalHandlerErrorAction;

	// We need the peer supported features to be able to handle a Config Request coming
	// through (to be able to negotiate channel mode properly), which may be any time after 
	// OpenChannelRequest, so OpenChannelRequest is delayed until Information Response comes
	// through.
	TBool iOpenChannelRequestAwaitingPeerEntityConfig;
	// A ConfigureChannelRequest from the SAP is only expected in an active open scenario.
	// On passive open we kick the configuration ourselves.
	TBool iAwaitConfigureChannelRequest;

	friend class TL2CAPSigStateClosed;
	friend class TL2CAPSigStateWaitConnectRsp;
	friend class TL2CAPSigStateWaitConnect;
	friend class TL2CAPSigStateConfigBase;
	};

inline TUint8 CL2CapSAPSignalHandler::GetOutstandingRequestID()
	{
	return iOutstandingRequestID;
	}	

inline void CL2CapSAPSignalHandler::SetOutstandingRequestID(TUint8 aOutstandingRequestID)
	{
	iOutstandingRequestID = aOutstandingRequestID;
	}	

inline CL2CapChannelConfig& CL2CapSAPSignalHandler::ChannelConfig()
	{
	return *iChannelConfig;
	}

inline const CL2CapChannelConfig& CL2CapSAPSignalHandler::ChannelConfig() const
	{
	return *iChannelConfig;
	}

inline TInt CL2CapSAPSignalHandler::GetNegotiatedChannelMode(TL2CapChannelMode& aMode) const
	{
	return iSigState->GetNegotiatedChannelMode(*this, aMode);
	}

inline CL2CAPConnectionSAP* CL2CapSAPSignalHandler::SAP()
	{
	return iSAP;
	}
	
inline TL2CAPPort CL2CapSAPSignalHandler::LocalPort() const
	{
	return iLocalPort;
	}
inline TL2CAPPort CL2CapSAPSignalHandler::RemotePort() const
	{
	return iRemotePort;
	}

inline const TBTDevAddr& CL2CapSAPSignalHandler::RemoteBTAddress() const
	{
	return iSAP->RemoteDev();
	}

inline TBool CL2CapSAPSignalHandler::IsChannelClosed() const
	{
	return iSigState->IsChannelClosed();
	}

inline TBool CL2CapSAPSignalHandler::IsChannelOpen() const
	{
	return iSigState->IsChannelOpen();
	}

inline TUint8 CL2CapSAPSignalHandler::RTXTimerDuration() const
	{
	return iDefaultRTXTimerDuration;
	}
	
inline TUint16 CL2CapSAPSignalHandler::ERTXTimerDuration() const
	{
	return iERTXTimerDuration;
	}

inline TInt CL2CapSAPSignalHandler::SignalHandlerErrorCode() const
	{
	return iSignalHandlerErrorCode;
	}
	
inline MSocketNotify::TOperationBitmasks CL2CapSAPSignalHandler::SignalHandlerErrorAction() const	
	{
	return iSignalHandlerErrorAction;
	}

inline void CL2CapSAPSignalHandler::SetSignalHandlerErrorCode(TInt aError)
	{
	iSignalHandlerErrorCode = aError;
	}

inline TBool CL2CapSAPSignalHandler::IsPeerInfoDefined()
	{
	return iChannelConfig->IsPeerInfoDefined();
	}

inline void CL2CapSAPSignalHandler::SetSignalHandlerErrorAction(MSocketNotify::TOperationBitmasks aAction)
	{
	iSignalHandlerErrorAction = aAction;
	}

inline void CL2CapSAPSignalHandler::ConfigureChannel()
	{
	iSigState->ConfigureChannel(*this);
	}

#endif
