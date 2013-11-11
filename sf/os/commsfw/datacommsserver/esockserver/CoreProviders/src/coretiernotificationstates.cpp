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

#include <comms-infras/cs_connservparams.h>
#include <comms-infras/cs_connservparams_internal.h>
#include <comms-infras/es_connectionservparameterbundle.h>
#include <comms-infras/ss_nodemessages.h>
#include "coretiernotificationstates.h"
#include <comms-infras/coretiernotificationactivity.h>
#include <comms-infras/coretiernotificationcollectors.h>
#include <comms-infras/ss_nodemessages_tiermanager.h>

#include <elements/nm_messages_child.h>

#include <comms-infras/es_connectionservparameterbundletrace.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
// UNCOMMENT IF EVER ADDING AN ASSERT
//_LIT(KSpecAssert_ESockCrPrvTNotSC, "ESockCrPrvTNotSC");
#endif


using namespace ESock;
using namespace TMStates;
using namespace ConnectionServ;
using namespace TierNotification;
using namespace Messages;


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
// Tier Notification/Query - state
//
EXPORT_DEFINE_SMELEMENT(TAwaitingTierNotificationRegistration, NetStateMachine::MState, TierNotification::TContext)
EXPORT_C TBool TAwaitingTierNotificationRegistration::Accept()
	{
	if(	iContext.iMessage.IsMessage<TCFTierStatusProvider::TTierStatusQuery>() ||
		iContext.iMessage.IsMessage<TCFTierStatusProvider::TTierNotificationRegistration>() )
		{
		return ETrue;
		}

	// after the activity has started we'll re-read the message id to determine whether it's
	//   an ongoing notification session or just a one-off query
	return EFalse;
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingDataCollectors, NetStateMachine::MState, TContext)
EXPORT_C TBool TAwaitingDataCollectors::Accept()
	{
	return iContext.iMessage.IsMessage<TCFDataCollector::TTierDataCollectors>();
	}


EXPORT_DEFINE_SMELEMENT(TAwaitingCancelOrErrorOrDestroy, NetStateMachine::MState, TierNotification::TContext)
EXPORT_C TBool TAwaitingCancelOrErrorOrDestroy::Accept()
	{
	return(	( iContext.iMessage.IsMessage<TEBase::TCancel>() && iContext.Activity()->FindOriginator(iContext.iSender) != KErrNotFound ) ||
			iContext.iMessage.IsMessage<TEBase::TError>() ||
			iContext.iMessage.IsMessage<TEChild::TDestroy>() );
	}

EXPORT_DEFINE_SMELEMENT(TAwaitingDestroy, NetStateMachine::MState, TContext)
EXPORT_C TBool TAwaitingDestroy::Accept()
	{
	if(iContext.iMessage.IsMessage<TEBase::TCancel>() ||
		iContext.iMessage.IsMessage<TEBase::TError>() )
		{
		// swallow it...
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}
		return iContext.iMessage.IsMessage<TEChild::TDestroy>();
	}


//
// Tier Notification/Query - transition
//
EXPORT_DEFINE_SMELEMENT(TInitTierNotifications, NetStateMachine::MStateTransition, TierNotification::TContext)
EXPORT_C void TInitTierNotifications::DoL()
	{
	const RMessage2* platSecInfo;
	TBool oneOffQuery;
	CRefCountOwnedParameterBundle* bundleOwner;

	if(iContext.iMessage.IsMessage<TCFTierStatusProvider::TTierStatusQuery>())
		{
		TCFTierStatusProvider::TTierStatusQuery& inMsg = message_cast<TCFTierStatusProvider::TTierStatusQuery>(iContext.iMessage);
		platSecInfo = inMsg.iMessage;
		bundleOwner = inMsg.iBundle;
		oneOffQuery = ETrue;
		}
	else
		{
		TCFTierStatusProvider::TTierNotificationRegistration& inMsg = message_cast<TCFTierStatusProvider::TTierNotificationRegistration>(iContext.iMessage);
		platSecInfo = inMsg.iMessage;
		bundleOwner = inMsg.iBundle;
		oneOffQuery = EFalse;
		}

	CTierNotificationActivity* act = static_cast<CTierNotificationActivity*>(iContext.iNodeActivity);

	// my custom InitL (sets up the vars for the activity)
	act->InitL(bundleOwner, *platSecInfo, oneOffQuery);

	// NOTE: We deliberately do not close the reference to the bundle since
	// the activity will now own it

	// fetch collectors before we leave mesh land..
	RClientInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
		iContext.NodeId(), TCFDataCollector::TTierGetDataCollectors().CRef());
	}


