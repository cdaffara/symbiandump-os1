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
// Defines the ACL SAP class, and ACL State Classes (using State and Flyweight pattern)
// 
//

#ifndef _ACLSAP_H
#define _ACLSAP_H

#include "debug.h"
#include <es_prot.h>
#include <es_mbuf.h>
#include <bluetooth/aclsockaddr.h>
#include "basebandsap.h"
#include "notification.h"
#include "linkutil.h"

class CLinkMgrProtocol;
class CPhysicalLink;
class CPhysicalLinksManager;

class TACLLinkState;
class CACLLink;
class CACLDataItem;
class CACLDataQController;

/**
   Factory for the ACL Link states

   The states are flyweight classes
**/
NONSHARABLE_CLASS(CACLLinkStateFactory) : public CBase
	{
public:
	static CACLLinkStateFactory* NewL();
	~CACLLinkStateFactory();
	enum TACLLinkStates
		{
		EClosed,
		EListening,
		EAccepting,
		EWaitForLink,
		EWaitForStart,
		EWaitForStartError,
		EWaitForStartParked,
		EOpen,	
		EOpenParked,
		EClosing,
		EAcceptingClosing,
	// *** keep next one last ***
		EACLLinkMaxState,
		};
	
	TACLLinkState& GetState(TACLLinkStates aState);
	TInt StateIndex(const TACLLinkState* aState) const;
private:
	void ConstructL();
	TFixedArray<TACLLinkState*, EACLLinkMaxState> iStates;
	};


NONSHARABLE_CLASS(TACLLinkState)
	{
public:
	TACLLinkState(CACLLinkStateFactory& aFactory);

	virtual void Enter(CACLLink& aContext) const;

	virtual void ActiveOpen(CACLLink& aContext) const;
	virtual TInt PassiveOpen(CACLLink& aContext, TUint aQueSize) const;
	virtual void Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	virtual void DataReceived(CACLLink& aContext, TUint8 aFlag, const TDesC8& aData) const;
	virtual TUint Write(CACLLink& aContext, const TDesC8& aData, TUint aOptions, TSockAddr* aAddr) const;
	virtual void Start(CACLLink& aContext) const;
	virtual TBool ConnectRequest(CACLLink& aContext, const TBTConnect& aConnectDetails, const CPhysicalLink& aPhysicalLink) const;
	virtual void ConnectComplete(CACLLink& aContext, const TBTConnect& aConnectDetails) const;
	virtual void Disconnection(CACLLink& aContext) const;
	virtual void Error(CACLLink& aContext, TInt aError) const;

	virtual void Timeout(CACLLink& aContext, TBasebandTimeout aTimeout) const;
	virtual void Exit(CACLLink& aContext) const;
	virtual void Deletion(CACLLink& aContext) const;

	virtual TBool IsIdle() const;
	virtual void Park(CACLLink& aContext) const;
	virtual void Unpark(CACLLink& aContext) const;
protected:
	// Exits old state, sets the new state, and enters it.
	void ChangeState(CACLLink& aContext, CACLLinkStateFactory::TACLLinkStates aState) const;
	void PanicInState(TLinkPanic aPanic) const;

protected:
	CACLLinkStateFactory& iFactory;
#ifdef __FLOG_ACTIVE
	TBuf<48> iName;
#endif
	};

NONSHARABLE_CLASS(TACLLinkStateListening) : public TACLLinkState
	{
public:
	TACLLinkStateListening(CACLLinkStateFactory& aFactory);

	void Enter(CACLLink& aContext) const;

	void Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	TBool ConnectRequest(CACLLink& aContext, const TBTConnect& aConnect, const CPhysicalLink& aPhysicalLink) const;

	void Deletion(CACLLink& aContext) const;

	void Exit(CACLLink& aContext) const;
	
	void Error(CACLLink& aContext, TInt aError) const;
private:
	void SpawnL(CACLLink& aContext, CPhysicalLink& aPhysicalLink) const;
	};

NONSHARABLE_CLASS(TACLLinkStateAccepting) : public TACLLinkState
	{
public:
	TACLLinkStateAccepting(CACLLinkStateFactory& aFactory);
	void Enter(CACLLink& aContext) const;
	void Exit(CACLLink& aContext) const;
	void ConnectComplete(CACLLink& aContext, const TBTConnect& aConnect) const;
	void Error(CACLLink& aContext, TInt aError) const;
	void Disconnection(CACLLink& aContext) const;
	void Deletion(CACLLink& aContext) const;
	void Timeout(CACLLink& aContext, TBasebandTimeout aTimeout) const;
	void Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	};

