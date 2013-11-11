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
// Dummy PR Factories class definition.
// 
//

/**
 @file
 @internalComponent
*/

#include <ecom/implementationproxy.h>
#include <in_sock.h>
#include <ss_glob.h>
#include <comms-infras/ss_log.h>
#include "dummypr_factory.h"
#include "dummypr_subconnprov.h"
#include "dummypr_connprov.h"
#include "dummypr_metaconnprov.h"
#include "dummypr_tiermanager.h"
#include "dummypr_network_flow.h"
#include "dummypr_flow.h"

#include <comms-infras/ss_msgintercept.h>

#ifdef __CFLOG_ACTIVE
#define KDummyTierManagerTag KESockTierTag
#define KDummyCPRTag KESockConnectionTag
#define KDummySCPRTag KESockSubConnectionTag
#define KDummyExtendedSCPRTag KESockSubConnectionTag
#define KDummyMCPRTag KESockMetaConnectionTag
#endif

using namespace ESock;


const TInt KDummyMajorVersionNumber = 0;
const TInt KDummyMinorVersionNumber = 1;
const TInt KDummyBuildVersionNumber = 1;


//-=========================================================
//
// ECOM data
//
//-=========================================================	
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(CDummyTierManagerFactory::iUid, CDummyTierManagerFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CDummyMetaConnectionProviderFactory::iUid, CDummyMetaConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CDummyConnectionProviderFactory::iUid, CDummyConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CDummyHangingConnectionProviderFactory::iUid, CDummyHangingConnectionProviderFactory::NewL),
    IMPLEMENTATION_PROXY_ENTRY(CDummyVanillaConnectionProviderFactory::iUid, CDummyVanillaConnectionProviderFactory::NewL), 	
	IMPLEMENTATION_PROXY_ENTRY(CDummySubConnectionProviderFactory::iUid, CDummySubConnectionProviderFactory::NewL),
    IMPLEMENTATION_PROXY_ENTRY(CDummyVanillaSubConnectionProviderFactory::iUid, CDummyVanillaSubConnectionProviderFactory::NewL),	
	IMPLEMENTATION_PROXY_ENTRY(CDummyExtendedSubConnectionProviderFactory::iUid, CDummyExtendedSubConnectionProviderFactory::NewL),

	// Flow and flow description
	IMPLEMENTATION_PROXY_ENTRY(CDummyFlowFactory::iUid, CDummyFlowFactory::NewL),
	};