EXPORT_DEFINE_SMELEMENT(TStartTierNotifications, NetStateMachine::MStateTransition, TierNotification::TContext)
EXPORT_C void TStartTierNotifications::DoL()
	{
	TCFDataCollector::TTierDataCollectors& inMsg =
				message_cast<TCFDataCollector::TTierDataCollectors>(iContext.iMessage);

	RPointerArray<MDataCollector>* collrs = static_cast< RPointerArray<MDataCollector>*> (inMsg.iPtr);
	CleanupStack::PushL(collrs); // We take ownership of the collector array
	CleanupResetAndDestroyPushL(*collrs);  // Note that RPointerArray<> doesn't destroy contained items at destruction so this cleanup item is needed

	CTierNotificationActivity* act = static_cast<CTierNotificationActivity*>(iContext.iNodeActivity);
	// my custom StartL (sets the activity going)
	act->StartL( *collrs );

	CleanupStack::Pop();
	collrs->Close(); // don't delete contained collectors - they belong to activity now.
	CleanupStack::PopAndDestroy(collrs);
	}




EXPORT_DEFINE_SMELEMENT(TInitiateTierNotificationCancel, NetStateMachine::MStateTransition, TierNotification::TContext)
EXPORT_C void TInitiateTierNotificationCancel::DoL()
	{
	CTierNotificationActivity* act = static_cast<CTierNotificationActivity*>(iContext.iNodeActivity);
	if(act->Error() == KErrNone)
		{ // if we're not shutting down for an error case we must be cancelling...
		act->SetError(KErrCancel);
		}
	act->InitiateShutdown(KErrCancel);
	}




