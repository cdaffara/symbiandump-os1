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

/**
 @file
 @internalTechnology
*/


#include <e32def.h>
#include <commsdattypesv1_1.h> // CommsDat
#include <metadatabase.h> // CommsDat

#include <comms-infras/ss_activities.h>

#include <comms-infras/cs_connservparams.h>
#include <comms-infras/cs_connservparams_internal.h>
#include <elements/nm_messages_child.h>

#include "coretiernotificationactivity.h"
#include <comms-infras/coretiernotificationstates.h>
#include <comms-infras/coretiernotificationcollectors.h>
#include <comms-infras/ss_nodemessages_tiermanager.h>

#include <comms-infras/es_connectionservparameterbundletrace.h>
#include <comms-infras/es_connectionservparameterbundletraceimpl.h> // include this once per dll


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrPrvTNotAC, "ESockCrPrvTNotAC");
#endif



using namespace CommsDat;
using namespace ESock;
using namespace TierNotification;
using namespace NetStateMachine;
using namespace ConnectionServ;
using namespace CoreStates;
using namespace Messages;
using namespace MeshMachine;


template <class T>
class CleanupResetAndDestroy
	{
public:
	inline static void PushL(T& aRef);
private:
	static void ResetAndDestroy(TAny *aPtr);
	};

template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef);


template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
	{
	CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef));
	}

template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
	{
	static_cast<T*>(aPtr)->ResetAndDestroy();
	}

template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
	{
	CleanupResetAndDestroy<T>::PushL(aRef);
	}


//
// Tier Notification
//
// The reference tier status query / notification activity.
//
namespace TMTierNotificationActivity
{
DEFINE_EXPORT_CUSTOM_NODEACTIVITY(ECFActivityTierNotification, TierNotify, Messages::TNodeSignal::TNullMessageId, CTierNotificationActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(TierNotification::TAwaitingTierNotificationRegistration, TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, TierNotification::TInitTierNotifications, TierNotification::TAwaitingDataCollectors, TCancelOrErrorOrTag<KNoTag>)
	NODEACTIVITY_ENTRY(KNoTag, TierNotification::TStartTierNotifications, TierNotification::TAwaitingCancelOrErrorOrDestroy, TCancelOrErrorOrTag<KNoTag>)
	THROUGH_TRIPLE_ENTRY(KErrorTag, MeshMachine::TStoreError, TTag<KCancelTag>)
	NODEACTIVITY_ENTRY(KCancelTag, TierNotification::TInitiateTierNotificationCancel, TierNotification::TAwaitingDestroy, TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TSetIdle)
NODEACTIVITY_END()
}

namespace TMTierNotificationGetDataCollectorsActivity
{
DEFINE_EXPORT_NODEACTIVITY(ECFActivityTierGetDataCollectors, GetCollectors, TCFDataCollector::TTierGetDataCollectors)
	NODEACTIVITY_ENTRY(KNoTag, TierNotification::TGetDataCollectors, MeshMachine::TAwaitingAny, MeshMachine::TNoTag)
NODEACTIVITY_END()
}


EXPORT_C /*static*/ MeshMachine::CNodeActivityBase* CTierNotificationActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
	CTierNotificationActivity* inst = new (ELeave) CTierNotificationActivity(aActivitySig, aNode, c);
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "NewL()"),inst);
	return inst;
	}


/*virtual*/ CTierNotificationActivity::~CTierNotificationActivity()
	{
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "~CTierNotificationActivity()"),this);

	if (iQueryBundleOwner)
	    {
	    iQueryBundleOwner->Close();
	    }
	iCollectorSessions.ResetAndDestroy();
	delete iCache;
	delete iDbSession;
	}


