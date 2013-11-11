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
// coretiernotificationcollectors.h
// This file contains the data collectors as used by the tier notification activity
// in the core generic & legacy tiers.
// These collectors may be reused as they are by custom tier implementations, or
// serve as reference implementations if there is a need to create custom collectors.
// Included are the following collectors (inheritance hierarchy shown by indent).
// All but bottom leaf classes in this list are abstract. Bottom leaf classes implement all virtuals
// so are concrete / instantiable.
// -	CDataCollectorBase	--	common functions for all collectors in the notification engine
// 		(can be moved out if notification engine used in other planes)
// --		CTierDataCollector	--	tier specifics.. for all tier notification collectors
// ---			CFactoryNotifyDataCollector	--	uses IFactoryNotify to receive created/destroyed events
// ----				CMcprFactoryNotifyDataCollector	--	tells other collectors when MCPRs created/destroyed
// ----				CCprFactoryNotifyDataCollector	--	same for CPRs - not used at present, maybe needed later
// ---			CAccessPointPlaneStatusCollector	--	collects status of planes (which providers are up/started)
// 														for each AP. Currently only "CPR up" attribute implemented.
// ---			CIpProtoAccessPointParameterCollector	--	collects legacy info for legacy implementations.
// 															Use by clients outside comms process should be
// 															discouraged/prevented
// ---			CCommsDatDataCollector	--	collects Configured/Restricted attribute for APs.
// 											uses CCommsDatNotificationSubscriber to spot added records
// ---			CTierDataCollectorNode	--	TCFNode-based collector. for collectors which receive data via transport
// ----				CMCprFactoryQueryDataCollector	--	performs one-off query of MCPR factory ctnr to see which
// 							     						APs are in use. Optimised to only create factories+MCPRs if
// 									    				necessary. Otherwise just finds existing MCPRs
//										     			(e.g. for a "what exists now" type query).
// 												    	Node-based purely so it can join to found MCPRs in order to lock
// 													    the notification engine's interest in them
//                  CCprFactoryQueryDataCollector -- performs one-off query of CPR factory to see what cprs have been
//                                                   created.
// ----				CTierInfoCollectorNode	--	node-based collector which collects information from another node
// 												and requires joining/leaving that node before/after the
//												query made.
// -----				CAvailabilityDataCollector	--	collects Available attribute by talking to MCPR over transport
// -----				CActiveStatusDataCollector	--	collects Started attribute by talking to MCPR over transport
// 					The use of the transport between objects in the management plane is not optimal (because they'll
// 					always* be in the same thread) so may be replaced later by synchronous interfaces on MCPRs.
//

/**
 @file
 @internalTechnology
*/


#include <d32dbms.h> // for RDbNotifier::ECommit value
#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/es_connectionservparameterbundle.h>
#include <comms-infras/cs_connservparams.h>
#include <comms-infras/cs_connservparams_internal.h>
#include <comms-infras/coretiernotificationactivity.h>
#include "coretiernotificationcollectors.h"
#include <comms-infras/ss_nodemessages_availability.h>
#include <comms-infras/ss_nodemessages_mcpr.h>

#include <commsdattypesv1_1.h> // CommsDat
#include <metadatabase.h> // CommsDat
#include <comms-infras/ss_tiermanagerutils.h> // TierManagerUtils
#include <ss_glob.h> // SockManGlobals
#include <comms-infras/ss_metaconnprov_internal.h>
#include <comms-infras/es_availability.h>
#include <elements/nm_messages_child.h>
#include <elements/nm_messages_peer.h>

#include <comms-infras/agentmessages.h> // for extracting IAP # on agent layer


#include <comms-infras/ss_logext.h>

#include <comms-infras/es_connectionservparameterbundletrace.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#include <commsdattypesv1_1_partner.h>
#endif

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrPrvTNotCC, "ESockCrPrvTNotCC");
#endif



using namespace TierNotification;
using namespace ConnectionServ;
using namespace ESock;
using namespace CommsDat;
using namespace Messages;
using namespace Factories;

//
// MDataCollector //
//

/*virtual*/ MDataCollector::~MDataCollector()
	{
	_TIER_LOG_2(_L8("MDataCollector %08x:\t" "~MDataCollector()"),this);
	}



//
// CDataCollectorBase //
//

/*virtual*/ CDataCollectorBase::~CDataCollectorBase()
	{
	iQueryBundleOwner->Close();
	_TIER_LOG_2(_L8("\tColl (base) %08x:\t" "~CDataCollectorBase()"),this);
	}

/*virtual*/ void CDataCollectorBase::SetReceiver(MCollectedDataReceiver& aReceiver)
	{
	_TIER_LOG_2(_L8("\tColl (base) %08x:\t" "SetReceiver()"),this);
	iReceiver = &aReceiver;
	}

/*virtual*/ void CDataCollectorBase::StartL()
	{
	_TIER_LOG_2(_L8("\tColl (base) %08x:\t" "StartL()"),2);
	DoStartL();
	}

/*virtual*/ void CDataCollectorBase::Shutdown()
	{
	_TIER_LOG_2(_L8("\tColl (base) %08x:\t" "Shutdown()"),this);
	// nothing interesting if synchronous shutdown possible
	iReceiver->ShutdownFinished();
	}

/*virtual*/ void CDataCollectorBase::CacheModifiedL(CParameterSetContainer& aModifiedEntry, TInt& aIsRunning /*output*/)
	{
	_TIER_LOG_2(_L8("\tColl (base) %08x:\t" "CacheModifiedL()"),this);
	// ignore by default.. override if you want to monitor the cache
	(void)aModifiedEntry;
	// no interest by default.. so don't set running flag
	(void)aIsRunning;
	}




//
// CTierDataCollector //
//

/*virtual*/ CTierDataCollector::~CTierDataCollector()
	{
	_TIER_LOG_2(_L8("\tColl (tier data) %08x:\t" "~CTierDataCollector()"),this);
	}

/*virtual*/ CParameterSetContainer* CTierDataCollector::ConstructEmptyRecordLC(TUint32 aId /*=0*/)
	{
	_TIER_LOG_2(_L8("\tColl (tier data) %08x:\t" "ConstructEmptyRecordLC()"),this);
	// This has to happen down in the collector as only the collector knows what an empty
	//   record will look like (by adding technology specifics &c)
	CParameterSetContainer* aEmptyInstance = CParameterSetContainer::NewL(aId);
	CleanupStack::PushL(aEmptyInstance);

// rjl why bother with this???	// for all inbuilt collectors this is sufficient. Specific technologies will need to create more.
	XAccessPointGenericParameterSet* aBlankParamSet = XAccessPointGenericParameterSet::NewL(*aEmptyInstance);
	aBlankParamSet->AccessPointInfo(TAccessPointInfo(aId));

	return aEmptyInstance;
	}



/*virtual*/ void CTierDataCollector::MergeL(CParameterSetContainer& aCacheEntry, CParameterSetContainer& aNewData,
								TBool& aInSet, TBool& aToReport)
	{
	_TIER_LOG_2(_L8("\tColl (tier data) %08x:\t" "MergeL()"),this);

	__ASSERT_DEBUG(aCacheEntry.Id() == 0 || aCacheEntry.Id() == aNewData.Id(), User::Panic(KSpecAssert_ESockCrPrvTNotCC, 1));

	// algo:

	// old (cache)	#3		a1  b2  c3
	// new (data)	#3		a2  b2      d4

	//	for each type in data
	//		if type not in cache... transfer to cache
	//

	XConnectionServParameterSet* cacheSet;
	XConnectionServParameterSet* newSet;
	TInt i=0;
	while((newSet = static_cast<XConnectionServParameterSet*>(aNewData.GetParameterSet(i))) != NULL)
		{
		STypeId newType = newSet->GetTypeId();
		cacheSet = static_cast<XConnectionServParameterSet*>(aCacheEntry.FindParameterSet(newType));

		if(!cacheSet)
			{
			// remove it from the new data..
			aNewData.ClearParameterSetPointer(i);

			// keep hold of it in case next call fails
			CleanupStack::PushL(newSet);

			// add it to the cache instead..
			aCacheEntry.AddParameterSetL(newSet);

			// ok, the cache owns it now..
			CleanupStack::Pop(newSet);
			}
		else
			{
			// more complicated merging required..
			// we must only call this once we know the set types are the same.
			cacheSet->UpdateWithL(*newSet);
			}
		++i;
		}


	TBool newInSet = SetMatchesQuery(aCacheEntry);

	if( (!aInSet && newInSet)  // just started to match
	 || (aInSet && !newInSet)  // just stopped matching
	  )
		{
		aToReport = ETrue;
		}
	aInSet = newInSet;
	}


EXPORT_C CTierDataCollector::CTierDataCollector(CTierNotificationActivity& aActivity)
	: CDataCollectorBase(aActivity.QueryBundleOwner()), iActivity(aActivity)
	{}


//
// CTierDataCollectorNode //
//

