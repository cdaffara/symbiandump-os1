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
 @file connservparams.cpp
*/

#include <e32std.h>
#include <e32test.h>

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <comms-infras/metatypearray.h>
#include "cs_connservparams.h"
#include <comms-infras/cs_connservparams_internal.h>

using namespace ConnectionServ;
using namespace Meta;

START_ATTRIBUTE_TABLE( XAccessPointGenericParameterSet, KConnectionServGenericParamImplUid, KAccessPointGenericParameterSetType )
	REGISTER_ATTRIBUTE( XAccessPointGenericParameterSet, iApInfo, TMeta<TAccessPointInfo> )
	REGISTER_ATTRIBUTE( XAccessPointGenericParameterSet, iApStatus, TMeta<TAccessPointStatus> )
END_ATTRIBUTE_TABLE_BASE( XConnectionServParameterSet, 0 )

START_ATTRIBUTE_TABLE( XAccessPointGenericQuery, KConnectionServGenericParamImplUid, KAccessPointGenericQueryType )
	REGISTER_ATTRIBUTE( XAccessPointGenericQuery, iApsToMatch, TMetaArray<TAccessPointInfo> )
	REGISTER_ATTRIBUTE( XAccessPointGenericQuery, iApStatusFilter, TMeta<TAccessPointStatusFilter> )
	REGISTER_ATTRIBUTE( XAccessPointGenericQuery, iParameterSetTypesToReturn, TMetaArray<STypeId> )
END_ATTRIBUTE_TABLE_BASE( XConnectionServParameterSet, 0 )


// Used internally by the notification mechanism.
//  Contains 2 bit maps. The collector session with index i will mark bit i in each of these,
//   to denote:
//    iRunning  -  whether the collector knows about this access point and must match to allow report of this AP
//    iReceived -  whether the collector has received any data for this access point
//    iInSet    -  that the cache entry matches the collector's interpretation of the query filter
//    iToReport -  that the cache entry needs to be reported to the client
START_ATTRIBUTE_TABLE( XNotificationCacheParameterSet, KConnectionServGenericParamImplUid, KNotificationCacheParameterSetType )
	REGISTER_ATTRIBUTE( XNotificationCacheParameterSet, iRunning, TMeta<TUint32> )
	REGISTER_ATTRIBUTE( XNotificationCacheParameterSet, iReceived, TMeta<TUint32> )
	REGISTER_ATTRIBUTE( XNotificationCacheParameterSet, iInSet, TMeta<TUint32> )
	REGISTER_ATTRIBUTE( XNotificationCacheParameterSet, iToReport, TMeta<TUint32> )
END_ATTRIBUTE_TABLE_BASE( XConnectionServParameterSet, 0 )

// Used internally by the notification mechanism between different core data collectors.
//   To allow the separation of the collector of MCPRs and the collector of data from those MCPRs.
//    iMcprCommsId	-  the comms id of the MCPR of this access point.. for passing between collectors
//    iMcprPtr		-  pointer to the same MCPR
START_ATTRIBUTE_TABLE( XCoreCollectorSharedParameterSet, KConnectionServGenericParamImplUid, KCoreCollectorSharedParameterSetType )
	REGISTER_ATTRIBUTE( XCoreCollectorSharedParameterSet, iMcprCommsId, TMeta<Messages::TNodeId> )
	REGISTER_ATTRIBUTE( XCoreCollectorSharedParameterSet, iMcprPtr, TMeta<CBase*> )
END_ATTRIBUTE_TABLE_BASE( XConnectionServParameterSet, 0 )

// Used internally to contain information about the IP Proto layer by CConnection legacy calls.
START_ATTRIBUTE_TABLE( XIpProtoAccessPointParameterSet, KConnectionServGenericParamImplUid, KIpProtoAccessPointParameterSetType )
	REGISTER_ATTRIBUTE( XIpProtoAccessPointParameterSet, iIapId, TMeta<TUint32> )
	REGISTER_ATTRIBUTE( XIpProtoAccessPointParameterSet, iNetworkId, TMeta<TUint32> )
	REGISTER_ATTRIBUTE( XIpProtoAccessPointParameterSet, iConnectionType, TMeta<TUint32> )
	REGISTER_ATTRIBUTE( XIpProtoAccessPointParameterSet, iControlClientId, TMeta<Messages::TNodeId> )
