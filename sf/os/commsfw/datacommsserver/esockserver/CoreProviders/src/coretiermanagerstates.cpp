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
//

#include "coretiermanagerstates.h"

#include <comms-infras/coremcprstates.h>
#include <comms-infras/coretiermanageractivities.h>
#include <comms-infras/coretiermanager.h>
#include "coretiermanagerselector.h"
#include <comms-infras/ss_nodemessages_selector.h>
#include <comms-infras/ss_nodemessages_availability.h>
#include <comms-infras/ss_metaconnprov_internal.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include <commsdattypesv1_1.h> // CommsDat
#include <metadatabase.h> // CommsDat
#include <ss_glob.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrPrvTMStaC, "ESockCrPrvTMStaC");
#endif

#if defined(__CFLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)
	#define KCoreMCprStatesTag KESockMetaConnectionTag
#endif

using namespace TMStates;
using namespace NetStateMachine;
using namespace ESock;
using namespace CommsDat;
using namespace TMActivities;
using namespace Messages;


//-=========================================================
// Panics
#ifdef _DEBUG
_LIT (KCoreTMPanic,"CoreTMPanic");
static const TInt KPanicNoActivity = 1;
#endif

//
//Select Provider
EXPORT_DEFINE_SMELEMENT(TAwaitingSelectProvider, NetStateMachine::MState, TMStates::TContext)
EXPORT_C TBool TAwaitingSelectProvider::Accept()
	{
	if (! iContext.iMessage.IsMessage<TCFSelector::TSimpleSelect>())
		{
		return EFalse;
		}

	__ASSERT_DEBUG(!iContext.iSender.IsNull(), User::Panic(KSpecAssert_ESockCrPrvTMStaC, 1)); //Client's id must not be NULL.
	return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingSelectProviderSuper, NetStateMachine::MState, TMStates::TContext)
EXPORT_C TBool TAwaitingSelectProviderSuper::Accept()
	{
	if (! iContext.iMessage.IsMessage<TCFSelector::TSelect>())
		{
		return EFalse;
		}

	// Client's id must not be NULL
	__ASSERT_DEBUG(!iContext.iSender.IsNull(), User::Panic(KSpecAssert_ESockCrPrvTMStaC, 2));
	return ETrue;
	}

EXPORT_DEFINE_SMELEMENT(TSelectProvider, NetStateMachine::MStateTransition, TMStates::TContext)
EXPORT_C void TSelectProvider::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreTMPanic, KPanicNoActivity));
	CSelectProviderActivity* act = static_cast<CSelectProviderActivity*>(iContext.iNodeActivity);

	//Create selector
	act->iSelector = iContext.Node().DoCreateProviderSelectorL(message_cast<TCFSelector::TSimpleSelect>(iContext.iMessage).iSelectionPrefs);

	ISelectionNotify selectionNotify(act,act->InterfaceVTable());
	act->iSelector->SelectL(selectionNotify);
	//Do not do any further processing here as selectors may be returning synchronously (via ISelectionNotify).
	}

EXPORT_DEFINE_SMELEMENT(TSelectProviderSuper, NetStateMachine::MStateTransition, TMStates::TContext)
EXPORT_C void TSelectProviderSuper::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreTMPanic, KPanicNoActivity));
	CSelectProviderActivity* act = static_cast<CSelectProviderActivity*>(iContext.iNodeActivity);

	//Creates the core selector - this selector is not required, it is always the
	//same selector created so the code could be moving into this transition.
	//The method FindOrCreateProviderL would need to be recreated somewhere else
	//for this to be possible (currently inherited off ASimpleSelectorBase.
	act->iSelector = new (ELeave) CCoreProviderSelectorSuper(
		iContext.Node().TierId(),
		message_cast<TCFSelector::TSelect>(iContext.iMessage).iConnPrefList
		);

	ISelectionNotify selectionNotify(act,act->InterfaceVTable());
	act->iSelector->SelectL(selectionNotify);
	//Do not do any further processing here as selectors may be returning synchronously (via ISelectionNotify).
	}


//
// Availability Notification - states
EXPORT_DEFINE_SMELEMENT(TAwaitingRegistrationForTierAvailabilityNotification, NetStateMachine::MState, TMStates::TContext)
EXPORT_C TBool TAwaitingRegistrationForTierAvailabilityNotification::Accept()
	{
	TBool accept = (iContext.iMessage.IsMessage<TCFAvailabilityProvider::TAvailabilityNotificationRegistration>());

	if(accept)
		{
		TClientIter<TFlagsOnlyClientMatchPolicy> iter = iContext.Node().GetClientIter<TFlagsOnlyClientMatchPolicy>(TClientType(0, TCFClientType::EAvailabilityProvider));
		RNodeAvailabilityProviderInterface* provider = static_cast<RNodeAvailabilityProviderInterface*>(iter++);
		while (provider)
			{
			if (provider->AvailabilityStatus().IsKnown())
				{
				ESock::TCFAvailabilityControlClient::TAvailabilityNotification msg(provider->AvailabilityStatus());
				iContext.PostToSender(msg);
				}
			provider = static_cast<RNodeAvailabilityProviderInterface*>(iter++);
			}
		}

	return accept;
	}

