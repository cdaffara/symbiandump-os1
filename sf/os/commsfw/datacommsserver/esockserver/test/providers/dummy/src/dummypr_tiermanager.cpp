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
// TestDummy implementation file for a TierManager
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/coretiermanagerstates.h>
#include <comms-infras/coretiermanageractivities.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include <commsdattypesv1_1.h>
#include <commsdattypesv1_1_partner.h>
#include <ss_glob.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include <comms-infras/ss_metaconnprov_internal.h>
#include <es_connpref.h>
#include "dummypr_tiermanager.h"
#include "dummypr_factory.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestdmyprtr, "ESockTestdmyprtr");
#endif

#ifdef __CFLOG_ACTIVE
	#define KDummyTierMgrTag KESockMetaConnectionTag
	_LIT8(KDummyTierMgrSubTag, "dummyTierManager");
#endif

using namespace ESock;
using namespace NetStateMachine;
using namespace CommsDat;
using namespace Messages;
using namespace MeshMachine;

CDummyTierManager* CDummyTierManager::NewL(ESock::CTierManagerFactoryBase& aFactory)
	{
 	return new (ELeave) CDummyTierManager(aFactory, TMActivities::coreTMActivities::Self());
	}

CDummyTierManager::CDummyTierManager(ESock::CTierManagerFactoryBase& aFactory,
                                 const MeshMachine::TNodeActivityMap& aActivityMap)
:	CCoreTierManager(aFactory,aActivityMap),
	TIfStaticFetcherNearestInHierarchy(this)
	{
	LOG_NODE_CREATE(KDummyTierMgrTag, CDummyTierManager);
	}

CDummyTierManager::~CDummyTierManager()
	{
	LOG_NODE_DESTROY(KDummyTierMgrTag, CDummyTierManager);
	DeleteResponder();
	}

void CDummyTierManager::ScheduleResponder(ESock::CDataMonitoringResponder*& aResponder)
	{
	if (iAsyncResponder)
		{
		CDataMonitoringResponder::Error(aResponder, KErrInUse);
		return;
		}
	TRAPD(err, iAsyncResponder = CAsyncResponder::NewL(this, aResponder));
	if (KErrNone != err)
		{
		CDataMonitoringResponder::Error(aResponder, err);
		return;
		}
	iAsyncResponder->After(2e6);
	}


void CDummyTierManager::CancelResponder()
	{
	if (iAsyncResponder)
		{
		iAsyncResponder->Cancel();
		delete iAsyncResponder;
		iAsyncResponder = NULL;
		}
	}

void CDummyTierManager::DeleteResponder()
	{
	if (iAsyncResponder)
		{
		delete iAsyncResponder;
		iAsyncResponder = NULL;
		}
	}


