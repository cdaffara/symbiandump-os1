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
// CORETIERMANAGER.CPP
// 
//

#include "simpleselectorbase.h"

#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <commsdattypesv1_1.h> // CommsDat
#include <comms-infras/ss_tiermanagerutils.h>
#include <in_sock.h> //KAfInet

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1_partner.h>
#include <es_sock_internal.h>
#endif

#include <comms-infras/ss_metaconnprov_internal.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSimpleSelsm, "ESockSimpleSelsm");
#endif

using namespace ESock;

//
//ASimpleSelectorBase
EXPORT_C ASimpleSelectorBase::ASimpleSelectorBase()
:	iDbs(NULL),
	iMetaContainer((*SockManGlobals::Get()->iMetaConnectionFactories))
	{}

EXPORT_C ASimpleSelectorBase::ASimpleSelectorBase(const Meta::SMetaData& aSelectionPreferences)
	: iDbs(NULL),
	  iMetaContainer((*SockManGlobals::Get()->iMetaConnectionFactories)),
	  iTypeId(0)
	{
	iSelectionPrefs.Copy(aSelectionPreferences);
	}

EXPORT_C ASimpleSelectorBase::ASimpleSelectorBase(const TUint aTypeId, const Meta::SMetaData& aSelectionPreferences)
	: iDbs(NULL),
	  iMetaContainer((*SockManGlobals::Get()->iMetaConnectionFactories)),
	  iTypeId(aTypeId)
	{
	iSelectionPrefs.Copy(aSelectionPreferences);
	}

EXPORT_C ASimpleSelectorBase::~ASimpleSelectorBase()
	{
	Cancel();
	delete iTierRecord;
	delete iDbs;
	}

EXPORT_C void ASimpleSelectorBase::Cancel()
	{
	}

EXPORT_C CMetaConnectionProviderBase* ASimpleSelectorBase::FindOrCreateProviderL(TUint aAccessPoint, TAny* aInstance)
	{
    /**
	At this point the 'aAccessPoint' parameter is ALWAYS a tag field!!
	*/
	__ASSERT_DEBUG(aAccessPoint, User::Panic(KSpecAssert_ESockSimpleSelsm, 1)); //Must not be 0
    
	
	TUid mCprUid = TierManagerUtils::ReadMCprUidL(aAccessPoint,*iDbs);
	CMetaConnectionProviderFactoryBase* factory = static_cast<CMetaConnectionProviderFactoryBase*>(iMetaContainer.FindOrCreateFactoryL(mCprUid));

	//Create the provider
	TUid tierId = TUid::Uid(iTierRecord->iRecordTag);
	TUid tierImplUid = TUid::Uid(iTierRecord->iTierImplUid);
	TMetaConnectionFactoryQuery query (TProviderInfo(tierId,aAccessPoint,aInstance),tierImplUid);
	CMetaConnectionProviderBase* provider = static_cast<CMetaConnectionProviderBase*>(factory->FindOrCreateObjectL(query));
	__ASSERT_DEBUG(provider, User::Panic(KSpecAssert_ESockSimpleSelsm, 2));
	provider->IncrementBlockingDestroy();
	return provider;
	}

EXPORT_C CMetaConnectionProviderBase* ASimpleSelectorBase::FindProviderL(TUint aAccessPoint, TAny* aInstance)
	{
	/**
	At this point the 'aAccessPoint' parameter is ALWAYS a tag field!!
	*/
	__ASSERT_DEBUG(aAccessPoint, User::Panic(KSpecAssert_ESockSimpleSelsm, 3)); //Must not be 0

	//Find factory
	TUid mCprUid = TierManagerUtils::ReadMCprUidL(aAccessPoint,*iDbs);
	CMetaConnectionProviderFactoryBase* factory = static_cast<CMetaConnectionProviderFactoryBase*>(iMetaContainer.FindFactory(mCprUid));
	User::LeaveIfError(factory? KErrNone : KErrNotFound);

	//Create the provider
	TUid tierId = TUid::Uid(iTierRecord->iRecordTag);
	TUid tierImplUid = TUid::Uid(iTierRecord->iTierImplUid);
	TMetaConnectionFactoryQuery query (TProviderInfo(tierId,aAccessPoint,aInstance),tierImplUid);
	return static_cast<CMetaConnectionProviderBase*>(factory->Find(query));
	}

//
//CSimplePromptingSelectorBase
EXPORT_C CSimplePromptingSelectorBase::CSimplePromptingSelectorBase()
: CActive(CActive::EPriorityUserInput)
, ASimpleSelectorBase()
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C CSimplePromptingSelectorBase::CSimplePromptingSelectorBase(const TUint aTypeId, const Meta::SMetaData& aSelectionPreferences)
:	CActive(CActive::EPriorityUserInput),
	ASimpleSelectorBase(aTypeId, aSelectionPreferences)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C CSimplePromptingSelectorBase::CSimplePromptingSelectorBase(const Meta::SMetaData& aSelectionPreferences)
:	CActive(CActive::EPriorityUserInput),
	ASimpleSelectorBase(aSelectionPreferences)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C CSimplePromptingSelectorBase::~CSimplePromptingSelectorBase()
	{
	static_cast<CActive*>(this)->Cancel();
	iDlgServ.Close();
	}

EXPORT_C void CSimplePromptingSelectorBase::DoCancel()
	{
	iDlgServ.CancelIapConnection();
	static_cast<MProviderSelector*>(this)->Cancel(); //Invoke the virtual Cancel not the CActive's one.
	}

EXPORT_C void CSimplePromptingSelectorBase::PromptUserL(ISelectionNotify& aSelectionNotify)
	{
	User::LeaveIfError(iDlgServ.Connect());
	iDlgServ.AccessPointConnection(iSelectedAPId,KAfInet,iStatus);
	SetActive();
	iSelectionNotify = aSelectionNotify;
	}

//prompting for an IAP/Link level AP
EXPORT_C void CSimplePromptingSelectorBase::PromptUserL(ESock::ISelectionNotify& aSelectionNotify,
						  							    const TConnectionPrefs& aPrefs)
	{
	User::LeaveIfError(iDlgServ.Connect());
	iDlgServ.IapConnection(iSelectedAPId, aPrefs, iStatus);
	SetActive();
	iSelectionNotify = aSelectionNotify;
	}


