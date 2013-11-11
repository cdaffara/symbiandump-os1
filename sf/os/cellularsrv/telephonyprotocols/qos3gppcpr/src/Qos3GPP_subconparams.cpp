// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Qos3GPP_subconnProvFactory.cpp
// Implementation file for the 3GPP SubConnection Parameter Set (R99/R4)
// 
//

/**
 @file
 @internalComponent
*/


#include <e32std.h>
#include <e32test.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <networking/qos3gpp_subconparams.h>
#include <comms-infras/metatypearray.h>
#include <networking/qos3gpp_subconparams_factory.h>

START_ATTRIBUTE_TABLE( CSubConQosR99ParamSet, KSubCon3GPPExtParamsFactoryUid, KSubConQosR99ParamsType )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iTrafficClass, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iDeliveryOrder, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iDeliveryOfErroneusSdu, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iResidualBer, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iErrorRatio, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iPriority, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iTransferDelay, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iMaxSduSize, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iMaxBitrateUplink, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iMaxBitrateDownlink, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iGuaBitrateUplink, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConQosR99ParamSet, iGuaBitrateDownlink, TMetaNumber )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CSubConSBLPR5ExtensionParamSet, KSubCon3GPPExtParamsFactoryUid, KSubConnSBLPR5ExtensionParamsType )
	REGISTER_ATTRIBUTE( CSubConSBLPR5ExtensionParamSet, iAuthToken, TMeta<TAuthToken>)
	REGISTER_ATTRIBUTE( CSubConSBLPR5ExtensionParamSet, iFlowIds, TMetaArray<RFlowIdentifiers>)
END_ATTRIBUTE_TABLE()


#ifdef SYMBIAN_NETWORKING_UMTSR5  
START_ATTRIBUTE_TABLE( CSubConQosR5ParamSet, KSubCon3GPPExtParamsFactoryUid, KSubConQosR5ParamsType )
	REGISTER_ATTRIBUTE( CSubConQosR5ParamSet, iSignallingIndicator, TMeta<TBool>)
	REGISTER_ATTRIBUTE( CSubConQosR5ParamSet, iSrcStatsDesc, TMetaNumber)
END_ATTRIBUTE_TABLE_BASE(CSubConQosR99ParamSet,0)

START_ATTRIBUTE_TABLE( CSubConImsExtParamSet, KSubCon3GPPExtParamsFactoryUid, KSubConImsExtParamsType )
	REGISTER_ATTRIBUTE(CSubConImsExtParamSet, iImsSignallingIndicator, TMeta<TBool>)
END_ATTRIBUTE_TABLE()
#endif 
// SYMBIAN_NETWORKING_UMTSR5 


CSubConExtensionParameterSet* CSubCon3GPPExtParamsFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{

	case (KSubConQosR99ParamsType):
		return new (ELeave) CSubConQosR99ParamSet;

	case (KSubConnSBLPR5ExtensionParamsType):
		return new (ELeave) CSubConSBLPR5ExtensionParamSet;
	
#ifdef SYMBIAN_NETWORKING_UMTSR5 
	case (KSubConQosR5ParamsType):
		return new(ELeave) CSubConQosR5ParamSet; 
	
	case (KSubConImsExtParamsType):
		return new(ELeave) CSubConImsExtParamSet;
#endif 
// SYMBIAN_NETWORKING_UMTSR5 
 
	default:
		User::Leave(KErrNotFound);
		}
	return NULL;
	}

