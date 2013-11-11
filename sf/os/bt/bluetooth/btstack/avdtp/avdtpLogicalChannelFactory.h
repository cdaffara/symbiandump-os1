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
// Defines the avdtp logical channel factory
// which creates the logical (L2CAP) channels for transport and signalling channels
// 
//

/**
 @file
 @internalComponent
*/

#include <bt_sock.h>
#include <es_prot.h>

#ifndef AVDTPLOGICALCHANNELFACTORY_H
#define AVDTPLOGICALCHANNELFACTORY_H

#include "avdtpAllocators.h"

class XLogicalChannelFactoryClient;
class CManagedLogicalChannel;
class CProtocolBase;
struct TLogicalChannelRecord;
class CLogicalChannelFactory;
class CBluetoothProtocolBase;

const TInt KInitialSequenceNumber = 1;
const TInt KAvdtpChannelArraySize = 3;

typedef TInt8 TLogicalChannelFactoryRequestId;


NONSHARABLE_CLASS(TRequestIdManager) : public TBitFieldAllocator
	{
public:
	inline TInt	GetId(TLogicalChannelFactoryRequestId& aId);
	inline void	FreeId(TLogicalChannelFactoryRequestId aId);
	};

// abstract
NONSHARABLE_CLASS(CLogicalChannelFactoryRequest) : public CBase
	{
friend class CLogicalChannelFactory;
protected:	
	CLogicalChannelFactoryRequest(XLogicalChannelFactoryClient& aClient, TLogicalChannelFactoryRequestId aId);
	XLogicalChannelFactoryClient&	iClient;
	TSglQueLink						iFactoryQLink;
	TLogicalChannelFactoryRequestId iId;
	TInt							iNumChannelsRequired; // those left to connect
	};
	
// abstract
NONSHARABLE_CLASS(CLogicalChannelFactoryPassiveRequest) : public CLogicalChannelFactoryRequest
	{
public:
 	~CLogicalChannelFactoryPassiveRequest();
protected:
	CLogicalChannelFactoryPassiveRequest(XLogicalChannelFactoryClient& aClient,
										 TLogicalChannelFactoryRequestId aId,
										 CBluetoothProtocolBase& aAvdtp);
 	void BaseConstructL();
private:
 	CBluetoothProtocolBase& iAvdtp;
	};

NONSHARABLE_CLASS(CExpectSignallingLogicalChannel) : public CLogicalChannelFactoryPassiveRequest
	{
public:
 	static CExpectSignallingLogicalChannel* NewL(XLogicalChannelFactoryClient& aClient, TLogicalChannelFactoryRequestId aId, CBluetoothProtocolBase& aAvdtp);
private:
 	void ConstructL();
 	CExpectSignallingLogicalChannel(XLogicalChannelFactoryClient& aClient, TLogicalChannelFactoryRequestId aId, CBluetoothProtocolBase& aAvdtp);
	};
/**
Use to tell factory to expect explicitly sequenced logical channels, typically used for Direct Transport channels
*/
NONSHARABLE_CLASS(CExpectSessionLogicalChannels) : public CLogicalChannelFactoryPassiveRequest
	{
public:
	static CExpectSessionLogicalChannels* NewL(XLogicalChannelFactoryClient& aClient, TLogicalChannelFactoryRequestId aId,
												TInt aNumRequired, CBluetoothProtocolBase& aAvdtp);
private:
	void ConstructL();
	CExpectSessionLogicalChannels(XLogicalChannelFactoryClient& aClient, TLogicalChannelFactoryRequestId aId,
									TInt aNumRequired, CBluetoothProtocolBase& aAvdtp);
	};
	