/*virtual*/ void CTierNotificationActivity::InitL(CRefCountOwnedParameterBundle* aQuery, const RMessage2& aPlatSecInfo, TBool aOneOffQuery)
	{
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "StartL()"),this);

	// note we don't open a reference since we take the ownership from the DoL
	// that calls us
	__ASSERT_DEBUG(!iQueryBundleOwner, User::Panic(KSpecAssert_ESockCrPrvTNotAC, 1));
	iQueryBundleOwner = aQuery;
	iOneOffQuery = aOneOffQuery;

	if(!HasSufficientCapabilities(aPlatSecInfo))
		{
		User::Leave(KErrPermissionDenied);
		}
	}
	// now pass control back to the mesh so a separate activity can decide on the required collectors


/*virtual*/ void CTierNotificationActivity::StartL(RPointerArray<MDataCollector>& aCollectors)
	{
	if(aCollectors.Count() == 0)
		{
		User::Leave(KErrArgument);
		}

	iDbSession = CMDBSession::NewL(KCDVersion1_2);

	iCache = CTierNotificationCache::NewL();

	// set up a session with each collector
	for (TInt sessionId=0 ; sessionId<aCollectors.Count() ; ++sessionId)
		{
		MDataCollector* collector = aCollectors[sessionId];
		aCollectors[sessionId] = NULL;
		CleanupStack::PushL(collector);
		CDataCollectorSession* newCollectorSession = CDataCollectorSession::NewL(collector, *this, sessionId);
		CleanupStack::Pop(collector);
		CleanupStack::PushL(newCollectorSession);
		iCollectorSessions.AppendL(newCollectorSession);
		CleanupStack::Pop(newCollectorSession);
		}

	// Must run session creation and start loops separately in case the initial data fetch is synchronous.
	// Reasoning:
	//  when a full set of data for each collector is received, notification is only allowed if all
	//  collectors know they've got their full set.
	//   so all collectors must already be available to answer that question by the time this loop runs.
	for(TInt i=0; i<iCollectorSessions.Count() ; ++i)
		{
		iCollectorSessions[i]->StartL();
		}
	}


TBool CompareTypeIds(const STypeId& aFirst, const STypeId& aSecond)
    {
    return( aFirst == aSecond );
    }