/*virtual*/ CTierDataCollectorNode::~CTierDataCollectorNode()
	{
	_TIER_LOG_2(_L8("\tColl (tier data node) %08x:\t" "~CTierDataCollectorNode()"),this);
	iProviders.Close();
	}


CTierDataCollectorNode::TDataProviderStatus* CTierDataCollectorNode::FindDataProviderStatus(TAccessPointInfo aApId)
	{
	_TIER_LOG_2(_L8("\tColl (tier data node) %08x:\t" "FindDataProviderStatus()"),this);
	TInt i=0;
	for( ; i<iProviders.Count() ; ++i)
		{
		TDataProviderStatus& provStatus = iProviders[i];
		if(provStatus.iApId == aApId)
			{
			return &provStatus;
			}
		}
	return NULL;
	}

CTierDataCollectorNode::TDataProviderStatus* CTierDataCollectorNode::FindDataProviderStatus(const Messages::TNodeId& aId)
	{
	_TIER_LOG_2(_L8("\tColl (tier data node) %08x:\t" "FindDataProviderStatus()"),this);
	TInt i=0;
	for( ; i<iProviders.Count() ; ++i)
		{
		TDataProviderStatus& provStatus = iProviders[i];
		if(provStatus.iProvider == aId)
			{
			return &provStatus;
			}
		}
	return NULL;
	}


/*virtual*/ void CTierDataCollectorNode::Shutdown()
	{
	_TIER_LOG_2(_L8("\tColl (tier data node) %08x:\t" "Shutdown()"),this);
	// need to post ClientLeavingRequest to each provider.
	//  the responses are all ticked off before we can destruct.
	iShuttingDown_Leaving=ETrue;
	TEPeer::TLeaveRequest leavingRequestMsg;
	TInt i=0;
	for( ; i<iProviders.Count() ; ++i )
		{
		RClientInterface::OpenPostMessageClose(Id(), iProviders[i].iProvider, leavingRequestMsg);
		}

	if(iReceiver)
		{
		if(i == 0) // nothing sent so nothing to wait for!
			{
			iReceiver->ShutdownFinished();
			}
		else
			{
			// Ok.. so we must hang around waiting for all
			//  cancel completes before it's safe to destruct self.
			// Let's not let this get in the way of activity synchronous
			//  shutdown.. so tell the receiver it can forget about us.
			iReceiver->Detach();
			iReceiver = 0;
			}
		}
	}








// from Messages::ASimpleNodeIdBase
/*virtual*/ void CTierDataCollectorNode::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage)
	{
	_TIER_LOG_2(_L8("\tColl (tier data node) %08x:\t" "ReceivedL()"),this);

	if(aMessage.IsMessage<TEBase::TError>())
		{
		ReceivedErrorL(aSender, aMessage);
		}
	else if(aMessage.IsMessage<TEPeer::TLeaveComplete>())
		{
		ReceivedLeaveCompleteL(aSender, aMessage);
		}
	}




void CTierDataCollectorNode::ReceivedErrorL(const TRuntimeCtxId& /*aSender*/, Messages::TSignatureBase& aCFMessage)
	{
	_TIER_LOG_2(_L8("\tColl (tier data node) %08x:\t" "ReceivedErrorL()"),this);
	TEBase::TError& inMsg =
		message_cast<TEBase::TError>(aCFMessage);
	if(iShuttingDown_Leaving)
		{
		// meh. swallow it. log it just in case it's of interest
		_TIER_LOG_2(_L8("\t\t.. received an error %d while already shutting down-ignoring it."),inMsg.iValue);
		}
	else
		{
		// ok bad things have happened.. tell session..
		iReceiver->Error(inMsg.iValue);
		}
	}

void CTierDataCollectorNode::ReceivedLeaveCompleteL(const TRuntimeCtxId& aSender, Messages::TSignatureBase& /*aCFMessage*/)
	{
	_TIER_LOG_2(_L8("\tColl (tier data node) %08x:\t" "ReceivedLeaveCompleteL()"),this);
	if(iShuttingDown_Leaving)
		{
		TInt i;
		for(i=0 ; i<iProviders.Count() ; ++i )
			{
			if(iProviders[i].iProvider == address_cast<TNodeId>(aSender))
				{
				_TIER_LOG(_L8("\t\t.. received a TLeaveComplete- removing ref to provider."));
				iProviders.Remove(i);
				--i;
				break;
				}
			}
		__ASSERT_DEBUG(i < iProviders.Count(), User::Panic(KSpecAssert_ESockCrPrvTNotCC, 2)); // should have recognised the provider
		if(iProviders.Count()==0)
			{
			// ok.. all done shutting down. As I disowned my parent
			//  some time ago and now it has died, I must now destroy myself.
			delete this;
			}
		}
	else
		{
		// why the heck did I receive this?
		_TIER_LOG(_L8("\t\t.. why did I receive a stray LeaveComplete? ignoring it."));
		__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockCrPrvTNotCC, 3));
		}
	}






//
// CTierInfoCollectorNode //
//

/*virtual*/ CTierInfoCollectorNode::~CTierInfoCollectorNode()
	{
	_TIER_LOG_2(_L8("\tColl (tier info node) %08x:\t" "~CTierInfoCollectorNode()"),this);
	iProviders.Close();
	}



/*virtual*/ void CTierInfoCollectorNode::DoStartL()
	{
	_TIER_LOG_2(_L8("\tColl (tier info node)  %08x:\t" "DoStartL()"),this);
	// do nothing.. wait for AP records to come in from the other collector(s)
	//  so we don't want to block anything..
	iReceiver->Unlock();
	}

/*virtual*/ void CTierInfoCollectorNode::CacheModifiedL(CParameterSetContainer& aModifiedEntry, TInt& aIsRunning /*output*/)
	{
	_TIER_LOG_2(_L8("\tColl (tier info node)  %08x:\t" "CacheModifiedL()"),this);
	// ok a new record may have come in.. let's see if we know about it

	TDataProviderStatus* ps = FindDataProviderStatus(aModifiedEntry.Id());
	if(!ps)
		{
		// new record.. let's look for the comms id
		const XCoreCollectorSharedParameterSet* ccsps = XCoreCollectorSharedParameterSet::FindInParamSetContainer(aModifiedEntry);
		if(ccsps)
			{
			if(ccsps->McprPtr() != NULL)
				{
				// ok.. fire off query to this new provider...
				TDataProviderStatus provider;
				provider.iProvider = ccsps->McprCommsId();
				provider.iProviderPtr = static_cast<CMetaConnectionProviderBase*>(ccsps->McprPtr());
				provider.iApId = aModifiedEntry.Id();
				iProviders.AppendL(provider);

				// should only be joining once!
				__ASSERT_DEBUG(provider.iProviderPtr->FindClient(Id()) == NULL, User::Panic(KSpecAssert_ESockCrPrvTNotCC, 4));

				// Join the provider from this collector node.
                _TIER_LOG_SVG(Id().Node(), provider.iProviderPtr->Id().Node(), "Join");
				provider.iProviderPtr->AddClientL(Id(), TClientType(TCFClientType::ECtrl, TCFClientType::EAdministrative));

				SubscribeWithProviderL(provider);
				// we set this so notification functionality waits for data to be received
				//   before reporting this AP is permitted.
				aIsRunning = ETrue;
				}
			else
				{
				// eek. we can only assume we couldn't fetch all the data.. so at least mark this as done by creating
				//   a record full of unknowns
				// may be able to optimise this by just setting flag to running, received, !tonotify, !inset
				CParameterSetContainer* emptyRecord = ConstructEmptyRecordLC(aModifiedEntry.Id());
				CleanupStack::Pop(emptyRecord);

				iReceiver->DataReceivedL(emptyRecord,EFalse); // EFalse prevents infinite recursion
 				}
 			}
		}
	}


/*virtual*/ void CTierInfoCollectorNode::Shutdown()
	{
	_TIER_LOG_2(_L8("\tColl (tier info node)  %08x:\t" "Shutdown()"),this);
	// need to post cancels.. we'll track the responses
	//   before sending ClientLeaveRequests.....
	iShuttingDown_Cancelling=ETrue;
	TEBase::TCancel cancelMsg;
	TInt i=0;
	for( ; i<iProviders.Count() ; ++i )
		{
		RClientInterface::OpenPostMessageClose(Id(), iProviders[i].iProvider, cancelMsg);
		}

	if(iReceiver)
		{
		if(i == 0) // nothing sent so nothing to wait for!
			{
			iReceiver->ShutdownFinished();
			}
		else
			{
			// Ok.. so we must hang around waiting for all
			//  cancel completes before it's safe to destruct self.
			// Let's not let this get in the way of activity synchronous
			//  shutdown.. so tell the receiver it can forget about us.
			iReceiver->Detach();
			iReceiver = 0;
			}
		}
	}


