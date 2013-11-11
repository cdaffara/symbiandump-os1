/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Entry point for Ecom plug-in.
*
*/


#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "tiltcompensationssyconsts.hrh"
#include "tcssycontrol.h"
#include "trace.h"
#include "common.h"

// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY( KTiltCompensationSsyImplUid, CTCSsyControl::NewL )
	};

// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

// End of File
