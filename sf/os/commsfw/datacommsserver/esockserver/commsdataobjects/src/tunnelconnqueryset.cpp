// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <comms-infras/connectionqueryset.h>

using namespace Meta;
using namespace ESock;


EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XTunnelConnQuerySet, CCommsDataObjectFactory::iUid, XTunnelConnQuerySet::iUid)
	REGISTER_ATTRIBUTE(XTunnelConnQuerySet, iLayerNum, TMeta<TInt>)
	REGISTER_ATTRIBUTE(XTunnelConnQuerySet, iQueries, TMetaObject<XTunnelConnQuerySet::RMetaDataQueryContainer>)
END_ATTRIBUTE_TABLE()


EXPORT_C XCommsDataObject::TProgressAction XTunnelConnQuerySet::DispatchL(TAny *aItfPtr, ESock::MPlatsecApiExt* aPlatsecItf)
	{
	if(EBottomIAPLayer == iLayerNum)
		{
		MQueryConnSettingsApiExt* interface = reinterpret_cast<MQueryConnSettingsApiExt*>(aItfPtr);
		TConnectionQueryIter iter = GetQueryIter(XConnectionQueryBase::ENotAnswered | XConnectionQueryBase::EAnswered);
		XConnectionQueryBase* query = iter++;
		while (query) 
			{
			query->GetSetting(interface, aPlatsecItf);
			query = iter++;
			}
		return EContinue;
		}

	if(iLayerNum > 1)
		{
		--iLayerNum;
		return EContinue;
		}
	else
		{
		return XConnectionQuerySet::DispatchL(aItfPtr, aPlatsecItf);
		}
	}