void CDummyTierManager::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
	{
   	TNodeContext<CDummyTierManager> ctx(*this, aMessage, aSender, aRecipient);
   	CCoreTierManager::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

void CDummyTierManager::ReturnInterfacePtrL(ESock::MLegacyControlApiExt*& aInterface)
    {
    aInterface = this;
    }

TInt CDummyTierManager::ControlL(TUint /*aOptionLevel*/, TUint /*aOptionName*/, TDes8& /*aOption*/, ESock::MPlatsecApiExt* /*aPlatsecIfc*/)
    {
    return KErrNone;
    }

void CDummyTierManager::ReturnInterfacePtrL(ADataMonitoringProtocolReq*& aInterface)
	{
	aInterface = this;
	}

void CDummyTierManager::RequestDataTransferred(ESock::CDataMonitoringResponder*& aResponder, TSubSessionUniqueId /*aClientId*/)
	{
	ScheduleResponder(aResponder);
	}

void CDummyTierManager::CancelDataTransferredRequest(TSubSessionUniqueId /*aClientId*/)
	{
	CancelResponder();
	DeleteResponder();
	}

void CDummyTierManager::RequestDataReceivedNotification(ESock::CDataMonitoringResponder*& aResponder, TUint32 /*aDelta*/, TUint32 /*aReceivedBytes*/, TSubSessionUniqueId /*aClientId*/)
	{
	ScheduleResponder(aResponder);
	}

void CDummyTierManager::CancelDataReceivedNotificationRequest(TSubSessionUniqueId /*aClientId*/)
	{
	CancelResponder();
	DeleteResponder();
	}

void CDummyTierManager::RequestDataSentNotification(ESock::CDataMonitoringResponder*& aResponder, TUint32 /*aDelta*/, TUint32 /*aSentBytes*/, TSubSessionUniqueId /*aClientId*/)
	{
	ScheduleResponder(aResponder);
	}

void CDummyTierManager::CancelDataSentNotificationRequest(TSubSessionUniqueId /*aClientId*/)
	{
	CancelResponder();
	DeleteResponder();
	}





CDummyTierManager::CAsyncResponder::CAsyncResponder(CDummyTierManager *aTierManager, CDataMonitoringResponder *aResponder)
: CTimer(EPriorityStandard), iTierManager(aTierManager), iResponder(aResponder)
	{
	}


CDummyTierManager::CAsyncResponder *CDummyTierManager::CAsyncResponder::NewL(
										CDummyTierManager *aTierManager, CDataMonitoringResponder *aResponder)
	{
	CDummyTierManager::CAsyncResponder *asyncResponder = new (ELeave) CDummyTierManager::CAsyncResponder(aTierManager, aResponder);
	CleanupStack::PushL(asyncResponder);
	asyncResponder->ConstructL();
	CleanupStack::Pop(asyncResponder);
	return asyncResponder;
	}

void CDummyTierManager::CAsyncResponder::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CDummyTierManager::CAsyncResponder::RunL()
	{
	CDataMonitoringResponder::Error(iResponder, KErrNone);
	iTierManager->DeleteResponder();
	}

void CDummyTierManager::CAsyncResponder::DoCancel()
	{
	CDataMonitoringResponder::Error(iResponder, KErrCancel);
	CTimer::DoCancel();
	}



class CDummyProviderSelector : public CBase, public ESock::MProviderSelector
	{
public:
	static ESock::MProviderSelector* NewL(const Meta::SMetaData& aSelectionPreferences);

protected:
	explicit CDummyProviderSelector(const Meta::SMetaData& aSelectionPreferences);
	virtual ~CDummyProviderSelector();
	virtual void Cancel();
	ESock::CMetaConnectionProviderBase* FindOrCreateProviderL(TUint aAccessPoint);
	virtual void SelectL(ESock::ISelectionNotify& aSelectionNotify);

protected:
	ESock::TSelectionPrefs iSelectionPreferences;
	CommsDat::CCDTierRecord* iTierRecord;
	CommsDat::CMDBSession* iDbs;
	ESock::CMetaConnectionFactoryContainer& iMetaContainer;
	};

MProviderSelector* CDummyTierManager::DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences)
	{
	return CDummyProviderSelector::NewL(aSelectionPreferences);
	}

MProviderSelector* CDummyProviderSelector::NewL(const Meta::SMetaData& aSelectionPreferences)
	{
	__CFLOG_VAR((KDummyTierMgrTag, KDummyTierMgrSubTag, _L8("CDummyProviderSelector::\tNewL()")));
	__ASSERT_DEBUG(aSelectionPreferences.IsTypeOf(TSelectionPrefs::TypeId()), User::Panic(KSpecAssert_ESockTestdmyprtr, 1));
	CDummyProviderSelector* self = NULL;
	CMDBSession* dbs = CMDBSession::NewLC(KCDVersion1_2);

	__CFLOG_VAR((KDummyTierMgrTag, KDummyTierMgrSubTag, _L8("CDummyProviderSelector::\tNewL() - No Mapping")));
	self = new (ELeave) CDummyProviderSelector(aSelectionPreferences);

	CleanupStack::PushL(self);
	__ASSERT_DEBUG(self->iTierRecord==NULL, User::Panic(KSpecAssert_ESockTestdmyprtr, 2));
	self->iTierRecord = TierManagerUtils::LoadTierRecordL(TUid::Uid(CDummyTierManagerFactory::iUid),*dbs);
	CleanupStack::Pop(self);
	CleanupStack::Pop(dbs);
	__ASSERT_DEBUG(self->iDbs==NULL, User::Panic(KSpecAssert_ESockTestdmyprtr, 3));
	self->iDbs = dbs;
	return self;
	}

CDummyProviderSelector::CDummyProviderSelector(const Meta::SMetaData& aSelectionPreferences)
:	iDbs(NULL),
	iMetaContainer((*SockManGlobals::Get()->iMetaConnectionFactories))
	{
	iSelectionPreferences.Copy(aSelectionPreferences);
	}

