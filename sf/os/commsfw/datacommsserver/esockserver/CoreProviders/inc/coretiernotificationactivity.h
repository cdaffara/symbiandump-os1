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

#ifndef _CORETIERNOTIFICATIONACTIVITY_H_
#define _CORETIERNOTIFICATIONACTIVITY_H_

#include <comms-infras/es_parameterbundle.h>
#include <comms-infras/ss_activities.h>
#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/ss_common.h>

namespace CommsDat
{
	class CMDBSession;
}

namespace ConnectionServ
{
	class CConnectionServParameterBundle;
}

namespace TMTierNotificationActivity
{
	DECLARE_EXPORT_NODEACTIVITY(TierNotify)
}

namespace TMTierNotificationGetDataCollectorsActivity
{
	DECLARE_EXPORT_NODEACTIVITY(GetCollectors)
}

namespace TierNotification
{

class CDataCollectorSession;
class MDataCollector;
class CTierNotificationCache;
class TTierNotificationActivity;


class MEqualityComparator
	{
public:
	virtual TBool Matches(const CParameterSetContainer& aRhs) = 0 ;
	};

class CAccessPointIdComparator : public MEqualityComparator
	{
public:
	CAccessPointIdComparator(const CParameterSetContainer& aToCompare):
		iToCompare(aToCompare) {}

	virtual TBool Matches(const CParameterSetContainer& aRhs);

protected:
	const CParameterSetContainer& iToCompare;
	};



//
//CTierNotificationActivity
class CTierNotificationActivity : public MeshMachine::CNodeParallelActivityBase
	{
public:
    IMPORT_C static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

protected:
	CTierNotificationActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount)
	:	MeshMachine::CNodeParallelActivityBase(aActivitySig, aNode, aActivitiesCount)
		{
		}

public:
	virtual ~CTierNotificationActivity();


	ESock::CTierManagerBase& TierManager() { return static_cast<ESock::CTierManagerBase&>(iNode); }


	CommsDat::CMDBSession& GetDbSession() { return *iDbSession; }


	/**
	Performs the given query

	@param aQuery input: query to run. ownership is given to this method
	*/
	/*virtual*/ void InitL(ESock::CRefCountOwnedParameterBundle* aQuery, const RMessage2& aPlatSecInfo, TBool aOneOffQuery);


	/*virtual*/ void StartL(RPointerArray<MDataCollector>& aCollectors);

	/**
	Scans cache for changes marked worthy for reporting.
	Copies them into a new bundle (whilst removing the report flags in
	 the cache).
	Then sends the bundle to the originator.
	*/
	IMPORT_C void ReportChangesInCacheL();

	/**
	See if all data collectors are unlocked
	*/
	IMPORT_C TBool AllCollectorsUnlocked() const;

	/**
	See if all data is ready to be reported
	*/
	TBool AllAccessPointsReadyForAllCollectors() const;

	/**
	Call this whenever notification might be appropriate,
	 it'll figure out using the cache flags whether to report.
	*/
	IMPORT_C void ReportIfReadyL();


	IMPORT_C void InitiateShutdown(TInt aErrorToReport);

	IMPORT_C void ShutdownFinished();




	/**
	To be called by collector (via collector session) to tell the activity that data has been received.
	Updates the cache (by making merge calls back to the collector (via collector session)
	 and notifies the client if appropriate.
	*/
	IMPORT_C void DataReceivedL(CDataCollectorSession& aSource, CParameterSetContainer& aNewData, TBool aEnableNotify = ETrue);


	// maybe better done by registering interested collectors as cache watchers
	void NotifyOtherCollectorsOfChangeL(CDataCollectorSession& aSource, CParameterSetContainer& aModifiedCacheEntry);



protected:
	virtual TBool HasSufficientCapabilities(const RMessage2& aPlatSecInfo) const;

public:
	/**
	Accessors to help build the collectors
	*/
	TBool OneOffQuery() const
	    {
	    return iOneOffQuery;
	    }

	ESock::CRefCountOwnedParameterBundle& QueryBundleOwner() const
	    {
	    return *iQueryBundleOwner;
	    }



	/**
	Creates the equality comparator for the given parameter set for this class.
	If this ever needs to be different for a tier, this function can be overridden
	*/
	virtual MEqualityComparator* CreateEqualityComparatorL(const CParameterSetContainer& aData);


private:
    void PostRefCountedBundleToOriginators(const Messages::TSignatureBase& aMsg, ESock::CRefCountOwnedParameterBundle& aBundleOwner);

private:
	// inputs. owned.
	TBool iOneOffQuery; // true for query, false for ongoing notification
	ESock::CRefCountOwnedParameterBundle*	iQueryBundleOwner; // query to run

	// working objects needed to run query. owned.
	RPointerArray<CDataCollectorSession>	iCollectorSessions;
	CTierNotificationCache*					iCache;
	TInt iMessageCount; // number of messages sent so far
	TBool iShuttingDown;
	TInt iErrorToReport; // code to send back once shutdown has finished