EXPORT_C void CTierNotificationActivity::ReportChangesInCacheL()
	{
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "ReportChangesInCacheL()"),this);

	const CConnectionServParameterBundle* queryBundle = static_cast<const CConnectionServParameterBundle*>(iQueryBundleOwner->Ptr());
	const RArray<Meta::STypeId>& typesToReturn = queryBundle->GetParameterSetsToReturnL();

	CParameterBundleBase* resultBundle = NULL;
	CRefCountOwnedParameterBundle* bundleOwner = NULL;
	CParameterSetContainer* cacheParSetCtr = NULL;
	XNotificationCacheParameterSet* cacheParams = NULL;
	TInt i = 0;

	while ((cacheParSetCtr = iCache->Get(i++)) != NULL)
		{
		cacheParams = XNotificationCacheParameterSet::FindInParamSetContainer(*cacheParSetCtr);
		__ASSERT_DEBUG(cacheParams, User::Panic(KSpecAssert_ESockCrPrvTNotAC, 3));

		if(cacheParams->ShouldReport())
			{
			cacheParams->ClearToReport();

			if (!iOneOffQuery && iMessageCount == 0)
				{
				// Swallow first notification (pseudo "status report").
				//  This must be done because the first notification arises
				//   as a result of the difference between UNKNOWN
				//   and discovering the MATCHING data for the first time
				//   meaning that this data enters the "match set".
				//  This way only notifications of CHANGE will be sent
				//   N.B. a client must run a tier query AFTER it's registered
				//   for notification, in order to ensure its view of the data
				//   is up to date.
				continue;
				}

			// create a new bundle when we get the first result
			// (if bundleOwner is NULL then resultBundle is also NULL)
			if (!bundleOwner)
				{
				resultBundle = CParameterBundleBase::NewL();
				CleanupStack::PushL(resultBundle);
				bundleOwner = new(ELeave)CRefCountOwnedParameterBundle(resultBundle);
				CleanupStack::Pop();

				bundleOwner->Open();
                CleanupClosePushL(*bundleOwner);
				}

			CParameterSetContainer* newParSetCtr = CParameterSetContainer::NewL(*resultBundle,cacheParSetCtr->Id());
			const XParameterSetBase* cacheSet;
			TInt j=0;
			while((cacheSet = cacheParSetCtr->GetParameterSet(j++)) != NULL)
				{
				// don't copy the cache parameters
				if(cacheSet == cacheParams) continue;

				if(typesToReturn.Find(cacheSet->GetTypeId(),TIdentityRelation<STypeId>(CompareTypeIds)) == KErrNotFound )
					{
					_TIER_LOG_3(_L8("\t.. not including type 0x %08x , %d"),cacheSet->GetTypeId().iUid.iUid,cacheSet->GetTypeId().iType);
					continue;
					}

				XConnectionServParameterSet* newSet = static_cast<XConnectionServParameterSet*>(XParameterSetBase::NewInstanceL(cacheSet->GetTypeId()));
				CleanupStack::PushL(newSet);
				newParSetCtr->AddParameterSetL(newSet);
				CleanupStack::Pop(newSet);

				// this filters out any information we didn't ask for
				newSet->CopyFieldsRequiredByQueryFromL(*queryBundle, static_cast<const XConnectionServParameterSet&>(*cacheSet));
				}

//			if(newParSetCtr->GetParameterSet(0) == NULL)
//				{
//				rob reject param set ctr if no types contained therein
//				}
			}
		}
	iMessageCount++;

	__ASSERT_DEBUG(iOriginators.Count(), User::Panic(KSpecAssert_ESockCrPrvTNotAC, 4));

	// ok, result bundle has now been generated, let's send it to the originator.
	//   ownership is given to the recipient so pop it from the stack first
	if (bundleOwner)
		{
		CleanupStack::Pop(bundleOwner);
		}


	if (iOneOffQuery)
		{
		if (!bundleOwner)
			{
			// ok.. so no results were found in query...
			//   so create an empty bundle to signify this.
			resultBundle = CParameterBundleBase::NewL();
			CleanupStack::PushL(resultBundle);
			bundleOwner = new(ELeave) CRefCountOwnedParameterBundle(resultBundle);
			CleanupStack::Pop();

			bundleOwner->Open();
			}

		_TIER_LOG_BUNDLE("About to send result.", resultBundle);
		TCFTierStatusProvider::TTierStatus msg(bundleOwner);
		PostRefCountedBundleToOriginators(msg, *bundleOwner);
		}
	else // notification then
		{
		if(!bundleOwner)
			{
			if(iMessageCount == 1)
				{
				// ok.. so or notification has started up.
				//   so create an empty bundle to signify this.
				resultBundle = CParameterBundleBase::NewL();
    			CleanupStack::PushL(resultBundle);
    			bundleOwner = new(ELeave) CRefCountOwnedParameterBundle(resultBundle);
    			CleanupStack::Pop();
    			bundleOwner->Open();
				}
			else
				{
				// don't send empty bundles for notification
				_TIER_LOG(_L8("\tSomething changed but it's not appropriate to generate a notification.."));
				return;
				}
			}

    	_TIER_LOG_BUNDLE("About to send notification.", resultBundle);
		TCFTierStatusProvider::TTierNotification msg(bundleOwner);
		PostRefCountedBundleToOriginators(msg, *bundleOwner);
		}

	// and finally release our ref on the bundle
	bundleOwner->Close();
	}


