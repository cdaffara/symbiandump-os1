/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#ifndef __LEGACY_LOOPBACK_BINDER_H__
#define __LEGACY_LOOPBACK_BINDER_H__

#include <ip4_hdr.h>
#include <ip6_hdr.h>
#include <udp_hdr.h>

#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/commsdebugutility.h>

#include "flow.h"

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <comms-infras/legacy_loopback_driver.h>
#include <ss_protflow.h>
#include "es_protbinder.h"
#else
//this header is not exported, needs to be a user include
#include "legacy_loopback_driver.h"
#include <comms-infras/ss_protflow.h>
#include <comms-infras/es_protbinder.h>
#endif

#include <nifmbuf.h>


class CLegacyLoopbackErrorOneShot : public CAsyncOneShot
/**
Class used to asynchronously signal a binder error
*/
	{
	friend class CDummyNifBinder4;		// for iUpperControl
public:
	CLegacyLoopbackErrorOneShot();
	void RunL();
	void Schedule(ESock::MUpperControl* iUpperControl);

private:
	ESock::MUpperControl* iUpperControl;
	};

class CLegacyLoopbackSubConnectionFlow;
class CLegacyLoopbackFlowTestingSubscriber;

class CLegacyLoopbackBinder4 : public CBase, public ESock::MLowerDataSender, public ESock::MLowerControl
/**
IP4 binder
*/
	{
public:
	enum { KLoopbackBearerMTU = 1532 };

	static CLegacyLoopbackBinder4* NewL(CLegacyLoopbackSubConnectionFlow& aLegacyLoopbackSubConnectionFlow);
	virtual ~CLegacyLoopbackBinder4();
	
	MLowerDataSender* Bind(ESock::MUpperDataReceiver& aUpperReceiver , ESock::MUpperControl& aUpperControl);
	void Unbind (ESock::MUpperDataReceiver& aUpperReceiver, ESock::MUpperControl& aUpperControl);

	void BinderReady();
	void ProcessPacket(RMBufChain& aData);

	// from MLowerDataSender
	virtual ESock::MLowerDataSender::TSendResult Send(RMBufChain& aData);
	
	// from MLowerControl
	virtual TInt GetName(TDes& aName);
	virtual TInt BlockFlow(ESock::MLowerControl::TBlockOption aOption);
	virtual TInt GetConfig(TBinderConfig& aConfig);
	virtual TInt Control(TUint aLevel, TUint aName, TDes8& aOption);

	// Utility functions
	TBool MatchesUpperControl(ESock::MUpperControl* aUpperControl) const;

protected:
	CLegacyLoopbackBinder4(CLegacyLoopbackSubConnectionFlow& aLegacyLoopbackSubConnectionFlow);

private:
    void UpdateHeaders(TInet6HeaderIP4* aIp4, TInet6HeaderUDP* aUdp);
	inline CLegacyLoopbackSubConnectionFlow* Flow();
	void LogPacketDetails(TInet6HeaderIP4* aIp4, const TDesC8& aCaption);

private:
   	TUint32 iLocalAddressBase;
   	TUint32 iLocalAddress;
    CLegacyLoopbackSubConnectionFlow& iLegacyLoopbackSubConnectionFlow;
	ESock::MUpperDataReceiver* iUpperReceiver;
	ESock::MUpperControl* iUpperControl;
	CLegacyLoopbackErrorOneShot iErrorOneShot;

	// Loopback driver
	TBool iLoopbackLoadAttempted;	// we only try once
	RLegacyLoopbackDriver iDrv;
	RMBufAllocator iAllocator;

	class CDrvSender : public CActive
		{
	public:
		CDrvSender(TInt aPriority);
		~CDrvSender();
		void Start(CLegacyLoopbackBinder4* aBinder, RLegacyLoopbackDriver aDrv); 
		void QueueForSend(RMBufPacket& aPkt);
	private:
		void RunL();
		void DoCancel();
		void SendPacket();

		CLegacyLoopbackBinder4* iBinder;
		RMBufPktQ iSendQueue;
		RLegacyLoopbackDriver iDrv;
		RMBufPacket iSendPkt;

		TInt iManyBufCount;
		TInt iOneBufCount;

		__FLOG_DECLARATION_MEMBER;
		};
	CDrvSender* iDrvSender;
	
	class CDrvReceiver : public CActive
		{
	public:
		CDrvReceiver(TInt aPriority);
		~CDrvReceiver();
		void Start(CLegacyLoopbackBinder4* aBinder, RLegacyLoopbackDriver aDrv); 
	private:
		void RunL();
		void DoCancel();
		
		void RequestReceipt();
		
		CLegacyLoopbackBinder4* iBinder;
		RLegacyLoopbackDriver iDrv;
		TBuf8<RLegacyLoopbackDriver::KLoopbackMTU> iDesBuf;
		RMBufPacket iPkt;
		RCommsBufPond iPond;
		TPtr8 iRxPtr;
		};
	CDrvReceiver* iDrvReceiver;

public:
	CLegacyLoopbackFlowTestingSubscriber* iTestSubscriber;

protected:
    __FLOG_DECLARATION_MEMBER;
	};

