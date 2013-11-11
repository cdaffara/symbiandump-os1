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
// Implementation for CEventParamsGranted and CEventParamsChanged.
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/


#include "cs_genevent.h"


START_ATTRIBUTE_TABLE( CEventParamsGranted, KEventTypeImplUid, KEventParamsGranted )
	REGISTER_ATTRIBUTE( CEventParamsGranted, iFamilyBundle, TMeta<RParameterFamilyBundle>)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CEventParamsChanged, KEventTypeImplUid, KEventParamsChanged )
	REGISTER_ATTRIBUTE( CEventParamsChanged, iError, TMetaNumber)
END_ATTRIBUTE_TABLE_BASE(CEventParamsGranted, 0)


CEventParamsGranted::~CEventParamsGranted()
	{
	if(!iFamilyBundle.IsNull())
		{
		iFamilyBundle.Destroy();
		}
	}

CEventParamsChanged::~CEventParamsChanged()
	{
	}


XPlaneEvent* CPlaneEventFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
	case (KEventParamsGranted):
		return new (ELeave) CEventParamsGranted;
	case (KEventParamsChanged):
		return new (ELeave) CEventParamsChanged;
	default:
		User::Leave(KErrNotFound);
		}
	return NULL;
	}




