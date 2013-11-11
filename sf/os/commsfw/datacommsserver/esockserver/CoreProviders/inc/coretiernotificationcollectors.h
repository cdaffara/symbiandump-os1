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


#ifndef _CORETIERNOTIFICATIONCOLLECTORS_H_
#define _CORETIERNOTIFICATIONCOLLECTORS_H_

#include <e32property.h>
#include <comms-infras/es_parameterbundle.h>
#include <comms-infras/ss_mmcommsprov.h>

#include <elements/factorynotify.h>
#include <comms-infras/ss_connprov.h>

#include <comms-infras/es_connectionservparameterbundletrace.h>

namespace CommsDat
{
	class CMDBSession;
	class CCDAccessPointRecord;
}

namespace TierNotification
{



class MCollectedDataReceiver;
class CTierNotificationActivity;

class MDataCollector
	{
public:

	virtual ~MDataCollector();

	virtual CParameterSetContainer* ConstructEmptyRecordLC(TUint32 aId) = 0;

	virtual void MergeL(CParameterSetContainer& aCacheEntry, CParameterSetContainer& aNewData,
									TBool& aInSet, TBool& aToReport) = 0;

	virtual void CacheModifiedL(CParameterSetContainer& aModifiedEntry,
														TInt& aIsRunning /*output*/) = 0;

	virtual void SetReceiver(MCollectedDataReceiver& aReceiver) = 0;

	virtual void StartL() = 0;

	virtual TBool SetMatchesQuery(const CParameterSetContainer& aSetToTest) = 0;

	virtual void Shutdown() = 0;

	};


class CDataCollectorBase : public CBase, public MDataCollector
	{
public:
	virtual void SetReceiver(MCollectedDataReceiver& aReceiver);

	virtual void StartL();

	virtual ~CDataCollectorBase();

	virtual void DoStartL() = 0;

	virtual void CacheModifiedL(CParameterSetContainer& aModifiedEntry,
														TInt& aIsRunning /*output*/);

	virtual void Shutdown();

protected:
	CDataCollectorBase(ESock::CRefCountOwnedParameterBundle& aQuery)
		{
		aQuery.Open();
		iQueryBundleOwner = &aQuery;
		}

    inline const CParameterBundleBase& QueryBundleBase()
        {
        return *(iQueryBundleOwner->Ptr());
        }

protected:
	// not owned but must be able to be null
	MCollectedDataReceiver*	iReceiver;

	// not owned
	ESock::CRefCountOwnedParameterBundle*	iQueryBundleOwner;
	};


class CTierDataCollector : public CDataCollectorBase
	{
public:
	CTierNotificationActivity& Activity() {return iActivity;}

	virtual ~CTierDataCollector();

	virtual CParameterSetContainer* ConstructEmptyRecordLC(TUint32 aId = 0);

	virtual void MergeL(CParameterSetContainer& aCacheEntry, CParameterSetContainer& aNewData,
									TBool& aInSet, TBool& aToReport);

protected:
	IMPORT_C CTierDataCollector(CTierNotificationActivity& aActivity);

	const ConnectionServ::CConnectionServParameterBundle& Query()
		{
		return static_cast<const ConnectionServ::CConnectionServParameterBundle&>(QueryBundleBase());
		}

protected:
	// not owned but must be able to be null
	CTierNotificationActivity&	iActivity;
	};


// collector which is also a node (so it can receive responses from other nodes, e.g. availability
class CTierDataCollectorNode : public CTierDataCollector, public Messages::ASimpleNodeIdBase
	{
protected:

	struct TDataProviderStatus
		{
		Messages::TNodeId					iProvider;		// the provider we're monitoring
		ESock::CMMCommsProviderBase*	iProviderPtr;	// the provider we're monitoring
		TAccessPointInfo				iApId;			// access point id of the provider we're monitoring

		// forgivable sneakiness
		void MarkCancelled() {iProviderPtr=0;}
			TBool WasCancelled() {return (iProviderPtr == NULL); }
		};

	CTierDataCollectorNode(CTierNotificationActivity& aActivity)
		: CTierDataCollector(aActivity)
		{}

public:
	virtual ~CTierDataCollectorNode();

public:
	TDataProviderStatus* FindDataProviderStatus(TAccessPointInfo aApId);
	TDataProviderStatus* FindDataProviderStatus(const Messages::TNodeId& aId);

