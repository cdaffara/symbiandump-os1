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
#ifndef __LEGACY_LOOPBACK_FLOW_H__
#define __LEGACY_LOOPBACK_FLOW_H__

#include <e32base.h>
#include <e32std.h>

#include <comms-infras/nifif.h>
#include "binder.h"
#include <comms-infras/ss_subconnflow.h>

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <comms-infras/legacy_loopback_driver.h>
#else
//this header is not exported, needs to be a user include
#include "legacy_loopback_driver.h"
#endif

class CLegacyLoopbackProvision;
class TLegacyLoopbackIp6Provision;
class TLegacyLoopBackAgentProvision;

const TInt KLegacyLoopbackFlowImplementationUid = 0x102871C3;

// String literals for protocol name used during flow binding
_LIT8(KProtocol4, "ip");
_LIT8(KProtocol6, "ip6");

class CLegacyLoopbackSubConnectionFlowFactory : public ESock::CSubConnectionFlowFactoryBase
/**
*/
	{
public:
	static CLegacyLoopbackSubConnectionFlowFactory* NewL(TAny* aConstructionParameters);
	virtual ESock::CSubConnectionFlowBase* DoCreateFlowL(ESock::CProtocolIntfBase* aProtocolIntf, ESock::TFactoryQueryBase& aQuery);
protected:
	CLegacyLoopbackSubConnectionFlowFactory(TUid aFactoryId, ESock::CSubConnectionFlowFactoryContainer& aParentContainer);
	};

class CLegacyLoopbackBinder4;
class CLegacyLoopbackBinder6;
class CLegacyLoopbackFlowTestingSubscriber;
class CLegacyLoopbackSubConnectionFlow : public ESock::CSubConnectionFlowBase, public ESock::MFlowBinderControl
/**
*/
	{
	friend class CLegacyLoopbackSubConnectionFlowFactory;
public:
	static CLegacyLoopbackSubConnectionFlow* NewL(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);

	// from Messages::ANode (via CSubConnectionFlowBase)
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	// Methods called from Binders
	void FlowDown(TInt aError, TInt aAction = MNifIfNotify::EDisconnect);
	void Progress(TInt aStage, TInt aError);
	const TLegacyLoopbackIp6Provision* Ip6Provision() const;
	const TLegacyLoopBackAgentProvision* AgentProvision() const;

	// Functions for dealing with SCPR messages
	void StartFlowL();
	void StopFlow(TInt aError);
	void Destroy();
	void SubConnectionGoingDown();
	void SubConnectionError(TInt aError);

	// from MFlowBinderControl
	virtual ESock::MLowerControl* GetControlL(const TDesC8& aProtocol);
	virtual ESock::MLowerDataSender* BindL(const TDesC8& aProtocol, ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl);
	virtual void Unbind(ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl);
	virtual ESock::CSubConnectionFlowBase* Flow();

protected:
	CLegacyLoopbackSubConnectionFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);
	virtual ~CLegacyLoopbackSubConnectionFlow();

	// CSubConnectionFlowBase
	virtual ESock::MFlowBinderControl* DoGetBinderControlL();

	// Utilities for posting SCPR messages
	void PostProgressMessage(TInt aStage, TInt aError);
	void PostDataClientStartedMessage();
	void PostFlowDownMessage(TInt aError, TInt aAction = MNifIfNotify::EDisconnect);
	void MaybePostDataClientIdle();

    inline void SetBinder4(CLegacyLoopbackBinder4* aBinder4);
    inline void SetBinder6(CLegacyLoopbackBinder6* aBinder4);

private:
	virtual void ProvisionConfig(const ESock::RMetaExtensionContainerC& aConfigData);
	void AgentProvisionConfigL();

protected:
    __FLOG_DECLARATION_MEMBER;

	CLegacyLoopbackBinder4* iBinder4;					// IPv4 binder
	CLegacyLoopbackBinder6* iBinder6;					// IPv6 binder
	const CLegacyLoopbackProvision* iProvision;		// cached pointer to provisioning structure in SCPR (in control side memory)
	const TLegacyLoopBackAgentProvision* iAgentProvision;// cached pointer to provisioning structure in SCPR (in control side memory)
	TInt iSavedError;							// errors during processing of ProvisionConfig message
	TBool iFlowStarted:1;

public:
	TBool iDisableStart:1;

	RLegacyLoopbackDriver iLoopbackLdd;
	};

enum TLegacyLoopbackPanicNum
	{
	EUnexpectedMessage
	};

void Panic(TLegacyLoopbackPanicNum);

#include "flow.inl"

#endif // __LEGACY_LOOPBACK_FLOW_H__
