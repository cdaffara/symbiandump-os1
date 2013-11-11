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
// Defines the SCO SAP class, and SCO State Classes (using State and Flyweight pattern)
// 
//

#ifndef _SCOSAP_H_
#define _SCOSAP_H_

#include "debug.h"
#include "basebandsap.h"
#include "notification.h"
#include "Basebandmodel.h"
#include "internaltypes.h"
#include "linkutil.h"

class CPhysicalLink;
class CHctlSynchronousDataFrame;

static const TInt KInboundSCODataSize  = 255;
static const TInt KMinESCOPacketSize = 1;
static const TInt KMinESCOBandwidth = 1;
static const TInt KMilliSecondsFactor = 1000;
static const TInt KBasebandSlotsPerSecond = 1600;

static const TInt KMaxEV3Size = 30;
static const TInt KMaxEV4Size = 120;
static const TInt KMaxEV5Size = 180;

static const TInt KEV3Slots = 1;
static const TInt KEV4Slots = 3;
static const TInt KEV5Slots = 3;


class TSyncLinkState;

NONSHARABLE_CLASS(CBTSynchronousLink) : public CBTBasebandSAP, public MLogicalLink
	{
	friend class TSyncLinkState;
	friend class TSCOLinkStateClosed;
	friend class TSCOLinkStateWaitForSCO;
	friend class TSCOLinkStateListening;
	friend class TSCOLinkStateAccepting;
	friend class TSCOLinkStateWaitForStart;
	friend class TSCOLinkStateWaitForStartError;
	friend class TSCOLinkStateOpen;
	friend class TSCOLinkStateClosing;
	
	public:
		~CBTSynchronousLink();

		virtual void Start();
		virtual void ActiveOpen();
		virtual TInt PassiveOpen(TUint aQueSize);
		virtual void Shutdown(TCloseType option);
		virtual void Shutdown(TCloseType aClose, const TDesC8& /*aDisconnectionData*/);
		virtual void AutoBind();
		virtual TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* aAddr);
		virtual void GetData(TDes8& aDesc,TUint options,TSockAddr* aAddr=NULL);
		virtual TInt SetLocalName(TSockAddr& aAddr);
		virtual TInt SetRemName(TSockAddr& aAddr);
		
		virtual TInt SAPSetOption(TUint level, TUint name, const TDesC8& aOption) =0;
		virtual TInt GetOption(TUint level, TUint name, TDes8& aOption) const =0;
		
		// Pure virtuals from base classes
		virtual void Timeout(TBasebandTimeout aTimeout);
		virtual void PacketsSent(THCIConnHandle /*aHandle*/, TUint16 /*aNumPackets*/);

		void PhysicalLinkChange(const TBTBasebandEventNotification& aEvent, CPhysicalLink& aPhysicalLink);


		virtual TBool ConnectRequest(const TBTConnect& aConnect, const CPhysicalLink& aPhysicalLink);
		virtual void ConnectComplete(const TBTConnect& aSCOLink);
		virtual void SyncConnectComplete(const TBTConnect& aSCOLink, const TBTSyncConnectOpts& aSyncOpts);
		virtual void Disconnection();
		virtual void Error(TInt aErr);
		virtual void DataReceived(THCIConnHandle aConnH, TUint8 aIgnored, const TDesC8& aData);
		virtual TBool IsIdle() const;
		virtual TBool IsOpen() const;
	
		TInt CommonShutdown();
		void DoActiveOpenL();
		void ReleaseAndUnbind();
		
		virtual TUint DoWrite(const TDesC8& aData, TUint aOptions, TSockAddr* aAddr) =0;
		virtual TDes8& InboundFrame() =0;
		virtual TBool IsLinkProbablyPossible() =0;
		virtual TInt MakeConnection(CPhysicalLink* aPhysLink) =0;
		virtual void SetExtOptions(TBTSyncConnectOpts aSyncOpts) =0;
		
		inline const TLinkType& LinkType() { return iLinkType; };
		
		void ParentClosing();
		TBool ConnectPending() const;
	
	protected:
		CBTSynchronousLink(CPhysicalLinksManager& aLinksMan, CPhysicalLink* aPhysicalLink, TLinkType aLinkType);
		void ConstructL();


	protected:
		TSyncLinkState*	iState;
		TBool			iClosePending;
		TLinkType		iLinkType;
	};


