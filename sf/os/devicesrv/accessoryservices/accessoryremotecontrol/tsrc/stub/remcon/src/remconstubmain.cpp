/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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


/** @file
@internalComponent
*/

#include <ECom/ImplementationProxy.h>
#include "remconstub.h"
#include "siftrace.h"

// Define the private interface UIDs
const TImplementationProxy ImplementationTable[] =
	{
	
	IMPLEMENTATION_PROXY_ENTRY(0x10205084, CRemConStub::NewL),
	};
	
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::ImplementationGroupProxy() - Enter" ) ) );
	
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::ImplementationGroupProxy() - Return" ) ) );
	return ImplementationTable;
	}

//
// End of file