//
// Tier Notification/Query - selection of data collectors..
//  This should be the only part that needs to be overridden - the core engine
//   should be sufficient to coordinate the queries
//
EXPORT_DEFINE_SMELEMENT(TGetDataCollectors, NetStateMachine::MStateTransition, TierNotification::TContext)
EXPORT_C void TGetDataCollectors::DoL()
	{
	// we are just using "FindAddressedActivity" here to locate the availability activity by its act#
	TUint sendersActivityId = address_cast<TNodeCtxId>(iContext.iSender).NodeCtx(); //The sender is always CTierNotificationActivity
   	CTierNotificationActivity* act = static_cast<CTierNotificationActivity*>(
											iContext.Node().FindActivityById(sendersActivityId) );

	if(!act)
		{
		// activity has gone away (presumably been aborted), hence replying is pointless..
		return;
		}


	// N.B. the selection of the appropriate collectors is purely a manouevre to
	//    reduce the amount of work performed by the query.
	//
	//  If in doubt of this algorithm you should be able to turn them all on and your
	//  result will look the same. Of course it will be slower.

	TBool queryAndWatchCommsdat(EFalse);
	TBool queryMcprFactories(EFalse);
	TBool queryMcprFactoriesFindDontCreate(EFalse);
	TBool watchMcprFactories(EFalse);
	TBool queryCprFactories(EFalse);
	TBool watchCprFactories(EFalse);
	TBool watchAvailabilityStatus(EFalse);
	TBool watchStartedStatus(EFalse);
	TBool apPlaneStatusCollector(EFalse);
	TBool ipProtoApParamCollector(EFalse);

	act->QueryBundleOwner().Open(); // get a ref on it while we use it

	CleanupClosePushL(act->QueryBundleOwner()); // ensure we Close the ref if we leave
	const CConnectionServParameterBundle& queryBundle = static_cast<const CConnectionServParameterBundle&>(*(act->QueryBundleOwner().PtrL()));
	TBool oneOffQuery = act->OneOffQuery();


	RPointerArray<MDataCollector>* collectors = new(ELeave) RPointerArray<MDataCollector>;
	CleanupStack::PushL(collectors);
	CleanupResetAndDestroyPushL(*collectors);

	const XAccessPointGenericQuery* query = queryBundle.FindGenericQuery();
	if(query)
		{
		const TAccessPointStatusFilter& filter = query->AccessPointStatusFilter();

		//
		// 1. Access point collectors. These implement DoStartL to go and find access point instances. //
		//           and feed them to the parameter collectors below                                   //
		//

		// the below block is to create access point collectors which are needed for mesh-based parameter queries
		//
		if(	filter.Available() != EAccessPointFlagIgnore ||
			filter.Started()   != EAccessPointFlagIgnore )
			{
			queryMcprFactories = ETrue;

			if( filter.Started() == EAccessPointFlagMatchTrue /* || !oneOffQuery */ )
				{
				//  if we're only scanning for started connections it'll use Find (i.e. not FindOrCreate) so we
				//   don't go to loads of effort creating shedloads of MCPRs
				queryMcprFactoriesFindDontCreate=ETrue;
				}

			if( ! oneOffQuery)
				{
				watchMcprFactories=ETrue;
				}
			}

		if (filter.Active() != EAccessPointFlagIgnore)
			{
			queryCprFactories = ETrue;

			if (!oneOffQuery)
				{
				watchCprFactories=ETrue;
				}
			}

		//
		// 2. Parameter collectors. These implement CacheModifiedL to enable the above providers to    //
		//           steer them towards the right places to find their information                     //
		//

		if(	filter.Available() != EAccessPointFlagIgnore )
			{
			watchAvailabilityStatus=ETrue;
			}

		if(	filter.Started() != EAccessPointFlagIgnore )
			{
			watchStartedStatus=ETrue;
			}

		if(	(collectors->Count() == 0  &&  filter.Configured() != EAccessPointFlagIgnore)
					// i.e. nobody else found out configured state for us
			 ||
			filter.Restricted() != EAccessPointFlagIgnore )
			{
			queryAndWatchCommsdat=ETrue;
			}

		} // if(query)

	const XAccessPointPlaneStatusQuery* psq = XAccessPointPlaneStatusQuery::FindInBundle(queryBundle);
	if(psq)
		{
		if( ! oneOffQuery)
			{
			// not supporting notifications of these attribues yet
			User::Leave(KErrNotSupported);
			}

		const TAccessPointPlaneStatusFilter& psf = psq->PlaneStatusFilter();
		if(psf.Connection_Exists() != EAccessPointFlagIgnore)
			{
			apPlaneStatusCollector=ETrue;
			queryMcprFactories=ETrue;
			if(psf.Connection_Exists() == EAccessPointFlagMatchTrue)
				{
				//  if we're only scanning for started connections it'll use Find (i.e. not FindOrCreate) so we
				//   don't go to loads of effort creating shedloads of MCPRs
				queryMcprFactoriesFindDontCreate=ETrue;
				}
			}
		} // if(XAccessPointPlaneStatusQuery)

	if(query && query->ShouldReturnType(XIpProtoAccessPointParameterSet::Type()))
		{
		ipProtoApParamCollector = ETrue;
		}



	// ok now we know what we need, let's create it..
	if(queryAndWatchCommsdat)
		{
		// does an initial one-off read, then ongoing notification of new records in commsdat.
		//  also updates the "restricted" field if it's requested in the query
		_TIER_LOG(_L8("TGetDataCollectors:\tusing CCommsDatDataCollector"));
		CCommsDatDataCollector* newCDC = CDataCollectorFactory<CCommsDatDataCollector>::AddNewToArrayL(*act,*collectors);
		// rjl: can probably optimise this out be retrieving Restricted flag during factory query instead.
		//  however it will still be necessary to ensure the commsdat change notification works.
		}

	if(queryMcprFactories)
		{
		// does a one-off scan of providers via factory (synchronous currently, may change)..
		_TIER_LOG(_L8("TGetDataCollectors:\tusing CFactoryQueryDataCollector"));
		CMCprFactoryQueryDataCollector* newFQC = CDataCollectorFactory<CMCprFactoryQueryDataCollector>::AddNewToArrayL(*act,*collectors);

		if(	queryMcprFactoriesFindDontCreate)
			{
			newFQC->FindDontCreate(ETrue);
			}
		}

	if(watchMcprFactories)
		{
		// watches creation/deletion events occurring on factories.. so only applicable to ongoing notification sessions, not one-off query
		_TIER_LOG(_L8("TGetDataCollectors:\tusing CMcprFactoryNotifyDataCollector"));
		CDataCollectorFactory<CMcprFactoryNotifyDataCollector>::AddNewToArrayL(*act,*collectors);
		}

	if(queryCprFactories)
		{
		CDataCollectorFactory<CCprFactoryQueryDataCollector>::AddNewToArrayL(*act,*collectors);
		}

	if(watchCprFactories)
		{
		// watches creation/deletion events occurring on factories.. so only applicable to ongoing notification sessions, not one-off query
		_TIER_LOG(_L8("TGetDataCollectors:\tusing CMcprFactoryNotifyDataCollector"));
		CDataCollectorFactory<CCprFactoryNotifyDataCollector>::AddNewToArrayL(*act,*collectors);
		}

	if( watchAvailabilityStatus )
		{
		// collects availability from MCPRs. which is where availability is supposed to come from as it may depend on the config of the
		//  particular MCPR... or require looking at its service providers.
		_TIER_LOG(_L8("TGetDataCollectors:\tusing CAvailabilityDataCollector"));
		CAvailabilityDataCollector* newAC = CDataCollectorFactory<CAvailabilityDataCollector>::AddNewToArrayL(*act,*collectors);
		}

	if(	watchStartedStatus )
		{
		// collects active status from MCPRs
		_TIER_LOG(_L8("TGetDataCollectors:\tusing CActiveStatusDataCollector"));
		CActiveStatusDataCollector* newASC = CDataCollectorFactory<CActiveStatusDataCollector>::AddNewToArrayL(*act,*collectors);
		}


	if( apPlaneStatusCollector )
		{
		// collects plane status from whoever, driven by various events
		_TIER_LOG(_L8("TGetDataCollectors:\tusing CAccessPointPlaneStatusCollector"));
		CAccessPointPlaneStatusCollector* newAPPSC = CDataCollectorFactory<CAccessPointPlaneStatusCollector>::AddNewToArrayL(*act,*collectors);
		}


	if( ipProtoApParamCollector )
		{
		// collects ip proto info where it can
		_TIER_LOG(_L8("TGetDataCollectors:\tusing CIpProtoAccessPointParameterCollector"));
		CIPProtoAccessPointParameterCollector* newIp = CDataCollectorFactory<CIPProtoAccessPointParameterCollector>::AddNewToArrayL(*act,*collectors);
		}

	if(collectors->Count() == 0)
		{
		User::Leave(KErrArgument);
		}

    CleanupStack::Pop();
	CleanupStack::Pop(collectors);
	CleanupStack::PopAndDestroy(); // Close the ref to the bundle

	RClientInterface::OpenPostMessageClose(iContext.NodeId(), iContext.NodeId(),
		TCFDataCollector::TTierDataCollectors(collectors).CRef());
	}