/**
For clients to issue requests to the ChannelFactory
*/
NONSHARABLE_CLASS(CLogicalChannelFactoryActiveRequest) : public CLogicalChannelFactoryRequest
	{
friend class CLogicalChannelFactory;
public:
	~CLogicalChannelFactoryActiveRequest();
protected:
	CLogicalChannelFactoryActiveRequest(const TBTDevAddr& aAddr, XLogicalChannelFactoryClient& aClient, TLogicalChannelFactoryRequestId aId);
protected:
	TFixedArray<CManagedLogicalChannel*, KAvdtpChannelArraySize>	iLogicalChannels;
	TBTDevAddr				iRemoteDev;
	};

NONSHARABLE_CLASS(CCreateSignallingLogicalChannel) : public CLogicalChannelFactoryActiveRequest
	{
public:
	static CCreateSignallingLogicalChannel* NewL(const TBTDevAddr&, 
												 XLogicalChannelFactoryClient& aClient,
												 TLogicalChannelFactoryRequestId aId,
												 CLogicalChannelFactory& aLogicalChannelFactory);

	static CCreateSignallingLogicalChannel* NewLC(const TBTDevAddr&,
												  XLogicalChannelFactoryClient& aClient,
												  TLogicalChannelFactoryRequestId aId,
												  CLogicalChannelFactory& aLogicalChannelFactory);
private:
	CCreateSignallingLogicalChannel(const TBTDevAddr&, XLogicalChannelFactoryClient& aClient, TLogicalChannelFactoryRequestId aId);
	void ConstructL(CLogicalChannelFactory& aLogicalChannelFactory);
	};


/**
To create n logical channels
*/
NONSHARABLE_CLASS(CCreateSessionLogicalChannels) : public CLogicalChannelFactoryActiveRequest
	{
public:
	static CCreateSessionLogicalChannels* NewL(const TBTDevAddr& aAddr, XLogicalChannelFactoryClient& aClient,
									   		   TLogicalChannelFactoryRequestId aId, TInt aNumRequired);
	static CCreateSessionLogicalChannels* NewLC(const TBTDevAddr& aAddr, XLogicalChannelFactoryClient& aClient,
									   		   TLogicalChannelFactoryRequestId aId, TInt aNumRequired);
private:
	CCreateSessionLogicalChannels(const TBTDevAddr& aAddr, XLogicalChannelFactoryClient& aClient,
						   TLogicalChannelFactoryRequestId aId, TInt aNumRequired);
	};

/** 
To close logical channels
*/
NONSHARABLE_CLASS(CCloseSessionLogicalChannels) : public CLogicalChannelFactoryRequest
	{
friend class CLogicalChannelFactory;
public:
	static CCloseSessionLogicalChannels* NewL(XLogicalChannelFactoryClient& aClient, 
										TLogicalChannelFactoryRequestId aId);
	static CCloseSessionLogicalChannels* NewLC(XLogicalChannelFactoryClient& aClient, 
										TLogicalChannelFactoryRequestId aId);
	~CCloseSessionLogicalChannels();

	void StartJob(TInt aTimeout);
	void ChannelClosed(CManagedLogicalChannel* aChannel);
private:
	CCloseSessionLogicalChannels(XLogicalChannelFactoryClient& aClient,
						  TLogicalChannelFactoryRequestId aId);
	static TInt WatchdogBarked(TAny* aCloseLogicalChannels);
	void CloseChannels(TBool aNotifyCompletion);

private:
	TFixedArray<CManagedLogicalChannel*, KAvdtpChannelArraySize> iLogicalChannels;
	TDeltaTimerEntry iTimerEntry;
	};

#ifdef _DEBUG	
#define DEBUG_STORE_FACTORY_REQUEST 	iChannelFactoryRequest = &\

#else
#define DEBUG_STORE_FACTORY_REQUEST
#endif