END_ATTRIBUTE_TABLE_BASE( XConnectionServParameterSet, 0 )

//  Only used internally at the moment, by EnumerateConnections
START_ATTRIBUTE_TABLE( XAccessPointPlaneStatusParameterSet, KConnectionServGenericParamImplUid, KAccessPointPlaneStatusParameterSetType )
	REGISTER_ATTRIBUTE( XAccessPointPlaneStatusParameterSet, iPlaneStatus, TMeta<TAccessPointPlaneStatus> )
END_ATTRIBUTE_TABLE_BASE( XConnectionServParameterSet, 0 )

//  Only used internally at the moment, by EnumerateConnections
START_ATTRIBUTE_TABLE( XAccessPointPlaneStatusQuery, KConnectionServGenericParamImplUid, KAccessPointPlaneStatusQueryType )
	REGISTER_ATTRIBUTE( XAccessPointPlaneStatusQuery, iPlaneStatusFilter, TMeta<TAccessPointPlaneStatusFilter> )
END_ATTRIBUTE_TABLE_BASE( XConnectionServParameterSet, 0 )





// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KConnectionServGenericParamImplUid, CConnectionServGenericParamsFactory::NewL),
	};

/**
ECOM Implementation Factories
*/

//extern "C" { IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount); }

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

/**
Plugin Implementation
*/

XConnectionServParameterSet* CConnectionServGenericParamsFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
	case (KAccessPointGenericParameterSetType):
		return new (ELeave) XAccessPointGenericParameterSet;
	case (KAccessPointGenericQueryType):
		return new (ELeave) XAccessPointGenericQuery;
	case (KNotificationCacheParameterSetType):
		return new (ELeave) XNotificationCacheParameterSet;
	case (KCoreCollectorSharedParameterSetType):
		return new (ELeave) XCoreCollectorSharedParameterSet;
	case (KIpProtoAccessPointParameterSetType):
		return new (ELeave) XIpProtoAccessPointParameterSet;
	case (KAccessPointPlaneStatusParameterSetType):
		return new (ELeave) XAccessPointPlaneStatusParameterSet;
	case (KAccessPointPlaneStatusQueryType):
		return new (ELeave) XAccessPointPlaneStatusQuery;
	default:
		User::Leave(KErrNotFound);
		}
	return NULL;
	}


/*virtual*/ void XAccessPointGenericParameterSet::UpdateWithL(const XParameterSet& aRhsBase)
	{
	const XAccessPointGenericParameterSet& aRhs = static_cast<const XAccessPointGenericParameterSet&>(aRhsBase);
	if(aRhs.AccessPointInfo().AccessPoint())
		{
		iApInfo = aRhs.iApInfo;
		}
	iApStatus.UpdateWith(aRhs.iApStatus);
	}

/*virtual*/ void XAccessPointGenericParameterSet::CopyFieldsRequiredByQueryFromL(
						const CParameterBundleBase& aQueryBundle,const XConnectionServParameterSet& aSourceBase)
	{
	const XAccessPointGenericParameterSet& aSource = static_cast<const XAccessPointGenericParameterSet&>(aSourceBase);
	const CParameterSetContainer* aQueryCtr = aQueryBundle.GetParamSetContainer(0);
	User::LeaveIfNull((TAny*)aQueryCtr);
	const XAccessPointGenericQuery* aQuerySet = XAccessPointGenericQuery::FindInParamSetContainer(*aQueryCtr);
	User::LeaveIfNull((TAny*)aQuerySet);
	iApInfo = aSource.iApInfo;
	iApStatus.CopyFieldsRequiredByQueryFrom(aQuerySet->AccessPointStatusFilter(),aSource.iApStatus);
	}