EXPORT_DEFINE_SMELEMENT(TJoinToMCPRs, NetStateMachine::MStateTransition, TMStates::TContext)
EXPORT_C void TJoinToMCPRs::DoL()
	{
//	__CFLOG_VAR((KCoreMCprTag, KCoreMCprStatesSubTag, _L8("ARegisterForAvailabilityNotifications::DoL()")));

//  1. AP table: read all records with your Tier Id
//		EXPORT_C CMDBRecordSet<CCDAccessPointRecord>* TierManagerUtils::LoadAccessPointRecordsL(TUint aTierId, CMDBSession& aDbs)
//  2. instantiate 1 MCPR per AP record
//  3. join to each one

	CMDBSession* dbs = CMDBSession::NewLC(KCDVersion1_2);
	CMDBRecordSet<CCDAccessPointRecord>* apRecs = TierManagerUtils::LoadAccessPointRecordsL(iContext.Node().TierId(), *dbs);
	CleanupStack::PushL(apRecs);


	TInt i = 0;
	for( ; i < apRecs->iRecords.Count() ; ++i )
		{
		CCDAccessPointRecord* apRec = apRecs->operator[](i);
		//Find MCPR factory
		TUid mCprUid = TierManagerUtils::ReadMCprUidL(apRec->iMCpr,*dbs);
		CMetaConnectionProviderFactoryBase* factory = static_cast<CMetaConnectionProviderFactoryBase*>((*SockManGlobals::Get()->iMetaConnectionFactories).FindOrCreateFactoryL(mCprUid));

		//Create the provider
		TProviderInfo providerInfo(TUid::Uid(apRec->iTier),apRec->iMCpr);
		TUid tierImplUid = TierManagerUtils::MapTierIdtoTierImplIdL(TUid::Uid(apRec->iTier),*dbs);
		TMetaConnectionFactoryQuery query (providerInfo,tierImplUid);
		CMetaConnectionProviderBase* mcpr = static_cast<CMetaConnectionProviderBase*>(factory->FindOrCreateObjectL(query));
		__ASSERT_DEBUG(mcpr, User::Panic(KSpecAssert_ESockCrPrvTMStaC, 3));

		//Send request
		TNodeId mcprId = mcpr->Id();

		mcpr->AddClientL(iContext.NodeId(), TClientType(TCFClientType::ECtrlProvider));
		iContext.Node().AddClientL(mcprId, TClientType(TCFClientType::EData));
		}

	CleanupStack::PopAndDestroy(apRecs);
	CleanupStack::PopAndDestroy(dbs);
	}

EXPORT_DEFINE_SMELEMENT(TRegisterForAvailability, NetStateMachine::MStateTransition, TMStates::TContext)
EXPORT_C void TRegisterForAvailability::DoL()
	{
	TCFAvailabilityProvider::TAvailabilityNotificationRegistration& inMsg =
			message_cast<TCFAvailabilityProvider::TAvailabilityNotificationRegistration>(iContext.iMessage);

	iContext.Node().PostToClients<TDefaultClientMatchPolicy>(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), inMsg, TClientType(TCFClientType::EData));

	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData));

	RNodeInterface* ctl;
	TBool found = EFalse;

	while ((ctl = iter++) != NULL)
		{
		found = ETrue;
		ctl->SetFlags(TCFClientType::EAvailabilityProvider);
		}

	if(!found) // if no providers, error the activity
		{
		User::Leave(KErrNotFound);
		}
	}

EXPORT_DEFINE_SMELEMENT(TPassAvailabilityNotificationToOriginators, NetStateMachine::MStateTransition, TMStates::TContext)
EXPORT_C void TPassAvailabilityNotificationToOriginators::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KSpecAssert_ESockCrPrvTMStaC, 4));

	__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockCrPrvTMStaC, 5)); //AV_TODO - is this used? What is this exported code doing here???

	TCFAvailabilityControlClient::TAvailabilityNotification& inMsg =
		message_cast<TCFAvailabilityControlClient::TAvailabilityNotification>(iContext.iMessage);

	//const TNodeId& incomingId = inMsg.iSender;
	//TUint incomingAPId = inMsg.iValue1;
	//TAvailabilityStatus incomingAv = inMsg.iValue2;

	// just store and forward to all originators
//	CAvailabilityNodeActivity* act = static_cast<CAvailabilityNodeActivity*>(iContext.iNodeActivity);
//	act->StoreAvailability( incomingId, incomingAPId , incomingAv );

	__ASSERT_DEBUG(iContext.iPeer, User::Panic(KSpecAssert_ESockCrPrvTMStaC, 6));
	RNodeAvailabilityProviderInterface* provider = static_cast<RNodeAvailabilityProviderInterface*>(iContext.iPeer);
	provider->SetAvailabilityStatus(inMsg.iAvailabilityStatus);

	ESock::TCFAvailabilityControlClient::TAvailabilityNotification msg(inMsg.iAvailabilityStatus);
	iContext.iNodeActivity->PostToOriginators(msg);
	}