/**
Class representing the job the factory has undertaken
This is always returned synchronously - the client should inspect if the job has been completed synchronously
via the State() method.
If the job is process asynchronously then a new one is returned upon completion
*/
NONSHARABLE_CLASS(TLogicalChannelFactoryTicket)
	{
friend class CLogicalChannelFactory;
public:
	enum TLogicalChannelFactoryRequestState
		{
		ERequestIdle,
		ERequestOutstanding,
		ERequestComplete,
		ERequestErrored
		};
		
	TLogicalChannelFactoryTicket(CLogicalChannelFactory* aFactory, TLogicalChannelFactoryRequestId aId);
	TLogicalChannelFactoryTicket();
	TLogicalChannelRecord GetLogicalChannel(TInt aSequenceNumber=1);	

	inline TLogicalChannelFactoryRequestState State() const { return iState; }
	inline TLogicalChannelFactoryRequestId Id() const { return iId; }
	
private:
	void SetState(TLogicalChannelFactoryRequestState aNewState);
	
private:
	CLogicalChannelFactory*			iFactory; // non-owned, cannot be reference in default ctor
	TLogicalChannelFactoryRequestId iId;
	TLogicalChannelFactoryRequestState	iState;
	};
	

/*	
The class provides a callback for someone that asked for logical channels
@note this is an X class (see Programming DB) as it is effectively an M-class
but must have a member to be que-able.
*/
NONSHARABLE_CLASS(XLogicalChannelFactoryClient)
	{
public:
	virtual void LogicalChannelFactoryRequestComplete(TLogicalChannelFactoryTicket, TInt aResult)=0;
	TSglQueLink	iFactoryQLink;
	};
	
	
