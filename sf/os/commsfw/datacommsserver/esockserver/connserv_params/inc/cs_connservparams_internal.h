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
// Generic connection server parameter sets used by internal components - plugin for Connection Server Parameter Bundle
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef CS_CONNSERVPARAMS_INTERNAL_H_
#define CS_CONNSERVPARAMS_INTERNAL_H_

#include <comms-infras/ss_nodemessages_mcpr.h>

//const TInt32 Assigned in cs_connservparams.h = 0x10272F3E;
//const TInt32 Assigned in cs_connservparams.h = 1;
//const TInt32 Assigned in cs_connservparams.h = 2;
const TInt32 KNotificationCacheParameterSetType = 3;
const TInt32 KCoreCollectorSharedParameterSetType = 4;
const TInt32 KIpProtoAccessPointParameterSetType = 5;
const TInt32 KAccessPointPlaneStatusParameterSetType = 6;
const TInt32 KAccessPointPlaneStatusQueryType = 7;
const TInt32 KProviderStatusParameterSetType = 8;

namespace ConnectionServ
{

class XNotificationCacheParameterSet : public XConnectionServParameterSet,
		public MParameterSetTemplateMethods<CConnectionServParameterSetContainer,XNotificationCacheParameterSet,
								KConnectionServGenericParamImplUid, KNotificationCacheParameterSetType>
/** Notification Cache Parameter Set

// Used internally by the notification mechanism.
//  Contains 4 bit maps. The collector session with index i will mark bit i in each of these,
//   to denote:
//    iRunning  -  that a response from this collector is needed before we have
//                  a complete picture of this access point (i.e. before we can release this
//                  record for a one off query). All so necessary in the brave new asynchronous world.
//    iReceived -  that a response has been received at least once from this collector
//                  (so it's possible to release this record for a one-off query)
//    iInSet    -  that the cache entry matches this collector's interpretation of the query.
//                  Setting this saves us having to keep running the query against this collector,
//                  until at least the data is changed.
//    iToReport -  that this collector thinks the cache entry needs to be reported to the client.
//                  generally matches iInSet, but also set in the case that we're in notification
//                  mode and the record leaves the matched set.. so it is no longer inSet but
//                  its leaving must be reported

//  Also holds intelligence as to whether notification should occur of the accompanying parameter sets

@note SMetaDataECom must be the first in the superclasses' list
@internalTechnology
*/
{
public:
	inline XNotificationCacheParameterSet(): iRunning(0),iReceived(0),iInSet(0),iToReport(0) {}

// queries

	inline TBool AnyToReport() const {return iToReport?ETrue:EFalse;}

	inline TBool ReadyToReport() const
		{
		return (iRunning && iRunning == (iRunning & iReceived));
		}
		
	inline TBool ShouldReport() const
		{
		// only consider reporting if at least 1 report flag is on
		if(AnyToReport() == EFalse) return EFalse;
		
		// ok, given that at least 1 report flag is on..
		//    we should only report if all collectors are either..
		//  - tagged for report
		//  - tagged as InSet (already matches filter for that collector)
		//  - both
		TUint32 combinedFlags = iInSet | iToReport;

		// make sure that all running for this AP are either tagged for report
		//  or already in set.
		return iRunning == (iRunning & combinedFlags);
		}

	inline TBool InSet(TInt aIndex) const
		{ return (iInSet & (1<<aIndex)); }


// raw value accessors for trace only
	inline TUint32 RunningValue() const { return iRunning; }
	inline TUint32 ReceivedValue() const { return iReceived; }
	inline TUint32 InSetValue() const { return iInSet; }
	inline TUint32 ToReportValue() const { return iToReport; }


// setters

	inline void SetRunning(TInt aIndex)
		{AssignBitValue(aIndex,1,iRunning);}

	inline void SetReceived(TInt aIndex)
		{AssignBitValue(aIndex,1,iReceived);}

	inline void InSet(TInt aIndex, TBool aVal)
		{AssignBitValue(aIndex,aVal,iInSet);}
		
	inline void ToReport(TInt aIndex, TBool aVal)
		{AssignBitValue(aIndex,aVal,iToReport);}
	inline void ClearToReport() {iToReport=0;}


	inline void AssignBitValue(TInt aIndex, TBool aVal, TUint32& aDest)
		{ TUint32 mask=(1<<aIndex); if(aVal) {aDest |= mask;} else if(mask&aDest) {aDest ^= mask;} }

// polymorphic logging
	virtual TInt AppendPrintableTo(TDes& aDestination) const;

	DATA_VTABLE

protected:

	TUint32 iRunning; // the collectors from which a response is required to make a valid match
	TUint32 iReceived; // the collectors from which a response has been received
	TUint32 iInSet;	// the collectors which match this data set in their query
	TUint32 iToReport; // the collectors which say this data set should be reported
	};



class XCoreCollectorSharedParameterSet : public XConnectionServParameterSet,
		public MParameterSetTemplateMethods<CConnectionServParameterSetContainer,XCoreCollectorSharedParameterSet,
								KConnectionServGenericParamImplUid, KCoreCollectorSharedParameterSetType>
/** Core Collector Shared Parameter Set

// Used internally by the notification mechanism between different core data collectors.
//   To allow the separation of the collector of MCPRs and the collector of data from those MCPRs.
//    iMcprCommsId	-  the comms id of the MCPR of this access point.. for passing between collectors
//						who talk as nodes rather than in direct C++ calls.
//	  iMcprPtr		-  the C++ pointer to the same MCPR. not a problem as all this processing happens
//						within the management plane.

@note SMetaDataECom must be the first in the superclasses' list
@internalTechnology
*/
{
public:

	inline XCoreCollectorSharedParameterSet(): iMcprCommsId(),iMcprPtr(NULL) {}

	const Messages::TNodeId& McprCommsId() const
		{
		return iMcprCommsId;
		}

	void McprCommsId(const Messages::TNodeId& aMcprCommsId)
		{
		iMcprCommsId = aMcprCommsId;
		}

	CBase* McprPtr() const
		{
		return iMcprPtr;
		}

	void McprPtr(CBase* aMcprPtr)
		{
		iMcprPtr = aMcprPtr;
		}

	virtual void UpdateWithL(const XParameterSet& aRhsBase);

// polymorphic logging
	virtual TInt AppendPrintableTo(TDes& aDestination) const;

	DATA_VTABLE

protected:

	Messages::TNodeId iMcprCommsId;
	CBase*	iMcprPtr;
	};



class XIpProtoAccessPointParameterSet: public XConnectionServParameterSet,
		public MParameterSetTemplateMethods<CConnectionServParameterSetContainer,XIpProtoAccessPointParameterSet,
								KConnectionServGenericParamImplUid, KIpProtoAccessPointParameterSetType>
/** XIpProtoAccessPointParameterSet

// Used internally to contain information about the IP Proto layer by CConnection legacy calls.
//  Further use highly discouraged.

@note SMetaDataECom must be the first in the superclasses' list
@internalTechnology
*/
{
public:
	inline XIpProtoAccessPointParameterSet():
			iIapId(0),iNetworkId(0),iConnectionType(EConnectionGeneric),iControlClientId() {}

// getters
	TUint32 IapId() const { return iIapId; }
	TUint32 NetworkId() const { return iNetworkId; }
	TConnectionType ConnectionType() const
		{ 
		return static_cast<TConnectionType>(iConnectionType); 
		}
		
	const Messages::TNodeId& ControlClientId() const
		{ 
		return iControlClientId; 
		}

// setters
	void IapId(TUint32 aIapId) { iIapId = aIapId; }
	void NetworkId(TUint32 aNetworkId) { iNetworkId = aNetworkId; }
	void ConnectionType(TConnectionType aConnectionType)
		{ 
		iConnectionType = aConnectionType; 
		}
		
	void ControlClientId(const Messages::TNodeId& aControlClientId)
		{ 
		iControlClientId = aControlClientId; 
		}

// extra intelligence
	virtual void UpdateWithL(const XParameterSet& aRhsBase);

// polymorphic logging
	virtual TInt AppendPrintableTo(TDes& aDestination) const;

	DATA_VTABLE

protected:

	TUint32 iIapId;
	TUint32 iNetworkId;
	TUint32 iConnectionType;
	// the pointer to the ACFNodeId in the mcpr on the layer above this one
	Messages::TNodeId iControlClientId;
	};


// PLANE STATUS - PARAMETER SET

// "plane status" structure, holds the status of the providers
//  in each plane for an access point
struct TAccessPointPlaneStatusDataBitmap
	{
	TUint iTier_Exists:2;
	TUint iTier_Up:2;		// will probably need this soon
	TUint iTier_Reserved:6; // room for 3 more
	
