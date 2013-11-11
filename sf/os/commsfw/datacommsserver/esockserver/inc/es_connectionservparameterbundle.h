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
 @released
*/

#ifndef __ES_CONNSERVPARAMETERBUNDLE_H
#define __ES_CONNSERVPARAMETERBUNDLE_H

#include <comms-infras/es_parameterbundle.h>

class TAccessPointInfo;

namespace ConnectionServ
{

class XAccessPointGenericParameterSet;
class XAccessPointGenericQuery;
struct TAccessPointStatusFilter;




const TInt KConnectionServParameterSetTypeUid = 0x10272F45;

class XConnectionServParameterSet : public XParameterSet
	{
public:
	IMPORT_C virtual ~XConnectionServParameterSet();

	// This decides which fields in the source parameter set are
	//  requested/allowed by the given query, and copies them to this set
	//
	// The base implementation ignores the query and just performs a binary copy.
	IMPORT_C virtual void CopyFieldsRequiredByQueryFromL(
						const CParameterBundleBase& aQuery,const XConnectionServParameterSet& aSource);

	EXPORT_TYPEID_TABLE
	};

class CConnectionServParameterBundle;


typedef class CParameterSetContainer CConnectionServParameterSetContainer;

class CConnectionServParameterBundle : public CParameterBundle<CConnectionServParameterSetContainer,CConnectionServParameterBundle>
	{
public:

	// to help in populating the query bundle:

	IMPORT_C XAccessPointGenericQuery* FindOrCreateGenericQueryL();

	// Adds the specified TAccessPointInfo filter to this query bundle
	// (- constructs a default query ParameterSetContainer (if needed),
	//  - constructs a XAccessPointGenericQuery under it (if needed),
	//  - then sets its TAccessPointInfo filter)
	IMPORT_C void AddMatchConditionL(const TAccessPointInfo& aApInfo);

	// Adds the specified TAccessPointStatusFilter filter to this query bundle
	// (- constructs a default query ParameterSetContainer (if needed),
	//  - constructs a XAccessPointGenericQuery under it (if needed),
	//  - then sets its TAccessPointStatusFilter filter)
	IMPORT_C void AddMatchConditionL(const TAccessPointStatusFilter& aApStatusFilter);

	// Adds the specified (parameter set type to return) to this query bundle.
	// This is an array of type Ids which we are asking to be returned by the
	//  query. The comms server uses this to decide which information it needs to
	//  retrieve.
	// (- constructs a default query ParameterSetContainer (if needed),
	//  - constructs a XAccessPointGenericQuery under it (if needed),
	//  - then adds the specified type to its iParameterSetTypesToReturn array)
	IMPORT_C void AddParameterSetToReturnL(const Meta::STypeId& aTypeId);

	
	// to help in reading the query bundle:

	// Retrieves (parameter set types to return) array.
	// (- finds default query ParameterSetContainer,
	//  - finds XAccessPointGenericQuery under it,
	//  - returns its iParameterSetTypesToReturn array)
	IMPORT_C const RArray<Meta::STypeId>& GetParameterSetsToReturnL() const;

	// Finds the XAccessPointGenericQuery in this query bundle
	// (- finds default query ParameterSetContainer,
	//  - returns any XAccessPointGenericQuery found under it)
	IMPORT_C const XAccessPointGenericQuery* FindGenericQuery() const;
	};





} // namespace ConnectionServ

#endif
// __ES_CONNSERVPARAMETERBUNDLE_H