// from Messages::ASimpleNodeIdBase
/*virtual*/ void CTierInfoCollectorNode::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage)
	{
	_TIER_LOG_2(_L8("\tColl (tier info node) %08x:\t" "ReceivedL()"),this);

	if(aMessage.IsMessage<TEBase::TError>())
		{
		ReceivedErrorL(aSender, aMessage);
		}
	else if(aMessage.IsMessage<TEPeer::TLeaveComplete>())
		{
		ReceivedLeaveCompleteL(aSender, aMessage);
		}
	}




void CTierInfoCollectorNode::ReceivedErrorL(const TRuntimeCtxId& aSender, Messages::TSignatureBase& aCFMessage)
	{
	_TIER_LOG_2(_L8("\tColl (tier info node) %08x:\t" "ReceivedErrorL()"),this);
	if(iShuttingDown_Cancelling)
		{
		TInt i;
		TInt cancelledCount=0;
		for(i=0 ; i<iProviders.Count() ; ++i )
			{
			if(iProviders[i].iProvider == address_cast<TNodeId>(aSender))
				{
				_TIER_LOG(_L8("\t\t.. received a TError- marking as cancelled."));
				iProviders[i].MarkCancelled();
				}
			if(iProviders[i].WasCancelled())
				{
				++cancelledCount;
				}
			}

		if(iProviders.Count()==cancelledCount)
			{
			// ok.. all done cancelling. Time to leave
			iShuttingDown_Cancelling = EFalse;
			CTierDataCollectorNode::Shutdown();
			}
		}
	else
		{
		// let the base try to handle it
		CTierDataCollectorNode::ReceivedErrorL(aSender, aCFMessage);
		}
	}




//
// CCommsDatNotificationSubscriber //
//

CCommsDatNotificationSubscriber::CCommsDatNotificationSubscriber(MCommsDatNotificationReceiver* aReceiver)
		: CActive(0),iReceiver(aReceiver)
	{
	}

void CCommsDatNotificationSubscriber::ConstructL()
	{
	CActiveScheduler::Add(this);
	TInt result = iProperty.Attach(KUidSystemCategory, KUidCommDbNotificationEvent.iUid);
	if(result != KErrNone)
		{
		User::LeaveIfError(iProperty.Define(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, RProperty::EInt));
		User::LeaveIfError(iProperty.Attach(KUidSystemCategory, KUidCommDbNotificationEvent.iUid));
		}

	iProperty.Subscribe(iStatus);
	SetActive();
	}


void CCommsDatNotificationSubscriber::RunL()
	{
	// .. and repeat..
	iProperty.Subscribe(iStatus);

	TInt publishedValue;
	TInt result = iProperty.Get(publishedValue);
	if(result != KErrNone)
		{
		User::Panic(_L("CAvailabilityTestingSubscriber get"), 1);
		}

	iReceiver->CommsDatNotificationL(publishedValue);
	SetActive();
	}

void CCommsDatNotificationSubscriber::DoCancel()
	{
	iProperty.Cancel();
	}


//
// CCommsDatDataCollector //
//

CCommsDatDataCollector::CCommsDatDataCollector(CTierNotificationActivity& aActivity)
		: CTierDataCollector(aActivity)
	{
	_TIER_LOG_2(_L8("\tColl -> Commsdat %08x:\t" "CCommsDatDataCollector()"),this);
	}


/*virtual*/ CCommsDatDataCollector::~CCommsDatDataCollector()
	{
	_TIER_LOG_2(_L8("\tColl -> Commsdat %08x:\t" "~CCommsDatDataCollector()"),this);
	iKnownAccessPoints.Close();
	delete iSubscriber;
	}


/*virtual*/ void CCommsDatDataCollector::DoStartL()
	{
	_TIER_LOG_2(_L8("\tColl -> Commsdat %08x:\t" "DoStartL()"),this);

	const XAccessPointGenericQuery* query = Query().FindGenericQuery();
	if(query == NULL)
		{
		User::Leave(KErrArgument);
		}

	TAccessPointFlagFilterValue hiddenFilter = query->AccessPointStatusFilter().Restricted();

	// optimisation.. only look at hidden records if query tells us it cares
	if(hiddenFilter == EAccessPointFlagMatchFalse
		|| hiddenFilter == EAccessPointFlagMatchAny)
		{
		Activity().GetDbSession().SetAttributeMask(ECDHidden);
		}

	// Register for change notifications
	iSubscriber = new CCommsDatNotificationSubscriber(this);

	ReadDbL();

	// finished 1st query so unlock collector
	iReceiver->Unlock();
	}

void CCommsDatDataCollector::ReadDbL()
	{
	_TIER_LOG_2(_L8("\tColl -> Commsdat %08x:\t" "ReadDbL()"),this);
	//	1.AP table: read all records with your Tier Id and AP Id in allowed set
	CMDBRecordSet<CCDAccessPointRecord>* apRecs;
	const XAccessPointGenericQuery* query = Query().FindGenericQuery();
	if(query)
	{
		apRecs = TierManagerUtils::LoadAccessPointRecordsL(Activity().TierManager().TierId(), query->AccessPointsToMatch(), Activity().GetDbSession());
	}
	else // get all records
	{
		apRecs = TierManagerUtils::LoadAccessPointRecordsL(Activity().TierManager().TierId(), Activity().GetDbSession());
	}
	CleanupStack::PushL(apRecs);

	TInt i=0;
	for( ; i < apRecs->iRecords.Count() ; ++i )
		{
		CCDAccessPointRecord* apRec = apRecs->operator[](i);

		TInt apId = apRec->iRecordTag;

		CParameterSetContainer* newAP = CParameterSetContainer::NewL(apId);
		CleanupStack::PushL(newAP);

		XAccessPointGenericParameterSet* newStatus = XAccessPointGenericParameterSet::NewL(*newAP);

		newStatus->AccessPointInfo(TAccessPointInfo(apId));

		TAccessPointFlagValue restrictedFlag =
			(apRec->iAppSID) ? EAccessPointFlagTrue : EAccessPointFlagFalse;

		TAccessPointStatus apStatus;
		apStatus.Configured(EAccessPointFlagTrue); // in commsdat so must be configured
		apStatus.Restricted(restrictedFlag);

		newStatus->AccessPointStatus(apStatus);


		// pop the new access point record because we're about to pass ownership of it to DataReceivedL
		CleanupStack::Pop(newAP);
		iReceiver->DataReceivedL(newAP);
		}

	CleanupStack::PopAndDestroy(apRecs);

	}

/*virtual*/ void CCommsDatDataCollector::CommsDatNotificationL(TInt aEvent)
	{
	_TIER_LOG_2(_L8("\tColl -> Commsdat %08x:\t" "CommsDatNotification()"),this);
	// slightly rubbish notification mechanism.. rescan the whole lot!!!
	if(aEvent == RDbNotifier::ECommit)
		{
		ReadDbL();
		}
	}

