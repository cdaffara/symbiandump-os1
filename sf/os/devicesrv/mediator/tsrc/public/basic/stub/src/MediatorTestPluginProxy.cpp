/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Source file for MediatorTest Plugin Proxy
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <ecom/implementationproxy.h>
#include "MediatorTestPlugin.h"


// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(0x87654323, CMediatorTestPlugin::NewL )
	};

// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}


