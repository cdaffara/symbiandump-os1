// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
*/

#define SYMBIAN_NETWORKING_UPS

#include <comms-infras/ss_log.h>
#include <comms-infras/api_ext_msg.h>
#include <es_ini.h>
#include <ss_glob.h>
#include <comms-infras/ss_metaconnprov_internal.h>
#include <comms-infras/ss_connselect.h>
#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include <comms-infras/ss_tiermanager_internal.h>
#include <comms-infras/ss_protcfgldr.h>
#include <commsdattypesv1_1.h> // CommsDat

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1_partner.h>
#include <commsdattypesv1_1_internal.h>
#endif

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksmtcnp, "ESockSSocksmtcnp");
#endif

using namespace ESock;
using namespace CommsDat;
using namespace Messages;
using namespace Factories;

//
//CConfigAccessPointConfig
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(CConfigAccessPointConfig, CConfigAccessPointConfig::EUid, CConfigAccessPointConfig::ETypeId)
END_ATTRIBUTE_TABLE()

EXPORT_C CConfigAccessPointConfig* CConfigAccessPointConfig::NewL()
    {
    return new(ELeave) CConfigAccessPointConfig();
    }

EXPORT_C CConfigAccessPointConfig::~CConfigAccessPointConfig()
    {
    iLayers.Close();
	iSCprs.Close();
    }

EXPORT_C void CConfigAccessPointConfig::AppendLayerL(const TConfigAccessPointLayer& aLayer)
    {
    for (TUint i = 0; i < iLayers.Count(); i++)
		if (iLayers[i].Id() == aLayer.Id())
			return;

    iLayers.AppendL(aLayer);
    }

EXPORT_C TBool CConfigAccessPointConfig::LayerExists(const TConfigAccessPointLayer& aLayer) const
    {
    for (TUint i = 0; i < iLayers.Count(); i++)
		if (iLayers[i].Id() == aLayer.Id())
			return ETrue;

    return EFalse;
    }

EXPORT_C void CConfigAccessPointConfig::GetTopLayersL(TUid aSCprUid, RPointerArray<TConfigAccessPointLayer>& aTopLayers) const
    {
    for (TUint i = 0; i < iLayers.Count(); i++)
		{
		if (iLayers[i].SCprUid() == aSCprUid && iLayers[i].TopLevel())
			aTopLayers.AppendL(&iLayers[i]);
		}
    }

EXPORT_C const TConfigAccessPointLayer& CConfigAccessPointConfig::GetLayerL(TUint aId) const
    {
    for (TUint i = 0; i < iLayers.Count(); i++)
		if (iLayers[i].Id() == aId)
			return iLayers[i];

    User::Leave(KErrNotFound);

    return iLayers[0]; // this will never be reached.
    }

EXPORT_C TUint CConfigAccessPointConfig::GetLayerCount() const
    {
    return iLayers.Count();
    }

EXPORT_C void CConfigAccessPointConfig::AppendSCprL(const TUid& aUid)
	{
	if (iSCprs.Find(aUid) == KErrNotFound)
		iSCprs.AppendL(aUid);
	}

EXPORT_C const RArray<TUid>& CConfigAccessPointConfig::SCprs() const
	{
	return iSCprs;
	}

//
//CMetaConnectionProviderBase
EXPORT_C CMetaConnectionProviderBase::CMetaConnectionProviderBase(CMetaConnectionProviderFactoryBase& aFactory,
                                                                  const TProviderInfo& aProviderInfo,
                                                                  const MeshMachine::TNodeActivityMap& aActivityMap)
:	CMMCommsProviderBase( aFactory,aActivityMap ),
    iProviderInfo(aProviderInfo),
	iBlockingDestroy(0)
/**
Constructor for CMetaConnectionProviderBase
@param aFactory Parent container for the provider
*/
	{
	LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("CMetaConnectionProviderBase %08x:\tcreated [%d], factory Uid %08x"),
		 this, sizeof(CMetaConnectionProviderBase), Factory().Uid().iUid));
	}

EXPORT_C CMetaConnectionProviderBase::~CMetaConnectionProviderBase()
/**
Destructor for CMetaConnectionProviderBase.
Once constructed, a meta-CPR should only be destroyed when absolutely not needed any more. It should be the last
thing to die and therefore doesn't need to tell anyone about it.
*/
	{
	if (iTierManager)
		{
		iTierManager->RemoveMetaConnectionProvider(this);
		}

	iAccessPointConfig.Close();
	}