TBool CCommsDatDataCollector::SetMatchesQuery(const CParameterSetContainer& aDestEntry)
	{
	const XAccessPointGenericParameterSet* apgps = XAccessPointGenericParameterSet::FindInParamSetContainer(aDestEntry);
	if(apgps)
		{
		const XAccessPointGenericQuery* apgq = Query().FindGenericQuery();
		if(apgq)
			{
			const TAccessPointStatus& aps = apgps->AccessPointStatus();
			const TAccessPointStatusFilter& apsf = apgq->AccessPointStatusFilter();

			if(	TAccessPointStatusFilter::Matches(apsf.Configured(),aps.Configured()) &&
				TAccessPointStatusFilter::Matches(apsf.Restricted(),aps.Restricted()) )
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}






//
// CMCprFactoryQueryDataCollector //
//

CMCprFactoryQueryDataCollector::CMCprFactoryQueryDataCollector(CTierNotificationActivity& aActivity)
	: CTierDataCollectorNode(aActivity)
	{
	LOG_NODE_CREATE(KTierTag, CMCprFactoryQueryDataCollector);
	}


/*virtual*/ CMCprFactoryQueryDataCollector::~CMCprFactoryQueryDataCollector()
	{
	LOG_NODE_DESTROY(KTierTag, CMCprFactoryQueryDataCollector);
	}


/*virtual*/ void CMCprFactoryQueryDataCollector::DoStartL()
	{
	_TIER_LOG_2(_L8("\tColl -> FactQuery %08x:\t" "DoStartL()"),this);

	if (Activity().TierManager().TierId().iUid == 0x10281DF0) // doesn't work for ipproto, because ipproto behaves differently to every other layer
		{
		User::Leave(KErrNotSupported);
		}

	//  1. AP table: read matching records with your Tier Id
	//  2. instantiate 1 MCPR per AP record that matches the query
	//  3. join to each one
	//
	//  4. update AccessPointPlaneStatus with existence state of CPR
	//
	//  5. create the skeleton record for and report it to the session, so the
	//      other interested collectors can use it as a seed

	//	1.AP table: read all records with your Tier Id and AP Id in allowed set

	CMDBRecordSet<CCDAccessPointRecord>* apRecs;
	const XAccessPointGenericQuery* query = Query().FindGenericQuery();
	if(query)
	{
		apRecs = TierManagerUtils::LoadAccessPointRecordsL(Activity().TierManager().TierId(), query->AccessPointsToMatch(), Activity().GetDbSession());
	}
	else // get all records
	{
		apRecs = TierManagerUtils::LoadAccessPointRecordsL(Activity().TierManager().TierId(), Activity().GetDbSession());
	}
	CleanupStack::PushL(apRecs);

	TInt i=0;
	for( ; i < apRecs->iRecords.Count() ; ++i )
		{
		CCDAccessPointRecord* apRec = apRecs->operator[](i);

		TInt apId = apRec->iRecordTag;

		// Find MCPR factory
		CCDMCprRecord* mcprRec=NULL;
		CMetaConnectionProviderFactoryBase* factory=NULL;
		CMetaConnectionProviderBase* mcpr=NULL;

		TRAPD(err,mcprRec = TierManagerUtils::LoadMCprRecordL(apRec->iMCpr,Activity().GetDbSession()));

		if(err)
			{
			__ASSERT_DEBUG(mcprRec == NULL, User::Panic(KSpecAssert_ESockCrPrvTNotCC, 5));
			_TIER_LOG_3(_L8("DoStartL(): TierManagerUtils::LoadMCprRecordL(%d) failed, err code %d."),TInt(apRec->iMCpr),err);
			}
		else
			{
			CleanupStack::PushL(mcprRec);
			TUid mCprUid = TUid::Uid(mcprRec->iMCprUid);

			//  2. instantiate 1 MCPR per AP record
			if(iFindOnly)
				{
				TRAP(err, factory = static_cast<CMetaConnectionProviderFactoryBase*>((*SockManGlobals::Get()->iMetaConnectionFactories).FindFactory(mCprUid)));
				if(!factory)
					{
					err = KErrNotFound;
					}
				}
			else
				{
				TRAP(err, factory = static_cast<CMetaConnectionProviderFactoryBase*>((*SockManGlobals::Get()->iMetaConnectionFactories).FindOrCreateFactoryL(mCprUid)));
				}

			if(err)
				{
				if(iFindOnly)
					{
					_TIER_LOG_4(_L8("DoStartL(): Can't find factory w uid %d (0x%08x), err code %d. Not necessarily a problem as I'm trying not to create any factory objects."),mCprUid,mCprUid,err);
					}
				else
					{
					_TIER_LOG_4(_L8("DoStartL(): Can't find/create factory w uid %d (0x%08x), err code %d !!!"),mCprUid,mCprUid,err);
					}
				}
			else
				{
				//Find or create the provider
				TUid tierId = TierManagerUtils::MapElementIdToTagId(apRec->iTier, Activity().GetDbSession());
				TProviderInfo providerInfo(tierId,apRec->iRecordTag);
				TUid tierImplUid = TierManagerUtils::MapTierIdtoTierImplIdL(tierId, Activity().GetDbSession());
				TMetaConnectionFactoryQuery query (providerInfo,tierImplUid);

				if(iFindOnly)
					{
					TRAP(err, mcpr = static_cast<CMetaConnectionProviderBase*>(factory->Find(query)));
					if(!mcpr)
						{
						err = KErrNotFound;
						}
					}
				else
					{
					TRAP(err, mcpr = static_cast<CMetaConnectionProviderBase*>(factory->FindOrCreateObjectL(query)));
					}

				if(err)
					{
					if(iFindOnly)
						{
						_TIER_LOG_3(_L8("DoStartL(): Can't find MCPR id %d, err code %d. Not necessarily a problem as I'm trying not to create any factory objects."),TInt(apRec->iMCpr),err);
						}
					else
						{
						_TIER_LOG_3(_L8("DoStartL(): Can't find/create MCPR id %d, err code %d!!"),TInt(apRec->iMCpr),err);
						}
					}
				}
			}

		CParameterSetContainer* newAP = CParameterSetContainer::NewL(apId);
		CleanupStack::PushL(newAP);

		// Not pushed onto cleanup stack since if the NewL succeeds it is owned by newAP
		XAccessPointGenericParameterSet* newStatus = XAccessPointGenericParameterSet::NewL(*newAP);
		newStatus->AccessPointInfo(TAccessPointInfo(apId));
		TAccessPointStatus apStatus;
		apStatus.Configured(EAccessPointFlagTrue); // in commsdat so must be configured
		newStatus->AccessPointStatus(apStatus);

		// Not pushed onto cleanup stack since if the NewL succeeds it is owned by newAP
		XCoreCollectorSharedParameterSet* ccsps = XCoreCollectorSharedParameterSet::NewL(*newAP);

		if(err)
			{
			if(iFindOnly)
				{
				_TIER_LOG_3(_L8("DoStartL(): Err code %d. Possibly just because MCPR hasn't been created yet. Treating MCPR on AP %d as all unknown w.r.t. this collector."),err, apId);
				}
			else
				{
				_TIER_LOG_3(_L8("DoStartL(): Err code %d, ensure commsdat is configured correctly. Treating MCPR on AP %d as all unknown w.r.t. this collector."),err, apId);
				}
			// ok.. will simulate receiving this record with all "unknown" data..
			ccsps->McprPtr(NULL);
			}
		else
			{
			//Send request
			TNodeId mcprId = mcpr->Id();

			if(mcpr->FindClient(Id()))
				{
				// weird.. duplicated MCPR in commsdat. let's be tolerant and ignore it.
				//   It means we'll update the data if anything more has been discovered.
				}
			else
				{
				//  3. join collector node to each one as a control client.. this ensures it stays alive til the other collectors
				//            join to it for their purposes
				mcpr->AddClientL(Id(), TClientType(TCFClientType::ECtrl, TCFClientType::EAdministrative));

				// .. and track the join from my side (for further processing / cleanup)
				TDataProviderStatus provider;
				provider.iProvider = mcprId;
				provider.iProviderPtr = mcpr;
				provider.iApId = TAccessPointInfo(apId);
				iProviders.AppendL(provider);
				}

			//  5. create the skeleton record for and report it to the session, so the
			//      other interested collectors can use it as a seed
			ccsps->McprCommsId(mcprId);
			ccsps->McprPtr(mcpr);

			}

		CleanupStack::Pop(newAP); // give data to following function
		iReceiver->DataReceivedL(newAP);

		if(mcprRec)
			{
			CleanupStack::PopAndDestroy(mcprRec);
			}
		}

	CleanupStack::PopAndDestroy(apRecs);


	// This is a "one shot" synchronous collector, so is not waiting on any providers..
	//  so unlock session and go idle (implicitly in that nothing else will be calling this collector)
	iReceiver->Unlock();
	}


/*virtual*/ TBool CMCprFactoryQueryDataCollector::SetMatchesQuery(const CParameterSetContainer& aDestEntry)
	{
	// I've done my job... The other collectors can decide if the data matches..
	(void)aDestEntry;
	return ETrue;
	}



//
// CCprFactoryQueryDataCollector //
//

CCprFactoryQueryDataCollector::CCprFactoryQueryDataCollector(CTierNotificationActivity& aActivity)
	: CTierDataCollectorNode(aActivity)
	{
	LOG_NODE_CREATE(KTierTag, CCprFactoryQueryDataCollector);
	}


CCprFactoryQueryDataCollector::~CCprFactoryQueryDataCollector()
	{
	LOG_NODE_DESTROY(KTierTag, CCprFactoryQueryDataCollector);
	}


class TFactoryIteratorQuery : public Factories::MFactoryQuery
    {
public:
    TFactoryIteratorQuery(TUid aTM)
        :iTM(aTM),
         iSkip(0),
         iIndex(0)
        {
        }
    
    TFactoryIteratorQuery(TUid aTM, TInt aSkip)
        :iTM(aTM),
         iSkip(aSkip),
         iIndex(aSkip)
        {
        }

    TFactoryIteratorQuery& operator++()
        {
        iIndex = ++iSkip;
        return *this;
        }

protected:
    Factories::MFactoryQuery::TMatchResult Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo);

private:
    TUid iTM;
    TInt iSkip;
    TInt iIndex;
    };


Factories::MFactoryQuery::TMatchResult TFactoryIteratorQuery::Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo)
    {
    CMMCommsProviderBase& prov = static_cast<CMMCommsProviderBase&>(*aFactoryObjectInfo.iInfo.iFactoryObject);
    const TLayerConfig* layer = static_cast<const TLayerConfig*>(prov.AccessPointConfig().FindExtension(TLayerConfig::TypeId()));
    if (layer && layer->TierId() == iTM)
        {
        if (iIndex <= 0)
            {
            iIndex = iSkip;
            return Factories::MFactoryQuery::EMatch;
            }
        --iIndex;
        }
    return Factories::MFactoryQuery::EContinue;
    }