NONSHARABLE_CLASS(CSCOLink) : public CBTSynchronousLink
/**
	Provides a representation of the SCO link(s)

	This are created when the (linkmgr) protocol is asked
	for a SAP with KBTSCOLink type.


**/
	{
public:
	static CSCOLink* NewLC(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aConnection);
	static CSCOLink* NewL(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aConnection);
	~CSCOLink();
	// from CServProviderBase
	virtual TInt SAPSetOption(TUint aLevel, TUint aName, const TDesC8& aOption);
	virtual TInt GetOption(TUint level, TUint name, TDes8& aOption) const;
	
// events for the SCO Link (not PHY)
	void PacketTypeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewPacket);

	TDes8& InboundFrame();
	TBool IsLinkProbablyPossible();
	TInt MakeConnection(CPhysicalLink* aPhysLink);	
	
private:
	CSCOLink(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aConnection);
	void ConstructL();
	TUint DoWrite(const TDesC8& aData, TUint aOptions, TSockAddr* aAddr);
	void SetExtOptions(TBTSyncConnectOpts aSyncOpts);


private:
	CHctlSynchronousDataFrame*	iOutboundSCOFrame;
	TBuf8<KInboundSCODataSize>	iInboundSCOFrame;
	TBTSyncPacketTypes			iUserHVPacketTypes;
	};


NONSHARABLE_CLASS(CeSCOLink) : public CBTSynchronousLink
/**
	Provides a representation of the eSCO link(s)

	This are created when the (linkmgr) protocol is asked
	for a SAP with KBTeSCOLink type.
**/
	{
public:
	static CeSCOLink* NewLC(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aConnection);
	static CeSCOLink* NewL(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aConnection);
	~CeSCOLink();
	// from CServProviderBase
	virtual TInt SAPSetOption(TUint aLevel, TUint aName, const TDesC8& aOption);
	virtual TInt GetOption(TUint level, TUint name, TDes8& aOption) const;
	
// events for the SCO Link (not PHY)
	void PacketTypeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewPacket);

	TDes8& InboundFrame();
	TBool IsLinkProbablyPossible();
	TInt MakeConnection(CPhysicalLink* aPhysLink);	
	void SetExtOptions(TBTSyncConnectOpts aSyncOpts);
	
	TUint16 GetPacketMask() const;
	void GetExtOptions(TBTeSCOLinkParams& aOptions) const;

private:
	CeSCOLink(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aConnection);
	void ConstructL();
	TUint DoWrite(const TDesC8& aData, TUint aOptions, TSockAddr* aAddr);

	TUint MinBandwidth();
	TUint MaxBandwidth();

private:
	CHctlSynchronousDataFrame*	iOutboundSCOFrame;
	TBuf8<KInboundSCODataSize>	iInboundSCOFrame;
	TBTSyncPacketTypes			iUserPacketTypes;
	TUint						iTransmitBandwidth;
	TUint						iReceiveBandwidth;
	TUint16						iMaxLatency;
	TUint16						iVoiceSettings;
	TUint8						iRetransmissionEffort;
	};



//--------------------------------------------------
// STATE FACTORY
//--------------------------------------------------

/**
   Factory for the ACL Link states

   The states are flyweight classes
**/
NONSHARABLE_CLASS(CSyncLinkStateFactory) : public CBase
	{
public:
	static CSyncLinkStateFactory* NewL();
	~CSyncLinkStateFactory();
	enum TSyncLinkStates
		{
		EClosed,
		EListening,
		EAccepting,
		EWaitForStart,
		EWaitForStartError,
		EWaitForSCO,
		EOpen,	
		EClosing,
	// *** keep next one last ***
		ESyncLinkMaxState,
		};
	
	TSyncLinkState& GetState(TSyncLinkStates aState);
	TInt StateIndex(const TSyncLinkState* aState) const;
private:
	void ConstructL();
	TFixedArray<TSyncLinkState*, ESyncLinkMaxState> iStates;
	};



