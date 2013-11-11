// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Parameter set base class for applying subconnection protocol attributes
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef __SS_PROTOCOLPARAMETERSET_H__
#define __SS_PROTOCOLPARAMETERSET_H__

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>
#include <comms-infras/metatypearray.h>
#include <in_sock.h>

#include <es_sock.h>
#include <cs_subconparams.h>

#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_internal.h>


namespace ESock
{

/** Conveys attributes of a flow request
@internalTechnology
*/

class CFlowRequestParameters : public CSubConGenericParameterSet
/** Conveys attributes of a flow request
@internalTechnology
*/
	{
public:
	enum
		{
		EUid=KSubConnGenericParamsImplUid,
		EType=KFlowRequestParametersType
		};

	IMPORT_C static CFlowRequestParameters* NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType);
	IMPORT_C static CFlowRequestParameters* NewL();
	inline CFlowRequestParameters();
	
	inline void SetFlowParams(ESock::TFlowParams& aFlowParams);

	inline ESock::TFlowParams& GetFlowParams();
	inline TUid GetProtocolUid() const;

protected:
	DATA_VTABLE

	ESock::TFlowParams iFlowParams;
	};


/**
@internalComponent
*/

/** Specifier for protocol extension factory
@internalComponent
*/
struct TProtocolExtensionSpecifier
	{
	TProtocolExtensionSpecifier(
		TUid aTierId,
		TUid aSCPRFactoryUid
		)
			: iSCPRFactoryUid(aSCPRFactoryUid), iTierId(aTierId) {}
	
	TProtocolExtensionSpecifier() : iSCPRFactoryUid(TUid::Null()), iTierId(TUid::Null()) {}

	TUid iSCPRFactoryUid;
	TUid iTierId;
	};


class CSubConnectionProtocolParameterSet : public CSubConGenericParameterSet
/** Base class for protocol extension parameters

This class is a base class for protocol extensions such as RTP

@internalComponent
*/
	{
public:
	enum
		{
		EUid=KSubConnGenericParamsImplUid,
		EType=KSubConnProtocolGenericParamsType
		};


	IMPORT_C static CSubConnectionProtocolParameterSet* NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType);
	IMPORT_C static CSubConnectionProtocolParameterSet* NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType);
	IMPORT_C static CSubConnectionProtocolParameterSet* NewL();

	IMPORT_C CSubConnectionProtocolParameterSet();
	
	IMPORT_C TInt FindAndRemoveExtension(TProtocolExtensionSpecifier& aExtensionSpec, const TUid& aTierId);

	IMPORT_C virtual ~CSubConnectionProtocolParameterSet();

protected:
	IMPORT_C TInt AddExtension(const TProtocolExtensionSpecifier& aExtensionSpec);

	EXPORT_DATA_VTABLE_AND_FN

	RArray<TProtocolExtensionSpecifier> iExtensions;
	};
}

#include <comms-infras/ss_protocolparameterset.inl>


#endif // __SS_PROTOCOLPARAMETERSET_H__