	virtual void Shutdown();

	virtual void ReceivedErrorL(const Messages::TRuntimeCtxId& aSender, Messages::TSignatureBase& aCFMessage);

	virtual void ReceivedLeaveCompleteL(const Messages::TRuntimeCtxId& aSender, Messages::TSignatureBase& aCFMessage);

// from Messages::ASimpleNodeIdBase
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

protected:
	RArray<TDataProviderStatus> iProviders;
	TBool iShuttingDown_Leaving;
	};



class CTierInfoCollectorNode : public CTierDataCollectorNode
	{
protected:
	CTierInfoCollectorNode(CTierNotificationActivity& aActivity)
		: CTierDataCollectorNode(aActivity)
		{}

public:
	virtual ~CTierInfoCollectorNode();

public:
	// no initialisation.. driven by records coming in from other collectors ...
	virtual void DoStartL();

	// ... via this function
	virtual void CacheModifiedL(CParameterSetContainer& aModifiedEntry,
														TInt& aIsRunning /*output*/);

	virtual void SubscribeWithProviderL(TDataProviderStatus& aProvider) = 0 ;

	virtual void Shutdown();

	virtual void ReceivedErrorL(const Messages::TRuntimeCtxId& aSender, Messages::TSignatureBase& aCFMessage);

	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

protected:
	TBool iShuttingDown_Cancelling;
	};




class MCommsDatNotificationReceiver
	{
public:
	virtual void CommsDatNotificationL(TInt aEvent) = 0 ;
	};

class CCommsDatNotificationSubscriber : public CActive
	{
public:
	CCommsDatNotificationSubscriber(MCommsDatNotificationReceiver* aReceiver);
	void ConstructL();
	void RunL();
	void DoCancel();
private:
	RProperty iProperty;
	MCommsDatNotificationReceiver* iReceiver;
	};




class CCommsDatDataCollector : public CTierDataCollector, public MCommsDatNotificationReceiver
	{
protected:
	struct TConfiguredAccessPoint
		{
		TAccessPointInfo	iApId;			// access point id of the provider we're monitoring
		TBool				iRestricted;	// whether access point is restricted
		TBool				iChecked;		// whether it has been seen during this DB fetch
		};

public:
	static CCommsDatDataCollector* NewL(CTierNotificationActivity& aActivity)
		{ return new(ELeave) CCommsDatDataCollector(aActivity); }

protected:
	CCommsDatDataCollector(CTierNotificationActivity& aActivity);

public:
	// from CDataCollectorBase

//	IMPORT_C static TBool IsRequiredForQueryL(const CParameterBundleBase& aQuery, TBool aOneOffQuery);

	virtual ~CCommsDatDataCollector();

	virtual void DoStartL();

protected:
	void ReadDbL();

	virtual void CommsDatNotificationL(TInt aEvent);


public:
	virtual TBool SetMatchesQuery(const CParameterSetContainer& aSetToTest);

private:
	RArray<TConfiguredAccessPoint> iKnownAccessPoints;
	CCommsDatNotificationSubscriber* iSubscriber;
	};





class CMCprFactoryQueryDataCollector : public CTierDataCollectorNode
	{
public:
	static CMCprFactoryQueryDataCollector* NewL(CTierNotificationActivity& aActivity)
		{ return new(ELeave) CMCprFactoryQueryDataCollector(aActivity); }

//	IMPORT_C static TBool IsRequiredForQueryL(const CParameterBundleBase& aQuery, TBool aOneOffQuery);

protected:
	CMCprFactoryQueryDataCollector(CTierNotificationActivity& aActivity);

public:
	// from CDataCollectorBase

	virtual ~CMCprFactoryQueryDataCollector();

	// these functions are overridden for this "one-shot" collector.
	//  i.e. the DoStartL does all the work...
	virtual void DoStartL();

	// .. and the CacheModifiedL does nothing.
	//virtual void CacheModifiedL(CParameterSetContainer& aModifiedEntry,
	//													TInt& aIsRunning /*output*/);

public:
	virtual TBool SetMatchesQuery(const CParameterSetContainer& aSetToTest);