void CCprFactoryQueryDataCollector::DoStartL()
	{
	_TIER_LOG_2(_L8("\tColl -> CprFactQuery %08x:\t" "DoStartL()"),this);

	/*
	 * The collector collects all the MCPRs, for which CPRs exist. 
	 * Existing CPR denotes an active connection (started CPR denotes started connection).
	 */

	/*
	 * Iterate through existing MCPRs
	 */
	CMetaConnectionFactoryContainer& mcprfc=*SockManGlobals::Get()->iMetaConnectionFactories;
	CConnectionFactoryContainer& cprfc=*SockManGlobals::Get()->iConnectionFactories;

    CMetaConnectionProviderBase* mcpr=NULL;
	TFactoryIteratorQuery it(Activity().TierManager().TierId());
	// Find MCPR factory
	while ((mcpr = static_cast<CMetaConnectionProviderBase*>(mcprfc.Find(it))) != NULL)
		{
		++it;
		CConnectionProviderBase* cpr=NULL;
	
		TDefaultConnectionFactoryQuery cprquery(mcpr->Id());
		cpr = static_cast<CConnectionProviderBase*>(cprfc.Find(cprquery));

		if (!cpr)
			{
			_TIER_LOG_3(_L8("DoStartL(): Failed to find Cpr for ap %d (%x)."), (TUint)mcpr->Id().Ptr(), Activity().TierManager().TierId().iUid);
			continue;
			}

		/*
		 * Since we've got this far, a cpr exist, i.e. the conneciton is active
		 * Create skeleton record, with info we know. This won't be reported just
		 * yet, as IAP, and network id have to be filled in by CIPProtoAccessPointParameterCollector
		 */
		TInt apId = mcpr->ProviderInfo().APId();
		
		CParameterSetContainer* newAP = CParameterSetContainer::NewL(apId);
		CleanupStack::PushL(newAP);

		XAccessPointGenericParameterSet* newStatus = XAccessPointGenericParameterSet::NewL(*newAP);
		newStatus->AccessPointInfo(TAccessPointInfo(apId));
		TAccessPointStatus apStatus;
		apStatus.Configured(EAccessPointFlagTrue); // in commsdat so must be configured
		apStatus.Active(EAccessPointFlagTrue); // cpr exists, so this ap is active
		newStatus->AccessPointStatus(apStatus);

		XCoreCollectorSharedParameterSet* ccsps = XCoreCollectorSharedParameterSet::NewL(*newAP);

		TNodeId mcprId = mcpr->Id();
		__ASSERT_DEBUG(mcpr->FindClient(Id()) == NULL, User::Panic(KSpecAssert_ESockCrPrvTNotCC, 17));
        //  Join collector node to each one as a control client.. this ensures it stays alive til the other collectors
        //  join to it for their purposes
        mcpr->AddClientL(Id(), TClientType(TCFClientType::ECtrl, TCFClientType::EAdministrative));
        
        // .. and track the join from my side (for further processing / cleanup)
        TDataProviderStatus provider;
        provider.iProvider = mcprId;
        provider.iProviderPtr = mcpr;
        provider.iApId = TAccessPointInfo(apId);
        iProviders.AppendL(provider);
		
		ccsps->McprCommsId(mcprId);
		ccsps->McprPtr(mcpr);

		CleanupStack::Pop(newAP); // give data to following function
		iReceiver->DataReceivedL(newAP);
		}

	// This is a "one shot" synchronous collector, so is not waiting on any providers..
	//  so unlock session and go idle (implicitly in that nothing else will be calling this collector)
	iReceiver->Unlock();

	}


/*virtual*/ TBool CCprFactoryQueryDataCollector::SetMatchesQuery(const CParameterSetContainer& aDestEntry)
	{
	// I've done my job... The other collectors can decide if the data matches..
	(void)aDestEntry;
	return ETrue;
	}



//
// CFactoryNotifyDataCollector //
//

/*virtual*/ CFactoryNotifyDataCollector::~CFactoryNotifyDataCollector()
	{
	iRegisteredFactories.Close();
	}

/*virtual*/ void CFactoryNotifyDataCollector::DoStartL()
	{
	_TIER_LOG_2(_L8("\tColl -> FactNotify %08x:\t" "DoStartL()"),this);

	// This is a notification collector, so is not waiting on any providers..
	//  so unlock session and go idle (implicitly in that nothing else will be calling this collector)
	iReceiver->Unlock();


	//	find the factory for each MCPR type in this tier..
	//   it's not worth getting notifications from CPR factory because
	//    new CPRs will not have been provisioned i.e. no way to associate access point ID

	CMDBRecordSet<CCDAccessPointRecord>* apRecs;
	const XAccessPointGenericQuery* query = Query().FindGenericQuery();
	if(query)
	{
		apRecs = TierManagerUtils::LoadAccessPointRecordsL(Activity().TierManager().TierId(), query->AccessPointsToMatch(), Activity().GetDbSession());
	}
	else // get all records
	{
		apRecs = TierManagerUtils::LoadAccessPointRecordsL(Activity().TierManager().TierId(), Activity().GetDbSession());
	}
	CleanupStack::PushL(apRecs);

	TInt i=0;
	for( ; i < apRecs->iRecords.Count() ; ++i )
		{
		CCDAccessPointRecord* apRec = apRecs->operator[](i);
		TUid prUid;
		TRAPD(err, prUid = FindFactoryUidForApL(apRec));
		if(err || prUid == TUid::Null())
			{
			TInt apId = apRec->iRecordTag;
			_TIER_LOG_4(_L8("\tColl -> FactNotify %08x:\t" "DoStartL() - provider record not found for AP Id %d (err code %d) - skipping."), this,apId,err);
			continue;
			}

		if(KErrNotFound == iRegisteredFactories.Find(prUid))
			{
			CCommsFactoryBase* factory = NULL;
			TRAP(err, factory = FindOrCreateFactoryL(prUid));
			if(factory)
				{
				_TIER_LOG_3(_L8("\tColl -> FactNotify found a factory. registering for notifications on factory %d ( %08x ).."),prUid,prUid);
				IFactoryNotify intf(this, GetVTable());
				factory->RegisterNotifierL(intf);
				iRegisteredFactories.AppendL(prUid);
				}
			else
				{
				_TIER_LOG_3(_L8("\tColl -> FactNotify couldn't find factory for uid %d ( %08x ).."),prUid,prUid);
				continue;
				}
			}
		} // foreach AP record

	if(iRegisteredFactories.Count() == 0)
		{
		_TIER_LOG_2(_L8("\tColl -> FactNotify %08x:\t" "DoStartL() - no factories were found to register with!"),this);
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(apRecs);
	}


/*virtual*/ TBool CFactoryNotifyDataCollector::SetMatchesQuery(const CParameterSetContainer& /*aDestEntry*/)
	{
	// I'm just here to seed further queries..
	return ETrue;
	}



//
// CMcprFactoryNotifyDataCollector //
//

CMcprFactoryNotifyDataCollector::CMcprFactoryNotifyDataCollector(CTierNotificationActivity& aActivity)
	: CFactoryNotifyDataCollector(aActivity)
	{
	_TIER_LOG_2(_L8("CMcprFactoryNotifyDataCollector[%08x]:\tCMcprFactoryNotifyDataCollector()"), this);
	}


/*virtual*/ CMcprFactoryNotifyDataCollector::~CMcprFactoryNotifyDataCollector()
	{
	_TIER_LOG_2(_L8("CMcprFactoryNotifyDataCollector[%08x]:\t~CMcprFactoryNotifyDataCollector()"), this);
	}


const Factories::TAnyFn CMcprFactoryNotifyDataCollector::iInterfaceVTableF[] =
	{
	(Factories::TAnyFn)3, // number of methods. the following entries must be in this order!
	(Factories::TAnyFn)0, // Notification function (not implemented so nulled)
	(Factories::TAnyFn)0, // Deletion function (not implemented so nulled)
	(Factories::TAnyFn)(TFactoryNotify<CMcprFactoryNotifyDataCollector>::NotifyCreation),
	};


/*virtual*/ TUid CMcprFactoryNotifyDataCollector::FindFactoryUidForApL(CCDAccessPointRecord* aApRec)
	{
	// Find MCPR factory
	CCDMCprRecord* prRec=NULL;
	prRec = TierManagerUtils::LoadMCprRecordL(aApRec->iMCpr,Activity().GetDbSession());

	__ASSERT_DEBUG(prRec, User::Panic(KSpecAssert_ESockCrPrvTNotCC, 7));
	User::LeaveIfNull(prRec);
	TUid prUid = TUid::Uid(prRec->iMCprUid);
	delete prRec;
	return prUid;
	}

/*virtual*/ CCommsFactoryBase* CMcprFactoryNotifyDataCollector::FindOrCreateFactoryL(TUid aPrUid)
	{
	return static_cast<CCommsFactoryBase*>((*SockManGlobals::Get()->iMetaConnectionFactories).FindOrCreateFactoryL(aPrUid));
	}

TInt CMcprFactoryNotifyDataCollector::NotifyCreation(AFactoryObject& aObject, CFactoryBase& aFactory)
	{
	_TIER_LOG_2(_L8("\tColl -> McprFactNotify %08x:\t" "NotifyCreation()"),this);

	(void)aFactory;

	TAccessPointStatus apStatus;
	apStatus.Configured(EAccessPointFlagTrue); // for new entities not in existence at start of query
	TRAPD(err, ProcessNotificationL(aObject, apStatus));

	return err;
	}

void CMcprFactoryNotifyDataCollector::ProcessNotificationL(AFactoryObject& aObject,
															const TAccessPointStatus& aApStatus)
	{
	CMetaConnectionProviderBase& mcpr = static_cast<CMetaConnectionProviderBase&>(aObject);
	const TProviderInfo& providerInfo = static_cast<const TProviderInfoExt&>(mcpr.AccessPointConfig().FindExtensionL(
	        STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId))).iProviderInfo;
	TInt apId = providerInfo.APId();

	CParameterSetContainer* newData = CParameterSetContainer::NewL(apId);
	CleanupStack::PushL(newData);

	XAccessPointGenericParameterSet* newStatus = XAccessPointGenericParameterSet::NewL(*newData);
	newStatus->AccessPointInfo(TAccessPointInfo(apId));
	newStatus->AccessPointStatus(aApStatus);

	//  create the skeleton record for and report it to the session, so the
	//      other interested collectors can use it as a seed
	XCoreCollectorSharedParameterSet* ccsps = XCoreCollectorSharedParameterSet::NewL(*newData);
	ccsps->McprCommsId(mcpr.Id());
	ccsps->McprPtr(&mcpr);

	// pop the new access point record because we're about to pass ownership of it to DataReceivedL
	CleanupStack::Pop(newData);
	iReceiver->DataReceivedL(newData);
	}

