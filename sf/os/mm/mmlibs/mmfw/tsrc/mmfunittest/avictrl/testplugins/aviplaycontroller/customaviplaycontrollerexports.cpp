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
// add the header file of the ordinary controller here
//

#include <mmfcontrollerimplementationuids.hrh>
#include "customaviplaycontrolleruids.hrh"
#include "customaviplaycontroller.h"




// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
// move to another module to allow custom audio controller for testing

/**
*
* ImplementationTable
*
*/
const TImplementationProxy ImplementationTable[] =
	{

	// uid of the custom audio plugin !
		IMPLEMENTATION_PROXY_ENTRY(KCustomMmfAviPlayControllerImpUid, CCustomAviPlayController::NewL)
	};

/**
* ImplementationGroupProxy
* @param aTableCount
* @returns "TImplementationProxy*"
*/

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