void CTierNotificationActivity::PostRefCountedBundleToOriginators(const Messages::TSignatureBase& aMsg, CRefCountOwnedParameterBundle& aBundleOwner)
    {
    TInt originators = iOriginators.Count();

    for (TInt i=0; i<originators; i++)
        {
        // Open as many refcounts as there are originators before we do the Post.
        // This avoids a potential race where an originator, and the aBundleOwner's
        // creator is running in a higher priority thread than this. If both of
        // these conditions are met the bundle may be destroyed before we finish
        // sending. By ensuring "enough" refs are opened before we send we can
        // avoid this.
        aBundleOwner.Open();
        }

    TInt unusedRefs = originators - PostToOriginators(aMsg);

    // Close any unused refcounts on the bundle
    while (unusedRefs-- > 0)
        {
        aBundleOwner.Close();
        }
    }


EXPORT_C TBool CTierNotificationActivity::AllCollectorsUnlocked() const
	{
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "AllCollectorsUnlocked()"),this);
	for(TInt i=0 ; i<iCollectorSessions.Count() ; ++i)
		{
		if(iCollectorSessions[i]->IsLocked())
			{
			return EFalse;
			}
		}
	return ETrue;
	}

TBool CTierNotificationActivity::AllAccessPointsReadyForAllCollectors() const
	{
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "AllAccessPointsReadyForAllCollectors()"),this);

	// possible optimisation here for notification mode
	//  once we've reached return True we could set a flag.. which it would be up to
	//   disoverers of new access points to clear when it sees them, so we can start running the
	//    check again.

	CParameterSetContainer* cacheParSetCtr;
	XNotificationCacheParameterSet* cacheParams;
	TInt i = 0;
	while((cacheParSetCtr = iCache->Get(i++)) != NULL)
		{
		_TIER_LOG_PSC("Checking if ready: ",cacheParSetCtr);
		cacheParams = XNotificationCacheParameterSet::FindInParamSetContainer(*cacheParSetCtr);
		__ASSERT_DEBUG(cacheParams, User::Panic(KSpecAssert_ESockCrPrvTNotAC, 5));
		if( ! cacheParams->ReadyToReport())
			{
			return EFalse;
			}
		}
	return ETrue;
	}


EXPORT_C void CTierNotificationActivity::ReportIfReadyL()
	{
	if( ! AllCollectorsUnlocked() )
		{
		return;
		}

	if( ! AllAccessPointsReadyForAllCollectors() )
		{
		return;
		}

	// If we got this far then all sessions are unlocked. In the case of a query we should
	//  now initiate shutdown of all collectors before formulating our response.
	// This is because TierStatus is the last message this activity will send
	//  so it must be possible for the client to leave this node as soon as it has received TierStatus.
	//
	if(iOneOffQuery)
		{
		InitiateShutdown(KErrNone); // will send the results once everything is cleaned up
		}
	else
		{
		// for notify mode this will just mark the cache instead of formulating and sending a response.
		//  actual change notifications will happen via DataReceived
		ReportChangesInCacheL();
		}
	return;
	}


EXPORT_C void CTierNotificationActivity::InitiateShutdown(TInt aErrorToReport)
	{
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "ShutdownFinished()"),this);
	if(iShuttingDown)
		{
		return;
		}
	iShuttingDown=ETrue;

	SetError(aErrorToReport);
	TInt i=0 ;
	for( ; i<iCollectorSessions.Count() ; ++i)
		{
		CDataCollectorSession* sess = iCollectorSessions[i];
		if(sess)
			{
			sess->Shutdown();
			}
		}
	if(i==0)
		{
		ShutdownFinished();
		}
	}


