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
// Defines the avdtp signalling channel
// Signalling channel - not a transport channel by type, but called "signalling channel" as it maps very
// clearly onto that defined in the AVDTP specification
// There is at most one signalling channel for a remote device.  Many applications can use the signalling
// channel via the multiplexing of signalling sessions.
// The signalling channel is given a logical channel (l2cap shannel) to run on from the logical channel factory
// The signalling channels responsibilities are:
// - offer signalling protocol send primitives
// - offer incoming signalling protocol indications to all the attached signalling sessions
// - provide a mechanism to patch up deficiencies in the specification (eg providing SEIDs in protocol confirms
// to signalling sessions that call send primitives
// All signalling sessions on a listening signalling channel become connected once the listening channel is
// connected. 
// All signalling sessions on a listening signalling channel are transfered to the new connected
// signalling channel upon the completion of an active open. The listening signalling channel is
// left to continue listening. New sessions attached to it will not be connected to the existing 
// channel.
// The signalling channel does not parse incoming messages. Incoming data from the logical channel is
// given to the incoming message.  The incoming message builds and parses itself.
// The signalling message then calls the Handle methods on the signalling channel which liases with the
// signalling sessions.
// 
//

#ifndef AVDTPSIGNALLINGCHANNEL_H
#define AVDTPSIGNALLINGCHANNEL_H

#include <es_prot.h>
#include "avdtpSignallingMessages.h"
#include "avdtpLogicalChannelFactory.h"
#include "avdtpSEPCache.h"
#include "avdtpAllocators.h"

class CAvdtpProtocol;
class XAvdtpSignalReceiver;
class RMBufChain;
class CSignallingTransaction;

static const TInt KAvdtpReleaseAcceptPriority = CActive::EPriorityHigh;
#ifdef _OOM_TEST
static const TInt KAvdtpSignallingChannelIdleTimeout = 800;
#else
static const TInt KAvdtpSignallingChannelIdleTimeout = 1000000; // 1s
#endif