EXPORT_C const TProviderInfo& CMetaConnectionProviderBase::ProviderInfo() const
/** Get the connection information

@return Const reference to a descriptor with the connection information */
	{
	return iProviderInfo;
	}

EXPORT_C void CMetaConnectionProviderBase::SetProviderInfo(const TProviderInfo& aProviderInfo)
    {
    iProviderInfo = aProviderInfo;
    }

//This fn could be optimised
EXPORT_C void CMetaConnectionProviderBase::ConstructL()
    {
    CMMCommsProviderBase::ConstructL();
    
    RMetaExtensionContainer mec;
    mec.Open();
    CleanupClosePushL(mec);
    
    TProviderInfoExt* providerInfo = new TProviderInfoExt(iProviderInfo);
    CleanupStack::PushL(providerInfo);
    mec.AppendExtensionL(providerInfo);
    CleanupStack::Pop(providerInfo);
    
	iAccessPointConfig.Close();
	iAccessPointConfig.Open(mec);
    CleanupStack::PopAndDestroy(&mec);
    
    SetAccessPointConfigL();
    }

EXPORT_C RNodeInterface* CMetaConnectionProviderBase::NewClientInterfaceL(const TClientType& aClientType, TAny* aClientInfo)
	{
	if (aClientType.Type() & TCFClientType::EServProvider)
		{
		//For all service providers, use the RMetaServiceProviderInterface (==RNodeAvailabilityProviderInterface)
		__ASSERT_DEBUG(aClientInfo, User::Panic(KSpecAssert_ESockSSocksmtcnp, 1));
		const TProviderInfo* pi = static_cast<const TProviderInfo*>(aClientInfo);
		return new (ELeave) RMetaServiceProviderInterface(*pi);
		}
	else if (aClientType.Flags() & TCFClientType::EAvailabilityProvider)
		{
		//For all other types that use the EAvailabilityProvider flag use RNodeAvailabilityProviderInterface
		return new (ELeave) RNodeAvailabilityProviderInterface();
		}
	else if (aClientType.Type() & TCFClientType::EData) //Assume that data client can't be availability provider
		{
		TUint priority = KMaxTUint;
		return new (ELeave) RCFNodePriorityInterface(priority);
		}
	else
		{
		return MeshMachine::AMMNodeBase::NewClientInterfaceL(aClientType, aClientInfo);
		}
	}

EXPORT_C RMetaServiceProviderInterface* CMetaConnectionProviderBase::FindServiceProvider(TUint aAccessPoint)
	{
	__ASSERT_DEBUG(aAccessPoint, User::Panic(KSpecAssert_ESockSSocksmtcnp, 2)); //???

	TClientIter<TDefaultClientMatchPolicy> iter = GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
	RMetaServiceProviderInterface* serviceProvider;
	while ((serviceProvider = static_cast<RMetaServiceProviderInterface*>(iter++)) != NULL)
		{
		TUint ap = serviceProvider->ProviderInfo().APId();
		if (ap == aAccessPoint)
			{
			return serviceProvider;
			}
		}

	return NULL;
	}

EXPORT_C void CMetaConnectionProviderBase::SetTierManagerL(CTierManagerBase* aTierManager)
	{
	//ASSERT(iTierManager==NULL); //This function should be called only once.
	if (iTierManager!=aTierManager)
		{
		aTierManager->AddMetaConnectionProviderL(this);
		// Save aTierMaanger pointer only after AddMetaConnectionProviderL.
		// If it was not possible to add this MCPR to TM due to OOM, then we do not need TM pointer also.
		iTierManager = aTierManager;
		}
	}