//--------------------------------------------------
// STATES 
//--------------------------------------------------

NONSHARABLE_CLASS(TSyncLinkState)
	{
public:
	TSyncLinkState(CSyncLinkStateFactory& aFactory);

	virtual void Enter(CBTSynchronousLink& aContext) const;

	virtual void ActiveOpen(CBTSynchronousLink& aContext) const;
	virtual TInt PassiveOpen(CBTSynchronousLink& aContext, TUint aQueSize) const;
	virtual void Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const;

	virtual void DataReceived(CBTSynchronousLink& aContext, const TDesC8& aData) const;
	virtual void ConnectComplete(CBTSynchronousLink& aContext, const TBTConnect& aSCOInfo, const TBTSyncConnectOpts& aSyncOpts) const;
	virtual void Disconnection(CBTSynchronousLink& aContext) const;
	virtual void Error(CBTSynchronousLink& aContext, TInt aErr) const;
	virtual TBool ConnectRequest(CBTSynchronousLink& aContext, const TBTConnect& aSCOLink, const CPhysicalLink& aPhysicalLink) const;

	virtual void PacketTypeChange(CBTSynchronousLink& aContext, THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewPacket) const;
	virtual TUint Write(CBTSynchronousLink& aContext, const TDesC8& aData, TUint aOptions, TSockAddr* aAddr) const;
	virtual void Start(CBTSynchronousLink& aContext) const;
	virtual void Exit(CBTSynchronousLink& aContext) const;
	virtual void Deletion(CBTSynchronousLink& aContext) const;
	virtual TBool IsIdle() const;
	virtual TBool IsOpen() const;
	virtual void Timeout(CBTSynchronousLink& aContext, TBasebandTimeout aTimeout) const;
	virtual void ParentClosing(CBTSynchronousLink& aContext) const;
	virtual TBool IsConnectPending() const;

protected:
	// Exits old state, sets the new state, and enters it.
	void ChangeState(CBTSynchronousLink& aContext, CSyncLinkStateFactory::TSyncLinkStates aState) const;
	void PanicInState(TLinkPanic aPanic) const;

protected:
	CSyncLinkStateFactory& iFactory;
#ifdef __FLOG_ACTIVE
	TBuf<48> iName;
#endif
	};


NONSHARABLE_CLASS(TSCOLinkStateClosed) : public TSyncLinkState
	{
public:
	TSCOLinkStateClosed(CSyncLinkStateFactory& aFactory);

	void Enter(CBTSynchronousLink& aContext) const;

	void Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	void Deletion(CBTSynchronousLink& aContext) const;
	void ActiveOpen(CBTSynchronousLink& aContext) const;
	TInt PassiveOpen(CBTSynchronousLink& aContext, TUint aQueSize) const;
	void Disconnection(CBTSynchronousLink& aContext) const;
	void Start(CBTSynchronousLink& aContext) const;
	TBool IsIdle() const;
	};


NONSHARABLE_CLASS(TSCOLinkStateWaitForSCO) : public TSyncLinkState
	{
public:
	TSCOLinkStateWaitForSCO(CSyncLinkStateFactory& aFactory);

	void Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	void ConnectComplete(CBTSynchronousLink& aContext, const TBTConnect& aSCOInfo, const TBTSyncConnectOpts& aSyncOpts) const;
	void Disconnection(CBTSynchronousLink& aContext) const;
	void Error(CBTSynchronousLink& aContext, TInt aErr) const;
	TBool IsConnectPending() const;
	};


