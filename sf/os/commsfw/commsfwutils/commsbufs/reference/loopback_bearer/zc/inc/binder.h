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
#ifndef __ZEROCOPY_LOOPBACK_BINDER_H__
#define __ZEROCOPY_LOOPBACK_BINDER_H__

#include <ip4_hdr.h>
#include <ip6_hdr.h>
#include <udp_hdr.h>

#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/commsdebugutility.h>

#include "flow.h"

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <comms-infras/zerocopy_loopback_driver.h>
#include <ss_protflow.h>
#include "es_protbinder.h"
#else
//this header is not exported, needs to be a user include
#include "zerocopy_loopback_driver.h"
#include <comms-infras/ss_protflow.h>
#include <comms-infras/es_protbinder.h>
#endif

#include <nifmbuf.h>


class CZeroCopyLoopbackErrorOneShot : public CAsyncOneShot
/**
Class used to asynchronously signal a binder error
*/
	{
	friend class CDummyNifBinder4;		// for iUpperControl
public:
	CZeroCopyLoopbackErrorOneShot();
	void RunL();
	void Schedule(ESock::MUpperControl* iUpperControl);

private:
	ESock::MUpperControl* iUpperControl;
	};

class CZeroCopyLoopbackSubConnectionFlow;
class CZeroCopyLoopbackFlowTestingSubscriber;

class CZeroCopyLoopbackBinder4 : public CBase, public ESock::MLowerDataSender, public ESock::MLowerControl
/**
IP4 binder
*/
	{
public:
	enum { KLoopbackBearerMTU = 1532 };

	static CZeroCopyLoopbackBinder4* NewL(CZeroCopyLoopbackSubConnectionFlow& aZeroCopyLoopbackSubConnectionFlow);
	virtual ~CZeroCopyLoopbackBinder4();
	
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
	CZeroCopyLoopbackBinder4(CZeroCopyLoopbackSubConnectionFlow& aZeroCopyLoopbackSubConnectionFlow);

private:
    void UpdateHeaders(TInet6HeaderIP4* aIp4, TInet6HeaderUDP* aUdp);
	inline CZeroCopyLoopbackSubConnectionFlow* Flow();

private:
   	TUint32 iLocalAddressBase;
   	TUint32 iLocalAddress;
    CZeroCopyLoopbackSubConnectionFlow& iZeroCopyLoopbackSubConnectionFlow;
	ESock::MUpperDataReceiver* iUpperReceiver;
	ESock::MUpperControl* iUpperControl;
	CZeroCopyLoopbackErrorOneShot iErrorOneShot;

	
	// Loopback driver
	TBool iLoopbackLoadAttempted;	// we only try once
	RZeroCopyLoopbackDriver iDrv;
	RMBufAllocator iAllocator;

	class CDrvSender : public CActive
		{
	public:
		CDrvSender(TInt aPriority);
		~CDrvSender();
		void Start(CZeroCopyLoopbackBinder4* aBinder, RZeroCopyLoopbackDriver aDrv); 
		void QueueForSend(RMBufPacket& aPkt);
	private:
		void RunL();
		void DoCancel();
		void SendPacket();

		CZeroCopyLoopbackBinder4* iBinder;
		RMBufPktQ iSendQueue;
		RZeroCopyLoopbackDriver iDrv;

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
		void Start(CZeroCopyLoopbackBinder4* aBinder, RZeroCopyLoopbackDriver aDrv); 
	private:
		void RunL();
		void DoCancel();
		
		void RequestReceipt();
		
		CZeroCopyLoopbackBinder4* iBinder;
		RZeroCopyLoopbackDriver iDrv;
		RMBufPacket iRxPkt;
		RCommsBufPond iPond;
		};
	CDrvReceiver* iDrvReceiver;

public:
	CZeroCopyLoopbackFlowTestingSubscriber* iTestSubscriber;

protected:
    __FLOG_DECLARATION_MEMBER;
	};

class CZeroCopyLoopbackBinder6 : public CBase, public ESock::MLowerDataSender, public ESock::MLowerControl
/**
IP6 binder
*/
	{
public:
	enum { KLoopbackBearerMTU = 1532 };

	static CZeroCopyLoopbackBinder6* NewL(CZeroCopyLoopbackSubConnectionFlow& aZeroCopyLoopbackSubConnectionFlow);
	virtual ~CZeroCopyLoopbackBinder6();
	
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
	CZeroCopyLoopbackBinder6(CZeroCopyLoopbackSubConnectionFlow& aZeroCopyLoopbackSubConnectionFlow);

private:
    void UpdateHeaders(TInet6HeaderIP* aIp6, TInet6HeaderUDP* /*aUdp*/);
	void StaticDnsConfiguration(TBinderConfig6& aConfig);
	inline CZeroCopyLoopbackSubConnectionFlow* Flow();

private:
    CZeroCopyLoopbackSubConnectionFlow& iZeroCopyLoopbackSubConnectionFlow;
	ESock::MUpperDataReceiver* iUpperReceiver;
	ESock::MUpperControl* iUpperControl;

protected:
    __FLOG_DECLARATION_MEMBER;

	};

//
// Inlines
//

CZeroCopyLoopbackSubConnectionFlow* CZeroCopyLoopbackBinder4::Flow()
	{
	return &iZeroCopyLoopbackSubConnectionFlow;
	}

CZeroCopyLoopbackSubConnectionFlow* CZeroCopyLoopbackBinder6::Flow()
	{
	return &iZeroCopyLoopbackSubConnectionFlow;
	}

// various things that will get set up on each interface by dummy nifs
// will be added to the address base to make the broadcast address...
const TUint KBroadcastAddressSuffix = 255;
// some arbitrary num to add to the base to give the default gateway machine...
const TUint KDefaultGatewayAddressSuffix = 10;
// some arbitrary num to add to the base to give the secondary dns server...
const TUint KSecondaryDnsAddressSuffix = 11;
// obviously all the above addresses are totally arbitrary to a certain extent... :-)


const TUid KZeroCopyLoopbackTestingPubSubUid = {0x10286A11};

NONSHARABLE_CLASS(CZeroCopyLoopbackFlowTestingSubscriber) : public CActive
	{
public:
	TBool IsEnabled()
		{
		return iIsEnabled;
		}

	static TBool ShouldRun(TUint aApId);

	static CZeroCopyLoopbackFlowTestingSubscriber* NewL(CZeroCopyLoopbackSubConnectionFlow& aFlow, TUint aApId)
		{
		CZeroCopyLoopbackFlowTestingSubscriber* ats = new(ELeave)CZeroCopyLoopbackFlowTestingSubscriber(aFlow, aApId);
		CleanupStack::PushL(ats);
		ats->ConstructL();
		CleanupStack::Pop(ats);
		return ats;
		}

	virtual ~CZeroCopyLoopbackFlowTestingSubscriber();

public:
	CZeroCopyLoopbackFlowTestingSubscriber(CZeroCopyLoopbackSubConnectionFlow& aFlow, TUint aApId);
	void ConstructL();
	
	void RunL();
	void DoCancel();

private:
	CZeroCopyLoopbackSubConnectionFlow& iFlow;
	TInt iApId;
	RProperty iProperty;
	TBool iIsEnabled : 1;
	};

#endif // __ZEROCOPY_LOOPBACK_BINDER_H__