void CMetaConnectionProviderBase::SetAccessPointConfigL()
    {
    LOG( ESockLog::Printf(KESockMetaConnectionTag, _L("CMetaConnectionProviderBase %08x:\tSetAccessPointConfigL() - loading access point record"), this) );

    CMDBSession* dbs = CMDBSession::NewLC(KCDVersion1_2);
    CCDAccessPointRecord* apRec = TierManagerUtils::LoadAccessPointRecordL(iProviderInfo.APId(1/*dummy for the overload*/),*dbs);
    
    CleanupStack::PushL(apRec);

    LOG( ESockLog::Printf(KESockMetaConnectionTag, _L("\t\t\tTier=%d Mcpr=%d SelPol=%d Cpr=%d CprCfg=%d Scpr=%d Proto=%d AppSID=%d Priority=%d"), 
						  static_cast<TUint32>(apRec->iTier),
						  static_cast<TUint32>(apRec->iMCpr),
						  static_cast<TUint32>(apRec->iSelectionPolicy),
						  static_cast<TUint32>(apRec->iCpr),
						  static_cast<TUint32>(apRec->iCprConfig),
						  static_cast<TUint32>(apRec->iSCpr),
						  static_cast<TUint32>(apRec->iProtocol),
						  static_cast<TUint32>(apRec->iAppSID),
						  static_cast<TUint32>(apRec->iPriority) ) );
    CCDTierRecord* tierRec = TierManagerUtils::LoadTierRecordL(apRec->iTier,*dbs);
    TInt tempTierId = tierRec->iRecordTag;
    delete tierRec;
    tierRec = NULL;
    
	TUid tierId = TUid::Uid(tempTierId);
	
	User::LeaveIfError((tierId==iProviderInfo.TierId())? KErrNone : KErrArgument);

	TUint selectionPolicy = apRec->iSelectionPolicy;
	TUint cprConfig = apRec->iCprConfig;
	TUint apPriority = (apRec->iPriority.IsNull()) ? KMaxTUint : (TUint)apRec->iPriority;
	if(apPriority == 0)
		{
	    LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("Error: Invalid priority value (%d) stored in access point record."),apPriority));
	    User::Leave(KErrArgument);
		}
	
    CCDMCprRecord* mcprRec = TierManagerUtils::LoadMCprRecordL(apRec->iMCpr,*dbs);
    TUint mCprUid = mcprRec->iMCprUid;
    delete mcprRec;
    mcprRec = NULL;

    CCDCprRecord* cprRec = TierManagerUtils::LoadCprRecordL(apRec->iCpr,*dbs);
    TUint cprUid = cprRec->iCprUid;
    delete cprRec;
    cprRec = NULL;

    CCDSCprRecord* scprRec = TierManagerUtils::LoadSCprRecordL(apRec->iSCpr,*dbs);
    TUint sCprUid = scprRec->iSCprUid;
    delete scprRec;
    scprRec = NULL;

    CCDProtocolRecord* prtRec = TierManagerUtils::LoadProtocolRecordL(apRec->iProtocol,*dbs);
    TUint protocolUid = prtRec->iProtocolUid;
    delete prtRec;
    prtRec = NULL;
    
    TUint customSelectionPolicy = TierManagerUtils::ReadCustomSelectionPolicyIdL(iProviderInfo.APId(),*dbs);
    TUint appSid = apRec->iAppSID;
    
    
    RMetaExtensionContainer mec;
    mec.Open(iAccessPointConfig);
    CleanupClosePushL(mec);
    
    const TDesC& configAPIdList = apRec->iConfigAPIdList;
    if (configAPIdList.Length() > 0)
		{
		RArray<TUint> tlids;
		CleanupClosePushL(tlids);

		CConfigAccessPointConfig *capext = CConfigAccessPointConfig::NewL();

		CleanupStack::PushL(capext);
		mec.AppendExtensionL(capext);
		CleanupStack::Pop(capext);

		TierManagerUtils::ParseTLConfigAccessPointIdsL(tlids, apRec->iConfigAPIdList);

		for (TUint i = 0; i < tlids.Count(); i++)
			{
			/**
			   Load the top level CAP record from the database,
			   and save it to the provisioning structure
			*/
			CCDConfigAccessPointRecord* caprec = TierManagerUtils::LoadConfigAccessPointRecordL(tlids[i], *dbs);
			CleanupStack::PushL(caprec);

			CCDSCprRecord* scprRec = TierManagerUtils::LoadSCprRecordL(caprec->iSCpr,*dbs);
			TUint capSCprUid = scprRec->iSCprUid;
			delete scprRec;
			scprRec = NULL;

			CCDProtocolRecord* prtRec = TierManagerUtils::LoadProtocolRecordL(caprec->iProtocol,*dbs);
			TUint capProtocolUid = prtRec->iProtocolUid;
			TUint capProtocolConfigLoaderUid = prtRec->iProtocolConfigLoaderUid;
			delete prtRec;
			prtRec = NULL;

			/**
			   Record SCpr to be started. Needed by Cpr
			*/
			TUid scprUid = TUid::Uid(capSCprUid);
			capext->AppendSCprL(scprUid);

			TConfigAccessPointLayer topLayer(caprec->iRecordTag, TUid::Uid(capSCprUid),
											 TUid::Uid(capProtocolUid), caprec->iLayerBelow, ETrue);
			if(!capext->LayerExists(topLayer))
				{
				capext->AppendLayerL(topLayer);

				/**
				   Load configuration for each protocol using ecom plugin.
				*/
				if (capProtocolConfigLoaderUid)
					{
					CProtocolConfigLoader *cfgldr = CProtocolConfigLoader::NewL(dbs, TUid::Uid(capProtocolConfigLoaderUid));
					CleanupStack::PushL(cfgldr);
					cfgldr->LoadConfigL(mec, TUid::Uid(capProtocolUid), caprec->iProtocolConfig);
					CleanupStack::PopAndDestroy(cfgldr);
					}
				}

			TUint layerbelow = caprec->iLayerBelow;
			CleanupStack::PopAndDestroy(caprec);
			caprec = NULL;

			/**
			   Now load the info for each of it's lower layers.
			*/
			while (layerbelow != 0)
				{
				/* Load next layer from the database */
				CCDConfigAccessPointRecord* caprec = TierManagerUtils::LoadConfigAccessPointRecordL(layerbelow, *dbs);
				CleanupStack::PushL(caprec);

				CCDProtocolRecord* prtRec = TierManagerUtils::LoadProtocolRecordL(caprec->iProtocol,*dbs);
				capProtocolUid = prtRec->iProtocolUid;
				capProtocolConfigLoaderUid = prtRec->iProtocolConfigLoaderUid;
				delete prtRec;
				prtRec = NULL;

				TConfigAccessPointLayer layer(caprec->iRecordTag, TUid::Uid(capSCprUid),
											  TUid::Uid(capProtocolUid), caprec->iLayerBelow);

                if(!capext->LayerExists(layer))
                    {
					capext->AppendLayerL(layer);

					if (capProtocolConfigLoaderUid)
						{
						CProtocolConfigLoader *cfgldr
							= CProtocolConfigLoader::NewL(dbs, TUid::Uid(capProtocolConfigLoaderUid));
						CleanupStack::PushL(cfgldr);
						TUid capuid = TUid::Uid(capProtocolUid);
						cfgldr->LoadConfigL(mec, capuid, caprec->iProtocolConfig);

						CleanupStack::PopAndDestroy(cfgldr);
						}
					}

				layerbelow = caprec->iLayerBelow;
				CleanupStack::PopAndDestroy(caprec);
				}
			}

		CleanupStack::PopAndDestroy(&tlids);
		}


	TLayerConfig* layerConfig = new(ELeave)TLayerConfig(TUid::Uid(mCprUid), TUid::Uid(cprUid),
		TUid::Uid(sCprUid), TUid::Uid(protocolUid), tierId);
	CleanupStack::PushL(layerConfig);
	mec.AppendExtensionL(layerConfig);
	CleanupStack::Pop(layerConfig);
    
	TLayerSelectionInfo* layerSelectInfo = new(ELeave)TLayerSelectionInfo(selectionPolicy, cprConfig, customSelectionPolicy);
	CleanupStack::PushL(layerSelectInfo);
	mec.AppendExtensionL(layerSelectInfo);
	CleanupStack::Pop(layerSelectInfo);

	TAppSidConfig* appSidConfig = new(ELeave)TAppSidConfig(appSid);
	CleanupStack::PushL(appSidConfig);
	mec.AppendExtensionL(appSidConfig);
	CleanupStack::Pop(appSidConfig);
	
	TAccessPointPriority* apPriorityExt = new(ELeave)TAccessPointPriority;
	CleanupStack::PushL(apPriorityExt);
	apPriorityExt->SetPriority(apPriority);
	mec.AppendExtensionL(apPriorityExt);
	CleanupStack::Pop(apPriorityExt);
	
	iAccessPointConfig.Close();
	iAccessPointConfig.Open(mec);
	CleanupStack::PopAndDestroy(&mec);