EXPORT_C void CTierNotificationActivity::ShutdownFinished()
	{
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "ShutdownFinished()"),this);
	for(TInt i=0 ; i<iCollectorSessions.Count() ; ++i)
		{
		CDataCollectorSession* sess = iCollectorSessions[i];
		if(sess->HasShutDown())
			return;
		}

	// If we got this far then all sessions are shut down. In the case of a query we should
	//  now fomulate our response and send it.
	// Otherwise error originators saying we cancelled.
	//
	if(iOneOffQuery)
		{
		TInt err = Error();
		if(err == KErrNone)
			{
			TRAP(err,ReportChangesInCacheL());
			}
		if(err != KErrNone)
			{
			TEBase::TError msg(TCFTierStatusProvider::TTierStatusQuery::Id(), Error());
			PostToOriginators(msg);
			}
		}
	else
		{
		__ASSERT_DEBUG(Error() != KErrNone, User::Panic(KSpecAssert_ESockCrPrvTNotAC, 6));
		TEBase::TError msg(TCFTierStatusProvider::TTierNotificationRegistration::Id(), Error());
		PostToOriginators(msg);
		}

	// Any error has been handled. This prevents panic of mesh machinery
	SetError(KErrNone);

	// Our query/notification activity has finished.
	//  We need to send a message back to ourselves so the node can finish
	//   processing the Activity as defined in the map. We are (ab)using the
	//    TDestroy message for this purpose :-I
	TNodeCtxId us(ActivityId(),iNode.Id());
	RClientInterface::OpenPostMessageClose(us, us, TEChild::TDestroy().CRef());
	return;
	}



/*virtual*/ TBool CTierNotificationActivity::HasSufficientCapabilities(const RMessage2& aPlatSecInfo) const
	{
	// default.. all necessary policing has been done back at RConnectionServ IPC level.
	//  Specific technology implementations may wish to override this function in order to examine the client capabilities more deeply.
	(void)aPlatSecInfo;
	return ETrue;
	}

/*virtual*/ MEqualityComparator* CTierNotificationActivity::CreateEqualityComparatorL(const CParameterSetContainer& aData)
	{
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "CreateEqualityComparatorL()"),this);
	return new(ELeave) CAccessPointIdComparator(aData);
	}


EXPORT_C void CTierNotificationActivity::DataReceivedL(CDataCollectorSession& aSource, CParameterSetContainer& aNewData, TBool aAllowNotify)
	{
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "DataReceivedL()"),this);
	_TIER_LOG_PSC("Data received: ",&aNewData);

	MEqualityComparator* comparator = CreateEqualityComparatorL(aNewData);
	CleanupStack::PushL(comparator);

	CParameterSetContainer* cacheEntry = iCache->Find(*comparator);

	if(cacheEntry)
		{
		aSource.MergeL(*cacheEntry, aNewData);
		}
	else
		{
		// ok.. no record found.
		//  So create a record populated with unknowns and merge the new data
		//   into it.. this way only 1 merge function is needed
		//    for determining whether we should notify with this data.

		// This has to happen down in the collector as only the collector knows what an empty
		//   record should look like (by adding technology specifics &c).
		cacheEntry = aSource.ConstructEmptyRecordLC(aNewData.Id());

		aSource.MergeL(*cacheEntry, aNewData);
		iCache->AddL(cacheEntry);
		CleanupStack::Pop(cacheEntry);
		}

	if(aAllowNotify)
		{
		NotifyOtherCollectorsOfChangeL(aSource,*cacheEntry);
		}

	ReportIfReadyL();

	CleanupStack::PopAndDestroy(comparator);
	}



void CTierNotificationActivity::NotifyOtherCollectorsOfChangeL(CDataCollectorSession& aSource, CParameterSetContainer& aModifiedCacheEntry)
	{
	_TIER_LOG_2(_L8("CTierNotificationActivity %08x:\t" "NotifyOtherCollectorsOfChange()"),this);
	for(TInt i=0; i<iCollectorSessions.Count() ; ++i)
		{
		// don't go round in circles
		if(iCollectorSessions[i] != &aSource)
			{
			_TIER_LOG_3(_L8("\tNotifying collector session %d ( %08x ) of change"),i,iCollectorSessions[i]);
			iCollectorSessions[i]->CacheModifiedL(aModifiedCacheEntry);
			}
		}
	_TIER_LOG_PSC("After notifications: ",&aModifiedCacheEntry);
	}


//#define COMPARE_BY_ID_IN_PARAMETER_SET