	void FindDontCreate(TBool aSetting)
					{iFindOnly = aSetting;}

protected:
	TBool iFindOnly;

	};


class CCprFactoryQueryDataCollector : public CTierDataCollectorNode
	{
public:
	static CCprFactoryQueryDataCollector* NewL(CTierNotificationActivity& aActivity)
		{ return new(ELeave) CCprFactoryQueryDataCollector(aActivity); }

protected:
	CCprFactoryQueryDataCollector(CTierNotificationActivity& aActivity);

public:
	// from CDataCollectorBase
	virtual ~CCprFactoryQueryDataCollector();

	// these functions are overridden for this "one-shot" collector.
	//  i.e. the DoStartL does all the work...
	virtual void DoStartL();

	// .. and the CacheModifiedL does nothing.
	//virtual void CacheModifiedL(CParameterSetContainer& aModifiedEntry,
	//													TInt& aIsRunning /*output*/);

public:
	virtual TBool SetMatchesQuery(const CParameterSetContainer& aSetToTest);
	};


class CFactoryNotifyDataCollector : public CTierDataCollector
	{
public:
	// from CDataCollectorBase

	virtual void DoStartL();

	virtual ~CFactoryNotifyDataCollector();

protected:
	CFactoryNotifyDataCollector(CTierNotificationActivity& aActivity)
		: CTierDataCollector(aActivity)
		{}

protected:
	// override these to look for different provider types
	virtual TUid FindFactoryUidForApL(CommsDat::CCDAccessPointRecord* aApRec) = 0;
	virtual ESock::CCommsFactoryBase* FindOrCreateFactoryL(TUid aUid) = 0;

	// .. and override this to provide callbacks for the notifications
	virtual const ESock::TAnyFn& GetVTable() = 0;

public:
	virtual TBool SetMatchesQuery(const CParameterSetContainer& aSetToTest);

protected:
	RArray<TUid> iRegisteredFactories;
	};


class CMcprFactoryNotifyDataCollector : public CFactoryNotifyDataCollector
	{
public:
	static CMcprFactoryNotifyDataCollector* NewL(CTierNotificationActivity& aActivity)
		{ return new(ELeave) CMcprFactoryNotifyDataCollector(aActivity); }

protected:
	CMcprFactoryNotifyDataCollector(CTierNotificationActivity& aActivity);

public:
	virtual ~CMcprFactoryNotifyDataCollector();

protected:

	// customised for MCPR
	virtual TUid FindFactoryUidForApL(CommsDat::CCDAccessPointRecord* aApRec);
	virtual ESock::CCommsFactoryBase* FindOrCreateFactoryL(TUid aUid);

	// access table to callbacks
	virtual const Factories::TAnyFn& GetVTable() {return iInterfaceVTableF[0];}
	
public:
	// the actual callback
	TInt NotifyCreation(Factories::AFactoryObject& aObject, Factories::CFactoryBase& aFactory);
	void ProcessNotificationL(Factories::AFactoryObject& aObject, const ConnectionServ::TAccessPointStatus& aApStatus);
	virtual void Shutdown();
	
protected:
	static const ESock::TAnyFn iInterfaceVTableF[];
	};

class CCprFactoryNotifyDataCollector : public CFactoryNotifyDataCollector
	{
public:
	static CCprFactoryNotifyDataCollector* NewL(CTierNotificationActivity& aActivity)
		{ return new(ELeave) CCprFactoryNotifyDataCollector(aActivity); }

protected:
	CCprFactoryNotifyDataCollector(CTierNotificationActivity& aActivity);

public:
	virtual ~CCprFactoryNotifyDataCollector();

protected:

	// customised for MCPR
	virtual TUid FindFactoryUidForApL(CommsDat::CCDAccessPointRecord* aApRec);
	virtual ESock::CCommsFactoryBase* FindOrCreateFactoryL(TUid aUid);