#ifdef SYMBIAN_NETWORKING_UPS
	ShowAccessPointRecordL(dbs, apRec);
#endif

	CleanupStack::PopAndDestroy(apRec);
    CleanupStack::PopAndDestroy(dbs);

	LOG( ESockLog::Printf(KESockMetaConnectionTag, _L("\t\t\tTierId=%08x McprUid=%08x CprUid=%08x ScprUid=%08x ProtoUid=%08x"), 
						  tierId.iUid,
						  layerConfig->MCprUid(),
						  layerConfig->CprUid(),
						  layerConfig->SCprUid(),
						  layerConfig->ProtocolUid()
			 ) );
    }


#ifdef SYMBIAN_NETWORKING_UPS

EXPORT_C void CMetaConnectionProviderBase::ShowAccessPointRecordL(CommsDat::CMDBSession* /*aSession*/, CommsDat::CCDAccessPointRecord* /*aApRec*/)
/**
Allow derived classes to read fields from the Access Point Record whilst we already have it open.
*/
	{
	}

#endif


//
//Factories - Container
CMetaConnectionFactoryContainer* CMetaConnectionFactoryContainer::NewL()
/** Create a new instance of a meta connection factory container
@exception Leaves in out of memory conditions
@return Pointer to new instance of a meta connection factory container
*/
	{
	CMetaConnectionFactoryContainer* container = new (ELeave) CMetaConnectionFactoryContainer;
	return container;
	}


