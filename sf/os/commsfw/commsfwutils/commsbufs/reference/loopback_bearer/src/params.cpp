/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include "params.h"
#include <e32std.h>
#include <e32test.h>

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <ecom.h>
#include <implementationproxy.h>
#include <metadata.h>
#else
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <comms-infras/metadata.h>
#endif

#include <comms-infras/metatype.h>

//SMetaDataECom macros

START_ATTRIBUTE_TABLE( TLegacyLoopbackPref, TLegacyLoopbackPref::EUid, TLegacyLoopbackPref::ETypeId )
	REGISTER_ATTRIBUTE( TLegacyLoopbackPref, iAP, TMetaNumber )	
END_ATTRIBUTE_TABLE()


SMetaDataECom* CLegacyLoopbackParamaterFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
	case (TLegacyLoopbackPref::ETypeId):
		return new (ELeave) TLegacyLoopbackPref;
	default:
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
