// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// coretiermanagersuperselector.cpp
// 
//

/**
 @file
 @internalTechnology
*/

#include "coretiermanagerselector.h"
#include <comms-infras/esock_params.h>
#include <connpref.h>
#include <comms-infras/metadata.h>
#include <comms-infras/ss_log.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1_partner.h>
#endif

// remove DETAILED_LOG condition if ever used for more than detailed logs. Condition added to remove build warnings
#if (defined(__CFLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)) && defined(DETAILED_LOG)
	_LIT8(KCoreTMgrSelSupSubTag, "coretiermgrselectorsuper");
#endif

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrPrvTMSelC, "ESockCrPrvTMSelC");
#endif

//
//CCoreSelector
void CCoreProviderSelectorSuper::SelectL(ISelectionNotify& aSelectionNotify)
	{
	//This code doesn't need to be in a seperate selector. It could be moved to
	//a transition if the method FindOrCreateProviderL was re-created in
	//another file (it is currently inherited from ASimpleSelectorBase)

	CMDBSession* dbs = CMDBSession::NewLC(KCDVersion1_2);

	iTierRecord = TierManagerUtils::LoadTierRecordL(iTierId,*dbs);
	CleanupStack::Pop();
	iDbs = dbs;

	__ASSERT_DEBUG(iDbs, User::Panic(KSpecAssert_ESockCrPrvTMSelC, 1));
	__ASSERT_DEBUG(iTierRecord, User::Panic(KSpecAssert_ESockCrPrvTMSelC, 2));

	TUint accessPoint = 0;
	CCDAccessPointRecord* apRec;
	TUid tierId;
	TUint selected = 0;

	//This iterator links to all the elements of TConnAPPref that exist in the list
	ESock::RConnPrefList::TIter<TConnAPPref> iterAP = iConnPrefList.getIter<TConnAPPref>();

	// If there are no TConnAPPref in the prefs, load the default AP
	if (iterAP.IsEnd())
		{
		accessPoint = iTierRecord->iDefaultAccessPoint;
		LOG_DETAILED( ESockLog::Printf(KCoreTMgrSelSupSubTag, _L("CCoreProviderSelectorSuper %08x:\t No AP specified, using default for tier [Tier: %08x, AP: %d]"), this, iTierId, accessPoint ) );
		apRec = TierManagerUtils::LoadAccessPointRecordL(accessPoint, *iDbs);
	    tierId = TierManagerUtils::MapElementIdToTagId(apRec->iTier, *iDbs);
		delete apRec;

		if(iTierId == tierId)
			{
			aSelectionNotify.SelectComplete(this,FindOrCreateProviderL(accessPoint));
			++selected;
			}
		else
			{
			LOG_DETAILED( ESockLog::Printf(KCoreTMgrSelSupSubTag, _L("CCoreProviderSelectorSuper %08x:\t AP %d not in tier %08x"), this, accessPoint, iTierId) );
			User::Leave(KErrArgument);
			}
		}
	
	//This iterator runs through all the instances of TConnAPPref
	while(!iterAP.IsEnd())
		{
		accessPoint = iterAP->GetAP();
		//The access point record that that access point represents is retrived
		apRec = TierManagerUtils::LoadAccessPointRecordL(accessPoint,*iDbs);
		//tierId = TUid::Uid(apRec->iTier);
		tierId = TierManagerUtils::MapElementIdToTagId(apRec->iTier, *iDbs);
		//TUid TierManagerUtils::MapElementIdToTagId(TUint aElementId, CommsDat::CMDBSession& aDbs)
		delete apRec;
		//If the access point is related to this tier manager the provider
		//is created and the TConnAPPref removed
		if(iTierId == tierId)
			{
			aSelectionNotify.SelectComplete(this,FindOrCreateProviderL(accessPoint));
			++selected;
			delete iterAP.Remove();
			}
		else
			{
			LOG_DETAILED( ESockLog::Printf(KCoreTMgrSelSupSubTag, _L("CCoreProviderSelectorSuper %08x:\t AP %d not in tier %08x"), this, accessPoint, iTierId) );
			iterAP++;
			}
		}

	if (selected == 0)
		{
		LOG_DETAILED( ESockLog::Printf(KCoreTMgrSelSupSubTag, _L("CCoreProviderSelectorSuper %08x:\t No APs selected [Tier: %08x]"), this, iTierId) );
		User::Leave(KErrArgument);
		}
	
	aSelectionNotify.SelectComplete(this,NULL);
	return;
	}