void CMcprFactoryNotifyDataCollector::Shutdown()
	{
	_TIER_LOG_2(_L8("\tColl -> FactNotify %08x:\t" "Shutdown()"),this);
	for(TInt i = 0 ; i < iRegisteredFactories.Count() ; ++i )
		{
		CMetaConnectionProviderFactoryBase* factory=0;
		TRAPD(err, factory = static_cast<CMetaConnectionProviderFactoryBase*>((*SockManGlobals::Get()->iMetaConnectionFactories).FindOrCreateFactoryL(iRegisteredFactories[i])));
		__ASSERT_DEBUG(err == KErrNone && factory, User::Panic(KSpecAssert_ESockCrPrvTNotCC, 6));
		if(err == KErrNone && factory)
			{
			IFactoryNotify intf(this, GetVTable());
			factory->DeRegisterNotifier(intf);
			}
		}
	iReceiver->ShutdownFinished();
	}



//
// CCprFactoryNotifyDataCollector //
//
CCprFactoryNotifyDataCollector::CCprFactoryNotifyDataCollector(CTierNotificationActivity& aActivity)
	: CFactoryNotifyDataCollector(aActivity)
	{
	_TIER_LOG_2(_L8("CCprFactoryNotifyDataCollector[%08x]:\tCCprFactoryNotifyDataCollector()"), this);
	}


CCprFactoryNotifyDataCollector::~CCprFactoryNotifyDataCollector()
	{
	_TIER_LOG_2(_L8("CCprFactoryNotifyDataCollector[%08x]:\t~CCprFactoryNotifyDataCollector()"), this);
	}

const Factories::TAnyFn CCprFactoryNotifyDataCollector::iInterfaceVTableF[] =
	{
	(Factories::TAnyFn)3, // number of methods. the following entries must be in this order!
	(Factories::TAnyFn)0, // Notification function (not implemented so nulled)
    (Factories::TAnyFn)(TFactoryNotify<CCprFactoryNotifyDataCollector>::NotifyDeletion),
	(Factories::TAnyFn)(TFactoryNotify<CCprFactoryNotifyDataCollector>::NotifyCreation)
	};


TUid CCprFactoryNotifyDataCollector::FindFactoryUidForApL(CCDAccessPointRecord* aApRec)
	{
	// Find MCPR factory
	CCDCprRecord* prRec=NULL;
	prRec = TierManagerUtils::LoadCprRecordL(aApRec->iCpr, Activity().GetDbSession());

	__ASSERT_DEBUG(prRec, User::Panic(KSpecAssert_ESockCrPrvTNotCC, 9));
	User::LeaveIfNull(prRec);
	TUid prUid = TUid::Uid(prRec->iCprUid);
	delete prRec;
	return prUid;
	}

CCommsFactoryBase* CCprFactoryNotifyDataCollector::FindOrCreateFactoryL(TUid aPrUid)
	{
	return static_cast<CCommsFactoryBase*>((*SockManGlobals::Get()->iConnectionFactories).FindOrCreateFactoryL(aPrUid));
	}



TInt CCprFactoryNotifyDataCollector::NotifyCreation(AFactoryObject& aObject, CFactoryBase& /*aFactory*/)
	{
	_TIER_LOG_2(_L8("\tColl -> CprFactNotify %08x:\t" "NotifyCreation()"),this);

	TAccessPointStatus apStatus;
	apStatus.Active(EAccessPointFlagTrue);
	TRAPD(err, ProcessNotificationL(aObject,apStatus));

	return err;
	}

void CCprFactoryNotifyDataCollector::NotifyDeletion(AFactoryObject& aObject, CFactoryBase& /*aFactory*/)
	{
	_TIER_LOG_2(_L8("\tColl -> CprFactNotify %08x:\t" "NotifyDeletion()"),this);

	TAccessPointStatus apStatus;
	apStatus.Active(EAccessPointFlagFalse);
	TRAP_IGNORE(ProcessNotificationL(aObject,apStatus));
	}

void CCprFactoryNotifyDataCollector::ProcessNotificationL(AFactoryObject& aObject, const TAccessPointStatus& aAccessPointStatus)
	{
	CConnectionProviderBase& cpr = static_cast<CConnectionProviderBase&>(aObject);

// We have no context yet (no provision til later) so no way of knowing what to update.
//  so this assert will fail...
//  This makes this collector rather useless.

	RNodeInterface* mcpritf = cpr.ControlProvider();
	ASSERT(mcpritf);
	CMetaConnectionProviderBase* mcpr = static_cast<CMetaConnectionProviderBase*>(reinterpret_cast<Messages::ASimpleNodeIdBase*>(
			mcpritf->RecipientId().Ptr()));

	TInt apId = mcpr->ProviderInfo().APId();

	CParameterSetContainer* newData = CParameterSetContainer::NewL(apId);
	CleanupStack::PushL(newData);

	XAccessPointGenericParameterSet* newStatus = XAccessPointGenericParameterSet::NewL(*newData);
	newStatus->AccessPointInfo(TAccessPointInfo(apId));
	newStatus->AccessPointStatus(aAccessPointStatus);

	/* We know the Cpr is changes, but don't know what IAP for. Create parameter set for that
	   which will be filled in magically before reporting */
	XCoreCollectorSharedParameterSet* ccsps = XCoreCollectorSharedParameterSet::NewL(*newData);
	ccsps->McprCommsId(mcpritf->RecipientId());
	ccsps->McprPtr(mcpr);

	// pop the new access point record because we're about to pass ownership of it to DataReceivedL
	CleanupStack::Pop(newData);
	iReceiver->DataReceivedL(newData);
	}