/**
Knows about ordering of L2CAP channels
Hands ownership of newly established channels to clients
Provides them with the TransportSession to
	a) remind them
	b) to assert it's ok
	
The use of this class will to some extent be by someone who
additionally knows (or is implcitily designed as such) of the
channel order - for we cannot go adding a reporting bearer merely
to obtain a recovery bearer....(presumably :o)

Once ownership is transferred the caller will synchronously have to set
itself as the SocketNotify of the L2CAP SAP
@internalComponent
*/
class CDirectChannel;
NONSHARABLE_CLASS(CLogicalChannelFactory) : public CBase, public MSocketNotify, public XLogicalChannelFactoryClient
	{
friend class TLogicalChannelFactoryTicket;
public:
	static CLogicalChannelFactory* NewL(CBluetoothProtocolBase& iProtocol, CProtocolBase& aSAPFactory);
	
	TLogicalChannelFactoryTicket CreateSignallingLogicalChannelL(const TBTDevAddr& aAddr,
										 XLogicalChannelFactoryClient& aClient);
	
	TLogicalChannelFactoryTicket CreateSessionLogicalChannelsL(const TBTDevAddr& aAddr,
									   XLogicalChannelFactoryClient& aClient, TInt aNumRequired);

	void CloseSessionLogicalChannelsL(TArray<CDirectChannel*>& aChannels,
									   TInt aTimeout);

	TLogicalChannelFactoryTicket  ExpectSignallingLogicalChannelL(XLogicalChannelFactoryClient& aClient);

	TLogicalChannelFactoryTicket ExpectSessionLogicalChannelsL(XLogicalChannelFactoryClient& aClient,
																	TInt aNumRequired);

	void LogicalChannelLost(CManagedLogicalChannel* aChannel);
	void Cancel(TLogicalChannelFactoryTicket& aJobSpec);
	~CLogicalChannelFactory();
	inline CProtocolBase& SAPFactory() const;
	TInt BearerConnectComplete(const TBTDevAddr& /*aAddr*/,
								CServProviderBase* aSAP); // forward from AVDTP protocol when listen complete
private:
// from MSocketNotify
	virtual void NewData(TUint aCount);
	virtual void CanSend();
	virtual void ConnectComplete();
	virtual void ConnectComplete(const TDesC8& aConnectData);
    virtual void ConnectComplete(CServProviderBase& aSSP);
	virtual void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	virtual void CanClose(TDelete aDelete=EDelete);
	virtual void CanClose(const TDesC8& aDisconnectData,TDelete aDelete=EDelete);
	virtual void Error(TInt aError,TUint aOperationMask=EErrorAllOperations);
	virtual void Disconnect();
	virtual void Disconnect(TDesC8& aDisconnectData);
	virtual void IoctlComplete(TDesC8* aBuf);
	virtual void NoBearer(const TDesC8& aConnectionInfo);
	virtual void Bearer(const TDesC8& aConnectionInfo);

private:
// from XLogicalChannelFactoryClient
	virtual void LogicalChannelFactoryRequestComplete(TLogicalChannelFactoryTicket, TInt aResult);
	
private:
	CLogicalChannelFactory(CBluetoothProtocolBase& iProtocol, CProtocolBase& aSAPFactory);
	void ConstructL();
	void DoObtainChannelL();
	static TInt TryNextJob(TAny* aAny);
	void TryNextActiveJob();
	TBool CheckActiveJobComplete(CLogicalChannelFactoryActiveRequest& aJob);
	void CompleteActiveJob(TInt aError);
	void NotifyComplete(TInt aError, CLogicalChannelFactoryRequest& aRequest);
	void SetId(CLogicalChannelFactoryActiveRequest& aRequest);
	static void FreeId(TAny* aId);
	void DeleteRequest(CLogicalChannelFactoryRequest *aRequest);
	
	TLogicalChannelRecord ClaimLogicalChannel(TInt aSequenceNumber, TLogicalChannelFactoryRequestId aId, TBool& aFinished);
	CManagedLogicalChannel* FindUnclaimedLogicalChannel(const TBTDevAddr& aAddr,
														TInt aSequenceNumber,
														TLogicalChannelFactoryRequestId& aId);
	TInt TryToTakeConnection(const TBTDevAddr& aRemote,	CServProviderBase* aSAP, 
							TSglQue<CLogicalChannelFactoryPassiveRequest>& aJobQueue);
	
private:
	CBluetoothProtocolBase&				iProtocol;	 //AVDTP
	CProtocolBase&						iBearerSAPFactory; //L2CAP
	TDblQue<CManagedLogicalChannel>		iUnclaimedLogicalChannels;
	TSglQue<CLogicalChannelFactoryActiveRequest>	iPendingActiveJobs;
	TSglQue<CLogicalChannelFactoryPassiveRequest>	iPendingPassiveSignallingJobs;
	TSglQue<CLogicalChannelFactoryPassiveRequest>	iPendingPassiveSessionJobs;
	TSglQue<CCloseSessionLogicalChannels>			iCloseChannelJobs;
	CLogicalChannelFactoryActiveRequest*		iCurrentActiveJob;
	TBool								iCurrentJobCancelled;
	TRequestIdManager					iIdManager;
	TLogicalChannelFactoryRequestId 	iId;  // used for cleaning up if the got (getid) is to be lost due to a leave
	CAsyncCallBack*						iAsyncTryNextJob;
	};