CDummyProviderSelector::~CDummyProviderSelector()
	{
	Cancel();
	delete iTierRecord;
	delete iDbs;
	}

void CDummyProviderSelector::Cancel()
	{
	}


CMetaConnectionProviderBase* CDummyProviderSelector::FindOrCreateProviderL(TUint aAccessPoint)
	{
	//Find factory
	TUid mCprUid = TierManagerUtils::ReadMCprUidL(aAccessPoint,*iDbs);
	CMetaConnectionProviderFactoryBase* factory = static_cast<CMetaConnectionProviderFactoryBase*>(iMetaContainer.FindOrCreateFactoryL(mCprUid));

	//Create the provider
	TUid tierId = TUid::Uid(iTierRecord->iRecordTag);
	TUid tierImplUid = TUid::Uid(iTierRecord->iTierImplUid);
	TProviderInfo providerInfo(tierId,aAccessPoint);
	TMetaConnectionFactoryQuery query (providerInfo,tierImplUid);
	CMetaConnectionProviderBase* provider = static_cast<CMetaConnectionProviderBase*>(factory->Find(query));
	if (provider==NULL)
		{
		if (iSelectionPreferences.Scope() & TSelectionPrefs::ESelectFromExisting)
			{
			//We do not care about the legacy attach here (let it continue)
			const TConnPref& prefs = iSelectionPreferences.Prefs();
			//Legacy attach uses EConnPrefCommDb
			if (iSelectionPreferences.IsEmpty() || prefs.ExtensionId() != TConnPref::EConnPrefCommDb)
				{
				User::Leave(KErrNotFound);
				}
			}
	    provider = static_cast<CMetaConnectionProviderBase*>(factory->CreateObjectL(query));
	    }
	provider->IncrementBlockingDestroy();
	return provider;
	}

//
//CTelProtoProviderSelector
void CDummyProviderSelector::SelectL(ISelectionNotify& aSelectionNotify)
	{
	__ASSERT_DEBUG(iDbs, User::Panic(KSpecAssert_ESockTestdmyprtr, 4));
	__ASSERT_DEBUG(iTierRecord, User::Panic(KSpecAssert_ESockTestdmyprtr, 5));
	TUint32 defaultAccessPoint = iTierRecord->iDefaultAccessPoint;

	//This selector deals only with the AccessPoint aware system!
	__ASSERT_DEBUG(defaultAccessPoint!=0, User::Panic(KSpecAssert_ESockTestdmyprtr, 6));

   	const TConnPref& prefs = iSelectionPreferences.Prefs();
	if (prefs.ExtensionId() == TConnPref::EConnPrefSnap)
		{
		TUint accessPoint = static_cast<const TConnSnapPref&>(prefs).Snap();
		__CFLOG_VAR((KDummyTierMgrTag, KDummyTierMgrSubTag, _L8("CDummyProviderSelector %08x::\tSelectL() Using TConnPrefSnap, AP:%d"),this,accessPoint));
		aSelectionNotify.SelectComplete(this,FindOrCreateProviderL(accessPoint));
		}
	else if (prefs.ExtensionId() == TConnPref::EConnPrefIdList)
		{
		const TConnIdList& list = static_cast<const TConnIdList&>(prefs);
		for (TInt i = 0; i < list.Count(); i++)
			{
			TUint accessPoint = list.Get(i);
			__CFLOG_VAR((KDummyTierMgrTag, KDummyTierMgrSubTag, _L8("CDummyProviderSelector %08x::\tSelectL() Using TConnIdList, AP:%d"),this,accessPoint));
			aSelectionNotify.SelectComplete(this,FindOrCreateProviderL(accessPoint));
			}
		}
	else
		{
		//In this selector we _must_ have the new preferences, otherwise it means that
		//a critical, non-recoverable mistake has occured before when this selector has been picked.
		__CFLOG_VAR((KDummyTierMgrTag, KDummyTierMgrSubTag, _L8("ERROR: CDummyProviderSelector %08x::\tSelectL() Unexpected selection preferences"),this));
		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockTestdmyprtr, 7));
		}

	//Complete the selection request
	aSelectionNotify.SelectComplete(this,NULL);
	}