NONSHARABLE_CLASS(CSignallingChannel) : public CBase, public MSocketNotify, public XLogicalChannelFactoryClient
/**
Relatively stateless entity - driven mainly by GC
Provides primitives that accord with spec, but parameterised and initiated by
GC and Avdtp[Signalling]SAPs.
*/
	{
friend class CAvdtpProtocol; // for queing
friend class CAvdtpInboundSignallingMessage; //for SendingRejects

public:
	static CSignallingChannel* NewL(CAvdtpProtocol& aProtocol,
									CLogicalChannelFactory& aChannelFactory,
									const TBTDevAddr& aRemoteAddr);
									
	static CSignallingChannel* NewLC(CAvdtpProtocol& aProtocol,
									 CLogicalChannelFactory& aChannelFactory,
									 const TBTDevAddr& aRemoteAddr);

	TInt AttachSignallingUser(XAvdtpSignalReceiver& aReceiver);
	void DetachSignallingUser(XAvdtpSignalReceiver& aReceiver);
	void CancelTransactions(XAvdtpSignalReceiver& aUser);
	void RemoveTransaction(CSignallingTransaction& aTransaction);

	TBTDevAddr RemoteAddress(); // intentional signature
	inline TBool IsListening() const;
	
// for this remote (this class models "Connection-local" stuff in spec too)	
	inline TTCIDManager& TCIDManager();
	inline TTSIDManager& TSIDManager();
// Service Primitives 

// Signalling sessions provide a reference to themselves so that we can do some muxing
// 	their details can form the basis of the AVDTP TransactionLabel.
//	as we can bind the caller (SAP) address with the TransLabel
//	see CSignallingTransaction
// we can't do away with Attaching/Detaching of SAPs as that is required
// to signal when this object is ready

// INT primitives
	TInt SendDiscoverSEPs(XAvdtpSignalReceiver& aReceiver);
	TInt SendGetCapabilities(XAvdtpSignalReceiver& aReceiver, TSEID aACPSEID);
	TInt SendSetConfiguration(XAvdtpSignalReceiver& aReceiver, TSEID aINTSEID, TSEID aACPSEID, const RBuf8& aConfiguration);
	TInt SendRelease(XAvdtpSignalReceiver& aReceiver, TSEID aACPSEID);
	TInt SendAbort(XAvdtpSignalReceiver& aReceiver, TSEID aACPSEID);
	TInt SendStartStream(XAvdtpSignalReceiver& aReceiver, TSEID aACPSEID);
	TInt SendSuspendStream(XAvdtpSignalReceiver& aReceiver, TSEID aACPSEID);
	TInt SendSecurityControl(XAvdtpSignalReceiver& aReceiver, 
									    TSEID aACPSEID,
									    const TDesC8& aSecurityData);
	TInt SendReconfigure(XAvdtpSignalReceiver& aReceiver,
										  TSEID aACPSEID,
										  const RBuf8& aConfiguration);

	TInt SendOpenStream(XAvdtpSignalReceiver& aReceiver, TSEID aACPSEID);
	

// Upcall primitives from packet parsing - bit like XAvdtpSignalReceiver, but more params
	
	// confirms
	void DiscoverConfirm(TAvdtpTransactionLabel aLabel, TInt aResult, const TAvdtpInternalDiscoverConfirm* const aConfirm);
	void GetCapsConfirm(TAvdtpTransactionLabel aLabel, TInt aResult, const HBufC8* aCapsDataAsProtocol = NULL);
	void SetConfigConfirm(TAvdtpTransactionLabel aLabel, TInt aResult, TAvdtpServiceCategory aErrorCategory);
	void ReleaseConfirm(TAvdtpTransactionLabel aLabel, TInt aResult);
	void GetConfigConfirm(TAvdtpTransactionLabel aLabel, TInt aResult);
	void ReconfigConfirm(TAvdtpTransactionLabel aLabel, TInt aResult, TAvdtpServiceCategory aErrorCategory);
	void OpenConfirm(TAvdtpTransactionLabel aLabel, TInt aResult);
	void StartConfirm(TAvdtpTransactionLabel aLabel, TInt aResult);
	void AbortConfirm(TAvdtpTransactionLabel aLabel); // no error
	void SuspendConfirm(TAvdtpTransactionLabel aLabel, TInt aResult);
	void SecurityControlConfirm(TAvdtpTransactionLabel aLabel, TInt aResult, const TDesC8& aResponseData);

	// indications
	void DiscoverIndication(TAvdtpTransactionLabel aLabel);	
	void GetCapsIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID);
	void SetConfigIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID,
							  TSEID aINTSEID, RBuf8& aConfigData);
	void GetConfigIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID);
	void ReconfigIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID, RBuf8& aConfigData);
	void OpenIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID);
	void StartIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID);
	void ReleaseIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID);
	void SuspendIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID);
	void AbortIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID);
	void SecurityControlIndication(TAvdtpTransactionLabel aLabel, TSEID aACPSEID, const HBufC8* aSecurityData);
	
	
	TInt SendSetConfigurationAccept(TAvdtpTransactionLabel aLabel);
	TInt SendSetConfigurationReject(TAvdtpTransactionLabel aLabel, TBluetoothAvDistributionError aResult,
									  TAvdtpServiceCategory aCategory);

	TInt SendReconfigureAccept(TAvdtpTransactionLabel aLabel);	
	TInt SendReconfigureReject(TAvdtpTransactionLabel aLabel, TBluetoothAvDistributionError aResult,
									  TAvdtpServiceCategory aCategory);

	TInt SendStartAccept(TAvdtpTransactionLabel aLabel);
	TInt SendStartReject(TAvdtpTransactionLabel aLabel,TBluetoothAvDistributionError aResult, TSEID aBadSEID);
	TInt SendSuspendAccept(TAvdtpTransactionLabel aLabel);
	TInt SendSuspendReject(TAvdtpTransactionLabel aLabel,TBluetoothAvDistributionError aResult, TSEID aBadSEID);
	
	TInt SendSecurityControlAccept(TAvdtpTransactionLabel aLabel, const TDesC8* aOptionalData = NULL);
	TInt SendSecurityControlReject(TAvdtpTransactionLabel aLabel, TBluetoothAvDistributionError aResult);

	TInt SendReleaseAccept(TAvdtpTransactionLabel aLabel);
	
// non-spec stuff
	TInt GetData(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr=NULL);
	virtual void NewData(TUint aCount);
	
// async stuff
	void StartTryToSendCallback();
	void CancelTryToSendCallback();
	static TInt TryToSendCallback(TAny *aSigCh); //the static one we have to register
	TInt TryToSendCallback(); //on this Signalling Channel
private:	
// from MSocketNotify - transport (rather than packet) upcalls
	virtual void CanSend();
	virtual void ConnectComplete();
	virtual void ConnectComplete(const TDesC8& aConnectData);
    virtual void ConnectComplete(CServProviderBase& aSSP);
	virtual void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	virtual void CanClose(TDelete aDelete=EDelete);
	virtual void CanClose(const TDesC8& aDisconnectData,TDelete aDelete=EDelete);
	virtual void Disconnect(void);
	virtual void Disconnect(TDesC8& aDisconnectData);
	virtual void IoctlComplete(TDesC8 *aBuf);
	virtual void Error(TInt anError,TUint anOperationMask=EErrorAllOperations);
	virtual void NoBearer(const TDesC8& /*aConnectionInfo*/);
	virtual void Bearer(const TDesC8& aConnectionInfo);
	static TInt TryToClose(TAny* aSignallingChannel);

private:
// from MLogicalChannelFactoryClient
	void LogicalChannelFactoryRequestComplete(TLogicalChannelFactoryTicket aTicket, TInt aErr);