NONSHARABLE_CLASS(TACLLinkStateWaitForLink) : public TACLLinkState
	{
public:
	TACLLinkStateWaitForLink(CACLLinkStateFactory& aFactory);

	void Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	void ConnectComplete(CACLLink& aContext, const TBTConnect& aConnectDetails) const;
	void Error(CACLLink& aContext, TInt aError) const;
	void Disconnection(CACLLink& aContext) const;
	void Exit(CACLLink& aContext) const;
	};

NONSHARABLE_CLASS(TACLLinkStateWaitForStart) : public TACLLinkState
	{
public:
	TACLLinkStateWaitForStart(CACLLinkStateFactory& aFactory);
	void Enter(CACLLink& aContext) const;
	void Start(CACLLink& aContext) const;
	void Error(CACLLink& aContext, TInt aError) const;
	void Deletion(CACLLink& aContext) const;
	void Disconnection(CACLLink& aContext) const;
	void Park(CACLLink& aContext) const;
	void Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	};

NONSHARABLE_CLASS(TACLLinkStateWaitForStartError) : public TACLLinkState
	{
public:
	TACLLinkStateWaitForStartError(CACLLinkStateFactory& aFactory);
	void Start(CACLLink& aContext) const;
	void Disconnection(CACLLink& aContext) const;
	void Error(CACLLink& aContext, TInt aError) const;
	void Deletion(CACLLink& aContext) const;
	};

NONSHARABLE_CLASS(TACLLinkStateWaitForStartParked) : public TACLLinkStateWaitForStart
	{
public:
	TACLLinkStateWaitForStartParked(CACLLinkStateFactory& aFactory);
	void Unpark(CACLLink& aContext) const;
	void Start(CACLLink& aContext) const;
	};


NONSHARABLE_CLASS(TACLLinkStateOpen) : public TACLLinkState
	{
public:
	TACLLinkStateOpen(CACLLinkStateFactory& aFactory);
	void Enter(CACLLink& aContext) const;

	TUint Write(CACLLink& aContext, const TDesC8& aData, TUint aOptions, TSockAddr* aAddr) const;
	void DataReceived(CACLLink& aContext, TUint8 aFlag, const TDesC8& aData) const;	
	void Disconnection(CACLLink& aContext) const;
	void Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	void Park(CACLLink& aContext) const;
	void Error(CACLLink& aContext, TInt aError) const;
	void Exit(CACLLink& aContext) const;
	};

NONSHARABLE_CLASS(TACLLinkStateOpenParked) : public TACLLinkStateOpen
	{
public:
	TACLLinkStateOpenParked(CACLLinkStateFactory& aFactory);
	void Enter(CACLLink& aContext) const;

	TUint Write(CACLLink& aContext, const TDesC8& aData, TUint aOptions, TSockAddr* aAddr) const;
	void DataReceived(CACLLink& aContext, TUint8 aFlag, const TDesC8& aData) const;	
	void Unpark(CACLLink& aContext) const;

	void Exit(CACLLink& aContext) const;
	};

NONSHARABLE_CLASS(TACLLinkStateClosing) : public TACLLinkState
	{
public:
	TACLLinkStateClosing(CACLLinkStateFactory& aFactory);

	void ActiveOpen(CACLLink& aContext) const;
	void Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	void Disconnection(CACLLink& aContext) const;
	void DataReceived(CACLLink& aContext, TUint8 aFlag, const TDesC8& aData) const;
	TUint Write(CACLLink& aContext, const TDesC8& aData, TUint aOptions, TSockAddr* aAddr) const;
	void Enter(CACLLink& aContext) const;
	void Deletion(CACLLink& aContext) const;

	TBool IsIdle() const;
	
	void Error(CACLLink& aContext, TInt aError) const;
	};

NONSHARABLE_CLASS(TACLLinkStateAcceptingClosing) : public TACLLinkState
	{
public:
	TACLLinkStateAcceptingClosing(CACLLinkStateFactory& aFactory);

	void Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	void Deletion(CACLLink& aContext) const;
	
	void Error(CACLLink& aContext, TInt aError) const;
	};


NONSHARABLE_CLASS(TACLLinkStateClosed) : public TACLLinkState
	{
public:
	TACLLinkStateClosed(CACLLinkStateFactory& aFactory);

	void Start(CACLLink& aContext) const; // CSockets Start
	void Enter(CACLLink& aContext) const;
	void ActiveOpen(CACLLink& aContext) const;
	TInt PassiveOpen(CACLLink& aContext, TUint aQueSize) const;
	void Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const;

	void Deletion(CACLLink& aContext) const;
	TBool IsIdle() const;
	};


