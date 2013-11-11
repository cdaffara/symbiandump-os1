// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "pcmprocessingunit.h"
#include <ecom/implementationproxy.h>
#include "uids.hrh"

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KUidPcmP8P16PU, CPcmProcessingUnit::NewP8P16L),
	IMPLEMENTATION_PROXY_ENTRY(KUidPcmP16P8PU, CPcmProcessingUnit::NewP16P8L),
	IMPLEMENTATION_PROXY_ENTRY(KUidPcmPU8P16PU, CPcmProcessingUnit::NewPU8P16L),
	IMPLEMENTATION_PROXY_ENTRY(KUidPcmP16PU8PU, CPcmProcessingUnit::NewP16PU8L),
	IMPLEMENTATION_PROXY_ENTRY(KUidPcmDummyP8P16U, CPcmProcessingUnit::NewP8P16L),
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