NONSHARABLE_CLASS(CManagedLogicalChannel) : public CBase, public MSocketNotify
/**
	for queuing inbound unclaimed SAPs.  needs to be a socket so that SAP
	can declare newdata, disconnection etc.
*/
	{
friend class CLogicalChannelFactory; //for quing
public:
	static CManagedLogicalChannel* NewL(CLogicalChannelFactory& aFactory,
								 		const TBTDevAddr& aAddr,
								 		TInt aSequenceNumber,
								 		TLogicalChannelFactoryRequestId aId,
								 		CServProviderBase* aSAP = NULL);
	static CManagedLogicalChannel* NewL(CLogicalChannelFactory& aFactory,
								 		TLogicalChannelFactoryRequestId aId);
	
	~CManagedLogicalChannel();

	void Shutdown();

private:	
// from MSocketNotify
	virtual void NewData(TUint aCount);
	virtual void CanSend();
	virtual void ConnectComplete();
	virtual void ConnectComplete(const TDesC8& aConnectData);
    virtual void ConnectComplete(CServProviderBase& aSSP);
	virtual void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	virtual void CanClose(TDelete aDelete=EDelete);
	virtual void CanClose(const TDesC8& aDisconnectData,TDelete aDelete=EDelete);
	virtual void Error(TInt aError,TUint aOperationMask=EErrorAllOperations);
	virtual void Disconnect();
	virtual void Disconnect(TDesC8& aDisconnectData);
	virtual void IoctlComplete(TDesC8* aBuf);
	virtual void NoBearer(const TDesC8& aConnectionInfo);
	virtual void Bearer(const TDesC8& aConnectionInfo);

private:
	CManagedLogicalChannel(CLogicalChannelFactory& aFactory,
						   const TBTDevAddr& aAddr,
						   TInt aSequenceNumber,
						   TLogicalChannelFactoryRequestId aId);

	CManagedLogicalChannel(CLogicalChannelFactory& aFactory,
						   TLogicalChannelFactoryRequestId aId);

	void ConstructL(CServProviderBase* aPrecreatedSAP);
	CServProviderBase* ObtainSAP();
	void ProvideSAP(CServProviderBase* aSAP);
	
private:
	CLogicalChannelFactory&	iFactory;
	TBTDevAddr				iRemoteAddress;
	TInt					iSequenceNumber;	// for sequence creations/MCs
	CServProviderBase*		iLogicalChannelSAP;
	TUint					iDataCount;
	TBool					iEndOfData;		// bit annoying, but safer that bittwiddling on iDataCount
	TDblQueLink				iFactoryQLink;
	TLogicalChannelFactoryRequestId iId; // the request this was part of
	};

inline TInt	TRequestIdManager::GetId(TLogicalChannelFactoryRequestId& aId)
	{
	TInt val, res;
	res = Get(val, 30, 1); // 0 is "invalid"
	aId = static_cast<TLogicalChannelFactoryRequestId>(val);
	return res;
	}
	
inline void	TRequestIdManager::FreeId(TLogicalChannelFactoryRequestId aId)
	{
	Free(aId);
	}

 inline CProtocolBase& CLogicalChannelFactory::SAPFactory() const
 	{
 	return iBearerSAPFactory;
 	}

NONSHARABLE_CLASS(TLogicalChannelRecord)
/*
Effectively a struct for transferring ownership of resulting logical channels
Binds together the SAP, and any data count that has appeared whilst the logical channel
lay unclaimed.
*/
	{
public:
	inline TLogicalChannelRecord();
	inline void Reset();
public:
	CServProviderBase*	iLogicalChannelSAP;	// non-owned
	TUint				iDataCount;
	TBool				iEndOfData;		// bit annoying, but safer that bittwiddling on iDataCount	
	};

/**
To help claim logical channels
*/	
NONSHARABLE_CLASS(TLogicalChannelFactoryTicketInspector)
	{
public:
	TLogicalChannelFactoryTicketInspector(TLogicalChannelFactoryTicket& aTicket,
										TBool aRequireReporting,
										TBool aRequireRecovery,
										TBool aMuxed);
	TLogicalChannelRecord GetLogicalChannel(TAvdtpTransportSessionType aType);
private:
	TLogicalChannelFactoryTicket& iTicket;
	const TInt iSignallingSequenceNumber;
	const TInt iMediaSequenceNumber; // always first whether muxed or not
	TInt iReportingSequenceNumber;
	TInt iRecoverySequenceNumber;										
	TLogicalChannelRecord iCachedRecord; // eg for Reporting which is the same LC as media
	TBool iCached;
#ifdef _DEBUG
	TBool iRequireReporting;
	TBool iRequireRecovery;
#endif
	};
	

inline TLogicalChannelRecord::TLogicalChannelRecord()
	{
	Reset();
	}
	
inline void TLogicalChannelRecord::Reset()
	{
	iLogicalChannelSAP = NULL;
	iDataCount = 0;
	iEndOfData = EFalse;
	}
	
#endif //AVDTPLOGICALCHANNELFACTORY_H
