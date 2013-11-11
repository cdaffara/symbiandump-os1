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
// PAN Ethernet MCPR
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "paneth802.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_PROVIDERS);
#endif

#ifdef _DEBUG
PANICCATEGORY("panethmc");
#endif

#ifdef ESOCK_EXTLOG_ACTIVE
#define KPanMCprTag KESockMetaConnectionTag
_LIT8(KPanMCprSubTag, "panmcpr");
#endif

//-=========================================================
//
//CPanEthMetaConnectionProvider implementation
//
//-=========================================================

CPanEthMetaConnectionProvider* CPanEthMetaConnectionProvider::NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo)
    {
    CPanEthMetaConnectionProvider* self = new (ELeave) CPanEthMetaConnectionProvider(aFactory, aProviderInfo, EthMCprStates::stateMap::Self());
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CPanEthMetaConnectionProvider::ConstructL()
	{
	CEthMetaConnectionProvider::ConstructL();
    iAgentHandler = new (ELeave) CPanAgtNotificationHandler;
	SetAgentNotificationHandlerL(iAgentHandler);
	
	ESock::RMetaExtensionContainer mec;
	mec.Open(AccessPointConfig());
	CleanupClosePushL(mec);
	
	CPanProvisionInfo* panInfo = new (ELeave) CPanProvisionInfo;
	CleanupStack::PushL(panInfo);
	mec.AppendExtensionL(panInfo);
	CleanupStack::Pop(panInfo);
	
	AccessPointConfig().Close();
	AccessPointConfig().Open(mec);
	CleanupStack::PopAndDestroy(&mec);
	}

CPanEthMetaConnectionProvider::CPanEthMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory,
    						   const ESock::TProviderInfo& aProviderInfo,
							   const MeshMachine::TNodeActivityMap& aActivityMap)
    : CEthMetaConnectionProvider(aFactory,aProviderInfo,aActivityMap)
	{
	NM_LOG_NODE_CREATE(KPanMCprSubTag, CPanEthMetaConnectionProvider);
	}

CPanEthMetaConnectionProvider::~CPanEthMetaConnectionProvider()
    {
    delete iAgentHandler;
    NM_LOG_NODE_DESTROY(KPanMCprSubTag, CPanEthMetaConnectionProvider);
    }

TInt CPanAgtNotificationHandler::NotificationFromAgent(TAgentToFlowEventType aEvent, TAny* aInfo)
    {
    switch (aEvent)
        {
        case EPanAgentToNifGetConnectionManager:
            // This notification is no longer supported under 399
            return KErrNotSupported;            
            //break;
            
        case EPanAgentToNifGetPanProvisionInfo:
            // Get the control thread Id only once
            CPanProvisionInfo* panInfo = const_cast<CPanProvisionInfo*>(static_cast<const CPanProvisionInfo*>(GetExtension(CPanProvisionInfo::TypeId())));
            ASSERT_DEBUG(panInfo);
            (*reinterpret_cast<const CPanProvisionInfo**>(aInfo)) = panInfo;
            return KErrNone;
            //break;
        }
        
    return CAgentNotificationHandler::NotificationFromAgent(aEvent, aInfo);
    }


CPanEthMCPRFactory* CPanEthMCPRFactory::NewL(TAny* aParentContainer)
    {
    CPanEthMCPRFactory* self = new (ELeave) CPanEthMCPRFactory(TUid::Uid(CPanEthMCPRFactory::iUid),
      *reinterpret_cast<ESock::CMetaConnectionFactoryContainer*>(aParentContainer));
    return self;
    }

CPanEthMCPRFactory::CPanEthMCPRFactory(TUid aFactoryId, ESock::CMetaConnectionFactoryContainer& aParentContainer)
    : ESock::CMetaConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
    }

ESock::ACommsFactoryNodeId* CPanEthMCPRFactory::DoCreateObjectL(ESock::TFactoryQueryBase& aQuery)
    {
	const ESock::TMetaConnectionFactoryQuery& query = static_cast<const ESock::TMetaConnectionFactoryQuery&>(aQuery);
    return CPanEthMetaConnectionProvider::NewL(*this, query.iProviderInfo);
    }