/*virtual*/ TInt XAccessPointGenericParameterSet::AppendPrintableTo(TDes& aBuf) const
	{
#ifdef __CFLOG_ACTIVE
	TDes16IgnoreOverflow ignoreOverflow;
	TAccessPointInfo inf = this->AccessPointInfo();
	TAccessPointStatus stat = this->AccessPointStatus();
	aBuf.AppendFormat(_L("GPS: AP:%d C:%c R:%c A:%c S:%c"),&ignoreOverflow,
						inf.AccessPoint(),
						TAccessPointStatus::FlagValueAsChar(stat.Configured()),
						TAccessPointStatus::FlagValueAsChar(stat.Restricted()),
						TAccessPointStatus::FlagValueAsChar(stat.Available()),
						TAccessPointStatus::FlagValueAsChar(stat.Started()) );
	return KErrNone;
#else
	(void)aBuf;
	return KErrNotSupported;
#endif // __CFLOG_ACTIVE
	}


/*virtual*/ XAccessPointGenericQuery::~XAccessPointGenericQuery()
	{
	iApsToMatch.Close();
	iParameterSetTypesToReturn.Close();
	}

TBool CompareTypeIds(const STypeId& aFirst, const STypeId& aSecond)
    {
    return( aFirst == aSecond );
    }

/*virtual*/ TBool XAccessPointGenericQuery::ShouldReturnType(const STypeId& aTypeId) const
	{
	return iParameterSetTypesToReturn.Find(aTypeId,TIdentityRelation<STypeId>(CompareTypeIds)) != KErrNotFound ;
	}

/*virtual*/ TInt XAccessPointGenericQuery::AppendPrintableTo(TDes& aBuf) const
	{
#ifdef __CFLOG_ACTIVE
	TDes16IgnoreOverflow ignoreOverflow;
	aBuf.AppendFormat(_L("GQ: APs("),&ignoreOverflow);

	const RArray<TAccessPointInfo>& aps = this->AccessPointsToMatch();
	TAccessPointStatusFilter apsf = this->AccessPointStatusFilter();
	const RArray<STypeId>& types = this->GetParameterSetTypesToReturn();

	TInt i=0;
	for( ; i<aps.Count(); ++i)
		{
		if(i)
			{
			aBuf.AppendFormat(_L(","),&ignoreOverflow);
			}
		aBuf.AppendFormat(_L("%d"),&ignoreOverflow,aps[i].AccessPoint());
		}
	
	aBuf.AppendFormat(_L(") C:%c R:%c A:%c S:%c typ("),&ignoreOverflow,
					TAccessPointStatusFilter::FilterValueAsChar(apsf.Configured()),
					TAccessPointStatusFilter::FilterValueAsChar(apsf.Restricted()),
					TAccessPointStatusFilter::FilterValueAsChar(apsf.Available()),
					TAccessPointStatusFilter::FilterValueAsChar(apsf.Started()) );

	for(i=0 ; i<types.Count(); ++i)
		{
		if(i)
			{
			aBuf.AppendFormat(_L(","),&ignoreOverflow);
			}
		aBuf.AppendFormat(_L("%S-%08x)"),&ignoreOverflow,
				types[i].iUid.iUid,
				types[i].iType );
		}
	return KErrNone;
#else
	(void)aBuf;
	return KErrNotSupported;
#endif // __CFLOG_ACTIVE
	}


/*virtual*/ TInt XNotificationCacheParameterSet::AppendPrintableTo(TDes& aBuf) const
	{
#ifdef __CFLOG_ACTIVE
	TDes16IgnoreOverflow ignoreOverflow;
	aBuf.AppendFormat(_L("NCP: Rg%032b Rd%032b IS%032b TR%032b"),&ignoreOverflow,
			this->RunningValue(),
			this->ReceivedValue(),
			this->InSetValue(),
			this->ToReportValue() );
	return KErrNone;
#else
	(void)aBuf;
	return KErrNotSupported;
#endif // __CFLOG_ACTIVE
	}


/*virtual*/ void XCoreCollectorSharedParameterSet::UpdateWithL(const XParameterSet& aRhsBase)
	{
	const XCoreCollectorSharedParameterSet& aRhs = static_cast<const XCoreCollectorSharedParameterSet&>(aRhsBase);

	if(aRhs.iMcprPtr)
		{
		iMcprPtr=aRhs.iMcprPtr;
		iMcprCommsId=aRhs.iMcprCommsId;
		}
	}