NONSHARABLE_CLASS(CACLLink) : public CBTBasebandSAP, public MLogicalLink
	{
friend class TACLLinkState;
friend class TACLLinkStateOpen;
friend class TACLLinkStateOpenParked;
friend class TACLLinkStateListening;
friend class TACLLinkStateAccepting;
friend class TACLLinkStateWaitForStart;
friend class TACLLinkStateWaitForStartParked;
friend class TACLLinkStateWaitForLink;
friend class TACLLinkStateClosing;
friend class TACLLinkStateClosed;
public:
	static CACLLink* NewL(CPhysicalLinksManager& aLinksMan,
						  CPhysicalLink* aConnection,
						  CLinkMgrProtocol& aProtocol);
	static CACLLink* NewLC(CPhysicalLinksManager& aLinksMan,
						   CPhysicalLink* aConnection,
						   CLinkMgrProtocol& aProtocol);
	~CACLLink();
	// from SAP
	virtual void Start();
	virtual void AutoBind();
	virtual void LocalName(TSockAddr& anAddr) const ;
	virtual TInt SetLocalName(TSockAddr& anAddr);
	virtual void RemName(TSockAddr& anAddr) const ;
	virtual TInt SetRemName(TSockAddr& anAddr);
	virtual TInt GetOption(TUint level,TUint name,TDes8& anOption)const ;
	virtual void Ioctl(TUint level,TUint name,TDes8* anOption);
	virtual void CancelIoctl(TUint aLevel,TUint aName);
	virtual TInt SAPSetOption(TUint level,TUint name,const TDesC8 &anOption);

	virtual void ActiveOpen();
	virtual TInt PassiveOpen(TUint aQueSize);
	virtual void Shutdown(TCloseType option);
	virtual void Shutdown(TCloseType option,const TDesC8& aDisconnectionData);


	TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* aAddr);

	void GetData(TDes8& aDesc,TUint aOptions,TSockAddr* anAddr=NULL);
	TInt GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* aAddr=NULL);

	void Timeout(TBasebandTimeout aTimeout);

	TBool IsIdle() const;
	TBool IsLocallyInitiated() const;

	void ACLDataReceived(THCIConnHandle aConnH, TUint8 aFlag, const TDesC8& aData);

// from MLogicalLink
	virtual TBool ConnectRequest(const TBTConnect& aConnect, const CPhysicalLink& aPhysicalLink);
	virtual void ConnectComplete(const TBTConnect& aConnect);
	virtual void Disconnection();
	virtual void Error(TInt aError);
	virtual void DataReceived(THCIConnHandle aConnH, TUint8 aFlag, const TDesC8& aData);
	
	void PacketsSent(THCIConnHandle aConnH, TUint16 aNumPackets);
	void PhysicalLinkChange(const TBTBasebandEventNotification& aEvent, CPhysicalLink& aPhysicalLink);
	void TryToSend();
	
private:
	CACLLink(CPhysicalLinksManager& aManager,
			 CPhysicalLink* aConnection,
			 CLinkMgrProtocol& aProtocol);
	void ConstructL();
	void NotifyDataToSocket(TUint8 aFlag, const TDesC8& aData);
	void CloseLogicalLink();
	TInt OpenLogicalLink();
	void ParkLogicalLink();
	void UnparkLogicalLink();
	void BindAndConnectPhysicalLinkL();
	void ClearPhysicalLink();
	TInt DoACLGetOption(TUint aName,TDes8& aOption) const;
	TInt DoACLSetOption(TUint aName, const TDesC8& aOption);
	TUint DoWrite(CACLDataItem& aFrame, CACLDataQController& aQctrl,
		   	   const TDesC8& aData, TUint8 aOptions);
	TBool CanHandleFlags(TUint8 aFlag) const;
	
	TInt ProcessFlush();
	
private: // from MAccessRequestResponseHandler
	void AccessRequestComplete(TInt aResult);
	
private:
	enum TInitiationState
		{
		EInvalidInitiationState	= 0x00,
		ELocallyInitiated		= 0x01,
		ERemotelyInitiated		= 0x02,
		};
	void SetInitiationState(TInitiationState aState);
	void ClearInitiationState();
	
private:
	CLinkMgrProtocol&		iProtocol;
	TACLLinkState*			iState;

	RMBufPktQ				iInboundBuffer;

	TACLPort			iLocalPort;		// based on PB flags
	TBool				iSocketBlocked;

	TInitiationState	iInitiationState; // simpler than modelling in state machine.
	};

#ifdef __FLOG_ACTIVE
#define STATENAME(x)  iName=_L(x)
#else
#define STATENAME(x)
#endif

#endif // _ACLSAP_H