/*virtual*/ TBool CAccessPointIdComparator::Matches(const CParameterSetContainer& aRhs)
	{
#ifdef COMPARE_BY_ID_IN_PARAMETER_SET
	const XAccessPointGenericParameterSet* lapgps = XAccessPointGenericParameterSet::FindInParamSetContainer(iToCompare);
	const XAccessPointGenericParameterSet* rapgps = XAccessPointGenericParameterSet::FindInParamSetContainer(aRhs);
	return(lapgps && rapgps && lapgps->AccessPointInfo() == rapgps->AccessPointInfo());
#else
	return (iToCompare.Id() == aRhs.Id());
#endif
	}




// Tier Notification Cache

/*static*/ CTierNotificationCache* CTierNotificationCache::NewL()
	{
	CTierNotificationCache* inst = new(ELeave) CTierNotificationCache();
	CleanupStack::PushL(inst);
	_TIER_LOG_2(_L8("CTierNotificationCache %08x:\t" "NewL()"), inst);
	inst->SetCacheBundle(CGenericParameterBundle::NewL());
	CleanupStack::Pop(inst);
	return inst;
	}


CTierNotificationCache::~CTierNotificationCache()
	{
	_TIER_LOG_2(_L8("CTierNotificationCache %08x:\t" "~CTierNotificationCache()"),this);
	delete iCacheBundle;
	}


CParameterSetContainer* CTierNotificationCache::Find(MEqualityComparator& aFinder)
	{
	_TIER_LOG_2(_L8("CTierNotificationCache %08x:\t" "Find()"),this);
	CParameterSetContainer* entry;
	TInt i=0;
	while((entry = iCacheBundle->GetParamSetContainer(i++)) != NULL)
		{
		if(aFinder.Matches(*entry))
			{
			return entry;
			}
		}
		return 0;
	}


void CTierNotificationCache::AddL(CParameterSetContainer* aItemToAdd)
	{
	_TIER_LOG_PSC("Adding to cache: ",aItemToAdd);
	iCacheBundle->AddParamSetContainerL(*aItemToAdd);
	}



// Data Collector Session



EXPORT_C CDataCollectorSession::CDataCollectorSession(MDataCollector* aCollector, CTierNotificationActivity& aActivity, TInt aSessionId)
	:	iActivity(aActivity)
	,	iCollector(aCollector)
	,	iSessionId(aSessionId)
	,	iIsLocked(ETrue)
	{
	iCollector->SetReceiver(*this);
	}


/*virtual*/
CDataCollectorSession::~CDataCollectorSession()
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "~CDataCollectorSession()"),this);
	delete iCollector;
	}


/*virtual*/
void CDataCollectorSession::StartL()
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "StartL()"),this);
	iCollector->StartL();
	}


/*virtual*/
CParameterSetContainer* CDataCollectorSession::ConstructEmptyRecordLC(TUint32 aId /*=0*/)
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "ConstructEmptyRecordLC()"),this);
	return iCollector->ConstructEmptyRecordLC(aId);
	}

/*virtual*/
void CDataCollectorSession::MergeL(CParameterSetContainer& aCacheData, CParameterSetContainer& aNewData)
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "MergeL()"),this);

	XNotificationCacheParameterSet* cacheParams = XNotificationCacheParameterSet::FindInParamSetContainer(aCacheData);
	if(!cacheParams)
		{
		cacheParams = XNotificationCacheParameterSet::NewL(aCacheData);
		}

	// PREPARE THE CACHE FLAGS

	// 1. InSet flag - whether the currently cached data matches the set of data to watch w.r.t. this data collector.
	//
	// We need to remember whether known data is currently in matching set for this data collector, and pass it into the merge fn.
	//  This means doesn't have to run a "before" and "after" match of the query.
	TBool inSet(cacheParams->InSet(iSessionId));

	// 2. ToReport flag - whether we should report this change to client
	//
	// The collector's MergeL function decides on the basis of the InSet flag and the new data whether it's appropriate to report
	//  the new data to the client.. This is an output variable only.. but let's set it to EFalse to be sure it's not garbage.
	TBool toReport(EFalse);

	_TIER_LOG_PSC("cache before merge: ",&aCacheData);
	_TIER_LOG_PSC("new data: ",&aNewData);

	iCollector->MergeL(aCacheData,aNewData,inSet,toReport);

	cacheParams->InSet(iSessionId,inSet);
	cacheParams->ToReport(iSessionId,toReport);

	cacheParams->SetRunning(iSessionId); // we need to match this collector
	cacheParams->SetReceived(iSessionId); // and we just received data for it

	_TIER_LOG_PSC("cache after merge: ",&aCacheData);
	}