	TUint iMetaConnection_Exists:2;
	TUint iMetaConnection_Up:2;
	TUint iMetaConnection_Reserved:6;
	
	TUint iConnection_Exists:2;
	TUint iConnection_Up:2;
	TUint iConnection_Reserved:6;
	
	TUint iSubConnection_Exists:2;
	TUint iSubConnection_Up:2;
	TUint iSubConnection_Reserved:6;
	
	TUint iData_Exists:2;
	TUint iData_Up:2;
	TUint iData_Reserved:6;
	};

struct TAccessPointPlaneStatusFilter;


struct TAccessPointPlaneStatus
/** TAccessPointPlaneStatus

Holds "plane status" structure- the status of the providers
in each plane for an access point

@note SMetaDataECom must be the first in the superclasses' list
@internalTechnology
*/
	{
	union
		{
		TAccessPointPlaneStatusDataBitmap iS;
		TInt64 iAsInt;
		} iU;
	
	TAccessPointPlaneStatus(TInt64 aI)
		{
		iU.iAsInt = aI;
		}

	TAccessPointPlaneStatus()
		{
		iU.iAsInt = TInt64(0);
		}

	TInt64 AsInt64() const { return iU.iAsInt; }


	void Connection_Exists(TAccessPointFlagValue aVal) {iU.iS.iConnection_Exists = aVal;}
	TAccessPointFlagValue Connection_Exists() const {return static_cast<TAccessPointFlagValue>(iU.iS.iConnection_Exists);}
	// to add as and when needed:
	//  iTier
	//  iMetaConnection
	//  iSubConnection
	//  iData
	//    .. and _Up for all above


	void UpdateWithL(const TAccessPointPlaneStatus& aSrc)
		{
		// Fields:	TMCSD

		// Initial:	?????
		// +
		// New:		??e??
		// =
		// Result	??e??

		// +
		// New:		?u???
		// =
		// Result:	?ue??

		TAccessPointFlagValue value = static_cast<TAccessPointFlagValue>(iU.iS.iConnection_Exists);
		TAccessPointStatus::PickUpIfSrcKnown( value, aSrc.Connection_Exists() );
		iU.iS.iConnection_Exists = value;
		// to add as and when needed:
		//  iTier
		//  iMetaConnection
		//  iSubConnection
		//  iData
		//    .. and _Up for all above
		}

	inline void CopyFieldsRequiredByQueryFrom(const TAccessPointPlaneStatusFilter& aFilter,const TAccessPointPlaneStatus& aSource);
	};

struct TAccessPointPlaneStatusFilter
/** TAccessPointPlaneStatusFilter

Query for TAccessPointPlaneStatusFilter

@note SMetaDataECom must be the first in the superclasses' list
@internalTechnology
*/
	{
	union
		{
		TAccessPointPlaneStatusDataBitmap iS;
		TInt64 iAsInt;
		} iU;

	TAccessPointPlaneStatusFilter(TInt64 aI)
		{
		iU.iAsInt = aI;
		}

	TAccessPointPlaneStatusFilter()
		{
		iU.iAsInt = TInt64(0);
		}

	TInt64 AsInt64() const { return iU.iAsInt; }


	void Connection_Exists(TAccessPointFlagFilterValue aVal) {iU.iS.iConnection_Exists = aVal;}
	TAccessPointFlagFilterValue Connection_Exists() const {return static_cast<TAccessPointFlagFilterValue>(iU.iS.iConnection_Exists);}
	// to add as and when needed:
	//  iTier
	//  iMetaConnection
	//  iSubConnection
	//  iData
	//    .. and _Up for all above
	};

inline void TAccessPointPlaneStatus::CopyFieldsRequiredByQueryFrom(const TAccessPointPlaneStatusFilter& aFilter,const TAccessPointPlaneStatus& aSource)
	{
	if(aFilter.Connection_Exists() != EAccessPointFlagIgnore)
		{
		Connection_Exists(aSource.Connection_Exists());
		}
	// to add as and when needed:
	//  iTier
	//  iMetaConnection
	//  iSubConnection
	//  iData
	//    .. and _Up for all above
	}


class XAccessPointPlaneStatusParameterSet: public XConnectionServParameterSet,
		public MParameterSetTemplateMethods<CConnectionServParameterSetContainer,XAccessPointPlaneStatusParameterSet,
								KConnectionServGenericParamImplUid, KAccessPointPlaneStatusParameterSetType>
/** XAccessPointPlaneStatusParameterSet

// The status of the various planes in comms framework.
// Currently only connection plane 'started' supported by this interface

@note SMetaDataECom must be the first in the superclasses' list
@internalTechnology
*/
{
public:
	inline XAccessPointPlaneStatusParameterSet(): iPlaneStatus(0) {}

	const TAccessPointPlaneStatus& PlaneStatus() const
		{ return iPlaneStatus; }

	void PlaneStatus(const TAccessPointPlaneStatus& aPSF)
		{ iPlaneStatus = aPSF; }


	virtual void UpdateWithL(const XParameterSet& aRhsBase);
	virtual void CopyFieldsRequiredByQueryFromL(
						const CParameterBundleBase& aQuery,const XConnectionServParameterSet& aSource);

// polymorphic logging
	virtual TInt AppendPrintableTo(TDes& aDestination) const;


	DATA_VTABLE

protected:

	TAccessPointPlaneStatus iPlaneStatus;
	};




// PLANE STATUS - QUERY


class XAccessPointPlaneStatusQuery: public XConnectionServParameterSet,
		public MParameterSetTemplateMethods<CConnectionServParameterSetContainer,XAccessPointPlaneStatusQuery,
								KConnectionServGenericParamImplUid, KAccessPointPlaneStatusQueryType>
/** XAccessPointPlaneStatusQuery

// Query the status of the various planes in comms framework.
// Currently only connection plane 'started' is supported by this interface

// Currently only used by legacy "EnumerateConnections" so kept internal for
//  this release.

@note SMetaDataECom must be the first in the superclasses' list
@internalTechnology
*/
{
public:
	inline XAccessPointPlaneStatusQuery(): iPlaneStatusFilter(0) {}

	const TAccessPointPlaneStatusFilter& PlaneStatusFilter() const
		{ return iPlaneStatusFilter; }

	void PlaneStatusFilter(const TAccessPointPlaneStatusFilter& aPSF)
		{ iPlaneStatusFilter = aPSF; }

// polymorphic logging
	virtual TInt AppendPrintableTo(TDes& aDestination) const;

	DATA_VTABLE

protected:

	TAccessPointPlaneStatusFilter iPlaneStatusFilter;
	};

} // namespace ConnectionServ

#endif /*CS_CONNSERVPARAMS_INTERNAL_H_*/

