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
//

#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "cmocklightsy.h"
#include "cmockvibrasy.h"
#include "cmockfmtxsy.h"
#include "cmockpowerstatesy.h"
#include "cmockpowersy.h"


// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
   	IMPLEMENTATION_PROXY_ENTRY(0x9911FF34, CMockLightSY::NewL),
    IMPLEMENTATION_PROXY_ENTRY(0x9911FF35, CMockVibraSY::NewL),
    IMPLEMENTATION_PROXY_ENTRY(0x9911FF36, CMockPowerStateSY::NewL),
    IMPLEMENTATION_PROXY_ENTRY(0x9911FF37, CMockFmTxSY::NewL),
    IMPLEMENTATION_PROXY_ENTRY(0x9911FF38, CMockPowerSY::NewL),
   };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