// Called when cache is modified.. to possibly kickstart further discovery.
// 	be VERY careful if this leads to calling DataReceivedL as potentially
//   this could result in an infinite recursion
//    (DataReceived->AddToCache->NotifyOtherCollectors->CacheModified->DataReceived->....)
/*virtual*/
void CDataCollectorSession::CacheModifiedL(CParameterSetContainer& aModifiedEntry)
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "CacheModifiedL()"),this);
	TBool isRunning=EFalse; // collector sets this if it decides the notification
					// represents an AP it is collecting information about..
	iCollector->CacheModifiedL(aModifiedEntry,isRunning);
	if(isRunning)
		{
		XNotificationCacheParameterSet* cacheParams = XNotificationCacheParameterSet::FindInParamSetContainer(aModifiedEntry);
		__ASSERT_DEBUG(cacheParams, User::Panic(KSpecAssert_ESockCrPrvTNotAC, 7));
		// this collector is active in deciding that the AP is ready to report
		//   i.e. it is collecting data about this AP and it won't unlock til
		//      it has received data
		cacheParams->SetRunning(iSessionId);
		}
	}



// From MCollectedDataReceiver : called by data collector.
//   Takes ownership of aNewData and deletes it when finished
/*virtual*/
void CDataCollectorSession::DataReceivedL(CParameterSetContainer* aNewData, TBool aAllowNotify)
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "DataReceivedL()"),this);
	// take ownership straight away
	CleanupStack::PushL(aNewData);
	iActivity.DataReceivedL(*this,*aNewData,aAllowNotify);
	CleanupStack::PopAndDestroy(aNewData);
	}

// From MCollectedDataReceiver : called by data collector
/*virtual*/
void CDataCollectorSession::Lock()
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "Lock()"),this);
	iIsLocked = ETrue;
	}

// From MCollectedDataReceiver : called by data collector
/*virtual*/
void CDataCollectorSession::Unlock()
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "Unlock()"),this);
	iIsLocked = EFalse;

	// release notifications if necessary
	TRAP_IGNORE(iActivity.ReportIfReadyL());
	}

// From MCollectedDataReceiver : called by data collector
/*virtual*/
TBool CDataCollectorSession::IsLocked() const
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "IsLocked()"),this);
	return iIsLocked;
	}


// From MCollectedDataReceiver : called by data collector
/*virtual*/
void CDataCollectorSession::ShutdownFinished()
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "ShutdownFinished()"),this);

	delete iCollector;
	iCollector=0;

	iActivity.ShutdownFinished();
	}

// From MCollectedDataReceiver : called by data collector when it wants to do
//           its own cleanup and destruction
/*virtual*/
void CDataCollectorSession::Detach()
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "Detach()"),this);

	iCollector=0;

	iActivity.ShutdownFinished();
	}


void CDataCollectorSession::Error(TInt aErr)
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "Error()"),this);

	iActivity.InitiateShutdown(aErr);
	}



/*virtual*/
TBool CDataCollectorSession::HasShutDown()
	{
	return iCollector?ETrue:EFalse;
	}


/*virtual*/
void CDataCollectorSession::Shutdown()
	{
	_TIER_LOG_2(_L8("CDataCollectorSession %08x:\t" "Shutdown()"),this);
	iCollector->Shutdown();
	}

