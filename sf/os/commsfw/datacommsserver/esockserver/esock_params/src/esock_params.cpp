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
//

#include "esock_params.h"
#include <comms-infras/esock_params_internal.h>
#include <e32std.h>
#include <e32test.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <comms-infras/metatype.h>

//SMetaDataECom macros

START_ATTRIBUTE_TABLE( TConnAPPref, TConnAPPref::EUid, TConnAPPref::ETypeId )
	REGISTER_ATTRIBUTE( TConnAPPref, iAP, TMetaNumber )	
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( TConnCSRPref, TConnCSRPref::EUid, TConnCSRPref::ETypeId )
	REGISTER_ATTRIBUTE( TConnCSRPref, iScope, TMetaNumber )	
	REGISTER_ATTRIBUTE( TConnCSRPref, iFlags, TMetaNumber )
	REGISTER_ATTRIBUTE( TConnCSRPref, iSubSessionUniqueId, TMeta<Den::TSubSessionUniqueId>)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( TConnAutoStartPref, TConnAutoStartPref::EUid, TConnAutoStartPref::ETypeId )
END_ATTRIBUTE_TABLE()


/**
Plugin Implementation
*/

Meta::SMetaDataECom* CESockParamaterFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
	case (TConnAPPref::ETypeId):
		return new (ELeave) TConnAPPref;
	case (TConnCSRPref::ETypeId):
		return new (ELeave) TConnCSRPref;
	case (TConnAutoStartPref::ETypeId):
		return new (ELeave) TConnAutoStartPref;
	default:
		User::Leave(KErrNotFound);
		return NULL;
		}
	}


