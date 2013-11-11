// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PAN Ethernet MCPR & Flow
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PANETHMCPR_H
#define SYMBIAN_PANETHMCPR_H

#include <comms-infras/agentmessages.h>
#include <networking/ethmcpr.h>
#include <networking/ethproto.h>
#include "pancommon.h"
#include "panprovisioninfo.h"

class CPanEthMetaConnectionProviderFactory;
class CPanAgtNotificationHandler;


/**
PAN override of the ethernet meta connection provider
@internalTechnology
*/
NONSHARABLE_CLASS(CPanEthMetaConnectionProvider) : public CEthMetaConnectionProvider
    {
    friend class EthMCprStates::TSendProvision;

public:
    typedef CPanEthMetaConnectionProviderFactory FactoryType;

	static CPanEthMetaConnectionProvider* NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);

    ~CPanEthMetaConnectionProvider();
    
protected:
    CPanEthMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory,
							      const ESock::TProviderInfo& aProviderInfo,
							      const MeshMachine::TNodeActivityMap& aActivityMap);    
	void ConstructL();
    CPanAgtNotificationHandler* iAgentHandler;
    };


NONSHARABLE_CLASS(CPanAgtNotificationHandler) : public CAgentNotificationHandler
   {
public:   
    virtual TInt NotificationFromAgent(TAgentToFlowEventType aEvent, TAny* aInfo);
   };


NONSHARABLE_CLASS(CPanEtherFlow) : public CLANLinkCommon
    {
public:
	static CPanEtherFlow* NewL(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);
	CPanEtherFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);
	virtual ~CPanEtherFlow();
	virtual ESock::MLowerDataSender* BindL(const TDesC8& aProtocol, ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl);	


    inline const CPanProvisionInfo* GetPanProvisionInfo()
        {
        return static_cast<const CPanProvisionInfo*>(AccessPointConfig().FindExtension(CPanProvisionInfo::TypeId()));
        }
    };


NONSHARABLE_CLASS(CPanEthMCPRFactory) : public ESock::CMetaConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x102835A2 };
	static CPanEthMCPRFactory* NewL(TAny* aParentContainer);

protected:
	CPanEthMCPRFactory(TUid aFactoryId, ESock::CMetaConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};
	

NONSHARABLE_CLASS(CPanEthFlowFactory) : public ESock::CSubConnectionFlowFactoryBase
	{
public:
    enum { iUid = 0x102835A3 };
	static CPanEthFlowFactory* NewL(TAny* aConstructionParameters);
	virtual ESock::CSubConnectionFlowBase* DoCreateFlowL(ESock::CProtocolIntfBase* aProtocolIntf, ESock::TFactoryQueryBase& aQuery);

protected:
	CPanEthFlowFactory(TUid aFactoryId, ESock::CSubConnectionFlowFactoryContainer& aParentContainer);
	};


#endif
//SYMBIAN_PANETHMCPR_H
