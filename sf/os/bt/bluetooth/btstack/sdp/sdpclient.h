// Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SDPCLIENT_H
#define SDPCLIENT_H

#include <es_prot.h>
#include <bttypes.h>

class CSdpPdu;
class CSdpNetDbProvider;
class CSdpProtocol;
class CProtocolBase;
class TUUID;

/**
   Acts as the local SDP client Agent, for a given remote device.
   
   There is one of these per remote device.  This class
   acts as an internal socket binding to a lower level SAP.  Thus we
   derive from MSocketNotify so that the SAP can directly notify us of events.
**/
NONSHARABLE_CLASS(CSdpClient) : public CBase, private MSocketNotify, private MProvdSecurityChecker
	{
friend class CSdpProtocol;
public:
	// Create/destroy
	static CSdpClient* NewL(CSdpProtocol& aProt, CProtocolBase& aL2CAP);
	~CSdpClient();
	// Interface for the Protocol to request services
	void Open(TBTDevAddr& aAddr);
	void AddNetDbProvider(CSdpNetDbProvider& aNetDbProvider);
	void RemoveNetDbProvider(CSdpNetDbProvider& aNetDbProvider);

	// Interface for the NetDbProviders to request actions
	void ServiceSearchRequest(CSdpNetDbProvider& aNetDbProvider,
		TUint16 aMaxCount, const TUUID &aUUID, const TDesC8& aContState);
	void ServiceAttributeRequest(CSdpNetDbProvider& aNetDbProvider,
		TUint aRecordHandle, TUint16 aMaxLength, TBool aRange,
		TUint aAttribute, const TDesC8& aContState);
	void EncodedRequest(CSdpNetDbProvider& aNetDbProvider,
						TUint8 aPduRequestId,
						const TDesC8& aEncodedData);
	
	
	// From MSocketNotify
	void NewData(TUint aCount);
	void CanSend();
	void ConnectComplete();
	void ConnectComplete(const TDesC8& aConnectData);
	void ConnectComplete(CServProviderBase& aSSP);
	void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	void CanClose(TDelete aDelete=EDelete);
	void CanClose(const TDesC8& aDisconnectData,TDelete aDelete=EDelete);
	void Error(TInt aError,TUint aOperationMask=EErrorAllOperations);
	void Disconnect(void);
	void Disconnect(TDesC8& aDisconnectData);
	void IoctlComplete(TDesC8 *aBuf);
	void NoBearer(const TDesC8& /*aConnectionInf*/) {};
	void Bearer(const TDesC8& /*aConnectionInf*/) {};

private:
	// From MProvdSecurityChecker
	TInt CheckPolicy(const TSecurityPolicy& aPolicy, const char *aDiagnostic);
	
	// Create/destroy
	CSdpClient(CSdpProtocol& aProtocol, CProtocolBase& aL2CAP); 
	void ConstructL();

	// Out & In PDU handling could be generalised in a CSDP base, which
	// the SDP client and server specialise as appropriate.

	// Processing incoming pdus
	void ParseNextPacket();
	void HandlePDU(TUint8 aPduId, TUint16 aTransId, const TDesC8& aParams);
	void HandleErrorResponse(CSdpNetDbProvider* aNetDbProv, const TDesC8& aParams);
	void HandleServiceSearchResponse(CSdpNetDbProvider* aNetDbProv, const TDesC8& aParams);
	void HandleServiceAttributeResponse(CSdpNetDbProvider* aNetDbProv, const TDesC8& aParams);

	// Outgoing pdus
	CSdpPdu* NewPdu(CSdpNetDbProvider* aNetDb);
	TUint16 NextTransId();

	void EnqueOutboundPdu(CSdpPdu& aPdu);
	void TryToSend();

	// Hide these to make replacing with bitmask easier
	TBool ChannelUp() const;
	void ChannelUp(TBool aState);
	
	TBool RequestOutstanding() const;
	void RequestOutstanding(TBool aBool);   // True if OK to send

	TBool L2CAPBlocked() const;
	void L2CAPBlocked(TBool aState);
	
	// Misc
	void CheckForIdle(TInt aTimeoutInSeconds);
	void CancelIdleTimer();
	static TInt IdleTimerExpired(TAny* aPtr);
	
private:
	// Data items
	TDblQue<CSdpNetDbProvider> iNetDbProviders;  // NetDbProviders using this Client
	TDblQue<CSdpPdu> iOutboundQ;	// PDUs to be sent
	CSdpProtocol& iProtocol;
	CProtocolBase& iL2CAP;
	CServProviderBase* iBoundSAP;
	TBTDevAddr  iRemoteAddr;

	// Reassembly buffer
	HBufC8*  iNextPacket;   // Next packet coming up from L2CAP

	// Client state
	TBool  iClientChannelUp;
	TBool  iRequestOutstanding; // A request has been sent
	TBool  iL2CAPSendBlocked;   // Is L2CAP ok to send data?
	TUint16 iTransId;		// Last used transaction ID for this client

	// Clean up stuff
	TDeltaTimerEntry iIdleTimerEntry;  //< Idle timer, that kills us eventually
	TBool iIdleQueued;		//< Idle timer is on Q.

	// Go on a Q
	TDblQueLink iLink;
	};


inline TBool CSdpClient::ChannelUp() const
	{return iClientChannelUp;}

inline void CSdpClient::ChannelUp(TBool aState)
	{iClientChannelUp=aState;}

inline TBool CSdpClient::RequestOutstanding() const
	{return iRequestOutstanding;}

inline void CSdpClient::RequestOutstanding(TBool aBool)
	{iRequestOutstanding=aBool;}

inline TBool CSdpClient::L2CAPBlocked() const
	{return iL2CAPSendBlocked;}

inline void CSdpClient::L2CAPBlocked(TBool aState)
	{iL2CAPSendBlocked=aState;}



#endif