	// session shared by all collectors
	CommsDat::CMDBSession*	iDbSession;
	};


class CTierNotificationCache : public CBase
	{
public:
	static CTierNotificationCache* NewL();

protected:
	CTierNotificationCache()
		{
		}

	void SetCacheBundle(CParameterBundleBase* aBundle)
		{
		iCacheBundle=aBundle;
		}
public:
	virtual ~CTierNotificationCache();

public:
	CParameterSetContainer* Find(MEqualityComparator& aFinder);

protected:
	inline const CParameterSetContainer* Find(MEqualityComparator& aFinder) const
		{ return (const_cast<CTierNotificationCache*>(this))->Find(aFinder); }

public:
	inline const CParameterSetContainer* Get(TInt aIndex) const
		{ return iCacheBundle->GetParamSetContainer(aIndex); }

	inline CParameterSetContainer* Get(TInt aIndex)
		{ return iCacheBundle->GetParamSetContainer(aIndex); }

	// ownership of this param set container passes to the cache
	void AddL(CParameterSetContainer* aItemToAdd);

private:
	CParameterBundleBase*	iCacheBundle;
	};


// contains all the methods that can be called from the collector
class MCollectedDataReceiver
	{
public:
	/**
	To be called by collector to tell the session about the data that has been received.
	Implementation passes this on to the client of the data collector session.
	Ownership of the parameter set container passes with this call.
	*/
	virtual void DataReceivedL(CParameterSetContainer* aNewData, TBool aEnableNotify = ETrue) = 0;

	/**
	Setters and getters for the lock state. This tells the client whether we want
	 to allow notifications to take place to the end client of the notification.
	This allows a collector to feed information one at a time, or if it knows it's in the
	 middle of reporting a load of information at once, it can lock the session til the end,
	 in which case all the changed information would be reported in one big lump.
	*/
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	virtual TBool IsLocked() const = 0;

	virtual void Detach() = 0;
	virtual void ShutdownFinished() = 0;

	virtual void Error(TInt aErr) = 0;
	};



// created by activity, given to each collector
class CDataCollectorSession :	public CBase, public MCollectedDataReceiver
	{
public:
	inline static CDataCollectorSession* NewL(MDataCollector* aCollector, CTierNotificationActivity& aActivity, TInt aSessionId)
		{ return new (ELeave) CDataCollectorSession(aCollector,aActivity,aSessionId); }

protected:
	IMPORT_C CDataCollectorSession(MDataCollector* aCollector, CTierNotificationActivity& aActivity, TInt aSessionId);

public:
	virtual ~CDataCollectorSession();

	virtual void StartL();

	virtual CParameterSetContainer* ConstructEmptyRecordLC(TUint32 aId = 0);

	virtual void MergeL(CParameterSetContainer& aCacheEntry, CParameterSetContainer& aNewData);

	// Called when cache is modified.. to possibly kickstart further discovery.
	// 	be VERY careful if this leads to calling DataReceivedL as potentially
	//   this could result in an infinite recursion
	//    (DataReceived->AddToCache->NotifyOtherCollectors->CacheModified->DataReceived->....)
	virtual void CacheModifiedL(CParameterSetContainer& aModifiedEntry);

	/** to tell the client that we've finished shutting down so deletion is
	     allowed */
	virtual void Shutdown();
	virtual TBool HasShutDown();


// From MCollectedDataReceiver
	virtual void DataReceivedL(CParameterSetContainer* aNewData, TBool aEnableNotify = ETrue);
	virtual void Lock();
	virtual void Unlock();
	virtual TBool IsLocked() const;


	virtual void Detach();
	virtual void ShutdownFinished();


	virtual void Error(TInt aErr);


private:
	CTierNotificationActivity&	iActivity; // owns me
	MDataCollector*				iCollector; // i own this

	TInt						iSessionId;

	TBool						iIsLocked;
	};



template<class TYPE>
class CDataCollectorFactory
	{
public:
	static TYPE* AddNewToArrayL(CTierNotificationActivity& aActivity, RPointerArray<MDataCollector>& aResult)
		{
		TYPE* inst = TYPE::NewL(aActivity);
		CleanupStack::PushL(inst);
		aResult.AppendL(inst);
		CleanupStack::Pop(inst);
		return inst;
		}

//	static TYPE* AddIfRequiredL(CTierNotificationActivity& aActivity, RPointerArray<MDataCollector>& aResult)
//		{
//		if(TYPE::IsRequiredForQueryL(*(aActivity.QueryBundle()),aActivity.OneOffQuery()))
//			{
//			return CDataCollectorFactory<TYPE>::AddNewToArrayL(aActivity,aResult);
//			}
//		return 0;
//		}
	};


} //namespace TMActivities



#endif // _CORETIERNOTIFICATIONACTIVITY_H_