private:
	CSignallingChannel(CAvdtpProtocol& aProtocol, CLogicalChannelFactory& aChannelFactory, const TBTDevAddr& aRemoteAddr);
	TInt EnqueueMessage(CSignallingTransaction& aTok);
	void ConstructL();
	void RemoveTransactions(XAvdtpSignalReceiver& aUser);
	void PacketSent(CSignallingTransaction& aToken);
	void ObtainMTU();
	void ConfigConfirm(TAvdtpTransactionLabel aLabel,
							  TInt aResult,
							  TAvdtpServiceCategory aCategory,
							  TBool aReconfigure);
	
	CSignallingTransaction* PrepareSignallingCommand(XAvdtpSignalReceiver& aReceiver, TAvdtpMessage aMessage);
	CSignallingTransaction* PrepareSignallingResponse(TAvdtpMessageType aMessageType, TAvdtpMessage aMessage, TAvdtpTransactionLabel aLabel);
	CSignallingTransaction* PrepareSignallingPacket(TAvdtpMessageType aMessageType, TAvdtpMessage aMessage);
	
	
	CSignallingTransaction* FindTransaction(TAvdtpTransactionLabel aLabel);
	void HandleSignallingCommand(CAvdtpSignallingMessage& aSigMessage);
	
	void CheckOutboundQueue();
	void ServiceOutboundQueue();
	
	inline void SetBlocked(TBool aBlocked);
	inline TBool Blocked() const;
	
	~CSignallingChannel(); //only this can delete this
	inline const TDblQue<XAvdtpSignalReceiver>& Users() const;

// PDU operations not available for external use
	TInt SendAccept(TAvdtpTransactionLabel aLabel, TAvdtpMessage aMessage, const TDesC8* aOptionalData = NULL);
	
	TInt SendReject(TAvdtpTransactionLabel aLabel,
					TAvdtpMessage aMessage,
					TBluetoothAvDistributionError aError,
					const TDesC8* aRejectionData = NULL);
	void QueIdleTimer();
	void ErrorPermanentUsers(TInt aError);
	void ErrorServiceRequesters(TInt aError);
	TBool CheckSignal(const CSignallingTransaction& aToken, TAvdtpMessage aSignal) const;
	void CancelIdleTimer();
	TBool IsIdle() const;
	void IdledD();

private:
	CAvdtpProtocol&				iProtocol;	
	CLogicalChannelFactory&		iLogicalChannelFactory;
	CServProviderBase*			iBearer;
	TDblQueLink					iProtocolQLink;
	TDblQue<XAvdtpSignalReceiver> iPermanentUsers;
	TDblQue<CSignallingTransaction>	iTransactions;
	TInt						iBearerMTU;
	
	/**
	this value is either the intended remote or the actual one
	whereas RemoteAddress() returns 0 if the connection is not actually in place
	*/
	TBTDevAddr					iRemoteAddress;
	TTransactionLabelManager	iLabelGen;

	/**
	There is always one and only one incoming message [can be empty]
	*/
	CAvdtpInboundSignallingMessage		iInboundMessage;

	/**
	Messages being prepared by their Signalling Sessions.
	NB: this que will need to be purged of messages owned by a SS
	when it detaches (do we want to limit how many messages a SS can
	be preparing at once?)
	*/
	TDblQue<CAvdtpOutboundSignallingMessage>	iDraftMessages;
	
	/**
	Messages sitting in our outbound queue
	*/
	TDblQue<CAvdtpOutboundSignallingMessage>	iQueuedMessages;	
	
	/**
	The fragments which have been prepared from a message plucked
	off the outbound que (and fragmented according to the MTU of
	the L2CAP channel).  Only flushed if an ABORT appears in the
	queue for the _same_SEP_ [subtle, see AVDTP spec pp62].
	*/
	RMBufChain					iOutgoingSignallingMessage;
	TBool						iMoreFrags;
	CSignallingTransaction*		iCurrentTransaction;
	
	TBool						iBlocked;
	
	CAsyncCallBack*				iTryToSendCallback; // sig msgs sent asynchronously
	
// these managers are "Connection-local" in spec, hence this class owns them
	TTCIDManager				iTCIDManager;
	TTSIDManager				iTSIDManager;

	TDeltaTimerEntry iIdleTimerEntry; 	 //< Disconnection idle timer
	TBool			 iIdleTimerActive;	//< Is Idle timer running.
	
	TLogicalChannelFactoryTicket iLogicalChannelRequest; // for passive request for now
	};
	
inline TTCIDManager& CSignallingChannel::TCIDManager()
	{
	return iTCIDManager;
	}
	
inline TTSIDManager& CSignallingChannel::TSIDManager()
	{
	return iTSIDManager;	
	}

inline TBool CSignallingChannel::Blocked() const
	{
	return iBlocked;
	}

inline void CSignallingChannel::SetBlocked(TBool aBlocked)
	{
	iBlocked=aBlocked;
	}

inline const TDblQue<XAvdtpSignalReceiver>& CSignallingChannel::Users() const
	{
	return iPermanentUsers;
	}

inline TBool CSignallingChannel::IsListening() const
	{
	return iRemoteAddress==TBTDevAddr(0);
	}
	
#endif //AVDTPSIGNALLINGCHANNEL_H

