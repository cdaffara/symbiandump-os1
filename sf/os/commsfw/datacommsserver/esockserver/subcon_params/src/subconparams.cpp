// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file subconparams.cpp
*/

#include <e32std.h>
#include <e32test.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <cs_subconparams.h>
#include <cs_subconevents.h>

#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_protocolparameterset.h>

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
#include <cs_genevent.h>
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

#include "cs_subconeventsfactory.h"

class CSubConGenericParamsFactory : public CBase
/** Sub connection generic parameter set factory.

@internalComponent
@released since v9.1 */
	{
public:
	static CSubConGenericParameterSet* NewL(TAny* aConstructionParameters);
	};

START_ATTRIBUTE_TABLE( CSubConQosGenericParamSet, CSubConQosGenericParamSet::EUid, CSubConQosGenericParamSet::EType)
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iDownlinkBandwidth, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iUplinkBandwidth, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iDownLinkMaximumBurstSize, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iUpLinkMaximumBurstSize, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iDownLinkAveragePacketSize, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iUpLinkAveragePacketSize, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iDownLinkMaximumPacketSize, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iUpLinkMaximumPacketSize, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iDownLinkDelay, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iUpLinkDelay, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iDownLinkDelayVariation, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iUpLinkDelayVariation, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iDownLinkPriority, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iUpLinkPriority, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iHeaderMode, TMeta<TBool> )
	REGISTER_ATTRIBUTE( CSubConQosGenericParamSet, iName, TMeta<TName> )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CSubConAuthorisationGenericParamSet, CSubConAuthorisationGenericParamSet::EUid, CSubConAuthorisationGenericParamSet::EType)
	REGISTER_ATTRIBUTE( CSubConAuthorisationGenericParamSet, iId, TMetaNumber )
END_ATTRIBUTE_TABLE()


START_ATTRIBUTE_TABLE(ESock::CFlowRequestParameters, ESock::CFlowRequestParameters::EUid, ESock::CFlowRequestParameters::EType)
	REGISTER_ATTRIBUTE(ESock::CFlowRequestParameters, iFlowParams, TMeta<ESock::TFlowParams>)
END_ATTRIBUTE_TABLE()


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KSubConnGenericParamsImplUid, CSubConGenericParamsFactory::NewL),
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	IMPLEMENTATION_PROXY_ENTRY(KSubConnGenericEventsImplUid, CSubConGenEventsFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KEventTypeImplUid, CPlaneEventFactory::NewL)
#else
	IMPLEMENTATION_PROXY_ENTRY(KSubConnGenericEventsImplUid, CSubConGenEventsFactory::NewL)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
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

CSubConGenericParameterSet* CSubConGenericParamsFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
	case (CSubConQosGenericParamSet::EType):
		return new (ELeave) CSubConQosGenericParamSet;
		// break;
	case (CSubConAuthorisationGenericParamSet::EType):
		return new (ELeave) CSubConAuthorisationGenericParamSet;
		// break;
	case (ESock::CFlowRequestParameters::EType):
		return new (ELeave) ESock::CFlowRequestParameters;
		// break;
	default:
		User::Leave(KErrNotFound);
		}
	return NULL;
	}


