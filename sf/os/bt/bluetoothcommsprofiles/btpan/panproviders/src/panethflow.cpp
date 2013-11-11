// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PAN Ethernet Flow
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "paneth802.h"
#include "panprovisioninfo.h"
#include "CBnepBridge.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_PROVIDERS);
#endif

#ifdef _DEBUG
PANICCATEGORY("panethfl");
#endif

#ifdef ESOCK_EXTLOG_ACTIVE
_LIT8(KBnepSubTag, "panflow");
#endif

CPanEthFlowFactory* CPanEthFlowFactory::NewL(TAny* aConstructionParameters)
    {
	CPanEthFlowFactory* ptr = new (ELeave) CPanEthFlowFactory(TUid::Uid(CPanEthFlowFactory::iUid), *(reinterpret_cast<ESock::CSubConnectionFlowFactoryContainer*>(aConstructionParameters)));
	return ptr;
    }

ESock::CSubConnectionFlowBase* CPanEthFlowFactory::DoCreateFlowL(ESock::CProtocolIntfBase* aProtocolIntf, ESock::TFactoryQueryBase& aQuery)
    {
    const ESock::TDefaultFlowFactoryQuery& query = static_cast<const ESock::TDefaultFlowFactoryQuery&>(aQuery);	
    return CPanEtherFlow::NewL(*this, query.iSCprId, aProtocolIntf);
    }

CPanEthFlowFactory::CPanEthFlowFactory(TUid aFactoryId, ESock::CSubConnectionFlowFactoryContainer& aParentContainer)
    : ESock::CSubConnectionFlowFactoryBase(aFactoryId, aParentContainer)
    {
    }
    
CPanEtherFlow* CPanEtherFlow::NewL(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf)
    {
	CPanEtherFlow* s=new (ELeave) CPanEtherFlow(aFactory, aSubConnId, aProtocolIntf);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;    
    }

CPanEtherFlow::CPanEtherFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf)
    : CLANLinkCommon(aFactory, aSubConnId, aProtocolIntf)
    {
    NM_LOG_NODE_CREATE(KBnepSubTag, CPanEtherFlow);
    }
	
CPanEtherFlow::~CPanEtherFlow()
    {
    // Reset the provisioned Bnep Connection Manager
    CPanProvisionInfo* panInfo = const_cast<CPanProvisionInfo*>(static_cast<const CPanProvisionInfo*>(AccessPointConfig().FindExtension(CPanProvisionInfo::TypeId())));
    ASSERT_DEBUG(panInfo);
    panInfo->Reset();

    NM_LOG_NODE_DESTROY(KBnepSubTag, CPanEtherFlow);
    }

ESock::MLowerDataSender* CPanEtherFlow::BindL(const TDesC8& aProtocol, ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl)
    {
    CPanProvisionInfo* panInfo = const_cast<CPanProvisionInfo*>(static_cast<const CPanProvisionInfo*>(AccessPointConfig().FindExtension(CPanProvisionInfo::TypeId())));
    ASSERT_DEBUG(panInfo);

    if (panInfo->BnepConnectionMgr() == Messages::TNodeId::NullId())
        {
        ASSERT_DEBUG(iPktDrv);
        // Passing the packet driver owner to BNEP and getting the
        // BnepConnectionManager set into the provision info for PAN
        panInfo->SetPacketDriverOwner(*iPktDrvOwner);
    	iPktDrv->Notification((TAgentToNifEventType)EPanAgentToNifGetPanProvisionInfo, static_cast<TAny*>(panInfo));
        ASSERT_DEBUG(panInfo->BnepConnectionMgr() != Messages::TNodeId::NullId());
        }
    return CLANLinkCommon::BindL(aProtocol, aReceiver, aControl);
    }


