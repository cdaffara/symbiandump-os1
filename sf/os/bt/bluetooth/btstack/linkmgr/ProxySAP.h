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
// Defines the Proxy SAP class
// 
//

#ifndef _PROXYSAP_H_
#define _PROXYSAP_H_

#include <bt_sock.h>
#include <es_mbuf.h>
#include "physicallinks.h"
#include "notification.h"
#include "btconsts.h"

class CACLRawConduit;

NONSHARABLE_CLASS(TRequestedLinkPolicy)
/**
	Not to be confused with the TLinkPolicy - that's just for the HCI Command
	This is to store the preferences of the RBTBaseband within a Proxy SAP
**/
	{
public:
	TRequestedLinkPolicy(TUint8 aSettings, TBool aSwitchAllowed);
	TBool IsModeAllowed(TBTLinkMode aMode) const;
	TBool IsSwitchAllowed() const;

	void SetModeAllowed(TBTLinkMode, TBool aAllowed);
	void SetSwitchAllowed(TBool aAllowed);

	TBool IsModeRequested(TBTLinkMode aMode) const;
	TBool IsAnyModeRequested() const;

	TInt SetModeRequested(TBTLinkMode aMode);

	TUint8 ModesAllowed() const;
	TUint8 CurrentModeRequest()	const;

private:
	TBool	iSwitchAllowed;
	TUint8	iModesAllowed;
	TUint8	iCurrentModeRequest;
	};


NONSHARABLE_CLASS(TBTQueuedBasebandEventNotification) : public TBTBasebandEventNotification
/**
	Extends via aggregation public TBTBasebandEventNotification class to make it queue-able
**/
	{
public:
	TBTQueuedBasebandEventNotification(const TBTBasebandEventNotification& aEvent)
		: TBTBasebandEventNotification(aEvent) {};

public:
	TSglQueLink iLink;
	};


NONSHARABLE_CLASS(CBTProxySAP) : public CBTBasebandSAP
/**
	Acts as the proxy to RBTBaseband - its MSocketNotify is a CSocket

	It is itself an MSocketNotify as this is an ideal way for the real SAP to talk to it
**/
	{
friend class CACLRawConduit;
public:
	static CBTProxySAP* NewLC(CPhysicalLinksManager& aConnectionMan, CPhysicalLink*);
	static CBTProxySAP* NewL(CPhysicalLinksManager& aConnectionMan, CPhysicalLink*);
	~CBTProxySAP();

// from SAP - the proxy will not do all of these
	virtual void Start();
	virtual void RemName(TSockAddr& anAddr) const ;
	virtual TInt SetRemName(TSockAddr& anAddr);
	virtual TInt GetOption(TUint level,TUint name,TDes8& anOption)const ;
	virtual void Ioctl(TUint level,TUint name,TDes8* anOption);
	virtual void CancelIoctl(TUint aLevel,TUint aName);
	virtual TInt SAPSetOption(TUint level,TUint name,const TDesC8 &anOption);
	virtual void ActiveOpen();
	virtual void ActiveOpen(const TDesC8& aConnectionData);
	virtual TInt PassiveOpen(TUint aQueSize);
	virtual TInt PassiveOpen(TUint aQueSize,const TDesC8& aConnectionData);
	virtual void Shutdown(TCloseType aCloseType);
	virtual void Shutdown(TCloseType aCloseType,const TDesC8& aDisconnectionData);
	virtual void AutoBind();
	virtual void IoctlComplete(TInt aErr, TUint aLevel, TUint aName, TDesC8* aBuf=NULL);
	
	TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* aAddr);
	void GetData(TDes8& aDesc,TUint options,TSockAddr* anAddr=NULL);

	void Timeout(TBasebandTimeout aTimeout);

	static TInt SignalConnectComplete(TAny* aCPhysicalLink);// async callback

	TBool IsModeRequested(TBTLinkMode aMode) const;
	TBool IsAnyModeChangeRequested() const; 

	TUint8 GetRequestedModes() const;
	TUint8 GetAllowedModes() const;
	TBool IsRoleSwitchAllowed() const;
	TBool RequestedActiveMode() const;

	void DataReceived();

	void TerminatePhysicalLinksComplete();

// from MPhysicalLinkObserver
	void PhysicalLinkChange(const TBTBasebandEventNotification & aEvent, CPhysicalLink& aPhysicalLink);

	void Error(TInt aError);
	void Disconnect();
	
public:
	TSglQueLink			iQueueLink;	
	TBTDblQueLink		iPLMLink;	

private:
	CBTProxySAP(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aPhysicalSAP);
	void ConstructL();
	TInt CreateRawConduit();
	void AsyncCheckLinkUp();

	TInt SetModeRequested(TBTLinkMode aMode);
	void SetBasebandNotificationOptions(TUint32 aOption);

	void PhysicalLinkUp();
	void PhysicalLinkDown();
	void PhysicalLinkError(TInt aError);

	void ClearBasebandEventQueue();
	TBool CanCreatePhysicalLink();
	void ClearPhysicalLink();
	void SetNullBasebandState(TBTBasebandEventNotification & aEvent);

#ifdef HOSTCONTROLLER_TO_HOST_FLOW_CONTROL
	void TryToFreeBuffers();
	static TInt AsyncFreeBuffers(TAny* aCACLLink);
	CAsyncCallBack*							iBufferFreeCallback;
	RArray<RMBufChain>						iInboundBuffer; 
	TInt									iInboundPacketIndex;
	TInt									iInboundBuffersFreed;
#else
	// for no flow control
	RMBufPktQ								iInboundBuffer;
#endif

private:
	enum TEventNotificationStatus
		{
		EDisabled,
		EEnabledCanSend,
		EEnabledOneShot,
		EEnabledQueuing,
		};

	enum TTerminateStatus
		{
		ENone,
		ETerminatingAllLinks,
		ETerminatingSingleLink,
		};
		
	CAsyncCallBack*			iAsyncCallback;
	CACLRawConduit*			iRawConduit; 
	TRequestedLinkPolicy	iRequestedLinkPolicy;
	TBool					iRawConduitReady;
	TBool					iNotifiedUp;
	TTerminateStatus		iTerminating;
	TUint32					iBasebandNotifyOptions;
	TSglQue<TBTQueuedBasebandEventNotification> iEventNotificationQueue;
	TEventNotificationStatus					iEventNotificationStatus;
	TBool					iRequestedActiveMode;
	};


#endif //_PROXYSAP_H_