/**
ECOM Implementation Factory
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
   }

//-=========================================================
//
// CDummyTierManagerFactory methods
//
//-=========================================================
CDummyTierManagerFactory* CDummyTierManagerFactory::NewL(TAny* aParentContainer)
	{
 	return new (ELeave) CDummyTierManagerFactory(TUid::Uid(CDummyTierManagerFactory::iUid),TUid::Uid(CDummyTierManagerFactory::iUid),*(reinterpret_cast<CTierManagerFactoryContainer*>(aParentContainer)));
	}

CDummyTierManagerFactory::CDummyTierManagerFactory(TUid aTierTypeId,TUid aFactoryUid, CTierManagerFactoryContainer& aParentContainer)
	: CTierManagerFactoryBase(aTierTypeId,aFactoryUid,aParentContainer)
	{
	//LOG_NODE_CREATE(KDummyTierManagerTag, CDummyTierManagerFactory);
	}

ESock::ACommsFactoryNodeId* CDummyTierManagerFactory::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
	{
	return CDummyTierManager::NewL(*this);
	}

//-=========================================================
//
// CDummyMetaConnectionProviderFactory implementation
//
//-=========================================================	
CDummyMetaConnectionProviderFactory* CDummyMetaConnectionProviderFactory::NewL(TAny* aParentContainer)
	{
 	return new (ELeave) CDummyMetaConnectionProviderFactory(TUid::Uid(CDummyMetaConnectionProviderFactory::iUid), *(reinterpret_cast<CMetaConnectionFactoryContainer*>(aParentContainer)));
	}

CDummyMetaConnectionProviderFactory::CDummyMetaConnectionProviderFactory(TUid aFactoryUid, CMetaConnectionFactoryContainer& aParentContainer)
	: CMetaConnectionProviderFactoryBase(aFactoryUid,aParentContainer)
	{
	//LOG_NODE_CREATE(KDummyMCPRTag, CDummyMetaConnectionProviderFactory);
	}

ESock::ACommsFactoryNodeId* CDummyMetaConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& aQuery)
	{
	const TMetaConnectionFactoryQuery& query = static_cast<const TMetaConnectionFactoryQuery&>(aQuery);
	return CDummyMetaConnectionProvider::NewL(*this,query.iProviderInfo);
	}
	
//-=========================================================
//
// CDummyConnectionProviderFactory methods
//
//-=========================================================	
CDummyConnectionProviderFactory* CDummyConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CDummyConnectionProviderFactory(TUid::Uid(CDummyConnectionProviderFactory::iUid), *reinterpret_cast<ESock::CConnectionFactoryContainer*>(aParentContainer));
    }
    
CDummyConnectionProviderFactory::CDummyConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer)
	: ESock::CConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
	//LOG_NODE_CREATE(KDummyCPRTag, CDummyConnectionProviderFactory);
    }

ACommsFactoryNodeId* CDummyConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
    {    
    CConnectionProviderBase* provider = CDummyConnectionProvider::NewL(*this, CDummyConnectionProvider::EConnIncoming);
    
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);

    return provider;
    }
    
CDummyHangingConnectionProviderFactory* CDummyHangingConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CDummyHangingConnectionProviderFactory(TUid::Uid(CDummyHangingConnectionProviderFactory::iUid), *reinterpret_cast<ESock::CConnectionFactoryContainer*>(aParentContainer));
    }
    
CDummyHangingConnectionProviderFactory::CDummyHangingConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer)
	: ESock::CConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
	//LOG_NODE_CREATE(KDummyCPRTag, CDummyHangingConnectionProviderFactory);
    }

ACommsFactoryNodeId* CDummyHangingConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
    {    
    CConnectionProviderBase* provider = CDummyConnectionProvider::NewL(*this, CDummyConnectionProvider::EConnNoIncoming);
    
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);

    return provider;
    }

CDummyVanillaConnectionProviderFactory* CDummyVanillaConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CDummyVanillaConnectionProviderFactory(TUid::Uid(CDummyVanillaConnectionProviderFactory::iUid), *reinterpret_cast<ESock::CConnectionFactoryContainer*>(aParentContainer));
    }
    
CDummyVanillaConnectionProviderFactory::CDummyVanillaConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer)
    : ESock::CConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
    //LOG_NODE_CREATE(KDummyCPRTag, CDummyHangingConnectionProviderFactory);
    }

ACommsFactoryNodeId* CDummyVanillaConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
    {    
    CConnectionProviderBase* provider = CDummyConnectionProvider::NewVanillaL(*this );
    
    ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);

    return provider;
    }

//-=========================================================
//
// CDummySubConnectionProviderFactory methods
//
//-=========================================================	
CDummySubConnectionProviderFactory* CDummySubConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CDummySubConnectionProviderFactory(TUid::Uid(CDummySubConnectionProviderFactory::iUid), 
                                            *reinterpret_cast<ESock::CSubConnectionFactoryContainer*>(aParentContainer));
    }
    
CDummySubConnectionProviderFactory::CDummySubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer)
	: CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
	//LOG_NODE_CREATE(KDummySCPRTag, CDummySubConnectionProviderFactory);
    }

ACommsFactoryNodeId* CDummySubConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& aQuery)
    {
    const TDefaultSCPRFactoryQuery& query = static_cast<const TDefaultSCPRFactoryQuery&>(aQuery);
    CSubConnectionProviderBase* provider = NULL;
    if (query.iSCPRType == RSubConnection::EAttachToDefault)
        {
        provider = CDummySubConnectionProvider::NewL(*this);
		ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
        }
    else if (query.iSCPRType == RSubConnection::ECreateNew)
        {
        provider = CDummySubConnectionProvider::NewL(*this);
		ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    return provider;
    }

//-=========================================================
//
// CDummyExtendedSubConnectionProviderFactory methods
//
//-=========================================================	

CDummyExtendedSubConnectionProviderFactory* CDummyExtendedSubConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CDummyExtendedSubConnectionProviderFactory(TUid::Uid(CDummyExtendedSubConnectionProviderFactory::iUid), 
                                            *reinterpret_cast<ESock::CSubConnectionFactoryContainer*>(aParentContainer));
    }
    
CDummyExtendedSubConnectionProviderFactory::CDummyExtendedSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer)
	: CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
	//LOG_NODE_CREATE(KDummyExtendedSCPRTag, CDummyExtendedSubConnectionProviderFactory);
    }

ACommsFactoryNodeId* CDummyExtendedSubConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& aQuery)
    {
    const TDefaultSCPRFactoryQuery& query = static_cast<const TDefaultSCPRFactoryQuery&>(aQuery);    
    CSubConnectionProviderBase* provider = NULL;

    if ((query.iSCPRType == RSubConnection::EAttachToDefault) || (query.iSCPRType == RSubConnection::ECreateNew))
        {
        provider = CDummyExtendedSubConnectionProvider::NewL(*this);
		ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    return provider;
    }



//-=========================================================
//
// CDummyExtendedSubConnectionProviderFactory methods
//
//-=========================================================    
CDummyVanillaSubConnectionProviderFactory* CDummyVanillaSubConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CDummyVanillaSubConnectionProviderFactory(TUid::Uid(CDummyVanillaSubConnectionProviderFactory::iUid), 
                                            *reinterpret_cast<ESock::CSubConnectionFactoryContainer*>(aParentContainer));
    }
    
CDummyVanillaSubConnectionProviderFactory::CDummyVanillaSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer)
    : CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
    //LOG_NODE_CREATE(KDummyExtendedSCPRTag, CDummyExtendedSubConnectionProviderFactory);
    }

ACommsFactoryNodeId* CDummyVanillaSubConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& aQuery)
    {
    const TDefaultSCPRFactoryQuery& query = static_cast<const TDefaultSCPRFactoryQuery&>(aQuery);    
    CSubConnectionProviderBase* provider = NULL;

    if ((query.iSCPRType == RSubConnection::EAttachToDefault) || (query.iSCPRType == RSubConnection::ECreateNew))
        {
        provider = CDummySubConnectionProvider::NewVanillaL(*this);
        ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    return provider;
    }

//-=========================================================
//
// CDummyFlowFactory methods
//
//-=========================================================	
CDummyFlowFactory* CDummyFlowFactory::NewL(TAny* aConstructionParameters)
	{
	CDummyFlowFactory* fact = new (ELeave) CDummyFlowFactory(
			TUid::Uid(iUid),
			*(reinterpret_cast<CSubConnectionFlowFactoryContainer*>(aConstructionParameters)));
	return fact;
	}

CDummyFlowFactory::CDummyFlowFactory(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer)
 : CSubConnectionFlowFactoryBase(aFactoryId, aParentContainer)
 	{
	//LOG_NODE_CREATE(KESockDataFactTag, CDummyFlowFactory)
 	}

CDummyFlowFactory::~CDummyFlowFactory()
	{
	//LOG_NODE_DESTROY(KESockDataFactTag, CDummyFlowFactory)
 	}

CSubConnectionFlowBase* CDummyFlowFactory::DoCreateFlowL(CProtocolIntfBase* aProtocolIntf, TFactoryQueryBase& aQuery)
	{
	const TDefaultFlowFactoryQuery& query = static_cast<const TDefaultFlowFactoryQuery&>(aQuery);
	return CDummyNetworkFlow::NewL(*this, query.iSCprId, aProtocolIntf);
	}

ACommsFactoryNodeId* CDummyFlowFactory::DoFindOrCreateObjectL(TFactoryQueryBase& aQuery)
	{
	const TDefaultFlowFactoryQuery& query = static_cast<const TDefaultFlowFactoryQuery&>(aQuery);
	User::After(0.3*1000000); //Artificial yield to test cancellation. Do not remove.

	if(query.iMessageId != TCFServiceProvider::TCommsBinderRequest::Id())
		{
		//the factory only reacts to creation triggered by TCommsBinderRequest
		//otherwise it bails out - flow not needed for the stack setup.
		return NULL;
		}
	// Find or create the ProtocolIntf instance corresponding to the Flow. If there is no ProtocolIntf instance, then fail.
	TDefaultProtocolIntfFactoryQuery queryProto(query.iCprId);
	CProtocolIntfBase* protocolInterface = SockManGlobals::Get()->iProtocolIntfFactories->FindOrCreateProtocolIntfL(Uid(), queryProto);
	if (protocolInterface == NULL)
    	{
    	LOG( ESockLog::Printf(KESockDataFactTag, _L8("CDummyFlowFactory %08x:\tCreateFlowL(protocolId '%08x') Cannot find protocol interface, bailing out"), this, Uid()) );
    	User::Leave(KErrNotFound);
    	}

	// The type of flow created is protocol id specific
	CSubConnectionFlowBase* flow = NULL;
	switch(query.iProtocolType)
		{
		case CDummyNetworkFlow::EProtocolId:
			flow = CDummyNetworkFlow::NewL(*this, query.iSCprId, protocolInterface);
			break;

        case CDummyFlow::EProtocolId:
            flow = CDummyFlow::NewL(*this, query.iSCprId, protocolInterface);
            break;
            
		default:
			User::Leave(KErrNotFound);
			break;
		}
	
	LOG( ESockLog::Printf(
		KESockDataFactTag,
		_L8("CDummyFlowFactory %08x:\tCreateFlowL(protocolId '%08x'): flow %08x"),
		this,
		Uid(),
		&flow));

	// The factory takes ultimate custody of the new flow
	CleanupStack::PushL(flow);
	AddManagedObjectL(*flow);
	CleanupStack::Pop(flow);

	return flow;
	}

/**
Description of the protocol this factory creates
*/
TServerProtocolDesc* CDummyFlowFactory::DoCreateFlowDescriptionL(TInt aProtocol)
	{
	TServerProtocolDesc* protocolDescription = new(ELeave) TServerProtocolDesc();

	// Poached from udp and modified to represent an rtp like protocol
	switch(aProtocol)
		{
        case CDummyNetworkFlow::EProtocolId:
            protocolDescription->iName          = KDummyProtocolName;
            protocolDescription->iAddrFamily    = KAfInet;
            protocolDescription->iSockType      = KSockDatagram;
            protocolDescription->iProtocol      = CDummyNetworkFlow::EProtocolId;
            protocolDescription->iVersion       = TVersion(KDummyMajorVersionNumber,
                                                         KDummyMinorVersionNumber,
                                                         KDummyBuildVersionNumber);
            protocolDescription->iByteOrder     = EBigEndian;
            protocolDescription->iServiceInfo   = KSIConnectionLess | KSIDatagram |
                                                  KSIGracefulClose | KSIPeekData |
                                                  KSIRequiresOwnerInfo;
            protocolDescription->iNamingServices= KNSNameResolution | KNSRequiresConnectionStartup;
            protocolDescription->iSecurity      = KSocketNoSecurity;
            protocolDescription->iMessageSize   = 65536-128; /*KSocketMessageSizeUndefined;*/
            protocolDescription->iServiceTypeInfo= ESocketSupport | ETransport |
                                                      EPreferMBufChains | ENeedMBufs |
                                                      EUseCanSend;
            protocolDescription->iNumSockets     = KUnlimitedSockets;
            break;
	    
		case CDummyFlow::EProtocolId:
			protocolDescription->iName		     = KDummyProtocolName;
			protocolDescription->iAddrFamily     = CDummyTierManagerFactory::iUid;
			protocolDescription->iSockType	     = KSockDatagram;
			protocolDescription->iProtocol	     = CDummyFlow::EProtocolId;
			protocolDescription->iVersion	     = TVersion(KDummyMajorVersionNumber,
                                                         KDummyMinorVersionNumber,
                                                         KDummyBuildVersionNumber);
			protocolDescription->iByteOrder	     = EBigEndian;
			protocolDescription->iServiceInfo	 = KSIConnectionLess | KSIDatagram |
                                                   KSIGracefulClose | KSIPeekData |
                                                   KSIRequiresOwnerInfo;
			protocolDescription->iNamingServices = KNSNameResolution | KNSRequiresConnectionStartup;
			protocolDescription->iSecurity	     = KSocketNoSecurity;
			protocolDescription->iMessageSize	 = 65536-128; /*KSocketMessageSizeUndefined;*/
			protocolDescription->iServiceTypeInfo= ESocketSupport | ETransport |
                                                    EPreferMBufChains | ENeedMBufs |
                                                    EUseCanSend;
			protocolDescription->iNumSockets	 = KUnlimitedSockets;
			break;
		
		default:
			User::Leave(KErrGeneral);
			break;
		}

	return protocolDescription;
	}

