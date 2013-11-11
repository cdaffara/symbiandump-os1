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
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <metadata.h>
#else
#include <comms-infras/metadata.h>
#endif
#include <comms-infras/metatype.h>

//SMetaDataECom macros

START_ATTRIBUTE_TABLE( TZeroCopyLoopbackPref, TZeroCopyLoopbackPref::EUid, TZeroCopyLoopbackPref::ETypeId )
	REGISTER_ATTRIBUTE( TZeroCopyLoopbackPref, iAP, TMetaNumber )	
END_ATTRIBUTE_TABLE()


SMetaDataECom* CZeroCopyLoopbackParamaterFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
	case (TZeroCopyLoopbackPref::ETypeId):
		return new (ELeave) TZeroCopyLoopbackPref;
	default:
		User::Leave(KErrNotFound);
		return NULL;
		}
	}
