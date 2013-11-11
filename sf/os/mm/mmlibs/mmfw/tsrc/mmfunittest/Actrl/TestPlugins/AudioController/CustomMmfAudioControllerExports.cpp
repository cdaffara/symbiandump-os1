// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CustomMmfAudioController.h"

#include "CustomMmfAudioController.h"
#include <mmf/server/mmfformat.h>
#include <mmf/server/mmfclip.h>
#include <mdaaudiosampleeditor.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfpaniccodes.h>




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
		IMPLEMENTATION_PROXY_ENTRY(0x101f7d2A, CCustomMmfAudioController::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x102834B6, CMemoryFailAudioController::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x102834B7, CMemoryPassAudioController::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x10286661, CPanicAudioController::NewL)
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

