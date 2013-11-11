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
 @publishedPartner
 @released since 399
*/

#include <comms-infras/es_connectionservparameterbundle.h>
#include <comms-infras/cs_connservparams.h>
#include <comms-infras/cs_connservparams_internal.h>

#ifdef _DEBUG
#include <comms-infras/es_connectionservparameterbundletrace.h>
#include <comms-infras/es_connectionservparameterbundletraceimpl.h>
#endif

using namespace ConnectionServ;
using namespace Meta;


EXPORT_START_ATTRIBUTE_TABLE_NO_FN(XConnectionServParameterSet, KConnectionServParameterSetTypeUid, Meta::KNetMetaTypeAny)
END_ATTRIBUTE_TABLE_BASE( XParameterSet, 0 )

EXPORT_C /*virtual*/ XConnectionServParameterSet::~XConnectionServParameterSet()
	{
	}

EXPORT_C /*virtual*/ void XConnectionServParameterSet::CopyFieldsRequiredByQueryFromL(
						const CParameterBundleBase& aQuery,const XConnectionServParameterSet& aSource)
	{
	(void)aQuery;
	Copy(aSource);
	}

// Connection Server Parameter Bundle Implementation

EXPORT_C XAccessPointGenericQuery* CConnectionServParameterBundle::FindOrCreateGenericQueryL()
	{
	CConnectionServParameterSetContainer* parSetCtr = GetParamSetContainer(0);
	XAccessPointGenericQuery* querySet = 0;
	if(!parSetCtr)
		{
		parSetCtr = CConnectionServParameterSetContainer::NewL(*this, 0 /*no id needed*/);
		}
	else
		{
		querySet = XAccessPointGenericQuery::FindInParamSetContainer(*parSetCtr); 
		}

	if(!querySet)
		{
		querySet = XAccessPointGenericQuery::NewL(*parSetCtr);
		}

	return querySet;
	}

EXPORT_C void CConnectionServParameterBundle::AddMatchConditionL(const TAccessPointInfo& aApInfo)
	{
	XAccessPointGenericQuery* querySet = FindOrCreateGenericQueryL();
	querySet->AddAccessPointToMatchL(aApInfo);
	}

EXPORT_C void CConnectionServParameterBundle::AddMatchConditionL(const TAccessPointStatusFilter& aApStatusFilter)
	{
	XAccessPointGenericQuery* querySet = FindOrCreateGenericQueryL();
	querySet->AccessPointStatusFilter(aApStatusFilter);
	}

EXPORT_C void CConnectionServParameterBundle::AddParameterSetToReturnL(const STypeId& aTypeId)
	{
	XAccessPointGenericQuery* querySet = FindOrCreateGenericQueryL();
	querySet->AddParameterSetTypeToReturnL(aTypeId);
	}

EXPORT_C const RArray<Meta::STypeId>& CConnectionServParameterBundle::GetParameterSetsToReturnL() const
	{
	const XAccessPointGenericQuery* querySet = const_cast<CConnectionServParameterBundle*>(this)->FindOrCreateGenericQueryL();
	return querySet->GetParameterSetTypesToReturn();
	}


EXPORT_C const XAccessPointGenericQuery* CConnectionServParameterBundle::FindGenericQuery() const
	{
	/*const*/ CConnectionServParameterSetContainer* parSetCtr = const_cast<CConnectionServParameterBundle*>(this)->GetParamSetContainer(0);
	if(parSetCtr)
		{
		return XAccessPointGenericQuery::FindInParamSetContainer(*parSetCtr);
		}
	return 0;
	}