TBool CCprFactoryNotifyDataCollector::SetMatchesQuery(const CParameterSetContainer& aDestEntry)
	{
	const XAccessPointGenericParameterSet* apgps = XAccessPointGenericParameterSet::FindInParamSetContainer(aDestEntry);
	if(apgps)
		{
		const XAccessPointGenericQuery* apgq = Query().FindGenericQuery();
		if(apgq)
			{
			const TAccessPointStatus& aps = apgps->AccessPointStatus();
			const TAccessPointStatusFilter& apsf = apgq->AccessPointStatusFilter();

			if(	TAccessPointStatusFilter::Matches(apsf.Active(),aps.Active()) )
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

void CCprFactoryNotifyDataCollector::MergeL(CParameterSetContainer& aCacheEntry, CParameterSetContainer& aNewData,
								TBool& aInSet, TBool& aToReport)
	{
	CFactoryNotifyDataCollector::MergeL(aCacheEntry, aNewData, aInSet, aToReport);
	aToReport = ETrue;
	}

void CCprFactoryNotifyDataCollector::Shutdown()
	{
	_TIER_LOG_2(_L8("\tColl -> FactNotify %08x:\t" "Shutdown()"),this);
	for(TInt i = 0 ; i < iRegisteredFactories.Count() ; ++i )
		{
		CConnectionProviderFactoryBase* factory=0;
		TRAPD(err, factory = static_cast<CConnectionProviderFactoryBase*>((*SockManGlobals::Get()->iConnectionFactories).FindOrCreateFactoryL(iRegisteredFactories[i])));
		__ASSERT_DEBUG(err == KErrNone && factory, User::Panic(KSpecAssert_ESockCrPrvTNotCC, 16));
		if(err == KErrNone && factory)
			{
			IFactoryNotify intf(this, GetVTable());
			factory->DeRegisterNotifier(intf);
			}
		}
	iReceiver->ShutdownFinished();
	}

//
// CAvailabilityDataCollector //
//

CAvailabilityDataCollector::CAvailabilityDataCollector(CTierNotificationActivity& aActivity)
	: CTierInfoCollectorNode(aActivity)
	{
	LOG_NODE_CREATE(KTierTag, CAvailabilityDataCollector);
	}

/*virtual*/ CAvailabilityDataCollector::~CAvailabilityDataCollector()
	{
	LOG_NODE_DESTROY(KTierTag, CActiveStatusDataCollector);
	}


void CAvailabilityDataCollector::SubscribeWithProviderL(TDataProviderStatus& aProvider)
	{
	_TIER_LOG_2(_L8("\tColl ->Avail %08x:\t" "SubscribeWithProviderL()"),this);
	TAvailabilitySubscriptionOptions avSubOpt;

#ifndef __GCCXML__
	/* there's only one activity running on this data collector node which we'll set to 101 */

	//TODO - 101? Why not a proper enum to avoid clash and lenghty debugging?
	TCFAvailabilityProvider::TAvailabilityNotificationRegistration msg(avSubOpt);

	RClientInterface::OpenPostMessageClose(Id(), aProvider.iProvider, msg);
#endif
	}


TBool CAvailabilityDataCollector::SetMatchesQuery(const CParameterSetContainer& aDestEntry)
	{
	const XAccessPointGenericParameterSet* apgps = XAccessPointGenericParameterSet::FindInParamSetContainer(aDestEntry);
	if(apgps)
		{
		const XAccessPointGenericQuery* apgq = Query().FindGenericQuery();
		if(apgq)
			{
			const TAccessPointStatus& aps = apgps->AccessPointStatus();
			const TAccessPointStatusFilter& apsf = apgq->AccessPointStatusFilter();

			if(	TAccessPointStatusFilter::Matches(apsf.Available(),aps.Available()) )
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}



void CAvailabilityDataCollector::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	_TIER_LOG_2(_L8("\tColl ->Avail %08x:\t" "ReceivedL()"),this);

	if(aMessage.IsMessage<TCFAvailabilityControlClient::TAvailabilityNotification>() ||
	   ( (!iShuttingDown_Cancelling) && aMessage.IsMessage<TEBase::TError>() )
		)
		{
		if(iShuttingDown_Cancelling && aMessage.IsMessage<TCFAvailabilityControlClient::TAvailabilityNotification>())
			{
			// swallow it..
			return;
			}

		const TNodeId& incomingId = address_cast<TNodeId>(aSender);

		TDataProviderStatus* status = FindDataProviderStatus(incomingId);
		__ASSERT_DEBUG(status, User::Panic(KSpecAssert_ESockCrPrvTNotCC, 11));
		TAccessPointInfo apInfo = status->iApId;
		TInt apId = apInfo.AccessPoint();

		CParameterSetContainer* newAP = CParameterSetContainer::NewL(apId);
		CleanupStack::PushL(newAP);

		XAccessPointGenericParameterSet* newStatus = XAccessPointGenericParameterSet::NewL(*newAP);

		newStatus->AccessPointInfo(apInfo);


		TAccessPointFlagValue availFlag = EAccessPointFlagUnknown;

		TCFAvailabilityControlClient::TAvailabilityNotification* inAvailMsg =
			message_cast<TCFAvailabilityControlClient::TAvailabilityNotification>(&aMessage);
		if(inAvailMsg)
			{
//			ASSERT(apId == inAvailMsg->iValue1);

//			TAvailabilityStatus incomingAv(inAvailMsg->iAvailabilityStatus);

			if (inAvailMsg->iAvailabilityStatus.Score())
				{
				availFlag = EAccessPointFlagTrue;
				}
			else
				{
				availFlag = EAccessPointFlagFalse;
				}
			}
		// otherwise was an error.. so leave as unknown but call DataReceivedL to acknowledge
		//  we have a reply. To save further code and complexity we'll just wait til shutdown to clean up

		TAccessPointStatus apStatus;
		apStatus.Available(availFlag);

		newStatus->AccessPointStatus(apStatus);

		// pop the new access point record because we're about to pass ownership of it to DataReceivedL
		CleanupStack::Pop(newAP);
		iReceiver->DataReceivedL(newAP);
		// this may trigger the last "received" flag, thus enabling reporting of data
		}
	else
		{
		CTierInfoCollectorNode::ReceivedL(aSender, aRecipient, aMessage);
    	}
	}





//
// CActiveStatusDataCollector //
//

CActiveStatusDataCollector::CActiveStatusDataCollector(CTierNotificationActivity& aActivity)
	: CTierInfoCollectorNode(aActivity)
	{
	LOG_NODE_CREATE(KTierTag, CActiveStatusDataCollector);
	}

/*virtual*/ CActiveStatusDataCollector::~CActiveStatusDataCollector()
	{
	LOG_NODE_DESTROY(KTierTag, CActiveStatusDataCollector);
	}


void CActiveStatusDataCollector::SubscribeWithProviderL(TDataProviderStatus& aProvider)
	{
	_TIER_LOG_2(_L8("\tColl ->ActStat %08x:\t" "SubscribeWithProviderL()"),this);
#ifndef __GCCXML__
	/* there's only one activity running on this data collector node which we'll set to 101 */
	TCFMcpr::TProviderStatusChangeRegistration msg;
	RClientInterface::OpenPostMessageClose(Id(), aProvider.iProvider, msg);
#endif
	}


TBool CActiveStatusDataCollector::SetMatchesQuery(const CParameterSetContainer& aDestEntry)
	{
	const XAccessPointGenericParameterSet* apgps = XAccessPointGenericParameterSet::FindInParamSetContainer(aDestEntry);
	if(apgps)
		{
		const XAccessPointGenericQuery* apgq = Query().FindGenericQuery();
		if(apgq)
			{
			const TAccessPointStatus& aps = apgps->AccessPointStatus();
			const TAccessPointStatusFilter& apsf = apgq->AccessPointStatusFilter();

			if(	TAccessPointStatusFilter::Matches(apsf.Started(),aps.Started()) )
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}


void CActiveStatusDataCollector::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	_TIER_LOG_2(_L8("\tColl ->ActStat %08x:\t" "ReceivedL()"),this);

	if(aMessage.IsMessage<TCFMcpr::TProviderStatusChange>() ||
	   ( (!iShuttingDown_Cancelling) && aMessage.IsMessage<TEBase::TError>() )
		)
		{
		if(iShuttingDown_Cancelling && aMessage.IsMessage<TCFMcpr::TProviderStatusChange>())
			{
			// swallow it..
			return;
			}

		TDataProviderStatus* status = FindDataProviderStatus(address_cast<TNodeId>(aSender));
		__ASSERT_DEBUG(status, User::Panic(KSpecAssert_ESockCrPrvTNotCC, 12));
		TAccessPointInfo apInfo = status->iApId;
		TInt apId = apInfo.AccessPoint();

		CParameterSetContainer* newAP = CParameterSetContainer::NewL(apId);
		CleanupStack::PushL(newAP);

		XAccessPointGenericParameterSet* newStatus = XAccessPointGenericParameterSet::NewL(*newAP);

		newStatus->AccessPointInfo(apInfo);


		TAccessPointFlagValue startedFlag = EAccessPointFlagUnknown;

		TCFMcpr::TProviderStatusChange* inPscMsg =
			message_cast<TCFMcpr::TProviderStatusChange>(&aMessage);
		if(inPscMsg)
			{

			TUint incomingStatusValue = inPscMsg->iValue;

			switch(incomingStatusValue)
				{
				case TCFMcpr::EStopped:
					startedFlag = EAccessPointFlagFalse;
					break;
				case TCFMcpr::EStarted:
					startedFlag = EAccessPointFlagTrue;
					break;
				default:
					CleanupStack::PopAndDestroy(); // newAP
					return;
				}
			}
		// otherwise was an error.. so leave as unknown but call DataReceivedL to acknowledge
		//  we have a reply. To save further code and complexity we'll just wait til shutdown to clean up

		TAccessPointStatus apStatus;
		apStatus.Started(startedFlag);

		newStatus->AccessPointStatus(apStatus);

		// pop the new access point record because we're about to pass ownership of it to DataReceivedL
		CleanupStack::Pop(newAP);
		iReceiver->DataReceivedL(newAP);
		// this may trigger the last "received" flag, thus enabling reporting of data
		}
	else
		{
		CTierInfoCollectorNode::ReceivedL(aSender, aRecipient, aMessage);
    	}
	}

//
// CAccessPointPlaneStatusCollector //
//

CAccessPointPlaneStatusCollector::CAccessPointPlaneStatusCollector(CTierNotificationActivity& aActivity)
	: CTierDataCollector(aActivity)
	{
//	LOG_NODE_CREATE(KTierTag, CAccessPointPlaneStatusCollector);
	}

/*virtual*/ CAccessPointPlaneStatusCollector::~CAccessPointPlaneStatusCollector()
	{
//	LOG_NODE_DESTROY(KTierTag, CAccessPointPlaneStatusCollector);
	}


/*virtual*/ void CAccessPointPlaneStatusCollector::DoStartL()
	{
	_TIER_LOG_2(_L8("\tColl (AP plane status)  %08x:\t" "DoStartL()"),this);
	// do nothing.. wait for AP records to come in from the other collector(s)
	//  so we don't want to block anything..
	iReceiver->Unlock();
	}


/*virtual*/ void CAccessPointPlaneStatusCollector::CacheModifiedL(CParameterSetContainer& aModifiedEntry, TInt& /*aIsRunning*/ /*output*/)
	{
	_TIER_LOG_2(_L8("\tColl (AP plane status)  %08x:\t" "CacheModifiedL()"),this);
	// ok a new record may have come in.. let's see if we know about it

	const XCoreCollectorSharedParameterSet* ccsps = XCoreCollectorSharedParameterSet::FindInParamSetContainer(aModifiedEntry);
	if(ccsps)
		{
		CMetaConnectionProviderBase* mcpr = static_cast<CMetaConnectionProviderBase*>(ccsps->McprPtr());
		if(mcpr != NULL)
			{
			XAccessPointPlaneStatusParameterSet* appsps = XAccessPointPlaneStatusParameterSet::FindInParamSetContainer(aModifiedEntry);

			TAccessPointFlagValue hasCpr =
					mcpr->CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData)) ?
					EAccessPointFlagTrue : EAccessPointFlagFalse;

//			TAccessPointFlagValue startedCpr =
//					mcpr->CountClients<TDefaultClientMatchPolicy>(RNodeInterface::EData | RNodeInterface::EStarted) ?
//					EAccessPointFlagTrue : EAccessPointFlagFalse;

			if(!appsps
				|| appsps->PlaneStatus().Connection_Exists() != hasCpr
				/*|| appsps->PlaneStatus().Connection_Started() != startedCpr*/)
				{
				CParameterSetContainer* newData = ConstructEmptyRecordLC(aModifiedEntry.Id());
				XAccessPointPlaneStatusParameterSet* newappsps = XAccessPointPlaneStatusParameterSet::FindInParamSetContainer(*newData);

				TAccessPointPlaneStatus apPs;
				apPs.Connection_Exists(hasCpr);
//				apPs.Connection_Started(startedCpr);
				newappsps->PlaneStatus(apPs);

				CleanupStack::Pop(newData);
				iReceiver->DataReceivedL(newData);
				}
			}
		else
			{
			// eek. we can only assume we couldn't fetch all the data.. so at least mark this as done by creating
			//   a record full of unknowns
			// may be able to optimise this by just setting flag to running, received, !tonotify, !inset
			CParameterSetContainer* emptyRecord = ConstructEmptyRecordLC(aModifiedEntry.Id());

			CleanupStack::Pop(emptyRecord);
			iReceiver->DataReceivedL(emptyRecord,EFalse); // EFalse prevents infinite recursion
			}
		}
	}


/*virtual*/ CParameterSetContainer* CAccessPointPlaneStatusCollector::ConstructEmptyRecordLC(TUint32 aId /*=0*/)
	{
	_TIER_LOG_2(_L8("\tColl (AP plane status) %08x:\t" "ConstructEmptyRecordLC()"),this);

	CParameterSetContainer* aEmptyInstance = CParameterSetContainer::NewL(aId);
	CleanupStack::PushL(aEmptyInstance);

	XAccessPointPlaneStatusParameterSet::NewL(*aEmptyInstance);

	return aEmptyInstance;
	}


TBool CAccessPointPlaneStatusCollector::SetMatchesQuery(const CParameterSetContainer& aDestEntry)
	{
	// we're only the owner of XAccessPointPlaneStatusParameterSet.. the other stuff is just to
	//  help other collectors

	const XAccessPointPlaneStatusParameterSet* appsps = XAccessPointPlaneStatusParameterSet::FindInParamSetContainer(aDestEntry);
	if(appsps)
		{
		const XAccessPointPlaneStatusQuery* apgq = XAccessPointPlaneStatusQuery::FindInBundle(Query());
		if(apgq)
			{
			const TAccessPointPlaneStatus& aps = appsps->PlaneStatus();
			const TAccessPointPlaneStatusFilter& apsf = apgq->PlaneStatusFilter();

			return TAccessPointStatusFilter::Matches(apsf.Connection_Exists(),aps.Connection_Exists()) ;
			}
		}
 	return ETrue;
	}




//
// CIPProtoAccessPointParameterCollector //
//

CIPProtoAccessPointParameterCollector::CIPProtoAccessPointParameterCollector(CTierNotificationActivity& aActivity)
	: CTierDataCollector(aActivity)
	{
//	LOG_NODE_CREATE(KTierTag, CIpProtoAccessPointParameterCollector);
	}

/*virtual*/ CIPProtoAccessPointParameterCollector::~CIPProtoAccessPointParameterCollector()
	{
//	LOG_NODE_DESTROY(KTierTag, CIpProtoAccessPointParameterCollector);
	}


/*virtual*/ void CIPProtoAccessPointParameterCollector::DoStartL()
	{
	_TIER_LOG_2(_L8("\tColl (IP proto AP param)  %08x:\t" "DoStartL()"),this);
	// do nothing.. wait for AP records to come in from the other collector(s)
	//  so we don't want to block anything..
	iReceiver->Unlock();
	}



/*virtual*/ void CIPProtoAccessPointParameterCollector::CacheModifiedL(CParameterSetContainer& aModifiedEntry, TInt& /*aIsRunning*/ /*output*/)
	{
	_TIER_LOG_2(_L8("\tColl (IP proto AP param)  %08x:\t" "CacheModifiedL()"),this);
	// ok a new record may have come in.. let's see if we know about it

	const XIpProtoAccessPointParameterSet* ipapps = XIpProtoAccessPointParameterSet::FindInParamSetContainer(aModifiedEntry);
	if(ipapps && ipapps->IapId())
		{
		// if known and set, don't try again..
		return;
		}

	// otherwise try to find it out / populate it.

	const XCoreCollectorSharedParameterSet* ccsps = XCoreCollectorSharedParameterSet::FindInParamSetContainer(aModifiedEntry);
	if(ccsps == NULL)
		{
		return; // no shared parameters, can't do anything with it
		}
	
	CParameterSetContainer* newData = ConstructEmptyRecordLC(aModifiedEntry.Id());
	XIpProtoAccessPointParameterSet* newipapps = XIpProtoAccessPointParameterSet::FindInParamSetContainer(*newData);
	
	CMetaConnectionProviderBase* mcpr = static_cast<CMetaConnectionProviderBase*>(ccsps->McprPtr());
	if(mcpr == NULL)
		{
		CleanupStack::PopAndDestroy(newData);
		return; 
		}

	TInt apId = mcpr->ProviderInfo().APId();
	CCommsDatIapView* iapView = CCommsDatIapView::NewL(apId);
	CleanupStack::PushL(iapView);
	
	newipapps->IapId(iapView->IapId());
	TUint32 netId;
	iapView->GetIntL(KCDTIdIAPNetwork,netId);
	newipapps->NetworkId(netId);
	newipapps->ConnectionType(EConnectionGeneric);

	Messages::TNodeId dcId;
	TClientIter<TDefaultClientMatchPolicy> iter = mcpr->GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData));
	if(iter[0])
		{
		dcId = iter[0]->RecipientId();
		}
	newipapps->ControlClientId(dcId);

	CleanupStack::PopAndDestroy(iapView);
	CleanupStack::Pop(newData);
	
	// if found valid data, submit it..
	// otherwise just ensure unknowns are added
	if(newipapps->IapId())
		{
		iReceiver->DataReceivedL(newData);
		}
	else
		{
		if(ipapps == NULL)
			{
			// make sure unknowns are set in cache
			iReceiver->DataReceivedL(newData,EFalse); // EFalse prevents infinite recursion
			}
		else
			{
			// unknown before, unknown now => no change
			delete newData;
			}
		}
	}


/*virtual*/ CParameterSetContainer* CIPProtoAccessPointParameterCollector::ConstructEmptyRecordLC(TUint32 aId /*=0*/)
	{
	_TIER_LOG_2(_L8("\tColl (IP proto AP param) %08x:\t" "ConstructEmptyRecordLC()"),this);

	CParameterSetContainer* aEmptyInstance = CParameterSetContainer::NewL(aId);
	CleanupStack::PushL(aEmptyInstance);

	XIpProtoAccessPointParameterSet::NewL(*aEmptyInstance);

	return aEmptyInstance;
	}


TBool CIPProtoAccessPointParameterCollector::SetMatchesQuery(const CParameterSetContainer& /*aDestEntry*/)
	{
	// we're just collecting..
 	return ETrue;
	}