CMetaConnectionProviderFactoryBase* CMetaConnectionFactoryContainer::Factory(TInt aIndex) const
	{
	return static_cast<CMetaConnectionProviderFactoryBase*>(CCommsFactoryContainer::Factory(aIndex));
	}

CMetaConnectionFactoryContainer::~CMetaConnectionFactoryContainer()
/** Empty meta connection factory container destructor */
	{
	LOG_NODE_DESTROY(KESockMetaConnectionTag, CMetaConnectionFactoryContainer);
	}

CMetaConnectionFactoryContainer::CMetaConnectionFactoryContainer()
:CCommsFactoryContainer((CCommsFactoryContainer::TContaineeType)CMetaConnectionFactoryContainer::EId)
/** Empty meta connection factory container constructor */
	{
	LOG_NODE_CREATE(KESockMetaConnectionTag, CMetaConnectionFactoryContainer);
	}

void CMetaConnectionFactoryContainer::ReceivedL(const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage)
	{
	(void)aMessage;

    NM_LOG_START_BLOCK(KESockMetaConnectionTag, _L8("CMetaConnectionFactoryContainer:ReceivedL"));
    NM_LOG((KESockMetaConnectionTag, _L8("ERROR: KErrNotSupported [this=0x%08x]"), this));
    NM_LOG_MESSAGE(KESockMetaConnectionTag, aMessage);
	NM_LOG_END_BLOCK(KESockMetaConnectionTag, _L8("CMetaConnectionFactoryContainer:ReceivedL"));

	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksmtcnp, 3)); //For debug configurations
	User::Leave(KErrNotSupported); //For release configurations
	}

//
//Factories
EXPORT_C CMetaConnectionProviderFactoryBase::CMetaConnectionProviderFactoryBase(TUid aFactoryId, CMetaConnectionFactoryContainer& aParentContainer)
/** Meta connection provider factory constructor
@param aFactoryId Unique Integer Identifier of the meta connection provider factory
@param aParentContainer Container to add the factory to */
	:	CCommsFactoryBase(aFactoryId, aParentContainer)
	{
	LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("CMetaConnectionProviderFactoryBase %08x:\tCMetaConnectionProviderFactoryBase(%08x)"), this, aFactoryId));
	}

EXPORT_C CMetaConnectionProviderFactoryBase::~CMetaConnectionProviderFactoryBase()
/** Empty meta connection factory base destructor */
	{
	LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("CMetaConnectionProviderFactoryBase %08x:\t~CMetaConnectionProviderFactoryBase()"), this));
	}

EXPORT_C ACommsFactoryNodeId* CMetaConnectionProviderFactoryBase::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksmtcnp, 4));
	User::Leave(KErrNotSupported);
	return NULL;
	}

EXPORT_C void CMetaConnectionProviderFactoryBase::DoPostCreationL(ACommsFactoryNodeId* aObject,TFactoryQueryBase& aQuery)
	{ //setting tiermanger to MCPR
	CMetaConnectionProviderBase* provider = static_cast<CMetaConnectionProviderBase*>(aObject);
	TMetaConnectionFactoryQuery tempQuery= static_cast<TMetaConnectionFactoryQuery&>(aQuery);
	CTierManagerFactoryBase* factory = static_cast<CTierManagerFactoryBase*>((*SockManGlobals::Get()->iTierManagerFactories).FindOrCreateFactoryL(tempQuery.iTierImplUid));
	
	if (factory)
		{
		TAlwaysFindFactoryQuery query;
		CTierManagerBase* tierManager = static_cast<CTierManagerBase*>(factory->Find(query));
		if (tierManager)
			provider->SetTierManagerL(tierManager);
		}

	}