class CLegacyLoopbackBinder6 : public CBase, public ESock::MLowerDataSender, public ESock::MLowerControl
/**
IP6 binder
*/
	{
public:
	enum { KLoopbackBearerMTU = 1532 };

	static CLegacyLoopbackBinder6* NewL(CLegacyLoopbackSubConnectionFlow& aLegacyLoopbackSubConnectionFlow);
	virtual ~CLegacyLoopbackBinder6();
	
	MLowerDataSender* Bind(ESock::MUpperDataReceiver& aUpperReceiver , ESock::MUpperControl& aUpperControl);
	void Unbind (ESock::MUpperDataReceiver& aUpperReceiver, ESock::MUpperControl& aUpperControl);

	void BinderReady();
	void ProcessPacket(RMBufChain& aData);

	// from MLowerDataSender
	virtual ESock::MLowerDataSender::TSendResult Send(RMBufChain& aData);
	
	// from MLowerControl
	virtual TInt GetName(TDes& aName);
	virtual TInt BlockFlow(MLowerControl::TBlockOption /*aOption*/);
	virtual TInt GetConfig(TBinderConfig& aConfig);
	virtual TInt Control(TUint aLevel, TUint aName, TDes8& aOption);

	// Utility functions
	TBool MatchesUpperControl(ESock::MUpperControl* aUpperControl) const;

protected:
	CLegacyLoopbackBinder6(CLegacyLoopbackSubConnectionFlow& aLegacyLoopbackSubConnectionFlow);

private:
    void UpdateHeaders(TInet6HeaderIP* aIp6, TInet6HeaderUDP* /*aUdp*/);
	void StaticDnsConfiguration(TBinderConfig6& aConfig);
	inline CLegacyLoopbackSubConnectionFlow* Flow();

private:
    CLegacyLoopbackSubConnectionFlow& iLegacyLoopbackSubConnectionFlow;
	ESock::MUpperDataReceiver* iUpperReceiver;
	ESock::MUpperControl* iUpperControl;

protected:
    __FLOG_DECLARATION_MEMBER;

	};

//
// Inlines
//

CLegacyLoopbackSubConnectionFlow* CLegacyLoopbackBinder4::Flow()
	{
	return &iLegacyLoopbackSubConnectionFlow;
	}

CLegacyLoopbackSubConnectionFlow* CLegacyLoopbackBinder6::Flow()
	{
	return &iLegacyLoopbackSubConnectionFlow;
	}

// various things that will get set up on each interface by dummy nifs
// will be added to the address base to make the broadcast address...
const TUint KBroadcastAddressSuffix = 255;
// some arbitrary num to add to the base to give the default gateway machine...
const TUint KDefaultGatewayAddressSuffix = 10;
// some arbitrary num to add to the base to give the secondary dns server...
const TUint KSecondaryDnsAddressSuffix = 11;
// obviously all the above addresses are totally arbitrary to a certain extent... :-)


const TUid KLegacyLoopbackTestingPubSubUid = {0x102871C7};

NONSHARABLE_CLASS(CLegacyLoopbackFlowTestingSubscriber) : public CActive
	{
public:
	TBool IsEnabled()
		{
		return iIsEnabled;
		}

	static TBool ShouldRun(TUint aApId);

	static CLegacyLoopbackFlowTestingSubscriber* NewL(CLegacyLoopbackSubConnectionFlow& aFlow, TUint aApId)
		{
		CLegacyLoopbackFlowTestingSubscriber* ats = new(ELeave)CLegacyLoopbackFlowTestingSubscriber(aFlow, aApId);
		CleanupStack::PushL(ats);
		ats->ConstructL();
		CleanupStack::Pop(ats);
		return ats;
		}

	virtual ~CLegacyLoopbackFlowTestingSubscriber();

public:
	CLegacyLoopbackFlowTestingSubscriber(CLegacyLoopbackSubConnectionFlow& aFlow, TUint aApId);
	void ConstructL();
	
	void RunL();
	void DoCancel();

private:
	CLegacyLoopbackSubConnectionFlow& iFlow;
	TInt iApId;
	RProperty iProperty;
	TBool iIsEnabled : 1;
	};

#endif // __LEGACY_LOOPBACK_BINDER_H__