NONSHARABLE_CLASS(TSCOLinkStateAccepting) : public TSyncLinkState
	{
public:
	TSCOLinkStateAccepting(CSyncLinkStateFactory& aFactory);

	void Enter(CBTSynchronousLink& aContext) const;
	void Exit(CBTSynchronousLink& aContext) const;	
	void Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	void Disconnection(CBTSynchronousLink& aContext) const;

// the logical link
	void ConnectComplete(CBTSynchronousLink& aContext, const TBTConnect& aSCOInfo, const TBTSyncConnectOpts& aSyncOpts) const;
	void Error(CBTSynchronousLink& aContext, TInt aError) const;
	void Timeout(CBTSynchronousLink& aContext, TBasebandTimeout aTimeout) const;

	void Deletion(CBTSynchronousLink& aContext) const;
	void ParentClosing(CBTSynchronousLink& aContext) const;
	};


NONSHARABLE_CLASS(TSCOLinkStateListening) : public TSyncLinkState
	{
public:
	TSCOLinkStateListening(CSyncLinkStateFactory& aFactory);

	void Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	void Deletion(CBTSynchronousLink& aContext) const;
	TBool ConnectRequest(CBTSynchronousLink& aContext, const TBTConnect& aSCOLink, const CPhysicalLink& aPhysicalLink) const;

	void Exit(CBTSynchronousLink& aContext) const;

private:
	void SpawnL(CBTSynchronousLink& aContext, CPhysicalLink& aPhysicalLink) const;
	};


NONSHARABLE_CLASS(TSCOLinkStateWaitForStart) : public TSyncLinkState
	{
public:
	TSCOLinkStateWaitForStart(CSyncLinkStateFactory& aFactory);

	void Start(CBTSynchronousLink& aContext) const;
	void Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	
	void Deletion(CBTSynchronousLink& aContext) const;
	void Disconnection(CBTSynchronousLink& aContext) const;

	void Error(CBTSynchronousLink& aContext, TInt aError) const;
	};

NONSHARABLE_CLASS(TSCOLinkStateWaitForStartError) : public TSyncLinkState
	{
public:
	TSCOLinkStateWaitForStartError(CSyncLinkStateFactory& aFactory);
	void Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const;

	void Start(CBTSynchronousLink& aContext) const;
	void Deletion(CBTSynchronousLink& aContext) const;
	};

NONSHARABLE_CLASS(TSCOLinkStateOpen) : public TSyncLinkState
	{
public:
	TSCOLinkStateOpen(CSyncLinkStateFactory& aFactory);

	void Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	void DataReceived(CBTSynchronousLink& aContext, const TDesC8& aData) const;
	void Disconnection(CBTSynchronousLink& aContext) const;
	void Error(CBTSynchronousLink& aContext, TInt aErr) const;
	void PacketTypeChange(CBTSynchronousLink& aContext, THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewPacket) const;
	TUint Write(CBTSynchronousLink& aContext, const TDesC8& aData, TUint aOptions, TSockAddr* aAddr) const;
	TBool IsOpen() const;

	void Exit(CBTSynchronousLink& aContext) const;
	};



NONSHARABLE_CLASS(TSCOLinkStateClosing) : public TSyncLinkState
	{
public:
	TSCOLinkStateClosing(CSyncLinkStateFactory& aFactory);
	void Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const;
	void ConnectComplete(CBTSynchronousLink& aContext, const TBTConnect& /*aSCOInfo*/, const TBTSyncConnectOpts& /*aSyncOpts*/) const;
	void ActiveOpen(CBTSynchronousLink& aContext) const;
	void Error(CBTSynchronousLink& /*aContext*/, TInt /*aError*/) const;
	void Deletion(CBTSynchronousLink& /*aContext*/) const;
	void Disconnection(CBTSynchronousLink& aContext) const;
	TBool IsIdle() const;
	};

#ifdef __FLOG_ACTIVE
#define STATENAME(x)  iName=_L(x)
#else
#define STATENAME(x)
#endif

#endif // _SCOSAP_H_