	// access table to callbacks
	virtual const Factories::TAnyFn& GetVTable() {return iInterfaceVTableF[0];}

public:
	// the actual callback
	TInt NotifyCreation(Factories::AFactoryObject& aObject, Factories::CFactoryBase& aFactory);
	void NotifyDeletion(Factories::AFactoryObject& aObject, Factories::CFactoryBase& aFactory);
	void ProcessNotificationL(Factories::AFactoryObject& aObject, const ConnectionServ::TAccessPointStatus& aApStatus);

	virtual void MergeL(CParameterSetContainer& aCacheEntry, CParameterSetContainer& aNewData, TBool& aInSet, TBool& aToReport);
	virtual void Shutdown();
	
protected:
	virtual TBool SetMatchesQuery(const CParameterSetContainer& aSetToTest);
	
	static const ESock::TAnyFn iInterfaceVTableF[];
	};

class CAvailabilityDataCollector : public CTierInfoCollectorNode
	{
public:
	static CAvailabilityDataCollector* NewL(CTierNotificationActivity& aActivity)
		{ return new(ELeave) CAvailabilityDataCollector(aActivity); }

protected:
	CAvailabilityDataCollector(CTierNotificationActivity& aActivity);

public:
// from CDataCollectorBase

//	IMPORT_C static TBool IsRequiredForQueryL(const CParameterBundleBase& aQuery, TBool aOneOffQuery);

	virtual ~CAvailabilityDataCollector();

	virtual void SubscribeWithProviderL(TDataProviderStatus& aProvider);

	virtual TBool SetMatchesQuery(const CParameterSetContainer& aSetToTest);

// from Messages::ASimpleNodeIdBase

	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	};


class CActiveStatusDataCollector : public CTierInfoCollectorNode
	{
public:
	static CActiveStatusDataCollector* NewL(CTierNotificationActivity& aActivity)
		{ return new(ELeave) CActiveStatusDataCollector(aActivity); }

protected:
	CActiveStatusDataCollector(CTierNotificationActivity& aActivity);

public:
// from CDataCollectorBase

//	IMPORT_C static TBool IsRequiredForQueryL(const CParameterBundleBase& aQuery, TBool aOneOffQuery);

	virtual ~CActiveStatusDataCollector();

	virtual void SubscribeWithProviderL(TDataProviderStatus& aProvider);

	virtual TBool SetMatchesQuery(const CParameterSetContainer& aSetToTest);

// from Messages::ASimpleNodeIdBase

	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	};

class CAccessPointPlaneStatusCollector : public CTierDataCollector
	{
public:
	static CAccessPointPlaneStatusCollector* NewL(CTierNotificationActivity& aActivity)
		{ return new(ELeave) CAccessPointPlaneStatusCollector(aActivity); }

protected:
	CAccessPointPlaneStatusCollector(CTierNotificationActivity& aActivity);

public:
	virtual ~CAccessPointPlaneStatusCollector();

public:
// from CDataCollectorBase

	virtual void DoStartL();

	virtual TBool SetMatchesQuery(const CParameterSetContainer& aSetToTest);

	virtual CParameterSetContainer* ConstructEmptyRecordLC(TUint32 aId = 0);

	virtual void CacheModifiedL(CParameterSetContainer& aModifiedEntry,
														TInt& aIsRunning /*output*/);
	};


// should probably move this to link tier only. if nothing else this would
//  be an exercise in ensuring that base collectors can be reused from custom tiers
class CIPProtoAccessPointParameterCollector : public CTierDataCollector
	{
public:
	static CIPProtoAccessPointParameterCollector* NewL(CTierNotificationActivity& aActivity)
		{ return new(ELeave) CIPProtoAccessPointParameterCollector(aActivity); }

protected:
	CIPProtoAccessPointParameterCollector(CTierNotificationActivity& aActivity);

public:
	virtual ~CIPProtoAccessPointParameterCollector();

public:
// from CDataCollectorBase

	virtual void DoStartL();

	virtual TBool SetMatchesQuery(const CParameterSetContainer& aSetToTest);

	virtual CParameterSetContainer* ConstructEmptyRecordLC(TUint32 aId = 0);

	virtual void CacheModifiedL(CParameterSetContainer& aModifiedEntry,
														TInt& aIsRunning /*output*/);
	};




} // namespace TMActivities

#endif // _CORETIERNOTIFICATIONCOLLECTORS_H_