/*virtual*/ TInt XCoreCollectorSharedParameterSet::AppendPrintableTo(TDes& aBuf) const
	{
#ifdef __CFLOG_ACTIVE
	TDes16IgnoreOverflow ignoreOverflow;
	aBuf.AppendFormat(_L("CCSPS:mcpr %08x"),&ignoreOverflow,
		reinterpret_cast<TUint32>(this->McprPtr()) );
	return KErrNone;
#else
	(void)aBuf;
	return KErrNotSupported;
#endif // __CFLOG_ACTIVE
	}


/*virtual*/ void XIpProtoAccessPointParameterSet::UpdateWithL(const XParameterSet& aRhsBase)
	{
	const XIpProtoAccessPointParameterSet& aRhs = static_cast<const XIpProtoAccessPointParameterSet&>(aRhsBase);

	iIapId = aRhs.iIapId;
	iNetworkId = aRhs.iNetworkId;
	iConnectionType = aRhs.iConnectionType;
	iControlClientId = aRhs.iControlClientId;
	}

/*virtual*/ TInt XIpProtoAccessPointParameterSet::AppendPrintableTo(TDes& aBuf) const
	{
#ifdef __CFLOG_ACTIVE
	TDes16IgnoreOverflow ignoreOverflow;
	aBuf.AppendFormat(_L("IPPAPS:IAP:%d Net:%d ConnType:%d"),&ignoreOverflow,
							this->IapId(),
							this->NetworkId(),
							this->ConnectionType() );	
	return KErrNone;
#else
	(void)aBuf;
	return KErrNotSupported;
#endif // __CFLOG_ACTIVE
	}


/*virtual*/ void XAccessPointPlaneStatusParameterSet::UpdateWithL(const XParameterSet& aRhsBase)
	{
	const XAccessPointPlaneStatusParameterSet& aRhs = static_cast<const XAccessPointPlaneStatusParameterSet&>(aRhsBase);
	iPlaneStatus.UpdateWithL(aRhs.iPlaneStatus);
	}

/*virtual*/ void XAccessPointPlaneStatusParameterSet::CopyFieldsRequiredByQueryFromL(
						const CParameterBundleBase& aQueryBundle,const XConnectionServParameterSet& aSourceBase)
	{
	const XAccessPointPlaneStatusParameterSet& aSource = static_cast<const XAccessPointPlaneStatusParameterSet&>(aSourceBase);
	const CParameterSetContainer* aQueryCtr = aQueryBundle.GetParamSetContainer(0);
	User::LeaveIfNull((TAny*)aQueryCtr);
	const XAccessPointPlaneStatusQuery* aQuerySet = XAccessPointPlaneStatusQuery::FindInParamSetContainer(*aQueryCtr);
	User::LeaveIfNull((TAny*)aQuerySet);

	iPlaneStatus.CopyFieldsRequiredByQueryFrom(aQuerySet->PlaneStatusFilter(),aSource.iPlaneStatus);
	}

/*virtual*/ TInt XAccessPointPlaneStatusParameterSet::AppendPrintableTo(TDes& aBuf) const
	{
#ifdef __CFLOG_ACTIVE
	TDes16IgnoreOverflow ignoreOverflow;
	const TAccessPointPlaneStatus& ps = this->PlaneStatus();
	aBuf.AppendFormat(_L("PlSt:T..M..C%c.S..F.."),&ignoreOverflow,
			TAccessPointStatus::FlagValueAsChar(ps.Connection_Exists()));
	return KErrNone;
#else
	(void)aBuf;
	return KErrNotSupported;
#endif // __CFLOG_ACTIVE
	}

/*virtual*/ TInt XAccessPointPlaneStatusQuery::AppendPrintableTo(TDes& aBuf) const
	{
#ifdef __CFLOG_ACTIVE
	TDes16IgnoreOverflow ignoreOverflow;
	const TAccessPointPlaneStatusFilter& psf = this->PlaneStatusFilter();
	aBuf.AppendFormat(_L("PlSt:T..M..C%c.S..F.."),&ignoreOverflow,
			TAccessPointStatusFilter::FilterValueAsChar(psf.Connection_Exists()));
	return KErrNone;
#else
	(void)aBuf;
	return KErrNotSupported;
#endif // __CFLOG_ACTIVE
	}


