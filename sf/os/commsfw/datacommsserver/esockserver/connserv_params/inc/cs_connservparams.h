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
// Generic connection server parameter sets - plugin for Connection Server Parameter Bundle
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CS_CONNSERVPARAMS_H__
#define __CS_CONNSERVPARAMS_H__

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>

#include <es_sock.h>

#include <comms-infras/es_parameterbundle.h>
#include <comms-infras/es_connectionservparameterbundle.h>
#include <comms-infras/es_accesspointstatus.h>

#include <comms-infras/ss_common.h>

const TInt32 KConnectionServGenericParamImplUid = 0x10272F3E;
const TInt32 KAccessPointGenericParameterSetType = 1;
const TInt32 KAccessPointGenericQueryType = 2;
//const TInt32 Assigned in cs_connservparams_internal.h = 3;
//const TInt32 Assigned in cs_connservparams_internal.h = 4;
//const TInt32 Assigned in cs_connservparams_internal.h = 5;
//const TInt32 Assigned in cs_connservparams_internal.h = 6;
//const TInt32 Assigned in cs_connservparams_internal.h = 7;


namespace ConnectionServ
{

class XAccessPointGenericParameterSet : public XConnectionServParameterSet,
		public MParameterSetTemplateMethods<CConnectionServParameterSetContainer,XAccessPointGenericParameterSet,
								KConnectionServGenericParamImplUid, KAccessPointGenericParameterSetType>
/** Access Point generic parameter set.

This class contains generic Access Point properties i.e, independent of any technology and provides the interface to access them.

@note SMetaDataECom must be the first in the superclasses' list
@publishedPartner
@released
*/
{
public:
	inline XAccessPointGenericParameterSet(): iApInfo(),iApStatus() {}

// getters
	inline const TAccessPointInfo& AccessPointInfo() const {return iApInfo;}
	inline const TAccessPointStatus& AccessPointStatus() const {return iApStatus;}

// setters
	inline void AccessPointInfo(const TAccessPointInfo& aApInfo) {iApInfo=aApInfo;}
	inline void AccessPointStatus(const TAccessPointStatus& aApStatus) {iApStatus=aApStatus;}

// extra intelligence. This is required because many different collectors may
//  return an instance of this class and we'll need to merge instances together.
	virtual void UpdateWithL(const XParameterSet& aRhsBase);
	virtual void CopyFieldsRequiredByQueryFromL(
						const CParameterBundleBase& aQuery,const XConnectionServParameterSet& aSource);
// polymorphic logging
	virtual TInt AppendPrintableTo(TDes& aDestination) const;

	DATA_VTABLE

protected:

	TAccessPointInfo iApInfo;
	TAccessPointStatus iApStatus;

	};

class XAccessPointGenericQuery : public XConnectionServParameterSet,
		public MParameterSetTemplateMethods<CConnectionServParameterSetContainer,XAccessPointGenericQuery,
								KConnectionServGenericParamImplUid, KAccessPointGenericQueryType>
/** Access Point generic query.

This class represents a query of the attributes of XAccessPointGenericParameterSet

@note SMetaDataECom must be the first in the superclasses' list
@publishedPartner
@released
*/
{
public:
	inline XAccessPointGenericQuery(): iApStatusFilter() {}
	virtual ~XAccessPointGenericQuery(); // due to having an array

// getters
	inline const RArray<TAccessPointInfo>& AccessPointsToMatch() const
		{return iApsToMatch;}
	inline const TAccessPointStatusFilter& AccessPointStatusFilter() const {return iApStatusFilter;}
	inline const RArray<STypeId>& GetParameterSetTypesToReturn() const
		{return iParameterSetTypesToReturn;}
	virtual TBool ShouldReturnType(const STypeId& aTypeId) const;

// setters
	inline void AddAccessPointToMatchL(const TAccessPointInfo& aApInfo)
		{iApsToMatch.AppendL(aApInfo);}
	inline void AccessPointStatusFilter(const TAccessPointStatusFilter &aApStatusFilter) {iApStatusFilter=aApStatusFilter;}
	inline void AddParameterSetTypeToReturnL(const STypeId& aTypeId)
		{iParameterSetTypesToReturn.AppendL(aTypeId);}

// No extra intelligence is required here, because this will only ever be part
//  of a query and will not be in the result (in particular will not be
//  coming back from many sources)

// polymorphic logging
	virtual TInt AppendPrintableTo(TDes& aDestination) const;

	DATA_VTABLE

protected:

	RArray<TAccessPointInfo> iApsToMatch;
	TAccessPointStatusFilter iApStatusFilter;
	RArray<STypeId> iParameterSetTypesToReturn;
	};



class CConnectionServGenericParamsFactory : public CBase
/** Factory for the various connection server parameter sets.

@internalComponent
*/
	{
public:
	static XConnectionServParameterSet* NewL(TAny* aConstructionParameters);
	};


} // namespace ConnectionServ

//#include <cs_connservparams.inl>
#endif	// __CS_CONNSERVPARAMS_H__

