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

#include "provinfoqueryset.h"
#include <comms-infras/ss_commsdataobject.h>

using namespace Meta;
using namespace ESock;

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XProviderInfoQuerySet, CCommsDataObjectFactory::iUid, XProviderInfoQuerySet::iUid)
	REGISTER_ATTRIBUTE(XProviderInfoQuerySet, iConnProviderInfo, TMeta<TConnProviderInfo>)
END_ATTRIBUTE_TABLE_BASE(XCommsDataObject, 0)


EXPORT_C XProviderInfoQuerySet::~XProviderInfoQuerySet()
		{
		}


EXPORT_C XCommsDataObject::TProgressAction XProviderInfoQuerySet::DispatchL(TAny* aItfPtr, MPlatsecApiExt* /*aPlatsecItf*/)
	{
	MAccessPointConfigApi* interface = reinterpret_cast<MAccessPointConfigApi*>(aItfPtr);
	
	const TProviderInfo& providerInfo = static_cast<const TProviderInfoExt&>(interface->GetAccessPointConfig().FindExtensionL(
	        STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId))).iProviderInfo;

	iConnProviderInfo.iInfo[0] = providerInfo.TierId().iUid;
	iConnProviderInfo.iInfo[1] = providerInfo.APId();
	iConnProviderInfo.iInfo[2] = (TUint)providerInfo.Instance();
	
	return XCommsDataObject::EComplete;
	}

